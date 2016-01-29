#include <list>
using namespace std;
#include "hvutils.h"
#include "hvsocket.h"
#include "HvSockUtils.h"
#include "hvthread.h"
#include "hvthreadbase.h"

#define CLASSNAME(clsName) virtual const char* GetName(){ return #clsName;}

class CTestServer : public CHvThreadBase
{
    CLASSNAME(CTestServer)
public:
    CTestServer();
    virtual ~CTestServer();
    bool IsValid(void);
    bool Create(const int iPort);
    void Close(void);
    bool Send(const void* pbData, int iSize);
    bool HasClient(void);
protected:
    virtual HRESULT Run(void* pvParam);
private:
	bool m_fListen;
    int m_iListenPort;
    HV_SOCKET_HANDLE m_shServer;
    list<HV_SOCKET_HANDLE>m_lstClientSocket;
    HV_SEM_HANDLE m_hSemQueCtrl;
};
