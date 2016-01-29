#ifndef __CONFIG_H__
#define __CONFIG_H__

#define PLATFORM_WINDOWS            1
#define PLATFORM_DSP_BIOS           2
#define PLATFORM_LINUX              3
#define PLATFORM_DSP_NOBIOS         4

#ifdef WIN32
    #define RUN_PLATFORM            PLATFORM_WINDOWS
    #ifdef RELEASE_TO_MARKET
        #include <windows.h>
        #include "swWinError.h"
    #endif
#else
    #ifdef LINUX
        #include <sys/types.h>
        #include <stdint.h>
        #include "swwinerror.h"
        #define RUN_PLATFORM        PLATFORM_LINUX
    #else
        #ifndef _NO_DSPBIOS_
            #define RUN_PLATFORM    PLATFORM_DSP_BIOS
        #else
            #define RUN_PLATFORM    PLATFORM_DSP_NOBIOS
        #endif
    #endif
#endif

// 单板模式编译宏
#ifndef SINGLE_BOARD_PLATFORM
//#define SINGLE_BOARD_PLATFORM
#endif

// 单相机模式编译宏
#ifndef _HV_CAMERA_PLATFORM_
//#define _HV_CAMERA_PLATFORM_
#endif

// 200W相机25fps模式编译宏
#ifndef _CAMERA_PIXEL_200W_25FPS_
//#define _CAMERA_PIXEL_200W_25FPS_
#endif

// 500W相机模式编译宏
#ifndef _CAMERA_PIXEL_500W_
//#define _CAMERA_PIXEL_500W_
#endif

// SEED仿真器调试模式编译宏
#ifndef _USE_SEED_DEBUG_
//#define _USE_SEED_DEBUG_
#endif

// 一体机RTM模式编译宏
#ifndef _HVCAM_PLATFORM_RTM_
#define _HVCAM_PLATFORM_RTM_
#endif

// IPT放在主CPU端模式编译宏
#ifndef IPT_IN_MASTER
#define IPT_IN_MASTER
#endif

#endif // #ifndef __CONFIG_H__
