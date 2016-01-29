//*****************************************************
//****注意hv2ipl为调用opencv库测试显示用文件***********
//****对应的cpp和h文件只能在debug编译模式下使用********
//*****************************************************

//#define _OPENCV_DEBUG

#include "swimage.h"

#if defined(_OPENCV_DEBUG) && defined(_DEBUG) && (RUN_PLATFORM != PLATFORM_DSP_BIOS)

#ifndef _HV2IPL_H_
#define _HV2IPL_H_

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

HRESULT HvImageToIplImageBGR(IplImage *pDst, const HV_COMPONENT_IMAGE *pSrc);
HRESULT HvImageToIplImageGrey(IplImage *pDst, const HV_COMPONENT_IMAGE *pSrc);

HRESULT HvImageDebugShow(const HV_COMPONENT_IMAGE *pImg, CvSize csShowWinSize);

#endif

#endif
