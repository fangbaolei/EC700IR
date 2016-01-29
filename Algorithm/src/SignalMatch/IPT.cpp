#include "IPT.h"
#include "IPTControlImpl.h"
#include "HvPciLinkApi.h"

#define MAX_IPT_PORT_NUM	4	        //最多4个IPT口
#define IPT_CLOCK_SYNC_TIME	43200000	// IPT时钟同步时间(12个小时)
#define SAFE_DELETE_OBJ(pObj)						\
if (pObj)										    \
{													\
	delete pObj;									\
	pObj = NULL;									\
}

bool g_fResetIPT = true;
CIPT::CIPT()
{
// m_iStackSize = 32 * 1024;
    m_bInitialize = false;
    m_pComm = NULL;
    m_pIPTControl = NULL;
    m_pIPTDevice = NULL;
    memset(m_rgbDeviceParam, 0, sizeof(m_rgbDeviceParam));
    m_iDeviceParamLen = 0;
    m_dwLastSyncTime = 0;
}
CIPT::~CIPT()
{
    SAFE_DELETE_OBJ(m_pIPTDevice);
    SAFE_DELETE_OBJ(m_pIPTControl);
    SAFE_DELETE_OBJ(m_pComm);
}

bool CIPT::Initialize(const BYTE8 * pbData, int size)
{
    if (!m_bInitialize)
    {
        size = 260;
        m_iDeviceParamLen = size;
        memcpy(m_rgbDeviceParam, pbData, size);
        HV_Trace(5, "CIPT::Initialize size = %d\n", size);
        m_bInitialize = true;
    }
    return m_bInitialize;
}

void CIPT::UnInitialize(void)
{
    Stop();
}
/*
#pragma pack(push) // 将当前pack设置压栈保存
#pragma pack(1) // 必须在结构体定义之前使用

typedef struct
{
    uint8_t			    CoilType;
    uint16_t			Distance;
    uint8_t				Speedc;
    uint8_t				SpeedThreshold;
    uint8_t				JitterDelay;
    uint8_t				TimeSnap;
    uint8_t				End[2];
    uint8_t				Reserve[7];
}sCoilConfig;

typedef struct
{
    uint8_t DeviceSetEnable[3];
    uint8_t Reserve;
    struct
    {
        uint8_t Reserve1[16];
        sCoilConfig CoilConfig;
        uint8_t Reserve3[16];
        uint8_t Reserve4[16];
    }DeviceConfigSet[3];
    struct
    {
        uint8_t Reserve1[64];
    };
}sExtDeviceConfigCollection;

#pragma pack(pop) // 恢复先前的pack设置
*/
HRESULT CIPT::Run(void* pvParam)
{
    int iTimes = 0;
    while (!m_bInitialize)
    {
        if (!(iTimes % 60))
        {
            HV_Trace(5, "IPT waiting initialize.\n");
        }
        iTimes++;
        HV_Sleep(1000);
    }
    HV_Trace(5, "IPT thread start work.\n");
#ifdef _CAMERA_PIXEL_500W_
    for (;;)
    {
        HV_Sleep(1000);
    }
#endif
    //判断IO输出是否使能
    BOOL fIOOut = ((m_rgbDeviceParam[0] & 0x4) || (m_rgbDeviceParam[1] & 0x4) || (m_rgbDeviceParam[2] & 0x4));
    int iRet = g_cCameraController.SetControlBoardIOOpt(fIOOut);
    HV_Trace(5, "SetControlBoardIOOpt = %d, hr = %d\n", fIOOut, iRet);
    //打印结构题，看赋值是否合理
    char szData[256] = "";
    //1、先打印启用的设备,4个字节
    HV_Trace(5, "0x%02x 0x%02x 0x%02x 0x%02x", m_rgbDeviceParam[0], m_rgbDeviceParam[1], m_rgbDeviceParam[2], m_rgbDeviceParam[3]);
    //2、每16个字节打印一次
    for(int i = 0; i < 256; i++)
    {
        if(!i || (i % 16))
        {
            sprintf(szData + strlen(szData), "0x%02x ", m_rgbDeviceParam[i + 4]);
        }
        else
        {
            HV_Trace(5, szData);
            sprintf(szData, "0x%02x ", m_rgbDeviceParam[i + 4]);
        }
    }
    if(strlen(szData) > 0)
    {
        HV_Trace(5, szData);
        strcpy(szData, "");
    }
   // 创建IPT设备对象
    HV_Trace(5, "create CreateIComm");
    if (m_pComm == NULL)
    {
        RTN_HR_IF_FAILED(CreateIComm(&m_pComm, "/dev/ttyS0"));
    }
    HV_Trace(5, "create CreateIIPTControl");
    if (m_pIPTControl == NULL)
    {
        RTN_HR_IF_FAILED(CreateIIPTControl(&m_pIPTControl, m_pComm));
    }
    HV_Trace(5, "create CreateIPTDevice");
    if (m_pIPTDevice == NULL)
    {
        RTN_HR_IF_FAILED(CreateIPTDevice(&m_pIPTDevice, m_pIPTControl));
    }
    // 同步信号
    HRESULT hr(S_OK);
    BYTE8 * pbData = new BYTE8[100];
    BYTE8 * buf = pbData;
    BYTE8 * bEventId = buf;
    buf++;
    BYTE8 * bType = buf;
    buf++;
    BYTE8 * bEventDeviceType = buf;
    buf++;
    BYTE8 * dwTimePos = buf;
    buf += 4;
    DWORD32 dwTime;
    BYTE8 * rgbData = buf;
    DWORD32 dwSize;
    DWORD32 dwCurTime;

    bool fIsConnected = false;
    int iSpaceTimeMs = 150;
    while (!m_fExit)
    {
        HV_Sleep(iSpaceTimeMs);			// 每次休眠 iSpaceTimeMs MS
        m_dwIPTThreadIsOkTime = GetSystemTick();
        dwCurTime = m_dwIPTThreadIsOkTime;
        if ((dwCurTime - m_dwLastSyncTime) > IPT_CLOCK_SYNC_TIME || (!fIsConnected) || (g_fResetIPT))
        {
            g_fResetIPT = false;
            // 同步时间
            hr = IPT_SetTime(dwCurTime, m_pIPTControl);
            if (SUCCEEDED(hr))
            {
                m_dwLastSyncTime = dwCurTime;
                if ( S_OK != IPT_WriteDeviceWorkParam(m_rgbDeviceParam, m_rgbDeviceParam + 4, m_iDeviceParamLen - 4, m_pIPTControl) )
                {
                    HV_Trace(5, "Initialized IPT WriteDeviceWorkParam error! SetParam error, len = %d!\n", m_iDeviceParamLen - 4);
                }
                else
                {
                    fIsConnected = true;
                    HV_Trace(5, "IPT WriteDeviceWordParam ok ...\n");
                }
                HV_Sleep(10);
            }
            else
            {
                HV_Trace(5, "IPT_SetTime failed");
                HV_Sleep(1000);
                continue;
            }
        }
        // 读取IPT事件队列
        dwSize = 64;
        hr = IPT_ReadDeviceEvent(bEventId, bType, bEventDeviceType, &dwTime, rgbData, dwSize, &dwSize, m_pIPTControl);
        if (FAILED(hr))
        {
            HV_Trace(5, "read event failed.\n");
            // 读取事件失败则退出重连
            fIsConnected = false;
            HV_Sleep(1000);     
            continue;
        }
        if (*bEventId >= MAX_IPT_PORT_NUM)
        {
            continue;
        }
        strcpy(szData, "");
        for(int i = 0; i < (int)dwSize; i++)
        {
            sprintf(szData + strlen(szData), "data[%d]=%d ", i, rgbData[i]);
        }
        HV_Trace(5, "id=0x%02x, bType =0x%02x, device type = 0x%02x, time=%d, %s size = %d", *bEventId, *bType, *bEventDeviceType, dwTime, szData, dwSize);
        //通过PCI发送到从端
        memcpy(dwTimePos, &dwTime, sizeof(DWORD32));
        dwSize += 7;
        //红绿灯的状态信息，只有变化才发送
        if(*bType == 8 && *bEventDeviceType == 0x0b)
        {
            static BYTE8 data = 0;
            if(rgbData[0] == data)
            {
                continue;
            }
            data = rgbData[0];
        }
        HV_Trace(5, "send signal to slave, EventId=%d, Type=%d, EventDeviceType=%d, Time=%d.\n", *bEventId, *bType, *bEventDeviceType, dwTime);
        g_cHvPciLinkApi.SendData(PCILINK_PCIIPT, pbData, dwSize);
    }
    delete []pbData;
    return S_OK;
}
