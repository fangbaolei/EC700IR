#include "svEPDetResult.h"
#include "platedetector.h"

using namespace sv;

#define DET_HEAD_CHECK_ID 2424

namespace svEPDetApi
{
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
        )
    {
        int nSize = sizeof(EP_DET_API_RESULT_HEAD) + sizeof(DET_ROI) * nDetROICnt 
            + sizeof(RECTA) * nPlateCnt;  

        if (pBufLen != NULL)
        {
            *pBufLen = nSize;
        }
        // 允许pBuf为空时返回大小
        if (pBuf != NULL && nSize <= nMaxBufLen)
        {
            char* pData = (char*)pBuf;

            int nTmpLen = 0;

            EP_DET_API_RESULT_HEAD* pHead = (EP_DET_API_RESULT_HEAD*)pBuf;
            pHead->nCheckID = DET_HEAD_CHECK_ID;
            pHead->nFrameTime = nFrameTime;
            pHead->nDetROICnt = nDetROICnt;
            pHead->nPlateROICnt = nPlateCnt;
            nTmpLen = sizeof(EP_DET_API_RESULT_HEAD);
            pData += nTmpLen;

            nTmpLen = sizeof(DET_ROI)*nDetROICnt;  
            memcpy(pData, rgROI, nTmpLen);
            pData += nTmpLen;

            nTmpLen = sizeof(RECTA)*nPlateCnt;  
            memcpy(pData, rgRectA, nTmpLen);
            pData += nTmpLen;
        }

        return RS_S_OK;
    }

    // 将组装过的数据拆分成BUF
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
        )
    {
        EP_DET_API_RESULT_HEAD* pHead = (EP_DET_API_RESULT_HEAD*)pBuf;

        if (pBuf == NULL || nBufLen == 0 || pHead->nCheckID != DET_HEAD_CHECK_ID)
        {
            if (pDetROICnt != NULL) *pDetROICnt = 0;
            if (pPlateCnt != NULL) *pPlateCnt = 0;
            return RS_S_OK;
        }


        if (pFrameTime != NULL)
        {
            *pFrameTime = pHead->nFrameTime;
        }
        char* pData = (char*)pBuf;
        pData += sizeof(EP_DET_API_RESULT_HEAD);

        int nTmpLen = 0;

        if (pHead->nDetROICnt < nMaxROICnt)  nMaxROICnt = pHead->nDetROICnt;
        nTmpLen = sizeof(DET_ROI) * nMaxROICnt;
        if (rgROI != NULL) memcpy(rgROI, pData, nTmpLen);
        pData += sizeof(DET_ROI) * pHead->nDetROICnt;  
        if (pDetROICnt != NULL)  *pDetROICnt = nMaxROICnt;

        if (pHead->nPlateROICnt < nMaxPlateROICnt)  nMaxPlateROICnt = pHead->nPlateROICnt;
        nTmpLen = sizeof(RECTA) * nMaxPlateROICnt;
        if (rgPlateROI != NULL) memcpy(rgPlateROI, pData, nTmpLen);
        pData += sizeof(RECTA) * pHead->nPlateROICnt; 
        if (pPlateCnt != NULL) *pPlateCnt = pHead->nPlateROICnt;

        return RS_S_OK;
    }

    // 将组装过的数据拆分到指针上，引用关系，直接使用源数据
    sv::SV_RESULT SeparateResultRefPoint(
        void* pBuf,    //
        int nBufLen,
        unsigned int* pFrameTime,
        DET_ROI** prgROI,  
        int* pDetROICnt, 
        RECTA** prgPlateROI, 
        int* pPlateCnt
        )
    {
        EP_DET_API_RESULT_HEAD* pHead = (EP_DET_API_RESULT_HEAD*)pBuf;

        if (pBuf == NULL || nBufLen == 0  || pHead->nCheckID != DET_HEAD_CHECK_ID)
        {
            if (prgROI != NULL) *prgROI = NULL;
            if (pDetROICnt != NULL) *pDetROICnt = 0;
            if (prgPlateROI != NULL) *prgPlateROI = NULL;
            if (pPlateCnt != NULL) *pPlateCnt = 0;
            return RS_S_OK;
        }

        if (pFrameTime != NULL)
        {
            *pFrameTime = pHead->nFrameTime;
        }
        char* pData = (char*)pBuf;
        pData += sizeof(EP_DET_API_RESULT_HEAD);

        int nTmpLen = 0;

        nTmpLen = sizeof(DET_ROI) * pHead->nDetROICnt;
        if (prgROI != NULL) *prgROI = (DET_ROI*)pData;
        pData += nTmpLen;  
        if (pDetROICnt != NULL) *pDetROICnt = pHead->nDetROICnt;

        nTmpLen = sizeof(RECTA) * pHead->nPlateROICnt;
        if (prgPlateROI != NULL) *prgPlateROI = (RECTA*)pData;
        pData += nTmpLen; 
        if (pPlateCnt != NULL) *pPlateCnt = pHead->nPlateROICnt;

        return RS_S_OK;
    }
}
