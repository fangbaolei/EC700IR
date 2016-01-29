#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWTollGateCarLeft.h"
#include "SWLPRParameter.h"

CSWTollGateCarLeft::CSWTollGateCarLeft()
{
}

CSWTollGateCarLeft::~CSWTollGateCarLeft()
{
}

HRESULT CSWTollGateCarLeft::GetEventDetInfo(CSWString& strInfo)
{
    if (FAILED(CSWCarLeft::GetEventDetInfo(strInfo)))
    {
        return E_FAIL ;
    }
    return S_OK;
}

HRESULT CSWTollGateCarLeft::BuildPlateString(TiXmlDocument& xmlDoc)
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
    if (theApp->GetParam().Get().cTrackerCfgParam.cScaleSpeed.fEnable && !GetCarspeed())
    {
        swpa_utils_srand(CSWDateTime::GetSystemTick());
        SetCarspeed(swpa_utils_rand() % 20 + 20.0f);
        if (!GetContent()[0]
            && (GetCarType() == CT_WALKMAN || GetCarType() == CT_BIKE))
        {
            SetCarColor(CC_GREY);
        }
    }

    //输出速度
    if (0 < GetCarspeed())
    {
        //车速
        pValue = new TiXmlElement("VideoScaleSpeed");
        if (pValue)
        {
            strTmp.Format("%d Km/h", (int)GetCarspeed());
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "视频测速");
            pResult->LinkEndChild(pValue);
        }
        //限速值
        pValue = new TiXmlElement("SpeedLimit");
        if (pValue)
        {
            strTmp.Format("%d Km/h", theApp->GetParam().Get().cTrackerCfgParam.iSpeedLimit);
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "限速值");
            pResult->LinkEndChild(pValue);
        }
        //距离计算的误差比例
        pValue = new TiXmlElement("ScaleSpeedOfDistance");
        if (pValue)
        {
            strTmp.Format("%0.2f", GetScaleOfDistance());
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "距离测量误差比例");
            pResult->LinkEndChild(pValue);
        }
    }

    //有效帧数
    if (theApp->GetParam().Get().cResultSenderParam.fOutputObservedFrames)
    {
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
    }

    //车辆检测时间
    if (theApp->GetParam().Get().cResultSenderParam.fOutputCarArriveTime)
    {
        pValue = new TiXmlElement("CarArriveTime");
        if (pValue)
        {
            CSWDateTime dt(GetFirstFrameTime());
            strTmp.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", dt.GetYear(),
                dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(),
                dt.GetSecond(), dt.GetMSSecond());
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "车辆检测时间");
            pResult->LinkEndChild(pValue);
        }
    }

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

    //路口名称
    pValue = new TiXmlElement("StreetName");
    if (pValue)
    {
        pValue->SetAttribute("value", GetRoadName());
        pValue->SetAttribute("chnname", "路口名称");
        pResult->LinkEndChild(pValue);
    }

    //路口方向
    pValue = new TiXmlElement("StreetDirection");
    if (pValue)
    {
        pValue->SetAttribute("value", GetRoadDirection());
        pValue->SetAttribute("chnname", "路口方向");
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

    //当前亮度级别
    pValue = new TiXmlElement("PlateLightType");
    if (pValue)
    {
        strTmp.Format("%02d级", GetPlateLightType());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "摄像机亮度等级");
        pResult->LinkEndChild(pValue);
    }

    //当前偏光镜状态
    pValue = new TiXmlElement("CplStatus");
    if (pValue)
    {
        strTmp.Format("%d", GetCplStatus());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "偏光镜状态");
        pResult->LinkEndChild(pValue);
    }

    //当前补光灯脉宽级别
    // todo.
    // 金星下不使用频闪灯，不输出补光脉宽等级。
    if( m_pTrackerCfg != NULL && m_pTrackerCfg->iUsedLight == 1 )
    {
		//当前补光灯脉宽级别
		pValue = new TiXmlElement("PulseLevel");
		if (pValue)
		{
			strTmp.Format("%d", GetPulseLevel());
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "补光脉宽等级");
			pResult->LinkEndChild(pValue);
		}
    }

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

    //车牌亮度
    pValue = new TiXmlElement("PlateLight");
    if (pValue)
    {
        strTmp.Format("%d", GetCarAvgY());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "车牌亮度");
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

	//行驶类型
	pValue = new TiXmlElement("FilterReverseEnable");
    if (pValue)
    {
        strTmp.Format("%s", GetReverseRun() ? "逆行" : "正行");
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "行驶方向");
        pResult->LinkEndChild(pValue);
    }

    return S_OK ;
}

