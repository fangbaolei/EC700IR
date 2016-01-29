/**
* @file SWFilterStruct.h 
* @brief Filter对外发布的结构体。
* @copyright Signalway All Rights Reserved
* @author zhaopy
* @date 2013-04-02
* @version 1.0
*/

#ifndef _SWFILTER_STRUCT_H_
#define _SWFILTER_STRUCT_H_

#include "SWBaseType.h"

// CSWNetSourceFilter
typedef struct tag_CameraInfo
{
	INT iCameraType;
	CHAR szIp[32];
	WORD wPort;
}
CAMERA_INFO;

// CSWH264SourceFilter
typedef struct tag_OverlayInfo
{
	INT iTopX; //顶点X
	INT iTopY;//顶点Y
	INT iFontSize;//字体大小，不超过128
	DWORD dwColor;// 字体RGB颜色
	BOOL fEnableTime;// 使能时间显示
	INT	iTextLen;// 叠加字符串长度
	CHAR szInfo[512]; // 叠加字符串
}
H264_OVERLAY_INFO;

typedef struct _JPEG_OVERLAY_INFO
{
	BOOL fEnable;	       //是否使能JPEG字符叠加
	INT  iX;             //叠加位置
	INT  iY;             //叠加位置
	INT  iFontSize;      //叠加字符大小
	INT  iColor;         //叠加颜色
	BOOL fEnableTime;    //叠加时间
	CHAR szInfo[255];    //叠加附加信息
	BOOL fEnablePlate;   //识别结果是否要叠加车牌号码
}JPEG_OVERLAY_INFO;

typedef struct tag_H264Param
{
	INT iForceFrameType; //强制下一帧输出类型，当前无效，应设置为-1
	INT iTargetBitrate; // 码流比特率：32Kbps-16Mbps
	INT intraFrameInterval; // I帧间隔
	INT iResolution; // H.264分辨率
	INT iRateControl;	//码率控制 0：VBR 1：CBR
	INT iVbrDuration;   //场景变化的采样时间
	INT iVbrSensivitity; //场景变化的敏感度
}
H264_PARAM;

typedef struct tag_H264FilterParam
{
	H264_OVERLAY_INFO cOverlayInfo;
	H264_PARAM cH264Param;
	INT iChannelNum;		//通道号，用于区分不同的H264
}
H264_FILTER_PARAM;


#endif
