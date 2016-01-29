#ifndef _SWDOMECAMERAMASK_H_
#define _SWDOMECAMERAMASK_H_

#include "SWObject.h"
#include "SWDomeCameraBase.h"



class CSWDomeCameraMask : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraMask, CSWDomeCameraBase)
public:
    CSWDomeCameraMask();
    virtual ~CSWDomeCameraMask();
	
	HRESULT Initialize(CSWDomeCameraSetting * pcSetting);
            
    HRESULT Set(const DWORD& dwID, const MASK_PARAM_STRUCT& sSetting);
    
    HRESULT Clear(const DWORD& dwID);
    
    HRESULT Show(const DWORD& dwID);

    HRESULT Hide(const DWORD& dwID);
    
    HRESULT Get(const DWORD& dwID, MASK_PARAM_STRUCT& sSetting);

	HRESULT SaveSetting(const DWORD& dwID);
    
private:
	static PVOID InitMasks(PVOID pvArg);
		
	CSWThread m_cInitThread;
};

#endif // _SWDOMECAMERAMASK_H_

