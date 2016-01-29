#ifndef _HV_AUTOLINK_DEVICE_H_
#define _HV_AUTOLINK_DEVICE_H_

/* 主动链接监听类型 */
#define	LISTEN_TYPE_RECORD  0x00000001  /**< 结果链接 */
#define LISTEN_TYPE_VEDIO   0x00000002  /**< 视频链接 */
#define LISTEN_TYPE_IMAGE   0x00000003  /**< 图像链接 */
#define LISTEN_TYPE_CMD     0x00000004  /**< 命令链接 */


/**
* @brief			加载主动链接服务器 监控模块
* @param[in]		nMaxMonitorCount 最大监控数 
* @param[in]		szApiVer		 对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：S_OK；失败：E_FAIL
*/

HV_API HRESULT CDECL HVAPI_LoadMonitor(INT nMaxMonitorCount, LPCSTR szApiVer);

/**
* @brief			打开指定类型 服务器监控
* @param[in]		wPort			监控端口
* @param[in]		nType			监控链接类型（结果、视频、图像、命令）
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_OpenServer(INT nPort , INT nType, LPCSTR szApiVer);

/**
* @brief			关闭指定类型 服务器监控
* @param[in]		nType			监控链接类型
* @return			成功：S_OK ；失败：E_FAIL
*/
HV_API HRESULT CDECL HVAPI_CloseServer(INT nType);

/**
* @brief			卸载主动链接服务器 监控模块
* @return			成功：S_OK；失败：E_FAIL
*/

HV_API HRESULT CDECL HVAPI_UnLoadMonitor();


/**
* @brief			搜索主动链接设备
* @param[in]		nDevListLen		设备列表的长度
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：设备句柄；失败：E_FAIL
*/

HV_API HRESULT CDECL HVAPI_GetDeviceListSize(INT& nDevListLen , LPCSTR szApiVer);

/**
* @brief			搜索主动链接设备
* @param[in]		szDevList		接收设备列表的缓存区
* @param[in]		nDevListLen		缓存区长度
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：设备句柄；失败：E_FAIL
*/

HV_API INT CDECL HVAPI_GetDeviceList(LPSTR szDevList, INT nDevListLen , LPCSTR szApiVer);

/**
* @brief			打开设备句柄
* @param[in]		szDevSN			设备编号
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @return			成功：设备句柄；失败：NULL
*/

HV_API HRESULT CDECL HVAPI_GetDeviceStatus(LPSTR szDevSN ,INT& nRecordLinkStatus , INT& nRecordLinkReConCount , LPCSTR szApiVer);


/**
* @brief			打开设备句柄
* @param[in]		pDevName		设备的名称
* @param[in]		szApiVer		对应设备的API版本。注：为NULL则默认HVAPI_API_VERSION
* @param[in]		nType			对应设备名称的类型：IP 或 设备编号
* @return			成功：设备句柄；失败：NULL
*/
HV_API HVAPI_HANDLE_EX CDECL HVAPI_OpenAutoLink(LPCSTR szDevName, LPCSTR szApiVer);








#endif