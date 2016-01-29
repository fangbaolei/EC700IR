#include "swpa.h"
#include "swpa_list.h"

typedef struct tagLISTNODE
{
	void* value;                //节点的值
	struct tagLISTNODE *prev;   //上一个节点
	struct tagLISTNODE *next;   //下一个节点
}LISTNODE;

typedef struct tagLIST
{
	int       mutex;  //互斥量
	int       sem;    //信号量
	int       lock;   //是否锁的标志
	LISTNODE *head;	  //头结点
	LISTNODE *tail;   //尾节点
	LISTNODE *now;    //当前节点
	int       fdel;   //是否删除
}LIST;

/**
 *@brief 创建链表
 *@return 成功返回链表句柄，失败返回-1
 */
int swpa_list_create(int max_size)
{
	LIST *lst = (LIST *)swpa_mem_alloc(sizeof(LIST));
	if(0 != lst)
	{
		swpa_memset(lst, 0, sizeof(LIST));
		swpa_mutex_create(&lst->mutex, 0);
		swpa_sem_create(&lst->sem, 0, max_size);
		return (int)lst;
	}
	return -1;
}

/**
 *@brief 释放链表资源
 *@param [in] handle 链表句柄
 *@return 成功返回0，失败返回-1
 */
int swpa_list_destroy(int handle)
{
	LIST *lst = (LIST *)handle;
	if(lst)
	{
		swpa_mutex_lock(&lst->mutex, -1);
		lst->lock++;
		LISTNODE *node = lst->head;
		while(node)
		{
			LISTNODE *tmp = node->next;
			node = node->next;
			swpa_mem_free(tmp);
		}
		lst->lock--;
		swpa_mutex_unlock(&lst->mutex);
		swpa_mutex_delete(&lst->mutex);
		swpa_mem_free(lst);
	}
	return 0;
}

/**
 *@brief 向链表添加一个节点
 *@param [in] handle 链表句柄
 *@param [in] value 节点的值
 *@return 成功返回新插入节点的位置，失败返回-1
 */
int swpa_list_add(int handle, void *value)
{
	LIST *lst = (LIST *)handle;
	LISTNODE *now = 0;
	if(lst)
	{
		swpa_mutex_lock(&lst->mutex, -1);
		lst->lock++;
		if(0 == swpa_sem_post(&lst->sem))
		{
			now = (LISTNODE *)swpa_mem_alloc(sizeof(LISTNODE));
			if(now)
			{
				now->value = value;
				now->prev = 0;
				now->next = 0;
				
				if(1 == swpa_sem_value(&lst->sem))
				{
					lst->head = lst->tail = now;
				}
				else
				{
					now->prev = lst->tail;
					lst->tail->next = now;
					lst->tail = now;
				}	
			}
		}
		lst->lock--;
		swpa_mutex_unlock(&lst->mutex);		
	}
	return 0 == now ? -1 : (int)now;
}

/**
 *@brief 向链表插入一个节点
 *@param [in] handle 链表句柄
 *@param [in] pos 节点位置
 *@param [in] value 节点的值
 *@return 成功返回节点位置，失败返回-1
 */
int swpa_list_insert(int handle, int pos, void *value)
{
	LIST *lst = (LIST *)handle;
	LISTNODE *now = 0;
	if(lst)
	{
		swpa_mutex_lock(&lst->mutex, -1);
		lst->lock++;
		if(0 == swpa_sem_post(&lst->sem))
		{
			LISTNODE *ln = (LISTNODE *)pos;
			if(ln)
			{
				now = (LISTNODE *)swpa_mem_alloc(sizeof(LISTNODE));
				now->value = value;
				now->prev = ln;
				now->next = ln->next;
				ln->next = now;
			}
		}
		lst->lock--;
		swpa_mutex_unlock(&lst->mutex);
	}
	return 0 == now ? -1 : (int)now;
}

/**
 *@brief 删除链表中的一个节点
 *@param [in] handle 链表句柄     
 *@param [in] pos 节点位置
 *@param [in] 成功返回0，失败返回-1
 */
int swpa_list_delete(int handle, int pos)
{
	LIST *lst = (LIST *)handle;
	LISTNODE *now = 0;
	if(lst && 0 < swpa_sem_value(&lst->sem) && 0 == swpa_sem_pend(&lst->sem, -1))
	{
		swpa_mutex_lock(&lst->mutex, -1);
		lst->lock++;
		now = (LISTNODE *)pos;
		if(now)
		{
			if(lst->now == now)
			{
				lst->fdel = 1;
				lst->now = lst->now->next;
			}
			if(now->prev)
			{
				now->prev->next = now->next;
			}
			if(now->next)
			{
				now->next->prev = now->prev;
			}
			if(now == lst->tail)
			{
				lst->tail = lst->tail->prev;
			}
			if(now == lst->head)
			{
				lst->head = lst->head->next;
			}
			swpa_mem_free(now);			
		}
		lst->lock--;
		swpa_mutex_unlock(&lst->mutex);
	}
	return 0 == now ? -1 : 0;
}

/**
 *@brief 从链表的头结点删除一个节点
 *@param [in] handle 链表句柄
 *@param [in] from_head 为0表明删除头节点，为1表明删除尾节点
 *@param [in] timeout -1表示阻塞锁定；0表示尝试锁定；大于0表示锁定超时时限。
 *@retval 返回节点的值
 */
void* swpa_list_remove(int handle, int from_head, int timeout)
{
	LIST *lst = (LIST *)handle;
	void* value = 0;
	if(lst)
	{
		if(0 == swpa_sem_pend(&lst->sem, timeout))
		{
			swpa_mutex_lock(&lst->mutex, -1);
			LISTNODE *now = 0;
			if(0 == from_head)
			{
				now = lst->head;
				lst->head = lst->head->next;
				if(now == lst->tail)
				{
					lst->tail = lst->tail->next;
				}
			}
			else
			{
				now = lst->tail;
				lst->tail = lst->tail->prev;
				if(now == lst->head)
				{
					lst->head = lst->head->prev;
				}
			}
			if(lst->now == now)
			{
				lst->fdel = 1;
				lst->now = lst->now->next;
			}
			value = now->value;
			swpa_mem_free(now);			
			swpa_mutex_unlock(&lst->mutex);
		}
	}
	return value;
}

/**
 *@brief 根据节点位置获得节点的值
 *@param [in] handle 链表句柄
 *@param [in] pos 节点位置
 *@return 成功返回节点值，失败返回0
 */
void* swpa_list_value(int handle, int pos)
{
	LIST *lst = (LIST *)handle;
	if(lst && 0 < pos)
	{
		swpa_mutex_lock(&lst->mutex, -1);
		lst->lock++;
		LISTNODE *now = (LISTNODE *)pos;
		void *value = now->value;
		lst->lock--;
		swpa_mutex_unlock(&lst->mutex);
		return value;
	}
	return 0;
}

/**
 *@brief 链表的大小
 *@param [in] handle 链表句柄
 *@return 返回链表节点的个数
 */
int swpa_list_size(int handle)
{
	int size = 0;
	LIST *lst = (LIST *)handle;
	if(lst)
	{
		size = swpa_sem_value(&lst->sem);
	}
	return size;
}

/**
 *@brief 遍历链表，遍历之前必须先open
 *@param [in] handle 链表句柄
 *@return 返回链表头结点
 */
int swpa_list_open(int handle, int from_head)
{
	LIST *lst = (LIST *)handle;
	if(lst && swpa_list_size(handle) > 0)
	{
		swpa_mutex_lock(&lst->mutex, -1);
		lst->lock++;
		lst->now = (0 == from_head) ? lst->head : lst->tail;
		lst->fdel = 0;
		return lst->now ? (int)lst->now : -1;
	}
	return -1;
}

/**
 *@brief 链表节点下移一个节点
 *@brief handle 链表句柄
 *@return 返回当前节点位置
 */
int swpa_list_next(int handle)
{
	LIST *lst = (LIST *)handle;
	if(lst && lst->now)
	{
		if(lst->fdel)
		{
			lst->fdel = 0;
		}
		else
		{
			lst->now = lst->now->next;
		}
	}
	return lst->now ? (int)lst->now : -1;
}

/**
 *@brief 链表节点上移一个节点
 *@brief handle 链表句柄
 *@return 返回当前节点位置
 */
int swpa_list_prev(int handle)
{
	LIST *lst = (LIST *)handle;
	if(lst && lst->now)
	{
		lst->fdel = 0;
		lst->now = lst->now->prev;
	}
	return lst->now ? (int)lst->now : -1;
}

/**
 *@brief 停止遍历
 *@brief handle 链表句柄
 *@return 成功返回0，失败返回-1
 */
int swpa_list_close(int handle)
{
	LIST *lst = (LIST *)handle;
	if(lst && lst->lock > 0)
	{
		lst->lock--;
		swpa_mutex_unlock(&lst->mutex);
		return 0;
	}
	return -1;
}

