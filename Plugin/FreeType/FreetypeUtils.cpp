// 该文件编码必须为WINDOWS-936

#include "transcoding.h"
#include "FreetypeUtils.h"

// 注意！！必须设置freetype的头文件路径
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#pragma comment(lib, "lib/freetype2410MT.lib")
#endif

/////////debug
#ifdef WIN32
// Windows风格的宏
#ifdef FT_DEBUG
#define debug printf
#define error_msg printf
#else
#define debug
#define error_msg
#endif

#else
// linux风格的宏
#ifdef FT_DEBUG
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define error_msg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#define error_msg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#endif
//////////// debug end

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define FT_FLOOR(X)    ((X & -64) / 64)
#define FT_CEIL(X)    (((X + 63) & -64) / 64)

// 字体句柄，参考了SDL_ttf代码
struct _FT_Font
{
    FT_Face face;	// 字体外观对象
    int height;	    // 字体高
    int baseline;   // 基线
};

/**
 * 初始化FreeType库
 *
 * @return
 *          0    初始化OK
 *          -1   初始化错误
 */
int FT_Init(bool& fInited, FT_Library& libFT)
{
    int status = 0;

    if (!fInited)
    {
        FT_Error error = FT_Init_FreeType(&libFT);
        if (error)
        {
            error_msg("FT_Init_FreeType failed, error: 0x%x\n", error);
            status = -1;
			return status;
        }
    }

    // test
    int major = 0;
    int minor = 0;
    int patch = 0;
    FT_Library_Version(libFT, &major, &minor, &patch);
    debug("Init: version: %d.%d.%d\n", major, minor, patch);
	
	fInited = true;
    return status;
}

/**
 * 销毁FT
 *
 */
void FT_Quit(bool& fInited, FT_Library& libFT)
{
    if (fInited)
    {
        FT_Done_FreeType(libFT);
		libFT = 0;
    }

	fInited = false;
}

/**
 * 关闭字库，释放字体结构体
 *
 */
void FT_CloseFont(FT_Font* font)
{
    int error;
    if (font)
    {
        if (font->face)
        {
            error = FT_Done_Face(font->face);
            if (error)
            {
                error_msg("FT_Done_Face error, 0x%X.\n", error);
            }
        }
        free(font);
        font = NULL;
    }
}

/**
 * 打开字库文件
 *
 * @param pFontFile 字库文件名
 * @param iFontSize  字体大小
 *
 * @return
 *          成功返回FT_Font结构体，出错返回NULL
 */
FT_Font* FT_OpenFont(FT_Library& libFT, const char* pFontFile, int iFontSize)
{
    FT_Font* font;
    FT_Face face;
    FT_Error error;
    FT_Fixed scale;

    font = (FT_Font *)calloc(1, sizeof(FT_Font));
    if (font == NULL)
    {
        error_msg("malloc for font failed!\n");
        return NULL;
    }

    // 加载一个字库文件
    error = FT_New_Face(libFT, pFontFile, 0, &font->face); // face对象为FT生成
    if (error)
    {
        error_msg("FT_New_Face %s failed, error: 0x%x\n", pFontFile, error);
        FT_CloseFont(font);
        return NULL;
    }
    face = font->face; // 传递的是对象指针

    // 设定为UNICODE，默认也是
    error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    if (error)
   {
       error_msg("FT_Select_Charmap %s failed, error: 0x%x\n", pFontFile, error);
       FT_CloseFont(font);
       return NULL;
   }

    // 可缩放
    if (FT_IS_SCALABLE(face))
    {
        // 设置字体大小
        // 注：大小要乘64，默认DPI为72
        error = FT_Set_Char_Size(face, 0, iFontSize << 6, 0, 0);
        if (error)
        {
            error_msg("Set char size failed, error: 0x%x\n", error);
            FT_CloseFont(font);
            return NULL;
        }

      scale = face->size->metrics.y_scale;
      int ascent  = FT_CEIL(FT_MulFix(face->ascender, scale));
      int descent = FT_CEIL(FT_MulFix(face->descender, scale));
      font->height  = ascent - descent;
      font->baseline = font->height + descent;

      //int underline = FT_CEIL(FT_MulFix(face->underline_position, scale));
      //debug("underline_position: %d %d\n", underline, FT_CEIL(FT_MulFix(face->height, scale)));
    }

    return font;
}

// 字体相关工具函数
int FT_GetFontHeight(FT_Font* font)
{
    return font->height;
}

int FT_GetFontBaseline(FT_Font* font)
{
    return font->baseline;
}

/**
 * 取单个字符的点阵
 *
 * @param font              字体结构体
 * @param pUnicodeChar      unicode格式的字符
 * @param pBitmap           字符点阵缓冲区
 * @param iBitmapWidth      点阵宽度
 * @param iBitmapHeight     点阵高度
 * @param iStartX           字符点阵从X起点开始copy
 * @param iStartY           字符点阵从Y起点开始copy
 * @param iFontStride[OUT]  字符的宽跨度
 *
 * @return  -1：出错   0：正常
 *
 * @note
 *       使用iFontStride的原因是每个字符的宽度不相同，
         不能事先规定其宽，如w宽度超过半角宽，而i宽度不足半角宽
 */
int FT_GetCharBitmap(
    FT_Font* font,
    const unsigned short* pUnicodeChar,
    unsigned char* pBitmap,
    int iBitmapWidth,
    int iBitmapHeight,
    int iStartX,
    int iStartY,
    int& iFontStride
    )
{
    FT_GlyphSlot  slot = font->face->glyph;
    FT_UInt glyph_index;
    FT_Error error;
    FT_Bitmap *bm;
    unsigned short iPlaceUnicodeChar = 0x25a1;  // 首选，方框“□”

    int rows;
    int width;
    int top;
    int left;

    if (pBitmap == NULL)
    {
        error_msg("pBitmap is NULL\n");
        return -1;
    }

    glyph_index = FT_Get_Char_Index(font->face, *pUnicodeChar); // 找到字符索引
    if(glyph_index == 0)
    {
        // 出现字库文件里没有的字符时，使用“□”代替
        // 使用空白方框“□”来替换没有的字符
        error_msg("For char UNICODE 0x%04X, got glyph_index = 0, changed to UNICODE: 0x%04X.\n", *pUnicodeChar, iPlaceUnicodeChar);
        glyph_index = FT_Get_Char_Index(font->face, iPlaceUnicodeChar);
        if(glyph_index == 0)
        {
            // 如果没有上面的方框，则使用首部“囗”
            error_msg("For char UNICODE 0x%04X, got glyph_index = 0 again, ", iPlaceUnicodeChar);
            iPlaceUnicodeChar = 0x56d7; // 次选首部“囗”
            error_msg("changed to UNICODE: 0x%04X.\n", iPlaceUnicodeChar);
            glyph_index = FT_Get_Char_Index(font->face, iPlaceUnicodeChar);
            if(glyph_index == 0)
            {
                error_msg("For char UNICODE 0x%04X, got glyph_index = 0 once again, ", iPlaceUnicodeChar);
                iPlaceUnicodeChar = 0x53e3; // 再选口字“口”
                error_msg("changed to UNICODE: 0x%04X.\n", iPlaceUnicodeChar);
                glyph_index = FT_Get_Char_Index(font->face, iPlaceUnicodeChar);
                if(glyph_index == 0)
                {
                    error_msg("For char UNICODE 0x%04X, trying other char but got glyph_index = 0, failed!!!!!, ", *pUnicodeChar);
                    return -1;
                }
            }
        }
    }

    // 装载字型图像到字形槽(得到字模)
    error = FT_Load_Glyph(font->face, glyph_index, FT_LOAD_DEFAULT);
    if (error)
    {
        error_msg("FT_Load_Glyph error, 0x%x.\n", error);
        return -1;
    }

    // 加粗，暂不要
    //FT_Outline_Embolden( &(slot->outline), 1<<6);

    // 转换为位图数据
    if (slot->format != FT_GLYPH_FORMAT_BITMAP)
    {
        // 转化成位图
        // 第二个参数为渲染模式
        // FT_RENDER_MODE_NORMAL渲染出一个高质量的抗锯齿(256级灰度)位图
        // FT_RENDER_MODE_MONO渲染为1位位图（黑白位图）
        error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
        if (error)
        {
            error_msg("FT_Render_Glyph error, 0x%x.\n", error);
            return -1;
        }
    }

    bm = &slot->bitmap;
    rows = slot->bitmap.rows;
    width = slot->bitmap.width;
    iFontStride = (int)slot->metrics.horiAdvance>>6;    // 真实宽度

    // 适用于仅生成一个字符点阵情况
    if (iBitmapWidth == 0)
    {
        iBitmapWidth = iFontStride;
    }

    if (iBitmapHeight == 0)
    {
        iBitmapHeight = font->height;
    }

    // 调整边距
    left = ((rows - font->height) >= 0) ? 0 : slot->bitmap_left / 2;
    top = font->baseline - slot->bitmap_top;
    rows = MIN(rows, font->height);

    // 防止越界
    //int x_max = MIN(iStartX + left + width, iBitmapWidth);
    //int y_max = MIN(iStartY + top + rows, iBitmapHeight);
    //int i, j, p, q;

    ////拷贝点阵，用1字节表示1像素，为1表示需要显示的点
    //for (i = MAX(iStartY + top, 0), p = 0; i < y_max; i++, p++)
    //{
    //    for (j = MAX(iStartX + left, 0), q = 0; j < x_max; j++, q++)
    //    {
    //        pBitmap[i * iBitmapWidth + j] = (bm->buffer[p * width + q] ? 1 : 0);
    //    }
    //}

    // 似乎这种方法不及上述方法快
    //int x = 0;
    //int y = 0;
    for (int i = 0; i < rows && (i + top + iStartY) < iBitmapHeight; i++)
    {
        for (int j = 0; j < width && (j + left + iStartX) < iBitmapWidth; j++)
        {
            //x = j + left + iStartX;
            //y = i + top + iStartY;
            //int v = 0;
            //if (bm->buffer[/*i * bm->pitch + */j / 8] & (0x80 >> (x&7)))
            //    v = 1;
            //else
            //    v = 0;
            pBitmap[(i + top + iStartY) * iBitmapWidth + (j + left + iStartX)] = (bm->buffer[i * width + j] ? 1 : 0);
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////

CTTF2Bitmap::CTTF2Bitmap()
            :m_cFont(NULL),
            m_iCharWidth(0),
            m_iCharHeight(0),
            m_fAdjustWidth(false),
            m_iBitmapWidth(0),
            m_iBitmapHeight(0),
            m_pbBitmapBuffer(NULL),
            m_iBitmapDataSize(0),
            m_iMaxBlocks(0),
            m_piBlockWidths(NULL),
            m_fNeedGenerateFontData(false),
            m_pbAsciiFontData(NULL)
{
#ifdef FT_DEBUG
	m_iRealTextNum = 0;
#endif
	m_fInitialized = false;
	m_sLibrary = 0;
}

CTTF2Bitmap::~CTTF2Bitmap()
{
    Destroy();
}

HRESULT CTTF2Bitmap::Init(const char* pFontFile, int iFontSize, int iMaxBitmapWidth)
{

    m_iBitmapWidth = iMaxBitmapWidth;

    if (m_iBitmapWidth == 0)
    {
        m_fAdjustWidth = true;
    }

    // m_iMaxColumns为垂直对齐的块的个数，即一行最多可以有多少个'\t'
    if (m_fAdjustWidth)
    {
        m_iMaxBlocks = 50; // 足够了
    }
    else
    {
        m_iMaxBlocks = (m_iBitmapWidth) / (iFontSize / 2);
    }

    m_piBlockWidths = new int[m_iMaxBlocks];

    if (FT_Init(m_fInitialized, m_sLibrary) < 0)
    {
        error_msg("FT_Init failed!!\n");
        return E_FAIL;
    }

    m_cFont = FT_OpenFont(m_sLibrary, pFontFile, iFontSize);
    if (m_cFont == NULL)
    {
        error_msg("FT_OpenFont failed!!\n");
        return E_FAIL;
    }

    m_iCharWidth = iFontSize;
    m_iCharHeight = FT_GetFontHeight(m_cFont);

    m_fNeedGenerateFontData = true;

    return S_OK;
}

void CTTF2Bitmap::Destroy()
{
    if (m_pbBitmapBuffer != NULL)
    {
        delete[] m_pbBitmapBuffer;
        m_pbBitmapBuffer = NULL;
        m_iBitmapDataSize = 0;
    }
    if (m_piBlockWidths != NULL)
    {
        delete[] m_piBlockWidths;
        m_piBlockWidths = NULL;
    }

    if (m_pbAsciiFontData != NULL)
    {
        delete[] m_pbAsciiFontData;
        m_pbAsciiFontData = NULL;
    }

    if (m_cFont != NULL)
    {
        FT_CloseFont(m_cFont);
        m_cFont = NULL;
        FT_Quit(m_fInitialized, m_sLibrary);
    }
}

HRESULT CTTF2Bitmap::ExpandBitmapBuffer(int iTargetSize)
{
    if (iTargetSize <= 0)
    {
        error_msg("expand size = %d.\n", iTargetSize);
        return E_FAIL;
    }

    if (m_pbBitmapBuffer != NULL && m_iBitmapDataSize != 0)
    {
        if (iTargetSize <= m_iBitmapDataSize)
        {
            return S_OK;
        }
        else
        {
            delete[] m_pbBitmapBuffer;
            m_pbBitmapBuffer = NULL;
        }
    }
    else if (
            (m_pbBitmapBuffer != NULL && m_iBitmapDataSize == 0)
            ||
            (m_pbBitmapBuffer == NULL && m_iBitmapDataSize != 0)
            )
    {
        error_msg("State error m_pbBitmapBuffer = %p, m_iBitmapDataSize = %d.\n", m_pbBitmapBuffer, m_iBitmapDataSize);
    }

    m_pbBitmapBuffer = new BYTE[iTargetSize];
    if (m_pbBitmapBuffer)
    {
        m_iBitmapDataSize = iTargetSize;
    }
    else
    {
        error_msg("Allocate m_pbBitmapBuffer fail.\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CTTF2Bitmap::GenerateBitmap(const char* pText, const Encoding cEncoding,
    BYTE** ppbBitmap, int& iBitmapDataSize, int& iBitmapWidth, int& iBitmapHeight)
{
    int ret = -1;
    int iNumRows = 0;
    int iColSpacing = 0;

    if(m_cFont == NULL)
    {
        error_msg("m_cFont is NULL\n");
        return E_FAIL;
    }

    // 转换字符集
    int iTextLen = (int)strlen(pText);
    unsigned short* pUnicodeText = new unsigned short[iTextLen];
    if(pUnicodeText == NULL)
    {
        error_msg("Allocate pUnicodeText failed.\n");
        return E_FAIL;
    }
    int iUnicodeLen = 0;
#ifdef FT_DEBUG
    m_iRealTextNum = 0;
#endif
    switch (cEncoding)
    {
        case ENCODE_UTF8:
            iUnicodeLen = utf8_to_unicode(pUnicodeText, pText, iTextLen);
            break;
        case ENCODE_GB2312:
            iUnicodeLen = gb2312_to_unicode(pUnicodeText, pText, iTextLen);
            break;
        default:
            return E_FAIL;
    }

    // 扫描文字，以确定行及对齐宽度
    ScanTextForRowCol(pUnicodeText, iUnicodeLen, iNumRows, iColSpacing);

    m_iBitmapHeight = m_iCharHeight * iNumRows;

    // 传出参数
    iBitmapWidth = m_iBitmapWidth;
    iBitmapHeight = m_iBitmapHeight;
    iBitmapDataSize = iBitmapWidth * iBitmapHeight;

    debug("m_iCharHeight = %d, iNumRows = %d.\n", m_iCharHeight, iNumRows);
    debug("m_iBitmapWidth = %d.\n", m_iBitmapWidth);
    debug("m_iBitmapHeight = %d.\n", m_iBitmapHeight);
    debug("iBitmapDataSize = %d.\n", iBitmapDataSize);

    if ( S_OK != ExpandBitmapBuffer(iBitmapDataSize))
    {
        error_msg("ExpandBitmapBuffer failed.\n");
        return E_FAIL;
    }

    // 每次提取都清空缓冲区，防止旧数据干扰
    memset(m_pbBitmapBuffer, 0, iBitmapDataSize);

    int rowIndex = 0;
    int columnIndex = 0;

    int iStartX = 0;
    int iStartY = 0;
    int iFontStride = 0;
    for ( int n = 0; n < iUnicodeLen; n++ )
    {
        // 换行，X归0，Y移到下一行
        if (pUnicodeText[n] == '\n')
        {
            iStartX = 0;
            iStartY = (++rowIndex) * m_iCharHeight;
            columnIndex = 0;
            continue;
        }
        // 水平对齐，X右移，Y不变
        if (pUnicodeText[n] == '\t')
        {
            iStartX = 0;
            for (int i = 0; i <= columnIndex; i++)
                iStartX += m_piBlockWidths[i] + iColSpacing;
            columnIndex++;
            continue;
        }

        ret = FT_GetCharBitmap(m_cFont, &pUnicodeText[n],
                        m_pbBitmapBuffer, m_iBitmapWidth, m_iBitmapHeight,
                        iStartX, iStartY, iFontStride);
        if (ret < 0)
        {
            error_msg("FT_GetCharBitmap failed.\n");
            delete[] pUnicodeText;
            pUnicodeText = NULL;
            return E_FAIL;
        }

#ifdef FT_DEBUG
        m_iRealTextNum++;
#endif
        iStartX += iFontStride;
    }

    *ppbBitmap = m_pbBitmapBuffer;

    delete[] pUnicodeText;
    pUnicodeText = NULL;

    return S_OK;
}

HRESULT CTTF2Bitmap::GenerateAsciiBitmap(const char* pAscii, BYTE** ppbBitmap,
        int& iBitmapDataSize, int& iBitmapWidth, int& iBitmapHeight)
{
    if(m_cFont == NULL)
    {
        error_msg("m_cFont is NULL\n");
        return E_FAIL;
    }

    // 转换字符集
    int iTextLen = (int)strlen(pAscii);
    unsigned short* pUnicodeText = new unsigned short[iTextLen + 1];
    int iUnicodeLen = ascii_to_unicode(pUnicodeText, pAscii, iTextLen);;

    pUnicodeText[iUnicodeLen] = '\0';

    if (m_fNeedGenerateFontData)
    {
        printf("Need generate a new ASCII font database.\n");
        if ( S_OK != GenerateAsciiFont())
        {
            error_msg("GenerateAsciiFont fail.\n");
            return E_FAIL;
        }
        m_fNeedGenerateFontData = false;
    }

    // 扫描文字，以确定行及对齐宽度
    int iNumRows = 0;
    int iColSpacing = 0;
    if ( S_OK != ScanAsciiTextForRowCol(pUnicodeText, iUnicodeLen, iNumRows, iColSpacing))
    {
        error_msg("Error in ScanAsciiTextForRowCol.\n");
    }

    m_iBitmapHeight = m_iCharHeight * iNumRows;

    // 传出参数
    iBitmapWidth = m_iBitmapWidth;
    iBitmapHeight = m_iBitmapHeight;
    iBitmapDataSize = iBitmapWidth * iBitmapHeight;

    if ( S_OK != ExpandBitmapBuffer(iBitmapDataSize))
    {
        error_msg("ExpandBitmapBuffer failed.\n");
        return E_FAIL;
    }

#ifdef FT_DEBUG
    m_iRealTextNum = 0;
#endif

    // 每次提取都清空缓冲区，防止旧数据干扰
    memset(m_pbBitmapBuffer, 0, iBitmapDataSize);

    int rowIndex = 0;
    int columnIndex = 0;

    int iStartX = 0;
    int iStartY = 0;
    int iFontStride = 0;    //= m_iCharWidth / 2;
    //int iAsciiCharSize = m_iCharWidth / 2 * m_iCharHeight;

    for ( int n = 0; n < iUnicodeLen; n++ )
    {
        // 换行，X归0，Y移到下一行
        if (pUnicodeText[n] == '\n')
        {
            iStartX = 0;
            iStartY = (++rowIndex) * m_iCharHeight;
            columnIndex = 0;
            continue;
        }
        // 水平对齐，X右移，Y不变
        if (pUnicodeText[n] == '\t')
        {
            iStartX = 0;
            for (int i = 0; i <= columnIndex; i++)
                iStartX += m_piBlockWidths[i] + iColSpacing;
            columnIndex++;
            continue;
        }

        // 查表
		int index = pUnicodeText[n] - 0x20;
		char * chtmp = (char *)m_pbAsciiFontData + m_cAsciiAttr[index].offset;

		iFontStride = m_cAsciiAttr[index].width;

        //debug("font: %d[%c] width: %d offset: %d\n", pUnicodeText[n]-0x20, pUnicodeText[n], iFontStride, m_cAsciiAttr[index].offset);

        // 点阵转换
        // old
        //for (int i = iStartY, p = 0; i < m_iBitmapHeight; i++, p++)
        //{
        //    for (int j = iStartX, q = 0; j < m_iBitmapWidth; j++, q++)
        //    {
        //        m_pbBitmapBuffer[i*m_iBitmapWidth + j] = chtmp[p*iFontStride + q];
        //    }
        //}
        // 比上述方法循环次数少
        for (int i = 0; i < m_iCharHeight && (i + iStartY) < m_iBitmapHeight; i++)
        {
            for (int j = 0; j < iFontStride && (j + iStartX) < m_iBitmapWidth; j++)
            {
                m_pbBitmapBuffer[(i + iStartY) * m_iBitmapWidth + (j + iStartX)] = chtmp[i*iFontStride + j];
            }
        }
#ifdef FT_DEBUG
        m_iRealTextNum++;
#endif

        iStartX += iFontStride;
    }

    *ppbBitmap = m_pbBitmapBuffer;

    delete[] pUnicodeText;
    pUnicodeText = NULL;

    return S_OK;
}

// 生成95个可打印的ASCII码(即除0~31、127外的ASCII)点阵
// 并不是所有的矢量字体中ASCII码宽都是全角的一半，有些宽大于半角，有些小于半角，这里暂时以半角为准。
HRESULT CTTF2Bitmap::GenerateAsciiFont()
{
    int ret = -1;
    int iTextLen = 0;
    int iUnicodeTextLen = 0;
    unsigned short* pUnicodeText = NULL;
    int iWidth = 0;
    int iHeight = 0;
    int iCharSize = 0;	// 一个字符的大小
    int offset = 0;
    int iTotalAsciiSize = 0;
    BYTE* pBuffer = NULL;
    BYTE* pIncreaseBuffer = NULL;
    const char pAscii[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";    // 95个

    iTextLen = (int)strlen(pAscii);
    pUnicodeText = new unsigned short[iTextLen];
    iUnicodeTextLen = ascii_to_unicode(pUnicodeText, pAscii, iTextLen);

    iWidth = m_iCharWidth / 2;				// 暂时每个字符宽度为半角
    iHeight = m_iCharHeight;
    iCharSize = iWidth * iHeight;
    iTotalAsciiSize = iCharSize * iTextLen; // 总大小
    printf("pre-alloc memory buffer size is %d.\n", iTotalAsciiSize);

    if (m_pbAsciiFontData != NULL)
    {
        error_msg("memory leak fixed!.\n");
        delete [] m_pbAsciiFontData;
        m_pbAsciiFontData = NULL;
    }

    m_pbAsciiFontData = new BYTE[iTotalAsciiSize]; // 预先生成好缓冲
    if (m_pbAsciiFontData == NULL)
    {
        error_msg("memory insufficient!.\n");
        return E_FAIL;
    }

    memset(m_pbAsciiFontData, 0, iTotalAsciiSize);

    memset(m_cAsciiAttr, 0, 95*sizeof(AsciiFontAttr));

    pBuffer = m_pbAsciiFontData;

    int ss = 0;
    for (int i = 0; i < iUnicodeTextLen; i++)
    {
        if (pBuffer >= m_pbAsciiFontData + iTotalAsciiSize - 128*iHeight) // 预留下一个字符的内存; 按最大字符宽度128假设
        {
            debug("Overflow pBuffer = %p, m_pbAsciiFontData = %p, iHeight = %d\n", pBuffer, m_pbAsciiFontData, iHeight);
            debug("need increase buffer size to %d.\n", offset+128*iHeight);
            iTotalAsciiSize = offset+128*iHeight;
            pIncreaseBuffer = new BYTE[iTotalAsciiSize];
            if (pIncreaseBuffer == NULL)
            {
                error_msg("memory insufficient, %s :line %d.\n", __func__, __LINE__);
            }
            memcpy(pIncreaseBuffer, m_pbAsciiFontData, offset); // 拷贝已生成的所有数据
            if (iTotalAsciiSize == 1)
                delete m_pbAsciiFontData;
            else
                delete[] m_pbAsciiFontData;
            pBuffer += pIncreaseBuffer - m_pbAsciiFontData;
            m_pbAsciiFontData = pIncreaseBuffer;
            pIncreaseBuffer = NULL;
            debug("increase done.\n");
        }

        int iFontStride = 0;
        ret = FT_GetCharBitmap(m_cFont, &pUnicodeText[i], pBuffer, 0, 0, 0, 0, iFontStride);
        if (ret < 0)
        {
            error_msg("FT_GetCharBitmap failed.\n");
            return E_FAIL;
        }
        iCharSize = iFontStride * iHeight;
		m_cAsciiAttr[i].width = iFontStride;
		ss+=iFontStride;
		m_cAsciiAttr[i].offset = offset;
		pBuffer += iCharSize;
        offset += iCharSize;

        debug("debug-: %d width: %d charsize: %d offset: %d\n", i, m_cAsciiAttr[i].width, iCharSize, m_cAsciiAttr[i].offset);
    }

    printf("total width: %d, calc width: %d; total size %d byte(s).\n", ss, 95*m_iCharWidth/2, iTotalAsciiSize);

	delete[] pUnicodeText;
    return S_OK;
}

// 以'\t'、'\n'为分块标志，扫描字符串，得出每块长度，继而得到对齐所需要长度
HRESULT CTTF2Bitmap::ScanTextForRowCol(unsigned short* pUnicodeText, int iUnicodeLen,
                                       int& iNumRows, int& iColSpacing)
{
    memset(m_piBlockWidths, 0, m_iMaxBlocks * sizeof(int));

    int iNumColumns = 0;    // 一行块的个数
    int numColumnsCurRow = 0; // 当前行块的个数
    int columnWidth = 0;

    for (int i = 0; i < iUnicodeLen; i++) {
        switch (pUnicodeText[i]) {
        case '\n':
            m_piBlockWidths[numColumnsCurRow] = MAX(m_piBlockWidths[numColumnsCurRow], columnWidth);
            numColumnsCurRow++;
            if (numColumnsCurRow >= m_iMaxBlocks)
                return E_FAIL;
            iNumRows++;
            iNumColumns = MAX(iNumColumns, numColumnsCurRow);
            numColumnsCurRow = 0;
            columnWidth = 0;
            break;
        case '\t':
            m_piBlockWidths[numColumnsCurRow] = MAX(m_piBlockWidths[numColumnsCurRow], columnWidth);
            numColumnsCurRow++;
            if (numColumnsCurRow >= m_iMaxBlocks)
                return E_FAIL;
            columnWidth = 0;
            break;
        default:
            if (pUnicodeText[i] <= 0x80)
            {
                columnWidth += m_iCharWidth/ 2;
                if (m_iCharWidth % 2)
                    columnWidth += 1;   // note：防止字体大小为奇数时出现截断字符串现象
            }

            else
                columnWidth += m_iCharWidth;
            if (i == iUnicodeLen - 1) {
                m_piBlockWidths[numColumnsCurRow] = MAX(m_piBlockWidths[numColumnsCurRow], columnWidth);
                numColumnsCurRow++;
                if (numColumnsCurRow >= m_iMaxBlocks)
                    return E_FAIL;
                iNumRows++;
                iNumColumns = MAX(iNumColumns, numColumnsCurRow);
            }
            break;
        }
    }
    int textWidth = 0;
    for (int i = 0; i < iNumColumns; i++)
        textWidth += m_piBlockWidths[i];

    // 自动调整宽
    if (m_fAdjustWidth)
    {
        m_iBitmapWidth = textWidth + m_iCharWidth * (iNumColumns - 1);
        iColSpacing = m_iCharWidth; // 以一个字符宽为对齐空间
    }
    else
    {
        if ((m_iBitmapWidth - textWidth) <= 0)
            iColSpacing = 0;
        else
            iColSpacing = (m_iBitmapWidth - textWidth) / iNumColumns;
    }
    debug("font height: %d text width: %d bitmap width: %d space: %d col: %d\n",
        FT_GetFontHeight(m_cFont), textWidth, m_iBitmapWidth, iColSpacing, iNumColumns);

    return S_OK;
}

HRESULT CTTF2Bitmap::ScanAsciiTextForRowCol(unsigned short* pUnicodeText, int iUnicodeLen, int& iNumRows, int& iColSpacing)
{
    memset(m_piBlockWidths, 0, m_iMaxBlocks * sizeof(int));

    int iNumMaxBlocks = 0;    // 一行块的个数(所有行中的最大值),占用最大宽度
    int numBlocksCurRow = 0; // 当前行的块个数
    int blockWidth = 0; // 一个块的宽度

    int index;
    int iFontStride = 0;

    for (int i = 0; i < iUnicodeLen; i++) {
        switch (pUnicodeText[i]) {
        case '\n':
            m_piBlockWidths[numBlocksCurRow] = MAX(m_piBlockWidths[numBlocksCurRow], blockWidth);
            numBlocksCurRow++;
            if (numBlocksCurRow >= m_iMaxBlocks)
                return E_FAIL;
            iNumRows++;
            iNumMaxBlocks = MAX(iNumMaxBlocks, numBlocksCurRow);
            numBlocksCurRow = 0;
            blockWidth = 0;
            break;
        case '\t':
            m_piBlockWidths[numBlocksCurRow] = MAX(m_piBlockWidths[numBlocksCurRow], blockWidth);
            numBlocksCurRow++;
            if (numBlocksCurRow >= m_iMaxBlocks)
                return E_FAIL;
            blockWidth = 0;
            break;
        default:
            index = pUnicodeText[i] - 0x20;
            if (index >= 0)
            {
                iFontStride = m_cAsciiAttr[index].width;
            }
            else
            {
                iFontStride = m_iCharWidth;
            }

            if (pUnicodeText[i] <= 0x80)
                blockWidth += iFontStride;
            else
                blockWidth += m_iCharWidth;
            if (i == iUnicodeLen - 1) {
                m_piBlockWidths[numBlocksCurRow] = MAX(m_piBlockWidths[numBlocksCurRow], blockWidth);
                numBlocksCurRow++;
                if (numBlocksCurRow >= m_iMaxBlocks)
                    return E_FAIL;
                iNumRows++;
                iNumMaxBlocks = MAX(iNumMaxBlocks, numBlocksCurRow);
            }
            break;
        }
    }
    int textWidth = 0;
    for (int i = 0; i < iNumMaxBlocks; i++)
        textWidth += m_piBlockWidths[i];

    // 自动调整宽
    if (m_fAdjustWidth)
    {
        m_iBitmapWidth = textWidth + m_iCharWidth * (iNumMaxBlocks - 1);
        iColSpacing = m_iCharWidth; // 以一个字符宽为对齐空间
    }
    else
    {
        if ((m_iBitmapWidth - textWidth) <= 0)
            iColSpacing = 0;
        else
            iColSpacing = (m_iBitmapWidth - textWidth) / iNumMaxBlocks;
    }
    debug("font height: %d text width: %d bitmap width: %d space: %d col: %d\n",
        FT_GetFontHeight(m_cFont), textWidth, m_iBitmapWidth, iColSpacing, iNumMaxBlocks);

    return S_OK;
}

#ifdef FT_DEBUG
void CTTF2Bitmap::PrintDebugInfo()
{
    printf("bitmap buffer: %dB(%.1fKB) = %d*%d font width: %d font height: %d\n",
            m_iBitmapDataSize, m_iBitmapDataSize/1024.0, m_iBitmapWidth, m_iBitmapHeight, m_iCharWidth, m_iCharHeight);
    printf("real text num: %d\n", m_iRealTextNum);
}
#endif
