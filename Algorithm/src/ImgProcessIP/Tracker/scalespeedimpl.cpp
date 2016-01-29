#include "ScaleSpeedImpl.h"
#include "GlobalDataTbl.h"
#include "HvUtils.h"
#include "hv_math.h"
#include "PlateRecogParam.h"
#include <math.h>

//#define LOG_VIDEO_DET

#define SCALE_SPEED_PI       3.14159265358979323846
#define STD_PLATE_WIDTH	0.415f
#define STD_DOUBLE_PLATE_WIDTH	0.415f
#define STD_WJ_PLATE_WIDTH	0.415f
#define STD_PLATE_MOTO	0.190f
#define STD_DOUBLE_GREEN 0.273f

#define ARRSIZE(a) sizeof(a)/sizeof(a[0])
#define SAFE_DELETE(pObj)							\
	if (pObj)										\
{													\
	delete pObj;									\
	pObj = NULL;									\
}

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(pObj)						\
	if (pObj)										\
{													\
	delete[] pObj;									\
	pObj = NULL;									\
}
#endif

#define SAFE_RELEASE_IMAGE(pObj)					\
	if (pObj)										\
{													\
	pObj->Release();								\
	pObj = NULL;									\
}

HRESULT CreateScaleSpeedInstance(IScaleSpeed **ppScaleSpeed, int iVideoID/* = 0*/)
{
	if (ppScaleSpeed == NULL)
	{
		return E_POINTER;
	}

	CScaleSpeedImpl *pScaleSpeedImpl = new CScaleSpeedImpl();

	if (pScaleSpeedImpl == NULL)
	{
		return E_OUTOFMEMORY;
	}
	pScaleSpeedImpl->SetVideoID(iVideoID);

	// final assignment
	*ppScaleSpeed = pScaleSpeedImpl;

	return S_OK;
}

CScaleSpeedImpl::CScaleSpeedImpl()
	: m_fInitialized(FALSE)
	, m_pHvParam(NULL)
	, m_prgfltPixelsDisTable(NULL)
	, m_dwHeight(0)
{
}

void CScaleSpeedImpl::Clear()
{
	SAFE_DELETE_ARRAY(m_prgfltPixelsDisTable);
}

CScaleSpeedImpl::~CScaleSpeedImpl()
{
	Clear();
}

HRESULT CScaleSpeedImpl::InitScaleSpeed(SCALESPEED_CFG_PARAM *pScaleSpeedParam)
{
	Clear();

	if (pScaleSpeedParam == NULL)
	{
		return E_INVALIDARG;
	}

	// 软件测速默认参数
	m_fltCameraHigh = pScaleSpeedParam->fltCameraHigh;
	m_fltDistanceAll = pScaleSpeedParam->fltFullDistance;
	m_fltDistanceLeft = pScaleSpeedParam->fltLeftDistance;
	m_fltAdjustCoef = pScaleSpeedParam->fltAdjustCoef;
	m_nFlagMethods = pScaleSpeedParam->nMethodsFlag;
	//memset(m_fltTransMatrix, 0, 11 * sizeof(float));
	for(int i = 0; i < 11; i++)
	{
		m_fltTransMatrix[i] = pScaleSpeedParam->rgfltTransMarix[i];
	}
	if(m_fltDistanceLeft >= m_fltDistanceAll)
	{
		m_fltDistanceLeft = 25.0f;
		m_fltDistanceAll = 75.0f;
	}

/*	m_fltTransMatrix[0] = 0.4970550537f;
	m_fltTransMatrix[1] = -241.9865264893f;
	m_fltTransMatrix[2] = -30.4937019348f;
	m_fltTransMatrix[3] = 0.5036621094f;
	m_fltTransMatrix[4] = -21.2472610474f;
	m_fltTransMatrix[5] = -6.6555485725f;
	m_fltTransMatrix[6] = -231.5502014160f;
	m_fltTransMatrix[7] = 0.7746582031f;
	m_fltTransMatrix[8] = 0.0300335791f;
	m_fltTransMatrix[9] = -0.0085402671f;
	m_fltTransMatrix[10] = -0.0195075944f;

	char szTrackerName[20];
	if (1 == m_iVideoID)
	{
		sprintf(szTrackerName, "Tracker_2");
	}
	else
	{
		sprintf(szTrackerName, "Tracker");
	}
	char szSectionName[256];
	memset(szSectionName, 0, sizeof(szSectionName));

	if (m_pHvParam)
	{
		if (GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE && GetCurrentParam()->g_PlateRcogMode != PRM_PARK
			&& GetCurrentParam()->g_PlateRcogMode != PRM_PARK_SINGLE)
		{
			sprintf(szSectionName, "%s\\ScaleSpeed", szTrackerName);

			//电警模式已经将平面标定法去掉，因此在电警下屏蔽相关参数
			if (GetCurrentParam()->g_PlateRcogMode != PRM_ELECTRONIC_POLICE)
			{
				m_pHvParam->GetInt(
					szSectionName, "MethodsFlag",
					&m_nFlagMethods, m_nFlagMethods,
					0, 1,
					"计算模式,平面标定法：0,空间标定法：1", "", 5
					);

				m_pHvParam->GetFloat(
					szSectionName, "LeftDistance", 
					&m_fltDistanceLeft, m_fltDistanceLeft,
					0.01,10000,
					"屏幕下沿到龙门架距离","",5
					);

				m_pHvParam->GetFloat(
					szSectionName, "FullDistance", 
					&m_fltDistanceAll, m_fltDistanceAll,
					0.01,10000,
					"屏幕上沿到龙门架距离","",5
					);
			}

			m_pHvParam->GetFloat(
				szSectionName, "CameraHigh", 
				&m_fltCameraHigh, m_fltCameraHigh,
				0.01,10000,
				"摄像机高度","",5
				);

			m_pHvParam->GetFloat(
				szSectionName, "AdjustCoef", 
				&m_fltAdjustCoef, m_fltAdjustCoef,
				0.01,10,
				"修正系数","",5
				);

			//有效性判断
			if(m_fltDistanceLeft >= m_fltDistanceAll)
			{
				m_pHvParam->SetFloat(
					szSectionName, "LeftDistance", 
					25.0f
					);

				m_pHvParam->SetFloat(
					szSectionName, "FullDistance", 
					75.0f
					);
			}

			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_0", 
				&m_fltTransMatrix[0], m_fltTransMatrix[0],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_0","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_1", 
				&m_fltTransMatrix[1], m_fltTransMatrix[1],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_1","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_2", 
				&m_fltTransMatrix[2], m_fltTransMatrix[2],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_2","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_3", 
				&m_fltTransMatrix[3], m_fltTransMatrix[3],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_3","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_4", 
				&m_fltTransMatrix[4], m_fltTransMatrix[4],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_4","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_5", 
				&m_fltTransMatrix[5], m_fltTransMatrix[5],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_5","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_6", 
				&m_fltTransMatrix[6], m_fltTransMatrix[6],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_6","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_7", 
				&m_fltTransMatrix[7], m_fltTransMatrix[7],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_7","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_8", 
				&m_fltTransMatrix[8], m_fltTransMatrix[8],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_8","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_9", 
				&m_fltTransMatrix[9], m_fltTransMatrix[9],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_9","",5
				);
			m_pHvParam->GetFloat(
				szSectionName, "TransMarix_10", 
				&m_fltTransMatrix[10], m_fltTransMatrix[10],
				-3.40282347e+37F, 3.40282347e+37F,
				"变换矩阵_10","",5
				);
		}

	}
*/
	m_fInitialized = TRUE;

	return S_OK;
}

HRESULT CScaleSpeedImpl::SetHvParam(HvCore::IHvParam2* pHvParam)
{
	m_pHvParam = pHvParam;
	return S_OK;
}

HRESULT CScaleSpeedImpl::SetFrameProperty(const FRAME_PROPERTY& cFrameProperty)
{
	// 初始化屏幕距离对照表
	m_fltX0 = (float)cFrameProperty.iWidth;			// 全屏幕的象素点数量
	m_fltY0 = (float)(cFrameProperty.iHeight * 2);   // 进入的象素点是场的

	m_anBeta = atan(m_fltDistanceLeft / m_fltCameraHigh);
	float fltAlpha = atan(m_fltDistanceAll / m_fltCameraHigh);					// 弧度值， 需 * (180/PI)
	float anCAE = fltAlpha - m_anBeta;		// 弧度值， 需 * (180/PI)

	// TODO 根据车牌高度校正（车牌高度0.45m）
	float fltHighNew = m_fltCameraHigh - 0.45f;
	float fltDistanceAllNew = (fltHighNew *  m_fltDistanceAll)/ m_fltCameraHigh;
	float fltDistanceLeftNew = (fltHighNew *  m_fltDistanceLeft)/ m_fltCameraHigh;
	if (fltHighNew > 0)
	{
		m_fltCameraHigh = fltHighNew;
		m_fltDistanceAll = fltDistanceAllNew;
		m_fltDistanceLeft = fltDistanceLeftNew;
	}

	if (NULL != m_prgfltPixelsDisTable)
	{
		delete [] m_prgfltPixelsDisTable;
		m_prgfltPixelsDisTable = NULL;
	}
	if (m_prgfltPixelsDisTable == NULL)
	{
		// 申请内存
		m_prgfltPixelsDisTable = new float[cFrameProperty.iHeight];
		if (m_prgfltPixelsDisTable == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}
	memset(m_prgfltPixelsDisTable, 0, cFrameProperty.iHeight * sizeof(float));			// 全部清0
	m_dwHeight = (DWORD32)cFrameProperty.iHeight;

	// 计算屏幕所有点对应的实际距离
	// zhaopy
	if( 1 == m_nFlagMethods )
	{
		HV_POINT hpOrg, hpCur;
		hpOrg.x = (cFrameProperty.iWidth / 2);
		hpOrg.y = 0;
		float fltCD,fltW;
		m_prgfltPixelsDisTable[0] = 0;
		for(int i = 1; i < cFrameProperty.iHeight; i++)
		{
			hpCur.x = hpOrg.x;
			hpCur.y = i;
			m_prgfltPixelsDisTable[i] = CalcActureDistance(hpOrg, hpCur);
		}
	}
	else if( 0 == m_nFlagMethods )
	{
	for(int i = 0; i < cFrameProperty.iHeight; i++)
	{
		float fltDelta1 =  ((float)(i *  anCAE))/ cFrameProperty.iHeight;
		m_prgfltPixelsDisTable[i] = m_fltDistanceAll - (tan(fltAlpha - fltDelta1) * m_fltCameraHigh);
	}
	}

	// 新算法的一些中间变量计算
	m_anACF = (float)(SCALE_SPEED_PI - anCAE) / 2;
	m_disAC = sqrt(m_fltCameraHigh * m_fltCameraHigh + m_fltDistanceLeft * m_fltDistanceLeft);
	m_disCG = sqrt(m_disAC * m_disAC * 2 * (1 - cos(anCAE)));

	return S_OK;
}

bool CScaleSpeedImpl::GetActurePoint(HV_POINT ptOrg, HV_POINT& ptDest, float fltDistance)
{
	bool fRet = false;
	if(m_dwHeight == 0) return fRet;

	float fltDt = 0.0;
	float fltOrg = 0.0;

	if( 1 == m_nFlagMethods ) 
		fltOrg = m_prgfltPixelsDisTable[ptOrg.y];
	else if( 0 == m_nFlagMethods )
		fltOrg = m_prgfltPixelsDisTable[m_dwHeight - ptOrg.y];

	for(int i = ptOrg.y; i > 0; --i)
	{
		if( 1 == m_nFlagMethods ) 
			fltDt = m_prgfltPixelsDisTable[i] - fltOrg;
		else if( 0 == m_nFlagMethods )
			fltDt = m_prgfltPixelsDisTable[m_dwHeight - i] - fltOrg;

		if( fltDt < 0 ) fltDt = -fltDt;
		if( fltDt >= fltDistance)
		{
			fRet = true;
			ptDest.x = ptOrg.x;
			ptDest.y = i;
			break;
		}
	}

	return fRet;
}

void CScaleSpeedImpl::CalcActurePoint(HV_POINT poScreen, float &fltCD, float &fltW)
{
	switch (m_nFlagMethods)
	{
	case 0:
		{
			float disCF = m_disCG * (1 - (float)poScreen.y / m_fltY0);
			float disAF = sqrt(m_disAC * m_disAC + disCF * disCF - 2 * m_disAC * disCF * cos(m_anACF));
			float anCAF = asin((disCF / disAF) * sin(m_anACF));

			// 返回的Y坐标
			fltCD = m_fltCameraHigh * tan(m_anBeta + anCAF)	- m_fltDistanceLeft;

			float disAD = sqrt(m_fltCameraHigh * m_fltCameraHigh
				+ (m_fltDistanceLeft + fltCD) * (m_fltDistanceLeft + fltCD));
			float disKN = (disAD / disAF) * m_disCG * (m_fltX0 / m_fltY0);

			// 返回的X坐标
			fltW = disKN * ((float)poScreen.x / m_fltX0 - 0.5f);
		}
		break;
	case 1:
		{
			float rgfltA[2][2];
			float rgfltB[2];
			float *pM = m_fltTransMatrix;
			int iU = poScreen.x;
			int iV = poScreen.y;
			float fltZ = 0.45f;		//默认车牌高度0.45米
			rgfltA[0][0] = pM[0] - pM[8] * iU;
			rgfltA[0][1] = pM[1] - pM[9] * iU;
			rgfltA[1][0] = pM[4] - pM[8] * iV;
			rgfltA[1][1] = pM[5] - pM[9] * iV;
			rgfltB[0] = iU - pM[3] - (pM[2] - pM[10] * iU) * fltZ;
			rgfltB[1] = iV - pM[7] - (pM[6] - pM[10] * iV) * fltZ;
			
			if ((rgfltA[0][0] * rgfltA[1][1] - rgfltA[0][1] * rgfltA[1][0]) != 0)
			{
				fltW = (rgfltB[0] * rgfltA[1][1] - rgfltA[0][1] * rgfltB[1]) / (rgfltA[0][0] * rgfltA[1][1] - rgfltA[0][1] * rgfltA[1][0]);
				fltCD = (rgfltA[0][0] * rgfltB[1] - rgfltB[0] * rgfltA[1][0]) / (rgfltA[0][0] * rgfltA[1][1] - rgfltA[0][1] * rgfltA[1][0]);
			}
			else
			{
				fltW = 0;
				fltCD = 0;
			}
		}
		break;
	}
}

float CScaleSpeedImpl::CalcActureDistance(HV_POINT point1, HV_POINT point2)
{
	float fltCDBegin, fltCDEnd, fltWBegin, fltWEnd;
	fltCDBegin = fltCDEnd = fltWBegin = fltWEnd = 0.0;

	CalcActurePoint(point1, fltCDBegin, fltWBegin);
	CalcActurePoint(point2, fltCDEnd, fltWEnd);

	return sqrt((fltCDEnd - fltCDBegin) * (fltCDEnd - fltCDBegin)
		+ (fltWEnd - fltWBegin) * (fltWEnd - fltWBegin));

}

// 返回车辆速度
float CScaleSpeedImpl::CalcCarSpeed(
    		 CRect rcBegin,				// 开始位置坐标
             CRect rcEnd,				// 结束位置坐标
             DWORD32 dwDruTime	,		// 时间(毫秒)
			 bool fAdjustDistance,	// 是否通过车牌宽度矫正距离测量
			 PLATE_TYPE PlateType,		//车牌类型
			 float &fltScale			// 返回测量距离与实际距离的比例
			 )
{
	// 根据位置计算车辆速度并返回
	if (m_fInitialized == FALSE)
	{
		return 0;
	}

	int nCenterY1 = rcBegin.bottom;
	int nCenterY2 = rcEnd.bottom;
	float fltDistance = m_prgfltPixelsDisTable[nCenterY2] - m_prgfltPixelsDisTable[nCenterY1];	// 距离（米）
	if (fltDistance < 0)
	{
		fltDistance = -fltDistance;
	}
	//计算误差比例
	float fltStdPlateWidth = STD_PLATE_WIDTH;
	if (PlateType == PLATE_DOUBLE_YELLOW)
	{
		fltStdPlateWidth = STD_DOUBLE_PLATE_WIDTH;
	}
	else if (PlateType == PLATE_WJ)
	{
		fltStdPlateWidth = STD_WJ_PLATE_WIDTH;
	}
	if (fAdjustDistance)
	{
		fltScale = CalcActureDistance(CPoint(rcEnd.left, (rcEnd.bottom << 1)), CPoint(rcEnd.right, (rcEnd.bottom << 1))) / fltStdPlateWidth;
	}
	else
	{
		fltScale = 1.0;
	}

	return(fltDistance * 3600 * m_fltAdjustCoef / fltScale / dwDruTime);			// 速度(公里/小时)
}

float CScaleSpeedImpl::CalcCarSpeedNewMethod(
									CRect rcBegin,				// 开始位置坐标
									CRect rcEnd,				// 结束位置坐标
									DWORD32 dwDruTime,			// 时间(毫秒)
									bool fAdjustDistance,	// 是否通过车牌宽度矫正距离测量
									PLATE_TYPE PlateType,		//车牌类型
									float &fltScale			// 返回测量距离与实际距离的比例
									)
{
	// 根据位置计算车辆速度并返回
	if (m_fInitialized == FALSE)
	{
		return 0;
	}

	HV_POINT pointBegin, pointEnd;

	// 开始车牌中心点坐标
	pointBegin.x = rcBegin.left;
	pointBegin.y = rcBegin.bottom << 1;

	// 结束车牌中心点坐标
	pointEnd.x = rcEnd.left;
	pointEnd.y = rcEnd.bottom << 1;

	// 换算到实际坐标
	float fltDistance = CalcActureDistance(pointBegin, pointEnd);

	float fltStdPlateWidth = STD_PLATE_WIDTH;
	if (PlateType == PLATE_DOUBLE_YELLOW)
	{
		fltStdPlateWidth = STD_DOUBLE_PLATE_WIDTH;
	}
	else if (PlateType == PLATE_WJ)
	{
		fltStdPlateWidth = STD_WJ_PLATE_WIDTH;
	}
	else if (PlateType == PLATE_DOUBLE_MOTO)
	{
		fltStdPlateWidth = STD_PLATE_MOTO;
	}
	else if (PlateType == PLATE_DOUBLE_GREEN)
	{
		fltStdPlateWidth = STD_DOUBLE_GREEN;
	}

	if (fAdjustDistance)
	{
		fltScale = CalcActureDistance(CPoint(rcEnd.left, (rcEnd.bottom << 1)), CPoint(rcEnd.right, (rcEnd.bottom << 1))) / fltStdPlateWidth;
	}
	else
	{
		fltScale = 1.0;
	}

	return(fltDistance * 3600 * m_fltAdjustCoef / fltScale / dwDruTime);			// 速度(公里/小时)
}

HRESULT CScaleSpeedImpl::SetVideoID(int iVideoID)
{
	m_iVideoID = iVideoID;
	return S_OK;
}

