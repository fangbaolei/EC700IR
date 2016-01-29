#pragma once
#include "jpegsequence.h"
#include "interface.h"
#include "hvthread.h"

class CCaptureSequence : public CJpegSequence
{
public:
	
	CCaptureSequence(void)
	{
		return;
	}
	virtual ~CCaptureSequence(void)
	{
		return;
	}

	//IPlayControl接口
	STDMETHOD(Pause)()
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetSequenceEditor)(ISequenceEditor **ppSequenceEditor)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(ShowSetup)()
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetCamParameter)(LIGHT_TYPE nLightType)
	{
		return E_NOTIMPL;
	}

	virtual BOOL GetNextJpgFrame(struct JPG_FRAME &jpgframe, HVPOSITION &pCurrent)
	{
		return FALSE;
	}
	virtual BOOL GetPrevJpgFrame(struct JPG_FRAME &jpgframe, HVPOSITION &pCurrent)
	{
		return FALSE;
	}

	virtual bool ThreadIsOk()
	{
		return true;
	}

	HRESULT Process()
	{
		struct JPG_FRAME jpgframe;
		if (!m_pfnCallback)	return S_FALSE;

		while (m_fOpened)
		{
			HV_Sleep(1000);
		}
		memset(&jpgframe, 0, sizeof(jpgframe));
		m_queJpeg.AddTail(jpgframe);
		m_psemJpegQue->Post(HiVideo::ISemaphore::WAIT);

		return S_OK;
	}

	HRESULT PutImage(HV_COMPONENT_IMAGE imgCapture, DWORD32 dwRefTime, int nCamID)
	{
		struct JPG_FRAME jpgframe;
		if (!m_pfnCallback)	return S_FALSE;

		if(imgCapture.nImgType != HV_IMAGE_JPEG) return S_FALSE;

		IReferenceComponentImage *pRefImage = NULL;
		if (FAILED(CreateReferenceComponentImage(
					&pRefImage, 
					HV_IMAGE_JPEG, 
#ifdef CHIP_DM642
					imgCapture.iWidth, 1, 
#else
					2048,1536,
#endif
					0, 0, 0, 
					NULL,
					FALSE
					)))
		{
			HV_Trace("\nCCaptureSequence::Process(): CreateReferenceComponentImage() failed!\n");
			return E_OUTOFMEMORY;
		}
		HV_COMPONENT_IMAGE imgFrame;
		pRefImage->GetImage(&imgFrame);

		if(imgCapture.iWidth > imgFrame.iWidth)
		{
			HV_Trace("\nJpeg size too big\n");
			pRefImage->Release();
			return E_OUTOFMEMORY;
		}

		HV_memcpy(imgFrame.pbData[0], imgCapture.pbData[0], imgCapture.iWidth);
		pRefImage->SetImageSize(imgCapture);

		pRefImage->SetRefTime(dwRefTime);
		jpgframe.pRefImage = pRefImage;
		jpgframe.iVideoID = nCamID;

		//如果添加失败直接返回
		if( 0 == m_queJpeg.AddTail(jpgframe))
		{
			pRefImage->Release();
			return E_FAIL;
		}

		while (m_psemJpegQue->Post() != S_OK)
		{
//			HV_Trace("AddTail\n");
			HV_Sleep(100);
		}

		return S_OK;
	}
};
