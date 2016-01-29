#include "SWFC.h"
#include "SWBaseDevice.h"

CSWBaseDevice::CSWBaseDevice(DEVICEPARAM *pParam)
{
	m_pDevParam = pParam;
	m_pOnEvent = NULL;
	m_pvParam = NULL;
	m_iCOMM = 0;
	m_fRS485 = (pParam->iCommType != 0);
	m_strParam.Format("stop:1 parity:N databits:8 baudrate:%d", pParam->iBaudrate);
}

CSWBaseDevice::~CSWBaseDevice()
{
}

HRESULT CSWBaseDevice::Initialize(VOID (*OnEvent)(PVOID pvParam, CSWBaseDevice* pDevice, CSWBaseDevice::DEVICE_TYPE type, DWORD dwTime, PDWORD pdwValue), PVOID pvParam, INT iCOMM)
{
	CSWString strCOMM;
	m_iCOMM = iCOMM;
	strCOMM.Format("COMM/%d", iCOMM);
	HRESULT hr = E_FAIL;
	Close();
	if(S_OK == (hr = m_cFile.Open((LPCSTR)strCOMM, "r+")))
	{		
		m_cFile.IOCtrl(SWPA_FILE_IOCTL_COMM_SET_ATTR, (PVOID)(LPCSTR)m_strParam);
		m_cFile.IOCtrl(SWPA_FILE_IOCTL_COMM_IS_RS485, &m_fRS485);	
		INT iTimeOut = 1000;
		m_cFile.IOCtrl(SWPA_FILE_SET_READ_TIMEOUT, &iTimeOut);
		m_cFile.IOCtrl(SWPA_FILE_SET_WRITE_TIMEOUT, &iTimeOut);
		m_pOnEvent = OnEvent;
		m_pvParam = pvParam;		
	}
	SW_TRACE_NORMAL("open %s[%s] %s is485:%s.", (LPCSTR)strCOMM, (LPCSTR)m_strParam, S_OK == hr ? "ok" : "fail", m_fRS485 ? "yes" : "no");
	return hr;
}

HRESULT CSWBaseDevice::Close(VOID)
{
	m_cFile.Close();
	if(S_OK == IsValid())
	{
		Stop();
	}
	return S_OK;
}

HRESULT CSWBaseDevice::Write(PVOID pBuffer, INT iSize, INT iTimeOut)
{
	return m_cFile.Write(pBuffer, iSize);
}

HRESULT CSWBaseDevice::Read(PVOID pBuffer, INT iSize, INT iTimeOut)
{
	return m_cFile.Read(pBuffer, iSize);
}

INT CSWBaseDevice::GetCOMM(VOID)
{
	return m_iCOMM;
}