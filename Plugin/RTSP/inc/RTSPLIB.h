#ifndef RTSPLIB_HH_INCLUDED
#define RTSPLIB_HH_INCLUDED

#include "SWOnvifRtspParameter.h"

#ifdef __cplusplus
extern "C" {
#endif


bool StartRTSP( int* piErr, RTSP_PARAM_INFO *cRtspParamInfo);

bool StopRTSP( int* piErr);

bool SendH264Data(unsigned char *pbData,int iDataSize );


/*
@brief: 发送H264数据
@note: 阻塞式，无缓存
*/
bool SendData(unsigned char* pbData, const int iDataSize);


#ifdef __cplusplus
}
#endif

#endif // RTSPLIB_HH_INCLUDED
