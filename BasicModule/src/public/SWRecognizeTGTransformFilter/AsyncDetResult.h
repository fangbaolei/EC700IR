#pragma once
#include "svBase/svBase.h"

namespace swTgApp
{
    /// 检测区域
    class DET_ROI : public sv::SV_RECT
    {
    public:
        int nType;
        int nSubType;
        float fltConf;
        sv::SV_UINT32 dwFlag;
    };

    // 传输协议结构体头
    // 具体数据将在结构体尾部按如下顺序数据排列：DET_ROI*nDetROICnt、RECTA*nPlateROICnt
    struct TG_DET_API_RESULT_HEAD
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
        int* pDetROICnt
        );
    // 将组装过的数据拆分到指针上，引用关系，直接使用源数据
    sv::SV_RESULT SeparateResultRefPoint(
        void* pBuf,    //
        int nBufLen,
        unsigned int* pFrameTime,
        DET_ROI** prgROI,
        int* pDetROICnt
        );
}
