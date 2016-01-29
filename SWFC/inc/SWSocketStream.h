///////////////////////////////////////////////////////////
//  CSWSocketStream.h
//  Implementation of the Class CSWTcpStream
//  Created on:      28-二月-2013 14:09:47
//  Original author: zy
///////////////////////////////////////////////////////////

#if !defined(EA_51732DFE_5587_44d0_98F3_4454E1A20423__INCLUDED_)
#define EA_51732DFE_5587_44d0_98F3_4454E1A20423__INCLUDED_

#include "SWStream.h"

//todo:
//typedef (BOOL (*)(VOID *pContext,* CSWSocketStream * client) ONACCEPT;
//CSWSocketStream未定义

class CSWTCPSocket;
/**
 * @brief TCP流管理基类
 */
class CSWTcpStream : public CSWStream
{
CLASSINFO(CSWTcpStream,CSWStream)
public:
	/**
	 * @brief 构造函数
	 */
	CSWTcpStream( CSWTCPSocket *pTCPSocket );
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWTcpStream();

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
	virtual HRESULT Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten);

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
	virtual HRESULT Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead);

	/**
	 * @brief 获取承载流的媒介对象的函数
	 *
	 * @param [out] pMediumObj : 媒介对象指针
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT GetMedium(CSWObject ** pMediumObj);

private:
	/**
	 * @brief 提交并清空流缓冲
	 *
	 * @param [in] dwCommitFlags : 提交标志
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT Commit(DWORD dwCommitFlags){}


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
	virtual HRESULT CopyTo(CSWStream * pStm, DWORD dwCb, DWORD dwSrcPos
                        , DWORD dwDstPos, DWORD * pdwLen){}
	/**
	 * @brief 用于向设备发控制和配置命令
	 *
	 * @param [in] dwCmd : 控制命令ID
	 * @param [in] pvArg : 控制命令参数指针
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT IOCtrl(DWORD dwCmd, VOID * pvArg){}

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
	virtual HRESULT Seek(INT sdwOffset, DWORD dwFromWhere, DWORD * pdwNewPos){}

private:
    CSWTCPSocket *m_pTCPSocket;
};

class CSWUDPSocket;
/**
 * @brief UDP流管理基类
 */
class CSWUdpStream : public CSWStream
{
CLASSINFO(CSWUdpStream,CSWStream)
public:
	/**
	 * @brief 构造函数
	 */
	CSWUdpStream( CSWUDPSocket *pUPDSocket );
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWUdpStream();

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
	virtual HRESULT Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten);

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
	virtual HRESULT Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead);

	/**
	 * @brief 获取承载流的媒介对象的函数
	 *
	 * @param [out] pMediumObj : 媒介对象指针
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT GetMedium(CSWObject ** pMediumObj);

private:
	/**
	 * @brief 提交并清空流缓冲
	 *
	 * @param [in] dwCommitFlags : 提交标志
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT Commit(DWORD dwCommitFlags){}
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
	virtual HRESULT CopyTo(CSWStream * pStm, DWORD dwCb, DWORD dwSrcPos
                        , DWORD dwDstPos, DWORD * pdwLen){}
	/**
	 * @brief 用于向设备发控制和配置命令
	 *
	 * @param [in] dwCmd : 控制命令ID
	 * @param [in] pvArg : 控制命令参数指针
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT IOCtrl(DWORD dwCmd, VOID * pvArg){}

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
	virtual HRESULT Seek(INT sdwOffset, DWORD dwFromWhere, DWORD * pdwNewPos){}

private:
    // UDP套接字
    CSWUDPSocket *m_pUDPSocket;
};
#endif // !defined(EA_51732DFE_5587_44d0_98F3_4454E1A20423__INCLUDED_)

