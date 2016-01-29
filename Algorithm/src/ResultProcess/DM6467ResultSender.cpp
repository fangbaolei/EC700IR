// 该文件编码格式必须为WINDOWS-936格式

#include "DM6467ResultSender.h"
#include "tinyxml.h"

CDM6467ResultSender::CDM6467ResultSender()
{
    m_pRecordLinkCtrl = NULL;
    m_pImageLinkCtrl = NULL;
    m_pSafeSaver = NULL;
    m_iCarID = 0;
    m_fSendRecord = false;
    m_timeLastRecord.wYear = 1970;
    m_timeLastRecord.wMonth = 1;
    m_timeLastRecord.wDay = 1;
    m_timeLastRecord.wHour = 0;
    m_fCarIdSet = FALSE;
}

CDM6467ResultSender::~CDM6467ResultSender()
{
}

void CDM6467ResultSender::EnableRecordSend()
{
    m_fSendRecord = true;
}

void CDM6467ResultSender::DisableRecordSend()
{
#ifdef _HVCAM_PLATFORM_RTM_
    m_fSendRecord = false;
#else
    // 在非RTM版本下，即使加密认证失败也照常发送识别结果。
    m_fSendRecord = true;
#endif
}

bool CDM6467ResultSender::IsSameHour(REAL_TIME_STRUCT* prtLeft, REAL_TIME_STRUCT* prtRight)
{
    if ( prtLeft == NULL || prtRight == NULL )
    {
        return false;
    }

    if ( prtLeft->wYear == prtRight->wYear
            && prtLeft->wMonth == prtRight->wMonth
            && prtLeft->wDay == prtRight->wDay
            && prtLeft->wHour == prtRight->wHour )
    {
        return true;
    }

    return false;
}

HRESULT CDM6467ResultSender::PutResult(
    LPCSTR szResultInfo,
    LPVOID lpcData
)
{
    if (!lpcData || !m_fSendRecord)
    {
        return E_FAIL;
    }
    CARLEFT_INFO_STRUCT *lpCarLeftInfo = (CARLEFT_INFO_STRUCT*)lpcData;

    static char szAppendInfo[4096] = {0};
    static char szValue[32] = {0};

    TiXmlDocument xmlDoc;
    xmlDoc.Parse(szResultInfo);
    if (xmlDoc.Error())
    {
        HV_Trace(5, "%s\n", xmlDoc.ErrorDesc());
        return E_FAIL;
    }
    TiXmlElement* pRootElement = xmlDoc.RootElement();
    TiXmlElement* pResultSet = pRootElement->FirstChildElement("ResultSet");
    TiXmlElement* pResult = pResultSet->FirstChildElement("Result");
    TiXmlElement* pPlateName = pResult->FirstChildElement("PlateName");
    TiXmlElement* pTemp;
    TiXmlElement* pValue;

    DWORD32 dwCarArriveTime = GetSystemTick();
    DWORD32 dwTimeMsLow, dwTimeMsHigh;
    ConvertTickToSystemTime(
        dwCarArriveTime,
        dwTimeMsLow,
        dwTimeMsHigh
    );

    // 计算CarID
    if (!m_fCarIdSet && m_pSafeSaver)
    {
        int iCurIndex = m_pSafeSaver->GetCurRecordIndex();
        if (iCurIndex != -1)
        {
            m_fCarIdSet = TRUE;
            m_iCarID = iCurIndex;
        }
    }
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeMsLow, dwTimeMsHigh, &realtime);
    if (!IsSameHour(&realtime, &m_timeLastRecord))
    {
        m_iCarID = 0;
    }
    else
    {
        ++m_iCarID;
    }
    m_timeLastRecord = realtime;

    // 车辆ID
    pValue = new TiXmlElement("CarID");
    if (pValue)
    {
        sprintf(szValue, "%u", m_iCarID);
        pValue->SetAttribute("value", szValue);
        pResult->LinkEndChild(pValue);
    }

    // 低32位识别结果时间
    pValue = new TiXmlElement("TimeLow");
    if (pValue)
    {
        sprintf(szValue, "%u", dwTimeMsLow);
        pValue->SetAttribute("value", szValue);
        pResult->LinkEndChild(pValue);
    }

    // 高32位识别结果时间
    pValue = new TiXmlElement("TimeHigh");
    if (pValue)
    {
        sprintf(szValue, "%u", dwTimeMsHigh);
        pValue->SetAttribute("value", szValue);
        pResult->LinkEndChild(pValue);
    }

    if (m_pResultSenderParam->cProcRule.fLeach
            || m_pResultSenderParam->cProcRule.fReplace)
    {
        //从XML中解析出车牌字符串和附加信息
        const char* pszPlateName = pPlateName->GetText();
        const char* pszChnName = NULL;
        const char* pszValue = NULL;
        szAppendInfo[0] = 0;
        pTemp = pResult->FirstChildElement();
        while (pTemp)
        {
            pszChnName = pTemp->Attribute("chnname");
            pszValue = pTemp->Attribute("value");
            if (pszChnName && pszValue)
            {
                if (strstr(pszChnName, "事件检测") == NULL)
                {
                    strcat(szAppendInfo, pszChnName);
                    strcat(szAppendInfo, ":");
                    strcat(szAppendInfo, pszValue);
                    strcat(szAppendInfo, "\n");
                }
                else
                {
                    strcat(szAppendInfo, pszValue);
                    strcat(szAppendInfo, "\n");
                }
            }
            pTemp = pTemp->NextSiblingElement();
        }

        RESULT_INFO resultInfo;
        resultInfo.strPlate = pszPlateName;
        resultInfo.strOther = szAppendInfo;
        resultInfo.strApplied = "";

        HV_Trace(5, "%s\n", szAppendInfo);
        if (m_resultFilter.FilterProcess(&resultInfo))
        {
            if (resultInfo.fLeach)
            {
                return S_FALSE;  //不发送要过滤的结果
            }

            pPlateName->FirstChild()->SetValue(resultInfo.strPlate.GetBuffer());
            if (!resultInfo.strApplied.IsEmpty()
                    && m_pResultSenderParam->fOutputFilterInfo)
            {
                pValue = new TiXmlElement("ResultProcess");
                if (pValue)
                {
                    pValue->SetAttribute("value", resultInfo.strApplied.GetBuffer());
                    pValue->SetAttribute("chnname", "后处理信息");
                    pResult->LinkEndChild(pValue);
                }
            }
        }
    }

    //重新生成XML字符串
    TiXmlPrinter cTxPr;
    xmlDoc.Accept(&cTxPr);
    if (xmlDoc.Error())
    {
        HV_Trace(5, "%s\n", xmlDoc.ErrorDesc());
        return E_FAIL;
    }
    char * szPlateInfo = new char[MAX_PLATE_STRING_SIZE];
    memset(szPlateInfo, 0, MAX_PLATE_STRING_SIZE);
    strncpy(szPlateInfo, cTxPr.CStr(), MAX_PLATE_STRING_SIZE);
    //开始网络发送,修改成指定用4兆共享内存空间作为网络发送,2011-09-23
    DWORD32 dwDataSize = 1024 * 1024 * 4;
    HV_Trace(5, "PutResult size = %d\n", dwDataSize);
    if (dwDataSize > 0)
    {
        // zhaopy
        IReferenceMemory* pRefMemory = NULL;
        int iTimes = 0;
        while ( iTimes++ < 10 && S_OK != CreateReferenceMemory(&pRefMemory, dwDataSize) )
        {
            HV_Trace(5, "CreateReferenceMemory failed, retry time=%d...\n", iTimes);
            HV_Sleep(200);
        }
        if (!pRefMemory)
        {
            HV_Trace(5, "pRefMemory is NULL..\n");
            delete []szPlateInfo;
            szPlateInfo = NULL;
            return E_OUTOFMEMORY;
        }
        PBYTE8 pbRecord = NULL;
        pRefMemory->GetData(&pbRecord);
        if (!pbRecord)
        {
            delete []szPlateInfo;
            szPlateInfo = NULL;
            return S_FALSE;
        }
        PBYTE8 buf = pbRecord;
        IReferenceComponentImage *prgImage[5] =
        {
            lpCarLeftInfo->cCoreResult.cResultImg.pimgBestSnapShot,
            lpCarLeftInfo->cCoreResult.cResultImg.pimgLastSnapShot,
            lpCarLeftInfo->cCoreResult.cResultImg.pimgBeginCapture,
            lpCarLeftInfo->cCoreResult.cResultImg.pimgBestCapture,
            lpCarLeftInfo->cCoreResult.cResultImg.pimgLastCapture
        };

        PCILINK_IMAGE_TYPE rgcImageType[5] =
        {
            PCILINK_IMAGE_BEST_SNAPSHOT,
            PCILINK_IMAGE_LAST_SNAPSHOT,
            PCILINK_IMAGE_BEGIN_CAPTURE,
            PCILINK_IMAGE_BEST_CAPTURE,
            PCILINK_IMAGE_LAST_CAPTURE
        };
        if (m_pResultSenderParam->iBestSnapshotOutput)
        {
            buf = CopyBigImageToBuffer(
                      buf,
                      prgImage[0],
                      rgcImageType[0],
                      m_iCarID,
                      lpCarLeftInfo->cCoreResult.rcBestPlatePos,
                      lpCarLeftInfo->cCoreResult.rcRedLightPos,
                      lpCarLeftInfo->cCoreResult.rcRedLightCount
                  );
        }
        if (m_pResultSenderParam->iLastSnapshotOutput)
        {
            buf = CopyBigImageToBuffer(
                      buf,
                      prgImage[1],
                      rgcImageType[1],
                      m_iCarID,
                      lpCarLeftInfo->cCoreResult.rcLastPlatePos,
                      lpCarLeftInfo->cCoreResult.rcRedLightPos,
                      lpCarLeftInfo->cCoreResult.rcRedLightCount
                  );
        }
        if (m_pResultSenderParam->iOutputCaptureImage
                || strstr(szPlateInfo, "违章:是") != NULL || strstr(szPlateInfo, "违章:否&lt") != NULL)
        {
            HV_RECT *rc[3] =
            {
                &lpCarLeftInfo->cCoreResult.rcFirstPos,
                &lpCarLeftInfo->cCoreResult.rcSecondPos,
                &lpCarLeftInfo->cCoreResult.rcThirdPos
            };
            for (int i = 2; i < 5; i++)
            {
                buf = CopyBigImageToBuffer(
                          buf,
                          prgImage[i],
                          rgcImageType[i],
                          m_iCarID,
                          *rc[i - 2],
                          lpCarLeftInfo->cCoreResult.rcRedLightPos,
                          lpCarLeftInfo->cCoreResult.rcRedLightCount
                      );
            }
        }
        buf = CopySmallImageToBuffer(
                  buf,
                  lpCarLeftInfo->cCoreResult.cResultImg.pimgPlate,
                  dwTimeMsLow,
                  dwTimeMsHigh,
                  m_iCarID
              );

        buf = CopyBinImageToBuffer(
                  buf,
                  lpCarLeftInfo->cCoreResult.cResultImg.pimgPlateBin,
                  dwTimeMsLow,
                  dwTimeMsHigh,
                  m_iCarID
              );

        // 发送识别结果
        SEND_RECORD recordInfo;
        recordInfo.iCurCarId = m_iCarID;
        recordInfo.dwRecordType = CAMERA_RECORD_NORMAL;
        recordInfo.pbXML = (PBYTE8)szPlateInfo;
        recordInfo.dwXMLSize = strlen(szPlateInfo) + 128;//预留点空间
        recordInfo.pbRecord = pbRecord;
        recordInfo.dwRecordSize = (DWORD32)(buf - pbRecord);
        recordInfo.pRefMemory = pRefMemory;
        HV_Trace(5, "send data size = %d\n", recordInfo.dwRecordSize);
        if ( FAILED(m_pRecordLinkCtrl->SendRecord(&recordInfo, dwTimeMsLow, dwTimeMsHigh)))
        {
            HV_Trace(5, "<link> SendRecord failed!\n");
        }
        pRefMemory->Release();
    }
    if (szPlateInfo)
    {
        delete []szPlateInfo;
        szPlateInfo = NULL;
    }
    return S_OK;
}

HRESULT CDM6467ResultSender::PutVideo(
    DWORD32* pdwSendCount,
    LPVOID lpFrameData,
    int nRectCount/* = 0*/,
    HV_RECT *pRect/* = NULL*/
)
{
    IMG_FRAME *pFrame = (IMG_FRAME *)lpFrameData;

    DWORD32 dwImgTime = pFrame->pRefImage->GetRefTime();
    static DWORD32 dwLastSendTime = 0;

    if (0 != dwImgTime)
    {
        if (dwImgTime -  dwLastSendTime < (DWORD32)m_pResultSenderParam->iVideoDisplayTime)
        {
            return S_OK;
        }
        else
        {
            dwLastSendTime = dwImgTime;
        }
    }

    //计算图片偏移量
    DWORD32 dwImageOffset = 0;
    //旋转标志
    if (m_pResultSenderParam->iEddyType)
    {
        dwImageOffset += 8;
    }
    //红灯位置
    if (nRectCount > 64)
    {
        nRectCount = 64;
    }

    if (nRectCount && m_pResultSenderParam->iDrawRect)
    {
        dwImageOffset += sizeof(int) + sizeof(int) + sizeof(HV_RECT)*nRectCount;
    }
    //图片信息
    HV_COMPONENT_IMAGE img;
    pFrame->pRefImage->GetImage(&img);
    // zhaopy
    static DWORD32 dwVideoImageSize = 1024 * 1024;
    IReferenceMemory* pRefMemory = NULL;
    if ( S_OK != CreateReferenceMemory(&pRefMemory, dwVideoImageSize) )
    {
        HV_Trace(5, "Put Video pRefMemory is NULL..\n");
        return E_OUTOFMEMORY;
    }
    PBYTE8 pbJpegData = NULL;
    pRefMemory->GetData(&pbJpegData);
    PBYTE8 buf = pbJpegData;

    //图片偏移信息
    if (m_pResultSenderParam->iEddyType)
    {
        memcpy(buf, "EddyLeft", 8);
        buf += 8;
    }
    if (nRectCount && m_pResultSenderParam->iDrawRect)
    {
        memcpy(buf, "rect", sizeof(int));
        buf += sizeof(int);
        memcpy(buf, &nRectCount, sizeof(int));
        buf += sizeof(int);
        for (int i = 0; i < (int)nRectCount; i++)
        {
            pRect[i].top *= 2;
            pRect[i].bottom *= 2;
            memcpy(buf, &pRect[i], sizeof(HV_RECT));
            buf += sizeof(HV_RECT);
        }
    }
    //图片数据
    memcpy(buf, GetHvImageData(&img, 0), img.iWidth);

    SEND_CAMERA_IMAGE imageInfo;
    imageInfo.dwImageType = CAMERA_IMAGE_JPEG_SLAVE;
    imageInfo.dwWidth = img.iHeight & 0x0000FFFF;
    imageInfo.dwHeight = img.iHeight >> 16;
    imageInfo.dwImageSize = dwImageOffset + img.iWidth;
    imageInfo.dwImageOffset = dwImageOffset;
    imageInfo.pbImage = pbJpegData;
    // zhaopy
    imageInfo.pRefMemory = pRefMemory;

    // 发送图片
    if ( m_pImageLinkCtrl != NULL )
    {
        if ( FAILED(m_pImageLinkCtrl->SendCameraImage(&imageInfo)))
        {
            HV_Trace(5, "<link>SendCameraImage failed!\n");
        }
    }

    pRefMemory->Release();
    return S_OK;
}

HRESULT CDM6467ResultSender::PutString(
    WORD16 wVideoID,
    WORD16 wStreamID,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    const char *pString
)
{
    if ( NULL != m_pRecordLinkCtrl )
    {
        char szVideoID[8] = {0};
        char szStreamID[8] = {0};
        char szTimeLow[16] = {0};
        char szTimeHigh[16] = {0};
        sprintf(szVideoID,  "%d", wVideoID);
        sprintf(szStreamID, "%d", wStreamID);
        sprintf(szTimeLow,  "%u", dwTimeLow);
        sprintf(szTimeHigh, "%u", dwTimeHigh);

        /*
        <?xml version="1.0" encoding="GB2312" standalone="yes" ?>
        <HvStringInfo VideoID="xxx" StreamID="xxx" TimeLow="xxx" TimeHigh="xxx" />
        */
        TiXmlDocument cStringInfoXmlDoc;
        TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
        TiXmlElement* pRootElement = new TiXmlElement("HvStringInfo");

        pRootElement->SetAttribute("VideoID", szVideoID);
        pRootElement->SetAttribute("StreamID", szStreamID);
        pRootElement->SetAttribute("TimeLow", szTimeLow);
        pRootElement->SetAttribute("TimeHigh", szTimeHigh);

        cStringInfoXmlDoc.LinkEndChild(pDecl);
        cStringInfoXmlDoc.LinkEndChild(pRootElement);

        TiXmlPrinter cPrinter;
        cStringInfoXmlDoc.Accept(&cPrinter);

        IReferenceMemory* pRefMemory = NULL;
        if ( S_OK != CreateReferenceMemory(&pRefMemory, strlen((char*)pString)+1) )
        {
            HV_Trace(5, "PutString pRefMemory is NULL..\n");
            return E_OUTOFMEMORY;
        }
        PBYTE8 pbStringInfo = NULL;
        pRefMemory->GetData(&pbStringInfo);

        strcpy((char*)pbStringInfo, pString);

        // 发送字符串（事件检测信息）
        SEND_RECORD recordInfo;
        recordInfo.dwRecordType = CAMERA_RECORD_STRING;
        recordInfo.pbXML = (PBYTE8)cPrinter.CStr();
        recordInfo.dwXMLSize = cPrinter.Size();
        recordInfo.pbRecord = pbStringInfo;
        recordInfo.dwRecordSize = strlen((char*)pbStringInfo)+1;
        recordInfo.pRefMemory = pRefMemory;

        DWORD32 dwTimeLow = 0;
        DWORD32 dwTimeHigh = 0;
        ConvertTickToSystemTime(GetSystemTick(), dwTimeLow, dwTimeHigh);
        if ( FAILED(m_pRecordLinkCtrl->SendRecord(&recordInfo, dwTimeLow, dwTimeHigh)))
        {
            HV_Trace(5, "<link> SendRecord(HvStringInfo) failed!\n");
        }

        pRefMemory->Release();
    }

    return S_OK;
}

HRESULT CDM6467ResultSender::PutStatusString(const char * pString)
{
    if (pString)
    {
        IReferenceMemory* pRefMemory = NULL;
        if ( S_OK != CreateReferenceMemory(&pRefMemory, strlen((char*)pString)+1) )
        {
            HV_Trace(5, "PutString pRefMemory is NULL..\n");
            return E_OUTOFMEMORY;
        }
        PBYTE8 pbStringInfo = NULL;
        pRefMemory->GetData(&pbStringInfo);

        strcpy((char*)pbStringInfo, pString);

        // 发送字符串（事件检测信息）
        SEND_RECORD recordInfo;
        recordInfo.dwRecordType = CAMERA_RECORD_STATUS;
        recordInfo.pbXML = (BYTE8 *)pString;
        recordInfo.dwXMLSize = strlen(pString) + 1;
        recordInfo.pbRecord = pbStringInfo;
        recordInfo.dwRecordSize = strlen((char*)pbStringInfo)+1;
        recordInfo.pRefMemory = pRefMemory;

        DWORD32 dwTimeLow = 0;
        DWORD32 dwTimeHigh = 0;
        ConvertTickToSystemTime(GetSystemTick(), dwTimeLow, dwTimeHigh);
        if ( FAILED(m_pRecordLinkCtrl->SendRecord(&recordInfo, dwTimeLow, dwTimeHigh)))
        {
            HV_Trace(5, "<link> SendRecord(HvStringInfo) failed!\n");
        }

        pRefMemory->Release();
        return S_OK;
    }
    return E_FAIL;
}

int CDM6467ResultSender::CalcShareMemorySize(CARLEFT_INFO_STRUCT * carLeft, LPCSTR szResultInfo)
{
    int size = 0;
    //计算大图大小
    IReferenceComponentImage *prgImage[5] =
    {
        carLeft->cCoreResult.cResultImg.pimgBestSnapShot,
        carLeft->cCoreResult.cResultImg.pimgLastSnapShot,
        carLeft->cCoreResult.cResultImg.pimgBeginCapture,
        carLeft->cCoreResult.cResultImg.pimgBestCapture,
        carLeft->cCoreResult.cResultImg.pimgLastCapture
    };
    HV_COMPONENT_IMAGE imgTemp;
    int nHeaderSize = sizeof(int) + sizeof(PCI_IMAGE_INFO) + sizeof(int);
    for (int i = 0; i < 5; i++)
    {
        if (prgImage[i] && (i < 2 || i >= 2 && (m_pResultSenderParam->iOutputCaptureImage || strstr(szResultInfo, "违章:是") != NULL || strstr(szResultInfo, "违章:否&lt") != NULL)))
        {
            prgImage[i]->GetImage(&imgTemp);
            //一张图的数据格式:头大小+头数据+图像大小+图像格式
            size += nHeaderSize + imgTemp.iWidth;
        }
    }
    //计算小图大小
    if (carLeft->cCoreResult.cResultImg.pimgPlate)
    {
        carLeft->cCoreResult.cResultImg.pimgPlate->GetImage(&imgTemp);
        size += nHeaderSize + imgTemp.iWidth * imgTemp.iHeight * 2;
    }
    //计算二值图大小
    if (carLeft->cCoreResult.cResultImg.pimgPlateBin)
    {
        carLeft->cCoreResult.cResultImg.pimgPlateBin->GetImage(&imgTemp);
        size += nHeaderSize + (imgTemp.iWidth >> 3) * imgTemp.iHeight;
    }
    return size;
}

PBYTE8 CDM6467ResultSender::CopyBigImageToBuffer(PBYTE8 buf, IReferenceComponentImage *pImage, int nImageType, DWORD32 dwCarID, HV_RECT rcPlate, HV_RECT *rcRedLight, int nRedLightCount)
{
    if (!pImage)
    {
        return buf;
    }

    HV_COMPONENT_IMAGE imgTemp;
    DWORD32 dwTimeLow, dwTimeHigh;
    PCI_IMAGE_INFO cImageInfo;
    int nImgInfoSize = sizeof(PCI_IMAGE_INFO);

    pImage->GetImage(&imgTemp);

    ConvertTickToSystemTime(pImage->GetRefTime(), dwTimeLow, dwTimeHigh);

    //图片信息大小
    memcpy(buf, &nImgInfoSize, sizeof(int));
    buf += sizeof(int);

    //图片类型
    cImageInfo.dwCarID = dwCarID;
    cImageInfo.dwImgType = nImageType;
    cImageInfo.dwTimeLow = dwTimeLow;
    cImageInfo.dwTimeHigh = dwTimeHigh;
    cImageInfo.dwImgWidth = ((imgTemp.iHeight & 0x0000FFFF) & (~0x1));
    cImageInfo.dwImgHeight = imgTemp.iHeight >> 16;

    if (m_pResultSenderParam->iEddyType)
    {
        cImageInfo.dwEddyType = 1;
    }

    cImageInfo.rcPlate = rcPlate;
    cImageInfo.nRedLightCount = nRedLightCount;
    memcpy(cImageInfo.rcRedLightPos, rcRedLight, nRedLightCount*sizeof(HiVideo::CRect));

    memcpy(buf, &cImageInfo, sizeof(PCI_IMAGE_INFO));
    buf += sizeof(PCI_IMAGE_INFO);

    //图片数据大小
    memcpy(buf, &imgTemp.iWidth, sizeof(int));
    buf += sizeof(int);

    //复制图片数据
    memcpy(buf, GetHvImageData(&imgTemp, 0), imgTemp.iWidth);
    buf += imgTemp.iWidth;

    return buf;
}

PBYTE8 CDM6467ResultSender::CopySmallImageToBuffer(PBYTE8 buf, IReferenceComponentImage *pImage, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh, DWORD32 dwCarID)
{
    if (!pImage)
    {
        return buf;
    }

    HV_COMPONENT_IMAGE imgTemp;
    PCI_IMAGE_INFO cImageInfo;
    int nImgInfoSize = sizeof(PCI_IMAGE_INFO);

    pImage->GetImage(&imgTemp);

    unsigned char *pY = GetHvImageData(&imgTemp, 0);
    unsigned char *pCb = GetHvImageData(&imgTemp,1);
    unsigned char *pCr = GetHvImageData(&imgTemp, 2);
    if ((pY == NULL) || (pCb == NULL) || (pCr == NULL))
    {
        return buf;
    }

    //图片信息大小
    memcpy(buf, &nImgInfoSize, sizeof(int));
    buf += sizeof(int);

    //图片类型
    cImageInfo.dwCarID = dwCarID;
    cImageInfo.dwImgType = PCILINK_IMAGE_SMALL_IMAGE;
    cImageInfo.dwTimeLow = dwTimeMsLow;
    cImageInfo.dwTimeHigh = dwTimeMsHigh;
    cImageInfo.dwImgWidth = imgTemp.iWidth & (~0x1);
    cImageInfo.dwImgHeight = imgTemp.iHeight;
    memcpy(buf, &cImageInfo, sizeof(PCI_IMAGE_INFO));
    buf += sizeof(PCI_IMAGE_INFO);

    //图片数据大小
    DWORD32 dwImgSize = cImageInfo.dwImgWidth * cImageInfo.dwImgHeight * 2;
    memcpy(buf, &dwImgSize, sizeof(DWORD32));
    buf += sizeof(DWORD32);

    int iHeight;
    size_t iLineSize = (size_t)cImageInfo.dwImgWidth / 2;

    //复制图片数据
    unsigned char *pSrc = pY;
    for (iHeight = 0; iHeight < imgTemp.iHeight; iHeight++, pSrc += imgTemp.iStrideWidth[0])
    {
        memcpy(buf, pSrc, cImageInfo.dwImgWidth);
        buf += cImageInfo.dwImgWidth;
    }
    pSrc = pCb;
    for (iHeight = 0; iHeight < imgTemp.iHeight; iHeight++, pSrc += (imgTemp.iStrideWidth[0] / 2))
    {
        memcpy(buf, pSrc, iLineSize);
        buf += iLineSize;
    }
    pSrc = pCr;
    for (iHeight = 0; iHeight < imgTemp.iHeight; iHeight++, pSrc += (imgTemp.iStrideWidth[0] / 2))
    {
        memcpy(buf, pSrc, iLineSize);
        buf += iLineSize;
    }
    return buf;
}

PBYTE8 CDM6467ResultSender::CopyBinImageToBuffer(PBYTE8 buf, IReferenceComponentImage *pImage, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh, DWORD32 dwCarID)
{
    if (!pImage)
    {
        return buf;
    }

    HV_COMPONENT_IMAGE imgTemp;
    PCI_IMAGE_INFO cImageInfo;
    int nImgInfoSize = sizeof(PCI_IMAGE_INFO);

    pImage->GetImage(&imgTemp);

    //图片信息大小
    memcpy(buf, &nImgInfoSize, sizeof(int));
    buf += sizeof(int);

    //图片类型
    cImageInfo.dwCarID = dwCarID;
    cImageInfo.dwImgType = PCILINK_IMAGE_BIN_IMAGE;
    cImageInfo.dwTimeLow = dwTimeMsLow;
    cImageInfo.dwTimeHigh = dwTimeMsHigh;
    cImageInfo.dwImgWidth = imgTemp.iWidth;
    cImageInfo.dwImgHeight = imgTemp.iHeight;
    memcpy(buf, &cImageInfo, sizeof(PCI_IMAGE_INFO));
    buf += sizeof(PCI_IMAGE_INFO);

    //图片数据大小
    DWORD32 dwImgSize = (imgTemp.iWidth >> 3) * imgTemp.iHeight;
    memcpy(buf, &dwImgSize, sizeof(DWORD32));
    buf += sizeof(DWORD32);

    //复制图片数据
    unsigned char *pSrc = GetHvImageData(&imgTemp, 0);
    int iHeight;
    int iDestLine = imgTemp.iWidth >> 3;
    for (iHeight = 0; iHeight < imgTemp.iHeight; iHeight++, pSrc += iDestLine)
    {
        memcpy(buf, pSrc, iDestLine);
        buf += iDestLine;
    }
    return buf;
}
