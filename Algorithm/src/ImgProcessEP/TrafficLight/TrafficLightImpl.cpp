#include ".\trafficlightimpl.h"
#include "..\Tracker\EPAppUtils.h"

extern char g_szDebugInfo[256];
CTrafficLightImpl::CTrafficLightImpl(void)
{
	InitTrafficLightParam();
}

CTrafficLightImpl::~CTrafficLightImpl(void)
{
	
}
int CTrafficLightImpl::InitTrafficLightParam()
{
	m_iRedLightCount = 0;
	m_iLightTypeCount = 0;
	m_iLightTypeCount = 0;
	m_iLastLightStatus = -1;
	m_iCurLightStatus = -1;
	m_iLastOkLightStatus = -1;
	m_fCheckSpeed = false;
	m_iErrorSceneCount = 0;
	memset(m_rgiSceneStatus, 0, sizeof(m_rgiSceneStatus));
	memset(m_rgRedLightRect, 0, sizeof(m_rgRedLightRect));
	memset(m_rgLightRect, 0, sizeof(m_rgLightRect));
	
	for (int i = 0; i < MAX_LIGHT_TYPE_COUNT; i++)
	{
		m_rgLightType[i].iPos = m_rgLightType[i].iPos = -1;
	}
	for (int i = 0; i < MAX_SCENE_COUNT; i++)
	{
		memset(m_rgsiInfo[i].pszInfo, 0, 64);
		m_rgLightScene[i].lsForward = m_rgLightScene[i].lsLeft = m_rgLightScene[i].lsRight = m_rgLightScene[i].lsTurn = TLS_UNSURE;
	}
	for(int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
	{
		m_rgLightInfo[i].Reset();
		m_rgLightRect[i] = HiVideo::CRect(0, 0, 0, 0);
	}
	for(int i = 0; i < MAX_IO_LIGHT_COUNT; ++i)
	{
		m_rgIOLight[i].iPos = -1;
		m_rgIOLight[i].iTeam = -1;
		m_rgIOLight[i].tlsStatus = TLS_UNSURE;
		m_rgIOLight[i].tpPos = TP_UNSURE;
	}

	m_redrealtime.Reset();
	m_greentick.Reset();
	m_tlpTrafficLightParam.iLightGroupCount = 0;
	for (int i  = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
	{
		m_tlpTrafficLightParam.rgtgiLight[i].iLightCount = 0;
		m_tlpTrafficLightParam.rgtgiLight[i].iLightType = 0;
		m_tlpTrafficLightParam.rgtgiLight[i].rcLight = HiVideo::CRect(0, 0, 0, 0);
		for (int j = 0; j < 10; j++)
		{
			m_tlpTrafficLightParam.rgtgiLight[i].lsLightStatus[j] = TLS_UNSURE;
			m_tlpTrafficLightParam.rgtgiLight[i].tpLightPos[j] = TP_UNSURE;
		}
	}
	return 0;
}
HRESULT CTrafficLightImpl::SetTrafficLightParam(TRAFFIC_LIGHT_PARAM tlpTrafficLightParam)
{
	InitTrafficLightParam();
	HV_Trace(5, "SetTrafficLightParam():%d\n", tlpTrafficLightParam.iLightGroupCount);
	m_tlpTrafficLightParam = tlpTrafficLightParam;
	for (int i = 0; i <  m_tlpTrafficLightParam.iLightGroupCount; i++)
	{
		m_rgLightInfo[i].SetRect(m_tlpTrafficLightParam.rgtgiLight[i].rcLight, m_tlpTrafficLightParam.rgtgiLight[i].iLightCount, m_tlpTrafficLightParam.rgtgiLight[i].iLightType);		
		m_rgLightInfo[i].SetAdjustRect(m_tlpTrafficLightParam.rgtgiLight[i].rcLight);
		
		for (int j  = 0; j < m_tlpTrafficLightParam.rgtgiLight[i].iLightCount; j++)
		{
			m_rgLightInfo[i].SetLightStatus(j, m_tlpTrafficLightParam.rgtgiLight[i].lsLightStatus[j]);
			SetTrafficLightType(i, j, m_tlpTrafficLightParam.rgtgiLight[i].tpLightPos[j], m_tlpTrafficLightParam.rgtgiLight[i].lsLightStatus[j]);
			if (m_tlpTrafficLightParam.rgtgiLight[i].lsLightStatus[j] == TLS_RED)
			{
				m_rgRedLightRect[m_iRedLightCount++] = m_tlpTrafficLightParam.rgtgiLight[i].rcLight;
			}
		}
		m_rgLightInfo[i].SetTrafficLightParam(tlpTrafficLightParam);
	}
	for (int i = 0; i < MAX_SCENE_COUNT; i++)
	{
		//获取场景
		m_rgLightScene[i] = m_tlpTrafficLightParam.rgtsLightScene[i];
		SetSceneInfo(i, m_tlpTrafficLightParam.rgsiInfo[i]);
	}
	for (int i = 0; i < MAX_IO_LIGHT_COUNT; i++)
	{
		m_rgIOLight[i] = m_tlpTrafficLightParam.rgIOLight[i];
	}
	if (m_tlpTrafficLightParam.iValidSceneStatusCount < 5)
	{
		m_fCheckSpeed = m_tlpTrafficLightParam.fCheckSpeed = true;
	}
	if (m_tlpTrafficLightParam.iCheckType == 1)
	{
		m_tlpTrafficLightParam.iValidSceneStatusCount = 1;
	}
	return S_OK;
}
HRESULT CTrafficLightImpl::SetTrafficLightCallback(ITracker *pCallback)
{
	m_pCallback = pCallback;
	return S_OK;
}
bool CTrafficLightImpl::GetLightScene(int iPos, TRAFFICLIGHT_SCENE* pts)
{
	bool fRet = false;
	if( pts == NULL ) return fRet;
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT)
	{
		*pts = m_rgLightScene[iPos];
		fRet = true;
	}
	else if(iPos == -1)
	{
		(*pts).lsForward = TLS_UNSURE;
		(*pts).lsRight = TLS_UNSURE;
		(*pts).lsLeft = TLS_UNSURE;
		(*pts).lsTurn = TLS_UNSURE;
		fRet = true;
	}
	return fRet;
}
bool CTrafficLightImpl::GetRedLightStartTime(LIGHT_REALTIME* plrRedLightTime)
{
	*plrRedLightTime = m_redrealtime;
	return true;
}
bool CTrafficLightImpl::GetGreenLightStartTime(LIGHT_TICK* pGreenTick)
{
	*pGreenTick = m_greentick;
	return true;
}
CTrafficLightInfo* CTrafficLightImpl::GetTrafficLightInfo(int iIndex)
{
	if (iIndex < 0 || iIndex > MAX_TRAFFICLIGHT_COUNT) return NULL;
	return (&m_rgLightInfo[iIndex]);
}
//通过IO取当前红绿灯状态
DWORD32 CTrafficLightImpl::GetIOStatus(int iTeam, BYTE8 bLevel)
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
//lixh 2011-09-19 新增红绿灯灯组输出信息
HRESULT CTrafficLightImpl::PrintLightStatus(const LIGHT_TEAM_STATUS& ltsInfo , char* pLightStatus,int iLightStatusLen)
{

	int iPos = 0;
	DWORD32 dwStatus = 0;


	if(NULL == pLightStatus || iLightStatusLen<=0)
	{
		return S_FALSE;
	}
	for(DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i)
	{
		dwStatus = ltsInfo.pdwStatus[i];
		for(int k = ltsInfo.pnLightCount[i] - 1; k >= 0 ; --k)
		{
			if(iPos + k >= iLightStatusLen)
			{
				return S_FALSE;	
			}
			DWORD32 dwLightStatus = dwStatus & 0x0F;
			//外接红绿灯需要跟设置场景对应上20120525
			if (m_tlpTrafficLightParam.iCheckType == 1 && dwLightStatus == 1)
			{
				switch(m_rgIOLight[k].tlsStatus)
				{
				case TLS_RED:
					dwLightStatus = TS_RED;
					break;
				case TLS_YELLOW:
					dwLightStatus = TS_YELLOW;
					break;
				case TLS_GREEN:
					dwLightStatus = TS_GREEN;
					break;
				case TLS_UNSURE:
					dwLightStatus = TS_UNSURE;
					break;
				default:
					break;
				}
			}
			//end
			switch (dwLightStatus)
			{
			case TS_RED:
				pLightStatus[iPos + k] = '1';
				break;
			case TS_YELLOW:
				pLightStatus[iPos + k] = '2';
				break;
			case TS_GREEN:
				pLightStatus[iPos + k] = '3';
				break;
			case TS_UNSURE:
				pLightStatus[iPos + k] = '0';
				break;
			default:
				pLightStatus[iPos + k] = '?';
				break;
			}
			dwStatus >>= 4;
		}
		iPos += ltsInfo.pnLightCount[i];
		if(  DWORD32(i+1) < ltsInfo.nTeamCount )
		{
			if(iPos+1>= iLightStatusLen)
			{
				return S_FALSE;
			}
			pLightStatus[iPos++] = '_';
		}
		//
	}
	return S_OK;
}
HRESULT CTrafficLightImpl::RecognizeTrafficLight(HV_COMPONENT_IMAGE* pSceneImage, int* iCurrentStatus, int* iLastStatus, bool fNight)
{
	if (m_tlpTrafficLightParam.iLightGroupCount <= 0) return S_FALSE;
	HRESULT hResult = S_FALSE;
	static int nSceneStatusPos= 0;

	LIGHT_TEAM_STATUS ltsInfo;
	ltsInfo.nTeamCount = m_tlpTrafficLightParam.iLightGroupCount;

	bool fDynamic = false;
	static int iSkipFrameNo = m_tlpTrafficLightParam.iSkipFrameNo;	
	for(int i = 0; i<m_tlpTrafficLightParam.iLightGroupCount; i++)
	{
		if (m_tlpTrafficLightParam.iCheckType == 0)
		{
			//红绿灯识别
			if (iSkipFrameNo == 0 && m_tlpTrafficLightParam.iAutoScanLight == 1)
			{
				m_rgLightInfo[i].UpdateStatus3( pSceneImage , fNight);
				m_rgLightRect[i] = m_rgLightInfo[i].GetRect();
				m_rgLightStatus[i] = m_rgLightInfo[i].GetLastStatus();		
				fDynamic = true;
			}
			else
			{
				m_rgLightInfo[i].UpdateStatus( pSceneImage );
				m_rgLightRect[i] = m_rgLightInfo[i].GetRect();
				m_rgLightStatus[i] = m_rgLightInfo[i].GetLastStatus();
			}
			sv::utTrace("<123dsp light>status:%08x.", m_rgLightStatus[i]);
		}
		else
		{
			if (m_pCallback == NULL)
			{
				return E_FAIL;
			}
			m_rgLightRect[i] = m_rgLightInfo[i].GetRect();
			BYTE8 bLevel = 0;
			if (S_OK != m_pCallback->GetTrafficLightStatus(&bLevel))
			{
				return E_FAIL;
			}
			m_rgLightStatus[i] = GetIOStatus(i, bLevel);
		}
		m_rgLightInfo[i].m_iRedLightCountFinded = 0;
		m_rgLightInfo[i].m_iGreenLightCountFinded = 0;
		ltsInfo.pdwStatus[i] = m_rgLightStatus[i];
		ltsInfo.pnLightCount[i] = m_rgLightInfo[i].GetPosCount();
		//传检测到的灯传出去显示
		if(m_pCallback)
		{
			m_pCallback->TrafficLightStatusEx(
				i,
				m_rgLightInfo[i].m_iRedLightCountFinded,
				m_rgLightInfo[i].m_rcRedLightPosFinded,
				m_rgLightInfo[i].m_iGreenLightCountFinded,
				m_rgLightInfo[i].m_rcGreenLightPosFinded
			);
		}
	}
	if (iSkipFrameNo <= 0)
	{
		iSkipFrameNo = m_tlpTrafficLightParam.iSkipFrameNo;
	}
	else
	{
		iSkipFrameNo--;
	}
	//取得当前场景
	bool fFlag = (m_tlpTrafficLightParam.iSceneCheckMode == 0);
	int nCurScene = TransitionScene(ltsInfo, m_tlpTrafficLightParam.iSceneCount, m_iLastOkLightStatus, fFlag);
	sv::utTrace("<123dsp light>CurScene:%08x.", nCurScene);

	//调整发送出去的红灯的坐标
	if (m_tlpTrafficLightParam.iAutoScanLight == 1 && fDynamic)
	{
		////更新最后可正常识别的灯组的位置
		if (nCurScene != -1)
		{
			for (int i = 0; i < m_tlpTrafficLightParam.iLightGroupCount; i++)
			{
				m_rgLightInfo[i].UpdateLastSureLight(m_rgLightInfo[i].GetRect());
			}
		}
		else
		{
			//处于中间状态时,则将灯红坐标还原回最后可识别出的状态
			for (int i = 0; i < m_tlpTrafficLightParam.iLightGroupCount; i++)
			{
				m_rgLightInfo[i].SetRect(m_rgLightInfo[i].GetLastSureLight(), m_rgLightInfo[i].GetPosCount(), m_rgLightInfo[i].GetLightDirection());
			}
		}
		//更新红灯的坐标
		UpdateRedLightPos();
	}

	if( m_pCallback )
	{
		int nSceneStatus = -1;

		if( m_pCallback != NULL)
		{
			//把红灯的位置传出去
			m_pCallback->TrafficLightStatus(
				m_tlpTrafficLightParam.iLightGroupCount,
				m_rgLightStatus,
				&nSceneStatus,
				m_rgLightRect,
				m_iRedLightCount,
				m_rgRedLightRect,
				pSceneImage
				);
		}

		nSceneStatus = nCurScene;
#if 0
		//新增输出红绿灯灯组信息  lixh 2011-09-19 
		static char pLightStatus[64] = "", pLastStatus[64] = "";
		memset(pLightStatus, 0, 64);
		PrintLightStatus(ltsInfo , pLightStatus , 64);
		if(0 != strcmp(pLightStatus, pLastStatus))
		{
			strcpy(pLastStatus, pLightStatus);
			sprintf(g_szDebugInfo, "rg status = %s", pLastStatus);
			for(int i = 0; i < m_tlpTrafficLightParam.iLightGroupCount; i++)
			{
				CRect rc = m_rgLightInfo[i].GetRect();
				sprintf(g_szDebugInfo + strlen(g_szDebugInfo), "(%d,%d,%d,%d)", rc.left, rc.top, rc.right, rc.bottom);
			}
		}
#endif
		m_iCurLightStatus = nSceneStatus;
		//如果用户设置了场景状态则加入场景状态队列
		//if(hr == S_OK && nSceneStatus != -1)
		if( nSceneStatus != -1 )
		{
			m_iErrorSceneCount = 0;
			SetSceneStatus(nSceneStatusPos, nSceneStatus);
			nSceneStatusPos++;	//下一个存储位置,同时也是计数

			int iRelayTimeMs = m_tlpTrafficLightParam.iValidSceneStatusCount * 100;

			//加入场景状态后判断是否输出
			if(m_tlpTrafficLightParam.iValidSceneStatusCount > 0 && nSceneStatusPos >= m_tlpTrafficLightParam.iValidSceneStatusCount)
			{
				int nLastPos = nSceneStatusPos - 1;
				int nLastStatus = GetSceneStatus(nLastPos);
				BOOL fValid = TRUE;
				for( int i = nLastPos - 1; i > (nLastPos - m_tlpTrafficLightParam.iValidSceneStatusCount); i--)
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
					int iNextStatus = (m_iLastOkLightStatus + 1) % m_tlpTrafficLightParam.iSceneCount; 
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
					if( m_tlpTrafficLightParam.iRedLightDelay > 0 )
					{
						if( tsLast.lsLeft == TLS_GREEN && tsRelay.lsLeft == TLS_RED )
						{
							if( m_ltRedRelay.dwLeft == 0 ) m_ltRedRelay.dwLeft = dwTick;
							if( int(dwTick - m_ltRedRelay.dwLeft) <  (m_tlpTrafficLightParam.iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
						if( tsLast.lsForward == TLS_GREEN && tsRelay.lsForward == TLS_RED )
						{
							if( m_ltRedRelay.dwForward == 0 ) m_ltRedRelay.dwForward = dwTick;
							if( int(dwTick - m_ltRedRelay.dwForward) <  (m_tlpTrafficLightParam.iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
						if( tsLast.lsRight == TLS_GREEN && tsRelay.lsRight == TLS_RED )
						{
							if( m_ltRedRelay.dwRight == 0 ) m_ltRedRelay.dwRight = dwTick;
							if( int(dwTick - m_ltRedRelay.dwRight) <  (m_tlpTrafficLightParam.iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
						if( tsLast.lsTurn == TLS_GREEN && tsRelay.lsTurn == TLS_RED )
						{
							if( m_ltRedRelay.dwTurn == 0 ) m_ltRedRelay.dwTurn = dwTick;
							if( int(dwTick - m_ltRedRelay.dwTurn) <  (m_tlpTrafficLightParam.iRedLightDelay * 1000) )
							{
								fRelayTimeOut = false;
							}
						}
					}

					if( fRelayTimeOut )
					{
						m_ltRedRelay.Reset();
						m_iLastLightStatus = nLastStatus;
						TRAFFICLIGHT_SCENE tsSceneStatus, tsNow;
						GetLightScene(m_iLastLightStatus, &tsSceneStatus);
						GetLightScene(m_iLastLightStatus, &tsNow);

						int iGreenRelayMs = 1000;

						//新增输出红绿灯灯组信息  lixh 2011-09-19 
						char pLightStatus[64];
						memset(pLightStatus, 0, 64);
						PrintLightStatus(ltsInfo , pLightStatus , 64);


						if( m_pCallback != NULL )
						{
							m_pCallback->TrafficSceneStatus(tsSceneStatus,pLightStatus);
						}

						//更新红灯开始时间
						if( nLastStatus != -1 )
						{
							m_iLastOkLightStatus = nLastStatus;
						}

						//DWORD32 dwTimeLow, dwTimeHigh;
						//ConvertTickToSystemTime((GetSystemTick() - iRelayTimeMs), dwTimeLow, dwTimeHigh);
						//一体机代码，红绿灯时间只有L，没有H，在ARM端在通过ConvertTickToSystemTime进行转换,黄国超修改,2011-07-21
						DWORD32 dwTimeLow = GetSystemTick() - iRelayTimeMs, dwTimeHigh = 0;
						if(  tsNow.lsLeft == TLS_RED && tsLast.lsLeft != TLS_RED )
						{
							m_redrealtime.dwLeftL = dwTimeLow;
							m_redrealtime.dwLeftH = dwTimeHigh;
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
			m_iErrorSceneCount++;
			if( m_iErrorSceneCount >= (m_tlpTrafficLightParam.iValidSceneStatusCount * 2))
			{
				m_iLastLightStatus = -1;
				TRAFFICLIGHT_SCENE tsSceneStatus;
				GetLightScene(m_iLastLightStatus, &tsSceneStatus);

				//新增输出红绿灯灯组信息  lixh 2011-09-19 
				char pLightStatus[64];
				memset(pLightStatus, 0, 64);
				PrintLightStatus(ltsInfo , pLightStatus , 64);
				if( m_pCallback != NULL )
				{
					m_pCallback->TrafficSceneStatus(tsSceneStatus,pLightStatus);
				}
				//无效场景有可能是中间状态，红灯时间不能重置。
			}
			//如果无效场景帧数大过设定的值，则重置红绿灯框位置,红灯时间重置
			if( m_iErrorSceneCount > MAX_ERROR_COUNT )
			{
				m_iErrorSceneCount = 0;
				m_iLastOkLightStatus = -1;
				for(int i = 0; i < 4; ++i)
				{
					m_redrealtime.Reset();
					m_greentick.Reset();
				}
				if( m_tlpTrafficLightParam.iAutoScanLight == 1 )
				{
					for(int i = 0; i <m_tlpTrafficLightParam.iLightGroupCount; ++i)
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
	*iCurrentStatus = m_iCurLightStatus;
	*iLastStatus = m_iLastLightStatus;
	return hResult;
}
//如果是灯组状态与场景是邦定的
int CTrafficLightImpl::GetSceneNumber(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount)
{
	int iRet = -1;
	char szSceneInfo[64];
	memset(szSceneInfo, 0, 64);
	int iPos = 0;
	DWORD32 dwStatus = 0;
	for(DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i)
	{
		dwStatus = ltsInfo.pdwStatus[i];
		for(int k = ltsInfo.pnLightCount[i] - 1; k >= 0 ; --k)
		{
			switch (dwStatus & 0x0F)
			{
			case TS_RED:
				szSceneInfo[iPos + k] = '1';
				break;
			case TS_YELLOW:
				szSceneInfo[iPos + k] = '2';
				break;
			case TS_GREEN:
				szSceneInfo[iPos + k] = '3';
				break;
			case TS_UNSURE:
				szSceneInfo[iPos + k] = '0';
				break;
			default:
				szSceneInfo[iPos + k] = '?';
				break;
			}
			dwStatus >>= 4;
		}
		iPos += ltsInfo.pnLightCount[i];
		if(  DWORD32(i+1) < ltsInfo.nTeamCount )
		{
			szSceneInfo[iPos++] = '_';
		}
	}

	SCENE_INFO stInfo;
	int iLen = 0;
	for( int i = 0; i < iSceneCount; ++i )
	{
		if( GetSceneInfo(i, &stInfo) )
		{
			iLen = (int)strlen(stInfo.pszInfo);
			if( strlen(szSceneInfo) != iLen ) continue;
			
			int k = 0;
			for(k = 0; k < iLen; ++k)
			{
				if( stInfo.pszInfo[k] == '?' ) continue;
				if( stInfo.pszInfo[k] == '*' &&  szSceneInfo[k] > '0' ) continue;
				if( stInfo.pszInfo[k] != szSceneInfo[k] ) break; 
			}
			if( k >= iLen ) 
			{
				iRet = i;
				break;
			}
		}
	}
	return iRet;
}
//传入各灯组的状态，返回场景状态编号
int CTrafficLightImpl::TransitionScene(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount, const int& iLastOkLightStatus,  bool fFlag)
{
	int iRet = -1;

	if( !fFlag ) return GetSceneNumber(ltsInfo, iSceneCount);

	if( ltsInfo.nTeamCount == 0 || ltsInfo.nTeamCount > MAX_TRAFFICLIGHT_COUNT ) return iRet;

	//场景
	TRAFFICLIGHT_SCENE* pts = new TRAFFICLIGHT_SCENE[ltsInfo.nTeamCount];
	if( pts == NULL ) return iRet;

	int* piUsed = new int[ltsInfo.nTeamCount * 4];
	if( piUsed == NULL )
	{
		delete[] pts;
		return iRet;
	}
	memset(piUsed, 0, sizeof(int) *(ltsInfo.nTeamCount * 4));
	memset(pts, 0, sizeof(TRAFFICLIGHT_SCENE) * ltsInfo.nTeamCount);
	TRAFFICLIGHT_TYPE lt;
	int iCount = 0;
	DWORD32 dwTmp = 0;

	for( DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i )
	{	
		dwTmp = ltsInfo.pdwStatus[i];
		iCount = ltsInfo.pnLightCount[i];
		for(int k = iCount - 1; k >= 0; --k)
		{
			if( (dwTmp & 0x01) && GetTrafficLightType(i, k, &lt) )
			{
				LIGHT_STATUS tlsStatus = lt.tlsStatus;
				
				//如果灯组是三色灯 zhaopy 不判断灯颜色，在灯不亮时会有问题，因绿灯域值太低。
				//if (lt.tlsStatus == TLS_UNSURE)
				//外接红绿灯信号时,不能进入此判断
				if (m_tlpTrafficLightParam.iCheckType != 1)
				{
					switch (dwTmp & 0x0F)
					{
					case TS_RED:
						tlsStatus = TLS_RED;
						break;
					case TS_YELLOW:
						tlsStatus = TLS_YELLOW;
						break;
					case TS_GREEN:
						tlsStatus = TLS_GREEN;
						break;
					default:
						tlsStatus = TLS_UNSURE;
						break;
					}
				}

				switch(lt.tpPos)
				{
				case TP_LEFT:
					pts[i].lsLeft = tlsStatus;
					piUsed[i * 4 + 0] = 1;
					break;
				case TP_RIGHT:
					pts[i].lsRight = tlsStatus;
					piUsed[i * 4 + 1] = 1;
					break;
				case TP_FORWARD:
					pts[i].lsForward = tlsStatus;
					piUsed[i * 4 + 2] = 1;
					break;
				case TP_TURN:
					pts[i].lsTurn = tlsStatus;
					piUsed[i * 4 + 3] = 1;
					break;
				case TP_UNSURE:
					{
						//未定义只能用在黄灯
						if( tlsStatus == TLS_YELLOW && iLastOkLightStatus != -1 )
						{
							int iNextStatus = (iLastOkLightStatus + 1) % iSceneCount; 
							TRAFFICLIGHT_SCENE tsLast, tsNext;
							GetLightScene(iLastOkLightStatus, &tsLast);
							GetLightScene(iNextStatus, &tsNext);
							if( (tsLast.lsForward == TLS_GREEN && tsNext.lsForward == TLS_YELLOW)
								|| tsLast.lsForward == TLS_YELLOW ) 
							{
								pts[i].lsForward = TLS_YELLOW;
								piUsed[i * 4 + 2] = 1;
							}
							else if( (tsLast.lsLeft == TLS_GREEN && tsNext.lsLeft == TLS_YELLOW)
								|| tsLast.lsLeft == TLS_YELLOW )
							{
								pts[i].lsLeft = TLS_YELLOW;
								piUsed[i * 4 + 0] = 1;
							}
							else if( (tsLast.lsRight == TLS_GREEN && tsNext.lsRight == TLS_YELLOW)
								|| tsLast.lsRight == TLS_YELLOW)
							{
								pts[i].lsRight = TLS_YELLOW;
								piUsed[i * 4 + 1] = 1;
							}
							else if( (tsLast.lsTurn == TLS_GREEN && tsNext.lsTurn == TLS_YELLOW)
								|| tsLast.lsTurn == TLS_YELLOW )
							{
								pts[i].lsTurn = TLS_YELLOW;
								piUsed[i * 4 + 3] = 1;
							}
						}
					}
					break;

				default:
					break;
				}
			}
			dwTmp >>= 4;
		}
	}

	TRAFFICLIGHT_SCENE tsDest = {TLS_UNSURE, TLS_UNSURE, TLS_UNSURE, TLS_UNSURE};
	bool fL, fR, fF, fT;
	fL = fR = fF = fT = false;
	for(DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i)
	{
		if( !fL && pts[i].lsLeft != TLS_UNSURE && piUsed[i * 4 + 0] == 1)
		{
			tsDest.lsLeft = pts[i].lsLeft;
			fL = true;
		}
		else if( fL && piUsed[i * 4 + 0] == 1 && pts[i].lsLeft != tsDest.lsLeft )
		{
			tsDest.lsLeft = TLS_UNSURE;
		}

		if( !fR && pts[i].lsRight != TLS_UNSURE && piUsed[i * 4 + 1] == 1)
		{
			tsDest.lsRight = pts[i].lsRight;
			fR = true;
		}
		else if( fR && piUsed[i * 4 + 1] == 1 && pts[i].lsRight != tsDest.lsRight )
		{
			tsDest.lsRight = TLS_UNSURE;
		}

		if( !fF && pts[i].lsForward != TLS_UNSURE && piUsed[i * 4 + 2] == 1)
		{
			tsDest.lsForward = pts[i].lsForward;
			fF = true;
		}
		else if( fF && piUsed[i * 4 + 2] == 1 && pts[i].lsForward != tsDest.lsForward )
		{
			tsDest.lsForward = TLS_UNSURE;
		}

		if( !fT && pts[i].lsTurn != TLS_UNSURE && piUsed[i * 4 + 3] == 1)
		{
			tsDest.lsTurn = pts[i].lsTurn;
			fT = true;
		}
		else if( fT && piUsed[i * 4 + 3] == 1 && pts[i].lsTurn != tsDest.lsTurn )
		{
			tsDest.lsTurn = TLS_UNSURE;
		}
	}

	TRAFFICLIGHT_SCENE tsTmp;
	for(int i = 0; i < iSceneCount; ++i)
	{
		if( GetLightScene(i, &tsTmp) )
		{
			if( tsTmp.lsLeft == tsDest.lsLeft && tsTmp.lsRight == tsDest.lsRight 
				&& tsTmp.lsForward == tsDest.lsForward && tsTmp.lsTurn == tsDest.lsTurn )
			{
				iRet = i;
				break;
			}
		}
	}

	delete[] pts;
	delete[] piUsed;

	return iRet;
}
void CTrafficLightImpl::SetSceneInfo(int iPos, const SCENE_INFO& ts)
{
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT )
	{
		memcpy(m_rgsiInfo[iPos].pszInfo, ts.pszInfo, 64);
	}
}
bool CTrafficLightImpl::GetSceneInfo(int iPos, SCENE_INFO* pts)
{
	bool fRet = false;
	if( pts == NULL ) return fRet;
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT)
	{
		memcpy((*pts).pszInfo, m_rgsiInfo[iPos].pszInfo, 64);
		fRet = true;
	}
	return fRet;
}
void CTrafficLightImpl::SetTrafficLightType(int iTeam, int iPos, _TRAFFICLIGHT_POSITION tpPos, _TRAFFICLIGHT_LIGHT_STATUS tlsStatush)
{
	if(m_iLightTypeCount >= MAX_LIGHT_TYPE_COUNT ) return;
	m_rgLightType[m_iLightTypeCount].iTeam = iTeam;
	m_rgLightType[m_iLightTypeCount].iPos = iPos;
	m_rgLightType[m_iLightTypeCount].tpPos = tpPos;
	m_rgLightType[m_iLightTypeCount].tlsStatus = tlsStatush;
	m_iLightTypeCount++;
}

bool CTrafficLightImpl::GetTrafficLightType(int iTeam, int iPos, TRAFFICLIGHT_TYPE* ltLightType)
{
	if( iTeam < 0 || iPos < 0 || ltLightType == NULL ) return false;
	for(int i = 0; i < MAX_LIGHT_TYPE_COUNT; ++i)
	{
		if( m_rgLightType[i].iTeam == iTeam 
			&& m_rgLightType[i].iPos == iPos )
		{
			*ltLightType = m_rgLightType[i];
			return true;
		}
	}
	return false;
}
//修正红灯的坐标
HRESULT CTrafficLightImpl::UpdateRedLightPos()
{
	int iIndex  = 0;
	for(int i = 0; i < m_iLightTypeCount; i++)
	{
		if ( m_rgLightInfo[m_rgLightType[i].iTeam].GetLightDirection() == 0 )
		{
			//横向灯
			if (m_rgLightType[i].tlsStatus == TLS_RED)
			{
				HV_RECT rectRedLight;
				int iWidth = m_rgLightRect[m_rgLightType[i].iTeam].right - m_rgLightRect[m_rgLightType[i].iTeam].left;				
				int iLightCount = m_rgLightInfo[m_rgLightType[i].iTeam].GetPosCount();

				rectRedLight.left = m_rgLightRect[m_rgLightType[i].iTeam].left + m_rgLightType[i].iPos * iWidth / iLightCount;
				rectRedLight.right = rectRedLight.left + iWidth / iLightCount;
				rectRedLight.top = m_rgLightRect[m_rgLightType[i].iTeam].top;
				rectRedLight.bottom = m_rgLightRect[m_rgLightType[i].iTeam].bottom;

				m_rgRedLightRect[iIndex++] = rectRedLight;
			}
		}
		else
		{
			if (m_rgLightType[i].tlsStatus == TLS_RED)
			{
				HV_RECT rectRedLight;
				int iHeight = m_rgLightRect[m_rgLightType[i].iTeam].bottom - m_rgLightRect[m_rgLightType[i].iTeam].top;
				int iLightCount = m_rgLightInfo[m_rgLightType[i].iTeam].GetPosCount();

				rectRedLight.left = m_rgLightRect[m_rgLightType[i].iTeam].left;
				rectRedLight.right = m_rgLightRect[m_rgLightType[i].iTeam].right;
				rectRedLight.top = m_rgLightRect[m_rgLightType[i].iTeam].top  + m_rgLightType[i].iPos * iHeight / iLightCount;
				rectRedLight.bottom = rectRedLight.top + iHeight / iLightCount;

				m_rgRedLightRect[iIndex++] = rectRedLight;
			}
		}
	}
	return S_OK;
}
void CTrafficLightImpl::SetSceneStatus(int nPos, int nStatus)
{
	m_rgiSceneStatus[nPos%STATUS_BUF_LEN] = nStatus;
}

int CTrafficLightImpl::GetSceneStatus(int nPos)
{
	return m_rgiSceneStatus[nPos%STATUS_BUF_LEN];
}
