///////////////////////////////////////////////////////////
//  SWDspLinkCtrl.cpp
//  Implementation of the Class CSWImage
//  Created on:      
//  Original author: 
///////////////////////////////////////////////////////////
#include "SWFC.h"
#include "DspLinkCmd.h"
#include "SWDspLinkCtrl.h"

/**
* @brief 构造函数
*/
CSWDspLinkCtrl::CSWDspLinkCtrl()
{
	m_pDspLink = NULL;
}	

/**
* @brief 析构函数
*/
CSWDspLinkCtrl::~CSWDspLinkCtrl()
{
}

HRESULT CSWDspLinkCtrl::Initialize(VOID)
{
	m_pDspLink = new CSWFile("DSPLINK/1", "w");
	if( NULL == m_pDspLink )
	{
		return E_OUTOFMEMORY;
	}	
	return CSWBaseLinkCtrl::Initialize();
}

VOID CSWDspLinkCtrl::Clear()
{
	if(NULL != m_pDspLink)
	{
		m_pDspLink->Close();
		delete m_pDspLink;
		m_pDspLink = NULL;
	}
	CSWBaseLinkCtrl::Clear();
}
/**
* @brief 视频流识别初始化接口
* 
* @param [in] cVideoRecogerParam : 视频流识别参数。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWDspLinkCtrl::InitVideoRecoger(INIT_VIDEO_RECOGER_PARAM& cVideoRecogerParam)
{
	cVideoRecogerParam.cTrackerCfgParam.iPlatform = 2;
	return CSWBaseLinkCtrl::InitVideoRecoger(cVideoRecogerParam);
}

/**
* @brief 图片版识别初始化接口
*
* @param [in] cPhotoRecogerParam : 图片版识别参数。
* @return
* - S_OK : 成功
* - E_FAIL : 失败
*/
HRESULT CSWDspLinkCtrl::InitPhotoRecoger(INIT_PHOTO_RECOGER_PARAM& cPhotoRecogerParam)
{
	cPhotoRecogerParam.cTrackerCfgParam.iPlatform = 2;
	return CSWBaseLinkCtrl::InitPhotoRecoger(cPhotoRecogerParam);
}

HRESULT CSWDspLinkCtrl::RegisterCallBack(VOID (*OnDSPAlarm)(PVOID pvParam, INT iInfo), PVOID pvParam)
{
	HRESULT hr = (SWPAR_OK == swpa_device_register_callback(OnDSPAlarm, pvParam) ? S_OK : E_FAIL);
	SW_TRACE_NORMAL("RegisterCallBack(0x%08x,0x%08x) return = 0x%08x.\n", OnDSPAlarm, pvParam, hr);
	return hr;
}

HRESULT CSWDspLinkCtrl::DoProcess(CSWImage* pImage, const INT& iTimeOut)
{
	HRESULT hr = m_pDspLink->IOCtrl(SWPA_FILE_SET_READ_TIMEOUT, (PVOID)(&iTimeOut));
	if( S_OK != hr )
	{
		SW_TRACE_NORMAL("<DspLink IOCtrl failed. 0x%08x>.\n", hr);
		return hr;
	}
	
	hr = m_pDspLink->Write(m_pMemoryPoint->GetBuffer(1), 4, NULL);
	if( S_OK != hr )
	{
		SW_TRACE_NORMAL("<DspLink Write failed. 0x%08x>.\n", hr);
		// DSP超时失败要返回指定的值。
		hr = E_UNEXPECTED;
		return hr;
	}
	return S_OK;
}


