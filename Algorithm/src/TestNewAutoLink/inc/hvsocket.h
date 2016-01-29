#ifndef _HV_SOCKET_H_
#define _HV_SOCKET_H_

#include "HvStream.h"
#include "HvInterface.h"

namespace HiVideo
{
	class ISocket 
	{
	public:
		enum {CLOSED, LISTENING, CONNECTING};
		virtual ~ISocket() {};
		virtual HRESULT Connect(const char *pszAddress, int iPort, int iTimeout) = 0;
		virtual HRESULT Listen(int iPort, int iCount) = 0;
		virtual HRESULT Accept(ISocket **ppSocket, int iTimeout) = 0;
		virtual HRESULT Close() = 0;
		virtual HRESULT GetStream(HvCore::IHvStream **ppStream) = 0;
		virtual HRESULT GetPeerName(DWORD32* pdwAddress, int* piPort) = 0;
		virtual HRESULT GetSockName(DWORD32* pdwAddress, int* piPort) = 0;
		virtual HRESULT GetStatus(int &iStatus) = 0;
		virtual HRESULT SetRevTimeOut(int iTimeout) = 0;
		virtual HRESULT SetSendTimeOut(int iTimeout) = 0;
		virtual HRESULT UDPBind(WORD16 wLocalPort) = 0;
		virtual int UDPSend(BYTE8 *pData, int iDataLen, DWORD32 dwRemoteAddr, WORD16 wRemotePort) = 0;
		virtual int UDPReceive(BYTE8 *pBuffer, int iBufferLen, DWORD32 *pRemoteAddr, WORD16 *pRemotePort, int iTimeout) = 0;
		static HRESULT CreateInstance(ISocket **ppSocket, bool fIsUDP = false);
	};
}

#endif
