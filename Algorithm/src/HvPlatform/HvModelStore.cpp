#include "hvmodelstore.h"
#include "hvutils.h"

#include "modeldatinfo.h"

using namespace HvCore;

CHvModelStore::CHvModelStore()
 :m_dwCookie(0)
{
	return;
}

CHvModelStore::~CHvModelStore()
{
	return;
}

HRESULT STDMETHODCALLTYPE	CHvModelStore::GetModelList(
		MODEL_TYPE nType,
		LPCSTR* rgName,
		DWORD32* pdwCount
		)
{
	HRESULT hr;

	switch( nType )
	{
	case MODEL_TYPE_DET:
		hr = GetDetModelList( rgName, pdwCount );
		break;
	case MODEL_TYPE_RECOG:
		hr  = GetRecogModelList( rgName, pdwCount );
		break;
	default:
		hr = E_FAIL;
		break;
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE	CHvModelStore::LoadModelDat(
		MODEL_TYPE nType,
		LPCSTR szName,
		void* pModelDat,
		DWORD32* pdwDatLen,
		DWORD32 dwFlag
		)
{
	if ( pModelDat == NULL || pdwDatLen == NULL ) return E_POINTER;

	HRESULT hr;

	switch( nType )
	{
	case MODEL_TYPE_DET:
		hr = LoadDetDat( szName, pModelDat, pdwDatLen , dwFlag);
		break;
	case MODEL_TYPE_RECOG:
		hr  = LoadRecogDat( szName, pModelDat, pdwDatLen, dwFlag);
		break;
	default:
		hr = E_FAIL;
		break;
	}

	return hr;
}
