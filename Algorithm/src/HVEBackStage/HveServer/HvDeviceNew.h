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

/* 回调数据类型 */
#define CALLBACK_TYPE_RECORD_PLATE			0xFFFF0001
#define CALLBACK_TYPE_RECORD_BIGIMAGE		0xFFFF0002
#define CALLBACK_TYPE_RECORD_SMALLIMAGE		0xFFFF0003
#define CALLBACK_TYPE_RECORD_BINARYIMAGE	0xFFFF0004
#define CALLBACK_TYPE_RECORD_INFOBEGIN		0xFFFF0005
#define CALLBACK_TYPE_RECORD_INFOEND		0xFFFF0006
#define CALLBACK_TYPE_STRING				0xFFFF0007
#define CALLBACK_TYPE_JPEG_FRAME			0xFFFF0008
#define CALLBACK_TYPE_H264_VIDEO			0xFFFF0009
#define CALLBACK_TYPE_HISTORY_VIDEO			0xFFFF0010

/* 识别结果大图类型定义 */
#define RECORD_BIGIMG_BEST_SNAPSHOT			0x0001	/**< 最清晰识别图 */
#define RECORD_BIGIMG_LAST_SNAPSHOT			0x0002	/**< 最后识别图 */
#define RECORD_BIGIMG_BEGIN_CAPTURE			0x0003	/**< 开始抓拍图 */
#define RECORD_BIGIMG_BEST_CAPTURE			0x0004	/**< 最清晰抓拍图 */
#define RECORD_BIGIMG_LAST_CAPTURE			0x0005	/**<  最后抓拍图 */

/* 连接类型定义 */
#define CONN_TYPE_UNKNOWN         0xffff0000	/**< 未知 */
#define CONN_TYPE_IMAGE           0xffff0001	/**< 图片 */
#define CONN_TYPE_VIDEO           0xffff0002	/**< 视频 */
#define CONN_TYPE_RECORD          0xffff0003	/**< 识别结果 */
//
///* 图片类型 */
//#ifndef IMAGE_TYPE_UNKNOWN
//#define IMAGE_TYPE_UNKNOWN          0xffff0100	/**< 未知 */
//#endif	
//#ifndef IMAGE_TYPE_JPEG_NORMAL
//#define IMAGE_TYPE_JPEG_NORMAL      0xffff0101	/**< Jpeg普通图 */
//#endif
//#ifndef IMAGE_TYPE_JPEG_CAPTURE
//#define IMAGE_TYPE_JPEG_CAPTURE     0xffff0102	/**< Jpeg抓拍图 */
//#endif
//#ifndef IMAGE_TYPE_JPEG_LPR 
//#define IMAGE_TYPE_JPEG_LPR         0xffff0103	/**< Jpeg调试码流 */
//#endif
//

/**
* @brief			识别结果开始回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		dwCarID			车辆ID
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_RECORD_INFOBEGIN)(PVOID pUserData, DWORD dwCarID);

/**
* @brief			识别结果开始回调函数
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
* @brief			识别器信息回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pString			信息字符串
* @param[out]		dwStrLen		信息字符串长度
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_STRING)(PVOID pUserData, LPCSTR pString, DWORD dwStrLen);

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
* @return			0
*/
typedef INT (CDECL* HVAPI_CALLBACK_HISTORY_VIDEO)(PVOID pUserData,
												  PBYTE pbVideoData,
												  DWORD dwVideoDataLen,
												  DWORD dwVideoType,
												  LPCSTR szVideoExtInfo);

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
* @param[out]		pfIsNewProtol	该连接使用的XML协议版本，false为1.0版本，true为2.0版本
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetXmlVersionEx(HVAPI_HANDLE_EX hHandle, bool* pfIsNewProtol);

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
HV_API_EX HRESULT CDECL HVAPI_GetExtensionInfoEx(HVAPI_HANDLE_EX hHnadle, DWORD dwType, LPVOID pRetData, INT* iBufLen);

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

#endif
