#include "IPTDeviceImpl.h"
#include "IPTControlImpl.h"

HRESULT CreateIPTDevice(IIPTDevice** ppIPTDevice, IIPTControl *pIControl)
{
    if (ppIPTDevice == NULL)
    {
        return E_POINTER;
    }

    CIPTDeviceImpl *pIPTDeviceImpl = new CIPTDeviceImpl(pIControl);

    if (pIPTDeviceImpl == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // final assignment
    *ppIPTDevice = pIPTDeviceImpl;

    return S_OK;
}

CIPTDeviceImpl::CIPTDeviceImpl(IIPTControl *pIControl)
        : m_pIControl(NULL)
{
    m_pIControl = pIControl;
}

CIPTDeviceImpl::~CIPTDeviceImpl(void)
{
}

HRESULT CIPTDeviceImpl::ReadEvent(
    PBYTE8 pbEventId,
    PBYTE8 pbType,
    PBYTE8 pbEventDeviceType,
    DWORD32* pdwTime,
    PBYTE8 pbData,
    PDWORD32 pdwSize
)
{
    if ( m_pIControl == NULL
            || pbEventId == NULL
            ||  pbType == NULL
            || pdwTime == NULL
            || pbData == NULL
            || pdwSize == NULL
            || pbEventDeviceType == NULL)
    {
        return E_POINTER;
    }

	if(S_OK != ((CIPTControlImpl*)m_pIControl)->Lock())
	{
		return E_FAIL;
	}

    HRESULT hr = m_pIControl->SendCommand(CMD_READ_DEVICE_EVENT, NULL, 0);
    if ( hr == S_OK )
    {
        while (true)
        {
            BYTE8 bCmdId = 0;
            BYTE8 rgbData[MAX_DATA_COUNT] = {0};
            DWORD32 dwLen = MAX_DATA_COUNT;

            hr = m_pIControl->RecvData(&bCmdId, rgbData, &dwLen, 80);

            if ( hr == S_OK && bCmdId != CMD_READ_DEVICE_EVENT )
            {
                continue;
            }

            if ( hr == S_OK
                    && bCmdId == CMD_READ_DEVICE_EVENT
                    && dwLen > 8
                    && rgbData[0] == FALG_OK )
            {
                *pbEventId = rgbData[1];
                *pbType = rgbData[2];
                *pbEventDeviceType = rgbData[3];
                memcpy(pdwTime, &rgbData[4], 4);
                *pdwSize = ((dwLen - 8) > *pdwSize) ? *pdwSize : (dwLen - 8);
                memcpy( pbData, &rgbData[8], *pdwSize );
                break;
            }
            else
            {
                hr = E_FAIL;
                break;
            }
        }
    }

	((CIPTControlImpl*)m_pIControl)->UnLock();

    return hr;
}

HRESULT CIPTDeviceImpl::ReadEventEx(
    BYTE8* pbInData,
    DWORD32 dwInLen,
    BYTE8* pbEventId,
    BYTE8* pbType,
    PBYTE8 pbEventDeviceType,
    DWORD32* pdwTime,
    PBYTE8 pbData,
    PDWORD32 pdwSize
)
{
    if ( m_pIControl == NULL
            || pbEventId == NULL ||  pbType == NULL || pdwTime == NULL || pbData == NULL || pdwSize == NULL
            || pbEventDeviceType == NULL || pbInData == NULL || dwInLen == 0 )
    {
        return E_POINTER;
    }

	if(S_OK != ((CIPTControlImpl*)m_pIControl)->Lock())
	{
		return E_FAIL;
	}

    HRESULT hr = m_pIControl->SendCommand(CMD_READ_DEVICE_EVENT_EX, pbInData, dwInLen);
    if ( hr == S_OK )
    {
        while (true)
        {
            BYTE8 bCmdId = 0;
            BYTE8 rgbData[MAX_DATA_COUNT] = {0};
            DWORD32 dwLen = MAX_DATA_COUNT;

            hr = m_pIControl->RecvData(&bCmdId, rgbData, &dwLen, 80);

            if ( hr == S_OK && bCmdId != CMD_READ_DEVICE_EVENT_EX )
            {
                continue;
            }

            if ( hr == S_OK
                    && bCmdId == CMD_READ_DEVICE_EVENT_EX
                    && dwLen > 8
                    && rgbData[0] == FALG_OK )
            {
                *pbEventId = rgbData[1];
                *pbType	  = rgbData[2];
                *pbEventDeviceType = rgbData[3];
                memcpy(pdwTime, &rgbData[4], 4);
                *pdwSize = ((dwLen - 8) > *pdwSize) ? *pdwSize : (dwLen - 8);
                memcpy( pbData, &rgbData[8], *pdwSize );
                break;
            }
            else
            {
                hr = E_FAIL;
                break;
            }
        }
    }

	((CIPTControlImpl*)m_pIControl)->UnLock();

    return hr;
}

HRESULT CIPTDeviceImpl::SyncTime( DWORD32 dwTime )
{
    if ( m_pIControl == NULL ) return E_POINTER;

	if(S_OK != ((CIPTControlImpl*)m_pIControl)->Lock())
	{
		return E_FAIL;
	}

    BYTE8 rgbData[16] = {0};
    memcpy( rgbData, &dwTime, 4 );
    HRESULT hr = m_pIControl->SendCommand( CMD_SET_TIME, rgbData, 4 );
    if ( S_OK == hr )
    {
        while (true)
        {
            BYTE8 bCmd = 0;
            DWORD32 cbRead = 16;
            hr = m_pIControl->RecvData( &bCmd, rgbData, &cbRead, 100 );

            if ( S_OK == hr  && bCmd != CMD_SET_TIME )
            {
                continue;
            }

            if ( S_OK == hr
                    && bCmd == CMD_SET_TIME && cbRead == 1 && rgbData[0] == FALG_OK )
            {
                hr = S_OK;
                break;
            }
            else
            {
                hr = E_FAIL;
                break;
            }
        }
    }

	((CIPTControlImpl*)m_pIControl)->UnLock();

    return hr;
}

HRESULT CIPTDeviceImpl::SendSignal(int nIoID, int nLevel, int nSignalWidth)
{
    return IPT_OutputPulse((BYTE8)nIoID, (BYTE8)nLevel, (WORD16)nSignalWidth, m_pIControl);
}
