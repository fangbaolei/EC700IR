#ifndef __SW_ONVIF_RTSP_PARAMETER_H__
#define __SW_ONVIF_RTSP_PARAMETER_H__

#define RTSP_STRING_LEN 	128
#define RTSP_STREAM_MAX 	2			//目前码流数量


//ONVIF 参数相关
typedef struct _ONVIF_PARAM_INFO
{
	int iOnvifEnable;
	int iAuthenticateEnable;							//监权 0 不使能  1 使能
	_ONVIF_PARAM_INFO()
	{
		iOnvifEnable = 0;
		iAuthenticateEnable = 0;
	}
}ONVIF_PARAM_INFO;


//RTSP 参数相关
typedef enum _RTSP_STREAM_INDEX
{
	RTSP_STREAM_MAJOR = 0,
	RTSP_STREAM_MINOR
}RTSP_STREAM_INDEX;


typedef struct _RTSP_PARAM_INFO
{
	int iRTSPStreamNum;									//RTSP 流数量
	int rgCommunicationMode[RTSP_STREAM_MAX];			//通讯模式 0 单播  1 组播
	int iAuthenticateEnable;							//监权 0 不使能  1 使能
	int iAutoControlBitrateEnable;						//自动控制码率 0 不使能 1 使能
	int iRTSPServerPort;								//RTSP服务端口号
	char rgStreamName[RTSP_STREAM_MAX][RTSP_STRING_LEN];//流名称
//	char rgUserName[RTSP_STRING_LEN];					//用户名
//	char rgPassword[RTSP_STRING_LEN];					//监权密码张

	_RTSP_PARAM_INFO()
	{
		iRTSPStreamNum			= 2;				//默认标配两路码流
		rgCommunicationMode[0]	= 0;
		rgCommunicationMode[1]	= 0;				//两路默认都是单播
		iAuthenticateEnable		= 0;				//默认不开鉴权
		iAutoControlBitrateEnable = 1;
		iRTSPServerPort			= 554;				//默认端口号
		strcpy(rgStreamName[0], "h264ESVideoTest");
		strcpy(rgStreamName[1], "h264ESVideoTestSecond");
//		strcpy(rgUserName, "RTSPUserName");
//		strcpy(rgPassword, "RTSPPassword");
	}
}RTSP_PARAM_INFO;


typedef enum E_H264
{
	H264_STATUS_FIRST_STREAM_CONN = 0,		//连接状态指令
	H264_STATUS_FIRST_STREAM_REDUCE,		//降低信号
	H264_STATUS_FIRST_STREAM_ASCEND,		//升高信号
	H264_STATUS_SECOND_STREAM_CONN,
	H264_STATUS_SECOND_STREAM_REDUCE,
	H264_STATUS_SECOND_STREAM_ASCEND,
	H264_STATUS_AUTHENTICATE
} E_H264_STATUS;

typedef struct _H264_STATUS_HEADER
{
	E_H264_STATUS eH264StatusCmd;
	unsigned int iDataLen;
}H264_STATUS_HEADER;

#endif
