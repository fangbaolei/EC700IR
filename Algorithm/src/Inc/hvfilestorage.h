/**
* @file		hvfilestorage.h
* @version	1.0
* @brief	供CParamStore类进行底层读写的夸平台简单流类
*/

#ifndef _FILE_STORAGE_H_
#define _FILE_STORAGE_H_

#include "hvutils.h"
#include "hvinterface.h"

//特别提醒：
//如果是动态分配CFileStorage的，则必须记得释放掉，否则文件不会被保存！
//原因：关闭文件的操作是在该类的析构函数中进行的。

class CFileStorage : public HvCore::IHvStream
{
public:
    enum _MODE
    {
        MODE_READ = 0x00000,
        MODE_WRITE = 0x00001,
        MODE_READWRITE = 0x00002,

        MODE_CREATE = 0x01000
    };

    //IUnknown
    STDMETHOD(QueryInterface)(
        const IID& iid,
        void** ppv
    )
    {
        HRESULT hr = S_OK;
        if ( iid == HvCore::IID_IUnknown ||
                iid == HvCore::IID_IHvStream )
        {
            *ppv = static_cast<IHvStream*>(this);
            AddRef();
        }
        else
        {
            *ppv = NULL;
            hr = E_NOINTERFACE;
        }
        return hr;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return 1;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        return 0;
    }

    //IHvStream
    STDMETHOD(Read)(
        PVOID pv,
        UINT cb,
        PUINT pcbRead
    );

    STDMETHOD(Write)(
        const void* pv,
        UINT cb,
        PUINT pcbWritten
    );

    STDMETHOD(Seek)(
        INT iOffset,
        HvCore::STREAM_SEEK ssOrigin,
        PUINT pnNewPosition
    );

    STDMETHOD(Commit)(
        DWORD32 grfCommitFlags = 0
    );

public:
    CFileStorage();
    virtual ~CFileStorage();

    HRESULT STDMETHODCALLTYPE Initialize(
        const char* szFileName,
        DWORD32 dwFlag = MODE_READWRITE
    );

    HRESULT Close();

private:
    FILE* m_hFile;
};

#endif
