#include "hvfilestorage.h"

//S_OK : 读取成功, 通过读取字节数判断是否已到尾
//其他: 读取失败
HRESULT STDMETHODCALLTYPE CFileStorage::Read(
	PVOID pv,
	UINT cb,
	PUINT pcbRead
	)
{
	if ( !m_hFile ) return E_OBJ_NO_INIT;

	UINT uRead = (UINT)fread( pv, 1, cb, m_hFile );
	if( pcbRead != NULL )
	{
		*pcbRead = uRead;
	}

	return (uRead == 0)? E_FAIL : S_OK;
}

//S_OK : 写入成功
//其他: 写入失败
HRESULT STDMETHODCALLTYPE CFileStorage::Write(
	const void* pv,
	UINT cb,
	PUINT pcbWritten
	)
{
	if (!m_hFile) return E_OBJ_NO_INIT;

	UINT cbWrite = (UINT)fwrite( pv, 1, cb, m_hFile);

	if( pcbWritten != NULL )
	{
		*pcbWritten = cbWrite;
	}
	return S_OK;
}

//S_OK: 跳转成功
//其他:　跳转失败
HRESULT STDMETHODCALLTYPE CFileStorage::Seek(
	INT iOffset,
	HvCore::STREAM_SEEK ssOrigin,
	PUINT pnNewPosition
	)
{
	if (!m_hFile) return E_OBJ_NO_INIT;

	if( 0 == fseek( m_hFile, iOffset, (INT)ssOrigin ) )
	{
		long lpos = ftell( m_hFile );
		if( pnNewPosition != NULL )
		{
			*pnNewPosition = (UINT) lpos;
		}
		return S_OK;
	}
	return E_FAIL;
}

//S_OK: flush成功
//其他: flush失败
HRESULT STDMETHODCALLTYPE CFileStorage::Commit(
	DWORD32 grfCommitFlags
	)
{
	if (!m_hFile) return E_OBJ_NO_INIT;

	return S_OK;
}

CFileStorage::CFileStorage()
	:m_hFile(NULL)
{
}

CFileStorage::~CFileStorage()
{
	Close();
}

HRESULT CFileStorage::Close()
{
	if (m_hFile != NULL)
	{
		fclose( m_hFile );
		m_hFile = NULL;
	}

	return S_OK;
}

//S_OK : 初始化成功
//其他: 初始化失败
HRESULT STDMETHODCALLTYPE CFileStorage::Initialize(const char* szFileName, DWORD32 dwFlag /*=MODE_READWRITE*/)
{
	Close();

	DWORD32 dwMode = dwFlag & (~MODE_CREATE);

    //目的是为了创建文件
	FILE* pfile = fopen( szFileName, "a+" );
	if( pfile != NULL )
	{
		fclose( pfile );
	}

	switch( dwMode )
	{
	case MODE_READ:
		m_hFile = fopen( szFileName, "rb" );
		break;
	case MODE_WRITE:
		m_hFile = fopen( szFileName, "wb" );
		break;
	case MODE_READWRITE:
		m_hFile = fopen( szFileName, "r+b" );
		break;
	default:
		m_hFile = fopen( szFileName, "r+b" );
		break;
	}

	return (m_hFile == NULL)? E_FAIL : S_OK;
}
