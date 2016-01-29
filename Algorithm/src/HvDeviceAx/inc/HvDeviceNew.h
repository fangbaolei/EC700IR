#ifndef _HVDEVICENEW_H__
#define _HVDEVICENEW_H__

#include "HvDeviceBaseType.h"
#include "HvDeviceCommDef.h"

#ifdef HVDEVICE_EXPORTS
#define HV_API_EX extern "C" __declspec(dllexport)
#elif HVDEVICE_LIB
#define HV_API_EX
#else
#define HV_API_EX extern "C" __declspec(dllimport)
#endif

#define HVAPI_API_VERSION_EX	"2.0"
typedef PVOID HVAPI_HANDLE_EX;         /**< HvAPI句柄类型 */


/**
* @brief			识别结果开始回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_INFOBEGIN)(PVOID pUserData, DWORD dwCarID);

/**
* @brief			识别结果结束回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_INFOEND)(PVOID pUserData, DWORD dwCarID);

/**
* @brief			车牌回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @param[out]		pcPlateNo		车牌字符串
* @param[out]		pcAppendInfo	车牌附加信息
* @param[out]		dw64TimeMS		识别时间
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_PLATE)(PVOID pUserData,
												 DWORD dwCarID,
												 LPCSTR pcPlateNo,
												 LPCSTR pcAppendInfo,
												 DWORD dwRecordType,
												 DWORD64 dw64TimeMS);

/**
* @brief			识别结果大图回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @param[out]		wImgType		大图类型
* @param[out]		wWidth			大图宽
* @param[out]		wHeight			大图高
* @param[out]		pbPicData		大图数据
* @param[out]		dwImgDataLen	大图数据长度
* @param[out]		dw64TimeMS		识别时间
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_BIGIMAGE)(PVOID pUserData,
													DWORD dwCarID, 
													WORD  wImgType,
													WORD  wWidth,
													WORD  wHeight,
													PBYTE pbPicData,
													DWORD dwImgDataLen,
													DWORD dwRecordType,
													DWORD64 dw64TimeMS);

/**
* @brief			识别结果车牌小图回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @param[out]		wWidth			小图宽
* @param[out]		wHeight			小图高
* @param[out]		pbPicData		小图数据
* @param[out]		dwImgDataLen	小图数据长度
* @param[out]		dw64TimeMS		识别时间
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_SMALLIMAGE)(PVOID pUserData,
													  DWORD dwCarID,
													  WORD wWidth,
													  WORD wHeight,
													  PBYTE pbPicData,
													  DWORD dwImgDataLen,
													  DWORD dwRecordType,
													  DWORD64 dw64TimeMS);

/**
* @brief			识别结果车牌二值图回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @param[out]		wWidth			二值图宽
* @param[out]		wHeight			二值图高
* @param[out]		pbPicData		二值图数据
* @param[out]		dwImgDataLen	二值图数据长度
* @param[out]		dw64TimeMS		识别时间
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_BINARYIMAGE)(PVOID pUserData,
													   DWORD dwCarID,
													   WORD wWidth,
													   WORD wHeight,
													   PBYTE pbPicData,
													   DWORD dwImgDataLen,
													   DWORD dwRecordType,
													   DWORD64 dw64TimeMS);

/**
* @brief			违法视频回调函数
* @param[out]		pUserData			由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID				车辆ID
* @param[out]		dwVideoType			视频帧类型
* @param[out]		dwVideoWidth		视频的宽
* @param[out]		dwVideoHeight		视频的高
* @param[out]		dw64TimeMS			视频时间
* @param[out]		pbVideoData			视频帧数据
* @param[out]		dwVideoDataLen		视频帧数据长度
* @param[out]		szVideoExtInfo		视频帧信息
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_ILLEGALVideo)(PVOID pUserData,
													   DWORD dwCarID,													   
													   DWORD dwVideoType,
													   DWORD dwVideoWidth,
													   DWORD dwVideoHeight,
													   DWORD64 dw64TimeMS,
													   PBYTE pbVideoData,
													   DWORD dwVideoDataLen,
													   LPCSTR szVideoExtInfo);

/**
* @brief			识别器信息回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pString			信息字符串
* @param[out]		dwStrLen		信息字符串长度
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_STRING)(PVOID pUserData, LPCSTR pString, DWORD dwStrLen);

/**
* @brief			识别器信息回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pString			信息字符串
* @param[out]		dwStrLen		信息字符串长度
* @param[out]		dwType			信息字符串类型 历史或实时
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_TFD_STRING)(PVOID pUserData, LPCSTR pString, DWORD dwStrLen, DWORD dwType);


/**
* @brief			图片回调函数
* @param[out]		pUserData			由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbImageData			图片数据
* @param[out]		dwImageDataLen		图片数据长度
* @param[out]		dwImageType			图片类型
* @param[out]		szImageExtInfo		图片信息
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_JPEG)(PVOID pUserData,
										 PBYTE pbImageData,
										 DWORD dwImageDataLen,
										 DWORD dwImageType,
										 LPCSTR szImageExtInfo);

/**
* @brief			视频回调函数
* @param[out]		pUserData			由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbVideoData			视频帧数据
* @param[out]		dwVideoDataLen		视频帧数据长度
* @param[out]		dwVideoType			视频帧类型
* @param[out]		szVideoExtInfo		视频帧信息
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_H264)(PVOID pUserData,
										 PBYTE pbVideoData,
										 DWORD dwVideoDataLen,
										 DWORD dwVideoType,
										 LPCSTR szVideoExtInfo);

/**
* @brief			历史视频回调函数
* @param[out]		pUserData			由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbVideoData			视频帧数据
* @param[out]		dwVideoDataLen		视频帧数据长度
* @param[out]		dwVideoType			视频帧类型
* @param[out]		szVideoExtInfo		视频帧信息
* @return			成功S_OK 失败 E_FAILE
*/
typedef INT (CDECL* HVAPI_CALLBACK_HISTORY_VIDEO)(PVOID pUserData,
												  PBYTE pbVideoData,
												  DWORD dwVideoDataLen,
												  DWORD dwVideoType,
												  LPCSTR szVideoExtInfo);


//回调函数的定义

/**
* @brief               视频回调函数
* @param[out]          pUserData          由HVAPI_StartRecvH264Video设置的传入回调函数的用户数据指针
* @param[out]          dwVedioFlag        视频标记 ：有效数据标记、无效数据标记、历史结束标记
* @param[out]          dwVideoType        视频类型 ：历史视频、实时视频
* @param[out]          dwWidth            宽度
* @param[out]          dwHeight			  高度
* @param[out]          dw64TimeMS         视频时间
* @param[out]          pbVideoData        视频帧数据
* @param[out]          dwVideoDataLen     视频帧长度
* @param[out]          szVideoExtInfo     视频附加信息
* @return              成功S_OK 失败 E_FAILE
*/
typedef INT (CDECL* HVAPI_CALLBACK_H264_EX)(
												PVOID pUserData,  
												DWORD dwVedioFlag,
												DWORD dwVideoType, 
												DWORD dwWidth,
												DWORD dwHeight,
												DWORD64 dw64TimeMS,
												PBYTE pbVideoData, 
												DWORD dwVideoDataLen,
												LPCSTR szVideoExtInfo
												);

/**
* @brief               jpeg流回调函数
* @param[out]          pUserData          由HVAPI_StartRecvH264Video设置的传入回调函数的用户数据指针
* @param[out]          dwImageFlag        图片标记 ：有效图片数据、无效图片数据
* @param[out]          dwImageType        图片类型 ：调试码流、正常码流
* @param[out]          dwWidth            宽度
* @param[out]          dwHeight			  高度
* @param[out]          dw64TimeMS         JPEG时间
* @param[out]          pbImageData        图片帧数据
* @param[out]          dwImageDataLen     图片帧长度
* @param[out]          szImageExtInfo     JPEG附加信息
* @return              成功S_OK 失败 E_FAILE
*/

typedef INT (CDECL* HVAPI_CALLBACK_JPEG_EX)(
											PVOID pUserData,  
											DWORD dwImageFlag,
											DWORD dwImageType, 
											DWORD dwWidth,
											DWORD dwHeight,
											DWORD64 dw64TimeMS,
											PBYTE pbImageData, 
											DWORD dwImageDataLen,
											LPCSTR szImageExtInfo
											);

/**
* @brief               识别结果回调函数
* @param[out]          pUserData          由HVAPI_StartRecvResult设置的传入回调函数的用户数据指针
* @param[out]          dwResultFlag		  结果标志：有效结果标记、无效结果标记、接收历史结束标记
* @param[out]          dwResultType		  结果类型：实时结果、历史结果
* @param[out]          dwCarID            结果ID
* @param[out]          pcPlateNo          车牌字符串
* @param[out]          dw64TimeMS		  结果时标
* @param[out]          pPlate			  车牌小图
* @param[out]          pPlateBin          车牌二值图
* @param[out]          pBestSnapshot      最清晰大图
* @param[out]          pLastSnapshot      最后大图
* @param[out]          pBeginCapture      第一张抓拍图
* @param[out]          pBestCapture       第二张抓拍图
* @param[out]          pLastCapture       第三张抓拍图
* @return              成功S_OK 失败 E_FAILE
*/
//注意 plate 大图 小图 二维图 回传有dwCardID信息，_string 和 _tfd_string无dwCardID信息
typedef INT (CDECL* HVAPI_CALLBACK_RESULT)(
	PVOID pUserData, 
	DWORD dwResultFlag,			
	DWORD dwResultType,		
	DWORD dwCarID,
	LPCSTR pcPlateNo,
	LPCSTR pcAppendInfo,
	DWORD64 dw64TimeMS,
	CImageInfo   Plate,
	CImageInfo   PlateBin,
	CImageInfo   BestSnapshot,
	CImageInfo   LastSnapshot,
	CImageInfo   BeginCapture,
	CImageInfo   BestCapture,
	CImageInfo   LastCapture
	);

/**
* @brief               二次开发结果回调函数
* @param[out]          pUserData          由HVAPI_StartRecvReDevelopResult设置的传入回调函数的用户数据指针
* @param[out]          pbInfo        数据
* @param[out]          dwInfoLen     长度
* @param[out]          pbData        图片数据
* @param[out]          dwDataLen     图片长度
* @param[out]          nWidth     图片宽度
* @param[out]          nHeight    图片高度度
* @return              成功S_OK 失败 E_FAILE
*/
typedef INT (CDECL* HVAPI_CALLBACK_REDEVELOP_RESULT)(
	PVOID pUserData, 
	PBYTE pbInfo,
	DWORD dwInfoLen,
	PBYTE pbData,
	DWORD dwDataLen,
	INT nWidth,
	INT nHeight
	);

/**
* @brief			监听信号触发回调函数
* @param[in]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[in]		dwTime			触发时间
* @param[in]		dwRoadID		道路通道ID
* @param[in]		dwTrigerID		触发ID
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_LISTEN_CMD)(PVOID pUserData,DWORD64 dwTime,DWORD dwRoadID,DWORD dwTrigerID);

/**
* @brief		搜索设备
* @param[out]	pdwDeviceCount			设备数
* @return		成功S_OK 失败 E_FAILE
*/
HV_API_EX HRESULT HVAPI_SearchDeviceCount(DWORD32* pdwDeviceCount);

/**
* @brief		获取设备IP
* @param[in]	dwIndex			搜索时保存的数组位置
* @param[out]	dw64MacAddr		设备MAC地址
* @param[out]	dwIP			设备IP
* @param[out]	dwMask			掩码
* @param[out]	dwGateWay		网关
* @return		成功S_OK 失败 E_FAILE
*/
HV_API_EX HRESULT HVAPI_GetDeviceAddr(DWORD32 dwIndex, DWORD64* dw64MacAddr, DWORD32* dwIP,
								   DWORD32* dwMask, DWORD32* dwGateWay);

/**
* @brief		    地球新接口 注释待整理
* @param[in,out]	rgDevInfo	搜索到的设备信息结构体数组
* @param[in,out]	pnDevCount	搜索到的设备数量
* @return			成功：S_OK；失败：E_FAIL, 传入结构体数量太少：S_FALSE 
*/
HV_API_EX HRESULT CDECL HVAPI_SearchDeviceEx( CDevBasicInfo* rgDevInfo , INT* pnDevCount );

/**
* @brief		通过MAC地址设置设备IP
* @param[in]	dw64MacAddr		设备MAC地址
* @param[in]	dwIP			设备IP
* @param[in]	dwMask			掩码
* @param[in]	dwGateWay		网关
* @return		成功S_OK 失败 E_FAILE
*/
HV_API_EX HRESULT CDECL HVAPI_SetIPByMacAddr(DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway);

/**
* @brief		从搜索到的信息里获取设备附加信息
* @param[in]	dwIndex			搜索时保存的数组位置
* @param[out]	lpExtInfo		设备类型信息缓存
* @param[in]	iBufLen			设备类型信息缓存长度
* @return		成功S_OK 失败 E_FAILE
*/
HV_API_EX HRESULT HVAPI_GetDeviceInfoEx(int iIndex, LPSTR lpExtInfo, int iBufLen);

/**
* @brief		获取设备类型
* @param[in]	pcIP		设备IP
* @param[out]	iDeviceType	设备类型
* @return		成功S_OK 失败 E_FAILE
*/
HV_API_EX HRESULT CDECL HVAPI_GetDevTypeEx(PSTR pcIP, int* iDeviceType);

/**
* @brief			打开设备句柄
* @param[in]		szIp			设备的IP地址
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：设备句柄；失败：NULL
*/
HV_API_EX HVAPI_HANDLE_EX CDECL HVAPI_OpenEx(LPCSTR szIp, LPCSTR szApiVer);

/**
* @brief			关闭设备句柄
* @param[in]		hHandle			对应设备的有效句柄
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_CloseEx(HVAPI_HANDLE_EX hHandle);

/**
* @brief			获取连接设备使用的XML协议版本
* @param[in]		hHandle			对应设备的有效句柄
* @param[out]		pemProtocolVersion	该连接使用的XML协议版本
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetXmlVersionEx(HVAPI_HANDLE_EX hHandle, PROTOCOL_VERSION* pemProtocolVersion);

/**
* @brief			获取连接设备使用的协议版本
* @param[in]		hHandle			对应设备的有效句柄
* @param[out]		pemProtocolVersion	该连接使用的协议版本
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetProtocolVersionEx(HVAPI_HANDLE_EX hHandle, PROTOCOL_VERSION* pemProtocolVersion);

/**
* @brief			获取连接状态
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		nStreamType		数据流类型（即：连接类型）
* @param[out]		pdwConnStatus	该连接的当前状态
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetConnStatusEx(HVAPI_HANDLE_EX hHandle, INT nStreamType, DWORD* pdwConnStatus);

/**
* @brief			获取重连次数
* @param[in]		hHandle				对应设备的有效句柄
* @param[in]		nStreamType			数据流类型（即：连接类型）
* @param[out]		pdwReConnectTimes	该连接的当前重连次数
* @param[in]		fIsReset			获取完后是否将该连接的重连次数清零
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetReConnectTimesEx(HVAPI_HANDLE_EX hHandle, INT nStreamType, DWORD* pdwReConnectTimes, BOOL fIsReset);


/**
* @brief			执行命令
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szCmd			用于向设备发出某条命令的字符串
* @param[in]		szRetBuf		返回的执行结果(XML形式)
* @param[in]		nBufLen			szRetBuf缓冲区的长度
* @param[out]		pnRetLen		实际返回的执行结果长度。注：为NULL则忽略该参数。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_ExecCmdEx(HVAPI_HANDLE_EX hHandle,
								   LPCSTR szCmd,
								   LPSTR szRetBuf,
								   INT nBufLen,
								   INT* pnRetLen);

/**
* @brief			获取设备参数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szParamDoc		获取到的参数（XML形式）
* @param[in]		nBufLen			szParam缓冲区的长度
* @param[out]		pnRetLen		实际返回的参数长度。注：为NULL则忽略该参数。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetParamEx(HVAPI_HANDLE_EX hHandle,
									LPSTR szParamDoc,
									INT nBufLen,
									INT* pnRetLen);

/**
* @brief			设置设备参数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szParamDoc		要保存到设备的参数（XML形式）
* @return			成功：S_OK；失败：E_FAIL
* @warning			更改过的参数要在设备下次启动后才会生效
*/
HV_API_EX HRESULT CDECL HVAPI_SetParamEx(HVAPI_HANDLE_EX hHandle, LPCSTR szParamDoc);

/**
* @brief			上传数控版控制板升级文件到一体机
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szParamDoc		要保存到设备的参数（XML形式）
* @return			成功：S_OK；失败：E_FAIL
* @warning			更改过的参数要在设备下次启动后才会生效
*/
HV_API_EX HRESULT CDECL HVAPI_SendControllPannelUpdateFileEx(HVAPI_HANDLE_EX hHandle,
														PBYTE pUpdateFileBuffer,
														DWORD dwFileSize);

/**
* @brief			设置数据流接收回调函数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		pFunc			回调函数指针。注：为NULL时表示关闭nStreamType类型的数据流。
* @param[in]		pUserData		传入回调函数的用户数据指针
* @param[in]		iVideoID		视频通道，目前只使用0
* @param[in]		nStreamType		回调数据流类型
* @param[in]		szConnCmd		数据流连接命令
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetCallBackEx(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData,
									   INT iVideoID, INT iCallBackType, LPCSTR szConnCmd);


/**
* @brief              启动H264接收
* @param[in]          hHandle				设备句柄
* @param[in]          pFunc				    回调函数指针
* @param[in]          pUserData			    用户数据
* @param[in]          iVideoID				视频通道，目前只使用0
* @param[in]          dw64BeginTimeMS       开始时间
* @param[in]          dw64EndTimeMS         结束时间 
* @param[in]          dwRecvFlag            接收标志 :实时视频、历史视频
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartRecvH264Video(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID ,       
	DWORD64 dw64BeginTimeMS,
	DWORD64 dw64EndTimeMS,
	DWORD dwRecvFlag
);
/**
* @brief              停止H264接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StopRecvH264Video(HVAPI_HANDLE_EX hHandle);


/**
* @brief              启动MJPEG接收
* @param[in]          hHandle				 设备句柄
* @param[in]          pFunc					 回调函数指针
* @param[in]          pUserData				 用户数据
* @param[in]          iVideoID				 视频通道，目前只使用0
* @param[in]          dwRecvFlag			 接收标志：调试码流、正常码流
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartRecvMJPEG(
					 HVAPI_HANDLE_EX hHandle,
					 PVOID pFunc,
					 PVOID pUserData,
					 INT iVideoID ,
					 DWORD dwRecvFlag
					 );
/**
* @brief              停止MJPEG接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL  HVAPI_StopRecvMJPEG(HVAPI_HANDLE_EX hHandle);
/**
* @brief              更改调试码流打开状态
* @param[in]          hHandle          设备句柄
* @param[in]          fEnable          是否打开调试码流
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetDebugJpegStatus(HVAPI_HANDLE_EX hHandle, BOOL fEnable);

/**
* @brief              启动接收结果
* @param[in]          hHandle			设备句柄
* @param[in]          pFunc				回调函数指针
* @param[in]          pUserData			用户数据
* @param[in]          iVideoID          视频通道，目前只使用0
* @param[in]          dw64BeginTimeMS       开始时间
* @param[in]          dw64EndTimeMS         结束时间
* @param[in]          dwStartIndex			起始帧号
* @param[in]          dwRecvFlag			接收标志
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartRecvResult(
						HVAPI_HANDLE_EX hHandle,
						PVOID pFunc,
						PVOID pUserData,
						INT iVideoID,
						DWORD64 dw64BeginTimeMS,
						DWORD64 dw64EndTimeMS,
						DWORD dwStartIndex,
						DWORD dwRecvFlag
					 );
/**
* @brief              停止结果接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StopRecvResult(HVAPI_HANDLE_EX hHandle);


/**
* @brief              启动状态信息接收
* @param[in]          hHandle				 设备句柄
* @param[in]          pFunc					 回调函数指针
* @param[in]          pUserData				 用户数据
* @param[in]          iVideoID				 视频通道，目前只使用
* @param[in]          dwRecvFlag			 接收标志：保留
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartRecvMsg(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID ,
	DWORD dwRecvFlag
	);

/**
* @brief		启动二次开发结果接收
* @param[in]	hHandle	        对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL  HVAPI_StartRecvRedevelopResult
(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData
 );

/**
* @brief		停止二次开发结果接收
* @param[in]	hHandle	        对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL  HVAPI_StopRecvRedevelopResult
(
	HVAPI_HANDLE_EX hHandle
);

/**
* @brief		触发信号（用于开闸）
* @param[in]	hHandle	        对应设备的有效句柄
*@param[in]   iVideo		   视频编号
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_TriggerSignal(HVAPI_HANDLE_EX hHandle, INT iVideoID);

/**
* @brief		触发报警信号， 用于控制报警设备
* @param[in]	hHandle	        对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL
命令名为： SendAlarmTriggerOut
*/
HV_API_EX HRESULT CDECL HVAPI_TriggerAlarmSignal(HVAPI_HANDLE_EX hHandle);


/**
* @brief		导入黑白名单
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szWhileNameList	 白名单列表,各车牌(车牌-有效期)用$连接起来，如:桂A11111-2013010101$桂A2222-2013010202$桂A33333-2013020202   如果指针为NULL,表示不设置。
* @param[in]	iWhileListLen		白名单字符串，
* @param[in]	szBlackNameList		名单字符串， 各车牌(车牌-有效期)用$连接起来，如:桂A11111-2013010101$桂A2222-2013010202$桂A33333-2013020202   如果指针表示不设置
* @param[in]	iListLen		名单字符串长度 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_InportNameList(HVAPI_HANDLE_EX hHandle, CHAR* szWhileNaneList, INT iWhileListLen, CHAR* szBlackNameList, INT iBlackListLen);


/**
* @brief		获取黑白名单
* @param[in]	hHandle	        对应设备的有效句柄 
* @param[out]	szWhileNameList		白名单字符串缓存区， 各车牌(车牌-有效期)用$连接起来，如:桂A11111-2013010101$桂A2222-2013010202$桂A33333-2013020202   
* @param[in/out]	iWhileListLen		缓存区长度指针， 用于输入和返回 
@param[out]	szBlackNameList		黑名单字符串缓存区， 各车牌(车牌-有效期)用$连接起来，如:桂A11111-2013010101$桂A2222-2013010202$桂A33333-2013020202   
* @param[in/out]	iBlackListLen		缓存区长度指针， 用于输入和返回 

* @return		成功：S_OK；失败：E_FAIL
*/

HV_API_EX HRESULT CDECL HVAPI_GetNameList(HVAPI_HANDLE_EX hHandle, CHAR* szWhileNameList, INT *iWhileListLen, CHAR* szBlackNameList, INT *iBlackListLen);


/**
* @brief              停止状态信息接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL  HVAPI_StopRecvMsg(HVAPI_HANDLE_EX hHandle);

/**
* @brief			设置字符叠加
* @param[in]		hHandle					对应设备的有效句柄
* @param[in]		szCharacterDataBuff		点阵数据
* @param[in]		nBuffLen				点阵数据长度
* @param[in]		nTopLeftX				叠加字符左上角坐标X值
* @param[in]		nTopLeftY				叠加字符左上角坐标Y值
* @param[in]		nDateType				日期类型
* @param[in]		nSize					字体大小（像素）
* @param[in]		nR						RGB的R值
* @param[in]		nG						RGB的G值
* @param[in]		nB						RGB的B值
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPIUTILS_SetCharacterValue(HVAPI_HANDLE_EX hHandle, const char* szCharacterDataBuff, 
												  INT nTopLeftX, INT nTopLeftY, INT nDateType, INT nSize, INT nR, INT nG, INT nB);

/**
* @brief			结果大图红灯加红设置
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		dwEnhanceStyle	加红处理模式 0不加红 1只加红违章结果 2全加红 3只加红抓拍图
* @param[in]		iBrightness		加红后图片亮度增强处理
* @param[in]		iHubThreshold	红灯色度阀值
* @param[in]		iCompressRate	加红处理后图片压缩品质
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle, DWORD dwEnhanceStyle, INT iBrightness, INT iHubThreshold, INT iCompressRate);

/**
* @brief			历史结果JPEG图片帧红灯加红设置
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		dwEnhanceStyle	加红处理模式 0不加红 1加红
* @param[in]		iBrightness		加红后图片亮度增强处理
* @param[in]		iHubThreshold	红灯色度阀值
* @param[in]		iCompressRate	加红处理后图片压缩品质
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetHistoryVideoEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle, DWORD dwEnhanceStyle, INT iBrightness, INT iHubThreshold, INT iCompressRate);

/**
* @brief			预留接口，用于设备扩展新功能后获取相关的扩展信息
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		dwType			信息类型
* @param[in]		iBrightness		保存信息的缓存地址
* @param[in/out]	iBrightness		保存信息的缓存地址长度和实际返回信息长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetExtensionInfoEx(HVAPI_HANDLE_EX hHandle, DWORD dwType, LPVOID pRetData, INT* iBufLen);

/**
* @brief			从结果附加信息中获取某项信息
* @param[in]		szAppened			结果附加信息
* @param[in]		szInfoName			获取项名
* @param[out]		szRetInfo			结果字串
* @param[in]		iRetInfoBufLen		结果字串保存缓存区长
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPIUTILS_GetRecordInfoFromAppenedStringEx(LPCSTR szAppened, LPCSTR szInfoName, LPSTR szRetInfo, INT iRetInfoBufLen);

/**
* @brief			从结果附加信息中提取全部信息每项目信息使用回车符区分
* @param[in]		pszXmlPlateInfo			结果附加信息
* @param[out]		pszPlateInfoBuf			结果字串缓存区
* @param[in]		iPlateInfoBufLen		结果字串缓存区长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPIUTILS_ParsePlateXmlStringEx(LPCSTR pszXmlPlateInfo, LPSTR pszPlateInfoBuf, INT iPlateInfoBufLen);

/**
* @brief			结果车牌小图数据转换成BMP格式
* @param[in]		pbSmallImageData		车牌小图数据
* @param[in]		nSmallImageWidth		车牌小图宽
* @param[in]		nSmallImageHeight		车牌小图高
* @param[out]		pbBitmapData			转换结果BMP数据
* @param[out]		pnBitmapDataLen			转换结果BMP数据长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPIUTILS_SmallImageToBitmapEx(PBYTE pbSmallImageData, INT nSmallImageWidth, INT nSmallImageHeight, PBYTE pbBitmapData, INT* pnBitmapDataLen);

/**
* @brief			将识别结果二值图转换为BMP位图
* @param[in]		pbBinImageData		识别结果二值图
* @param[in]		pbBitmapData		BMP位图数据存放缓冲区
* @param[in,out]	pnBitmapDataLen		BMP位图数据存放缓冲区长度。函数返回后则为：BMP位图数据实际长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPIUTILS_BinImageToBitmapEx(PBYTE pbBinImageData,
												 PBYTE pbBitmapData,
												 INT* pnBitmapDataLen);

/**
* @brief		从命令执行结果XML字符串中获取相应信息
* @param[in]	fIsNewXmlProtocol	XML协议类型
* @param[in]	pszRetXmlStr		执行命令时返回的执行结果XML字符串
* @param[in]	pszCmdName			命令名称
* @param[in]	pszInfoName			获取信息名称
* @param[out]	pszInfoValue		获取信息结果字串
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPIUTILS_GetExeCmdRetInfoEx(BOOL fIsNewXmlProtocol, LPCSTR pszRetXmlStr, LPCSTR pszCmdName, 
													  LPCSTR pszInfoName, LPSTR pszInfoValue);


HV_API_EX HRESULT CDECL HVAPI_Capture(HVAPI_HANDLE_EX hHandle, char* szValue);

/**
* @brief              向185相机发送触发命令
* @param[in]          hHandle          设备句柄
* @param[in]          dw64CaptureTime    DWORD64型，当前系统时间,为从1970-01-01 00:00:00开始至目前所经过的毫秒数。为触发抓拍时间，如果为空 (NULL)，则自动获取当前时间
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SendCaptureCmd(HVAPI_HANDLE_EX hHandle, DWORD64 dw64CaptureTime);

/**
* @brief			开启触发信号监听服务
@param[in]			nNum	最大监听数
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_OpenListenCMDSever(int nNum);

/**
* @brief			关闭触发信号监听服务
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_CloseListenCMDSever();

/**
* @brief			设置监听回调函数
* @param[in]		hHandle			对应设备监听的有效句柄
* @param[in]		pFunc			回调函数指针
* @param[in]		pUserData		传入回调函数的用户数据指针
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetCallBackListen(HVAPI_HANDLE_EX hHandle,PVOID pFunc, PVOID pUserData);

/**
* @brief			链接监听设备句柄
* @param[in]		szIp			设备的IP地址
* @return			成功：设备句柄；失败：NULL
*/
HV_API_EX HVAPI_HANDLE_EX CDECL HVAPI_OpenListenClientEx(LPCSTR szIp);

/**
* @brief			关闭监听设备句柄
* @param[in]		hHandle			对应设备监听的有效句柄
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_CloseListenClientEx(HVAPI_HANDLE_EX hHandle);


/**
* @brief              取行人卡口人流量(最大只能取一个月数据)
* @param[in]          hHandle          设备句柄
* @param[in]          dw64StartTime    对应获取客流量信息的起始时间,为从1970-01-01 00:00:00开始至目前所经过的毫秒数
* @param[in]          dw64EndTime      对应获取客流量信息的结束时间,为从1970-01-01 00:00:00开始至目前所经过的毫秒数
* @param[out]         szRetInfo		   对应使用者申请的内存区域，函数调用成功后客流量信息更新于此
* @param[in,out]      iLen		       szRetInfo缓冲区长度，函数返回后为字符串实际长度
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetPCSFlow(HVAPI_HANDLE_EX hHandle, DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen);

#endif
