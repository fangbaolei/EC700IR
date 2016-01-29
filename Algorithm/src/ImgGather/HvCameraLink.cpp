#include "ImgGatherer.h"
#include "tinyxml.h"
#include "HvCameraLinkOpt.h"
#include "platerecogparam.h"

using namespace HiVideo;

//XML的一个属性
struct SXmlAttr
{
	std::string            strName;
	std::string            strValue;

	SXmlAttr()
	{
		strName = "";
		strValue = "";
	}
};

//XML命令
struct SXmlCmd
{
	std::string            strCmdName;
	std::vector<SXmlAttr>  listAttr;

	SXmlCmd()
	{
		strCmdName = "";
		listAttr.clear();
	}
};

//XML命令集合
typedef std::vector<SXmlCmd>    XmlCmdList;

// 生成Xml协议信息请求的Xml数据
static int HvMakeXmlCmdReq2(const XmlCmdList& listXmlCmd, char* szXmlBuf)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "2.0");

	doc.LinkEndChild(pXmlRootElement);

	std::vector<SXmlCmd>::const_iterator iter = listXmlCmd.begin();
	for(; iter!=listXmlCmd.end(); ++iter)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		//命令名称
		TiXmlText *pXmlCmdText = new TiXmlText(iter->strCmdName.c_str());
		pXmlElementCmd->LinkEndChild(pXmlCmdText);

		//参数(属性)
		std::vector<SXmlAttr>::const_iterator iterAttr = iter->listAttr.begin();
		for(; iterAttr!=iter->listAttr.end(); ++iterAttr)
		{
			if (iterAttr->strName.size() > 0 && iterAttr->strValue.size() > 0)
			{
				pXmlElementCmd->SetAttribute(iterAttr->strName.c_str(), iterAttr->strValue.c_str());
			}
		}
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), iLen);
	szXmlBuf[iLen] = '\0';

	return iLen;
}

HRESULT HvMakeXmlCmdByString2(const char* inXmlOrStrBuf, int nInlen,
					  char* szOutXmlBuf, int& nOutlen)
{
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(inXmlOrStrBuf))
	{
		memcpy(szOutXmlBuf, inXmlOrStrBuf, nInlen);
		nOutlen = nInlen;
		return S_OK;
	}
	bool bGet = false;

	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	char* pszCmd = pszInBufCopy;
	XmlCmdList listXmlCmd;
	while(pszCmd)
	{
		int iBufLen = (int)strlen(pszCmd);
		char* pTmpCmd = new char[iBufLen + 1];//pszCmd;
		memcpy(pTmpCmd, pszCmd, iBufLen);
		pTmpCmd[iBufLen] = '\0';
		char* pTmpCmdEnd = strstr(pTmpCmd, ";");
		if(pTmpCmdEnd)
		{
			*pTmpCmdEnd = '\0';
		}
		int nCount = 0;
		char* pszCmdName = pTmpCmd;
		char* pValueID[64];
		char* pValueText[64];
		char* pTemp;
		while(pTmpCmd = strchr(pTmpCmd, ','))
		{
			*pTmpCmd = '\0';
			pValueID[nCount] = pTmpCmd + 1;
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2表示必需要有一个字符
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2表示必需要有一个字符
			{
				goto getone;
			}
			pValueText[nCount] = pTemp + 1;
			nCount++;

			*pTemp = '\0';
			*pTmpCmd = '\0';
			pTmpCmd++;
		}

getone:
		bGet = TRUE;
		SXmlCmd  sXmlCmd;
		sXmlCmd.strCmdName = pszCmdName;
		for (int i=0; i<nCount; ++i)
		{
			if (pValueID[i] && pValueText[i])
			{
				SXmlAttr  sXmlAttr;
				sXmlAttr.strName = pValueID[i];
				sXmlAttr.strValue = pValueText[i];
				sXmlCmd.listAttr.push_back(sXmlAttr);
			}
		}
		listXmlCmd.push_back(sXmlCmd);

		pszCmd = strstr(pszCmd, ";");
		if(pszCmd)
		{
			pszCmd += 1;
		}
	}

	nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
	SAFE_DELETE(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}

/**
*  实现一体机协议
*/
CHvCameraLink::CHvCameraLink()
        : m_sktData(INVALID_SOCKET)
        , m_sktCmd(INVALID_SOCKET)
        , m_pstmData(NULL)
        , m_pstmCmd(NULL)
{
    m_pcTmpData = (char*)HV_AllocMem(MAX_RECV_BUF_SIZE);
}

CHvCameraLink::~CHvCameraLink()
{
    GetImageStop();
    CtrtCamStop();
    if (m_pcTmpData)
    {
        HV_FreeMem(m_pcTmpData, MAX_RECV_BUF_SIZE);
    }
}

HRESULT CHvCameraLink::GetImageStart(const char* pszIP)
{
    GetImageStop();

    m_sktData = HvCreateSocket();

    if (m_sktData == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    if (S_OK != HvConnect(m_sktData, pszIP, CAMERA_IMAGE_LINK_PORT, 4000))
    {
        GetImageStop();
        return E_FAIL;
    }

    //设置超时
    if (S_OK != HvSetRecvTimeOut(m_sktData, 5000)
            || S_OK != HvSetSendTimeOut(m_sktData, 5000))
    {
        GetImageStop();
        return E_FAIL;
    }

    m_pstmData = new CSocketStream(m_sktData);

    if (m_pstmData == NULL)
    {
        GetImageStop();
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHvCameraLink::GetImageStop()
{
    if (m_sktData != INVALID_SOCKET)
    {
        CloseSocket(m_sktData);
        m_sktData = INVALID_SOCKET;
    }
    if (m_pstmData)
    {
        delete m_pstmData;
        m_pstmData = NULL;
    }

    return S_OK;
}

HRESULT CHvCameraLink::GetOneFrame(char* pcBuf, DWORD32* pdwSize, bool fMustData, DWORD32* pdwImgType)
{
    if (!m_pstmData || !pdwSize || !pcBuf) return E_POINTER;

    HRESULT hr = E_FAIL;

    INFO_HEADER_HVCAM cInfoHeader;
	INFO_HEADER_HVCAM cInfoHeaderResponse;
	BLOCK_HEADER_HVCAM cBlockHeader;

	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwInfoLen = 0;
	cInfoHeaderResponse.dwDataLen = 0;

	char* pbInfo = NULL;

	// 接收包头
    hr = m_pstmData->Read(&cInfoHeader, sizeof(cInfoHeader), NULL);
    if (FAILED(hr)) return hr;

    // 如果是通信心跳帧，返回心跳应答
    while(cInfoHeader.dwType == CAMERA_THROB)
    {
        hr = m_pstmData->Write(&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), NULL);
        if (FAILED(hr)) return hr;
        // 如果不需要得到图片数据，则返回
        if (!fMustData) return S_OK;
        // 继续接收下一包头
        hr = m_pstmData->Read(&cInfoHeader, sizeof(cInfoHeader), NULL);
        if (FAILED(hr)) return hr;
    }

    // 是普通帧或者抓拍帧，接收图像帧数据
    if (*pdwSize < cInfoHeader.dwDataLen)
    {
        return E_OUTOFMEMORY;
    }

    if(cInfoHeader.dwDataLen <= 0)
    {
        return E_FAIL;
    }

    pbInfo = new char[cInfoHeader.dwInfoLen + 1];
    if( pbInfo == NULL )
    {
        HV_Trace(5,"[ImageLink] Reconnect,Malloc Buffer Failed...");
        SAFE_DELETE(pbInfo);
        HV_Sleep(100);
        return E_FAIL;
    }
    else
    {
        HV_memset(pbInfo, 0, cInfoHeader.dwInfoLen + 1);
    }
    if(cInfoHeader.dwInfoLen > 0)
    {
        hr = m_pstmData->Read(pbInfo, cInfoHeader.dwInfoLen, NULL);
        if (FAILED(hr))
        {
            HV_Trace(5,"[ImageLink] Reconnect,Recv Packet Header Failed...");
            SAFE_DELETE(pbInfo);
            return hr;
        }
    }

    DWORD32 dwImageType = 0;
    DWORD32 dwImageWidth = 0;
    DWORD32 dwImageHeight = 0;
    DWORD64 dw64ImageTime = 0;
    DWORD32 dwImageOffset = 0;

    if(cInfoHeader.dwDataLen > 0)
    {
        // 获取图片数据
        hr = m_pstmData->Read(pcBuf, cInfoHeader.dwDataLen, NULL);
        if (FAILED(hr))
        {
            HV_Trace(5,"[ImageLink] Reconnect,Recv Packet Data Failed...");
            SAFE_DELETE(pbInfo);
            return hr;
        }

        if(cInfoHeader.dwType == CAMERA_IMAGE)
        {
            ImageExtInfo cImageExtInfo = {0};
            char* pbTemp = pbInfo;
            for( int i = 0; i < (int)cInfoHeader.dwInfoLen;)
            {
                HV_memcpy(&cBlockHeader,  pbTemp, sizeof(cBlockHeader));
                pbTemp += sizeof(cBlockHeader);
                i += sizeof(cBlockHeader);

                if( cBlockHeader.dwID == BLOCK_IMAGE_TYPE )
                {
                    HV_memcpy(&dwImageType, pbTemp, sizeof(dwImageType));
                }
                else if( cBlockHeader.dwID == BLOCK_IMAGE_WIDTH )
                {
                    HV_memcpy(&dwImageWidth, pbTemp, sizeof(dwImageWidth));
                }
                else if( cBlockHeader.dwID == BLOCK_IMAGE_HEIGHT )
                {
                    HV_memcpy(&dwImageHeight, pbTemp, sizeof(dwImageHeight));
                }
                else if( cBlockHeader.dwID == BLOCK_IMAGE_TIME )
                {
                    HV_memcpy(&dw64ImageTime, pbTemp, sizeof(dw64ImageTime));
                }
                else if( cBlockHeader.dwID == BLOCK_IMAGE_OFFSET )
                {
                    HV_memcpy(&dwImageOffset, pbTemp, sizeof(dwImageOffset));
                }
                else if( cBlockHeader.dwID == BLOCK_IMAGE_EXT_INFO )
                {
                    HV_memcpy(&cImageExtInfo, pbTemp, sizeof(cImageExtInfo));
                }

                pbTemp += cBlockHeader.dwLen;
                i += cBlockHeader.dwLen;
            }
        }
        else
        {
            SAFE_DELETE(pbInfo);
            return E_FAIL;
        }
    }

    *pdwSize = cInfoHeader.dwDataLen;
    *pdwImgType = dwImageType;

    m_dwHeight = dwImageHeight;
    m_dwWidth = dwImageWidth;

    SAFE_DELETE(pbInfo);
    return hr;
}

HRESULT CHvCameraLink::SendImageCmd(char* pszCmd)
{
    if(!m_pstmData || pszCmd == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    CAMERA_CMD_HEADER_HVCAM cCmdHeader;
	CAMERA_CMD_RESPOND_HVCAM cCmdRespond;

    int iBufLen = (1024 << 4);
	char* pszXmlBuf = new char[iBufLen];
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	int iXmlBufLen = iBufLen;
	TiXmlDocument cXmlDoc;

	if(!cXmlDoc.Parse(pszCmd))
	{
        // 生成XML
        if(HvMakeXmlCmdByString2(pszCmd, (int)strlen(pszCmd), pszXmlBuf, iXmlBufLen)
            != S_OK)
        {
            SAFE_DELETE(pszXmlBuf);
            return E_FAIL;
        }

        pszCmd = (char*)pszXmlBuf;
	}

	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
    cCmdHeader.dwInfoSize = (int)strlen(pszCmd)+1;

	const int iMaxLen = (1024 << 10);
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
	{
	    SAFE_DELETE(pszXmlBuf);
		return E_FAIL;
	}
	HV_memset(pszTmpXmlBuf, 0, iMaxLen);

	hr = m_pstmData->Write(&cCmdHeader,sizeof(cCmdHeader),NULL);
	if(FAILED(hr))
	{
	    SAFE_DELETE(pszTmpXmlBuf);
        SAFE_DELETE(pszXmlBuf);
	    return hr;
	}

	hr = m_pstmData->Write((char*)pszCmd,cCmdHeader.dwInfoSize,NULL);
	if(FAILED(hr))
	{
	    SAFE_DELETE(pszTmpXmlBuf);
        SAFE_DELETE(pszXmlBuf);
	    return hr;
	}

	hr = m_pstmData->Read(&cCmdRespond, sizeof(cCmdRespond),NULL);
	if(FAILED(hr))
	{
	    SAFE_DELETE(pszTmpXmlBuf);
        SAFE_DELETE(pszXmlBuf);
	    return hr;
	}

	if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult
			&& 0 < cCmdRespond.dwInfoSize )
    {
        hr = m_pstmData->Read(pszTmpXmlBuf, cCmdRespond.dwInfoSize,NULL);
        if(FAILED(hr))
        {
            SAFE_DELETE(pszTmpXmlBuf);
            SAFE_DELETE(pszXmlBuf);
            return hr;
        }

        if ( iBufLen > (int)cCmdRespond.dwInfoSize )
        {
            iBufLen = cCmdRespond.dwInfoSize;

            SAFE_DELETE(pszTmpXmlBuf);
            SAFE_DELETE(pszXmlBuf);
            return S_OK;
        }
        else
        {
            iBufLen = cCmdRespond.dwInfoSize;
            SAFE_DELETE(pszTmpXmlBuf);
            SAFE_DELETE(pszXmlBuf);
            return E_FAIL;
        }
    }

    SAFE_DELETE(pszTmpXmlBuf);
	SAFE_DELETE(pszXmlBuf);

    return S_OK;
}

HRESULT CHvCameraLink::GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime, DWORD32* pdwImgType)
{
    if (!pcBuf || !pdwSize || !pdwRefTime) return E_POINTER;
    HRESULT hr = GetOneFrame(pcBuf, pdwSize, true, pdwImgType);
    *pdwRefTime = GetSystemTick();
    return hr;
}

HRESULT CHvCameraLink::ProcessData()
{
    DWORD32 dwDataLen = MAX_RECV_BUF_SIZE;
    DWORD32 dwImgeType = 0;
    return GetOneFrame(m_pcTmpData, &dwDataLen, false,&dwImgeType);
}

//获取当前帧对应车道,抓拍回来的帧才带有车道号,
int CHvCameraLink::GetCurRoadNum()
{
    return -1;
}

HRESULT CHvCameraLink::GetImageWidth(DWORD32& dwWidth)
{
    dwWidth = m_dwWidth;
    return 0;
}

HRESULT CHvCameraLink::GetImageHeight(DWORD32& dwHeight)
{
    dwHeight = m_dwHeight;
    return 0;
}

HRESULT CHvCameraLink::CtrtCamStart(const char* pszIP)
{
    CtrtCamStop();

    m_sktCmd = HvCreateSocket();

    if (m_sktCmd == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    if (S_OK != HvConnect(m_sktCmd, pszIP, CAMERA_CMD_LINK_PORT, 4000))
    {
        CtrtCamStop();
        return E_FAIL;
    }

    //设置超时
    if (S_OK != SetCmdSktTimeout(5000))
    {
        CtrtCamStop();
        return E_FAIL;
    }

    m_pstmCmd = new CSocketStream(m_sktCmd);

    if (m_pstmCmd == NULL)
    {
        CtrtCamStop();
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHvCameraLink::CtrtCamStop()
{
    if (m_sktCmd != INVALID_SOCKET)
    {
        CloseSocket(m_sktCmd);
        m_sktCmd = INVALID_SOCKET;
    }
    if (m_pstmCmd)
    {
        delete m_pstmCmd;
        m_pstmCmd = NULL;
    }

    return S_OK;
}

HRESULT CHvCameraLink::SendCommand( LPCSTR szCmd )
{
    if(!m_pstmCmd || szCmd == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    CAMERA_CMD_HEADER_HVCAM cCmdHeader;
	CAMERA_CMD_RESPOND_HVCAM cCmdRespond;

    int iBufLen = (1024 << 4);
	char* pszXmlBuf = new char[iBufLen];
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	int iXmlBufLen = iBufLen;
	TiXmlDocument cXmlDoc;

	if(!cXmlDoc.Parse(szCmd))
	{
        // 生成XML
        if(HvMakeXmlCmdByString2(szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen)
            != S_OK)
        {
            SAFE_DELETE(pszXmlBuf);
            return E_FAIL;
        }

        szCmd = (char*)pszXmlBuf;
	}

	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
    cCmdHeader.dwInfoSize = (int)strlen(szCmd)+1;

	const int iMaxLen = (1024 << 10);
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
	{
	    SAFE_DELETE(pszXmlBuf);
		return E_FAIL;
	}
	HV_memset(pszTmpXmlBuf, 0, iMaxLen);

	hr = m_pstmCmd->Write(&cCmdHeader,sizeof(cCmdHeader),NULL);
	if(FAILED(hr))
	{
	    SAFE_DELETE(pszTmpXmlBuf);
        SAFE_DELETE(pszXmlBuf);
	    return hr;
	}

	hr = m_pstmCmd->Write((char*)szCmd,cCmdHeader.dwInfoSize,NULL);
	if(FAILED(hr))
	{
	    SAFE_DELETE(pszTmpXmlBuf);
        SAFE_DELETE(pszXmlBuf);
	    return hr;
	}

	hr = m_pstmCmd->Read(&cCmdRespond, sizeof(cCmdRespond),NULL);
	if(FAILED(hr))
	{
	    SAFE_DELETE(pszTmpXmlBuf);
        SAFE_DELETE(pszXmlBuf);
	    return hr;
	}

	if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult
			&& 0 < cCmdRespond.dwInfoSize )
    {
        hr = m_pstmCmd->Read(pszTmpXmlBuf, cCmdRespond.dwInfoSize,NULL);
        if(FAILED(hr))
        {
            SAFE_DELETE(pszTmpXmlBuf);
            SAFE_DELETE(pszXmlBuf);
            return hr;
        }

        if ( iBufLen > (int)cCmdRespond.dwInfoSize )
        {
            iBufLen = cCmdRespond.dwInfoSize;

            SAFE_DELETE(pszTmpXmlBuf);
            SAFE_DELETE(pszXmlBuf);
            return S_OK;
        }
        else
        {
            iBufLen = cCmdRespond.dwInfoSize;
            SAFE_DELETE(pszTmpXmlBuf);
            SAFE_DELETE(pszXmlBuf);
            return E_FAIL;
        }
    }

    SAFE_DELETE(pszTmpXmlBuf);
	SAFE_DELETE(pszXmlBuf);

    return S_OK;
}

HRESULT CHvCameraLink::SetDataSktTimeout(DWORD32 dwTimeout)
{
    if (S_OK != HvSetRecvTimeOut(m_sktData, dwTimeout)
            || S_OK != HvSetSendTimeOut(m_sktData, dwTimeout))
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CHvCameraLink::SetCmdSktTimeout(DWORD32 dwTimeout)
{
    if (S_OK != HvSetRecvTimeOut(m_sktCmd, dwTimeout)
            || S_OK != HvSetSendTimeOut(m_sktCmd, dwTimeout))
    {
        return E_FAIL;
    }
    return S_OK;
}

/**
*  实现一体机触发协议
*/
CHVTrigger::CHVTrigger()
        : m_iCurRoadNum(0)
        , m_nCurLightType(DAY)
        , m_pCfgCamParam(NULL)
{
    CreateSemaphore(&m_hSemNeedTrigger, 0, 10);
    m_dwLastThreadIsOkTime = GetSystemTick();
}

CHVTrigger::~CHVTrigger(void)
{
    Stop(-1);
    DestroySemaphore(&m_hSemNeedTrigger);
}

HRESULT CHVTrigger::ProcessOnTrigger()
{
    while (!m_fExit)
    {
        m_dwLastThreadIsOkTime = GetSystemTick();
        if (!m_pCfgCamParam->iDynamicTriggerEnable)
        {
            HV_Sleep(4000);
            continue;
        }

        int iCurExposureTime = -1;
        int iCurGain = -1;

        if (SemPend(&m_hSemNeedTrigger, 1000) == 0)
        {
            iCurExposureTime = m_pCfgCamParam->irgExposureTime[m_nCurLightType];
            iCurGain = m_pCfgCamParam->irgGain[m_nCurLightType];

            // 触发抓拍相机
            HRESULT hr = m_cHvCameraLink.CtrtCamStart(m_pCfgCamParam->szIP);
            if (hr != S_OK)
            {
                HV_Trace(5, "CamTrigger: CtrtCamStart Error..\n");
                continue;
            }

            char szCmd[255];
            HV_memset(szCmd,0,255);
            sprintf(szCmd, "SoftTriggerCapture");
            hr = m_cHvCameraLink.SendCommand(szCmd);
            if (hr != S_OK)
            {
                HV_Trace(5, "CamTrigger: SendCommand Error..\n");
                m_cHvCameraLink.CtrtCamStop();
                continue;
            }

            m_cHvCameraLink.CtrtCamStop();
        }
    }
    m_fExit = TRUE;

    return S_OK;
}

HRESULT CHVTrigger::Run(void* pvParam)
{
    HV_Sleep(1000);
    // 有车时触发摄像机
    return ProcessOnTrigger();
}

HRESULT CHVTrigger::Trigger(LIGHT_TYPE nLightType, int iRoadNum)
{
    if (!m_pCfgCamParam->iDynamicTriggerEnable) return S_FALSE;

    m_nCurLightType = nLightType;
    m_iCurRoadNum = iRoadNum;
    SemPost(&m_hSemNeedTrigger, 0);
    return S_OK;
}

HRESULT CHVTrigger::HardTrigger(int iRoadNum)
{
    m_iCurRoadNum = iRoadNum;
    SemPost(&m_hSemNeedTrigger, 0);
    return S_OK;
}

bool CHVTrigger::ThreadIsOk()
{
    bool fRtn = false;
    if (GetSystemTick() - m_dwLastThreadIsOkTime < 100000)
    {
        fRtn = true;
    }
    return fRtn;
}

HRESULT CHVTrigger::SetCamTriggerParam(CAM_CFG_PARAM *pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_FAIL;
    }

    m_pCfgCamParam = pCfgCamParam;

    if (GetCurrentMode() == PRM_HVC)
    {
        if (strcmp(m_pCfgCamParam->szIP, "0.0.0.0") == 0)
        {
            m_pCfgCamParam->iDynamicTriggerEnable = false;
        }
        else
        {
            m_pCfgCamParam->iDynamicTriggerEnable = true;
        }
    }
    return S_OK;
}




