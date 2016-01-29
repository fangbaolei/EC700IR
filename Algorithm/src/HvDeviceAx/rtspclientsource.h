#ifndef RTSPCLIENTSOURCE_H
#define RTSPCLIENTSOURCE_H
#include <string>

#include <winsock2.h>
#include<stdio.h>

class TaskScheduler;
class UsageEnvironment;
class XLW_Thread;

#define YY_STATIC_LIB 0
#if YY_STATIC_LIB
//#include <QDebug>
#define YY_DEBUG(var)
#define DLL_HERAD
#else
#define YY_DEBUG(var)
#define DLL_HERAD extern "C" __declspec(dllexport)
#endif


//视频信息
typedef struct VIDEO_INFO_S
{
    int		u16ImageWidth;					//图像宽度
    int		u16ImageHeight;					//图像高度
    int		u32FrameRate;					//帧率
    int      u16EncodeType;                  //视频编码格式 0 264 1 avi 2 mjpeg
}VIDEO_INFO_S,*LPVIDEO_INFO_S;


//音视频帧头
typedef struct FRAME_HEAD_S
{
    int		u16FrameType;					// 0 音频帧  1 I帧 3 P帧
    time_t             TimeStamp;                      //时间戳
    int                 u32Rsvd;						//保留
    VIDEO_INFO_S        videoInfo;
}FRAME_HEAD_S, *LP_FRAME_HEAD_S;


#if YY_STATIC_LIB
typedef void ( SLW_StreamBack)(void* pFirstParameter, unsigned char* frame,int frameSize,FRAME_HEAD_S frame_head);
#else
typedef void (WINAPI SLW_StreamBack)(void* pFirstParameter, unsigned char* frame,int frameSize,FRAME_HEAD_S frame_head);
#endif





DLL_HERAD HANDLE xlw_RtspClient_start(const char *url,SLW_StreamBack *stream_back,void* pFirstParameter);
DLL_HERAD void xlw_RtspClient_stop(HANDLE fd);
DLL_HERAD int xlw_RtspClient_queryState(HANDLE fd);



#endif // RTSPCLIENTSOURCE_H
