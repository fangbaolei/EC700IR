#ifndef _SWIMAGEOBJ_INCLUDED__
#define _SWIMAGEOBJ_INCLUDED__

#include "swImage.h"
#include "swWinError.h"
#include "swObjBase.h"
#include "stack_alloc.h"

#define CLASSINFO(cls, par) \
  public: \
  static const char * Class(){ return #cls; } \
  virtual bool InternalIsDescendant(const char * clsName) const {return strcmp(clsName, cls::Class()) == 0 || (strcmp(cls::Class(),par::Class()) && par::InternalIsDescendant(clsName));}


class IReferenceComponentImage
{
	CLASSINFO(IReferenceComponentImage,IReferenceComponentImage)
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
	STDMETHOD(Serialize)(bool fOut) = 0;

	virtual void AddRef()=0;
	virtual void Release()=0;

	virtual void ClearYUV()=0;
	virtual DWORD32 GetFrameNo()=0;
	virtual char *GetFrameName()=0;
	virtual DWORD32 GetRefTime()=0;
	virtual DWORD32 GetFlag()=0;
	virtual IReferenceComponentImage *GetLastRefImage()=0;
	virtual IReferenceComponentImage *GetNextRefImage()=0;

	virtual void SetFrameNo(DWORD32 dwFrameNo)=0;
	virtual void SetFrameName(const char *lpszFrameName)=0;
	virtual void SetRefTime(DWORD32 dwRefTime)=0;
	virtual void SetFlag(DWORD32 dwFlag)=0;
	virtual void SetLastRefImage(IReferenceComponentImage *pLast)=0;
	virtual void SetNextRefImage(IReferenceComponentImage *pNext)=0;

	virtual void SetImageContent(HV_COMPONENT_IMAGE& img)=0;
	virtual void SetImageSize(const HV_COMPONENT_IMAGE& img)=0;
};

class CPersistentComponentImage : public HV_COMPONENT_IMAGE
{
public:
	CPersistentComponentImage();
	~CPersistentComponentImage();

	HRESULT Create(HV_IMAGE_TYPE nType, int width, int height, bool fFastMem = FALSE);
	HRESULT Assign(const HV_COMPONENT_IMAGE& imgInit);
	HRESULT Convert(const HV_COMPONENT_IMAGE& imgSrc);
	HRESULT Detach(CPersistentComponentImage& imgDest);
	HRESULT Serialize(bool fOut);
	
	void Clear();
	void ClearOnlyData();

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
						   HV_COMPONENT_IMAGE *pImageSrc 				//指向原始图象数据.
						   );

//计算矫正角度
HRESULT hvCalcRectifyInfo(HV_COMPONENT_IMAGE &imgSrc, float &fltVAngle, float &fltHAngle);

//=================================================
// 创建接口
HRESULT CreateReferenceComponentImage(
	IReferenceComponentImage **ppReferenceImage,
	HV_IMAGE_TYPE nType = HV_IMAGE_YUV_422,
	int iWidth = 0,
	int iHeight = 0,
	DWORD32 dwFrameNo = 0,
	DWORD32 dwRefTime = 0,
	DWORD32 dwFlag = 0,
	LPSTR lpszFrameName = NULL,
	BOOL fFastMem = FALSE
	);

#endif // _SWIMAGEOBJ_INCLUDED__
