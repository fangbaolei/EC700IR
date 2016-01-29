#include "SWFC.h"
#include "SWDomeCameraCtrl.h"


CSWDomeCameraCtrl::CSWDomeCameraCtrl() 
	: m_fDayDetectCarConfigFlag(FALSE)
	, m_fNightDetectCarConfigFlag(FALSE)
	, m_fNightDetectPlateConfigFlag(FALSE)
	, m_fIsFocusFlag(FALSE)
	, m_fNightUseLedLight(TRUE)
	, m_iMinPlateAvgY(60)
	, m_iMaxPlateAvgY(180)
{

}

CSWDomeCameraCtrl::~CSWDomeCameraCtrl()
{

}


HRESULT CSWDomeCameraCtrl::SetDetectCarConfig(BOOL fIsDay)
{
	HRESULT hr = S_OK;
	m_fIsDay = fIsDay;
	if(fIsDay)
	{
		if(!IsDayDetectCarConfig())
		{
			hr = SetDayDetectCarConfig();
			if(hr == S_OK)
			{
				SetDayDetectCarConfigFlag(TRUE);
				SetNightDetectCarConfigFlag(FALSE);
			}
			else
			{
				SetDayDetectCarConfigFlag(FALSE);
			}
		}
	}
	else
	{
		if(!IsNightDetectCarConfig())
		{
			hr = SetNightDetectCarConfig();
			if(hr == S_OK)
			{
				SetNightDetectCarConfigFlag(TRUE);
				SetDayDetectCarConfigFlag(FALSE);
			}
			else
			{
				SetNightDetectCarConfigFlag(FALSE);
			}
		}
	}
	return hr;
}

BOOL CSWDomeCameraCtrl::IsDayDetectCarConfig()
{
	return m_fDayDetectCarConfigFlag;
}

BOOL CSWDomeCameraCtrl::IsNightDetectCarConfig()
{
	return m_fNightDetectCarConfigFlag;
}

VOID CSWDomeCameraCtrl::SetDayDetectCarConfigFlag(BOOL fDayConfig)
{
	m_fDayDetectCarConfigFlag = fDayConfig;
}

VOID CSWDomeCameraCtrl::SetNightDetectCarConfigFlag(BOOL fNightConfig)
{
	m_fNightDetectCarConfigFlag = fNightConfig;
}

HRESULT CSWDomeCameraCtrl::SetDayDetectCarConfig()
{
	SW_TRACE_DEBUG("INFO:Set day detect car config.");
	SetAEMode(ENUM_AE_AUTO);  		//AE自动
	SetAFEnable(ENUM_AF_AUTO);		//AF自动
	if(m_iCameraModel == SONY_FCBCH6500)
	{
		SetSaturationThreshold(8);		//设置饱和度
	}
	else
	{

		SetSaturationThreshold(5);		//设置饱和度
	}
	SetEdgeEnhance(3);				//锐度值	
	SetNRLevel(0);					//关闭降噪
	SetExpoComp(7);					//设置曝光补偿

	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetNightDetectCarConfig()
{
	SW_TRACE_DEBUG("*****INFO: Set night detect car config.\n");

	SetAEMode(ENUM_AE_SHUTTER);
	SetShutter(ENUM_SHUTTER_1_12);
	SetAFEnable(ENUM_AF_AUTO);
	if(m_iCameraModel == SONY_FCBCH6500)
	{
		SetSaturationThreshold(3);
		SetEdgeEnhance(1);
		SetGainLimit(0x0C);
	}
	else
	{
		//SetAWBMode(ENUM_AWB_AUTO);			//动力视讯机芯自动白平衡速度较慢，此为规避方法
		SetSaturationThreshold(4);
		SetEdgeEnhance(3);
		SetGainLimit(0x0F);
	}

	SetNRLevel(5);		//降噪等级
	SetExpoComp(0xA);   //曝光补偿 待确定
	SetNightDetectPlateConfigFlag(FALSE);
	SetLedMode(0);

	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetDetectPlateConfig(INT iBrightness)
{
	if(IsDayDetectCarConfig())
	{
		SetDayDetectPlateConfig(iBrightness);
	}
	else
	{
		SetNightDetectPlateConfig(iBrightness);
	}

	return S_OK;
}

VOID CSWDomeCameraCtrl::SetDetectPlateConfigFlag(BOOL fFlag)
{
	m_fIsSetDetectPlateConfigFlag = fFlag;
}

BOOL CSWDomeCameraCtrl::IsSetDetectPlateConfig()
{
	return m_fIsSetDetectPlateConfigFlag;
}

HRESULT CSWDomeCameraCtrl::SetDayDetectPlateConfig(INT iBrightness)
{
	INT iCurExpoCompValue = 0;

	GetExpoComp(&iCurExpoCompValue);
	if(iBrightness >= m_iMaxPlateAvgY)
	{
		if(m_iCameraModel == SONY_FCBCH6500)
			iCurExpoCompValue -= 4;
		else
			iCurExpoCompValue -= 2;
		iCurExpoCompValue = iCurExpoCompValue > 0 ? iCurExpoCompValue : 1;
		SetExpoComp(iCurExpoCompValue);
	}
	else if(iBrightness <= m_iMinPlateAvgY)
	{
		iCurExpoCompValue += 2;
		iCurExpoCompValue = iCurExpoCompValue > 0xE ? 0xE : iCurExpoCompValue;
		SetExpoComp(iCurExpoCompValue);
	}


	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetNightDetectPlateConfig(INT iBrightness)
{
	INT iLedPower = 0;
	if(!IsNightDetectPlateConfig())
	{
		SW_TRACE_DEBUG("Set night detect plate config.");
		FLOAT fltZoom = 0;
		INT iZoom = 0;
		INT iFocus = 0;
		if(CSWMessage::SendMessage(MSG_GET_ZOOM, NULL, (LPARAM)&iZoom))
			return E_FAIL;
		fltZoom = (FLOAT)iZoom / 100.0;
		ZoomToFocusValue(fltZoom, &iFocus);
		ZoomToLedPowerValue(fltZoom, &iLedPower);
		SetAFEnable(ENUM_AF_MANUAL);
		SetFocusValue(iFocus);
		SetManualFocusFlag(FALSE);
		SetNightDetectPlateConfigFlag(TRUE);
		if(IsNightUseLedLight())
		{
			SetLedMode(1);
			SetLedPower(iLedPower);
		}
		m_iGainLimit = 0x0C;
	}
	else if(iBrightness != 0)
	{
		INT iFar = 0, iMid = 0, iNear = 0; 
		GetLedPower(&iLedPower);
		iNear = (iLedPower >> 16) & 0xFF;
		iMid  = (iLedPower >> 8) & 0xFF;
		iFar = iLedPower & 0xFF;
		//TODO 计算LedPower最小值最大值
		if((m_iMinPlateAvgY + 20) > iBrightness)
		{
			iNear += 3;
			iNear = iNear > 10 ? 10 : iNear;
		}
		else if((m_iMaxPlateAvgY) < iBrightness)
		{
			if(iNear > 1)
			{
				iNear = 1;
				iLedPower = ((iNear & 0xFF) << 16) | ((iMid & 0xFF) << 8) | (iFar & 0xFF);
				SetLedPower(iLedPower);
			}
			else if(iNear == 1)
			{
				//SetAEMode(ENUM_AE_SHUTTER); //操作灯以后AE自动变成自动，此为规避方式
				if(m_iCameraModel == SONY_FCBCH6500)
				{
					if(iBrightness >= 220)
						m_iGainLimit -= 4;
					else if(iBrightness >= 200)
						m_iGainLimit -= 3;
					else
						m_iGainLimit -= 2;
				}
				else
				{
					if(iBrightness >= 200)
						m_iGainLimit -= 2;
					else
						m_iGainLimit -= 1;
				}
				m_iGainLimit = m_iGainLimit < 1 ? 1 : m_iGainLimit;

				if(0 != SetGainLimit(m_iGainLimit))
				{
					SW_TRACE_DEBUG("-----INFO: Set gain limit fail\n");
				}
			}
		}
	}
	return S_OK;
}

BOOL CSWDomeCameraCtrl::IsNightDetectPlateConfig()
{
	return m_fNightDetectPlateConfigFlag;
}

HRESULT CSWDomeCameraCtrl::SetNightDetectPlateConfigFlag(BOOL fFlag)
{
	m_fNightDetectPlateConfigFlag = fFlag;
	return S_OK;
}

BOOL CSWDomeCameraCtrl::IsNightUseLedLight()
{
	return m_fNightUseLedLight;
}

VOID CSWDomeCameraCtrl::SetNightUseLedLight(BOOL fFlag)
{
	m_fNightUseLedLight = fFlag;
}

HRESULT CSWDomeCameraCtrl::RestoreConfig()
{
	if(IsDayDetectCarConfig())
	{
		SetDayDetectCarConfig();
	}
	else
	{
		SetNightDetectCarConfig();
	}

	return S_OK;
}

HRESULT CSWDomeCameraCtrl::ResetConfig()
{
	RestoreConfig();
	SetGainLimit(0x0F);
	
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetDomeCameraPresetPTZ(FLOAT *fltPan, 
												  FLOAT *fltTilt,
												  FLOAT *fltZoom)
{
	INT iPan, iTilt, iZoom;
	if(CSWMessage::SendMessage(MSG_GET_PAN_COORDINATE, NULL, (LPARAM)&iPan))
		return E_FAIL;
	if(CSWMessage::SendMessage(MSG_GET_TILT_COORDINATE, NULL, (LPARAM)&iTilt))
		return E_FAIL;
	if(CSWMessage::SendMessage(MSG_GET_ZOOM, NULL, (LPARAM)&iZoom))
		return E_FAIL;

	*fltPan = (FLOAT)iPan / 10.0;
	*fltTilt = (FLOAT)iTilt / 10.0;
	*fltZoom = (FLOAT)iZoom / 100.0;

	return S_OK;
}


HRESULT CSWDomeCameraCtrl::SetDomeCameraPresetPTZ(FLOAT fltPan,
												  FLOAT fltTilt,
												  FLOAT fltZoom)
{
	INT iPan, iTilt, iZoom;
	iPan = (INT)(fltPan * 10);
	iTilt = (INT)(fltTilt * 10);
	iZoom = ZoomRatioToValue(fltZoom);

	if(CSWMessage::SendMessage(MSG_SET_PAN_COORDINATE, WPARAM(&iPan), NULL))
	{
		SW_TRACE_DEBUG("ERROR:Set Pan fail.\n");
		return E_FAIL;
	}
 	if(CSWMessage::SendMessage(MSG_SET_TILT_COORDINATE, WPARAM(&iPan), NULL))
	{
		SW_TRACE_DEBUG("ERROR:Set Tilt fail.\n");
		return E_FAIL;
	}
	if(CSWMessage::SendMessage(MSG_SET_ZOOM, WPARAM(&iZoom), NULL))
	{
		return E_FAIL;
	}
	
	return S_OK;
}


HRESULT CSWDomeCameraCtrl::GetDomeCameraCruiseNum(INT *iCruiseNum)
{
	//TODO
}

HRESULT CSWDomeCameraCtrl::GetDomeCameraPresetNum(INT *iPresetNum)
{
	//TODO
}


HRESULT CSWDomeCameraCtrl::GetDomeCameraPresetName(INT iPresetNum,
												   CHAR *szName, INT iNameLen)
{
	PRESETPOS_PARAM_STRUCT cPresetStruct;
	swpa_memset(&cPresetStruct, 0, sizeof(PRESETPOS_PARAM_STRUCT));

	if(CSWMessage::SendMessage(MSG_GET_PRESET_POS, (WPARAM)iPresetNum,
							   (LPARAM)&cPresetStruct))
	{
		SW_TRACE_DEBUG("ERROR: get preset pos struct fail.\n");
		return E_FAIL;
	}
	
	swpa_memcpy(szName, cPresetStruct.szName, 32);

	return S_OK;
}

HRESULT CSWDomeCameraCtrl::DomeCameraCruisePause()
{
	HRESULT hr = S_OK;
	if(hr != CSWMessage::SendMessage(MSG_PAUSE, NULL, NULL))
	{
		SW_TRACE_DEBUG("ERROR:Cruise pause fail:%d.\n", hr);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::DomeCameraCruiseResume()
{
	HRESULT hr = S_OK;
	if(hr != CSWMessage::SendMessage(MSG_RESUME, NULL, NULL))
	{
		SW_TRACE_DEBUG("ERROR:Cruise resume fail: %d.\n", hr);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::CallDomeCameraPreset(INT iPresetNum)
{
	HRESULT hr = S_OK;
	SW_TRACE_DEBUG("-------INFO: CALL PRESET %d\n", iPresetNum);
	if(hr != CSWMessage::SendMessage(MSG_CALL_PRESET_POS, (WPARAM)iPresetNum, (LPARAM)1))
	{
		SW_TRACE_DEBUG("ERROR:Call dome preset position fail:%d.\n", hr);
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CSWDomeCameraCtrl::GetGammaMode(INT *pGammaMode)
{

	if(CSWMessage::SendMessage(MSG_GET_GAMMA_STRENGTH, NULL, (LPARAM)pGammaMode))
	{
		SW_TRACE_DEBUG("ERROR:Get gamma mode fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::ZoomToFocusValue(FLOAT fltZoom, INT *pFocusValue)
{
// 聚焦经验值
	if(fltZoom > 20.0)
		*pFocusValue = 6300;
	else if(fltZoom > 10.0)
		*pFocusValue = 6500;
	else
		*pFocusValue = 6800;

	return S_OK;
}


/*
  @brief 利用放大倍数来计算球机水平方向的角度
  @param [IN] fltZoom 相机放大倍数
  @retval 水平角度值
*/
FLOAT CSWDomeCameraCtrl::ZoomToHViewAngle( FLOAT fltZoom )
{
    if( fltZoom > 30 || fltZoom < 1 )
    {
        return -1.0f;
    }
	//此表是根据机芯手册中最小焦距值与最大视场角值计算出来，具体计算方式详见相关文档
	FLOAT fltAngleOfHViewTable[30] = {
		59.500000, 31.897069, 21.572963, 16.263531, 
		13.042284, 10.882907, 9.335644, 8.172922, 
		7.267404, 6.542329, 5.948693, 5.453751, 
		5.034794, 4.675580, 4.364187, 4.091665, 
		3.851165, 3.637360, 3.446038, 3.273832, 
		3.118015, 2.976352, 2.847000, 2.728420, 
		2.619322, 2.518612, 2.425359, 2.338763,
		2.258138, 2.182885
	};

	//整数倍的视场角减去两个缩放倍数之间的差值乘以浮点部分
	INT iZoom = (INT)fltZoom;
	FLOAT fltDecimal = fltZoom - (FLOAT)iZoom;
	FLOAT fltDiff = fltAngleOfHViewTable[iZoom - 1] - fltAngleOfHViewTable[iZoom];
	FLOAT fltAngleOfView = fltAngleOfHViewTable[iZoom - 1] - fltDecimal * fltDiff;

    return fltAngleOfView;
}

/*
  @brief 利用放大倍数来计算球机和垂直方向的角度
  @param [IN] iZoom 相机放大倍数
  @retval 垂直角度值
*/
FLOAT CSWDomeCameraCtrl::ZoomToVViewAngle( FLOAT fltZoom )
{
    if( fltZoom > 30 || fltZoom < 1 )
    {
        return -1.0f;
    }
	//此表是放大倍数对应的视场角度值
	//此表根据机芯手册中最小焦距值与最大视场角值推算出来，具体计算方式详见相关文档
	float fltAngleOfVViewTable[30] = {
		35.644701, 18.264076, 12.233524, 9.190404, 
		7.357999, 6.134240, 5.259253, 4.602603, 
		4.091665, 3.682795, 3.348196, 3.069319, 
		2.833318, 2.631012, 2.455667, 2.302230, 
		2.166838, 2.046485, 1.938796, 1.841873, 
		1.754179, 1.674455, 1.601663, 1.534935, 
		1.473544, 1.416875, 1.364404, 1.315679, 
		1.270315, 1.227974
	};

	INT iZoom = (INT)fltZoom;
	FLOAT fltDecimal = fltZoom - (FLOAT)iZoom;
	FLOAT fltDiff = fltAngleOfVViewTable[iZoom - 1] - fltAngleOfVViewTable[iZoom];
	FLOAT fltAngleOfView = fltAngleOfVViewTable[iZoom - 1] - fltDecimal * fltDiff;

    return fltAngleOfView;
}



FLOAT CSWDomeCameraCtrl::ZoomValueToRatio(INT iDomeZoomValue)
{
	const INT iTableSize = 30;
	INT i = 0;
	FLOAT fltZoomRatio = 0.0;
	FLOAT fltDecimal = 0.0;		//小数部分
	ZOOM_RATIO s_cZoomTable[] =
	{
		{1, 0x0000}, {2, 0x16A1}, {3, 0x2063}, {4, 0x2628},
		{5, 0x2A1D}, {6, 0x2D13}, {7, 0x2F6D}, {8, 0x3161},
		{9, 0x330D}, {10, 0x3486}, {11, 0x35D7}, {12, 0x3709},
		{13, 0x3820}, {14, 0x3920}, {15, 0x3ACA}, {16, 0x3ADD},
		{17, 0x3B9C}, {18, 0x3C46}, {19, 0x3CDC}, {20, 0x3D60},
		{21, 0x3DD4}, {22, 0x3E39}, {23, 0x3E90}, {24, 0x3EDC},
		{25, 0x3F1E}, {26, 0x3F57}, {27, 0x3F8A}, {28, 0x3FB6},
		{29, 0x3FDC}, {30, 0x4000}
	};
	if(iDomeZoomValue < s_cZoomTable[0].iValue ||
	   iDomeZoomValue > s_cZoomTable[iTableSize - 1].iValue)
	{
		return -1.0;
	}
	for(i = 0; i < iTableSize - 1; i++)
	{
		if(s_cZoomTable[i].iValue <= iDomeZoomValue && 
			iDomeZoomValue < s_cZoomTable[i + 1].iValue)
		{
			fltDecimal = (FLOAT)(iDomeZoomValue - s_cZoomTable[i].iValue) / 
						(FLOAT)(s_cZoomTable[i + 1].iValue - s_cZoomTable[i].iValue);
			fltZoomRatio = (FLOAT)s_cZoomTable[i].iRatio + fltDecimal;
			return fltZoomRatio;
		}
	}
	if(i == (iTableSize - 1))
	{
		return (FLOAT)s_cZoomTable[iTableSize - 1].iRatio;
	}

	return -1.0;
}

BOOL CSWDomeCameraCtrl::ComputeDemoCameraPTZ( DOME_SRC_STATUS *pSrcParam, 
											  DOME_DST_STATUS *pDstParam)
{
	if( pSrcParam == NULL
		|| pDstParam == NULL )
	{
		return false;
	}
	
	BOOL fIsZoom = TRUE;
	DOUBLE HH = 0.0;			
	DOUBLE HV = 0.0;			
	DOUBLE HA = 0.0;			
	DOUBLE VA = 0.0;			
	DOUBLE dbNewHViewAng = 0.0; 
	DOUBLE dbCCDW = 0.0;		
	DOUBLE linBoxWidth = 0.0;	
	DOUBLE linBoxHeight = 0.0;	 
	DOUBLE lBoxCenterX = 0.0;	
	DOUBLE lBoxCenterY = 0.0;	
	DOUBLE dbCurFocalDis = 0.0; 
	INT nfactor = 0;
	
	if(pSrcParam->dbInputLTopX == 0.0 && 
	   pSrcParam->dbInputLTopY == 0.0 &&
	   pSrcParam->dbInputRLowerX == 0.0 &&
	   pSrcParam->dbInputRLowerY == 0.0)
	{
		fIsZoom = FALSE;
	}
	
	DOUBLE X0 = pSrcParam->dbImageCenterX;
	DOUBLE Y0 =  pSrcParam->dbImageCenterY;
	
	linBoxWidth = pSrcParam->dbInputRLowerX - pSrcParam->dbInputLTopX;
	
	linBoxHeight = pSrcParam->dbInputRLowerY - pSrcParam->dbInputLTopY;
	
	lBoxCenterX = pSrcParam->dbInputLTopX + linBoxWidth / 2;
	lBoxCenterY = pSrcParam->dbInputLTopY + (pSrcParam->dbInputRLowerY - pSrcParam->dbInputLTopY) / 2;
	
	
	HH = X0 / tan((pi*pSrcParam->dbCurHViewAng/2)/180);
	
	if( HH == 0.0000 )
	{
		return false;
	}
	
		
	if(fIsZoom)
	{
		nfactor = (lBoxCenterX - X0) < 0 ? -1 : 1;
		HA = pSrcParam->dbCurHTurnAng + 180*atan( fabs(lBoxCenterX - X0)/HH )/pi*nfactor;
	}
	else
	{
		nfactor = (pSrcParam->dbInputPointX - X0) < 0 ? -1 : 1;
		HA = pSrcParam->dbCurHTurnAng + 180*atan( fabs(pSrcParam->dbInputPointX - X0)/HH )/pi*nfactor;	
	}
	
	HA = HA < 0 ? 360 + HA : HA;
	HA = HA > 360 ? HA - 360 : HA;
	pDstParam->dbPan = HA;
	
	HV = Y0 / tan((pi*pSrcParam->dbCurVViewAng/2)/180);
		
	if( HV == 0.0000 )
	{
		return false;
	}
	

	
	if(fIsZoom)
	{
		nfactor = (lBoxCenterY - Y0) < 0 ? -1 : 1;
		VA = pSrcParam->dbCurVTurnAng + 180*atan(fabs(lBoxCenterY - Y0)/HV)/pi*nfactor;
	}
	else
	{
		nfactor = (pSrcParam->dbInputPointY - Y0) < 0 ? -1 : 1;
		VA = pSrcParam->dbCurVTurnAng + 180*atan(fabs(pSrcParam->dbInputPointY - Y0)/HV)/pi*nfactor;
	}
	VA = VA < 0 ? 0 : VA;
	VA = VA > 90 ? 90 : VA;
	pDstParam->dbTilt = VA;
	
	if(fIsZoom)
	{
		DOUBLE dblWidthAdjust = 0;
		if (0 >= (INT)(10*pDstParam->dbTilt)) //
		{
			//DOUBLE dblDistance = 540.0 / tan(pSrcParam->dbCurVViewAng/2);
			//nfactor = (lBoxCenterY - Y0) < 0 ? -1 : 1;
			//DOUBLE dblAdjust = fabs(tan(180*atan(fabs(lBoxCenterY - Y0)/HV)/pi*nfactor))*dblDistance;
				
			DOUBLE dblAdjust = pSrcParam->dbCurVTurnAng > 0 ? (pSrcParam->dbInputLTopY) + fabs(lBoxCenterY - Y0) : 2 *(pSrcParam->dbInputLTopY);
			dblWidthAdjust = 1920 * (1080 - dblAdjust)/*linBoxHeight*/ /1080;
		}
		else
		{
			dblWidthAdjust = 1920*linBoxHeight/1080;
		}

		if (linBoxWidth < dblWidthAdjust)
		{
			linBoxWidth = dblWidthAdjust;
		}
		dbNewHViewAng = 2*180*atan( 0.5*linBoxWidth / HH )/pi;
	}
	else
	{
		dbNewHViewAng = pSrcParam->dbCurHViewAng;	
	}
	
	dbCCDW = tan((pi*pSrcParam->dbMaxHViewAng / 2)/180 )*pSrcParam->dbMinFocalDis*2;
	
	
	dbCurFocalDis = 0.5*dbCCDW / tan(0.5*pi*dbNewHViewAng / 180);
	
	dbCurFocalDis = dbCurFocalDis > pSrcParam->dbMaxFocalDis ? pSrcParam->dbMaxFocalDis : dbCurFocalDis;
	
	pDstParam->dbZoomPosition = dbCurFocalDis;
	pDstParam->dbZoom = dbCurFocalDis / pSrcParam->dbMinFocalDis;
	
	return true;
}

HRESULT CSWDomeCameraCtrl::PositionToPTZ(HV_RECT *pRect, DOME_DST_STATUS *pDst)
{
	DOME_SRC_STATUS sSrcParam;
	FLOAT fltPan, fltTilt, fltZoom;

	GetDomeCameraPresetPTZ(&fltPan, &fltTilt, &fltZoom);
	
	sSrcParam.dbMaxHViewAng		= 59.500000;
	sSrcParam.dbMinHViewAng		= 2.182885;
	sSrcParam.dbMaxVViewAng		= 35.644701;
	sSrcParam.dbMinVViewAng		= 1.227974;
	sSrcParam.dbMaxFocalDis		= 129;		
	sSrcParam.dbMinFocalDis		= 4.3;
	sSrcParam.dbCurHViewAng		= ZoomToHViewAngle(fltZoom);
	sSrcParam.dbCurVViewAng		= ZoomToVViewAngle(fltZoom);
	sSrcParam.dbCurHTurnAng		= fltPan;
	sSrcParam.dbCurVTurnAng		= fltTilt;
	sSrcParam.dbImageCenterX	= 1920 / 2;  //TODO
	sSrcParam.dbImageCenterY	= 1080 / 2;
	sSrcParam.dbInputLTopX		= pRect->left;
	sSrcParam.dbInputLTopY		= pRect->top;
	sSrcParam.dbInputRLowerX	= pRect->right;
	sSrcParam.dbInputRLowerY	= pRect->bottom;
	sSrcParam.dbInputPointX     = 0;
	sSrcParam.dbInputPointY     = 0;

	ComputeDemoCameraPTZ(&sSrcParam, pDst);

	return S_OK;
}


INT CSWDomeCameraCtrl::ZoomRatioToValue(FLOAT fltZoomRatio)
{
	INT iZoomRatio = (INT)fltZoomRatio;
	FLOAT fltDecimal = 0;
	INT iZoomValue = 0;
	ZOOM_RATIO s_cZoomTable[] =
	{
		{1, 0x0000}, {2, 0x16A1}, {3, 0x2063}, {4, 0x2628},
		{5, 0x2A1D}, {6, 0x2D13}, {7, 0x2F6D}, {8, 0x3161},
		{9, 0x330D}, {10, 0x3486}, {11, 0x35D7}, {12, 0x3709},
		{13, 0x3820}, {14, 0x3920}, {15, 0x3ACA}, {16, 0x3ADD},
		{17, 0x3B9C}, {18, 0x3C46}, {19, 0x3CDC}, {20, 0x3D60},
		{21, 0x3DD4}, {22, 0x3E39}, {23, 0x3E90}, {24, 0x3EDC},
		{25, 0x3F1E}, {26, 0x3F57}, {27, 0x3F8A}, {28, 0x3FB6},
		{29, 0x3FDC}, {30, 0x4000}
	};

	if(iZoomRatio < 1 || iZoomRatio > 30)
	{
		return -1;
	}
	fltDecimal = fltZoomRatio - (FLOAT)iZoomRatio;	
	iZoomValue = s_cZoomTable[iZoomRatio - 1].iValue;
	FLOAT fltZoomValue = (FLOAT)(s_cZoomTable[iZoomRatio].iValue - s_cZoomTable[iZoomRatio - 1].iValue) * fltDecimal;
	return iZoomValue + (INT)fltZoomValue;
}


HRESULT CSWDomeCameraCtrl::SetGammaMode(INT iGammaMode)
{
	if(iGammaMode < 0)
		return S_OK;
	if(CSWMessage::SendMessage(MSG_SET_GAMMA_STRENGTH, (WPARAM)iGammaMode, 1))
	{
		SW_TRACE_DEBUG("ERROR:Set gamma mode fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::MoveBlockToCenter(HV_RECT *pBlowUpParam)
{
	INT BlowUpParam[4] = {0};
	HRESULT hr = S_OK;
	BlowUpParam[0] = pBlowUpParam->left;
	BlowUpParam[1] = pBlowUpParam->top;
	BlowUpParam[2] = pBlowUpParam->right - pBlowUpParam->left;
	BlowUpParam[3] = pBlowUpParam->bottom - pBlowUpParam->top;
	// if(m_iCameraModel != SONY_FCBCH6500 && !m_fIsDay)	//动力视讯机芯自动白平衡速度较慢，此为规避方法
	// {
	// 	GetRGBGain(m_rgRGBGain);
	// 	SetAWBMode(ENUM_AWB_MANUAL);
	// 	SetRGBGain(m_rgRGBGain);
	// }
	hr = CSWMessage::SendMessage(MSG_MOVE_BLOCK_TO_CENTER, (WPARAM)BlowUpParam, (LPARAM)1);
	
	return hr;
}

HRESULT CSWDomeCameraCtrl::SetAWBMode(INT iAWBMode)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_AWB_MODE, (WPARAM)iAWBMode, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set AWB mode fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetRGBGain(INT *pRGBGain)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_RGBGAIN, 1, (LPARAM)pRGBGain)))
	{
		SW_TRACE_DEBUG("ERROR:Get RGB Gain fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetRGBGain(INT *pRGBGain)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_RGBGAIN, (WPARAM)pRGBGain, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set RGB Gain fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetAEMode(INT iAEMode)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_AE_MODE, (WPARAM)iAEMode, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set AE mode fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetAEMode(INT *pAEMode)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_AE_MODE, 1, (LPARAM)pAEMode)))
	{
		SW_TRACE_DEBUG("ERROR:Get AE mode fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetIris(INT iIrisValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_IRIS, (WPARAM)iIrisValue, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set iris fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetIris(INT *pIrisValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_IRIS, 1, (LPARAM)pIrisValue)))
	{
		SW_TRACE_DEBUG("ERROR:Get iris fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetAGCGain(INT *pAGCGain)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_AGCGAIN, 1, (LPARAM)pAGCGain)))
	{
		SW_TRACE_DEBUG("ERROR:Get AGC gain fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetShutter(INT iShutterValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_SHUTTER, (WPARAM)iShutterValue, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set shutter fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetShutter(INT *pShutterValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_SHUTTER, 1, (LPARAM)pShutterValue)))
	{
		SW_TRACE_DEBUG("ERROR:Get shutter fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetAFEnable(INT iEnable)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_AF_ENABLE, (WPARAM)iEnable, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set AF Enable fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetAFEnable(INT *pEnable)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_AF_ENABLE, 1, (LPARAM)pEnable)))
	{
		SW_TRACE_DEBUG("ERROR:Get AF Enable fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetFocusMode(INT iFocusMode)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_FOCUS_MODE, (WPARAM)iFocusMode, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set FocusMode fail.\n");
		return E_FAIL;
	}
//	SetManualFocusFlag(TRUE);
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::DoOneFocus()
{
	if(FAILED(CSWMessage::SendMessage(MSG_DO_ONE_FOCUS, 1, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set One Focus Push fail.\n");
		return E_FAIL;
	}
	SetManualFocusFlag(TRUE);
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetManualFocusFlag(BOOL fFlag)
{
	m_fIsFocusFlag = fFlag;
	return S_OK;
}

BOOL CSWDomeCameraCtrl::IsFocused()
{
	return m_fIsFocusFlag;
}

HRESULT CSWDomeCameraCtrl::SetSaturationThreshold(INT iSaturationThreshold)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_SATURATIONTHRESHOLD, 
									  (WPARAM)iSaturationThreshold, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set saturation threshold fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetEdgeEnhance(INT iEdgeValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_SHARPENTHRESHOLD, 
									  (WPARAM)iEdgeValue, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set edge threshold fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetNRLevel(INT iNRLevel)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_DENOISE_STATE, 
									  (WPARAM)iNRLevel, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set NR Level fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetExpoComp(INT iExpoCompValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_EXPOCOMP_VALUE, 
									  (WPARAM)iExpoCompValue, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set ExpoCompValue fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetExpoComp(INT *pExpoCompValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_EXPOCOMP_VALUE, 
									  1, (LPARAM)pExpoCompValue)))
	{
		SW_TRACE_DEBUG("ERROR:Get ExpoCompValue fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetFocusValue(INT iFocusValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_FOCUS, 
									  (WPARAM)iFocusValue, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set focus value fail.\n");
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CSWDomeCameraCtrl::GetFocusValue(INT *pFocusValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_FOCUS, 
									  1, (LPARAM)pFocusValue)))
	{
		SW_TRACE_DEBUG("ERROR:Get focus value fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetLedMode(INT iLedMode)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_LED_MODE, 
									  (WPARAM)iLedMode, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set LED mode fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::EstimateDistance(FLOAT fltZoom,
											INT *pDistance)
{
	if(pDistance == NULL)
	{
		SW_TRACE_DEBUG("ERROR: Invalid argument.\n");
		*pDistance = 0;
		return E_FAIL;
	}

	
	FLOAT fltSceneWidth = 2.0;			//当前场景宽
	FLOAT fltAngle = ZoomToHViewAngle(fltZoom);
	if(fltAngle < 0.0)
		return E_FAIL;

	*pDistance = (INT)(fltSceneWidth / tan((pi / 180) * (fltAngle / 2)));
	
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::ZoomToLedPowerValue(FLOAT fltZoom, INT *pLedPowerValue)
{
	INT iFar = 0, iMid = 0, iNear = 0;
	if(fltZoom > 25)
	{
		iFar = 1;
		iNear = 0;
	}
	else if(fltZoom > 18)
	{
		iNear = 10;
	}
	else if(fltZoom > 10)
	{
		iNear = 5;
	}
	else if((INT)fltZoom >= 1)
	{
		iNear = 1;
	}
	*pLedPowerValue = ((iNear & 0xFF) << 16) | ((iMid & 0xFF) << 8) | (iFar & 0xFF);
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetLedPower(INT iPowerValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_SET_LED_POWER, 
									  (WPARAM)iPowerValue, 1)))
	{
		SW_TRACE_DEBUG("ERROR:Set LED power fail.\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSWDomeCameraCtrl::GetLedPower(INT *pPowerValue)
{
	if(FAILED(CSWMessage::SendMessage(MSG_GET_LED_POWER, 
									  1, (LPARAM)pPowerValue)))
	{
		SW_TRACE_DEBUG("ERROR:Get LED power fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDomeCameraCtrl::SetGainLimit(INT iGainLimit)
{
	return swpa_camera_basicparam_set_gain_limit(iGainLimit);
}

HRESULT CSWDomeCameraCtrl::SetCameraModel(INT iCameraModel)
{
	m_iCameraModel = iCameraModel;
	return S_OK;
}
