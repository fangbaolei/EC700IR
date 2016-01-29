// 该文件编码格式必须为WINDOWS-936格式

#include "ImgGatherer.h"

CBroadcastLink::CBroadcastLink()
	: m_sktData(INVALID_SOCKET)
	, m_pstmData(NULL)
	, m_dwRefTime(0)
{
}

CBroadcastLink::~CBroadcastLink()
{
	if (m_sktData != INVALID_SOCKET)
	{
		CloseSocket(m_sktData);
	    m_sktData = INVALID_SOCKET;
	}

	if (m_pstmData != NULL)
    {
        delete m_pstmData;
        m_pstmData = NULL;
    }
}

typedef struct _UDP_JPEG_DATA {
	char szFileName[128];
	DWORD32 dwFileSize;
	DWORD32 dwRefTime;
	DWORD32 dwPacketStart;
	DWORD32 dwPacketSize;
	BYTE8 rgbPacketData[1024];
	_UDP_JPEG_DATA()
	{
		memset(this, 0, sizeof(_UDP_JPEG_DATA));
	}
} UDP_JPEG_DATA;

HRESULT CBroadcastLink::GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, char* pszFileName)
{
	if (m_sktData == INVALID_SOCKET || !m_pstmData
        || !pdwSize || !pdwTime || !pszFileName)
    {
        return E_POINTER;
    }

	int iPacketSize(0);
	DWORD32 dwPacketStart(0);
	UDP_JPEG_DATA udp_jpeg_data;
	DWORD32 dwRemoteAddr;
	WORD16 wRemotePort;

	//定位文件头
	while (true)
	{
		iPacketSize = m_pstmData->UDPRead(
			(PBYTE8)&udp_jpeg_data,
			sizeof(udp_jpeg_data),
			&dwRemoteAddr,
			&wRemotePort,
			RCV_TIMEOUT);
		if (iPacketSize != sizeof(udp_jpeg_data) ||
			udp_jpeg_data.dwPacketSize > sizeof(udp_jpeg_data.rgbPacketData))
		{
			return E_FAIL;
		}
		if (udp_jpeg_data.dwPacketStart == 0)
		{
			*pdwSize = udp_jpeg_data.dwFileSize;
			*pdwTime = GetSystemTick();
			memcpy(pcBuf, udp_jpeg_data.rgbPacketData, udp_jpeg_data.dwPacketSize);
			strncpy(pszFileName, udp_jpeg_data.szFileName, 128);
			pcBuf += udp_jpeg_data.dwPacketSize;
			dwPacketStart += udp_jpeg_data.dwPacketSize;
			break;
		}
	}
	// 接收图片数据
	while (udp_jpeg_data.dwPacketStart + udp_jpeg_data.dwPacketSize < udp_jpeg_data.dwFileSize)
	{
		iPacketSize = m_pstmData->UDPRead(
			(PBYTE8)&udp_jpeg_data,
			sizeof(udp_jpeg_data),
			&dwRemoteAddr,
			&wRemotePort,
			RCV_TIMEOUT);
		if (iPacketSize == sizeof(udp_jpeg_data) &&
			udp_jpeg_data.dwPacketSize <= sizeof(udp_jpeg_data.rgbPacketData) &&
			udp_jpeg_data.dwPacketStart == dwPacketStart)
		{
			memcpy(pcBuf, udp_jpeg_data.rgbPacketData, udp_jpeg_data.dwPacketSize);
			pcBuf += udp_jpeg_data.dwPacketSize;
			dwPacketStart += udp_jpeg_data.dwPacketSize;
			if (dwPacketStart == udp_jpeg_data.dwFileSize)
			{
				break;
			}
			else if (dwPacketStart > udp_jpeg_data.dwFileSize)
			{
				return E_FAIL;
			}
		}
		else
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBroadcastLink::Initialize()
{
	if (m_sktData != INVALID_SOCKET)
	{
		CloseSocket(m_sktData);
	    m_sktData = INVALID_SOCKET;
	}

	if (m_pstmData != NULL)
    {
        delete m_pstmData;
        m_pstmData = NULL;
    }

	m_sktData = HvCreateSocket(AF_INET, SOCK_DGRAM);

	if (m_sktData == INVALID_SOCKET)
	{
	    return E_FAIL;
    }

    BOOL on = 1;
    SetSockOpt(m_sktData, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));

    int iOpt = 1;
    SetSockOpt(m_sktData, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOpt, sizeof(iOpt));

    m_pstmData = new CSocketStream(m_sktData);

    if (m_pstmData == NULL)
    {
        return E_FAIL;
    }

    return HvUDPBind(m_sktData, SVR_BROADCAST_PORT);
}

