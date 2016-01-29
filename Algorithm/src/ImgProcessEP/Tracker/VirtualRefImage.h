#pragma once
#include "swimageobj.h"
#include "DspLinkCmd.h"


// 接口 IReferenceSvImage 
class IVirtualRefImage
{
public:
    static void SetImgMemOperLog(IMG_MEM_OPER_LOG* pLog);

    virtual IReferenceComponentImage* GetImage()=0;

    virtual void AddRef()=0;
    virtual void Release()=0;

    virtual DWORD32 GetFrameNo()=0;
    virtual char *GetFrameName()=0;
    virtual DWORD32 GetRefTime()=0;
    virtual DWORD32 GetFlag()=0;

    virtual void SetFrameNo(DWORD32 dwFrameNo)=0;
    virtual void SetFrameName(const char *lpszFrameName)=0;
    virtual void SetRefTime(DWORD32 dwRefTime)=0;
    virtual void SetFlag(DWORD32 dwFlag)=0;
};

//=====================================================
// 图象计数创建接口数据创建函数
HRESULT CeaeteIVirtualRefImage(
    IVirtualRefImage** ppReferenceImage,
    IReferenceComponentImage* pIRefImage,
    DWORD32 dwFrameNo = 0,
    DWORD32 dwRefTime = 0,
    DWORD32 dwFlag = 0,
    LPSTR lpszFrameName = NULL
    );
