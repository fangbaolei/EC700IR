#ifndef _HV_INTERFACE_
#define _HV_INTERFACE_

#include "hvcoretype.h"

//接口定义
namespace HvCore
{
	//接口ID
	extern const IID IID_IUnknown ;
	extern const IID IID_IClassFactory ;
	extern const IID IID_ICoreDebuger ;
	extern const IID IID_IHvParam;
	extern const IID IID_IHvParam2;
	extern const IID IID_IHvStream;
	extern const IID IID_IHvModel ;
	extern const IID IID_IHvPerformance ;

	//类ID
	extern const CLSID CLSID_HvCore;

	interface IUnknown
	{
		STDMETHOD(QueryInterface)(
			const IID& iid,
			void** ppv
			) = 0;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
		virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
	};

	interface IClassFactory : IUnknown
	{
		STDMETHOD(CreateInstance)(
			IUnknown* pUnknownOuter,
			const IID& iid,
			void** ppv
			) = 0;

		STDMETHOD(LockServer)(
			BOOL bLock
			) = 0;
	};

	interface IHvParam : IUnknown
	{
		STDMETHOD(GetInt)(
			const char* szSection,
			const char* szKey,
			int* pVal,
			int iDefault
			) = 0;

		STDMETHOD(GetFloat)(
			const char* szSection,
			const char* szKey,
			float* pVal,
			float fltDefault
			) = 0;

		STDMETHOD(GetString)(
			const char* szSection,
			const char* szKey,
			char* szRet,
			int nLen
			) = 0;

		STDMETHOD(GetBin)(
			const char* szSection,
			const char* szKey,
			void* pBuf,
			int* pLen
			) = 0;

		STDMETHOD(SetInt)(
			const char* szSection,
			const char* szKey,
			int iVal
			) = 0;

		STDMETHOD(SetFloat)(
			const char* szSection,
			const char* szKey,
			float fltVal
			) = 0;

		STDMETHOD(SetString)(
			const char* szSection,
			const char* szKey,
			const char* szSrc
			) = 0;

		STDMETHOD(SetBin)(
			const char* szSection,
			const char* szKey,
			const void* pBuf,
			int nLen
			) = 0;

		STDMETHOD(Save)() = 0;
	};

	typedef enum tagPARAM_SAVE_MODE
	{
		PSM_NORMAL = 0,	    //正常模式, 存储附加信息
		PSM_SIMPLE = 1		//简单模式, 不存储附加信息
	}
	PARAM_SAVE_MODE;

	interface IHvParam2 : IUnknown
	{
		STDMETHOD(GetInt)(
			const char* szSection,
			const char* szKey,
			int* pVal,
			int nDefault,
			int nMin,
			int nMax,
			const char* szChName,
			const char* szComment,
			BYTE8	nRank
			) = 0;

		STDMETHOD(GetFloat)(
			const char* szSection,
			const char* szKey,
			float* pVal,
			float fltDefault,
			float fltMin,
			float fltMax,
			const char* szChName,
			const char* szComment,
			BYTE8	nRank
			) = 0;

		STDMETHOD(GetString)(
			const char* szSection,
			const char* szKey,
			char* szRet,
			int nLen,
			const char* szChName,
			const char* szComment,
			BYTE8 nRank
			) = 0;

		STDMETHOD(GetBin)(
			const char* szSection,
			const char* szKey,
			void* pBuf,
			int* pLen,
			const char* szChName,
			const char* szComment,
			BYTE8 nRank
			) = 0;

		STDMETHOD(SetInt)(
			const char* szSection,
			const char* szKey,
			int iVal
			) = 0;

		STDMETHOD(SetFloat)(
			const char* szSection,
			const char* szKey,
			float fltVal
			) = 0;

		STDMETHOD(SetString)(
			const char* szSection,
			const char* szKey,
			const char* szSrc
			) = 0;

		STDMETHOD(SetBin)(
			const char* szSection,
			const char* szKey,
			const void* pBuf,
			int nLen
			) = 0;

		STDMETHOD(Save)(
			DWORD32 dwFlag
			) = 0;
	};

	typedef enum tagSTREAM_SEEK
	{
		STREAM_SEEK_SET = 0,
		STREAM_SEEK_CUR = 1,
		STREAM_SEEK_END = 2
	}
	STREAM_SEEK;

	interface IHvStream : IUnknown //流接口
	{
		STDMETHOD(Read)(
			PVOID pv,
			UINT cb,
			PUINT pcbRead
			) = 0;

		STDMETHOD(Write)(
			const void* pv,
			UINT cb,
			PUINT pcbWritten
			) = 0;

		STDMETHOD(Seek)(
			INT iOffset,
			STREAM_SEEK ssOrigin,
			PUINT pnNewPosition
			) = 0;

		STDMETHOD(Commit)(
			DWORD32 grfCommitFlags = 0
			) = 0;

        STDMETHOD(UDPRead)(
            BYTE8 *pBuffer,
            int iBufferLen,
            DWORD32 *pRemoteAddr,
            WORD16 *pRemotePort,
            int iTimeout
            ) = 0;

        STDMETHOD(UDPSend)(
            BYTE8 *pBuffer,
            int iBufferLen,
            DWORD32 dwRemoteAddr,
            WORD16 wRemotePort
            ) = 0;
	};

	interface IHvModel : IUnknown //模型加载接口
	{
		STDMETHOD(GetModelList)(
			MODEL_TYPE nType,
			LPCSTR* rgName,
			DWORD32* pdwCount
			) = 0;

		STDMETHOD(LoadModelDat)(
			MODEL_TYPE nType,
			LPCSTR szName,
			void* pDat,
			DWORD32* pdwDatLen,
			DWORD32 dwFlag = 0
			) = 0;
	};

	typedef enum _IMG_TYPE
	{
		IMG_COMP = 0,
		IMG_GRAY = 1,
		IMG_RGB = 2,
		IMG_BIN = 3
	}
	IMG_TYPE;

	typedef struct _IMG_INFO
	{
		DWORD32 dwFlag;
		UINT nWidth;
		UINT nHeight;
		UINT nStride;
	}
	IMG_INFO;

	typedef struct _BIN_INFO
	{
		DWORD32 dwFlag;
		UINT nLen;
	}
	BIN_INFO;

	interface IHvPerformance : IUnknown
	{
		STDMETHOD(BeginSession)(	//开始一个输出进程
			LPCSTR szName
			) = 0;

		STDMETHOD(EndSession)( //关闭最近一个输出进程
			LPCSTR szName
			) = 0;

		STDMETHOD(StartTimer)(
			LPCSTR szName
			) = 0;

		STDMETHOD(StopTimer)(
			LPCSTR szName
			) = 0;

		STDMETHOD(OutputText)(
			LPCSTR szName,
			LPCSTR szText
			) = 0;

		STDMETHOD(OutputImg)(
			LPCSTR szName,
			DWORD32 dwFlag,	//最后一字节用于指定图像类型
			PVOID pImg,
			UINT nWidth,
			UINT nHeight,
			UINT nStride
			) = 0;

		STDMETHOD(OutputBinary)(
			LPCSTR szName,
			DWORD32 dwFlag,
			PVOID pDat,
			UINT nLen
			) = 0;
	};

	extern "C" HRESULT CreateHvObj(
		CLSID clsid,
		IUnknown* pUnkOuter,
		const IID& iid,
		PVOID* ppvObj
		);

}

#endif //_HV_INTERFACE_
