/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		HvSockUtils.h
* @version	1.0
* @brief	对原始Socket函数进行封装后的实用工具函数
* @author	Shaorg
* @date		2010-8-12
*/

#ifndef _HVSOCKUTILS_H_
#define _HVSOCKUTILS_H_

#include "hvsocket.h"
using namespace HiVideo;

/**
* @brief	    通过默认参数快速创建一个socket
* @param[in]    af          地址类型
* @param[in]    type        数据流类型
* @param[in]    protocol    协议类型
* @return	    成功：一个有效的socket描述符，失败：INVALID_SOCKET
*/
HV_SOCKET_HANDLE HvCreateSocket(int af=AF_INET, int type=SOCK_STREAM, int protocol=0);

/**
* @brief	    侦听套接字
* @param[in]    hSocket     一个有效的套接字描述符
* @param[in]    wPort       要开启的用于侦听的本地端口
* @param[in]    backlog     最大侦听队列的长度
* @return	    成功：S_OK，失败：E_FAIL
*/
HRESULT HvListen(const HV_SOCKET_HANDLE& hSocket, WORD16 wPort, int backlog);

/**
* @brief	    设置套接字接收超时
* @param[in]    hSocket     一个有效的套接字描述符
* @param[in]    iMS         超时时限（单位：毫秒）
* @return	    成功：S_OK，失败：E_FAIL
*/
HRESULT HvSetRecvTimeOut(const HV_SOCKET_HANDLE& hSocket, int iMS);

/**
* @brief	    设置套接字发送超时
* @param[in]    hSocket     一个有效的套接字描述符
* @param[in]    iMS         超时时限（单位：毫秒）
* @return	    成功：S_OK，失败：E_FAIL
*/
HRESULT HvSetSendTimeOut(const HV_SOCKET_HANDLE& hSocket, int iMS);

/**
* @brief	    连接
* @param[in]    hSocket     一个有效的套接字描述符
* @param[in]    pszIPv4     连接地址
* @param[in]    wPort       连接端口
* @param[in]    iTimeout    超时时限（单位：毫秒；-1表示无限等待）
* @return	    成功：S_OK，失败：E_FAIL，超时：S_FALSE
*/
HRESULT HvConnect(const HV_SOCKET_HANDLE& hSocket, const char* pszIPv4, WORD16 wPort, int iTimeout=-1);

/**
* @brief	    接受
* @param[in]    hSocket     一个有效的套接字描述符
* @param[out]   hNewSocket  一个新的用于收发数据的套接字描述符
* @param[in]    iTimeout    超时时限（单位：毫秒；-1表示无限等待）
* @return	    成功：S_OK，失败：E_FAIL，超时：S_FALSE
*/
HRESULT HvAccept(const HV_SOCKET_HANDLE& hSocket, HV_SOCKET_HANDLE& hNewSocket, int iTimeout=-1);

/**
* @brief	    获取对方地址及端口
* @param[in]    hSocket     一个有效的套接字描述符
* @param[out]   pdwIPv4     获取到的地址（网络字节序）
* @param[out]   pwPort      获取到的端口（主机字节序）
* @return	    成功：S_OK，失败：E_FAIL
*/
HRESULT HvGetPeerName(const HV_SOCKET_HANDLE& hSocket, DWORD32* pdwIPv4, WORD16* pwPort);

/**
* @brief	    绑定UDP端口
* @param[in]    hSocket     一个有效的套接字描述符
* @param[in]    wLocalPort   要开启的用于绑定的本地端口
* @return	    成功：S_OK，失败：E_FAIL，句柄无效：E_HANDLE
*/
HRESULT HvUDPBind(const HV_SOCKET_HANDLE& hSocket, WORD16 wLocalPort);

#endif
