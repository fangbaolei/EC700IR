/**
* @file		HvDebug.h
* @version	1.0
* @brief	用于对设备进行研发调试的一些工具宏
* @author	Shaorg
* @date		2011-07-01
*/

#ifndef _HVDEBUG_H_
#define _HVDEBUG_H_

// 使能计时器宏
#define ENABLE_HVDEBUG_TIMER

// 使能标记宏
#define ENABLE_HVDEBUG_MARK

//------------------------------------------

/*
 * 计时器宏使用示例1：
 * ...
 * tS(MyMark);
 * SomeFunc();
 * tE(MyMark);
 * ...
 * 如上所示，即可方便快捷的知道SomeFunc这个函数的执行时间。
 *
 * 计时器宏使用示例2：
 * ...
 * tSS(MyMark,10);
 * SomeFunc();
 * tEE(MyMark,10);
 * ...
 * 如上所示，即可方便快捷的知道SomeFunc这个函数执行10次的平均时间。
 */

#define tS L_TIMER_RUN
#define tE L_TIMER_END

#define tSS L_TIMER_RUN_EX
#define tEE L_TIMER_END_EX

#ifdef ENABLE_HVDEBUG_TIMER

    #define L_TIMER_RUN(id) \
        \
        DWORD dwStart##id = GetSystemTick();

    #define L_TIMER_END(id) \
        \
        char szMsg##id[256];\
        sprintf(szMsg##id, "{%s} %6d ms\r\n", #id, (int)(GetSystemTick() - dwStart##id));\
        HV_Trace(5, "%s", szMsg##id);

#else

    #define L_TIMER_RUN(id)
    #define L_TIMER_END(id)

#endif

#ifdef ENABLE_HVDEBUG_TIMER

    #define L_TIMER_RUN_EX(id,T) \
        \
        DWORD dwStart##id##T = GetSystemTick();\
        static DWORD dwTimeMs##id##T = 0;\
        static DWORD dwCount##id##T = 0;

    #define L_TIMER_END_EX(id,T) \
        \
        char szMsg##id##T[256];\
        dwTimeMs##id##T += (GetSystemTick() - dwStart##id##T);\
        dwCount##id##T++;\
        if ( dwCount##id##T >= T )\
        {\
            sprintf(szMsg##id##T, "{%s,%s} %d ms\r\n", #id, #T, int((float)dwTimeMs##id##T / dwCount##id##T));\
            HV_Trace(5, "%s", szMsg##id##T);\
            dwTimeMs##id##T = 0;\
            dwCount##id##T = 0;\
        }

#else

    #define L_TIMER_RUN_EX(id,T)
    #define L_TIMER_END_EX(id,T)

#endif

//------------------------------------------

// 标记宏使用说明： 该宏用于在代码中打印标记，通过打印信息查看代码执行到何处。

#ifdef ENABLE_HVDEBUG_MARK
    #define HVDEBUG_MARK \
    HV_Trace(5, "HvDebugMark:[%s]-[%s]-[%d]\n", __FILE__, __FUNCTION__, __LINE__);
#else
    #define HVDEBUG_MARK
#endif

//------------------------------------------

#endif
