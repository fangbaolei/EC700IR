///////////////////////////////////////////////////////////
//  CSWSocketStream.cpp
//  Implementation of the Class CSWTcpStream
//  Created on:      28-二月-2013 14:09:47
//  Original author: zy
///////////////////////////////////////////////////////////

#include "SWTCPSocket.h"
#include "SWUDPSocket.h"
#include "SWSocketStream.h"

/**
 * @brief 构造函数
 */
CSWTcpStream::CSWTcpStream( CSWTCPSocket *pTCPSocket )
:m_pTCPSocket( pTCPSocket )
{
    if( m_pTCPSocket != NULL ) m_pTCPSocket->AddRef( );
}

/**
 * @brief 析构函数
 */
CSWTcpStream::~CSWTcpStream(){
    if( m_pTCPSocket != NULL ) m_pTCPSocket->Release( );
}

/**
 * @brief 流写入函数
 *
 * @param [in] pvBuf : 存放要写入数据的的缓冲区
 * @param [in] dwCB : 写入数据的的大小
 * @param [out] pdwWritten : 实际写入的字节数
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWTcpStream::Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten){

    if( m_pTCPSocket == NULL || !m_pTCPSocket->IsValid()) return E_FAIL;

    return m_pTCPSocket->Send( pvBuf , dwCB , pdwWritten);
}

/**
 * @brief 流读取函数
 *
 * @param [in] pvBuf : 存放读取结果的缓冲区
 * @param [in] dwCB : 存放读取结果的缓冲区的大小
 * @param [out] pdwRead : 实际读取到的字节数
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWTcpStream::Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead){

    if( m_pTCPSocket == NULL || !m_pTCPSocket->IsValid()) return E_FAIL;

    return m_pTCPSocket->Read( pvBuf , dwCB , pdwRead);
}


/**
 * @brief 获取承载流的媒介对象的函数
 *
 * @param [out] pMediumObj : 媒介对象指针
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWTcpStream::GetMedium(CSWObject ** pMediumObj)
{
	if (NULL == pMediumObj)
	{
		return E_FAIL;
	}

	*pMediumObj = m_pTCPSocket;

	return S_OK;
}



/**
 * @brief 构造函数
 */
CSWUdpStream::CSWUdpStream( CSWUDPSocket *pUPDSocket )
:m_pUDPSocket( pUPDSocket )
{
    if( m_pUDPSocket != NULL ) m_pUDPSocket->AddRef( );
}
/**
 * @brief 析构函数
 */
CSWUdpStream::~CSWUdpStream(){
    if( m_pUDPSocket != NULL ) m_pUDPSocket->Release( );
}

/**
 * @brief 流写入函数
 *
 * @param [in] pvBuf : 存放要写入数据的的缓冲区
 * @param [in] dwCB : 写入数据的的大小
 * @param [out] pdwWritten : 实际写入的字节数
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWUdpStream::Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten){

    if( m_pUDPSocket == NULL || !m_pUDPSocket->IsValid()) return E_FAIL;

	return  m_pUDPSocket->SendTo( pvBuf , dwCB , pdwWritten);
}
/**
 * @brief 流读取函数
 *
 * @param [in] pvBuf : 存放读取结果的缓冲区
 * @param [in] dwCB : 存放读取结果的缓冲区的大小
 * @param [out] pdwRead : 实际读取到的字节数
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWUdpStream::Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead){
   if( m_pUDPSocket == NULL || !m_pUDPSocket->IsValid()) return E_FAIL;

	return  m_pUDPSocket->RecvFrom( pvBuf, dwCB, pdwRead );
}


/**
 * @brief 获取承载流的媒介对象的函数
 *
 * @param [out] pMediumObj : 媒介对象指针
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWUdpStream::GetMedium(CSWObject ** pMediumObj)
{
	if (NULL == pMediumObj)
	{
		return E_FAIL;
	}

	*pMediumObj = m_pUDPSocket;

	return S_OK;
}


