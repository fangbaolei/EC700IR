#include <string.h>
#include "swbasetype.h"

#if 0
// 函数作用：点阵格式转换
// 参数说明：
// pStringDotMatrix：包含“LJJ”格式的字符点阵，字符高度规格为32（单位：像素）
// iDotMatrixWidth：pStringDotMatrix的宽度（单位：像素）
// rgbDotMatrixDsp：转换出来的DSP端使用的字符点阵格式
void DotMatrixConvert(const char* pStringDotMatrix, int iDotMatrixWidth, PBYTE8 rgbDotMatrixDsp)
{
    int w = iDotMatrixWidth;
    char* p = (char*)pStringDotMatrix;

    // 因为字符高度规格为32像素，所以这里要乘以32字节
    memset(rgbDotMatrixDsp, 0, iDotMatrixWidth * 32);

    int i = 0;
    for (i = 0; i < w; ++i )
    {
        int iDstLine = 0;

        char c0 = p[0];
        char c1 = p[1];
        char c2 = p[2];
        char c3 = p[3];

        #define BIT0 0x01
        #define BIT1 0x02
        #define BIT2 0x04
        #define BIT3 0x08
        #define BIT4 0x10
        #define BIT5 0x20
        #define BIT6 0x40
        #define BIT7 0x80

        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT0); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT1); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT2); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT3); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT4); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT5); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT6); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c0&BIT7); ++iDstLine;

        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT0); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT1); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT2); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT3); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT4); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT5); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT6); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c1&BIT7); ++iDstLine;

        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT0); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT1); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT2); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT3); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT4); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT5); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT6); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c2&BIT7); ++iDstLine;

        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT0); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT1); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT2); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT3); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT4); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT5); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT6); ++iDstLine;
        rgbDotMatrixDsp[w*iDstLine + i] = (c3&BIT7); ++iDstLine;

        p += 4;
    }
}
#endif

/**
 * old_lattice: 原点阵数据，取模方式：逐列式，低位在前，8个像素占1字节
 * new_lattice: 新点阵数据，每个像素占1字节
 * width：      新点阵宽
 * height:      新点阵高(注：高必须是8的整数倍)
 * 注：
 *     内存由调用者分配
 */
void DotMatrixConvert(unsigned char* old_lattice, unsigned char* new_lattice, int width, int height)
{
    unsigned char tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    int offset_byte = 0;    // 点阵所占字节偏移
    unsigned char* p = NULL;
    unsigned char* q = NULL;
    int i = 0;
    int j = 0;

    if (old_lattice == NULL || new_lattice == NULL)
        return;
    p = old_lattice;
    q = new_lattice;

    for(i = 0; i < width; i++)
    {
        for (j = 0; j < height/8; j++)
        {
            offset_byte = j*8;
            // 一个字节
            /// 低位在前
            tmp1 = (*p & 0x01) >> 0;
            tmp2 = (*p & 0x02) >> 1;
            tmp3 = (*p & 0x04) >> 2;
            tmp4 = (*p & 0x08) >> 3;
            tmp5 = (*p & 0x10) >> 4;
            tmp6 = (*p & 0x20) >> 5;
            tmp7 = (*p & 0x40) >> 6;
            tmp8 = (*p & 0x80) >> 7;
            p++;
            // height/8个字节
            q[width * (0+offset_byte) + i] = tmp1;
            q[width * (1+offset_byte) + i] = tmp2;
            q[width * (2+offset_byte) + i] = tmp3;
            q[width * (3+offset_byte) + i] = tmp4;
            q[width * (4+offset_byte) + i] = tmp5;
            q[width * (5+offset_byte) + i] = tmp6;
            q[width * (6+offset_byte) + i] = tmp7;
            q[width * (7+offset_byte) + i] = tmp8;
        }
    }
}

