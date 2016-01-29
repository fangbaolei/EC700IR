#include "SWFC.h"
#include "SWJPEGDecodeTransformFilter.h"

CSWJPEGDecodeTransformFilter::CSWJPEGDecodeTransformFilter():CSWBaseFilter(1,1)
{
	m_lstImage = swpa_list_create();
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
}

CSWJPEGDecodeTransformFilter::~CSWJPEGDecodeTransformFilter()
{
	//clear the callback 
	if (swpa_ipnc_mode() == JPEG_DEC) // 模拟协议才注册
	{
		swpa_ipnc_setcallback(SWPA_LINK_VIDEO, 2, NULL, NULL);
	}
	
	swpa_list_destroy(m_lstImage);
}

HRESULT CSWJPEGDecodeTransformFilter::Run()
{
	if (swpa_ipnc_mode() == JPEG_DEC)
	{
		swpa_ipnc_setcallback(SWPA_LINK_VIDEO, 2, OnResult,this);
	}
	return CSWBaseFilter::Run();
}

HRESULT CSWJPEGDecodeTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage *)obj;
        /*if ( pImage->IsCaptureImage() )
        {
            SW_TRACE_DEBUG("CSWJPEGDecodeTransformFilter got a capture image: id = %d, refTime = %d.\n", pImage->GetFlag(), pImage->GetRefTime());
        }*/

		if(pImage->GetType() == SW_IMAGE_JPEG)
		{
			while(swpa_list_size(m_lstImage) > 5 && GetState() == FILTER_RUNNING)
			{
				CSWApplication::Sleep(100);
			}
			
			pImage->AddRef();
			swpa_list_add(m_lstImage, pImage);
			swpa_ipnc_add_jpg(pImage->GetImageBuffer(), pImage->GetSize());
		}
		else
		{
			GetOut(0)->Deliver(pImage);
		}
	}
	return S_OK;
}

int CSWJPEGDecodeTransformFilter::OnResult(void *pContext, int type, void *struct_ptr)
{
    CSWJPEGDecodeTransformFilter *pThis= (CSWJPEGDecodeTransformFilter *)pContext;
    CSWImage *pImgJPEG = (CSWImage *)swpa_list_remove(pThis->m_lstImage);
	  if(type == CALLBACK_TYPE_IMAGE && NULL != pImgJPEG)
    {
        IMAGE *image = (IMAGE *)struct_ptr;
        CSWImage* pImage = NULL;
        if(S_OK == CSWImage::CreateSWImage(
                    &pImage
                    , SW_IMAGE_YUV420SP
                    , image->pitch
                    , image->width
                    , image->height
                    , CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)
                    , image->data.addr
                    , image->data.phys
                    , image->data.size
                    , 0
                    , 0
                    , FALSE
                    , "NULL"))
      {
      //解码之后知道JPEG的真实的宽高，需要重新赋值正确的宽高，否则web显示会有问题
      	SW_COMPONENT_IMAGE img;
      	pImgJPEG->GetImage(&img);
		img.iWidth = pImage->GetWidth();
		img.iHeight = pImage->GetHeight();
		pImgJPEG->SetImage(img);
		
      	pImage->SetLastImage(pImgJPEG);
      	pImage->SetFrameName(pImgJPEG->GetFrameName());
		pImage->SetFrameNo(pImgJPEG->GetFrameNo());
		pImage->SetRefTime(pImgJPEG->GetRefTime());
		//自动控制模块依赖这些信息
        IMAGE_EXT_INFO cExtInfo;
        pImage->GetImageExtInfo(&cExtInfo);

        cExtInfo.iAvgY = 100;
        cExtInfo.iShutter = 3000;
        cExtInfo.iGain =  100;
        cExtInfo.iRGain =  30;
        cExtInfo.iGGain =  30;
        cExtInfo.iBGain =  30;
        pImage->SetImageExtInfo(cExtInfo);

      	SAFE_RELEASE(pImgJPEG);
      	pThis->GetOut(0)->Deliver(pImage);
      	SAFE_RELEASE(pImage);
      	return 1;
      }
      SAFE_RELEASE(pImgJPEG);
    }
    return 0;
}
