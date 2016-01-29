/**
* @file SWFile.cpp
* @brief CWFileStream`ʵЖτ¼�
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-20
* @version 1.0
*/

#include "SWLog.h"
#include "SWFileStream.h"
#include "swpa.h"


//todo: log output
#define CSWFILESTREAM_PRINT SW_TRACE_DEBUG

#define CSWFILESTREAM_CHECK(arg)	\
if (!(arg)) 																	\
{ 																				\
	CSWFILESTREAM_PRINT("%s: ", __FUNCTION__);	\
	CSWFILESTREAM_PRINT("Check %s FAILED! [%d]\n", #arg, (INT)E_INVALIDARG);	\
	return E_INVALIDARG;													\
}


HRESULT CSWFileStream::Initialize(PCSTR szFileName, PCSTR szMode)
{
	
	DWORD dwRead = 0;

	if (m_flgInited)
	{
		return S_OK;
	}

	if (NULL == szFileName)
	{
		CSWFILESTREAM_PRINT("%s() Err: szFileName is NULL\n", __FUNCTION__);
		return E_INVALIDARG;
	}
	if (NULL == szMode)
	{
		CSWFILESTREAM_PRINT("%s() Err: szMode is NULL\n", __FUNCTION__);
		return E_INVALIDARG;
	}

	m_flgInited = FALSE;	
	m_dwOffset = 0;
	m_dwSize = 0;
	m_dwRealSize = 0;

	if (NULL != m_pSWFile)
	{
		delete m_pSWFile;
	}
	
	m_pSWFile = new CSWFile();
	if (NULL == m_pSWFile)
	{
		CSWFILESTREAM_PRINT("Err: failed to alloc memory for m_pSWFile\n");
		return E_OUTOFMEMORY;
	}

	if (FAILED(m_pSWFile->Open(szFileName, szMode)))
	{
		CSWFILESTREAM_PRINT("Err: failed to opne %s with %s mode \n", szFileName, szMode);
		return E_FAIL;
	}
	
	if (NULL == m_pbBuf)
	{
		if (S_OK != m_pSWFile->GetSize(&m_dwSize))
		{
			CSWFILESTREAM_PRINT("%s() Err: failed to GetSize() \n", __FUNCTION__);
			m_pSWFile->Close();
			m_dwSize = 0;
			m_pbBuf = NULL;
			m_dwRealSize = 0;
			return E_FAIL;
		}

	

		m_dwRealSize = m_dwSize;
		
		if (0 == m_dwSize)
		{
			m_flgInited = TRUE;
			return S_OK;
		}

		//read the whole file into buffer
		CSWFile cTempFile;
		if (FAILED(cTempFile.Open(szFileName, "r")))
		{
			CSWFILESTREAM_PRINT("Err: failed to Open %s \n", szFileName);
			m_pSWFile->Close();
			return E_FAIL;
		}

		m_pbBuf = (PBYTE)swpa_mem_alloc(m_dwSize);
		if (NULL == m_pbBuf)
		{
			CSWFILESTREAM_PRINT("Err: failed to alloc mem for m_pvBuf \n");
			cTempFile.Close();
			m_pSWFile->Close();
			return E_FAIL;
		}

		//read the whole file into buffer
		if (S_OK != cTempFile.Read((PVOID)m_pbBuf, m_dwSize, &dwRead) ||
			dwRead != m_dwSize)
		{
			CSWFILESTREAM_PRINT("%s() Err: failed to Read() \n", __FUNCTION__);
			
			swpa_mem_free(m_pbBuf);
			m_pbBuf = NULL;
			m_dwOffset = 0;
			m_dwRealSize = 0;
			cTempFile.Close();
			m_pSWFile->Close();
			return E_FAIL;
		}

		cTempFile.Close();
		m_flgInited = TRUE;
	}
	
	return S_OK;
	
}

/**
 * @brief 构造函数
 */
CSWFileStream::CSWFileStream()
{

	m_pSWFile = NULL;
	m_pbBuf = NULL;
	m_dwSize = 0;
	m_dwOffset = 0;
	m_dwRealSize = 0;
	m_flgInited = FALSE;
}



/**
 * @brief 析构函数
 */
CSWFileStream::~CSWFileStream()
{

	if (m_flgInited)
	{
		//Commit(0);

		m_dwOffset = 0;
		m_dwSize = 0;
		m_dwRealSize = 0;
		m_flgInited = FALSE;
		
		swpa_mem_free(m_pbBuf);
		m_pbBuf = NULL;
	}

	if (NULL != m_pSWFile)
	{
		delete m_pSWFile;
		m_pSWFile = NULL;
	}
}


/**
 * @brief 提交并清空流缓冲
 *
 * @param [in] dwCommitFlags : 提交标志
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWFileStream::Commit(DWORD dwCommitFlags)
{

	CSWFILESTREAM_CHECK(m_flgInited);	

	if (S_OK != m_pSWFile->Seek(0, SWPA_SEEK_SET, NULL))
	{
		CSWFILESTREAM_PRINT("%s() Err: failed to Seek to Beginning \n", __FUNCTION__);
		return E_FAIL;
	}

	
	return  m_pSWFile->Write((PVOID)m_pbBuf, m_dwRealSize, NULL);
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
HRESULT CSWFileStream::CopyTo(CSWStream * pStm, DWORD dwCB, DWORD dwSrcPos, DWORD dwDstPos, DWORD * pdwLen)
{

	DWORD dwPos = 0;
	DWORD dwSize = 0;
	DWORD dwWritten = 0;

	CSWFILESTREAM_CHECK(m_flgInited);
	
	CSWFILESTREAM_CHECK(NULL != pStm);
	
	if (Seek(dwSrcPos, SWPA_SEEK_SET, &dwPos) ||
		dwPos != dwSrcPos)
	{
		CSWFILESTREAM_PRINT("%s() Err: failed to Seek dwSrcPos \n", __FUNCTION__);
		return E_FAIL;
	}

	if (pStm->Seek(dwDstPos, SWPA_SEEK_SET, &dwPos) ||
		dwPos != dwDstPos)
	{
		CSWFILESTREAM_PRINT("%s() Err: failed to Seek dwDstPos \n", __FUNCTION__);
		return E_FAIL;
	}

	dwSize = (dwCB > m_dwSize - dwSrcPos) ? (m_dwSize - dwSrcPos) : dwCB;

	if (S_OK != pStm->Write(m_pbBuf + dwSrcPos, dwSize, &dwWritten))
	{
		CSWFILESTREAM_PRINT("%s() Err: failed to pStm->Write() \n", __FUNCTION__);
		return E_FAIL;
	}

	if (NULL != pdwLen)
	{
		*pdwLen = dwWritten;
	}
	
	return  S_OK;

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
HRESULT CSWFileStream::IOCtrl(DWORD dwCmd, VOID * pvArg)
{
	
	CSWFILESTREAM_CHECK(m_flgInited);

	return  m_pSWFile->IOCtrl(dwCmd, pvArg);
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
HRESULT CSWFileStream::Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead)
{

	DWORD dwRead = 0;
	DWORD dwMin = 0;
		
	CSWFILESTREAM_CHECK(m_flgInited);
	CSWFILESTREAM_CHECK(NULL != pvBuf);
	CSWFILESTREAM_CHECK(0 != dwCB);

	dwMin = (dwCB > m_dwRealSize) ? m_dwRealSize : dwCB;
	if (0 == m_dwRealSize || NULL == m_pbBuf)
	{	
		//no data 
		return E_FAIL;
	}

	if (m_dwOffset + dwMin > m_dwRealSize)
	{
		dwMin = m_dwRealSize - m_dwOffset;
	}

	swpa_memcpy(pvBuf, m_pbBuf + m_dwOffset, dwMin);	

	m_dwOffset += dwMin;
	
	if (NULL != pdwRead)
	{
		*pdwRead = dwMin;
	}
	
	return S_OK;
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
HRESULT CSWFileStream::Seek(INT iOffset, DWORD dwFromWhere, DWORD * pdwNewPos)
{

	CSWFILESTREAM_CHECK(m_flgInited);
	
	switch (dwFromWhere)
	{
		case SWPA_SEEK_SET:
		{
			if (iOffset < 0 || iOffset > m_dwRealSize)
			{
				CSWFILESTREAM_PRINT("%s() Err: iOffset = %d is invalid \n", __FUNCTION__, iOffset);
				return E_INVALIDARG;
			}

			m_dwOffset = iOffset;
		}
		break;

		case SWPA_SEEK_CUR:
		{
			if (m_dwOffset + iOffset < 0 || m_dwOffset + iOffset > m_dwRealSize)
			{
				CSWFILESTREAM_PRINT("%s() Err: iOffset = %d is invalid \n", __FUNCTION__, iOffset);
				return E_INVALIDARG;
			}

			m_dwOffset += iOffset;
		}
		break;

		case SWPA_SEEK_END:
		{
			if (m_dwRealSize + iOffset < 0 || m_dwRealSize + iOffset > m_dwRealSize)
			{
				CSWFILESTREAM_PRINT("%s() Err: iOffset = %d is invalid \n", __FUNCTION__, iOffset);
				return E_INVALIDARG;
			}

			m_dwOffset = m_dwRealSize + iOffset;
		}
		break;

		default:
		{
			CSWFILESTREAM_PRINT("%s() Err: dwFromWhere = %d is invalid \n", __FUNCTION__, dwFromWhere);
			return E_INVALIDARG;
		}
	}
	

	if (NULL != pdwNewPos)
	{		
		*pdwNewPos = m_dwOffset;
	}

	return S_OK;
	
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
HRESULT CSWFileStream::Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten)
{

	DWORD dwRead = 0;
	DWORD dwMin = 0;
	DWORD dwNewSize = dwCB + m_dwOffset;
	
	CSWFILESTREAM_CHECK(m_flgInited);
	
	CSWFILESTREAM_CHECK(NULL != pvBuf);
	CSWFILESTREAM_CHECK(0 != dwCB);

	
	if (dwNewSize > m_dwSize)
	{
		dwNewSize *= 2;
		PBYTE pbBufTemp = (PBYTE)swpa_mem_alloc(dwNewSize);
		if (NULL == pbBufTemp)
		{
			CSWFILESTREAM_PRINT("%s() Err: no enough memory for pbBufTemp \n", __FUNCTION__);
			return E_OUTOFMEMORY;
		}

		swpa_memset(pbBufTemp, 0, dwNewSize);

		if (NULL != m_pbBuf)
		{
			swpa_memcpy(pbBufTemp, m_pbBuf, m_dwSize);			
			swpa_mem_free(m_pbBuf);
			m_pbBuf = NULL;
		}
		
		m_pbBuf = pbBufTemp;		
		m_dwSize = dwNewSize;
	}
			
	/*dwMin = (dwCB > m_dwSize) ? m_dwSize : dwCB;

	if (m_dwOffset + dwMin > m_dwSize)
	{
		dwMin = m_dwSize - m_dwOffset;
	}*/

	swpa_memcpy(m_pbBuf + m_dwOffset, pvBuf, dwCB);	

	m_dwOffset += dwCB;
	m_dwRealSize = m_dwOffset;

	if (NULL != pdwWritten)
	{
		*pdwWritten = dwCB;
	}
	
	return S_OK;
}


/**
* @brief 获取文件流大小
*
* @return
* - 文件流大小
* 
*/
DWORD CSWFileStream::GetSize(VOID)
{
    if (m_flgInited)
    {
        return m_dwRealSize;
    }
    
    return 0;
}
	
/**
* @brief 获取文件流当前读写位置
*
* @return
* - 当前读写位置 
*/
DWORD CSWFileStream::GetCurrentPos(VOID)
{
    if (m_flgInited)
    {
        return m_dwOffset;
    }
    
    return 0;
}



