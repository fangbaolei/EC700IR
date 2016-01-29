// 该文件编码格式必须是WINDOWS-936格式

#include "ImgGatherer.h"
#include "CamyuLinkOpt.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"

using namespace HiVideo;

CVideoGetter_Pci::CVideoGetter_Pci()
        : m_nEddyType(0)
{
}

CVideoGetter_Pci::~CVideoGetter_Pci()
{
    Stop(-1);
}

HRESULT CVideoGetter_Pci::PutOneFrame(IMG_FRAME imgFrame)
{
    if ( 1 == m_nEddyType )
    {
        HV_COMPONENT_IMAGE imgTmp;
        imgFrame.pRefImage->GetImage(&imgTmp);
        if ( imgTmp.nImgType == HV_IMAGE_JPEG )
        {
            // 因为逆时针旋转90度，所以要调换Jpeg图的宽高。
            int iTmp = 0;
            iTmp = imgTmp.iHeight & 0xffff;
            imgTmp.iHeight >>= 16;
            imgTmp.iHeight |= (iTmp << 16);
            imgFrame.pRefImage->SetImageSize(imgTmp);
        }
    }

    return IImgGatherer::PutOneFrame(imgFrame);
}
