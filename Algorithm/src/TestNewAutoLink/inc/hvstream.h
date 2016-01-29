#ifndef _HVSTREAM_INCLUDED__
#define _HVSTREAM_INCLUDED__

#include "HvSecurity.h"
#include "swBaseType.h"
#include "swWinError.h"

#if (RUN_PLATFORM == PLATFORM_WINDOWS)
#define HVMETHODCALLTYPE __stdcall
#define LPTSTR TCHAR*
#define LPCTSTR const TCHAR*
#else
#define HVMETHODCALLTYPE
#define LPTSTR char*
#define LPCTSTR const char*
#define _stricmp strcmp
#endif

#define HVRESULT HRESULT

#define HR_S_OK S_OK

#define HR_E_NOTIMPL E_NOTIMPL

#ifndef RTN_HR_IF_FAILED
#define RTN_HR_IF_FAILED(fun) { hr = fun;if(FAILED(hr)) return hr;}
#endif

#define RTN_HR_IF_BADPTR(p) { if (p == NULL) return E_POINTER; }

#define HVMETHOD(method) virtual HRESULT HVMETHODCALLTYPE method

typedef const void* LPCVOID;

typedef const char* LPCSTR;

#define HR_E_INVALIDARG E_INVALIDARG

#define HR_S_FALSE S_FALSE

#define HR_E_FAIL E_FAIL

#define HR_E_OUTOFMEMORY E_OUTOFMEMORY

#define HR_SUCCEEDED SUCCEEDED

#define RTN_HR_IF_BADNEW(p) {if (p == NULL) return E_OUTOFMEMORY;}

#ifndef min
#define min(a,b) (a<b?a:b)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p!=NULL) {p->Release() ; p = NULL;}
#endif

namespace HiVideo
{

#if (RUN_PLATFORM == PLATFORM_WINDOWS)

const UINT g_kcCopyBuf=2048;

#else

const UINT g_kcCopyBuf=64;

#endif

// 用于流的读写
class ISequentialStream
{
public:
	HVMETHOD(Read)( PVOID pv, UINT cb, PUINT pcbRead) = 0;
	HVMETHOD(Write)( LPCVOID pv, UINT cb, PUINT pcbWritten) = 0;
};

class IPersist
{
public:
	HVMETHOD(Load)(ISequentialStream *pStream) = 0;
	HVMETHOD(Save)(ISequentialStream *pStream) = 0;
	HVMETHOD(GetSize)(PUINT pcb) = 0;
};

typedef enum tagSTREAM_SEEK
{
	STREAM_SEEK_SET = 0, 
	STREAM_SEEK_CUR = 1, 
	STREAM_SEEK_END = 2
} STREAM_SEEK;

// 能通过ISequentialStream完成的事情，最好不要用此流
class IStream : public ISequentialStream
{
public:
	HVMETHOD(Seek)( 
		INT dlibMove,
		STREAM_SEEK ssOrigin,
		PUINT pnNewPosition
	) = 0;

	HVMETHOD(SetSize)( UINT nNewSize ) = 0;

	// 为了节省内存复制的额外开销而设计
	HVMETHOD(CopyTo)( 
		IStream *pstm,
		UINT cb,
		PUINT pcbRead,
		PUINT pcbWritten
	) = 0;
	
	HVMETHOD(GetVerifyInfo)(DWORD32 *dwCrc, DWORD32 *dwSize) = 0;
};

class IEncryptStream : public IStream
{
public:
	HVMETHOD(Initialize)(
		IStream* pIOStream,
		BOOL fWrite,
		const DWORD32* pKey,
		const RSA_KEY* pRsaKey,
		DWORD32 nEncryptBufSize = 2048,
		DWORD32 dwFlags = 0x121
	) = 0;

	HVMETHOD(Initialize)(
		IStream* pIOStream,
		BOOL fWrite,
		const DWORD32* pKey,
		const RSA_PRIVATE_KEY* pRsaKey,
		DWORD32 nEncryptBufSize = 2048,
		DWORD32 dwFlags = 0x121
	) = 0;

	HVMETHOD(Release)() = 0;

	HVMETHOD(Close)(void) = 0;

	HVMETHOD(GetVerifyInfo)(DWORD32* pCrc, DWORD32* pSize) = 0;
};

class IFileStream : public IStream
{
public:
	// 统一使用文件名接口，DSP下的文件名就是数字
	HVMETHOD(Initialize)(
		const char* szFilePath,
		BOOL fWrite) = 0;
	HVMETHOD(Close)(void) = 0;

	HVMETHOD(GetVerifyInfo)(DWORD32* pCrc,DWORD32* pSize) = 0;

	HVMETHOD(Release)() = 0;
};

class IMemoryStream : public IStream
{
public:
	HVMETHOD(Initialize)(
		DWORD32 nAllocSize
	) = 0;

	HVMETHOD(Initialize)(
		BYTE8* pbBuf,
		DWORD32 dwLen
	) = 0;

	HVMETHOD(Close)() = 0;
	
	HVMETHOD(GetVerifyInfo)(DWORD32* pCrc,DWORD32* pSize) = 0;

	HVMETHOD(GetBuf)(
		BYTE8** ppBuf,
		DWORD32* pSize
	) = 0;
	
	HVMETHOD(Release)() = 0;
};

extern HVRESULT CreateEncryptStream(IEncryptStream **ppEncryptStream);
extern HVRESULT CreateFileStream(IFileStream **ppFileStream);
extern HVRESULT CreateMemoryStream(IMemoryStream **ppMemoryStream);
}

#endif // _HIVIDEOINTERFACE_INCLUDED__
