/*
 * CSWTriggerOut.cpp
 *
 *  Created on: 2013-7-22
 *      Author: Administrator
 */

#include "SWTriggerOut.h"
#include "swpa_camera.h"

CSWTriggerOut::CSWTriggerOut() : CSWMessage(MSG_TRIGGER_OUT_BEGIN, MSG_TRIGGER_OUT_END)
{
    m_semCount.Create(0, 50);
    m_fIsInited = FALSE;
}

CSWTriggerOut::~CSWTriggerOut()
{
    Stop();
}

HRESULT CSWTriggerOut::Run()
{
    if (m_cParam.fEnableTriggerOut && m_fIsInited == FALSE)
    {
        m_thTriggerPro.Start(OnTriggerOutRun, this);
        m_fIsInited = TRUE;
    }
    return S_OK;
}

HRESULT CSWTriggerOut::Stop()
{
    m_thTriggerPro.Stop();
    m_fIsInited = FALSE;
    return S_OK;
}

void* CSWTriggerOut::OnTriggerOutRun(void* pvParam)
{
    CSWTriggerOut *pThis = (CSWTriggerOut*)pvParam;

    while (S_OK == pThis->m_thTriggerPro.IsValid())
    {
#ifdef DM8127
		DWORD dwParam = 0;
		CSWMessage::SendMessage(MSG_SET_TRIGGEROUT_STATE,(WPARAM)dwParam,0);
#else
		swpa_camera_io_set_flash_param(3, 0, pThis->m_cParam.nTriggerOutNormalStatus, 0, pThis->m_cParam.nCaptureSynOutputType);
#endif
        while (S_OK == pThis->m_thTriggerPro.IsValid() && S_OK != pThis->m_semCount.Pend(1000));
#ifdef DM8127
		dwParam = 1;
		CSWMessage::SendMessage(MSG_SET_TRIGGEROUT_STATE,(WPARAM)dwParam,0);
#else
        swpa_camera_io_set_flash_param(3, 0, !pThis->m_cParam.nTriggerOutNormalStatus, 0, 0);
#endif
        swpa_thread_sleep_ms(pThis->m_cParam.nTriggerOutPlusWidth);
    }

    return 0;
}

HRESULT CSWTriggerOut::InitTrigger(
    BOOL fEnableTriggerOut,
    int nTriggerOutNormalStatus,
    int nTriggerOutPlusWidth,
    int nCaptureSynOutputType)
{
    if (m_fIsInited == TRUE)
    {
        return S_OK ;
    }

    m_cParam.fEnableTriggerOut = fEnableTriggerOut;
    m_cParam.nTriggerOutNormalStatus = nTriggerOutNormalStatus;
    m_cParam.nTriggerOutPlusWidth = nTriggerOutPlusWidth;
		m_cParam.nCaptureSynOutputType = nCaptureSynOutputType;
		if(m_cParam.nCaptureSynOutputType)
		{
			m_cParam.nTriggerOutNormalStatus = 0;
		}
    return Run() ;
}

HRESULT CSWTriggerOut::OnTriggerProcess(WPARAM wParam, LPARAM lParam)
{
    if (m_fIsInited != TRUE)
    {
        return S_OK ;
    }
	
	if (TRUE == m_cParam.fEnableTriggerOut)
	{
		m_semCount.Post();
	}
    
    return S_OK ;
}
