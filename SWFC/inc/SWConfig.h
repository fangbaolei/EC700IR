//平台定义相关的头文件
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define PLATFORM_WINDOWS            1
#define PLATFORM_DSP_BIOS           2
#define PLATFORM_LINUX              3
#define PLATFORM_DSP_NOBIOS         4

#ifdef WIN32
    #define RUN_PLATFORM            PLATFORM_WINDOWS
    #include <windows.h>
    #include <stdio.h>
#else
    #ifdef LINUX
        #include <stdint.h>        
        #include <sys/types.h>
        #define RUN_PLATFORM        PLATFORM_LINUX
    #else
        #ifndef _NO_DSPBIOS_
            #define RUN_PLATFORM    PLATFORM_DSP_BIOS
        #else
            #define RUN_PLATFORM    PLATFORM_DSP_NOBIOS
        #endif
    #endif
#endif

#define SWFC_DEBUG_ARG_ENABLE
#define SWFC_DEBUG_ERR_ENABLE

#endif // #ifndef __CONFIG_H__

