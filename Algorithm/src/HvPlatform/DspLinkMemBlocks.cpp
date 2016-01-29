// 该文件编码必须是WINDOWS-936格式
#include "DspLinkMemBlocks.h"
#include "hvtarget_ARM.h"
#include "HvPciLinkApi.h"

#ifdef SINGLE_BOARD_PLATFORM
    // 单板
    #define SHARE_MEM_START_ADDR 0x86000000
    #define SHARE_MEM_END_ADDR 0x8b800000
#else
    // 双板
    #ifdef _CAM_APP_
        // 一体机主
        #ifdef _CAMERA_PIXEL_500W_
            // 500w
            #define SHARE_MEM_START_ADDR 0x84000000 // ARM:64M
            #define SHARE_MEM_END_ADDR 0x8d800000
        #else
            // 200w
            #define SHARE_MEM_START_ADDR 0x88000000 // ARM:128M
            #define SHARE_MEM_END_ADDR 0x8b800000
        #endif
    #else
        // 一体机从
        #define SHARE_MEM_START_ADDR 0x85000000
        #define SHARE_MEM_END_ADDR 0x8c000000
    #endif // _CAM_APP_
#endif // SINGLE_BOARD_PLATFORM

static HV_SEM_HANDLE g_hSemShareMemPool;
static MemBlocksInfo g_rgcMemBlocksInfo[MAX_MEMBLOCKS_COUNT];
static CDspLinkMemBlock* g_rgpDspLinkMemBlock[MAX_MEMBLOCKS_COUNT];

// -----------------------------------------------------------------------------------
// 200w一体机主CPU内存使用模式：
// |128M|56M|8M|64M|
// |ARM端|SHARE|硬盘DMA|DSP端|
// 另注：SHARE中有2个1M分别用于DSPLink的输入块和输出块，1个1M用于DSPLink消息传递块。
// -----------------------------------------------------------------------------------
// 200w一体机从CPU内存使用模式：
// |64M|8M|8M|112M|64M|
// |ARM端|PCI读｜PCI写|SHARE|DSP端|
// 另注：SHARE中有2个1M分别用于DSPLink的输入块和输出块，1个1M用于DSPLink消息传递块。
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// 单板CPU内存使用模式：
// |96M|88M|8M|64M|
// |ARM端|SHARE|硬盘DMA|DSP端|
// 另注：SHARE中有2个1M分别用于DSPLink的输入块和输出块，1个1M用于DSPLink消息传递块。
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// 500w一体机主CPU内存使用模式：
// |64M|152M|8M|32M|
// |ARM端|SHARE|硬盘DMA|DSP端|
// 另注：SHARE中有2个1M分别用于DSPLink的输入块和输出块，1个1M用于DSPLink消息传递块。
// -----------------------------------------------------------------------------------
// 500w一体机从CPU内存使用模式：
// |64M|8M|8M|112M|64M|
// |ARM端|PCI读｜PCI写|SHARE|DSP端|
// 另注：SHARE中有2个1M分别用于DSPLink的输入块和输出块，1个1M用于DSPLink消息传递块。
// -----------------------------------------------------------------------------------

/**
* 将DSPLink共享内存初始化为指定模式
*/
HRESULT InitDspLinkMemBlocks(SHARE_MEM_MODE t)
{
    HRESULT hr = E_FAIL;

    // Comment by Shaorg: dwBlockSize必须按从小到大的顺序排列，否则将照成内存浪费！
    if ( CAMAPP_FULLPLATFORM_HVCAM == t )
    {
        // 200w一体机内存分配模式（运行在一体式相机协议下）
        MemBlocksInfo rgcMemBlocksInfo[3];
        // 用途：HV_IMAGE_JPEG, HV_IMAGE_H264, HV_IMAGE_BT1120_ROTATE_Y
        rgcMemBlocksInfo[0].dwBlockCount = 14;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 1024 * 1;
        // 用途：HV_IMAGE_BT1120_UV
        rgcMemBlocksInfo[1].dwBlockCount = 12;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 2;
        // 用途：识别结果
        rgcMemBlocksInfo[2].dwBlockCount = 6;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2.5;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( CAMAPP_FULLPLATFORM_HVCAM_500W_PIXEL == t )
    {
        // 500w一体机内存分配模式（运行在一体式相机协议下）
        MemBlocksInfo rgcMemBlocksInfo[4];
        rgcMemBlocksInfo[0].dwBlockCount = 8;
        rgcMemBlocksInfo[0].dwBlockSize = 768 * 1024;
        rgcMemBlocksInfo[1].dwBlockCount = 35;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[2].dwBlockCount = 2;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 3;
        rgcMemBlocksInfo[3].dwBlockCount = 20;
        rgcMemBlocksInfo[3].dwBlockSize = 1024 * 1024 * 5;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( CAMAPP_FULLPLATFORM_OTHER == t )
    {
        // 200w一体机内存分配模式（运行在测试类协议下）
        MemBlocksInfo rgcMemBlocksInfo[2];
        rgcMemBlocksInfo[0].dwBlockCount = 8;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[1].dwBlockCount = 15;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 3;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( CAMAPP_FULLPLATFORM_OTHER_500W_PIXEL == t )
    {
        // 500w一体机内存分配模式（运行在测试类协议下）
        MemBlocksInfo rgcMemBlocksInfo[2];
        rgcMemBlocksInfo[0].dwBlockCount = 4;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 1024 * 2;
        rgcMemBlocksInfo[1].dwBlockCount = 15;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 3;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( CAMAPP_CAMERAONLY == t )
    {
        // 单200w相机内存分配模式
        MemBlocksInfo rgcMemBlocksInfo[2];
        rgcMemBlocksInfo[0].dwBlockCount = 17;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[1].dwBlockCount = 18;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 2;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( CAMAPP_CAMERAONLY_500W_PIXEL == t )
    {
        // 单500w相机内存分配模式
        MemBlocksInfo rgcMemBlocksInfo[3];
        rgcMemBlocksInfo[0].dwBlockCount = 19;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[1].dwBlockCount = 15;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 2;
        rgcMemBlocksInfo[2].dwBlockCount = 20;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 5;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_NORMAL == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[3];
        rgcMemBlocksInfo[0].dwBlockCount = 64;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 73;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[2].dwBlockCount = 16;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_CAP_FACE == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[3];
        rgcMemBlocksInfo[0].dwBlockCount = 144;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 68;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[2].dwBlockCount = 16;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_CAP_FACE_500W == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[3];
        rgcMemBlocksInfo[0].dwBlockCount = 144;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 37;
        rgcMemBlocksInfo[1].dwBlockSize = DWORD32(1024 * 1024 * 2.4);
        rgcMemBlocksInfo[2].dwBlockCount = 2;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 4;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_EDDY == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[3];
        rgcMemBlocksInfo[0].dwBlockCount = 64;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 45;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[2].dwBlockCount = 30;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_OTHER == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[5];
        rgcMemBlocksInfo[0].dwBlockCount = 96;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 59;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024 * 1;
        rgcMemBlocksInfo[2].dwBlockCount = 5;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2;
        rgcMemBlocksInfo[3].dwBlockCount = 6;
        rgcMemBlocksInfo[3].dwBlockSize = 1024 * 1024 * 4;
        rgcMemBlocksInfo[4].dwBlockCount = 1;
        rgcMemBlocksInfo[4].dwBlockSize = 1024 * 1024 * 10;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    //200w电子警察共享内存单独分配,黄国超增加,2011-10-10
    else if ( LPRAPP_EPOLICE_S == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[4];
        rgcMemBlocksInfo[0].dwBlockCount = 96;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 47;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024;
        rgcMemBlocksInfo[2].dwBlockCount = 2;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2;
        //1个存放解码后的图片，6个存放识别结果
        rgcMemBlocksInfo[3].dwBlockCount = 7;
        rgcMemBlocksInfo[3].dwBlockSize = 1024 * 1024 * 4;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_EPOLICE_500W == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[3];
        rgcMemBlocksInfo[0].dwBlockCount = 64;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 40;
        rgcMemBlocksInfo[1].dwBlockSize = DWORD32(1024 * 1024 * 2.4);
        rgcMemBlocksInfo[2].dwBlockCount = 2;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 4;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_SINGLE_BOARD_PLATFORM == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[4];
        rgcMemBlocksInfo[0].dwBlockCount = 96;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 64;
        rgcMemBlocksInfo[1].dwBlockCount = 41;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 1024;
        rgcMemBlocksInfo[2].dwBlockCount = 7;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024 * 2;
        rgcMemBlocksInfo[3].dwBlockCount = 6;
        rgcMemBlocksInfo[3].dwBlockSize = 1024 * 1024 * 4;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }
    else if ( LPRAPP_CAP_FACE_S == t )
    {
        MemBlocksInfo rgcMemBlocksInfo[6];
        rgcMemBlocksInfo[0].dwBlockCount = 256;
        rgcMemBlocksInfo[0].dwBlockSize = 1024 * 4;
        rgcMemBlocksInfo[1].dwBlockCount = 256;
        rgcMemBlocksInfo[1].dwBlockSize = 1024 * 32;
        rgcMemBlocksInfo[2].dwBlockCount = 40;
        rgcMemBlocksInfo[2].dwBlockSize = 1024 * 1024;
        // 2M和3.5M用于存放300W解码成YUV422后的图片
        rgcMemBlocksInfo[3].dwBlockCount = 4;
        rgcMemBlocksInfo[3].dwBlockSize = 1024 * 1024 * 2;
        rgcMemBlocksInfo[4].dwBlockCount = 1;
        rgcMemBlocksInfo[4].dwBlockSize = (DWORD32)(1024 * 1024 * 3.5);
        // 1个存放非300W解码成CbYCrY后的图片，6个存放识别结果
        rgcMemBlocksInfo[5].dwBlockCount = 6;
        rgcMemBlocksInfo[5].dwBlockSize = 1024 * 1024 * 4;
        hr = InitMemBlocks(rgcMemBlocksInfo, ARRSIZE(rgcMemBlocksInfo));
    }

    return hr;
}

/**
* DSPLink共享内存初始化
*/
HRESULT InitMemBlocks(
    MemBlocksInfo* rgcMemBlocksInfo,
    DWORD32 dwCount
)
{
    if ( dwCount > MAX_MEMBLOCKS_COUNT )
    {
        return E_INVALIDARG;
    }

    if (HiVideo::CreateSemaphore(&g_hSemShareMemPool, 1, 1) != 0)
    {
        HV_Trace(1, "create m_hSemCreateShareMemOnPool is failed!\n");
        return E_FAIL;
    }

    for ( DWORD32 i = 0; i < dwCount; ++i )
    {
        g_rgcMemBlocksInfo[i] = rgcMemBlocksInfo[i];
        g_rgpDspLinkMemBlock[i] = new CDspLinkMemBlock(rgcMemBlocksInfo[i].dwBlockCount, rgcMemBlocksInfo[i].dwBlockSize);
        if ( NULL == g_rgpDspLinkMemBlock[i] )
        {
            return E_FAIL;
        }
    }

    char szText[256];

#ifdef SINGLE_BOARD_PLATFORM
    if (HV_System("rmmod /root/cmemk.ko"))
    {
    	HV_Trace(5, "rmmod /root/cmemk.ko error.\n");
    	// TODO:共享内存驱动存在卸载失败的情况，该情况并不影响使用，
    	// 只要确保加载驱动成功即可，因此将return E_FAIL;屏蔽掉
    	// huanggr 2011-11-21
    	//return E_FAIL;
    }
#else
    if (HV_System("rmmod /lib/modules/cmemk.ko"))
    {
    	HV_Trace(5, "rmmod /lib/modules/cmemk.ko error.\n");
    	// TODO:共享内存驱动存在卸载失败的情况，该情况并不影响使用，
    	// 只要确保加载驱动成功即可，因此将return E_FAIL;屏蔽掉
    	// huanggr 2011-11-21
    	//return E_FAIL;
    }
#endif

    sprintf(
        szText,
#ifdef SINGLE_BOARD_PLATFORM
        "insmod /root/cmemk.ko phys_start=0x%08x phys_end=0x%08x, pools=%dx%d",
#else
        "insmod /lib/modules/cmemk.ko phys_start=0x%08x phys_end=0x%08x, pools=%dx%d",
#endif
        SHARE_MEM_START_ADDR, SHARE_MEM_END_ADDR,
        3, 1024*1024*1 /* DSPLink专用的3个1M的共享内存块 */
    );

    char szTmp[32];
    for ( DWORD32 i = 0; i < dwCount; ++i )
    {
        sprintf(szTmp, ",%dx%d", rgcMemBlocksInfo[i].dwBlockCount, rgcMemBlocksInfo[i].dwBlockSize);
        strcat(szText, szTmp);
    }

    if (HV_System(szText) != 0)
    {
        return E_FAIL;
    }
    else
    {
        // 初始化CMEM模块
        int iRetryTimes = 0;
        while ( 0 != CMEM_init() )
        {
            if ( iRetryTimes++ < 10 )
            {
                HV_Trace(5, "waiting CMEM_init...\n");
                WdtHandshake();
                HV_Sleep(500);
            }
            else
            {
                HV_Exit(HEC_FAIL|HEC_RESET_DEV, "CMEM_init is failed!");
            }
        }
        CDspLinkMemBlock::Initialize();
        HV_Trace(5, "CMEM_init is succ.\n");

        // Comment by Shaorg: 把所有共享内存都分配好并放入应用层的共享内存池。
        DWORD32 i = 0;
        DWORD32 j = 0;
        DSPLinkBuffer* rgcDSPLinkBuffer = NULL;

        for ( i = 0; i < dwCount; ++i )
        {
            rgcDSPLinkBuffer = new DSPLinkBuffer[g_rgcMemBlocksInfo[i].dwBlockCount];

            for ( j = 0; j < g_rgcMemBlocksInfo[i].dwBlockCount; ++j )
            {
                CreateShareMemOnPool(&rgcDSPLinkBuffer[j], g_rgcMemBlocksInfo[i].dwBlockSize);
                if ( NULL == rgcDSPLinkBuffer[j].addr )
                {
                    HV_Trace(5, "CreateShareMemOnPool is Error.[%d]\n", g_rgcMemBlocksInfo[i].dwBlockSize);

                    char szInfoTmp[64];
                    sprintf(szInfoTmp, "CreateShareMemOnPool is Error.[%d]\n", g_rgcMemBlocksInfo[i].dwBlockSize);
                    HV_Exit(HEC_FAIL|HEC_RESET_DEV, szInfoTmp);
                }
            }

            for ( j = 0; j < g_rgcMemBlocksInfo[i].dwBlockCount; ++j )
            {
                FreeShareMemOnPool(&rgcDSPLinkBuffer[j]);
            }

            SAFE_DELETE_ARRAY(rgcDSPLinkBuffer);
        }

        HV_Trace(5, "InitMemBlocks\n");
        ShareMemPoolStatusShow();
        return S_OK;
    }
}

/**
* DSPLink共享内存申请接口
*/
void CreateShareMemOnPool2(DSPLinkBuffer *pObj, DWORD32 dwLen)
{
    if ( NULL == pObj ) return;

    HiVideo::SemPend(&g_hSemShareMemPool);

    HRESULT hr = E_FAIL;

    //先清零，避免内存重用，2011-12-21
    memset(pObj, 0, sizeof(DSPLinkBuffer));

    int i = 0;
    while ( g_rgcMemBlocksInfo[i].dwBlockCount > 0 )
    {
        if ( dwLen <= g_rgcMemBlocksInfo[i].dwBlockSize )
        {
            g_rgpDspLinkMemBlock[i]->AllocMem(pObj);
            hr = S_OK;
            break;
        }

        i++;
    }

    if ( hr != S_OK )
    {
        HV_Trace(1, "Warning:try to alloc an oversize mem:%d\n", dwLen);
#ifndef _CAM_APP_
        char szMsg[255];
        sprintf(szMsg, "create share memory %d failed.", dwLen);
        PciSendSlaveDebugInfo(szMsg);
#endif
    }

    HiVideo::SemPost(&g_hSemShareMemPool);
}

/**
* DSPLink共享内存释放接口
*/
void FreeShareMemOnPool(DSPLinkBuffer *pObj)
{
    if ( NULL == pObj || NULL == pObj->addr ) return;

    HiVideo::SemPend(&g_hSemShareMemPool);

    HRESULT hr = E_FAIL;

    int i = 0;
    while ( g_rgcMemBlocksInfo[i].dwBlockCount > 0 )
    {
        if ( pObj->len <= g_rgcMemBlocksInfo[i].dwBlockSize )
        {
            g_rgpDspLinkMemBlock[i]->FreeMem(pObj);
            hr = S_OK;
            break;
        }

        ++i;
    }

    if ( hr != S_OK )
    {
        HV_Trace(1, "Warning:try to free an oversize mem:%d\n", pObj->len);
    }

    HiVideo::SemPost(&g_hSemShareMemPool);
}

bool CDspLinkMemBlock::m_fInitialize = false;
CDspLinkMemBlock::CDspLinkMemBlock(DWORD32 dwBlockCount, DWORD32 dwBlockSize)
{
    m_dwBlockCount = dwBlockCount;
    m_dwBlockSize = dwBlockSize;
    m_iMemIndex = -1;
    m_prgMemFrame = new DSPLinkBuffer[m_dwBlockCount];
    for (DWORD32 i = 0; i < m_dwBlockCount; i++)
    {
        m_prgMemFrame[i].addr = NULL;
        m_prgMemFrame[i].len = 0;
    }
}

CDspLinkMemBlock::~CDspLinkMemBlock()
{
    for (DWORD32 i = 0; i < m_dwBlockCount; i++)
    {
        if (m_prgMemFrame[i].addr != NULL)
        {
            FreeDSPBuffer(&m_prgMemFrame[i]);
        }
    }
    if (m_prgMemFrame)
    {
        delete[] m_prgMemFrame;
        m_prgMemFrame = NULL;
    }
}

void CDspLinkMemBlock::Initialize(void)
{
    m_fInitialize = true;
}

void CDspLinkMemBlock::AllocMem(DSPLinkBuffer *pObj)
{
    if ( NULL == pObj || !m_fInitialize) return;

    if (m_iMemIndex < 0)
    {
        CreateDSPBuffer(pObj, m_dwBlockSize);
    }
    else if (m_prgMemFrame[m_iMemIndex].addr != NULL)
    {
        *pObj = m_prgMemFrame[m_iMemIndex];
        m_iMemIndex--;
    }
    else
    {
        HV_Trace(5, "Error:share memory pool occur abnormality [%d, %d] ...\n", m_iMemIndex, m_dwBlockSize);

        char szInfoTmp[128];
        sprintf(szInfoTmp, "Error:share memory pool occur abnormality [%d, %d] ...\n", m_iMemIndex, m_dwBlockSize);
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, szInfoTmp);
    }
}

void CDspLinkMemBlock::FreeMem(DSPLinkBuffer *pObj)
{
    if ( NULL == pObj || NULL == pObj->addr || !m_fInitialize) return;

    if (m_iMemIndex >= (int)m_dwBlockCount - 1)
    {
        HV_Trace(5, "Warning:share memory pool is full:%d, %d, %d...\n", m_iMemIndex, m_dwBlockSize, pObj->len);
        FreeDSPBuffer(pObj);
    }
    else
    {
        m_prgMemFrame[++m_iMemIndex] = *pObj;
    }
}

void ShareMemPoolStatusShow()
{
    int i = 0;
    HV_Trace(5, "<<<\n");
    while ( g_rgcMemBlocksInfo[i].dwBlockCount > 0 )
    {
        HV_Trace(5, "c = %d, size = %d, idx = %d.\n",
                 g_rgpDspLinkMemBlock[i]->m_dwBlockCount,
                 g_rgpDspLinkMemBlock[i]->m_dwBlockSize,
                 g_rgpDspLinkMemBlock[i]->m_iMemIndex);
        ++i;
    }
    HV_Trace(5, ">>>\n");
}
