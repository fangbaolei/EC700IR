#include "swpa.h"
#include "SWFC.h"
#include "SWCarLeft.h"
#include "swplate.h"
#include "SWMessage.h"

CHAR CSWCarLeft::s_szRoadName[128] = {0};
CHAR CSWCarLeft::s_szRoadDirection[128] = {0};
INT CSWCarLeft::s_iOutputBestImage = 1;
INT CSWCarLeft::s_iOutputLastImage = 1;
INT CSWCarLeft::s_iOutputCaptureImage = 0;
INT CSWCarLeft::s_iOutputCropImage = 0;
INT CSWCarLeft::s_iCropWidth = 1024;
INT CSWCarLeft::s_iCropHeight = 1024;
INT CSWCarLeft::s_iOutputHSL = 0;

CSWCarLeft::CSWCarLeft()
{
	m_pTrackerCfg = NULL;
	m_pCarLeft = new CARLEFT_INFO_STRUCT;
	swpa_memset(m_pCarLeft, 0, sizeof(CARLEFT_INFO_STRUCT));
	SetCarArriveTime(CSWDateTime::GetSystemTick());
	m_strFilterInfo = "";
	m_strSpeedType = "视频测速";
	m_fReadOnly = false;
    m_fCropLastImage = FALSE;
    m_fCaptureRecogResult = FALSE;
}

HRESULT CSWCarLeft::SetParameter(TRACKER_CFG_PARAM *pCfg, CARLEFT_INFO_STRUCT *pCarLeft)
{
	if(pCfg)
	{
		m_pTrackerCfg = pCfg;
	}
	if(pCarLeft)
	{
		swpa_memcpy(m_pCarLeft, pCarLeft, sizeof(CARLEFT_INFO_STRUCT));
		if(!s_iOutputBestImage)
		{
			SetImage(0, NULL);
		}
		if(!s_iOutputLastImage)
		{
			SetImage(1, NULL);
		}
        if (!s_iOutputCropImage)
        {
            SetImage(0, NULL);
        }
		for(int i = 0; i < m_pCarLeft->cCoreResult.nFaceCount; i++)
		{
			m_pCarLeft->cCoreResult.rgFaceRect[i].top *= 2;
			m_pCarLeft->cCoreResult.rgFaceRect[i].bottom *= 2;
		}
		m_strPlateNo = "";
		m_strFilterInfo = "";
	}
	return S_OK;
}

CSWCarLeft::~CSWCarLeft()
{
   CSWImage *pImage[] = 
	{
		  (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgBestSnapShot
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgLastSnapShot
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgBeginCapture
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgBestCapture
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgLastCapture
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgPlate
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgPlateBin
	};
	for(int i = 0; i < GetImageCount(); i++)
	{
		SAFE_RELEASE(pImage[i]);
	}
	delete m_pCarLeft;
}



void CSWCarLeft::SetRoadInfo(const CHAR* szName, const CHAR* szDirection)
{
	if( szName != NULL && swpa_strlen(szName) < 128 )
	{
		swpa_strcpy(s_szRoadName, szName);
	}
	else
	{
		s_szRoadName[0] = 0;
	}

	if( szDirection != NULL && swpa_strlen(szDirection) < 128 )
	{
		swpa_strcpy(s_szRoadDirection, szDirection);
	}
	else
	{
		s_szRoadDirection[0] = 0;
	}
}

CHAR* CSWCarLeft::GetRoadName()
{
	return s_szRoadName;
}
CHAR* CSWCarLeft::GetRoadDirection()
{
	return s_szRoadDirection;
}

void CSWCarLeft::SetOutputMode(const INT& iBestImage, const INT& iLastImage, const INT& iCaptureImage, const INT& iLastCropImage, const INT &iCropWidthLevel, const INT &iCropHeightLevel)
{
	s_iOutputBestImage = iBestImage;
	s_iOutputLastImage = iLastImage;
	s_iOutputCaptureImage = iCaptureImage;
    s_iOutputCropImage = iLastCropImage;

    switch(iCropWidthLevel)
    {
        case 1:
            s_iCropWidth = 640;
            break;
        case 2:
            s_iCropWidth = 768;
            break;
        case 3:
            s_iCropWidth = 896;
            break;
        case 4:
            s_iCropWidth = 1024;
            break;
        case 5:
            s_iCropWidth = 1152;
            break;
        case 6:
            s_iCropWidth = 1280;
            break;
        default:
            break;
    }

    switch(iCropHeightLevel)
    {
        case 1:
            s_iCropHeight = 640;
            break;
        case 2:
            s_iCropHeight = 768;
            break;
        case 3:
            s_iCropHeight = 896;
            break;
        case 4:
            s_iCropHeight = 1024;
            break;
        case 5:
            s_iCropHeight = 1152;
            break;
        case 6:
            s_iCropHeight = 1280;
            break;
        default:
            break;
    }
}

INT CSWCarLeft::GetOutputBestImage()
{
	return s_iOutputBestImage;
}

INT CSWCarLeft::GetOutputLastImage()
{
	return s_iOutputLastImage;
}

INT CSWCarLeft::GetOutputCaptureImage()
{
    return s_iOutputCaptureImage;
}

INT CSWCarLeft::GetOutputCropImage()
{
    return s_iOutputCropImage;
}

void CSWCarLeft::GetCropWidthHeight(INT &iCropWidth, INT &iCropHeight)
{
    iCropWidth = s_iCropWidth;
    iCropHeight = s_iCropHeight;
}

void CSWCarLeft::SetOutputHSLFlag(const INT& iEnable)
{
	s_iOutputHSL = iEnable;
}

INT CSWCarLeft::GetOutputHSLFlag()
{
	return s_iOutputHSL;
}

VOID CSWCarLeft::SetFilterInfo(CSWString& strFilterInfo)
{
	m_strFilterInfo = strFilterInfo;
}

CSWString CSWCarLeft::GetFilterInfo()
{
	return m_strFilterInfo;
}

DWORD CSWCarLeft::GetOutPutRoadNo(VOID)
{
	if (m_pTrackerCfg != NULL)
	{
		if (0 == m_pTrackerCfg->iRoadNumberBegin)//从左开始
		{
			return m_pTrackerCfg->iStartRoadNum + GetRoadNo();
		}
		else		//从右开始
		{
			return m_pTrackerCfg->iStartRoadNum + (m_pTrackerCfg->nRoadLineNumber-1-1 - GetRoadNo());
		}
	}
	else
	{
		return GetRoadNo();
	}
}

HRESULT CSWCarLeft::GetEventDetInfo(CSWString& strInfo)
{
	// todo.
	// 在些做速度的修改与超速的违章判断
	// 与DONE接口有重复的操作。

	// 超速违章的判断，目前只能是软件测速的超速。外接设备的方式不支持。
	// 超速违章在此判断有此不太合适。todo.
	BOOL fIsOverSpeed = FALSE;
	if ( NULL != m_pTrackerCfg && GetCarspeed() >= 0.1f && ((int)GetCarspeed() > m_pTrackerCfg->iSpeedLimit))
	{
		fIsOverSpeed = true;
	}
	if (fIsOverSpeed)
	{
		INT iPtType = GetPTType();
		iPtType |= PT_OVERSPEED;
		SetPTType(iPtType);
	}

	// 识别结果预处理。
	// 车速修正,不会出现0的车速。
	// 不加入违章的判断中。
	if (NULL != m_pTrackerCfg && m_pTrackerCfg->cScaleSpeed.fEnable && GetCarspeed() == 0 )
	{
		swpa_utils_srand(CSWDateTime::GetSystemTick());
		FLOAT fltCarspeed = swpa_utils_rand() % 20 + 10.0f;
		SetCarspeed(fltCarspeed);
	}


	return S_OK;  
}

CSWString CSWCarLeft::BuildNormalString()
{
	CSWString strInfo = "";

	if( NULL != m_pTrackerCfg )
	{
		CSWString strTmp;
		//平均可信度
		//strTmp.Format("平均可信度:%.3f\n", swpa_exp(swpa_log(GetAverageConfidence()) * 0.143));
		//strInfo.Append(strTmp);
		//首字符可信度
		//strTmp.Format("首字可信度:%.3f\n", GetFirstAverageConfidence());
		//strInfo.Append(strTmp);

		//输出车辆逆行标识
		if(m_pTrackerCfg->nDetReverseRunEnable)
		{
			strTmp.Format("车辆逆向行驶:%s\n", GetReverseRun()? "是" : "否");
			strInfo.Append(strTmp);
		}

		//车道编号
		//strTmp.Format("车道:%d\n", GetRoadNo() + (255 == GetRoadNo() ? 0 : m_pTrackerCfg->iStartRoadNum));
		strTmp.Format("车道:%d\n",GetOutPutRoadNo());
		strInfo.Append(strTmp);

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
		if(0 < GetCarspeed())
		{ 
			//车速
			strTmp.Format("视频测速:%d Km/h\n", (int)GetCarspeed());
			strInfo.Append(strTmp);
		}

		//双层牌类型
		/*switch(GetPlateType())
		{
		case PLATE_DOUBLE_YELLOW: strTmp = "车牌类型:双"; break;
		case PLATE_DOUBLE_MOTO  : strTmp = "车牌类型:摩"; break;
		default : strTmp = ""; break;
		}
		if(!strTmp.IsEmpty())
		{
			strInfo.Append(strTmp);
		}*/

		//违章类型
		//strTmp.Format("违章类型:%d\n", GetPTType());
		//strInfo.Append(strTmp);
	}
	
	return strInfo;
}

HRESULT CSWCarLeft::BuildPlateString(TiXmlDocument& xmlDoc)
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
  TiXmlText* pText = new TiXmlText((LPCSTR)strPlate);
  if (pValue && pText)
  {
      pValue->LinkEndChild(pText);
      pResult->LinkEndChild(pValue);
  }
  
  //车牌颜色
  /*pValue = new TiXmlElement("Color");
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
  pValue = new TiXmlElement("Type");
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
      		strTmp.Format("%s", GetReverseRun() ? "是" : "否");
          pValue->SetAttribute("value", (LPCSTR)strTmp);
          pValue->SetAttribute("chnname", "车辆逆向行驶");
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

    // 结果说明输出
    {
        pValue = new TiXmlElement("ResultType");
        if (pValue)
        {
            CSWImage* pImage = GetImage(1);
            if (pImage)
            {
                strTmp.Format("<结果来源：%s  大图来源：%s>",
                              m_fCaptureRecogResult ? "抓拍识别" : "视频识别" ,
                              pImage->IsCaptureImage() ? "抓拍大图"  : "视频流图");

                pValue->SetAttribute("value", (LPCSTR)strTmp);
                pValue->SetAttribute("chnname", "结果说明");
                pResult->LinkEndChild(pValue);
            }
        }
    }
    
    //输出速度
    if(0 < GetCarspeed())
    { //车速
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
          pValue->SetAttribute("chnname", "限速值");
          pResult->LinkEndChild(pValue);
      }
//      //距离计算的误差比例
//      pValue = new TiXmlElement("ScaleSpeedOfDistance");
//      if (pValue)
//      {
//          strTmp.Format("%0.2f", GetScaleOfDistance());
//          pValue->SetAttribute("value", (LPCSTR)strTmp);
//          pValue->SetAttribute("chnname", "距离测量误差比例");
//          pResult->LinkEndChild(pValue);
//      }
    }
    
    //有效帧数
    pValue = new TiXmlElement("ObservedFrames");
    if (pValue)
    {
        strTmp.Format("%d", GetObservedFrames());
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "有效帧数");
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
    }

    //首字符可信度
    pValue = new TiXmlElement("FirstCharConf");
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
        pValue->SetAttribute("chnname", "车辆检测时间");
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
    if(m_pTrackerCfg->fOutputCarSize && GetCarType() != CT_WALKMAN && GetCarType() != CT_BIKE && GetPlateType() != PLATE_DOUBLE_MOTO)
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
    }
    
    //车辆类型
    pValue = new TiXmlElement("CarType");
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
    }
    
//    //车身颜色
//    if (m_pTrackerCfg->fEnableRecgCarColor)
//    {
//        pValue = new TiXmlElement("CarColor");
//        if (pValue)
//        {
//            switch (GetCarColor())
//    			  {
//    			  case CC_WHITE:  strTmp = "白色"; break;
//    			  case CC_GREY :  strTmp = "灰色"; break;
//    			  case CC_BLACK:  strTmp = "黑色"; break;
//    			  case CC_RED:    strTmp = "红色"; break;
//    			  case CC_YELLOW: strTmp = "黄色"; break;
//    			  case CC_GREEN:  strTmp = "绿色"; break;
//    			  case CC_BLUE:   strTmp = "蓝色"; break;
//    			  case CC_PURPLE: strTmp = "紫色"; break;
//    			  case CC_PINK:   strTmp = "粉色"; break;
//    			  case CC_BROWN:  strTmp = "棕色"; break;
//    			  default:        strTmp = "未知"; break;
//    			  }
//            pValue->SetAttribute("value", (LPCSTR)strTmp);
//            pValue->SetAttribute("chnname", "车身颜色");
//            pResult->LinkEndChild(pValue);
//        }
//    }
    
    //车道号
    pValue = new TiXmlElement("RoadNumber");
    if (pValue)
    {
        //strTmp.Format("%d", GetRoadNo() + (255 == GetRoadNo() ? 0 : m_pTrackerCfg->iStartRoadNum));
		strTmp.Format("%d", GetOutPutRoadNo());
		pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "车道");
        pResult->LinkEndChild(pValue);
    }
    
    //起始车道号
    pValue = new TiXmlElement("BeginRoadNumber");
    if (pValue)
    {
        if (m_pTrackerCfg->iRoadNumberBegin == 0)
        {
            strTmp.Format("<左,%d>", m_pTrackerCfg->iStartRoadNum);
        }
        else
        {
            strTmp.Format("<右,%d>", m_pTrackerCfg->iStartRoadNum);
        }
        pValue->SetAttribute("value", (LPCSTR)strTmp);
        pValue->SetAttribute("chnname", "起始车道号");
        pResult->LinkEndChild(pValue);
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
  }
  
  //事件检测信息
  strTmp = "";
  if (S_OK == GetEventDetInfo(strTmp) && !strTmp.IsEmpty())
  {
  	pValue = new TiXmlElement("EventCheck");
  	if (pValue)
  	{
  	    pValue->SetAttribute("value", (LPCSTR)strTmp);
  	    pValue->SetAttribute("chnname", "事件检测");
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

//  //当前偏光镜状态
//  pValue = new TiXmlElement("CplStatus");
//  if (pValue)
//  {
//	  strTmp.Format("%d", GetCplStatus());
//	  pValue->SetAttribute("value", (LPCSTR)strTmp);
//	  pValue->SetAttribute("chnname", "偏光镜状态");
//	  pResult->LinkEndChild(pValue);
//  }
//    pValue = new TiXmlElement("WdrLevel");
//    if (pValue)
//    {
//        strTmp.Format("%d", GetWdrLevel());
//        pValue->SetAttribute("value", (LPCSTR)strTmp);
//        pValue->SetAttribute("chnname", "WDR等级");
//        pResult->LinkEndChild(pValue);
//    }
//  //当前补光灯脉宽级别
//  // todo.
//  // 金星下不使用频闪灯，不输出补光脉宽等级。
//  if( m_pTrackerCfg != NULL && m_pTrackerCfg->iUsedLight == 1 )
//  {
//	  pValue = new TiXmlElement("PulseLevel");
//	  if (pValue)
//	  {
//		  strTmp.Format("%d", GetPulseLevel());
//		  pValue->SetAttribute("value", (LPCSTR)strTmp);
//		  pValue->SetAttribute("chnname", "补光脉宽等级");
//		  pResult->LinkEndChild(pValue);
//	  }
//  }

  // 最后大图车牌坐标
  /*pValue = new TiXmlElement("LastPlatePos");
  if (pValue)
  {
      HV_RECT rectLast = GetLastPlateRect();
      strTmp.Format("left = %d, top = %d, right = %d, bottom = %d",
                    rectLast.left, rectLast.top, rectLast.right, rectLast.bottom);
      pValue->SetAttribute("value", (LPCSTR)strTmp);
      pValue->SetAttribute("chnname", "最后大图车牌坐标");
      pResult->LinkEndChild(pValue);
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
      pValue->SetAttribute("chnname", "环境亮度");
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
  }

  //车牌对比度
  pValue = new TiXmlElement("PlateVariance");
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
		  pValue->SetAttribute("chnname", "后处理信息");
		  pResult->LinkEndChild(pValue);
	  } 
  }

  return S_OK;
}

TRACKER_CFG_PARAM* CSWCarLeft::GetTrackerParam()
{
	return m_pTrackerCfg;
}

CSWString CSWCarLeft::BuildPlateString(VOID)
{
	TiXmlDocument xmlDoc;
	if(S_OK == BuildPlateString(xmlDoc))
	{
		TiXmlPrinter cTxPr;
		xmlDoc.Accept(&cTxPr);
		return cTxPr.CStr();
	}
	return "";
}

INT CSWCarLeft::GetImageCount(void)
{
	return 7;
}

CSWImage* CSWCarLeft::GetImage(INT index, SW_RECT* rc)
{
	CSWImage *pImage[] = 
	{
		  (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgBestSnapShot
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgLastSnapShot
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgBeginCapture
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgBestCapture
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgLastCapture
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgPlate
		, (CSWImage*)m_pCarLeft->cCoreResult.cResultImg.pimgPlateBin
	};
	SW_RECT *imgRC[] =
	{
		  (SW_RECT *)&m_pCarLeft->cCoreResult.rcBestPlatePos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcLastPlatePos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcFirstPos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcSecondPos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcThirdPos
		, NULL
		, NULL
	};
	for(int i = 0; i < GetImageCount(); i++)
	{
		if(i == index)
		{
			if(NULL != rc)
			{
				if(NULL != imgRC[index])
				{
					//*rc = *imgRC[index];
					swpa_memcpy(rc, imgRC[index], sizeof(SW_RECT));
				}
				else
				{
					swpa_memset(rc, 0, sizeof(SW_RECT));
				}
			}
			return pImage[i];
		}
	}
    return NULL;
}

void CSWCarLeft::GetResultRect(INT index, SW_RECT *rc)
{
    SW_RECT *imgRC[] =
    {
          (SW_RECT *)&m_pCarLeft->cCoreResult.rcBestPlatePos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcLastPlatePos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcFirstPos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcSecondPos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcThirdPos
        , NULL
        , NULL
    };

    for(int i = 0; i < GetImageCount(); i++)
    {
        if(i == index)
        {
            if(NULL != rc)
            {
                if(NULL != imgRC[index])
                {
                    //*rc = *imgRC[index];
                    swpa_memcpy(rc, imgRC[index], sizeof(SW_RECT));
                }
                else
                {
                    swpa_memset(rc, 0, sizeof(SW_RECT));
                }
            }
            return ;
        }
    }
}

VOID CSWCarLeft::SetImage(INT index, CSWImage *pImg, SW_RECT* rc)
{
	//CHECK_LOCK() //marked by zhouy@2014/02/12: no need to lock at image setting.
	CSWImage **pImage[] = 
	{
		  (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgBestSnapShot
		, (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgLastSnapShot
		, (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgBeginCapture
		, (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgBestCapture
		, (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgLastCapture
		, (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgPlate
		, (CSWImage**)&m_pCarLeft->cCoreResult.cResultImg.pimgPlateBin
	};
	SW_RECT *imgRC[] =
	{
		  (SW_RECT *)&m_pCarLeft->cCoreResult.rcBestPlatePos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcLastPlatePos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcFirstPos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcSecondPos
		, (SW_RECT *)&m_pCarLeft->cCoreResult.rcThirdPos
		, NULL
		, NULL
	};
    for(int i = 0; i < sizeof(pImage)/sizeof(CSWImage *); i++)
	{
		if(i == index)
		{
			// 避免同一图像的异常处理。
			if( *pImage[i] != pImg )
			{
				SAFE_RELEASE(*pImage[i]);
				*pImage[i] = pImg;
				SAFE_ADDREF(pImg);
			}
			if(NULL != rc && NULL != imgRC[index])
			{
				swpa_memcpy(imgRC[index], rc, sizeof(SW_RECT));
			}
		}
    }
}

void CSWCarLeft::SetResultRect(INT index, SW_RECT *rc)
{
    SW_RECT *imgRC[] =
    {
          (SW_RECT *)&m_pCarLeft->cCoreResult.rcBestPlatePos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcLastPlatePos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcFirstPos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcSecondPos
        , (SW_RECT *)&m_pCarLeft->cCoreResult.rcThirdPos
        , NULL
        , NULL
    };

    for(int i = 0; i < GetImageCount(); i++)
    {
        if(i == index)
        {
            if(NULL != rc && NULL != imgRC[index])
            {
                swpa_memcpy(imgRC[index], rc, sizeof(SW_RECT));
            }
            return ;
        }
    }
}

CSWString CSWCarLeft::GetPlateNo(VOID)
{	
	if(m_strPlateNo.IsEmpty())
	{
		CHAR szPlateName[255] = "";
		GetPlateNameAlpha(szPlateName, (PLATE_TYPE)GetPlateType(), (PLATE_COLOR)GetPlateColor(), GetContent());
		m_strPlateNo = szPlateName;
	}
	return m_strPlateNo;	
}

HRESULT CSWCarLeft::SetPlateNo(LPCSTR szPlateNo)
{
	if(!m_fReadOnly)
	{
		m_strPlateNo = szPlateNo;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWCarLeft::GetCarSize(INT &iOutType, FLOAT &fltWidth, FLOAT &fltHeight)
{
	iOutType = m_pCarLeft->cCoreResult.cCarSize.nOutType;
	fltWidth = m_pCarLeft->cCoreResult.cCarSize.iCarWidth;
	fltHeight= m_pCarLeft->cCoreResult.cCarSize.iCarHeight;
	return S_OK;
}

SW_RECT CSWCarLeft::GetFaceRect(INT index)
{
	SW_RECT rc = {0, 0, 0, 0};
	return index < m_pCarLeft->cCoreResult.nFaceCount ? *(SW_RECT *)&m_pCarLeft->cCoreResult.rgFaceRect[index] : rc;
}

VOID CSWCarLeft::SetFaceRect(CSWCarLeft *pCarLeft)
{
	for(int i = 0; i < pCarLeft->m_pCarLeft->cCoreResult.nFaceCount; i++)
	{
		SW_TRACE_DEBUG("face(%d,%d,%d,%d)", pCarLeft->m_pCarLeft->cCoreResult.rgFaceRect[i].left, pCarLeft->m_pCarLeft->cCoreResult.rgFaceRect[i].top, pCarLeft->m_pCarLeft->cCoreResult.rgFaceRect[i].right, pCarLeft->m_pCarLeft->cCoreResult.rgFaceRect[i].bottom);
	}
	m_pCarLeft->cCoreResult.nFaceCount = pCarLeft->m_pCarLeft->cCoreResult.nFaceCount;
	swpa_memcpy(m_pCarLeft->cCoreResult.rgFaceRect, pCarLeft->m_pCarLeft->cCoreResult.rgFaceRect, pCarLeft->m_pCarLeft->cCoreResult.nFaceCount*sizeof(HV_RECT));
}

void CSWCarLeft::Done(void)
{
	// 识别结果预处理。
	// 车速修正,不会出现0的车速。
	if (NULL != m_pTrackerCfg && m_pTrackerCfg->cScaleSpeed.fEnable && GetCarspeed() == 0 )
	{
		swpa_utils_srand(CSWDateTime::GetSystemTick());
		FLOAT fltCarspeed = swpa_utils_rand() % 20 + 20.0f;
		SetCarspeed(fltCarspeed);
	}
	// 超速违章的判断，目前只能是软件测速的超速。外接设备的方式不支持。
	// 超速违章在此判断有此不太合适。todo.
	BOOL fIsOverSpeed = FALSE;
	if ( NULL != m_pTrackerCfg && GetCarspeed() >= 0.1f && ((int)GetCarspeed() > m_pTrackerCfg->iSpeedLimit))
	{
		fIsOverSpeed = true;
	}
	if (fIsOverSpeed)
	{
		INT iPtType = GetPTType();
		iPtType |= PT_OVERSPEED;
		SetPTType(iPtType);
	}
	// 处理无牌车的车身颜色。
	if( GetPlateNo().Find("无车牌") != -1 && GetCarType() != CT_WALKMAN && GetCarType() != CT_BIKE)
	{
		SetCarColor(CC_GREY);
	}
	// 处理车辆类型
	if( GetCarType() == CT_UNKNOWN )
	{
		SetCarType(CT_SMALL);
	}

	// zhaopy 无车牌才进行此类替换。
	if( GetPlateNo().Find("无车牌") != -1 )
	{
	  if(GetCarType() == CT_WALKMAN)
	  {
		  SetPlateNo("  行人");
		  SetCarColor(CC_UNKNOWN);
	  }
	  //todo,为何要如此判断？又是非机动车又是摩托车？？
	  else if(GetCarType() == CT_BIKE && GetPlateType() != PLATE_DOUBLE_MOTO)
	  {
		  SetPlateNo("  非机动车");
		  SetCarColor(CC_UNKNOWN);
	  }
	}

	// 替换相机等级相关参数。与实时的一致。
	INT rgiInfo[3] = {0};
	if( m_pTrackerCfg != NULL && S_OK == CSWMessage::SendMessage(MSG_AUTO_CONTROL_GET_REALPARAM,0,(LPARAM)rgiInfo) )
	{
		SetLightType(rgiInfo[0]);
		if( m_pTrackerCfg->nCtrlCpl == 1 )
		{
			SetCplStatus(rgiInfo[1]);
		}
		else
		{
			SetCplStatus(-1);
		}
		SetPulseLevel(rgiInfo[2]);
	}	
	
	//todo,在此函数内部也存在修改违章类型，故需要在锁定之前调用此函数
	CSWString strEvent;
	GetEventDetInfo(strEvent);
	
	m_fReadOnly = TRUE;
}

