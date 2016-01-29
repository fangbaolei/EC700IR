///////////////////////////////////////////////////////////
//  CSWMemoryStream.cpp
//  Implementation of the Class CSWMemoryStream
//  Created on:      28-二月-2013 14:09:51
//  Original author: zhouy
///////////////////////////////////////////////////////////

#include "SWMemory.h"

#include "SWMemoryStream.h"

/**
 * @brief 构造函数
 *
 * @param [in] dwType : 内存种类，目前取值范围有【系统堆内存，DSP共享内存，Cache高速缓存，文件映射内存】
 * @param [in] dwSize : 要申请的内存的大小，单位为字节
 */
CSWMemoryStream::CSWMemoryStream(DWORD dwType, DWORD dwSize)
: m_pMemoryManagement( NULL )
, m_pBuffer( NULL )
{
	/*
    switch( dwType )
    {
    case 0:
        m_pMemoryManagement = new CSWCacheMemory( ); break;
    case 1:
        m_pMemoryManagement = new CSWMappedMemory( ); break;
    case 2:
        m_pMemoryManagement = new CSWSharedMemory( ); break;
    default:
        break;
    }

    // 初始化内存块
    InitialMemory( dwSize );
	*/
}


/**
 * @brief 析构函数
 */
CSWMemoryStream::~CSWMemoryStream(){
	/*
    // 释放内存资源
    if( m_pMemoryManagement != NULL ){

        if( m_pBuffer != NULL )
            m_pMemoryManagement->Free( m_pBuffer );
        m_pBuffer = NULL;

         delete m_pMemoryManagement;
    }

    m_pMemoryManagement = NULL;
	*/
}

 /**
 * @brief 初始化内存块
 *
 * @param [DWORD] dwSize : 分配内存大小
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWMemoryStream::InitialMemory( DWORD dwSize )
{
	/*
    if( m_pMemoryManagement == NULL || dwSize <= 0) return E_FAIL;

    m_pBuffer = m_pMemoryManagement->Alloc( dwSize );

    return ( m_pBuffer != NULL ? S_OK : E_FAIL);
	*/
	return E_NOTIMPL;
}

/**
 * @brief 提交并清空流缓冲
 *
 * @param [in] dwCommitFlags : 提交标志
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWMemoryStream::Commit(DWORD dwCommitFlags)
{
	return  E_NOTIMPL;
}


/**
 * @brief 流数据拷贝函数
 *
 * @param [in] pStm : 目标流指针
 * @param [in] dwCb : 拷贝数据的长度
 * @param [in] dwSrcPos : 源流的起始点
 * @param [in] dwDstPos : 目标流的起始点
 * @param [out] pdwLen : 实际拷贝的数据长度
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWMemoryStream::CopyTo(CSWStream * pStm, DWORD dwCb, DWORD dwSrcPos, DWORD dwDstPos, DWORD * pdwLen)
{
	return  E_NOTIMPL;
}


/**
 * @brief 获取对象申请到的内存块首地址
 *
 * @return
 * - NULL : 该对象分配内存失败
 * -其它: 内存的地址
 */
PVOID CSWMemoryStream::GetAddr(){

	return  NULL;
}


/**
 * @brief 获取对象申请到的内存块物理首地址
 *
 * @return
 * - NULL : 该对象分配内存失败
 * -其它: 内存的物理地址
 * @note
 * - 目前只对DSP共享内存有效；对其它种类内存而言，返回值将与GetAddr()的返回值相同
 */
LPVOID CSWMemoryStream::GetPhyAddr(){

	return  NULL;
}


/**
 * @brief 用于向设备发控制和配置命令
 *
 * @param [in] dwCmd : 控制命令ID
 * @param [in] pvArg : 控制命令参数指针
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWMemoryStream::IOCtrl(DWORD dwCmd, VOID * pvArg)
{
	return  E_NOTIMPL;
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
HRESULT CSWMemoryStream::Read(PBYTE pvBuf, DWORD dwCB, PDWORD pdwRead)
{
	return  E_NOTIMPL;
}


/**
 * @brief 重定位流的内部位置指针
 *
 * @param [in] sdwOffset : 偏移量
 * @param [in] dwFromWhere : 相对起始位置，0:文件头(SEEK_SET)，1:当前位置(SEEK_CUR)，2:
 * 文件尾(SEEK_END)
 * @param [out] pdwNewPos : 新位置的内部文件指针值
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWMemoryStream::Seek(INT sdwOffset, DWORD dwFromWhere, DWORD * pdwNewPos)
{
	return  E_NOTIMPL;
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
HRESULT CSWMemoryStream::Write(PBYTE pvBuf, DWORD dwCB, PDWORD pdwWritten)
{
	return  E_NOTIMPL;
}

