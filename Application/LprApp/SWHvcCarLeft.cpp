#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWHvcCarLeft.h"

CSWHvcCarLeft::CSWHvcCarLeft()
	: m_fTollEvasion(FALSE)
	, m_dwTollEvasionTick(0)
{
	m_dwTollEvasionTick = CSWDateTime::GetSystemTick();
}

CSWHvcCarLeft::~CSWHvcCarLeft()
{
}

HRESULT CSWHvcCarLeft::GetEventDetInfo(CSWString& strInfo)
{
    if (FAILED(CSWCarLeft::GetEventDetInfo(strInfo)))
    {
        return E_FAIL ;
    }
    return S_OK ;
}

CSWString CSWHvcCarLeft::BuildPlateString(VOID)
{
    TiXmlDocument xmlDoc;
    if (S_OK == BuildPlateString(xmlDoc))
    {
        TiXmlPrinter cTxPr;
        xmlDoc.Accept(&cTxPr);
        return cTxPr.CStr();
    }
    return "";
}

HRESULT CSWHvcCarLeft::BuildPlateString(TiXmlDocument& xmlDoc)
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
        case PLATE_DOUBLE_WJ:
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

    if (theApp->GetParam().Get().cResultSenderParam.iUseRushRule)
    {
    	pValue = new TiXmlElement("TollEvasion");
        if (pValue)
        {
            pValue->SetAttribute("value", m_fTollEvasion ? "是" : "否");
            pValue->SetAttribute("chnname", "冲卡逃费");
            pResult->LinkEndChild(pValue);
        }

        if (m_fTollEvasion)
        {
            CSWDateTime cBeginTime(m_dwTollEvasionTick - 10* 1000);
            CSWDateTime cEndTime(m_dwTollEvasionTick + 10* 1000);
            CHAR szBeginTime[256] = {0};
            CHAR szEndTime[256] = {0};

            swpa_snprintf(szBeginTime, sizeof(szBeginTime), "%4d-%02d-%02d-%02d-%02d-%02d",
                cBeginTime.GetYear(), cBeginTime.GetMonth(), cBeginTime.GetDay(), cBeginTime.GetHour(), cBeginTime.GetMinute(), cBeginTime.GetSecond());
        
            swpa_snprintf(szEndTime, sizeof(szEndTime), "%4d-%02d-%02d-%02d-%02d-%02d",
                cEndTime.GetYear(), cEndTime.GetMonth(), cEndTime.GetDay(), cEndTime.GetHour(), cEndTime.GetMinute(), cEndTime.GetSecond());
        
            pValue = new TiXmlElement("TollEvasionBeginTime");
            if (pValue)
            {
                pValue->SetAttribute("value", szBeginTime);
                pValue->SetAttribute("chnname", "开始时间");
                pResult->LinkEndChild(pValue);
        }

            pValue = new TiXmlElement("TollEvasionEndTime");
            if (pValue)
            {
                pValue->SetAttribute("value", szEndTime);
                pValue->SetAttribute("chnname", "结束时间");
                pResult->LinkEndChild(pValue);
            }
        }
    }

    //有效帧数
    if (theApp->GetParam().Get().cResultSenderParam.fOutputObservedFrames)
    {
        pValue = new TiXmlElement("ObservedFrames");
        if (pValue)
        {
            pValue->SetAttribute("value", "1");
            pValue->SetAttribute("chnname", "有效帧数");
            pResult->LinkEndChild(pValue);
        }
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
                strTmp = "灰色";
                break;
            }
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "车身颜色");
            pResult->LinkEndChild(pValue);
        }
    }

    //处理时间
    DWORD32 dwProcTime = 180 - DWORD32(1 + (54.0 * rand() / (RAND_MAX + 1.0)) - 27);
    pValue = new TiXmlElement("ProcTime");
    if (pValue)
    {
        strTmp.Format("%d", dwProcTime);
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "处理时间");
        pResult->LinkEndChild(pValue);
    }

    //车牌HSL值
    if(1 == GetOutputHSLFlag()) //黑牌参数使能
    {
        pValue = new TiXmlElement("HSL");
        if (pValue)
        {
        	CHAR szConf[256];
            sprintf(szConf, "H:%d\tS:%d\tL:%d", GetiH(), GetiS(), GetiL());
            pValue->SetAttribute("value", szConf);
            pValue->SetAttribute("chnname", "车牌HSL值");
            pResult->LinkEndChild(pValue);
        }
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

    return S_OK ;
}

