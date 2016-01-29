
#ifndef _HV_API_SAVEH264_H_
#define _HV_API_SAVEH264_H_
#include "stdafx.h"
#include <Windows.h>

#ifndef _H264_API
#define _H264_API extern "C" __declspec(dllexport)
#else
#define _H264_API extern "C" __declspec(dllimport)
#endif

_H264_API int H264VideoSaverInit();
_H264_API int H264VideoSaverCoInit();
_H264_API int H264VideoSaverClose(void* pvHandle);
_H264_API int H264VideoSaverWirteOneFrame(void* pvHandle, PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame);
// 说明：支持mp4、avi、mkv等视频封装格式。
//_H264_API void* H264VideoSaverOpen(const char* szOutputFile, int iFrameNum = 12);
_H264_API void* H264VideoSaverOpen(const char* szOutputFile, int iFrameNum=12, int iWidth=1600, int iHeight=1080, int videoCodeRate=8000000);
_H264_API int H264CheckDistSpace(char *pszDrive);


#endif