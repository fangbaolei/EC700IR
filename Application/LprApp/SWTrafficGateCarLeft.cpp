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
      strEventCheck += "逆行";
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
      strEventCheck += "超速";

      INT iPtType = GetPTType();
      iPtType |= PT_OVERSPEED;
      SetPTType(iPtType);
  }

  if (!strEventCheck.IsEmpty())
  {
  		strInfo += "违章:是<";
  		strInfo += strEventCheck;
  		strInfo += ">";
  }
	return S_OK;
}

HRESULT CSWTrafficGateCarLeft::BuildPlateString(TiXmlDocument& xmlDoc)
{
	if(FAILED(CSWCarLeft::BuildPlateString(xmlDoc)))
	{
		return E_FAIL;
	}
	return S_OK;
}

