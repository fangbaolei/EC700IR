#ifndef _VIDEO_PRIVATE_H
#define _VIDEO_PRIVATE_H

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

#include "swBaseType.h"

/* 回调函数定义. */
typedef HRESULT VIDEO_CALLBACK( PVOID pvUserData, PVOID pvImgInfo, PVOID pvImage );

/* 视频帧信息数据结构定义. */
typedef struct {
	DWORD32			dwTime;		//取得视频时间.
	DWORD32			dwFlag;		//某些标识.
	DWORD32			dwFrameNo; //视频编号.
	PVOID			pvCustom;	//指向其他一些信息.
} VP_IMAGE_INFO;

typedef struct tagVideoOptionParam {
	int 			iBuffMode;
	int				iBuffNum;
	int 			iBuffCtrlNum;
	int				iWidth;
	int				iHeight;
	int				iBrightness;
	int				iContrast;
	int				iSaturation;
	int 			iSaturation_U;
	int				iSaturation_V;
	int				iHue;
	int				iMode;
	int 			iImageType;
	int				iMaxWidth;
	int				iMaxHeight;
} VideoOptionParam;

typedef enum {
	IMAGE_FM_JPEG,
	IMAGE_FM_YUV,
	IMAGE_FM_RGB,
	IMAGE_MAX_FM_NUM
} IMAGE_FILE_FORMAT;

typedef enum {
	VP_SAT_UV = 0,
	VP_SAT_U = 1,
	VP_SAT_V = 2
} VP_SAT_TYPE;

#ifdef __cplusplus
	}
#endif	/* #ifdef __cplusplus */

#endif
