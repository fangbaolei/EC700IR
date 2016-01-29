#include "stdio.h"
#include "swpa_timer.h"
#include "swpa.h"
#include "swpa_private.h"
#include <signal.h>
#include <sys/time.h>

#define MAX_TIMER_COUNT 256

#ifdef SWPA_TIMER
#define SWPA_TIMER_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_TIMER_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_TIMER_PRINT(fmt, ...)
#define SWPA_TIMER_CHECK(arg)
#endif

static char g_timerstatus[MAX_TIMER_COUNT] = {0};

typedef struct _SWPA_TIMER_T
{
    int id;
    int timeout;
    void (*alarm_routine)(int);
}SWPA_TIMER_T;



/**
* @brief 创建定时器
* @param [out] timer 定时器结构体指针
* @param [in] timeout 定时时间。单位：毫秒
* @param [in] alarm_routine 定时器警报函数。注：该函数的参数即为“定时器唯一标识码”
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_create(
    int* timer,
    unsigned int timeout,
    void (*alarm_routine)(int)
)
{
    int index;
    SWPA_TIMER_T * ptimer = NULL; 
    
    SWPA_TIMER_CHECK(timer != NULL && timeout > 0 && alarm_routine != NULL);
    SWPA_TIMER_PRINT("timer=0x%08x\n", timer);
    SWPA_TIMER_PRINT("timeout=%d\n", timeout);
    SWPA_TIMER_PRINT("alarm_routine=0x%08x\n", alarm_routine);
    
    ptimer = (SWPA_TIMER_T *)swpa_mem_alloc(sizeof(SWPA_TIMER_T));
    if (NULL == ptimer)
    {
        SWPA_PRINT(("Err: no enough memory for ptimer! [SWPAR_OUTOFMEMERY]\n"));
        return SWPAR_OUTOFMEMORY;
    }
    
    for(index=0; index<MAX_TIMER_COUNT; index++)
    {
        if(g_timerstatus[index] == 0)
          break;
    }
    if(index >= MAX_TIMER_COUNT)
    {
        return SWPAR_OUTOFMEMORY;
    }
    ptimer->id = index;
    ptimer->timeout = timeout;
    ptimer->alarm_routine = alarm_routine;
    
    *timer = (int)ptimer;
    
    return SWPAR_OK;
}

/**
* @brief 获取定时器唯一标识码
* @param [in] timer 定时器结构体指针
* @param [out] id 定时器唯一标识码
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_getid(int timer, unsigned int* id)
{
    SWPA_TIMER_T * ptimer = (SWPA_TIMER_T *)timer; 
    
    SWPA_TIMER_CHECK(NULL != ptimer);
    SWPA_TIMER_CHECK(ptimer->id >= 0 && ptimer->id < MAX_TIMER_COUNT && id != NULL);
    SWPA_TIMER_PRINT("ptimer->id=%d\n", ptimer->id);
    SWPA_TIMER_PRINT("id=0x%08x\n", id);
    
    *id = ptimer->id;
    
    return SWPAR_OK;
}






/**
* @brief 启动定时器
* @param [in] timer 定时器结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_start(int timer)
{
    SWPA_TIMER_T * ptimer = (SWPA_TIMER_T *)timer; 
    
    SWPA_TIMER_CHECK(NULL != ptimer);
    
    //printf("%p\n", timer.alarm_routine);
    SWPA_TIMER_CHECK(ptimer->id >= 0 && ptimer->id < MAX_TIMER_COUNT
    && ptimer->timeout > 0 && ptimer->alarm_routine != NULL);
    SWPA_TIMER_PRINT("ptimer->id=%d\n", ptimer->id);
    SWPA_TIMER_PRINT("ptimer->timeout=%d\n", ptimer->timeout);
    SWPA_TIMER_PRINT("ptimer->alarm_routine=%d\n", ptimer->alarm_routine);
    
    struct itimerval itv;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 200;
    int iret = -1;
    //char X = '0';
    //printf("%p\n", timer.alarm_routine);
    signal(SIGALRM, ptimer->alarm_routine);
    setitimer(ITIMER_REAL, &itv, NULL);
    //if(iret < 0)
    //{
     //   printf("333333333333\n");
    //}
    //itv.it_value.tv_sec = 1;//timer.timeout / 1000;
    //itv.it_value.tv_usec = 0;//(timer.timeout % 1000) * 1000;
    //setitimer(ITIMER_REAL, &itv, NULL);
    //printf("%p\n", timer.alarm_routine);
    //signal(SIGALRM, timer.alarm_routine);
    return SWPAR_OK;
}

