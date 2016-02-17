#ifndef __SW_BASE_DEVICE_H__
#define __SW_BASE_DEVICE_H__
#include "SWMatchTansformDataStruct.h"

class CSWBaseDevice : public CSWThread
{
	CLASSINFO(CSWBaseDevice, CSWThread)
public:
	enum DEVICE_TYPE{NONE, SPEED, TRIGGER, IMAGE, MAX_COUNT};
	CSWBaseDevice(DEVICEPARAM *pParam);
	virtual ~CSWBaseDevice();
	virtual HRESULT Initialize(VOID (*OnEvent)(PVOID pvParam, CSWBaseDevice* pDevice, DEVICE_TYPE type, DWORD dwTime, PDWORD pdwValue), PVOID pvParam, INT iCOMM);
	virtual HRESULT Close(VOID);
	virtual HRESULT Write(PVOID pBuffer, INT iSize, INT iTimeOut = 2000);
	virtual HRESULT Read(PVOID pBuffer, INT iSize, INT iTimeOut = 2000);
	INT GetCOMM(VOID);
protected:
	DEVICEPARAM *m_pDevParam;
	CSWFile m_cFile;	
	VOID (*m_pOnEvent)(PVOID pvParam, CSWBaseDevice* pDevice, DEVICE_TYPE type, DWORD dwTime, PDWORD pdwValue);
	PVOID m_pvParam;
	INT m_iCOMM;
	BOOL m_fRS485;
	CSWString m_strParam;
};
#endif

