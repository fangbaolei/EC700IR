#ifndef _HV_PERFSTORE_
#define _HV_PERFSTORE_

#include "HvInterface.h"
#include "ObjBase.h"
#include "HvEnum.h"

#include "HvUtils.h"

typedef struct _TIMER_INFO
{
	char szName[32];
	int iTimeCount;

	_TIMER_INFO()
	{
		HV_memset(szName, 0, 32);
		iTimeCount = -1;
	}
}
TIMER_INFO;

typedef struct _TEST_SESSION
{
	static const int MAX_TIMER = 32;

	TIMER_INFO rgTimer[MAX_TIMER];
	IStorage* pStorage;

	char szName[32];

	_TEST_SESSION()
	{
		HV_memset(szName, 0, 32);
		pStorage = NULL;
	}
}
TEST_SESSION;

class CHvPerfStore : public HvCore::IHvPerformance
{
public:
	// IUnknown
	STDMETHOD(QueryInterface)( 
		const IID& iid, 
		void** ppv
		)
	{
		HRESULT hr = S_OK;
		if ( iid == HvCore::IID_IUnknown ||
			iid == HvCore::IID_IHvPerformance )
		{
			*ppv = static_cast<IHvPerformance*>(this); 
			AddRef();
		}
		else
		{
			*ppv = NULL;
			hr = E_NOINTERFACE;
		}
		return hr;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		return 0;
	}

	//IHvPerformance
	STDMETHOD(BeginSession)(		//开始一个测试会话
		LPCSTR szName
		);

	STDMETHOD(EndSession)(
		LPCSTR szName
		);

	STDMETHOD(StartTimer)(
		LPCSTR szName
		);

	STDMETHOD(StopTimer)(
		LPCSTR szName
		);
	
	STDMETHOD(OutputText)(
		LPCSTR szName,
		LPCSTR szText
		);

	STDMETHOD(OutputImg)(
		LPCSTR szName,
		DWORD32 dwFlag,	//最后一字节用于指定图像类型
		PVOID pImg,
		UINT nWidth,
		UINT nHeight,
		UINT nStride
		);

	STDMETHOD(OutputBinary)(
		LPCSTR szName,
		DWORD32 dwFlag,
		PVOID pDat,
		UINT nLen
		);

public:
	CHvPerfStore();
	~CHvPerfStore();

	HRESULT Initialize(LPCSTR szFilePath);

	DWORD32 m_dwCookie;

private:
	CSimpStack<TEST_SESSION*> m_cSessionStack;
};

#endif