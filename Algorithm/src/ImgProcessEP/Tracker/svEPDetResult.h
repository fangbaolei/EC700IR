#pragma once
#include "svEPDetApi/svEPDetApi.h"
struct RECTA;  // 前置声明，老的检测结果 

namespace svEPDetApi
{
    // 传输协议结构体头
    // 具体数据将在结构体尾部按如下顺序数据排列：DET_ROI*nDetROICnt、RECTA*nPlateROICnt
    struct EP_DET_API_RESULT_HEAD
    {
        int nCheckID;  // 较检ID
        unsigned int nFrameTime;
        int nDetROICnt;
        int nPlateROICnt;
    }; 

    // 将得到的数据组装成BUF
    sv::SV_RESULT MakeResultBuf(
        int nFrameTime,
        DET_ROI* rgROI,  
        int nDetROICnt, 
        RECTA* rgRectA, 
        int nPlateCnt,
        void* pBuf,    // 可为空，为空时 pBufLen 将直接返回数据长度
        int nMaxBufLen,
        int* pBufLen   // 返回数据长度
        );
    
    // 将组装过的数据拆分到各数组里
    sv::SV_RESULT SeparateResultBuf(
        void* pBuf,    //
        int nBufLen,
        unsigned int* pFrameTime,
        DET_ROI* rgROI, 
        int nMaxROICnt,
        int* pDetROICnt, 
        RECTA* rgPlateROI, 
        int nMaxPlateROICnt,
        int* pPlateCnt
        );
    // 将组装过的数据拆分到指针上，引用关系，直接使用源数据
    sv::SV_RESULT SeparateResultRefPoint(
        void* pBuf,    //
        int nBufLen,
        unsigned int* pFrameTime,
        DET_ROI** prgROI,  
        int* pDetROICnt, 
        RECTA** prgPlateROI, 
        int* pPlateCnt
        );
}
