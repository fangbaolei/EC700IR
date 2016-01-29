#include "TLDetResult.h"

using namespace sv;

#define DET_HEAD_CHECK_ID 2424

namespace svTLDetApi
{
    // 将得到的数据组装成BUF
    sv::SV_RESULT MakeResultBuf(
        int nAvgY,
        SV_PLATERECT* rgPlateData, 
        int nPlateCnt,
		DET_ROI* rgROI,  
        int nDetROICnt, 
        void* pBuf,    // 可为空，为空时 pBufLen 将直接返回数据长度
        int nMaxBufLen,
        int* pBufLen   // 返回数据长度
        )
    {
        int nSize = sizeof(TL_DET_API_RESULT_HEAD) + sizeof(nAvgY) + sizeof(DET_ROI) * nDetROICnt 
			+ sizeof(SV_PLATERECT) * nPlateCnt;  
        if (pBufLen != NULL)
        {
            *pBufLen = nSize;
        }
        // 允许pBuf为空时返回大小
        if (pBuf != NULL && nSize <= nMaxBufLen)
        {
            char* pData = (char*)pBuf;

            int nTmpLen = 0;
 
            TL_DET_API_RESULT_HEAD* pHead = (TL_DET_API_RESULT_HEAD*)pBuf;
            pHead->nCheckID = DET_HEAD_CHECK_ID;
            pHead->nPlateROICnt = nPlateCnt;
			pHead->nDetROICnt = nDetROICnt;
            nTmpLen = sizeof(TL_DET_API_RESULT_HEAD);
            pData += nTmpLen;

            nTmpLen = sizeof(nAvgY);
            memcpy(pData, &nAvgY, nTmpLen);
            pData += nTmpLen;

            nTmpLen = sizeof(SV_PLATERECT)*nPlateCnt;  
            memcpy(pData, rgPlateData, nTmpLen);
            pData += nTmpLen;
			
			nTmpLen = sizeof(DET_ROI)*nDetROICnt;  
            memcpy(pData, rgROI, nTmpLen);
            pData += nTmpLen;
        }

        return RS_S_OK;
    }

    // 将组装过的数据拆分成BUF
    sv::SV_RESULT SeparateResultBuf(
        void* pBuf,    
        int nBufLen,
        int* pnAvgY,
        SV_PLATERECT* rgPlateROI, 
        int nMaxPlateROICnt,
        int* pPlateCnt,
		DET_ROI* rgROI, 
        int nMaxROICnt,
        int* pDetROICnt
        )
    {
        TL_DET_API_RESULT_HEAD* pHead = (TL_DET_API_RESULT_HEAD*)pBuf;

        if (pBuf == NULL || nBufLen == 0 || pHead->nCheckID != DET_HEAD_CHECK_ID)
        {
            if (pDetROICnt != NULL) *pDetROICnt = 0;
            if (pPlateCnt != NULL) *pPlateCnt = 0;
            return RS_S_OK;
        }

        char* pData = (char*)pBuf;
        pData += sizeof(TL_DET_API_RESULT_HEAD);

        int nTmpLen = 0;

        if (pnAvgY != NULL) memcpy(pnAvgY, pData, sizeof(int));
        pData += sizeof(int); 

        if (pHead->nPlateROICnt < nMaxPlateROICnt)  nMaxPlateROICnt = pHead->nPlateROICnt;
        nTmpLen = sizeof(rgPlateROI) * nMaxPlateROICnt;
        if (rgPlateROI != NULL) memcpy(rgPlateROI, pData, nTmpLen);
        pData += sizeof(rgPlateROI) * pHead->nPlateROICnt; 
        if (pPlateCnt != NULL) *pPlateCnt = pHead->nPlateROICnt;

        if (pHead->nDetROICnt < nMaxROICnt)  nMaxROICnt = pHead->nDetROICnt;
        nTmpLen = sizeof(DET_ROI) * nMaxROICnt;
        if (rgROI != NULL) memcpy(rgROI, pData, nTmpLen);
        pData += sizeof(DET_ROI) * pHead->nDetROICnt;  
        if (pDetROICnt != NULL)  *pDetROICnt = nMaxROICnt;
		
        return RS_S_OK;
    }

    // 将组装过的数据拆分到指针上，引用关系，直接使用源数据
    sv::SV_RESULT SeparateResultRefPoint(
        void* pBuf,   
        int nBufLen,
        int* pnAvgY,
        SV_PLATERECT** prgPlateROI, 
        int* pPlateCnt,
		DET_ROI** prgROI,  
        int* pDetROICnt
        )
    {
        TL_DET_API_RESULT_HEAD* pHead = (TL_DET_API_RESULT_HEAD*)pBuf;

        if (pBuf == NULL || nBufLen == 0  || pHead->nCheckID != DET_HEAD_CHECK_ID)
        {
            if (prgPlateROI != NULL) *prgPlateROI = NULL;
            if (pPlateCnt != NULL) *pPlateCnt = 0;
            return RS_S_OK;
        }

        char* pData = (char*)pBuf;
        pData += sizeof(TL_DET_API_RESULT_HEAD);

        int nTmpLen = 0;

        if (pnAvgY != NULL) memcpy(pnAvgY, pData, sizeof(int));
        pData += sizeof(int); 

        nTmpLen = sizeof(SV_PLATERECT) * pHead->nPlateROICnt;
        if (prgPlateROI != NULL) *prgPlateROI = (SV_PLATERECT*)pData;
        pData += nTmpLen; 
        if (pPlateCnt != NULL) *pPlateCnt = pHead->nPlateROICnt;

        nTmpLen = sizeof(DET_ROI) * pHead->nDetROICnt;
        if (prgROI != NULL) *prgROI = (DET_ROI*)pData;
        pData += nTmpLen;  
        if (pDetROICnt != NULL) *pDetROICnt = pHead->nDetROICnt;
		
        return RS_S_OK;
    }
}
