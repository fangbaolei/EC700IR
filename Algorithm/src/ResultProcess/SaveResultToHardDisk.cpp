#include "hvutils.h"
#include "resultsend.h"
#include "safesaver.h"
#include "HvPciLinkApi.h"

static DWORD32 g_dwNowCarID = 0;

/*
static bool GetHVIOSmallImg(HVIO_SmallImage* pcSmallImage, PBYTE8 pbImageData, DWORD32 dwLen, int iWidth, int iHeight)
{
    if ( pcSmallImage == NULL )
    {
        return false;
    }

    pcSmallImage->m_wImageWidth = iWidth & ( ~0x1 );
    pcSmallImage->m_wImageHeight = iHeight;
    pcSmallImage->m_wImageType = IMAGE_YUV;
    pcSmallImage->m_wType = _TYPE_SMALL_IMAGE;
    pcSmallImage->m_wImageOffset = 0;
    pcSmallImage->m_dwImageSize = pcSmallImage->m_wImageWidth * pcSmallImage->m_wImageHeight * 2;
    pcSmallImage->m_dwLen = (unsigned int)( ( DWORD32 )pcSmallImage->m_pImage - ( DWORD32 )&pcSmallImage->m_dwLen ) + pcSmallImage->m_wImageOffset + pcSmallImage->m_dwImageSize;

    unsigned char *pDest = pcSmallImage->m_pImage;
    HV_memcpy(pDest, pbImageData, (size_t)dwLen);

    return true;
}

static bool GetHVIOBinaryImg(HVIO_SmallImage* pcBinaryImg, PBYTE8 pbImageData, DWORD32 dwLen, int iWidth, int iHeight)
{
    if ( pcBinaryImg == NULL )
    {
        return false;
    }

    pcBinaryImg->m_wImageWidth =  iWidth;
    pcBinaryImg->m_wImageHeight = iHeight;
    pcBinaryImg->m_wImageType = IMAGE_BIN;
    pcBinaryImg->m_wType = _TYPE_SMALL_IMAGE;
    pcBinaryImg->m_wImageOffset = 0;
    pcBinaryImg->m_dwImageSize = (pcBinaryImg->m_wImageWidth >> 3) * pcBinaryImg->m_wImageHeight;
    pcBinaryImg->m_dwLen = (unsigned int)( ( DWORD32 )pcBinaryImg->m_pImage - ( DWORD32 )&pcBinaryImg->m_dwLen ) + pcBinaryImg->m_wImageOffset + pcBinaryImg->m_dwImageSize;

    unsigned char *pDest = pcBinaryImg->m_pImage;
    HV_memcpy(pDest, pbImageData, (size_t)dwLen);

    return true;
}

static bool GetHVIOBigImg(HVIO_BigImage* pcBigImg, PBYTE8 pbImageData, DWORD32 dwLen, int iWidth, int iHeight)
{
    if ( pcBigImg == NULL || dwLen > BIGIMG_BUFSIZE )
    {
        return false;
    }

    PBYTE8 pbJpegBuf = NULL;
    DWORD32 jpeg_size = 0;

    pbJpegBuf = (PBYTE8)pbImageData;
    jpeg_size = dwLen;

    pcBigImg->m_wImageWidth = iWidth;
    pcBigImg->m_wImageHeight = iHeight;
    pcBigImg->m_wImageType = IMAGE_JPEG;
    pcBigImg->m_dwImageSize =  jpeg_size;
    pcBigImg->m_dwLen = (unsigned int)( ( DWORD32 )pcBigImg->m_pImage - ( DWORD32 )&pcBigImg->m_dwLen ) + pcBigImg->m_wImageOffset + pcBigImg->m_dwImageSize;
    pcBigImg->m_wType = _TYPE_BIG_IMAGE;

    HV_memcpy(pcBigImg->m_pImage, pbJpegBuf, jpeg_size);

    return true;
}
*/

HRESULT SaveResultToHardDisk(
    ISafeSaver* pSafeSaver,
    const char* pPlateStr,
    PBYTE8 pbRecord,
    DWORD32 dwRecordSize,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    /* zhaopy
    if ( NULL != pSafeSaver )
    {
        //填充安全保存数据结构体
        CSafeSaverData cSafeData;  //其中申请了至少5MB内存

        PBYTE8 pbData = pbRecord;
        DWORD32 dwRemain = dwRecordSize;

        PCI_IMAGE_INFO cImgInfo;
        DWORD32 dwImgInfoSize;
        DWORD32 dwImgSize;
        PBYTE8 pbImageData;

        while ( dwRemain > 0 )
        {
            memset(&cImgInfo, 0, sizeof(cImgInfo));
            dwImgInfoSize = 0;
            dwImgSize = 0;
            pbImageData = NULL;

            memcpy(&dwImgInfoSize, pbData, 4);
            pbData += 4;
            dwRemain -= 4;

            memcpy(&cImgInfo, pbData, dwImgInfoSize);
            pbData += dwImgInfoSize;
            dwRemain -= dwImgInfoSize;

            memcpy(&dwImgSize, pbData, 4);
            pbData += 4;
            dwRemain -= 4;

            pbImageData = pbData;
            pbData += dwImgSize;
            dwRemain -= dwImgSize;

            switch (cImgInfo.dwImgType)
            {
            case PCILINK_IMAGE_BEST_SNAPSHOT:
                GetHVIOBigImg(cSafeData.rgpBigimg[0], pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.rgpBigimg[0]->m_dwCarID = g_dwNowCarID;
                cSafeData.rgpBigimg[0]->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.rgpBigimg[0]->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.rgpBigimg[0]->m_wVideoID = 0;
                cSafeData.rgpBigimg[0]->m_wImageID = BEST_SNAPSHOT;
                cSafeData.rgpBigimg[0]->m_wImageWidth = cImgInfo.dwImgWidth & ( ~0x1 );
                break;

            case PCILINK_IMAGE_LAST_SNAPSHOT:
                GetHVIOBigImg(cSafeData.rgpBigimg[1], pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.rgpBigimg[1]->m_dwCarID = g_dwNowCarID;
                cSafeData.rgpBigimg[1]->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.rgpBigimg[1]->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.rgpBigimg[1]->m_wVideoID = 0;
                cSafeData.rgpBigimg[1]->m_wImageID = LAST_SNAPSHOT;
                cSafeData.rgpBigimg[1]->m_wImageWidth = cImgInfo.dwImgWidth & ( ~0x1 );
                break;

            case PCILINK_IMAGE_BEGIN_CAPTURE:
                GetHVIOBigImg(cSafeData.rgpBigimg[2], pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.rgpBigimg[2]->m_dwCarID = g_dwNowCarID;
                cSafeData.rgpBigimg[2]->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.rgpBigimg[2]->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.rgpBigimg[2]->m_wVideoID = 0;
                cSafeData.rgpBigimg[2]->m_wImageID = BEGIN_CAPTURE;
                break;

            case PCILINK_IMAGE_BEST_CAPTURE:
                GetHVIOBigImg(cSafeData.rgpBigimg[3], pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.rgpBigimg[3]->m_dwCarID = g_dwNowCarID;
                cSafeData.rgpBigimg[3]->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.rgpBigimg[3]->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.rgpBigimg[3]->m_wVideoID = 0;
                cSafeData.rgpBigimg[3]->m_wImageID = BEST_CAPTURE;
                break;

            case PCILINK_IMAGE_LAST_CAPTURE:
                GetHVIOBigImg(cSafeData.rgpBigimg[4], pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.rgpBigimg[4]->m_dwCarID = g_dwNowCarID;
                cSafeData.rgpBigimg[4]->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.rgpBigimg[4]->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.rgpBigimg[4]->m_wVideoID = 0;
                cSafeData.rgpBigimg[4]->m_wImageID = LAST_CAPTURE;
                break;

            case PCILINK_IMAGE_SMALL_IMAGE:
                GetHVIOSmallImg(cSafeData.pSmallImg, pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.pSmallImg->m_dwCarID = g_dwNowCarID;
                cSafeData.pSmallImg->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.pSmallImg->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.pSmallImg->m_wVideoID = 0;
                break;

            case PCILINK_IMAGE_BIN_IMAGE:
                GetHVIOBinaryImg(cSafeData.pBinaryImg, pbImageData, dwImgSize, cImgInfo.dwImgWidth, cImgInfo.dwImgHeight);
                cSafeData.pBinaryImg->m_dwCarID = g_dwNowCarID;
                cSafeData.pBinaryImg->m_dwTimeLow = cImgInfo.dwTimeLow;
                cSafeData.pBinaryImg->m_dwTimeHigh = cImgInfo.dwTimeHigh;
                cSafeData.pBinaryImg->m_wVideoID = 0;
                break;

            default:
                break;
            }
        }

        //填入车牌信息
        WORD16 wPlateLen;
        wPlateLen = (WORD16)strlen( pPlateStr );

        assert( wPlateLen < MAX_PLATE_STRING_SIZE );
        wPlateLen = wPlateLen < MAX_PLATE_STRING_SIZE ? wPlateLen : MAX_PLATE_STRING_SIZE;

        cSafeData.pPlateNoInfo->m_dwLen = 16 + wPlateLen + 1;
        cSafeData.pPlateNoInfo->m_wType = _TYPE_PLATE_STR;
        cSafeData.pPlateNoInfo->m_dwCarID = g_dwNowCarID;
        cSafeData.pPlateNoInfo->m_dwTimeLow = dwTimeMsLow;
        cSafeData.pPlateNoInfo->m_dwTimeHigh = dwTimeMsHigh;
        cSafeData.pPlateNoInfo->m_wVideoID = 0;
        strncpy(cSafeData.pPlateNoInfo->m_pCarPlate, pPlateStr, wPlateLen);
        cSafeData.pPlateNoInfo->m_pCarPlate[wPlateLen] = '\0';

        //将识别结果保存在本地硬盘上
        if ( S_OK == pSafeSaver->SaveData(&cSafeData) )
        {
            ++g_dwNowCarID;
            return S_OK;
        }
    }
    */

    g_dwNowCarID = g_dwNowCarID; //fix warning
    return E_FAIL;
}
