
#include "SWFC.h"
#include "SWMessage.h"
#include "SWDomeCameraCruise.h"

CSWDomeCameraCruise::CSWDomeCameraCruise()
{
	m_dwCruiseID = MAX_CRUISE;
	m_fPaused = FALSE;
	m_fInit = FALSE;
}


CSWDomeCameraCruise::~CSWDomeCameraCruise()
{
	Stop(m_dwCruiseID);
	m_dwCruiseID = MAX_CRUISE;
	m_cThread.Stop();
	m_fInit = FALSE;
}

HRESULT CSWDomeCameraCruise::Set(const DWORD& dwID, const CRUISE_PARAM_STRUCT& sParam)
{
	CHECK_ID(dwID, MAX_CRUISE);

	swpa_memcpy(&GetSetting().Get().sCruiseParam[dwID], &sParam, sizeof(GetSetting().Get().sCruiseParam[dwID]));
	return SaveSetting(dwID);
}


HRESULT CSWDomeCameraCruise::Call(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_CRUISE);

	CRUISE_PARAM_STRUCT sParam;

	Get(dwID, sParam);

	if (!sParam.fValid || 0 >= sParam.iMotionCount)
	{
		SW_TRACE_DEBUG("Err: Cruise #%d has no motion...\n", dwID);
		return E_FAIL;
	}

	//if (MAX_CRUISE > m_dwCruiseID) //每次调用前都先stop一下
	{
		m_dwCruiseID = MAX_CRUISE;
		m_cThread.Stop();
	}

	m_dwCruiseID = dwID;
	m_fPaused = FALSE;
	if (FAILED(m_cThread.Start(CruiseProxy, (PVOID)this)))
	{
		SW_TRACE_DEBUG("Err: failed to call cruise #%d\n", dwID);
		return E_INVALIDARG;
	}
		
	return S_OK;
}



HRESULT CSWDomeCameraCruise::Clear(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_CRUISE);

	Stop(dwID);

	swpa_memset(&GetSetting().Get().sCruiseParam[dwID], 0, sizeof(GetSetting().Get().sCruiseParam[dwID]));
	swpa_snprintf(GetSetting().Get().sCruiseParam[dwID].szName, 31, "巡航扫描%d", dwID);
	GetSetting().Get().sCruiseParam[dwID].fValid = FALSE;
	GetSetting().Commit();
	
	return S_OK;
}


HRESULT CSWDomeCameraCruise::Get(const DWORD& dwID, CRUISE_PARAM_STRUCT& sParam)
{
	CHECK_ID(dwID, MAX_CRUISE);

	swpa_memcpy(&sParam, &GetSetting().Get().sCruiseParam[dwID], sizeof(sParam));

	return S_OK;
}


HRESULT CSWDomeCameraCruise::SaveSetting(const DWORD& dwID)
{
	char szSection[256];
    swpa_sprintf(szSection, "\\DomeCamera\\Cruise\\Path%d", dwID);

	GetSetting().UpdateString(szSection, "Name", GetSetting().Get().sCruiseParam[dwID].szName);
	GetSetting().UpdateInt(szSection, "MotionCount", GetSetting().Get().sCruiseParam[dwID].iMotionCount);
	GetSetting().UpdateInt(szSection, "CheckPT", GetSetting().Get().sCruiseParam[dwID].fCheckPT);
	GetSetting().UpdateInt(szSection, "Valid", GetSetting().Get().sCruiseParam[dwID].fValid);

	for (int i = 0; i < GetSetting().Get().sCruiseParam[dwID].iMotionCount; i++)
	{
		swpa_sprintf(szSection, "\\DomeCamera\\Cruise\\Path%d\\Motion%d", dwID, i);

		GetSetting().UpdateInt(szSection, "MoveType", GetSetting().Get().sCruiseParam[dwID].sMotion[i].iMoveType);
		GetSetting().UpdateInt(szSection, "MoveID", GetSetting().Get().sCruiseParam[dwID].sMotion[i].iMoveID);
		GetSetting().UpdateInt(szSection, "LingerTime", GetSetting().Get().sCruiseParam[dwID].sMotion[i].iLingerTime);
	}
	
	return GetSetting().Commit();
}

HRESULT CSWDomeCameraCruise::Stop(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_CRUISE);

	if (dwID != m_dwCruiseID)
	{
		SW_TRACE_DEBUG("Warning: Cruise #%d is not cruising...\n", dwID);
		return S_OK;
	}

	m_dwCruiseID = MAX_CRUISE;
	//为快速停止，不在这里等待线程退出，留到下次调用或者程序退出时再调用Stop()
	/*if (FAILED(m_cThread.Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop cruise #%d\n", dwID);
		return E_INVALIDARG;
	}*/


	GetPresetInstance().Stop(MAX_PRESET);
	GetFScanInstance().BreakWalk(TRUE);
	
	if (SWPAR_OK != swpa_camera_ball_stop()
		|| FAILED(CSWMessage::SendMessage(MSG_STOP_ZOOM, 0, 1)) /* to SyncZoom*/
		)
	{
		SW_TRACE_DEBUG("Err: failed to stop cruise #%d\n", dwID);
		return E_INVALIDARG;
	}

	CRUISE_PARAM_STRUCT sParam;
	Get(dwID, sParam);
	for (DWORD dwIndex = 0; dwIndex < sParam.iMotionCount; dwIndex++)
	{
		if (MOVETYPE_PRESET == sParam.sMotion[dwIndex].iMoveType)
		{
		    PRESETPOS_PARAM_STRUCT sPresetParam;
			GetPresetInstance().Get(sParam.sMotion[dwIndex].iMoveID, sPresetParam);
			if (/*sParam.fCheckPT && */sPresetParam.fCheckPT)
			{
				CSWMessage::SendMessage(MSG_RECOGNIZE_STOP, 0, 0);
				break;
			}
		}
	}
	
	return S_OK;
}



HRESULT CSWDomeCameraCruise::Pause(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_CRUISE);

	if (m_dwCruiseID != dwID)
	{
		SW_TRACE_DEBUG("Info: cruise #%d is not running(Running #%d)... skip pause cmd\n", dwID, m_dwCruiseID);
		return S_OK;
	}

	m_fPaused = TRUE;

	return S_OK;
}


HRESULT CSWDomeCameraCruise::Resume(const DWORD& dwID)
{
	CHECK_ID(dwID, MAX_CRUISE);

	if (m_dwCruiseID != dwID)
	{
		SW_TRACE_DEBUG("Info: cruise #%d is not running(Running #%d)... skip resume cmd\n", dwID, m_dwCruiseID);
		return S_OK;
	}

	m_fPaused = FALSE;

	return S_OK;
}




HRESULT CSWDomeCameraCruise::Cruise()
{
	DWORD dwID = m_dwCruiseID;
	CRUISE_PARAM_STRUCT sParam;

	SW_TRACE_DEBUG("Info: Cruise #%d started.\n", dwID);
	
	while (MAX_CRUISE > m_dwCruiseID && S_OK == m_cThread.IsValid())
	{
	
		Get(dwID, sParam);

		// 如果是空的直接退出
		if( sParam.iMotionCount <= 0 )
		{
			SW_TRACE_DEBUG("Info: Cruise #%d has no motion.\n", dwID);
			break;
		}

		SW_TRACE_DEBUG("Cruise #%d: iMotionCount = %d\n", dwID, sParam.iMotionCount);

	    for (INT i=0; i<sParam.iMotionCount && MAX_CRUISE > m_dwCruiseID; i++)
    	{
	    	// 是否已经停止
    		if( S_OK != m_cThread.IsValid() )
    		{
    			break;
    		}

			if (m_fPaused)
			{
				if (MAX_CRUISE <= m_dwCruiseID)
				{
					//stopped
					break;
				}
				else
				{
					i--;
					swpa_thread_sleep_ms(100);
					continue;
				}
			}
    		
    		switch (sParam.sMotion[i].iMoveType)
			{
				case MOVETYPE_PRESET:
				{
					SW_TRACE_DEBUG("Cruise MoveType: Preset\n");
					//GetPresetInstance().Call(sParam.sMotion[i].iMoveID);
					CSWMessage::SendMessage(MSG_CALL_PRESET_POS, (WPARAM)sParam.sMotion[i].iMoveID, OP_FROM_PROGRAM);
					PRESETPOS_PARAM_STRUCT sPresetParam;
					GetPresetInstance().Get(sParam.sMotion[i].iMoveID, sPresetParam);
					if (/*sParam.fCheckPT && */sPresetParam.fCheckPT)
					{
						CSWMessage::SendMessage(MSG_RECOGNIZE_CHECKPT, (WPARAM)sParam.sMotion[i].iMoveID, 0);
					}
					
					for (INT iLinger = 0; MAX_CRUISE > m_dwCruiseID && iLinger < sParam.sMotion[i].iLingerTime*10; iLinger++)
					{
						swpa_thread_sleep_ms(100);
						if (MAX_CRUISE <= m_dwCruiseID)
						{
							//stopped
							break;
						}
					}
				}
				break;

				case MOVETYPE_HSCAN:
				{
					//todo : GetHScanInstance().Call(sMotion.iMoveID);
					for (INT iLinger = 0; MAX_CRUISE > m_dwCruiseID && iLinger < sParam.sMotion[i].iLingerTime*2; iLinger++)
					{
						swpa_thread_sleep_ms(500);
					}
				}
				break;

				case MOVETYPE_FSCAN:
				{
					SW_TRACE_DEBUG("Cruise MoveType: Fscan\n");
					FSCAN_PARAM_STRUCT sSetting;
					GetFScanInstance().Get(sParam.sMotion[i].iMoveID,  sSetting);

					SW_TRACE_DEBUG("MoveID: %d, iMotionCount = %d\n", sParam.sMotion[i].iMoveID, sSetting.iMotionCount);
					
					for (INT j=0; j<sSetting.iMotionCount && MAX_CRUISE > m_dwCruiseID; j++)
					{
						// 是否已经停止
			    		if( S_OK != m_cThread.IsValid() )
			    		{
			    			break;
			    		}
						
						SW_TRACE_DEBUG("Cruise Fscan Step %d\n", j);
						GetFScanInstance().BreakWalk(FALSE);
						GetFScanInstance().Walk(sParam.sMotion[i].iMoveID, (DWORD)j);
					}

					for (INT iLinger = 0; MAX_CRUISE > m_dwCruiseID && iLinger < sParam.sMotion[i].iLingerTime*10; iLinger++)
					{
						swpa_thread_sleep_ms(100);
						if (MAX_CRUISE <= m_dwCruiseID)
						{
							//stopped
							break;
						}
					}
				}
				break;

				default:
					SW_TRACE_DEBUG("Info: Cruise #%d MoveType %d is not supported.\n", dwID, sParam.sMotion[i].iMoveType);
					break;
			}
    	}
		swpa_thread_sleep_ms(100);
	}
	

	SW_TRACE_DEBUG("Info: Cruise #%d quit.\n", dwID);

	return S_OK;
}

PVOID CSWDomeCameraCruise::CruiseProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWDomeCameraCruise* pThis = (CSWDomeCameraCruise*)pvArg;

		pThis->Cruise();
	}
}


