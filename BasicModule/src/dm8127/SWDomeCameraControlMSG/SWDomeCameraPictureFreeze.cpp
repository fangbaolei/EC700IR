#include "SWDomeCameraPictureFreeze.h"

CSWDomeCameraPictureFreeze::CSWDomeCameraPictureFreeze()
{
}


CSWDomeCameraPictureFreeze::~CSWDomeCameraPictureFreeze()
{
}


HRESULT CSWDomeCameraPictureFreeze::Set(const PICTUREFREEZE_PARAM_STRUCT& sSetting)
{
	GetSetting().Get().sPictureFreezeParam.fEnable = sSetting.fEnable;

	return S_OK;
	//return SaveSetting();
}


HRESULT CSWDomeCameraPictureFreeze::Freeze(const BOOL& fFreeze)
{
	PICTUREFREEZE_PARAM_STRUCT sSetting;
	Get(sSetting);
	
	INT iMode = fFreeze ? 1 : 0;
	if (SWPAR_OK != swpa_camera_basicparam_set_freeze(iMode))
	{
		SW_TRACE_DEBUG("Err: failed to freeze picture. flag = %d\n", fFreeze);
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CSWDomeCameraPictureFreeze::Clear()
{	
	GetSetting().Get().sPictureFreezeParam.fEnable = FALSE;
	
	return SaveSetting();
}


HRESULT CSWDomeCameraPictureFreeze::Get(PICTUREFREEZE_PARAM_STRUCT& sSetting)
{
	sSetting.fEnable = GetSetting().Get().sPictureFreezeParam.fEnable;

	return S_OK;
}


HRESULT CSWDomeCameraPictureFreeze::SaveSetting()
{
	GetSetting().UpdateInt("\\DomeCamera\\PictureFreeze", "Enable", GetSetting().Get().sPictureFreezeParam.fEnable);

	return GetSetting().Commit();
}



