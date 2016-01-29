/****************************************************************************
                 使用Freetype提取矢量字库字符点阵

使用freetype注意事项：
1、使用IDE或Makefile设置好freetype库的头文件路径；
2、freetype使用的是unicode码，因此必须将非unicode字符进行转换。

功能：
1、支持GB2312和UTF8编码格式的文字(前者使用查表法，后者使用转换算法)，如果是UTF8，必须保证文字编码格式为UTF8，否则会出现乱码。
2、支持且仅支持\n、\t特殊字符，分别对应换行、列对齐。
3、支持指定点阵位图宽和自适应宽两种方式。

log:
先将ASCII生成点阵，存储在缓冲区，使用查表法得到对应的ASCII码。缺点：浪费内存
****************************************************************************/

#ifndef __FREETYPEUTILS_H__
#define __FREETYPEUTILS_H__

#include "SWFC.h"

#include <ft2build.h>

#include FT_FREETYPE_H


// 字符存储编码格式
enum Encoding {
    ENCODE_UTF8,
    ENCODE_GB2312
};

// 字体句柄
typedef struct _FT_Font FT_Font;

class CTTF2Bitmap
{
public:
    CTTF2Bitmap();
    ~CTTF2Bitmap();

public:
    /**
    * 初始化
    *
    * @param pFontFile         矢量字库文件(.ttf或.ttc文件)
    * @param iFontSize         字体大小
    * @param iMaxBitmapWidth   生成点阵位图缓冲区最大长度，如为0，表示让程序自动计算出宽度
    * @return  E_FAIL：出错   S_OK：正常
    *
    */
    HRESULT Init(const char* pFontFile, int iFontSize, int iMaxBitmapWidth);

    /**
    * 释放内存，关闭字库
    *
    * @note
    *       显式释放内存，如不调用此函数，将在析构函数中释放内存
    */
    void Destroy();

    /**
    * 产生点阵位图数据
    *
    * @param pText                 需要转换成点阵的字符串
    * @param cEncoding             pText字符串所用的编码格式
    * @param ppbBitmap[OUT]        生成的点阵位图的缓冲区
    * @param iBitmapDataSize[OUT]  缓冲区大小
    * @param iBitmapWidth[OUT]     缓冲区宽
    * @param iBitmapHeight[OUT]    缓冲区高
    *
    * @return S_OK: 正常  E_FAIL：出错
    *
    * @note
    *       必须注意pText的编码格式，目前仅支持GB2312和UTF8，如编码不正确，会出现乱码。
    */
    HRESULT GenerateBitmap(const char* pText, const Encoding cEncoding,
            BYTE** ppbBitmap, int& iBitmapDataSize, int& iBitmapWidth, int& iBitmapHeight);

    // 与上接口类似，目前只用于生成时间信息，其它字符未全面测试其可用性
    HRESULT GenerateAsciiBitmap(const char* pAscii, BYTE** ppbBitmap,
            int& iBitmapDataSize, int& iBitmapWidth, int& iBitmapHeight);
    int GetFontSize() {return m_iCharWidth; }
    int GetFontHeight() { return m_iCharHeight; }

#ifdef FT_DEBUG
    int GetRealTextNum() { return m_iRealTextNum; }
    void PrintDebugInfo();
#endif

private:
    HRESULT ScanTextForRowCol(unsigned short* pUnicodeText, int iUnicodeLen, int& iNumRows, int& iColSpacing);
    HRESULT ScanAsciiTextForRowCol(unsigned short* pUnicodeText, int iUnicodeLen, int& iNumRows, int& iColSpacing);

    // 生成字符串“!"#$%&'()*+,-./0123456789:”的点阵，每个字符单独存储
    // 无法生成全部ASCII码，因为每个ASCII的点阵大小不相同，不好统一。
	HRESULT GenerateAsciiFont();

    void ChangeMatrix(char* pString, BYTE* pAsciiFontData, int iAsciiWidth, int iAsciiHeight,
        BYTE* pBitmap, int& iBitmapSize, int& iWidth, int& iHeight);

    HRESULT ExpandBitmapBuffer(int iTargetSize);

private:
    FT_Font* m_cFont;       // 字体句柄

    int m_iCharWidth;       // 字符宽
    int m_iCharHeight;      // 字符实际的高(与初始化的FontSize值不一定相等)

    bool m_fAdjustWidth;    // 自适应宽标志

	bool m_fInitialized;
	FT_Library    m_sLibrary;

    int m_iBitmapWidth;     // 位图宽
    int m_iBitmapHeight;    // 位图高
    BYTE* m_pbBitmapBuffer;// 位图缓冲区指针
    int m_iBitmapDataSize;  // 位图数据总大小

    int m_iMaxBlocks;      // 一行对齐最大段数(即\t的数量)
    int* m_piBlockWidths;  // 一行每段的宽

    //unsigned short* m_pUnicodeText; // Unicode编码文字(由用户输入文字转换而成)
    //int m_iUnicodeTextLen;  // unicode长度

#ifdef FT_DEBUG
    int m_iRealTextNum;     // 实际文字个数
#endif

	//////////////////////////////////////////////

    typedef struct _AsciiFontAttr
    {
        int width;
        int offset;
    } AsciiFontAttr;

    bool m_fNeedGenerateFontData;
    BYTE* m_pbAsciiFontData;   // 存储ASCII码点阵缓冲区

    AsciiFontAttr m_cAsciiAttr[95]; // 保存每个ASCII宽度及偏移量
};

#endif // __FREETYPEUTILS_H__
