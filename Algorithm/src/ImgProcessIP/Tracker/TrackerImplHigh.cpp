#include "TrackerImplHigh.h"
#include "HvUtils.h"
#include "GlobalDataTbl.h"
#include "EPAppUtils.h"

// todo.
#include "SwCore.h"

#define NORMAL_NUM 12
#define YELLOW_NUM 20
#define BIGSUN_NUM 10
#define MAX2(a, b)  ((a) > (b) ? (a) : (b))
#define MIN2(a, b)  ((a) < (b) ? (a) : (b))
#define WALK_MAN_MISSING_TIME	6

_CARCOLOR_INFO g_rgCarColorInfo[] =
{
	{ 0,	"未知",	    0		},
	{ 1,	"白",		116		},
	{ 2,	"灰",		120		},
	{ 3,	"黑",		47	    },
	{ 4,	"红",	    118		},
	{ 5,	"黄",		119		},
	{ 6,	"绿",	    122		},
	{ 7,	"蓝",		121		},
	{ 8,	"紫",		123		},
	{ 9,	"粉",		124		},
	{ 10,	"棕",	    125		}
};

/*
_LOGO_INFO g_rgLogoInfo[] =
{
	{ 0,	"未知",	0		},
	{ 1,	"爱卡",		126		},
	{ 2,	"奥迪",		127		},
	{ 3,	"奔驰",		128		},
	{ 4,	"本田",	129		},
	{ 5,	"比亚迪",		130		},
	{ 6,	"标致",	    131		},
	{ 7,	"别克",		132		},
	{ 8,	"大众",		133		},
	{ 9,	"道奇",		134		},
	{ 10,	"帝豪",	    135		},
	{ 11,	"东风",	    136		},
	{ 12,	"东南",	137		},
	{ 13,	"丰田",		138		},
	{ 14,	"福特",	139		},
	{ 15,	"哈飞",		140		},
	{ 16,	"红旗",	    141	    },
	{ 17,	"皇冠",		142		},
	{ 18,	"吉奥",		143		},
	{ 19,	"吉利",		144		},
	{ 20,	"江淮",	145		},
	{ 21,	"凯迪拉克",	146		},  
	{ 22,	"雷克萨斯",	    147		},
	{ 23,	"猎豹",	    148		},
	{ 24,	"铃木",	149		},
	{ 25,	"马自达",		150	},
	{ 26,	"欧宝",	151		},
	{ 27,	"奇瑞",		152		},
	{ 28,	"起亚",	    153	    },
	{ 29,	"日产",		154		},
	{ 30,	"三菱",		155		},
	{ 31,	"双环",		156		},
	{ 32,	"斯巴鲁",	157		},
	{ 33,	"五菱",	158		},
	{ 34,	"夏利",	    159		},
	{ 35,	"现代",	    160		},
	{ 36,	"雪佛兰",	161		},
	{ 37,	"一汽",		162		},
	{ 38,	"英菲尼迪",	163		},
	{ 39,	"长安",		164		},
	{ 40,	"长城",	    165	    },
	{ 41,	"中兴",		166		},
	{ 42,	"其他",	167		}

};
*/

#if RUN_PLATFORM == PLATFORM_DSP_BIOS
#include "HvDspTime.h"
#endif
#if RUN_PLATFORM == PLATFORM_WINDOWS
#include <TIME.h>
#endif

extern char g_szDebugInfo[256];

#define MAX_SMARTSAVE_COUNT 4

int CFinalResultParam::m_iBlockTwinsTimeout = 90;
int CFinalResultParam::m_iSendTwinResult = 1;

// 视频检测区域默认值
const int VIDEO_DETECT_LEFT = 0;
const int VIDEO_DETECT_RIGHT = 100;
const int VIDEO_DETECT_TOP = 0;
const int VIDEO_DETECT_BOTTOM = 100;
int CTrackerImpl::m_sMinTriggerTimeMs = 0;


// zhaopy todo.
// 调试信息输出
// 平台级接口
PLATFORM_FUCTION_LIST* g_pFuction = NULL;

void Venus_OutputDebug(char* szMsg)
{
	return;
	if( g_pFuction != NULL && g_pFuction->pfDebugOutPut != NULL )
	{
		g_pFuction->pfDebugOutPut(szMsg);
	}
}

enum
{
	LOG_LV_ERROR 	= 0,
	LOG_LV_NORMAL  	= 1,
	LOG_LV_OPERATE 	= 2,
	LOG_LV_DEBUG   	= 3
};

void OutPutDebugMsg(int iLevel, char* szMsg)
{
	if( g_pFuction != NULL && g_pFuction->pfDebugOutPut != NULL )
	{
		char* szMsgInfo = new char[strlen(szMsg) + 64];
		if( NULL != szMsgInfo )
		{
			sprintf(szMsgInfo, "<DSPLOG><LV:%d>%s", iLevel, szMsg);
			g_pFuction->pfDebugOutPut(szMsgInfo);
			delete[] szMsgInfo;
			szMsgInfo = NULL;
		}
	}
}

int Dsp_DebugInfo(char* szfmt, ...)
{
    //	if ( !IsDebugDeviceEnabled(iDeviceID) ) return 0;
#if 1
	return 0;
#else
    static char s_szbuf[1024];

    va_list arglist;
    int iRetVal = 0;

    s_szbuf[0] = 0;

    va_start( arglist, szfmt );
    iRetVal = std::vsnprintf(s_szbuf, sizeof(s_szbuf) - 1, szfmt, arglist);
    va_end( arglist );


#if 1
    Venus_OutputDebug(s_szbuf);
#else
    int nAddLen = strlen(s_szbuf);
    if (nAddLen + g_nDspDebugInfoLen < 1024-1)
    {
        memcpy(g_pDspDebugInfo + g_nDspDebugInfoLen, s_szbuf, nAddLen + 1);	  
        g_nDspDebugInfoLen += nAddLen;	
    }
#endif

    return iRetVal;
#endif
}

unsigned int Venus_GetSystemTick()
{
	if( g_pFuction != NULL && g_pFuction->pfGetSystemTick != NULL )
	{
		return g_pFuction->pfGetSystemTick();
	}
	return GetSystemTick();
}

// 算法二进制数据输出接口
void OutPutDebugInfo(unsigned char* pbData, unsigned int cbDataSize)
{
	if( g_pFuction != NULL && g_pFuction->pfOutPutDebugInfo != NULL )
	{
		g_pFuction->pfOutPutDebugInfo(pbData, cbDataSize);
	}
}

namespace Plate
{
	// grammer of the characters
#define DEFINE_PLATE_GRAMMER_NAME(a, b, r1, r2, r3, r4, r5, r6, r7)		r1, r2, r3, r4, r5, r6, r7,
	const int g_cnType[PLATE_TYPE_COUNT][7]=
	{
		PLATE_TYPE_LIST(DEFINE_PLATE_GRAMMER_NAME)
	};
}

#undef RTN_HR_IF_FAILED
#define RTN_HR_IF_FAILED(func)                          \
{	                                                    \
	HRESULT hr=func;                                    \
	if (FAILED(hr))                                     \
	{                                                   \
		char szMsg[256];								\
		sprintf(szMsg, "%s l:%d FAILED:%08X\n", #func, __LINE__, hr);\
		Venus_OutputDebug(szMsg);                       \
		HV_DebugInfo(		                            \
				DEBUG_STR_ERROR_FILE,                   \
				"FAILED %s(hr=0x%08X): ", #func"\n", hr \
				);                                      \
		return hr;	                                    \
	}	                                                \
}

#define VALUE_LIMIT(x, down, up)  ((x > up) ? up : ((x < down) ? down : x))

extern int GetLineX(int iY, int iLineNum);

static void CalcTLTrapDetArea(
		TRAP_DETAREA* pTrapDetArea,
		int nStartY,
		int nEndY,
		int nFrameWidth,
		int nFrameHeight
		)
{
	int nBottomLine = nFrameHeight * nEndY / 100;
	int nTopLine = nFrameHeight * nStartY / 100;
	if (nTopLine < 0)  nTopLine = 0;

	int nBottomLineX0 = 0;
	int nBottomLineX1 = 0;
	int nTopLineX0 = 0;
	int nTopLineX1 = 0;
	int nSlopeRatio0 = 0.f;  //road0, dx / dy * 100
	int nSlopeRatio1 = 0.f;  //road1, dx / dy * 100

	char szMsg[255];
	sprintf(szMsg, "nCarArriveLine:%d [%d %d %d %d]\n", nBottomLine, GetLineX(nTopLine, 0), GetLineX(nTopLine, 1), GetLineX(nBottomLine, 0), GetLineX(nBottomLine, 1));

	Venus_OutputDebug(szMsg);

	if(CTrackInfo::m_iRoadNumberBegin == 0)
	{
		nBottomLineX0 = GetLineX(nBottomLine, 0);
		nBottomLineX1 = GetLineX(nBottomLine, 1);
		nTopLineX0 = GetLineX(nTopLine, 0);
		nTopLineX1 = GetLineX(nTopLine, 1);
	}
	else
	{
		nBottomLineX0 = GetLineX(nBottomLine, 1);
		nBottomLineX1 = GetLineX(nBottomLine, 0);
		nTopLineX0 = GetLineX(nTopLine, 1);
		nTopLineX1 = GetLineX(nTopLine, 0);
	}

	nTopLineX0 = (nTopLineX0 + nBottomLineX0) >> 1;
	nTopLineX1 = (nTopLineX1 + nBottomLineX1) >> 1;

	nBottomLineX0 = nBottomLineX0 * 100 / nFrameWidth;
	nBottomLine = nBottomLine * 100 / nFrameHeight;
	nBottomLineX1 = nBottomLineX1 * 100 / nFrameWidth;
	nTopLineX0 = nTopLineX0 * 100 / nFrameWidth;
	nTopLine = nTopLine * 100 / nFrameHeight;
	nTopLineX1 = nTopLineX1 * 100 / nFrameWidth;

	pTrapDetArea->BottomLeftX = VALUE_LIMIT(nBottomLineX0, 0, 100);
	pTrapDetArea->BottomLeftY = VALUE_LIMIT(nBottomLine, 0, 100);
	pTrapDetArea->BottomRightX = VALUE_LIMIT(nBottomLineX1, 0, 100);
	pTrapDetArea->BottomRightY = VALUE_LIMIT(nBottomLine, 0, 100);

	pTrapDetArea->TopLeftX = VALUE_LIMIT(nTopLineX0, 0, 100);
	pTrapDetArea->TopLeftY = VALUE_LIMIT(nTopLine, 0, 100);
	pTrapDetArea->TopRightX = VALUE_LIMIT(nTopLineX1, 0, 100);
	pTrapDetArea->TopRightY = VALUE_LIMIT(nTopLine, 0, 100);

	sprintf(szMsg,"CalcTrapDetArea [%d %d] [%d %d] [%d %d] [%d %d]\n",
	pTrapDetArea->TopLeftX, pTrapDetArea->TopLeftY,
	pTrapDetArea->TopRightX, pTrapDetArea->TopRightY,
	pTrapDetArea->BottomLeftX, pTrapDetArea->BottomLeftY,
	pTrapDetArea->BottomRightX, pTrapDetArea->BottomRightY);

	Venus_OutputDebug(szMsg);
}

static void CalcPlateScale(TRAP_DETAREA* pTrapDetArea, int nFrameHeight,  int* pnMinScale, int* pnMaxScale)
{
    //计算车牌宽度
    int iMin = 0;
    int iMax = 0;
    int iPlateWidth = (int)((pTrapDetArea->TopRightX - pTrapDetArea->TopLeftX) * nFrameHeight /100 * 0.18f);
    for (int i = 0; i < 16;i++)
    {
        if (56 * pow(1.1, i) > iPlateWidth)
        {
            iMax = i;
            break;
        }
        else
        {
            iMin = i;
        }
    }
    int iDetMinScaleNum = iMin;
    iPlateWidth = (int)((pTrapDetArea->BottomRightX - pTrapDetArea->BottomLeftX) * nFrameHeight /100 * 0.18f);
    for (int i = iMin; i < 16;i++)
    {
        if (56 * pow(1.1, i) > iPlateWidth)
        {
            iMax = i;
            break;
        }
    }
    int iDetMaxScaleNum = iMax;
    int iDiff = iDetMaxScaleNum - iDetMinScaleNum;
    if (iDiff > 4)
    {
    	iDetMinScaleNum =  iDetMaxScaleNum - 4;
    }
    if (iDetMinScaleNum < 0) iDetMinScaleNum = 0;
    if (iDetMaxScaleNum > 16) iDetMaxScaleNum = 16;
    if (iDetMaxScaleNum <= iDetMinScaleNum) iDetMaxScaleNum = iDetMinScaleNum + 2;

    *pnMinScale = iDetMinScaleNum;
    *pnMaxScale = iDetMaxScaleNum;
}

// 卡口专用计算检测、扫描区域
void CTrackerImpl::CalcTrapDetArea(
		TRAP_DETAREA* pTrapDetArea,
		int nCarArriveLineY,  // 百分比
		int nFrameWidth,
		int nFrameHeight
		)
{
	if (nFrameWidth == 0 || nFrameHeight == 0) return;
	if (nCarArriveLineY < 50) nCarArriveLineY = 50;
	int nBottomLine = nFrameHeight * nCarArriveLineY / 100;
	int nTopLine = nFrameHeight * (nCarArriveLineY - 35) / 100;

	int nBottomLineX0 = 0;
	int nBottomLineX1 = 0;
	int nTopLineX0 = 0;
	int nTopLineX1 = 0;
	int nSlopeRatio0 = 0.f;  //road0, dx / dy * 100
	int nSlopeRatio1 = 0.f;  //road1, dx / dy * 100

	if(CTrackInfo::m_iRoadNumberBegin == 0)
	{
		nBottomLineX0 = GetLineX(nBottomLine, 0);
		nBottomLineX1 = GetLineX(nBottomLine, 1);
		nTopLineX0 = GetLineX(nTopLine, 0);
		nTopLineX1 = GetLineX(nTopLine, 1);
	}
	else
	{
		nBottomLineX0 = GetLineX(nBottomLine, 1);
		nBottomLineX1 = GetLineX(nBottomLine, 0);
		nTopLineX0 = GetLineX(nTopLine, 1);
		nTopLineX1 = GetLineX(nTopLine, 0);
	}

	nBottomLineX0 = nBottomLineX0 * 100 / nFrameWidth;
	nBottomLine = nBottomLine * 100 / nFrameHeight;
	nBottomLineX1 = nBottomLineX1 * 100 / nFrameWidth;
	nTopLineX0 = nTopLineX0 * 100 / nFrameWidth;
	nTopLine = nTopLine * 100 / nFrameHeight;
	nTopLineX1 = nTopLineX1 * 100 / nFrameWidth;

	pTrapDetArea->BottomLeftX = VALUE_LIMIT(nBottomLineX0, 0, 100);
	pTrapDetArea->BottomLeftY = VALUE_LIMIT(nBottomLine, 0, 100);
	pTrapDetArea->BottomRightX = VALUE_LIMIT(nBottomLineX1, 0, 100);
	pTrapDetArea->BottomRightY = VALUE_LIMIT(nBottomLine, 0, 100);

	pTrapDetArea->TopLeftX = VALUE_LIMIT(nTopLineX0, 0, 100);
	pTrapDetArea->TopLeftY = VALUE_LIMIT(nTopLine, 0, 100);
	pTrapDetArea->TopRightX = VALUE_LIMIT(nTopLineX1, 0, 100);
	pTrapDetArea->TopRightY = VALUE_LIMIT(nTopLine, 0, 100);

	// 根据矩形形区域计算检测区
	m_cDetAreaInfo.DetectorAreaLeft = MIN(m_cfgParam.cTrapArea.TopLeftX, m_cfgParam.cTrapArea.BottomLeftX);
	m_cDetAreaInfo.DetectorAreaTop = MIN(m_cfgParam.cTrapArea.TopLeftY, m_cfgParam.cTrapArea.BottomLeftY);
	m_cDetAreaInfo.DetectorAreaRight = MAX(m_cfgParam.cTrapArea.TopRightX, m_cfgParam.cTrapArea.BottomRightX);
	m_cDetAreaInfo.DetectorAreaBottom = MAX(m_cfgParam.cTrapArea.TopRightY, m_cfgParam.cTrapArea.BottomRightY);

	m_rcVideoDetArea.left = 10;//m_cDetAreaInfo.DetectorAreaLeft;
	m_rcVideoDetArea.top = 0;//m_cDetAreaInfo.DetectorAreaTop;
	m_rcVideoDetArea.right = 90;//m_cDetAreaInfo.DetectorAreaRight;
	m_rcVideoDetArea.bottom = 100;//m_cDetAreaInfo.DetectorAreaBottom;

	/*char szT[1024] = {0};
	sprintf(szT, "[%d_%d_%d_%d]-----[%d_%d_%d_%d]\n",
		m_cDetAreaInfo.DetectorAreaLeft,
		m_cDetAreaInfo.DetectorAreaTop,
		m_cDetAreaInfo.DetectorAreaRight,
		m_cDetAreaInfo.DetectorAreaBottom,
		m_rcVideoDetArea.left,
		m_rcVideoDetArea.top,
		m_rcVideoDetArea.right,
		m_rcVideoDetArea.right
		);
	Venus_OutputDebug(szT);*/
}

void CFinalResultParam::Create(CTrackInfo &TrackInfo, PlateRecogParam *pParam)
{
	*((CTrackInfoHeader*)this)=TrackInfo;
	m_pParam=pParam;
	m_dwLastResultTime = GetSystemTick();

	m_pObj = NULL;				// 不能复制物体对象，否则释放的时候错误
}

void CFinalResultParam::UpdateDetectSaver(DWORD32 dw_TimeRef)
{
	int iCount = 0;
	DWORD32 dwNowTime = dw_TimeRef;

	if(dwNowTime == 0)
	{
		dwNowTime = GetSystemTick();
	}

	for(int ix=0; ix<m_iTimeinCount; ix++)
	{
		if(m_detectsaver[ix].dw_TrackEndTime <= 0)
		{
			m_detectsaver[ix].dw_TrackEndTime = GetSystemTick();
		}

		if(dwNowTime > m_detectsaver[ix].dw_TrackEndTime)
		{
			if( (dwNowTime - m_detectsaver[ix].dw_TrackEndTime) >= (DWORD32)(m_iBlockTwinsTimeout * 1000))
			{
				iCount ++;
			}
		}
		else if(dwNowTime < m_detectsaver[ix].dw_TrackEndTime)
		{
			if( (m_detectsaver[ix].dw_TrackEndTime - dwNowTime) >= (DWORD32)( m_iBlockTwinsTimeout * 1000))
			{
				iCount ++;
			}
		}
	}

	if(iCount > 0)
	{
		this->m_iTimeinCount -= iCount;

		if(this->m_iTimeinCount < 0)
		{
			//HV_Trace("\nOutOfRange... m_iTimeinCount = 0\n");
			this->m_iTimeinCount = 0;
		}

		for(int i=0; i<m_iTimeinCount; i++)
		{
			m_detectsaver[i].dw_TrackEndTime = m_detectsaver[i+iCount].dw_TrackEndTime;
			m_detectsaver[i+iCount].dw_TrackEndTime = 0;
			for(int j=0; j<8; j++)
			{
				m_detectsaver[i].rgContent[j] = m_detectsaver[i+iCount].rgContent[j];
				m_detectsaver[i+iCount].rgContent[j] = 0;
			}
		}
	}	
}

void CFinalResultParam::SetOrder()
{
	DWORD32 dwEndTimeTemp = 0;
	BYTE8 rgContentTemp[8] = {0};
	for(int iX=0; iX<=m_iTimeinCount; iX++)
	{
		for(int iY=iX+1; iY<=m_iTimeinCount; iY++)
		{
			if(m_detectsaver[iX].dw_TrackEndTime > m_detectsaver[iY].dw_TrackEndTime)
			{
				dwEndTimeTemp = m_detectsaver[iX].dw_TrackEndTime;
				m_detectsaver[iX].dw_TrackEndTime = m_detectsaver[iY].dw_TrackEndTime;
				m_detectsaver[iY].dw_TrackEndTime = dwEndTimeTemp;

				for(int iH=0;iH<8;iH++)
				{
					rgContentTemp[iH] = m_detectsaver[iX].rgContent[iH];
					m_detectsaver[iX].rgContent[iH] = m_detectsaver[iY].rgContent[iH];
					m_detectsaver[iY].rgContent[iH] = rgContentTemp[iH];
				}
			}
		}
	}

}
//TODO: 根据车牌出现的时间判断
bool CFinalResultParam::IsTwinResult(const CTrackInfo &TrackInfo)
{
	int iRoad = MatchRoad(TrackInfo.m_rgPlateInfo[0].rcPos.CenterPoint().x, TrackInfo.m_rgPlateInfo[0].rcPos.CenterPoint().y);

	if(m_iTimeinCount <= 0)
	{
		return false;
	}
	//判断是否允许一辆车一定时间内通过两次 先比较后存放入队列中
	if(m_iSendTwinResult)
	{	//采用出牌的时间作为条件  
		DWORD32 dwTimeRef = 0;//TrackInfo.m_dwBestSnapShotRefTime;
		//更新队列
		UpdateDetectSaver(dwTimeRef);
	
		//比较结果
		int iCount = 0;
		//HV_Trace("队列更新后数组长度:%d\n",m_iTimeinCount);
		for(int ix=0; ix<m_iTimeinCount; ix++)
		{
			for(int iy=0; iy<8; iy++)
			{
				if(m_detectsaver[ix].rgContent[iy] == TrackInfo.m_rgbVotedResult[iy])
				{
					iCount++;
				}
			}
			if(iCount == 8)
			{
				// 电子警察还需要判断同一个车道内才行
				if(GetCurrentMode() == PRM_ELECTRONIC_POLICE)
				{
					if(iRoad == MatchRoad(m_detectsaver[ix].rcPos.CenterPoint().x, m_detectsaver[ix].rcPos.CenterPoint().y))
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
			else
			{
				iCount = 0;
			}
		}

		//将结果添加入队列
		if(dwTimeRef != 0)
		{
			m_detectsaver[m_iTimeinCount].dw_TrackEndTime = dwTimeRef;
		}
		else
		{
			m_detectsaver[m_iTimeinCount].dw_TrackEndTime = GetSystemTick();
		}

		for(int i = 0; i < TrackInfo.m_cPlateInfo; i++)
		{
			if(TrackInfo.m_rgPlateInfo[i].nInfoType == PI_LPR)
			{
				m_detectsaver[m_iTimeinCount].rcPos = TrackInfo.m_rgPlateInfo[i].rcPos;
			}
		}

		for(int i=0; i<8; i++)
		{
			m_detectsaver[m_iTimeinCount].rgContent[i] = TrackInfo.m_rgbVotedResult[i];
		}

		//排序
		SetOrder();
		
		m_iTimeinCount ++;
	}

	// 判断是否允许一辆车通过两次以上
	if ( m_iSendTwinResult &&
		GetSystemTick() - m_dwLastResultTime >= ( DWORD32 )( m_iBlockTwinsTimeout * 1000 )
		)
	{
		return false;
	}
	// 如果是停车状态，则输出
	if (m_fCarIsStop)
	{
		return false;
	}
	CRect rcLastEnd=LastInfo().rcPos;
	CRect rcThisBegin=TrackInfo.m_rgPlateInfo[0].rcPos;
	int cDiff(0);

	// TODO: Magic Number
	for (int i=0; i<7; i++)
	{
		if (TrackInfo.m_rgbVotedResult[i]!=m_rgbVotedResult[i])
		{
			cDiff++;
		}
	}

	if (cDiff == 0)
	{
		// 电子警察还需要判断同一个车道内才行
		if(GetCurrentMode() == PRM_ELECTRONIC_POLICE)
		{
			if(iRoad == MatchRoad(rcLastEnd.CenterPoint().x, rcLastEnd.CenterPoint().y))
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	if (cDiff > 2)
	{
		return false;
	}

	rcThisBegin.InflateRect(
		(m_pParam->g_fltTrackInflateX*rcThisBegin.Width()).ToInt(),
		(m_pParam->g_fltTrackInflateY*rcThisBegin.Height()).ToInt()
		);

	switch (m_pParam->g_MovingDirection)
	{
	case MD_TOP2BOTTOM:			// 由远及近
		if (rcThisBegin.bottom >= rcLastEnd.top)
		{
			return true;
		}
		break;
	case MD_BOTTOM2TOP:			// 由近及远
		if (rcThisBegin.top <= rcLastEnd.bottom)
		{
			return true;
		}
		break;
	case MD_LEFT2RIGHT:			// 由左及右
		if (rcThisBegin.right >= rcLastEnd.left)
		{
			return true;
		}
		break;
	case MD_RIGHT2LEFT:			// 由右及左
		if (rcThisBegin.left <= rcLastEnd.right)
		{
			return true;
		}
		break;
	}

	return false;
}


HRESULT CFinalResultParam::UpdateResult(const CTrackInfo &TrackInfo)
{
	// TODO: Did nothing, just block the subsequent result
	return S_OK;
}

HRESULT CreateTracker_Video(ITracker** ppTracker, int iVideoID/* = 0*/)
{
	if (ppTracker == NULL)
	{
		return E_POINTER;
	}

	CTrackerImpl *pTrackerImpl = new CTrackerImpl();

	if (pTrackerImpl == NULL)
	{
		return E_OUTOFMEMORY;
	}

	//创建各功能组件
	RTN_HR_IF_FAILED(pTrackerImpl->OnCreate());

	// final assignment
	*ppTracker=pTrackerImpl;

	return S_OK;
}

CTrackerImpl::CTrackerImpl()
: m_fInitialized(FALSE)
, m_nWidth(0)
, m_nHeight(0)
, m_nStride(0)
, m_pParam(NULL)
, m_pPlateDetector(NULL)
, m_pProcessPlate(NULL)
, m_pRecognition(NULL)
, m_pScaleSpeed(NULL)
, m_cTrackInfo(0)
, m_cDetected(0)
, m_cObjTrackInfo(0)
, m_pInspector(NULL)
, m_pLastResult(NULL)
, m_fEnablePreciseRectify(FALSE)
, m_nVideoDetMode(NO_VIDEODET)
, m_pHvModel(NULL)
, m_LightType(DAY)	
, m_nCarLenTop(0)
, m_nCarLenBottom(0)
, m_nCarWTop(0)
, m_nCarWBottom(0)
, m_dwLastCarLeftTime(0)
, m_iLastLightStatus(-1)
, m_iCurLightStatus(-1)
, m_iLastOkLightStatus(-1)
, m_nFirstLightType(LIGHT_TYPE_COUNT)
, m_fOutputCarSize(FALSE)
, m_iLastTriggerTimeMs(0)
, m_iLastFirstTime(0)
, m_iLastSecondTime(0)
, m_iLastThirdTime(0)
,m_dwTriggerTimes(0)
,m_dwLastTriggerInfo(0)
,m_iRushSpeed(200)
,m_iVoteFrameCount(30)
,m_iCheckAcross(0)
,m_iFilterRushPeccancy(0)
,m_iCarArriveTrig(0)
,m_cEPObjTrackInfo(0)
,m_iNoPlateDetMode(0)
,m_iPulseLevel(0)
,m_iLightDetected(0)
,m_iEdgeFilterThreld(150)
, m_fEnableDetFace(FALSE)
, m_nMinFaceScale(2)
, m_nMaxFaceScale(20)
, m_cScanEP(0)
, m_iUsedLight(1)
, m_fIsModifyParam(false)

{
	srand(0);
	for (int i = 0; i < s_knMaxVideoChannel; i++)
	{
		m_rgpObjectDetector[i] = NULL;
	}
	m_PlateLightType = LIGHT_TYPE((int)LIGHT_TYPE_COUNT / 2);

	// 厦门项目新增参数初始化
	m_iOutPutType = 0;
	m_iWalkManSpeed = 100;
	m_fltBikeSensitivity = 4.0;

	m_nIOLevel = 0;

	OnInit();
}

void CTrackerImpl::Clear()
{
	SAFE_DELETE(m_pPlateDetector);
	SAFE_DELETE(m_pProcessPlate);
	SAFE_DELETE(m_pRecognition);
	//	SAFE_DELETE(m_pPretreatment);
	SAFE_DELETE(m_pScaleSpeed);

	for (int i = 0; i < s_knMaxVideoChannel; i++)
	{
		if (m_rgpObjectDetector[i] != NULL)
		{
			m_rgpObjectDetector[i]->Release();
			m_rgpObjectDetector[i] = NULL;
		}
	}
}

CTrackerImpl::~CTrackerImpl()
{
	Clear();
}

//创建功能模块
HRESULT CTrackerImpl::OnCreate()
{
	Clear();

	//初始化公用数据
	GlobleDataSrcInit();

	//组件创建
	RTN_HR_IF_FAILED(CreatePlateDetectorInstance(&m_pPlateDetector));
	RTN_HR_IF_FAILED(CreateRecognition(&m_pRecognition));
	RTN_HR_IF_FAILED(CreateProcessPlateInstance(&m_pProcessPlate));
	//	RTN_HR_IF_FAILED(CreatePretreatmentInstance(&m_pPretreatment));

	RTN_HR_IF_FAILED(CreateScaleSpeedInstance(&m_pScaleSpeed));				// 测速接口

	for (int i = 0; i < s_knMaxVideoChannel; i++)
	{
		RTN_HR_IF_FAILED(CreateObjectDetector(&m_rgpObjectDetector[i]));
	}

	//建立组件关系
	RTN_HR_IF_FAILED(m_pProcessPlate->SetDetector(m_pPlateDetector));
	RTN_HR_IF_FAILED(m_pProcessPlate->SetRecognition(m_pRecognition));


	RTN_HR_IF_FAILED(m_cCarDetector.Init());

	return S_OK;
}

HRESULT CTrackerImpl::OnInit()
{
	//清理
	//ClearTrackInfo();

	//内部变量恢复初始值
	m_fInitialized = FALSE;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nStride = 0;
	m_cTrackInfo = 0;
	m_pLastResult = NULL;

	//可配置参数
	m_cDetAreaInfo.fEnableDetAreaCtrl = FALSE;
	PlateRecogMode prmCur = GetCurrentParam()->g_PlateRcogMode;
	if(  prmCur >= PRM_MaxMode )
	{
		m_cDetAreaInfo.DetectorAreaLeft = 0;
		m_cDetAreaInfo.DetectorAreaRight = 100;
		m_cDetAreaInfo.DetectorAreaTop = 0;
		m_cDetAreaInfo.DetectorAreaBottom = 40;
		m_cDetAreaInfo.nDetectorMinScaleNum = 6;
		m_cDetAreaInfo.nDetectorMaxScaleNum = 12;
	}
	else
	{
		m_cDetAreaInfo.DetectorAreaLeft = g_rgPlateRecogParam[prmCur].g_rectDetectionArea.left;
		m_cDetAreaInfo.DetectorAreaRight = g_rgPlateRecogParam[prmCur].g_rectDetectionArea.right;
		m_cDetAreaInfo.DetectorAreaTop = g_rgPlateRecogParam[prmCur].g_rectDetectionArea.top;
		m_cDetAreaInfo.DetectorAreaBottom = g_rgPlateRecogParam[prmCur].g_rectDetectionArea.bottom;
		m_cDetAreaInfo.nDetectorMinScaleNum = g_rgPlateRecogParam[prmCur].g_nDetMinScaleNum;
		m_cDetAreaInfo.nDetectorMaxScaleNum = g_rgPlateRecogParam[prmCur].g_nDetMaxScaleNum;
	}

	m_fEnableScaleSpeed = FALSE;
	m_fOutputCarSize = FALSE;

	// 是否输出调试信息
	m_fOutputDebugInfo = 0;

	// 是否判断广西警标志
	m_iRecogGxPolice = 1;

	//新军牌识别开关
	m_iRecogNewMilitary = 1;

	//第5位是否开字母识别
	m_fEnableAlpha_5 = FALSE;

	// 平均得分下限
	m_iAverageConfidenceQuan = 0;

	// 汉字得分下限
	m_iFirstConfidenceQuan = 0;

	// 是否限制相同的车牌不能出两次
	m_iSendTwinResult = 1;

	// 限制相同车牌出现的时间间隔(秒)
	m_iBlockTwinsTimeout = 30;

	//事件检测参数
	CTrackInfo::m_ActionDetectParam.iDetectCarStopTime = 10;
	for (int i = 0; i < MAX_ROADLINE_NUM; i++)
	{
		CTrackInfo::m_ActionDetectParam.iIsCrossLine[i] = 0;
		CTrackInfo::m_ActionDetectParam.iIsYellowLine[i] = 0;
	}

	// 本地化汉字，58为“桂”
	m_nDefaultChnId = 58;

	//双层牌
	m_iEnableDefaultDBType = 0;
	m_nDefaultDBType = 0;
	m_iMiddleWidth = 0;
	m_iBottomWidth = 0;

	//车牌检测方差控制
	m_fMinVarianceUserCtrl = FALSE;
	m_iMinVariance = 9;

	m_iSpan = 5;

	m_fEnableNewWJSegment = TRUE;
	m_fEnableDefaultWJ    = TRUE;
	m_fEnableBigPlate = FALSE;
	HV_memset(m_rgiFiveCharWidth, 0, sizeof(m_rgiFiveCharWidth));
	HV_memset( m_strDefaultWJChar, 0, 3 );
	HV_memcpy( m_strDefaultWJChar, "16", 2);

	m_fEnablePlateEnhance = TRUE;
	m_iPlateResizeThreshold = 120;

	//白天黑夜视频亮度阈值
	m_iNightThreshold = 55;
	//默认值全改为5
	m_iPlateLightCheckCount = 5;
	//车牌最低亮度（低于此亮度会切换摄像机模式）
	m_iMinPlateBrightness = 80;
	//车牌最高亮度（高于此亮度会切换摄像机模式)
	m_iMaxPlateBrightness = 120;
	//控制偏光镜
	m_iCtrlCpl = 0;
	m_iCplStatus = -1;
	m_iAdjustForAvgBrigtness = 0;
	m_dwAdjustTick = GetSystemTick();
	m_nLightTypeCpl = LIGHT_TYPE((int)LIGHT_TYPE_COUNT / 2);

	//车辆到达触发位置
	m_iCarArrivedPos = 50;
	m_iCarArrivedPosYellow = 40;
	m_iCarArrivedDelay = 0;

	memset(m_rgRedLightRect, 0, sizeof(m_rgRedLightRect));

	if(GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE)
	{
		m_nVideoDetMode = 2;
	}
	else
	{
		m_nVideoDetMode = 0;
	}
	m_rcVideoDetArea.left = VIDEO_DETECT_LEFT;
	m_rcVideoDetArea.right = VIDEO_DETECT_RIGHT;
	m_rcVideoDetArea.top = VIDEO_DETECT_TOP;
	m_rcVideoDetArea.bottom = VIDEO_DETECT_BOTTOM;

	//灯组数量
	m_nLightCount = 0;
	m_iLastLightStatus = -1;
	m_iCurLightStatus = -1;
	m_iLastOkLightStatus = -1;
	m_nRedLightCount = 0;
	//允许输出的最小相同场景状态数
	m_nValidSceneStatusCount = 5;
	m_fCheckSpeed = false;
	m_iSceneCount = 0;
	m_nErrorSceneCount = 0;

	m_iBigCarDelayFrameNo = 32;

	// 投票数
	CTrackInfo::m_nVoteFrameNum = 35;
	// 连续结果相同, Vote
	CTrackInfo::m_nMaxEqualFrameNumForVote = 15;
	// 是否输出投票满足条件
	CTrackInfo::m_fOutputVoteCondition = 0;

	//投票时，去除得分最低的车牌
	CTrackInfo::m_nRemoveLowConfForVote = 40;

	// 识别得分低于阈值(百分比)，用*表示
	CTrackInfo::m_nRecogAsteriskThreshold = 0;

	CTrackInfo::m_iCarArrivedPos = 50;

	if( GetCurrentParam()->g_MovingDirection == MD_TOP2BOTTOM  )
	{
		CTrackInfo::m_iCaptureOnePos  = 45;
		CTrackInfo::m_iCaptureTwoPos  = 60;
		CTrackInfo::m_iCaptureThreePos = 80;
		m_fEnableProcessBWPlate = FALSE;
	}
	else if( GetCurrentParam()->g_MovingDirection == MD_BOTTOM2TOP )
	{
		CTrackInfo::m_iCaptureOnePos  = 80;
		CTrackInfo::m_iCaptureTwoPos  = 60;
		CTrackInfo::m_iCaptureThreePos = 45;
		m_fEnableProcessBWPlate = TRUE;
	}
	//红绿灯相关
	CTrackInfo::m_iStopLinePos      = 65;
	CTrackInfo::m_iLeftStopLinePos = 65;
	CTrackInfo::m_iCheckType = 0;
	CTrackInfo::m_iAutoScanLight = 0;
	CTrackInfo::m_iSceneCheckMode = 0;
	CTrackInfo::m_iRedLightDelay = 0;

	CTrackInfo::m_iFlashlightMode = 0;
	CTrackInfo::m_iFlashlightThreshold = 25;
	CTrackInfo::m_iRealForward = 40;

#if RUN_PLATFORM == PLATFORM_WINDOWS
	CTrackInfo::m_iMatchScanSpeed = 1;
#else
	CTrackInfo::m_iMatchScanSpeed = 0;
#endif

	CTrackInfo::m_iSpecificLeft		= 0;
	CTrackInfo::m_iRunTypeEnable	= 0;

	CTrackInfo::m_iBestLightMode = 0;
	CTrackInfo::m_iBestLightWaitTime = 250;

	CObjectDetector::m_iDayBackgroudThreshold = 20;	//白天背景检测阈值
	CObjectDetector::m_iNightBackgroudThreshold = 50;	//晚上背景检测阈值

	for(int i = 0; i < MAX_IO_LIGHT_COUNT; ++i)
	{
		m_rgIOLight[i].iPos = -1;
		m_rgIOLight[i].iTeam = -1;
		m_rgIOLight[i].tlsStatus = TLS_UNSURE;
		m_rgIOLight[i].tpPos = TP_UNSURE;
	}

	for(int i = 0; i < 4; ++i)
	{
		m_redrealtime.Reset();
		m_greentick.Reset();
	}

	m_iOutputInLine = 1;

	m_fltOverLineSensitivity = 2.75f;

	m_iOutPutType = 0;
	m_iWalkManSpeed = 100;
	m_fltBikeSensitivity = 4.0;

	m_iCheckAcross = 0;
	m_iVoteFrameCount = 30;

	m_iFilterRushPeccancy = 0;

	m_fLastResultInited = false;

	m_iCarArrivedPosNoPlate = 50;

	CTrackInfo::m_iTrafficLightThread = 3;
	CTrackInfo::m_iTrafficLightBrightValue = 3;
	CTrackInfo::m_iAutoScanThread = 8;
	m_fFilterMoreReview = FALSE;

	m_fUseEdgeMethod = FALSE;

	// zhaopy
	m_dwTriggerCameraTimes = 0;
	m_dwLastTriggerTick = 0;
	if(GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
	{
		m_fEnableCarArriveTrigger = TRUE;
	}
	else
	{
		m_fEnableCarArriveTrigger = FALSE;
	}
	m_iCarArriveTrig = 0;
	m_iTriggerPlateType = 0;
	m_fltHMThreld = 0.48;       //白天阈值
	m_fltHMThreldNight = 0.48;  //夜景阈值
	//模板匹配直方图延迟更新帧数
	m_HMDelay = 8;             //白天延迟帧数
	m_HMDelayNight = 8;        //夜景延迟帧数
	m_iMaxMatchDiff = 18;		// 连续最大相差阈值

	m_fFilterNoPlatePeccancy = FALSE;
	//车身颜色识别控制
	m_nEnableColorReRecog = 0;
	m_nWGColorReThr =  60;
	m_nWColorReThr = 150;
	m_nBColorReThr = 20;
	m_fEnableCorlorThr = false;
	m_nBGColorThr = 25;
	m_nWGColorThr = 150;
	
	// 是否使用补光灯
	m_iUsedLight = 1;
	
	SetSVCallBack();
	return S_OK;
}

//检测区域设错会导致识别器无法连接,所以匦朐黾有效性处?
#define TRIM_PARAM(v, min, max) ( v = MIN_INT(MAX_INT((min), v), (max)) )


//为HvCore实现的动态修改参数接口
HRESULT CTrackerImpl::ModifyTracker(TRACKER_CFG_PARAM* pCfgParam)
{
	if (pCfgParam == NULL)
	{
		return E_INVALIDARG;
	}

	m_pParam = GetCurrentParam();

	//恢复初始状态
	// 如果已经初始化了就不再做此操作。
	if( !m_fInitialized )
	{
		OnInit();
	}
	//道路信息参数
	CTrackInfo::m_iRoadNumber = pCfgParam->nRoadLineNumber;
	CTrackInfo::m_iStartRoadNum = pCfgParam->iStartRoadNum;
	CTrackInfo::m_iRoadNumberBegin = pCfgParam->iRoadNumberBegin;
	for(int i = 0; i < CTrackInfo::s_iMaxRoadNumber; i++)
	{
		CTrackInfo::m_roadInfo[i] = pCfgParam->rgcRoadInfo[i];
		CTrackInfo::m_roadInfo[i].ptBottom.y /=2;
		CTrackInfo::m_roadInfo[i].ptTop.y /=2;
	}

	// ganzz 02-19
	CObjectDetector::m_iRoadNumber = CTrackInfo::m_iRoadNumber;

	//三张抓拍图位置
	CTrackInfo::m_iCaptureOnePos = pCfgParam->nCaptureOnePos;
	CTrackInfo::m_iCaptureTwoPos = pCfgParam->nCaptureTwoPos;
	CTrackInfo::m_iCaptureThreePos = pCfgParam->nCaptureThreePos;

	//检测逆行开关
	m_pParam->m_iDetectReverseEnable = pCfgParam->nDetReverseRunEnable;
	m_iSpan = pCfgParam->nSpan;

	//指定行驶方向
	m_TravelDirection = (MovingDirection)pCfgParam->iMovingDirection;

	//收费站模式下进行抓拍
	m_iCarArriveTrig = pCfgParam->nCarArriveTrig;
	if(pCfgParam->nCarArriveTrig == 1)
	{
		m_fEnableCarArriveTrigger = true;
	}
	else if(pCfgParam->nCarArriveTrig == 0)
	{
		m_fEnableCarArriveTrigger = false;
	}
	//触发抓拍的车牌类型,黄国超增加
	m_iTriggerPlateType = pCfgParam->nTriggerPlateType;

	//压线灵敏度参数
	m_fltOverLineSensitivity = pCfgParam->fltOverLineSensitivity;

	//事件检测参数
	if (m_pParam->m_iDetectCrossLineEnable)
	{
		for (int i = 0; i < MAX_ROADLINE_NUM; i++)
		{
			CTrackInfo::m_ActionDetectParam.iIsCrossLine[i] = pCfgParam->cActionDetect.iIsCrossLine[i];
		}
	}
	if (m_pParam->m_iDetectOverYellowLineEnable)
	{
		for (int i = 0; i < MAX_ROADLINE_NUM; i++)
		{
			CTrackInfo::m_ActionDetectParam.iIsYellowLine[i] = pCfgParam->cActionDetect.iIsYellowLine[i];
		}
	}
	CTrackInfo::m_ActionDetectParam.iDetectCarStopTime = pCfgParam->cActionDetect.iDetectCarStopTime;

	//扫描区域参数
	m_cDetAreaInfo = pCfgParam->cDetectArea;

	//重识别扫描区域参数
	m_cRecogSnapArea = pCfgParam->cRecogSnapArea;

	TRAFFIC_LIGHT_PARAM tlpTrafficLightParam;

	//红绿频参数
	if(GetCurrentMode() == PRM_ELECTRONIC_POLICE)
	{
		CObjectDetector::m_iEPOutPutDetectLine = pCfgParam->cVideoDet.iEPOutPutDetectLine;
		CObjectDetector::m_iEPFGDetectLine = pCfgParam->cVideoDet.iEPFGDetectLine;
		CTrackInfo::m_iCheckType = pCfgParam->cTrafficLight.nCheckType;
		tlpTrafficLightParam.iCheckType = pCfgParam->cTrafficLight.nCheckType;
		CTrackInfo::m_iAutoScanLight = pCfgParam->cTrafficLight.nAutoScanLight;
		tlpTrafficLightParam.iAutoScanLight = pCfgParam->cTrafficLight.nAutoScanLight;
		tlpTrafficLightParam.fDeleteLightEdge = pCfgParam->cTrafficLight.fDeleteLightEdge;
		CTrackInfo::m_iSceneCheckMode = pCfgParam->cTrafficLight.nSenceCheckMode;
		tlpTrafficLightParam.iSceneCheckMode = pCfgParam->cTrafficLight.nSenceCheckMode;
		CTrackInfo::m_iStopLinePos = pCfgParam->cTrafficLight.nStopLinePos;
		CTrackInfo::m_iLeftStopLinePos = pCfgParam->cTrafficLight.nLeftStopLinePos;
		CTrackInfo::m_iMatchScanSpeed = pCfgParam->cTrafficLight.nMatchScanSpeed;
		CTrackInfo::m_iFlashlightMode = pCfgParam->fEnableFlashLight;
		CTrackInfo::m_iFlashlightThreshold = pCfgParam->nFlashLightThreshold;
		// zhaopy
		CTrackInfo::m_iRealForward = pCfgParam->cTrafficLight.iRealForward;
		m_fltHMThreld = (float)((float)pCfgParam->cTrafficLight.fltHMThreld / 100.0);
		m_fltHMThreldNight = (float)((float)pCfgParam->cTrafficLight.fltHMThreldNight / 100.0);
		m_HMDelay = pCfgParam->cTrafficLight.iHMDelay;
		m_HMDelayNight = pCfgParam->cTrafficLight.iHMDelayNight;
		m_iMaxMatchDiff = pCfgParam->cTrafficLight.iMaxMatchDiff;
		m_nValidSceneStatusCount = pCfgParam->cTrafficLight.nValidSceneStatusCount;
		if(CTrackInfo::m_iCheckType == 1)
		{
			m_nValidSceneStatusCount = 1;
		}
		tlpTrafficLightParam.iValidSceneStatusCount = m_nValidSceneStatusCount;
		//如果设置的有效场景小于5，则使用快速转换模式，快速转换模式下场景变化一定是顺序的。
		if( m_nValidSceneStatusCount < 5 )
		{
			m_fCheckSpeed = true;
			tlpTrafficLightParam.fCheckSpeed = m_fCheckSpeed;
		}
		m_iOutputInLine = pCfgParam->cTrafficLight.nOutputInLine;
		CTrackInfo::m_iRedLightDelay = pCfgParam->cTrafficLight.nRedLightDelay;
		tlpTrafficLightParam.iRedLightDelay = pCfgParam->cTrafficLight.nRedLightDelay;
		CTrackInfo::m_iSpecificLeft = pCfgParam->cTrafficLight.nSpecificLeft;
		CTrackInfo::m_iRunTypeEnable = pCfgParam->cTrafficLight.nRunTypeEnable;
		m_nLightCount = pCfgParam->cTrafficLight.nLightCount;
		tlpTrafficLightParam.iLightGroupCount = m_nLightCount;
		CTrackInfo::m_iTrafficLightThread = pCfgParam->cTrafficLight.nTrafficLightThread;
		CTrackInfo::m_iTrafficLightBrightValue = pCfgParam->cTrafficLight.nTrafficLightBrightValue;
		tlpTrafficLightParam.iTrafficLightBrightValue = pCfgParam->cTrafficLight.nTrafficLightBrightValue;
		CTrackInfo::m_iAutoScanThread = pCfgParam->cTrafficLight.nAutoScanThread;
		tlpTrafficLightParam.iAutoScanThread = pCfgParam->cTrafficLight.nAutoScanThread;
		tlpTrafficLightParam.iSkipFrameNo = pCfgParam->cTrafficLight.nSkipFrameNO;
		tlpTrafficLightParam.fIsLightAdhesion = pCfgParam->cTrafficLight.fIsLightAdhesion;

		m_iMinPlateBrightness = pCfgParam->nMinPlateBrightness;
		m_iMaxPlateBrightness = pCfgParam->nMaxPlateBrightness;

		m_iFilterRushPeccancy = pCfgParam->cTrafficLight.nFilterRushPeccancy;
		m_fFilterMoreReview = pCfgParam->cTrafficLight.nFilterMoreReview;

		// 模板开始的线。
		CTrackInfo::m_nTrafficLightTempleYPos = CTrackInfo::m_iStopLinePos + 16;
		if( CTrackInfo::m_nTrafficLightTempleYPos > 85 ) CTrackInfo::m_nTrafficLightTempleYPos = 85;

		int p1(0),p2(0),p3(0),p4(0),p5(0),p6(0);
		char szLightState[32] = {0};
		
		//重置红绿灯状态
		for(int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
		{
			m_rgLightInfo[i].Reset();
		}

		//ClearTrafficLightType();

		for(int i = 0; i < m_nLightCount; i++)
		{
			sscanf(
				pCfgParam->cTrafficLight.rgszLightPos[i], 
				"(%d,%d,%d,%d),%d,%d,%s",&p1,&p2,&p3,&p4,&p5,&p6,
				szLightState
			);
			//坐标,ID,灯数,类型(0=横向,1=竖向)
			m_rgLightInfo[i].SetRect(p1,p2,p3,p4,MIN_INT(CTrafficLightInfo::MAX_POS_COUNT, p5), p6);
			
			CRect rcTmp = CRect(p1,p2,p3,p4);
			tlpTrafficLightParam.rgtgiLight[i].iLightCount = MIN_INT(CTrafficLightInfo::MAX_POS_COUNT, p5);
			tlpTrafficLightParam.rgtgiLight[i].iLightType = p6;
			tlpTrafficLightParam.rgtgiLight[i].rcLight = rcTmp;

			if( strlen(szLightState) >= (p5 * 2) )
			{
				int index = 0;
				for(int k = 0; k < p5; ++k)
				{
					tlpTrafficLightParam.rgtgiLight[i].tpLightPos[k] = (TRAFFICLIGHT_POSITION)(szLightState[index] - '0');
					tlpTrafficLightParam.rgtgiLight[i].lsLightStatus[k] = (_TRAFFICLIGHT_LIGHT_STATUS)(szLightState[index+1] - '0');						

					if( (_TRAFFICLIGHT_LIGHT_STATUS)(szLightState[index+1] - '0') == TLS_RED 
						&& (m_nRedLightCount < MAX_TRAFFICLIGHT_COUNT * 2) )
					{
						if( p6 == 0 )
						{
							int iTmpX = ((p3 - p1) / p5);
							m_rgRedLightRect[m_nRedLightCount].left = iTmpX * k + p1;
							m_rgRedLightRect[m_nRedLightCount].right = iTmpX * (k + 1) + p1;
							m_rgRedLightRect[m_nRedLightCount].top = p2;
							m_rgRedLightRect[m_nRedLightCount].bottom = p4;
							m_nRedLightCount++;
						}
						else if( p6 == 1)
						{
							int iTmpY = ((p4 - p2) / p5);
							m_rgRedLightRect[m_nRedLightCount].left = p1;
							m_rgRedLightRect[m_nRedLightCount].right = p3;
							m_rgRedLightRect[m_nRedLightCount].top = iTmpY * k + p2;
							m_rgRedLightRect[m_nRedLightCount].bottom = iTmpY * (k + 1) + p2;
							m_nRedLightCount++;
						}
					}
					//SetTrafficLightType(i, k, (TRAFFICLIGHT_POSITION)(szLightState[index] - '0'), (_TRAFFICLIGHT_LIGHT_STATUS)(szLightState[index+1] - '0'));
					index += 2;
				}
			}
		}

		//外接IO红绿灯设置
		char szIOLight[64] = {0};
		strcpy(szIOLight, pCfgParam->cTrafficLight.szIOConfig);
		if( strcmp(szIOLight, "00,00,00,00,00,00,00,00") != 0 )
		{
			int iIndex = (strlen(szIOLight) + 1) / 3;
			for(int i = 0; i < iIndex; ++i)
			{
				tlpTrafficLightParam.rgIOLight[i].iTeam = szIOLight[i * 3 + 0] - '0' - 1;
				tlpTrafficLightParam.rgIOLight[i].iPos = szIOLight[i * 3 + 1] - '0' - 1;
				tlpTrafficLightParam.rgIOLight[i].tpPos = tlpTrafficLightParam.rgtgiLight[tlpTrafficLightParam.rgIOLight[i].iTeam].tpLightPos[tlpTrafficLightParam.rgIOLight[i].iPos];
				tlpTrafficLightParam.rgIOLight[i].tlsStatus = tlpTrafficLightParam.rgtgiLight[tlpTrafficLightParam.rgIOLight[i].iTeam].lsLightStatus[tlpTrafficLightParam.rgIOLight[i].iPos]; 

				m_rgIOLight[i].iTeam = szIOLight[i * 3 + 0] - '0' - 1;
				m_rgIOLight[i].iPos = szIOLight[i * 3 + 1] - '0' - 1;

				//GetTrafficLightType(m_rgIOLight[i].iTeam, m_rgIOLight[i].iPos, &m_rgIOLight[i]);
			}
		}

		m_iSceneCount = pCfgParam->cTrafficLight.nSceneCount;
		tlpTrafficLightParam.iSceneCount = m_iSceneCount;

		char szOtherInfo[64] = {0};
		int rgiLightStatus[4] = {0};

		//ClearLightScene();
		//ClearSceneInfo();
		for(int i = 0; i < MAX_SCENE_COUNT; i++)
		{
			memset(szOtherInfo, 0, 64);
			int nCount = sscanf(
				pCfgParam->cTrafficLight.rgszScene[i], 
				"(%d,%d,%d,%d),%s",
				&rgiLightStatus[0],
				&rgiLightStatus[1],
				&rgiLightStatus[2],
				&rgiLightStatus[3],
				szOtherInfo
			);
			if( 4 <= nCount)
			{
				TRAFFICLIGHT_SCENE ts;
				ts.lsLeft = (LIGHT_STATUS)rgiLightStatus[0];
				ts.lsForward = (LIGHT_STATUS)rgiLightStatus[1];
				ts.lsRight = (LIGHT_STATUS)rgiLightStatus[2];
				ts.lsTurn = (LIGHT_STATUS)rgiLightStatus[3];
		  //	SetLightScene(i, ts);
				SCENE_INFO tsInfo;
				strncpy(tsInfo.pszInfo, szOtherInfo, strlen(szOtherInfo) + 1);
			//	SetSceneInfo(i, tsInfo);

				tlpTrafficLightParam.rgtsLightScene[i] = ts;
				memcpy(tlpTrafficLightParam.rgsiInfo[i].pszInfo, tsInfo.pszInfo, 64);
			}

			if (m_nLightCount > 0)
			{
				m_cTrafficLight.SetTrafficLightParam(tlpTrafficLightParam);
				m_cTrafficLight.SetTrafficLightCallback(this);
			}
		}
	}

	//视频检测参数
	m_nVideoDetMode = pCfgParam->cVideoDet.nVideoDetMode;
	m_rcVideoDetArea = pCfgParam->cVideoDet.rcVideoDetArea;
	CObjectDetector::m_iDayBackgroudThreshold = pCfgParam->cVideoDet.nDayBackgroudThreshold;
	CObjectDetector::m_iNightBackgroudThreshold = pCfgParam->cVideoDet.nNightBackgroudThreshold;
	m_iVoteFrameCount = pCfgParam->cVideoDet.nVoteFrameCount;
	m_iCheckAcross = pCfgParam->cVideoDet.nCheckAcross;
	CObjectDetector::m_iShadowDetect = pCfgParam->cVideoDet.nShadowDetMode;
	CObjectDetector::m_iShadowEffect = pCfgParam->cVideoDet.nShadowDetEffect;
	CObjectDetector::m_iDiffType = pCfgParam->cVideoDet.nDiffType;
	CObjectDetector::m_iMedFilter = pCfgParam->cVideoDet.nMedFilter;
	CObjectDetector::m_iMedFilterUpLine = pCfgParam->cVideoDet.nMedFilterUpLine;
	CObjectDetector::m_iMedFilterDownLine = pCfgParam->cVideoDet.nMedFilterDownLine;
	m_iBigCarDelayFrameNo = pCfgParam->cVideoDet.nBigCarDelayFrameNo;

	if (m_cDetAreaInfo.fEnableDetAreaCtrl)
	{
		m_pParam->g_rectDetectionArea.left = m_cDetAreaInfo.DetectorAreaLeft;
		m_pParam->g_rectDetectionArea.right = m_cDetAreaInfo.DetectorAreaRight;
		m_pParam->g_rectDetectionArea.top = m_cDetAreaInfo.DetectorAreaTop;
		m_pParam->g_rectDetectionArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;
		m_pParam->g_nDetMinScaleNum = m_cDetAreaInfo.nDetectorMinScaleNum;
		m_pParam->g_nDetMaxScaleNum = m_cDetAreaInfo.nDetectorMaxScaleNum;
	}

	if (m_fMinVarianceUserCtrl)
	{
		m_pParam->g_nDetMinStdVar = m_iMinVariance;
	}
	else
	{
		m_iMinVariance = m_pParam->g_nDetMinStdVar;
	}
	m_iNoPlateDetMode = pCfgParam->cVideoDet.iNoPlateDetMode;
	CTrackInfo::m_fEPUseCarTailModel = pCfgParam->cVideoDet.iEPUseCarTailModel;
	CTrackInfo::m_iEPNoPlateScoreThreld = pCfgParam->cVideoDet.iEPNoPlateScoreThreld;

	PROCESS_PLATE_CFG_PARAM cfgProcessPlateParam;
	RECOGNITION_CFG_PARAM cfgRecognitionParam;
	PLATE_DETECT_CFG_PARAM cfgPlateDetectParam;
	SCALESPEED_CFG_PARAM cfgScaleSpeedParam;

	//ProcessPlate
	memcpy(&m_cfgParam, pCfgParam, sizeof(m_cfgParam));
	cfgProcessPlateParam.nBlackPlate_S = pCfgParam->nProcessPlate_BlackPlate_S;
	cfgProcessPlateParam.nBlackPlate_L = pCfgParam->nProcessPlate_BlackPlate_L;
	cfgProcessPlateParam.nBlackPlateThreshold_H0 = pCfgParam->nProcessPlate_BlackPlateThreshold_H0;
	cfgProcessPlateParam.nBlackPlateThreshold_H1 = pCfgParam->nProcessPlate_BlackPlateThreshold_H1;
	cfgProcessPlateParam.nProcessPlate_LightBlue = pCfgParam->nProcessPlate_LightBlue;
	cfgProcessPlateParam.fEnableDoublePlate = pCfgParam->fDoublePlateEnable;
	cfgProcessPlateParam.iSegWhitePlate = pCfgParam->nSegWhitePlate; //白牌分割
    cfgProcessPlateParam.fltMinWHRatioCoef = 0.5f; //支持倾斜牌

	//Recognition
	cfgRecognitionParam.fEnableAlphaRecog = pCfgParam->fAlpha7;
	cfgRecognitionParam.fEnableT1Model = pCfgParam->fEnableT1Model;
	cfgRecognitionParam.fEnableAlpha_5 = pCfgParam->fEnableAlpha_5;

	//Detector
	cfgPlateDetectParam.nPlateDetect_Green = pCfgParam->nPlateDetect_Green;

	m_iNightThreshold = pCfgParam->nNightThreshold;
	m_iPlateLightCheckCount = pCfgParam->nPlateLightCheckCount;
	m_iMinPlateBrightness = pCfgParam->nMinPlateBrightness;
	m_iMaxPlateBrightness = pCfgParam->nMaxPlateBrightness;
	m_iCtrlCpl = pCfgParam->nCtrlCpl;
	if(m_iCtrlCpl == 1)
	m_iCplStatus = 0;
	m_nLightTypeCpl = (LIGHT_TYPE)pCfgParam->nLightTypeCpl;
	m_iForceLightOffAtDay = pCfgParam->iForceLightOffAtDay;
	m_iForceLightOffThreshold = pCfgParam->iForceLightThreshold;
	m_iForceLightOffFlag = 0;
	// 是否使用补光灯
	m_iUsedLight = pCfgParam->iUsedLight;
	m_fEnablePlateEnhance = pCfgParam->nEnablePlateEnhance;
	m_iPlateResizeThreshold = pCfgParam->nPlateResizeThreshold;
	m_fEnableBigPlate = pCfgParam->nEnableBigPlate;
	CTrackInfo::m_nRecogAsteriskThreshold = pCfgParam->nRecogAsteriskThreshold;
	m_fUseEdgeMethod = pCfgParam->fUseEdgeMethod;
	CTrackInfo::m_fEnableRecgCarColor = pCfgParam->fEnableRecgCarColor;
	m_fEnableDefaultWJ = pCfgParam->fEnableDefaultWJ;
	strncpy(m_strDefaultWJChar, pCfgParam->szDefaultWJChar, 3);
	m_fEnableProcessBWPlate = pCfgParam->nEnableProcessBWPlate;
	m_iRecogGxPolice = pCfgParam->nRecogGxPolice;
	m_iRecogNewMilitary = pCfgParam->nRecogNewMilitary;
	m_fEnableAlpha_5 = pCfgParam->fEnableAlpha_5;
	m_iCarArrivedPos = pCfgParam->nCarArrivedPos;
	CTrackInfo::m_iCarArrivedPos = m_iCarArrivedPos;
	m_iCarArrivedPosNoPlate = pCfgParam->nCarArrivedPosNoPlate;
	CObjectDetector::m_iProcessType = pCfgParam->nProcessType;
	m_iOutPutType = pCfgParam->nOutPutType;
	CObjectDetector::m_iNightPlus = pCfgParam->nNightPlus;
	m_iWalkManSpeed = pCfgParam->nWalkManSpeed;
	m_fltBikeSensitivity = pCfgParam->fltBikeSensitivity;
	m_iCarArrivedDelay = pCfgParam->nCarArrivedDelay;
	CTrackInfo::m_nVoteFrameNum = pCfgParam->nVoteFrameNum;
	CTrackInfo::m_nMaxEqualFrameNumForVote = pCfgParam->nMaxEqualFrameNumForVote;
	m_iBlockTwinsTimeout = pCfgParam->nBlockTwinsTimeout;
	m_iAverageConfidenceQuan = pCfgParam->nAverageConfidenceQuan;
	m_iFirstConfidenceQuan = pCfgParam->nFirstConfidenceQuan;
	CTrackInfo::m_nRemoveLowConfForVote = pCfgParam->nRemoveLowConfForVote;
	m_fOutputCarSize = pCfgParam->fOutputCarSize;
	m_iEnableDefaultDBType = pCfgParam->nDoublePlateTypeEnable;
	m_nDefaultDBType = pCfgParam->nDefaultDBType;
	m_iMiddleWidth = pCfgParam->nMiddleWidth;
	m_iBottomWidth = pCfgParam->nBottomWidth;
	m_fEnableDetFace = pCfgParam->fEnableDetFace;
	m_nMinFaceScale = pCfgParam->nMinFaceScale;
	m_nMaxFaceScale = pCfgParam->nMaxFaceScale;
	m_fFilterNoPlatePeccancy = pCfgParam->cTrafficLight.fFilterNoPlatePeccancy;

	//软件测速参数
	cfgScaleSpeedParam.nMethodsFlag = pCfgParam->cScaleSpeed.nMethodsFlag;
	cfgScaleSpeedParam.fltLeftDistance = pCfgParam->cScaleSpeed.fltLeftDistance;
	cfgScaleSpeedParam.fltFullDistance = pCfgParam->cScaleSpeed.fltFullDistance;
	cfgScaleSpeedParam.fltCameraHigh = pCfgParam->cScaleSpeed.fltCameraHigh;
	cfgScaleSpeedParam.fltAdjustCoef = pCfgParam->cScaleSpeed.fltAdjustCoef;
	m_fEnableScaleSpeed = pCfgParam->cScaleSpeed.fEnable;
	
	if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
	{
		m_iCarArrivedPosYellow = pCfgParam->nCarArrivedPos - pCfgParam->iBuleTriggerToYellowOffset > 0 ?
			pCfgParam->nCarArrivedPos - pCfgParam->iBuleTriggerToYellowOffset : 0;
	}
	else
	{
		m_iCarArrivedPosYellow = pCfgParam->nCarArrivedPosYellow;
	}

	//车身颜色识别控制
	m_nEnableColorReRecog = pCfgParam->nEnableColorReRecog;
	m_nWGColorReThr =  pCfgParam->nWGColorReThr;
	m_nWColorReThr = pCfgParam->nWColorReThr;
	m_nBColorReThr = pCfgParam->nBColorReThr;
	m_fEnableCorlorThr = pCfgParam->fEnableCorlorThr;
	m_nBGColorThr = pCfgParam->nBGColorThr;
	m_nWGColorThr = pCfgParam->nWGColorThr;

	for(int i = 0; i < 11; i++)
	{
		cfgScaleSpeedParam.rgfltTransMarix[i] = pCfgParam->cScaleSpeed.rgfltTransMarix[i];
	}

	if(GetCurrentMode() != PRM_HVC)
	{
		// 非抓拍模式
		if (!((m_fOutputCarSize == TRUE) && (m_nVideoDetMode == USE_VIDEODET)))
		{
			m_fOutputCarSize = FALSE;
		}
	}
	CFinalResultParam::m_iBlockTwinsTimeout = m_iBlockTwinsTimeout;
	CFinalResultParam::m_iSendTwinResult = m_iSendTwinResult;

	//各组件初始化
	// TODO magic number
	{
		RTN_HR_IF_FAILED(m_pPlateDetector->Initialize(m_pParam, &cfgPlateDetectParam, 0, 0));
		RTN_HR_IF_FAILED(m_pRecognition->InitRecognition(&cfgRecognitionParam));
		RTN_HR_IF_FAILED(m_pProcessPlate->Initialize(NO_PREDICT_PROCESS_PLATE_MODE, &cfgProcessPlateParam));
	}
	RTN_HR_IF_FAILED(m_pScaleSpeed->InitScaleSpeed(&cfgScaleSpeedParam));			// 测速接口初始化

	//配置组件属性
	for (int i=0; i<s_knMaxTrackInfo; i++)
	{
		m_rgTrackInfo[i].m_pParam=m_pParam;
	}

	// 卡口模式下由车道线自动计算扫描区域和视频检测区域
	if(GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
	{
		// 计算矩形区域
		CalcTrapDetArea(&m_cfgParam.cTrapArea,  CTrackInfo::m_iCaptureOnePos, m_nWidth, m_nHeight);
	}
	else if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
	{
		CalcTLTrapDetArea(
				&m_cfgParam.cTrapArea,
				m_iCarArrivedPos-pCfgParam->iScanAreaTopOffset > 0 ? m_iCarArrivedPos-pCfgParam->iScanAreaTopOffset : 0,
				m_iCarArrivedPos+pCfgParam->iScanAreaBottomOffset < 99 ? m_iCarArrivedPos+pCfgParam->iScanAreaBottomOffset : 99,
				m_nWidth,
				m_nHeight);
		
		// 根据矩形形区域计算检测区
		m_cDetAreaInfo.DetectorAreaLeft = MIN(m_cfgParam.cTrapArea.TopLeftX, m_cfgParam.cTrapArea.BottomLeftX);
		m_cDetAreaInfo.DetectorAreaTop = MIN(m_cfgParam.cTrapArea.TopLeftY, m_cfgParam.cTrapArea.BottomLeftY);
		m_cDetAreaInfo.DetectorAreaRight = MAX(m_cfgParam.cTrapArea.TopRightX, m_cfgParam.cTrapArea.BottomRightX);
		m_cDetAreaInfo.DetectorAreaBottom = MAX(m_cfgParam.cTrapArea.TopRightY, m_cfgParam.cTrapArea.BottomRightY);

		m_pParam->g_rectDetectionArea.left = m_cDetAreaInfo.DetectorAreaLeft;
		m_pParam->g_rectDetectionArea.right = m_cDetAreaInfo.DetectorAreaRight;
		m_pParam->g_rectDetectionArea.top = m_cDetAreaInfo.DetectorAreaTop;
		m_pParam->g_rectDetectionArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;

		m_rcVideoDetArea.left = m_cDetAreaInfo.DetectorAreaLeft;
		m_rcVideoDetArea.top = m_cDetAreaInfo.DetectorAreaTop;
		m_rcVideoDetArea.right = m_cDetAreaInfo.DetectorAreaRight;
		m_rcVideoDetArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;
	}

	m_fIsModifyParam = true;

	return S_OK;
}

//初始化TRACKER和各组件
HRESULT CTrackerImpl::InitTracker(TRACKER_CFG_PARAM* pCfgParam)
{
	m_fInitialized = FALSE;

	RTN_HR_IF_FAILED(ModifyTracker(pCfgParam));


	m_fInitialized = TRUE;

	return S_OK;
}

//设置模型加载接口
HRESULT CTrackerImpl::SetHvModel(HvCore::IHvModel* pHvModel)
{
	m_pHvModel = pHvModel;

	if (m_pPlateDetector) m_pPlateDetector->SetHvModel(m_pHvModel);
	if (m_pRecognition) m_pRecognition->SetHvModel(m_pHvModel);
	return S_OK;
}

HRESULT CTrackerImpl::SetHvPerformance(HvCore::IHvPerformance* pHvPerf)
{
	m_pHvPerf = pHvPerf;

	if (m_pPlateDetector) m_pPlateDetector->SetHvPerformance(pHvPerf);
	if (m_pRecognition) m_pRecognition->SetHvPerformance(pHvPerf);

	if (m_pProcessPlate) m_pProcessPlate->SetHvPerformance(pHvPerf);

	return S_OK;
}

//设置功能模块参数
HRESULT CTrackerImpl::SetLPRMode(PlateRecogMode nMode)
{
	HRESULT hr;

	DWORD32 dwParamCount(0);
	GetPlateRecogParamCount(&dwParamCount);

	if (nMode < 0 || nMode >= (int)dwParamCount)
	{
		hr = E_INVALIDARG;
	}
	else
	{
		LoadPlateRecogParam(nMode);
		m_pParam = GetCurrentParam();

		if (m_cDetAreaInfo.fEnableDetAreaCtrl)
		{
			m_pParam->g_rectDetectionArea.left = m_cDetAreaInfo.DetectorAreaLeft;
			m_pParam->g_rectDetectionArea.right = m_cDetAreaInfo.DetectorAreaRight;
			m_pParam->g_rectDetectionArea.top = m_cDetAreaInfo.DetectorAreaTop;
			m_pParam->g_rectDetectionArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;
			m_pParam->g_nDetMinScaleNum = m_cDetAreaInfo.nDetectorMinScaleNum;
			m_pParam->g_nDetMaxScaleNum = m_cDetAreaInfo.nDetectorMaxScaleNum;
		}

		if (m_fMinVarianceUserCtrl)
		{
			m_pParam->g_nDetMinStdVar = m_iMinVariance;
		}

//		if (m_pHvParam)
//		{
//			m_pHvParam->SetInt("Tracker", "PlateRecogMode", nMode);
//			m_pHvParam->Save(0);
//		}

		hr = S_OK;
	}

	return hr;
}

HRESULT CTrackerImpl::SetFrameProperty(
									   const FRAME_PROPERTY& FrameProperty,
									   PROCESS_ONE_FRAME_RESPOND* pProcessRespond
									   )
{
	ClearTrackInfo(pProcessRespond);

	m_nWidth = FrameProperty.iWidth;
	m_nHeight = FrameProperty.iHeight;
	m_nStride = FrameProperty.iStride;

	CTrackInfo::m_iHeight = m_nHeight;
    CTrackInfo::m_iWidth = m_nWidth;

	if (m_pPlateDetector)
	{
		m_pPlateDetector->SetImgSize(m_nWidth, m_nHeight);
	}
	if (m_fEnableScaleSpeed)
	{
		// 初始化测速数据
		m_pScaleSpeed->SetFrameProperty(FrameProperty);
	}

	// 车型判断参数设置
	SetCarTypeParam(m_nWidth, m_nHeight);

	// 卡口模式下由车道线自动计算扫描区域和视频检测区域
	if(GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
	{
		// 计算矩形区域
		CalcTrapDetArea(&m_cfgParam.cTrapArea,  CTrackInfo::m_iCaptureOnePos, m_nWidth, m_nHeight);
	}
	else if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
	{
		CalcTLTrapDetArea(
				&m_cfgParam.cTrapArea,
				m_iCarArrivedPos-m_cfgParam.iScanAreaTopOffset > 0 ? m_iCarArrivedPos-m_cfgParam.iScanAreaTopOffset : 0,
				m_iCarArrivedPos+m_cfgParam.iScanAreaBottomOffset < 99 ? m_iCarArrivedPos+m_cfgParam.iScanAreaBottomOffset : 99,
				m_nWidth,
				m_nHeight);

		CalcPlateScale(&m_cfgParam.cTrapArea, m_nWidth, &m_pParam->g_nDetMinScaleNum, &m_pParam->g_nDetMaxScaleNum);

		// 根据矩形形区域计算检测区
		m_cDetAreaInfo.DetectorAreaLeft = MIN(m_cfgParam.cTrapArea.TopLeftX, m_cfgParam.cTrapArea.BottomLeftX);
		m_cDetAreaInfo.DetectorAreaTop = MIN(m_cfgParam.cTrapArea.TopLeftY, m_cfgParam.cTrapArea.BottomLeftY);
		m_cDetAreaInfo.DetectorAreaRight = MAX(m_cfgParam.cTrapArea.TopRightX, m_cfgParam.cTrapArea.BottomRightX);
		m_cDetAreaInfo.DetectorAreaBottom = MAX(m_cfgParam.cTrapArea.TopRightY, m_cfgParam.cTrapArea.BottomRightY);

		m_pParam->g_rectDetectionArea.left = m_cDetAreaInfo.DetectorAreaLeft;
		m_pParam->g_rectDetectionArea.right = m_cDetAreaInfo.DetectorAreaRight;
		m_pParam->g_rectDetectionArea.top = m_cDetAreaInfo.DetectorAreaTop;
		m_pParam->g_rectDetectionArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;

		m_rcVideoDetArea.left = m_cDetAreaInfo.DetectorAreaLeft;
		m_rcVideoDetArea.top = m_cDetAreaInfo.DetectorAreaTop;
		m_rcVideoDetArea.right = m_cDetAreaInfo.DetectorAreaRight;
		m_rcVideoDetArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;
	}

	return S_OK;
}

HRESULT CTrackerImpl::SetCharSet(ChnCharSet nCharSet)
{
	return m_pRecognition->SetCharSet(nCharSet);
}

HRESULT CTrackerImpl::SetVideoDet(BOOL fEnabled, BOOL fSkipFrame , int nDetLeft, int nDetRight)
{
	return S_OK;
}

HRESULT CTrackerImpl::SetCalibrateParam(BOOL fEnable, int nRotate, int nTilt, BOOL fOutputImg)
{
	//	m_fEnablePreCalibrate = fEnable;
	//	m_fOutputCalibrateImg = fOutputImg;
	//	if (m_pHvParam)
	//	{
	//		m_pHvParam->SetInt("Tracker\\PreTreatment","Enable", m_fEnablePreCalibrate);
	//		m_pHvParam->SetInt("Tracker\\PreTreatment","OutputImg", m_fOutputCalibrateImg);
	//		m_pHvParam->Save(0);
	//	}

	//	if (m_pPretreatment)
	//	{
	//		m_pPretreatment->SetParam(nRotate,nTilt);
	//	}

	return S_OK;
}

HRESULT CTrackerImpl::SetPreSegmentParam(BOOL fEnable)
{
	m_fEnablePreciseRectify = fEnable;
	return S_OK;
}

HRESULT CTrackerImpl::SetDetArea(	const DETECTAREA& cDetAreaInfo)
{
	m_cDetAreaInfo = cDetAreaInfo;

	if (m_cDetAreaInfo.fEnableDetAreaCtrl)
	{
		m_pParam->g_rectDetectionArea.left = m_cDetAreaInfo.DetectorAreaLeft;
		m_pParam->g_rectDetectionArea.right = m_cDetAreaInfo.DetectorAreaRight;
		m_pParam->g_rectDetectionArea.top = m_cDetAreaInfo.DetectorAreaTop;
		m_pParam->g_rectDetectionArea.bottom = m_cDetAreaInfo.DetectorAreaBottom;
		m_pParam->g_nDetMinScaleNum = m_cDetAreaInfo.nDetectorMinScaleNum;
		m_pParam->g_nDetMaxScaleNum = m_cDetAreaInfo.nDetectorMaxScaleNum;
	}
	else
	{
//		LoadPlateRecogParam(GetWorkMode());
		m_pParam = GetCurrentParam();
	}

//	if (m_pHvParam)
//	{
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","Enable", m_cDetAreaInfo.fEnableDetAreaCtrl);
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","Left", m_cDetAreaInfo.DetectorAreaLeft);
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","Right", m_cDetAreaInfo.DetectorAreaRight);
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","Top", m_cDetAreaInfo.DetectorAreaTop);
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","Bottom", m_cDetAreaInfo.DetectorAreaBottom);
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","MinScale", m_cDetAreaInfo.nDetectorMinScaleNum);
//		m_pHvParam->SetInt("Tracker\\DetAreaCtrl","MaxScale", m_cDetAreaInfo.nDetectorMaxScaleNum);
//		m_pHvParam->Save(0);
//	}

	if (m_fMinVarianceUserCtrl)
	{
		m_pParam->g_nDetMinStdVar = m_iMinVariance;
	}

	return S_OK;
}

//TODO:	覃宇：扩大区域是否考虑移到检测模块中
HRESULT CTrackerImpl::GetInflateRect(
									 CRect &rect,			// in, out.
									 PLATE_COLOR color,
									 PLATE_TYPE &PlateType,
									 INT maxRectHeight		// 判断是否用inflate rate2
									 )
{
	int nWidth(rect.Width());
	int nHeight(rect.Height());

	if (	PLATE_DOUBLE_YELLOW == PlateType ||
		PLATE_DOUBLE_MOTO == PlateType )
	{
		rect.left -= (nWidth * 80) >> 10;
		rect.right += (nWidth * 80) >> 10;
		rect.top -= (nHeight * 120) >> 10;
		rect.bottom += (nHeight * 40) >> 10;
	}
	else if (PLATE_DOUBLE_GREEN == PlateType)
	{
		rect.right += (nWidth * 40) >> 10;
		rect.top += (nHeight * 130) >> 10;
		rect.bottom -= (nHeight * 20) >> 10;
	}
	else
	{
		rect.left -= (nWidth * 120) >> 10;
		rect.right += (nWidth * 120) >> 10;
	}

	nHeight = rect.Height();
	if (nHeight >= 100)		//100为分割时积分图最大高度
	{
		rect.top += (nHeight - 100) >> 1;
		rect.bottom -= nHeight - 100 - ((nHeight - 100) >> 1);
	}
	nWidth = rect.Width();
	if (nWidth >= 400)		//400为分割时积分图最大宽度
	{
		rect.left += (nWidth - 399) >> 1;
		rect.right -= nWidth - 399 - ((nWidth - 399) >> 1);
	}

	return S_OK;
}

HRESULT CTrackerImpl::RecognizePlate(
									 HV_COMPONENT_IMAGE imgPlate,
									 PlateInfo &Info,
									 CPersistentComponentImage& imgRectified
									 )
{
	HV_DebugInfo(DEBUG_STR_TRACK_FILE, "Enter RecognizePlate()\n");

	if (imgPlate.iHeight <= 0 || imgPlate.iWidth <= 0
        || imgPlate.iWidth * 1 < imgPlate.iHeight * 3 && imgPlate.iWidth < 120)  // 异常宽度车牌不处理, ganzz140708
	{
		return S_FALSE;
	}

	CPersistentComponentImage imgYUVPlate;
	if (imgPlate.nImgType != HV_IMAGE_YUV_422)
	{
		imgYUVPlate.Create(HV_IMAGE_YUV_422, imgPlate.iWidth, imgPlate.iHeight);
		imgYUVPlate.Convert(imgPlate);
		imgPlate = (HV_COMPONENT_IMAGE)imgYUVPlate;
	}
	HV_COMPONENT_IMAGE imgOrigPlate = imgPlate;
	CPersistentComponentImage imgBlackWhitePlate;
	if (m_fEnableProcessBWPlate && DAY == m_LightType)
	{
		imgBlackWhitePlate.Assign(imgPlate);
		ProcessBWPlate(&imgBlackWhitePlate);
		imgPlate = (HV_COMPONENT_IMAGE)imgBlackWhitePlate;
	}

	CPersistentComponentImage imgEnhancePlate;
	if (m_fEnablePlateEnhance)
	{
		imgEnhancePlate.Assign(imgPlate);
		PlateEnhance(&imgEnhancePlate);
		imgPlate = (HV_COMPONENT_IMAGE)imgEnhancePlate;
	}

	int iLastCharHeight=Info.iLastCharHeight;
	float fltPlateScaled = 1.0f;
	CPersistentComponentImage imgSample;
	/*
	int nStandarPlateWidth = m_iPlateResizeThreshold;
	if (imgPlate.iWidth > 250)
	{
	    nStandarPlateWidth = 250;	
	}

	if (imgPlate.iWidth < nStandarPlateWidth || imgPlate.iWidth > 250)
	{
		fltPlateScaled = imgPlate.iWidth / (float)nStandarPlateWidth;
		imgSample.Create(HV_IMAGE_YUV_422, nStandarPlateWidth, (int)(imgPlate.iHeight / fltPlateScaled));
		ImageScaleSize(imgSample, imgPlate);
		imgPlate = (HV_COMPONENT_IMAGE)imgSample;
		iLastCharHeight = (int)(iLastCharHeight / fltPlateScaled);
	}
	*/

	if (imgPlate.iWidth < m_iPlateResizeThreshold)
	{
		fltPlateScaled = imgPlate.iWidth / (float)m_iPlateResizeThreshold;
		imgSample.Create(HV_IMAGE_YUV_422, m_iPlateResizeThreshold, (int)(imgPlate.iHeight / fltPlateScaled));
		ImageScaleSize(imgSample, imgPlate);
		imgPlate = (HV_COMPONENT_IMAGE)imgSample;
		iLastCharHeight = (int)(iLastCharHeight / fltPlateScaled);
	}
	if (imgPlate.iHeight <= MIN_SEG_IMG_HIGH)
	{
		return S_FALSE;
	}

	if (m_pInspector)
	{
		RTN_HR_IF_FAILED(m_pInspector->ShowComponentImage(ipOriginalPlate, &imgPlate));
	}

	DWORD32 dwTickCount = GetSystemTick();

	RTN_HR_IF_FAILED(m_pProcessPlate->SetWorkMode(
		m_pParam->g_fSegPredict ? FORWARD_PREDICT_PROCESS_PLATE_MODE :NO_PREDICT_PROCESS_PLATE_MODE
		));

	CRectifyInfo RectifyInfo;
	CBinaryInfo BinaryInfo;

	RTN_HR_IF_FAILED(imgRectified.Assign(imgPlate));

	// 卡口车头 晚上 使用新的车牌矫正方法
	if (m_LightType == NIGHT 
		&& (GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE || GetCurrentParam()->g_PlateRcogMode == PRM_HIGHWAY_HEAD))
	{	
		RTN_HR_IF_FAILED(m_pProcessPlate->CalRectifyInfoKL(&imgPlate, &RectifyInfo, &(Info.nPlateType), &(Info.color)));
		if (RectifyInfo.fltHAngle > 4)
		{
			RectifyInfo.fltHAngle = 4;
		}
		else if (RectifyInfo.fltHAngle < -4)
		{
			RectifyInfo.fltHAngle = -4;
		}
//		RectifyInfo.fltVAngle = 90 + RectifyInfo.fltHAngle;		// 原垂直矫正有问题 垂直矫正参考水平矫正
		RectifyInfo.fltVAngle = (RectifyInfo.fltVAngle - 90) / 2 + 90;	// 垂直角度只要一半
	}
	else
	{
	RTN_HR_IF_FAILED(m_pProcessPlate->CalcRectifyInfo(&imgPlate,
		&imgRectified, &RectifyInfo, &BinaryInfo, &(Info.nPlateType), 
		&(Info.color), false, m_fEnableBigPlate));
	}

	// TODO: Why none constant RectifyInfo?
	// ASSUME: The rectified image won't be larger than the original one

	imgRectified.iHeight=imgPlate.iHeight;
	imgRectified.iWidth=imgPlate.iWidth;

	RTN_HR_IF_FAILED(m_pProcessPlate->RectifyPlate(
		&imgPlate, &RectifyInfo, &imgRectified, m_fEnableBigPlate
		));

	HV_DebugInfo(	DEBUG_STR_TRACK_FILE,
		"Rectify Info: HAngle=%f, VAngle=%f, RectifiedWidth=%d, RectifiedHeight=%d\n",
		RectifyInfo.fltHAngle, RectifyInfo.fltVAngle, imgRectified.iWidth, imgRectified.iHeight
		);


	HV_DebugInfo( DEBUG_STR_TIME_FILE,
		"Calc Rectify Time = %d\n",

		GetSystemTick()-dwTickCount
		);

	if (m_pInspector)
	{
		RTN_HR_IF_FAILED(m_pInspector->ShowComponentImage(ipRectified, &imgRectified));
	}

	int iMaxScore = 0;

	int iNormalFiveCharWidth(0);
	if (m_iEnableDefaultDBType > 0 &&
		m_pParam->g_MovingDirection == MD_BOTTOM2TOP)
	{
		// 车尾摩托模式
		int iOffset = (Info.rcPos.bottom
			+ Info.rcPos.top) / 50;
		iNormalFiveCharWidth = m_rgiFiveCharWidth[iOffset];
	}

	int iFastSegMode(2);

	const unsigned int knPlateStrLen = 8;		// TODO: Magic Number
	CRect rgrcCharPos[knPlateStrLen];
	HV_RECT *pCharPos=rgrcCharPos;
	//清晰牌出未检测新增，卡口车头和卡口人脸抓拍模式下，第二个参数为TRUE
	//使用新增方法，为false使用旧方法
	//刘杰修改
	if((GetCurrentParam()->g_PlateRcogMode == PRM_HIGHWAY_HEAD ||GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
		&& m_fUseEdgeMethod )
	{
		m_pProcessPlate->SetSrcImgGray(m_iAvgY, true);
	}
	
	if(Info.nPlateType == PLATE_DOUBLE_YELLOW &&
		MD_BOTTOM2TOP != m_pParam->g_MovingDirection)
	{
		iNormalFiveCharWidth = 1; //非车尾模式不判断摩托类型
	}

	RTN_HR_IF_FAILED(m_pProcessPlate->SegmentPlate(
		&imgRectified, &(Info.color), &(Info.nPlateType),
		&iMaxScore, &iLastCharHeight, &(Info.iLastVscale),
		&BinaryInfo, &pCharPos, iFastSegMode,
		(m_LightType == NIGHT),
		iNormalFiveCharWidth
		));


	if ((Info.nPlateType == PLATE_DOUBLE_MOTO)
		&& (!(m_iEnableDefaultDBType > 0 && MD_BOTTOM2TOP == m_pParam->g_MovingDirection)))
	{
		// 非摩托车尾模式, 没有摩托车牌
		Info.nPlateType = PLATE_DOUBLE_YELLOW;
	}

	Info.iLastCharHeight = (int)(iLastCharHeight * fltPlateScaled);

	//将双军牌、个性牌颜色设置为白色
	if (Info.nPlateType == PLATE_INDIVIDUAL)
		Info.color = PC_WHITE;

	{
		HV_DebugInfo(DEBUG_STR_TRACK_FILE, "分割结果：\n");
		HV_DebugInfo( DEBUG_STR_TRACK_FILE ,
			"Type=%d, Color=%d\n",
			Info.nPlateType, Info.color
			);
		for (int i=0; i<g_cnPlateChars; i++)
		{
			HV_DebugInfo( DEBUG_STR_TRACK_FILE,
				"[%02d] (%d, %d) -> (%d, %d)\n",
				i, rgrcCharPos[i].left, rgrcCharPos[i].top,
				rgrcCharPos[i].right, rgrcCharPos[i].bottom
				);
		}
	}

	HV_DebugInfo( DEBUG_STR_TIME_FILE, "Segmentation Time = %d\n", GetSystemTick()-dwTickCount );

	// 如果分割失败
	if (iMaxScore<0 || Info.nPlateType==PLATE_UNKNOWN)
	{
		HV_DebugInfo( DEBUG_STR_TRACK_FILE, "分割失败 iMaxScore=%d\n", iMaxScore);

		for (int i=0; i<g_cnPlateChars; i++)
		{
			Info.rgbContent[i] = 0;
			Info.rgfltConfidence[i] = 0.0f;
		}
		Info.fltTotalConfidence = 0.0f;

		return S_FALSE;
	}

	int iCharNum = (Info.nPlateType == PLATE_DOUBLE_GREEN) ? 8 : 7;
	if (m_pInspector)
	{
		CPersistentComponentImage imgSegResult;
		RTN_HR_IF_FAILED(imgSegResult.Assign(imgRectified));

		for (int i = 0; i < iCharNum; i++)
		{
			MarkRect(imgSegResult, rgrcCharPos[i], 240);
		}

		if (m_fOutputDebugInfo)
		{
			//TODO:	覃宇：整理成模块
			CRect rcPlatePos(imgRectified.iWidth + 1, imgRectified.iHeight+1, -1, -1);
			rcPlatePos.left = rgrcCharPos[0].left;
			rcPlatePos.right = rgrcCharPos[6].right;
			rcPlatePos.top = (rgrcCharPos[0].top < rgrcCharPos[6].top) ?
				rgrcCharPos[0].top : rgrcCharPos[6].top;
			rcPlatePos.bottom = (rgrcCharPos[0].bottom > rgrcCharPos[6].bottom) ?
				rgrcCharPos[0].bottom : rgrcCharPos[6].bottom;

			// 计算车牌字符亮度
			CFastMemAlloc FastStack;
			HV_COMPONENT_IMAGE *pImgSeg = &imgRectified;
			int iSumL = 0;
			int iWidth = rcPlatePos.right - rcPlatePos.left;
			int iHeight = rcPlatePos.bottom - rcPlatePos.top;
			int iSize = iWidth * iHeight;

			BYTE8 *hsbData = ( BYTE8* )FastStack.StackAlloc(iSize * 3);
			if (NULL == hsbData)
			{
				return E_OUTOFMEMORY;
			}

			if (ConvertYCbCr2HSB(iWidth * 3,
				&hsbData[0],
				pImgSeg,
				rcPlatePos.left, rcPlatePos.top, iHeight, iWidth) == S_OK)
			{
				for ( int i =0; i < iSize; i++)
				{
					iSumL += hsbData[i * 3 + 2];
				}
			}
			iSumL /= iSize;
			Info.iHL = iSumL;

		}

		RTN_HR_IF_FAILED(m_pInspector->ShowComponentImage(ipSegmented, &imgSegResult, &Info));
	}

	// 计算螺钉位置
	GetPlateNail(Info.color, imgRectified, rgrcCharPos);

	dwTickCount = GetSystemTick();

	CRect rcPlatePos(imgRectified.iWidth+1, imgRectified.iHeight+1, -1, -1);
	Info.fltTotalConfidence = 1.0f;
	int n1Count(0);
    int nInValidCount = 0;    // 拒识无效数
    int nValidNum1Count = 0;  // 拒识有效且为1的个数

	bool bIsOne[10] = {0};//added by liujie 算连续的1的个数
	for (int i=0; i<iCharNum; i++)
	{
		if (rcPlatePos.left>rgrcCharPos[i].left)
		{
			rcPlatePos.left=rgrcCharPos[i].left;
		}
		if (rcPlatePos.top>rgrcCharPos[i].top)
		{
			rcPlatePos.top=rgrcCharPos[i].top;
		}
		if (rcPlatePos.right<rgrcCharPos[i].right)
		{
			rcPlatePos.right=rgrcCharPos[i].right;
		}
		if (rcPlatePos.bottom<rgrcCharPos[i].bottom)
		{
			rcPlatePos.bottom=rgrcCharPos[i].bottom;
		}

		Info.rgrcChar[i].SetRect( 
			(int)(rgrcCharPos[i].left * fltPlateScaled),
			(int)(rgrcCharPos[i].top * fltPlateScaled),
			(int)(rgrcCharPos[i].right * fltPlateScaled),
			(int)(rgrcCharPos[i].bottom * fltPlateScaled)
			);

		HV_COMPONENT_IMAGE imgChar;
		imgChar.nImgType = HV_IMAGE_GRAY;

		SetHvImageData(&imgChar, 0, GetHvImageData(&imgRectified, 0)+rgrcCharPos[i].left+
			rgrcCharPos[i].top*imgRectified.iStrideWidth[0]);
		imgChar.iWidth=rgrcCharPos[i].Width();
		imgChar.iHeight=rgrcCharPos[i].Height();
		imgChar.iStrideWidth[0]=imgRectified.iStrideWidth[0];

		SDWORD32 swProb;
		bool fRecogChar = true;		// 识别字符开关

		RECOGNITION_TYPE rtToRecog;
		if (i >= knPlateStrLen - 1)
		{
			rtToRecog = RECOG_DIGIT;
		}
		else
		{
			rtToRecog = (RECOGNITION_TYPE)Plate::g_cnType[Info.nPlateType][i];
		}

		if ((4 == i || 5 == i) && PC_LIGHTBLUE == Info.color && PLATE_WJ != Info.nPlateType && PLATE_POLICE2 != Info.nPlateType)
		{
			rtToRecog = (RECOGNITION_TYPE)Plate::g_cnType[PLATE_NORMAL][i];
		}

		//第0位非车尾模式使用全汉字模型
		/*if(	 i == 0 && MD_BOTTOM2TOP != m_pParam->g_MovingDirection && PLATE_DOUBLE_YELLOW == Info.nPlateType)
		{
			rtToRecog = RECOG_CHN;
		}*/

		// 建立补充 前处理规则
		switch(Info.nPlateType)
		{
		case PLATE_NORMAL:			// 蓝黑牌
			if (i == 6)
			{
				if ((Info.rgbContent[0] == 57 ) && (Info.rgbContent[1] == 36))
				{
					//增加对港澳牌的支持,港澳牌为"粤Z-****港"或"粤Z-****澳"
					rtToRecog = RECOG_GA;
				}
			}
			break;
		case PLATE_POLICE:							// 黄牌、广西警牌和白牌
		case PLATE_DOUBLE_YELLOW:					// 双层牌
			if (m_fEnableAlpha_5 && ((4 == i) || (5 == i)))
			{
				rtToRecog = RECOG_ALL;
			}
			if( m_iRecogNewMilitary == 1 && i==0)  
			{
				if(Info.nPlateType == PLATE_POLICE)
				{
					rtToRecog = RECOG_CHNALHPA;
				}
				if(Info.nPlateType ==PLATE_DOUBLE_YELLOW && Info.color != PC_YELLOW)
				{
					rtToRecog = RECOG_CHNALHPAMT;
				}
			}

			// 对京V 类型军牌的支持
			if (i >= 2 && (Info.rgbContent[0] == 38) && (Info.rgbContent[1] == 32))
			{
				rtToRecog = RECOG_DIGIT;		// 3~7 位全部使用数字识别
			}
			//第一位字符识别并不一定准确，按默认识别类型识别即可
			if (i >= 2 && i<= 3 && (IsMilitaryAlpha(Info.rgbContent[0]) || IsMilitaryChn(Info.rgbContent[0])))  //加入新军牌首字符为字母的判断
			{
				// 首字是军牌汉字
				rtToRecog = RECOG_DIGIT;		// 3~7 位全部使用数字识别
				break;
			}
			break;
		case PLATE_INDIVIDUAL:		// 个性化车牌
			if (i == 0)
			{
				//个性牌 i==0 时识别第二位的字母
				rtToRecog = RECOG_ALPHA;
			}
			break;
		case PLATE_WJ:				// 武警车牌
			if ( m_fEnableNewWJSegment && 1 == i)
			{
				// 新武警车牌支持
				rtToRecog = RECOG_CHNSF;
			}
			break;
		case PLATE_DOUBLE_WJ:
			if ( m_fEnableNewWJSegment && 1 == i)
			{
				// 新武警车牌支持
				rtToRecog = RECOG_CHNMT;
			}
			break;
		case PLATE_POLICE2:
			if (i ==6)
			{
				// 第7位不用识别
				Info.rgbContent[6] = 37;			// 强制设置成警字
				fRecogChar = false;					// 不识别
				swProb = 65535;						// 可信度
			}
			break;
		case PLATE_DOUBLE_MOTO:
			if( i == 4 || i == 5 || i == 6)
			{
				rtToRecog = RECOG_ALL;
			}
			break;
		default:
			break;
		}

		int nRecogCount(0);
		while(fRecogChar)
		{
			/*
			RTN_HR_IF_FAILED(m_pRecognition->Evaluate(
				&imgChar,
				rtToRecog,
				Info.nPlateType,
				Info.color,
				Info.rgbContent+i,
				&swProb,
				i
				));
				*/

			// zhaopy
			// 识别可能会返回失败，当分割有问题时，此处需处理此类问题。
			// 现默认给一个固定的字符。
			if( S_OK != m_pRecognition->Evaluate(
				&imgChar,
				rtToRecog,
				Info.nPlateType,
				Info.color,
				Info.rgbContent+i,
				&swProb,
				i
				) )
			{
				// 可信度为0.
				swProb = 0;	
				// 固定输出0字符
				Info.rgbContent[i] = 1;
			}


			fRecogChar = false;
			nRecogCount++;

			if (nRecogCount > 1)
			{
				break;					// 第二次识别直接退出
			}

			// 需要二次识别的规则
			if ((PLATE_POLICE == Info.nPlateType) && (i == 6))
			{
				// 对于广西警牌最后一位的处理，可能需要重新识别
				if (Info.rgbContent[6] == 37)			// 最后一位是“警”
				{
					// 只有打开了“广西警牌”开关的情况下，才允许出现“桂*****警”车牌
					// 非“桂”字头一律不允许“警”纸嵛?
					if (!((m_iRecogGxPolice == 1) && (Info.rgbContent[0] == 58)))	// “桂”，广西牌
					{
						// 使用数字模型重新识别
						rtToRecog = RECOG_DIGIT;
						fRecogChar = true;
					}
				}
				else if(Info.rgbContent[6] == 45)	// li 黄牌"辽"最后一位也有字母
				{
					rtToRecog = RECOG_ALL;
					fRecogChar = true;
				}
			} // if
		}

		// 缺省武警替换(和新规则无关)
		//if ( m_fEnableNewWJSegment 
		//	  && ((m_fEnableDefaultWJ && PLATE_WJ == Info.nPlateType)  || (m_fEnableDefaultWJ && PLATE_DOUBLE_WJ == Info.nPlateType) ) //双层武警强制替换
		//	  && strlen(m_strDefaultWJChar) > 1)
		//{
		//	if (0 == i)
		//	{
		//		*(Info.rgbContent) = m_strDefaultWJChar[0] - 47;
		//	}
		//	if (1 == i)
		//	{
		//		*(Info.rgbContent + 1) = m_strDefaultWJChar[1] - 47;
		//	}
		//}

		////如果不开默认武警小字且识别出来的小字>32,则强制为16
		//if(!m_fEnableDefaultWJ 
		//	&& (PLATE_WJ == Info.nPlateType || PLATE_DOUBLE_WJ == Info.nPlateType)
		//	&& (i == 1 && (Info.rgbContent[0] == 4 && Info.rgbContent[1] > 3 || Info.rgbContent[0] > 4))) 
		//{
		//	Info.rgbContent[0] = '1' - 47;
		//	Info.rgbContent[1] = '6' - 47;
		//}

		//识别双层牌时，对E误识别成A、B时进行再次识别(和新规则无关)
		if (Info.nPlateType == PLATE_DOUBLE_YELLOW && i == 1 &&
			( Info.rgbContent[1] == 11 || Info.rgbContent[1] == 12 )
			&& imgChar.iHeight >= 5
			)
		{
			BYTE8 bContentTemp(Info.rgbContent[i]);
			SDWORD32 swProbTemp(swProb);

			imgChar.iHeight--;

			/*
			RTN_HR_IF_FAILED(m_pRecognition->Evaluate(
				&imgChar,
				rtToRecog,
				Info.nPlateType,
				Info.color,
				Info.rgbContent+i,
				&swProb,
				i
				));
				*/

			if( S_OK != m_pRecognition->Evaluate(
				&imgChar,
				rtToRecog,
				Info.nPlateType,
				Info.color,
				Info.rgbContent+i,
				&swProb,
				i) )
			{
				// 可信度为0.
				swProb = 0;	
				// 固定输出0字符
				Info.rgbContent[i] = 1;
			}

			if (Info.rgbContent[i] != 15)
			{
				Info.rgbContent[i] = bContentTemp;
				swProb = swProbTemp;
			}
		}

		Info.rgfltConfidence[i]=(float)swProb/(1<<QUANTIFY_DIGIT);
		// 如果识别成1,则适当降低可信度
		if (Info.rgbContent[i] == 2)
		{
			n1Count++;
			bIsOne[i] = true;
		}

		// 拒识,检查是否为垃圾字符
        int fValid = TRUE;
        BOOL fIsWhite = (Info.color == PC_BLUE || Info.color == PC_BLACK);
        m_pRecognition->CheckValid(imgChar, fIsWhite, Info.rgbContent[i], &fValid);
        if(!fValid)
        { 
            Info.rgfltConfidence[i] = Info.rgfltConfidence[i].ToFloat() * 0.9;
            nInValidCount++;
        }
        else
        {
        	// 为'1'的情况判断
        	if (Info.rgbContent[i] == 2)
        	{
        		nValidNum1Count++;
        	}
        }

		HV_DebugInfo( DEBUG_STR_TRACK_FILE,
			"[%02d] Content=%d Prob=%d\n",
			i, Info.rgbContent[i], swProb
			);

		// WJ格式不用中间的字，而是用第一个字
		if ((Info.nPlateType == PLATE_WJ && i>1) ||
			(Info.nPlateType == PLATE_DOUBLE_GREEN && i > 2) ||
			(Info.nPlateType != PLATE_WJ && Info.nPlateType != PLATE_DOUBLE_GREEN && i>0))
		{
			Info.fltTotalConfidence*=Info.rgfltConfidence[i];
		}

		if (Info.fltTotalConfidence<m_pParam->g_fltThreshold_BeObservation)
		{
			HV_DebugInfo( DEBUG_STR_TRACK_FILE,
				"正在离开RecognizePlate(), "
				"TotalConf<fltThreshold_BeObservation\n"
				"i=%d, TotalConf=%f\n",
				i, Info.fltTotalConfidence.ToFloat()
				);

			return S_FALSE;
		}
	}

	// 如果多位识别成1,则适当降低可信度
	//if (n1Count > 3 && m_pParam->g_PlateRecogSpeed == PR_SPEED_NORMAL)
	//{
	//	Info.fltTotalConfidence*= 0.8f;
	//}
	//added by liujie
	//运算其最大连1个数，达到3个连1降低可信度
	int iMax1CC = 0;
	int tempCC = 0;
	for (int i = 0; i < iCharNum; i++)
	{
		if(bIsOne[i])
		{
			tempCC++;
		} else if(i < iCharNum - 1){
			if (tempCC > iMax1CC)
			{
				iMax1CC = tempCC;
			}
			tempCC = 0;
		} 
	}
	iMax1CC = tempCC>iMax1CC?tempCC:iMax1CC;
	if (iMax1CC >= 3)
	{
		Info.fltTotalConfidence*= 0.5f;
	}
	//end added

	if (Info.rgbContent[0] >= 103 && Info.rgbContent[0] <= 113
		&& (37 == Info.rgbContent[6] || PLATE_NORMAL == Info.nPlateType) )
	{
		Info.rgbContent[0] = m_nDefaultChnId;
	}

	Info.rcPlatePos = rcPlatePos;

	//计算车牌亮度
	int iPlateYSum = 0, iPlatePixelCount = 0;
	CRect rcOrigPlatePos(rcPlatePos);
	rcOrigPlatePos.left = (int)(rcOrigPlatePos.left * fltPlateScaled);
	rcOrigPlatePos.right = (int)(rcOrigPlatePos.right * fltPlateScaled);
	rcOrigPlatePos.top = (int)(rcOrigPlatePos.top * fltPlateScaled);
	rcOrigPlatePos.bottom = (int)(rcOrigPlatePos.bottom * fltPlateScaled);
	for (int y = rcOrigPlatePos.top; y < rcOrigPlatePos.bottom; y++)
	{
		for (int x = rcOrigPlatePos.left; x < rcOrigPlatePos.right; x++)
		{
			iPlatePixelCount++;
			iPlateYSum += GetHvImageData(&imgOrigPlate, 0)[y * imgOrigPlate.iStrideWidth[0] + x];
		}
	}
	Info.nAvgY = iPlateYSum / iPlatePixelCount;

	HV_DebugInfo(	DEBUG_STR_TRACK_FILE,
		"PlatePos: (%d, %d) - (%d, %d)\n",
		rcPlatePos.left, rcPlatePos.top, rcPlatePos.right, rcPlatePos.bottom
		);

	rcPlatePos.InflateRect(rcPlatePos.Width()/8, rcPlatePos.Height()/4);
	if (rcPlatePos.IntersectRect(rcPlatePos, CRect(0, 0, imgRectified.iWidth, imgRectified.iHeight)))
	{
		HV_COMPONENT_IMAGE imgCropped;
		imgCropped.nImgType = HV_IMAGE_YUV_422;
		CropImage(imgRectified, rcPlatePos, &imgCropped);

		// 调整分割后坐标
		Info.rcPlatePos.OffsetRect(-rcPlatePos.left, -rcPlatePos.top);	
		//Info.rcPlatePos = rcPlatePos;

		CPersistentComponentImage imgTemp;
		imgTemp.Assign(imgCropped);
		imgTemp.Detach(imgRectified);
	}

	Info.rcPlatePos.left = (int)(Info.rcPlatePos.left * fltPlateScaled);
	Info.rcPlatePos.right = (int)(Info.rcPlatePos.right * fltPlateScaled);
	Info.rcPlatePos.top = (int)(Info.rcPlatePos.top * fltPlateScaled);
	Info.rcPlatePos.bottom = (int)(Info.rcPlatePos.bottom * fltPlateScaled);
    Info.nInValidCount = nInValidCount;
    if (nInValidCount == 3)
    {
    	if (nValidNum1Count > 1)
    	{
    		Info.nInValidCount += nValidNum1Count - 1;
    	}
    }

	HV_DebugInfo(DEBUG_STR_TIME_FILE, "Recognition Time = %d\n", GetSystemTick()-dwTickCount);

	HV_DebugInfo(DEBUG_STR_TRACK_FILE, "==== BEGIN RecognizePlateResult\n");
	HV_DebugInfo(DEBUG_STR_TRACK_FILE,
		"TotalConf=%f, Type=%d, Color=%d\n",
		Info.fltTotalConfidence.ToFloat(),
		Info.nPlateType, Info.color
		);

	for (int i=0; i<g_cnPlateChars; i++)
	{
		HV_DebugInfo( DEBUG_STR_TRACK_FILE,
			"Char [%d], Content=%d Conf=%f\n",
			i, Info.rgbContent[i], Info.rgfltConfidence[i].ToFloat());
	}
	HV_DebugInfo(DEBUG_STR_TRACK_FILE, "==== END RecognizePlateResult\n");


	return S_OK;
}

// 计算螺钉的位置
HRESULT CTrackerImpl::GetPlateNail(PLATE_COLOR color, 
								   HV_COMPONENT_IMAGE imgPlate, 
								   CRect* pCharPos)
{
	CRect rgNailPos[7];
	bool fIsNail = false;
	HV_RECT rgrcNailPos = {0,0,0,0};
	int rgBottomLine[8] = {0};
	int iCharNum = 7;
	if (color == PC_YELLOW || color == PC_WHITE || color == PC_LIGHTBLUE)
	{
		for (int i = 0; i < iCharNum; i++)
		{
			GetNailPos(imgPlate, pCharPos[i], &rgNailPos[i].top, &rgNailPos[i].bottom, &rgBottomLine[i], color);
		}
		int iCountTop = 0;
		int iCountDown = 0;
		int iPointTop = 0;
		int iPointDown = 0;
		for (int i = 0; i < iCharNum; i++)
		{
			if ((i != 1 || i != 5) 
				&& rgNailPos[1].top < rgNailPos[i].top && rgNailPos[5].top < rgNailPos[i].top
				&& rgNailPos[1].top == 0 && rgNailPos[5].top == 0)
			{
				if(rgNailPos[i].top)
					iPointTop += rgNailPos[i].top;
				iCountTop++;
			}
		}
		for (int i = 0; i < iCharNum; i++)
		{
			if ((i != 1 || i != 5) 
				&& rgNailPos[1].bottom > rgNailPos[i].bottom && rgNailPos[5].bottom > rgNailPos[i].bottom
				&& rgNailPos[1].bottom + 1== pCharPos[1].Height()  && rgNailPos[5].bottom  + 1 == pCharPos[5].Height())
			{
				iPointDown += rgNailPos[i].bottom;
				iCountDown++;
			}
		}

		if (iCountDown >= 4 || iCountTop >= 4)
		{
			fIsNail = true;
			if(iCountTop >= 4)
				rgrcNailPos.top = iPointTop / iCountTop;
			if(iCountDown >= 4)
				rgrcNailPos.bottom = (iPointDown + (iCountDown >> 1)) / iCountDown;	// 四舍五入
		}
		// 针对F-E 去F的底框
		int iCountLineNum = 0;
		int iCountCol = 0;
		for (int i = 0; i < iCharNum; ++i)
		{
			if(rgBottomLine[i] < pCharPos[i].Height() && rgBottomLine[i] != 0)
			{
				iCountLineNum++;
				iCountCol += rgBottomLine[i];
			}
		}

		if (iCountLineNum < 6 ||
			(iCountLineNum == 7 && iCountCol == (pCharPos[0].Height() - 4) * iCountLineNum))	// 下部全白时 也不去
		{
			memset(rgBottomLine, 0, sizeof(rgBottomLine));
		}
	}
	m_pRecognition->EraseNail(fIsNail, rgrcNailPos, rgBottomLine);

	return S_OK;
}
HRESULT CTrackerImpl::DownSampleYUV422(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst)
{
	if (imgSrc.nImgType != HV_IMAGE_YUV_422 || imgDst.nImgType != HV_IMAGE_YUV_422)
	{
		return E_INVALIDARG;
	}
	int i, j;
	int iBlockNum = imgDst.iWidth >> 1;
	int iStepW = imgSrc.iWidth / iBlockNum;
	int iStepW2 = iStepW >> 1;
	int iStepH = imgSrc.iHeight / imgDst.iHeight;
	PBYTE8 pbSrcY = GetHvImageData(&imgSrc, 0);
	PBYTE8 pbSrcU = GetHvImageData(&imgSrc, 1);
	PBYTE8 pbSrcV = GetHvImageData(&imgSrc, 2);
	PBYTE8 pbDstY = GetHvImageData(&imgDst, 0);
	PBYTE8 pbDstU = GetHvImageData(&imgDst, 1);
	PBYTE8 pbDstV = GetHvImageData(&imgDst, 2);
	PBYTE8 pbSrcYTemp, pbSrcUTemp, pbSrcVTemp, pbDstYTemp, pbDstUTemp, pbDstVTemp;
	for (i = 0; i < imgDst.iHeight; i++)
	{
		pbSrcYTemp = pbSrcY; pbSrcUTemp = pbSrcU; pbSrcVTemp = pbSrcV;
		pbDstYTemp = pbDstY; pbDstUTemp = pbDstU; pbDstVTemp = pbDstV;
		for (j = 0; j < iBlockNum; j++)
		{
			*pbDstYTemp = *pbSrcYTemp;
			*(pbDstYTemp + 1) = *(pbSrcYTemp + 1);
			*pbDstUTemp = *pbSrcUTemp;
			*pbDstVTemp = *pbSrcVTemp;
			pbSrcYTemp += iStepW; pbSrcUTemp += iStepW2; pbSrcVTemp += iStepW2;
			pbDstYTemp += 2; pbDstUTemp++;	pbDstVTemp++;
		}
		pbSrcY += imgSrc.iStrideWidth[0] * iStepH;
		pbSrcU += imgSrc.iStrideWidth[1] * iStepH;
		pbSrcV += imgSrc.iStrideWidth[2] * iStepH;
		pbDstY += imgDst.iStrideWidth[0];
		pbDstU += imgDst.iStrideWidth[1];
		pbDstV += imgDst.iStrideWidth[2];
	}
	return S_OK;	
}

inline int CaclAvgGray(HV_COMPONENT_IMAGE imgCropPlate, int iAdd)
{
	// iAdd 为截图间隔
	int nWidth = imgCropPlate.iWidth;
	int nHeight = imgCropPlate.iHeight;
	int nCount = nWidth * nHeight;
	if(nCount == 0) return 0;

	nCount = 0;
	int nSum = 0;
	RESTRICT_PBYTE8 pLine = GetHvImageData(&imgCropPlate, 0);
	if (imgCropPlate.nImgType == HV_IMAGE_CbYCrY)
	{
		for(int i = 0; i < nHeight; i++, pLine += imgCropPlate.iStrideWidth[0])
		{
			for(int j = 1; j < nWidth; j += 2)
			{
				nSum += pLine[j];
				nCount++;
			}
		}
	}
	if(imgCropPlate.nImgType == HV_IMAGE_YCbYCr)
	{
		for(int i = 0; i < nHeight; i++, pLine += imgCropPlate.iStrideWidth[0])
		{
			for(int j = 0; j < nWidth; j += 2)
			{
				nSum += pLine[j];
				nCount++;
			}
		}
	}
	if(imgCropPlate.nImgType == HV_IMAGE_YUV_422 ||
		imgCropPlate.nImgType == HV_IMAGE_BT1120 ||
		imgCropPlate.nImgType ==  HV_IMAGE_BT1120_FIELD ||
		imgCropPlate.nImgType == HV_IMAGE_YUV_420 )
	{
		for(int i = 0; i < nHeight; i++, pLine += imgCropPlate.iStrideWidth[0])
		{
			for(int j = 0; j < nWidth; j++)
			{
				nSum += pLine[j];
				nCount++;
			}
		}
	}

	return nSum/nCount;
}

int GetCarColorType( DWORD32 dwCarColorID )
{
	DWORD32 dwCarColorType(0);
	int i;
	for (i = 0; i < CARCOLOR_NUM; i++)
	{
		if ( dwCarColorID == g_rgCarColorInfo[i].dwCarColorID)
		{
			dwCarColorType = g_rgCarColorInfo[i].dwCarColorType;
			break;
		}
	}

	if(i == CARCOLOR_NUM) return 0;
	return dwCarColorType;
}

HRESULT CTrackerImpl::RecognizeCarColour(
	HV_COMPONENT_IMAGE imgPlate,
	PlateInfo &Info,
	int iAvgGray                                  //环境亮度
	)
{

	// 抓拍车身颜色识别只在晚上进行
	// 抓拍图车身颜色识别的效果会比视频流的好，不再做这个限制。
//	if (m_LightType != NIGHT)
//	{
//		return S_FALSE;
//	}
	//LockTracker();
	bool FisNight = FALSE;
	if(NIGHT == m_LightType)
		FisNight = TRUE;

	HRESULT hr = RecognizeCarColour(imgPlate, Info,iAvgGray, true);
	//UnLockTracker();
	return hr;
}

HRESULT CTrackerImpl::RecognizeColorArea(
	const HV_COMPONENT_IMAGE imgInput, 
	HV_RECT rcCrop, 
	BYTE8 *cTempAdd, 
	BYTE8 *piResult,
	int iCutLeng,
	LIGHT_TYPE m_PlateLightType
	)
{
	HV_COMPONENT_IMAGE imgCutArea;

	CPersistentComponentImage CTempImg;
	CTempImg.Create(HV_IMAGE_BGR,iCutLeng + 2, iCutLeng + 2);

	if(GetHvImageData(&CTempImg, 0) == NULL)
	{
		*piResult = 0;
		return E_POINTER;
	}

	if(rcCrop.right - rcCrop.left <= 0 ||  rcCrop.bottom - rcCrop.top <= 0
		|| rcCrop.left < 0 || rcCrop.right >= imgInput.iWidth || rcCrop.top < 0 || rcCrop.bottom >= imgInput.iHeight
	    //	|| rcCrop.right - rcCrop.left > iCutLeng || rcCrop.bottom - rcCrop.top > iCutLeng
		|| rcCrop.left >= imgInput.iWidth || rcCrop.top >= imgInput.iHeight 
		|| rcCrop.right < 0 || rcCrop.bottom < 0)
	{
		*piResult = 0;
		return S_OK;
	}

	RTN_HR_IF_FAILED(CropImage(imgInput, rcCrop, &imgCutArea));

	if(SUCCEEDED(ConvertYCbCr2BGR(CTempImg.iStrideWidth[0], GetHvImageData(&CTempImg, 0), &imgCutArea)))
	{
		HV_COMPONENT_IMAGE pInput;
		pInput.iHeight = 1;
		pInput.iWidth  = 7;
		SDWORD32 idwProb = 0;
		BYTE8 cResult = 0;
		int iTempAdd[4] = {0};
		int iTempCut = imgCutArea.iHeight * imgCutArea.iWidth;
		BYTE8 *pcRBG = GetHvImageData(&CTempImg, 0);
		for(int i = 0; i < imgCutArea.iHeight; i++, pcRBG += CTempImg.iStrideWidth[0])
		{
			BYTE8 *pbRGBTemp = pcRBG;						// 当前行的首地址
			for(int j = 0; j < imgCutArea.iWidth; j++, pbRGBTemp += 3)
			{
				iTempAdd[1] += pbRGBTemp[0];
				iTempAdd[2] += pbRGBTemp[1];
				iTempAdd[3] += pbRGBTemp[2];
			}
		}

		cTempAdd[3] = iTempAdd[1] / iTempCut;		// B
		cTempAdd[2] = iTempAdd[2] / iTempCut;		// G
		cTempAdd[1] = iTempAdd[3] / iTempCut;		// R
		BGR2HSL(cTempAdd[3],cTempAdd[2], cTempAdd[1], &cTempAdd[4], &cTempAdd[5], &cTempAdd[6]);
		SetHvImageData(&pInput, 0, cTempAdd);
		//m_PlateLightType<=25用白天模型识别，否则用晚上模型识别
		m_pRecognition->EvaluateColor(&pInput, &cResult, &idwProb, m_PlateLightType);
		CAR_COLOR nCarColor = (CAR_COLOR)GetCarColorType(cResult);

		//颜色修正
		if(nCarColor == CC_BLUE)
		{
			if(cTempAdd[4] >=115 && cTempAdd[4] <= 130 && cTempAdd[6] >= 190)
			{
				nCarColor = CC_WHITE;
			}
			else if((cTempAdd[4] >=120 && cTempAdd[4] <= 135 && (cTempAdd[5]+cTempAdd[6]) < 120)
				||(cTempAdd[4] >=120 && cTempAdd[4] <= 125 && cTempAdd[6] <= 60 && cTempAdd[5]<=130)
				||(cTempAdd[4]<120 && cTempAdd[6]<150))
			{
				nCarColor = CC_GREEN;
			}
			else if(cTempAdd[4] >=140 && cTempAdd[4] <= 150 && cTempAdd[5] <= 65 && cTempAdd[6] <= 45)
			{
				nCarColor = CC_BLACK;
			}
		}
		if(nCarColor == CC_GREY )
		{
			if((cTempAdd[4] >=125 && cTempAdd[4] <= 140 && cTempAdd[6] >= 180) ||
				(cTempAdd[4] >=50 && cTempAdd[4] <= 140 && cTempAdd[5]<=10 && cTempAdd[6]>= 170))
			{
				nCarColor = CC_WHITE;
			}
			if(cTempAdd[4] >=140 && cTempAdd[4] <= 160 && cTempAdd[5]<=35 && cTempAdd[6]<=50)
			{
				nCarColor = CC_BLACK;
			}
		}
		if(nCarColor == CC_YELLOW && cTempAdd[5] < 30)
		{
			nCarColor = CC_GREY;
		}
		if(nCarColor == CC_RED)
		{
			if(cTempAdd[6] < 10 ||(cTempAdd[6] < 20 && cTempAdd[5]<120))
			{
				nCarColor = CC_BLACK;
			}
			else if((cTempAdd[4] > 20&& cTempAdd[4] < 30 && (cTempAdd[5] +cTempAdd[6]) > 200)|| (cTempAdd[4] >12 && cTempAdd[4]<=20 && cTempAdd[6]<=40))
			{
				nCarColor = CC_BROWN;
			}		
		}

		*piResult = nCarColor;
	}
	else
	{
		*piResult = 0;		// 未知结果置0
	}

	return S_OK;
}

HRESULT CTrackerImpl::RecognizeCarColour(
	HV_COMPONENT_IMAGE imgPlateOri,                   //帧图（原始大图）
	PlateInfo &Info,
	int iAvgGray,                                    //环境亮度
	BOOL fIsNight
	)
{
	Info.iCarColor = 0;
	if (Info.nPlateType == PLATE_DOUBLE_MOTO || Info.nCarType == CT_BIKE)
	{
		Info.nCarColour = CC_UNKNOWN;
		return S_OK;
	}

	if(imgPlateOri.iHeight < 200 || imgPlateOri.iWidth < 200)			
	{
		Info.nCarColour = CC_UNKNOWN;
		return S_OK;
	}

	//识别结果为警牌的车身颜色直接赋值为白色
	if(Info.rgbContent[6] == 37)
	{
		Info.nCarColour = CC_WHITE;
		Info.iCarColor = 1;
		return S_OK;
	}


	HV_COMPONENT_IMAGE imgCutOri;
	LIGHT_TYPE m_PlateLightType = DAY;
	BYTE8 cTempAdd[7];	
	CRect rcCharPos; 

	bool fIsSmallCar = (Info.color != PC_YELLOW && Info.rgbContent[6] != 114 && Info.rgbContent[6] != 115);
	int iWidth = Info.rgrcChar[6].right - Info.rgrcChar[0].left;
	int iHeight = Info.rcPos.bottom - Info.rcPos.top;   //车牌高度
	float fAdjTop = 3;             //高度调整系数
	CRect rcPositionPos;
	if(fIsSmallCar)  
	{
		rcPositionPos.top = (long)MAX2(Info.rcPos.top - iHeight*3,0);
		rcPositionPos.bottom = (long)MAX2(Info.rcPos.top - iHeight,0);
		rcPositionPos.left = (long)MAX2(Info.rcPos.left - iWidth/2,0);
		rcPositionPos.right = (long)MIN2(Info.rcPos.right + iWidth/2,imgPlateOri.iWidth-1);
	}
	else 
	{
		rcPositionPos.top = (long)MAX2(Info.rcPos.top - iHeight*4,0);
		rcPositionPos.bottom = Info.rcPos.top;
		rcPositionPos.left = (long)MAX2(Info.rcPos.left - iWidth*1.8,0);
		rcPositionPos.right = Info.rcPos.right;
	}
	CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
	cTempAdd[0] = BYTE8(CaclAvgGray(imgCutOri, 2));	

	int DiffY = cTempAdd[0] - iAvgGray;  
	//港澳牌、车身区域亮度较小——fIsSmallCar为真
	if((Info.rgbContent[6] == 114 || Info.rgbContent[6] == 115) && cTempAdd[0] < 50)
	{
		fIsSmallCar = true;
	}

	//单层牌
	if (Info.nPlateType != PLATE_DOUBLE_YELLOW)  
	{	
		rcCharPos.top = Info.rcPos.top + Info.rgrcChar[0].top;
		rcCharPos.left = Info.rcPos.left + Info.rgrcChar[0].left;
		rcCharPos.bottom = Info.rcPos.top + Info.rgrcChar[0].bottom;
		rcCharPos.right = Info.rcPos.left + Info.rgrcChar[6].right;
	}
	//双层牌
	else
	{
		rcCharPos.bottom = Info.rcPos.top + Info.rgrcChar[3].bottom;
		rcCharPos.right = Info.rcPos.left + Info.rgrcChar[6].right;
		rcCharPos.top = Info.rcPos.top + Info.rgrcChar[3].top;
		rcCharPos.left = Info.rcPos.left + Info.rgrcChar[2].left;	
	}
	int iPlateH = rcCharPos.bottom - rcCharPos.top + 1;               //车牌高度
	int iPlateW = rcCharPos.right - rcCharPos.left + 1;		           //车牌宽度
	int iPlateMid = int((rcCharPos.right + rcCharPos.left) * 0.5);     //车牌宽度中点坐标

	int iCutLeng = iPlateH;
	if(iCutLeng > 14)  iCutLeng = 14;
	if(iCutLeng < 8)	iCutLeng = 8;
	if(iPlateH < 15) iPlateH =15;
	if(fIsSmallCar && (Info.rgbContent[6] == 114 || Info.rgbContent[6] == 115))
	{
		fAdjTop = 5;
	}

	CRect rcCutArea[YELLOW_NUM];            //车身颜色截取区域
	BYTE8 rgResult[2][YELLOW_NUM] = {0};   //车身颜色识别结果
	BYTE8 rgCutAreaL[2][YELLOW_NUM] = {0}; 
	int iPlateColor,iPlateColor2;
	int rgPlateColor[2];            //投票后的车身颜色识别结果
	int rgPlateColorProb[2] = {0};	// 可信度

	IReferenceComponentImage* pTmpImg = NULL;		

	CreateReferenceComponentImage(
		&pTmpImg,
		HV_IMAGE_BGR,
		iCutLeng + 1,
		iCutLeng + 1
		);


	// 小车（车牌颜色：非黄色）
	if(fIsSmallCar)
	{
		rcCutArea[0].top = (long)MAX2((Info.rcPos.top - fAdjTop*iPlateH),0);
		rcCutArea[0].left  = iPlateMid - iCutLeng/2;

		rcCutArea[1].top = rcCutArea[0].top;
		rcCutArea[1].left  = (long)MAX2((iPlateMid - iPlateW),0);
		rcCutArea[2].top = rcCutArea[0].top;
		rcCutArea[2].left  = (long)MIN2((iPlateMid + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[11].top = (long)MAX2((rcCutArea[0].top - iPlateH),0);
		rcCutArea[11].left  = rcCutArea[0].left;
		rcCutArea[3].top = (long)MAX2((rcCutArea[0].top -iPlateH),0);
		rcCutArea[3].left  = (long)MAX2((iPlateMid - iPlateW/2),0);
		rcCutArea[4].top = rcCutArea[3].top;
		rcCutArea[4].left  = (long)MAX2((rcCutArea[3].left  - iPlateW),0);

		rcCutArea[5].top = rcCutArea[3].top;
		rcCutArea[5].left  = (long)MIN2((iPlateMid + iPlateW/2), imgPlateOri.iWidth - iCutLeng - 1);
		rcCutArea[6].top = rcCutArea[3].top;
		rcCutArea[6].left  = (long)MIN2((rcCutArea[5].left  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[7].top = (long)MAX2((rcCutArea[0].top - 2*iPlateH),0);
		rcCutArea[7].left  = (long)MAX2((iPlateMid - iPlateW),0);
		rcCutArea[8].top = rcCutArea[7].top;
		rcCutArea[8].left  = (long)MAX2((iPlateMid  - 0.3*iPlateW),0);
		rcCutArea[9].top = rcCutArea[7].top;
		rcCutArea[9].left  = (long)MIN2((iPlateMid + 0.3*iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		rcCutArea[10].top = rcCutArea[7].top;
		rcCutArea[10].left  = (long)MIN2((iPlateMid  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		for(int i=0;i<NORMAL_NUM;i++)
		{
			rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
			rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
		}
		//车身颜色识别
		for(int i = 0; i < NORMAL_NUM; i++)
		{
			BYTE8 *piResult = &rgResult[0][i];
			RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
			rgCutAreaL[0][i] = cTempAdd[6];
		}
		//统计颜色个数
		int rgCountColor[11] = {0};
		for(int j = 0; j < NORMAL_NUM; j++)
		{
			rgCountColor[rgResult[0][j]]++;
		}
		//各颜色评分
		int nColorScore[11] = {0};
		int nColorScore2[11] = {0};
		for(int j=0;j<11;j++)
		{
			if(j<4)
			{
				nColorScore[j] = rgCountColor[j] * 10;
			}
			else
			{
				if(rgCountColor[j] <= 3)
				{
					nColorScore[j] =  rgCountColor[j] * 18;
				}
				else
				{
					nColorScore[j] =  rgCountColor[j] * 24;
				}
			}		
		}

		//保存分数最大的颜色作为车身颜色的结果
		int nColorResult=0,nSecondResult = 0;;
		for(int i=0;i<11;i++)
		{
			if(nColorScore[nColorResult] < nColorScore[i] || (nColorScore[nColorResult] == nColorScore[i] && i>3 && nColorResult < 4)) 
			{
				nSecondResult = nColorResult;
				nColorResult = i;
			}
			else if(nColorScore[nSecondResult] <= nColorScore[i] && nColorScore[i] > 0) 
			{
				nSecondResult = i;
			}
		}
		//黑色车一般不可能有彩色区域存在
		if(nColorResult == 3 && nSecondResult > 3 && nColorScore[nSecondResult] > 70)
		{
			nColorResult = nSecondResult;
		}

		iPlateColor = nColorResult;
		//深浅色区分
		int iAvgL = 0;
		int nCountL = 0;

		for(int i=0;i<NORMAL_NUM;i++)
		{
			if(rgResult[0][i] == iPlateColor)
			{
				iAvgL += rgCutAreaL[0][i];
				nCountL++;
			}
		}
		iAvgL = iAvgL/nCountL;

		if( iAvgL >=130)
			Info.iCarColor = 1;
		else
			Info.iCarColor = 2;

		rgPlateColor[0] = nColorResult;  //第一个识别结果
		rgPlateColorProb[0] = nColorScore[nColorResult];  //第一个识别结果可信度
		//识别成黑灰的重识别一次（主要是为了提高彩色的识别）
		if(nColorResult == 2 || nColorResult == 3)  
		{
			//向上移动一个高度再识别一次
			for(int i = 0; i < NORMAL_NUM; i++)
			{
				rcCutArea[i].top = MAX2(rcCutArea[i].top - iPlateH, 0);
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;

				BYTE8 *piResult = &rgResult[1][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
				rgCutAreaL[1][i] = cTempAdd[6];
			}
			for(int j=0;j<11;j++)
			{
				rgCountColor[j] = 0;
			}
			//统计颜色个数
			for(int j = 0; j < NORMAL_NUM; j++)
			{
				rgCountColor[rgResult[1][j]]++;
			}
			//各颜色评分
			for(int j=0;j<11;j++)
			{
				if(j<4)
				{
					nColorScore2[j] = rgCountColor[j] * 10;
				}
				else
				{
					if(rgCountColor[j] < 3)
					{
						nColorScore2[j] =  rgCountColor[j] * 18;
					}
					else
					{
						nColorScore2[j] =  rgCountColor[j] * 24;
					}
				}		
			}

			//保存分数最大的颜色作为车身颜色的结果
			nColorResult=0;
			nSecondResult = 0;;
			for(int i=0;i<11;i++)
			{
				if(nColorScore2[nColorResult] < nColorScore2[i] || (nColorScore2[nColorResult] == nColorScore2[i] && i>3 && nColorResult < 4)) 
				{
					nSecondResult = nColorResult;
					nColorResult = i;
				}
				else if(nColorScore2[nSecondResult] <= nColorScore2[i] && nColorScore2[i] > 0) 
				{
					nSecondResult = i;
				}
			}
			//黑色车一般不可能有彩色区域存在
			if(nColorResult == 3 && nSecondResult > 3 && nColorScore2[nSecondResult] > 70)
			{
				nColorResult = nSecondResult;
			}

			rgPlateColor[1] = nColorResult;  //第2个识别结果
			rgPlateColorProb[1] = nColorScore2[nColorResult];  //第2个识别结果可信度
			//两次结果取分数
			if((rgPlateColorProb[0] < rgPlateColorProb[1] && rgPlateColor[1] != 3) || (rgPlateColor[1] > 3 && rgPlateColorProb[1] >= 96))
			{
				iPlateColor = rgPlateColor[1];
			}
		}

		//黑重识别，取车牌两边位置识别
		if(iPlateColor == CC_BLACK && (m_nEnableColorReRecog & 1) && DiffY < m_nBColorReThr) 
		{
			//计算车牌右边一小块区域的车牌亮度
			rcPositionPos.top = Info.rcPos.top;
			rcPositionPos.bottom = Info.rcPos.bottom;
			rcPositionPos.left =Info.rcPos.left + Info.rgrcChar[6].right + 15;
			rcPositionPos.right = (long)MIN2(rcPositionPos.left + iHeight*2,imgPlateOri.iWidth-1);
			CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
			int TempY = BYTE8(CaclAvgGray(imgCutOri, 2));		

			rcCutArea[0].top = Info.rcPos.top;
			rcCutArea[0].left  = Info.rcPos.left;
			rcCutArea[1].top = rcCutArea[0].top;
			rcCutArea[1].left  = Info.rcPos.left-iPlateW/2;
			rcCutArea[2].top = rcCutArea[0].top;
			rcCutArea[2].left  = Info.rcPos.right;
			rcCutArea[3].top = rcCutArea[0].top;
			rcCutArea[3].left  = Info.rcPos.right + iPlateW/2;
			for(int i=0;i<4;i++)
			{
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
				rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
			}

			for(int i = 0; i < 4; i++)
			{
				BYTE8 *piResult = &rgResult[0][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);	
			}

			int rgCountColor[11] = {0};
			for(int j = 0; j < 4; j++)
			{
				rgCountColor[rgResult[0][j]]++;
			}
			for(int j=4;j<=7;j++)
			{
				if(rgCountColor[j] == 4 && j!= 5) //车牌两边4块区域都识别成红绿蓝
				{
					iPlateColor = j;
				}
			}
			if(iPlateColor <=3 && DiffY >= -10 && (rgCountColor[3] <= 2 || TempY >= 30)) 
			{
				iPlateColor = CC_GREY;
			}

		}
		//根据环境亮度和车身颜色设置黑和灰结果
		if(iPlateColor == CC_BLACK && m_fEnableCorlorThr && DiffY > m_nBGColorThr)
		{
			iPlateColor = CC_GREY;
		}
		//白重识别
		if(iPlateColor == CC_WHITE && (m_nEnableColorReRecog & 2) && cTempAdd[0]> m_nWColorReThr)
		{
			//计算车牌右边一小块区域的车牌亮度
			rcPositionPos.top = Info.rcPos.top;
			rcPositionPos.bottom = Info.rcPos.bottom;
			rcPositionPos.left =Info.rcPos.left + Info.rgrcChar[6].right + 15;
			rcPositionPos.right = (long)MIN2(rcPositionPos.left + iHeight*2,imgPlateOri.iWidth-1);
			CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
			int TempY = BYTE8(CaclAvgGray(imgCutOri, 2));		

			rcCutArea[0].top = Info.rcPos.top - 3;
			rcCutArea[0].left  = rcCharPos.left - iPlateW/2;
			rcCutArea[1].top = rcCutArea[0].top - 2;
			rcCutArea[1].left  = rcCharPos.left-iPlateW;
			rcCutArea[2].top = rcCutArea[0].top;
			rcCutArea[2].left  = rcCharPos.right + iPlateW/2;
			rcCutArea[3].top = rcCutArea[1].top;
			rcCutArea[3].left  = rcCharPos.right + iPlateW;
			for(int i=0;i<4;i++)
			{
				rcCutArea[i].bottom = rcCutArea[i].top + 12;
				rcCutArea[i].right = rcCutArea[i].left + 12;
			}
			
			for(int i = 0; i < 4; i++)
			{
				BYTE8 *piResult = &rgResult[0][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);	
				
			}
			int rgCountColor[11] = {0};
			for(int j = 0; j < 4; j++)
			{
				rgCountColor[rgResult[0][j]]++;
			}

			for(int j=4;j<=7;j++)
			{
				if(rgCountColor[j] == 4 /*&& j!= 5*/) //车牌两边4块区域都识别成红绿蓝
				{
					iPlateColor = j;
				}
			}
			if(iPlateColor <=3 && TempY <= 160 && iAvgL-cTempAdd[0] <=60 
				&& rgCountColor[1] <= 2 && rgCountColor[3] < 4) 
			{
				iPlateColor = CC_GREY;
			}

		}

		if(m_LightType == NIGHT && iPlateColor == CC_GREY && m_fEnableCorlorThr && DiffY > m_nWGColorThr)
		{
			iPlateColor = CC_WHITE;
		}
		//识别成白或灰——修正成黑（收费站场景下），取车牌两边区域识别
		if((iPlateColor == CC_GREY || iPlateColor == CC_WHITE)&& (m_nEnableColorReRecog & 4) && (DiffY < m_nWGColorReThr || iAvgGray < (m_nWGColorReThr+40)))
		{
			//计算车牌右边一小块区域的车牌亮度
			rcPositionPos.top = Info.rcPos.top;
			rcPositionPos.bottom = Info.rcPos.bottom;
			rcPositionPos.left =Info.rcPos.left + Info.rgrcChar[6].right + 15;
			rcPositionPos.right = (long)MIN2(rcPositionPos.left + iHeight*2,imgPlateOri.iWidth-1);
			CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
			int TempY = BYTE8(CaclAvgGray(imgCutOri, 2));

			rcCutArea[0].top = Info.rcPos.top - 3;
			rcCutArea[0].left  = rcCharPos.left - iPlateW/2;
			rcCutArea[1].top = rcCutArea[0].top - 2;
			rcCutArea[1].left  = rcCharPos.left-iPlateW;
			rcCutArea[2].top = rcCutArea[0].top;
			rcCutArea[2].left  = rcCharPos.right + iPlateW/2;
			rcCutArea[3].top = rcCutArea[1].top;
			rcCutArea[3].left  = rcCharPos.right + iPlateW;

			for(int i=0;i<4;i++)
			{
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
				rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
			}
			for(int i = 0; i < 4; i++)
			{
				BYTE8 *piResult = &rgResult[0][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);	
				//收费站下黑识别成灰
				if(*piResult == CC_GREY && cTempAdd[6] < 35 && TempY < 30 && (cTempAdd[0]-cTempAdd[6]) > 120)
				{
					*piResult = CC_BLACK;
				}
			}

			int rgCountColor[11] = {0};
			for(int j = 0; j < 4; j++)
			{
				rgCountColor[rgResult[0][j]]++;
			}

			for(int j=4;j<=7;j++)
			{
				if(rgCountColor[j] >= 3 && j!= 5) //车牌两边4块区域都识别成红绿蓝
				{
					iPlateColor = j;
				}
			}
			if(iPlateColor <=3 && (rgCountColor[3] >= 3 || TempY < 20 || (TempY < 35 && rgCountColor[3] >= 2)))   //车牌两边有3块以上识别成黑
			{
				iPlateColor = CC_BLACK;
			}
			else if(iPlateColor == CC_WHITE && rgCountColor[1] <= 2)
			{
				iPlateColor = CC_GREY;
			}
		}
	}

	else
	{
		rcCutArea[0].top = (long)MAX2((Info.rcPos.top - fAdjTop*iPlateH),0);
		rcCutArea[0].left  = (long)MAX2((iPlateMid - iPlateW),0);

		rcCutArea[1].top = rcCutArea[0].top;
		rcCutArea[1].left  = (long)MIN2((iPlateMid + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[2].top = (long)MAX2((rcCutArea[0].top - iPlateH),0);
		rcCutArea[2].left  = (long)MAX2((iPlateMid - 1.5*iPlateW),0);

		rcCutArea[3].top = rcCutArea[2].top;
		rcCutArea[3].left  = (long)MIN2((iPlateMid + 1.5*iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[4].top = rcCutArea[2].top;
		rcCutArea[4].left  = (long)MAX2((rcCutArea[2].left  - iPlateW/2),0);

		rcCutArea[5].top = rcCutArea[2].top;
		rcCutArea[5].left  =  (long)MIN2((rcCutArea[3].left  + iPlateW/2), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[6].top = (long)MAX2((rcCutArea[0].top - 2*iPlateH),0);
		rcCutArea[6].left  = (long)MAX2((iPlateMid - 0.5*iPlateW),0);
		rcCutArea[7].top = rcCutArea[6].top;
		rcCutArea[7].left  = (long)MIN2((iPlateMid + 0.5*iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[8].top = rcCutArea[6].top;
		rcCutArea[8].left  = (long)MAX2((rcCutArea[6].left  - iPlateW),0);
		rcCutArea[9].top = rcCutArea[6].top;
		rcCutArea[9].left  = (long)MIN2((rcCutArea[7].left + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[10].top = rcCutArea[6].top;
		rcCutArea[10].left  = (long)MAX2((rcCutArea[8].left  - iPlateW),0);
		rcCutArea[11].top = rcCutArea[6].top;
		rcCutArea[11].left  = (long)MIN2((rcCutArea[9].left  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[12].top = (long)MAX2((rcCutArea[0].top - 3*iPlateH),0);
		rcCutArea[12].left  = (long)MAX2((iPlateMid - iPlateW),0);
		rcCutArea[13].top = rcCutArea[12].top;
		rcCutArea[13].left  = (long)MIN2((iPlateMid + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[14].top = rcCutArea[12].top;
		rcCutArea[14].left  = (long)MAX2((rcCutArea[12].left  - iPlateW),0);
		rcCutArea[15].top = rcCutArea[12].top;
		rcCutArea[15].left  = (long)MIN2((rcCutArea[13].left  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//车牌两边区域
		rcCutArea[16].top = Info.rcPos.top;
		rcCutArea[16].left  = (long)MAX2(rcCharPos.left - iPlateW/2,0);
		rcCutArea[17].top = rcCutArea[16].top;
		rcCutArea[17].left  = (long)MAX2(rcCharPos.left-iPlateW,0);
		rcCutArea[18].top = rcCutArea[16].top;
		rcCutArea[18].left  = (long)MIN2(rcCharPos.right + iPlateW/2,imgPlateOri.iWidth - iCutLeng -1);
		rcCutArea[19].top = rcCutArea[16].top;
		rcCutArea[19].left  = (long)MIN2(rcCharPos.right + iPlateW,imgPlateOri.iWidth - iCutLeng -1);

		for(int i=0;i<YELLOW_NUM;i++)
		{
			rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
			rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
		}

		for(int i = 0; i < YELLOW_NUM; i++)
		{
			BYTE8 *piResult = &rgResult[0][i];
			RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
			rgCutAreaL[0][i] = cTempAdd[6];
		}
		//统计颜色个数
		int rgCountColor[11] = {0};
		for(int j = 0; j < YELLOW_NUM; j++)
		{
			rgCountColor[rgResult[0][j]]++;
		}
		//各颜色评分
		int nColorScore[11] = {0};
		int nColorScore2[11] = {0};
		for(int j=0;j<11;j++)
		{	
			if(j==1 || j>3)
			{
				nColorScore[j] = rgCountColor[j] * 30;
			}
			else if(j==2)
			{
				nColorScore[j] = rgCountColor[j] * 12;
			}
			else if(j==3)
			{
				nColorScore[j] = rgCountColor[j] * 8;
			}
		}

		//保存分数最大的颜色作为车身颜色的结果
		int nColorResult=0,nSecondResult = 0;
		for(int i=0;i<11;i++)
		{
			if(nColorScore[nColorResult] < nColorScore[i] || (nColorScore[nColorResult] == nColorScore[i] && i>3 && nColorResult < 4)) 
			{
				nSecondResult = nColorResult;
				nColorResult = i;
			}
			else if(nColorScore[nSecondResult] <= nColorScore[i] && nColorScore[i] > 0) 
			{
				nSecondResult = i;
			}
		}
		//红色车识别成蓝色车修正
		if(nColorResult == 7 && nSecondResult == 4 && nColorScore[nSecondResult] > 120)
		{
			nColorResult = nSecondResult;
		}
		//黑色车一般不可能有彩色
		if(nColorResult == 3 && nSecondResult > 3 && nColorScore[nSecondResult] >= 90)
		{
			nColorResult = nSecondResult;
		}

		iPlateColor = nColorResult;
		rgPlateColor[0] = nColorResult;  //第一个识别结果
		rgPlateColorProb[0] = nColorScore[nColorResult];  //第一个识别结果可信度

		//部分蓝色大车车身较高，截取到车灯部分，晚上容易识别成红——设置成黑，向上一个高度重识别
		if(nColorResult == 4 && nSecondResult == 7 && nColorScore[nColorResult]- nColorScore[nColorResult] <= 60)
		{
			nColorResult = 3;
		}

		//深浅色区分
		int iAvgL = 0;
		int nCountL = 0;

		for(int i=0;i<YELLOW_NUM;i++)
		{
			if(rgResult[0][i] == iPlateColor)
			{
				iAvgL += rgCutAreaL[0][i];
				nCountL++;
			}
		}
		iAvgL = iAvgL/nCountL;

		if(iAvgL >=130)
			Info.iCarColor = 1;
		else
			Info.iCarColor = 2;


		//识别成黑灰的重识别一次
		if(nColorResult == 1 || nColorResult ==2 || nColorResult == 3 || nColorResult == 10)
		{
			//向上移动2个高度再识别一次
			for(int i = 0; i < YELLOW_NUM; i++)
			{
				rcCutArea[i].top = MAX(rcCutArea[i].top - 2 * iCutLeng, 0);
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;

				BYTE8 *piResult = &rgResult[1][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
				rgCutAreaL[1][i] = cTempAdd[6];
			}

			//统计颜色个数
			for(int j=0;j<11;j++)
			{
				rgCountColor[j] = 0;
			}
			for(int j = 0; j < YELLOW_NUM; j++)
			{
				rgCountColor[rgResult[1][j]]++;
			}

			//各颜色评分
			for(int j=0;j<11;j++)
			{	
				if(j==1 || j>3)
				{
					nColorScore2[j] = rgCountColor[j] * 30;
				}
				else if(j==2)
				{
					nColorScore2[j] = rgCountColor[j] * 10;
				}
				else if(j==3)
				{
					nColorScore2[j] = rgCountColor[j] * 5;
				}
			}

			//保存分数最大的颜色作为车身颜色的结果
			nColorResult=0;
			nSecondResult = 0;;
			for(int i=0;i<11;i++)
			{
				if(nColorScore2[nColorResult] < nColorScore2[i] || (nColorScore2[nColorResult] == nColorScore2[i] && i>3 && nColorResult < 4)) 
				{
					nSecondResult = nColorResult;
					nColorResult = i;
				}
				else if(nColorScore2[nSecondResult] <= nColorScore2[i] && nColorScore2[i] > 0) 
				{
					nSecondResult = i;
				}
			}
			//黑色车一般不可能有彩色区域存在
			if(nColorResult == 3 && (nSecondResult > 3 && nColorScore2[nSecondResult] > 60))
			{
				nColorResult = nSecondResult;
			}

			rgPlateColor[1] = nColorResult;  //第2个识别结果
			rgPlateColorProb[1] = nColorScore2[nColorResult];  //第2个识别结果可信度
			
			if(rgPlateColor[0] == 1 || rgPlateColor[0] == 10)  //第一次识别成白色或棕色，向上移动一个位置识别成红黄绿蓝等并且分数大于120
			{
				if(rgPlateColorProb[1] >= 120 && rgPlateColor[1] >=4 && rgPlateColor[1] <= 7)
				{
					iPlateColor = rgPlateColor[1];
				}
				if((rgPlateColor[0] == 1 && rgPlateColorProb[0] < 100 && rgCountColor[1]<=2) ||
					(rgPlateColor[0] == 10 && rgPlateColorProb[0] < 100 && rgCountColor[10]<=2))
				{
					iPlateColor = 2;
				}
			}
			else if((rgPlateColorProb[0] < rgPlateColorProb[1] && rgPlateColor[1] != 3) || 
				((rgPlateColor[1] > 3 || rgPlateColor[1] == 1) &&rgPlateColorProb[1] > 90))
			{
				iPlateColor = rgPlateColor[1];
			}

		}
		//车身亮度大于环境亮度——黑设置成灰
		if(iPlateColor == CC_BLACK && m_fEnableCorlorThr && DiffY > m_nBGColorThr)
		{
			iPlateColor = CC_GREY;
		}
		if(m_LightType == NIGHT && iPlateColor == CC_GREY && m_fEnableCorlorThr&& DiffY > m_nWGColorThr)
		{
			iPlateColor = CC_WHITE;
		}
		//港澳牌大车一般为白色
		if((Info.rgbContent[6] == 114 || Info.rgbContent[6] == 115) && ((iPlateColor > 3 && rgPlateColorProb[0] < 150 &&  rgPlateColorProb[1] < 150)
			||(iPlateColor == 2 && (nColorScore[1] >=90 || nColorScore2[1] >= 90))))
		{
			iPlateColor = 1;
		}
	}
	Info.nCarColour = (CAR_COLOR)iPlateColor;
	if (iPlateColor == CC_BLACK)  Info.iCarColor = 2;
	if(iPlateColor == CC_WHITE)  Info.iCarColor = 1;

	pTmpImg->Release();
	pTmpImg = NULL;

	return S_OK;
}

HRESULT MapParamRect(CRect& rcResult, CRect rcWhole, CRect rcParam, float fltScale=.01f)
{
	rcResult.left=rcWhole.left+(int)(rcWhole.Width()*rcParam.left*fltScale + 0.5);
	rcResult.top=rcWhole.top+(int)(rcWhole.Height()*rcParam.top*fltScale + 0.5);
	rcResult.right=rcWhole.left+(int)(rcWhole.Width()*rcParam.right*fltScale + 0.5);
	rcResult.bottom=rcWhole.top+(int)(rcWhole.Height()*rcParam.bottom*fltScale + 0.5);

	return S_OK;
}

//通过IO取当前红绿灯状态
DWORD32 CTrackerImpl::GetIOStatus(int iTeam, BYTE8 bLevel)
{
	DWORD32 dwRet = 0;
	BYTE8 bTmp = bLevel;
	int iLightCount = m_rgLightInfo[iTeam].GetPosCount();

	for( int i = 0; i < 8; ++i )
	{
		if( (bTmp & 1) && (m_rgIOLight[i].iTeam == iTeam) )
		{
			dwRet |= (0x01 << (4 * (iLightCount - m_rgIOLight[i].iPos -1)));
		}
		bTmp >>= 1;
	}

	return dwRet;
}
#if 1
HRESULT CTrackerImpl::CheckTrafficLight(HV_COMPONENT_IMAGE* pSceneImage)
{
	//调用红绿灯接口,返回当前帧灯的状态和稳定识别的状态
	m_cTrafficLight.RecognizeTrafficLight(pSceneImage, &m_iCurLightStatus, &m_iLastLightStatus, m_LightType == NIGHT);
	return S_OK;
}
#else
HRESULT CTrackerImpl::CheckTrafficLight(HV_COMPONENT_IMAGE* pSceneImage)
{
	static int nSceneStatusPos= 0;

	LIGHT_TEAM_STATUS ltsInfo;
	ltsInfo.nTeamCount = m_nLightCount;

	for(int i = 0; i<m_nLightCount; i++)
	{
		if( CTrackInfo::m_iCheckType == 0 )
		{
			if( CTrackInfo::m_iAutoScanLight == 1 )
			{
				m_rgLightInfo[i].UpdateStatus2( pSceneImage );
				m_rgLightRect[i] = m_rgLightInfo[i].GetLastRect();
			}
			else
			{
				m_rgLightInfo[i].UpdateStatus( pSceneImage );
				m_rgLightRect[i] = m_rgLightInfo[i].GetRect();
			}

			m_rgLightStatus[i] = m_rgLightInfo[i].GetLastStatus();
		}
		else
		{
			m_rgLightRect[i] = m_rgLightInfo[i].GetRect();
			BYTE8 bLevel = m_nIOLevel;
//			if( S_OK != m_pCallback->GetTrafficLightStatus(&bLevel) ) 
//			{
//				return E_FAIL;
//			}
			m_rgLightStatus[i] = GetIOStatus( i, bLevel );
		}
		ltsInfo.pdwStatus[i] = m_rgLightStatus[i];
		ltsInfo.pnLightCount[i] = m_rgLightInfo[i].GetPosCount();
	}

	//取得当前场景
	bool fFlag = (CTrackInfo::m_iSceneCheckMode == 0);
/*
	char szTemp[255];
	sprintf(szTemp, "m_iSceneCount = %d, m_iLastOkLightStatus = %d, fFlag =%d\n", m_iSceneCount, m_iLastOkLightStatus, fFlag);
	strcat(g_szDebugInfo, szTemp);
*/
	int nCurScene = TransitionScene(ltsInfo, m_iSceneCount, m_iLastOkLightStatus, fFlag);
/*
	sprintf(szTemp, "m_iSceneCount = %d, m_iLastOkLightStatus = %d, fFlag =%d, nCurScene = %d\n", m_iSceneCount, m_iLastOkLightStatus, fFlag, nCurScene);
	strcat(g_szDebugInfo, szTemp);
*/
	//if(m_pCallback)
	if(true)
	{
		int nSceneStatus = -1;

		//把红灯的位置传出去
		//m_pCallback->TrafficLightStatus(
		//	m_nLightCount, 
		//	m_rgLightStatus,
		//	&nSceneStatus,
		//	m_rgLightRect, 
		//	m_nRedLightCount,
		//	m_rgRedLightRect,
		//	pSceneImage
		//	);

		nSceneStatus = nCurScene;

		m_iCurLightStatus = nSceneStatus;
		//如果用户设置了场景状态则加入场景状态队列
		//if(hr == S_OK && nSceneStatus != -1)
		if( nSceneStatus != -1 )
		{
			m_nErrorSceneCount = 0;
			SetSceneStatus(nSceneStatusPos, nSceneStatus);
			nSceneStatusPos++;	//下一个存储位置,同时也是计数

			int iRelayTimeMs = m_nValidSceneStatusCount * 100;

			//加入场景状态后判断是否输出
			if(m_nValidSceneStatusCount > 0 && nSceneStatusPos >= m_nValidSceneStatusCount)
			{
				int nLastPos = nSceneStatusPos - 1;
				int nLastStatus = GetSceneStatus(nLastPos);
				BOOL fValid = TRUE;
				for( int i = nLastPos - 1; i > (nLastPos - m_nValidSceneStatusCount); i--)
				{
					if( GetSceneStatus(i) != nLastStatus)
					{
						fValid = FALSE;
						break;
					}
				}

				//如果是快速模式，则有一帧判断为下一场景就跳变.
				if( m_fCheckSpeed && !fValid && m_iLastOkLightStatus != -1 && m_iLastOkLightStatus != nSceneStatus)
				{
					int iNextStatus = (m_iLastOkLightStatus + 1) % m_iSceneCount; 
					if( iNextStatus == nSceneStatus ) 
					{
						fValid = true;
						iRelayTimeMs = 100;
						nLastStatus = nSceneStatus;
					}
				}

				if( fValid && nLastStatus != m_iLastLightStatus)
				{
					//如果有绿灯变成红灯且设有黄灯延迟
					TRAFFICLIGHT_SCENE tsLast, tsRelay;
					GetLightScene(m_iLastOkLightStatus, &tsLast);
					GetLightScene(nLastStatus, &tsRelay);
					DWORD32 dwTick = GetSystemTick();
					bool fRelayTimeOut = true;
					if( CTrackInfo::m_iRedLightDelay > 0 )
					{
						if( tsLast.lsLeft == TLS_GREEN && tsRelay.lsLeft == TLS_RED )
						{
							if( m_ltRedRelay.dwLeft == 0 ) m_ltRedRelay.dwLeft = dwTick;
							if( int(dwTick - m_ltRedRelay.dwLeft) <  (CTrackInfo::m_iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
						if( tsLast.lsForward == TLS_GREEN && tsRelay.lsForward == TLS_RED )
						{
							if( m_ltRedRelay.dwForward == 0 ) m_ltRedRelay.dwForward = dwTick;
							if( int(dwTick - m_ltRedRelay.dwForward) <  (CTrackInfo::m_iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
						if( tsLast.lsRight == TLS_GREEN && tsRelay.lsRight == TLS_RED )
						{
							if( m_ltRedRelay.dwRight == 0 ) m_ltRedRelay.dwRight = dwTick;
							if( int(dwTick - m_ltRedRelay.dwRight) <  (CTrackInfo::m_iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
						if( tsLast.lsTurn == TLS_GREEN && tsRelay.lsTurn == TLS_RED )
						{
							if( m_ltRedRelay.dwTurn == 0 ) m_ltRedRelay.dwTurn = dwTick;
							if( int(dwTick - m_ltRedRelay.dwTurn) <  (CTrackInfo::m_iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
					}

					if( fRelayTimeOut )
					{
						m_ltRedRelay.Reset();
						//zhaopy
						TRAFFICLIGHT_SCENE tsShow;
						GetLightScene(nLastStatus, &tsShow);
						char szInfo[256];
						char szTemp[64];
						switch(tsShow.lsLeft)
						{
							case TLS_GREEN:
								strcpy( szTemp, "LG，");
								strcat(szInfo, szTemp);
								break;
							case TLS_RED:
								strcpy( szTemp, "LR，");
								strcat(szInfo, szTemp);
								break;
							default:
								strcpy( szTemp, "L??，");
								strcat(szInfo, szTemp);
								break;
						}
						switch(tsShow.lsForward)
						{
							case TLS_GREEN:
								strcpy( szTemp, "FG，");
								strcat(szInfo, szTemp);
								break;
							case TLS_RED:
								strcpy( szTemp, "FR，");
								strcat(szInfo, szTemp);
								break;
							default:
								strcpy( szTemp, "F??，");
								strcat(szInfo, szTemp);
								break;
						}
						switch(tsShow.lsRight)
						{
							case TLS_GREEN:
								strcpy( szTemp, "RG，");
								strcat(szInfo, szTemp);
								break;
							case TLS_RED:
								strcpy( szTemp, "RR，");
								strcat(szInfo, szTemp);
								break;
							default:
								strcpy( szTemp, "R??，");
								strcat(szInfo, szTemp);
								break;
						}
						HV_Trace(5, "%s\n", szInfo);
						//strcpy(g_szDebugInfo, szInfo);
						
						m_iLastLightStatus = nLastStatus;
						TRAFFICLIGHT_SCENE tsSceneStatus, tsNow;
						GetLightScene(m_iLastLightStatus, &tsSceneStatus);
						GetLightScene(m_iLastLightStatus, &tsNow);

						int iGreenRelayMs = 1000;

						//m_pCallback->TrafficSceneStatus(tsSceneStatus);

						//更新红灯开始时间
						if( nLastStatus != -1 )
						{
							m_iLastOkLightStatus = nLastStatus;
						}
						//一体机代码，红绿灯时间只有L，没有H，在ARM端在通过ConvertTickToSystemTime进行转换,黄国超修改,2011-07-21
						DWORD32 dwTimeLow = GetSystemTick() - iRelayTimeMs, dwTimeHigh = 0;
						//ConvertTickToSystemTime((GetSystemTick() - iRelayTimeMs), dwTimeLow, dwTimeHigh);

						if(  tsNow.lsLeft == TLS_RED && tsLast.lsLeft != TLS_RED )
						{
							m_redrealtime.dwLeftL = dwTimeLow;
							m_redrealtime.dwLeftH = dwTimeHigh;
						}
						else if( tsNow.lsLeft != TLS_RED )
						{
							m_redrealtime.dwLeftL = m_redrealtime.dwLeftH = 0;
						}
						if(  tsNow.lsLeft == TLS_GREEN && tsLast.lsLeft != TLS_GREEN )
						{
							m_greentick.dwLeft = GetSystemTick() - iRelayTimeMs - iGreenRelayMs;
						}
						else if( tsNow.lsLeft != TLS_GREEN )
						{
							m_greentick.dwLeft = 0;
						}

						if( tsNow.lsForward == TLS_RED && tsLast.lsForward != TLS_RED )
						{
							m_redrealtime.dwForwardL = dwTimeLow;
							m_redrealtime.dwForwardH = dwTimeHigh;
						}
						else if( tsNow.lsForward != TLS_RED )
						{
							m_redrealtime.dwForwardL = m_redrealtime.dwForwardH = 0;
						}
						if( tsNow.lsForward == TLS_GREEN && tsLast.lsForward != TLS_GREEN )
						{
							m_greentick.dwForward = GetSystemTick() - iRelayTimeMs - iGreenRelayMs;
						}
						else if( tsNow.lsForward != TLS_GREEN )
						{
							m_greentick.dwForward = 0;
						}

						if(  tsNow.lsRight == TLS_RED && tsLast.lsRight != TLS_RED )
						{
							m_redrealtime.dwRightL = dwTimeLow;
							m_redrealtime.dwRightH = dwTimeHigh;

						}
						else if( tsNow.lsRight != TLS_RED )
						{
							m_redrealtime.dwRightL = m_redrealtime.dwRightH = 0;
						}
						if(  tsNow.lsRight == TLS_GREEN && tsLast.lsRight != TLS_GREEN )
						{
							m_greentick.dwRight = GetSystemTick() - iRelayTimeMs - iGreenRelayMs;
						}
						else if( tsNow.lsRight != TLS_GREEN )
						{
							m_greentick.dwRight = 0;
						}

						if(  tsNow.lsTurn == TLS_RED && tsLast.lsTurn != TLS_RED )
						{
							m_redrealtime.dwTurnL = dwTimeLow;
							m_redrealtime.dwTurnH = dwTimeHigh;
						}
						else if( tsNow.lsTurn != TLS_RED )
						{
							m_redrealtime.dwTurnL = m_redrealtime.dwTurnH = 0;
						}
						if(  tsNow.lsTurn == TLS_GREEN && tsLast.lsTurn != TLS_GREEN )
						{
							m_greentick.dwTurn = GetSystemTick() - iRelayTimeMs - iGreenRelayMs;
						}
						else if( tsNow.lsTurn != TLS_GREEN )
						{
							m_greentick.dwTurn = 0;
						}
					}
				}
			}
		}
		else
		{
			//如果无效场景数大于设定的值，则把当前的场景设成-1.
			m_nErrorSceneCount++;
			if( m_nErrorSceneCount >= (m_nValidSceneStatusCount * 2))
			{
				m_iLastLightStatus = -1;
				TRAFFICLIGHT_SCENE tsSceneStatus;
				GetLightScene(m_iLastLightStatus, &tsSceneStatus);
				//m_pCallback->TrafficSceneStatus(tsSceneStatus);
				//无效场景有可能是中间状态，红灯时间不能重置。
			}
			//如果无效场景帧数大过设定的值，则重置红绿灯框位置,红灯时间重置
			if( m_nErrorSceneCount > CTrafficLightInfo::MAX_ERROR_COUNT )
			{
				m_nErrorSceneCount = 0;
				for(int i = 0; i < 4; ++i)
				{
					m_redrealtime.Reset();
					m_greentick.Reset();
				}
				if( CTrackInfo::m_iAutoScanLight == 1 )
				{
					for(int i = 0; i <m_nLightCount; ++i)
					{
						if( S_OK == m_rgLightInfo[i].ReScan() )
						{
							HV_Trace(5, "TrafficLight ReScan ...\n");
						}
					}
				}
			}
		}
	}
	return S_OK;
}
#endif

HRESULT CTrackerImpl::DetectionParkingBox(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	)
{
	int nResult = 0;
	const int MAX_DET_ROI = 100;
	svIPDetApi::DET_ROI rgDetRoi[MAX_DET_ROI];

	HRESULT hr(S_OK);

	if ( ( pParam == NULL ) || ( pProcessData == NULL ) || ( pProcessRespond == NULL ) ) return E_FAIL;
	HV_COMPONENT_IMAGE imgFrame = pProcessData->hvImageYuv;

	hr = m_cCarDetector.DetectCar(pParam, imgFrame, rgDetRoi, MAX_DET_ROI, &nResult);

	if(S_OK != hr)
	{
		return hr;
	}

	if(nResult >= MAX_DET_ROI)
	{
		nResult = MAX_DET_ROI;
	}

	pProcessRespond->cTrackRectInfo.dwTrackCount = nResult;

	for(int i = 0; i < nResult; i++)
	{
		if( i < 20 )
		{
			pProcessRespond->cTrackRectInfo.rgTrackRect[i].left = rgDetRoi[i].m_nLeft;
			pProcessRespond->cTrackRectInfo.rgTrackRect[i].top = rgDetRoi[i].m_nTop;
			pProcessRespond->cTrackRectInfo.rgTrackRect[i].right = rgDetRoi[i].m_nRight;
			pProcessRespond->cTrackRectInfo.rgTrackRect[i].bottom = rgDetRoi[i].m_nBottom;
			pProcessRespond->iDetType[i] = rgDetRoi[i].m_nType;
		}
	}

	return S_OK;
}


HRESULT CTrackerImpl::ProcessOneFrame(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
)
{
	HV_COMPONENT_IMAGE& imgSnapFrame = pProcessData->hvImageYuv;
	HV_COMPONENT_IMAGE imgCapFrame;
	SetHvImageData(&imgCapFrame, 0, NULL);
	SetHvImageData(&imgCapFrame, 1, NULL);
	SetHvImageData(&imgCapFrame, 2, NULL);

	if ( (imgSnapFrame.iWidth != m_nWidth || imgSnapFrame.iHeight != m_nHeight)
			|| m_fIsModifyParam )
	{
		FRAME_PROPERTY cFrameProperty;
		cFrameProperty.iWidth = imgSnapFrame.iWidth;
		cFrameProperty.iHeight = imgSnapFrame.iHeight;
		cFrameProperty.iStride = imgSnapFrame.iStrideWidth[0];

		SetFrameProperty(cFrameProperty, pProcessRespond);
		m_fIsModifyParam = false;
	}

//	char szMsg[256];
//	sprintf(szMsg, "ProcessOneFrame begin.");
//	OutPutDebugMsg(LOG_LV_NORMAL, szMsg);

	if (!m_fInitialized)
	{
		return E_UNEXPECTED;
	}

	if (pParam->fIsCaptureImage)
	{
		CRect rc(
			pParam->cFrameRecognizeParam.cRecogSnapArea.DetectorAreaLeft,
			pParam->cFrameRecognizeParam.cRecogSnapArea.DetectorAreaTop,
			pParam->cFrameRecognizeParam.cRecogSnapArea.DetectorAreaRight,
			pParam->cFrameRecognizeParam.cRecogSnapArea.DetectorAreaBottom
			);
		return RecogSnapImg(
			pParam,
			pProcessData,
			pProcessRespond,
			imgSnapFrame,
			rc,
			pParam->cFrameRecognizeParam.cRecogSnapArea.nDetectorMinScaleNum,
			pParam->cFrameRecognizeParam.cRecogSnapArea.nDetectorMaxScaleNum
			);
	}

    if ( pParam->cDetectParam.fIsDetectStopBox )
	{
		pProcessData->hvImageYuv.iStrideWidth[0] = (pProcessData->hvImageYuv.iStrideWidth[0] >> 1);
		pProcessData->hvImageYuv.iHeight <<= 1;
		return DetectionParkingBox(pParam,pProcessData,pProcessRespond);
	}

	return S_OK;
}


bool CTrackerImpl::CheckPeccancyTrigger(CTrackInfo* pTrack)
{
	if( pTrack == NULL ) return false;
	if( pTrack->m_fHasTrigger ) return true;

	int iTwoPos = CTrackInfo::m_iCaptureTwoPos * m_nHeight / 100;
	int iCurPos = pTrack->LastInfo().rcPos.CenterPoint().y;
	bool fIsConverse = (pTrack->LastInfo().rcPos.CenterPoint().y - pTrack->m_rgPlateInfo[0].rcPos.CenterPoint().y) > (m_nHeight * 3 / 100);
	bool fPassPos = fIsConverse ? (iCurPos > iTwoPos) : (iTwoPos > iCurPos);

	if( fPassPos && CheckPeccancy(pTrack) )
	{
		DWORD32 dwCurTick = GetSystemTick();
		// trigger
		if( (dwCurTick - m_dwLastTriggerTick) > MIN_TRIGGER_TIME )
		{
			TriggerCamera(0xff);
			m_dwLastTriggerTick = dwCurTick;
			// set
			pTrack->m_fHasTrigger = true;
			pTrack->m_dwTriggerTimeMs = dwCurTick; // TODO: image time?
		}
		else
		{
			// set
			pTrack->m_fHasTrigger = true;
			pTrack->m_dwTriggerTimeMs = m_dwLastTriggerTick; // TODO: image time?
		}
	}

	return true;
}
//判断是否有可能违章
bool CTrackerImpl::CheckPeccancy(CTrackInfo *pTrack)
{
	if( pTrack == NULL ) return false;

	if( pTrack->m_cPlateInfo < 3 ) return false;

	int iMove = abs(pTrack->m_rgPlateInfo[0].rcPos.CenterPoint().y -  pTrack->LastInfo().rcPos.CenterPoint().y);
	if( iMove < (CTrackInfo::m_iHeight * 2 / 100) ) return false;

	//是否是非机动车道
	int iRoad = MatchRoad(pTrack->LastInfo().rcPos.CenterPoint().x, pTrack->LastInfo().rcPos.CenterPoint().y);
	if( iRoad < 0 || iRoad >= CTrackInfo::m_iRoadNumber )
	{
		return false;
	}
	if( CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_ESTOP )
	{
		return true;
	}
	//是否有可能闯红灯
	BOOL fRush = FALSE;
	TRAFFICLIGHT_SCENE tsStatus;
	//GetLightScene(m_iLastOkLightStatus, &tsStatus);
	m_cTrafficLight.GetLightScene(m_iLastLightStatus, &tsStatus);
	if( ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_FORWARD) && tsStatus.lsForward == TLS_RED)
		|| ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_LEFT) && tsStatus.lsLeft == TLS_RED)
		|| ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_RIGHT) && tsStatus.lsRight == TLS_RED)
		|| ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_TURN) && tsStatus.lsTurn == TLS_RED) )
	{
		return true;
	}

	//是否压线
	CROSS_OVER_LINE_TYPE olt = IsOverYellowLine((*pTrack));
	if( olt != COLT_INVALID && olt != COLT_NO )
	{
		return true;
	}
	//是否逆行
	switch (m_pParam->g_MovingDirection)
	{
	case MD_TOP2BOTTOM:
		if (pTrack->LastInfo().rcPos.top -
			pTrack->m_rgPlateInfo[0].rcPos.top > -(m_nHeight * 3 / 100))
		{
		}
		else
		{
			return true;
		}
		break;
	case MD_BOTTOM2TOP:
		if (pTrack->LastInfo().rcPos.top -
			pTrack->m_rgPlateInfo[0].rcPos.top
			< (m_nHeight * 3  / 100))
		{
		}
		else
		{
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

bool CTrackerImpl::CheckObjectPeccancyTrigger(CObjTrackInfo* pObjTrackInfo, int iObjIndex)
{
	if( pObjTrackInfo == NULL ) return false;
	if( pObjTrackInfo->m_fHasTrigger ) return true;

	//当前正在跟踪车牌的跳过
	bool fPlateTracking = false;
	for (int j = 0; j < m_cTrackInfo; j++)
	{
		if (m_rgTrackInfo[j].m_State == tsInit ||
			m_rgTrackInfo[j].m_State == tsWaitRemove)
		{
			continue;
		}
		if (m_rgObjTrackInfo[iObjIndex].MatchPlateTrackID(m_rgTrackInfo[j].m_nID))
		{
			fPlateTracking = true;
			break;
		}
	}
	if( fPlateTracking ) return true;

	int iTwoPos = CTrackInfo::m_iCaptureTwoPos * m_nHeight / 100;
	int iCurPos = pObjTrackInfo->LastInfo().m_rcFG.bottom;
	bool fIsConverse = (pObjTrackInfo->LastInfo().m_rcFG.bottom - pObjTrackInfo->LastInfo().m_rcFG.bottom) > (m_nHeight * 3  / 100);
	bool fPassPos = fIsConverse ? (iCurPos > iTwoPos) : (iTwoPos > iCurPos);

	if( fPassPos && CheckObjectPeccancy(pObjTrackInfo) )
	{
		DWORD32 dwCurTick = GetSystemTick();
		// trigger
		if( (dwCurTick - m_dwLastTriggerTick) > MIN_TRIGGER_TIME )
		{
			TriggerCamera(0xff);
			m_dwLastTriggerTick = dwCurTick;
			// set
			pObjTrackInfo->m_fHasTrigger = true;
			pObjTrackInfo->m_dwTriggerTimeMs = dwCurTick; // TODO: image time?
		}
		else
		{
			// set
			pObjTrackInfo->m_fHasTrigger = true;
			pObjTrackInfo->m_dwTriggerTimeMs = m_dwLastTriggerTick; // TODO: image time?
		}
	}

	return true;
}

//判断无牌车是否可能违章
bool CTrackerImpl::CheckObjectPeccancy(CObjTrackInfo* pObjTrackInfo)
{
	if( pObjTrackInfo == NULL ) return false;

	if( pObjTrackInfo->m_iObjInfoCount < 2 ) return false;

	int iMove = abs(pObjTrackInfo->m_rgObjInfo[0].m_rcFG.CenterPoint().y -  pObjTrackInfo->LastInfo().m_rcFG.CenterPoint().y);
	if( iMove < (CTrackInfo::m_iHeight * 2 / 100) ) return false;

	//是否是非机动车道
	int iRoad = MatchRoad(pObjTrackInfo->LastInfo().m_rcFG.CenterPoint().x, pObjTrackInfo->LastInfo().m_rcFG.CenterPoint().y);
	if( iRoad >= 0 && iRoad < CTrackInfo::m_iRoadNumber
		&& (CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_ESTOP) )
	{
		return true;
	}
	//是否有可能闯红灯
	BOOL fRush = FALSE;
	TRAFFICLIGHT_SCENE tsStatus;
	//GetLightScene(m_iLastOkLightStatus, &tsStatus);
	m_cTrafficLight.GetLightScene(m_iLastLightStatus, &tsStatus);
	if( ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_FORWARD) && tsStatus.lsForward == TLS_RED)
		|| ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_LEFT) && tsStatus.lsLeft == TLS_RED)
		|| ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_RIGHT) && tsStatus.lsRight == TLS_RED)
		|| ((CTrackInfo::m_roadInfo[iRoad].iRoadType & RRT_TURN) && tsStatus.lsTurn == TLS_RED) )
	{
		fRush = TRUE;
	}

	//是否逆行
	switch (m_pParam->g_MovingDirection)
	{
	case MD_TOP2BOTTOM:
		if (pObjTrackInfo->LastInfo().m_rcFG.top -
			pObjTrackInfo->m_rgObjInfo[0].m_rcFG.top > -(m_nHeight * 3 / 100))
		{
		}
		else
		{
			return true;
		}
		break;
	case MD_BOTTOM2TOP:
		if (pObjTrackInfo->LastInfo().m_rcFG.top -
			pObjTrackInfo->m_rgObjInfo[0].m_rcFG.top
			< m_nHeight * 3 / 100)
		{
		}
		else
		{
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}
HRESULT CTrackerImpl::CalcFrameSpeed(CTrackInfo *pTrack)
{
	int iPosOne  = CTrackInfo::m_iCaptureOnePos * CTrackInfo::m_iHeight / 100;
	if(pTrack->m_iFrameSpeedCount < 1000 && pTrack->m_cPlateInfo > 1 && pTrack->LastInfo().rcPos.CenterPoint().y < iPosOne)
	{
		float fltPos = (float)(pTrack->m_rgPlateInfo[0].rcPos.CenterPoint().y - pTrack->LastInfo().rcPos.CenterPoint().y);
		float fltTime = float((pTrack->LastInfo().dwFrameTime - pTrack->m_rgPlateInfo[0].dwFrameTime) / 1000.0);
		if(fltTime * 1000 > 0)
			pTrack->m_rgfltFrameSpeed[ pTrack->m_iFrameSpeedCount++ ] = fltPos / fltTime;
		else
			pTrack->m_rgfltFrameSpeed[ pTrack->m_iFrameSpeedCount++ ] = 0;
	}
	return S_OK;
}

HRESULT CTrackerImpl::ProcessTrackState(
	CTrackInfo *pTrack, 								//当前处理的跟踪
	CRect &rcTrackArea, 								//跟踪区域
	PlateInfo &plateObserved,							//当前帧检测到的车牌信息
	HV_COMPONENT_IMAGE &imgCalibratedFrame,		//经过矫正的视频帧
	CPersistentComponentImage &imgBestRectified,		//经过矫正的车牌小图
	HV_COMPONENT_IMAGE &imgOriSnapFrame,					//原始的视频帧
	HV_COMPONENT_IMAGE &imgOriCapFrame,	 				//原始的全景帧
	PROCESS_ONE_FRAME_PARAM* pParam,
	PROCESS_ONE_FRAME_RESPOND* pProcessRespond
)
{
	//抓拍补光灯模式下才需要算像素点与时间的比
	if(CTrackInfo::m_iFlashlightMode == 1)
	{
		CalcFrameSpeed(pTrack);
	}

	//weikt 20110423 保证车牌小图在视频检测区域内
	CRect rectIntersect;
	CRect rcPlatePosLast = plateObserved.rcPos;
	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcVideoDet;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDet, rcMax, m_rcVideoDetArea));
	BOOL fIntersect = rectIntersect.IntersectRect(&plateObserved.rcPos, &rcVideoDet);
	if (fIntersect)
	{
		if(rectIntersect != plateObserved.rcPos)
		{
			HV_Trace(1, "IntersectRect ...\n");
			plateObserved.rcPos = rectIntersect;
		}
	}
	else
	{
		//将完全超出检测区的车牌置为非 fObservaionExists
		pTrack->m_fObservationExists = false;
	}
	//weikt end


	if (pTrack->m_fObservationExists)
	{
		pTrack->m_nObservedFrames++;
		if (pTrack->m_cPlateInfo>0 &&
			0 == HV_memcmp(plateObserved.rgbContent,
			pTrack->LastInfo().rgbContent,
			sizeof(plateObserved.rgbContent)))
		{
			pTrack->m_nResultEqualTimes++;
		}
		else
		{
			pTrack->m_nResultEqualTimes=0;
		}

		pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo++]=plateObserved;

		HV_COMPONENT_IMAGE imgObservedPlate;
		// 转换图片格式,以适应识别模块, 旋转的BT1120格式做特殊处理
		CPersistentComponentImage imgPlateRotateY;
		if (imgCalibratedFrame.nImgType == HV_IMAGE_BT1120_ROTATE_Y)
		{
			imgPlateRotateY.Create(HV_IMAGE_YUV_422, plateObserved.rcPos.Width(), plateObserved.rcPos.Height());
			imgPlateRotateY.CropAssign(imgCalibratedFrame, plateObserved.rcPos);
			imgObservedPlate = imgPlateRotateY;
		}
		else
		{
			RTN_HR_IF_FAILED(CropImage(
				imgCalibratedFrame, plateObserved.rcPos, &imgObservedPlate
			));
		}
		

		if( CTrackInfo::m_iFlashlightMode == 1 && m_iAvgY < CTrackInfo::m_iFlashlightThreshold  )
		{
			CheckPeccancyTrigger(pTrack);
		}
		if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
		{
			//检测CarArrive
			CheckCarArrive(pProcessRespond, pParam->iDiffTick);
		}

		RTN_HR_IF_FAILED(pTrack->UpdatePlateImage(
			imgObservedPlate, pParam, m_iLastLightStatus, m_iCurLightStatus, pProcessRespond 
		));
	}

//	// 最亮图模式下替换最清晰图
//	RTN_HR_IF_FAILED(pTrack->UpdateBestImage(pImage));

	// State machine
	switch (pTrack->m_State)
	{
	case tsNew: 	// if the current track is new

		if (pTrack->m_fObservationExists)
		{
			pTrack->m_nMissingTime = 0;
			// perform the state transition
			if (pTrack->m_nObservedFrames >= m_pParam->g_nContFrames_EstablishTrack
				|| (pTrack->m_rgPlateInfo[0].nPlateType == PLATE_DOUBLE_GREEN && pTrack->m_nObservedFrames >= 2)
				)
			{
				pTrack->m_State=tsMoving;
			}
		}
		else
		{
			pTrack->m_nMissingTime++;
			pTrack->LastInfo().iLastCharHeight=-1;
			pTrack->LastInfo().iLastVscale=-1;
			if (pTrack->m_nMissingTime > 1)
			{
				pTrack->m_State = tsWaitRemove;
				pTrack->m_nVoteCondition = (int)NEW2END;
			}
		}
		break;
	case tsMoving:
		if(pTrack->m_fObservationExists)
		{
			pTrack->m_nMissingTime = 0;
			// 对于fast模式，只要进入end区域，就结束track
			BOOL fInEndTrackArea = !rcTrackArea.Contains(pTrack->LastInfo().rcPos);
			if (fInEndTrackArea && m_pParam->g_PlateRecogSpeed == PR_SPEED_FAST)
			{
				pTrack->m_State=tsEnd;
				pTrack->m_nVoteCondition = (int)LEAVE_TRACK_RECT;
				break;
			}
		}
		else
		{
			pTrack->m_nMissingTime++;
			pTrack->LastInfo().iLastCharHeight=-1;
			pTrack->LastInfo().iLastVscale=-1;
			BOOL fPredictInEndTrackArea = !rcTrackArea.IntersectsWith(pTrack->PredictPosition(pParam->dwImageTime, m_pParam->m_iDetectReverseEnable, m_pScaleSpeed));
			if (fPredictInEndTrackArea && pTrack->m_nMissingTime > m_pParam->g_nMissFrames_EndTrackQuick)
			{
				pTrack->m_State=tsEnd;
				pTrack->m_nVoteCondition = (int)ENDRECT_MISS;
			}
			else if (pTrack->m_nMissingTime > m_pParam->g_nMissFrames_EndTrack)
			{
				pTrack->m_State=tsEnd;
				pTrack->m_nVoteCondition = TRACK_MISS;
			}
		}
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CTrackerImpl::ShowTracks(PROCESS_ONE_FRAME_PARAM* pParam, PROCESS_ONE_FRAME_DATA* pProcessData, PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
//	if (NULL == pCurImage)
//	{
//		return S_FALSE;
//	}
// TODO:完成调试模块
/*	if (m_pInspector)
	{
		for (int i=0; i<ipTrackMax-ipTrack0; i++)
		{
			if (i<m_cTrackInfo)
			{
				CTrackInfo *pTrackInfo = m_rgTrackInfo + i;

				HV_COMPONENT_IMAGE imgBestPlate;
				RTN_HR_IF_FAILED(m_rgTrackInfo[i].GetLastPlateImage()->GetImage(&imgBestPlate));
				RTN_HR_IF_FAILED(m_pInspector->ShowComponentImage(
					ipTrack0+i, &imgBestPlate, (DEBUG_INSPECTOR_INFO *)pTrackInfo));
			}
			else
			{
				RTN_HR_IF_FAILED(m_pInspector->ShowComponentImage(
					ipTrack0+i, NULL));
			}
		}
	}
*/
	//检测CarArrive
	CheckCarArrive(pProcessRespond, pParam->iDiffTick);

	for (int i=0; i<m_cTrackInfo; i++)
	{	
		//电警下需要进行预投票,2011-8-17
		if(GetCurrentParam()->m_fUsedTrafficLight)
		{
			//判断与投票
			if(m_cfgParam.fPreVote
			&& m_rgTrackInfo[i].PreVote() 
			&& m_rgTrackInfo[i].AverageConfidence()*100 > m_iAverageConfidenceQuan
			&& m_rgTrackInfo[i].AverageFirstConfidence()*100 > m_iFirstConfidenceQuan)
			{
				for (int j = 0; j < m_cObjTrackInfo; j++)
				{
					if (m_rgObjTrackInfo[j].m_objTrackState != OBJ_TRACK_INIT 
					 && m_rgObjTrackInfo[j].MatchPlateTrackID(m_rgTrackInfo[i].m_nID))
						m_rgObjTrackInfo[j].ClearImage(pProcessRespond);
				}
			}			
		}
		//输出CarArrive
		if (m_rgTrackInfo[i].m_State == tsMoving &&
			!m_rgTrackInfo[i].m_fCarArrived)
		{
			int iTriggerPos;
			if (m_rgTrackInfo[i].LastInfo().color == PC_YELLOW)
			{
				iTriggerPos = m_iCarArrivedPosYellow;
			}
			else
			{
				iTriggerPos = m_iCarArrivedPos;
			}
			CRect rcPos = m_rgTrackInfo[i].LastInfo().rcPos;
			int iPosY = iTriggerPos * m_nHeight / 100;
			if (m_pParam->g_MovingDirection == MD_TOP2BOTTOM 
				&& rcPos.CenterPoint().y > iPosY)
			{
				//计算触发延时距离补偿
				float fltDistance = m_pScaleSpeed->CalcActureDistance(rcPos.CenterPoint(), CPoint(rcPos.CenterPoint().x, iPosY));
				fltDistance = m_iCarArrivedDelay - fltDistance;
				ProcessCarArrive(&m_rgTrackInfo[i], pParam->dwImageTime, fltDistance, pProcessRespond);
			}
			else if( m_pParam->g_MovingDirection == MD_BOTTOM2TOP 
				&& rcPos.CenterPoint().y < iPosY )
			{
				//计算触发延时距离补偿
				float fltDistance = m_pScaleSpeed->CalcActureDistance(rcPos.CenterPoint(), CPoint(rcPos.CenterPoint().x, iPosY));
				fltDistance = m_iCarArrivedDelay - fltDistance;
				ProcessCarArrive(&m_rgTrackInfo[i], pParam->dwImageTime, fltDistance, pProcessRespond);
			}
		}
		
		bool bOutputFinalResult = m_rgTrackInfo[i].PrepareOutPutFinalResult(pParam->dwImageTime);
		
		bool fDeleteObjTrack = false;
		if ( bOutputFinalResult )
		{
			if (!m_rgTrackInfo[i].m_fCarArrived)
			{
				int iTriggerPos;
				if (m_rgTrackInfo[i].LastInfo().color == PC_YELLOW)
				{
					iTriggerPos = m_iCarArrivedPosYellow;
				}
				else
				{
					iTriggerPos = m_iCarArrivedPos;
				}
				CRect rcPos = m_rgTrackInfo[i].LastInfo().rcPos;
				int iPosY = iTriggerPos * m_nHeight / 100;

				//计算触发延时距离补偿
				float fltDistance = m_pScaleSpeed->CalcActureDistance(rcPos.CenterPoint(), CPoint(rcPos.CenterPoint().x, iPosY));
				fltDistance = m_iCarArrivedDelay + fltDistance;
				ProcessCarArrive(&m_rgTrackInfo[i], pParam->dwImageTime, fltDistance, pProcessRespond);
			}
			m_rgTrackInfo[i].m_fReverseRunVoted = false;
			switch (m_pParam->g_MovingDirection)
			{
			case MD_TOP2BOTTOM:
				if (m_rgTrackInfo[i].LastInfo().rcPos.top - 
					m_rgTrackInfo[i].m_rgPlateInfo[0].rcPos.top < -(m_nHeight * m_iSpan * 0.01f))
				{
					m_rgTrackInfo[i].m_fReverseRunVoted = true;
				}
				break;
			case MD_BOTTOM2TOP:
				if (m_rgTrackInfo[i].LastInfo().rcPos.top -
					m_rgTrackInfo[i].m_rgPlateInfo[0].rcPos.top
					> m_nHeight * m_iSpan * 0.01f)
				{
					m_rgTrackInfo[i].m_fReverseRunVoted = true;
				}
				break;
			case MD_LEFT2RIGHT:
				if (m_rgTrackInfo[i].LastInfo().rcPos.left - 
					m_rgTrackInfo[i].m_rgPlateInfo[0].rcPos.left < -(m_nWidth * m_iSpan * 0.01f))
				{
					m_rgTrackInfo[i].m_fReverseRunVoted = true;
				}
				break;
			case MD_RIGHT2LEFT:
				if (m_rgTrackInfo[i].LastInfo().rcPos.left -
					m_rgTrackInfo[i].m_rgPlateInfo[0].rcPos.left
					> m_nWidth * m_iSpan * 0.01f)
				{
					m_rgTrackInfo[i].m_fReverseRunVoted = true;
				}
				break;
			}

			// Output Final Result
			RTN_HR_IF_FAILED(m_rgTrackInfo[i].Vote(
				m_rgTrackInfo[i].m_nVotedType,
				m_rgTrackInfo[i].m_nVotedColor,
				m_rgTrackInfo[i].m_rgbVotedResult));
			m_rgTrackInfo[i].m_fVoted = true;

			//判断其他路视频是否有相同的跟踪。
			for (int j = i + 1; j < m_cTrackInfo; j++)
			{
				if (m_rgTrackInfo[i].m_iVideoID == m_rgTrackInfo[j].m_iVideoID ||
					m_rgTrackInfo[j].m_fVoted)
					continue;
				if (m_rgTrackInfo[i].IsTwins(m_rgTrackInfo[j]))
				{
					m_rgTrackInfo[j].m_fVoted = true;
				}
			}

			//根据单层牌更新后五字宽度
			if (PLATE_NORMAL == m_rgTrackInfo[i].m_nVotedType ||
				PLATE_POLICE == m_rgTrackInfo[i].m_nVotedType)
			{
				int iBottomIndex;
				for (iBottomIndex = m_rgTrackInfo[i].m_cPlateInfo - 1; iBottomIndex >= 0; iBottomIndex--)
				{
					if (memcmp(m_rgTrackInfo[i].m_rgPlateInfo[iBottomIndex].rgbContent, m_rgTrackInfo[i].m_rgbVotedResult, 7) == 0)
					{
						break;
					}
				}
				int iTopIndex;
				for (iTopIndex = 0; iTopIndex < iBottomIndex; iTopIndex++)
				{
					if (memcmp(m_rgTrackInfo[i].m_rgPlateInfo[iTopIndex].rgbContent, m_rgTrackInfo[i].m_rgbVotedResult, 7) == 0)
					{
						break;
					}
				}
				if (iBottomIndex > iTopIndex)
				{
					int iTopY = m_rgTrackInfo[i].m_rgPlateInfo[iTopIndex].rcPos.top;
					int iBottomY = m_rgTrackInfo[i].m_rgPlateInfo[iBottomIndex].rcPos.top;
					int iTopWidth = m_rgTrackInfo[i].m_rgPlateInfo[iTopIndex].rgrcChar[6].right - 
						m_rgTrackInfo[i].m_rgPlateInfo[iTopIndex].rgrcChar[2].left;
					int iBottomWidth = m_rgTrackInfo[i].m_rgPlateInfo[iBottomIndex].rgrcChar[6].right - 
						m_rgTrackInfo[i].m_rgPlateInfo[iBottomIndex].rgrcChar[2].left;
					if (iBottomWidth < iTopWidth)
					{
						int iTemp = iTopY;
						iTopY = iBottomY;
						iBottomY = iTemp;
						iTemp = iTopWidth;
						iTopWidth = iBottomWidth;
						iBottomWidth = iTemp;
					}
					float fltDy = (float)(iBottomY - iTopY);
					float fltDWidth = (float)(iBottomWidth - iTopWidth);
					if (fltDy > 1.0f && fltDWidth > 1.0f)
					{
						float fltK = fltDWidth / fltDy;
						for (int j = 0; j < 39; j++)
						{
							float fltNewWidth = (j * 25 - iTopY) * fltK + iTopWidth + 0.5f;
							if (m_rgiFiveCharWidth[j] == 0)
							{
								m_rgiFiveCharWidth[j] = (int)fltNewWidth;
							}
							else
							{
								m_rgiFiveCharWidth[j] = (int)(m_rgiFiveCharWidth[j] * 0.75 + fltNewWidth * 0.25);
							}
							if (m_rgiFiveCharWidth[j] < 0)
							{
								m_rgiFiveCharWidth[j] = 0;
							}
						}
						m_rgiFiveCharWidth[39] = 1;
					}
					RTN_HR_IF_FAILED(m_rgpObjectDetector[pParam->iVideoID]->SetPlateWidth(m_rgiFiveCharWidth, 40, 25));
				}
			}

			if (MD_BOTTOM2TOP == m_pParam->g_MovingDirection)
			{
				if (0 == m_iEnableDefaultDBType)
				{
					if (PLATE_DOUBLE_MOTO == m_rgTrackInfo[i].m_nVotedType)
					{
						m_rgTrackInfo[i].m_nVotedType = PLATE_DOUBLE_YELLOW;
					}
				}
				else if (1 == m_iEnableDefaultDBType)
				{
					if (PLATE_DOUBLE_YELLOW == m_rgTrackInfo[i].m_nVotedType)
					{
						m_rgTrackInfo[i].m_nVotedType = PLATE_DOUBLE_MOTO;
					}
				}
				else
				{
					if (3 == m_iEnableDefaultDBType && !m_rgiFiveCharWidth[39])
					{
						if (m_iMiddleWidth > 0 && m_iBottomWidth > 0
							&& m_iBottomWidth - m_iMiddleWidth >= 0)
						{
							for (int j = 38; j >= 0; j--)
							{
								m_rgiFiveCharWidth[j] = m_iBottomWidth - 
									(m_iBottomWidth - m_iMiddleWidth) * 
									(m_nHeight - j * 25 - 12) * 2 / m_nHeight;
								if (m_rgiFiveCharWidth[j] < 0)
								{
									m_rgiFiveCharWidth[j] = 0;
								}
							}
						}
						m_rgiFiveCharWidth[39] = 1;
					}
					if (PLATE_DOUBLE_YELLOW == m_rgTrackInfo[i].m_nVotedType ||
						PLATE_DOUBLE_MOTO == m_rgTrackInfo[i].m_nVotedType)
					{
						if (m_nDefaultDBType)
						{
							m_rgTrackInfo[i].m_nVotedType = PLATE_DOUBLE_MOTO;
						}
						else
						{
							m_rgTrackInfo[i].m_nVotedType = PLATE_DOUBLE_YELLOW;
						}

						int iOffset, iCharWidth;
						bool fChangeType(false);
						for (int j = 0; j < m_rgTrackInfo[i].m_cPlateInfo; j++)
						{
							iOffset = (m_rgTrackInfo[i].m_rgPlateInfo[j].rcPos.bottom 
								+ m_rgTrackInfo[i].m_rgPlateInfo[j].rcPos.top) / 50;
							iCharWidth = m_rgTrackInfo[i].m_rgPlateInfo[j].rgrcChar[6].right
								- m_rgTrackInfo[i].m_rgPlateInfo[j].rgrcChar[2].left;
							if (iOffset >=0 && iOffset < 39 && iCharWidth > 0)
							{
								if (!fChangeType && m_rgiFiveCharWidth[iOffset] > 0)
								{
									fChangeType = true;
									if (iCharWidth < m_rgiFiveCharWidth[iOffset])
									{
										m_rgTrackInfo[i].m_nVotedType = PLATE_DOUBLE_MOTO;
									}
									else
									{
										m_rgTrackInfo[i].m_nVotedType = PLATE_DOUBLE_YELLOW;
									}
								}
							}
						}
					}
				}
			}

			float fltFirstCharConfidence = m_rgTrackInfo[i].AverageFirstConfidence();
			float fltAverageConfidence = m_rgTrackInfo[i].AverageConfidence();

			HV_DebugInfo( DEBUG_STR_TRACK_FILE, "\nfirst char = %f\n", fltFirstCharConfidence );
			HV_DebugInfo( DEBUG_STR_TRACK_FILE, "average = %f\n", fltAverageConfidence );

			//如果使能红绿灯抓拍，则过滤掉未过停车线静止的车辆以及静止的车辆.
			bool fMove = false;
			if( m_pParam->m_fUsedTrafficLight && m_pParam->g_MovingDirection == MD_BOTTOM2TOP )
			{
				const int MIN_REMOVE = 3;
				int iLastPos = (int)(((float)m_rgTrackInfo[i].LastInfo().rcPos.CenterPoint().y / (float)CTrackInfo::m_iHeight) * 100 + 0.5);
				int iFirstPos = (int)(((float)m_rgTrackInfo[i].m_rgPlateInfo[0].rcPos.CenterPoint().y / (float)CTrackInfo::m_iHeight) * 100 + 0.5);

				bool fIsStop = (IsCarStop(&m_rgTrackInfo[i]) || (HV_ABS(iFirstPos - iLastPos) < MIN_REMOVE));
				if( m_iOutputInLine == 0 
					&& !m_rgTrackInfo[i].m_fReverseRunVoted 
					&&  fIsStop )
				{
					fMove = true;
//						HV_Trace("Move... \n");
				}
				else if( m_iOutputInLine == 1
					&& !m_rgTrackInfo[i].m_fReverseRunVoted 
					&& ( (HV_ABS(iFirstPos - iLastPos) < MIN_REMOVE) ))
				{
					fMove = true;
//						HV_Trace("Move... \n");
				}
			}

			//added by liujie , 卡口车头条件下相似度条件设宽松
			bool fVoteSimCon = true;
			if (GetCurrentParam()->g_PlateRcogMode == PRM_HIGHWAY_HEAD || GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
			{
				fVoteSimCon = (m_rgTrackInfo[i].m_iVoteSimilarityNum >= m_pParam->g_nContFrames_EstablishTrack);
			} 
			else 
			{
				fVoteSimCon = (m_rgTrackInfo[i].m_iVoteSimilarityNum > m_pParam->g_nContFrames_EstablishTrack + 1);
			}//end added

			if (!fMove &&  
				fltAverageConfidence * 100 > m_iAverageConfidenceQuan &&
				(GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE 
				|| GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE && fltFirstCharConfidence * 100 >= 5) &&
				m_rgTrackInfo[i].m_nRecogInVaildCount <= m_cfgParam.nRecogInVaildRatioTH &&
				/*fltFirstCharConfidence * 100 > m_iFirstConfidenceQuan &&*/ fVoteSimCon)
			{
				//不是收费站模式下
				if(GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE && fltFirstCharConfidence * 100 > m_iFirstConfidenceQuan)
				{
					if (m_pLastResult && m_pLastResult->IsTwinResult(m_rgTrackInfo[i]))
					{
						//过滤相同车牌时，相应的视频检测跟踪也去掉.
						RTN_HR_IF_FAILED(m_pLastResult->UpdateResult(m_rgTrackInfo[i]));
						fDeleteObjTrack = true;
					}
					else
					{
						if( m_pParam->m_fUseTemplet )
						{
							m_rgTrackInfo[i].m_fCanOutput = true;
						}
						else
						{
							m_rgTrackInfo[i].m_fCanOutput = true;
							RTN_HR_IF_FAILED(FireCarLeftEvent(m_rgTrackInfo[i], pParam, pProcessData, pProcessRespond));

						}
					}			
				}
				//收费站工作模式下
				else if(GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
				{
					// 过滤逆行
					bool fFilterReverseRun = m_cfgParam.fFilterReverseEnable
							&& m_rgTrackInfo[i].m_cPlateInfo > 1
							&& (m_pParam->g_MovingDirection == MD_TOP2BOTTOM
									&& m_rgTrackInfo[i].LastInfo().rcPos.bottom < m_rgTrackInfo[0].LastInfo().rcPos.CenterPoint().y
							   || m_pParam->g_MovingDirection == MD_BOTTOM2TOP
									&& m_rgTrackInfo[i].LastInfo().rcPos.bottom > m_rgTrackInfo[0].LastInfo().rcPos.CenterPoint().y);

					// 过滤检测区顶部多检
					bool fFilterMultiDet = false;
					if (m_pParam->g_MovingDirection == MD_TOP2BOTTOM && m_rgTrackInfo[i].m_State == tsEnd)
					{
					    // 检测高度
						int nDetH = (m_cfgParam.cTrapArea.BottomLeftY + m_cfgParam.cTrapArea.BottomRightY
								- m_cfgParam.cTrapArea.TopLeftY - m_cfgParam.cTrapArea.TopRightY) / 2
								* m_nHeight / 100;
						// 最后一帧高度
						int nMoveH = m_rgTrackInfo[i].LastInfo().rcPos.CenterPoint().y - m_cfgParam.cTrapArea.TopLeftY * m_nHeight / 100;

						if (nMoveH < (nDetH >> 2))  // 没移动到1/4检测高度的去除
						{
							fFilterMultiDet = true;
						}
					}
					if((m_rgTrackInfo[i].m_rgbVotedResult[6] == 37 &&
						fltFirstCharConfidence * 100 < m_iFirstConfidenceQuan)
						||
						(!m_rgTrackInfo[i].m_fCarArrived && m_rgTrackInfo[i].m_nObservedFrames > 10
							&& m_rgTrackInfo[i].m_iVoteSimilarityNum * 100 < 25 * m_rgTrackInfo[i].m_nObservedFrames)
							|| fFilterReverseRun
							|| fFilterMultiDet)

					{		
						fDeleteObjTrack = true;
					}
					else
					{
						if (m_pLastResult && m_pLastResult->IsTwinResult(m_rgTrackInfo[i]))
						{
							//过滤相同车牌时，相应的视频检测跟踪也去掉.
							RTN_HR_IF_FAILED(m_pLastResult->UpdateResult(m_rgTrackInfo[i]));
							fDeleteObjTrack = true;
						}
						else
						{
							if( m_pParam->m_fUseTemplet )
							{
								m_rgTrackInfo[i].m_fCanOutput = true;
							}
							else
							{
								m_rgTrackInfo[i].m_fCanOutput = true;
								RTN_HR_IF_FAILED(FireCarLeftEvent(m_rgTrackInfo[i], pParam, pProcessData, pProcessRespond));
							}
						}
					}
					
				}
			}
			else
			{
				//收费站模式下，警牌同时首汉字平均可信度小于设置值
				if(GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE && 
					m_rgTrackInfo[i].m_rgbVotedResult[6] == 37 &&
					fltFirstCharConfidence * 100 < m_iFirstConfidenceQuan)
				{		
					fDeleteObjTrack = true;
				}
				else
				{
					/*
					//特殊类型牌强制出
					if(!m_pParam->m_fUseTemplet 
						&& m_rgTrackInfo[i].m_nVotedColor == PC_WHITE)
					{
						if (m_pLastResult && m_pLastResult->IsTwinResult(m_rgTrackInfo[i]))
						{
							//过滤相同车牌时，相应的视频检测跟踪也去掉.
							RTN_HR_IF_FAILED(m_pLastResult->UpdateResult(m_rgTrackInfo[i]));
							fDeleteObjTrack = true;
						}
						else
						{
							m_rgTrackInfo[i].m_fCanOutput = true;
							RTN_HR_IF_FAILED(FireCarLeftEvent(m_rgTrackInfo[i], pParam, pProcessData, pProcessRespond));

						}
					}
					*/
				}
			}
			if (m_pLastResult)
			{
				// 不需要Clear，目前的LastResult中只包含Header
				//m_pLastResult->Clear();
			}
			if(m_rgTrackInfo[i].m_fCanOutput == true)
			{
				m_pLastResult=&m_LastResult;
				m_pLastResult->Create(m_rgTrackInfo[i], m_pParam);

				//初始化车牌信息结构数组
				if(!m_fLastResultInited)
				{
					//初始化计数器及存放车牌信息的数组
					m_pLastResult->m_iTimeinCount = 0;
					if(m_pLastResult->m_iBlockTwinsTimeout > 0)
					{
						for(int iDetect=0; iDetect<m_iBlockTwinsTimeout*2; iDetect++)
						{
							m_pLastResult->m_detectsaver[iDetect].dw_TrackEndTime = 0;
							HV_memset(m_pLastResult->m_detectsaver[iDetect].rgContent,0,sizeof(m_pLastResult->m_detectsaver[iDetect].rgContent));
						}
					}
					else
					{
						m_pLastResult->m_detectsaver[0].dw_TrackEndTime = 0;
						HV_memset(m_pLastResult->m_detectsaver[0].rgContent,0,sizeof(m_pLastResult->m_detectsaver[0].rgContent));
					}

					//得到第一个车牌信息的记录
					DWORD32 dw_Now = m_rgTrackInfo[i].m_dwBestSnapShotRefTime;
					if(dw_Now != 0)
					{
						m_pLastResult->m_detectsaver[0].dw_TrackEndTime = dw_Now;
					}
					else
					{
						m_pLastResult->m_detectsaver[0].dw_TrackEndTime = GetSystemTick();
					}

					for(int iDet=0; iDet<8; iDet++)
					{
						m_pLastResult->m_detectsaver[0].rgContent[iDet] = m_rgTrackInfo[i].m_rgbVotedResult[iDet];//m_rgTrackInfo[i].m_rgPlateInfo[0].rgbContent[iDet];
					}

					m_pLastResult->m_iTimeinCount ++;
					m_fLastResultInited = true;
					HV_Trace(1, "\nFinalResultParam_Init...\n");
				}
			}
		}

		if (m_rgTrackInfo[i].m_fCarArrived)
		{
			for (int j = 0; j < m_cObjTrackInfo; j++)
			{
				if (!m_rgObjTrackInfo[j].m_fCarArrived &&
					m_rgObjTrackInfo[j].m_objTrackState != OBJ_TRACK_INIT &&
					m_rgObjTrackInfo[j].MatchPlateTrackID(m_rgTrackInfo[i].m_nID))
				{
					m_rgObjTrackInfo[j].m_fCarArrived = true;
					m_rgObjTrackInfo[j].m_nCarArriveTime = m_rgTrackInfo[i].m_nCarArriveTime;
					m_rgObjTrackInfo[j].m_nCarArriveRealTime = m_rgTrackInfo[i].m_nCarArriveRealTime;
				}
			}
		}
		if (m_rgTrackInfo[i].m_fCanOutput || fDeleteObjTrack)
		{
			for (int j = 0; j < m_cObjTrackInfo; j++)
			{
				if (!m_rgObjTrackInfo[j].m_fVoted &&
					m_rgObjTrackInfo[j].m_objTrackState != OBJ_TRACK_INIT &&
					m_rgObjTrackInfo[j].MatchPlateTrackID(m_rgTrackInfo[i].m_nID))
				{
					m_rgObjTrackInfo[j].m_fVoted = true;
					//增加电警情况下清除该物体跟踪的图片信息,黄国超增加,2011-08-14
					if(GetCurrentParam()->m_fUsedTrafficLight && m_cfgParam.fPreVote)
						m_rgObjTrackInfo[j].ClearImage(pProcessRespond);
				}
			}
			if (fDeleteObjTrack && GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
			{
				// 收费站模式下解除不出结果的有牌车与和它对应的无牌车的关联
				for (int j = 0; j < m_cObjTrackInfo; j++)
				{
					if (m_rgObjTrackInfo[j].m_cPlateTrackID == 1 &&
						m_rgObjTrackInfo[j].MatchPlateTrackID(m_rgTrackInfo[i].m_nID))
					{
						m_rgObjTrackInfo[j].m_cPlateTrackID = 0;
					}
				}
			}
		}

		if (m_rgTrackInfo[i].m_State == tsEnd)
		{
			if (!m_pParam->m_fUseTemplet)
			{
				m_rgTrackInfo[i].m_State = tsWaitRemove;
			}
			else if(m_pParam->m_fUseTemplet)
			{
				// 使用模版匹配(目前只在红绿色情况下)
				if (m_rgTrackInfo[i].m_fCanOutput)
				{
					if (m_rgTrackInfo[i].m_fhasTemple)
					{
						// 已经建立了模板，则进入模板跟踪状态
						m_rgTrackInfo[i].m_State = tsTemTrack;			// 对于已经出牌的，进入模版匹配跟踪状态
						m_rgTrackInfo[i].m_fTempCloseEdge = false;		// 模版是否靠近边缘
						m_rgTrackInfo[i].m_nCloseEdgeCount = 0;			// 模版靠近边缘帧计数器
					}
					else
					{
						// 没有模板，则结束跟踪
						RTN_HR_IF_FAILED(FireCarLeftEvent(m_rgTrackInfo[i], pParam, pProcessData, pProcessRespond));
						m_rgTrackInfo[i].m_State = tsWaitRemove;
					}
				}
				else
				{
					m_rgTrackInfo[i].m_State = tsWaitRemove;
				}
			}
		}
		else if (m_rgTrackInfo[i].m_State == tsTemTrackEnd)
		{
				if (m_rgTrackInfo[i].m_fCanOutput)
				{
					RTN_HR_IF_FAILED(FireCarLeftEvent(m_rgTrackInfo[i], pParam, pProcessData, pProcessRespond));
				}
				m_rgTrackInfo[i].m_State = tsWaitRemove;
		}
	}

	return S_OK;
}

HRESULT CTrackerImpl::FireCarLeftEvent(
	CTrackInfo& TrackInfo, 
	PROCESS_ONE_FRAME_PARAM* pParam, 
	PROCESS_ONE_FRAME_DATA* pProcessData, 
	PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	)
{
	//需要增加判断出牌的个数是否大于最大的个数，是则直接返回false
	if(pProcessRespond->cTrigEvent.iCarLeftCount >= MAX_EVENT_COUNT)
		return S_FALSE;

	// 设置输出车牌标志
	TrackInfo.m_fCarLefted = true;

	m_dwLastCarLeftTime = GetSystemTick();
	if(pParam->fIsCheckLightType)
	{
	    CheckLightType((int)TrackInfo.AveragePlateBrightness(), false);
	}
	//动态更新最小Variance，电警因为识别率低, 不能用动态方式
	if (m_pParam->g_PlateRecogSpeed == PR_SPEED_NORMAL 
		|| m_LightType == NIGHT
		|| GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
	{
		m_pParam->g_nDetMinStdVar = m_iMinVariance;
	}
	else
	{
		int iCurrentStdVar = TrackInfo.LastInfo().nVariance - 40;
		if (iCurrentStdVar < 1)
		{
			iCurrentStdVar = 1;
		}
		m_pParam->g_nDetMinStdVar = (int)(m_pParam->g_nDetMinStdVar * 0.95 +
			iCurrentStdVar * iCurrentStdVar * 0.05);
	}
	PROCESS_IMAGE_CORE_RESULT* pCurResult = &(pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[pProcessRespond->cTrigEvent.iCarLeftCount]);
	//zhaopy
	pCurResult->dwLastSnapShotRefTime = TrackInfo.m_dwLastSnapShotRefTime;
	pCurResult->dwBestSnapShotRefTime = TrackInfo.m_dwBestSnapShotRefTime;
	pCurResult->dwBeginCaptureRefTime = TrackInfo.m_dwBeginCaptureRefTime;
	pCurResult->dwBestCaptureRefTime = TrackInfo.m_dwBestCaptureRefTime;
	pCurResult->dwLastCaptureRefTime = TrackInfo.m_dwLastCaptureRefTime;

	pCurResult->dwTriggerTime = 0;
	if( TrackInfo.m_fHasTrigger && TrackInfo.m_dwTriggerTimeMs != 0 )
	{
		pCurResult->dwTriggerTime = TrackInfo.m_dwTriggerTimeMs;
	}

	if( m_fEnableCarArriveTrigger && TrackInfo.m_fIsTrigger && TrackInfo.m_nCarArriveRealTime != 0 )
	{
		pCurResult->dwTriggerTime = TrackInfo.m_nCarArriveRealTime;
	}
	//黄国超修改,2011-08-06
	if(!TrackInfo.m_pimgBestSnapShot)
	{
		TrackInfo.m_pimgBestSnapShot = TrackInfo.m_pimgBeginCapture;
		if(TrackInfo.m_pimgBestSnapShot)
			ImageAddRef(&pProcessRespond->cImgMemOperLog, TrackInfo.m_pimgBestSnapShot);
		TrackInfo.m_rcBestPos = TrackInfo.m_rgRect[2];
		TrackInfo.m_rgRect[0] = TrackInfo.m_rgRect[2];
		pCurResult->dwBestSnapShotRefTime = TrackInfo.m_dwBeginCaptureRefTime;
	}
	if(!TrackInfo.m_pimgLastSnapShot)
	{
		TrackInfo.m_pimgLastSnapShot = TrackInfo.m_pimgBestCapture;
		if(TrackInfo.m_pimgLastSnapShot)
			ImageAddRef(&pProcessRespond->cImgMemOperLog, TrackInfo.m_pimgLastSnapShot);		
		TrackInfo.m_rcLastPos = TrackInfo.m_rgRect[3];
		TrackInfo.m_rgRect[1] = TrackInfo.m_rgRect[3];
		pCurResult->dwLastSnapShotRefTime = TrackInfo.m_dwBestCaptureRefTime;
	}

	//增加红灯信息
	if(m_nRedLightCount < 20)
	{
		pCurResult->rcRedLightCount = m_nRedLightCount;
		for(int i = 0; i < m_nRedLightCount; i++)
		{
			pCurResult->rcRedLightPos[i].left   = m_rgRedLightRect[i].left;
			pCurResult->rcRedLightPos[i].top    = m_rgRedLightRect[i].top * 2;
			pCurResult->rcRedLightPos[i].right  = m_rgRedLightRect[i].right;
			pCurResult->rcRedLightPos[i].bottom = m_rgRedLightRect[i].bottom * 2;
		}
	}

	RESULT_IMAGE_STRUCT* pResultImage = &(pCurResult->cResultImg);
	pResultImage->pimgBestSnapShot = TrackInfo.m_pimgBestSnapShot;
	pResultImage->pimgLastSnapShot = TrackInfo.m_pimgLastSnapShot;

	// zhaopy 
	if(GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
	{
		pResultImage->pimgBeginCapture = TrackInfo.m_pimgBeginCapture;
		pResultImage->pimgBestCapture = TrackInfo.m_pimgBestCapture;
		pResultImage->pimgLastCapture = TrackInfo.m_pimgLastCapture;
	}
	else
	{
		pResultImage->pimgBeginCapture = NULL;
		pResultImage->pimgBestCapture = NULL;
		pResultImage->pimgLastCapture = NULL;
	}

	IReferenceComponentImage* pimgPlate = TrackInfo.m_rgBestPlateInfo[TrackInfo.m_nVotedType].pimgPlate;	// 小图
	bool fUseDB = false;

	if ((pimgPlate == NULL) && (PLATE_DOUBLE_MOTO == TrackInfo.m_nVotedType))
	{
		fUseDB = true;				// 使用双黄代替摩托
		pimgPlate = TrackInfo.m_rgBestPlateInfo[PLATE_DOUBLE_YELLOW].pimgPlate;	// 小图
	}

	if (pimgPlate != NULL)
	{
		// Copy小图到共享内存
		HV_COMPONENT_IMAGE imgPlate;

		if (FAILED(pimgPlate->GetImage(&imgPlate)) ||
			imgPlate.iWidth == 0 ||
			imgPlate.iHeight == 0 )
		{
			pimgPlate = NULL;
		}
		else
		{
			// 判断小图缓冲区是否足够
			bool fIsOk = true;
			int iImageBufferSize = pProcessData->rghvImageSmall[pProcessRespond->cTrigEvent.iCarLeftCount].iWidth * pProcessData->rghvImageSmall[pProcessRespond->cTrigEvent.iCarLeftCount].iHeight;

			if(imgPlate.iWidth * imgPlate.iHeight * 2 > iImageBufferSize)
			{
				fIsOk = false;
			}

			if( fIsOk )
			{
				CPersistentComponentImage imgTemp;
				if(SUCCEEDED(imgTemp.Create(HV_IMAGE_YUV_422, imgPlate.iWidth, 2*imgPlate.iHeight)))
				{
					// 使用此接口有些小图会有小横条，不确认是否是此算法的问题。现改成直接数据行的拷贝。
					//PlateResize(imgPlate, imgTemp);
					for (int i = 0; i < 3; i++)
					{
						PBYTE8 pSrc = GetHvImageData(&imgPlate, i);
						PBYTE8 pDes = GetHvImageData(&imgTemp, i);

						if (pSrc && pDes)
						{
							if( 0 == i )
							{
								for( int j = 0; j < imgPlate.iHeight; ++j )
								{
									memcpy(pDes, pSrc, imgPlate.iWidth);
									pDes += imgTemp.iStrideWidth[i];
									memcpy(pDes, pSrc, imgPlate.iWidth);
									pDes += imgTemp.iStrideWidth[i];
									pSrc += imgPlate.iStrideWidth[i];
								}
							}
							else
							{
								int iCopySize = imgPlate.iWidth / 2;
								for( int j = 0; j < imgPlate.iHeight; ++j )
								{
									memcpy(pDes, pSrc, iCopySize);
									pDes += imgTemp.iStrideWidth[i];
									memcpy(pDes, pSrc, iCopySize);
									pDes += imgTemp.iStrideWidth[i];
									pSrc += imgPlate.iStrideWidth[i];
								}
							}

						}
					}

					imgPlate = imgTemp;
				}
				for (int i = 0; i < 3; i++)
				{
					PBYTE8 pSrc = GetHvImageData(&imgPlate, i);
					PBYTE8 pDes = GetHvImageData(&pProcessData->rghvImageSmall[pProcessRespond->cTrigEvent.iCarLeftCount], i);
					if (pSrc && pDes)
					{
						memcpy(pDes, pSrc, imgPlate.iStrideWidth[i] * imgPlate.iHeight);
					}
				}

				pCurResult->nPlateWidth = imgPlate.iWidth;
				pCurResult->nPlateHeight = imgPlate.iHeight;
			}
			else
			{
				pimgPlate = NULL;
			}
		}
	}

	if (pimgPlate != NULL)
	{
		HV_COMPONENT_IMAGE imgPlate;

		if (pimgPlate->GetImage(&imgPlate) == S_OK
			&& GetHvImageData(&imgPlate, 0) != NULL)
		{
			BYTE8 rgbResizeImg[BINARY_IMAGE_WIDTH * BINARY_IMAGE_HEIGHT];
			HiVideo::CRect *prcPlatePos;
			/*prcPlatePos = &TrackInfo.m_rgBestPlateInfo[TrackInfo.m_nVotedType].rcPlatePos;
			if (fUseDB)
			{
				prcPlatePos = &TrackInfo.m_rgBestPlateInfo[PLATE_DOUBLE_YELLOW].rcPlatePos;
			}*/

			int iWidth = BINARY_IMAGE_WIDTH;
			int iHeight = BINARY_IMAGE_HEIGHT;
			int iStride = BINARY_IMAGE_WIDTH;

			// 只输出中心部分二值化图
			ScaleGrey(rgbResizeImg, iWidth, iHeight, iStride,
				GetHvImageData(&imgPlate, 0),
				imgPlate.iWidth,
				imgPlate.iHeight,
				imgPlate.iStrideWidth[0]
				);

			// 计算二值化阈值
			HiVideo::CRect rcArea(0, 0, iWidth, iHeight);
			int iThre = CalcBinaryThreByCenter(
				rgbResizeImg,
				iWidth,
				iHeight,
				iStride,
				&rcArea);

			// 根据车牌颜色设置二值化方向
			BOOL fIsWhiteChar = FALSE;
			if ((TrackInfo.m_nVotedColor == PC_BLUE) ||
				(TrackInfo.m_nVotedColor == PC_BLACK)	||
				(TrackInfo.m_nVotedColor == PC_GREEN))
			{
				fIsWhiteChar = TRUE;
			}
			GrayToBinByThre(
				fIsWhiteChar,
				rgbResizeImg,
				iWidth,
				iHeight,
				iStride,
				iThre,
				GetHvImageData(&pProcessData->rghvImageBin[pProcessRespond->cTrigEvent.iCarLeftCount], 0)
				);
		}
	}

	//计算车速
	float fltCarSpeed(0.0f);
	float fltScaleOfDistance(1.0f);
	CalcCarSpeed(fltCarSpeed, fltScaleOfDistance, TrackInfo);

	pCurResult->nDetectCrossLineEnable = m_pParam->m_iDetectCrossLineEnable;
	pCurResult->nDetectOverYellowLineEnable = m_pParam->m_iDetectOverYellowLineEnable;

	// 车辆类型
	pCurResult->nCarType = TrackInfo.m_nVotedCarType;			// 输出车型

	// 车辆尺寸
	if (m_fOutputCarSize == TRUE)
	{
		pCurResult->cCarSize.iCarWidth = TrackInfo.m_stCarInfo.fltCarW / 100;
		pCurResult->cCarSize.iCarHeight = TrackInfo.m_stCarInfo.fltCarH / 100;

		if (m_fEnableScaleSpeed)
		{
			// 具有标定
			pCurResult->cCarSize.nOutType = 1;		// 输出(米)
			if ((pCurResult->cCarSize.iCarWidth < 1.4f) 
				|| (pCurResult->cCarSize.iCarHeight < 2.7f))
			{
				float fltTempS = 1.0f;
				float fltRand = (float)(rand() % 51) / 150;
				fltTempS += fltRand;

				if (TrackInfo.m_nVotedCarType == CT_LARGE)
				{
					fltTempS *= 1.75f;
				}
				else if (TrackInfo.m_nVotedCarType == CT_MID)
				{
					fltTempS *= 1.20f;
				}

				pCurResult->cCarSize.iCarWidth = 1.5f * fltTempS;
				pCurResult->cCarSize.iCarHeight = 3.4f * fltTempS;

			}

		}
		else
		{
			pCurResult->cCarSize.nOutType = 0;			// 输出(像素)
			// 无标定
			if ((pCurResult->cCarSize.iCarWidth < m_nCarWBottom) 
				|| (pCurResult->cCarSize.iCarHeight < m_nCarLenBottom))
			{
				float fltTempS = 1.1f;
				float fltRand = (float)(rand() % 11) / 100;
				fltTempS += fltRand;

				if (TrackInfo.m_nVotedCarType == CT_LARGE)
				{
					fltTempS *= 1.75f;
				}
				else if (TrackInfo.m_nVotedCarType == CT_MID)
				{
					fltTempS *= 1.2f;
				}

				pCurResult->cCarSize.iCarWidth = m_nCarWBottom * fltTempS;
				pCurResult->cCarSize.iCarHeight = m_nCarLenBottom * fltTempS;
			}
		}
	}
	else
	{//因为arm传递进来是否输出车辆尺寸的标记会被DSP修改，通过输出类型为3来保证是否输出车辆尺寸
		pCurResult->cCarSize.nOutType = 2; 
	}

	// 结果结构赋值
	memcpy(pCurResult->rgbContent, TrackInfo.m_rgbVotedResult, 8);
	pCurResult->nType = TrackInfo.m_nVotedType;
	pCurResult->nColor = TrackInfo.m_nVotedColor;

	pCurResult->fltAverageConfidence = TrackInfo.AverageConfidence();
	pCurResult->fltFirstAverageConfidence = TrackInfo.AverageFirstConfidence();
	pCurResult->nStartFrameNo = TrackInfo.m_nStartFrameNo;
	pCurResult->nEndFrameNo = TrackInfo.m_nEndFrameNo;
	pCurResult->nFirstFrameTime = TrackInfo.m_dwFirstFlameTime;
	pCurResult->iObservedFrames = TrackInfo.m_nObservedFrames;
	pCurResult->iVotedObservedFrames = 0;
	pCurResult->nCarColor = TrackInfo.m_nVotedCarColor;
	pCurResult->fIsNight = (m_LightType == NIGHT) ? true : false;
//	pCurResult->fOutputCarColor = CTrackInfo::m_fEnableRecgCarColor;
	for( int i = 0; i < TrackInfo.m_cPlateInfo; ++i )
	{
		if( TrackInfo.m_rgPlateInfo[i].nInfoType == PI_LPR 
			&& TrackInfo.m_rgPlateInfo[i].nPlateType == TrackInfo.m_nVotedType )
		{
			pCurResult->iVotedObservedFrames++;
		}
	}
	
	if(m_pParam->m_iDetectReverseEnable)
	{
		pCurResult->fReverseRun = TrackInfo.m_fReverseRunVoted;
	}
	else
	{
		pCurResult->fReverseRun = false;
	}

	//用于判断行驶方向，检测车辆是否与车道设定方向一致
	switch ( m_TravelDirection )
	{
	case MD_TOP2BOTTOM:
		if (TrackInfo.LastInfo().rcPos.top - 
			TrackInfo.m_rgPlateInfo[0].rcPos.top < -(m_nHeight * m_iSpan * 0.01f))
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_BOTTOM2TOP:
		if (TrackInfo.LastInfo().rcPos.top -
			TrackInfo.m_rgPlateInfo[0].rcPos.top
			> m_nHeight * m_iSpan * 0.01f)
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_LEFT2RIGHT:
		if (TrackInfo.LastInfo().rcPos.left - 
			TrackInfo.m_rgPlateInfo[0].rcPos.left < -(m_nWidth * m_iSpan * 0.01f))
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_RIGHT2LEFT:
		if (TrackInfo.LastInfo().rcPos.left -
			TrackInfo.m_rgPlateInfo[0].rcPos.left
			> m_nWidth * m_iSpan * 0.01f)
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_NOTSURE:
	default:
	}

	//pCurResult->fTravelReverse =TrackInfo.m_fTravelReverse;
	
	pCurResult->nVoteCondition = TrackInfo.m_nVoteCondition;
	pCurResult->iAvgY = m_iAvgY;
	pCurResult->iCarAvgY = (int)TrackInfo.AveragePlateBrightness();
	pCurResult->iCarVariance = (int)TrackInfo.AverageStdVar();

	pCurResult->nRoadNo = TrackInfo.m_iRoad;			// 视频检测的两个参数设置为0(车道编号,设为-1)
	//pCurResult->iRoadNumberBegin =  CTrackInfo::m_iRoadNumberBegin;
	//pCurResult->iStartRoadNum = CTrackInfo::m_iStartRoadNum;
	
	pCurResult->nCarLeftCount = 0;
	pCurResult->fltCarspeed = fltCarSpeed;
	pCurResult->fltScaleOfDistance = fltScaleOfDistance;
	pCurResult->nPlateLightType = m_PlateLightType;
	pCurResult->iCplStatus = m_iCplStatus;
	pCurResult->iPulseLevel = m_iPulseLevel;
	if(pCurResult->fIsNight && pCurResult->iPulseLevel < 1)
	{
		pCurResult->iPulseLevel = 1;
	}
	
	if(pCurResult->iObservedFrames < 6 && pCurResult->fltCarspeed > 200.0f)
	{
		pCurResult->fltCarspeed = 0.0f;
	}

	if(m_iForceLightOffAtDay)
	{
		if(m_LightType == DAY && m_iPulseLevel > 0 
			&& m_iForceLightOffFlag == 1)
		{
			m_iPulseLevel = 0;
		}
	}
	if( TrackInfo.m_dwTriggerTime == 0 )
	{
		pCurResult->nCarArriveTime = TrackInfo.m_nCarArriveTime;
	}
	else
	{
		pCurResult->nCarArriveTime = TrackInfo.m_dwTriggerTime;
	}


	pCurResult->fCarIsStop = TrackInfo.m_fCarIsStop;
	pCurResult->iCarStopPassTime = TrackInfo.m_iCarStopPassTime;

	//事件检测
	pCurResult->coltIsOverYellowLine	= COLT_NO;
	pCurResult->coltIsCrossLine		= COLT_NO;
	//压黄线
	pCurResult->coltIsOverYellowLine = IsOverYellowLine(TrackInfo);
	//判断是压实线还是黄线
	if(CTrackInfo::m_ActionDetectParam.iIsYellowLine[pCurResult->coltIsOverYellowLine] == 2)
	{
		pCurResult->fIsDoubleYellowLine = true;
	}
	else
	{
		pCurResult->fIsDoubleYellowLine = false;
	}
	//越线
	pCurResult->coltIsCrossLine = IsCrossLine(TrackInfo);

	pCurResult->iUnSurePeccancy = 0;

	if( m_pParam->m_fUsedTrafficLight )
	{
		//红绿灯车道号的计算使用第一张抓拍大图的位置
		pCurResult->nRoadNo = MatchRoad(TrackInfo.m_rgPlateInfo[0].rcPos.CenterPoint().x, TrackInfo.m_rgPlateInfo[0].rcPos.CenterPoint().y);
		
		//行驶类型
		RUN_TYPE rt = RT_UNSURE;
		//如果是逆行
		if( pCurResult->fReverseRun )
		{
			rt = RT_CONVERSE;
		}
		else
		{
			rt = CheckRunType(TrackInfo.m_rgRect[2], TrackInfo.m_rectLastImage);
		}
		
		//过滤车道相关的行驶类型
		int iFilter = CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iFilterRunType;
		if( (iFilter & RRT_FORWARD) && rt == RT_FORWARD )
		{
			rt = RT_UNSURE;
		}
		else if( (iFilter & RRT_LEFT) && rt == RT_LEFT )
		{
			rt = RT_UNSURE;
		}
		else if( (iFilter & RRT_RIGHT) && rt == RT_RIGHT )
		{
			rt = RT_UNSURE;
		}
		else if( (iFilter & RRT_TURN) && rt == RT_TURN )
		{
			rt = RT_UNSURE;
		}

		pCurResult->rtType = rt;
		int iOnePosLightScene = TrackInfo.m_iOnePosLightScene;
		int iTwoPosLightScene = TrackInfo.m_iPassStopLightScene;

		if( CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iRoadType & RRT_LEFT )
		{
			iTwoPosLightScene = TrackInfo.m_iPassLeftStopLightScene;
		}
		//如果打开"过滤第三张抓拍图为绿灯的违章",则用第三线抓拍线灯的状态的作为判断的依据
		//韦开拓修改,2010-12-22
		if(m_iFilterRushPeccancy && iTwoPosLightScene != -1)
		{
			iTwoPosLightScene = TrackInfo.m_iThreePosLightScene;
		}
		//GetLightScene(iOnePosLightScene, &(pCurResult->tsOnePosScene));
		//GetLightScene(iTwoPosLightScene, &(pCurResult->tsTwoPosScene));
		m_cTrafficLight.GetLightScene(iOnePosLightScene, &pCurResult->tsOnePosScene);
		m_cTrafficLight.GetLightScene(iTwoPosLightScene, &pCurResult->tsTwoPosScene);

		int pt;
		int iRoadType = (RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
		if( pCurResult->nRoadNo != -1 )
		{
			iRoadType = CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iRoadType;
		}
		//只输出对应车道的红灯开始时间
		//pCurResult->lrtRedStart = m_redrealtime;
		m_cTrafficLight.GetRedLightStartTime(&pCurResult->lrtRedStart);
		if( (iRoadType & RRT_LEFT) == 0 ) 
		{
			pCurResult->lrtRedStart.dwLeftL = pCurResult->lrtRedStart.dwLeftH = 0;
		}
		if( (iRoadType & RRT_FORWARD) == 0 )
		{
			pCurResult->lrtRedStart.dwForwardL = pCurResult->lrtRedStart.dwForwardH = 0;
		}
		if( (iRoadType & RRT_RIGHT) == 0 ) 
		{
			pCurResult->lrtRedStart.dwRightL = pCurResult->lrtRedStart.dwRightH = 0;
		}
		if( (iRoadType & RRT_TURN) == 0 ) 
		{
			pCurResult->lrtRedStart.dwTurnL = pCurResult->lrtRedStart.dwTurnH = 0;
		}
		//如果行焕嘈褪俏粗宜诘某档乐挥幸恢址较虻模蚴涑龅男惺焕嘈臀飧龇较虻。
		if( pCurResult->rtType == RT_UNSURE && CTrackInfo::m_iRunTypeEnable == 1 )
		{
			if( iRoadType == RRT_LEFT )
			{
				pCurResult->rtType = RT_LEFT;
			}
			else if( iRoadType == RRT_FORWARD )
			{
				pCurResult->rtType = RT_FORWARD;
			}
			else if( iRoadType == RRT_RIGHT )
			{
				pCurResult->rtType = RT_RIGHT;
			}
			else if( iRoadType == RRT_TURN )
			{
				pCurResult->rtType = RT_TURN;
			}
		}
	
		bool fOverLine = false;
		if( (pCurResult->coltIsOverYellowLine != COLT_INVALID &&  pCurResult->coltIsOverYellowLine != COLT_NO)
			|| (pCurResult->coltIsCrossLine != COLT_INVALID && pCurResult->coltIsCrossLine != COLT_NO) )
		{
			fOverLine = true;
		}

		DWORD32 dwCaptureTime = (pResultImage->pimgBestCapture == NULL ? GetSystemTick() : TrackInfo.m_dwBestCaptureRefTime);

		if( S_OK != CheckPeccancyType(iOnePosLightScene, iTwoPosLightScene, rt, iRoadType, dwCaptureTime, &pt) )
		{
			pt = PT_NORMAL;
		}
		if(m_iFilterRushPeccancy  && (pt & PT_RUSH) != PT_RUSH)
		{
			int ptTmp;
			if( S_OK == CheckPeccancyType(iOnePosLightScene, TrackInfo.m_iPassStopLightScene, rt, iRoadType, dwCaptureTime, &ptTmp) )
			{
				if((ptTmp & PT_RUSH) == PT_RUSH)
				{
					HV_Trace(1, "第三抓拍图灯的状态不为红灯执行过滤\n");
				}
			}
		}

		//如果已经触发了两次相机,并且第三次没触发,则触发第三次
		if( TrackInfo.m_fFirstTrigger && TrackInfo.m_fSecondTrigger	&& !TrackInfo.m_fThirdTrigger && pt != PT_NORMAL)
		{
			//int iPosThree = CTrackInfo::m_iCaptureThreePos * CTrackInfo::m_iHeight / 100;

			if(GetSystemTick() > (DWORD32)m_sMinTriggerTimeMs)
			{
				m_dwTriggerTimes++;
				if(m_dwTriggerTimes >= 0xffff)
				{
					m_dwTriggerTimes = 1;
				}
				
				DWORD32 dwRoadNum = (m_dwTriggerTimes << 16) | pCurResult->nRoadNo;
				//m_pCallback->ThirdTrigger(m_LightType,dwRoadNum, NULL);
				m_sMinTriggerTimeMs = GetSystemTick() + 300;
				m_dwLastTriggerInfo = dwRoadNum;
				//HV_Trace(1, "firecarleft:%d,%d, %08x\n", m_dwTriggerTimes, stCarLeftInfo.nVideoDetID, dwRoadNum);
			}
			else
			{
				DWORD32 dwRoadNum = (m_dwLastTriggerInfo & 0xffff0000) | pCurResult->nRoadNo;
				//HV_Trace(1, "ThirdTrigger Readd:%08x\n", dwRoadNum);
				//m_pCallback->ReaddSignal(dwRoadNum);
			}
			TrackInfo.m_fThirdTrigger = true;
			TrackInfo.m_dwThirdTime = m_dwTriggerTimes;
			TrackInfo.m_dwThirdTimeMs = GetSystemTick();
		}

		//是否压线，有牌车才做这个判断.黄国超注释掉，只要是压线都要保持下来该违章类型,2010-10-12
		if( fOverLine )
		{
			pt |= PT_OVERLINE;
		}
		//黄国超修改，通过或得方式涵盖所有违章,2010-10-12
		pCurResult->ptType |= pt;

	//	sprintf(g_szDebugInfo, "[FireCarLeft]rtType=%d, ptType=%d, pt=%d", pCurResult->rtType, pCurResult->ptType, pt);

		if( (pResultImage->pimgBeginCapture == NULL || pResultImage->pimgBestCapture == NULL || pResultImage->pimgLastCapture == NULL )
			|| TrackInfo.m_dwBeginCaptureRefTime == TrackInfo.m_dwBestCaptureRefTime
			|| TrackInfo.m_dwBeginCaptureRefTime == TrackInfo.m_dwLastCaptureRefTime
			|| TrackInfo.m_dwBestCaptureRefTime == TrackInfo.m_dwLastCaptureRefTime 
			)
		{
			//图片不合违章规定
			pCurResult->iUnSurePeccancy = 1;
		}

		//过滤掉车道的逆行违章
		if( (pCurResult->ptType & PT_CONVERSE) == PT_CONVERSE 
			&& ( (iFilter & RRT_CONVERSE) && rt == RT_CONVERSE) )
		{
			pCurResult->ptType &= ~PT_CONVERSE;
		}
		
		//抓拍补光抓拍时间
		pCurResult->dwFirstTime = TrackInfo.m_dwFirstTime;
		pCurResult->dwSecondTime = TrackInfo.m_dwSecondTime;
		pCurResult->dwThirdTime = TrackInfo.m_dwThirdTime;
//		if(CTrackInfo::m_iFlashlightMode == 1 && m_iAvgY < CTrackInfo::m_iFlashlightThreshold)
//		{
//			HV_Trace("Tracker:s=%d:f=%d:t=%d\n", stCarLeftInfo.dwFirstTime, stCarLeftInfo.dwSecondTime, stCarLeftInfo.dwThirdTime);
//		}
	}
	else
	{
		pCurResult->rtType = RT_UNSURE;
		pCurResult->ptType = PT_NORMAL;
	}

	pCurResult->rcBestPlatePos = TrackInfo.m_rcBestPos;
	pCurResult->rcLastPlatePos = TrackInfo.m_rcLastPos;
	pCurResult->rcFirstPos = TrackInfo.m_rgRect[2];
	pCurResult->rcSecondPos = TrackInfo.m_rgRect[3];
	pCurResult->rcThirdPos = TrackInfo.m_rgRect[4];

	//校正坐标
	pCurResult->rcBestPlatePos.left   = pCurResult->rcBestPlatePos.left  * 100 / m_nWidth;
	pCurResult->rcBestPlatePos.top    = pCurResult->rcBestPlatePos.top   * 100 / m_nHeight;
	pCurResult->rcBestPlatePos.right  = pCurResult->rcBestPlatePos.right  * 100 / m_nWidth;
	pCurResult->rcBestPlatePos.bottom = pCurResult->rcBestPlatePos.bottom * 100 / m_nHeight;

	pCurResult->rcLastPlatePos.left   = pCurResult->rcLastPlatePos.left  * 100 / m_nWidth;
	pCurResult->rcLastPlatePos.top    = pCurResult->rcLastPlatePos.top   * 100 / m_nHeight;
	pCurResult->rcLastPlatePos.right  = pCurResult->rcLastPlatePos.right  * 100 / m_nWidth;
	pCurResult->rcLastPlatePos.bottom = pCurResult->rcLastPlatePos.bottom * 100 / m_nHeight;

	pCurResult->rcFirstPos.left   = pCurResult->rcFirstPos.left  * 100 / m_nWidth;
	pCurResult->rcFirstPos.top    = pCurResult->rcFirstPos.top   * 100 / m_nHeight;
	pCurResult->rcFirstPos.right  = pCurResult->rcFirstPos.right  * 100 / m_nWidth;
	pCurResult->rcFirstPos.bottom = pCurResult->rcFirstPos.bottom * 100 / m_nHeight;

	pCurResult->rcSecondPos.left   = pCurResult->rcSecondPos.left  * 100 / m_nWidth;
	pCurResult->rcSecondPos.top    = pCurResult->rcSecondPos.top   * 100 / m_nHeight;
	pCurResult->rcSecondPos.right  = pCurResult->rcSecondPos.right  * 100 / m_nWidth;
	pCurResult->rcSecondPos.bottom = pCurResult->rcSecondPos.bottom * 100 / m_nHeight;

	pCurResult->rcThirdPos.left   = pCurResult->rcThirdPos.left  * 100 / m_nWidth;
	pCurResult->rcThirdPos.top    = pCurResult->rcThirdPos.top   * 100 / m_nHeight;
	pCurResult->rcThirdPos.right  = pCurResult->rcThirdPos.right  * 100 / m_nWidth;
	pCurResult->rcThirdPos.bottom = pCurResult->rcThirdPos.bottom * 100 / m_nHeight;
	//抓拍标志
	if (TrackInfo.m_fIsTrigger)
	{
		pCurResult->dwTriggerIndex = TrackInfo.m_dwTriggerIndex;
		pCurResult->iCapCount = TrackInfo.m_iCapCount;
		pCurResult->iCapSpeed = TrackInfo.m_iCapSpeed;
	}

	// 厦门项目修改车辆类型
	if (m_iOutPutType == 1)
	{
		if ((pCurResult->nCarType == CT_SMALL)
			|| (pCurResult->nCarType == CT_MID)
			|| (pCurResult->nCarType == CT_LARGE))
		{
			pCurResult->nCarType = CT_VEHICLE;
		}
	}

//	RTN_HR_IF_FAILED( m_pCallback->CarLeft(
//		&stCarLeftInfo,
//		TrackInfo.m_pimgBestSnapShot->GetFrameName() ));

	pProcessRespond->cTrigEvent.dwEventId |= EVENT_CARLEFT;
	pProcessRespond->cTrigEvent.iCarLeftCount++;

	m_leftCar[pCurResult->nRoadNo].car_plate = (PLATE_COLOR)(pCurResult->nColor);
	m_leftCar[pCurResult->nRoadNo].frame_no = pParam->dwFrameNo;

	return S_OK;
}

HRESULT CTrackerImpl::ForceResult(int iFlag /*= 0 */)
{
/*	HV_Trace("\n*S*");

	CARARRIVE_INFO_STRUCT carArriveInfo;
	carArriveInfo.iPlateLightType = m_PlateLightType;
	carArriveInfo.dwIsOutTrig = 1;
	carArriveInfo.dwCarArriveTime = GetSystemTick();
	RTN_HR_IF_FAILED(m_pCallback->CarArrive(
		&carArriveInfo,
		NULL
		));

	int iBestPlate(-1);
	for (int i=0; i<m_cTrackInfo; i++)
	{
		if (m_rgTrackInfo[i].m_State == tsMoving ||
			m_rgTrackInfo[i].m_State == tsEnd)
		{
			RTN_HR_IF_FAILED(m_rgTrackInfo[i].Vote(
				m_rgTrackInfo[i].m_nVotedType,
				m_rgTrackInfo[i].m_nVotedColor,
				m_rgTrackInfo[i].m_rgbVotedResult));
			m_rgTrackInfo[i].m_fVoted = true;

			if (iBestPlate<0 ||
				m_rgTrackInfo[iBestPlate].AverageConfidence()<m_rgTrackInfo[i].AverageConfidence())
			{
				iBestPlate = i;
			}
		}
	}

	if (iBestPlate>=0)
	{
		if (!m_rgTrackInfo[iBestPlate].m_fCarArrived || m_rgTrackInfo[iBestPlate].m_fCarLefted)
		{
			CARARRIVE_INFO_STRUCT carArriveInfo;
			m_rgTrackInfo[iBestPlate].m_iRoad = MatchRoad(m_rgTrackInfo[iBestPlate].LastInfo().rcPos.CenterPoint().x, m_rgTrackInfo[iBestPlate].LastInfo().rcPos.CenterPoint().y);
			carArriveInfo.iRoadNumber = m_rgTrackInfo[iBestPlate].m_iRoad;
			carArriveInfo.iPlateLightType = m_PlateLightType;
			RTN_HR_IF_FAILED(m_pCallback->CarArrive(
				&carArriveInfo,
				NULL
				));
			m_rgTrackInfo[iBestPlate].m_fCarArrived = true;
			m_rgTrackInfo[iBestPlate].m_nCarArriveTime = GetSystemTick();
			m_rgTrackInfo[iBestPlate].m_rcCarArrivePos = m_rgTrackInfo[iBestPlate].LastInfo().rcPos;
		}
		RTN_HR_IF_FAILED(FireCarLeftEvent(m_rgTrackInfo[iBestPlate]));
	}
	return (iBestPlate>=0 ? S_OK : S_FALSE);*/
	return S_FALSE;
}


HRESULT CTrackerImpl::GetTrackCount(PDWORD32 pdwCount)
{
	if (pdwCount)
	{
		*pdwCount = m_cTrackInfo;
	}
	else
	{
		return E_POINTER;
	}

	return S_OK;
}

/*
HRESULT CTrackerImpl::GetPlatePos(
	DWORD32 dwIndex,
	DWORD32 *pdwLeft,
	DWORD32 *pdwTop,
	DWORD32 *pdwRight,
	DWORD32 *pdwBottom
)
{
	if ( dwIndex >= ( DWORD32 )m_cTrackInfo )
	{
		return E_FAIL;
	}

	if (m_rgTrackInfo[ dwIndex ].m_fObservationExists)
	{
		HiVideo::CRect& rect = m_rgTrackInfo[ dwIndex ].LastInfo().rcPos;
		*pdwLeft = rect.left;
		*pdwTop = rect.top;
		*pdwRight = rect.right;
		*pdwBottom = rect.bottom;
	}
	else
	{
		*pdwLeft = 0;
		*pdwTop = 0;
		*pdwRight = 0;
		*pdwBottom = 0;
	}

	return S_OK;
}
*/

HRESULT CTrackerImpl::GetPlatePos(
				  DWORD32 dwIndex,
				  CRect *prcPlate,
				  CRect *prcRect
				  )
{
	if (dwIndex >= ( DWORD32 )m_cTrackInfo ||
		prcPlate == NULL)
	{
		return E_FAIL;
	}

	bool fHasTrack(true);
	if (!m_pParam->m_fUseTemplet)
	{
		// 未使用模版匹配，则车牌跟踪是唯一标准
		fHasTrack = m_rgTrackInfo[ dwIndex ].m_fObservationExists;
	}
	else
	{
		// 使用了模版匹配，则车牌跟踪和模版匹配具备一个就有结果
		fHasTrack =  (m_rgTrackInfo[ dwIndex ].m_fObservationExists)
			|| (m_rgTrackInfo[ dwIndex ].m_fTempTrackExists);

	}

	if (fHasTrack)
	{
		*prcPlate = m_rgTrackInfo[ dwIndex ].LastInfo().rcPos;
		*prcRect = m_rgTrackInfo[ dwIndex ].m_rcPredict;
	}
	else
	{
		*prcPlate = CRect(-1, -1, -1, -1);
		*prcRect = CRect(-1, -1, -1, -1);
		return S_FALSE;
	}

	return S_OK;
}

HRESULT CTrackerImpl::GetDetectCount(PDWORD32 pdwCount)
{
	if (pdwCount == NULL)
	{
		return E_POINTER;
	}
	*pdwCount = m_cDetected;
	return S_OK;
}

HRESULT CTrackerImpl::GetDetectPos(DWORD32 dwIndex, CRect *prcDetected)
{
	if (dwIndex >= (DWORD32)m_cDetected ||
		prcDetected == NULL)
	{
		return E_FAIL;
	}
	*prcDetected = m_rgrcDetected[dwIndex];
	return S_OK;
}

HRESULT CTrackerImpl::GetObjCount(PDWORD32 pdwCount)
{
	if (pdwCount == NULL)
	{
		return E_POINTER;
	}
	*pdwCount = m_cObjTrackInfo;
	return S_OK;
}

HRESULT CTrackerImpl::GetObjPos(DWORD32 dwIndex, CRect *prcObj)
{
	if (dwIndex >= (DWORD32)m_cObjTrackInfo ||
		prcObj == NULL)
	{
		return E_FAIL;
	}
	if (m_rgObjTrackInfo[dwIndex].m_fTrack)
	{
		*prcObj = m_rgObjTrackInfo[dwIndex].LastInfo().m_rcFG;
	}
	else
	{
		*prcObj = CRect(-1, -1, -1, -1);
	}
	return S_OK;
}

HRESULT CTrackerImpl::GetEPObjCount(PDWORD32 pdwCount)
{
	if (pdwCount == NULL)
	{
		return E_POINTER;
	}
	*pdwCount = m_cEPObjTrackInfo;
	return S_OK;
}

HRESULT CTrackerImpl::GetEPObjPos(DWORD32 dwIndex, CRect *prcObj)
{
	if (dwIndex >= (DWORD32)m_cEPObjTrackInfo ||
		prcObj == NULL)
	{
		return E_FAIL;
	}
	if (m_rgEPObjTrackInfo[dwIndex].m_fTrack)
	{
		*prcObj = m_rgEPObjTrackInfo[dwIndex].LastInfo().m_rcFG;
	}
	else
	{
		*prcObj = CRect(-1, -1, -1, -1);
	}
	return S_OK;
}

HRESULT CTrackerImpl::ClearTrackInfo(PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	for (int i=0; i<m_cTrackInfo; i++)
	{
		m_rgTrackInfo[i].Clear(pProcessRespond);
	}
	m_cTrackInfo = 0;
	for (int i = 0; i < m_cObjTrackInfo; i++)
	{
		m_rgObjTrackInfo[i].Clear(pProcessRespond);
	}
	m_cObjTrackInfo = 0;
	for (int i = 0; i < s_knMaxVideoChannel; i++)
	{
		if (m_rgpObjectDetector[i])
		{
			m_rgpObjectDetector[i]->ClearBackgroup();
		}
	}
	m_cDetected = 0;
	return S_OK;
}

HRESULT CTrackerImpl::IsVoted(DWORD32 dwIndex, bool *pfVoted)
{
	if ( dwIndex >= ( DWORD32 )m_cTrackInfo ) return E_FAIL;
	*pfVoted = m_rgTrackInfo[ dwIndex ].m_fVoted;
	return S_OK;
}

const int TEST_DETECT_TYPE = 0;
const int TEST_SEGMENT_TYPE = 1;
const int TEST_RECOGNITION_TYPE = 2;
int g_iCoreTestType = TEST_DETECT_TYPE;		//测试的模块类型

const int DET_TEST_BLUE = 0;
const int DET_TEST_YELLOW = 1;
const int DET_TEST_DYELLOW = 2;
int g_iDetTestType = DET_TEST_YELLOW;			//检测模块测试类型
int g_iDetTestFastScanFlag = (int)FALSE;		//检测模块的SubScan标志
int g_iDetTestColor = (int)PC_UNKNOWN;			//检测模块的车牌颜色预测
int g_iDetTestLastWidth = 0;					//检测模块的车牌宽度预测

int g_iSegRecTestColor = (int)PC_UNKNOWN;		//分割模块的车牌颜色参数
int g_iSegRecTestType = (int)PLATE_UNKNOWN;		//分割模块的车牌类型参数


HRESULT CTrackerImpl::TestDetPerformance(
	HV_COMPONENT_IMAGE *phciTest,	//测试图像
	char *pszReport,				//测试报告
	DWORD32 dwReportBufSize,		//测试报告缓冲区长度
	DWORD32 dwMaxTestCount,			//最大测试次数
	DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
)
{
	strcat(pszReport, "检测模块测试数据\n");
	if (m_pPlateDetector == NULL)
	{
		strcat(pszReport, "\t检测模块未初始化!\n");
		return E_OBJ_NO_INIT;
	}
	DWORD32 dwCount = 0;
	DWORD32 dwBeginTime = GetSystemTick();
	DWORD32 dwEndTime;
	HRESULT hr;
	MULTI_ROI MultiROI =
	{
		ARRSIZE(m_rgTempPlatePos),
		m_rgTempPlatePos
	};
	switch (g_iDetTestType)
	{
		case DET_TEST_BLUE:
			do {
				MultiROI.dwNumROI = ARRSIZE(m_rgTempPlatePos);
				hr = m_pPlateDetector->ScanPlatesComponentImage(
					PLATE_BLUE_DETECTOR,
					phciTest,
					&MultiROI,
					m_pParam->g_nDetMinScaleNum,
					m_pParam->g_nDetMaxScaleNum,
					g_iDetTestFastScanFlag,
					(PLATE_COLOR)g_iDetTestColor,
					g_iDetTestLastWidth);
				if (hr != S_OK)
				{
					goto RETURN_REPORT;
				}
				dwEndTime = GetSystemTick();
				dwCount++;
				SystemDog();
			} while (dwCount < dwMaxTestCount &&
					 dwEndTime - dwBeginTime < dwMaxTestDuration);
			break;
		case DET_TEST_YELLOW:
			do {
				MultiROI.dwNumROI = ARRSIZE(m_rgTempPlatePos);
				hr = m_pPlateDetector->ScanPlatesComponentImage(
					PLATE_YELLOW_DETECTOR,
					phciTest,
					&MultiROI,
					m_pParam->g_nDetMinScaleNum,
					m_pParam->g_nDetMaxScaleNum,
					g_iDetTestFastScanFlag,
					(PLATE_COLOR)g_iDetTestColor,
					g_iDetTestLastWidth);
				if (hr != S_OK)
				{
					goto RETURN_REPORT;
				}
				dwEndTime = GetSystemTick();
				dwCount++;
				SystemDog();
			} while (dwCount < dwMaxTestCount &&
					 dwEndTime - dwBeginTime < dwMaxTestDuration);
			break;
		case DET_TEST_DYELLOW:
			do {
				MultiROI.dwNumROI = ARRSIZE(m_rgTempPlatePos);
				hr = m_pPlateDetector->ScanPlatesComponentImage(
					PLATE_YELLOW2_DETECTOR,
					phciTest,
					&MultiROI,
					m_pParam->g_nDetMinScaleNum,
					m_pParam->g_nDetMaxScaleNum,
					g_iDetTestFastScanFlag,
					(PLATE_COLOR)g_iDetTestColor,
					g_iDetTestLastWidth);
				if (hr != S_OK)
				{
					goto RETURN_REPORT;
				}
				dwEndTime = GetSystemTick();
				dwCount++;
				SystemDog();
			} while (dwCount < dwMaxTestCount &&
					 dwEndTime - dwBeginTime < dwMaxTestDuration);
			break;
		default:
			break;
	}

RETURN_REPORT:
	char szBuffer[200];
	sprintf(szBuffer, "\thr = 0x%08X\n", hr);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tROI_Count = %d\n", MultiROI.dwNumROI);
	strcat(pszReport, szBuffer);
	for (int i = 0; i < MultiROI.dwNumROI; i++)
	{
		sprintf(szBuffer, "\t\tROI left=%d, top=%d, right=%d, bottom=%d, color=%d, type=%d\n",
			MultiROI.rcROI[i].left, MultiROI.rcROI[i].top, MultiROI.rcROI[i].right, MultiROI.rcROI[i].bottom,
			MultiROI.rcROI[i].color, MultiROI.rcROI[i].nPlateType);
		strcat(pszReport, szBuffer);
	}
	sprintf(szBuffer, "\tg_iCoreTestType = %d\n", g_iCoreTestType);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tg_iDetTestType = %d\n", g_iDetTestType);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tg_iDetTestFastScanFlag = %d\n", g_iDetTestFastScanFlag);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tg_iDetTestColor = %d\n", g_iDetTestColor);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tg_iDetTestLastWidth = %d\n", g_iDetTestLastWidth);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tTotal Duration = %d\n", dwEndTime - dwBeginTime);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tTest Count = %d\n", dwCount);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tAverage Time = %d\n", (dwEndTime - dwBeginTime) / dwCount);
	strcat(pszReport, szBuffer);
	return hr;
}

/*
HRESULT CTrackerImpl::TestSegPerformance(
	HV_COMPONENT_IMAGE *phciTest,	//测试图像
	char *pszReport,				//测试报告
	DWORD32 dwReportBufSize,		//测试报告缓冲区长度
	DWORD32 dwMaxTestCount,			//最大测试次数
	DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
)
{
	strcat(pszReport, "分割识别模块测试数据:\n");
	DWORD32 dwCount = 0;
	DWORD32 dwBeginTime = GetSystemTick();
	DWORD32 dwEndTime;
	HRESULT hr;
	PlateInfo plateNew;
	do {
		plateNew.color = (PLATE_COLOR)g_iSegRecTestColor;
		plateNew.nPlateType = (PLATE_TYPE)g_iSegRecTestType;
		plateNew.iLastDetWidth = phciTest->iWidth;
		CPersistentComponentImage imgRectified;
		hr = RecognizePlate(*phciTest, plateNew, imgRectified);
		if (hr != S_OK)
		{
			goto RETURN_REPORT;
		}
		dwEndTime = GetSystemTick();
		dwCount++;
		SystemDog();
	} while (dwCount < dwMaxTestCount &&
			 dwEndTime - dwBeginTime < dwMaxTestDuration);

RETURN_REPORT:
	char szBuffer[200];
	sprintf(szBuffer, "\thr = 0x%08X\n", hr);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tg_iSegRecTestColor = %d\n", g_iSegRecTestColor);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tg_iSegRecTestType = %d\n", g_iSegRecTestType);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tTotalConf=%f, Type=%d, Color=%d\n",
		plateNew.fltTotalConfidence.ToFloat(),
		plateNew.nPlateType, plateNew.color
		);
	strcat(pszReport, szBuffer);
	char szPlate[20] = {0};
	GetPlateNameAlpha(
		szPlate,
		plateNew.nPlateType,
		plateNew.color,
		plateNew.rgbContent);
	sprintf(szBuffer, "\tPlateNum=%s\n", szPlate);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tTotal Duration = %d\n", dwEndTime - dwBeginTime);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tTest Count = %d\n", dwCount);
	strcat(pszReport, szBuffer);
	sprintf(szBuffer, "\tAverage Time = %d\n", (dwEndTime - dwBeginTime) / dwCount);
	strcat(pszReport, szBuffer);
	return hr;
}
*/

//测试检测模块的性能，测试的模块类型和测试参数通过全局参数列表设置,
//全局参数列表通过INI文件加载.
HRESULT CTrackerImpl::TestPerformance(
	HV_COMPONENT_IMAGE *phciTest,	//测试数据
	char *pszReport,				//测试报告
	DWORD32 dwReportBufSize,		//测试报告缓冲区长度
	DWORD32 dwMaxTestCount,			//最大测试次数
	DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
)
{
	return E_NOTIMPL;
/*
	if (phciTest == NULL || pszReport == NULL)
	{
		return E_POINTER;
	}
	if (dwReportBufSize == 0 ||
		dwMaxTestCount == 0 && dwMaxTestDuration == 0)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = S_OK;
	switch (g_iCoreTestType)
	{
		case TEST_DETECT_TYPE:
			hr = TestDetPerformance(
				phciTest,
				pszReport,
				dwReportBufSize,
				dwMaxTestCount,
				dwMaxTestDuration
			);
			break;
		case TEST_SEGMENT_TYPE:
			hr = TestSegPerformance(
				phciTest,
				pszReport,
				dwReportBufSize,
				dwMaxTestCount,
				dwMaxTestDuration
			);
			break;
		case TEST_RECOGNITION_TYPE:
		default:
			break;
	}
	return hr;*/
}


HRESULT CTrackerImpl::GetLightType(LIGHT_TYPE* pLightType)
{
	if (NULL == pLightType) return E_POINTER;
	*pLightType = m_PlateLightType;
	return S_OK;
}

HRESULT CTrackerImpl::CheckLight()
{
	const int FIRST_CHECK_COUNT = 5;
	const int FIRST_COUNT_THRESHOLD = 3;
	const int MAX_CHECK_COUNT = 1000;
	const int COUNT_THRESHOLD = 900;

	static bool fFirstTime = true;
	static int iFrameCount = 0;
	static int iPositiveDayCount = 0;
	static int iPositiveNightCount = 0;
	static int iPositiveLightOff = 0;
	static int iPositiveLightOn = 0;
	static int iSuccessCount = 0;

	iFrameCount++;
	if (fFirstTime)
	{
		if (iSuccessCount < 2)
		{
			iFrameCount = 0;
			if (m_nFirstLightType >= 0 && m_nFirstLightType < LIGHT_TYPE_COUNT)
			{
				m_PlateLightType = m_nFirstLightType;
				iSuccessCount = 2;
//				HV_Trace("\nUse last PlateLightType");
			}
			else
			{
				if (CheckLightType(m_iAvgY, true) == S_OK)
				{
					iSuccessCount++;
				}
			}
		}
		else
		{
			if ( iFrameCount >= FIRST_CHECK_COUNT )
			{
				fFirstTime = false;
				if (iPositiveNightCount >= FIRST_COUNT_THRESHOLD)
				{
					m_LightType = NIGHT;
				}
				else
				{
					m_LightType = DAY;
				}

				if(iPositiveLightOff >= FIRST_COUNT_THRESHOLD)
				{
					m_iForceLightOffFlag = 1;
				}
				if(iPositiveLightOn >= FIRST_COUNT_THRESHOLD)
				{
					m_iForceLightOffFlag = 0;
				}
				// 判断是否需要使能偏光镜
				// 白天模式并且车牌亮度等级小于中间值,使能偏光镜
				if (DAY == m_LightType && m_PlateLightType <= m_nLightTypeCpl)
				{
					if (1 != m_iCplStatus && 1 == m_iCtrlCpl)
					{
						m_iCplStatus = 1;
						m_iAdjustForAvgBrigtness = 1;
						m_dwAdjustTick = GetSystemTick();
						m_iForceLightOffFlag = 1;
					}
				}
//				HV_Trace("\nFirstTime end...\nLightType=%d, PlateLightType=%d", m_LightType, m_PlateLightType);
			}
			else
			{
				if (m_iAvgY <= m_iNightThreshold)
				{
					iPositiveNightCount++;
				}
				else
				{
					iPositiveDayCount++;
				}

				if(m_iAvgY <= m_iForceLightOffThreshold)
				{
					iPositiveLightOn++;
				}
				else
				{
					iPositiveLightOff++;
				}
			}
		}
	}
	else
	{
		// 5分钟内没结果，才通过环境亮度判断
		if (GetSystemTick() - m_dwLastCarLeftTime > 300000)
		{
			CheckLightType(m_iAvgY, true);
		}

		// 偏光镜状态有变化时，通过环境亮度判断一次
		// 偏光镜状态调整完成实际需要500ms时间
		if (m_iAdjustForAvgBrigtness)// && GetSystemTick() - m_dwAdjustTick >= 500)
		{
			CheckLightType(m_iAvgY, true);	
		}

		if ( m_iAvgY > m_iNightThreshold )
		{
			iPositiveDayCount++;
		}
		else
		{
			iPositiveNightCount++;
		}

		if(m_iAvgY <= m_iForceLightOffThreshold)
		{
			iPositiveLightOn++;
		}
		else
		{
			iPositiveLightOff++;
		}

		if ( iFrameCount >= MAX_CHECK_COUNT )
		{
			if ( iPositiveDayCount >= COUNT_THRESHOLD )
			{
				m_LightType = DAY;
			}
			if ( iPositiveNightCount >= COUNT_THRESHOLD )
			{
				m_LightType = NIGHT;
			}

			if(iPositiveLightOn >= COUNT_THRESHOLD)
			{
				m_iForceLightOffFlag = 0;
			}
			if(iPositiveLightOff >= COUNT_THRESHOLD)
			{
				m_iForceLightOffFlag = 1;
			}
//			HV_Trace("\nLightType=%d, PlateLightType=%d", m_LightType, m_PlateLightType);
			iFrameCount = 0;
			iPositiveDayCount = 0;
			iPositiveNightCount = 0;
			iPositiveLightOn = 0;
			iPositiveLightOff = 0;
		}

		// 判断是否需要使能偏光镜
		// 白天模式并且车牌亮度等级小于中间值,使能偏光镜
		if (DAY == m_LightType && m_PlateLightType <= m_nLightTypeCpl)
		{
			if (1 != m_iCplStatus && 1 == m_iCtrlCpl)
			{
				m_iCplStatus = 1;
				m_iAdjustForAvgBrigtness = 1;
				m_dwAdjustTick = GetSystemTick();
				m_iForceLightOffFlag = 1;
			}
		}

		if(NIGHT == m_LightType)
		{
			if (0 != m_iCplStatus && 1 == m_iCtrlCpl)
			{
				m_iCplStatus = 0;
				m_iAdjustForAvgBrigtness = 1;
				m_dwAdjustTick = GetSystemTick();
				m_iForceLightOffFlag = 0;
			}
		}
	}
	if(m_iForceLightOffAtDay)
	{
		if(m_LightType == DAY && m_iPulseLevel > 0 
			&& m_iForceLightOffFlag == 1)
		{
			m_iPulseLevel = 0;
		}
	}
	return S_OK;
}

// 
HRESULT CTrackerImpl::CheckLightType(int iCarY, bool fIsAvgBrightness)
{
	HRESULT hr = S_FALSE;
	static int iFrameCount = 0;
	static int iSumBrightness = 0;
	static int s_iMinValue = 255;
	static int s_iMaxValue = 0;
	static int s_iSucessCount = 0;
	static int s_iFirstSetLightType = 0;
	if((m_nFirstLightType >= 0 && m_nFirstLightType < LIGHT_TYPE_COUNT) || s_iSucessCount >= 2)
	{

		if(s_iFirstSetLightType != 1)
		{
			s_iFirstSetLightType = 1;
		}
		s_iSucessCount = 0;
	}

	// 摄像机设置完参数后需要响应时洌实却?S后才检测下一帧亮度
	static DWORD32 s_dwLastSetParamTick = GetSystemTick();
	DWORD32 dwTimeInterval = m_cfgParam.iPlatform == 0 ? 6000 : 6000;
	if (GetSystemTick() - s_dwLastSetParamTick < dwTimeInterval && fIsAvgBrightness)
	{
		if(m_iForceLightOffAtDay)
		{
			if(m_LightType == DAY && m_iPulseLevel > 0 
				&& m_iForceLightOffFlag == 1)
			{
				m_iPulseLevel = 0;
			}
		}
		return S_FALSE;
	}

	iFrameCount++;
	iSumBrightness += iCarY;

	int iMaxBrightness = 0;
	int iMinBrightness = 0;

	iMaxBrightness = m_iMaxPlateBrightness;
	iMinBrightness = m_iMinPlateBrightness;

	if (iCarY > s_iMaxValue)
	{
		s_iMaxValue = iCarY;
	}
	if (iCarY < s_iMinValue)
	{
		s_iMinValue = iCarY;
	}

	if (iFrameCount >= m_iPlateLightCheckCount && iFrameCount > 2)
	{
		int iAvgBrightness = (iSumBrightness - s_iMaxValue - s_iMinValue) / (iFrameCount - 2);
		if (iAvgBrightness < iMinBrightness && m_PlateLightType < LIGHT_TYPE_COUNT)
		{
			if (m_PlateLightType < LIGHT_TYPE_COUNT - 1)
			{
				m_PlateLightType = (LIGHT_TYPE)((int)m_PlateLightType + 1);
			}
			else
			{
				// 如亮度级别不能再提高，则关掉偏光镜
				if (0 != m_iCplStatus && 1 == m_iCtrlCpl)
				{
					m_iCplStatus = 0;
					m_iAdjustForAvgBrigtness = 1;
					m_dwAdjustTick = GetSystemTick();
				}
				else if( m_iUsedLight == 1 && m_iPulseLevel < 8)
				{
					if(s_iFirstSetLightType == 0)
					{
						s_iFirstSetLightType = 1;
						m_iPulseLevel = 6;
					}
					else
					{
						m_iPulseLevel++;
					}
					m_iAdjustForAvgBrigtness = 0;
					hr = S_OK;
					//通过环境亮度检测时灯这度不能达到最大等级
					if(fIsAvgBrightness && m_iPulseLevel > 6)
					{
						m_iPulseLevel = 6;
					}
				}
				else
				{
					m_iAdjustForAvgBrigtness = 0;
					hr = S_OK;
				}
			}
		}
		else if (iAvgBrightness > iMaxBrightness
				&& (m_iUsedLight == 1 && m_iPulseLevel > 0) )
		{
			m_iPulseLevel--;
			m_iAdjustForAvgBrigtness = 0;
			hr = S_OK;
		}
		else if(iAvgBrightness > iMaxBrightness && DAY == m_LightType
			&& m_iCtrlCpl == 1 && m_PlateLightType <= m_nLightTypeCpl && m_iCplStatus != 1)
		{
			m_iCplStatus = 1;
			m_iAdjustForAvgBrigtness = 1;
			m_dwAdjustTick = GetSystemTick();
		}
		else if (iAvgBrightness > iMaxBrightness && m_PlateLightType > 0)
		{
			m_PlateLightType = (LIGHT_TYPE)((int)m_PlateLightType - 1);
		}
		else
		{
			m_iAdjustForAvgBrigtness = 0;
			hr = S_OK;
		}
		s_dwLastSetParamTick = GetSystemTick();
		iFrameCount = 0;
		iSumBrightness = 0;
		s_iMinValue = 255;
		s_iMaxValue = 0;
	}
	if(hr == S_OK)
	{
		s_iSucessCount++;
	}
	if(m_iForceLightOffAtDay)
	{
		if(m_LightType == DAY && m_iPulseLevel > 0 
			&& m_iForceLightOffFlag == 1)
		{
			m_iPulseLevel = 0;
		}
	}
	return hr;
}

HRESULT CTrackerImpl::RemoveInvalidTracks(PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	int iDist;

	iDist = 0;
	for (int i=0; i<m_cTrackInfo; i++)
	{
		if (m_rgTrackInfo[i].m_State == tsWaitRemove)
		{
			m_rgTrackInfo[i].Clear(pProcessRespond); //这里曾经出错过。在释放m_pimgLastCapture指针时，m_pimgLastCaptureе刚?
			iDist++;
		}
		else
		{
			if (iDist>0)
			{
				RTN_HR_IF_FAILED(m_rgTrackInfo[i].Detach(m_rgTrackInfo[i-iDist], pProcessRespond));
			}
		}
	}
	m_cTrackInfo-=iDist;

	iDist = 0;
	for (int i=0; i<m_cObjTrackInfo; i++)
	{
		if (m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_END ||
			m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_INIT)
		{
			m_rgObjTrackInfo[i].Clear(pProcessRespond);
			iDist++;
		}
		else
		{
			if (iDist > 0)
			{
				m_rgObjTrackInfo[i - iDist].Assign(m_rgObjTrackInfo[i], pProcessRespond);
				m_rgObjTrackInfo[i].Clear(pProcessRespond);
			}
		}
	}
	m_cObjTrackInfo -= iDist;
	//EP track faults
    iDist = 0;
    for (int i=0; i<m_cEPObjTrackInfo; i++)
    {
        if (m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_END ||
            m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_INIT)
        {
            m_rgEPObjTrackInfo[i].Clear(pProcessRespond);
            //HV_Trace("\nclear id:[%d]",m_rgEPObjTrackInfo[i].m_Feature.iObjId);
            iDist++;
        }
        else
        {
            if (iDist > 0)
            {
                //m_rgEPObjTrackInfo[i - iDist] = m_rgEPObjTrackInfo[i];
                m_rgEPObjTrackInfo[i - iDist].Assign(m_rgEPObjTrackInfo[i], pProcessRespond);
                m_rgEPObjTrackInfo[i].Clear(pProcessRespond);
                //HV_Trace("\ncopy and clear id:[%d]",m_rgObjTrackInfo[i].m_Feature.iObjId);
            }
        }
    }
    m_cEPObjTrackInfo -= iDist;

	return S_OK;
}

HRESULT CTrackerImpl::FireNoPlateCarLeftEvent( 
	PROCESS_ONE_FRAME_PARAM* pParam, 
	CObjTrackInfo& ObjTrackInfo, 
	PROCESS_ONE_FRAME_RESPOND* pProcessRespond 
	)
{

	if(ObjTrackInfo.IsClearImage() || pProcessRespond->cTrigEvent.iCarLeftCount >= MAX_EVENT_COUNT)
		return S_FALSE;

	int iRoad = ObjTrackInfo.m_iRoad;
	if( iRoad == -1 )
	{
		iRoad = MatchRoad(ObjTrackInfo.m_rgRect[0].CenterPoint().x, ObjTrackInfo.m_rgRect[0].CenterPoint().y);
		if( iRoad < 0 || iRoad > 15 )
		{
			iRoad = 0;
		}
	}
	if (GetCurrentParam()->g_PlateRcogMode == PRM_HIGHWAY_HEAD 
		|| GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
	{
		DWORD32 frameElapse;
		if (m_leftCar[iRoad].car_plate == PC_YELLOW)
		{
			frameElapse = pParam->dwFrameNo - m_leftCar[iRoad].frame_no;
			if (frameElapse < m_iBigCarDelayFrameNo)
			{
				return S_OK;
			}
		}
	}

//	if (!m_pCallback)
//	{
//		return S_FALSE;
//	}

//	RESULT_IMAGE_STRUCT ResultImage;
//	ResultImage.pimgBestSnapShot = ObjTrackInfo.m_pBestReftImage;
//	ResultImage.pimgLastSnapShot = ObjTrackInfo.m_pLastRefImage;

	// 无牌车型投票
	ObjTrackInfo.Vote(ObjTrackInfo.m_nVotedCarType);
	float fltCount = 0.0;
	for(int i = 0; i < ObjTrackInfo.m_iObjFrameCount;i++)
	{
		fltCount += ObjTrackInfo.m_rgfltFrameSpeed[i];
	}

	if(ObjTrackInfo.m_iObjFrameCount > 1)
	{
		fltCount /= ObjTrackInfo.m_iObjFrameCount;
	}
	//HV_Trace("总帧数:%d,平均速度：%0.1f\r\n",ObjTrackInfo.m_iObjFrameCount, fltCount);
	
	if((ObjTrackInfo.m_nVotedCarType == CT_WALKMAN || ObjTrackInfo.m_nVotedCarType == CT_BIKE) 
		&& (int)fltCount < m_iWalkManSpeed)
	{
		ObjTrackInfo.m_nVotedCarType = CT_WALKMAN;			// 新的行人判断
	}
	else
	{
		if (ObjTrackInfo.m_nVotedCarType == CT_WALKMAN)
		{
			ObjTrackInfo.m_nVotedCarType = CT_BIKE;
		}
	}

	PROCESS_IMAGE_CORE_RESULT* pCurResult = &(pProcessRespond->cTrigEvent.rgCarLeftCoreInfo[pProcessRespond->cTrigEvent.iCarLeftCount]);

	// 结果结构赋值
	memset(pCurResult->rgbContent, 0, sizeof(pCurResult->rgbContent));
	pCurResult->nCarType = ObjTrackInfo.m_nVotedCarType;
	pCurResult->fltCarspeed = 0;

	//zhaopy
	// todo... 无车牌没有位置的输出，这个需要加入。
	pCurResult->dwBestSnapShotRefTime = ObjTrackInfo.m_dwBestSnapShotRefTime;
	pCurResult->dwLastSnapShotRefTime = ObjTrackInfo.m_dwLastSnapShotRefTime;
	pCurResult->dwBeginCaptureRefTime = ObjTrackInfo.m_dwBeginCaptureRefTime;
	pCurResult->dwBestCaptureRefTime = ObjTrackInfo.m_dwBestCaptureRefTime;
	pCurResult->dwLastCaptureRefTime = ObjTrackInfo.m_dwLastCaptureRefTime;

	pCurResult->dwTriggerTime = 0;
	if( ObjTrackInfo.m_fHasTrigger && ObjTrackInfo.m_dwTriggerTimeMs != 0 )
	{
		pCurResult->dwTriggerTime = ObjTrackInfo.m_dwTriggerTimeMs;
	}
	else if( m_fEnableCarArriveTrigger && ObjTrackInfo.m_nCarArriveRealTime != 0 )
	{
		pCurResult->dwTriggerTime = ObjTrackInfo.m_nCarArriveRealTime;
	}

	//增加红灯信息
	if(m_nRedLightCount < 20)
	{
		pCurResult->rcRedLightCount = m_nRedLightCount;
		for(int i = 0; i < m_nRedLightCount; i++)
		{
			pCurResult->rcRedLightPos[i].left   = m_rgRedLightRect[i].left;
			pCurResult->rcRedLightPos[i].top    = m_rgRedLightRect[i].top * 2;
			pCurResult->rcRedLightPos[i].right  = m_rgRedLightRect[i].right;
			pCurResult->rcRedLightPos[i].bottom = m_rgRedLightRect[i].bottom * 2;
		}
	}

	RESULT_IMAGE_STRUCT* pResultImage = &(pCurResult->cResultImg);
	if(!ObjTrackInfo.m_pBestReftImage)
	{
		ObjTrackInfo.m_pBestReftImage = ObjTrackInfo.m_pOneRefImage;
		if(ObjTrackInfo.m_pOneRefImage)
			ImageAddRef(&pProcessRespond->cImgMemOperLog, ObjTrackInfo.m_pBestReftImage);
		pCurResult->dwBestSnapShotRefTime = pCurResult->dwBeginCaptureRefTime;
	}
	if(!ObjTrackInfo.m_pLastRefImage)
	{
		ObjTrackInfo.m_pLastRefImage = ObjTrackInfo.m_pTwoRefImage;
		if(ObjTrackInfo.m_pLastRefImage)
			ImageAddRef(&pProcessRespond->cImgMemOperLog, ObjTrackInfo.m_pLastRefImage);
		pCurResult->dwLastSnapShotRefTime = pCurResult->dwBestCaptureRefTime;
	}
	pResultImage->pimgBestSnapShot = ObjTrackInfo.m_pBestReftImage;
	pResultImage->pimgLastSnapShot = ObjTrackInfo.m_pLastRefImage;

	//电警模式才输出三张抓拍图
	//黄冠榕 2011-08-12
	if(GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
	{
		pResultImage->pimgBeginCapture = ObjTrackInfo.m_pOneRefImage;
		pResultImage->pimgBestCapture = ObjTrackInfo.m_pTwoRefImage;
		pResultImage->pimgLastCapture = ObjTrackInfo.m_pThreeRefImage;
	}
	else
	{
		pResultImage->pimgBeginCapture = NULL;
		pResultImage->pimgBestCapture = NULL;
		pResultImage->pimgLastCapture = NULL;
	}


//	pCurResult->pResultImage = &ResultImage;
//	pCurResult->iFrameNo = ObjTrackInfo.m_pBestReftImage->GetFrameNo();
//	pCurResult->iRefTime = ObjTrackInfo.m_pLastRefImage->GetRefTime();
//	pCurResult->nStartFrameNo = ObjTrackInfo.m_pFirstRefImage->GetFrameNo();
//	pCurResult->nEndFrameNo = ObjTrackInfo.m_pLastRefImage->GetFrameNo();
//	pCurResult->nFirstFrameTime = ObjTrackInfo.m_pFirstRefImage->GetRefTime();
	pCurResult->nDetectCrossLineEnable = m_pParam->m_iDetectCrossLineEnable;
	pCurResult->nDetectOverYellowLineEnable = m_pParam->m_iDetectOverYellowLineEnable;
	pCurResult->nFirstFrameTime = ObjTrackInfo.m_dwFirstFlameTime;
	pCurResult->iObservedFrames = ObjTrackInfo.m_iObjInfoCount;
	pCurResult->iVotedObservedFrames = ObjTrackInfo.m_iObjInfoCount;	
	pCurResult->nPlateLightType = m_PlateLightType;
	pCurResult->iCplStatus = m_iCplStatus;
	pCurResult->iPulseLevel = m_iPulseLevel;
    if( ObjTrackInfo.m_fHasTrigger && ObjTrackInfo.m_dwTriggerTimeMs != 0 )
	{
		pCurResult->nCarArriveTime = pParam->dwCurTick;//ObjTrackInfo.m_dwTriggerTimeMs;
	}
	else
	{
		// 无牌车结果取图片时间
		pCurResult->nCarArriveTime = pParam->dwCurTick;//ObjTrackInfo.m_nCarArriveRealTime;
	}

	pCurResult->rcBestPlatePos = ObjTrackInfo.m_rcBestCarPos;
	pCurResult->rcLastPlatePos = ObjTrackInfo.m_rcLastCarPos;
	pCurResult->rcSecondPos = ObjTrackInfo.m_rgRect[1];
	//pCurResult->nCarColor = ObjTrackInfo.m_nVotedCarColor;
	//pCurResult->fOutputCarColor = CTrackInfo::m_fEnableRecgCarColor;
	pCurResult->fIsNight = (m_LightType == NIGHT) ? true : false;
	if(pCurResult->fIsNight && pCurResult->iPulseLevel < 1)
	{
		pCurResult->iPulseLevel = 1;
	}
	if(m_iForceLightOffAtDay)
	{
		if(m_LightType == DAY && m_iPulseLevel > 0 
			&& m_iForceLightOffFlag == 1)
		{
			m_iPulseLevel = 0;
		}
	}

	pCurResult->coltIsOverYellowLine	= COLT_NO;
	pCurResult->coltIsCrossLine		= COLT_NO;

	pCurResult->fReverseRun = false;
	switch (m_pParam->g_MovingDirection)
	{
	case MD_TOP2BOTTOM:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().y - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().y
			 < -(m_nHeight * m_iSpan * 0.01f))
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_BOTTOM2TOP:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().y - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().y
			 > m_nHeight * m_iSpan * 0.01f)
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_LEFT2RIGHT:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().x - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().x
			 < -(m_nHeight * m_iSpan * 0.01f))
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_RIGHT2LEFT:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().x - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().x
			  > m_nHeight * m_iSpan * 0.01f)
		{
			pCurResult->fReverseRun = true;
		}
		break;
	default:
		;
	}

	//用于判断行驶方向，检测车辆是否与车道设定方向一致
	switch ( m_TravelDirection )
	{
	case MD_TOP2BOTTOM:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().y - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().y
			 < -(m_nHeight * m_iSpan * 0.01f))
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_BOTTOM2TOP:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().y - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().y
			 > m_nHeight * m_iSpan * 0.01f)
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_LEFT2RIGHT:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().x - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().x
			 < -(m_nHeight * m_iSpan * 0.01f))
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_RIGHT2LEFT:
		if (ObjTrackInfo.LastInfo().m_rcFG.CenterPoint().x - ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().x
			  > m_nHeight * m_iSpan * 0.01f)
		{
			pCurResult->fReverseRun = true;
		}
		break;
	case MD_NOTSURE:
	default:
	}

	//如果是逆校抛ヅ耐妓承虻构?
	//电警模式才输出三张抓拍图
	//黄冠榕 2011-10-26
	if(GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
	{
		if( pCurResult->fReverseRun )
		{
			pResultImage->pimgBeginCapture = ObjTrackInfo.m_pThreeRefImage;
			pResultImage->pimgBestCapture = ObjTrackInfo.m_pTwoRefImage;
			pResultImage->pimgLastCapture = ObjTrackInfo.m_pOneRefImage;

			pCurResult->rcBestPlatePos = ObjTrackInfo.m_rgRect[2];
			pCurResult->rcLastPlatePos = ObjTrackInfo.m_rgRect[1];
			pCurResult->rcFirstPos = ObjTrackInfo.m_rgRect[2];
			pCurResult->rcSecondPos= ObjTrackInfo.m_rgRect[1];
			pCurResult->rcThirdPos = ObjTrackInfo.m_rgRect[0];
		}
		else 
		{
			pResultImage->pimgBeginCapture = ObjTrackInfo.m_pOneRefImage;
			pResultImage->pimgBestCapture = ObjTrackInfo.m_pTwoRefImage;
			pResultImage->pimgLastCapture = ObjTrackInfo.m_pThreeRefImage;

			pCurResult->rcBestPlatePos = ObjTrackInfo.m_rgRect[0];
			pCurResult->rcLastPlatePos = ObjTrackInfo.m_rgRect[1];
			pCurResult->rcFirstPos = ObjTrackInfo.m_rgRect[0];
			pCurResult->rcSecondPos= ObjTrackInfo.m_rgRect[1];
			pCurResult->rcThirdPos = ObjTrackInfo.m_rgRect[2];
		}
	}

	//校正坐标
	pCurResult->rcBestPlatePos.left   = pCurResult->rcBestPlatePos.left  * 100 / m_nWidth;
	pCurResult->rcBestPlatePos.top    = pCurResult->rcBestPlatePos.top   * 100 / m_nHeight;
	pCurResult->rcBestPlatePos.right  = pCurResult->rcBestPlatePos.right  * 100 / m_nWidth;
	pCurResult->rcBestPlatePos.bottom = pCurResult->rcBestPlatePos.bottom * 100 / m_nHeight;

	pCurResult->rcLastPlatePos.left   = pCurResult->rcLastPlatePos.left  * 100 / m_nWidth;
	pCurResult->rcLastPlatePos.top    = pCurResult->rcLastPlatePos.top   * 100 / m_nHeight;
	pCurResult->rcLastPlatePos.right  = pCurResult->rcLastPlatePos.right  * 100 / m_nWidth;
	pCurResult->rcLastPlatePos.bottom = pCurResult->rcLastPlatePos.bottom * 100 / m_nHeight;

	pCurResult->rcFirstPos.left   = pCurResult->rcFirstPos.left  * 100 / m_nWidth;
	pCurResult->rcFirstPos.top    = pCurResult->rcFirstPos.top   * 100 / m_nHeight;
	pCurResult->rcFirstPos.right  = pCurResult->rcFirstPos.right  * 100 / m_nWidth;
	pCurResult->rcFirstPos.bottom = pCurResult->rcFirstPos.bottom * 100 / m_nHeight;

	pCurResult->rcSecondPos.left   = pCurResult->rcSecondPos.left  * 100 / m_nWidth;
	pCurResult->rcSecondPos.top    = pCurResult->rcSecondPos.top   * 100 / m_nHeight;
	pCurResult->rcSecondPos.right  = pCurResult->rcSecondPos.right  * 100 / m_nWidth;
	pCurResult->rcSecondPos.bottom = pCurResult->rcSecondPos.bottom * 100 / m_nHeight;

	pCurResult->rcThirdPos.left   = pCurResult->rcThirdPos.left  * 100 / m_nWidth;
	pCurResult->rcThirdPos.top    = pCurResult->rcThirdPos.top   * 100 / m_nHeight;
	pCurResult->rcThirdPos.right  = pCurResult->rcThirdPos.right  * 100 / m_nWidth;
	pCurResult->rcThirdPos.bottom = pCurResult->rcThirdPos.bottom * 100 / m_nHeight;


	pCurResult->iAvgY = m_iAvgY;
	pCurResult->nRoadNo = ObjTrackInfo.m_iRoad;			// 车道编号
	//pCurResult->iRoadNumberBegin =  CTrackInfo::m_iRoadNumberBegin;
	//pCurResult->iStartRoadNum = CTrackInfo::m_iStartRoadNum;
	pCurResult->iUnSurePeccancy = 0;

	if (pCurResult->nRoadNo == -1)
	{
		pCurResult->nRoadNo = MatchRoad(ObjTrackInfo.m_rgRect[0].CenterPoint().x, ObjTrackInfo.m_rgRect[0].CenterPoint().y);
		//黄国超添加,2011-03-07
		//pCurResult->iRoadType = CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iRoadType;
	}

	if( m_pParam->m_fUsedTrafficLight )
	{
		//红绿灯车道号的计算使用抓拍的第一张大图的位置
		pCurResult->nRoadNo = MatchRoad(ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().x, ObjTrackInfo.m_rgObjInfo[0].m_rcFG.CenterPoint().y);

		//行驶类型
		RUN_TYPE rt = RT_UNSURE;
		//如果是逆行
		if( pCurResult->fReverseRun )
		{
			rt = RT_CONVERSE;
		}
		else
		{
			rt = CheckRunType(ObjTrackInfo.m_rgRect[0], ObjTrackInfo.m_rgRect[2]);
		}

		//过滤车道相关行驶类型
		int iFilter = CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iFilterRunType;
		if( (iFilter & RRT_FORWARD) && rt == RT_FORWARD )
		{
			rt = RT_UNSURE;
		}
		else if( (iFilter & RRT_LEFT) && rt == RT_LEFT )
		{
			rt = RT_UNSURE;
		}
		else if( (iFilter & RRT_RIGHT) && rt == RT_RIGHT )
		{
			rt = RT_UNSURE;
		}
		else if( (iFilter & RRT_TURN) && rt == RT_TURN )
		{
			rt = RT_UNSURE;
		}
		pCurResult->rtType = rt;

		int iOnePosLightScene = ObjTrackInfo.m_iOnePosLightScene;
		int iTwoPosLightScene = ObjTrackInfo.m_iPassStopLightScene;

		if( CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iRoadType & RRT_LEFT )
		{
			iTwoPosLightScene = ObjTrackInfo.m_iPassLeftStopLightScene;
		}
		//如果打开"过滤第三张抓拍图为绿灯的违章",则用第三线抓拍线灯的状态的作为判断的依据
		//韦开拓修改,2010-12-22
		if(m_iFilterRushPeccancy && iTwoPosLightScene != -1)
		{
			iTwoPosLightScene = ObjTrackInfo.m_iThreePosLightScene;
		}

		//GetLightScene(iOnePosLightScene, &pCurResult->tsOnePosScene);
		//GetLightScene(iTwoPosLightScene, &pCurResult->tsTwoPosScene);
		m_cTrafficLight.GetLightScene(iOnePosLightScene, &pCurResult->tsOnePosScene);
		m_cTrafficLight.GetLightScene(iTwoPosLightScene, &pCurResult->tsTwoPosScene);

		/*PECCANCY_TYPE*/int pt = 0;
		int iRoadType = (RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
		if( pCurResult->nRoadNo != -1 )
		{
			iRoadType = CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iRoadType;
			//黄国超添加,2011-03-07
			//pCurResult->iRoadType = CTrackInfo::m_roadInfo[pCurResult->nRoadNo].iRoadType;
		}

		//只输出对应车道的红灯开始时间
		//pCurResult->lrtRedStart = m_redrealtime;
		m_cTrafficLight.GetRedLightStartTime(&pCurResult->lrtRedStart);
		if( (iRoadType & RRT_LEFT) == 0 ) 
		{
			pCurResult->lrtRedStart.dwLeftL = pCurResult->lrtRedStart.dwLeftH = 0;
		}
		if( (iRoadType & RRT_FORWARD) == 0 )
		{
			pCurResult->lrtRedStart.dwForwardL = pCurResult->lrtRedStart.dwForwardH = 0;
		}
		if( (iRoadType & RRT_RIGHT) == 0 ) 
		{
			pCurResult->lrtRedStart.dwRightL = pCurResult->lrtRedStart.dwRightH = 0;
		}
		if( (iRoadType & RRT_TURN) == 0 ) 
		{
			pCurResult->lrtRedStart.dwTurnL = pCurResult->lrtRedStart.dwTurnH = 0;
		}

		//如果行驶类型是未知且所在的车道只有一种方向的，则输出的行驶类型为这个方向的。
		if( pCurResult->rtType == RT_UNSURE && CTrackInfo::m_iRunTypeEnable == 1 )
		{
			if( iRoadType == RRT_LEFT )
			{
				pCurResult->rtType = RT_LEFT;
			}
			else if( iRoadType == RRT_FORWARD )
			{
				pCurResult->rtType = RT_FORWARD;
			}
			else if( iRoadType == RRT_RIGHT )
			{
				pCurResult->rtType = RT_RIGHT;
			}
			else if( iRoadType == RRT_TURN )
			{
				pCurResult->rtType = RT_TURN;
			}
		}

		DWORD32 dwCaptureTime = (pResultImage->pimgBestCapture == NULL ? GetSystemTick() : ObjTrackInfo.m_dwFirstTime);
		if( S_OK != CheckPeccancyType(iOnePosLightScene, iTwoPosLightScene, rt, iRoadType, dwCaptureTime, &pt) )
		{
			pt = PT_NORMAL;
		}
		if(m_iFilterRushPeccancy  && (pt & PT_RUSH) != PT_RUSH)
		{
//			int ptTmp;
//			if( S_OK == CheckPeccancyType(iOnePosLightScene, ObjTrackInfo.m_iPassStopLightScene, rt, iRoadType, dwCaptureTime, &ptTmp) )
//			{
//				if((ptTmp & PT_RUSH) == PT_RUSH)
//				{
//					HV_Trace(1, "第三抓拍图灯的状态不为红灯执行过滤\n");
//				}
//			}
		}
		
		//如果已经触发了两次相机,并且第三次没触发,则触发第三次
		if( ObjTrackInfo.m_fFirstTrigger && ObjTrackInfo.m_fSecondTrigger && !ObjTrackInfo.m_fThirdTrigger && pt != PT_NORMAL )
		{
			if(GetSystemTick() > (DWORD32)m_sMinTriggerTimeMs)
			{
				m_dwTriggerTimes++;
				if(m_dwTriggerTimes >= 0xffff)
				{
					m_dwTriggerTimes = 1;

				}
				
			   	//DWORD32 dwRoadNum = (m_dwTriggerTimes << 16) | stCarLeftInfo.nVideoDetID;
				//m_pCallback->ThirdTrigger(m_LightType,dwRoadNum, NULL);
				m_sMinTriggerTimeMs = GetSystemTick() + 300;
				//m_dwLastTriggerInfo = dwRoadNum;
				//HV_Trace(1, "firecarleft:%d,%d, %08x\n", m_dwTriggerTimes, stCarLeftInfo.nVideoDetID, dwRoadNum);
			}
			else
			{
				//DWORD32 dwRoadNum = (m_dwLastTriggerInfo & 0xffff0000) | stCarLeftInfo.nVideoDetID;
				//HV_Trace(1, "ThirdTrigger Readd:%08x\n", dwRoadNum);
				//m_pCallback->ReaddSignal(dwRoadNum);
			}
			ObjTrackInfo.m_fThirdTrigger = true;
			ObjTrackInfo.m_dwThirdTime = m_dwTriggerTimes;
			ObjTrackInfo.m_dwThirdTimeMs = GetSystemTick();
		}
		// 韦开拓修改 end
		pCurResult->ptType |= pt;

		//sprintf(g_szDebugInfo, "[FireNoPlateCarLeft]rtType=%d, ptType=%d, pt=%d", pCurResult->rtType, pCurResult->ptType, pt);
/*
		if( (pResultImage->pimgBeginCapture == NULL || pResultImage->pimgBestCapture == NULL || pResultImage->pimgLastCapture == NULL )
			|| pResultImage->pimgBeginCapture->GetRefTime() == pResultImage->pimgBestCapture->GetRefTime()
			|| pResultImage->pimgBeginCapture->GetRefTime() == pResultImage->pimgLastCapture->GetRefTime()
			|| pResultImage->pimgBestCapture->GetRefTime() == pResultImage->pimgLastCapture->GetRefTime() 
			)
		{
			//图片不合违章规定
			pCurResult->iUnSurePeccancy = 1;
		}
		if( (pResultImage->pimgBeginCapture == NULL || pResultImage->pimgBestCapture == NULL || pResultImage->pimgLastCapture == NULL )
			|| TrackInfo.m_dwBeginCaptureRefTime == TrackInfo.m_dwBestCaptureRefTime
			|| TrackInfo.m_dwBeginCaptureRefTime == TrackInfo.m_dwLastCaptureRefTime
			|| TrackInfo.m_dwBestCaptureRefTime == TrackInfo.m_dwLastCaptureRefTime 
			)
		{
			//图片不合违章规定
			pCurResult->iUnSurePeccancy = 1;
		}
*/
		//过滤掉车道的逆行违章
		if( (pCurResult->ptType & PT_CONVERSE) == PT_CONVERSE
			&& ( (iFilter & RRT_CONVERSE) && rt == RT_CONVERSE) )
		{
			pCurResult->ptType &= ~PT_CONVERSE;
		}

		// zhaopy
		if (m_fFilterNoPlatePeccancy)
		{
			pCurResult->ptType = PT_NORMAL;
		}
		pCurResult->dwFirstTime = ObjTrackInfo.m_dwFirstTime;
		pCurResult->dwSecondTime = ObjTrackInfo.m_dwSecondTime;
		pCurResult->dwThirdTime = ObjTrackInfo.m_dwThirdTime;
		if(CTrackInfo::m_iFlashlightMode == 1 && m_iAvgY < CTrackInfo::m_iFlashlightThreshold)
		{
//			HV_Trace("Tracker:s=%d:f=%d:t=%d\n", pCurResult->dwFirstTime, pCurResult->dwSecondTime, pCurResult->dwThirdTime);
		}
	}
	else
	{
		pCurResult->rtType = RT_UNSURE;
		pCurResult->ptType = PT_NORMAL;
	}


	// 车辆尺寸
	if (m_fOutputCarSize == TRUE)
	{
		pCurResult->cCarSize.iCarWidth = ObjTrackInfo.m_stCarInfo.fltCarW / 100;
		pCurResult->cCarSize.iCarHeight = ObjTrackInfo.m_stCarInfo.fltCarH / 100;

		if (m_fEnableScaleSpeed)
		{
			// 具有标定
			pCurResult->cCarSize.nOutType = 1;		// 涑?米)
			if ((pCurResult->cCarSize.iCarWidth < 1.4f) 
				|| (pCurResult->cCarSize.iCarHeight < 2.7f))
			{
				float fltTempS = 1.0f;
				float fltRand = (float)(rand() % 51) / 150;
				fltTempS += fltRand;

				if (ObjTrackInfo.m_nVotedCarType == CT_LARGE)
				{
					fltTempS *= 1.75f;
				}
				else if (ObjTrackInfo.m_nVotedCarType == CT_MID)
				{
					fltTempS *= 1.20f;
				}

				pCurResult->cCarSize.iCarWidth = 1.5f * fltTempS;
				pCurResult->cCarSize.iCarHeight = 3.4f * fltTempS;
			}
		}
		else
		{
			pCurResult->cCarSize.nOutType = 0;
			//无标定									// 输出(像素)
			if ((pCurResult->cCarSize.iCarWidth < m_nCarWBottom) 
				|| (pCurResult->cCarSize.iCarHeight < m_nCarLenBottom))
			{
				float fltTempS = 1.1f;
				float fltRand = (float)(rand() % 11) / 100;
				fltTempS += fltRand;

				if (ObjTrackInfo.m_nVotedCarType == CT_LARGE)
				{
					fltTempS *= 1.75f;
				}
				else if (ObjTrackInfo.m_nVotedCarType == CT_MID)
				{
					fltTempS *= 1.2f;
				}

				pCurResult->cCarSize.iCarWidth = m_nCarWBottom * fltTempS;
				pCurResult->cCarSize.iCarHeight = m_nCarLenBottom * fltTempS;
			}
		}
	}

	// 厦门项目修改车辆类型
	if (m_iOutPutType == 1)
	{
		if ((pCurResult->nCarType == CT_SMALL)
			|| (pCurResult->nCarType == CT_MID)
			|| (pCurResult->nCarType == CT_LARGE))
		{
			pCurResult->nCarType = CT_VEHICLE;
		}
	}

	if (ObjTrackInfo.m_fIsTrigger)
	{
		pCurResult->dwTriggerIndex = ObjTrackInfo.m_dwTriggerIndex;
		pCurResult->iCapCount = ObjTrackInfo.m_iCapCount;
	}

//	RTN_HR_IF_FAILED( m_pCallback->CarLeft(
//		&stCarLeftInfo,
//		ResultImage.pimgBestSnapShot->GetFrameName()
//		));

	pProcessRespond->cTrigEvent.dwEventId |= EVENT_CARLEFT;
	pProcessRespond->cTrigEvent.iCarLeftCount++;

	return S_OK;
}

HRESULT CTrackerImpl::SetCarTypeParam(int nWidth, int nHeight)
{
	//m_nCarLenTop = 298;
	//m_nCarLenBottom = 695;

	m_nCarLenTop = 260;
	m_nCarLenBottom = 680;

	m_nCarWTop = 196;
	m_nCarWBottom = 570;

	if (nWidth == 1920)
	{
		// 1920 * 1200 的图象
		m_nCarLenTop = 266;
		m_nCarLenBottom = 534;

		m_nCarWTop = 202;
		m_nCarWBottom = 460;
	}
	else if (nWidth == 1600)
	{
		// 抓拍图1600 * 1200
		m_nCarLenTop = 420;
		m_nCarLenBottom = 690;

		m_nCarWTop = 320;
		m_nCarWBottom = 635;
	}

	{
		// 没有视频测速，使用默认的经验值参数计算斜率
		m_fltSlopeH =  (float)(m_nCarLenBottom - m_nCarLenTop) / (float)(nHeight * 2 - m_nCarLenTop);
		m_fltSlopeW =  (float)(m_nCarWBottom - m_nCarWTop) / (float)(nHeight * 2);
	}

	return S_OK;
}

HRESULT CTrackerImpl::GetItgArea(CItgArea** ppItgArea, DWORD32 dwRefTime)
{
	if(m_nWidth == 0 || m_nHeight == 0)
	{
		*ppItgArea = NULL;
		return S_FALSE;
	}

	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CItgArea *pItgArea, *pCurArea;
	pCurArea = pItgArea = &m_cItgArea; //全检框
	if (m_nVideoDetMode == NO_VIDEODET)
	{
		CRect rcTrackArea;
		RTN_HR_IF_FAILED(MapParamRect(rcTrackArea, rcMax, m_pParam->g_rectTrackArea));
		CRect rcFullSearch;
		RTN_HR_IF_FAILED(MapParamRect(rcFullSearch, rcMax, m_pParam->g_rectDetectionArea));
		RTN_HR_IF_FAILED(m_cItgArea.SetRect(rcFullSearch, FALSE));

		//取得每个Tracker的预测位置
		CItgArea *pTrackItgArea; 
		for(int i = 0; i < m_cTrackInfo; i++)
		{
			if( pTrackItgArea = (m_rgTrackInfo + i)->GetPredictArea(dwRefTime, rcFullSearch, rcTrackArea, m_pParam->m_iDetectReverseEnable, m_pScaleSpeed) )
			{
				pCurArea->SetNextArea(pTrackItgArea);
				pCurArea = pTrackItgArea;
			}
		}
	}
	else
	{
		CRect rcFullSearch;
		RTN_HR_IF_FAILED(MapParamRect(rcFullSearch, rcMax, m_rcVideoDetArea));
		RTN_HR_IF_FAILED(m_cItgArea.SetRect(rcFullSearch, FALSE));
	}
	//红绿灯位置
	if (m_pParam->m_fUsedTrafficLight)
	{
		for (int i = 0; i < m_nLightCount; i++)
		{
			//CRect rcLight = m_rgLightInfo[i].GetInflatedRect();
			CTrafficLightInfo* pTrafficLightInfo = m_cTrafficLight.GetTrafficLightInfo(i);
			if (pTrafficLightInfo == NULL) continue;
			CRect rcLight = pTrafficLightInfo->GetInflatedRect();
			if( rcLight.IntersectRect(rcMax, rcLight) )
			{
				m_rgLightArea[i].SetRect(rcLight, FALSE);
				pCurArea->SetNextArea(&m_rgLightArea[i]);
				pCurArea = &m_rgLightArea[i];
			}
		}
	}
	pCurArea->SetNextArea(NULL);
	*ppItgArea = pItgArea;

	return S_OK;
}

HRESULT CTrackerImpl::TrackingObj(PROCESS_ONE_FRAME_PARAM* pParam, HV_COMPONENT_IMAGE &imgCurImage, PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcFullSearch;
	RTN_HR_IF_FAILED(MapParamRect(rcFullSearch, rcMax, m_pParam->g_rectDetectionArea));
	CRect rcVideoDet;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDet, rcMax, m_rcVideoDetArea));
	
	//视频跟踪
	for (int i = 0; i < m_cObjTrackInfo; i++)
	{
		m_rgObjTrackInfo[i].m_fTrack = FALSE;
	}

	bool fCalcCarType;
	if ((CObjectDetector::m_iProcessType == 1) || (CObjectDetector::m_iProcessType == 2))
	{
		// 厦门项目
		fCalcCarType = m_rgiFiveCharWidth[39] || ((m_nCarLenTop > 0) && (m_nCarLenBottom > 0));
	}
	else
	{
		// 默认
		fCalcCarType = (m_nCarLenTop > 0) && (m_nCarLenBottom > 0);
	}
	for (int i = 0; i < m_cDetected; i++)
	{
		int rgiPlateTrackID[MAX_PLATETRACK_COUNT];
		int cPlateTrackID(0);
		CObjInfo objInfo;
		objInfo.m_rcFG = m_rgrcDetected[i];
		objInfo.m_dwFrameTime = pParam->dwImageTime;
		objInfo.m_nFrameNo = pParam->dwFrameNo;
		objInfo.m_nCarType = CT_UNKNOWN;

		// zhaopy 0624
		bool fIsLightGroup = (i < m_iLightDetected);
		objInfo.m_fLight = fIsLightGroup;

		// 计算车型
		if (fCalcCarType)
		{
			if ((CObjectDetector::m_iProcessType == 1) || (CObjectDetector::m_iProcessType == 2))
			{
				// 厦门项目
				if (m_rgiFiveCharWidth[39])
				{
					int iOffset = m_rgrcDetected[i].bottom / 25;
					if (iOffset >= 0 && iOffset < 39)
					{
						//HV_Trace("CarWidth=%02f\n", (float)m_rgrcDetected[i].Width() / m_rgiFiveCharWidth[iOffset]);
						if (m_rgrcDetected[i].Width() < m_rgiFiveCharWidth[iOffset] * m_fltBikeSensitivity)
						{
							objInfo.m_nCarType = CT_WALKMAN;
						}
					}
				}
				else  //  zhaopy 默认是非机动车。0624
				{
					objInfo.m_nCarType = CT_WALKMAN;
				}

				if (objInfo.m_nCarType == CT_UNKNOWN)
				{
					int nYPos1 = m_nHeight >> 1;
					int nYPos2 = m_nHeight - 10;

					if ((m_rgrcDetected[i].bottom < nYPos2) 
						&& (m_rgrcDetected[i].bottom > (m_nCarLenTop>> 1))
						&& ((m_rgrcDetected[i].top > 10) || (m_rgrcDetected[i].bottom >= nYPos1)))
					{
						float fltCarLen = m_fltSlopeH * (m_rgrcDetected[i].bottom * 2 - m_nCarLenTop) + m_nCarLenTop;
						float fltThre1 = fltCarLen * 1.40f / 2;
						float fltThre2 = fltCarLen * 1.70f / 2;
						if (m_rgrcDetected[i].Height() < fltThre1)
						{
							objInfo.m_nCarType = CT_SMALL;
						}
						else if (m_rgrcDetected[i].Height() > fltThre2)
						{
							objInfo.m_nCarType = CT_LARGE;
						}
						else
						{
							objInfo.m_nCarType = CT_MID;
						}
					}
				}
			}
			else
			{
				// 默认
				int nYPos1 = m_nHeight >> 1;
				int nYPos2 = m_nHeight - 10;

				if ((m_rgrcDetected[i].bottom < nYPos2) 
					&& (m_rgrcDetected[i].bottom > (m_nCarLenTop>> 1))
					&& ((m_rgrcDetected[i].top > 10) || (m_rgrcDetected[i].bottom >= nYPos1)))
				{
					float fltCarLen = m_fltSlopeH * (m_rgrcDetected[i].bottom * 2 - m_nCarLenTop) + m_nCarLenTop;
					float fltThre1 = fltCarLen * 1.25f / 2;
					float fltThre2 = fltCarLen * 1.70f / 2;
					if (m_rgrcDetected[i].Height() < fltThre1)
					{
						objInfo.m_nCarType = CT_SMALL;
					}
					else if (m_rgrcDetected[i].Height() > fltThre2)
					{
						objInfo.m_nCarType = CT_LARGE;
					}
					else
					{
						objInfo.m_nCarType = CT_MID;
					}
				}

				// 计算车长、宽
				if (m_fEnableScaleSpeed)
				{
					// 有视频测速
					objInfo.m_nCarW = int(100 * m_pScaleSpeed->CalcActureDistance(CPoint(m_rgrcDetected[i].left,
						m_rgrcDetected[i].bottom), CPoint(m_rgrcDetected[i].right, m_rgrcDetected[i].bottom)) / 1.0f); 
					objInfo.m_nCarH = int(100 * m_pScaleSpeed->CalcActureDistance(CPoint(m_rgrcDetected[i].left,
						m_rgrcDetected[i].top), CPoint(m_rgrcDetected[i].left, m_rgrcDetected[i].bottom)) / 2.5f);
				}
				else
				{
					int iTempCarW = m_rgrcDetected[i].right - m_rgrcDetected[i].left;
					int iTempCarH = (m_rgrcDetected[i].bottom - m_rgrcDetected[i].top) * 2;

					objInfo.m_nCarW = int(iTempCarW + m_fltSlopeW * (m_nHeight * 2 - iTempCarH));
					objInfo.m_nCarH = int(iTempCarH + m_fltSlopeH * (m_nHeight * 2 - iTempCarH));
				}
			}
		}

		//计算检测到的物体中有多少个车牌跟踪
		for (int j = 0; j < m_cTrackInfo; j++)
		{
			if (m_rgTrackInfo[j].m_iVideoID != pParam->iVideoID ||
				m_rgTrackInfo[j].m_State == tsInit ||
				m_rgTrackInfo[j].m_State == tsWaitRemove ||
				!m_rgTrackInfo[j].m_fObservationExists)
			{
				continue;
			}
			if (m_rgrcDetected[i].IntersectsWith(m_rgTrackInfo[j].LastInfo().rcPos))
			{
				rgiPlateTrackID[cPlateTrackID++] = m_rgTrackInfo[j].m_nID;
				if (fCalcCarType)
				{
					// 附加车型信息
					m_rgTrackInfo[j].LastInfo().nCarType = objInfo.m_nCarType;

					if (m_fOutputCarSize == TRUE)
					{
						m_rgTrackInfo[j].LastInfo().stCarInfo.rcFG = objInfo.m_rcFG;

						m_rgTrackInfo[j].LastInfo().stCarInfo.fltCarW = (float)objInfo.m_nCarW;
						m_rgTrackInfo[j].LastInfo().stCarInfo.fltCarH = (float)objInfo.m_nCarH;

						//// 计算宽、高
						//CRect rcTempCar = m_rgTrackInfo[j].LastInfo().stCarInfo.rcFG;
						//m_rgTrackInfo[j].LastInfo().stCarInfo.fltCarW = m_pScaleSpeed->CalcActureDistance(CPoint(rcTempCar.left,
						//	rcTempCar.bottom), CPoint(rcTempCar.right, rcTempCar.bottom));
						//m_rgTrackInfo[j].LastInfo().stCarInfo.fltCarH = m_pScaleSpeed->CalcActureDistance(CPoint(rcTempCar.left,
						//	rcTempCar.top), CPoint(rcTempCar.left, rcTempCar.bottom));
					} // if (m_fOutputCarSize)
					// TODO 根据车牌类型校正车型、统计
				}
			}
		}

		//计算检测到的物体属于哪个视频跟踪
		int iMaxArea = 0;
		int iSecondArea = 0;
		int iObjIndex(-1);
		int iObjSecondIndex = -1;

		for (int j = 0; j < m_cObjTrackInfo; j++)
		{
			if (m_rgObjTrackInfo[j].m_iVideoID == pParam->iVideoID)
			{
				HiVideo::CRect rcIntersect;
				if (rcIntersect.IntersectRect(&(m_rgObjTrackInfo[j].LastInfo().m_rcFG), &(objInfo.m_rcFG)))
				{
					int iInterArea = rcIntersect.Area();
					if (iInterArea > iMaxArea)
					{
						iObjIndex = j;
						iMaxArea = iInterArea;
					}
					else if( iInterArea > iSecondArea )
					{
						iObjSecondIndex = j;
						iSecondArea = iInterArea;
				}
			}
		}
		}
		// zhaopy 0624
		// 避免逆行的车辆被正行的删除。
		if( iObjIndex != -1  && iObjSecondIndex != -1 )
		{
			if( IsCanOutputReverseRun(&m_rgObjTrackInfo[iObjIndex])
				&& m_rgObjTrackInfo[iObjIndex].m_objTrackState == OBJ_TRACK_MOVING )
			{
				m_rgObjTrackInfo[iObjIndex].m_objTrackState = OBJ_TRACK_END;
				iObjIndex = -1;
			}
			if( IsCanOutputReverseRun(&m_rgObjTrackInfo[iObjSecondIndex])
				&& m_rgObjTrackInfo[iObjSecondIndex].m_objTrackState == OBJ_TRACK_MOVING )
			{
				m_rgObjTrackInfo[iObjSecondIndex].m_objTrackState = OBJ_TRACK_END;
				iObjSecondIndex = -1;
			}
			iObjIndex = (iObjIndex == -1 ? iObjSecondIndex : iObjIndex);
		}

		if (iObjIndex >= 0 && m_rgObjTrackInfo[iObjIndex].m_objTrackState != OBJ_TRACK_END)
		{
			if(S_OK == m_rgObjTrackInfo[iObjIndex].Process(objInfo, imgCurImage, rcVideoDet, m_iLastLightStatus, m_iCurLightStatus, pParam, pProcessRespond))
			{
			if (m_rgObjTrackInfo[iObjIndex].m_fTrack)
			{
				//m_rgObjTrackInfo[iObjIndex].RectifyRectBottom(rcMax.bottom, fLightIsNight);
				for (int k = 0; k < cPlateTrackID; k++)
				{
					if (!m_rgObjTrackInfo[iObjIndex].MatchPlateTrackID(rgiPlateTrackID[k]))
					{
						m_rgObjTrackInfo[iObjIndex].AddPlateTrackID(rgiPlateTrackID[k]);
					}
				}
			}

				if(CTrackInfo::m_iFlashlightMode == 1 && m_iAvgY < CTrackInfo::m_iFlashlightThreshold)
				{
					// zhaopy
					CheckObjectPeccancyTrigger(&m_rgObjTrackInfo[iObjIndex], iObjIndex);
				}
			}
		}

		CPoint ptTemp = m_rgrcDetected[i].CenterPoint();
		if (iObjIndex < 0 &&
			rcFullSearch.Contains(&ptTemp) &&
			(m_rgrcDetected[i].Height() < rcVideoDet.Height() * 0.5)
			|| GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE )
		{
			//只检测梯形区域内的移动物体 weikt 2011-7-21
			if (m_cfgParam.cTrapArea.fEnableDetAreaCtrl && GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE)
			{
				if (!IsContainOfTrap(m_cfgParam.cTrapArea, m_rgrcDetected[i]))
				{
					continue;
				}
			}
			//end

			if (m_cObjTrackInfo == s_knMaxObjCandidate)
			{
				m_cObjTrackInfo--;
			}
			m_rgObjTrackInfo[m_cObjTrackInfo].Clear(pProcessRespond);

			if ((CObjectDetector::m_iProcessType == 1) || (CObjectDetector::m_iProcessType == 2))
			{
				// 厦门项目
				bool fIsManRoad(false);
				if (CTrackInfo::m_iRoadNumber > 2)
				{
					// 计算车道
					int nTempRoad = MatchRoad(objInfo.m_rcFG.CenterPoint().x, objInfo.m_rcFG.CenterPoint().y);
					fIsManRoad = (CTrackInfo::m_iManRoadNum == nTempRoad);
				}

				RTN_HR_IF_FAILED(m_rgObjTrackInfo[m_cObjTrackInfo].New(pParam, objInfo, imgCurImage, m_iLastLightStatus, 
					m_iCurLightStatus, fIsManRoad, pProcessRespond));

			}
			else
			{
				// 默认
				RTN_HR_IF_FAILED(m_rgObjTrackInfo[m_cObjTrackInfo].New(pParam, objInfo, imgCurImage,m_iLastLightStatus, m_iCurLightStatus, false, pProcessRespond));
			}

			for (int k = 0; k < cPlateTrackID; k++)
			{
				m_rgObjTrackInfo[m_cObjTrackInfo].AddPlateTrackID(rgiPlateTrackID[k]);
			}
			m_cObjTrackInfo++;
		}
	}

	for (int i = 0; i < m_cObjTrackInfo; i++)
	{
		bool fIsManRoad = m_rgObjTrackInfo[i].m_fIsMan;			// 行人道建立的跟踪

		if (m_rgObjTrackInfo[i].m_iVideoID != pParam->iVideoID ||
			m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_INIT)
		{
			continue;
		}
		if (!m_rgObjTrackInfo[i].m_fTrack)
		{
			m_rgObjTrackInfo[i].m_iMissingCount++;
		}
		else
		{
			if (m_rgObjTrackInfo[i].LastInfo().m_rcFG == CRect(0, 0, 0, 0))
			{
				m_rgObjTrackInfo[i].m_iMissingCount++;
				m_rgObjTrackInfo[i].LastInfo().Clear();
				m_rgObjTrackInfo[i].m_iObjInfoCount--;
			}
			else
			{
				m_rgObjTrackInfo[i].m_iMissingCount = 0;
			}
		}
		switch (m_rgObjTrackInfo[i].m_objTrackState)
		{
		case OBJ_TRACK_NEW:
			if (!fIsManRoad)
			{
				// 机动车道建立的跟踪
				if (m_rgObjTrackInfo[i].m_iMissingCount >= 2)
				{
					m_rgObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
				}
				else if (m_rgObjTrackInfo[i].m_iObjInfoCount >= 3)
				{
					m_rgObjTrackInfo[i].m_objTrackState = OBJ_TRACK_MOVING;
				}
			}
			else
			{
				// 行人道建立的跟踪
				if (m_rgObjTrackInfo[i].m_iMissingCount >= WALK_MAN_MISSING_TIME)
				{
					m_rgObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
				}
				else if (m_rgObjTrackInfo[i].m_iObjInfoCount >= 2)
				{
					m_rgObjTrackInfo[i].m_objTrackState = OBJ_TRACK_MOVING;
				}
			}
			break;
		case OBJ_TRACK_MOVING:
			if (!fIsManRoad)
			{
				// 机动车道建立的跟踪
				if (m_rgObjTrackInfo[i].m_iMissingCount >= 3)
				{
					m_rgObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
				}
			}
			else
			{
				// 行人道建立的跟踪
				if (m_rgObjTrackInfo[i].m_iMissingCount >= WALK_MAN_MISSING_TIME)
				{
					m_rgObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
				}
			}
			break;
		}
	}

	//多个重叠的Track只保留一个
	for (int i = 0; i < m_cObjTrackInfo; i++)
	{
		if (m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_INIT ||
			!m_rgObjTrackInfo[i].m_fTrack)
		{
			continue;
		}
		for (int j = i + 1; j < m_cObjTrackInfo; j++)
		{
			if (m_rgObjTrackInfo[j].m_objTrackState == OBJ_TRACK_INIT ||
				!m_rgObjTrackInfo[j].m_fTrack)
			{
				continue;
			}
			if (m_rgObjTrackInfo[i].LastInfo().m_rcFG == m_rgObjTrackInfo[j].LastInfo().m_rcFG)
			{

				if (m_rgObjTrackInfo[i].CheckImageClear() && !m_rgObjTrackInfo[j].CheckImageClear() )
				{
					m_rgObjTrackInfo[i].Clear(pProcessRespond);
				}
				else if( !m_rgObjTrackInfo[i].CheckImageClear() && m_rgObjTrackInfo[j].CheckImageClear() )
				{
					m_rgObjTrackInfo[j].Clear(pProcessRespond);
				}
				else if (m_rgObjTrackInfo[i].m_iObjInfoCount >= m_rgObjTrackInfo[j].m_iObjInfoCount)
				{
					m_rgObjTrackInfo[j].Clear(pProcessRespond);
				}
				else
				{
					m_rgObjTrackInfo[i].Clear(pProcessRespond);
				}
/*
				if (m_rgObjTrackInfo[i].m_iObjInfoCount >= m_rgObjTrackInfo[j].m_iObjInfoCount)
				{
					m_rgObjTrackInfo[j].Clear(pProcessRespond);
				}
				else
				{
					m_rgObjTrackInfo[i].Clear(pProcessRespond);
				}
*/
			}
		}
	}
	return S_OK;
}

bool CTrackerImpl::IsCanOutputReverseRun(CObjTrackInfo* pObj)
{
	bool fRet = false;
	if( pObj == NULL ) return fRet;

	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcVideoDetArea;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDetArea, rcMax, m_rcVideoDetArea));

	CRect rcFirstFG = pObj->m_rgObjInfo[0].m_rcFG;
	CRect rcLastFG = pObj->LastInfo().m_rcFG;

	int iVoteDistance = rcVideoDetArea.Height() * 5 / 10;
	int iDistance = 0;
	switch (m_pParam->g_MovingDirection)
	{
	case MD_TOP2BOTTOM:
		iDistance = rcLastFG.CenterPoint().y - rcFirstFG.CenterPoint().y;
		if( iDistance < 0 && abs(iDistance) > iVoteDistance )
		{
			fRet = true;
		}
		break;
	case MD_BOTTOM2TOP:	
		iDistance = rcFirstFG.CenterPoint().y - rcLastFG.CenterPoint().y;
		if( iDistance < 0 && abs(iDistance) > iVoteDistance )
		{
			fRet = true;
		}
		break;
	}

	return fRet;
}

HRESULT CTrackerImpl::ShowObjTracks(
	PROCESS_ONE_FRAME_PARAM* pParam, 
	int iVideoID,
	PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	)
{
//	if (NULL == pCurImage)
//	{
//		return S_FALSE;
//	}
//	if (!m_pCallback)
//	{
//		return S_FALSE;
//	}
	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcSearchArea;
	CRect rcVideoDetArea;
	int iVoteDistance;
	RTN_HR_IF_FAILED(MapParamRect(rcSearchArea, rcMax, m_pParam->g_rectDetectionArea));
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDetArea, rcMax, m_rcVideoDetArea));
	for (int i = 0; i < m_cObjTrackInfo; i++)
	{
		//当前正在跟踪车牌的跳过
		bool fPlateTracking = false;
		for (int j = 0; j < m_cTrackInfo; j++)
		{
			if (m_rgTrackInfo[j].m_State == tsInit ||
				m_rgTrackInfo[j].m_State == tsWaitRemove)
			{
				continue;
			}
			if (m_rgObjTrackInfo[i].MatchPlateTrackID(m_rgTrackInfo[j].m_nID))
			{
				fPlateTracking = true;
				break;
			}
		}
		// 收费站有过牌的一直标记为有牌
		if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
		{
			if (m_rgObjTrackInfo[i].m_cPlateTrackID != 0)
			{
				fPlateTracking = true;
			}
		}

		//输出车辆到达信号
		if (m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_MOVING &&
			!m_rgObjTrackInfo[i].m_fCarArrived &&
			!fPlateTracking &&
			(m_rgObjTrackInfo[i].m_iObjInfoCount >= 5 && GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE
			|| m_rgObjTrackInfo[i].m_iObjInfoCount >= 4 && GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE))
		{
			// zhaopy todo... 逆行的车辆要第一时间抓拍，不然开出去太远有时车身都会看不全。
			CRect rcPos = m_rgObjTrackInfo[i].LastInfo().m_rcFG;
			//int iPosY = m_iCarArrivedPos * m_nHeight / 100;
			int iPosY = m_iCarArrivedPosNoPlate * m_nHeight / 100;
			// zhaopy 0624
			bool fIsMoving = false;
			int iMove = abs(m_rgObjTrackInfo[i].LastInfo().m_rcFG.CenterPoint().y - m_rgObjTrackInfo[i].m_rgObjInfo[0].m_rcFG.CenterPoint().y);
			if( iMove > (m_nHeight * 10 / 100) ) fIsMoving = true;
			if ( m_pParam->g_MovingDirection == MD_TOP2BOTTOM && rcPos.bottom > iPosY)
			{
				m_rgObjTrackInfo[i].m_nCarArrivedCount++;
				if (m_rgObjTrackInfo[i].m_nCarArrivedCount > 1 
					&& (fIsMoving || GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE))
				{
					//通知车辆到达
					// zhaopy 0624
					if ( m_iAvgY < 20 
						&& CObjectDetector::m_iNightPlus == 1 
						&& CObjectDetector::m_iProcessType == 2 )
					{
						if (!RainyFlashControl(&rcPos, iVideoID))
						{
							m_rgObjTrackInfo[i].m_nCarArrivedCount = 0;
							continue;
						}
					}

					ProcessNoPlateCarArrive(&m_rgObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
				}
			}
			else if( m_pParam->g_MovingDirection == MD_BOTTOM2TOP &&  rcPos.bottom < iPosY)
			{
				m_rgObjTrackInfo[i].m_nCarArrivedCount++;
				if (m_rgObjTrackInfo[i].m_nCarArrivedCount > 1 && fIsMoving)
				{
					//通知车辆到达
					ProcessNoPlateCarArrive(&m_rgObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
				}
			}
		}
		//输出无牌车信号
		//电警情况用模板跟俚慕峁词涑鑫夼瞥盗?
		BOOL fIsEPMode = false;
        if(1 == m_iNoPlateDetMode)
        {
            fIsEPMode = true;
        }
        
		if ((GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE && m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_MOVING
				|| m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_END)
				&& (GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE
				|| m_rgObjTrackInfo[i].LastInfo().m_rcFG.bottom > m_iCarArrivedPos * m_nHeight / 100)
				&&
			!m_rgObjTrackInfo[i].m_fVoted &&
			!fPlateTracking &&
			m_rgObjTrackInfo[i].m_iObjInfoCount >= 6 && (!fIsEPMode))
		{
			CRect rcFirstFG = m_rgObjTrackInfo[i].m_rgObjInfo[0].m_rcFG;
			CRect rcLastFG = m_rgObjTrackInfo[i].LastInfo().m_rcFG;

			int nObjCountTemp(50000);

			if ((CObjectDetector::m_iProcessType == 1) || (CObjectDetector::m_iProcessType == 2))
			{
				// 厦门需要针对数量进行判断
				nObjCountTemp = m_iVoteFrameCount;
			}

			// 厦门行人输出
			if (m_rgObjTrackInfo[i].m_iObjInfoCount > nObjCountTemp)
			{
				int iDistance;
				iDistance = rcLastFG.CenterPoint().y - rcFirstFG.CenterPoint().y;
				int iVoteDistance = rcVideoDetArea.Height() * 25 / 100;
				if ( (m_pParam->g_MovingDirection == MD_TOP2BOTTOM && iDistance < 0)
					|| (m_pParam->g_MovingDirection == MD_BOTTOM2TOP && iDistance > 0) )
				{
					iDistance = abs(iDistance);
					iVoteDistance >>= 1;
				}

				//横向位移
				int iDistanceX = abs(rcLastFG.CenterPoint().x - rcFirstFG.CenterPoint().x);
				int iVoteDistanceX = rcVideoDetArea.Width() * 50 / 100;

				
				if(iDistance >= iVoteDistance
					|| (iDistanceX > iVoteDistanceX && m_iCheckAcross == 1) )
				{
					//增加城市卡口雨夜情况的多检
					// zhaopy 0624
					BOOL fRainyOutPut = true;
					if( m_LightType == NIGHT 
						&& CObjectDetector::m_iProcessType == 2 
						&& CObjectDetector::m_iNightPlus == 1 
						&& m_pParam->g_MovingDirection == MD_TOP2BOTTOM)
					{

						fRainyOutPut = RainyObjOutPutControl(&rcLastFG, i);
					}

					if (!m_rgObjTrackInfo[i].m_fCarArrived && fRainyOutPut)
					{
						//通盗镜酱?
						ProcessNoPlateCarArrive(&m_rgObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
					}

					if( fRainyOutPut )
					{
					m_rgObjTrackInfo[i].m_fVoted = true;
					FireNoPlateCarLeftEvent(pParam, m_rgObjTrackInfo[i], pProcessRespond);
					}
					
					//HV_Trace("TrackObjCount:%d, Dis:%d, Vote:%d\n", m_rgObjTrackInfo[i].m_iObjInfoCount, iDistance, iVoteDistance);
				}
				else
				{
					//HV_Trace("ERROR: TrackObjCount:%d, Dis:%d, Vote:%d\n", m_rgObjTrackInfo[i].m_iObjInfoCount, iDistance, iVoteDistance);
				}
			}
			else
			{
				int iDistance;
				MovingDirection movingDirection;
				bool fMove = false;
				bool fTollGateOut = false;

				switch (m_pParam->g_MovingDirection)
				{
				case MD_TOP2BOTTOM:
//					iDistance = rcLastFG.CenterPoint().y - rcFirstFG.CenterPoint().y;
					iDistance = MAX_INT((rcLastFG.bottom - rcFirstFG.bottom),
						rcLastFG.CenterPoint().y - rcFirstFG.CenterPoint().y);
					if (m_LightType == NIGHT && rcFirstFG.top < (rcVideoDetArea.bottom >> 3)
						&& ((rcFirstFG.CenterPoint().x > rcVideoDetArea.right * 3 / 4 
						&& rcLastFG.CenterPoint().x > rcVideoDetArea.right * 3 / 4)
						|| (rcFirstFG.CenterPoint().x < rcVideoDetArea.right / 4
						&& rcLastFG.CenterPoint().x > rcVideoDetArea.right / 4)))	// 晚上两边的出牌阈值降低
					{
						iVoteDistance = rcVideoDetArea.Height() * 4 / 10;
					}
					else
					{
						iVoteDistance = rcVideoDetArea.Height() * 5 / 10;
					}
					if (iDistance < 0)
					{
						iDistance = -iDistance;
						movingDirection = MD_BOTTOM2TOP;
						if(CObjectDetector::m_iNightPlus == 1)
						{
							iVoteDistance >>= 1;
						}
					}
					else
					{
						movingDirection = MD_TOP2BOTTOM;
					}
                    static unsigned int s_LastOutNoPlateTime = 0;


                    if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
                    {
                    	bool fMoving = false;
                    	// 无牌车Moving状态必须停下来才给出结果
                    	if (m_rgObjTrackInfo[i].m_objTrackState == OBJ_TRACK_MOVING)
                    	{
                    		CRect rcPos = m_rgObjTrackInfo[i].LastInfo().m_rcFG;
        					CRect rcPosPre = m_rgObjTrackInfo[i].m_rgObjInfo[m_rgObjTrackInfo[i].m_iObjInfoCount-3].m_rcFG;
        					if (rcPosPre.IntersectRect(&rcPosPre, &rcPos))
        					{
        						if (rcPosPre.Area() * 100 < rcPos.Area() * 95)  // 高度相交
        						{
        							fMoving = true;
        						}
        					}
                    	}
                    	if (!fMoving && m_rgObjTrackInfo[i].m_rgObjInfo[0].m_rcFG.bottom < (m_iCarArrivedPos + 15) * m_nHeight / 100
								&& m_rgObjTrackInfo[i].m_rgObjInfo[0].m_rcFG.bottom + 3 * m_nHeight / 100 < m_rgObjTrackInfo[i].LastInfo().m_rcFG.bottom
								&& Venus_GetSystemTick() - s_LastOutNoPlateTime > 1000 * 10)   // 10s
						{
							fTollGateOut = true;
						}
                    }

					if ( fTollGateOut ||
							((GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE) &&
							 iDistance > iVoteDistance &&
							((movingDirection == MD_TOP2BOTTOM && rcSearchArea.top <= rcLastFG.top) ||
									(movingDirection == MD_BOTTOM2TOP && rcSearchArea.bottom >= rcLastFG.bottom))))
					{
						//增加城市卡口雨夜情况的多检
						// zhaopy 0624
						BOOL fRainyOutPut = true;
						if( m_LightType == NIGHT 
							&& CObjectDetector::m_iProcessType == 2 
							&& CObjectDetector::m_iNightPlus == 1 
							&& movingDirection == MD_TOP2BOTTOM)
						{

							fRainyOutPut = RainyObjOutPutControl(&rcLastFG, i);

						}
						if (!m_rgObjTrackInfo[i].m_fCarArrived && fRainyOutPut)
						{
							//通知车辆到达
							ProcessNoPlateCarArrive(&m_rgObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
						}

						if( fRainyOutPut )
						{
						m_rgObjTrackInfo[i].m_fVoted = true;
						FireNoPlateCarLeftEvent(pParam, m_rgObjTrackInfo[i], pProcessRespond);
						s_LastOutNoPlateTime = Venus_GetSystemTick();  //记录上次无牌出牌时间
						}
						
					}
					break;
				case MD_BOTTOM2TOP:
					iDistance = rcLastFG.CenterPoint().y - rcFirstFG.CenterPoint().y;
					iVoteDistance =rcVideoDetArea.Height() * 5 / 10;
					if (iDistance < 0)
					{
						iDistance = -iDistance;
						movingDirection = MD_BOTTOM2TOP;						
					}
					else
					{
						movingDirection = MD_TOP2BOTTOM;
						if(CObjectDetector::m_iNightPlus == 1)
						{
							iVoteDistance >>= 1;
						}
					}
					//2011-3-15 韦开拓 电警模式下去掉大车车身多检
					if( m_pParam->m_fUsedTrafficLight && m_pParam->g_MovingDirection == MD_BOTTOM2TOP && movingDirection == MD_BOTTOM2TOP && m_fFilterMoreReview )
					{						
						if(iDistance > iVoteDistance && FAILED(CheckNoPlate(&m_rgObjTrackInfo[i])))
						{
							fMove = true;
							//HV_Trace("Filter More Review...\n");
						}
					}

					if (iDistance > iVoteDistance && !fMove &&
						((movingDirection == MD_TOP2BOTTOM && rcSearchArea.top <= rcLastFG.top) ||
						(movingDirection == MD_BOTTOM2TOP && rcSearchArea.bottom >= rcLastFG.bottom)))
					{
						if (!m_rgObjTrackInfo[i].m_fCarArrived)
						{
							//通知车辆到达
							ProcessNoPlateCarArrive(&m_rgObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
						}
						m_rgObjTrackInfo[i].m_fVoted = true;
						FireNoPlateCarLeftEvent(pParam, m_rgObjTrackInfo[i], pProcessRespond);
					}
					break;
				case MD_LEFT2RIGHT:
				case MD_RIGHT2LEFT:
					iDistance = rcLastFG.CenterPoint().x - rcFirstFG.CenterPoint().x;
					if (iDistance < 0)
					{
						iDistance = -iDistance;
						movingDirection = MD_RIGHT2LEFT;
					}
					else
					{
						movingDirection = MD_LEFT2RIGHT;
					}
					iVoteDistance = rcVideoDetArea.Width() * 5 / 10;
					if (iDistance > iVoteDistance &&
						((movingDirection == MD_LEFT2RIGHT && rcSearchArea.left <= rcLastFG.left) ||
						(movingDirection == MD_RIGHT2LEFT && rcSearchArea.right >= rcLastFG.right)))
					{
						m_rgObjTrackInfo[i].m_fVoted = true;
						FireNoPlateCarLeftEvent(pParam, m_rgObjTrackInfo[i], pProcessRespond);
					}
					break;
				}
			}
		}
	}


	//电警无牌车新增的跟踪出牌以及闪光抓拍的条件
    //这里应该设置一个开关
    if(1 == m_iNoPlateDetMode)
    {
        for (int i = 0; i < m_cEPObjTrackInfo; i++)
        {
			/*char szInfo[32] = {0};
			sprintf(szInfo, "S:%d A:%d C:%d N:%d\n"
			       ,m_rgEPObjTrackInfo[i].m_objTrackState
			       ,m_rgEPObjTrackInfo[i].m_fCarArrived
			       ,m_rgEPObjTrackInfo[i].m_iObjInfoCount
			       ,m_rgEPObjTrackInfo[i].m_fNewTem
			       );
			strcat(g_szDebugInfo, szInfo);*/
            //输出车辆到达信号
            if (m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_TEM&&
                !m_rgEPObjTrackInfo[i].m_fCarArrived && m_rgEPObjTrackInfo[i].m_fEPOutput)
            {
                CRect rcPos = m_rgEPObjTrackInfo[i].LastInfo().m_rcFG;
                //int iPosY = m_iCarArrivedPos * m_nHeight / 100;
                int iPosY = m_iCarArrivedPosNoPlate * m_nHeight / 100;
                if(m_pParam->g_MovingDirection == MD_BOTTOM2TOP && rcPos.bottom < iPosY)
                {
                    m_rgEPObjTrackInfo[i].m_nCarArrivedCount++;
                    if (m_rgEPObjTrackInfo[i].m_nCarArrivedCount > 1)
                    {
                        //通知车辆到达
                        ProcessNoPlateCarArrive(&m_rgEPObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
                    }
                }
            }
            //输出跟踪结束的车辆信号
            if (m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_END 
                && !m_rgEPObjTrackInfo[i].m_fVoted 
                && m_rgEPObjTrackInfo[i].m_fEPOutput
                && m_rgEPObjTrackInfo[i].m_iObjInfoCount >= 6  
                && m_rgEPObjTrackInfo[i].m_fNewTem
                )
            {
                CRect rcFirstFG = m_rgEPObjTrackInfo[i].m_rgObjInfo[0].m_rcFG;
                CRect rcLastFG = m_rgEPObjTrackInfo[i].LastInfo().m_rcFG;
                int iDistance;
                int iTemDis, iVoteTemDis;
                MovingDirection movingDirection;
                bool fMove = false;
                switch (m_pParam->g_MovingDirection)
                {
                case MD_BOTTOM2TOP:
                    iDistance = rcLastFG.CenterPoint().y - rcFirstFG.CenterPoint().y;
                    iTemDis = rcLastFG.CenterPoint().y - m_rgEPObjTrackInfo[i].m_iTemYBegin;
                    iVoteDistance = rcVideoDetArea.Height() >> 1;
                    //iVoteDistance = rcVideoDetArea.Height() * 4 / 10;
                    iVoteTemDis = rcVideoDetArea.Height()*4/10;
                    if (iDistance < 0)
                    {
                        iDistance = -iDistance;
                        movingDirection = MD_BOTTOM2TOP;						
                    }
                    else
                    {
                        movingDirection = MD_TOP2BOTTOM;
                    }

                    if (iTemDis > 0)continue;
                    if (iTemDis < 0)iTemDis = -iTemDis;


                    //HV_Trace("\n id:[%d],dis:[%d]",  m_rgEPObjTrackInfo[i].m_Feature.iObjId, iDistance);
                    if ((iDistance > iVoteDistance || iTemDis > iVoteTemDis) &&
                    	((movingDirection == MD_BOTTOM2TOP && rcSearchArea.bottom >= rcLastFG.bottom)))
                    {
                        if (!m_rgEPObjTrackInfo[i].m_fCarArrived)
                        {
                            //通知车辆到达
                            ProcessNoPlateCarArrive(&m_rgEPObjTrackInfo[i], pParam->dwImageTime, 0.0, pProcessRespond);
                        }
                        m_rgEPObjTrackInfo[i].m_fVoted = true;
                        FireNoPlateCarLeftEvent(pParam, m_rgEPObjTrackInfo[i], pProcessRespond);
                    }
                    break;
                }
            }
        }
    }
        
	return S_OK;
}

HRESULT CTrackerImpl::TrackingObjByTem(PROCESS_ONE_FRAME_PARAM* pParam, HV_COMPONENT_IMAGE &imgCurImage, PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
    CRect rcMax(0, 0, m_nWidth, m_nHeight);
    CRect rcFullSearch;
    RTN_HR_IF_FAILED(MapParamRect(rcFullSearch, rcMax, m_pParam->g_rectDetectionArea));
    CRect rcVideoDet;
    RTN_HR_IF_FAILED(MapParamRect(rcVideoDet, rcMax, m_rcVideoDetArea));

    //视频跟踪
    for (int i = 0; i < m_cEPObjTrackInfo; i++)
    {
        m_rgEPObjTrackInfo[i].m_fTrack = FALSE;
    }
    
    //建立新的跟踪
    
    //for (int i = 0; i < m_cDetected; i++)
    for (int i = 0; i < m_cScanEP; i++)
    {
        CObjInfo objInfo;
        objInfo.m_rcFG = m_rgrcScanEP[i];
        //修正
        objInfo.m_dwFrameTime = pParam->dwImageTime;
        objInfo.m_nFrameNo = pParam->dwFrameNo;
        objInfo.m_nCarType = CT_UNKNOWN;
        
        //判断是否与当前模板跟踪的物体相交
        bool fInterSet = false;
        if(!fInterSet)
        {
            for (int j = 0; j < m_cEPObjTrackInfo; j++)
            {
                if (m_rgEPObjTrackInfo[j].m_objTrackState == OBJ_TRACK_TEM)
                {
                    HiVideo::CRect rInterSet;
                    HiVideo::CRect lastRect = m_rgEPObjTrackInfo[j].LastInfo().m_rcFG;
                    if(rInterSet.IntersectRect(lastRect, objInfo.m_rcFG))
                    {
                        fInterSet = true;
                    }
                }
            }
        }
         //
        if(!fInterSet)
        {
            for (int j = 0; j < m_cTrackInfo; j++)
            {
                if (m_rgTrackInfo[j].m_iVideoID != pParam->iVideoID )
                {
                    continue;
                }

				if( m_rgTrackInfo[j].m_State == tsNew 
				||	m_rgTrackInfo[j].m_State == tsWaitRemove)continue;
                //TODO: 以后也许会根据车牌的更多信息来修正无牌车的跟踪建立
                if (m_rgrcScanEP[i].IntersectsWith(m_rgTrackInfo[j].LastInfo().rcPos))
                {
                    fInterSet = true;
                    break;
                }
            }
        }
         
        if (fInterSet)continue;
        

        if (m_cEPObjTrackInfo == s_knMaxObjCandidate)
        {
            m_cEPObjTrackInfo--;
        }
        m_rgEPObjTrackInfo[m_cEPObjTrackInfo].Clear(pProcessRespond);

        int iMaxArea = 0;
        int iObjIndex(-1);
        for (int j = 0; j < m_cEPObjTrackInfo; j++)
        {
            if (m_rgEPObjTrackInfo[j].m_iVideoID == pParam->iVideoID && m_rgEPObjTrackInfo[j].m_objTrackState != OBJ_TRACK_TEM)
            {
                HiVideo::CRect rcIntersect;
                if (rcIntersect.IntersectRect(&(m_rgEPObjTrackInfo[j].LastInfo().m_rcFG), &(objInfo.m_rcFG)))
                {
                    int iInterArea = rcIntersect.Area();
                    if (iInterArea > iMaxArea)
                    {
                        iObjIndex = j;
                        iMaxArea = iInterArea;
                    }
                }
            }
        }
        if(iObjIndex>=0)
        {

            if (m_rgEPObjTrackInfo[iObjIndex].m_objTrackState != OBJ_TRACK_TEM)
            {
                m_rgEPObjTrackInfo[iObjIndex].Process(objInfo, imgCurImage, rcVideoDet, m_iLastLightStatus, m_iCurLightStatus, pParam, pProcessRespond);
            } 
        } else 
        {
            //如果不和任何的模板跟踪相交
            //CPoint ptTemp = m_rgrcDetected[i].CenterPoint();
            //CRect r = m_rgrcDetected[i];
            CPoint ptTemp = m_rgrcScanEP[i].CenterPoint();
            CRect r = m_rgrcScanEP[i];
            CRect rInter(0,0,0,0);
            BOOL fHasTem = false;
            for (int k = 0; k < m_cEPObjTrackInfo; k++)
            {
                if(m_rgEPObjTrackInfo[k].m_objTrackState != OBJ_TRACK_TEM)continue;
                CRect rTem = m_rgEPObjTrackInfo[k].LastInfo().m_rcFG;
                if (rInter.IntersectRect(r, rTem))
                {
                    fHasTem = true;
                    break;
                } 
            }

            if(iObjIndex < 0 && rcFullSearch.Contains(&ptTemp) && !fHasTem)
            {
                //跟踪建立
                
                if (m_cEPObjTrackInfo == s_knMaxObjCandidate)
                {
                    m_cEPObjTrackInfo--;
                }
                m_rgEPObjTrackInfo[m_cEPObjTrackInfo].Clear(pProcessRespond);
                RTN_HR_IF_FAILED(m_rgEPObjTrackInfo[m_cEPObjTrackInfo].New(pParam, objInfo, imgCurImage, m_iLastLightStatus, m_iCurLightStatus, false, pProcessRespond));
            }
        }

    //这几行写的是啥？应用的代码先保留
    /*for (int k = 0; k < cPlateTrackID; k++)
    {
        m_rgEPObjTrackInfo[m_cEPObjTrackInfo].AddPlateTrackID(rgiPlateTrackID[k]);
    }*/
    m_cEPObjTrackInfo++;
    
    }
    //已有的跟踪
    //显示预测框体
    //m_RectNum[3] = 0;
    for(int i = 0; i < m_cEPObjTrackInfo; i++)
    {
        if(m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_TEM)
        {
            //无牌车的模板跟踪，预测区域
            CRect rPredict = GetEPNoPlatePredictRect(pParam->dwImageTime, m_rgEPObjTrackInfo + i);
            //HV_Trace("\nrPredict.top:[%d], rPredict.left:[%d], rPredict.bottom:[%d], rPredict.top:[%d], area:[%d]", rPredict.top, rPredict.left, rPredict.bottom, rPredict.right, (rPredict.bottom - rPredict.top)*(rPredict.right - rPredict.left));
            rPredict.IntersectRect(&rPredict, &rcMax);
            //显示预测框体
            //m_DrawRect[3][m_RectNum[3]] = rPredict;
            //m_RectNum[3]++;
            //if(m_RectNum[3] >= DRAW_RECT_LEN)m_RectNum[3] = DRAW_RECT_LEN - 1;
            //DWORD32 dwBeginTick = GetSystemTick();

			// zhaopy 预测的区域有时会是无效的。预测函数应该有问题。
			if( !rPredict.IsRectEmpty() )
			{
				RTN_HR_IF_FAILED(m_rgEPObjTrackInfo[i].ProcessEx(pParam, imgCurImage, rcVideoDet, rPredict, m_pParam->m_iDetectReverseEnable, m_iLastLightStatus, m_LightType, pProcessRespond));
			}

			// RTN_HR_IF_FAILED(m_rgEPObjTrackInfo[i].ProcessEx(pParam, imgCurImage, rcVideoDet, rPredict, m_pParam->m_iDetectReverseEnable, m_iLastLightStatus, m_LightType, pProcessRespond));
			//HV_Trace("Perpare ProcessEx time = %d\n", GetSystemTick() - dwBeginTick);
        }
    }

    //跟踪状态转移
    for (int i = 0; i < m_cEPObjTrackInfo; i++)
    {
        //bool fIsManRoad = m_rgEPObjTrackInfo[i].m_fIsMan;			// 行人道建立的?

        if (m_rgEPObjTrackInfo[i].m_iVideoID != pParam->iVideoID ||
            m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_INIT)
        {
            continue;
        }
        if (!m_rgEPObjTrackInfo[i].m_fTrack)
        {
            m_rgEPObjTrackInfo[i].m_iMissingCount++;
        }
        else
        {
            if (m_rgEPObjTrackInfo[i].LastInfo().m_rcFG == CRect(0, 0, 0, 0))
            {
                m_rgEPObjTrackInfo[i].m_iMissingCount++;
                m_rgEPObjTrackInfo[i].LastInfo().Clear();
                m_rgEPObjTrackInfo[i].m_iObjInfoCount--;
            } 
            else
            {
                m_rgEPObjTrackInfo[i].m_iMissingCount = 0;
            }
        }
        int iLineY;
        BOOL fTransToTemState = false;
        switch (m_rgEPObjTrackInfo[i].m_objTrackState)
        {
        case OBJ_TRACK_NEW:

                // 机动车道建立的跟踪
                if (m_rgEPObjTrackInfo[i].m_iMissingCount >= 1)
                {
                    m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
                }
                else if (m_rgEPObjTrackInfo[i].m_iObjInfoCount >= 3)
                {
                    m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_MOVING;
                }
                break;
        case OBJ_TRACK_MOVING:
            if (m_rgEPObjTrackInfo[i].m_iMissingCount >= 1)
            {
                m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
            }
            //iLineY = rcFullSearch.top + (100 - CObjectDetector::m_iEPOutPutDetectLine)*rcFullSearch.Height()/100;
            iLineY = (100 - CObjectDetector::m_iEPOutPutDetectLine) * rcMax.Height()/100;
            fTransToTemState = m_rgEPObjTrackInfo[i].IsTransToTemState(imgCurImage, m_iLastLightStatus, m_iCurLightStatus, rcVideoDet.TopLeft(),  iLineY);
            if (fTransToTemState)
            {
                //判断是否有与旧的无牌车跟踪距离太近
                CRect roi = m_rgEPObjTrackInfo[i].LastInfo().m_rcFG;
                for (int j = 0; j < m_cEPObjTrackInfo; j++)
                {
                    if(j != i && m_rgEPObjTrackInfo[j].m_objTrackState == OBJ_TRACK_TEM)
                    {
                        //距离判断
                        CRect r = m_rgEPObjTrackInfo[j].LastInfo().m_rcFG;
                        CRect rVir = r;
                        rVir.left -= r.Width()*15/10;
                        rVir.right += r.Width()*15/10;
                        rVir.bottom += r.Height()*4;
                        rVir.IntersectRect(&rVir, &rcMax);
                        if (rVir.IntersectRect(&rVir, &roi))
                        {
                                fTransToTemState = false;
                                break;
                        }
                    }
                } 
            }
            //if (!fTransToTemState)
            //{
            //    m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
            //}
            //if (m_rgEPObjTrackInfo[i].IsTransToTemState(iVideoID,
            //    pImage,
            //    m_iLastLightStatus, m_iCurLightStatus,
            //    rcVideoDet.TopLeft(),  iLineY))
            if (fTransToTemState)
            {
                m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_TEM;
                CObjInfo objInfo = m_rgEPObjTrackInfo[i].LastInfo();
                HvSize EpSize;
                //估计出一个车牌的宽度,暂时用车道比例估计，之后如果要准确的话最好还是有那个5字车宽估计
                CRect roi = objInfo.m_rcFG;
                
                int iLineW = 100;
                MatchRoad(roi.CenterPoint().x, roi.CenterPoint().y, &iLineW);
                if (iLineW > 0)
                {
                    EpSize.width = iLineW/6;
                    EpSize.height = EpSize.width>>1;
                } else 
                {
                    EpSize.width = 50;
                    EpSize.height = EpSize.width>>1;
                }
                DWORD32 dwBeginTick = GetSystemTick();
                RTN_HR_IF_FAILED(m_rgEPObjTrackInfo[i].NewTemEx2(pParam->iVideoID, objInfo,
                    pParam, imgCurImage, pProcessRespond,
                    m_iLastLightStatus, m_iCurLightStatus,
                    rcVideoDet.TopLeft(), EpSize));
                    m_rgEPObjTrackInfo[i].m_fNewTem = true;
                    //HV_Trace("Perpare new tem time = %d\n", GetSystemTick() - dwBeginTick);
            }
            
            break;
        case OBJ_TRACK_TEM:
            if (m_rgEPObjTrackInfo[i].m_iMissingCount >= 1)
            {
                m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
            }
            //TODO 如果与车牌的跟踪相交或与黄牌虚拟区域相交则结束这个跟踪
             BOOL fConTemEnd = false; 
             HiVideo::CRect rNoPlate = m_rgEPObjTrackInfo[i].LastInfo().m_rcFG;
             CTrackInfo* pTrack = NULL;
             int iLineNoPlateNum = 0;
             iLineNoPlateNum = MatchRoad(rNoPlate.CenterPoint().x, rNoPlate.CenterPoint().y);
             int iEPLineY = (100 - CObjectDetector::m_iEPOutPutDetectLine)*rcMax.Height()/100;
            for (int j = 0; j < m_cTrackInfo; j++)
            {
                pTrack = m_rgTrackInfo + j;
                HiVideo::CRect rPlate = pTrack->LastInfo().rcPos;
                //如果与模板匹配相交则不输出
                PlateInfo plateInfo = pTrack->LastInfo();
                PLATE_COLOR plateColor = plateInfo.color;
                PLATEINFO_TYPE plateType = plateInfo.nInfoType;
                HiVideo::CRect rInterSet;
                
                if( pTrack->m_State == tsNew 
                    ||  pTrack->m_State == tsWaitRemove)continue;

                if(rInterSet.IntersectRect(rPlate, rNoPlate))
                {
                    fConTemEnd = true;
                }
                //if ( !fConTemEnd && plateColor == PC_YELLOW && plateType == PI_LPR)
                //{
                //    CRect rectVir = rPlate;
                //    int iWidth = rectVir.Width()*12/10;
                //    int iHeight = rectVir.Height();
                //    rectVir.left -= iWidth;
                //    rectVir.right += iWidth;
                //    rectVir.top -= iHeight*8;
                //    rectVir.IntersectRect(&rectVir,&rcMax);
                //    if(rInterSet.IntersectRect(rectVir, rNoPlate))
                //    {
                //        fConTemEnd = true;
                //        //m_rgEPObjTrackInfo[i].m_fEPOutput =false;
                //    }
                //}
                
                if ( !fConTemEnd && plateType == PI_LPR) 
                {
                    if (plateColor == PC_YELLOW)
                    {
                        //计算其所在车道
                        int iLineYellow = MatchRoad(rPlate.CenterPoint().x, rPlate.CenterPoint().y);
                        if (( iLineYellow == iLineNoPlateNum && rPlate.top > rNoPlate.bottom) 
                            && (rPlate.CenterPoint().y > iEPLineY )
                            && (rPlate.top - rNoPlate.bottom) < (rcMax.Height()>>1))
                        {
                            fConTemEnd = true;
                            m_rgEPObjTrackInfo[i].m_fEPOutput = false;
                        }
                    } else 
                    {
                        //其他牌
                            CRect rectVir = rPlate;
                            int iWidth = rectVir.Width()*12/10;
                            int iHeight = rectVir.Height();
                            
                            rectVir.left -= (iWidth);
                            rectVir.right += (iWidth);
                            rectVir.top -= (iHeight<<3);
                            
                            rectVir.IntersectRect(&rectVir,&rcMax);
                            if(rInterSet.IntersectRect(rectVir, rNoPlate))
                            {
                                fConTemEnd = true;
                                //if(rNoPlate.bottom > iEPLineY)fConTemEnd = true; 
                                //m_rgEPObjTrackInfo[i].m_fEPOutput = false;
                            }
                    }
                } 
                if (fConTemEnd)
                {
                    break;
                }
            }
            if (fConTemEnd)
            {
                m_rgEPObjTrackInfo[i].m_objTrackState = OBJ_TRACK_END;
            }
            break;
        }
    }

    
    //检查电警中每一个已经是模板跟踪状态的无牌车跟踪，用模型的方法去识别它到底是不是一个真的车辆
    if(m_LightType == DAY && CTrackInfo::m_fEPUseCarTailModel == 1)    
    {
        CheckEPNoPlateConfidence2(&imgCurImage, pParam->iVideoID);
    }

	return S_OK;
    
}

HRESULT CTrackerImpl::CalcCarSpeed(float &fltCarSpeed, float &fltScaleOfDistance, CTrackInfo &TrackInfo)
{
	const int iCalsCount = 3;
	float rgfltCarSpeed[iCalsCount] = {0.0f}, rgfltScale[iCalsCount] = {0.0f};
	if (m_fEnableScaleSpeed &&
		TrackInfo.m_cPlateInfo > 1)
	{
		// 取当前车牌和第一个车牌的计算平均速度，作为当前车牌的速度
		CRect rcBegin = TrackInfo.m_rgPlateInfo[0].rcPos.TopLeft() + TrackInfo.m_rgPlateInfo[0].rcPlatePos;
		int iInfoIndex = TrackInfo.m_cPlateInfo - 1;
		for (int i = 0; i < iCalsCount; i++)
		{
			while (iInfoIndex > 0)
			{
				if (TrackInfo.m_fVoted &&
					TrackInfo.m_rgPlateInfo[iInfoIndex].nInfoType != PI_LPR)
				{
					iInfoIndex--;
				}
				else
				{
					break;
				}
			}
			if (iInfoIndex == 0) break;
			CRect rcEnd = TrackInfo.m_rgPlateInfo[iInfoIndex].rcPos.TopLeft() + TrackInfo.m_rgPlateInfo[iInfoIndex].rcPlatePos;
			//需要用大的车牌来预测误差
			if (rcEnd.Width() < rcBegin.Width())
			{
				rgfltCarSpeed[i] = m_pScaleSpeed->CalcCarSpeedNewMethod(
					rcEnd,
					rcBegin,
					TrackInfo.m_rgPlateInfo[iInfoIndex].dwFrameTime - TrackInfo.m_rgPlateInfo[0].dwFrameTime,
					true,
					TrackInfo.m_nVotedType,
					rgfltScale[i]
					);
			}
			else
			{
				rgfltCarSpeed[i] = m_pScaleSpeed->CalcCarSpeedNewMethod(
					rcBegin,
					rcEnd,
					TrackInfo.m_rgPlateInfo[iInfoIndex].dwFrameTime - TrackInfo.m_rgPlateInfo[0].dwFrameTime,
					true,
					TrackInfo.m_nVotedType,
					rgfltScale[i]
					);
			}
			iInfoIndex--;
		}
		for (int i = 0; i < iCalsCount - 1; i++)
		{
			for (int j = i + 1; j< iCalsCount; j++)
			{
				if (rgfltCarSpeed[i] < rgfltCarSpeed[j])
				{
					float fltTemp = rgfltCarSpeed[i];
					rgfltCarSpeed[i] = rgfltCarSpeed[j];
					rgfltCarSpeed[j] = fltTemp;
					float fltTemp1 = rgfltScale[i];
					rgfltScale[i] = rgfltScale[j];
					rgfltScale[j] = fltTemp1;
				}
			}
		}
		for (int i = iCalsCount / 2; i >= 0; i--)
		{
			fltCarSpeed = rgfltCarSpeed[i];
			fltScaleOfDistance = rgfltScale[i];
			if (fltCarSpeed > 0.0f)
			{
				break;
			}
		}
		// 速度值修正，避免出现速度值为0的情况
		// huanggr 2011-11-07
		if (fltCarSpeed < 1.0f)
		{
			fltCarSpeed = 1.0f;
		}
	}
	return S_OK;
}

HRESULT CTrackerImpl::CalcObjSpeed( float &fltCarSpeed, float &fltScaleOfDistance, CObjTrackInfo &ObjTrackInfo )
{
	const int iCalsCount = 3;
	float rgfltObjSpeed[iCalsCount] = {0.0f}, rgfltScale[iCalsCount] = {0.0f};
	if (m_fEnableScaleSpeed &&
		ObjTrackInfo.m_iObjInfoCount > 1)
	{
		// 取当前位置和第一个位置计算平均速度，作为当前运动物体的速度
		CRect rcBegin = ObjTrackInfo.m_rgObjInfo[0].m_rcFG;
		int iInfoIndex = ObjTrackInfo.m_iObjInfoCount - 1;
		for (int i = 0; i < iCalsCount; i++)
		{
			if (iInfoIndex == 0) break;
			CRect rcEnd = ObjTrackInfo.m_rgObjInfo[iInfoIndex].m_rcFG;
			//rgfltObjSpeed[i] = m_pScaleSpeed->CalcCarSpeed(
			rgfltObjSpeed[i] = m_pScaleSpeed->CalcCarSpeedNewMethod(
				rcBegin,
				rcEnd,
				ObjTrackInfo.m_rgObjInfo[iInfoIndex].m_dwFrameTime - ObjTrackInfo.m_rgObjInfo[0].m_dwFrameTime,
				false,
				PLATE_UNKNOWN,
				rgfltScale[i]
				);
			iInfoIndex--;
		}
		for (int i = 0; i < iCalsCount - 1; i++)
		{
			for (int j = i + 1; j< iCalsCount; j++)
			{
				if (rgfltObjSpeed[i] < rgfltObjSpeed[j])
				{
					float fltTemp = rgfltObjSpeed[i];
					rgfltObjSpeed[i] = rgfltObjSpeed[j];
					rgfltObjSpeed[j] = fltTemp;
					float fltTemp1 = rgfltScale[i];
					rgfltScale[i] = rgfltScale[j];
					rgfltScale[j] = fltTemp1;
				}
			}
		}
		if (ObjTrackInfo.m_iObjInfoCount > 2)
		{
			fltCarSpeed = rgfltObjSpeed[iCalsCount / 2];
			fltScaleOfDistance = rgfltScale[iCalsCount / 2];
		}
		else
		{
			fltCarSpeed = rgfltObjSpeed[0];
			fltScaleOfDistance = rgfltScale[0];
		}
	}
	return S_OK;
}

// 处理模版匹配状态的车辆
HRESULT CTrackerImpl::ProcessTemTrackState(
	CTrackInfo *pTrack,									//当前处理的跟踪
	CRect &rcTrackArea
	)
{
	// 如果跟踪丢失，则结束模版跟踪状态(正常不会丢失)
	if ((!pTrack->m_fTempTrackExists) || (!pTrack->m_fhasTemple))
	{
		pTrack->m_State = tsTemTrackEnd;
		return S_OK;
	}

	// 车辆开出跟踪区域的判断
	CRect rcLast = pTrack->LastInfo().rcPos;

	bool fInEndTrackArea = !rcTrackArea.Contains(rcLast);		// 车辆开出区域
	int nTempGap = 5;

	if (!fInEndTrackArea)
	{
		// 判断当前帧是否靠近边缘
		bool fCloseEdge = (rcLast.top - nTempGap <= rcTrackArea.top) 
			|| (rcLast.bottom + nTempGap >= rcTrackArea.bottom)
			|| (rcLast.left - nTempGap <= rcTrackArea.left)
			|| (rcLast.right + nTempGap >= rcTrackArea.right);

		if (fCloseEdge)
		{
			pTrack->m_nCloseEdgeCount++;						// 靠近边缘计数器
		}

		if (!pTrack->m_fReverseRun)
		{
			// 正向行驶
			if (pTrack->m_fTempCloseEdge)
			{
				// 上一帧已经靠近边缘
				if (!fCloseEdge)
				{
					// 有反复，马上结束跟踪
					fInEndTrackArea = true;
				}
				else
				{
					if (pTrack->m_nCloseEdgeCount >= 2)
					{
						// 靠近边缘2帧内必须结束
						fInEndTrackArea = true;
					}
				}
			}
			else
			{
				// 上一帧没有靠近边缘
				if (fCloseEdge)
				{
					pTrack->m_fTempCloseEdge = true;
				}
			}
		}
		else
		{
			// 逆向行豢拷咴翟蚪崾?
			if (fCloseEdge)
			{
				fInEndTrackArea = true;					// 逆向行驶马上结束跟踪
			}
		}
	}

	// 最后判断是否结束跟踪
	if (fInEndTrackArea)
	{
		pTrack->m_State = tsTemTrackEnd;							// 结束当前跟踪
	}

	return S_OK;
}

void CTrackerImpl::RoadInfoParamInit()
{
	CTrackInfo::m_iRoadNumber = 2;
	CTrackInfo::m_iRoadNumberBegin = 0;
	CTrackInfo::m_iStartRoadNum = 0;
	for(int i = 0; i < CTrackInfo::s_iMaxRoadNumber; ++i)
	{
		CTrackInfo::m_roadInfo[i].ptTop.x = 0;
		CTrackInfo::m_roadInfo[i].ptTop.y = 0;
		CTrackInfo::m_roadInfo[i].ptBottom.x = 0;
		CTrackInfo::m_roadInfo[i].ptBottom.y = 0;
		CTrackInfo::m_roadInfo[i].iLineType = 0;

		//strcpy(CTrackInfo::m_roadInfo[i].szRoadTypeName, "NULL");
		CTrackInfo::m_roadInfo[i].iRoadType = (int)(RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
		CTrackInfo::m_roadInfo[i].iFilterRunType = 0;
	}
}

void CTrackerImpl::InitRoadInfoParam()
{
	RoadInfoParamInit();
/*	if(GetCurrentParam()->g_PlateRcogMode != PRM_TOLLGATE)
	{
		m_pHvParam->GetInt(
			"Tracker\\RoadInfo", "RoadLineNumber",
			&CTrackInfo::m_iRoadNumber, CTrackInfo::m_iRoadNumber,
			2, CTrackInfo::s_iMaxRoadNumber,
			"车道线数量", "", 5
			);		//车道数
		m_pHvParam->GetInt(
			"Tracker\\RoadInfo", "RoadNumberBegin",
			&CTrackInfo::m_iRoadNumberBegin, CTrackInfo::m_iRoadNumberBegin,
			0, 1,
			"车道编号起始方向", ",0:从左开始,1:从右开始", 5
			);
		m_pHvParam->GetInt(
			"Tracker\\RoadInfo", "StartRoadNumber",
			&CTrackInfo::m_iStartRoadNum, CTrackInfo::m_iStartRoadNum,
			0, 1,
			"车道号起始编号", ",0:从0开始,1:从1开始", 5
			);		//车道号起始编号

		// 厦门项目加入参数(行人道)
		if(CTrackInfo::m_iRoadNumber > 2)
		{
			m_pHvParam->GetInt(
				"Tracker\\RoadInfo", "ManRoadNumber",
				&CTrackInfo::m_iManRoadNum, CTrackInfo::m_iManRoadNum,
				0, CTrackInfo::s_iMaxRoadNumber - 1,
				"行人道编号", "", 3
				);
		}
		else
		{
			CTrackInfo::m_iManRoadNum = -2;
		}
		//黄国超增加,2011-03-07
		if(GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
		{
			m_pHvParam->GetInt("Tracker\\RoadInfo", "OutputRoadTypeNameFlag", 
			&CTrackInfo::m_iOutputRoadTypeName,CTrackInfo::m_iOutputRoadTypeName,
			0,1,
			"输出车道类型标识", "0-不输出;1-输出", 5);
		}
		char szSection[256] = {0};
		// 读取车道信息
		for(int i = 0; i < CTrackInfo::s_iMaxRoadNumber; ++i)
		{
			sprintf(szSection, "Tracker\\RoadInfo\\Road%02d", i);

			m_pHvParam->GetInt(
				szSection, "TopX",
				&CTrackInfo::m_roadInfo[i].ptTop.x, CTrackInfo::m_roadInfo[i].ptTop.x,
				0,5000,
				"顶X坐标","",5
				);
			m_pHvParam->GetInt(
				szSection, "TopY",
				&CTrackInfo::m_roadInfo[i].ptTop.y, CTrackInfo::m_roadInfo[i].ptTop.y,
				0,5000,
				"顶Y坐标","",5
				);
			m_pHvParam->GetInt(
				szSection, "BottomX",
				&CTrackInfo::m_roadInfo[i].ptBottom.x, CTrackInfo::m_roadInfo[i].ptBottom.x,
				0,5000,
				"底X坐标","",5
				);
			m_pHvParam->GetInt(
				szSection, "BottomY",
				&CTrackInfo::m_roadInfo[i].ptBottom.y, CTrackInfo::m_roadInfo[i].ptBottom.y,
				0,5000,
				"底Y座标","",5
				);
			m_pHvParam->GetInt(
				szSection, "LineType",
				&CTrackInfo::m_roadInfo[i].iLineType, CTrackInfo::m_roadInfo[i].iLineType,
				1,9999,
				"类型","",5
				);

			m_pHvParam->GetInt(
				szSection, "RoadType",
				&CTrackInfo::m_roadInfo[i].iRoadType, CTrackInfo::m_roadInfo[i].iRoadType,
				1,9999,
				"对应的车道类型(左边线)","",5
				);

			if (m_pParam->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
			{
				m_pHvParam->GetString(
					szSection, "RoadTypeName",
					CTrackInfo::m_roadInfo[i].szRoadTypeName, 255, 
					"车道类型名称", 
					"若为NULL，则取该车道类型的中文名",5
					);
			}
			if (m_pParam->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
			{
				m_pHvParam->GetInt(
					szSection, "FilterRunType",
					&CTrackInfo::m_roadInfo[i].iFilterRunType, CTrackInfo::m_roadInfo[i].iFilterRunType,
					0,9999,
					"过滤类型(1:直行2:左转4:右转8:调头32:逆行)","",5
					);
			}
		}
	}*/
}

//判断是否压线
bool CTrackerImpl::IsOverLine(int iX, int iY)
{
	bool fRet = false;
	return fRet;
}

RUN_TYPE CTrackerImpl::CheckRunTypeEx(HiVideo::CRect rect0, HiVideo::CRect rect1)
{
	CPoint cpBegin, cpEnd;
	cpBegin = rect0.CenterPoint();
	cpEnd = rect1.CenterPoint();

	RUN_TYPE rt = RT_UNSURE;
	int iRoadBegin = MatchRoad(cpBegin.x, cpBegin.y);
	int iRoadEnd =   MatchRoad(cpEnd.x, cpEnd.y);

	int iRoadCount = CTrackInfo::m_iRoadNumber - 1;
	bool fIsOutLeft = false;
	bool fIsOutRight = false;
	bool fIsOutForward = false;
	if( CTrackInfo::m_iRoadNumberBegin == 0 )
	{
		if( LeftOrRight(cpEnd.x, cpEnd.y, 0) == -1 )
		{
			fIsOutLeft = true;
		}
		if( LeftOrRight(cpEnd.x, cpEnd.y, CTrackInfo::m_iRoadNumber - 1) == 1 )
		{
			fIsOutRight = true;
		}
	}
	else
	{
		if( LeftOrRight(cpEnd.x, cpEnd.y, CTrackInfo::m_iRoadNumber - 1) == -1 )
		{
			fIsOutLeft = true;
		}
		if( LeftOrRight(cpEnd.x, cpEnd.y, 0) == 1 )
		{
			fIsOutRight = true;
		}
	}
	
	int iForwardPosY = CTrackInfo::m_iRealForward * CTrackInfo::m_iHeight / 100;
	if( cpEnd.y < iForwardPosY )
	{
		fIsOutForward = true;
	}

	if( fIsOutLeft )
	{
		rt = RT_LEFT;
	}
	else if( fIsOutRight )
	{
		rt = RT_RIGHT;
	}
	else if( fIsOutForward )
	{
		rt = RT_FORWARD;
	}

	int iStopLineY = CTrackInfo::m_iStopLinePos * CTrackInfo::m_iHeight / 100;
	if(cpEnd.y > iStopLineY)
	{
		rt = RT_UNSURE;
	}

	return rt;
}
RUN_TYPE CTrackerImpl::CheckRunType(HiVideo::CRect rect0, HiVideo::CRect rect1)
{
	// zhaopy
	return CheckRunTypeEx(rect0, rect1);
	int iX1, iY1, iX2, iY2;
	iX1 = rect0.CenterPoint().x;
	iY1 = rect0.CenterPoint().y;
	iX2 = rect1.CenterPoint().x;
	iY2 = rect1.CenterPoint().y;

	RUN_TYPE rt = RT_UNSURE;
	int iYScr = (int)((float)iY2 / (float)m_nHeight * 100);
	int iXScr = (int)((float)iX2 / (float)m_nWidth * 100);

	int iRoad1 = MatchRoad(iX1, iY1);
	int iRoad2 = MatchRoad(iX2, iY2);

	if( iYScr < (CTrackInfo::m_iLeftStopLinePos + 5) && iXScr < 10 )
	{
		return RT_LEFT;
	}
	else if( iYScr < (CTrackInfo::m_iStopLinePos + 5) && iXScr > 90 )
	{
		return RT_RIGHT;
	}

	bool fCanDo = (iYScr < 50 
					|| (iYScr <= CTrackInfo::m_iLeftStopLinePos && iXScr <= 15) 
					|| (iYScr <= CTrackInfo::m_iStopLinePos && iXScr >= 85) 
					|| (iYScr <= CTrackInfo::m_iStopLinePos && iRoad1 != iRoad2) );
	if( !fCanDo ) return rt;

	//是不是左转道
	bool fIsLeftRoad2 = 0 != (CTrackInfo::m_roadInfo[iRoad2].iRoadType & RRT_LEFT);
	//是不是右转道
	bool fIsRightRoad2 = 0 != (CTrackInfo::m_roadInfo[iRoad2].iRoadType & RRT_RIGHT);

	int iDL, iDR;
	int iLRL;
	int iLRR;
	if(CTrackInfo::m_iRoadNumberBegin == 0)
	{
		iLRL = LeftOrRight(iX2, iY2, iRoad2, &iDL);
		iLRR = LeftOrRight(iX2, iY2, iRoad2 + 1, &iDR);
	}
	else
	{
		iLRR = LeftOrRight(iX2, iY2, iRoad2, &iDL);
		iLRL = LeftOrRight(iX2, iY2, iRoad2 + 1, &iDR);
	}

	if( (iRoad2 == 0 && CTrackInfo::m_iRoadNumberBegin == 0) || 
		(iRoad2 == (CTrackInfo::m_iRoadNumber - 2) && CTrackInfo::m_iRoadNumberBegin == 1))
	{
		if( iLRL == -1)
		{
			if( fIsLeftRoad2 || (iDL > (rect1.Width() / 2)) || iRoad1 != iRoad2 ) rt = RT_LEFT;
		}
		else if( iRoad1 != iRoad2 && fIsLeftRoad2 && (CTrackInfo::m_iSpecificLeft == 1) && (iDR > (rect1.Width() / 2)) )
		{
			rt = RT_LEFT;
		}
		else if( fIsLeftRoad2 && iRoad1 == iRoad2 && (iDR < (rect1.Width() / 2)) )
		{
			rt = RT_FORWARD;
		}
		else if( !fIsLeftRoad2 )
		{
			rt = RT_FORWARD;
		}
	}
	else if( (iRoad2 == (CTrackInfo::m_iRoadNumber - 2)  && CTrackInfo::m_iRoadNumberBegin == 0 ) || 
		(iRoad2 == 0 && CTrackInfo::m_iRoadNumberBegin == 1))
	{
		if( iLRR == 1 )
		{
			if( (iRoad1 != iRoad2) || fIsRightRoad2 || (iDR > (rect1.Width() / 2)) ) rt = RT_RIGHT;
		}
		else
		{
			rt = RT_FORWARD;
		}
	}
	else 
	{
		rt = RT_FORWARD;
	}

	return rt;
}

//越线
//判断：第一帧和最后一帧是否属于同一车道
CROSS_OVER_LINE_TYPE CTrackerImpl::IsCrossLine(CTrackInfo& TrackInfo)
{
//	CROSS_OVER_LINE_TYPE coltType = COLT_NO;
	if (0 == m_pParam->m_iDetectCrossLineEnable)
	{
		return COLT_INVALID;
	}
	if (CTrackInfo::m_iRoadNumber <= 2)
	{
		return COLT_NO;
	}
	CROSS_OVER_LINE_TYPE coltLines[4] = {COLT_NO, COLT_LINE1, COLT_LINE2, COLT_LINE3};

	int iCenterX0, iCenterX1, iCenterY0, iCenterY1;
	iCenterX0 = (TrackInfo.m_rgPlateInfo[0].rcPos.right + TrackInfo.m_rgPlateInfo[0].rcPos.left) / 2;
	iCenterY0 = (TrackInfo.m_rgPlateInfo[0].rcPos.bottom + TrackInfo.m_rgPlateInfo[0].rcPos.top) / 2;

	//红绿灯模式只需要判断停止线前的车牌
	if (m_pParam->m_fUsedTrafficLight)
	{
		int iPosStop  = CTrackInfo::m_iStopLinePos * m_nHeight / 100;
		iCenterX1 = iCenterX0;
		iCenterY1 = iCenterY0;
		for (int i = 0; i < TrackInfo.m_cPlateInfo; i++)
		{
			if( TrackInfo.m_rgPlateInfo[i].nInfoType != PI_LPR )
			{
				continue;
			}
			int iTmpX = (TrackInfo.m_rgPlateInfo[i].rcPos.right + TrackInfo.m_rgPlateInfo[i].rcPos.left) / 2;
			int iTmpY = (TrackInfo.m_rgPlateInfo[i].rcPos.bottom + TrackInfo.m_rgPlateInfo[i].rcPos.top) / 2;
			if (iTmpY > iPosStop && iTmpY < iCenterY1)
			{
				iCenterX1 = iTmpX;
				iCenterY1 = iTmpY;
			}
		}
	}
	else
	{
		iCenterX1 = (TrackInfo.LastInfo().rcPos.right + TrackInfo.LastInfo().rcPos.left) / 2;
		iCenterY1 = (TrackInfo.LastInfo().rcPos.bottom + TrackInfo.LastInfo().rcPos.top) / 2;
	}

	int iRoad0, iRoad1;
	iRoad0 = MatchRoad(iCenterX0, iCenterY0);
	iRoad1 = MatchRoad(iCenterX1, iCenterY1);

	if (iRoad0 == iRoad1)
	{
		return COLT_NO;
	}
	if (iRoad0 > iRoad1)
	{
		int iTmp = iRoad1;
		iRoad1 = iRoad0;
		iRoad0 = iTmp;
	}

	int index = iRoad1;
	if( index >= 0 && index < 4 && CTrackInfo::m_ActionDetectParam.iIsCrossLine[index] == 1)
	{
		return coltLines[index];
	}

	return COLT_NO;
}

//压线判断
CROSS_OVER_LINE_TYPE CTrackerImpl::IsOverYellowLine(CTrackInfo& TrackInfo)
{
	if (0 == m_pParam->m_iDetectOverYellowLineEnable)
	{
		return COLT_INVALID;
	}

	CROSS_OVER_LINE_TYPE coltLines[5] = {COLT_LINE0, COLT_LINE1, COLT_LINE2, COLT_LINE3, COLT_LINE4};
	
	int iCount[5] = {0, 0, 0, 0, 0};
    
	for (int i = 0; i < CTrackInfo::m_iRoadNumber; i++)
	{
		if ((CTrackInfo::m_ActionDetectParam.iIsYellowLine[i] != 1 &&
			CTrackInfo::m_ActionDetectParam.iIsYellowLine[i] != 2) ||
			CTrackInfo::m_roadInfo[i].ptTop.y - CTrackInfo::m_roadInfo[i].ptBottom.y == 0 )
		{
			continue;
		}
		for (int t = 0; t < TrackInfo.m_cPlateInfo; t++)
		{
			if( TrackInfo.m_rgPlateInfo[t].nInfoType != PI_LPR )
			{
				continue;
			}
			//求车牌宽度的一半
			int iHalfWidth = (TrackInfo.m_rgPlateInfo[t].rcPos.right - TrackInfo.m_rgPlateInfo[t].rcPos.left) / 2;
			// 根据灵敏度修正
			iHalfWidth = (int)(iHalfWidth * m_fltOverLineSensitivity);
			// 根据车型，向两边括相应大小
			switch (TrackInfo.m_nVotedCarType)
			{
			case CT_SMALL:
				break;
			case CT_MID:
				iHalfWidth = (int)(iHalfWidth * 1.2f);
				break;
			case CT_LARGE:
				iHalfWidth = (int)(iHalfWidth * 1.5f);
				break;
			}
			//求中心点
			int iCenterX = (TrackInfo.m_rgPlateInfo[t].rcPos.right + TrackInfo.m_rgPlateInfo[t].rcPos.left) / 2;
			int iCenterY = (TrackInfo.m_rgPlateInfo[t].rcPos.bottom + TrackInfo.m_rgPlateInfo[t].rcPos.top) / 2;
			//红绿灯模式只需要判断停止线前的车牌
			if (m_pParam->m_fUsedTrafficLight && iCenterY < CTrackInfo::m_iStopLinePos * m_nHeight / 100)
			{
				continue;
			}
			//求黄线对应Y上的X
			float fTop = 1.0f*(iCenterY - CTrackInfo::m_roadInfo[i].ptTop.y) * 
				(CTrackInfo::m_roadInfo[i].ptTop.x - CTrackInfo::m_roadInfo[i].ptBottom.x);
			float fBottom = 1.0f*(CTrackInfo::m_roadInfo[i].ptTop.y - CTrackInfo::m_roadInfo[i].ptBottom.y);
			int iXCenter =  (int)(fTop / fBottom) + CTrackInfo::m_roadInfo[i].ptTop.x;
			if (abs(iCenterX - iXCenter) < iHalfWidth)
			{
				iCount[i]++;
			}
		}
	}
	
	int iMax = 0;
	int iIndex(0);
	for (int i = 0; i < 5; i++)
	{
		if (iCount[i] > iMax)
		{
			iMax = iCount[i];
			iIndex = i;
		}
	}
	if (0 >= iMax)
	{
		return COLT_NO;
	}
	else
	{
		return coltLines[iIndex];
	}
}

bool CTrackerImpl::IsCanFoundTemple(CTrackInfo *pTrack)
{
	bool fFoundTemple(false);				// 是否建立模板
	if (m_pParam->m_fUsedTrafficLight)
	{
		// 红绿灯只对车牌位置在屏幕高度73% 之前的建立模板
		if (pTrack->m_fTempTrackExists)
		{
			fFoundTemple = true;			// 模板跟踪成功，则继续建立模板
		}
		else if (pTrack->m_fObservationExists)
		{
			// 车牌跟踪，必须判断位置在屏幕高度72% 以上才建立模板(目的减少多检和提高效率)
			int nPerY = (pTrack->LastInfo().rcPos.top * 100) / m_nHeight;
			fFoundTemple = (nPerY < CTrackInfo::m_nTrafficLightTempleYPos);		// 红绿灯条件下72%以后不会建立模板匹配
		}
	}
	else
	{
		// 非红绿灯模式，一直都建立模板
		if ((pTrack->m_fObservationExists) || (pTrack->m_fTempTrackExists))
		{
			fFoundTemple = true;
		}
	}

	return fFoundTemple;
}

// 车辆停止
const int g_nStopCarYOffset = 2;					// 车辆停止判断依据(Y方向的像素偏差)
const int g_nFrameCount = 4;
bool CTrackerImpl::IsCarStop(CTrackInfo *pTrack)
{
	bool fStop = true;

	if (pTrack->m_cPlateInfo < g_nFrameCount)
	{
		pTrack->m_iCarStopFrame =0;
		return false;				// 少于g_nFrameCount帧不判断
	}

	// 计算3帧的平均中心Y 坐标
	int nMeanY(0);
	int rgnY[g_nFrameCount];
	//added by liujie 增加判断X方向条件
	int nMeanX(0);
	int rgnX[g_nFrameCount];
	for (int i = 0; i < g_nFrameCount; i++)
	{
		rgnY[i] = (pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameCount + i].rcPos.top
			+ pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameCount + i].rcPos.bottom) / 2;
		nMeanY += rgnY[i];
	}
	for (int i = 0; i < g_nFrameCount; i++)
	{
		rgnX[i] = (pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameCount + i].rcPos.left
			+ pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameCount + i].rcPos.right) / 2;
		nMeanX += rgnX[i];
	}
	nMeanY /= g_nFrameCount;
	nMeanX /= g_nFrameCount;

	// 进行停止判断
	for (int i = 0; i < g_nFrameCount; i++)
	{
		if (abs(rgnY[i] - nMeanY) > g_nStopCarYOffset)
		{
			// 偏差超过阈值,则车辆移动
			pTrack->m_iCarStopFrame =0;
			fStop = false;
			break;
		}
		//added by liujie 增加X方向上的判断
		if (abs(rgnX[i] - nMeanX) > g_nStopCarYOffset)
		{
			// 偏差超过阈值,则车辆移动
			pTrack->m_iCarStopFrame =0;
			fStop = false;
			break;
		}

	}

	if(pTrack->LastInfo().rcPos.CenterPoint().y ==  pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - 2].rcPos.CenterPoint().y
		&& pTrack->LastInfo().rcPos.CenterPoint().x ==  pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - 2].rcPos.CenterPoint().x)
	{
		pTrack->m_iCarStopFrame++;
	}

	// zhaopy 
	int iFrameCount = 12;
	if( m_LightType == NIGHT )
	{
		iFrameCount = 6;
	}
	if(pTrack->m_iCarStopFrame > iFrameCount)
	{
		fStop = true;
	}
	else {
		fStop = false;
	}

	return fStop;
}

//更新最大偏移量
static const  int X_OFFSET = 150;
static const  int Y_OFFSET = 250;
static const int g_nFrameOffsetCount = 3;

void CTrackerImpl::UpdateOffset(CTrackInfo *pTrack)
{
	if( pTrack == NULL || pTrack->m_cPlateInfo < g_nFrameOffsetCount ) return;
	// 计算g_nFrameCount帧的平均中心Y 坐标和X坐标
	int nMeanY(0);
	int nMeanX(0);
	int rgnY[g_nFrameOffsetCount];
	int rgnX[g_nFrameOffsetCount];
	for (int i = 0; i < g_nFrameOffsetCount; i++)
	{
		rgnY[i] = abs(pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameOffsetCount + i].rcPos.top
			- pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameOffsetCount + i].rcPos.bottom);
		nMeanY += rgnY[i];
		rgnX[i] = abs(pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameOffsetCount + i].rcPos.right
			- pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo - g_nFrameOffsetCount + i].rcPos.left);
		nMeanX += rgnX[i];
	}
	nMeanY /= g_nFrameOffsetCount;
	nMeanX /= g_nFrameOffsetCount;

	if( nMeanY > 5 )
	{
		pTrack->m_nMaxYPos = nMeanY * Y_OFFSET / 100;
	}
	if( nMeanX > 3 )
	{
		pTrack->m_nMaxXPos = nMeanX * X_OFFSET / 100;
	}
}

HRESULT CTrackerImpl::SetFirstLightType(LIGHT_TYPE nLightType)
{
	m_nFirstLightType = nLightType;
	m_dwLastCarLeftTime = GetSystemTick();
	return S_OK;
}

HRESULT CTrackerImpl::SetFirstPulseLevel(int nPulseLevel)
{
	m_iPulseLevel = nPulseLevel;
	return S_OK;
}

HRESULT CTrackerImpl::SetFirstCplStatus(int nCplStatus)
{
	m_iCplStatus = nCplStatus;
	return S_OK;
}


//是否按车道行驶
bool CTrackerImpl::CheckRoadAndRun(RUN_TYPE runtype, int runroadtype)
{
	bool fRet = false;
	if( runtype == RT_UNSURE )
	{
		fRet = true;
	}
	else if( runtype == RT_FORWARD && (runroadtype &  RRT_FORWARD) )
	{
		fRet = true;
	}
	else if( runtype == RT_LEFT && (runroadtype & RRT_LEFT) )
	{
		fRet = true;
	}
	else if( runtype == RT_RIGHT && (runroadtype & RRT_RIGHT) )
	{
		fRet = true;
	}
	else if( runtype == RT_TURN && (runroadtype & RRT_TURN) )
	{
		fRet = true;
	}
	else if(runtype == RT_CONVERSE)
	{
		fRet = true;
	}
	return fRet;
}

//是否闯红灯
bool CTrackerImpl::CheckRush(int iPreLight, int iAcrossLight, int runroadtype, int runtype, DWORD32 dwCaptureTime)
{
	if( iPreLight < 0 || iAcrossLight < 0 || iPreLight >= MAX_SCENE_COUNT || iAcrossLight >= MAX_SCENE_COUNT )
	{
		return false;
	}

	bool fRet = true;
	TRAFFICLIGHT_SCENE tsPre = {TLS_UNSURE, TLS_UNSURE, TLS_UNSURE, TLS_UNSURE};
	TRAFFICLIGHT_SCENE tsAcross = {TLS_UNSURE, TLS_UNSURE, TLS_UNSURE, TLS_UNSURE};
	//GetLightScene(iPreLight, &tsPre);
	//GetLightScene(iAcrossLight, &tsAcross);
	m_cTrafficLight.GetLightScene(iPreLight, &tsPre);
	m_cTrafficLight.GetLightScene(iAcrossLight, &tsAcross);
	//修正绿灯时间
	m_cTrafficLight.GetGreenLightStartTime(&m_greentick);
	if( m_greentick.dwLeft != 0 && dwCaptureTime > m_greentick.dwLeft && tsAcross.lsLeft != TLS_GREEN )
	{
//		HV_Trace("changes left light state to green\n");
		tsAcross.lsLeft = TLS_GREEN;
	}
	if( m_greentick.dwForward != 0 && dwCaptureTime > m_greentick.dwForward && tsAcross.lsForward != TLS_GREEN )
	{
//		HV_Trace("changes forward light state to green\n");
		tsAcross.lsForward = TLS_GREEN;
	}
	if( m_greentick.dwRight != 0 && dwCaptureTime > m_greentick.dwRight && tsAcross.lsRight != TLS_GREEN )
	{
//		HV_Trace("changes right light state to green\n");
		tsAcross.lsRight = TLS_GREEN;
	}
	if( m_greentick.dwTurn != 0 && dwCaptureTime > m_greentick.dwTurn && tsAcross.lsTurn != TLS_GREEN ) 
	{
//		HV_Trace("changes turn light state to green\n");
		tsAcross.lsTurn = TLS_GREEN;
	}

	bool fUnLeft = ( (tsPre.lsLeft == TLS_RED || tsPre.lsLeft == TLS_YELLOW) && (tsAcross.lsLeft == TLS_RED || tsAcross.lsLeft == TLS_YELLOW) );
	bool fUnRight = ( (tsPre.lsRight == TLS_RED || tsPre.lsRight == TLS_YELLOW) && (tsAcross.lsRight == TLS_RED || tsAcross.lsRight == TLS_YELLOW) );
	bool fUnForward = ( (tsPre.lsForward == TLS_RED || tsPre.lsForward == TLS_YELLOW) && (tsAcross.lsForward == TLS_RED || tsAcross.lsForward == TLS_YELLOW) );
	bool fUnTurn = ( (tsPre.lsTurn == TLS_RED || tsPre.lsTurn == TLS_YELLOW) && (tsAcross.lsTurn == TLS_RED || tsAcross.lsTurn == TLS_YELLOW) );

	//只要它所行驶的车道类型对应的灯状态不是禁行的，那就不谴澈斓啤?
	//反之则是。
	fRet = true;
	if( ((runroadtype & RRT_FORWARD) && !fUnForward)
		|| ((runroadtype & RRT_LEFT) && !fUnLeft) 
		|| ((runroadtype & RRT_RIGHT) && !fUnRight)
		|| ((runroadtype & RRT_TURN) && !fUnTurn)
		)
	{
		fRet = false;
	}
	
	if( !fRet &&  runtype != RT_UNSURE )
	{
		//如果行驶类型已确定
		//那只要对应的灯状态是禁行的，都是闯红灯。反之则不是。
		if( (runtype == RT_FORWARD && fUnForward && (runroadtype & RRT_FORWARD))
			|| (runtype == RT_LEFT && fUnLeft && (runroadtype & RRT_LEFT))
			|| (runtype == RT_RIGHT && fUnRight && (runroadtype & RRT_RIGHT))
			|| (runtype == RT_TURN && fUnTurn&& (runroadtype & RRT_TURN)) )
		{
			fRet = true;
		}
	}

	return fRet;
}

HRESULT CTrackerImpl::CheckPeccancyType(
						  int iPreLight,			//压线前的红绿灯场景
						  int iAcrossLight,		//过线后的红绿灯场景
						  RUN_TYPE runtype,	//行驶类型
						  int runroadtype,      //行驶车道类型
						  DWORD32 dwCaptureTime,	//抓拍时间
						  /*PECCANCY_TYPE*/int * pPeccancyType //违章类型
						  )
{
	if(pPeccancyType == NULL)
	{
		return E_POINTER;
	}
	*pPeccancyType = PT_NORMAL;
	//黄国超修改违章类型，有多种违章类型的均组合起来，2010-10-12
	if( (runtype == RT_CONVERSE) )
	{
		*pPeccancyType |= PT_CONVERSE; //逆行
	}
	/*else*/ if( (runroadtype & RRT_ESTOP) )
	{
		*pPeccancyType |= PT_ESTOP; //非机动车道
	}
	/*else*/ if(!(*pPeccancyType & RT_CONVERSE) && !(*pPeccancyType & PT_ESTOP) && CheckRush(iPreLight, iAcrossLight, runroadtype, runtype, dwCaptureTime) )
	{
		*pPeccancyType |= PT_RUSH; //闯红灯
	}
	/*else*/ if(!CheckRoadAndRun(runtype, runroadtype))
	{
		*pPeccancyType |= PT_ACROSS; //不按车道行驶
	}

	return S_OK;
}

HRESULT CTrackerImpl::CheckCarArrive(PROCESS_ONE_FRAME_RESPOND* pProcessRespond, int iDiffTime)
{

    //bool fTollMode = (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE);
	if (/*!fTollMode &&*/ (m_fEnableScaleSpeed == FALSE || m_fEnableCarArriveTrigger == FALSE) ) return S_FALSE;

	HRESULT hr = S_OK;
	DWORD32 dwCurTick = GetSystemTick();
	DWORD32 dwCurRealTick = dwCurTick;
	if( iDiffTime < 0 ) dwCurTick += (-iDiffTime);
	if( iDiffTime >= 0 ) dwCurTick -= iDiffTime;
	CTrackInfo* pTrack = NULL;
	//bool fNeedCarArrive = false;

    int nCarArrivePos = m_iCarArrivedPos * m_nHeight / 100;
	for(int i = 0; i < m_cTrackInfo; ++i)
	{
		bool fNeedCarArrive = false;
		pTrack = &m_rgTrackInfo[i];
		if( pTrack->m_fCarArrived ) continue;

        /*if (fTollMode)
        {
            if (pTrack->m_cPlateInfo > 1 && (
            		m_pParam->g_MovingDirection == MD_TOP2BOTTOM && pTrack->LastInfo().rcPos.bottom >= nCarArrivePos
                    && pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo-2].rcPos.bottom <= nCarArrivePos
                    ||
            		m_pParam->g_MovingDirection == MD_BOTTOM2TOP && pTrack->LastInfo().rcPos.bottom <= nCarArrivePos
                    && pTrack->m_rgPlateInfo[pTrack->m_cPlateInfo-2].rcPos.bottom >= nCarArrivePos)
                    )
            {
                pTrack->m_fCarArrived = true;
            }
            continue;
        }*/
		
		DWORD32 dwImageTime = pTrack->LastInfo().dwFrameTime;
		int iTriggerPos = pTrack->LastInfo().color == PC_YELLOW ? m_iCarArrivedPosYellow : m_iCarArrivedPos;
		CRect rcPos = pTrack->LastInfo().rcPos;
		int iPosY = iTriggerPos * m_nHeight / 100;
		int iCurPos = (rcPos.CenterPoint().y * 100 / m_nHeight);
		if ( (m_pParam->g_MovingDirection == MD_TOP2BOTTOM && rcPos.CenterPoint().y > iPosY)
			|| (m_pParam->g_MovingDirection == MD_BOTTOM2TOP && rcPos.CenterPoint().y < iPosY) )
		{
			fNeedCarArrive = true;
		}  
		else if (m_cfgParam.iPlatform == 1 && iCurPos > 40 && pTrack->m_cPlateInfo < 4)
		{
			//fNeedCarArrive = true;
		}
		else if( (pTrack->m_dwCarArriveDopeTime == 0 || pTrack->m_dwCarArriveDopeTime > dwCurTick) //预测时间为0或者大于当前时间
				&& ( pTrack->m_cPlateInfo >= 4 || (iCurPos > 25 && pTrack->m_cPlateInfo >= 2) )   //至少有两帧
				&& !pTrack->m_fPredictCarArrive //当前帧没有预测过
				)
		{

			pTrack->m_fPredictCarArrive = true;
			//预测CarArrive的时间.
			float fltDistance = m_pScaleSpeed->CalcActureDistance(rcPos.CenterPoint(), CPoint(rcPos.CenterPoint().x, iPosY));
			//计算车辆到达触发延迟时间
			float fltCarSpeed(0.0f);
			float fltScaleOfDistance(1.0f);
			CalcCarSpeed(fltCarSpeed, fltScaleOfDistance, (*pTrack));

			float fltSpeedTemp = fltCarSpeed;
			if( fltCarSpeed > 10.0f && fltDistance > 0.0f )
			{
				int iRelay = 80;
				if (m_cfgParam.iPlatform == 1)
				{
					iRelay = 120;
					if( m_iAvgY < 25 )
					{
						iRelay = 80;
					}
					if( fltCarSpeed < 20.0f )
					{
						iRelay = 80;
					}
					iRelay += 40;
				}
				fltCarSpeed /= 3600;
				pTrack->m_dwCarArriveDopeTime = (DWORD32) (fltDistance / fltCarSpeed) + dwImageTime;
				if( pTrack->m_dwCarArriveDopeTime > (DWORD32)iRelay )
				{
					pTrack->m_dwCarArriveDopeTime -= iRelay;
				}
				//HV_Trace("LastImageTime:%d, DopeTime:%d, CurTime:%d, speed: %0.2f pointY:%d, fltDistance:%0.2f\n", 
					//dwImageTime, pTrack->m_dwCarArriveDopeTime, dwCurTick, fltTemp, rcPos.CenterPoint().y, fltDistance);
			}
			if( pTrack->m_dwDopeFirstPos == 0 )
			{
				pTrack->m_dwDopeFirstPos = pTrack->m_dwCarArriveDopeTime;
				pTrack->m_dwDopeEndPos = iCurPos;
			}
			else
			{
				pTrack->m_dwDopeEndPos = pTrack->m_dwCarArriveDopeTime;
			}
		}

		if( fNeedCarArrive 
			|| (pTrack->m_dwCarArriveDopeTime != 0 
				&& pTrack->m_dwCarArriveDopeTime < dwCurTick ) )
		{
			float fltDistance = 0.0f;
			fltDistance = m_pScaleSpeed->CalcActureDistance(rcPos.CenterPoint(), CPoint(rcPos.CenterPoint().x, iPosY));
			if( fNeedCarArrive )
			{
				fltDistance = m_iCarArrivedDelay - fltDistance;
			}
			else
			{
				fltDistance = m_iCarArrivedDelay + fltDistance;
				//HV_Trace("LastImageTime:%d, DopeTime:%d, CurTime:%d\n", dwImageTime, pTrack->m_dwCarArriveDopeTime, dwCurTick);
			}
			hr = ProcessCarArrive(pTrack, fNeedCarArrive ? dwImageTime : dwCurTick, fltDistance, pProcessRespond);
			//hr = ProcessCarArrive(pTrack, dwImageTime, fltDistance, pProcessRespond);
		}
		else
		{
			pTrack->m_dwLastProcessCarArrvieTime = pTrack->m_dwCarArriveDopeTime;
		}
	}
	return hr;
}

HRESULT CTrackerImpl::ProcessNoPlateCarArrive(
				CObjTrackInfo* pObjTrack, 
				DWORD32 dwImageTime, 
				float fltDistance, 
				PROCESS_ONE_FRAME_RESPOND* pProcessRespond
				)
{
	HRESULT hr = S_OK;
	if( pObjTrack == NULL )
	{
		 return E_POINTER;
	}

	CARARRIVE_INFO_STRUCT carArriveInfo;
	CRect rcPos = pObjTrack->LastInfo().m_rcFG;
	pObjTrack->m_iRoad = MatchRoad(rcPos.CenterPoint().x, rcPos.CenterPoint().y);
	carArriveInfo.iRoadNumber = pObjTrack->m_iRoad;
	carArriveInfo.iPlateLightType = m_PlateLightType;

	carArriveInfo.dwTriggerOutDelay = 0;
	carArriveInfo.dwCarArriveTime = dwImageTime;
	carArriveInfo.dwCarArriveRealTime = GetSystemTick();

	CARARRIVE_INFO_STRUCT* pCarArriveInfo = &pProcessRespond->cTrigEvent.rgCarArriveInfo[pProcessRespond->cTrigEvent.iCarArriveCount++];
	memcpy(pCarArriveInfo, &carArriveInfo, sizeof(CARARRIVE_INFO_STRUCT));
	pProcessRespond->cTrigEvent.dwEventId |= EVENT_CARARRIVE;

	DWORD32 dwCurTick = GetSystemTick();
	pObjTrack->m_fCarArrived = true;
	pObjTrack->m_nCarArriveTime = dwImageTime;
	pObjTrack->m_nCarArriveRealTime = dwCurTick;
	pObjTrack->m_rcCarArrivePos = rcPos;

	// zhaopy 触发抓拍
	CapObjTrack(pObjTrack);

	return hr;
}

HRESULT CTrackerImpl::ProcessCarArrive(CTrackInfo* pTrack, DWORD32 dwImageTime, float fltDistance, PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	HRESULT hr = S_OK;
	// 通知车辆到达
	CARARRIVE_INFO_STRUCT carArriveInfo;
	pTrack->m_iRoad = MatchRoad(pTrack->LastInfo().rcPos.CenterPoint().x, pTrack->LastInfo().rcPos.CenterPoint().y);

	DWORD32 dwCurTick = GetSystemTick();

	// zhaopy
	carArriveInfo.iRoadNumber = pTrack->m_iRoad;
	carArriveInfo.iPlateLightType = m_PlateLightType;
	// zhaopy
	carArriveInfo.dwTriggerOutDelay = pTrack->m_dwCarArriveDopeTime;
	carArriveInfo.dwCarArriveTime = dwImageTime;
	carArriveInfo.dwCarArriveRealTime = dwCurTick;

	carArriveInfo.dwFirstPos = pTrack->m_dwDopeFirstPos;
	carArriveInfo.dwEndPos = pTrack->m_dwDopeEndPos;

	CARARRIVE_INFO_STRUCT* pCarArriveInfo = &pProcessRespond->cTrigEvent.rgCarArriveInfo[pProcessRespond->cTrigEvent.iCarArriveCount++];
	memcpy(pCarArriveInfo, &carArriveInfo, sizeof(CARARRIVE_INFO_STRUCT));
	
	pProcessRespond->cTrigEvent.dwEventId |= EVENT_CARARRIVE;

	pTrack->m_fCarArrived = true;
	if( !pTrack->m_fIsTrigger && m_fEnableCarArriveTrigger )
	{
		pTrack->m_nCarArriveTime = dwImageTime;
		pTrack->m_nCarArriveRealTime = dwCurTick;
	}
	else if( !m_fEnableCarArriveTrigger )
	{
		pTrack->m_nCarArriveTime = dwImageTime;
		pTrack->m_nCarArriveRealTime = dwCurTick;
	}
	pTrack->m_rcCarArrivePos = pTrack->LastInfo().rcPos;

	// zhaopy 触发抓拍
	CapTrack(pTrack);

	return hr;
}

HRESULT CTrackerImpl::CheckEvent()
{
	return CheckCarArriveTrigger();
}

HRESULT CTrackerImpl::CheckCarArriveTrigger()
{
	HRESULT hr = S_OK;
	if(m_fEnableCarArriveTrigger == FALSE)
	{
		return hr;
	}

	CTrackInfo* pTrack = NULL;
	DWORD32 dwCurTick = GetSystemTick();
	for(int i = 0; i < m_cTrackInfo; ++i)
	{
		pTrack = &m_rgTrackInfo[i];
		if( pTrack->m_fCarArrived || pTrack->m_fIsTrigger || pTrack->m_dwCarArriveDopeTime == 0) continue;

		if( pTrack->m_dwCarArriveDopeTime < dwCurTick)
		{
			// zhaopy 触发抓拍
			pTrack->m_nCarArriveTime = dwCurTick;
			pTrack->m_nCarArriveRealTime = dwCurTick - 1 ;
			CapTrack(pTrack);
		}
	}
	return hr;
}

//检查最后三帧是否有两帧以上跟踪框的底部是超过跟踪区域的一半的位置。
//只限于车尾模式。
// weikt 2011-4-1
HRESULT CTrackerImpl::CheckNoPlate(CObjTrackInfo* pObj)
{
	HRESULT fRet = E_FAIL;
	if( pObj == NULL ) return fRet;

	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcVideoDetArea;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDetArea, rcMax, m_rcVideoDetArea));


	
	int iFrameCount = 0;
	int iBottomPos = 0;
	int iVoteDistance = rcVideoDetArea.Height() * 30 / 100;
	for(int i = pObj->m_iObjInfoCount - 1; i >= pObj->m_iObjInfoCount - 4; --i)
	{
		iBottomPos = abs( pObj->m_rgObjInfo[i].m_rcFG.bottom - rcVideoDetArea.bottom );
		if( iBottomPos > iVoteDistance )
		{
			iFrameCount++;
		}
	}

	fRet = (iFrameCount >= 3 ? S_OK : E_FAIL);

	if(SUCCEEDED(fRet) && SUCCEEDED(RemoveMoreReview(pObj)))
	{
		fRet = E_FAIL;
	}

	return fRet;
}
//过滤大车在停止线前停下的多检  weikt 2011-4-1
HRESULT CTrackerImpl::RemoveMoreReview(CObjTrackInfo* pObj)
{
	int iPre = 0;
	//int iPreH = 0;
	int ilessY = 0;
	int iMoreY = 0;

	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcVideoDetArea;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDetArea, rcMax, m_rcVideoDetArea));
	int iVoteDistance = rcVideoDetArea.Height() * 60 / 100;

	CRect rcFirstFG = pObj->m_rgObjInfo[0].m_rcFG;
	CRect rcLastFG = pObj->LastInfo().m_rcFG;
	int iAvg = abs(rcLastFG.bottom - rcFirstFG.bottom) / pObj->m_iObjInfoCount;
	HRESULT fMove = E_FAIL;
	int iDiffMax = 0;
	int iTtoB = 0;
	for(int k = 0; k < pObj->m_iObjInfoCount - 1; k++)
	{
		int iTmp = pObj->m_rgObjInfo[k].m_rcFG.right - pObj->m_rgObjInfo[k].m_rcFG.left;
	
		if(k == 0)
		{
			iPre = iTmp;
		}
		if( ( iPre / iTmp > 3 ) 
			|| (iTmp / iPre > 3 )
			)
		{
			//前后两帧的宽度差大于指定值,并且高度差小于指定值时,过滤掉
			iDiffMax++;
			if(iDiffMax > 1)
			{
				fMove = S_OK;			
				//HV_Trace("Move(w1=%d,w2=%d)... \n",iPre,iTmp);
			}
		}
		iPre = iTmp;

		if(k > 0)
		{
			int iCurCenter = (pObj->m_rgObjInfo[k].m_rcFG.bottom + pObj->m_rgObjInfo[k].m_rcFG.top) / 2;
			int iPreCenter = (pObj->m_rgObjInfo[k - 1].m_rcFG.bottom + pObj->m_rgObjInfo[k - 1].m_rcFG.top) / 2;
			if(iPreCenter / iCurCenter > 3)
			{
				//如果前后两帧中心点坐标有大的跳变则过滤掉
				fMove = S_OK;
				//HV_Trace("Move(CurCenter=%d,PreCenter=%d)... \n",iCurCenter,iPreCenter);
			}
			if(iCurCenter >= iPreCenter)
			{
				iTtoB++;
			}
		}

		int Diff = abs(pObj->m_rgObjInfo[k].m_rcFG.bottom - pObj->m_rgObjInfo[k + 1].m_rcFG.bottom);
		if(Diff > iAvg)
		{
			iMoreY++;
		}
		else
		{
			ilessY++;
		}
	}
	if(iTtoB > 6)
	{
		fMove = S_OK;
		//HV_Trace("Move %d ...\n", iTtoB);
	}
	if(ilessY > 0 && iMoreY > 0)
	{
		if(ilessY / iMoreY > (pObj->m_iObjInfoCount - 1) * 0.9)
		{
			//如果大于平均底坐标的值小于小于平均底坐标值的很大倍数则过滤掉

			fMove = S_OK;
			//HV_Trace("Move(less=%d,more=%d)...\n", ilessY, iMoreY);
		}
	}
	return fMove;
}

void DM6467_DelayUS( int iDelayUS )
{
	volatile int iTemp1, iTemp2, i;


	for ( i = 0; i < iDelayUS; i ++ )
	{
		for ( iTemp1 = 0; iTemp1 < 10; iTemp1 ++ )
		{
			for ( iTemp2 = 0; iTemp2 < 1000; iTemp2 ++ )
			{
			}
		}
	}
}


extern "C"
{

#ifndef CHIP_8127
	void PRECISE_DelayUS(DWORD32 dwCount);
#else
	void PRECISE_DelayUS(DWORD32 dwCount)
	{
		return;
	}
#endif
}



//================--------- zhaopy 触发相机抓拍的代码 --------------===============================

#define PWM0_BAR    0x01C22000
#define PWM0_START_REG  (PWM0_BAR + 0xC )

void CTrackerImpl::TriggerCamera(const int iRoadNum)
{
	if (m_cfgParam.iPlatform  == 1)//单板
	{
	    volatile unsigned int *pwn_reg;
	    
	    pwn_reg = (volatile unsigned int *)(PWM0_START_REG); // val set to 1
	    (*pwn_reg) = 1;

	    return;
	}
	else if(m_cfgParam.iPlatform  == 2) //水星平台
	{
		// 4个字节 高1个字节做为车道号输出（起始为0），低三个字节做为抓拍次数输出。
		unsigned int dwFlag = 0;
		dwFlag = iRoadNum;
		dwFlag <<= 24;
		dwFlag |= (m_dwTriggerCameraTimes & 0x00FFFFFF);
		volatile unsigned int *pwm1 = (volatile unsigned int *)0x01C22414;
		*pwm1 = dwFlag;

		volatile unsigned int *pwm0 = (volatile unsigned int *)0x01C2200C;
		*pwm0 = 1;
		return;

	}

	const DWORD32 GPIO_REG_BASE = 0x01c67000;
	const DWORD32 GPIO_REG_BANK_01_BASE = GPIO_REG_BASE + 0x10;	 // GPIO[0:31]
	const DWORD32 GPIO_REG_BANK_23_BASE = GPIO_REG_BASE + 0x38;	// GPIO[32:47]

	volatile int gpio;
	unsigned int addr;
	volatile unsigned int *gpio_dir, *gpio_set, *gpio_clr;
	unsigned int mask;

	if (m_cfgParam.iPlatform == 0)
	{
#ifdef __OLD__
		gpio = 26; // 旧板, 通过GP26信号的下降沿触发抓拍
#else
		gpio = 36; // 新板, 通过GP36信号的下降沿触发抓拍
#endif
	}
	else
	{
		gpio = 41; // 单板
		// TODO: 此处GPIO操作会与ARM端CPLD操作冲突
		// 造成蜂鸣器长响，因此暂时屏蔽  huanggr 2012-04-16
		return;
	}

	if (gpio < 32) 
	{
		addr = GPIO_REG_BANK_01_BASE;
		mask = 1 << gpio;
	} 
	else 
	{
		addr = GPIO_REG_BANK_23_BASE;
		mask = 1 << (gpio - 32);
	}

	if (m_cfgParam.iPlatform == 0)
	{
		// 一体机触发方式
		gpio_clr = (volatile unsigned int *)(addr + 0x0c); // val set to 0
		(*gpio_clr) |= mask;


		int iSleepUs = 1800;
		if (iRoadNum >= 0)
		{
			int iRealRoadNum = iRoadNum;
			if (iRealRoadNum == 0)//第一车道
			{
				iSleepUs = 200;
			}
			else if (iRealRoadNum == 1)//第二车道
			{
				iSleepUs = 700;
			}
			else if (iRealRoadNum == 2)//第三车道
			{
				iSleepUs = 1300;
			}
		}

		PRECISE_DelayUS(iSleepUs);

		gpio_set = (volatile unsigned int *)(addr + 0x08); // val set to 1
		(*gpio_set) |= mask;
	}
	else
	{
		// 单板触发方式
		// 通过DSP端给ARM端一个中断，
		// 然后调用ARM端的回调函数进行触发抓拍相机
		gpio_set = (volatile unsigned int *)(addr + 0x08); // val set to 1
		(*gpio_set) |= mask;

		DM6467_DelayUS(1);

		gpio_clr = (volatile unsigned int *)(addr + 0x0c); // val set to 0
		(*gpio_clr) |= mask;
	}

	// dir set to out
	gpio_dir = (volatile unsigned int *)addr;
	(*gpio_dir) &= (~mask);
}

CRect CTrackerImpl::GetEPNoPlatePredictRect(UINT nNowTime, CObjTrackInfo* pObjTrack)
{
    CRect rect(0,0,0,0);
    if(!pObjTrack)
    {
        return CRect(0,0,0,0);
    }
    if(pObjTrack->m_objTrackState != OBJ_TRACK_TEM)
    {
        return CRect(0,0,0,0);
    }
    if(pObjTrack->m_iObjInfoCount < 3)
    {
        return CRect(0,0,0,0);
    }
    
    if(pObjTrack->m_fFirstPredict)
    {
        //第一次预测
        float fltCarSpeed(0.0f);
        CRect r1 = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG;
        CRect r2 = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 2].m_rcFG;
        //因为r1是模板
        //修正r2到一个与r1同样宽叩墓兰?
        int iSH = r2.Height();
        CPoint center = r2.CenterPoint();
        int iBottom = r2.bottom - (iSH>>1);
        int iTop = iBottom - r1.Height();
        int iLeft = center.x - (r1.Width()>>1);
        int iRight = center.x + (r1.Width()>>1);
        //
        r2.bottom = iBottom;
        r2.top = iTop;
        r2.left = iLeft;
        r2.right = iRight;
        //
        
        float fltInterval1 = (float)(pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_dwFrameTime
            - pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 2].m_dwFrameTime);
        float fltScale(0.0f);
        fltCarSpeed = m_pScaleSpeed->CalcCarSpeedNewMethod(
            r2,
            r1,
            (DWORD32)fltInterval1,
            true,
            PLATE_NORMAL,
            fltScale
            );
        //HV_Trace("\nfltCarSpeed:[%f]", fltCarSpeed);
        float fltInterval = (float)(nNowTime - pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_dwFrameTime);
        float fltDistanceNew = (fltInterval / 1000) * (fltCarSpeed * 1000 / 3600) * 3;
        //HV_Trace("\nfltDistanceNew:[%f]", fltDistanceNew);
        //if(fltDistanceNew >= 600)fltDistanceNew = 0.0f;
        if( fltDistanceNew < 0 ) fltDistanceNew = -fltDistanceNew;
        HV_POINT ptDest;
        HV_POINT ptOrg;
        ptOrg.x = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x;
        ptOrg.y = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().y;

        if (m_pScaleSpeed->GetActurePoint(ptOrg, ptDest, fltDistanceNew))
        {
            rect.top = ptDest.y - (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Height());
            rect.left = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x - (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width() * 15 / 10);
            //rect.left = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x - (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width());
            rect.bottom = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.bottom + pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Height() / 2;
            rect.right = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x + (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width() * 15 / 10);
            //rect.right = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x + (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width());
        } else {
        //不知为何获取不到
        }
        
           
            
        pObjTrack->m_fFirstPredict = false;
        
    } else 
    {
        float fltCarSpeed(0.0f);
        float fltInterval1 = (float)(pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_dwFrameTime
            - pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 2].m_dwFrameTime);

        float fltScale(0.0f);
        fltCarSpeed = m_pScaleSpeed->CalcCarSpeedNewMethod(
            pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 2].m_rcFG,
            pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG,
            (DWORD32)fltInterval1,
            true,
            PLATE_NORMAL,
            fltScale
            );

        float fltInterval = (float)(nNowTime - pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_dwFrameTime);
        float fltDistanceNew = (fltInterval / 1000) * (fltCarSpeed * 1000 / 3600) * 3;
        if( fltDistanceNew < 0 ) fltDistanceNew = -fltDistanceNew;
        HV_POINT ptDest;
        HV_POINT ptOrg;
        ptOrg.x = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x;
        ptOrg.y = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().y;


        if (m_pScaleSpeed->GetActurePoint(ptOrg, ptDest, fltDistanceNew))
        {
            rect.top = ptDest.y - (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Height());
            //rect.left = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x - (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width() * 15 / 10);
            rect.left = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x - (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width());
            rect.bottom = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.bottom + pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Height() / 2;
            //rect.right = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x + (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width() * 15 / 10);
            rect.right = pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.CenterPoint().x + (pObjTrack->m_rgObjInfo[pObjTrack->m_iObjInfoCount - 1].m_rcFG.Width());
        }
    }
    return rect;

}
HRESULT CTrackerImpl::CheckEPNoPlateConfidence(HV_COMPONENT_IMAGE* pImgCalibratedFrame, int iVideID)
{
    //for test first
    /////////////////////////////////////////////////////////////
    //加入代码进行试验
    HV_COMPONENT_IMAGE imgCarRegion;
    imgCarRegion.nImgType = HV_IMAGE_YUV_422;
    CRect rcMax1(0, 0, m_nWidth, m_nHeight);
    CRect temprcet(0,30,100,100);
    CRect rcSearchRegion;
    //HV_COMPONENT_IMAGE imgCalibratedFrame;
    RTN_HR_IF_FAILED(MapParamRect(rcSearchRegion, rcMax1, temprcet));
    RTN_HR_IF_FAILED(CropImage(*pImgCalibratedFrame, rcSearchRegion, &imgCarRegion));
    MULTI_ROI MultiROI=
    {
        ARRSIZE(m_rgEPCarPos),
            m_rgEPCarPos
    };
    int iDetMinScale = 8;
    int iDetMaxScale = 25;

    //int iDetMinScale = 7;
    //int iDetMaxScale = 17;
    RTN_HR_IF_FAILED(m_pPlateDetector->ScanPlatesComponentImage(
        CAR_DETECTOR,
        &imgCarRegion, //imgCalibratedFrame,
        &MultiROI,
        iDetMinScale,//m_pParam->g_nDetMinScaleNum,
        iDetMaxScale,//m_pParam->g_nDetMaxScaleNum,
        TRUE,
        0,
        FALSE,
        NULL,
        NULL
        ));

    //g_nDetResultCount = MultiROI.dwNumROI;
    //for(int i = 0; i < MultiROI.dwNumROI; i++)
    //{
    //    g_rgDetResult[i].left = m_rgTempPlatePos[i].left;
    //    g_rgDetResult[i].top = m_rgTempPlatePos[i].top+rcSearchRegion.top;
    //    g_rgDetResult[i].right = m_rgTempPlatePos[i].right;
    //    g_rgDetResult[i].bottom = m_rgTempPlatePos[i].bottom+rcSearchRegion.top;
    //}
    //填充模型车辆检测框
    /*m_RectNum[4] = 0;
    for (int i = 0; i < MultiROI.dwNumROI; i++)
    {
        m_DrawRect[4][m_RectNum[4]].top = m_rgEPCarPos[i].top + rcSearchRegion.top;
        m_DrawRect[4][m_RectNum[4]].left = m_rgEPCarPos[i].left;
        m_DrawRect[4][m_RectNum[4]].right = m_rgEPCarPos[i].right;
        m_DrawRect[4][m_RectNum[4]].bottom = m_rgEPCarPos[i].bottom + rcSearchRegion.top;
        m_RectNum[4]++;
        if(m_RectNum[4] >= DRAW_RECT_LEN)m_RectNum[4] = DRAW_RECT_LEN - 1;
    }*/
    
    return S_OK;
}

//根据检测的结果来进行车辆的扫描
HRESULT CTrackerImpl::CheckEPNoPlateConfidence2(HV_COMPONENT_IMAGE* pImgCalibratedFrame, int iVideID)
{
    //能否把尺寸降一倍
    //尝试下没帧都做的情况？以后要改成3帧做一次
    CRect rcMax(0, 0, m_nWidth, m_nHeight);
    int iDetMinScale = 8;
    int iDetMaxScale = 25;
    //int iDetMinScale = 7;
    //int iDetMaxScale = 17;
    MULTI_ROI MultiROI=
    {
        ARRSIZE(m_rgEPCarPos),
            m_rgEPCarPos
    };
    //m_RectNum[4] = 0;
    for(int i = 0; i < m_cEPObjTrackInfo; i++)
    {
        if(m_rgEPObjTrackInfo[i].m_objTrackState == OBJ_TRACK_TEM)
        {
            //if(((m_rgEPObjTrackInfo[i].m_iObjInfoCount) & 3) != 3)continue;
            CRect roi = m_rgEPObjTrackInfo[i].LastInfo().m_rcFG;
            if(roi.bottom < (m_nHeight>>1))
            {
                continue;
            }
            CRect rLarge = roi;
            //扩大roi
            int iWidth = roi.Width();
            int iHeight = roi.Height();
            
            rLarge.left -= iWidth*3;
            rLarge.right += iWidth*3;
            rLarge.top -= iHeight*3;
            rLarge.bottom += iHeight*3;
           
            rLarge.IntersectRect(&rLarge, &rcMax);
            //HV_Trace("rLarge.width [%d], rLarge.height [%d]", rLarge.Width(), rLarge.Height());
            //rLarge is the search area
            HV_COMPONENT_IMAGE imgCarRegion;
            imgCarRegion.nImgType = HV_IMAGE_YUV_422;
            RTN_HR_IF_FAILED(CropImage(*pImgCalibratedFrame, rLarge, &imgCarRegion));
            //MultiROI.dwNumROI = 0;
            //memset(&MultiROI, 0x0, sizeof(MultiROI));

            RTN_HR_IF_FAILED(m_pPlateDetector->ScanPlatesComponentImage(
                CAR_DETECTOR,
                &imgCarRegion, //imgCalibratedFrame,
                &MultiROI,
                iDetMinScale,//m_pParam->g_nDetMinScaleNum,
                iDetMaxScale,//m_pParam->g_nDetMaxScaleNum,
                TRUE,
                0,
                FALSE,
                NULL,
                NULL
                ));
            if(MultiROI.dwNumROI > 0)
            {
                for (int j = 0 ; j < MultiROI.dwNumROI; j++ )
                {
                    //判断是否和模板的矩形框是否相交？
                    CRect rDetect;
                    CRect rInterSet(0,0,0,0);
                    rDetect.top = m_rgEPCarPos[j].top + rLarge.top;
                    rDetect.bottom = m_rgEPCarPos[j].bottom + rLarge.top;
                    rDetect.left = m_rgEPCarPos[j].left + rLarge.left;
                    rDetect.right = m_rgEPCarPos[j].right + rLarge.left;
                    
                    //m_DrawRect[4][m_RectNum[4]] = rDetect;
                    
                   // m_RectNum[4]++;
                    //if(m_RectNum[4] >= DRAW_RECT_LEN)m_RectNum[4] = DRAW_RECT_LEN - 1;
                    
                    if(rInterSet.IntersectRect(&rDetect, &roi))
                    {
                        //good!
                        m_rgEPObjTrackInfo[i].m_iEPNoPlateScore += 100;
                        break;
                    }
                    
                }
            }
            
        }
    }
    return S_OK;
}
void CTrackerImpl::SetTriggerInfo(const DWORD32 dwLastImageTime
								 ,const DWORD32 dwCarArriveTime
						         ,const DWORD32 dwRoadId
						         ,const DWORD32 dwTriggerIndex
						         )
{
	if (m_cfgParam.iPlatform == 0)
	{
		return;
	}
	PBYTE8 pbTmp = (PBYTE8)m_cfgParam.cTriggerInfoBuf.phys;
	if (pbTmp && m_cfgParam.cTriggerInfoBuf.len >= sizeof(TRIGGER_CAM_INFO))
	{
		TRIGGER_CAM_INFO cTriggerCamInfo;
		cTriggerCamInfo.dwCarArriveTime = dwCarArriveTime;
		cTriggerCamInfo.dwRoadId = dwRoadId;
		cTriggerCamInfo.dwTriggerIndex = dwTriggerIndex;
		memcpy(pbTmp, &cTriggerCamInfo, sizeof(cTriggerCamInfo));

		// todo. 要阻塞？
		//CACHE_wbL2(pbTmp, sizeof(cTriggerCamInfo), CACHE_WAIT);
		HV_writeback(pbTmp, sizeof(cTriggerCamInfo));

		/*char szInfo[256] = {0};
		sprintf(szInfo, "DSP Trigger Info LTime(%d) ATime(%d) RTime(%d) Road(%d) Index(%d)\n"
				,dwLastImageTime
				,cTriggerCamInfo.dwCarArriveTime
				,GetSystemTick()
				,cTriggerCamInfo.dwRoadId
				,cTriggerCamInfo.dwTriggerIndex
				);
		strcat(g_szDebugInfo, szInfo);*/
	}
}

//抓拍跟踪(有车牌)
void CTrackerImpl::CapTrack(CTrackInfo* pTrack)
{
	if (pTrack->m_fIsTrigger)//已经抓拍过
	{
		return;
	}

	bool fCanTrigger = false; //业务上是否允许抓拍
	if (m_fEnableCarArriveTrigger)
	{
		if(!m_iTriggerPlateType //所有车牌
		  ||(m_iTriggerPlateType == 1 && pTrack->LastInfo().color == PC_YELLOW)//仅黄牌
		)
		{
			fCanTrigger = true;
		}
	}
	if (false  == fCanTrigger)
	{
		return;
	}

	bool fRealFirst = false;
	DWORD32 dwCurTick = GetSystemTick();
	if ((dwCurTick - m_dwLastTriggerTick) > MIN_TRIGGER_TIME
		|| m_cfgParam.iPlatform == 2) // 水星平台不限制抓拍间隔时间
	{
		fRealFirst = true;
		++m_dwTriggerCameraTimes;
		if(m_dwTriggerCameraTimes >= 0xffff)
		{
			m_dwTriggerCameraTimes = 1;
		}
		//将抓拍信息记录到共享内存中
		SetTriggerInfo(pTrack->LastInfo().dwFrameTime
		               ,pTrack->m_nCarArriveTime
					   ,pTrack->m_iRoad
					   ,m_dwTriggerCameraTimes
					   );
		
		TriggerCamera(pTrack->m_iRoad);
		m_dwLastTriggerTick = dwCurTick;
	}
	pTrack->m_dwTriggerIndex = m_dwTriggerCameraTimes;
	pTrack->m_iCapCount = 1;

	if (m_cfgParam.iPlatform == 0 && m_cfgParam.iCapMode)//第二次抓拍
	{
		bool fSecond = false;
		if (m_cfgParam.iCapMode == 1)
		{
			if (m_cfgParam.iSpeedLimit != 0)
			{
				float fltCarSpeed(0.0f);
				float fltScaleOfDistance(1.0f);
				CalcCarSpeed(fltCarSpeed, fltScaleOfDistance, (*pTrack));
				pTrack->m_iCapSpeed = fltCarSpeed;
				if (fltCarSpeed > m_cfgParam.iSpeedLimit * 0.9)
				{
					fSecond = true;
				}
			}
		}
		else if (m_cfgParam.iCapMode == 2)
		{
			fSecond = true;
		}
		if (fSecond)
		{
			//使CarArriveTime与第一次抓拍的时间更加接近
			if (false == fRealFirst)
			{
				pTrack->m_nCarArriveTime = m_dwLastTriggerTick;
			}
			
			pTrack->m_iCapCount = 2;

			++m_dwTriggerCameraTimes;
			if(m_dwTriggerCameraTimes >= 0xffff)
			{
				m_dwTriggerCameraTimes = 1;
			}
			HV_Sleep(5);
			TriggerCamera(pTrack->m_iRoad);
			m_dwLastTriggerTick = dwCurTick;
		}
	}
	pTrack->m_fIsTrigger = true;
}

//抓拍物体(无车牌)
void CTrackerImpl::CapObjTrack(CObjTrackInfo* pObjTrack)
{
	if (pObjTrack->m_fIsTrigger)//已经抓拍过
	{
		return;
	}
	
	//业务上是否允许抓拍
	if (false == m_fEnableCarArriveTrigger || m_iTriggerPlateType)
	{
		return;
	}

	bool fRealFirst = false;
	DWORD32 dwCurTick = GetSystemTick();
	if ((dwCurTick - m_dwLastTriggerTick) > MIN_TRIGGER_TIME)
	{
		fRealFirst = true;
		++m_dwTriggerCameraTimes;
		if(m_dwTriggerCameraTimes >= 0xffff)
		{
			m_dwTriggerCameraTimes = 1;
		}
		//将抓拍信息记录到共享内存中
		SetTriggerInfo(pObjTrack->LastInfo().m_dwFrameTime
					  ,pObjTrack->m_nCarArriveTime
		              ,pObjTrack->m_iRoad
		              ,m_dwTriggerCameraTimes
		              );
		TriggerCamera(pObjTrack->m_iRoad);
		m_dwLastTriggerTick = dwCurTick;
	}
	
	pObjTrack->m_dwTriggerIndex = m_dwTriggerCameraTimes;
	pObjTrack->m_iCapCount = 1;
	
	if (m_cfgParam.iCapMode == 2)
	{
		//使CarArriveTime与第一次抓拍的时间更加接近
		if (false == fRealFirst)
		{
			pObjTrack->m_nCarArriveTime = m_dwLastTriggerTick;
		}
		
		pObjTrack->m_iCapCount = 2;

		++m_dwTriggerCameraTimes;
		if(m_dwTriggerCameraTimes >= 0xffff)
		{
			m_dwTriggerCameraTimes = 1;
		}
		HV_Sleep(5);
		TriggerCamera(pObjTrack->m_iRoad);
		m_dwLastTriggerTick = dwCurTick;
	}
	
	pObjTrack->m_fIsTrigger = true;
}


//适应的修改！如在算法端能处理掉路面光斑将去除掉
BOOL CTrackerImpl::RainyFlashControl(HiVideo::CRect* pPos, 
									 int iVideoID)
{
	//减少雨夜天气车道2的车辆多闪
	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcVideoDet;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDet, rcMax, m_rcVideoDetArea));

	int iLineWidth = 0;
	int iLineNum = 0;
	BOOL fFlash = true;
	iLineNum = MatchRoad(pPos->CenterPoint().x, pPos->CenterPoint().y, &iLineWidth);
	float fltRadio = (iLineWidth == 0) ? (1.0f) : (pPos->Width()/(float)iLineWidth);

	if(iLineNum == 1)//右边车道
	{
		IReferenceComponentImage* pCurrGray = m_rgpObjectDetector[iVideoID]->GetCurrFrame();
		if (pCurrGray != NULL)
		{
			CPersistentComponentImage imgGray;
			pCurrGray->GetImage(&imgGray);
			//在框里面找有没有车灯，宽度比例是否符合
			//如果没有车灯，且比较像光斑则不闪
			int iDownScaleX, iDownScaleY;
			HV_RECT rect;
			iDownScaleX = iDownScaleY = m_rgpObjectDetector[iVideoID]->GetMultiDownSampleScale();
			HvSize S = hvSize(imgGray.iWidth, imgGray.iHeight);
			HvSize orgSize = hvSize(CTrackInfo::m_iWidth, CTrackInfo::m_iHeight);
			CoodinateTrans(iDownScaleX, 
				iDownScaleY, 
				rcVideoDet.TopLeft(), *pPos, rect 
				, S, orgSize,
				COODINATE_RECT_SHRINK_SHIFT);
			//HV_Trace("rect.top")
			//20120425 liujie modify
			//发现该方法中的区域生长耗时较长，大约30ms
			//现才有替代的做法
			//if(RainyFlashCheck(&imgGray, rect, iDownScaleX, iDownScaleY))fFlash = false;
			if(RainyFlashCheck2(&imgGray, rect, iDownScaleX, iDownScaleY))fFlash = false;
			if (fFlash)
			{
				//查看已有的检测框中是否有灯组
				//找到最靠近底部的灯组，其bottom值作为触发闪的抓拍条件
				CRect rLastYRect(0,0,0,0);
				int iMaxBottom = 0; 
				for (int i = 0; i < m_iLightDetected; i++)
				{
					if(m_rgrcDetected[i].bottom > iMaxBottom)
					{
						iMaxBottom = m_rgrcDetected[i].bottom;
						rLastYRect = m_rgrcDetected[i];
					}
				}
				if (iMaxBottom != 0)
				{
					if(iMaxBottom < rcVideoDet.CenterPoint().y)fFlash = false;
				}
			}
			//end of modify 20120425
			if (fFlash)
			{
				//如果贴边且宽高比不符合车辆，过滤掉
				if (fltRadio < 0.35f && rect.right > S.width - 5)
				{
					fFlash = false;
				}
			}

		}
	}
	return fFlash;
}

//适应性的修改方法！待视频检测端真正突破后可能要去掉!
//仅仅使用在城市卡口
BOOL CTrackerImpl::RainyObjOutPutControl(HiVideo::CRect* pPos, int iTrackID)
{
	CRect rcMax(0, 0, m_nWidth, m_nHeight);
	CRect rcVideoDet;
	RTN_HR_IF_FAILED(MapParamRect(rcVideoDet, rcMax, m_rcVideoDetArea));
	//应该用中点的历史轨迹统计来区分开是否边道上的车辆
	BOOL fOutPut = true;
	CObjTrackInfo* pObjTrack = m_rgObjTrackInfo + iTrackID;
	//把最后3帧弄出来
	//3帧都贴边且宽度较小
	int iCount = pObjTrack->m_iObjInfoCount;
	int iLineWidth = 0;
	int iLineNum = 0;
	BOOL fFlash = true;
	float fltRadio1, fltRadio2, fltRadio3;

	iLineNum = MatchRoad(pPos->CenterPoint().x, pPos->CenterPoint().y, &iLineWidth);
	float fltRadio = (iLineWidth == 0) ? (1.0f) : (pPos->Width()/(float)iLineWidth);

	if(iCount > 6 && iLineNum == 1)
	{
		CRect roi1 = pObjTrack->LastInfo().m_rcFG;
		CRect roi2 = pObjTrack->m_rgObjInfo[iCount - 2].m_rcFG;
		CRect roi3 = pObjTrack->m_rgObjInfo[iCount - 3].m_rcFG;

		fltRadio1 = (iLineWidth == 0) ? (1.0f) : (roi1.Width()/(float)iLineWidth);
		fltRadio2 = (iLineWidth == 0) ? (1.0f) : (roi2.Width()/(float)iLineWidth);
		fltRadio3 = (iLineWidth == 0) ? (1.0f) : (roi3.Width()/(float)iLineWidth);

		//算临近程度
		int iDis = 0;
		iDis += (abs(rcVideoDet.right - roi1.right) + abs(rcVideoDet.right - roi1.left))/2;
		iDis += (abs(rcVideoDet.right - roi2.right) + abs(rcVideoDet.right - roi2.left))/2;
		iDis += (abs(rcVideoDet.right - roi3.right) + abs(rcVideoDet.right - roi3.left))/2;
		iDis /= 3;
		//HV_Trace("\n iDis:[%d]", iDis);
		//if(iDis < 150)fOutPut = false;

		if (fltRadio1 < 0.35f 
			&& fltRadio2 < 0.38f 
			&& fltRadio3 < 0.40f
			//&& roi1.right > rcVideoDet.right - 20 
			//&& roi2.right > rcVideoDet.right - 20
			//&& roi3.right > rcVideoDet.right - 20
			&& iDis < m_iEdgeFilterThreld
			)
		{
			fOutPut = false;
		}       
	}
	return fOutPut;
}

bool CTrackerImpl::IsContainOfTrap(TRAP_DETAREA trapDetArea, CRect rectPlate)
{
	bool bret = false;
	int iLX = -1;
	int iRX = -1;
	int iTY = -1;
	int iBY = -1;

	// 动态修改需求，不能用静态。
	float fltTopLeftX = (float)(1.0f * trapDetArea.TopLeftX * m_nWidth / 100 + 0.5);
	float fltTopLeftY = (float)(1.0f * trapDetArea.TopLeftY * m_nHeight / 100 + 0.5);
	float fltBottomLeftX = (float)(1.0f * trapDetArea.BottomLeftX* m_nWidth / 100 + 0.5);
	float fltBottomLeftY = (float)(1.0f * trapDetArea.BottomLeftY * m_nHeight / 100 + 0.5);
	float fltTopRightX = (float)(1.0f * trapDetArea.TopRightX * m_nWidth / 100 + 0.5);
	float fltTopRightY = (float)(1.0f * trapDetArea.TopRightY * m_nHeight / 100 + 0.5);
	float fltBottomRightX = (float)(1.0f * trapDetArea.BottomRightX * m_nWidth / 100 + 0.5);
	float fltBottomRightY = (float)(1.0f * trapDetArea.BottomRightY * m_nHeight / 100 + 0.5);

	//左边线
	float fltDX1 = fltTopLeftX - fltBottomLeftX;
	float fltDY1 = fltTopLeftY - fltBottomLeftY;	
	if(fltDX1 == 0.0f) fltDX1 = 1.0f;
	if(fltDY1 == 0.0f) fltDY1 = 1.0f;

	float fltLDY = (float)rectPlate.CenterPoint().y - fltBottomLeftY;
	iLX = (int)((fltDX1 / fltDY1) * fltLDY + fltBottomLeftX + 0.5);

	//右边线
	float fltRDX1 = fltTopRightX - fltBottomRightX;
	float fltRDY1 = fltTopRightY - fltBottomRightY;	
	if(fltRDX1 == 0.0f) fltRDX1 = 1.0f;
	if(fltRDY1 == 0.0f) fltRDY1 = 1.0f;

	float fltRDY = (float)rectPlate.CenterPoint().y - fltBottomRightY;
	iRX = (int)((fltRDX1 / fltRDY1) * fltRDY + fltBottomRightX + 0.5);

	//上边线
	float fltTDX1 = fltTopRightX - fltTopLeftX;
	float fltTDY1 = fltTopRightY - fltTopLeftY;	
	if(fltTDX1 == 0.0f) fltTDX1 = 1.0f;
	if(fltTDY1 == 0.0f) fltTDY1 = 1.0f;

	float fltTDX = (float)rectPlate.CenterPoint().x - fltTopLeftX;
	iTY = (int)((fltTDY1 / fltTDX1 ) * fltTDX + fltTopLeftY + 0.5);

	//下边线
	float fltBDX1 = fltBottomRightX - fltBottomLeftX;
	float fltBDY1 = fltBottomRightY - fltBottomLeftY;	
	if(fltBDX1 == 0.0f) fltBDX1 = 1.0f;
	if(fltBDY1 == 0.0f) fltBDY1 = 1.0f;

	float fltBDX = (float)rectPlate.CenterPoint().x - fltBottomLeftX;
	iBY = (int)((fltBDY1 / fltBDX1 ) * fltBDX + fltBottomLeftY + 0.5);


	//判断包含关系
	if(rectPlate.CenterPoint().x >= iLX &&  rectPlate.CenterPoint().x <= iRX && rectPlate.CenterPoint().y >= iTY && rectPlate.CenterPoint().y <= iBY)
	{
		bret = true;
	}
	return bret;
}

HRESULT CTrackerImpl::GetColorResult(PBYTE8 RBG1,PBYTE8 RBG2,bool *fDiffThr, int &nColor)
{
	int Color1,Color2;
	//字体判断
	if(RBG1[5] > 130)
		Color2 = 4;
	else if(RBG1[5] < 90)
		Color2 = 3;
	else
	{
		if(RBG1[3] > 80 && RBG1[3] < 190)
			Color2 = 4;
		else
			Color2 = 3;
	}
	//颜色判断
	if(*fDiffThr)
	{
		if(RBG2[3]>=20 && RBG2[3]<60 && RBG2[4]>=RBG2[5])  //条件RBG2[4]>=RBG2[5]为了保证没有白牌识别成黄色（普通警牌判断）
			Color1 = 2;
		else if(RBG2[3]>=60 && RBG2[3]<100)
			Color1 = 6;
		else if(RBG2[3]>=100 && RBG2[3]<190)   //存在白牌识别成蓝牌（字判断错误+字偏蓝）
			Color1 = 1;
		else
			Color1 = 4;
	}
	else
	{
		if(RBG2[5] > 110)
			Color1 = 4;
		else
			Color1 = 3;
	}
	//颜色配对
	if(Color1 == 2 && Color2 == 3)
		nColor = 2;
	else if(Color1 == 1 && Color2 == 4)
		nColor = 1;
	else if(Color1 == 3 && Color2 == 4)
		nColor = 3;
	else if(Color1 == 4 && Color2 == 3)
		nColor = 4;
	else if(Color1 == 1 && Color2 == 3)
		nColor = 5;
	else if(Color1 == 6 && Color2 == 4)
		nColor = 6;
	else
		nColor = 0;   //是否可以直接设置为4

	return  S_OK;
}
//会存在偏蓝的白牌或偏黄的白牌
HRESULT CTrackerImpl::GetPlateColor(PBYTE8 RBG1,PBYTE8 RBG2, int &nColor)
{
	//计算RGB的差值
	int DiffRGB1,DiffRGB2;
	DiffRGB1 = HV_ABS(RBG1[2]-RBG1[1]) + HV_ABS(RBG1[2]-RBG1[0]) + HV_ABS(RBG1[1]-RBG1[0]);
	DiffRGB2 = HV_ABS(RBG2[2]-RBG2[1]) + HV_ABS(RBG2[2]-RBG2[0]) + HV_ABS(RBG2[1]-RBG2[0]);
	//取字体索引
	int FontIdx = 0;
	if(DiffRGB1>DiffRGB2)  //车牌类型错误或底色与字判断错误
	{
		if(RBG1[3]<60 && RBG2[3]<60)   //黄牌
		{
			if(RBG1[5] > RBG2[5])
				FontIdx = 2;
			else
				FontIdx = 1;
		}
		else if(RBG1[3]<180 && RBG2[3]<180)  //蓝牌
		{
			if(RBG1[5] > RBG2[5])
				FontIdx = 1;
			else
				FontIdx = 2;
		}
		else
		{
			FontIdx = 0;
		}
	}
	else
	{
		FontIdx = 1;
	}
	//颜色识别
	bool DiffThr = false;
	if(FontIdx == 1)
	{
		if(DiffRGB2 > 60) DiffThr = true;
		GetColorResult(RBG1,RBG2,&DiffThr,nColor);  //车牌丈畔⒖煽?
	}
	else if(FontIdx == 2)                           //部分车牌颜色识别错误
	{
		if(DiffRGB1 > 60) DiffThr = true;
		GetColorResult(RBG2,RBG1,&DiffThr,nColor);
		if(nColor == 3)
			nColor = 4;
		else if(nColor == 4)
			nColor = 3;
	}
	else
	{
		nColor = 0;
	}

	return  S_OK;
}
HRESULT CTrackerImpl::CalcPlateColor(HV_COMPONENT_IMAGE imgFrame,PBYTE8 pBW, int &nColor)
{
	int Y1=0,U1=0,V1=0,nCount1=0;
	int Y2=0,U2=0,V2=0,nCount2=0;
	//修改高度和起点宽度
	int nTop = imgFrame.iHeight/3;
	int nBottom = imgFrame.iHeight - nTop;
	int nLeft = imgFrame.iWidth/6;
	int nRight = imgFrame.iWidth - nLeft;

	RESTRICT_PBYTE8 pYLine = GetHvImageData(&imgFrame, 0) + nTop*imgFrame.iStrideWidth[0];
	RESTRICT_PBYTE8	pULine = GetHvImageData(&imgFrame, 1) + nTop*imgFrame.iStrideWidth[1];
	RESTRICT_PBYTE8 pVLine = GetHvImageData(&imgFrame, 2) + nTop*imgFrame.iStrideWidth[2];
	RESTRICT_PBYTE8 pBinLine = pBW+ nTop*imgFrame.iWidth;
	for(int i=nTop;i<=nBottom;i++,
		pYLine += imgFrame.iStrideWidth[0], 
		pULine += imgFrame.iStrideWidth[1],
		pVLine += imgFrame.iStrideWidth[2],
		pBinLine += imgFrame.iWidth)
	{
		for(int j=nLeft;j<=nRight;j++)
		{
			if(*(pBinLine + j) == 255)
			{
				Y1+= pYLine[j];
				U1+= pULine[j>>1];
				V1+= pVLine[j>>1];
				nCount1++;
			}
			else
			{
				Y2+= pYLine[j];
				U2+= pULine[j>>1];
				V2+= pVLine[j>>1];
				nCount2++;
			}
		}
	}
	//得到YUV信息——利用YUV转换成HSLRGB
	BYTE8 cTempRGB1[6],cTempRGB2[6];
	if (nCount1==0) nCount1=1;
	if (nCount2==0) nCount2=1;
	YCbCr2BGR(Y1/nCount1,U1/nCount1,V1/nCount1,&cTempRGB1[2],&cTempRGB1[1],&cTempRGB1[0]);
	YCbCr2BGR(Y2/nCount2,U2/nCount2,V2/nCount2,&cTempRGB2[2],&cTempRGB2[1],&cTempRGB2[0]);
	BGR2HSL(cTempRGB1[2],cTempRGB1[1], cTempRGB1[0], &cTempRGB1[3], &cTempRGB1[4], &cTempRGB1[5]);
	BGR2HSL(cTempRGB2[2],cTempRGB2[1], cTempRGB2[0], &cTempRGB2[3], &cTempRGB2[4], &cTempRGB2[5]);
	GetPlateColor(cTempRGB1,cTempRGB2,nColor);
	return  S_OK;

}

// 车辆检测模型的更新接口
HRESULT CTrackerImpl::UpdateModel(unsigned int uModelId, unsigned char* pbData, unsigned int cbDataSize)
{
	// todo.
	// 在此增加模型的动态加载。
	char szMsg[256];
	sprintf(szMsg, "UpdateModel. id:%d, pbData:%08x, size:%d.",
			uModelId, pbData, cbDataSize);
	OutPutDebugMsg(LOG_LV_DEBUG, szMsg);
	return S_OK;
}

// 动态更新算法参数接口
HRESULT CTrackerImpl::UpdateAlgParam(unsigned char* pbData, unsigned int cbDataSize)
{
	// todo.
	// 在此增加参数处理
	char szMsg[256];
	sprintf(szMsg, "UpdateAlgParam. pbData:%08x, size:%d.",
			pbData, cbDataSize);
	OutPutDebugMsg(LOG_LV_DEBUG, szMsg);
	return S_OK;
}


//=================------------------------------------=============================
