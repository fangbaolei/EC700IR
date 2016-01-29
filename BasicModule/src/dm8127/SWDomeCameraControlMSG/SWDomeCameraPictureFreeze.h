#ifndef _SWDOMECAMERAPICTUREFREEZE_H_
#define _SWDOMECAMERAPICTUREFREEZE_H_

#include "SWObject.h"
#include "SWDomeCameraBase.h"



class CSWDomeCameraPictureFreeze : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraPictureFreeze, CSWDomeCameraBase)
public:
    CSWDomeCameraPictureFreeze();
    virtual ~CSWDomeCameraPictureFreeze();
            
    HRESULT Set(const PICTUREFREEZE_PARAM_STRUCT& sSetting);
    
    HRESULT Clear();
    
    HRESULT Freeze(const BOOL& fFreeze);
    
    HRESULT Get(PICTUREFREEZE_PARAM_STRUCT& sSetting);

	HRESULT SaveSetting();

private:
	HRESULT CheckTime();
	static PVOID CheckTimeProxy(PVOID pvArg);
    
private:

	INT iTimer; //todo : need to implement the timer

};

#endif // _SWDOMECAMERAPICTUREFREEZE_H_

