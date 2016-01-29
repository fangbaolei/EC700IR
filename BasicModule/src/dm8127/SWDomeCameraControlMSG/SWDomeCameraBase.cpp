#include "SWDomeCameraBase.h"

const CAMERA_DEVICE CSWDomeCameraBase::m_cDeviceTable[CAMERA_DEVICE_MAX_NUM] = 
{
	//device enum		vendor	model	device string
	{SONY_FCBCH6500, 	0x0020, 0x0466, "SONY FCB-CH6500"},
	{POWERVIEW_PV8430, 	0x0010, 0x07BD, "POWERVIEW PV8430"},   //动力视讯
	{-1, 				0x0000, 0x0000, "UNKNOWN"}
};

CSWCamera::CSWCamera()
{
	m_nVendorID = -1;
	m_nModelID = -1;
	m_nROMVersion = -1;
	m_iCamID = -1;
	swpa_memset(m_chCamName, 0, sizeof(m_chCamName));
}

CSWCamera::~CSWCamera()
{

}

INT CSWCamera::Initialize(SHORT nVendorID, SHORT nModelID, SHORT nROMVersion)
{
	INT iCamera = -1;
	INT id = 0;
	for(id = 0; id < sizeof(CSWDomeCameraBase::m_cDeviceTable); id++)
	{
		if((CSWDomeCameraBase::m_cDeviceTable[id].nVendorID == nVendorID) &&
		   (CSWDomeCameraBase::m_cDeviceTable[id].nModelID == nModelID))
		{
			iCamera = CSWDomeCameraBase::m_cDeviceTable[id].iCameraDevice;
			SW_TRACE_DEBUG("CamName:%s\n", CSWDomeCameraBase::m_cDeviceTable[id].chName);
			break;
		}
	}
//	if(-1 == iCamera)
//		return E_FAIL;

	m_nVendorID = nVendorID;
	m_nModelID = nModelID;
	m_nROMVersion = nROMVersion;
	m_iCamID = iCamera;
	swpa_memcpy(m_chCamName, CSWDomeCameraBase::m_cDeviceTable[id].chName, 32);

	switch(iCamera)
	{
	case SONY_FCBCH6500:
		SonyInitialize();
		break;
	case POWERVIEW_PV8430:
		PowerViewInitialize();
		break;
	default:
		m_iCamID = SONY_FCBCH6500;
		SonyInitialize();
		SW_TRACE_DEBUG("WARNING: ***** unknown camera type. *****\n");
		break;
	}
	return S_OK;
}

INT CSWCamera::ZoomRatioToValue(FLOAT fltZoomRatio)
{
	INT iZoomRatio = (INT)fltZoomRatio;
	FLOAT fltDecimal = 0;
	INT iZoomValue = 0;
	
	if(iZoomRatio < 1 || iZoomRatio > m_iMaxZoom)
	{
		return -1;
	}
	fltDecimal = fltZoomRatio - (FLOAT)iZoomRatio;	
	iZoomValue = m_cZoomTable[iZoomRatio - 1].iValue;
	FLOAT fltZoomValue = (FLOAT)(m_cZoomTable[iZoomRatio].iValue - m_cZoomTable[iZoomRatio - 1].iValue) * fltDecimal;
	return iZoomValue + (INT)fltZoomValue;
}

FLOAT CSWCamera::ZoomValueToRatio(INT iDomeZoomValue)
{
	INT i = 0;
	FLOAT fltZoomRatio = 0.0;
	FLOAT fltDecimal = 0.0;

	const INT iTableSize = m_iMaxZoom;
		
	if (iDomeZoomValue <= m_cZoomTable[0].iValue ) return 1.0;
	if (iDomeZoomValue >= m_cZoomTable[iTableSize - 1].iValue) return 30.0;
	
	for(i = 0; i < iTableSize - 1; i++)
	{
		if(m_cZoomTable[i].iValue <= iDomeZoomValue && 
		   iDomeZoomValue < m_cZoomTable[i + 1].iValue)
		{
			fltDecimal = (FLOAT)(iDomeZoomValue - m_cZoomTable[i].iValue) / 
				(FLOAT)(m_cZoomTable[i + 1].iValue - m_cZoomTable[i].iValue);
				
			fltZoomRatio = (FLOAT)m_cZoomTable[i].iRatio + fltDecimal;
				
			return fltZoomRatio;
		}
	}
	if(i >= (iTableSize - 1))
	{
		return (FLOAT)m_cZoomTable[iTableSize - 1].iRatio;
	}
	
	SW_TRACE_NORMAL("Err: ZoomValueToRatio failed\n");
	return -1.0;
}

FLOAT CSWCamera::ZoomToHViewAngle( FLOAT fltZoom )
{
	if( fltZoom > m_iMaxZoom || fltZoom < 1 )
	{
		return -1.0f;
	}
	
	INT iZoom = (INT)fltZoom;
	FLOAT fltDecimal = fltZoom - (FLOAT)iZoom;
	FLOAT fltDiff = 0.0;
	if(iZoom < 30)
		fltDiff = m_fltAngleOfHViewTable[iZoom - 1] - m_fltAngleOfHViewTable[iZoom];
		  
	FLOAT fltAngleOfView = m_fltAngleOfHViewTable[iZoom - 1] - fltDecimal * fltDiff;
	
	return fltAngleOfView;
}
		
		
		
FLOAT CSWCamera::ZoomToVViewAngle( FLOAT fltZoom )
{
	if( fltZoom > m_iMaxZoom || fltZoom < 1 )
	{
		return -1.0f;
	}
	
	INT iZoom = (INT)fltZoom;
	FLOAT fltDecimal = fltZoom - (FLOAT)iZoom;
	FLOAT fltDiff = 0.0;
	if(iZoom < 30)
		fltDiff = m_fltAngleOfVViewTable[iZoom - 1] - m_fltAngleOfVViewTable[iZoom];
	FLOAT fltAngleOfView = m_fltAngleOfVViewTable[iZoom - 1] - fltDecimal * fltDiff;
	
	return fltAngleOfView;
}

FLOAT CSWCamera::HViewAngleToZoom(FLOAT fltHViewAngle)
{
	INT i = 0;
	FLOAT fltZoom = 0.0;
	FLOAT fltDecimal = 0.0;

	const INT iTableSize = m_iMaxZoom;
		
	if (fltHViewAngle <= m_fltAngleOfHViewTable[iTableSize - 1]) 
		return (FLOAT)m_iMaxZoom;
	if (fltHViewAngle >= m_fltAngleOfHViewTable[0]) 
		return 1.0;
	
	for(i = 0; i < iTableSize - 1; i++)
	{
		if(m_fltAngleOfHViewTable[i] >= fltHViewAngle && 
		   fltHViewAngle > m_fltAngleOfHViewTable[i + 1])
		{
			fltDecimal = (FLOAT)(m_fltAngleOfHViewTable[i] - fltHViewAngle) / 
				(FLOAT)(m_fltAngleOfHViewTable[i] - m_fltAngleOfHViewTable[i + 1]);
				
			fltZoom = (FLOAT)(i + 1) + fltDecimal;
				
			return fltZoom;
		}
	}
	if(i >= (iTableSize - 1))
	{
		return (FLOAT)m_iMaxZoom;
	}

	return -1.0;
}

VOID CSWCamera::SonyInitialize()
{
	SW_TRACE_DEBUG("CSWSonyCamera::Initialize()");
	ZOOM_RATIO cTmpZoomTable[] = {
		{ 1, 0x0000}, { 2, 0x16A1}, { 3, 0x2063}, { 4, 0x2628},
		{ 5, 0x2A1D}, { 6, 0x2D13}, { 7, 0x2F6D}, { 8, 0x3161},
		{ 9, 0x330D}, {10, 0x3486}, {11, 0x35D7}, {12, 0x3709},
		{13, 0x3820}, {14, 0x3920}, {15, 0x3ACA}, {16, 0x3ADD},
		{17, 0x3B9C}, {18, 0x3C46}, {19, 0x3CDC}, {20, 0x3D60},
		{21, 0x3DD4}, {22, 0x3E39}, {23, 0x3E90}, {24, 0x3EDC},
		{25, 0x3F1E}, {26, 0x3F57}, {27, 0x3F8A}, {28, 0x3FB6},
		{29, 0x3FDC}, {30, 0x4000}
	};

	FLOAT fltAngleOfVViewTable[30] = {
		33.468750, 17.617800, 11.909469, 8.976711, 
		7.198105, 6.006047, 5.151996, 4.510248, 
		4.010484, 3.610321, 3.282706, 3.009563, 
		2.778357, 2.580123, 2.408281, 2.257891, 
		2.125173, 2.007187, 1.901608, 1.806579, 
		1.720593, 1.642419, 1.571039, 1.505603, 
		1.445399, 1.389825, 1.338365, 1.290579, 
		1.246088, 1.181250 
	};

	FLOAT fltAngleOfHViewTable[30] = {
		57.607536, 30.743994, 20.771813, 15.653662, 
		12.550980, 10.471927, 8.982564, 7.863515, 
		6.992093, 6.294367, 5.723151, 5.246918, 
		4.843809, 4.498192, 4.198590, 3.936392, 
		3.705006, 3.499304, 3.315235, 3.149559, 
		2.999651, 2.863361, 2.738915, 2.624835, 
		2.519876, 2.422987, 2.333272, 2.249963, 
		2.172397, 2.100000 
	};

	m_fltMaxFocalDis = 129;
	m_fltMinFocalDis = 4.3;
	m_iMaxZoom = 30;
	swpa_memcpy(m_cZoomTable, cTmpZoomTable, sizeof(ZOOM_RATIO) * m_iMaxZoom);
	swpa_memcpy(m_fltAngleOfVViewTable, fltAngleOfVViewTable, sizeof(FLOAT) * m_iMaxZoom);
	swpa_memcpy(m_fltAngleOfHViewTable, fltAngleOfHViewTable, sizeof(FLOAT) * m_iMaxZoom); 
}


VOID CSWCamera::PowerViewInitialize()
{
	SW_TRACE_DEBUG("CSWPowerViewCamera::Initialize()");
	ZOOM_RATIO cTmpZoomTable[] = {
		{1, 0x0000}, {2, 0x120A}, {3, 0x1B53}, {4, 0x211D},
		{5, 0x2544}, {6, 0x286A}, {7, 0x2AFC}, {8, 0x2D00},
		{9, 0x2ED1}, {10, 0x3060}, {11, 0x31BD}, {12, 0x32E3},
		{13, 0x3407}, {14, 0x352D}, {15, 0x3635}, {16, 0x371C},
		{17, 0x37EF}, {18, 0x38BB}, {19, 0x3975}, {20, 0x3A2B},
		{21, 0x3AD2}, {22, 0x3B74}, {23, 0x3C12}, {24, 0x3CA2},
		{25, 0x3D31}, {26, 0x3DBA}, {27, 0x3E3B}, {28, 0x3EBD},
		{29, 0x3F38}, {30, 0x4000}
	};

	FLOAT fltAngleOfHViewTable[30] = {
		62.18750, 40.42187, 29.81250, 23.71875, 19.60937,
//		80.18750, 40.42187, 29.81250, 23.71875, 19.60937,
		16.67187, 14.42187, 12.82812, 11.46875, 10.37500,
		9.50000, 8.84375, 8.18750, 7.53125, 6.98437,
		6.56250, 6.18750, 5.85375, 5.54687, 5.28125,
		5.03125, 4.81250, 4.60937, 4.42187, 4.25000,
		4.07812, 3.93750, 3.79687, 3.65625, 3.46875
	};

/*
	FLOAT fltAngleOfVViewTable[30] = {
		35.90625, 21.14062, 14.57812, 11.03125, 8.85937,
		7.45312, 6.45312, 5.76562, 5.20312, 4.75000, 
		4.40625, 4.14062, 3.89062, 3.62500, 3.42187,
		3.25000, 3.10937, 2.98437, 2.87500, 2.78125,
		2.68750, 2.59375, 2.51562, 2.43750, 2.37500,
		2.31250, 2.25000, 2.18750, 2.12500, 2.06250
	};
*/
	// FLOAT fltAngleOfVViewTable[30] = {
	// 	40.90625, 21.14062, 14.57812, 11.03125, 8.85937,
	// 	7.45312, 6.45312, 5.76562, 5.20312, 4.75000, 
	// 	4.40625, 4.14062, 3.89062, 3.62500, 3.42187,
	// 	3.25000, 3.10937, 2.98437, 2.87500, 2.78125,
	// 	2.68750, 2.59375, 2.51562, 2.43750, 2.37500,
	// 	2.31250, 2.25000, 2.18750, 2.12500, 2.06250
	// };

	FLOAT fltAngleOfVViewTable[30] = {
		40.90625, 23.14062, 15.97812, 12.13125, 9.65937,
		8.15312, 7.05312, 6.26562, 5.70312, 5.15000, 
		4.84625, 4.54062, 4.19062, 3.92500, 3.72187,
		3.55000, 3.40937, 3.28437, 3.07500, 2.98125,
		2.88750, 2.79375, 2.71562, 2.63750, 2.57500,
		2.51250, 2.45000, 2.38750, 2.32500, 2.26250
	};

	m_fltMaxFocalDis = 120;
	m_fltMinFocalDis = 4.0;
	m_iMaxZoom = 30;
	swpa_memcpy(m_cZoomTable, cTmpZoomTable, sizeof(ZOOM_RATIO) * m_iMaxZoom);
	swpa_memcpy(m_fltAngleOfVViewTable, fltAngleOfVViewTable, sizeof(FLOAT) * m_iMaxZoom);
	swpa_memcpy(m_fltAngleOfHViewTable, fltAngleOfHViewTable, sizeof(FLOAT) * m_iMaxZoom); 
}

BOOL CSWCamera::ComputeDemoCameraPTZ( DOME_SRC_STATUS *pSrcParam, DOME_DST_STATUS *pDstParam)
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
	
	DOUBLE pi = 3.14159265;
		
	
	if((INT)pSrcParam->dbInputLTopX == 0 && 
	   (INT)pSrcParam->dbInputLTopY == 0 &&
	   (INT)pSrcParam->dbInputRLowerX == 0 &&
	   (INT)pSrcParam->dbInputRLowerY == 0)
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
	VA = VA < -5 ? -5 : VA;
	VA = VA > 90 ? 90 : VA;
	pDstParam->dbTilt = VA;
	
	if(fIsZoom)
	{
		DOUBLE dblWidthAdjust = 0;
		if (-50 >= (INT)(10*pDstParam->dbTilt)) //
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
//	pDstParam->dbZoom = dbCurFocalDis / pSrcParam->dbMinFocalDis;
	pDstParam->dbZoom = HViewAngleToZoom((FLOAT)dbNewHViewAng);

	if(pDstParam->dbZoom > 30.0)
		pDstParam->dbZoom = 30.0;
	
	return true;
}

SHORT CSWCamera::GetVendorID()
{
	return m_nVendorID;
}

SHORT CSWCamera::GetModelID()
{
	return m_nModelID;
}

SHORT CSWCamera::GetROMVersion()
{
	return m_nROMVersion;
}

VOID CSWCamera::GetCamName(CHAR *pCamName)
{
	if(pCamName != NULL)
	{
		swpa_memcpy(pCamName, m_chCamName, 32);
	}
}

INT CSWCamera::GetCamID()
{
	return m_iCamID;
}

FLOAT CSWCamera::GetMaxFocalDis()
{
	return m_fltMaxFocalDis;
}

FLOAT CSWCamera::GetMinFocalDis()
{
	return m_fltMinFocalDis;
}

