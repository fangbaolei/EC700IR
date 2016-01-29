// 该文件编码必须是WINDOWS-936格式
/*******************************************************************************

log:
1. 2012-04-19 lijj
   添加保存点阵到EEPROM功能。
2. 2012-05-10 lijj
   字符叠加新方案，时间点阵存储在FPGA中，一体机只发时间数值，不发时间点阵。
   时间精确到毫秒，时间信息在图像最后一行Y值中(详细参考硬件文档)
   汉字最多为(width-25*12)/24，如200W图像宽为1600，则汉字最多是54个。EEPROM
   保存的汉字最多为56个。
3. 2012-10-22 lijj
   字符叠加新机制：在DSP端进行叠加(原机制是在FPGA实现)，对外接口不变，此文件
   由线程类改为普通类。
*******************************************************************************/

#include "config.h"
// 一体机主，或简化版
#if defined(_HVCAM_LITE_PLATFORM_) || defined(_CAM_APP_)

#include "CharacterOverlap.h"
#include "hvtarget_ARM.h"

// 简化版一体机除纯相机外其它分型全局模块参数结构体
#if defined(_HVCAM_LITE_PLATFORM_) && !defined(_CAM_APP_)
#include "HvParamInc.h" //  g_cModuleParams
#endif

// 纯相机全局模块参数结构体
#if defined(_CAM_APP_)
#include "../CamApp/LoadParam.h"
#endif

extern HV_SEM_HANDLE g_hSemEDMA;

extern "C" void DotMatrixConvert(unsigned char* old_lattice, unsigned char* new_lattice, int width, int height);

/* LoadParam.cpp */
extern CParamStore g_cParamStore;

// EEPROM保存点阵数据
// 注：在EEPROM中预留4KB存储点阵数据，最大能存储的数量取决于点阵的大小。
// 计算公式：4096/(W*W/8)，其中W为点阵字符的宽(与高相同)
// 例：24*24点阵最多能存储56个汉字，或112个半角字符
//     32*32点阵最多能存储32个汉字，或64个半角字符
#define EEPROM_CHARACTER_LEN 4096

CCharacterOverlap::CCharacterOverlap()
{
}

CCharacterOverlap::~CCharacterOverlap()
{
}

/**
 * EnableCharacterOverlap - 设置字符叠加使能
 *
 * @param iValue  1: 使能(并发送点阵到DSP端进行叠加)  0：不使能
 *
 * @return E_FAIL：失败   S_OK：成功
 * TODO：找一种更好的方法实现使能与否的实时显示
 */
HRESULT CCharacterOverlap::EnableCharacterOverlap(int iValue)
{
    HRESULT hr = E_FAIL;

    g_cModuleParams.cCamAppParam.iEnableCharacterOverlap = iValue;
    //字符叠加使能判断
    if (g_cModuleParams.cCamAppParam.iEnableCharacterOverlap)
    {
        if (g_cModuleParams.cCamAppParam.iLatticeLen > EEPROM_CHARACTER_LEN)
        {
            g_cModuleParams.cCamAppParam.iLatticeLen = EEPROM_CHARACTER_LEN;
        }
        if (!g_cModuleParams.cCamAppParam.pbLattice && g_cModuleParams.cCamAppParam.iLatticeLen > 0)
        {
            g_cModuleParams.cCamAppParam.pbLattice = new char[g_cModuleParams.cCamAppParam.iLatticeLen];
            memset(g_cModuleParams.cCamAppParam.pbLattice, 0, g_cModuleParams.cCamAppParam.iLatticeLen);
            if (0 > EEPROM_Read(CHARACTER_START_ADDR, (PBYTE8)g_cModuleParams.cCamAppParam.pbLattice, g_cModuleParams.cCamAppParam.iLatticeLen))
            {
                delete []g_cModuleParams.cCamAppParam.pbLattice;
                g_cModuleParams.cCamAppParam.pbLattice = NULL;
            }
        }

        SemPend(&g_hSemEDMA);

        int iFontHeightByte = GetFontHeightByte(g_cModuleParams.cCamAppParam.iFontSize);
        int iFontHeiht = GetFontHeight(g_cModuleParams.cCamAppParam.iFontSize);
        m_cParam.x = g_cModuleParams.cCamAppParam.iX;
        m_cParam.y = g_cModuleParams.cCamAppParam.iY;
        m_cParam.w = g_cModuleParams.cCamAppParam.iLatticeLen/iFontHeightByte; //点阵数据总大小除以高占用的字节数，得出宽度。
        m_cParam.h = iFontHeiht;
        m_cParam.iIsFixedLight = g_cModuleParams.cCamAppParam.iEnableFixedLight;
        m_cParam.iFontColor = g_cModuleParams.cCamAppParam.iFontColor;
        m_cParam.fIsSideInstall = g_cModuleParams.cCamAppParam.fIsSideInstall;

        HV_Trace(5, "<EnableCharacterOverlap> len:%d,size:%d,x:%d,y:%d, w:%d,h:%d, color: %d",
                 g_cModuleParams.cCamAppParam.iLatticeLen,
                 g_cModuleParams.cCamAppParam.iFontSize,
                 g_cModuleParams.cCamAppParam.iX,
                 g_cModuleParams.cCamAppParam.iY,
                 m_cParam.w,
                 m_cParam.h,
                 m_cParam.iFontColor);

        // 申请共享内存
        int nBufferLen = m_cParam.w * m_cParam.h * sizeof(char);
        CreateShareMemOnPool(&m_cData.rgDotMatrix, nBufferLen);

        // 点阵转换
        DotMatrixConvert((PBYTE8)g_cModuleParams.cCamAppParam.pbLattice,
                        (PBYTE8)m_cData.rgDotMatrix.addr,
                        m_cParam.w,
                        m_cParam.h);

        hr = g_cHvDspLinkApi.SendStringOverlayInitCmd(&m_cParam, &m_cData, &m_cRespond);

        // 释放共享内存
        FreeShareMemOnPool(&m_cData.rgDotMatrix);
        SemPost(&g_hSemEDMA);
    }
    else
    {
        SemPend(&g_hSemEDMA);

        m_cParam.x = 0;
        m_cParam.y = 0;
        m_cParam.w = 0;
        m_cParam.h = 0;
        m_cParam.iIsFixedLight = 0;
        m_cParam.iFontColor = 0;

        m_cData.rgDotMatrix.addr = NULL;
        m_cData.rgDotMatrix.phys = 0;
        hr = g_cHvDspLinkApi.SendStringOverlayInitCmd(&m_cParam, &m_cData, &m_cRespond);

        SemPost(&g_hSemEDMA);
    }

    return hr;
}

/**
 * SetCharacterOverlap - 保存上位机发送过来的点阵数据到内存中，
                         并发送到DSP端进行显示
 *
 * @param pLattice 点阵数据缓冲区指针
 *
 * @return S_OK
 */
HRESULT CCharacterOverlap::SetCharacterOverlap(char* pLattice)
{
    if (g_cModuleParams.cCamAppParam.pbLattice != NULL)
    {
        delete []g_cModuleParams.cCamAppParam.pbLattice;
        g_cModuleParams.cCamAppParam.pbLattice = NULL;
    }
    if (g_cModuleParams.cCamAppParam.iLatticeLen > 0)
    {
        g_cModuleParams.cCamAppParam.pbLattice = new char[g_cModuleParams.cCamAppParam.iLatticeLen];
        if (g_cModuleParams.cCamAppParam.pbLattice)
        {
            memcpy(g_cModuleParams.cCamAppParam.pbLattice, pLattice, g_cModuleParams.cCamAppParam.iLatticeLen);
        }
    }
    EnableCharacterOverlap(g_cModuleParams.cCamAppParam.iEnableCharacterOverlap);
    return S_OK;
}

/**
 * SetCharacterLightness - 设置字体亮度是否固定
 *
 * @param iIsFixedLight 1：亮度固定(使用字体的RGB颜色)
 *                      0：亮度不固定(与背景色形成反差，在DSP实现)
 *
 * @return S_OK
 */
HRESULT CCharacterOverlap::SetCharacterLightness(int iIsFixedLight)
{
    g_cModuleParams.cCamAppParam.iEnableFixedLight = iIsFixedLight;
    EnableCharacterOverlap(g_cModuleParams.cCamAppParam.iEnableCharacterOverlap);
    return S_OK;
}
/**
 * SaveCharacterInfo - 保存点阵信息：点阵数据保存到EEPROM，坐标、颜色等保存到XML
 *
 *
 * @return S_OK：执行成功  E_FAIL:保存失败
 */
HRESULT CCharacterOverlap::SaveCharacterInfo(void)
{
    if (g_cModuleParams.cCamAppParam.pbLattice)
    {
        if (g_cModuleParams.cCamAppParam.iLatticeLen > EEPROM_CHARACTER_LEN)
        {
            g_cModuleParams.cCamAppParam.iLatticeLen = EEPROM_CHARACTER_LEN;
        }
        //将点阵数据写到EEPROM里面
        if (EEPROM_Write(CHARACTER_START_ADDR, (PBYTE8)g_cModuleParams.cCamAppParam.pbLattice, g_cModuleParams.cCamAppParam.iLatticeLen) < 0)
        {
            HV_Trace(5, "<SaveCharacterInfo>: WriteCharacterToEEPROM error!\n");
            return E_FAIL;
        }
        HV_Trace(5, "<CCharacterOverlap> Write to EEPROM x:%d, y:%d, size:%d, len: %d\n", g_cModuleParams.cCamAppParam.iX, g_cModuleParams.cCamAppParam.iY, g_cModuleParams.cCamAppParam.iFontSize, g_cModuleParams.cCamAppParam.iLatticeLen);
        g_cParamStore.SetInt("\\CamApp", "Character_X", g_cModuleParams.cCamAppParam.iX);
        g_cParamStore.SetInt("\\CamApp", "Character_Y", g_cModuleParams.cCamAppParam.iY);
        g_cParamStore.SetInt("\\CamApp", "Character_Size", g_cModuleParams.cCamAppParam.iFontSize);
        g_cParamStore.SetInt("\\CamApp", "Character_DateFormat", g_cModuleParams.cCamAppParam.iDateFormat);
        g_cParamStore.SetInt("\\CamApp", "Character_FontColor", g_cModuleParams.cCamAppParam.iFontColor);
        g_cParamStore.SetInt("\\CamApp", "Character_LatticeLen", g_cModuleParams.cCamAppParam.iLatticeLen);
        g_cParamStore.SetInt("\\CamApp", "EnableCharacter", g_cModuleParams.cCamAppParam.iEnableCharacterOverlap);
        g_cParamStore.Save();
        return S_OK;
    }
    return S_OK;
}

/**
 * MakeDateTime - 组装日期时间信息字符串
 *
 * @param pDateTimeString: 日期时间字符串，需调用者开辟空间
 * @param dwTimeLow      : 64位时间低32位
 * @param dwTimeHigh     : 64位时间高32位
 *
 * @return S_OK : 成功   E_FAIL : 失败
 */
HRESULT CCharacterOverlap::MakeDateTimeString(BYTE8* pDateTimeString, DWORD32 dwTimeLow, DWORD32 dwTimeHigh)
{
    if (pDateTimeString == NULL)
        return E_FAIL;
    if (g_cModuleParams.cCamAppParam.iEnableCharacterOverlap)
    {
        REAL_TIME_STRUCT rt;
        ConvertMsToTime(dwTimeLow, dwTimeHigh, &rt);
        if (g_cModuleParams.cCamAppParam.iDateFormat == 0)
        {
            sprintf((char *)pDateTimeString, "%04d/%02d/%02d %02d:%02d:%02d:%03d ", rt.wYear, rt.wMonth, rt.wDay, rt.wHour, rt.wMinute, rt.wSecond, rt.wMSecond);
        }
        else if (g_cModuleParams.cCamAppParam.iDateFormat == 1)
        {
            sprintf((char *)pDateTimeString, "%04d-%02d-%02d %02d:%02d:%02d:%03d ", rt.wYear, rt.wMonth, rt.wDay, rt.wHour, rt.wMinute, rt.wSecond, rt.wMSecond);
        }
    }
    else
    {
        strcpy((char *)pDateTimeString, "");
    }

    return S_OK;
}

#endif
