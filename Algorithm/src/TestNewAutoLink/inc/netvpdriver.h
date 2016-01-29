#ifndef _NETVPDRIVER_H
#define _NETVPDRIVER_H

#include "dspsys_def.h"
#include "videoBaseType.h"

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

PBYTE8 GetNetPacketBuffPtr( void );

HRESULT NET_InOneFrame(
	PBYTE8 				pbData,
	DWORD32				dwLen
);

HRESULT NET_GetOneFrame( 
	int 				iPort,		//当前操作的视频口.
	HV_COMPONENT_IMAGE 	*pImage,	//指向所取得的图象数据.
	VP_IMAGE_INFO		*pImageInfo	//指向当前所取得的图象信息,如果为空则急略.
);

HRESULT NET_FreeOneFrame(
	int 				iPort,		//当前操作的视频口.
	HV_COMPONENT_IMAGE 	*pImage,		//指向所要释放的图象.
	VP_IMAGE_INFO		*pImageInfo	//指向当前所取得的图象信息,如果为空则急略.
);

HRESULT NET_InOneFrame(
	PBYTE8 				pbData,
	DWORD32				dwLen
);

HRESULT NET_GetBufferCount(
	int					iPort,
	PINT 				piCount
);

HRESULT NET_SetBufferCount(
	int 				iPort,
	int		 			iCount
);

HRESULT NET_SetCallback(
	int 				iPort, 
	VIDEO_CALLBACK 		pfnCallback,
	PVOID 				pvUserData
);

HRESULT InitNetVideo(
	int 				iPort, 
	int					iHeap,
	PVOID				pvArg,
	VideoOptionParam 	*pParam
);

#ifdef __cplusplus
	}
#endif	/* #ifdef __cplusplus */

#endif	//#ifndef _NETINPUTPROCESS_H

