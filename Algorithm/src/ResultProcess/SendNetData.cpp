// 该文件格式必须为WINDOWS-936格式
#include "sendnetdata.h"
#include "DataLinkImpl.h"
#include "assert.h"
#include "ResultSenderImpl.h"

static DWORD32 g_dwNowCarID = 0;

//得到小图的YUV数据结构体
bool GetHVIOSmallImg(HVIO_SmallImage* pcSmallImage, const HV_COMPONENT_IMAGE &image)
{
    if ( pcSmallImage == NULL )
    {
        return false;
    }

    pcSmallImage->m_wImageWidth = image.iWidth & ( ~0x1 );
    pcSmallImage->m_wImageHeight = image.iHeight;
    pcSmallImage->m_wImageType = IMAGE_YUV;
    pcSmallImage->m_wType = _TYPE_SMALL_IMAGE;
    pcSmallImage->m_wImageOffset = 0;
    pcSmallImage->m_dwImageSize = pcSmallImage->m_wImageWidth * pcSmallImage->m_wImageHeight * 2;
    pcSmallImage->m_dwLen = (unsigned int)( ( QWORD64 )pcSmallImage->m_pImage - ( QWORD64 )&pcSmallImage->m_dwLen ) + pcSmallImage->m_wImageOffset + pcSmallImage->m_dwImageSize;

    unsigned char *pDest = pcSmallImage->m_pImage;
    unsigned char *pSrc =  GetHvImageData(&image, 0);
    int iHeight;
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += image.iStrideWidth[0], pDest += pcSmallImage->m_wImageWidth )
    {
        HV_memcpy( pDest, pSrc, ( size_t )pcSmallImage->m_wImageWidth );
    }
    pSrc =  GetHvImageData(&image,1);
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += (image.iStrideWidth[0] / 2), pDest += pcSmallImage->m_wImageWidth / 2 )
    {
        HV_memcpy( pDest, pSrc, ( size_t )pcSmallImage->m_wImageWidth / 2 );
    }
    pSrc =  GetHvImageData(&image,2);
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += (image.iStrideWidth[0] / 2), pDest += pcSmallImage->m_wImageWidth / 2 )
    {
        HV_memcpy( pDest, pSrc, ( size_t )pcSmallImage->m_wImageWidth / 2 );
    }

    return true;
}

//得到二值图的数据结构体
bool GetHVIOBinaryImg(HVIO_SmallImage* pcBinaryImg, const HV_COMPONENT_IMAGE &image)
{
    if ( pcBinaryImg == NULL )
    {
        return false;
    }

    pcBinaryImg->m_wImageWidth =  image.iWidth;
    pcBinaryImg->m_wImageHeight = image.iHeight;
    pcBinaryImg->m_wImageType = IMAGE_BIN;
    pcBinaryImg->m_wType = _TYPE_SMALL_IMAGE;
    pcBinaryImg->m_wImageOffset = 0;
    pcBinaryImg->m_dwImageSize = (pcBinaryImg->m_wImageWidth >> 3) * pcBinaryImg->m_wImageHeight;
    pcBinaryImg->m_dwLen = (unsigned int)( ( QWORD64 )pcBinaryImg->m_pImage - ( QWORD64 )&pcBinaryImg->m_dwLen ) + pcBinaryImg->m_wImageOffset + pcBinaryImg->m_dwImageSize;

    unsigned char *pDest = pcBinaryImg->m_pImage;
    unsigned char *pSrc = GetHvImageData(&image, 0);
    int iHeight;

    int iDestLine = pcBinaryImg->m_wImageWidth >> 3;
    //int iSrcLine  =  image.iStrideWidth[0] >> 3;  //   image.iStrideWidth[0] 这个是什么?
    int iSrcLine = pcBinaryImg->m_wImageWidth >> 3;
    for ( iHeight = 0; iHeight < pcBinaryImg->m_wImageHeight; iHeight ++, pSrc += iSrcLine, pDest += iDestLine )
    {
        HV_memcpy( pDest, pSrc, ( size_t )iDestLine );
    }

    return true;
}

//得到大图的数据结构体
bool GetHVIOBigImg( HVIO_BigImage* pcBigImg, const HV_COMPONENT_IMAGE &image, unsigned char* pbOffsetInfo, WORD16 wOffsetSize )
{
    if ( pcBigImg == NULL )
    {
        return false;
    }
    if ( image.nImgType != HV_IMAGE_JPEG )
    {
        return false;
    }

    if ( pbOffsetInfo != NULL && wOffsetSize > 0 )
    {
        pcBigImg->m_wImageOffset = wOffsetSize;
    }
    else
    {
        pcBigImg->m_wImageOffset = 0;
    }

    PBYTE8 pbJpegBuf = NULL;
    DWORD32 jpeg_size = 0;

    pbJpegBuf = GetHvImageData(&image, 0);
    jpeg_size = ((image.iWidth + pcBigImg->m_wImageOffset) > BIGIMG_BUFSIZE) ? (BIGIMG_BUFSIZE - pcBigImg->m_wImageOffset) : image.iWidth;

    pcBigImg->m_wImageWidth = 0;
    pcBigImg->m_wImageHeight = 0;
    pcBigImg->m_wImageType = IMAGE_JPEG;
    pcBigImg->m_dwImageSize =  jpeg_size;
    pcBigImg->m_dwLen = (unsigned int)( ( QWORD64 )pcBigImg->m_pImage - ( QWORD64 )&pcBigImg->m_dwLen ) + pcBigImg->m_wImageOffset + pcBigImg->m_dwImageSize;
    pcBigImg->m_wType = _TYPE_BIG_IMAGE;

    if ( pcBigImg->m_wImageOffset > 0 )
    {
        HV_memcpy(pcBigImg->m_pImage, pbOffsetInfo, pcBigImg->m_wImageOffset);
    }
    HV_memcpy( pcBigImg->m_pImage + pcBigImg->m_wImageOffset, pbJpegBuf, jpeg_size );

    return true;
}

//发送车辆信息开始接收标志
int SendInfoBegin( IResultIO* pResultIO, DWORD32 dwVideoID )
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;

    HVIO_CarInfoBegin *pcCarInfoBegin;

    int index = -1;
    pcCarInfoBegin = CDataLinkThread::s_queCarInfoBegin.GetEmptyCell( &index );
    if ( pcCarInfoBegin == NULL ) return S_FALSE;

    pcCarInfoBegin->m_dwLen = 8;
    pcCarInfoBegin->m_wType = _TYPE_CARINFO_BEGIN;
    pcCarInfoBegin->m_dwCarID = g_dwNowCarID;
    pcCarInfoBegin->m_wVideoID = dwVideoID;

    return pResultIO->SendInfoBegin(index);
}

//发送车辆信息结束接收标志
int SendInfoEnd( IResultIO* pResultIO, DWORD32 dwVideoID )
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;

    HVIO_CarInfoEnd *pcCarInfoEnd;
    int index = -1;
    pcCarInfoEnd = CDataLinkThread::s_queCarInfoEnd.GetEmptyCell( &index );
    if ( pcCarInfoEnd == NULL ) return S_FALSE;

    pcCarInfoEnd->m_dwLen = 8;
    pcCarInfoEnd->m_wType = _TYPE_CARINFO_END;
    pcCarInfoEnd->m_dwCarID = g_dwNowCarID;
    pcCarInfoEnd->m_wVideoID = dwVideoID;

    return pResultIO->SendInfoEnd(index);
}

//发送车辆到达离开标志
int SendCarEnterLeaveFlag( IResultIO* pResultIO, DWORD32 dwVideoID, WORD16 wInLeftFlag, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh )
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;

    static DWORD32 dwLastCarID = (DWORD32)(-1);

    if ( g_dwNowCarID != dwLastCarID )
    {
        HVIO_CarInLeft *pcCarInLeft;
        int index = -1;
        pcCarInLeft = CDataLinkThread::s_queCarInLeft.GetEmptyCell( &index );
        if ( pcCarInLeft == NULL ) return S_FALSE;

        pcCarInLeft->m_dwLen = 18;
        pcCarInLeft->m_wType = _TYPE_CAR_IN_AND_LEFT;
        pcCarInLeft->m_wVideoID = dwVideoID;
        pcCarInLeft->m_dwCarID = (g_dwNowCarID+1);
        pcCarInLeft->m_dwTimeLow = dwTimeMsLow;
        pcCarInLeft->m_dwTimeHigh = dwTimeMsHigh;
        pcCarInLeft->m_wInLeftFlag = wInLeftFlag;

        dwLastCarID = g_dwNowCarID;

        return pResultIO->SendInLeftFlag(index);
    }

    return S_FALSE;
}

//发送车牌信息
int SendResultText( IResultIO* pResultIO, DWORD32 dwVideoID, const char *pResult, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh )
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;
    assert( pResult != NULL );

    WORD16 wPlateLen;
    wPlateLen = (WORD16)strlen( pResult );
    if ( wPlateLen >= 65535 ) return E_OUTOFMEMORY;

    int index = -1;
    HVIO_CarPlate* pcCarPlate;
    pcCarPlate = CDataLinkThread::s_queCarPlate.GetEmptyCell( &index );
    if ( pcCarPlate == NULL ) return S_FALSE;

    pcCarPlate->m_dwLen = 16 + wPlateLen + 1;
    pcCarPlate->m_wType = _TYPE_PLATE_STR;
    pcCarPlate->m_wVideoID = dwVideoID;
    pcCarPlate->m_dwCarID = g_dwNowCarID;
    pcCarPlate->m_dwTimeLow = dwTimeMsLow;
    pcCarPlate->m_dwTimeHigh = dwTimeMsHigh;
    strcpy( pcCarPlate->m_pCarPlate, pResult );
    pcCarPlate->m_pCarPlate[wPlateLen] = '\0';

    return pResultIO->SendCarPlate(index);
}

//发送Jpeg大图
int SendBigImg(
    IResultIO* pResultIO,
    IReferenceComponentImage *pBigImg,
    WORD16 wImageID,
    DWORD32 dwVideoID,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;
    if ( pBigImg == NULL ) return S_FALSE;

    HV_COMPONENT_IMAGE imgTemp;
    IReferenceComponentImage* pJpegImg = pBigImg;
    if (pJpegImg)
    {
        HV_COMPONENT_IMAGE imgJpeg;
        if (S_OK == pJpegImg->GetImage(&imgJpeg) && imgJpeg.nImgType == HV_IMAGE_JPEG)
        {
            imgTemp = imgJpeg;
        }
        else
        {
            return S_FALSE;
        }
    }
    else
    {
        return S_FALSE;
    }

    int iSrcWidth = imgTemp.iWidth;
    int iSrcHeight = imgTemp.iHeight;
    int iSrcStrideWidth = imgTemp.iStrideWidth[0];

    bool fIsJpg = false;
    PBYTE8 pbJpegBuf = NULL;
    DWORD32 jpeg_size = 0;
    if (imgTemp.nImgType == HV_IMAGE_JPEG)
    {
        pbJpegBuf = GetHvImageData(&imgTemp, 0);
        jpeg_size = imgTemp.iWidth;

        iSrcWidth = (imgTemp.iHeight & 0xFFFF);
        iSrcHeight = (imgTemp.iHeight >> 16);

        fIsJpg = true;
    }
    if (pbJpegBuf == NULL) return S_FALSE;

    //读取参数
    int iSaveType = CResultSender::m_cParam.iSaveType;
    int iWidth = CResultSender::m_cParam.iWidth;
    int iHeigh = CResultSender::m_cParam.iHeight;

    if ( wImageID == BEST_SNAPSHOT )
    {
        if ( iSaveType == 1 || iSaveType == 2 )
        {
            iSrcWidth = iWidth;
            iSrcHeight = iHeigh;
        }
    }
    else if ( wImageID == LAST_SNAPSHOT )
    {
        if ( iSaveType == 1 || iSaveType == 3 )
        {
            iSrcWidth = iWidth;
            iSrcHeight = iHeigh;
        }
    }

    WORD16 wPlateWidth(0);
    WORD16 wPlateHeight(0);
    PBYTE8 pbInfoBuf = NULL;
    WORD16 wInfoSize = 0;

    //---
    WORD16 wOffset = 0;
    if ( wInfoSize > 0 && pbInfoBuf != NULL )
    {
        wOffset = wInfoSize;
    }
    if ((jpeg_size + wOffset) > BIGIMG_BUFSIZE) return E_OUTOFMEMORY;
    if ( pbJpegBuf == NULL ) return E_FAIL;

    int index = -1;
    HVIO_BigImage *pcBigImage = CDataLinkThread::s_queBigImage.GetEmptyCell( &index );
    if ( pcBigImage == NULL ) return S_FALSE;

    if ( wInfoSize > 0 && pbInfoBuf != NULL )
    {
        pcBigImage->m_wImageOffset = wInfoSize;
    }
    else
    {
        pcBigImage->m_wImageOffset = 0;
    }

    pcBigImage->m_wVideoID = dwVideoID;
    pcBigImage->m_dwCarID = g_dwNowCarID;
    pcBigImage->m_dwTimeLow = dwTimeMsLow;
    pcBigImage->m_dwTimeHigh = dwTimeMsHigh;
    pcBigImage->m_wImageID = wImageID;
    pcBigImage->m_wReserved = 0;
    pcBigImage->m_wPlateWidth = wPlateWidth;
    pcBigImage->m_wPlateHeight = wPlateHeight;
    pcBigImage->m_wImageWidth = iSrcWidth & ( ~0x1 );
    pcBigImage->m_wImageHeight = iSrcHeight;
    pcBigImage->m_wImageType = IMAGE_JPEG;
    pcBigImage->m_dwImageSize =  jpeg_size;
    pcBigImage->m_dwLen = ( ( DWORD32 )pcBigImage->m_pImage - ( DWORD32 )&pcBigImage->m_dwLen ) + pcBigImage->m_wImageOffset + pcBigImage->m_dwImageSize;
    pcBigImage->m_wType = _TYPE_BIG_IMAGE;

    if ( pcBigImage->m_wImageOffset > 0 )
    {
        HV_memcpy(pcBigImage->m_pImage, pbInfoBuf, pcBigImage->m_wImageOffset);
    }
    HV_memcpy( pcBigImage->m_pImage + pcBigImage->m_wImageOffset, pbJpegBuf, jpeg_size );

    HRESULT hr = pResultIO->SendBigImage(index);
    //---

    SAFE_DELETE(pbInfoBuf);
    return hr;
}

//发送YUV小图
int SendSmallImg(
    IResultIO* pResultIO,
    IReferenceComponentImage *pSmallImg,
    DWORD32 dwVideoID,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;
    if ( pSmallImg == NULL ) return S_FALSE;

    HV_COMPONENT_IMAGE imgTemp;
    if (FAILED(pSmallImg->GetImage(&imgTemp))) return E_FAIL;

    WORD16 wImageWidth = imgTemp.iWidth;
    WORD16 wImageHeight = imgTemp.iHeight;
    WORD16 wStrideWidth = imgTemp.iStrideWidth[0];

    unsigned char *pY = GetHvImageData(&imgTemp, 0);
    unsigned char *pCb = GetHvImageData(&imgTemp,1);
    unsigned char *pCr = GetHvImageData(&imgTemp, 2);

    //---
    if ( ( pY == NULL ) || ( pCb == NULL ) || ( pCr == NULL ) )
    {
        return E_FAIL;
    }

    int index = -1;
    HVIO_SmallImage *pcSmallImage = CDataLinkThread::s_queSmallImage.GetEmptyCell( &index );
    if ( pcSmallImage == NULL ) return S_FALSE;

    pcSmallImage->m_wVideoID = dwVideoID;
    pcSmallImage->m_dwCarID = g_dwNowCarID;
    pcSmallImage->m_dwTimeLow = dwTimeMsLow;
    pcSmallImage->m_dwTimeHigh = dwTimeMsHigh;
    pcSmallImage->m_wImageWidth = wImageWidth & ( ~0x1 );
    pcSmallImage->m_wImageHeight = wImageHeight;
    pcSmallImage->m_wImageType = IMAGE_YUV;
    pcSmallImage->m_wImageOffset = 0;
    pcSmallImage->m_dwImageSize = pcSmallImage->m_wImageWidth * pcSmallImage->m_wImageHeight * 2;
    pcSmallImage->m_dwLen = ( ( DWORD32 )pcSmallImage->m_pImage - ( DWORD32 )&pcSmallImage->m_dwLen ) + pcSmallImage->m_wImageOffset + pcSmallImage->m_dwImageSize;
    pcSmallImage->m_wType = _TYPE_SMALL_IMAGE;

    unsigned char *pDest = pcSmallImage->m_pImage;
    unsigned char *pSrc = pY;
    int iHeight;
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += wStrideWidth, pDest += pcSmallImage->m_wImageWidth )
    {
        HV_memcpy( pDest, pSrc, ( size_t )pcSmallImage->m_wImageWidth );
    }
    pSrc = pCb;
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += wStrideWidth / 2, pDest += pcSmallImage->m_wImageWidth / 2 )
    {
        HV_memcpy( pDest, pSrc, ( size_t )pcSmallImage->m_wImageWidth / 2 );
    }
    pSrc = pCr;
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += wStrideWidth / 2, pDest += pcSmallImage->m_wImageWidth / 2 )
    {
        HV_memcpy( pDest, pSrc, ( size_t )pcSmallImage->m_wImageWidth / 2 );
    }

    HRESULT hr = pResultIO->SendSmallImageYUV(index);
    //---

    return hr;
}

//发送二值化小图
int SendBinaryImg(
    IResultIO* pResultIO,
    IReferenceComponentImage *pBinImg,
    DWORD32 dwVideoID,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;
    if ( pBinImg == NULL ) return S_FALSE;

    HV_COMPONENT_IMAGE imgTemp;
    if (FAILED(pBinImg->GetImage(&imgTemp))) return E_FAIL;

    WORD16 wImageWidth = imgTemp.iWidth;
    WORD16 wImageHeight = imgTemp.iHeight;
    WORD16 wStrideWidth = imgTemp.iWidth;

    unsigned char *pBin = GetHvImageData(&imgTemp, 0);

    //---
    if ( pBin == NULL ) return E_FAIL;

    int index = -1;
    HVIO_SmallImage *pcSmallImage = CDataLinkThread::s_queSmallImage.GetEmptyCell( &index );
    if ( pcSmallImage == NULL ) return S_FALSE;

    pcSmallImage->m_wVideoID = dwVideoID;
    pcSmallImage->m_dwCarID = g_dwNowCarID;
    pcSmallImage->m_dwTimeLow = dwTimeMsLow;
    pcSmallImage->m_dwTimeHigh = dwTimeMsHigh;
    pcSmallImage->m_wImageWidth = wImageWidth;
    pcSmallImage->m_wImageHeight = wImageHeight;
    pcSmallImage->m_wImageType = IMAGE_BIN;
    pcSmallImage->m_wImageOffset = 0;
    pcSmallImage->m_dwImageSize = (pcSmallImage->m_wImageWidth >> 3) * pcSmallImage->m_wImageHeight;
    pcSmallImage->m_dwLen = ( ( DWORD32 )pcSmallImage->m_pImage - ( DWORD32 )&pcSmallImage->m_dwLen ) + pcSmallImage->m_wImageOffset + pcSmallImage->m_dwImageSize;
    pcSmallImage->m_wType = _TYPE_SMALL_IMAGE;

    unsigned char *pDest = pcSmallImage->m_pImage;
    unsigned char *pSrc = pBin;
    int iHeight;

    int iDestLine = pcSmallImage->m_wImageWidth >> 3;
    int iSrcLine  = wStrideWidth >> 3;
    for ( iHeight = 0; iHeight < pcSmallImage->m_wImageHeight; iHeight ++, pSrc += iSrcLine, pDest += iDestLine )
    {
        HV_memcpy( pDest, pSrc, ( size_t )iDestLine );
    }

    return pResultIO->SendBinaryImage(index);
    //---
}

// 发送视频
HRESULT SendVideo(
    IResultIO* pResultIO,
	DWORD32 dwVideoID,
	BYTE8* pImgData,
	DWORD32 dwImgSize,
	DWORD32* pdwSendCount,
	DWORD32 dwImgTime,
	int nRectCount/* = 0*/,
	HV_RECT *pRect/* = NULL*/
	)
{
	if (dwImgSize > BIGIMG_BUFSIZE) return E_OUTOFMEMORY;

	if (pdwSendCount != NULL) *pdwSendCount = 0;

	static DWORD32 dwLastSendTime = 0;

	if (0 == dwImgTime)
	{
	}
	else
	{
		if( dwImgTime -  dwLastSendTime < CResultSender::m_cParam.iVideoDisplayTime )
		{
			return S_OK;
		}
		else
		{
			dwLastSendTime = dwImgTime;
		}
	}

	if ( pImgData == NULL ) return E_INVALIDARG;

	// 判断是否设置了视频回调
	DWORD32 dwCount;
	if (pResultIO->SendVideoYUV(-1, &dwCount) != S_OK)
	{
		return E_FAIL;
	}
	else
	{
		if (dwCount <= 0)
		{
			return E_FAIL;
		}
	}

	int index = -1;
	HVIO_Video *pcVideo = CDataLinkThread::s_queVideo.GetEmptyCell( &index );

	if (pcVideo == NULL)
	{
		return S_FALSE;
	}

	pcVideo->m_wVideoID = dwVideoID;
	pcVideo->m_wImageType = IMAGE_JPEG;
	pcVideo->m_wImageOffset = 0;
	// 添加标记框到视频协议包中
	PBYTE8 pbDest = pcVideo->m_pImage;
	// 旋转标志
	if (CResultSender::m_cParam.iEddyType)
	{
		memcpy(pbDest, "EddyLeft", 8);
		pbDest += 8;
		pcVideo->m_wImageOffset += 8;
	}
	// 画框标志
	if (nRectCount && CResultSender::m_cParam.iDrawRect)
	{
		memcpy(pbDest, "rect", sizeof(int));
		pbDest += sizeof(int);
		pcVideo->m_wImageOffset += sizeof(int);
		memcpy(pbDest, &nRectCount, sizeof(int));
		pbDest += sizeof(int);
		pcVideo->m_wImageOffset += sizeof(int);
		for (int i = 0; i < nRectCount; i++)
		{
			pRect[i].top *= 2;
			pRect[i].bottom *= 2;
			memcpy(pbDest, &pRect[i], sizeof(HV_RECT));
			pbDest += sizeof(HV_RECT);
			pcVideo->m_wImageOffset += sizeof(HV_RECT);
		}
	}
	pcVideo->m_dwImageSize = dwImgSize;
	pcVideo->m_dwLen = ( ( DWORD32 )&pcVideo->m_pImage - ( DWORD32 )&pcVideo->m_dwLen ) + pcVideo->m_wImageOffset + pcVideo->m_dwImageSize;
	pcVideo->m_wType = _TYPE_FULL_CMP_IMAGE;
	HV_memcpy(pbDest, pImgData, dwImgSize );

	return pResultIO->SendVideoYUV(index, pdwSendCount);
}

//发送结果
//comment by Shaorg: 应用层不能直接调用这个函数，因为g_dwNowCarID没有在这里递增。
int SendResult(
    IResultIO* pResultIO,
    const char* pPlateStr,
    RESULT_IMAGE_STRUCT* pResultImage,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    if ( NULL == pResultIO || NULL == pPlateStr || NULL == pResultImage ) return S_FALSE;
    SendInfoBegin(pResultIO, 0);
    SendSmallImg(pResultIO, pResultImage->pimgPlate, 0, dwTimeMsLow, dwTimeMsHigh);
    SendBinaryImg(pResultIO, pResultImage->pimgPlateBin, 0, dwTimeMsLow, dwTimeMsHigh);

    //读取参数
    int iBestSnapshotOutput = CResultSender::m_cParam.iBestSnapshotOutput;
    int iLastSnapshotOutput = CResultSender::m_cParam.iLastSnapshotOutput;
    int iCaptureImageOutput = CResultSender::m_cParam.iOutputCaptureImage;
    if ( iBestSnapshotOutput != 0 )
    {
        SendBigImg(pResultIO, pResultImage->pimgBestSnapShot, BEST_SNAPSHOT, 0, dwTimeMsLow, dwTimeMsHigh);
    }
    if ( iLastSnapshotOutput != 0 )
    {
        SendBigImg(pResultIO, pResultImage->pimgLastSnapShot, LAST_SNAPSHOT, 0, dwTimeMsLow, dwTimeMsHigh);
    }
    if ( iCaptureImageOutput != 0 || strstr(pPlateStr, "违章:是") != NULL )
    {
        SendBigImg(pResultIO, pResultImage->pimgBeginCapture, BEGIN_CAPTURE, 0, dwTimeMsLow, dwTimeMsHigh);
        SendBigImg(pResultIO, pResultImage->pimgBestCapture, BEST_CAPTURE, 0, dwTimeMsLow, dwTimeMsHigh);
        SendBigImg(pResultIO, pResultImage->pimgLastCapture, LAST_CAPTURE, 0, dwTimeMsLow, dwTimeMsHigh);
    }

    SendResultText(pResultIO, 0, pPlateStr, dwTimeMsLow, dwTimeMsHigh);

    SendInfoEnd( pResultIO, 0 );

    return S_OK;
}

HRESULT SendString(
    IResultIO* pResultIO,
    WORD16 wVideoID,
    WORD16 wStreamID,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    const char *pString
)
{
    if ( pResultIO == NULL ) return E_OBJ_NO_INIT;
    assert( pString != NULL );

    WORD16 wStringLen;
    wStringLen = (WORD16)strlen( pString );
    if ( wStringLen >= (16 * 1024) ) return E_OUTOFMEMORY;

    int index = -1;
    HVIO_String* pcString;
    pcString = CDataLinkThread::s_queString.GetEmptyCell( &index );
    if ( pcString == NULL ) return S_FALSE;

    pcString->m_dwLen = 16 + wStringLen + 1;
    pcString->m_wType = wStreamID;
    pcString->m_wVideoID = wVideoID;
    pcString->m_dwTimeLow = dwTimeLow;
    pcString->m_dwTimeHigh = dwTimeHigh;
    strcpy(pcString->m_pStr, pString);
    pcString->m_pStr[wStringLen] = '\0';

    return pResultIO->SendString(index);
}

int SafeSendResult(
    IResultIO* pResultIO,
    ISafeSaver* pSafeSaver,
    const char* pPlateStr,
    RESULT_IMAGE_STRUCT* pResultImage,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
//    if ( NULL != pSafeSaver )
//    {
//        //填充安全保存数据结构体
//        CSafeSaverData cSafeData;  //其中申请了至少5MB内存
//
//        HV_COMPONENT_IMAGE imgTemp;
//
//        if ( NULL != pResultImage->pimgPlate)
//        {
//            pResultImage->pimgPlate->GetImage(&imgTemp);
//
//            GetHVIOSmallImg( cSafeData.pSmallImg, imgTemp );
//            cSafeData.pSmallImg->m_dwCarID = g_dwNowCarID;
//            cSafeData.pSmallImg->m_dwTimeLow = dwTimeMsLow;
//            cSafeData.pSmallImg->m_dwTimeHigh = dwTimeMsHigh;
//            cSafeData.pSmallImg->m_wVideoID = 0;
//        }
//
//        if ( NULL != pResultImage->pimgPlateBin )
//        {
//            pResultImage->pimgPlateBin->GetImage(&imgTemp);
//
//            GetHVIOBinaryImg( cSafeData.pBinaryImg, imgTemp );
//            cSafeData.pBinaryImg->m_dwCarID = g_dwNowCarID;
//            cSafeData.pBinaryImg->m_dwTimeLow = dwTimeMsLow;
//            cSafeData.pBinaryImg->m_dwTimeHigh = dwTimeMsHigh;
//            cSafeData.pBinaryImg->m_wVideoID = 0;
//        }
//
//        //图片的时间
//        DWORD32 dwLow = 0;
//        DWORD32 dwHigh = 0;
//
//        //图片的宽高
//        int iSrcWidth = 0;
//        int iSrcHeight = 0;
//
//        int iBestSnapshotOutput = CResultSender::m_cParam.iBestSnapshotOutput;
//        if ( NULL != pResultImage->pimgBestSnapShot && iBestSnapshotOutput != 0 )
//        {
//            pResultImage->pimgBestSnapShot->GetImage( &imgTemp );
//            if (imgTemp.nImgType == HV_IMAGE_JPEG)
//            {
//                iSrcWidth = (imgTemp.iHeight & 0xFFFF);
//                iSrcHeight = (imgTemp.iHeight >> 16);
//
//                ConvertTickToSystemTime(pResultImage->pimgBestSnapShot->GetRefTime(), dwLow, dwHigh);
//                GetHVIOBigImg( cSafeData.rgpBigimg[0], imgTemp );
//                cSafeData.rgpBigimg[0]->m_dwCarID = g_dwNowCarID;
//                cSafeData.rgpBigimg[0]->m_dwTimeLow = dwLow;
//                cSafeData.rgpBigimg[0]->m_dwTimeHigh = dwHigh;
//                cSafeData.rgpBigimg[0]->m_wVideoID = 0;
//                cSafeData.rgpBigimg[0]->m_wImageID = BEST_SNAPSHOT;
//                cSafeData.rgpBigimg[0]->m_wImageWidth = iSrcWidth & ( ~0x1 );
//                cSafeData.rgpBigimg[0]->m_wImageHeight = iSrcHeight;
//            }
//        }
//
//        int iLastSnapshotOutput = CResultSender::m_cParam.iLastSnapshotOutput;
//        if ( NULL != pResultImage->pimgLastSnapShot && iLastSnapshotOutput != 0 )
//        {
//            pResultImage->pimgLastSnapShot->GetImage( &imgTemp );
//            if (imgTemp.nImgType == HV_IMAGE_JPEG)
//            {
//                iSrcWidth = (imgTemp.iHeight & 0xFFFF);
//                iSrcHeight = (imgTemp.iHeight >> 16);
//
//                ConvertTickToSystemTime(pResultImage->pimgLastSnapShot->GetRefTime(), dwLow, dwHigh);
//                GetHVIOBigImg( cSafeData.rgpBigimg[1], imgTemp );
//                cSafeData.rgpBigimg[1]->m_dwCarID = g_dwNowCarID;
//                cSafeData.rgpBigimg[1]->m_dwTimeLow = dwLow;
//                cSafeData.rgpBigimg[1]->m_dwTimeHigh = dwHigh;
//                cSafeData.rgpBigimg[1]->m_wVideoID = 0;
//                cSafeData.rgpBigimg[1]->m_wImageID = LAST_SNAPSHOT;
//                cSafeData.rgpBigimg[1]->m_wImageWidth = iSrcWidth & ( ~0x1 );
//                cSafeData.rgpBigimg[1]->m_wImageHeight = iSrcHeight;
//            }
//        }
//
//        if ( NULL != pResultImage->pimgBeginCapture )
//        {
//            pResultImage->pimgBeginCapture->GetImage( &imgTemp );
//            if (imgTemp.nImgType == HV_IMAGE_JPEG)
//            {
//                iSrcWidth = (imgTemp.iHeight & 0xFFFF);
//                iSrcHeight = ((imgTemp.iHeight >> 16) & 0xFFFF);
//
//                ConvertTickToSystemTime(pResultImage->pimgBeginCapture->GetRefTime(), dwLow, dwHigh);
//                GetHVIOBigImg( cSafeData.rgpBigimg[2], imgTemp );
//                cSafeData.rgpBigimg[2]->m_dwCarID = g_dwNowCarID;
//                cSafeData.rgpBigimg[2]->m_dwTimeLow = dwLow;
//                cSafeData.rgpBigimg[2]->m_dwTimeHigh = dwHigh;
//                cSafeData.rgpBigimg[2]->m_wVideoID = 0;
//                cSafeData.rgpBigimg[2]->m_wImageID = BEGIN_CAPTURE;
//                cSafeData.rgpBigimg[2]->m_wImageWidth = iSrcWidth;
//                cSafeData.rgpBigimg[2]->m_wImageHeight = iSrcHeight;
//            }
//        }
//        if ( NULL != pResultImage->pimgBestCapture )
//        {
//            pResultImage->pimgBestCapture->GetImage( &imgTemp );
//            if (imgTemp.nImgType == HV_IMAGE_JPEG)
//            {
//                iSrcWidth = (imgTemp.iHeight & 0xFFFF);
//                iSrcHeight = ((imgTemp.iHeight >> 16) & 0xFFFF);
//
//                ConvertTickToSystemTime(pResultImage->pimgBestCapture->GetRefTime(), dwLow, dwHigh);
//                GetHVIOBigImg( cSafeData.rgpBigimg[3], imgTemp );
//                cSafeData.rgpBigimg[3]->m_dwCarID = g_dwNowCarID;
//                cSafeData.rgpBigimg[3]->m_dwTimeLow = dwLow;
//                cSafeData.rgpBigimg[3]->m_dwTimeHigh = dwHigh;
//                cSafeData.rgpBigimg[3]->m_wVideoID = 0;
//                cSafeData.rgpBigimg[3]->m_wImageID = BEST_CAPTURE;
//                cSafeData.rgpBigimg[3]->m_wImageWidth = iSrcWidth;
//                cSafeData.rgpBigimg[3]->m_wImageHeight = iSrcHeight;
//            }
//        }
//        if ( NULL != pResultImage->pimgLastCapture )
//        {
//            pResultImage->pimgLastCapture->GetImage( &imgTemp );
//            if (imgTemp.nImgType == HV_IMAGE_JPEG)
//            {
//                iSrcWidth = (imgTemp.iHeight & 0xFFFF);
//                iSrcHeight = ((imgTemp.iHeight >> 16) & 0xFFFF);
//
//                ConvertTickToSystemTime(pResultImage->pimgLastCapture->GetRefTime(), dwLow, dwHigh);
//                GetHVIOBigImg( cSafeData.rgpBigimg[4], imgTemp );
//                cSafeData.rgpBigimg[4]->m_dwCarID = g_dwNowCarID;
//                cSafeData.rgpBigimg[4]->m_dwTimeLow = dwLow;
//                cSafeData.rgpBigimg[4]->m_dwTimeHigh = dwHigh;
//                cSafeData.rgpBigimg[4]->m_wVideoID = 0;
//                cSafeData.rgpBigimg[4]->m_wImageID = LAST_CAPTURE;
//                cSafeData.rgpBigimg[4]->m_wImageWidth = iSrcWidth;
//                cSafeData.rgpBigimg[4]->m_wImageHeight = iSrcHeight;
//            }
//        }
//
//        //填入车牌信息
//        WORD16 wPlateLen;
//        wPlateLen = (WORD16)strlen( pPlateStr );
//
//        assert( wPlateLen < MAX_PLATE_STRING_SIZE );
//        wPlateLen = wPlateLen < MAX_PLATE_STRING_SIZE ? wPlateLen : MAX_PLATE_STRING_SIZE;
//
//        cSafeData.pPlateNoInfo->m_dwLen = 16 + wPlateLen + 1;
//        cSafeData.pPlateNoInfo->m_wType = _TYPE_PLATE_STR;
//        cSafeData.pPlateNoInfo->m_dwCarID = g_dwNowCarID;
//        cSafeData.pPlateNoInfo->m_dwTimeLow = dwTimeMsLow;
//        cSafeData.pPlateNoInfo->m_dwTimeHigh = dwTimeMsHigh;
//        cSafeData.pPlateNoInfo->m_wVideoID = 0;
//        strncpy(cSafeData.pPlateNoInfo->m_pCarPlate, pPlateStr, wPlateLen);
//        cSafeData.pPlateNoInfo->m_pCarPlate[wPlateLen] = '\0';
//
//        //将识别结果保存在本地硬盘上
//        if ( S_OK == pSafeSaver->SaveData(&cSafeData) )
//        {
//            //ID号用可靠性保存的ID号
//            g_dwNowCarID = cSafeData.pPlateNoInfo->m_dwCarID;
//        }
//        else
//        {
//            ++g_dwNowCarID;
//        }
//    }

    if ( NULL != pResultIO )
    {
        SendResult(pResultIO, pPlateStr, pResultImage, dwTimeMsLow, dwTimeMsHigh);

        if ( NULL == pSafeSaver )
        {
            ++g_dwNowCarID;
        }
    }

    return S_OK;
}
