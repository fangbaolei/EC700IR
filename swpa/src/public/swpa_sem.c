#include "swpa.h"
#include "swpa_private.h"
#include "swpa_sem.h"
#include "swpa_datetime.h"

#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#ifdef SWPA_SEM
#define SWPA_SEM_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_SEM_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_SEM_PRINT(fmt, ...)
#define SWPA_SEM_CHECK(arg)
#endif

typedef struct tag_SEM
{
	sem_t sem;
	int max; 
	int id; /* BEGIN: add by huangdch, 2015/9/4:进程间信号量唯一标识*/
}
SEM_HANDLE;

/* BEGIN: Modified by huangdch, 2015/9/4 
*进程间的信号量个数100要根据具体的应用来评估并修改这个值*/
#define SHM_PROCESS_SEM_MAX_NUM 	100   /* 进程间信号量的最大个数 */
#define SHM_PROCESS_SEM_KEY_ID      10000 /* 信号量对应的共享内存主键key */
static void* g_shm_sem_msg_addr = NULL;   /* 同一个进程内管理首地址 */
static int g_shm_sem_number = 0;          /* 同一个进程内的连接数 */

/* BEGIN: Modified by huangdch, 2015/9/4 
*信号量锁分配首地址*/
typedef struct _shm_msg_sem
{
	pthread_mutex_t mutex; /* 锁 */
	short total_attach; /* 总的连接数,目前冗余字段 */
	short attach[SHM_PROCESS_SEM_MAX_NUM]; /* 每个节点的连接数 */
}sw_shm_msg_sem;

/*****************************************************************************
 函 数 名  : sem_mem_alloc
 功能描述  : 根据id从共享内存块分配一个内存节点,分配成功将返回内存的首地址
 输入参数  : const int id 信号量标识id   
             const int size 未用
             int* state 保留     
 输出参数  : 无
 返 回 值  : NULL失败 非空成功
 调用函数  : 
 注意事项  : 只在当前文件内使用
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
void* sem_mem_alloc(const int id, const int size, int* state)
{
	if (id < 1 || size < 1)
	{
		return -1;
	}

	SWPA_SEM_PRINT("alloc sem id=%d\n", id);
	SWPA_SEM_PRINT("alloc sem size=%d\n", size);
	SWPA_SEM_PRINT("alloc sem state=%d\n", *state);
	int ret = 0;
	int shm_id = 0;
	int temp_size = sizeof(sw_shm_msg_sem) + SHM_PROCESS_SEM_MAX_NUM * sizeof(SEM_HANDLE);
	int key_id = SHM_PROCESS_SEM_KEY_ID;
	int temp_state = 0;
	int process_flag = 0;
	void* radd = NULL;

	if (NULL == g_shm_sem_msg_addr)
	{
		/* 连接管理区 */
		int temp_state = 0;
		g_shm_sem_msg_addr = swpa_normal_sharedmem_alloc(key_id, temp_size, &temp_state);
		if (NULL == g_shm_sem_msg_addr)
		{
			SWPA_SEM_PRINT("create key_id=[%d] size=[%d] failed,ret=[%d].\n", key_id, temp_size, ret);
			return NULL;
		}

		/* 初始化锁 */
		if (0 == temp_state)
		{
			sw_shm_msg_sem* msg_node = (sw_shm_msg_sem*)g_shm_sem_msg_addr;

			pthread_mutexattr_t attr;
			if (0 != pthread_mutexattr_init(&attr))
			{
				return NULL;
			}
			if (0 != pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED))
			{
				return NULL;
			}	
			if (0 != pthread_mutex_init(&msg_node->mutex, &attr))
			{
				return NULL;
			}
			if (0 != pthread_mutexattr_destroy(&attr))
			{
				return NULL;
			}

			msg_node->total_attach = 0;
			memset(&msg_node->attach, 0, sizeof(msg_node->attach));
			memset(msg_node + 1, 0, temp_size - sizeof(sw_shm_msg_sem));
		}

		process_flag = 1;
	}

	sw_shm_msg_sem* msg_node = (sw_shm_msg_sem*)g_shm_sem_msg_addr;

	pthread_mutex_lock(&msg_node->mutex);

	/* 根据id在已经分配的共享内存块查找一个已分配或未占用的节点 */
	SEM_HANDLE* shm_node = (SEM_HANDLE*)(msg_node + 1);
	int k = 0;
	int flag = -1;
	int node = -1;
	for (k = 0 ; k < SHM_PROCESS_SEM_MAX_NUM ; k++)
	{
		if (id == shm_node[k].id) /* 找已经分配的节点 */
		{
			flag = 2;
			msg_node->attach[k]++;/* 连接的进程数 */
			msg_node->total_attach++;
			radd = (void*)&shm_node[k]; /* 返回一个地址 */
			*state = 1;
			node = k;
			break;
		}
	}
	if (-1 == flag)
	{
		for (k = 0 ; k < SHM_PROCESS_SEM_MAX_NUM ; k++)
		{
			if (0 == shm_node[k].id) /* 找未分配的节点 */
			{
				msg_node->attach[k]++;/* 连接的进程数 */
				msg_node->total_attach++;
				shm_node[k].id = id;
				flag = 1;
				radd = (void*)&shm_node[k]; /* 返回一个地址 */
				*state = 0;
				node = k;
				break;
			}
		}
	}

	SWPA_SEM_PRINT("sem_id=[%d] flag=[%d] node=[%d]\n", id, flag, node);

	/* 没有找到满足条件的节点进行分配 */
	if (-1 == flag && 1 == process_flag)
	{
		pthread_mutex_unlock(&msg_node->mutex);
		
		ret = swpa_normal_sharedmem_free(SHM_PROCESS_SEM_KEY_ID, g_shm_sem_msg_addr);
		if (ret < 0)
		{
			SWPA_SEM_PRINT("free:sem_id=[%d] failed,rang out of [%d] ret=[%d].\n", SHM_PROCESS_SEM_KEY_ID, SHM_PROCESS_SEM_MAX_NUM, ret);
		}
		else
		{
			SWPA_SEM_PRINT("free:sem_id=[%d] success,ret=[%d].\n", SHM_PROCESS_SEM_KEY_ID, ret);
		}
		
		SWPA_SEM_PRINT("free:sem_id=[%d] failed,rang out of [%d] ret=[%d].\n", SHM_PROCESS_SEM_KEY_ID, SHM_PROCESS_SEM_MAX_NUM, ret);

		return NULL;
	}

	if (NULL != radd)
	{
		g_shm_sem_number++; /* 同一个进程内的连接数+1 */
	}

	pthread_mutex_unlock(&msg_node->mutex);

	return radd;
}

/*****************************************************************************
 函 数 名  : sem_mem_free
 功能描述  : 根据信号量id查找对应的内存块并释放相应的节点
 输入参数  : const int id  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 注意事项  : 
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int sem_mem_free(const int id)
{
	if (id < 1 || NULL == g_shm_sem_msg_addr)
	{
		return -1;
	}

	int ret = 0;
	int shm_id = 0;

	sw_shm_msg_sem* msg_node = (sw_shm_msg_sem*)g_shm_sem_msg_addr;

	pthread_mutex_lock(&msg_node->mutex);

	SEM_HANDLE* shm_node = (SEM_HANDLE*)(msg_node + 1);

	int flag = -1;
	int k = 0;
	for (k = 0 ; k < SHM_PROCESS_SEM_MAX_NUM ; k++)
	{
		if (id == shm_node[k].id)
		{
			flag = 1;
			g_shm_sem_number--;
			msg_node->attach[k]--;
			msg_node->total_attach--;
			if (0 == msg_node->attach[k])/* 如果信号量的内存节点连接数为0设置可再分配 */
			{
				SWPA_SEM_PRINT("node=[%d] id=[%d] is set 0.\n", k, id);
				shm_node[k].id = 0;
			}
			break;
		}
	}

	pthread_mutex_unlock(&msg_node->mutex);

	if (g_shm_sem_number == 0)
	{
		SWPA_SEM_PRINT("call swpa_normal_sharedmem_free, id=[%d] num=[%d] total_attach=[%d]\n", id, g_shm_sem_number, msg_node->total_attach);

		ret = swpa_normal_sharedmem_free(SHM_PROCESS_SEM_KEY_ID, g_shm_sem_msg_addr);
		if (0 == ret)
		{
			g_shm_sem_msg_addr = NULL;
		}
	}
	
	return ret;
}

/**
* @brief 创建信号量
* @param [out] sem 信号量结构体指针
* @param [in] init 信号量初始值
* @param [in] init 信号量最大值
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
* @note 该信号量为线程级别。
*/
int swpa_sem_create(int* sem, unsigned int init, unsigned int max)
{
	SWPA_SEM_CHECK(sem != NULL);
	SWPA_SEM_CHECK(init <= max);
	SWPA_SEM_PRINT("sem=0x%08x\n", (unsigned int)sem);
	SWPA_SEM_PRINT("init=%d\n", init);
	SWPA_SEM_PRINT("max=%d\n", max);

	int iret = -1;
	SEM_HANDLE* handle = swpa_mem_alloc(sizeof(SEM_HANDLE));
	if( handle == NULL )
	{
		return SWPAR_OUTOFMEMORY;
	}
	iret = sem_init((sem_t*)(&handle->sem), 0, init);
	if(iret < 0)
	{
		swpa_mem_free(handle);
		handle = NULL;
		return SWPAR_FAIL;
	}
	handle->max = max;
	handle->id = 0;

	(*sem) = (int)handle;
	return SWPAR_OK;
}

/* BEGIN: Modified by huangdch, 2015/9/4 
*进程间信号量的创建*/
int swpa_processsem_create(int* sem, unsigned int init, unsigned int max, const int sem_id)
{
	if (sem < 1)
	{
		return -1;
	}
	SWPA_SEM_PRINT("process sem=0x%08x\n", (unsigned int)sem);
	SWPA_SEM_PRINT("init=%d\n", init);
	SWPA_SEM_PRINT("max=%d\n", max);
	SWPA_SEM_PRINT("process sem_id=%d\n", sem_id);

	int k = 0;
	int ret = -1;
	int size = 0;
	int id = sem;

	SEM_HANDLE* psem = NULL;
	size = sizeof(*psem);

	/* 分配共享内存 */
	int state = 0;
	psem = (SEM_HANDLE *)sem_mem_alloc(sem_id, size, &state);
	if (NULL == psem)
	{
		return -1;
	}

	if (0 == state)
	{
		/* 初始化信号量 */
		ret = sem_init(&psem->sem, 3, init);/* 3表示进程间使用 */
		if (ret < 0)
		{
			return -2;
		}
		psem->max = max;
		psem->id = sem_id;
	}

	(*sem) = (int)psem;

	return 0;
}


/**
* @brief 锁定信号量
* @param [in] sem 信号量结构体指针
* @param [in] timeout 锁定超时。单位：毫秒
* @retval 0 成功或可锁
* @retval -1 失败或不可锁
* @see swpa_sem.h
* @note 当timeout为-1时，表示阻塞锁定；当timeout等于0时，表示尝试锁定；当timeout大于0时，即表示锁定超时时限。
*/
int swpa_sem_pend(int* sem, int timeout)
{
	SWPA_SEM_CHECK(sem != NULL);
	SWPA_SEM_PRINT("sem=0x%08x\n", (unsigned int)sem);
	SWPA_SEM_PRINT("timeout=%d\n", timeout);
	int iret = -1;
	SEM_HANDLE* handle = (SEM_HANDLE*)(*sem);
	// 设置时间
	struct timespec ts;
	if(timeout == -1)
	{
		iret = sem_wait(&(handle->sem));
		if(iret < 0)
		{
			return SWPAR_FAIL;
		}
		return SWPAR_OK;
	}

	clock_gettime(CLOCK_REALTIME, &ts);    //获取当前时间
	ts.tv_sec += (timeout / 1000);			//加上等待时间的秒数
	ts.tv_nsec += (timeout % 1000) * 1000000; //加上等待时间纳秒数
	// 纳秒数超过1秒时需放到秒里
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec = ts.tv_nsec % 1000000000;
	while(((iret = sem_timedwait(&(handle->sem), &ts)) != 0) && (errno == EINTR));
	if(iret < 0)
	{
		return SWPAR_FAIL;
	}

	return SWPAR_OK;
}

/**
* @brief 解锁信号量
* @param [in] sem 信号量结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_sem_post(int* sem)
{
	SWPA_SEM_CHECK(sem != NULL);
	SWPA_SEM_PRINT("sem=0x%08x\n", (unsigned int)sem);
	int nval = 0;
	int iret = -1;
	SEM_HANDLE* handle = (SEM_HANDLE*)(*sem);
	if(handle->max > 0)
	{
		iret = sem_getvalue(&(handle->sem), &nval);
		if(nval >= (int)handle->max)
		{
			return SWPAR_FAIL;
		}
	}
	iret = sem_post(&(handle->sem));
	if(iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

int swpa_sem_value(int* sem)
{
	SWPA_SEM_CHECK(sem != NULL);
	SWPA_SEM_PRINT("sem=0x%08x\n", (unsigned int)sem);
	SEM_HANDLE* handle = (SEM_HANDLE*)(*sem);
	int nval = 0;
	sem_getvalue(&(handle->sem), &nval);
	return nval;
}

/**
* @brief 删除信号量
* @param [in] sem 信号量结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_sem_delete(int* sem)
{
	SWPA_SEM_CHECK(sem != NULL);
	SWPA_SEM_PRINT("sem=0x%08x\n", (unsigned int)sem);
	int iret = -1;
	SEM_HANDLE* handle = (SEM_HANDLE*)(*sem);
	iret = sem_destroy(&(handle->sem));
	if (handle->id > 0)
	{
		sem_mem_free(handle->id);
	}
	else
	{
		swpa_mem_free(handle);
		handle = NULL;
	}

	if (iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}


typedef struct tag_Mutex
{
	pthread_mutex_t mutex;
	int id;  /* BEGIN: add by huangdch, 2015/9/4:进程间锁唯一标识*/
}
MUTEX_HANDLE;

/* BEGIN: Modified by huangdch, 2015/9/4 
*进程间锁的最大数10要根据具体的应用情况进行调用*/
#define SHM_PROCESS_MUTEX_MAX_NUM   10   /* 进程锁的最大个数 */
#define SHM_PROCESS_MUTEX_KEY_ID   10001 /* 进程锁对应的共享内存主键key */
static void* g_shm_mutex_msg_addr = NULL; /* 同一个进程内管理首地址 */
static int g_shm_mutex_number = 0;        /* 同一个进程内的连接数 */

typedef struct _shm_msg_mutex
{
	pthread_mutex_t mutex; /* 锁 */
	short total_attach; /* 总的连接数,目前冗余字段 */
	short attach[SHM_PROCESS_MUTEX_MAX_NUM]; /* 连接数,进程间锁用到 */
}sw_shm_msg_mutex;

/*****************************************************************************
 函 数 名  : mutex_mem_alloc
 功能描述  : 根据id从共享内存块分配一个内存节点,分配成功将返回内存的首地址
 输入参数  : const int id 信号量标识id   
             const int size 未用
             int* state 保留     
 输出参数  : 无
 返 回 值  : NULL失败 非空成功
 调用函数  : 
 注意事项  : 只在当前文件内使用
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
void* mutex_mem_alloc(const int id, const int size, int* state)
{
	if (id < 1 || size < 1)
	{
		return -1;
	}

	int ret = 0;
	int shm_id = 0;
	int temp_size = sizeof(sw_shm_msg_mutex) + SHM_PROCESS_MUTEX_MAX_NUM * sizeof(MUTEX_HANDLE);
	int key_id = SHM_PROCESS_MUTEX_KEY_ID;
	int temp_state = 0;
	int process_flag = 0;
	void* radd = NULL;

	if (NULL == g_shm_mutex_msg_addr)
	{
		/* 连接管理区 */
		int temp_state = 0;
		g_shm_mutex_msg_addr = swpa_normal_sharedmem_alloc(key_id, temp_size, &temp_state);
		if (NULL == g_shm_mutex_msg_addr)
		{
			SWPA_SEM_PRINT("create key_id=[%d] size=[%d] failed,ret=[%d].\n", key_id, temp_size, ret);
			return NULL;
		}

		/* 初始化锁 */
		if (0 == temp_state)
		{
			sw_shm_msg_mutex* msg_node = (sw_shm_msg_mutex*)g_shm_mutex_msg_addr;

			pthread_mutexattr_t attr;
			if (0 != pthread_mutexattr_init(&attr))
			{
				return NULL;
			}
			if (0 != pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED))
			{
				return NULL;
			}	
			if (0 != pthread_mutex_init(&msg_node->mutex, &attr))
			{
				return NULL;
			}
			if (0 != pthread_mutexattr_destroy(&attr))
			{
				return NULL;
			}

			/* 首次初始化 */
			memset(&msg_node->attach, 0, sizeof(msg_node->attach));
			msg_node->total_attach = 0;
			memset(msg_node + 1, 0, temp_size - sizeof(sw_shm_msg_mutex));
		}

		process_flag = 1;
	}

	sw_shm_msg_mutex* msg_node = (sw_shm_msg_mutex*)g_shm_mutex_msg_addr;

	pthread_mutex_lock(&msg_node->mutex);

	/* 根据id在已经分配的共享内存块查找一个已分配或未占用的节点 */
	MUTEX_HANDLE* shm_node = (MUTEX_HANDLE*)(msg_node + 1);
	int k = 0;
	int flag = -1;
	int node = -1;
	for (k = 0 ; k < SHM_PROCESS_MUTEX_MAX_NUM ; k++)
	{
		if (id == shm_node[k].id) /* 找已经分配的节点 */
		{
			flag = 2;
			msg_node->attach[k]++;/* 节点的连接数 */
			msg_node->total_attach++;
			radd = (void*)&shm_node[k]; /* 返回一个地址 */
			*state = 1;
			node = k;
			break;
		}
	}
	if (-1 == flag)
	{
		for (k = 0 ; k < SHM_PROCESS_MUTEX_MAX_NUM ; k++)
		{
			if (0 == shm_node[k].id) /* 找未分配的节点 */
			{
				msg_node->attach[k]++;/* 连接的进程数 */
				msg_node->total_attach++;
				shm_node[k].id = id;
				flag = 1;
				radd = (void*)&shm_node[k]; /* 返回一个地址 */
				*state = 0;
				node = k;
				break;
			}
		}
	}


	/* 没有找到满足条件的节点进行分配 */
	if (-1 == flag && 1 == process_flag)
	{
		pthread_mutex_unlock(&msg_node->mutex);

		ret = swpa_normal_sharedmem_free(SHM_PROCESS_MUTEX_KEY_ID, g_shm_mutex_msg_addr);
		if (ret < 0)
		{
			SWPA_SEM_PRINT("free:mutex_id=[%d] failed,rang out of [%d] ret=[%d].\n", SHM_PROCESS_MUTEX_KEY_ID, SHM_PROCESS_MUTEX_MAX_NUM, ret);
		}

		SWPA_SEM_PRINT("free:mutex_id=[%d] failed,rang out of [%d] ret=[%d].\n", SHM_PROCESS_MUTEX_KEY_ID, SHM_PROCESS_MUTEX_MAX_NUM, ret);

		return NULL;
	}

	if (NULL != radd)
	{
		g_shm_mutex_number++; /* 同一个进程内的连接数+1 */
	}

	SWPA_SEM_PRINT("alloc:mutex_id=[%d] flag=[%d] node=[%d] num=[%d] attch=[%d]\n", id, flag, node, g_shm_mutex_number, msg_node->total_attach);

	pthread_mutex_unlock(&msg_node->mutex);

	return radd;
}

/*****************************************************************************
 函 数 名  : sem_mem_free
 功能描述  : 根据锁id查找对应的内存块并释放相应的节点
 输入参数  : const int id  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 注意事项  : 
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int mutex_mem_free(const int id)
{
	if (id < 1 || NULL == g_shm_mutex_msg_addr)
	{
		return -1;
	}

	int ret = 0;
	int shm_id = 0;

	sw_shm_msg_mutex* msg_node = (sw_shm_msg_mutex*)g_shm_mutex_msg_addr;

	pthread_mutex_lock(&msg_node->mutex);

	MUTEX_HANDLE* shm_node = (MUTEX_HANDLE*)(msg_node + 1);

	int flag = -1;
	int k = 0;
	for (k = 0 ; k < SHM_PROCESS_MUTEX_MAX_NUM ; k++)
	{
		if (id == shm_node[k].id)
		{
			flag = 1;
			g_shm_mutex_number--;
			msg_node->attach[k]--;
			msg_node->total_attach--;
			if (0 == msg_node->attach[k])/* 如果锁的内存节点连接数为0设置可再分配 */
			{
				SWPA_SEM_PRINT("node=[%d] id=[%d] is set 0.\n", k, id);
				shm_node[k].id = 0;
			}
			break;
		}
	}

	pthread_mutex_unlock(&msg_node->mutex);

	if (g_shm_mutex_number == 0)
	{
		SWPA_SEM_PRINT("call swpa_normal_sharedmem_free, id=[%d] num=[%d] attach=[%d]\n", id, g_shm_mutex_number, msg_node->attach);
		
		ret = swpa_normal_sharedmem_free(SHM_PROCESS_MUTEX_KEY_ID, g_shm_mutex_msg_addr);
		if (0 == ret)
		{
			g_shm_mutex_msg_addr = NULL;
		}
	}
	
	return ret;
}


/**
* @brief 创建互斥体
* @param [out] mutex 互斥体结构体指针
* @param [in] name 互斥体的名称，不需要跨进程时可填NULL。
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
* @note 其中的name主要用于跨进程互斥时，以便让另一个进程能通过名称获取到该互斥体，所以请自行保证该名称的唯一性。
*/
int swpa_mutex_create(int* mutex, const char* name)
{
	SWPA_SEM_CHECK(mutex != NULL);
	SWPA_SEM_PRINT("mutex=0x%08x\n", (unsigned int)mutex);
	SWPA_SEM_PRINT("name=%s\n", name);
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	MUTEX_HANDLE* handle = (MUTEX_HANDLE*)swpa_mem_alloc(sizeof(MUTEX_HANDLE));
	if( handle == NULL)
	{
		return SWPAR_OUTOFMEMORY;
	}

	if(name == NULL || strlen(name) <= 0)
	{
		pthread_mutex_init(&handle->mutex, &attr);
		handle->id = 0;
		(*mutex) = (int)handle;
		return SWPAR_OK;
	}
	else
	{
		swpa_mem_free(handle);
		handle = NULL;
		return SWPAR_NOTIMPL;
	}

	return SWPAR_OK;
}

/* BEGIN: Modified by huangdch, 2015/9/4 
*进程间锁创建*/
int swpa_processmutex_create(int* mutex, const int mutex_id)
{
	if (mutex_id < 1)
	{
		return -1;
	}

	int size = 0;

	SWPA_SEM_PRINT("mutex=0x%08x\n", (unsigned int)mutex);
	SWPA_SEM_PRINT("mutex_id=%d\n", mutex_id);
	MUTEX_HANDLE* pmutex = NULL;
	size = sizeof(*pmutex);

	/* 根据id申请共享内存 */
	int state = 0;
	pmutex = (MUTEX_HANDLE *)mutex_mem_alloc(mutex_id, size, &state);
	if (NULL == pmutex)
	{
		return -1;
	}

	if (0 == state)
	{
		pthread_mutexattr_t attr;
		/*设置互斥锁属性*/
		if (0 != pthread_mutexattr_init(&attr))
		{
			return -3;
		}

		/*进程间使用*/
		if (0 != pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED))
		{
			return -4;
		}	

		/*设置互斥锁*/
		if (0 != pthread_mutex_init(&pmutex->mutex, &attr))
		{
			return -5;
		}

		/*释放互斥锁属性*/
		if (0 != pthread_mutexattr_destroy(&attr))
		{
			return -6;
		}

		pmutex->id = mutex_id;
	}

	(*mutex) = (int)pmutex;

	return 0;
}

/**
* @brief 锁定互斥体
* @param [in] mutex 互斥体结构体指针
* @param [in] timeout 锁定超时。单位：毫秒
* @retval 0 成功或可锁
* @retval -1 失败或不可锁
* @see swpa_sem.h
* @note 当timeout为-1时，表示阻塞锁定；当timeout等于0时，表示尝试锁定；当timeout大于0时，即表示锁定超时时限。
*/
int swpa_mutex_lock(int* mutex, int timeout)
{

	SWPA_SEM_CHECK(mutex != NULL);
	SWPA_SEM_PRINT("mutex=0x%08x\n", (unsigned int)mutex);
	SWPA_SEM_PRINT("timeout=%d\n", timeout);
	int iret = -1;
    // 设置时间
	struct timespec ts;
	MUTEX_HANDLE* handle = (MUTEX_HANDLE*)(*mutex);
	if(timeout == -1)
	{
		iret = pthread_mutex_lock(&handle->mutex);
	}
	else if(timeout == 0)
	{
		iret = pthread_mutex_trylock(&handle->mutex);
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &ts);    //获取当前时间
		ts.tv_sec += (timeout / 1000);			//加上等待时间的秒数
		ts.tv_nsec += (timeout % 1000) * 1000000; //加上等待时间纳秒数
		// 纳秒数超过1秒时需放到秒里
		ts.tv_sec += ts.tv_nsec / 1000000000;
		ts.tv_nsec = ts.tv_nsec % 1000000000;
		iret = pthread_mutex_timedlock(&(handle->mutex), &ts);
	}
	if(iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

/**
* @brief 解锁互斥体
* @param [in] mutex 互斥体结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_mutex_unlock(int* mutex)
{
	SWPA_SEM_CHECK(mutex != NULL);
	SWPA_SEM_PRINT("mutex=0x%08x\n", (unsigned int)mutex);

	MUTEX_HANDLE* handle = (MUTEX_HANDLE*)(*mutex);
	int iret = pthread_mutex_unlock(&handle->mutex);
	if(iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

/**
* @brief 删除互斥体
* @param [in] mutex 互斥体结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_mutex_delete(int* mutex)
{
	SWPA_SEM_CHECK(mutex != NULL);
	SWPA_SEM_PRINT("mutex=0x%08x\n", (unsigned int)mutex);

	MUTEX_HANDLE* handle = (MUTEX_HANDLE*)(*mutex);

	int iret = pthread_mutex_destroy(&handle->mutex);
	if (handle->id > 0)
	{
		mutex_mem_free(handle->id);
	}
	else
	{
		swpa_mem_free(handle);
		handle = NULL;
	}
	if (iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

typedef struct tag_rwlock
{
	pthread_rwlock_t rwlock;
}
RWLOCK_HANDLE;

/**
* @brief 创建读写锁
* @param [out] rwlock 读写锁结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_rwlock_create(int* rwlock, const char* name)
{
	SWPA_SEM_CHECK(rwlock != NULL);
	SWPA_SEM_PRINT("rwlock=%0x08x", (unsigned int)rwlock);

	RWLOCK_HANDLE* handle = swpa_mem_alloc(sizeof(RWLOCK_HANDLE));
	int iret = pthread_rwlock_init((pthread_rwlock_t*)&handle->rwlock, NULL);
	if(iret < 0)
	{
		swpa_mem_free(handle);
		handle = NULL;
		return SWPAR_FAIL;
	}
	*rwlock = (int)handle;
	return SWPAR_OK;
}

/**
* @brief 锁定读写锁
* @param [in] rwlock 读写锁结构体指针
* @param [in] timeout 锁定超时。单位：毫秒
* @param [in] is_read_mode 指明是否是以读模式锁定，否则为写模式。
* @retval 0 成功或可锁
* @retval -1 失败或不可锁
* @see swpa_sem.h
* @note 当timeout为-1时，表示阻塞锁定；当timeout等于0时，表示尝试锁定；当timeout大于0时，即表示锁定超时时限。
*/
int swpa_rwlock_lock(int* rwlock, int timeout, unsigned int is_read_mode)
{
	SWPA_SEM_CHECK(rwlock != NULL);
	SWPA_SEM_PRINT("rwlock=0x%08x\n", (unsigned int)rwlock);
	SWPA_SEM_PRINT("timeout=%d\n", timeout);
	SWPA_SEM_PRINT("is_read_mode=%d\n", is_read_mode);

	RWLOCK_HANDLE* handle = (RWLOCK_HANDLE*)(*rwlock);
	if(timeout == -1)
	{
		int iret = -1;
		if(is_read_mode)
		{
			iret = pthread_rwlock_rdlock((pthread_rwlock_t*)&handle->rwlock);
		}
		else
		{
			iret = pthread_rwlock_wrlock((pthread_rwlock_t*)&handle->rwlock);
		}
		if(iret < 0)
		{
			return SWPAR_FAIL;
		}
		return SWPAR_OK;
	}
	else if(timeout == 0)
	{
		int iret = -1;
		if(is_read_mode)
		{
			iret = pthread_rwlock_tryrdlock((pthread_rwlock_t*)&handle->rwlock);
		}
		else
		{
			iret = pthread_rwlock_trywrlock((pthread_rwlock_t*)&handle->rwlock);
		}
		if(iret < 0)
		{
			return SWPAR_FAIL;
		}
		return SWPAR_OK;
	}
	else
	{
		int iret = -1;
		struct timespec curtime;
		clock_gettime(CLOCK_MONOTONIC, &curtime);
		curtime.tv_sec += (timeout / 1000);
		curtime.tv_nsec = (timeout % 1000) * 1000000;
		if(is_read_mode)
		{
			iret = pthread_rwlock_timedrdlock((pthread_rwlock_t*)&handle->rwlock, &curtime);
		}
		else
		{
			iret = pthread_rwlock_timedwrlock((pthread_rwlock_t*)&handle->rwlock, &curtime);
		}
		if(iret < 0)
		{
			return SWPAR_FAIL;
		}
		return SWPAR_OK;
	}
}

/**
* @brief 解锁读写锁
* @param [in] rwlock 读写锁结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_rwlock_unlock(int* rwlock)
{
	SWPA_SEM_CHECK(rwlock != NULL);
	SWPA_SEM_PRINT("rwlock=0x%08x\n", (unsigned int)rwlock);
	RWLOCK_HANDLE* handle = (RWLOCK_HANDLE*)(*rwlock);

	int iret = pthread_rwlock_unlock((pthread_rwlock_t*)&handle->rwlock);
	if(iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

/**
* @brief 删除读写锁
* @param [in] rwlock 读写锁结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_rwlock_delete(int* rwlock)
{
	SWPA_SEM_CHECK(rwlock != NULL);
	SWPA_SEM_PRINT("rwlock=0x%08x\n", (unsigned int)rwlock);

	RWLOCK_HANDLE* handle = (RWLOCK_HANDLE*)(*rwlock);
	int iret = pthread_rwlock_destroy((pthread_rwlock_t*)&handle->rwlock);
	swpa_mem_free(handle);
	handle = NULL;
	if(iret < 0)
	{
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

