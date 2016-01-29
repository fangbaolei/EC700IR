#ifndef _HV_MODEL_STORE_H
#define _HV_MODEL_STORE_H

#include "hvinterface.h"

#include "hvutils.h"

class CHvModelStore : public HvCore::IHvModel
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
			iid == HvCore::IID_IHvModel )
		{
			*ppv = static_cast<HvCore::IHvModel*>(this);
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

	//IHvModel
	STDMETHOD(GetModelList)(
		MODEL_TYPE nType,
		LPCSTR* rgName,
		DWORD32* pdwCount
		);

	STDMETHOD(LoadModelDat)(
		MODEL_TYPE nType,
		LPCSTR szName,
		void* pDat,
		DWORD32* pdwDatLen,
		DWORD32 dwFlag = 0
		);

public:
	CHvModelStore();
	~CHvModelStore();

	DWORD32 m_dwCookie;
};

#endif

