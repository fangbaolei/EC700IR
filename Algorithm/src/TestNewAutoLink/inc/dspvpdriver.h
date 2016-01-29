#ifndef _DSPVPDRIVER_H
#define _DSPVPDRIVER_H

#include "dspsys_def.h"
#include "VideoBaseType.h"

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

#define DSP_VPPORT_MAX_NUM			3

/*
函数说明:该函数用来取得某一视频口的亮度值.
函数返回值:
	返回S_OK表示读取亮度值成功.
	返回E_POINTER表示piBrightness为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetVideoBrightness( 
	int 		iPort, 
	PINT 		piBrightness
);

/*
函数说明:该函数用来设置某一视频口的亮度值.
函数返回值:
	返回S_OK表示设置亮度值成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_SetVideoBrightness( 
	int 		iPort,
	int 		iBrightness
);

/*
函数说明:该函数用来取得某一视频口的饱和度值.
函数返回值:
	返回S_OK表示读取饱和度值成功.
	返回E_POINTER表示piSaturation为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错;
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetVideoSaturation(
	int 		iPort, 
	PINT 		piSaturation,
	int			iType
);

/*
函数说明:该函数用来设置某一视频口的饱和度值.
函数返回值:
	返回S_OK表示设置饱和度值成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_SetVideoSaturation( 
	int 		iPort, 
	int 		iSaturation,
	int			iType
);

/*
函数说明:该函数用来取得某一视频口的对比度值.
函数返回值:
	返回S_OK表示读取对比度值成功.
	返回E_POINTER表示piContrast为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错;
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetVideoContrast( 
	int 		iPort, 
	PINT 		piContrast
);

/*
函数说明:该函数用来设置某一视频口的对比度值.
函数返回值:
	返回S_OK表示设置对比度值成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_SetVideoContrast( 
	int 		iPort, 
	int 		iContrast
);

/*
函数说明:该函数用来取得某一视频口的色度值.
函数返回值:
	返回S_OK表示读取色度值成功.
	返回E_POINTER表示piHue为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错;
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetVideoHue( 
	int 		iPort, 
	PINT 		piHue
);

/*
函数说明:该函数用来设置某一视频口的色度值.
函数返回值:
	返回S_OK表示设置色度值成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_SetVideoHue( 
	int 		iPort, 
	int 		iHue
);

/*
函数说明:该函数用来取得某一视频口的状态,具体解析由应用层来进行.
	返回S_OK表示取状态值成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetStatus(
	int				iPort,
	PDWORD32 		pdwValue,
	DWORD32			dwFirstLen,
	PDWORD32		pdwReadLen
);

/*
函数说明:该函数用来复位某一视频口的前端.
	返回S_OK表示复位成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_ResetDevice(
	int 			iPort,
	PBOOL 			pfStatus
);

/*
函数说明:该函数用来复位某一视频口驱动程序.
	返回S_OK表示复位成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_ResetDriver(
	int 			iPort,
	PBOOL 			pfStatus
);

/* 
函数说明:该函数用来设置回调函数.在每次从VP口取得一桢时,允许用户通过回调函数保留一些状态值.
函数返回值:
	返回S_OK表示设置回调函数成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_POINTER表示pfnCaptureCallback为INVALID_POINTER.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_SetCaptureCallback(
	int 			iPort, 
	VIDEO_CALLBACK 	pfnCaptureCallback,
	PVOID 			pvUserData
);

/*
函数说明:该函数用来取得当前视频端口驱动层缓冲区个数.
函数返回值:
	返回S_OK表示取缓冲区个数成功.	
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_POINTER表示piCount为INVALID_POINTER.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetBufferCount( 
	int 			iPort, 
	PINT	 		piCount
);

/*
函数说明:该函数用来取得当前视频端口驱动层缓冲区个数.
函数返回值:
	返回S_OK表示设置缓冲区个数成功.	
	返回E_INVALIDARG表示传入的参数有错,如该端口系统当前不支持和缓冲区数目过大.	
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_SetBufferCount(
	int 			iPort,
	int 			iCount
);

/* 
函数说明:该函数用来从某一视频口取得一帧图象以及该图象的信息.
函数返回值:
	返回S_OK表示取缓冲区个数成功.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_POINTER表示pImage或pwImageInfo为INVALID_POINTER.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_GetOneFrame(
	int 				iPort,		//当前操作的视频口.
	HV_COMPONENT_IMAGE 	*pImage,	//指向所取得的图象数据.
	VP_IMAGE_INFO		*pImageInfo	//指向当前所取得的图象信息,如果为空则急略.
);

/* 
函数说明:该函数用来释放所取得的视频图象.
函数返回值:
	返回S_OK表示取缓冲区个数成功.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_POINTER表示pImage为INVALID_POINTER.
	返回S_FALSE表示当前的视频端口未使用. */
HRESULT VIDEO_FreeOneFrame(
	int 				iPort,		//当前操作的视频口.
	HV_COMPONENT_IMAGE 	*pImage,		//指向所要释放的图象.
	VP_IMAGE_INFO		*pImageInfo	//指向当前所取得的图象信息,如果为空则急略.
);

HRESULT InitVpVideo( 
	int 				iPort,
	int					iHeap,
	PVOID				pvArg,
	VideoOptionParam	*pParam
);

#ifdef __cplusplus
	}
#endif	//#ifdef __cplusplus

#endif	//#ifndef _VPDRIVER_H

