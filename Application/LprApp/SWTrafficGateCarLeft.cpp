#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWTrafficGateCarLeft.h"

CSWTrafficGateCarLeft::CSWTrafficGateCarLeft()
{
}

CSWTrafficGateCarLeft::~CSWTrafficGateCarLeft()
{
}

HRESULT CSWTrafficGateCarLeft::GetEventDetInfo(CSWString& strInfo)
{
	if(FAILED(CSWCarLeft::GetEventDetInfo(strInfo)))
	{
		return E_FAIL;
	}
	

	CSWString strEventCheck = "";
  if (theApp->GetParam().Get().cTrackerCfgParam.nDetReverseRunEnable && GetReverseRun())
  {
      if (!strEventCheck.IsEmpty())
      {
          strEventCheck += ",";
      }
      strEventCheck += "Reverse run";
      INT iPtType = GetPTType();
      iPtType |= PT_CONVERSE;
      SetPTType(iPtType);
  }

//  //压线,车道线从左到右编号从0开始
//  if (GetDetectOverYellowLineEnable() && GetIsOverYellowLine() != COLT_INVALID)
//  {
//      if (GetIsOverYellowLine() == COLT_NO)
//      {
//          strInfo += "压线:无\n";
//      }
//      else
//      {
//          CSWString strYellowLine = "";
//          strYellowLine.Format("压线:车道线%d<%s>", GetIsOverYellowLine() + 1, GetIsDoubleYellowLine() ? "黄线" : "实线");
          
//          strEventCheck += strYellowLine;
//          strEventCheck += ",";
          
//          strInfo += strYellowLine;
//          strInfo += "\n";

//          INT iPtType = GetPTType();
//          iPtType |= PT_OVERLINE;
//          SetPTType(iPtType);
//      }
//  }

//  //越线,车道线从左到右编号从0开始
//  if (GetDetectCrossLineEnable() && GetIsCrossLine() != COLT_INVALID)
//  {
//      if (GetIsCrossLine() == COLT_NO)
//      {
//          strInfo += "越线:无\n";
//      }
//      else
//      {
//          CSWString strCrossLine = "";
//          strCrossLine.Format("越线:车道线%d", GetIsCrossLine() + 1);
                    
//          strEventCheck += strCrossLine;
//          strEventCheck += ",";
          
//          strInfo += strCrossLine;
//          strInfo += "\n";

//          INT iPtType = GetPTType();
//          iPtType |= PT_OVERLINE;
//          SetPTType(iPtType);
//      }
//  }

  //超速
  if ((int)(GetCarspeed()) > theApp->GetParam().Get().cResultSenderParam.iSpeedLimit)
  {
      if (!strEventCheck.IsEmpty())
      {
          strEventCheck += ",";
      }
      strEventCheck += "Overspeed";

      INT iPtType = GetPTType();
      iPtType |= PT_OVERSPEED;
      SetPTType(iPtType);
  }

  if (!strEventCheck.IsEmpty())
  {
  		//strInfo += "Violate regulations:Yes<";
  		strInfo += strEventCheck;
  		//strInfo += ">";
  }
	return S_OK;
}

HRESULT CSWTrafficGateCarLeft::BuildPlateString(TiXmlDocument& xmlDoc)
{
	// if(FAILED(CSWCarLeft::BuildPlateString(xmlDoc)))
	// {
	// 	return E_FAIL;
	// }
	// return S_OK;
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
		if ( !pElement ) 
		{
			return E_OUTOFMEMORY;
		}
		xmlDoc.RootElement()->LinkEndChild(pElement);
	}

	//注意：一定要删除已经存在的节
	TiXmlNode *pResultOld = pElement->FirstChild("Result");
	if ( pResultOld )
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
	TiXmlText* pText=new TiXmlText("Unlicensed");
	if (pValue && pText)
	{
		pValue->LinkEndChild(pText);
		pResult->LinkEndChild(pValue);
	}
  
	//车牌颜色
	pValue = new TiXmlElement("Color");
	if ( pValue )
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
	/*pValue = new TiXmlElement("Type");
	if ( pValue )
	{
		pValue->SetAttribute("raw_value", GetPlateType());
		int nPlateType = 0;
		switch ( GetPlateType() )
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
	}*/
  
	//附加信息
	if(m_pTrackerCfg)
	{
		if(m_pTrackerCfg->nDetReverseRunEnable)
		{
			//输出车辆逆行标识
			pValue = new TiXmlElement("ReverseRun");
			if (pValue)
			{
				strTmp.Format("%s", GetReverseRun() ? "Yes" : "No");
				pValue->SetAttribute("value", (LPCSTR)strTmp);
				pValue->SetAttribute("chnname", "Reverse Run");
				pResult->LinkEndChild(pValue);
			}
		} 
    
		//无车牌速度
		if(m_pTrackerCfg->cScaleSpeed.fEnable && !GetCarspeed())
		{
			swpa_utils_srand(CSWDateTime::GetSystemTick());
			SetCarspeed(swpa_utils_rand() % 20 + 20.0f);
			if (!GetContent()[0] && (GetCarType() == CT_WALKMAN || GetCarType() == CT_BIKE))
			{
				SetCarColor(CC_GREY);
			}
		}

		//输出速度
		if(0 < (INT)GetCarspeed() && (INT)GetCarspeed() < 500)
		{			
			if((swpa_strcmp(m_strSpeedType,"Vedio Speed")!=0)||(m_pTrackerCfg->cScaleSpeed.fEnable&&(swpa_strcmp(m_strSpeedType,"Vedio Speed")==0)))
			{
				pValue = new TiXmlElement("VideoScaleSpeed");
				if (pValue)
				{
					strTmp.Format("%d Km/h", (int)GetCarspeed());
					pValue->SetAttribute("value", (LPCSTR)strTmp);
					pValue->SetAttribute("chnname", (LPCSTR)m_strSpeedType);
					pResult->LinkEndChild(pValue);
				}

				//限速值
				pValue = new TiXmlElement("SpeedLimit");
				if (pValue)
				{
			  		strTmp.Format("%d Km/h", m_pTrackerCfg->iSpeedLimit);
			  		pValue->SetAttribute("value", (LPCSTR)strTmp);
			  		pValue->SetAttribute("chnname", "Speed Limit");
			  		pResult->LinkEndChild(pValue);
				}
			}
		}
    
		//输出速度
		/*if(0 < (INT)GetCarspeed() && (INT)GetCarspeed() < 500)
		{ //车速
			pValue = new TiXmlElement("VideoScaleSpeed");
			if (pValue)
			{
				strTmp.Format("%d Km/h", (int)GetCarspeed());
				pValue->SetAttribute("value", (LPCSTR)strTmp);
				pValue->SetAttribute("chnname", (LPCSTR)m_strSpeedType);
				pResult->LinkEndChild(pValue);
			}
		}
		else
		{
			pValue = new TiXmlElement("VideoScaleSpeed");
			if (pValue)
			{
				swpa_utils_srand(CSWDateTime::GetSystemTick());
				INT iRandSpeed = (INT)(swpa_utils_rand() % 20 + 20.0f);
				strTmp.Format("%d Km/h", iRandSpeed);
				SW_TRACE_DEBUG("Plate %s set random speed value:%d", (LPCSTR)strPlate, iRandSpeed);
				pValue->SetAttribute("value", (LPCSTR)strTmp);
				pValue->SetAttribute("chnname", (LPCSTR)m_strSpeedType);
				pResult->LinkEndChild(pValue);
			}

			//限速值
			pValue = new TiXmlElement("SpeedLimit");
			if (pValue)
			{
		  		strTmp.Format("%d Km/h", m_pTrackerCfg->iSpeedLimit);
		  		pValue->SetAttribute("value", (LPCSTR)strTmp);
		  		pValue->SetAttribute("chnname", "Speed Limit");
		  		pResult->LinkEndChild(pValue);
			}  
		}*/
		//有效帧数
		pValue = new TiXmlElement("ObservedFrames");
		if (pValue)
		{
			strTmp.Format("%d", GetObservedFrames());
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "Effective Frames");
			pResult->LinkEndChild(pValue);
        }

        //track id
        pValue = new TiXmlElement("TrackID");
        if (pValue)
        {
            strTmp.Format("%d", GetTrackId());
            pValue->SetAttribute("value", (LPCSTR)strTmp);
            pValue->SetAttribute("chnname", "Track ID");
            pResult->LinkEndChild(pValue);
        }

		//可信度
		/*pValue = new TiXmlElement("Confidence");
		if (pValue)
		{
			strTmp.Format("%.3f", swpa_exp(swpa_log(GetAverageConfidence()) * 0.143));
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "平均可信度");
			pResult->LinkEndChild(pValue);
		}*/

		//首字符可信度
		/*pValue = new TiXmlElement("FirstCharConf");
		if (pValue)
		{
			strTmp.Format("%.3f", GetFirstAverageConfidence());
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "首字可信度");
			pResult->LinkEndChild(pValue);
		}*/
    
		//车辆检测时间
		pValue = new TiXmlElement("CarArriveTime");
		if (pValue)
		{
    		CSWDateTime dt(GetFirstFrameTime());
			strTmp.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond(), dt.GetMSSecond());
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "Vehicle Detection Time");
			pResult->LinkEndChild(pValue);
		}
    
		//双层牌类型
		/*switch(GetPlateType())
		{
		case PLATE_DOUBLE_YELLOW: strTmp = "双"; break;
		case PLATE_DOUBLE_MOTO  : strTmp = "摩"; break;
		default : strTmp = ""; break;
		}
		if(!strTmp.IsEmpty())
		{
			pValue = new TiXmlElement("DoublePlate");
			if (pValue)
			{
				pValue->SetAttribute("value", (LPCSTR)strTmp);
				pValue->SetAttribute("chnname", "车牌类型");
				pResult->LinkEndChild(pValue);
			}
		}*/                    
    
		//车辆尺寸
		/*if(m_pTrackerCfg->fOutputCarSize && GetCarType() != CT_WALKMAN && GetCarType() != CT_BIKE && GetPlateType() != PLATE_DOUBLE_MOTO)
		{
			INT iOutType;
			FLOAT iWidth, iHeight;
			pValue = new TiXmlElement("CarSize");
			if(pValue && S_OK == GetCarSize(iOutType, iWidth, iHeight))
			{
				if(!iOutType)
				{
					strTmp.Format("%d 车长(像素):%d", (INT)iWidth, (INT)iHeight);    			
				}
				else
				{    			
					strTmp.Format("%.2f 车长(米):%.2f", iWidth, iHeight);
				}
				pValue->SetAttribute("value", (LPCSTR)strTmp);
				strTmp.Format("车宽(%s)", iOutType == 0 ? "像素" : "米");
				pValue->SetAttribute("chnname", (LPCSTR)strTmp);
				pResult->LinkEndChild(pValue);
			}
		}*/
    
		//车辆类型
		/*pValue = new TiXmlElement("CarType");
		if (pValue)
		{
			if (CT_LARGE == GetCarType() && PLATE_DOUBLE_MOTO != GetPlateType())
			{
				strTmp = "大";
			}
			else if (CT_SMALL == GetCarType() && PLATE_DOUBLE_MOTO != GetPlateType())
			{
				strTmp = "小";
			}
			else if (CT_MID == GetCarType() && PLATE_DOUBLE_MOTO != GetPlateType())
			{
				strTmp = "中";
			}
			else if (PLATE_DOUBLE_MOTO == GetPlateType())
			{
				strTmp = "摩托车";
			}
			else if (CT_WALKMAN == GetCarType())
			{
				strTmp = "行人";
			}
			else if (CT_BIKE == GetCarType())
			{
				strTmp = "非机动车";
			}
			else if (CT_VEHICLE == GetCarType())
			{
				strTmp = "机动车";
			}
			else
			{
				strTmp = "未知";
			}
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "车辆类型");
			pResult->LinkEndChild(pValue);
		}*/
    
		//车身颜色
		/*if (m_pTrackerCfg->fEnableRecgCarColor)
		{
			pValue = new TiXmlElement("CarColor");
			if (pValue)
			{
				switch (GetCarColor())
				{
				case CC_WHITE:  strTmp = "白色"; break;
				case CC_GREY :  strTmp = "灰色"; break;
				case CC_BLACK:  strTmp = "黑色"; break;
				case CC_RED:    strTmp = "红色"; break;
				case CC_YELLOW: strTmp = "黄色"; break;
				case CC_GREEN:  strTmp = "绿色"; break;
				case CC_BLUE:   strTmp = "蓝色"; break;
				case CC_PURPLE: strTmp = "紫色"; break;
				case CC_PINK:   strTmp = "粉色"; break;
				case CC_BROWN:  strTmp = "棕色"; break;
				default:        strTmp = "未知"; break;
				}
				pValue->SetAttribute("value", (LPCSTR)strTmp);
				pValue->SetAttribute("chnname", "车身颜色");
				pResult->LinkEndChild(pValue);
			}
		}*/

		//车道号
		pValue = new TiXmlElement("RoadNumber");
		if (pValue)
		{
			//strTmp.Format("%d", GetRoadNo() + (255 == GetRoadNo() ? 0 : m_pTrackerCfg->iStartRoadNum));
			strTmp.Format("%d", GetOutPutRoadNo());
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "Lane");
			pResult->LinkEndChild(pValue);
		}
    
		//起始车道号
		pValue = new TiXmlElement("BeginRoadNumber");
		if (pValue)
		{
			if (m_pTrackerCfg->iRoadNumberBegin == 0)
			{
				strTmp.Format("<Left,%d>", m_pTrackerCfg->iStartRoadNum);
			}
			else
			{
				strTmp.Format("<Right,%d>", m_pTrackerCfg->iStartRoadNum);
			}
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "Start Lane");
			pResult->LinkEndChild(pValue);
		}
    
		//路口名称
		pValue = new TiXmlElement("StreetName");
		if (pValue)
		{
			pValue->SetAttribute("value", GetRoadName());
			pValue->SetAttribute("chnname", "Road Name");
			pResult->LinkEndChild(pValue);
		}
    
		//路口方向
		pValue = new TiXmlElement("StreetDirection");
		if (pValue)
		{
			pValue->SetAttribute("value", GetRoadDirection());
			pValue->SetAttribute("chnname", "Road Direction");
			pResult->LinkEndChild(pValue);
		}   
	}
  
  	//事件检测信息
  	strTmp = "";
  	if (S_OK == GetEventDetInfo(strTmp) && !strTmp.IsEmpty())
  	{
  		pValue = new TiXmlElement("EventCheck");
  		if (pValue)
  		{
  	   		pValue->SetAttribute("value", (LPCSTR)strTmp);
  	    	pValue->SetAttribute("chnname", "Event Check");
  	    	pResult->LinkEndChild(pValue);
  		}
	}
	//视频帧名
	CSWString strFrameName;
	for(int i = 0; i < 5; i++)
	{
		if(GetImage(i) && GetImage(i)->GetFrameName())
		{
			strFrameName = GetImage(i)->GetFrameName();
			break;
		}
	}
	if(!strFrameName.IsEmpty())
	{
		pValue = new TiXmlElement("FrameName");
		if ( pValue )
		{
			pValue->SetAttribute("value", (LPCSTR)strFrameName);
			pValue->SetAttribute("chnname", "Name of Video Frame");
			pResult->LinkEndChild(pValue);        
		}
	}
	
	//当前亮度级别
	pValue = new TiXmlElement("PlateLightType");
	if (pValue)
	{
		strTmp.Format("Level %02d", GetPlateLightType());
		pValue->SetAttribute("value", (LPCSTR)strTmp);
		pValue->SetAttribute("chnname", "Camera Brightness");
		pResult->LinkEndChild(pValue);
	}

	//当前偏光镜状态
	/*pValue = new TiXmlElement("CplStatus");
	  if (pValue)
	  {
	  strTmp.Format("%d", GetCplStatus());
	  pValue->SetAttribute("value", (LPCSTR)strTmp);
	  pValue->SetAttribute("chnname", "偏光镜状态");
	  pResult->LinkEndChild(pValue);
	  }

    pValue = new TiXmlElement("WdrLevel");
    if (pValue)
    {
        strTmp.Format("%d", GetWdrLevel());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "WDR等级");
        pResult->LinkEndChild(pValue);
    }*/

	//当前补光灯脉宽级别
	// todo.
	// 金星下不使用频闪灯，不输出补光脉宽等级。
	/*if( m_pTrackerCfg != NULL && m_pTrackerCfg->iUsedLight == 1 )
	{
		pValue = new TiXmlElement("PulseLevel");
		if (pValue)
		{
			strTmp.Format("%d", GetPulseLevel());
			pValue->SetAttribute("value", (LPCSTR)strTmp);
			pValue->SetAttribute("chnname", "Strobe Pulse Width");
			pResult->LinkEndChild(pValue);
		}
	}*/
  
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
		pValue->SetAttribute("chnname", "Ambient Brightness");
		pResult->LinkEndChild(pValue);
	}

	//车牌亮度
	/*pValue = new TiXmlElement("PlateLight");
	if (pValue)
	{
		strTmp.Format("%d", GetCarAvgY());
		pValue->SetAttribute("value", (LPCSTR)strTmp);
		pValue->SetAttribute("chnname", "车牌亮度");
		pResult->LinkEndChild(pValue);
	}*/

	//车牌对比度
	/*pValue = new TiXmlElement("PlateVariance");
	if (pValue)
	{
		strTmp.Format("%d", GetCarVariance());
		pValue->SetAttribute("value", (LPCSTR)strTmp);
		pValue->SetAttribute("chnname", "车牌对比度");
		pResult->LinkEndChild(pValue);
	} */

	// 后处理信息
	CSWString strFilterInfo = GetFilterInfo();
	if( !strFilterInfo.IsEmpty() )
	{
		pValue = new TiXmlElement("ResultProcess");
		if (pValue)
		{
			pValue->SetAttribute("value", strFilterInfo);
			pValue->SetAttribute("chnname", "Background processing information");
			pResult->LinkEndChild(pValue);
		} 
	}

	return S_OK;
}

