#ifndef _PANORAMIC_CAPTURER_H__
#define _PANORAMIC_CAPTURER_H__

#include "hvthreadbase.h"
#include "HvSockUtils.h"

typedef struct _PANORAMIC_CAPTURER_PARAM
{
    int nEnableFlag;
    char szCamaraIP[20];

    _PANORAMIC_CAPTURER_PARAM()
    {
        nEnableFlag = 0;
        memset(szCamaraIP, 0, 20);
    }
}PANORAMIC_CAPTURER_PARAM;

typedef struct _PANORAMIC_CAPTURER_FRAME_INFO
{
    DWORD32 dwLen;
    WORD16 wWidth;
    WORD16 wHeight;
    _PANORAMIC_CAPTURER_FRAME_INFO()
    {
        dwLen = 0;
        wWidth = 0;
        wHeight = 0;
    }
}PANORAMIC_CAPTURER_FRAME_INFO;
#endif
