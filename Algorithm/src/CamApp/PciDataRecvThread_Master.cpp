#include "HvPciLinkApi.h"
#include "sendnetdata.h"
#include "DataCtrl.h"
#include "tinyxml.h"
#include "ControlFunc.h"
#include "DspLinkMemBlocks.h"
#include "HvSerialLink.h"
#include "IPT.h"
#include "HvDebugStateInfo.h"
#include "RadarBase.h"
#include "CSRIRadar.h"

/* misc.cpp */
extern void DebugPrintf(const char* szDebugInfo, DWORD dwDumpLen, const char* szID);
extern HRESULT GetPlateNumByXmlExtInfo(char* szXmlExtInfo, char* szPlateNum);

PBYTE8 GetVirtualAddr(DSPLinkBuffer* pBuffer)
{
    if ( pBuffer == NULL )
    {
        return NULL;
    }
    else
    {
        return (PBYTE8)(pBuffer->addr);
    }
}

CPciDataRecvThreadMaster::CPciDataRecvThreadMaster()
{
    m_dwLastTime = 0;
}

CPciDataRecvThreadMaster::~CPciDataRecvThreadMaster()
{
}

void CPciDataRecvThreadMaster::SetPciParam(PCI_PARAM_MASTER cPciParam)
{
    m_cPciCmdProcess.m_cPciParam = cPciParam;
    if(m_cPciCmdProcess.m_cPciParam.pRecordLink)
    {
        SEND_RECORD_PARAM param;
        param.iOutputOnlyPeccancy = m_cPciCmdProcess.m_pResultSenderParam->iOutputOnlyPeccancy;
        param.iSendRecordSpace = m_cPciCmdProcess.m_pResultSenderParam->iSendRecordSpace;
        m_cPciCmdProcess.m_cPciParam.pRecordLink->SetParam(&param);
    }
    if(m_cPciCmdProcess.m_cPciParam.pVideoLink)
    {
        SEND_VIDEO_PARAM param;
        param.iSendHisVideoSpace = m_cPciCmdProcess.m_pResultSenderParam->iSendHisVideoSpace;
        m_cPciCmdProcess.m_cPciParam.pVideoLink->SetParam(&param);
    }
}

void CPciDataRecvThreadMaster::EnableRecordSend()
{
    m_cPciCmdProcess.m_fSendRecord = TRUE;
}

void CPciDataRecvThreadMaster::DisableRecordSend()
{
#ifdef _HVCAM_PLATFORM_RTM_
    m_cPciCmdProcess.m_fSendRecord = FALSE;
#else
    // 在非RTM版本下，即使加密认证失败也照常发送识别结果。
    m_cPciCmdProcess.m_fSendRecord = TRUE;
#endif
}

HRESULT CPciDataRecvThreadMaster::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    HRESULT hr = S_OK;
    DWORD32 dwCurTick = GetSystemTick();
    if ( m_dwLastTime != 0
            && (dwCurTick >= m_dwLastTime)
            && (dwCurTick - m_dwLastTime) > CPU_THROB_TIMEOUT )
    {
        hr = E_FAIL;
        HV_Trace(5, "<CPciDataRecvThreadMaster> GetCurStatus Error! "
                 "dwCurTick=[%d]ms, m_dwPciLastRecvTime=[%d]ms.\n",
                 dwCurTick, m_dwLastTime);
    }
    else if (m_cPciCmdProcess.IsThreadOk() != S_OK)
    {
        hr = E_FAIL;
    }
    return hr;
}

HRESULT CPciDataRecvThreadMaster::Run(void* pvParam)
{
    int nPciHandle;
    DWORD32 dwMsg;

    BOOL fIsCmdCorrect = TRUE;
    m_cPciCmdProcess.Start(NULL);

	while (!m_fExit)
	{
        if (g_cHvPciLinkApi.RecvMsg(&nPciHandle, &dwMsg) != S_OK)
        {
            HV_Trace(5, "Recv message failed!");
            HV_Sleep(50);
            continue;
        }

        if (nPciHandle <= 0 || nPciHandle > MAX_PCI_BUF_COUNT)
        {
            HV_Sleep(50);
            continue;
        }

        fIsCmdCorrect = TRUE;
        switch (dwMsg)
        {
            case NETLINK_TYPE_PCI_GETR:
            {
                m_cPciCmdProcess.PutOneCmd(nPciHandle);
                break;
            }

            case NETLINK_TYPE_PCI_RTN_VALUE:
            {
                g_cHvPciLinkApi.PutReturnValue(nPciHandle);
                break;
            }

            default:
            {
                fIsCmdCorrect = FALSE;
                break;
            }
        }

        if (fIsCmdCorrect)
        {
            m_dwLastTime = GetSystemTick();
        }
	}

    return S_OK;
}
