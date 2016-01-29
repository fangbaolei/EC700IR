#ifndef _SWDOMECAMERAPRESET_H_
#define _SWDOMECAMERAPRESET_H_

#include "SWObject.h"
#include "SWDomeCameraBase.h"
#include "SWDomeCameraPictureFreeze.h"

enum {									//功能预置位
	FUNC_ZERO_CORRECTION 	= 0,		//0度矫正
	FUNC_DEFOG_START 		= 109,		//除雾开始
	//FUNC_DEFOG_STOP 		= 110		//除雾停止
};			

class CSWDomeCameraPreset : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraPreset, CSWDomeCameraBase)
public:
    CSWDomeCameraPreset();
    virtual ~CSWDomeCameraPreset();
            
    HRESULT Set(const DWORD& dwID, const PRESETPOS_PARAM_STRUCT& sParam);
    
    HRESULT Clear(const DWORD& dwID);
    
    HRESULT Call(const DWORD& dwID);

	HRESULT Stop(const DWORD& dwID);
    
    HRESULT Get(const DWORD& dwID, PRESETPOS_PARAM_STRUCT& sParam);

	HRESULT SetPictureFreezeInstance(CSWDomeCameraPictureFreeze* pPictureFreeze){m_pPictureFreeze = pPictureFreeze;};
	
	HRESULT GotoPos(
		const INT& iPanCoord, const INT& iTiltCoord, const INT& iZoomValue, 
		const INT& iFocusMode, const INT& iFocusValue);

protected:
	HRESULT SaveSetting(const DWORD& dwID);

	
	CSWDomeCameraPictureFreeze& GetPictureFreezeInstance(){return *m_pPictureFreeze;};

    
private:

	DWORD m_dwCallingID;
	
	CSWDomeCameraPictureFreeze* m_pPictureFreeze;

};

#endif // _SWDOMECAMERAPRESET_H_

