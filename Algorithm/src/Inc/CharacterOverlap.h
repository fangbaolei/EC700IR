// 该文件编码必须是WINDOWS-936格式
/*******************************************************************************
 关于点阵的小常识：
 一般来说，点阵取模工具中每8个像素组成1个字节，取模式方式分横向和竖向，字节序
 分低位在前和高位在前，因此，共有4种取模方式，本文件使用的方式是竖向取模(代
 码注释写作“逐列式”)，低位在前。为了方便DSP显示，会将该种方式的点阵转换成每
 像素占1字节的新的点阵，从左至右，从上至下排列，为1者显示，为0者不显示。
 如下为字符“1”的12*12点阵(仅作示例，不代表实际情况)：
 000001000000
 000111000000
 000001000000
 000001000000
 000001000000
 000001000000
 000001000000
 000001000000
 000001000000
 000001000000
 000001000000
 000111111000

 如想了解，请使用搜索引擎进行搜索以了解相关知识。
*******************************************************************************/
#ifndef _CHARACTEROVERLAP_H
#define _CHARACTEROVERLAP_H

#include "CameraController.h"
#include "HvTime.h"
#include "DspLinkCmd.h"
#include "HvDspLinkApi.h"

// 时间显示字符长度为strlen("2012-04-19 09:02:10:999  ") = 25
#define TIMEINFO_LEN 25

// n位(或字节)对齐
#define ALIGN(x, n) (((x)+(n)-1)&~((n)-1))

// 注：下列宏使用函数代替，增加可扩展性
//#define CHARACTER_WIDTH 24                          // 24点阵
//#define CHARACTER_HALFWIDTH (CHARACTER_WIDTH/2)     // 半角宽(24点阵为12)
//#define CHARACTER_HEIGHT ALIGN(CHARACTER_WIDTH, 8)  // 高(24点阵为24)
//#define CHARACTER_BYTE ((ALIGN(CHARACTER_WIDTH, 8))/8)  // 点阵高所占用的字节数
//#define FULLWIDTH_SIZE (ALIGN(CHARACTER_WIDTH, 8)*CHARACTER_WIDTH/8)    // 全角占用空间字节
//#define HALFWIDTH_SIZE (ALIGN(CHARACTER_WIDTH, 8)*CHARACTER_WIDTH/2/8)  // 半角占用空间字节

// 获取字体相关属性函数
// 注：iFontSize指的是字体大小，并非点阵数据的大小，
// 如字体大小为24，是指该字体的点阵为24x24

// 获取一个点阵字体的半角宽
inline int GetFontHalfWidth(int iFontSize)
{
    return iFontSize / 2;
}

// 获取一个点阵字体的高
// 注：由于以列为主，所以高必须是8的整数倍
inline int GetFontHeight(int iFontSize)
{
    return ALIGN(iFontSize, 8);
}

// 获取一个点阵字体高所占用字节数
inline int GetFontHeightByte(int iFontSize)
{
    return ALIGN(iFontSize, 8) / 8;
}

// 获取一个点阵字体的半角大小
inline int GetFontHalfWidthSize(int iFontSize)
{
    return ALIGN(iFontSize, 8) * iFontSize / 2 / 8;
}

// 获取一个点阵字体的全角大小
inline int GetFontFullWidthSize(int iFontSize)
{
    return ALIGN(iFontSize, 8) * iFontSize / 8;
}

class CCharacterOverlap : public ICharacterOverlap
{
public:
    CCharacterOverlap();
    virtual ~CCharacterOverlap();

    virtual HRESULT EnableCharacterOverlap(int iValue);
    virtual HRESULT SetCharacterOverlap(char* pLattice);
    virtual HRESULT SaveCharacterInfo(void);
    virtual HRESULT SetCharacterLightness(int iIsFixedLight);
    HRESULT MakeDateTimeString(BYTE8* pDateTimeString, DWORD32 dwTimeLow, DWORD32 dwTimeHigh);

private:
    STRING_OVERLAY_PARAM m_cParam;
    STRING_OVERLAY_DATA m_cData;
    STRING_OVERLAY_RESPOND m_cRespond;
};

#endif // _CHARACTEROVERLAP_H
