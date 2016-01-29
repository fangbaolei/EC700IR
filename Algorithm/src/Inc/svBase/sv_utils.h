/// @file
/// @brief 工具函数定义
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_basetype.h"
#include "sv_error.h"

#include "sv_rect.h"
#include "sv_image.h"

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
#include <stdio.h>
#endif

namespace sv
{
    /// 文本信息输出回调原型
    typedef int (*TRACE_CALLBACK_TXT)(
        const char* szInfo,     ///< 字符串
        int nLen                ///< 字符串长度+1
    );

    /// 设置文本输出回调
    void utSetTraceCallBack_TXT(
        TRACE_CALLBACK_TXT hCallBack    ///< 回调函数指针
    );

    /// 调试信息输出
    /// @note 信息长度不能超过640字节
    void utTrace(
        char* szFmt,        ///< 输出格式
        ...
    );

    /// 取得系统TICK毫秒数
    /// @return 系统TICK毫秒数
    SV_UINT32 utGetSystemTick();

    /// 计时开始
#define TS(tag) SV_UINT32 nTime_##tag = sv::utGetSystemTick(); //

    /// 计时结束
#define TE(tag) utTrace("%s: %d ms\n", #tag, sv::utGetSystemTick()-nTime_##tag); //

#define SAFE_DELETE(p) if(p) {delete p; p = NULL;}

    /// 画竖线
    SV_RESULT utMarkLine(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        int nLinePos,                   ///< 画线位置
        SV_UINT8 nColor                 ///< 线颜色
    );

    /// 画框
    SV_RESULT utMarkRect(
        const SV_IMAGE* pImgSrc,        ///< 源图像
        const SV_RECT* pRect,           ///< 画框位置
        SV_UINT8 nColor                 ///< 框颜色
    );

    /// 保存灰度内存图
    SV_RESULT utSaveGrayImage(
        const SV_WCHAR* szPath,         ///< 保存路径
        const void* pImgBuf,            ///< 图像数据
        int nWidth,                     ///< 宽
        int nHeight,                    ///< 高
        int nStrideWidth                ///< 内存扫描行宽
    );

    /// 保存灰度内存图
    SV_RESULT utSaveGrayImage(
        const char* szPath,         ///< 保存路径
        const void* pImgBuf,            ///< 图像数据
        int nWidth,                     ///< 宽
        int nHeight,                    ///< 高
        int nStrideWidth                ///< 内存扫描行宽
    );

    /// 保存BMP图片
    SV_RESULT utSaveImage(
        const SV_WCHAR* szPath,     ///< 保存路径
        const SV_IMAGE* pImg        ///< 源图像
    );

    /// 保存BMP图片
    SV_RESULT utSaveImage(
        const char* szPath,         ///< 保存路径
        const SV_IMAGE* pImg        ///< 源图像
    );

    /// 保存JPEG图片，可支持GRAY和YUV422类型
    SV_RESULT utSaveImage_Jpeg(
        const SV_WCHAR* szPath,     ///< 保存路径
        const SV_IMAGE* pImg,       ///< 源图像
        int nQuality = 95           ///< 压缩质量
    );

    /// 保存JPEG图片，可支持GRAY和YUV422类型
    SV_RESULT utSaveImage_Jpeg(
        const char* szPath,         ///< 保存路径
        const SV_IMAGE* pImg,       ///< 源图像
        int nQuality = 95           ///< 压缩质量
    );

    /// 读取JPEG图片
    /// @note 可通过pImg 传入
    SV_RESULT utReadImage_Jpeg(
        const SV_UINT8* pJpegDat,       ///< JPEG数据
        int nDatLen,                    ///< 数据长度
        SV_IMAGE* pImg,                 ///< 解压后图像,只支持YUV422格式
        int* pWidth,                    ///< 宽
        int* pHeight                    ///< 高
    );

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
    /// 保存内存数据到文本文件
    template<typename T>
    SV_RESULT utMemDump(
        const char* szDumpFile,
        const T* pDat,
        int nWidth,
        int nHeight,
        int nStride = -1
    )
    {
        FILE* hFile = NULL;
        fopen_s(&hFile, szDumpFile, "w");

        if(hFile == NULL)
        {
            return RS_E_UNEXPECTED;
        }

        if(nStride == -1)
        {
            nStride = nWidth;
        }

        T* pLine = (T*)pDat;

        for(int i = 0; i < nHeight; i++, pLine += nStride)
        {
            for(int j = 0; j < nWidth; j++)
            {
                fprintf(hFile, "%8.3f,", (float)pLine[j]);
            }

            fprintf(hFile, "\n");
        }

        fclose(hFile);

        return RS_S_OK;
    }

    /// 读取文本文件到内存
    template<typename T>
    SV_RESULT utLoadMem(
        const char* szMemFile,
        T* pDat,
        int nLen,
        int* pWidth,
        int* pHeight
    )
    {
        if(pDat == NULL || pWidth == NULL || pHeight == NULL)
        {
            return RS_E_INVALIDARG;
        }

        *pWidth = 0;
        *pHeight = 0;

        FILE* hFile = NULL;
        fopen_s(&hFile, szMemFile, "r");

        if(hFile == NULL)
        {
            return RS_E_UNEXPECTED;
        }

        const int nMaxLen = 100000;
        CMemAlloc cAlloc;
        char* pLine = (char*)cAlloc.Alloc(nMaxLen * sizeof(char));

        if(pLine == NULL)
        {
            return RS_E_UNEXPECTED;
        }

        SV_BOOL fLoadOK = TRUE;
        int nReadCount = 0;
        int nHeight = 0;
        int nWidth = 0;

        while(fgets(pLine, nMaxLen, hFile))
        {
            pLine[strlen(pLine) - 1] = '\0';

            char* pTokNext = NULL;
            int nLineWidth = 0;
            char* pTok = strtok_s(pLine, ",", &pTokNext);

            while(pTok)
            {
                if(nReadCount >= nLen)
                {
                    fLoadOK = FALSE;
                    break;
                }

                pDat[nReadCount++] = (T)atof(pTok);
                pTok = strtok_s(NULL, ",", &pTokNext);
                nLineWidth++;
            }

            nWidth = SV_MAX(nWidth, nLineWidth);
            nHeight++;
        }

        fclose(hFile);

        if(!fLoadOK)
        {
            return RS_E_OUTOFMEMORY;
        }

        *pWidth = nWidth;
        *pHeight = nHeight;

        return RS_S_OK;
    }
#else
    template<typename T>
    SV_RESULT utMemDump(
        const char* szDumpFile,
        const T* pDat,
        int nWidth,
        int nHeight,
        int nStride = -1
    )
    {
        return RS_E_NOTIMPL;
    }

    template<typename T>
    SV_RESULT utLoadMem(
        const char* szMemFile,
        T* pDat,
        int nLen,
        int* pWidth,
        int* pHeight
    )
    {
        return RS_E_NOTIMPL;
    }
#endif

} // sv
