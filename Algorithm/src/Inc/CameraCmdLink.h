// 该文件编码格式必须是WIN936
//

#ifndef _CAMERACMDLINK_H_
#define _CAMERACMDLINK_H_

#include "HvCameraLink.h"
#include "hvenum.h"
#include "hvthreadbase.h"
#include "HvSockUtils.h"
#include "SocketStream.h"

class CCameraCmdLink : public HiVideo::ICameraCmdLink
{
public:
    // ICamCmdLink
    virtual HRESULT ReceiveHeader(CAMERA_CMD_HEADER* pCmdHeader);
    virtual HRESULT SendRespond(const CAMERA_CMD_RESPOND* pCmdRespond);
    virtual HRESULT ReceiveData(PBYTE8 pbData, const DWORD32& dwSize, DWORD32* pdwRecvSize);
    virtual HRESULT SendData(const PBYTE8 pbData, const DWORD32& dwSize);

	//命令处理主函数
    virtual HRESULT Process(HV_SOCKET_HANDLE hSocket, HiVideo::ICameraCmdProcess* pCmdProcess);

public:
    CCameraCmdLink();
    ~CCameraCmdLink();

private:
    CSocketStream* m_pStream;
};

class CCameraCmdLinkEnum : public CHvEnumBase<CCameraCmdLink*>
{
protected:
    HRESULT OnDelNode(NODE* pNode)
    {
        SAFE_DELETE(pNode->Dat);
        return S_OK;
    }
};

class CCameraCmdLinkCtrl : public CHvThreadBase
{
public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CCameraCmdLinkCtrl";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

public:
    CCameraCmdLinkCtrl( HiVideo::ICameraCmdProcess* pCmdProcess);
    ~CCameraCmdLinkCtrl();

    HRESULT EnumLock();
    HRESULT EnumUnLock();

    HRESULT SetParam(HiVideo::ICameraCmdProcess* pCmdProcess);

private:
    static const int MAX_CMD_CONNECT_COUNT = 1;
    CCameraCmdLinkEnum m_cCameraCmdLinkEnum;

    HV_SEM_HANDLE m_hSemEnum;
    HV_SOCKET_HANDLE m_hSocket;

    HiVideo::ICameraCmdProcess* m_pCmdProcess;

	CCameraCmdLink    m_cCameraCmdLink;
};


#endif
