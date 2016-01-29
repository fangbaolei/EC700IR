#include "VirtualRefImage.h"

//#define VIRTUAL_ADD_REF

class CVirtualRefImage : public IVirtualRefImage
{

private:
    IReferenceComponentImage *m_pIRefCompImage;
    static IMG_MEM_OPER_LOG* m_pImgMemOperLog;
    int m_cRef;					// 引用计数

    DWORD32 m_dwFrameNo;		// 帧编号
    DWORD32 m_dwRefTime;			// 图象取得时的系统时标.
    DWORD32 m_dwFlag;				// 图象取得时的其他标志,其具体意义不详.
    char m_szFrameName[260];		//帧文件名称

public:

    // 构造函数
    CVirtualRefImage(
        IReferenceComponentImage* pIRefCompImage,
        DWORD32 dwFrameNo,
        DWORD32 dwRefTime,
        DWORD32 dwFlag,
        LPSTR lpszFrameName
        )
        : m_cRef(1)
        , m_pIRefCompImage(NULL)
        , m_dwFrameNo(dwFrameNo)
        , m_dwRefTime(dwRefTime)
        , m_dwFlag(dwFlag)
    {
        if (lpszFrameName != NULL)
        {
            strncpy(m_szFrameName, lpszFrameName, sizeof(m_szFrameName) - 1);
        }
        else
        {
            m_szFrameName[0] = 0;
        }
        m_pIRefCompImage = pIRefCompImage;

#ifndef VIRTUAL_ADD_REF
        ImageAddRef(m_pImgMemOperLog, m_pIRefCompImage);
#endif
    }

    // 析构函数
    ~CVirtualRefImage()
    {
    }

    static void SetImgMemOperLog(IMG_MEM_OPER_LOG* pLog)
    {
        m_pImgMemOperLog = pLog;
    }

    IReferenceComponentImage* GetImage()
    {
        return m_pIRefCompImage;
    }

    void AddRef() 
    {
#ifndef VIRTUAL_ADD_REF
        ImageAddRef(m_pImgMemOperLog, m_pIRefCompImage);
#endif
        m_cRef++;
    }

    void Release()
    {
#ifndef VIRTUAL_ADD_REF
        ImageRelease(m_pImgMemOperLog, m_pIRefCompImage);
#endif
        m_cRef--;
        if (m_cRef == 0)
        {
            delete this;
        }
    }

    DWORD32 GetFrameNo() {return m_dwFrameNo;};
    DWORD32 GetRefTime() {return m_dwRefTime;};
    DWORD32 GetFlag() {return m_dwFlag;};
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

    void SetFrameNo(DWORD32 dwFrameNo) {m_dwFrameNo = dwFrameNo;};
    void SetRefTime(DWORD32 dwRefTime) {m_dwRefTime = dwRefTime;};
    void SetFlag(DWORD32 dwFlag) {m_dwFlag = dwFlag;};
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

};

IMG_MEM_OPER_LOG* CVirtualRefImage::m_pImgMemOperLog = NULL;

void IVirtualRefImage::SetImgMemOperLog(IMG_MEM_OPER_LOG* pLog)
{
    CVirtualRefImage::SetImgMemOperLog(pLog);
}


HRESULT CeaeteIVirtualRefImage(
    IVirtualRefImage** ppReferenceImage,
    IReferenceComponentImage* pIRefImage,
    DWORD32 dwFrameNo/* = 0*/,
    DWORD32 dwRefTime/* = 0*/,
    DWORD32 dwFlag/* = 0*/,
    LPSTR lpszFrameName/* = NULL*/
    )
{
    *ppReferenceImage = new CVirtualRefImage(pIRefImage, dwFrameNo, dwRefTime, dwFlag, lpszFrameName);
    if (*ppReferenceImage == NULL)
    {
        return E_OUTOFMEMORY;
    }      
    return S_OK;
}