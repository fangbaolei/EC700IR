#include "IPTControlImpl.h"
#include "config.h"

#if defined(SINGLE_BOARD_PLATFORM) || defined(IPT_IN_MASTER)
#include "IPTComImpl_Master.h"
#else
#include "IPTComImpl_Slave.h"
#endif

HRESULT CreateIComm(IComm** ppIComm, const char* pszCom)
{
	if (ppIComm == NULL)
	{
		return E_POINTER;
	}

    IComm *pICommImpl = NULL;
#if defined(SINGLE_BOARD_PLATFORM) || defined(IPT_IN_MASTER)
    CIPTComImpl_Master* pIPTComImpl_Master = new CIPTComImpl_Master;
    if (pIPTComImpl_Master->Open(pszCom) != S_OK)
    {
        delete pIPTComImpl_Master;
        pIPTComImpl_Master = NULL;
    }
    pICommImpl = pIPTComImpl_Master;
#else
    pICommImpl = new CIPTComImpl_Slave;
#endif
    if (pICommImpl == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// final assignment
	*ppIComm = pICommImpl;

	return S_OK;
}

// pIComm为空时默认使用/dev/ttyS0
HRESULT CreateIIPTControl(IIPTControl** ppIIPTControl, IComm *pIComm)
{
    if (ppIIPTControl == NULL)
    {
        return E_POINTER;
    }

    CIPTControlImpl *pIPTControlImpl = new CIPTControlImpl(pIComm);

    if (pIPTControlImpl == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // final assignment
    *ppIIPTControl = pIPTControlImpl;

    return S_OK;
}

// ---------------- CIPTControlImpl ------------
CIPTControlImpl::CIPTControlImpl(IComm* pIComm)
        : m_pIComm(NULL)
{
    m_pIComm = pIComm;
}

CIPTControlImpl::~CIPTControlImpl(void)
{
}

HRESULT CIPTControlImpl::Lock()
{
#if defined(SINGLE_BOARD_PLATFORM) || defined(IPT_IN_MASTER)
    return ((CIPTComImpl_Master*)m_pIComm)->Lock(3000);
#else
    return S_OK;
#endif
}

void CIPTControlImpl::UnLock()
{
#if defined(SINGLE_BOARD_PLATFORM) || defined(IPT_IN_MASTER)
    ((CIPTComImpl_Master*)m_pIComm)->UnLock();
#endif
}

HRESULT CIPTControlImpl::SendCommand( const BYTE8 bCmdId, PBYTE8 pbData, DWORD32 dwSize )
{
    if ( m_pIComm == NULL || (pbData != NULL && (dwSize > MAX_PACK_LEN - 6)))
    {
        return E_POINTER;
    }

    BYTE8 rgbCmd[MAX_PACK_LEN] = {FALG_BEGIN, FALG_NUMBER, bCmdId},tem[MAX_PACK_LEN]={0};
    DWORD32 dwCheck = 0;
    DWORD32 dwLen = 3;
    if (pbData !=NULL)
    {
        memcpy(tem,pbData,dwSize);
    }
    for (int i = 0;i < (int)dwSize;i++)
    {
        if (tem[i]==0xFD || tem[i]==0xFF || tem[i]==0xFE)
        {
            if (tem[i]==0xFD)
            {
                rgbCmd[dwLen++]=0xFD;
                rgbCmd[dwLen++]=0x00;
            }
            else if (tem[i]==0xFE)
            {
                rgbCmd[dwLen++]=0xFD;
                rgbCmd[dwLen++]=0x01;
            }
            else
            {
                rgbCmd[dwLen++]=0xFD;
                rgbCmd[dwLen++]=0x02;
            }
        }
        else
            rgbCmd[dwLen++]=tem[i];
    }

    for ( int i = 0; i < (int)dwLen; ++i )
    {
        dwCheck += rgbCmd[i];
    }
    if ((BYTE8)dwCheck==0xFD || (BYTE8)dwCheck==0xFF || (BYTE8)dwCheck==0xFE)
    {
        if ((BYTE8)dwCheck==0xFD)
        {
            rgbCmd[dwLen++]=0xFD;
            rgbCmd[dwLen++]=0x00;
        }
        else if ((BYTE8)dwCheck==0xFE)
        {
            rgbCmd[dwLen++]=0xFD;
            rgbCmd[dwLen++]=0x01;
        }
        else
        {
            rgbCmd[dwLen++]=0xFD;
            rgbCmd[dwLen++]=0x02;
        }
    }
    else
    {
        rgbCmd[dwLen++] = (BYTE8) dwCheck;
    }

    rgbCmd[dwLen++] = FALG_END;

    return m_pIComm->Send(rgbCmd, dwLen);
}

HRESULT CIPTControlImpl::RecvData( PBYTE8 pbCmdId, PBYTE8 pbData, PDWORD32 pdwLen, DWORD32 dwRelayMs )
{
    if ( m_pIComm == NULL || pbCmdId == NULL || pdwLen == NULL )
    {
        return E_POINTER;
    }

    memset(m_rgbRecvData, 0, MAX_PACK_LEN);
    DWORD32 dwRecvLen = 0;
    DWORD32 dwByteCount = 0;

    int iIntervalMs = 10;
    int  iTimeMs = (int)(dwRelayMs > 3000 ? 3000 : dwRelayMs);
    while ( true )
    {
        dwByteCount = 0;
        m_pIComm->Recv( &m_rgbRecvData[ 0 ], 1, &dwByteCount);
        if ( dwByteCount != 1 && iTimeMs <= 0 )
        {
            return E_FAIL;
        }
        if ( m_rgbRecvData[ 0 ] == FALG_BEGIN )
        {
            break;
        }
        HV_Sleep(iIntervalMs);
        iTimeMs -= iIntervalMs;
    }

    dwRecvLen = 1;
    while (true)
    {
        dwByteCount = 0;
        m_pIComm->Recv( &m_rgbRecvData[ dwRecvLen ], 1, &dwByteCount);
        if ( dwByteCount != 1 )
        {
            if ( iTimeMs <= 0 )
            {
                return E_FAIL;
            }
            else
            {
                HV_Sleep(iIntervalMs);
                iTimeMs -= iIntervalMs;
                continue;
            }
        }
        if ( m_rgbRecvData[dwRecvLen] == FALG_END )
        {
            dwRecvLen++;
            break;
        }
        dwRecvLen++;
    }

    HRESULT hr = CheckData( dwRecvLen );
    if ( hr == S_OK )
    {
        *pbCmdId = m_dwCmdId;
        *pdwLen = *pdwLen > m_dwDataLen ? m_dwDataLen : *pdwLen;
        memcpy(pbData, m_rgbData, *pdwLen);
    }
    else
    {
        *pdwLen = 0;
    }

    // TODO : 暂时屏蔽偏振镜回应，改PCI机制后去掉
    if (m_dwCmdId == 0x18)
    {
        hr = RecvData(pbCmdId, pbData, pdwLen, dwRelayMs);
    }
    return hr;
}

HRESULT CIPTControlImpl::CheckData( const DWORD32 dwRecvLen )
{
    if ( m_rgbRecvData[0] != FALG_BEGIN
            || m_rgbRecvData[1] != FALG_NUMBER
            || m_rgbRecvData[dwRecvLen-1] != FALG_END )
    {
        return E_FAIL;
    }

    BYTE8 rgbData[MAX_PACK_LEN] = {0};
    int iPos = 0;
    rgbData[iPos++] = FALG_BEGIN;
    int i = 0;
    for ( i = 1; i < (int)(dwRecvLen-1); ++i )
    {
        if ( m_rgbRecvData[i] == FALG_EXCHANGE )
        {
            switch (m_rgbRecvData[i+1])
            {
            case 0x00:
                rgbData[iPos++] = FALG_EXCHANGE;
                break;
            case 0x01:
                rgbData[iPos++] = FALG_END;
                break;
            case 0x02:
                rgbData[iPos++] = FALG_BEGIN;
                break;
            default:
                break;
            }
            ++i;
        }
        else
        {
            rgbData[iPos++] = m_rgbRecvData[i];
        }
    }
    rgbData[iPos++] = FALG_END;

    DWORD32 dwCheck = 0;
    for ( i = 0; i < iPos-2; ++i )
    {
        dwCheck += rgbData[i];
    }
    if ( (BYTE8)dwCheck != rgbData[iPos-2] )
    {
        return E_FAIL;
    }

    m_dwCmdId = rgbData[2];
    m_dwDataLen = iPos - 5;
    memcpy(m_rgbData, &rgbData[3], m_dwDataLen);

    return S_OK;
}

//==------------------------------ 命令 ----------------------------------------------==//

#ifndef IPT_OK
#define IPT_OK 0x00
#endif

#ifndef IPT_CMD_NORMAL_DELAY
#define IPT_CMD_NORMAL_DELAY 20
#endif

#ifndef IPT_CMD_SET_DELAY
#define TPT_CMD_SET_DELAY 200
#endif

inline void CmdNormalDelay()
{
	HV_Sleep( IPT_CMD_NORMAL_DELAY );
}
inline void CmdSetDelay()
{
	HV_Sleep(TPT_CMD_SET_DELAY);
}

//复位
HRESULT IPT_Reset( IIPTControl* pControl )
{
	if( pControl == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_RESET, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 1000 );
		if( S_OK == hr
			&& bCmd == CMD_RESET && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();

	return hr;
}

//读复位计数器
HRESULT IPT_ResetCount( PDWORD32 pdwCount, IIPTControl* pControl )
{
	if( pControl == NULL || pdwCount == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	*pdwCount = 0;
	HRESULT hr = pControl->SendCommand( CMD_READ_RESETCOUNT, NULL, 0 );
	if( S_OK == hr )
	{
		BYTE8 bCmd = 0;
		DWORD32 cbRead = 16;
		BYTE8 rgbData[16] = {0};
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( hr == S_OK && bCmd == CMD_READ_RESETCOUNT
			&& cbRead == 3 && rgbData[0] == IPT_OK )
		{
			memcpy( pdwCount, &rgbData[1], 2 );
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//清零复位计数器
HRESULT IPT_ClearResetCount( IIPTControl* pControl )
{
	if( pControl == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_CLEAR_RESETCOUNT, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_CLEAR_RESETCOUNT && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//设置时间
HRESULT IPT_SetTime( DWORD32 dwTime, IIPTControl* pControl )
{
	if( pControl == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {0};
	memcpy( rgbData, &dwTime, 4 );
	HRESULT hr = pControl->SendCommand( CMD_SET_TIME, rgbData, 4 );
	if( S_OK == hr )
	{
		BYTE8 bCmd = 0;
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_SET_TIME && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
		    HV_Trace(5, "IPT_SetTime, req cmd id = 0x%x return cmd=0x%x, cbRead = %d, rgbData[0]=%d", CMD_SET_TIME, bCmd, cbRead, rgbData[0]);
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//读时间
HRESULT IPT_ReadTime( PDWORD32 pdwTime, IIPTControl* pControl )
{
	if( pControl == NULL || pdwTime == NULL )
	{
		return E_POINTER;
	}

	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	*pdwTime = 0;
	HRESULT hr = pControl->SendCommand( CMD_READ_TIME, NULL, 0 );
	if( S_OK == hr )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( hr == S_OK
			&& bCmd == CMD_READ_TIME && cbRead == 5 && rgbData[0] == IPT_OK )
		{
			memcpy( pdwTime, &rgbData[1], 4 );
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//恢复默认设置
HRESULT IPT_ResetDefault( IIPTControl* pControl )
{
	if( pControl == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_RESET_DEFAULT, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 2000);
		if( S_OK == hr
			&& bCmd == CMD_RESET_DEFAULT && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//写透明串口命令
HRESULT IPT_WriteComCmd( BYTE8 bDB, PBYTE8 pbData, DWORD32 dwLen, IIPTControl* pControl )
{
	if( pControl == NULL || pbData == NULL || dwLen <= 0 )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 bCmd = 0;
	BYTE8 rgbData[384] = {0};
	rgbData[0] = bDB;
	rgbData[1] = (BYTE8)dwLen;
	memcpy( &rgbData[2], pbData, dwLen );
	DWORD32 dwSize = dwLen + 2;
	HRESULT hr = pControl->SendCommand( CMD_WRITE_COMM, rgbData, dwSize );
	if( hr == S_OK )
	{
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_WRITE_COMM && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//读透明串口命令
HRESULT IPT_ReadComCmd(  BYTE8 bDB, PBYTE8 pbData, DWORD32 dwLen, PDWORD32 pcbRead, IIPTControl* pControl )
{
	if( pControl == NULL || pbData == NULL || dwLen <= 0 || pcbRead == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}
	*pcbRead = 0;

	HRESULT hr = pControl->SendCommand( CMD_READ_COMM, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[384] = {0};
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_READ_COMM && cbRead > 3 && rgbData[0] == IPT_OK && rgbData[1] == bDB )
		{
			*pcbRead = dwLen > rgbData[2] ? rgbData[2] : dwLen;
			memcpy( pbData, &rgbData[3], *pcbRead );
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//配置透明串口参数
HRESULT IPT_SetComParam( BYTE8 bDB, DWORD32 dwBT, BYTE8 bCheck, DWORD32 dwBit, IIPTControl* pControl )
{
	if( pControl == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 bCmd = 0;
	BYTE8 rgbData[16] = {0};
	rgbData[0] = bDB;
	switch( dwBT)
	{
	case 2400:
		rgbData[1] = 0x00;
		break;
	case 4800:
		rgbData[1] = 0x01;
		break;
	case 9600:
		rgbData[1] = 0x02;
		break;
	case 19200:
		rgbData[1] = 0x03;
		break;
	default:
		rgbData[1] = 0x02;
	}
	rgbData[2] = bCheck;
	switch(dwBit)
	{
	case 7:
		rgbData[3] = 0x00;
		break;
	case 8:
		rgbData[3] = 0x01;
		break;
	default:
		rgbData[3] = 0x01;
	}

	HRESULT hr = pControl->SendCommand( CMD_SET_COMM_PARAM, rgbData, 4 );
	if( hr == S_OK )
	{
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_SET_COMM_PARAM && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//读透明串口配置参数
HRESULT IPT_ReadComParam(BYTE8 bDB, PDWORD32 pdwBT, PBYTE8 pbCheck, PDWORD32 pdwBit, IIPTControl* pControl )
{
	if( pControl == NULL || pdwBT == NULL || pdwBit == NULL || pbCheck == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_COMM_PARAM, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_READ_COMM_PARAM && cbRead == 5  && rgbData[0] == IPT_OK && rgbData[1] == bDB )
		{
			switch( rgbData[2] )
			{
			case 0x00:
				*pdwBT = 2400;
				break;
			case 0x01:
				*pdwBT = 4800;
				break;
			case 0x02:
				*pdwBT = 9600;
				break;
			case 0x03:
				*pdwBT = 19200;
				break;
			default:
				*pdwBT = 0;
			}
			*pbCheck = rgbData[3];
			switch(rgbData[4])
			{
			case 0x00:
				*pdwBit = 7;
				break;
			case 0x01:
				*pdwBit = 8;
				break;
			default:
				*pdwBit = 0;
			}
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//配置外部设备类型及参数
HRESULT ITP_SetDeviceParam( BYTE8 bDB, BYTE8 bType, PBYTE8 pbData, DWORD32 dwLen, IIPTControl* pControl )
{
	if( pControl == NULL || pbData == NULL || dwLen <= 0 )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[384] = {0};
	rgbData[0] = bDB;
	rgbData[1] = bType;
	memcpy(&rgbData[2], pbData, dwLen );

	HRESULT hr = pControl->SendCommand( CMD_SET_DEVICE, rgbData, dwLen + 2 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_SET_DEVICE && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//读外部设备参数
HRESULT IPT_ReadDeviceParam( BYTE8 bDB, PBYTE8 pbType, PBYTE8 pbData, DWORD32 dwLen, PDWORD32 pcbRead, IIPTControl* pControl )
{
	if( pControl == NULL || pbData == NULL || dwLen <= 0 || pcbRead == 0 || pbType == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	*pcbRead = 0;
	HRESULT hr = pControl->SendCommand( CMD_READ_DEVICE, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[384] = {0};
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_READ_DEVICE && cbRead > 3 && rgbData[0] == IPT_OK && rgbData[1] == bDB )
		{
			*pbType = rgbData[2];
			*pcbRead = dwLen > (cbRead - 3) ? (cbRead - 3) : dwLen;
			memcpy( pbData, &rgbData[3], *pcbRead );
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//读外部设备事件
HRESULT IPT_ReadDeviceEvent( PBYTE8 pbEvent, PBYTE8 pbType,PBYTE8 pbEventDeviceType, DWORD32* pdwTime, PBYTE8 pbData, DWORD32 dwLen, PDWORD32 pcbRead, IIPTControl* pControl )
{
	if( pControl == NULL || pbEvent == NULL || pbData == NULL || dwLen <= 0 || pcbRead == 0 || pbType == 0
		|| pdwTime == NULL  || pbEventDeviceType == NULL)
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	*pcbRead = 0;
	HRESULT hr = pControl->SendCommand( CMD_READ_DEVICE_EVENT, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[384] = {0};
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_DEVICE_EVENT && cbRead >= 8 && rgbData[0] == IPT_OK )
		{
			*pbEvent = rgbData[1];
			*pbType  = rgbData[2];
			*pbEventDeviceType  = rgbData[3];
			memcpy(pdwTime, &rgbData[4], 4);
			*pcbRead = dwLen > (cbRead - 8) ? (cbRead - 8) : dwLen;
			memcpy( pbData, &rgbData[8], *pcbRead );
		}
		else
		{
		    HV_Trace(5, "req cmd id = 0x%x, recv cmd id = 0x%x, cbRead = %d, hr = 0x%x", CMD_READ_DEVICE_EVENT, bCmd, cbRead, hr);
			hr = E_FAIL;
		}
	}
	else
	{
	    HV_Trace(5, "send CMD_READ_DEVICE_EVENT failed");
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//新版本读外部设备事件
HRESULT IPT_ReadDeviceEvent_Ex(
				 PBYTE8 pbInData, DWORD32 dwInLen,
			       PBYTE8 pbEvent, PBYTE8 pbType,
			       PBYTE8 pbEventDeviceType, DWORD32* pdwTime,
			       PBYTE8 pbData, DWORD32 dwLen,
			       PDWORD32 pcbRead, IIPTControl* pControl )
{
	if( pControl == NULL || pbEvent == NULL || pbData == NULL || dwLen == 0 || pcbRead == 0 || pbType == 0
		|| pdwTime == NULL  || pbEventDeviceType == NULL || pbInData == NULL || dwInLen == 0 )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	*pcbRead = 0;

	HRESULT hr = pControl->SendCommand( CMD_READ_DEVICE_EVENT_EX, pbInData, dwInLen );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[384] = {0};
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_DEVICE_EVENT_EX && cbRead > 8 && rgbData[0] == IPT_OK )
		{
			*pbEvent = rgbData[1];
			*pbType  = rgbData[2];
			*pbEventDeviceType  = rgbData[3];
			memcpy(pdwTime, &rgbData[4], 4);
			*pcbRead = dwLen > (cbRead - 8) ? (cbRead - 8) : dwLen;
			memcpy( pbData, &rgbData[8], *pcbRead );
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//设置I/O输出口屏蔽字
HRESULT IPT_SetIOShield( BYTE8 bDB, BYTE8 bFlag, IIPTControl* pControl )
{
	if( pControl == NULL || bFlag == (BYTE8)NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {bDB,bFlag};
	HRESULT hr = pControl->SendCommand( CMD_SET_IO_SHIELD, rgbData, 2 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,16);
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_SET_IO_SHIELD && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//读I/O输出口屏蔽字
HRESULT IPT_ReadIOShield( BYTE8 bDB, PBYTE8 pbFlag, IIPTControl* pControl )
{
	if( pControl == NULL || pbFlag == NULL || bDB>4 )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_IO_SHIELD, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_IO_SHIELD && cbRead == 2 && rgbData[0] == IPT_OK )
		{
			*pbFlag = rgbData[1];
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//设置IO输出信号源
HRESULT IPT_SetIOOutSignal( BYTE8 bDB, BYTE8 bFlag, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {bDB,bFlag};
	HRESULT hr = pControl->SendCommand( CMD_SET_IO_OUT_SIGNAL, rgbData, 2 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,16);
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_SET_IO_OUT_SIGNAL && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//读IO输出信号源
HRESULT IPT_ReadIOOutSignal( BYTE8 bDB, PBYTE8 pbFlag, IIPTControl* pControl )
{
	if( pControl == NULL || pbFlag == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_IO_OUT_SIGNAL, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_IO_OUT_SIGNAL && cbRead == 3 && rgbData[0] == IPT_OK )
		{
			*pbFlag = rgbData[2];
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//读I/O输入口电平
HRESULT IPT_ReadIOInputLevel( BYTE8 bDB, PBYTE8 pbFlag, IIPTControl* pControl )
{
	if( pControl == NULL || pbFlag == NULL || bDB>4 )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_IO_LEVEL, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_IO_LEVEL && cbRead == 3 && rgbData[0] == IPT_OK )
		{
			*pbFlag = rgbData[2];
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//读所有I/O输入口电平
HRESULT IPT_ReadAllIOInputLevel( PBYTE8 pbLevel, IIPTControl* pControl )
{
	if( pControl == NULL || pbLevel == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_ALL_IO_LEVEL, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 20 );
		if( S_OK == hr
			&& bCmd == CMD_READ_ALL_IO_LEVEL && cbRead == 2 && rgbData[0] == IPT_OK )
		{
			*pbLevel = rgbData[1];
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

//从I/O输出口输出一个脉冲
HRESULT IPT_OutputPulse( BYTE8 bDB, BYTE8 bLevel, WORD16 wWidth, IIPTControl* pControl )
{
	if( pControl == NULL )
	{
		return E_POINTER;
	}
	if( bLevel != 0 && bLevel != 1)
	{
		return E_FAIL;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {bDB};
	rgbData[0] = bDB;
	rgbData[1] = bLevel;
	memcpy(&rgbData[2], &wWidth, 2);
	HRESULT hr = pControl->SendCommand( CMD_OUTPUT_PULSE, rgbData, 4 );

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//设置IO输出口延时
HRESULT IPT_SetIOOutDelay( BYTE8 bDB, int pdelay, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {0};
	rgbData[0]=bDB;
	memcpy( &rgbData[1] , &pdelay,2);
	HRESULT hr = pControl->SendCommand( CMD_SET_IO_OUT_DELAY, rgbData, 3 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,16);
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_SET_IO_OUT_DELAY && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//读IO输出口延时
HRESULT IPT_ReadIOOutDelay( BYTE8 bDB, PBYTE8 pdelay, IIPTControl* pControl )
{
	if( pControl == NULL || pdelay == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_IO_OUT_DELAY, &bDB, 1 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_IO_OUT_DELAY && cbRead == 4 && rgbData[0] == IPT_OK )
		{
			memcpy(pdelay,&rgbData[1],3);
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//设置IO输出口的初始化电平
HRESULT IPT_SetIOOutInitLevel( BYTE8 bDB, BYTE8 initlevel, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {bDB,initlevel};
	HRESULT hr = pControl->SendCommand( CMD_SET_IO_OUT_INIT_LEVEL, rgbData, 2 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,16);
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_SET_IO_OUT_INIT_LEVEL && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//读IO输出口的初始化电平
HRESULT IPT_ReadIOOutInitLevel( PBYTE8 initlevel, IIPTControl* pControl )
{
	if( pControl == NULL || initlevel == NULL )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	HRESULT hr = pControl->SendCommand( CMD_READ_IO_OUT_INIT_LEVEL, NULL,0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		BYTE8 rgbData[16] = {0};
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_IO_OUT_INIT_LEVEL && cbRead == 2 && rgbData[0] == IPT_OK )
		{
			*initlevel = rgbData[1];
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//进入升级状态
HRESULT IPT_EnterUpdateState( PBYTE8 statedata, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {0};
	memcpy(rgbData,statedata,6);
	HRESULT hr = pControl->SendCommand( CMD_ENTER_UPDATE_STATE, rgbData, 6 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,16);
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 600 );
		if( S_OK == hr
			&& bCmd == CMD_ENTER_UPDATE_STATE && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//退出升级状态
HRESULT IPT_ExitUpdateState( PBYTE8 exitdata, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[16] = {0};
	memcpy(rgbData,exitdata,6);
	HRESULT hr = pControl->SendCommand( CMD_EXIT_UPDATE_STATE, rgbData, 6 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,16);
		DWORD32 cbRead = 16;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 200 );
		if( S_OK == hr
			&& bCmd == CMD_EXIT_UPDATE_STATE && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//上传升级数据
HRESULT IPT_UploadUpdateData( int page, PBYTE8 updatedata, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[384] = {0};
	memcpy(rgbData,&page,2);
	memcpy(&rgbData[2],updatedata,256);
	HRESULT hr = pControl->SendCommand( CMD_UPLOAD_UPDATE_DATA, rgbData, 258 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,384);
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 1000 );
		if( S_OK == hr
			&& bCmd == CMD_UPLOAD_UPDATE_DATA && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	((CIPTControlImpl*)pControl)->UnLock();

	return hr;
}
//下载升级数据
HRESULT IPT_DownloadUpdateData( int page,PBYTE8 updatedata, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[384] = {0};
	memcpy(rgbData,&page,2);
	HRESULT hr = pControl->SendCommand( CMD_DOWNLOAD_UPDATE_DATA, rgbData, 2 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,384);
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 1000 );
		if( S_OK == hr
			&& bCmd == CMD_DOWNLOAD_UPDATE_DATA && cbRead == 259 && rgbData[0] == IPT_OK )
		{
			memcpy( updatedata, &rgbData[1],258);
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	((CIPTControlImpl*)pControl)->UnLock();

	return hr;
}
//读转换器版本信息
HRESULT IPT_ReadSoftwareVersion( PBYTE8 info, IIPTControl* pControl )
{
	if( pControl == NULL  || info == NULL)
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[384] = {0};
	HRESULT hr = pControl->SendCommand( CMD_READ_SOFTWARE_VERSION, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,384);
		DWORD32 cbRead = 384;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 100 );
		if( S_OK == hr
			&& bCmd == CMD_READ_SOFTWARE_VERSION && cbRead == 65 && rgbData[0] == IPT_OK )
		{
			memcpy( info, &rgbData[1],65);
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//读设备工作参数,pbDeviceType的长度为4, pbworkData的长度为256
HRESULT IPT_ReadDeviceWorkParam(PBYTE8 pbDeviceType, PBYTE8 pbworkData, IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}

	BYTE8 rgbData[1024] = {0};
	HRESULT hr = pControl->SendCommand( CMD_READ_WORK_PARAM, NULL, 0 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,1024);
		DWORD32 cbRead = 1024;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 1000 );
		if( S_OK == hr
			&& bCmd == CMD_READ_WORK_PARAM && cbRead == 261 && rgbData[0] == IPT_OK )
		{
			memcpy( pbDeviceType, &rgbData[1],4);
			memcpy( pbworkData, &rgbData[5],256);
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}
//写设备工作参数
HRESULT IPT_WriteDeviceWorkParam(PBYTE8 pbDeviceType, PBYTE8 pbworkData,int iDataSize , IIPTControl* pControl )
{
	if( pControl == NULL  )
	{
		return E_POINTER;
	}
	if(S_OK != ((CIPTControlImpl*)pControl)->Lock())
	{
		return E_FAIL;
	}
	BYTE8 rgbData[512] = {0};
	memcpy(rgbData,pbDeviceType,4);
	memcpy(&rgbData[4],pbworkData,iDataSize);
	HRESULT hr = pControl->SendCommand( CMD_WRITE_WORK_PARAM, rgbData, iDataSize+4 );
	if( hr == S_OK )
	{
		BYTE8 bCmd = 0;
		memset(rgbData,0,512);
		DWORD32 cbRead = 512;
		hr = pControl->RecvData( &bCmd, rgbData, &cbRead, 2000 );
		if( S_OK == hr
			&& bCmd == CMD_WRITE_WORK_PARAM && cbRead == 1 && rgbData[0] == IPT_OK )
		{
			hr = S_OK;
		}
		else
		{
		    HV_Trace(5, "read data failed, req cmd id = 0x%x, bCmd = 0x%x, hr = 0x%08x, cbRead=%d, rgbData[0] = %d", CMD_WRITE_WORK_PARAM, bCmd, hr, cbRead, rgbData[0]);
			hr = E_FAIL;
		}
	}
	else
	{
	    HV_Trace(5, "send failed.");
	}

	((CIPTControlImpl*)pControl)->UnLock();
	return hr;
}

