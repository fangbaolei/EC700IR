#pragma once

#include "hvutils.h"
#include "svBase/svBase.h"

#define RECT_SV2HV(rcSv) (*((HV_RECT*)&rcSv))
#define SVRECT_WIDTH(rc) (rc.m_nRight - rc.m_nLeft + 1)
#define SVRECT_HEIGHT(rc) (rc.m_nBottom - rc.m_nTop + 1)

#ifndef ReleaseIReferenceComponentImage
#define ReleaseIReferenceComponentImage(pimg)\
    if (pimg != NULL)\
    {\
        pimg->Release();\
        pimg = NULL;\
    }
#endif

#ifndef RTN_HR_IF_SVFAILED
#define RTN_HR_IF_SVFAILED(fun)\
    {\
        HRESULT __hr = Svresult2Hresult(fun);\
        if(FAILED(__hr))\
        {\
            sv::utTrace("%s l:%d FAILED:%08X, \n", #fun, __LINE__, __hr);\
            return __hr;\
        }\
	}
#endif

#ifndef RTN_SVR_IF_FAILED
#define RTN_SVR_IF_FAILED(func) \
    {   \
        SV_RESULT __sv_hr=func; \
        if (SV_FAILED(__sv_hr)) \
        { \
            sv::utTrace("%s l:%d FAILED:%08X, \n", #func, __LINE__, __sv_hr);\
            return __sv_hr;   \
        }   \
    }
#endif

namespace swTgApp
{

HRESULT Svresult2Hresult(sv::SV_RESULT svrt);

inline sv::SV_IMAGE_TYPE HvimgType2SvimgType(int nHvType)
{
    sv::SV_IMAGE_TYPE svType = sv::SV_IMAGE_UNKNOWN;

    if(nHvType == HV_IMAGE_YUV_422)
    {
        svType = sv::SV_IMAGE_YUV422;
    }
    else if(nHvType == HV_IMAGE_YCbYCr
            || nHvType == HV_IMAGE_CbYCrY
            || nHvType == HV_IMAGE_BT1120
            || nHvType == HV_IMAGE_BT1120_FIELD
            || nHvType == HV_IMAGE_YUV_420)
    {
        svType = (sv::SV_IMAGE_TYPE)(nHvType + 100);
    }

    return svType;
}

/// 设置新算法库的回调
HRESULT SetSVCallBack();

}
