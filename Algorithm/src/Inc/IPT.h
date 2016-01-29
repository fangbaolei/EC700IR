#ifndef __IPT_H__
#define __IPT_H__
#include "hvutils.h"
#include "hvthreadbase.h"
#include "OuterControl.h"
#include "IPTInterface.h"

class CIPT : public CHvThreadBase
{
public:
	         CIPT();
	virtual ~CIPT();
	bool Initialize(const BYTE8 * pbData, int size);
	bool IsValid(void){return m_bInitialize;}
	void UnInitialize(void);
protected:
    virtual const char* GetName()
    {
        static char szName[] =  "CIPT";
        return szName;
    }
	virtual HRESULT Run(void* pvParam);
private:
	bool m_bInitialize;
	// IPT用到的参数
    IComm *m_pComm;
    IIPTControl *m_pIPTControl;
    IIPTDevice *m_pIPTDevice;
    static const int MAX_DEVICEPARAM_LEN = 512;
    BYTE8 m_rgbDeviceParam[MAX_DEVICEPARAM_LEN];
    int m_iDeviceParamLen;
    DWORD32 m_dwLastSyncTime;
    DWORD32 m_dwIPTThreadIsOkTime;
};
#endif

