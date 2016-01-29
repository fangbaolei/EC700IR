// 该文件编码格式必须为WINDOWS-936格式

#ifndef _SWIMAGEOBJ_INCLUDED__
#define _SWIMAGEOBJ_INCLUDED__

#include "swimage.h"
#include "swwinerror.h"
#include "swobjbase.h"
#include "stack_alloc.h"

// 图象计数接口
class IReferenceComponentImage
{
public:
    STDMETHOD(GetImage)(
        HV_COMPONENT_IMAGE* pImage
    )=0;
    STDMETHOD (Assign)(
        const HV_COMPONENT_IMAGE& imgInit
    )=0;
    STDMETHOD (Convert)(
        const HV_COMPONENT_IMAGE& imgInit
    )=0;

    STDMETHOD (Attach)(IReferenceComponentImage* pImage) = 0;

    STDMETHOD(Serialize)(bool fOut) = 0;

    virtual void AddRef()=0;
    virtual void Release()=0;
    // zhaopy
    virtual void ClearMemory()=0;

    virtual DWORD32 GetFrameNo()=0;
    virtual char *GetFrameName()=0;
    virtual DWORD32 GetRefTime()=0;
    virtual DWORD32 GetFlag()=0;

    virtual void SetFrameNo(DWORD32 dwFrameNo)=0;
    virtual void SetFrameName(const char *lpszFrameName)=0;
    virtual void SetRefTime(DWORD32 dwRefTime)=0;
    virtual void SetFlag(DWORD32 dwFlag)=0;

    virtual void SetImageContent(HV_COMPONENT_IMAGE& img)=0;
    virtual void SetImageSize(const HV_COMPONENT_IMAGE& img)=0;

    virtual BOOL IsCaptureImage()=0;
    virtual void SetCaptureFlag(BOOL fFlag)=0;
    virtual BOOL IsEmptyImage()=0;
    virtual void SetEmptyFlag(BOOL fFlag)=0;
};

class CPersistentComponentImage : public HV_COMPONENT_IMAGE
{
public:
    CPersistentComponentImage();
    ~CPersistentComponentImage();

    HRESULT Create(HV_IMAGE_TYPE nType, int width, int height, int iMemHeap = 0);
    HRESULT Assign(const HV_COMPONENT_IMAGE& imgInit);
    HRESULT CropAssign(const HV_COMPONENT_IMAGE& imgInit, HV_RECT& rcCrop);
    HRESULT Convert(const HV_COMPONENT_IMAGE& imgSrc);
    HRESULT Detach(CPersistentComponentImage& imgDest);
    HRESULT Serialize(bool fOut);
    BOOL IsEmptyImage();
    void SetEmptyFlag(BOOL fFlag);
    HRESULT ClearMemory();
    void Clear();

private:
    CFastMemAlloc m_cFastStack;
    CPersistentComponentImage& operator=(const CPersistentComponentImage& src);
};

//从HV_COMPONENT_IMAGE
HRESULT ImageDownSample(
    CPersistentComponentImage& imgDst,
    HV_COMPONENT_IMAGE& imgSrc
);
HRESULT ImageScaleSize(
    CPersistentComponentImage& imgDst,
    HV_COMPONENT_IMAGE& imgSrc
);

//切图
HRESULT CropImage(
    const HV_COMPONENT_IMAGE &imgInput,
    HV_RECT& rcCrop,
    HV_COMPONENT_IMAGE *pimgOutput
);

/* 函数说明:该函数用来积分图,对输入的原始图象(亮度)进行积分运算,生成一般积分图和平方积分图. */
HRESULT IntegralPreprocess(
    PDWORD32 pdwItgImage,
    PDWORD32 pdwItgSqImage,
    WORD16 &wItgWidth,
    WORD16 &wItgHeight,
    WORD16 nItgStrideWidth,
    HV_COMPONENT_IMAGE *pImageSrc //指向原始图象数据.
);

//计算矫正角度
HRESULT hvCalcRectifyInfo(HV_COMPONENT_IMAGE &imgSrc, float &fltVAngle, float &fltHAngle);

// 创建接口
HRESULT CreateReferenceComponentImage(
    IReferenceComponentImage **ppReferenceImage,
    HV_IMAGE_TYPE nType = HV_IMAGE_YUV_422,
    int iWidth = 0,
    int iHeight = 0,
    DWORD32 dwFrameNo = 0,
    DWORD32 dwRefTime = 0,
    DWORD32 dwFlag = 0,
    LPCSTR lpszFrameName = NULL,
    int iMemHeap = 1 // 申请内存位置 0:片外内存 1:片内内存 2:共享内存（DM6467）
);

/**
* @brief 得到图像数据地址
* @param
* @return 0 for success or -1 for failure.
*/
inline PBYTE8 GetHvImageData(
    const HV_COMPONENT_IMAGE* pImage,
    const int iDataIndex
)
{
#ifdef _DSP_
    return (PBYTE8)(pImage->rgImageData[iDataIndex]).phys;
#else
    return (PBYTE8)(pImage->rgImageData[iDataIndex]).addr;
#endif
}

/**
* @brief 设置图像数据地址指针
* @param
* @return 0 for success or -1 for failure.
*/
inline void SetHvImageData(
    HV_COMPONENT_IMAGE* pImage,
    const int iDataIndex,
    const PBYTE8 pAddr
)
{
#ifdef _DSP_
    pImage->rgImageData[iDataIndex].phys = (unsigned int)pAddr;
#else
    pImage->rgImageData[iDataIndex].addr = pAddr;
#endif
}


// zhaopy 内存引用计数接口
class IReferenceMemory
{
public:
	STDMETHOD(GetData)(
		BYTE8** ppbData
	)=0;

	virtual void AddRef()=0;
	virtual void Release()=0;
};

// 创建接口
HRESULT CreateReferenceMemory(
    IReferenceMemory **ppReferenceMemory,
    int iLen
);

#endif // _SWIMAGEOBJ_INCLUDED__
