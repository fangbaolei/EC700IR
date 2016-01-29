#include "HvPciLinkApi.h"
#include "misc.h"

CPciDataRecvThreadSlave::CPciDataRecvThreadSlave()
        : m_dwLastTime(0)
{
}

CPciDataRecvThreadSlave::~CPciDataRecvThreadSlave()
{
}

void CPciDataRecvThreadSlave::SetPciParam(PCI_PARAM_SLAVE cPciParam)
{
    m_cPciCmdProcess.m_cPciParam = cPciParam;
}

BOOL CPciDataRecvThreadSlave::PciLinkShakeHands()
{
    HV_Trace(5, "<CPciDataRecvThreadSlave> PciShakeHands Start.\n");
    struct mytv
    {
        int iTick;
        int fMaster;
    }tmp;

    for (int i = 0; i < 10000; ++i)
    {
        if (m_cPciCmdProcess.m_fShakeHandsSucceed)
        {
            break;
        }
        HV_Sleep(50);

        if (i % 200 == 0)
        {
            tmp.iTick = GetSystemTick();
            tmp.fMaster = 0;
            WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));
        }
    }
    HV_Trace(5, "<CPciDataRecvThreadSlave> PciShakeHands End [%s].\n",
             (1 == m_cPciCmdProcess.m_fShakeHandsSucceed) ? ("S_OK") : ("E_FAIL"));

    return m_cPciCmdProcess.m_fShakeHandsSucceed;
}

BOOL CPciDataRecvThreadSlave::DataCtrlHandshake()
{
    HV_Trace(5, "<CPciDataRecvThreadSlave> DataCtrlHandshake Start.\n");

    struct mytv
    {
        int iTick;
        int fMaster;
    }tmp;

    for (int i = 0; i < 200; ++i)
    {
        if (m_cPciCmdProcess.m_fDataCtrlHandsSucceed)
        {
            break;
        }
        HV_Sleep(100);
        if(!(i % 100))
        {
            tmp.iTick = GetSystemTick();
            tmp.fMaster = 0;
            WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));
        }
    }
    HV_Trace(5, "<CPciDataRecvThreadSlave> DataCtrlHandshake End [%s].\n",
             m_cPciCmdProcess.m_fDataCtrlHandsSucceed ? ("S_OK") : ("E_FAIL"));

    return m_cPciCmdProcess.m_fDataCtrlHandsSucceed;
}

HRESULT CPciDataRecvThreadSlave::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    HRESULT hr = S_OK;

    /* Comment by Shaorg: 临时解决PCI由主至从的通道断开问题。
    DWORD32 dwCurTick = GetSystemTick();
    if ( m_dwLastTime != 0
            && (dwCurTick >= m_dwLastTime)
            && (dwCurTick - m_dwLastTime) > CPU_THROB_TIMEOUT )
    {
        hr = E_FAIL;
        HV_Trace(5, "<CPciDataRecvThreadSlave> GetCurStatus Error! "
                 "dwCurTick=[%d]ms, m_dwPciLastRecvTime=[%d]ms.\n",
                 dwCurTick, m_dwLastTime);
    }
    */

    if (m_cPciCmdProcess.IsThreadOk() != S_OK)
    {
        hr = E_FAIL;
    }

    return hr;
}

HRESULT CPciDataRecvThreadSlave::Run(void* pvParam)
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

        fIsCmdCorrect = FALSE;
        switch (dwMsg)
        {
            case NETLINK_TYPE_PCI_GETR:
            {
                fIsCmdCorrect = TRUE;
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

