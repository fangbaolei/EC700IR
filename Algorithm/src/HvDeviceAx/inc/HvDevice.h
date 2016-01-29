/**
* @file		HvDevice.h
* @version	1.0
* @brief	HvDevice.dll头文件
* @date		2011-10-21
*/

#ifndef _HVAPI_H_
#define _HVAPI_H_

#include "HvDeviceBaseType.h"
#include "HvDeviceCommDef.h"

#ifdef HVDEVICE_EXPORTS
    #define HV_API extern "C" __declspec(dllexport)
#elif HVDEVICE_LIB
    #define HV_API
#else
    #define HV_API extern "C" __declspec(dllimport)
#endif

#define HVAPI_API_VERSION "1.0"     /**< API版本号 */

typedef PVOID HVAPI_HANDLE;         /**< HvAPI句柄类型 */

/* 回调数据流类型 */
#define STREAM_TYPE_UNKNOWN         0xffff0000	/**< 未知 */
#define STREAM_TYPE_IMAGE           0xffff0001	/**< 图片 */
#define STREAM_TYPE_VIDEO           0xffff0002	/**< 视频 */
#define STREAM_TYPE_RECORD          0xffff0003	/**< 识别结果 */
#define STREAM_TYPE_GATHER_INFO     0xffff0004	/**< 交通统计信息 */

#define PACKET_RECORD_VIDEO_ILLEGAL 0x80000001	/**违法视频数据流*/

/**
* @brief			搜索设备
* @param[in]		dwDevType		要搜索的设备类型
* @param[out]		szDevInfoList	结果缓冲区，返回以分号分隔的设备地址信息列表
* @param[in]		nBufLen			szDevInfoList缓冲区长度
* @return			搜索到的设备总数
*/
HV_API DWORD CDECL HVAPI_SearchDevice(DWORD dwDevType, LPSTR szDevInfoList, INT nBufLen);

/**
* @brief			通过设备MAC地址修改设备的IP地址
* @param[in]		dw64MacAddr			要修改的设备MAC地址
* @param[in]		dwIP				修改后的IP地址
* @param[in]		dwMask				修改后的子网掩码
* @param[in]		dwGateway			修改后的网关地址
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_SetIPFromMac(DWORD64 dw64MacAddr,
                                        DWORD32 dwIP,
                                        DWORD32 dwMask,
                                        DWORD32 dwGateway);

/**
* @brief		获取设备类型
* @param[in]	pcIP		设备IP
* @param[out]	iDeviceType	设备类型
* @return		成功S_OK 失败 E_FAILE
*/
HV_API HRESULT CDECL HVAPI_GetDevType(PSTR pcIP, int* iDeviceType);


/**
* @brief			打开设备句柄
* @param[in]		szIp			设备的IP地址
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：设备句柄；失败：NULL
*/
HV_API HVAPI_HANDLE CDECL HVAPI_Open(LPCSTR szIp, LPCSTR szApiVer);

/**
* @brief			关闭设备句柄
* @param[in]		hHandle			对应设备的有效句柄
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_Close(HVAPI_HANDLE hHandle);

/**
* @brief			获取设备参数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szParamDoc		获取到的参数（XML形式）
* @param[in]		nBufLen			szParam缓冲区的长度
* @param[out]		pnRetLen		实际返回的参数长度。注：为NULL则忽略该参数。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_GetParam(HVAPI_HANDLE hHandle,
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
HV_API HRESULT CDECL HVAPI_SetParam(HVAPI_HANDLE hHandle, LPCSTR szParamDoc);

/**
* @brief			执行命令
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szCmd			用于向设备发出某条命令的字符串
* @param[in]		szRetBuf		返回的执行结果(XML形式)
* @param[in]		nBufLen			szRetBuf缓冲区的长度
* @param[out]		pnRetLen		实际返回的执行结果长度。注：为NULL则忽略该参数。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_ExecCmd(HVAPI_HANDLE hHandle,
                                   LPCSTR szCmd,
                                   LPSTR szRetBuf,
                                   INT nBufLen,
                                   INT* pnRetLen);

/**
* @brief			获取设备信息
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szCmd			用于向设备查询某些信息的字符串
* @param[in]		szRetBuf		返回的执行结果(XML形式)
* @param[in]		nBufLen			szRetBuf缓冲区的长度
* @param[out]		pnRetLen		实际返回的执行结果长度。注：为NULL则忽略该参数。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_GetInfo(HVAPI_HANDLE hHandle,
                                   LPCSTR szCmd,
                                   LPSTR szRetBuf,
                                   INT nBufLen,
                                   INT* pnRetLen);

/**
* @brief			设置数据流接收回调函数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		pFunc			回调函数指针。注：为NULL时表示关闭nStreamType类型的数据流。
* @param[in]		pUserData		传入回调函数的用户数据指针
* @param[in]		nStreamType		回调数据流类型
* @param[in]		szConnCmd		数据流连接命令
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_SetCallBack(HVAPI_HANDLE hHandle,
                                       PVOID pFunc,
                                       PVOID pUserData,
                                       INT nStreamType,
                                       LPCSTR szConnCmd);

/**
* @brief			更新回调数据流连接命令
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		nStreamType		回调数据流类型
* @param[in]		szConnCmd		数据流连接命令
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_UpdateCallBackCmd(HVAPI_HANDLE hHandle,
                                             INT nStreamType,
                                             LPCSTR szConnCmd);

/**
* @brief			获取连接状态
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		nStreamType		数据流类型（即：连接类型）
* @param[out]		pdwConnStatus	该连接的当前状态
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_GetConnStatus(HVAPI_HANDLE hHandle,
                                         INT nStreamType,
                                         DWORD* pdwConnStatus);

/**
* @brief			图片回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbImageData		图片数据
* @param[out]		dwImageDataLen	图片数据长度
* @param[out]		dwImageType		图片类型
* @param[out]		szImageExtInfo	图片扩展信息（字符串形式）
* @return			0
*/
typedef INT (CDECL *HVAPI_CALLBACK_IMAGE)(PVOID pUserData,
                                          PBYTE pbImageData,
                                          DWORD dwImageDataLen,
                                          PBYTE pbImageInfoData,
                                          DWORD dwImageInfoLen,
                                          DWORD dwImageType,
                                          LPCSTR szImageExtInfo);

/**
* @brief			视频回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbVideoData		视频数据
* @param[out]		dwVideoDataLen	视频数据长度
* @param[out]		dwVideoType		视频类型
* @param[out]		szVideoExtInfo	视频扩展信息（字符串形式）
* @return			0
*/
typedef INT (CDECL *HVAPI_CALLBACK_VIDEO)(PVOID pUserData,
                                          PBYTE pbVideoData,
                                          DWORD dwVideoDataLen,
                                          DWORD dwVideoType,
                                          LPCSTR szVideoExtInfo);

/**
* @brief			识别结果回调函数
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbResultPacket	识别结果数据包
* @param[out]		dwPacketLen		识别结果数据包长度
* @param[out]		dwRecordType	识别结果类型
* @param[out]		szResultInfo	识别结果(XML形式)
* @return			0
*/
typedef INT (CDECL *HVAPI_CALLBACK_RECORD)(PVOID pUserData,
                                           PBYTE pbResultPacket,
                                           DWORD dwPacketLen,
                                           DWORD dwRecordType,
                                           LPCSTR szResultInfo);

/**
* @brief			车流量统计信息回调
* @param[out]		pUserData		由HVAPI_SetCallBack设置的传入回调函数的用户数据指针
* @param[out]		pbInfoData		车流量信息数据
* @param[out]		dwInfoDataLen	信息长度
* @return			0
*/
typedef INT (CDECL *HVAPI_CALLBACK_GATHER_INFO)(PVOID pUserData,
                                                PBYTE pbInfoData,
                                                DWORD dwInfoDataLen);

/* 结果图片类型 */
typedef enum
{
    RECORD_IMAGE_BEST_SNAPSHOT = 0,	/**< 最清晰大图。	注：数据格式为Jpeg */
    RECORD_IMAGE_LAST_SNAPSHOT,		/**< 最后大图。		注：数据格式为Jpeg */
    RECORD_IMAGE_BEGIN_CAPTURE,		/**< 第一张抓拍图。	注：数据格式为Jpeg */
    RECORD_IMAGE_BEST_CAPTURE,		/**< 第二张抓拍图。	注：数据格式为Jpeg */
    RECORD_IMAGE_LAST_CAPTURE,		/**< 第三张抓拍图。	注：数据格式为Jpeg */
    RECORD_IMAGE_SMALL_IMAGE,		/**< 车牌小图。		注：数据格式为YUV422 */
    RECORD_IMAGE_BIN_IMAGE			/**< 车牌二值图。	注：数据格式为二进制 */
}
RECORD_IMAGE_TYPE;

/* 结果图片信息 */
typedef struct _RECORD_IMAGE_INFO
{
    DWORD32 dwCarID;				/**< 该图片对应的车辆ID */
    DWORD32 dwWidth;				/**< 图片宽度 */
    DWORD32 dwHeight;				/**< 图片高度 */
    DWORD64 dw64TimeMs;				/**< 图片时间 */
}
RECORD_IMAGE_INFO;

/* 结果图片 */
typedef struct _RECORD_IMAGE
{
    RECORD_IMAGE_INFO cImgInfo;		/**< 图片信息 */
    PBYTE pbImgData;				/**< 图片数据 */
    PBYTE pbImgInfoEx;				/**< 图片原始信息 */
    DWORD dwImgDataLen;				/**< 图片长度 */
    DWORD dwImgInfoLen;				/**< 图片原始信息长度 */
}
RECORD_IMAGE;

/* 结果图片集 */
typedef struct _RECORD_IMAGE_GROUP
{
    RECORD_IMAGE_INFO cImgInfoBestSnapshot;			//最清晰大图信息
    RECORD_IMAGE_INFO cImgInfoLastSnapshot;			//最后大图信息
    RECORD_IMAGE_INFO cImgInfoBeginCapture;			//第一张抓拍图信息
    RECORD_IMAGE_INFO cImgInfoBestCapture;			//最清晰抓拍图信息
    RECORD_IMAGE_INFO cImgInfoLastCapture;			//最后一张抓拍图信息
    RECORD_IMAGE_INFO cImgInfoSmaller;				//车牌小图信息
    RECORD_IMAGE_INFO cImgInfoBinary;				//车牌二值图信息

    PBYTE pbImgDataBestSnapShot;					//最清晰大图数据
    PBYTE pbImgDataLastSnapShot;					//最后大图数据
    PBYTE pbImgDataBeginCapture;					//第一张抓拍图数据
    PBYTE pbImgDataBestCapture;						//最清晰抓拍图数据
    PBYTE pbImgDataLastCapture;						//最后一张抓拍图数据
    PBYTE pbImgDataSmaller;							//车牌小图数据
    PBYTE pbImgDataBinary;							//车牌二值图数据

    PBYTE pbImgInfoBestSnapShot;					//最清晰大图原始信息
    PBYTE pbImgInfoLastSnapShot;					//最后大图原始信息
    PBYTE pbImgInfoBeginCapture;					//第一张抓拍图原始信息
    PBYTE pbImgInfoBestCapture;						//最清晰抓拍图原始信息
    PBYTE pbImgInfoLastCapture;						//最后一张抓拍图原始信息
    PBYTE pbImgInfoSmaller;							//车牌小图原始信息
    PBYTE pbImgInfoBinary;							//车牌二值图原始信息

    DWORD dwImgDataBestSnapShotLen;					//最清晰大图图片数据长度
    DWORD dwImgDataLastSnapShotLen;					//最后大图图片数据长度
    DWORD dwImgDataBeginCaptureLen;					//第一张抓拍图图片数据长度
    DWORD dwImgDataBestCaptureLen;					//最清晰抓拍图图片数据长度
    DWORD dwImgDataLastCaptureLen;					//最后一张抓拍图图片数据长度
    DWORD dwImgDataSmallerLen;						//车牌小图图片数据长度
    DWORD dwImgDataBinaryLen;						//车牌二值图图片数据长度

    DWORD dwImgInfoBestSnapShotLen;					//最清晰大图原始信息长度
    DWORD dwImgInfoLastSnapShotLen;					//最后大图原始信息长度
    DWORD dwImgInfoBeginCaptureLen;					//第一张抓拍图原始信息长度
    DWORD dwImgInfoBestCaptureLen;					//最清晰抓拍图原始信息长度
    DWORD dwImgInfoLastCaptureLen;					//最后一张抓拍图原始信息长度
    DWORD dwImgInfoSmallerLen;						//车牌小图原始信息长度
    DWORD dwImgInfoBinaryLen;						//车牌二值图原始信息长度
}
RECORD_IMAGE_GROUP;

/**
* @brief			从识别数据信息中提取车牌号字符串
* @param[in]		szResultInfo	识别数据信息
* @param[out]		szPlateBuf		返回的车牌号字符串
* @param[in]		nBufLen			车牌号字符串缓存长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_GetPlateString(LPCSTR szResultInfo,
                                               LPSTR szPlateBuf,
                                               INT nBufLen);

/**
* @brief			从识别结果数据中提取图片
* @param[in]		pbResultPacket	识别结果数据包
* @param[in]		dwPacketLen		识别结果数据包长度
* @param[in]		eImageType		需要获取的图片类型
* @param[out]		pcRecordImage	返回的结果图片。注：pcRecordImage中pbImgData所指向的内存空间为pbResultPacket所指向内存空间的某一部分。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_GetRecordImage(PBYTE pbResultPacket,
                                               DWORD dwPacketLen,
                                               RECORD_IMAGE_TYPE eImageType,
                                               RECORD_IMAGE* pcRecordImage);

/**
* @brief			从识别结果数据中提取所有图片
* @param[in]		pbResultPacket			识别结果数据包
* @param[in]		dwPacketLen				识别结果数据包长度
* @param[out]		pcRecordImageGroup		返回的结果图片集。注：pcRecordImage中所有指针指向的内存空间为pbResultPacket所指向内存空间的某一部分。
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_GetRecordImageAll(PBYTE pbResultPacket,
                                                  DWORD dwPacketLen,
                                                  RECORD_IMAGE_GROUP* pcRecordImageGroup);

/**
* @brief			从历史录像数据包内获取图片数据及红灯坐标信息
* @param[in]		pbVideoFrameData		历史视频桢数据
* @param[in]		dwFrameLen				历史视频桢数据长度
* @param[out]		iRedLightCount			桢包含红灯坐标个数
* @param[in]		iRedLightBufLen			桢红灯坐标缓存区长度
* @param[out]		pbRedLightPos			桢红灯坐标缓存区
* @param[out]		pbJPEGData				桢图像数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_ParseHistoryVideoFrame(PBYTE pbVideoFrameData,
                                                       DWORD dwFrameLen,
                                                       int& iRedLightCount, 
                                                       int iRedLightBufLen,
                                                       PBYTE pbRedLightPos,
                                                       PBYTE& pbJPEGData,
                                                       DWORD& dwJPEGDataLen);

/**
* @brief			将识别结果小图转换为BMP位图
* @param[in]		pbSmallImageData	识别结果小图。注：因为小图的数据格式为YUV422，所以其大小为：(nSmallImageWidth * nSmallImageHeight * 2)
* @param[in]		nSmallImageWidth	识别结果小图宽度
* @param[in]		nSmallImageHeight	识别结果小图高度
* @param[in]		pbBitmapData		BMP位图数据存放缓冲区
* @param[in,out]	pnBitmapDataLen		BMP位图数据存放缓冲区长度。函数返回后则为：BMP位图数据实际长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_SmallImageToBitmap(PBYTE pbSmallImageData,
                                                   INT nSmallImageWidth,
                                                   INT nSmallImageHeight,
                                                   PBYTE pbBitmapData,
                                                   INT* pnBitmapDataLen);

/**
* @brief			将识别结果二值图转换为BMP位图
* @param[in]		pbBinImageData		识别结果二值图
* @param[in]		pbBitmapData		BMP位图数据存放缓冲区
* @param[in,out]	pnBitmapDataLen		BMP位图数据存放缓冲区长度。函数返回后则为：BMP位图数据实际长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_BinImageToBitmap(PBYTE pbBinImageData,
                                                 PBYTE pbBitmapData,
                                                 INT* pnBitmapDataLen);

/**
* @brief			结果大图红灯加红处理
* @param[in]		pcSrcImage			识别结果大图
* @param[in]		pbDstImage			加红后的结果大图数据
* @param[in out]	iDstBufLen			输入结果图片缓存区长度，输出结果图片数据长度
* @param[in]		iBrightness			亮度增强
* @param[in]		iHueThreshold		色度阀值
* @param[in]		iCompressRate		压缩率
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_TrafficLightEnhance(PBYTE pbSrcImg,
                                                    int iSrcImgDataLen,
                                                    int iRedLightCount,
                                                    PBYTE pbRedLightPos,
                                                    PBYTE pbDstImage,
                                                    int& iDstBufLen, 
                                                    int iBrightness,
                                                    int iHueThreshold,
                                                    int iCompressRate);

/**
* @brief			从识别结果大图中获取红灯坐标位置
* @param[in]		pcRecordImage		识别结果大图
* @param[in]		iRedLightPosBufLen	红灯位置缓存大小
* @param[in]		pbRedLightPos		红灯位置缓存
* @param[out]		iRedLightCount		红灯个数
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_GetRedLightPosFromeRecordImage(RECORD_IMAGE* pcRecordImage,
                                                               int iRedLightPosBufLen, 
                                                               PBYTE pbRedLightPos,
                                                               int& iRedLightCount);

/**
* @brief			从视频图像附加信息中获取车牌坐标位置
* @param[in]		pszVideoExtInfo		视频图片附加信息
* @param[in]		iPlatePosBufLen		车牌位置缓存大小
* @param[in]		pbPlatePos			车牌位置缓存
* @param[out]		iPlateCount			车牌个数
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_GetPlatePosFromeJpegInfo(PBYTE pbJpegInfo,
                                                         DWORD dwJpegInfoLen,
                                                         int iPlatePosBufLen, 
                                                         PBYTE pbPlatePos,
                                                         int& iPlateCount);

/**
* @brief			从HVAPI_ExecCmd执行返回的结果中解析出返回码
* @param[in]		szXmlBuf			HVAPI_ExecCmd执行返回的结果
* @param[out]		szRetcode			返回码
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_ParseXmlCmdRespRetcode(const char* szXmlBuf,
                                                       char* szRetcode);
/**
* @brief			从HVAPI_ExecCmd执行返回的结果中解析出返回文本信息
* @param[in]		szXmlBuf			HVAPI_ExecCmd执行返回的结果
* @param[in]		szMsgID				信息ID
* @param[out]		szMsgOut			信息ID对应的文本信息
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_ParseXmlCmdRespMsg(const char* szXmlBuf,
                                                   const char* szMsgID,
                                                   char* szMsgOut);

/**
* @brief			从HVAPI_GetInfo执行返回的结果中解析出指定信息
* @param[in]		szXmlBuf			HVAPI_GetInfo执行返回的结果
* @param[in]		szInfoName			信息名
* @param[in]		nInfoValueName		值名
* @param[out]		szInfoValueText		相应的文本信息
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_ParseXmlInfoRespValue(const char* szXmlBuf,
                                                      const char* szInfoName, 
                                                      const char* nInfoValueName,
                                                      char* szInfoValueText);

/**
* @brief			设置设备参数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szParamDoc		要保存到设备的参数（XML形式）
* @return			成功：S_OK；失败：E_FAIL
* @warning			更改过的参数要在设备下次启动后才会生效
*/
HV_API HRESULT CDECL HVAPI_SendControllPannelUpdateFile(HVAPI_HANDLE hHandle,
														PBYTE pUpdateFileBuffer,
														DWORD dwFileSize);

/**
* @brief			设置设备参数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szParamDoc		要保存到设备的参数（XML形式）
* @return			成功：S_OK；失败：E_FAIL
* @warning			更改过的参数要在设备下次启动后才会生效
*/
HV_API HRESULT CDECL HVAPIUTILS_GetExecXmlCmdResString(HVAPI_HANDLE hHandle, 
													   char* szXmlBuf, 
													   char* szCmdName, 
													   char* szCmdValueName, 
													   char* szCmdValueText);

/**
* @brief			从结果附加信息中获取某项信息
* @param[in]		szAppened			结果附加信息
* @param[in]		szInfoName			获取项名
* @param[out]		szRetInfo			结果字串
* @param[in]		iRetInfoBufLen		结果字串保存缓存区长
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_GetRecordInfoFromAppenedString(LPCSTR szAppened, LPCSTR szInfoName, LPSTR szRetInfo, INT iRetInfoBufLen);

/**
* @brief			从结果附加信息中提取全部信息每项目信息使用回车符区分
* @param[in]		pszXmlPlateInfo			结果附加信息
* @param[out]		pszPlateInfoBuf			结果字串缓存区
* @param[in]		iPlateInfoBufLen		结果字串缓存区长度
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPIUTILS_ParsePlateXmlString(LPCSTR pszXmlPlateInfo, LPSTR pszPlateInfoBuf, INT iPlateInfoBufLen);

#endif // _HVAPI_H_
