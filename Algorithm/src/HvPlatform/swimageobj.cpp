// 该文件编码必须为WINDOWS-936

#include "swimageobj.h"
#include "interface.h"
//#include "trackercallback.h"
//#include "srio.h"
#include "hvsyserr.h"
#include "hvutils.h"
#include "hvcrop.h"
#if (RUN_PLATFORM == PLATFORM_LINUX)
#include "DspLinkMemBlocks.h"
#include "hvthread.h"
using namespace HiVideo;
#endif

// #define LOG_IMAGEOBJ

CPersistentComponentImage::CPersistentComponentImage()
{
    nImgType = HV_IMAGE_YUV_422;
    iWidth = 0;
    iHeight = 0;

    for (int i = 0; i < 3; i++)
    {
        rgImageData[i].addr = NULL;
        rgImageData[i].phys = 0;
        rgImageData[i].len = 0;
        iStrideWidth[i] = 0;
    }
}

CPersistentComponentImage::~CPersistentComponentImage()
{
    Clear();
}

void CPersistentComponentImage::Clear()
{
#ifdef LOG_IMAGEOBJ
    {
        char buf[1000];
        sprintf(buf,
                "CPersistentComponentImage::Clear() this=0x%08X, "
                "pbData[0]=0x%08X, pbData[1]=0x%08X, pbData[2]=0x%08X, "
                "iWidth=%d, iHeight=%d, iStrideWidth[0]=%d\n",
                this,
                pbData[0],
                pbData[1],
                pbData[2],
                iWidth,
                iHeight,
                iStrideWidth[0]
               );
        OutputString(buf, DEBUG_STR_COMM_FILE);
    }
#endif // def LOG_IMAGEOBJ

    m_cFastStack.Clear();
    for (int i = 0; i < 3; i++)
    {
        if (2 == iMemPos)
        {
#ifdef _ARM_APP_
            FreeShareMemOnPool(&rgImageData[i]);
#endif
        }
        rgImageData[i].addr = NULL;
        rgImageData[i].phys = 0;
        rgImageData[i].len = 0;
        iStrideWidth[i] = 0;
    }
    iWidth = 0;
    iHeight = 0;
}

HRESULT CPersistentComponentImage::ClearMemory()
{
    m_cFastStack.Clear();
    for (int i = 0; i < 3; i++)
    {
        if (2 == iMemPos)
        {
#ifdef _ARM_APP_
            FreeShareMemOnPool(&rgImageData[i]);
#endif
        }
        rgImageData[i].addr = NULL;
        rgImageData[i].phys = 0;
        rgImageData[i].len = 0;
    }

    return S_OK;
}
HRESULT CPersistentComponentImage::Create(HV_IMAGE_TYPE nType, int width, int height, int iMemHeap)
{
    Clear();

    nImgType = nType;
    iWidth = width;
    iHeight = height;
    iMemPos = iMemHeap;

    int rgiCompHeight[3] = {0};
    switch (nImgType)
    {
    case HV_IMAGE_BGR:
    case HV_IMAGE_HSV:
    case HV_IMAGE_HSB:
        iStrideWidth[0] = ((iWidth * 3) + 3)& (~3);
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_GRAY:
        iStrideWidth[0] = (iWidth + 3)& (~3);
        iStrideWidth[1] = iStrideWidth[2] = 0;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_BIN:
        iStrideWidth[0] = (((iWidth + 7) >> 3) + 3)& (~3);
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_YUV_411: // 411 是宽度少一半，高度少一半，但是在iStrideWidth上表现不出来
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = iStrideWidth[2] = (iStrideWidth[0] >> 1);
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = rgiCompHeight[2] = (iHeight >> 1);
        break;
    case HV_IMAGE_JPEG:
        if (iWidth * iHeight > 800 * 300)
        {
            iStrideWidth[0] = 1 * 1024 * 1024;  // 1MB
        }
        else
        {
            iStrideWidth[0] = 128 * 1024;  // 128KB
        }
        iWidth = iStrideWidth[0];
        iHeight = 1;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_H264:
#ifdef _CAMERA_PIXEL_500W_
        iStrideWidth[0] = 768 * 1024;  // 768KB
#else
        iStrideWidth[0] = 1024 * 1024;  // 1MB
#endif
        iWidth = iStrideWidth[0];
        iHeight = 1;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_RAW12:
    case HV_IMAGE_YCbYCr:
    case HV_IMAGE_CbYCrY:
        iStrideWidth[0] = ((iWidth << 1) + 3)& (~3);
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_BT1120:
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = iStrideWidth[0];
        iStrideWidth[2] = 0;
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = iHeight;
        rgiCompHeight[2] = 0;
        break;
    case HV_IMAGE_BT1120_UV:
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = iStrideWidth[0];
        iStrideWidth[2] = iStrideWidth[0];
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = iHeight;
        rgiCompHeight[2] = iHeight;
        break;
    case HV_IMAGE_BT1120_FIELD:
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = iStrideWidth[0];
        iStrideWidth[2] = 0;
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = iHeight;
        rgiCompHeight[2] = 0;
        break;
    case HV_IMAGE_BT1120_ROTATE_Y:
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = (iHeight + 3) & (~3);
        iStrideWidth[2] = 0;
        rgiCompHeight[0] = iHeight >> 1;
        rgiCompHeight[1] = iWidth;
        rgiCompHeight[2] = 0;
        break;
    case HV_IMAGE_YUV_420:
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = iStrideWidth[0];
        iStrideWidth[2] = 0;
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = iHeight>>1;
        rgiCompHeight[2] = 0;
        break;
    case HV_IMAGE_YUV_422: // 缺省采用YUV_422，是为了对以前代码的容错
    default:
        iStrideWidth[0] = (iWidth + 3) & (~3);
        iStrideWidth[1] = iStrideWidth[2] = (iStrideWidth[0] >> 1);
        rgiCompHeight[0] = rgiCompHeight[1] = rgiCompHeight[2] = iHeight;
        break;
    }

    for (int i = 0; i < 3; i++)
    {
        if (iStrideWidth[i] > 0 && rgiCompHeight[i] > 0)
        {
#ifdef CHIP_6455
            rgImageData[i].addr = (PBYTE8)m_cFastStack.StackAlloc(iStrideWidth[i] * rgiCompHeight[i], iMemHeap);
            rgImageData[i].phys = (unsigned int)rgImageData[i].addr;
            rgImageData[i].len = iStrideWidth[i] * rgiCompHeight[i];
#elif defined CHIP_6467
            if (2 == iMemHeap)
            {
#ifdef _ARM_APP_
                CreateShareMemOnPool(&rgImageData[i], iStrideWidth[i] * rgiCompHeight[i]);
#endif
            }
            else
            {
                rgImageData[i].addr = (PBYTE8)m_cFastStack.StackAlloc(iStrideWidth[i] * rgiCompHeight[i], iMemHeap);
                rgImageData[i].phys = (unsigned int)rgImageData[i].addr;
                rgImageData[i].len = iStrideWidth[i] * rgiCompHeight[i];
            }
#else
            rgImageData[i].addr = (PBYTE8)m_cFastStack.StackAlloc(iStrideWidth[i] * rgiCompHeight[i], iMemHeap);
            rgImageData[i].phys = (unsigned int)rgImageData[i].addr;
            rgImageData[i].len = iStrideWidth[i] * rgiCompHeight[i];
#endif
            if (rgImageData[i].addr == NULL)
            {
                return E_OUTOFMEMORY;
            }
        }
    }

#ifdef LOG_IMAGEOBJ
    {
        char buf[1000];
        sprintf(buf,
                "CPersistentComponentImage::Create() "
                "pbData[0]=0x%08X, pbData[1]=0x%08X, pbData[2]=0x%08X, "
                "iWidth=%d, iHeight=%d, iStrideWidth[0]=%d\n",
                pbData[0],
                pbData[1],
                pbData[2],
                iWidth,
                iHeight,
                iStrideWidth[0]
               );
        OutputString(buf, DEBUG_STR_COMM_FILE);
    }
#endif // def LOG_IMAGEOBJ

    return S_OK;
}

// TODO: 利用中间变量，让这个Assign能够适应自我赋值
HRESULT CPersistentComponentImage::Assign(const HV_COMPONENT_IMAGE& imgInit)
{
    if (imgInit.nImgType != nImgType ||
            imgInit.iWidth != iWidth ||
            imgInit.iHeight != iHeight)
    {
        Clear();
        HRESULT hr = Create(imgInit.nImgType, imgInit.iWidth, imgInit.iHeight);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    int rgiCompWidth[3] = {0};
    int rgiCompHeight[3] = {0};
    switch (nImgType)
    {
    case HV_IMAGE_BGR:
    case HV_IMAGE_HSV:
    case HV_IMAGE_HSB:
        rgiCompWidth[0] = iWidth * 3;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_GRAY:
        rgiCompWidth[0] = iWidth;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_BIN:
        rgiCompWidth[0] = ((iWidth + 7) >> 3);
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_YUV_411: // 411 是宽度少一半，高度少一半，但是在iStrideWidth上表现不出来。
        rgiCompWidth[0] = iWidth;
        rgiCompWidth[1] = rgiCompWidth[2] = (iWidth >> 1);
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = rgiCompHeight[2] = (iHeight >> 1);
        break;
    case HV_IMAGE_JPEG:
        rgiCompWidth[0] = iWidth;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_YCbYCr:
    case HV_IMAGE_CbYCrY:
        rgiCompWidth[0] = iWidth * 2;
        rgiCompHeight[0] = iHeight;
        break;
	case HV_IMAGE_YUV_422: // 缺省采用YUV_422，是为了对以前代码的容错。
	default:
        rgiCompWidth[0] = iWidth;
        rgiCompWidth[1] = rgiCompWidth[2] = (iWidth >> 1);
        rgiCompHeight[0] = rgiCompHeight[1] = rgiCompHeight[2] = iHeight;
        break;
    }

    for (int i = 0; i < 3; i++)
    {
        if (rgImageData[i].addr != NULL)
        {
            for (int j = 0; j < rgiCompHeight[i]; j++)
            {
#ifdef _DSP_
                PBYTE8 pDesAddr = (PBYTE8)rgImageData[i].phys;
                PBYTE8 pSrcAddr = (PBYTE8)imgInit.rgImageData[i].phys;
#else
                PBYTE8 pDesAddr = (PBYTE8)rgImageData[i].addr;
                PBYTE8 pSrcAddr = (PBYTE8)imgInit.rgImageData[i].addr;
#endif
                memcpy(pDesAddr + j * iStrideWidth[i], pSrcAddr + j * imgInit.iStrideWidth[i], rgiCompWidth[i]);
            }
        }
    }

    return S_OK;
}


// 该函数返回后直接转为YUV422类型
HRESULT CPersistentComponentImage::CropAssign(const HV_COMPONENT_IMAGE& imgInit, HV_RECT& rcCrop)
{
	// 目前只支持HV_IMAGE_BT1120_ROTATE_Y类型
	if (imgInit.nImgType != HV_IMAGE_BT1120_ROTATE_Y || nImgType != HV_IMAGE_YUV_422)
	{
		return S_FALSE;
	}
#ifdef _DSP_
    PBYTE8 pDesAddrY = (PBYTE8)rgImageData[0].phys;
    PBYTE8 pSrcAddrY = (PBYTE8)imgInit.rgImageData[0].phys;
	PBYTE8 pDesAddrU = (PBYTE8)rgImageData[1].phys;
    PBYTE8 pSrcAddrUV = (PBYTE8)imgInit.rgImageData[1].phys;
	PBYTE8 pDesAddrV = (PBYTE8)rgImageData[2].phys;
#else
    PBYTE8 pDesAddrY = (PBYTE8)rgImageData[0].addr;
    PBYTE8 pSrcAddrY = (PBYTE8)imgInit.rgImageData[0].addr;
	PBYTE8 pDesAddrU = (PBYTE8)rgImageData[1].addr;
    PBYTE8 pSrcAddrUV = (PBYTE8)imgInit.rgImageData[1].addr;
	PBYTE8 pDesAddrV = (PBYTE8)rgImageData[2].addr;
#endif
	// Copy Y分量
	PBYTE8 pDesLineY = pDesAddrY;
	PBYTE8 pSrcLineY = pSrcAddrY;
	pSrcLineY += (imgInit.iStrideWidth[0] * rcCrop.top + rcCrop.left);
	for (int i = 0; i < iHeight; i++)
	{
		memcpy(pDesLineY, pSrcLineY, iStrideWidth[0]);
		pDesLineY += iStrideWidth[0];
		pSrcLineY += imgInit.iStrideWidth[0];
	}

	// Copy 并Rotate UV分量
	int iSrcStrideWidthUV = imgInit.iStrideWidth[1];
	int iSrcWidthUV = imgInit.iHeight * 2;
	for (int i = 0; i < iHeight; i++)
	{
		PBYTE8 pDesLineU = pDesAddrU + i * iStrideWidth[1];
		PBYTE8 pDesLineV = pDesAddrV + i * iStrideWidth[2];
		for (int j = 0; j < iWidth; j+=2)
		{
			// 坐标变换
			int x = j + rcCrop.left;
			int y = (i + rcCrop.top) * 2;
			int xSrc = iSrcWidthUV - y;
			int ySrc = x;
			int iOffSet = iSrcStrideWidthUV * ySrc + xSrc;
			iOffSet &= ~1;
			*pDesLineU = pSrcAddrUV[iOffSet];
			*pDesLineV = pSrcAddrUV[iOffSet + 1];
			pDesLineU++;
			pDesLineV++;
		}
	}

    return S_OK;
}


HRESULT CPersistentComponentImage::Convert(const HV_COMPONENT_IMAGE& imgSrc)
{
    if (nImgType != HV_IMAGE_YUV_422 ||
            iWidth != imgSrc.iWidth ||
            iHeight != imgSrc.iHeight)
    {
        return E_INVALIDARG;
    }
    if (imgSrc.nImgType == HV_IMAGE_YUV_422)
    {
        return Assign(imgSrc);
    }
    if (imgSrc.nImgType == HV_IMAGE_YCbYCr)
    {
#ifdef _DSP_
        PBYTE8 pbY = (PBYTE8)rgImageData[0].phys;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].phys;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].phys;
        PBYTE8 pbSrc = (PBYTE8)imgSrc.rgImageData[0].phys;
#else
        PBYTE8 pbY = (PBYTE8)rgImageData[0].addr;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].addr;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].addr;
        PBYTE8 pbSrc = (PBYTE8)imgSrc.rgImageData[0].addr;
#endif
        for (int i = 0; i < iHeight; i++)
        {
            PBYTE8 pbYTemp = pbY;
            PBYTE8 pbCbTemp = pbCb;
            PBYTE8 pbCrTemp = pbCr;
            PBYTE8 pbSrcTemp = pbSrc;
            for (int j = 0; j < iWidth; j+=2, pbYTemp+=2, pbCrTemp++, pbCbTemp++, pbSrcTemp+=4)
            {
                *pbYTemp = pbSrcTemp[0];
                *(pbYTemp+1) = pbSrcTemp[2];

                *pbCbTemp = pbSrcTemp[1];
                *pbCrTemp = pbSrcTemp[3];
            }
            pbY += iStrideWidth[0];
            pbCb += iStrideWidth[1];
            pbCr += iStrideWidth[2];
            pbSrc += imgSrc.iStrideWidth[0];
        }
        return S_OK;
    }

    if (imgSrc.nImgType == HV_IMAGE_CbYCrY)
    {
#ifdef _DSP_
        PBYTE8 pbY = (PBYTE8)rgImageData[0].phys;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].phys;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].phys;
        PBYTE8 pbSrc = (PBYTE8)imgSrc.rgImageData[0].phys;
#else
        PBYTE8 pbY = (PBYTE8)rgImageData[0].addr;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].addr;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].addr;
        PBYTE8 pbSrc = (PBYTE8)imgSrc.rgImageData[0].addr;
#endif
        for (int i = 0; i < iHeight; i++)
        {
            PBYTE8 pbYTemp = pbY;
            PBYTE8 pbCbTemp = pbCb;
            PBYTE8 pbCrTemp = pbCr;
            PBYTE8 pbSrcTemp = pbSrc;
            for (int j = 0; j < iWidth; j+=2, pbYTemp+=2, pbCrTemp++, pbCbTemp++, pbSrcTemp+=4)
            {
                *pbYTemp = pbSrcTemp[1];
                *(pbYTemp+1) = pbSrcTemp[3];

                *pbCbTemp = pbSrcTemp[0];
                *pbCrTemp = pbSrcTemp[2];
            }
            pbY += iStrideWidth[0];
            pbCb += iStrideWidth[1];
            pbCr += iStrideWidth[2];
            pbSrc += imgSrc.iStrideWidth[0];
        }
        return S_OK;
    }

    if (imgSrc.nImgType == HV_IMAGE_BT1120 || imgSrc.nImgType == HV_IMAGE_BT1120_FIELD)
    {
#ifdef _DSP_
        PBYTE8 pbY = (PBYTE8)rgImageData[0].phys;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].phys;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].phys;
        PBYTE8 pbSrcY = (PBYTE8)imgSrc.rgImageData[0].phys;
        PBYTE8 pbSrcUV = (PBYTE8)imgSrc.rgImageData[1].phys;
#else
        PBYTE8 pbY = (PBYTE8)rgImageData[0].addr;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].addr;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].addr;
        PBYTE8 pbSrcY = (PBYTE8)imgSrc.rgImageData[0].addr;
        PBYTE8 pbSrcUV = (PBYTE8)imgSrc.rgImageData[1].addr;
#endif
        for (int i = 0; i < iHeight; i++)
        {
            PBYTE8 pbYTemp = pbY;
            PBYTE8 pbCbTemp = pbCb;
            PBYTE8 pbCrTemp = pbCr;
            PBYTE8 pbSrcTempY = pbSrcY;
            PBYTE8 pbSrcTempUV = pbSrcUV;

            memcpy(pbYTemp, pbSrcTempY, iWidth);
            pbY += iStrideWidth[0];
            pbSrcY += imgSrc.iStrideWidth[0];

            for (int j = 0; j < iWidth; j+=2, pbCrTemp++, pbCbTemp++, pbSrcTempUV+=2)
            {
                *pbCbTemp = pbSrcTempUV[0];
                *pbCrTemp = pbSrcTempUV[1];
            }
            pbCb += iStrideWidth[1];
            pbCr += iStrideWidth[2];
            pbSrcUV += imgSrc.iStrideWidth[1];
        }
        return S_OK;
    }

    if (imgSrc.nImgType == HV_IMAGE_YUV_420)
    {
#ifdef _DSP_
        PBYTE8 pbY = (PBYTE8)rgImageData[0].phys;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].phys;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].phys;
        PBYTE8 pbSrcY = (PBYTE8)imgSrc.rgImageData[0].phys;
        PBYTE8 pbSrcUV = (PBYTE8)imgSrc.rgImageData[1].phys;
#else
        PBYTE8 pbY = (PBYTE8)rgImageData[0].addr;
        PBYTE8 pbCb = (PBYTE8)rgImageData[1].addr;
        PBYTE8 pbCr = (PBYTE8)rgImageData[2].addr;
        PBYTE8 pbSrcY = (PBYTE8)imgSrc.rgImageData[0].addr;
        PBYTE8 pbSrcUV = (PBYTE8)imgSrc.rgImageData[1].addr;
#endif
        for (int i = 0; i < iHeight; i++)
        {
            PBYTE8 pbYTemp = pbY;
            PBYTE8 pbCbTemp = pbCb;
            PBYTE8 pbCrTemp = pbCr;
            PBYTE8 pbSrcTempY = pbSrcY;
            PBYTE8 pbSrcTempUV = pbSrcUV;

            memcpy(pbYTemp, pbSrcTempY, iWidth);
            pbY += iStrideWidth[0];
            pbSrcY += imgSrc.iStrideWidth[0];

            for (int j = 0; j < iWidth; j+=2, pbSrcTempUV+=2)
            {
                *(pbCbTemp++) = pbSrcTempUV[0];
                *(pbCrTemp++) = pbSrcTempUV[1];
            }
            pbCb += iStrideWidth[1];
            pbCr += iStrideWidth[2];
            if( i % 2 != 0 )
            	pbSrcUV += imgSrc.iStrideWidth[1];
        }
        return S_OK;
    }

    if (imgSrc.nImgType == HV_IMAGE_BT1120_ROTATE_Y)
    {
        HV_RECT rcCrop = {0, 0, imgSrc.iWidth, imgSrc.iHeight};
        return CropAssign(imgSrc, rcCrop);
    }

    return E_INVALIDARG;
}

HRESULT CPersistentComponentImage::Detach(CPersistentComponentImage& imgDest)
{
    imgDest.Clear();

    m_cFastStack.Detach(imgDest.m_cFastStack);

    *((HV_COMPONENT_IMAGE *)&imgDest)=*((HV_COMPONENT_IMAGE *)this);
    memset((HV_COMPONENT_IMAGE *)this, 0, sizeof(HV_COMPONENT_IMAGE));

    return S_OK;
}

HRESULT CPersistentComponentImage::Serialize(bool fOut)
{
    // 双板模式已不使用，以下语句屏蔽
    //SerializeData((PBYTE8)this, sizeof(HV_COMPONENT_IMAGE), fOut);
    if (!fOut)
    {
        if (nImgType == HV_IMAGE_JPEG)
        {
            int iTempWidth = iWidth;
            Create(nImgType, iStrideWidth[0], iHeight);
            iWidth = iTempWidth;
        }
        else
        {
            Create(nImgType, iWidth, iHeight);
        }
    }

    int rgiCompWidth[3] = {0};
    int rgiCompHeight[3] = {0};
    switch (nImgType)
    {
    case HV_IMAGE_BGR:
    case HV_IMAGE_HSV:
    case HV_IMAGE_HSB:
        rgiCompWidth[0] = iWidth * 3;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_GRAY:
        rgiCompWidth[0] = iWidth;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_BIN:
        rgiCompWidth[0] = ((iWidth + 7) >> 3);
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_YUV_411: // 411 是宽度少一半，高度少一半，但是在iStrideWidth上表现不出来。
        rgiCompWidth[0] = iWidth;
        rgiCompWidth[1] = rgiCompWidth[2] = (iWidth >> 1);
        rgiCompHeight[0] = iHeight;
        rgiCompHeight[1] = rgiCompHeight[2] = (iHeight >> 1);
        break;
    case HV_IMAGE_JPEG:
        rgiCompWidth[0] = iWidth;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_YCbYCr:
    case HV_IMAGE_CbYCrY:
        rgiCompWidth[0] = iWidth * 2;
        rgiCompHeight[0] = iHeight;
        break;
    case HV_IMAGE_YUV_422: // 缺省采用YUV_422，是为了对以前代码的容错。
    default:
        rgiCompWidth[0] = iWidth;
        rgiCompWidth[1] = rgiCompWidth[2] = (iWidth >> 1);
        rgiCompHeight[0] = rgiCompHeight[1] = rgiCompHeight[2] = iHeight;
        break;
    }

    // 双板模式不使用
    /*
    for (int i = 0; i < 3; i++)
    {
        if (pbData[i] != NULL)
        {
            for (int j = 0; j < rgiCompHeight[i]; j++)
            {
                SerializeData(pbData[i] + j * iStrideWidth[i], rgiCompWidth[i], fOut);
            }
        }
    }
    */
    return S_OK;
}

HRESULT ImageDownSample(
    CPersistentComponentImage& imgDst,
    HV_COMPONENT_IMAGE& imgSrc
)
{
    if (imgSrc.nImgType != HV_IMAGE_YUV_422)
    {
        return E_INVALIDARG;
    }
    int iWidth(imgSrc.iWidth >> 1), iHeight(imgSrc.iHeight >> 1);
    iWidth &= ~1;
    HRESULT hr = imgDst.Create(imgDst.nImgType, iWidth, iHeight);
    if (hr != S_OK)
    {
        return hr;
    }

    PBYTE8 pbDst = NULL;
    PBYTE8 pbSrc = NULL;
    for (int i = 0; i < iHeight; i++)
    {
        for (int j = 0; j < iWidth; j++)
        {
#ifdef _DSP_
            pbDst = (PBYTE8)imgDst.rgImageData[0].phys;
            pbSrc = (PBYTE8)imgSrc.rgImageData[0].phys;
#else
            pbDst = (PBYTE8)imgDst.rgImageData[0].addr;
            pbSrc = (PBYTE8)imgSrc.rgImageData[0].addr;
#endif
            pbDst[i * imgDst.iStrideWidth[0] + j] =
                ((int)pbSrc[2 * i * imgSrc.iStrideWidth[0] + j * 2] +
                 (int)pbSrc[2 * i * imgSrc.iStrideWidth[0] + j * 2 + 1] +
                 (int)pbSrc[(2 * i + 1) * imgSrc.iStrideWidth[0] + j * 2] +
                 (int)pbSrc[(2 * i + 1) * imgSrc.iStrideWidth[0] + j * 2 + 1]) >> 2;
        }
    }

    WORD16 nSrcStride = (imgSrc.iStrideWidth[0]>>1);

    for (int i = 0; i < iHeight; i++)
    {
        for (int j = 0; j < iWidth / 2; j++)
        {
#ifdef _DSP_
            pbDst = (PBYTE8)imgDst.rgImageData[2].phys;
            pbSrc = (PBYTE8)imgSrc.rgImageData[2].phys;
#else
            pbDst = (PBYTE8)imgDst.rgImageData[2].addr;
            pbSrc = (PBYTE8)imgSrc.rgImageData[2].addr;
#endif
            pbDst[(i * (imgDst.iStrideWidth[0] >> 1)) + j] =
                ((int)pbSrc[ 2 * i * nSrcStride + j * 2] +
                 (int)pbSrc[ 2 * i * nSrcStride + j * 2 + 1] +
                 (int)pbSrc[ 2 * i * nSrcStride + nSrcStride + j * 2] +
                 (int)pbSrc[ 2 * i * nSrcStride + nSrcStride + j * 2 + 1]) >> 2;
#ifdef _DSP_
            pbDst = (PBYTE8)imgDst.rgImageData[1].phys;
            pbSrc = (PBYTE8)imgSrc.rgImageData[1].phys;
#else
            pbDst = (PBYTE8)imgDst.rgImageData[1].addr;
            pbSrc = (PBYTE8)imgSrc.rgImageData[1].addr;
#endif
            pbDst[(i * (imgDst.iStrideWidth[0] >> 1)) + j] =
                ((int)pbSrc[ 2 * i *nSrcStride + j * 2] +
                 (int)pbSrc[ 2 * i * nSrcStride + j * 2 + 1] +
                 (int)pbSrc[ 2 * i * nSrcStride + nSrcStride + j * 2] +
                 (int)pbSrc[ 2 * i * nSrcStride + nSrcStride + j * 2 + 1]) >> 2;
        }
    }
    return hr;
}

HRESULT ImageScaleSize(
    CPersistentComponentImage& imgDst,
    HV_COMPONENT_IMAGE& imgSrc
)
{
    HRESULT hr(S_OK);

    if (imgSrc.nImgType != HV_IMAGE_YUV_422 || imgDst.nImgType != HV_IMAGE_YUV_422)
    {
        return E_INVALIDARG;
    }

    PBYTE8 pbDst = NULL;
    PBYTE8 pbSrc = NULL;
#ifdef _DSP_
    pbDst = (PBYTE8)imgDst.rgImageData[0].phys;
    pbSrc = (PBYTE8)imgSrc.rgImageData[0].phys;
#else
    pbDst = (PBYTE8)imgDst.rgImageData[0].addr;
    pbSrc = (PBYTE8)imgSrc.rgImageData[0].addr;
#endif
    RTN_HR_IF_FAILED(ScaleGrey(pbDst, imgDst.iWidth, imgDst.iHeight, imgDst.iStrideWidth[0],
                               pbSrc, imgSrc.iWidth, imgSrc.iHeight, imgSrc.iStrideWidth[0]));

#ifdef _DSP_
    pbDst = (PBYTE8)imgDst.rgImageData[1].phys;
    pbSrc = (PBYTE8)imgSrc.rgImageData[1].phys;
#else
    pbDst = (PBYTE8)imgDst.rgImageData[1].addr;
    pbSrc = (PBYTE8)imgSrc.rgImageData[1].addr;
#endif
    RTN_HR_IF_FAILED(ScaleGrey(pbDst, imgDst.iWidth / 2, imgDst.iHeight, imgDst.iStrideWidth[1],
                               pbSrc, imgSrc.iWidth / 2, imgSrc.iHeight, imgSrc.iStrideWidth[1]));

#ifdef _DSP_
    pbDst = (PBYTE8)imgDst.rgImageData[2].phys;
    pbSrc = (PBYTE8)imgSrc.rgImageData[2].phys;
#else
    pbDst = (PBYTE8)imgDst.rgImageData[2].addr;
    pbSrc = (PBYTE8)imgSrc.rgImageData[2].addr;
#endif
    RTN_HR_IF_FAILED(ScaleGrey(pbDst, imgDst.iWidth / 2, imgDst.iHeight, imgDst.iStrideWidth[2],
                               pbSrc, imgSrc.iWidth / 2, imgSrc.iHeight, imgSrc.iStrideWidth[2]));

    return hr;
}

//=====================================================
// 图象计数接口的实现
class CReferencePersistentComponentImage : public IReferenceComponentImage
{
private:
    CPersistentComponentImage *m_pPersistentImage;
    int m_iRef;					    // 引用计数
#ifdef _ARM_APP_
    HV_SEM_HANDLE m_hSemRef;
#endif

    DWORD32 m_dwFrameNo;		    // 帧编号
    DWORD32 m_dwRefTime;			// 图象取得时的系统时标.
    DWORD32 m_dwFlag;				// 图象取得时的其他标志,其具体意义不详.
    char m_szFrameName[260];		// 帧文件名称

    BOOL m_fIsCaptureImage;
    BOOL m_fIsEmptyImage;
public:
    // 构造函数
    CReferencePersistentComponentImage(
        HV_IMAGE_TYPE nType,
        int nWidth,
        int nHeight,
        DWORD32 dwFrameNo,
        DWORD32 dwRefTime,
        DWORD32 dwFlag,
        LPCSTR lpszFrameName,
        int iMemHeap
    )
            : m_pPersistentImage(NULL)
            , m_iRef(1)
            , m_dwFrameNo(dwFrameNo)
            , m_dwRefTime(dwRefTime)
            , m_dwFlag(dwFlag)
            , m_fIsCaptureImage(FALSE)
            , m_fIsEmptyImage(FALSE)
    {
#ifdef _ARM_APP_
        if (CreateSemaphore(&m_hSemRef, 1, 1) != 0)
        {
            HV_Trace(5, "<CReferencePersistentComponentImage> CreateSemaphore m_hSemRef Failed!\n");
        }
#endif

        if (lpszFrameName != NULL)
        {
            strncpy(m_szFrameName, lpszFrameName, sizeof(m_szFrameName) - 1);
        }
        else
        {
            m_szFrameName[0] = 0;
        }
        m_pPersistentImage = new CPersistentComponentImage();
        if (NULL != m_pPersistentImage)
        {
            HRESULT hr = m_pPersistentImage->Create(nType, nWidth, nHeight, iMemHeap);
            if (FAILED(hr))
            {
                delete m_pPersistentImage;
                m_pPersistentImage = NULL;
            }
            if(m_pPersistentImage)
            {
            	if(!m_pPersistentImage->rgImageData[0].addr && !m_pPersistentImage->rgImageData[0].phys)
            	{
                    delete m_pPersistentImage;
                    m_pPersistentImage = NULL;
            	}
            }
        }
    }
    // 析构函数
    virtual ~CReferencePersistentComponentImage()
    {
        if (m_pPersistentImage)
        {
            delete m_pPersistentImage;
        }
#ifdef _ARM_APP_
        DestroySemaphore(&m_hSemRef);
#endif
    }

    STDMETHOD(GetImage)(HV_COMPONENT_IMAGE *pImage)
    {
        if (m_pPersistentImage)
        {
            *pImage = *m_pPersistentImage;
            return S_OK;
        }
        else
        {
            return E_POINTER;
        }
    }

    DWORD32 GetFrameNo()
    {
        return m_dwFrameNo;
    };
    DWORD32 GetRefTime()
    {
        return m_dwRefTime;
    };
    DWORD32 GetFlag()
    {
        return m_dwFlag;
    };
    char *GetFrameName()
    {
        if (m_szFrameName[0] == 0)
        {
            return NULL;
        }
        else
        {
            return m_szFrameName;
        }
    };

    void SetFrameNo(DWORD32 dwFrameNo)
    {
        m_dwFrameNo = dwFrameNo;
    };
    void SetRefTime(DWORD32 dwRefTime)
    {
        m_dwRefTime = dwRefTime;
    };
    void SetFlag(DWORD32 dwFlag)
    {
        m_dwFlag = dwFlag;
    };
    void SetFrameName(const char *lpszFrameName)
    {
        if (lpszFrameName != NULL)
        {
            strncpy(m_szFrameName, lpszFrameName, sizeof(m_szFrameName) - 1);
        }
        else
        {
            m_szFrameName[0] = 0;
        }
    };

    void AddRef()
    {
#ifdef _ARM_APP_
        SemPend(&m_hSemRef);
        ++m_iRef;
        SemPost(&m_hSemRef);
#else
        ++m_iRef;
#endif
    }

    void Release()
    {
#ifdef _ARM_APP_
        SemPend(&m_hSemRef);
        --m_iRef;
        if (m_iRef <= 0)
        {
            SemPost(&m_hSemRef);
            delete this;
        }
        else
        {
            SemPost(&m_hSemRef);
        }
#else
        --m_iRef;
        if (m_iRef <= 0)
        {
            delete this;
        }
#endif
    }

    // zhaopy
    virtual void ClearMemory()
    {
        if( m_pPersistentImage )
        {
            m_pPersistentImage->ClearMemory();
        }
    }
    void SetImageContent(HV_COMPONENT_IMAGE& img)
    {
        if (m_pPersistentImage)
        {
            CopyComponentImage(m_pPersistentImage, &img); // TODO 此函数需要修改
        }
    }

    void SetImageSize(const HV_COMPONENT_IMAGE& img)
    {
        if (m_pPersistentImage)
        {
            m_pPersistentImage->iStrideWidth[0] = img.iStrideWidth[0];
            m_pPersistentImage->iStrideWidth[1] = img.iStrideWidth[1];
            m_pPersistentImage->iStrideWidth[2] = img.iStrideWidth[2];
            m_pPersistentImage->iWidth = img.iWidth;
            m_pPersistentImage->iHeight = img.iHeight;
        }
    }

    STDMETHOD (Assign)(const HV_COMPONENT_IMAGE &imgInit)
    {
        if (m_pPersistentImage == NULL) return E_POINTER;
        return m_pPersistentImage->Assign(imgInit);
    }

    STDMETHOD (Convert)(const HV_COMPONENT_IMAGE& imgInit)
    {
        if (m_pPersistentImage == NULL) return E_POINTER;
        return m_pPersistentImage->Convert(imgInit);
    }
    STDMETHOD (Attach)(IReferenceComponentImage* pImage)
    {
#ifdef _ARM_APP_
        CReferencePersistentComponentImage* pImg = (CReferencePersistentComponentImage *)pImage;
        if(m_pPersistentImage && pImage && pImg->m_pPersistentImage)
        {
             m_pPersistentImage->Clear();
             *(HV_COMPONENT_IMAGE *)m_pPersistentImage = *(HV_COMPONENT_IMAGE *)pImg->m_pPersistentImage;
             memset((HV_COMPONENT_IMAGE *)pImg->m_pPersistentImage, 0, sizeof(HV_COMPONENT_IMAGE));
             return S_OK;
        }
#endif
        return E_FAIL;
    }
    STDMETHOD (Serialize)(bool fOut)
    {
        int cRef = 0;
        CPersistentComponentImage *pPersistentImage = NULL;
        if (!fOut)
        {
            cRef = m_iRef;
            pPersistentImage = m_pPersistentImage;
        }
//		SerializeData((PBYTE8)this, sizeof(CReferencePersistentComponentImage), fOut);
        if (!fOut)
        {
            m_iRef = cRef;
            if (m_pPersistentImage)
            {
                if (pPersistentImage)
                {
                    m_pPersistentImage = pPersistentImage;
                }
                else
                {
                    m_pPersistentImage = new CPersistentComponentImage();
                }
            }
            else if (pPersistentImage)
            {
                delete pPersistentImage;
            }
        }
        if (m_pPersistentImage)
        {
            m_pPersistentImage->Serialize(fOut);
        }
        return S_OK;
    }

    BOOL IsCaptureImage()
    {
        return m_fIsCaptureImage;
    }

    void SetCaptureFlag(BOOL fFlag)
    {
        m_fIsCaptureImage = fFlag;
    }

    BOOL IsEmptyImage()
    {
        return m_fIsEmptyImage;
    }
    void SetEmptyFlag(BOOL fFlag)
    {
        m_fIsEmptyImage = fFlag;
    }
};

//=================================================
// 创建接口
HRESULT CreateReferenceComponentImage(
    IReferenceComponentImage **ppReferenceImage,
    HV_IMAGE_TYPE nType,
    int iWidth,
    int iHeight,
    DWORD32 dwFrameNo,
    DWORD32 dwRefTime,
    DWORD32 dwFlag,
    LPCSTR lpszFrameName,
    int iMemHeap
)
{
    if (ppReferenceImage == NULL)
    {
        return E_POINTER;
    }
    if ((nType >= HV_IMAGE_TYPE_COUNT)
            || (iWidth < 0)
            || (iHeight < 0))
    {
        *ppReferenceImage = NULL;
        return E_INVALIDARG;
    }

    // 调用构造函数
    *ppReferenceImage = new CReferencePersistentComponentImage(
        nType, iWidth, iHeight,
        dwFrameNo, dwRefTime, dwFlag,
        lpszFrameName,
        iMemHeap
    );

    if ( (*ppReferenceImage) == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_COMPONENT_IMAGE img;
    if (S_OK != (*ppReferenceImage)->GetImage(&img))
    {
        (*ppReferenceImage)->Release();
        (*ppReferenceImage) = NULL;
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

//=====================================================
// 内存引用计数接口的实现
class CReferenceMemoryImpl : public IReferenceMemory
{
private:
    PBYTE8 m_pbData;
    DSPLinkBuffer m_memData;
    int m_iLen;
    int m_iRef;					// 引用计数
#ifdef _ARM_APP_
    HV_SEM_HANDLE m_hSemRef;
#endif

public:
    // 构造函数
    CReferenceMemoryImpl(
        int iLen
    )
            : m_pbData(NULL)
            , m_iLen(0)
            , m_iRef(1)
    {
        memset(&m_memData, 0, sizeof(DSPLinkBuffer));
        Create(iLen);

#ifdef _ARM_APP_
        if (CreateSemaphore(&m_hSemRef, 1, 1) != 0)
        {
            HV_Trace(5, "<CReferenceMemoryImpl> CreateSemaphore m_hSemRef Failed!\n");
        }
#endif
    }
    // 析构函数
    virtual ~CReferenceMemoryImpl()
    {
        Destroy();

#ifdef _ARM_APP_
        DestroySemaphore(&m_hSemRef);
#endif
    }

    STDMETHOD(GetData)(BYTE8** ppbData)
    {
        if( ppbData == NULL )
        {
            return E_POINTER;
        }
        *ppbData = m_pbData;
        return S_OK;
    }

    void AddRef()
    {
#ifdef _ARM_APP_
        SemPend(&m_hSemRef);
        ++m_iRef;
        SemPost(&m_hSemRef);
#else
        ++m_iRef;
#endif
    }

    void Release()
    {
#ifdef _ARM_APP_
        SemPend(&m_hSemRef);
        --m_iRef;
        if (m_iRef <= 0)
        {
            SemPost(&m_hSemRef);
            delete this;
        }
        else
        {
            SemPost(&m_hSemRef);
        }
#else
        --m_iRef;
        if (m_iRef <= 0)
        {
            delete this;
        }
#endif
    }

private:
    void Create(int iLen)
    {
        if ( m_pbData != NULL ) return;
#ifdef _ARM_APP_
        CreateShareMemOnPool(&m_memData, iLen);
        m_pbData = GetVirtualAddr(&m_memData);
#else
		m_pbData = new unsigned char[iLen];
#endif
        if( m_pbData != NULL ) m_iLen = iLen;
    }

    void Destroy()
    {
       // HV_Trace(5, "<>Destroy, ");
        if( m_pbData == NULL ) return;
#ifdef _ARM_APP_
        FreeShareMemOnPool(&m_memData);
#else
		if( m_pbData != NULL )
		{
			delete[] m_pbData;
			m_pbData = NULL;
		}
#endif
       // HV_Trace(5, "<>Destroy ok\n");
        m_pbData = NULL;
        m_iLen = 0;
    }

    PBYTE8 GetVirtualAddr(DSPLinkBuffer* pBuffer)
    {
        if ( pBuffer == NULL )
        {
            return NULL;
        }
        else
        {
            return (PBYTE8)(pBuffer->addr);
        }
    }
};

//=================================================
// 创建接口
HRESULT CreateReferenceMemory(
    IReferenceMemory **ppReferenceMemory,
    int iLen
)
{
    if (ppReferenceMemory == NULL)
    {
        return E_POINTER;
    }

    *ppReferenceMemory = new CReferenceMemoryImpl(iLen);

    if (*ppReferenceMemory == NULL)
    {
        return E_OUTOFMEMORY;
    }

    BYTE8 *pData = NULL;
    if ((*ppReferenceMemory)->GetData(&pData) != S_OK || pData == NULL)
    {
        (*ppReferenceMemory)->Release();
        (*ppReferenceMemory) = NULL;
        return E_OUTOFMEMORY;
    }

    return S_OK;
}
