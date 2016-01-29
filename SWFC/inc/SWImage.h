///////////////////////////////////////////////////////////
//  CSWImage.h
//  Implementation of the Class CSWImage
//  Created on:      2013/2/28 14:09:52
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_2FAEEA12_3A58_43d4_AAB6_89C6D92AA196__INCLUDED_)
#define EA_2FAEEA12_3A58_43d4_AAB6_89C6D92AA196__INCLUDED_

#include "SWObject.h"
#include "SWBaseType.h"
#include "SWImageBase.h"
#include "SWMemoryFactory.h"
#include "SWDateTime.h"
#include "SWMutex.h"

#define IMAGE_SECURITY_CODE_LEN (16)	//图片防伪码长度

typedef struct tag_ImageExtInfo
{
	INT iShutter;	// 快门
	INT iGain;		// 增益
	INT iRGain;		
	INT iGGain;
	INT iBGain;
	INT iAvgY;		// 平均亮度
}
IMAGE_EXT_INFO;

/**
 * @brief 图像数据基类
 */

class CSWImage : public CSWObject
{
CLASSINFO(CSWImage,CSWObject)

public:
	CSWImage();
	virtual ~CSWImage();

public:
	 /**
	 * @brief 创建图像
	 * 
	 * @param [in] cImageType : 图像类型
	 * @param [in] dwWidth : 图像宽度
	 * @param [in] dwHeight : 图像高度
	 * @param [in] pMemoryFactory : 内存分配器
	 * @param [in] iMemSize : 指定图片占用内存大小，！！注意，若需要CSWImage类自动根据图片宽高计算所需内存大小，请指定该值为0！（负值等同于0的作用）该值大于0时，CSWImage会根据该值进行内存分配，且只有第一个分量指针有效，后两个分量指针为NULL
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Create(const SW_IMAGE_TYPE& cImageType, INT iWidth, INT iHeight, CSWMemoryFactory* pMemoryFactory, const INT iMemSize = 0);
	/**
	 * @brief 通过cImageSrc来初始对象，如果对象已创建且图像格式或分辨率与pComponentImage不一致会先被清除。
	 * 
	 * @param [in] cImageSrc : 原图像
	 * @param [in] pMemoryFactory : 内存分配器
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Assign(const SW_COMPONENT_IMAGE& cImageSrc, CSWMemoryFactory* pMemoryFactory);
	
	/**
	 * @brief 图像内存的公用
	 * @param [in] pImage 传入一张图像,使得本对象和传入对象公用同一块内存
	 * @return 成功返回S_OK，其他值表示错误代码
	 */
	HRESULT Attach(CSWImage *pImage, PVOID pvExif = NULL, INT iSize = 0);

	/**
	 * @brief 将未降噪图像替换成已经降噪的图像
	 * @param [in] pMemoryFactory : 内存分配器
	 * @return 成功返回S_OK，其他值表示错误代码
	 */
	HRESULT ReplaceFrameBuffer(CSWMemoryFactory* pMemoryFactory);
	
	HRESULT AllocOverlayBuffer(INT iSize);
	BYTE*   GetOverlayBuffer(INT* iSize = NULL);
public:
	/**
	 * @brief 取到图像结构体
	 * 
	 * @param [out] pComponentImage : 图像结构体
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT GetImage(SW_COMPONENT_IMAGE* pComponentImage);

	/**
	 * @brief 取到图像结构体(兼容)
	 * 
	 * @param [out] pComponentImage : 图像结构体
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
//	HRESULT GetImage(HV_COMPONENT_IMAGE* pComponentImage);

	/**
	 * @brief 更新图像结构体
	 * 
	 * @param [in] pComponentImage : 图像结构体
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT SetImage(const SW_COMPONENT_IMAGE& pComponentImage, const INT& iAddrType = 0);
	
	/**
	 * @brief 取到图像占用内存大小
	 * 
	 * @return
	 * - 大于零的数: 成功
	 * - 0 : 失败
	 */
	INT GetSize();	
	/**
	 * @brief 取到图像宽度值
	 * 
	 * @return
	 * - 大于零的数: 成功
	 * - 0 : 失败
	 */
	INT GetWidth();	
	/**
	 * @brief 取到图像高度值
	 * 
	 * @return
	 * - 大于零的数: 成功
	 * - 0 : 失败
	 */
	INT GetHeight();	
	/**
	 * @brief 取到图像类型
	 * 
	 * @return
	 * - 图像类型枚举值
	 */
	SW_IMAGE_TYPE GetType();	
	/**
	 * @brief 取到指定截图区域的图像结构体
	 * 
	 * @param [out] pComponentImage : 图像结构体
	 * @param [in/out] cCorpRect : 要截取的区域
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT GetCropImage(SW_COMPONENT_IMAGE* pComponentImage, SW_RECT& cCorpRect);
	/**
	 * @brief 取到图像数据内存首地址
	 * 
	 * @param [in]  : 地址类型：0: 默认与运行平台有关，1：强制取物理地址，2：强制取虚拟地址。
	 * @return
	 * 内存首地址。
	 */
	PBYTE GetImageBuffer(const INT& iAddrType = 0);
	/**
	 * @brief 取到申请内存的大小
	 * 
	 * @return
	 * 内存大小。
	 */
	DWORD GetImageBufferSize();

public:
	 /**
	 * @brief 图像相关属性接口。
	 */
	DWORD GetFrameNo()
	{
		return m_dwFrameNo;
	};
	LPSTR GetFrameName()
	{
		return m_szFrameName;
	};
	DWORD GetRefTime()
	{
		return m_dwRefTimeMS;
	};

	BOOL IsCaptureImage()
	{
		return m_fIsCaptureImage;
	};
	DWORD GetFlag()
	{
		return m_dwFlag;
	}
	BOOL IsOverlayed()
	{
		return m_fOverlay;
	}
	CSWImage* GetLastImage(VOID)
	{
		return m_pLastImage;
	}
	VOID SetFrameNo(const DWORD& dwFrameNo)
	{
		m_dwFrameNo = dwFrameNo;
	};
	VOID SetFrameName(LPCSTR lpszFrameName)
	{
		m_szFrameName[0] = 0;
		if (lpszFrameName != NULL && strlen(lpszFrameName) < MAX_FRAME_NAME_SIZE)
		{
			strcpy(m_szFrameName, lpszFrameName);
		}
	};
	VOID SetRefTime(const DWORD& dwRefTime)
	{
		m_dwRefTimeMS = dwRefTime;
		CSWDateTime::TimeConvert(m_dwRefTimeMS, &m_dwRealTimeHigh, &m_dwRealTimeLow);
	};
	HRESULT GetRealTime(DWORD* pdwTimeHigh, DWORD* pdwTimeLow)
	{
		if (NULL == pdwTimeHigh || NULL == pdwTimeLow)
		{
			return E_INVALIDARG;
		}

		*pdwTimeHigh = m_dwRealTimeHigh;
		*pdwTimeLow  = m_dwRealTimeLow;

		return S_OK;
	};
	VOID SetRealTime(const DWORD dwTimeHigh, const DWORD  dwTimeLow)
	{
		m_dwRealTimeHigh = dwTimeHigh;
		m_dwRealTimeLow = dwTimeLow;
	};
	VOID SetCaptureFlag(BOOL fIsCapture)
	{
		m_fIsCaptureImage = fIsCapture;
	};
	VOID SetFlag(DWORD dwFlag)
	{
		m_dwFlag = dwFlag;
	}
	VOID SetOverlayFlag(BOOL fOverlay)
	{
		m_fOverlay = fOverlay;
	}
	
	VOID SetLastImage(CSWImage *pImage)
	{
		m_pLastImage = pImage;
		if(m_pLastImage)
		{
			m_pLastImage->AddRef();
		}
	}

	VOID SetImageExtInfo(const IMAGE_EXT_INFO& cInfo)
	{
		swpa_memcpy(&m_cImageExtInfo, &cInfo, sizeof(m_cImageExtInfo));
	}

	HRESULT GetImageExtInfo(IMAGE_EXT_INFO* pInfo)
	{
		if( pInfo == NULL )
		{
			return E_INVALIDARG;
		}
		swpa_memcpy(pInfo, &m_cImageExtInfo, sizeof(m_cImageExtInfo));
		return S_OK;
	}
	
	//保存防伪码
	VOID SetSecurityCode(PBYTE pCode,INT iDataLen)
	{
		if (iDataLen > 0)
			swpa_memcpy(m_pbSecurityCode,pCode,swpa_min(iDataLen,IMAGE_SECURITY_CODE_LEN));
	}
	//获取防伪码
	VOID GetSecurityCode(PBYTE pCode,INT iBufLen)
	{
		if (pCode != NULL && iBufLen > 0)
		{
			swpa_memcpy(pCode,m_pbSecurityCode,swpa_min(iBufLen,IMAGE_SECURITY_CODE_LEN));
		}
	}
	
public:
	 /**
	 * @brief 截取图像,图像数据不进行内存拷贝，只是修改指针指向内容。
	 *
	 * @param [in] cImageSrc : 原图像结构体
	 * @param [in] rcCrop : 要截取的区域
	 * @param [out] pImageDest : 截取的图像
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT CropImage(
		const SW_COMPONENT_IMAGE &cImageSrc,
		SW_RECT& rcCrop,
		SW_COMPONENT_IMAGE *pImageDest
		);
		
	
  /**
	 * @brief 创建图像，并申请必要的资源
	 *
	 * @param [out] ppImage : 图像指针
	 * @param [in] cImageType : 图像类型
	 * @param [in] dwWidth : 图像宽度
	 * @param [in] dwHeight : 图像高度
	 * @param [in] pMemoryFactory : 内存分配器
	 * @param [in] dwFrameNo : 图片帧编号
	 * @param [in] dwRefTime : 图片Tick时间
	 * @param [in] fIsCapture : 抓拍图标志
	 * @param [in] szFrameName : 图片帧名
	 * @param [in] iMemSize : 指定图片占用内存大小，！！注意，若需要CSWImage类自动根据图片宽高计算所需内存大小，请指定该值为0！（负值等同于0的作用）该值大于0时，CSWImage会根据该值进行内存分配，且只有第一个分量指针有效，后两个分量指针为NULL
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT CreateSWImage(
									CSWImage** ppImage
									, const SW_IMAGE_TYPE& cImageType
									, const INT& iWidth
									, const INT& iHeight
									, CSWMemoryFactory* pMemoryFactory
									,	const DWORD& dwFrameNo = 0
									, const DWORD& dwRefTime = 0
									, const BOOL& fIsCapture = false
									, LPCSTR szFrameName  = NULL
									, const INT iMemSize = 0
									);

	static HRESULT CreateSWImage(
									CSWImage** ppImage
									, const SW_IMAGE_TYPE& cImageType
									, const INT& iPitch
									, const INT& iWidth
									, const INT& iHeight
									, CSWMemoryFactory* pMemoryFactory
									, const PVOID* pvAddr
									, const PVOID* pvPhys
									, const INT& iSize
									, const DWORD& dwFrameNo = 0
									, const DWORD& dwRefTime = 0
									, const BOOL& fIsCapture = FALSE
									, LPCSTR szFrameName  = NULL
									);

	 /**
	 * @brief 清除数据
	 *
	 * @return
	 * - S_OK : 转换成功
	 * - E_FAIL : 转换失败
	 */
	HRESULT Clear();

private:
	SW_COMPONENT_IMAGE m_cComponentImage;
	CSWMemory* m_pMemory;
	CSWMemoryFactory* m_pMemoryFactory;
	BOOL m_fIsCreated;

	static const INT MAX_FRAME_NAME_SIZE = 256;
	DWORD m_dwFrameNo;							// 图像帧编号
	DWORD m_dwRefTimeMS;				        // 图像取得时的系统时标.毫秒.
	CHAR m_szFrameName[MAX_FRAME_NAME_SIZE];	// 图像帧文件名称
	BOOL m_fIsCaptureImage;						// 是否是抓拍帧。
	DWORD m_dwFlag;                   //用户自定义标志位
	BOOL m_fOverlay;                  //是否已经字符叠加
	CSWImage* m_pLastImage;

	DWORD m_dwBufferSize;
	IMAGE_EXT_INFO m_cImageExtInfo;
	DWORD m_dwRealTimeHigh;
	DWORD m_dwRealTimeLow;
	
	INT   m_iOverlaySize;
	BYTE* m_pbOverlayBuffer;
	BYTE m_pbSecurityCode[IMAGE_SECURITY_CODE_LEN];

	//降噪替换
	BOOL m_fBeReplaced;	//是否已被替换
	CSWMutex m_cMutexReplace;
};

#endif // !defined(EA_2FAEEA12_3A58_43d4_AAB6_89C6D92AA196__INCLUDED_)

