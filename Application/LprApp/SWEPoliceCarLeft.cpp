#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWEPoliceCarLeft.h"

CSWEPoliceCarLeft::CSWEPoliceCarLeft()
{
}

CSWEPoliceCarLeft::~CSWEPoliceCarLeft()
{
}

HRESULT CSWEPoliceCarLeft::GetEventDetInfo(CSWString& strInfo)
{	
	if(FAILED(CSWCarLeft::GetEventDetInfo(strInfo)))
	{
		return E_FAIL;
	}
	if (theApp->GetParam().Get().cTrackerCfgParam.nOutputRoadTypeName && GetRoadNo() >=0 && GetRoadNo() < MAX_ROADLINE_NUM - 1)
    {
#define STRING_ROAD_TYPE "直行;左转;直行,左转;右转;直行,右转;左转,右转;直行,左转,右转;调头;直行,调头;左转,调头;直行,左转,调头;右转,调头;直行,右转,调头;左转,右转,调头;直行,左转,右转,调头;非机动车道;"
        CSWString strRoadType = theApp->GetParam().Get().cTrackerCfgParam.rgcRoadInfo[GetRoadNo()].szRoadTypeName;
        CSWString szTmp = strRoadType;
        szTmp.ToUpper();
        if (strRoadType.IsEmpty() || szTmp == (CSWString)"NULL")
        {
            CSWString strTmp = STRING_ROAD_TYPE;
            int i = 0, j = 0;
            for (int pos = 0; j >= 0 && pos < theApp->GetParam().Get().cTrackerCfgParam.rgcRoadInfo[GetRoadNo()].iRoadType;)
            {
                j = strTmp.Find(";", i);
                pos++;
                if (pos < theApp->GetParam().Get().cTrackerCfgParam.rgcRoadInfo[GetRoadNo()].iRoadType)
                    i = j + 1;
            }
            if (j < 0)
                j = strTmp.Length();
            szTmp = strTmp.Substr(i, j - i);
            if (szTmp.IsEmpty())
                szTmp = "未知";
            strRoadType = CSWString("车道类型:") + szTmp;
        }
        else
            strRoadType = CSWString("车道类型:") + strRoadType;

        strInfo += strRoadType;
        strInfo += "\n";
    }
	//行驶类型
	switch (GetRunType())
  {
  case RT_FORWARD:  strInfo += "行驶类型:直行\n"; break;
  case RT_LEFT:     strInfo += "行驶类型:左转\n"; break;
  case RT_RIGHT:    strInfo += "行驶类型:右转\n"; break;
  case RT_TURN:     strInfo += "行驶类型:调头\n"; break;
  case RT_CONVERSE: strInfo += "行驶类型:逆行\n"; break;
  default:          strInfo += "行驶类型:未知\n"; break;
  }

  
  if(GetPTType() != PT_NORMAL)
  {
  	if(theApp->GetParam().Get().cResultSenderParam.iFilterUnSurePeccancy && GetUnSurePeccancy())
  	{
  		strInfo += "违章:否";
		// 如果是不合格违章，确保输出图片合理。
		if( GetOutputCaptureImage() != 1 )
		{
			if( s_iOutputBestImage == 0 )
			{
				SetImage(2, NULL);
			}
			if( s_iOutputLastImage == 0 )
			{
				SetImage(3, NULL);
			}
			SetImage(4, NULL);
		}
  	}
  	else
  	{
  		strInfo += "违章:是";
  	}
  	//这里需要增加违章优先级的判断
    if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
    {
        CSWString str = theApp->GetParam().Get().cResultSenderParam.szPeccancyPriority;
        if (str[str.Length()-1] != '-')
            str += "-";
    
        int i = 0, j = 0;
        while (i != -1 && j != -1)
        {
            j = str.Find("-", i);
            if (j != -1)
            {
                CSWString s = str.Substr(i, j-i);
                if (s == (CSWString)"逆行"
                        && (GetPTType() & PT_CONVERSE) == PT_CONVERSE)
                {
                    SetPTType(PT_CONVERSE);
                    break;
                }
                if (s == (CSWString)"非机动车道"
                        && (GetPTType() & PT_ESTOP) == PT_ESTOP)
                {
                    SetPTType(PT_ESTOP);
                    break;
                }
                if (s == (CSWString)"闯红灯"
                        && (GetPTType() & PT_RUSH) == PT_RUSH)
                {
                    SetPTType(PT_RUSH);
                    break;
                }
                if (s == (CSWString)"不按车道行驶"
                        && (GetPTType() & PT_ACROSS) == PT_ACROSS)
                {
                    SetPTType(PT_ACROSS);
                    break;
                }
                if (s == (CSWString)"压线"
                        && (GetPTType() & PT_OVERLINE) == PT_OVERLINE)
                {
                    SetPTType(PT_OVERLINE);
                    break;
                }
                if (s == (CSWString)"超速"
                        && (GetPTType() & PT_OVERSPEED) == PT_OVERSPEED)
                {
                    SetPTType(PT_OVERSPEED);
                    break;
                }
                i = j+1;
            }
        }
    }
  	//结束修改,2010-12-08
    BOOL fFound = FALSE;
    BOOL fFirst = TRUE;
    CSWString strMid = "";
    if (!fFound && (GetPTType() & PT_CONVERSE) == PT_CONVERSE)
    {
        strMid += "逆行";
        if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
            fFound = TRUE;
    }
    if (!fFound && (GetPTType() & PT_ESTOP) == PT_ESTOP)
    {
    		if(!strMid.IsEmpty())
    		{
    			strMid += ";";
    		}	
        strMid += "非机动车道";
        if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
            fFound = TRUE;
    }
    if (!fFound && (GetPTType() & PT_RUSH) == PT_RUSH)
    {
    		if(!strMid.IsEmpty())
    		{
    			strMid += ";";
    		}
        strMid += "闯红灯";
        if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
            fFound = TRUE;
    }
    if (!fFound && (GetPTType() & PT_ACROSS) == PT_ACROSS)
    {
    		if(!strMid.IsEmpty())
    		{
    			strMid += ";";
    		}
        strMid += "不按车道行驶";
        if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
            fFound = TRUE;
    }
    if (!fFound && (GetPTType() & PT_OVERLINE) == PT_OVERLINE)
    {
    		if(!strMid.IsEmpty())
    		{
    			strMid += ";";
    		}
        strMid += "压线";
        if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
            fFound = TRUE;
    }
    if (!fFound && (GetPTType() & PT_OVERSPEED) == PT_OVERSPEED)
    {
    		if(!strMid.IsEmpty())
    		{
    			strMid += ";";
    		}
        strMid += "超速";
        if (!theApp->GetParam().Get().cResultSenderParam.nOutputPeccancyType)
            fFound = TRUE;
    }
    if (!strMid.IsEmpty())
    {
        //去掉最后一个分号
        strInfo += "<";
        strInfo += strMid;
        strInfo += ">\n";
    }
  }
  else
  {
  	strInfo += "违章:否\n";
  }
  
  //压线,车道线从左到右编号从0开始
  if (GetDetectOverYellowLineEnable() && GetIsOverYellowLine() != COLT_INVALID)
  {
      if (GetIsOverYellowLine() == COLT_NO)
      {
          strInfo += "压线:无\n";
      }
      else
      {
          CSWString strYellowLine = "";
          strYellowLine.Format("压线:车道线%d<%s>\n", GetIsOverYellowLine() + m_pTrackerCfg->iStartRoadNum, GetIsDoubleYellowLine() ? "黄线" : "实线");
          strInfo += strYellowLine;
      }
  }
  //越线,车道线从左到右编号从0开始
  if (GetDetectCrossLineEnable() && GetIsCrossLine() != COLT_INVALID)
  {
      if (GetIsCrossLine() == COLT_NO)
      {
          strInfo += "越线:无\n";
      }
      else
      {
          CSWString strCrossLine = "";
          strCrossLine.Format("越线:车道线%d\n", GetIsCrossLine() + m_pTrackerCfg->iStartRoadNum);
          strInfo += strCrossLine;
      }
  }
  strInfo += "停止线前:";
  switch (GetOnePosScene().lsForward)
  {
  case TLS_UNSURE:strInfo += "直??;"; break;
  case TLS_RED:   strInfo += "直红;"; break;
  case TLS_YELLOW:strInfo += "直黄;"; break;
  case TLS_GREEN: strInfo += "直绿;"; break;
  }

  switch (GetOnePosScene().lsLeft)
  {
  case TLS_UNSURE:strInfo += "左??;"; break;
  case TLS_RED:   strInfo += "左红;"; break;
  case TLS_YELLOW:strInfo += "左黄;"; break;
  case TLS_GREEN: strInfo += "左绿;"; break;
  }

  switch (GetOnePosScene().lsRight)
  {
  case TLS_UNSURE:strInfo += "右??;"; break;
  case TLS_RED:   strInfo += "右红;"; break;
  case TLS_YELLOW:strInfo += "右黄;"; break;
  case TLS_GREEN: strInfo += "右绿;"; break;
  }

  switch (GetOnePosScene().lsTurn)
  {
  case TLS_UNSURE:strInfo += "调??"; break;
  case TLS_RED   :strInfo += "调红"; break;
  case TLS_YELLOW:strInfo += "调黄"; break;
  case TLS_GREEN :strInfo += "调绿"; break;
  }

  strInfo += "\n停止线后:";
  switch (GetTwoPosScene().lsForward)
  {
  case TLS_UNSURE:strInfo += "直??;"; break;
  case TLS_RED:   strInfo += "直红;"; break;
  case TLS_YELLOW:strInfo += "直黄;"; break;
  case TLS_GREEN: strInfo += "直绿;"; break;
  }

  switch (GetTwoPosScene().lsLeft)
  {
  case TLS_UNSURE:strInfo += "左??;"; break;
  case TLS_RED:   strInfo += "左红;"; break;
  case TLS_YELLOW:strInfo += "左黄;"; break;
  case TLS_GREEN: strInfo += "左绿;"; break;
  }

  switch (GetTwoPosScene().lsRight)
  {
  case TLS_UNSURE:strInfo += "右??;"; break;
  case TLS_RED:   strInfo += "右红;"; break;
  case TLS_YELLOW:strInfo += "右黄;"; break;
  case TLS_GREEN: strInfo += "右绿;"; break;
  }

  switch (GetTwoPosScene().lsTurn)
  {
  case TLS_UNSURE:strInfo +="调??"; break;
  case TLS_RED:   strInfo +="调红"; break;
  case TLS_YELLOW:strInfo +="调黄"; break;
  case TLS_GREEN: strInfo +="调绿"; break;
  }

  //红灯开始时间
  CSWString str;
  
  //左转红灯
  if ( GetRedStart().dwLeftL != 0 )
  {
  		CSWDateTime dt(GetRedStart().dwLeftL);
      str.Format("\n左转红灯时间:%02d:%02d:%02d:%03d", dt.GetHour(), dt.GetMinute(), dt.GetSecond(), dt.GetMSSecond());
      strInfo += str;
  }
  
  //直行红灯
  if ( GetRedStart().dwForwardL != 0 )
  {
      CSWDateTime dt(GetRedStart().dwForwardL);
      str.Format("\n直行红灯时间:%02d:%02d:%02d:%03d", dt.GetHour(), dt.GetMinute(), dt.GetSecond(), dt.GetMSSecond());
      strInfo += str;
  }
  
  //右转红灯
  if ( GetRedStart().dwRightL != 0 )
  {
      CSWDateTime dt(GetRedStart().dwRightL);
      str.Format("\n右转红灯时间:%02d:%02d:%02d:%03d", dt.GetHour(), dt.GetMinute(), dt.GetSecond(), dt.GetMSSecond());
      strInfo += str;
  }
  
  //调头红灯
  if ( GetRedStart().dwTurnL != 0 )
  {
      CSWDateTime dt(GetRedStart().dwTurnL);
      str.Format("\n调头红灯时间:%02d:%02d:%02d:%03d", dt.GetHour(), dt.GetMinute(), dt.GetSecond(), dt.GetMSSecond());
      strInfo += str;
  }
  
  //添加红灯坐标
  if (GetRedLightCount() > 0 && GetRedLightCount() < 20 && GetAvgY() < theApp->GetParam().Get().cResultSenderParam.nRedPosBrightness)
  {
      strInfo += "\nRedLight:";
      for (int i = 0; i < GetRedLightCount(); ++i)
      {
          str.Format("(%04d,%04d,%04d,%04d)", GetRedLightPos()[i].left, GetRedLightPos()[i].top, GetRedLightPos()[i].right, GetRedLightPos()[i].bottom);
          strInfo += str;
      }
  }
	return S_OK;
}


void CSWEPoliceCarLeft::Done(void)
{
	CSWCarLeft::Done();
	//图片输出限制
	//  如果是违章结果或者是强制输出三张轨迹图即前面两张大图不输出。
	//  如果不违章则按设置的两张大图的输出方式来输出。电警目前在DSP端第一、第二张大图用的是和第三、第四张大图一致。
	if( s_iOutputCaptureImage == 1 || m_pCarLeft->cCoreResult.ptType != PT_NORMAL )
	{
		SetImage(0, NULL);
		SetImage(1, NULL);
	}
	else
	{
		// 如果有五张图，输出图的标志为后面三张图的大图标志，无前面两张图的大图标志。目前电警是如此。
		if( GetImage(2) != NULL && GetImage(3) != NULL && GetImage(4) != NULL )
		{
			SetImage(0, NULL);
			SetImage(1, NULL);
			if( s_iOutputBestImage == 0 )
			{
				SetImage(2, NULL);
			}
			if( s_iOutputLastImage == 0 )
			{
				SetImage(3, NULL);
			}
			SetImage(4, NULL);
		}
		else
		{
			SetImage(2, NULL);
			SetImage(3, NULL);
			SetImage(4, NULL);
			if( s_iOutputBestImage == 0 )
			{
				SetImage(0, NULL);
			}
			if( s_iOutputLastImage == 0 )
			{
				SetImage(1, NULL);
			}
		}
		
	}	


}

