#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWPeccancyParkingCarLeft.h"

CSWPeccancyParkingCarLeft::CSWPeccancyParkingCarLeft()
{
	for(DWORD i = 0; i < 30; i++)
		m_rgpVideo[i] = NULL;
	m_iVideoCount = 0;
}

CSWPeccancyParkingCarLeft::~CSWPeccancyParkingCarLeft()
{
	for(DWORD i = 0; i < 30; i++)
	{
		if(m_rgpVideo[i] != NULL)
			SAFE_RELEASE(m_rgpVideo[i]);
	}
}


HRESULT CSWPeccancyParkingCarLeft::GetEventDetInfo(CSWString& strInfo)
{
    if (FAILED(CSWCarLeft::GetEventDetInfo(strInfo)))
    {
        return E_FAIL ;
    }
    return S_OK;
}

HRESULT CSWPeccancyParkingCarLeft::BuildPlateString(TiXmlDocument& xmlDoc)
{
    CSWString strTmp;
    TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    TiXmlElement* pElement = new TiXmlElement("HvcResultDoc");
    xmlDoc.LinkEndChild(pDecl);
    xmlDoc.LinkEndChild(pElement);

    //取得ResultSet段
    pElement = xmlDoc.RootElement()->FirstChildElement("ResultSet");
    if (!pElement)
    {
        pElement = new TiXmlElement("ResultSet");
        if (!pElement)
        {
            return E_OUTOFMEMORY ;
        }
        xmlDoc.RootElement()->LinkEndChild(pElement);
    }

    //注意：一定要删除已经存在的节
    TiXmlNode *pResultOld = pElement->FirstChild("Result");
    if (pResultOld)
    {
        pElement->RemoveChild(pResultOld);
    }
    TiXmlElement* pResult = new TiXmlElement("Result");
    if (pResult)
    {
        pElement->LinkEndChild(pResult);
    }

    //车牌号码
    CSWString strPlate = GetPlateNo();
    TiXmlElement* pValue = new TiXmlElement("PlateName");
    TiXmlText* pText = new TiXmlText((LPCSTR)strPlate);
    if (pValue && pText)
    {
        pValue->LinkEndChild(pText);
        pResult->LinkEndChild(pValue);
    }

    //车牌颜色
    pValue = new TiXmlElement("Color");
    if (pValue)
    {
        pValue->SetAttribute("raw_value", GetPlateColor());
        int nColorType = 0;
        if (swpa_strncmp((LPCSTR)strPlate, "蓝", 2) == 0)
        {
            nColorType = 1;
        }
        else if (swpa_strncmp((LPCSTR)strPlate, "黄", 2) == 0)
        {
            nColorType = 2;
        }
        else if (swpa_strncmp((LPCSTR)strPlate, "黑", 2) == 0)
        {
            nColorType = 3;
        }
        else if (swpa_strncmp((LPCSTR)strPlate, "白", 2) == 0)
        {
            nColorType = 4;
        }
        else if (swpa_strncmp((LPCSTR)strPlate, "绿", 2) == 0)
        {
            nColorType = 5;
        }
        else
        {
            nColorType = 0;
        }
        pValue->SetAttribute("value", nColorType);
        pResult->LinkEndChild(pValue);
    }

    //车牌类型
    pValue = new TiXmlElement("Type");
    if (pValue)
    {
        pValue->SetAttribute("raw_value", GetPlateType());
        int nPlateType = 0;
        switch (GetPlateType())
        {
        case PLATE_NORMAL:
            case PLATE_POLICE:
            nPlateType = 1;
            break;
        case PLATE_WJ:
            nPlateType = 2;
            break;
        case PLATE_POLICE2:
            nPlateType = 3;
            break;
        case PLATE_DOUBLE_YELLOW:
            case PLATE_DOUBLE_MOTO:
            nPlateType = 4;
            break;
        default:
            nPlateType = 0;
            break;
        }
        pValue->SetAttribute("value", nPlateType);
        pResult->LinkEndChild(pValue);
    }

    if (theApp->GetParam().Get().cResultSenderParam.fOutputAppendInfo == FALSE)
    {
        return S_OK ;
    }

    //附加信息
    /*if (theApp->GetParam().Get().cTrackerCfgParam.nDetReverseRunEnable)
    {
        //输出车辆逆行标识
        pValue = new TiXmlElement("ReverseRun");
        if (pValue)
        {
            strTmp.Format("%s", GetReverseRun() ? "是" : "否");
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "车辆逆向行驶");
            pResult->LinkEndChild(pValue);
        }
    }*/

    //无车牌速度
    // if (theApp->GetParam().Get().cTrackerCfgParam.cScaleSpeed.fEnable && !GetCarspeed())
    // {
    //     swpa_utils_srand(CSWDateTime::GetSystemTick());
    //     SetCarspeed(swpa_utils_rand() % 20 + 20.0f);
    //     if (!GetContent()[0]
    //         && (GetCarType() == CT_WALKMAN || GetCarType() == CT_BIKE))
    //     {
    //         SetCarColor(CC_GREY);
    //     }
    // }

    // //输出速度
    // if (0 < GetCarspeed())
    // {
    //     //车速
    //     pValue = new TiXmlElement("VideoScaleSpeed");
    //     if (pValue)
    //     {
    //         strTmp.Format("%d Km/h", (int)GetCarspeed());
    //         pValue->SetAttribute("value", (LPCSTR)strTmp);
    //         pValue->SetAttribute("chnname", "视频测速");
    //         pResult->LinkEndChild(pValue);
    //     }
    //     //限速值
    //     pValue = new TiXmlElement("SpeedLimit");
    //     if (pValue)
    //     {
    //         strTmp.Format("%d Km/h", theApp->GetParam().Get().cTrackerCfgParam.iSpeedLimit);
    //         pValue->SetAttribute("value", (LPCSTR)strTmp);
    //         pValue->SetAttribute("chnname", "限速值");
    //         pResult->LinkEndChild(pValue);
    //     }
    //     //距离计算的误差比例
    //     pValue = new TiXmlElement("ScaleSpeedOfDistance");
    //     if (pValue)
    //     {
    //         strTmp.Format("%0.2f", GetScaleOfDistance());
    //         pValue->SetAttribute("value", (LPCSTR)strTmp);
    //         pValue->SetAttribute("chnname", "距离测量误差比例");
    //         pResult->LinkEndChild(pValue);
    //     }
    // }

    //有效帧数
    if (theApp->GetParam().Get().cResultSenderParam.fOutputObservedFrames)
    {
		/*
        pValue = new TiXmlElement("ObservedFrames");
        if (pValue)
        {
            strTmp.Format("%d", GetObservedFrames());
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "有效帧数");
            pResult->LinkEndChild(pValue);
        }

        //可信度
        pValue = new TiXmlElement("Confidence");
        if (pValue)
        {
            strTmp.Format("%.3f", swpa_exp(swpa_log(GetAverageConfidence()) * 0.143));
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "平均可信度");
            pResult->LinkEndChild(pValue);
        }

        //首字符可信度
        pValue = new TiXmlElement("FirstCharConf");
        if (pValue)
        {
            strTmp.Format("%.3f", GetFirstAverageConfidence());
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "首字可信度");
            pResult->LinkEndChild(pValue);
        }
		*/
    }

	
	pValue = new TiXmlElement("BestSnapShotTime");
	if(pValue)
	{
		strTmp.Format("%d-%d-%d %02d:%02d:%02d:%03d", 
					  m_cDetectCarTime.year,
					  m_cDetectCarTime.month,
					  m_cDetectCarTime.day,
					  m_cDetectCarTime.hour,
					  m_cDetectCarTime.min,
					  m_cDetectCarTime.sec,
					  m_cDetectCarTime.msec);
		pValue->SetAttribute("value", (LPCSTR)strTmp);
		pValue->SetAttribute("chnname", "检测时间");
		pResult->LinkEndChild(pValue);
	}


	pValue = new TiXmlElement("BeginCaptureTime");
	if(pValue)
	{
		strTmp.Format("%d-%d-%d %02d:%02d:%02d:%03d", 
					  m_cPeccancyTime.year,
					  m_cPeccancyTime.month,
					  m_cPeccancyTime.day,
					  m_cPeccancyTime.hour,
					  m_cPeccancyTime.min,
					  m_cPeccancyTime.sec,
					  m_cPeccancyTime.msec);
		pValue->SetAttribute("value", (LPCSTR)strTmp);
		pValue->SetAttribute("chnname", "违章时间");
		pResult->LinkEndChild(pValue);
	}

    //车辆检测时间
    // if (theApp->GetParam().Get().cResultSenderParam.fOutputCarArriveTime)
    // {
    //     pValue = new TiXmlElement("CarArriveTime");
    //     if (pValue)
    //     {
    //         CSWDateTime dt(GetFirstFrameTime());
    //         strTmp.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", dt.GetYear(),
    //             dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(),
    //             dt.GetSecond(), dt.GetMSSecond());
    //         pValue->SetAttribute("value", (LPCSTR)strTmp);
    //         pValue->SetAttribute("chnname", "车辆检测时间");
    //         pResult->LinkEndChild(pValue);
    //     }
    // }

	/*
    //双层牌类型
    switch (GetPlateType())
    {
    case PLATE_DOUBLE_YELLOW:
        strTmp = "双";
        break;
    case PLATE_DOUBLE_MOTO:
        strTmp = "摩";
        break;
    default:
        strTmp = "";
        break;
    }
    if (!strTmp.IsEmpty())
    {
        pValue = new TiXmlElement("DoublePlate");
        if (pValue)
        {
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "车牌类型");
            pResult->LinkEndChild(pValue);
        }
    }
	*/

    //车身颜色
    if (theApp->GetParam().Get().cTrackerCfgParam.fEnableRecgCarColor)
    {
        pValue = new TiXmlElement("CarColor");
        if (pValue)
        {
            switch (GetCarColor())
            {
            case CC_WHITE:
                strTmp = "白色";
                break;
            case CC_GREY:
                strTmp = "灰色";
                break;
            case CC_BLACK:
                strTmp = "黑色";
                break;
            case CC_RED:
                strTmp = "红色";
                break;
            case CC_YELLOW:
                strTmp = "黄色";
                break;
            case CC_GREEN:
                strTmp = "绿色";
                break;
            case CC_BLUE:
                strTmp = "蓝色";
                break;
            case CC_PURPLE:
                strTmp = "紫色";
                break;
            case CC_PINK:
                strTmp = "粉色";
                break;
            case CC_BROWN:
                strTmp = "棕色";
                break;
            default:
                strTmp = "未知";
                break;
            }
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "车身颜色");
            pResult->LinkEndChild(pValue);
        }
    }

	//预置位名称
	pValue = new TiXmlElement("PresetName");
	if(pValue)
	{
		pValue->SetAttribute("value", m_strPresetName);
		pValue->SetAttribute("chnname", "预置位");
		pResult->LinkEndChild(pValue);
	}

    //路口名称
    pValue = new TiXmlElement("StreetName");
    if (pValue)
    {
        pValue->SetAttribute("value", GetRoadName());
        pValue->SetAttribute("chnname", "路口");
        pResult->LinkEndChild(pValue);
    }

    //路口方向
    // pValue = new TiXmlElement("StreetDirection");
    // if (pValue)
    // {
    //     pValue->SetAttribute("value", GetRoadDirection());
    //     pValue->SetAttribute("chnname", "路口方向");
    //     pResult->LinkEndChild(pValue);
    // }
	pValue = new TiXmlElement("EventCheck");
	if(pValue)
	{
		pValue->SetAttribute("value", "违章:是<违法停车>");
		pValue->SetAttribute("chnname", "事件检测");
		pResult->LinkEndChild(pValue);
	}

    //视频帧名
    if (GetImage(0) && GetImage(0)->GetFrameName())
    {
        pValue = new TiXmlElement("FrameName");
        if (pValue)
        {
            pValue->SetAttribute("value", GetImage(0)->GetFrameName());
            pValue->SetAttribute("chnname", "视频帧名");
            pResult->LinkEndChild(pValue);
        }
    }

    //车牌亮度
    pValue = new TiXmlElement("PlateLight");
    if (pValue)
    {
        strTmp.Format("%d", GetCarAvgY());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "车牌亮度");
        pResult->LinkEndChild(pValue);
    }

/*
    //环境光亮度
    pValue = new TiXmlElement("AmbientLight");
    if (pValue)
    {
        //todo: 规避环境亮度为0的输出
        static INT iPrevAvgY = 10;
        INT iAvgY = GetAvgY();
        if (0 == iAvgY)
        {
            iAvgY = iPrevAvgY;
        }
        else
        {
            iPrevAvgY = iAvgY;
        }
  
        strTmp.Format("%d", iAvgY);
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "环境亮度");
        pResult->LinkEndChild(pValue);
    }



    //车牌对比度
    pValue = new TiXmlElement("PlateVariance");
    if (pValue)
    {
        strTmp.Format("%d", GetCarVariance());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "车牌对比度");
        pResult->LinkEndChild(pValue);
    }


    // 后处理信息
    CSWString strFilterInfo = GetFilterInfo();
    if (!strFilterInfo.IsEmpty())
    {
        pValue = new TiXmlElement("ResultProcess");
        if (pValue)
        {
            pValue->SetAttribute("value", strFilterInfo);
            pValue->SetAttribute("chnname", "后处理信息");
            pResult->LinkEndChild(pValue);
        }
    }
*/


	//行驶类型
	// pValue = new TiXmlElement("FilterReverseEnable");
    // if (pValue)
    // {
    //     strTmp.Format("%s", GetReverseRun() ? "逆行" : "正行");
    //     pValue->SetAttribute("value", (LPCSTR)strTmp);
    //     pValue->SetAttribute("chnname", "行驶方向");
    //     pResult->LinkEndChild(pValue);
    // }

    return S_OK ;
}


HRESULT CSWPeccancyParkingCarLeft::SetDetectCarTime(SWPA_DATETIME_TM &cTime)
{
	m_cDetectCarTime = cTime;
	return S_OK;
}

HRESULT CSWPeccancyParkingCarLeft::SetPeccancyTime(SWPA_DATETIME_TM &cTime)
{
	m_cPeccancyTime = cTime;
	return S_OK;
}

HRESULT CSWPeccancyParkingCarLeft::SetPresetName(LPCSTR szName)
{
	m_strPresetName = szName;
}


VOID CSWPeccancyParkingCarLeft::SetVideo(INT index, CSWImage *pVideo)
{
	if(index < 0 || index >= 30)
		return;
	if(m_rgpVideo[index] != NULL)
	{
		SAFE_RELEASE(m_rgpVideo[index]);
		m_iVideoCount--;
		m_iVideoCount = m_iVideoCount < 0 ? 0 : m_iVideoCount;
	}
	m_rgpVideo[index] = pVideo;
	if(pVideo != NULL)
	{
		SAFE_ADDREF(m_rgpVideo[index]);
		m_iVideoCount++;
	}
	else
	{
		SW_TRACE_DEBUG("ERROR:set video fail. index:%d\n", index);
	}
}

CSWImage *CSWPeccancyParkingCarLeft::GetVideo(INT index)
{
	if(index < 0 || index >= 30)
		return NULL;
	return m_rgpVideo[index];
}

INT CSWPeccancyParkingCarLeft::GetVideoCount()
{
	return m_iVideoCount;
}
