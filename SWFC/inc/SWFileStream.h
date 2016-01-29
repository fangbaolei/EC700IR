///////////////////////////////////////////////////////////
//  CSWFileStream.h
//  Implementation of the Class CSWFileStream
//  Created on:      28-二月-2013 14:09:48
//  Original author: zy
///////////////////////////////////////////////////////////

#if !defined(_CSWFILESTREAM__INCLUDED_)
#define _CSWFILESTREAM__INCLUDED_

#include "SWStream.h"
#include "SWFile.h"


/**
 * @brief 文件流基类
 */
class CSWFileStream : public CSWStream
{
CLASSINFO(CSWFileStream,CSWStream)
public:
	/**
	 * @brief 构造函数
	 */
	CSWFileStream();
	/**
	 * @brief 构造函数
	 * @param [in] szFileName : 被打开的文件名，包含设备文件和一般的磁盘文件
	 * @param [in] szMode : 文件打开模式
	 */
	HRESULT Initialize(PCSTR szFileName, PCSTR szMode);
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWFileStream();
	/**
	 * @brief 提交并清空流缓冲
	 *
	 * @param [in] dwCommitFlags : 提交标志
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT Commit(DWORD dwCommitFlags=0);
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
	virtual HRESULT CopyTo(CSWStream * pStm, DWORD dwCB, DWORD dwSrcPos, DWORD dwDstPos, DWORD * pdwLen=NULL);
	/**
	 * @brief 用于向设备发控制和配置命令
	 *
	 * @param [in] dwCmd : 控制命令ID
	 * @param [in] pvArg : 控制命令参数指针
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT IOCtrl(DWORD dwCmd, VOID * pvArg);
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
	virtual HRESULT Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead=NULL);
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
	virtual HRESULT Seek(INT iOffset, DWORD dwFromWhere, DWORD * pdwNewPos=NULL);
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
	virtual HRESULT Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten=NULL);

	/**
	 * @brief 获取承载流的媒介对象的函数
	 *
	 * @param [out] pMediumObj : 媒介对象指针
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT GetMedium(CSWObject ** pMediumObj){};
	/**
	 * @brief 获取文件流大小
	 *
	 * @return
	 * - 文件流大小
	 * 
	 */
	virtual DWORD GetSize(VOID);
	
	/**
	 * @brief 获取文件流当前读写位置
	 *
	 * @return
	 * - 当前读写位置
	 * 
	 */
	virtual DWORD GetCurrentPos(VOID);

private	:
	CSWFile * m_pSWFile;
	PBYTE	m_pbBuf;
	DWORD	m_dwSize;
	DWORD	m_dwOffset;
	BOOL	m_flgInited;
	DWORD	m_dwRealSize;

};

#endif // !defined(_CSWFILESTREAM__INCLUDED_)



