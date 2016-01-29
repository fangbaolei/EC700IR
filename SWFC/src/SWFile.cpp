/**
* @file SWFile.cpp
* @brief CWFile实现
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-20
* @version 1.0
*/


#include "SWFile.h"
#include "SWLog.h"

#include "swpa.h"


#define CSWFILE_PRINT SW_TRACE_DEBUG

#if 0
#define CSWFILE_CHECK(arg)	\
if (!(arg)) 																	\
{ 																				\
	CSWFILE_PRINT("%s: ", __FUNCTION__);	\
	CSWFILE_PRINT("Check %s FAILED! [%d]\n", #arg, (INT)E_INVALIDARG);	\
	return E_INVALIDARG;													\
}
#else
#define CSWFILE_CHECK(arg)
#endif



/**
 * @brief 构造函数
 */
CSWFile::CSWFile(){

	m_iFileHandle = 0;

	
}


/**
 * @brief 构造函数
 * @param [in] szFileName : 被打开的文件名，包含设备文件和一般的磁盘文件
 * @param [in] szMode : 文件打开模式
 */
CSWFile::CSWFile(PCSTR szFileName, PCSTR szMode){

	m_iFileHandle = 0;
	
	if (NULL == szFileName)
	{
		CSWFILE_PRINT("Err: %s() szFileName is NULL\n", __FUNCTION__);
		return ;
	}
	if (NULL == szMode)
	{
		CSWFILE_PRINT("Err: %s() szMode is NULL\n", __FUNCTION__);
		return ;
	}

	
	//CSWFILE_PRINT("szFileName = %s\n", szFileName);
	//CSWFILE_PRINT("szMode = %s\n", szMode);

	
	m_iFileHandle = swpa_file_open(szFileName, szMode);
	if (!IsValid(m_iFileHandle))
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_open(%s, %s) failed! (%d)\n", 
			__FUNCTION__, szFileName, szMode, m_iFileHandle);
		m_iFileHandle = 0;
		return ;
	}	
}


/**
 * @brief 析构函数
 */
CSWFile::~CSWFile(){

	if (IsValid(m_iFileHandle))
	{
		swpa_file_close(m_iFileHandle);
		m_iFileHandle = 0;
	}	
}




/**
 * @brief 打开一个文件
 *
 * @param [in] szFileName : 文件名
 * @param [in] szMode : 打开模式
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
*/
HRESULT CSWFile::Open(PCSTR szFileName, PCSTR szMode){

	CSWFILE_CHECK(NULL != szFileName);	
	CSWFILE_CHECK(NULL != szMode);

	if (IsValid(m_iFileHandle))
	{
		if (S_OK != Close())
		{
			CSWFILE_PRINT("%s() Err: failed to close previous file!\n", __FUNCTION__);
			return E_FAIL;
		}
	}

	
	//CSWFILE_PRINT("szFileName = %s\n", szFileName);
	//CSWFILE_PRINT("szMode = %s\n", szMode);

	
	m_iFileHandle = swpa_file_open(szFileName, szMode);
	if (!IsValid(m_iFileHandle) )
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_open(%s, %s) failed! (%d)\n", 
			__FUNCTION__, szFileName, szMode, m_iFileHandle);
		m_iFileHandle = 0;

		return E_FAIL;
	}	
  
	return S_OK;
}


/**
 * @brief 关闭文件
 *
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
*/
HRESULT CSWFile::Close(void){

  	if (IsValid(m_iFileHandle))
	{
		INT iRet = swpa_file_close(m_iFileHandle);
		if (SWPAR_OK != iRet)
		{
			CSWFILE_PRINT("%s() Err: swpa_file_close() failed!\n", __FUNCTION__);
			switch (iRet)
			{
				case SWPAR_OK : return S_OK;
				case SWPAR_FAIL : return E_FAIL;
				case SWPAR_INVALIDARG : return E_INVALIDARG;
				case SWPAR_NOTIMPL : return E_NOTIMPL;
				case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
				case SWPAR_NOTINITED : return E_NOTIMPL;
				case SWPAR_OUTOFTIME : return E_OUTOFTIME;
			}
		}
		
		m_iFileHandle = 0;
	}

	return S_OK;
}



/**
 * @brief 获得文件的大小
 *
 * @param [out] pdwSize : 文件的大小
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWFile::GetSize(DWORD * pdwSize){

	INT iRet = 0;
	INT	iSize = 0;
	INT	iOffset = 0;

	CSWFILE_CHECK(IsValid(m_iFileHandle));
	
	CSWFILE_CHECK(NULL != pdwSize);	

	iOffset = swpa_file_tell(m_iFileHandle);
	if (0 > iOffset)
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_tell() failed! (%d)\n", __FUNCTION__, iRet);
		*pdwSize = 0;
		return E_FAIL;
	}

	iRet = swpa_file_seek(m_iFileHandle, 0, SWPA_SEEK_END);
	if (SWPAR_OK != iRet)
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_seek() failed! (%d)\n", __FUNCTION__, iRet);
		*pdwSize = 0;
		switch (iRet)
		{
			case SWPAR_OK : return S_OK;
			case SWPAR_FAIL : return E_FAIL;
			case SWPAR_INVALIDARG : return E_INVALIDARG;
			case SWPAR_NOTIMPL : return E_NOTIMPL;
			case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
			case SWPAR_NOTINITED : return E_NOTIMPL;
			case SWPAR_OUTOFTIME : return E_OUTOFTIME;
		}
	}

	iSize = swpa_file_tell(m_iFileHandle);
	if (0 > iSize)
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_tell() failed! (%d)\n", __FUNCTION__, iRet);
		*pdwSize = 0;
		return E_FAIL;
	}

	*pdwSize = (DWORD)iSize;
	
	iRet = swpa_file_seek(m_iFileHandle, iOffset, SWPA_SEEK_SET);
	if (SWPAR_OK != iRet)
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_seek() failed! (%d)\n", __FUNCTION__, iRet);
		*pdwSize = 0;
		switch (iRet)
		{
			case SWPAR_OK : return S_OK;
			case SWPAR_FAIL : return E_FAIL;
			case SWPAR_INVALIDARG : return E_INVALIDARG;
			case SWPAR_NOTIMPL : return E_NOTIMPL;
			case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
			case SWPAR_NOTINITED : return E_NOTIMPL;
			case SWPAR_OUTOFTIME : return E_OUTOFTIME;
		}
	}
	
	return S_OK;
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
HRESULT CSWFile::IOCtrl(DWORD dwCmd, VOID * pvArg){

	INT iRet = 0;

	CSWFILE_CHECK(IsValid(m_iFileHandle));

	iRet = swpa_file_ioctl(m_iFileHandle, (int)dwCmd, pvArg);
	if (SWPAR_OK != iRet)
	{
		SW_TRACE_DEBUG("Err: %s() call swpa_file_ioctl() failed! (%d)\n", __FUNCTION__, iRet);
		switch (iRet)
		{
			case SWPAR_OK : return S_OK;
			case SWPAR_FAIL : return E_FAIL;
			case SWPAR_INVALIDARG : return E_INVALIDARG;
			case SWPAR_NOTIMPL : return E_NOTIMPL;
			case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
			case SWPAR_NOTINITED : return E_NOTIMPL;
			case SWPAR_OUTOFTIME : return E_OUTOFTIME;
		}
	}

	return S_OK;
}

/**
 * @brief 文件读取函数
 *
 * @param [in] pvBuf : 存放读取结果的缓冲区
 * @param [in] dwCB : 存放读取结果的缓冲区的大小
 * @param [out] pdwRead : 实际读取到的字节数
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWFile::Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead){

	INT iRet = 0;
	INT iRead = 0;
	
	
	CSWFILE_CHECK(IsValid(m_iFileHandle));
	
	CSWFILE_CHECK(NULL != pvBuf);	
	CSWFILE_CHECK(0 < dwCB);

	iRet = swpa_file_read(m_iFileHandle, pvBuf, dwCB, &iRead);	
	if (SWPAR_OK != iRet)
	{
		//CSWFILE_PRINT("Err: %s() call swpa_file_read() failed! (%d)\n", __FUNCTION__, iRet);

		switch (iRet)
		{
			case SWPAR_OK : return S_OK;
			case SWPAR_FAIL : return E_FAIL;
			case SWPAR_INVALIDARG : return E_INVALIDARG;
			case SWPAR_NOTIMPL : return E_NOTIMPL;
			case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
			case SWPAR_NOTINITED : return E_NOTIMPL;
			case SWPAR_OUTOFTIME : return E_OUTOFTIME;
		}
	}

	if (NULL != pdwRead)
	{
		*pdwRead = iRead;
	}
	
	return S_OK;
}


/**
 * @brief 重定位文件内部位置指针
 *
 * @param [in] sdwOffset : 偏移量
 * @param [in] dwFromWhere : 相对起始位置，0:文件头(SEEK_SET)，1:当前位置(SEEK_CUR)，2:
 * 文件尾(SEEK_END)
 * @param [out] pdwNewPos : 新位置的内部文件指针值
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWFile::Seek(INT iOffset, DWORD dwFromWhere, DWORD * pdwNewPos){

	INT iRet = 0;
	INT iPos = 0;

	
	CSWFILE_CHECK(IsValid(m_iFileHandle));

	iRet = swpa_file_seek(m_iFileHandle, iOffset, dwFromWhere);
	if (SWPAR_OK != iRet)
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_seek() failed! (%d)\n", __FUNCTION__, iRet);
		switch (iRet)
		{
			case SWPAR_OK : return S_OK;
			case SWPAR_FAIL : return E_FAIL;
			case SWPAR_INVALIDARG : return E_INVALIDARG;
			case SWPAR_NOTIMPL : return E_NOTIMPL;
			case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
			case SWPAR_NOTINITED : return E_NOTIMPL;
			case SWPAR_OUTOFTIME : return E_OUTOFTIME;
		}
	}
	

	if (NULL != pdwNewPos)
	{
		iPos = swpa_file_tell(m_iFileHandle);
		if (0 > iPos)
		{
			CSWFILE_PRINT("Err: %s() call swpa_file_tell() failed! (%d)\n", __FUNCTION__, iRet);
			return E_FAIL;
		}
		
		*pdwNewPos = (DWORD)iPos;
	}
	
	
	return S_OK;
}


/**
 * @brief 文件写入函数
 *
 * @param [in] pvBuf : 存放要写入数据的的缓冲区
 * @param [in] dwCB : 写入数据的的大小
 * @param [out] pdwWritten : 实际写入的字节数
 * @return
 * - S_OK : 成功
 * _ E_FAIL : 失败
 */
HRESULT CSWFile::Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten){

	INT iRet = 0;
	INT iWritten = 0;

	
	CSWFILE_CHECK(IsValid(m_iFileHandle));

	CSWFILE_CHECK(NULL != pvBuf);	
	CSWFILE_CHECK(0 < dwCB);

	iRet = swpa_file_write(m_iFileHandle, pvBuf, dwCB, &iWritten);	
	if (SWPAR_OK != iRet)
	{
		CSWFILE_PRINT("Err: %s() call swpa_file_write() failed! (%d)\n", __FUNCTION__, iRet);
		switch (iRet)
		{
			case SWPAR_OK : return S_OK;
			case SWPAR_FAIL : return E_FAIL;
			case SWPAR_INVALIDARG : return E_INVALIDARG;
			case SWPAR_NOTIMPL : return E_NOTIMPL;
			case SWPAR_OUTOFMEMORY : return E_OUTOFMEMORY;
			case SWPAR_NOTINITED : return E_NOTIMPL;
			case SWPAR_OUTOFTIME : return E_OUTOFTIME;
		}
	}

	if (NULL != pdwWritten)
	{
		*pdwWritten = iWritten;
	}
	
	return S_OK;
}


BOOL CSWFile::IsValid(INT iFileHandle)
{
	if (0 >= iFileHandle && SWPAR_MIN < iFileHandle )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

