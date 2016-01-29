#ifndef _DATACTRLBASE_H_
#define _DATACTRLBASE_H_

#include "config.h"
#include "hvutils.h"
#include "swimageobj.h"
#include "HvList.h"
#include "hvthreadbase.h"
#include "hvsocket.h"
#include "trackerdef.h"

#ifdef SINGLE_BOARD_PLATFORM
#define MAX_CARLEFT_LIST_COUNT 6
#else
#define MAX_CARLEFT_LIST_COUNT 8
#endif

class IDataCtrl
{
public:
    virtual ~IDataCtrl(){};
    virtual HRESULT CarArrive(CARARRIVE_INFO_STRUCT *pCarArriveInfo, LPVOID pvUserData) = 0;
    virtual HRESULT CarLeft(CARLEFT_INFO_STRUCT *pCarLeftInfo, LPVOID pvUserData) = 0;
    virtual float GetFrameRate() = 0;
};

class CCarLeftThread : public CHvThreadBase
{
public:
    CCarLeftThread(IDataCtrl *pDataCtrl);
    ~CCarLeftThread();

    HRESULT IsThreadOK();
    HRESULT PutOneResult(CARLEFT_INFO_STRUCT cCarLeftInfo);
    HRESULT GetOneResult(CARLEFT_INFO_STRUCT *pCarLeftInfo);
    virtual HRESULT Run(void*pvParam);

    virtual const char* GetName()
    {
        static char szName[] =  "CCarLeftThread";
        return szName;
    }

private:
    CHvList<CARLEFT_INFO_STRUCT> m_cCarLeftList;
    IDataCtrl *m_pDataCtrl;
    HV_SEM_HANDLE m_hSemLock;
    HV_SEM_HANDLE m_hSemResultCnt;
    DWORD32 m_dwLastTick;
};

#endif
