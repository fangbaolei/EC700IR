#ifndef _HV_CHVLISTENCMD_H_
#define _HV_CHVLISTENCMD_H_

#include "HvDeviceUtils.h"
#include "HvDeviceNew.h"
#include "HVAPI_HANDLE_CONTEXT_EX.h"
#include <vector>
//#include <WinSock.h>

typedef struct _HVLITENDATA_  
{
	char szIP[16];
	SOCKET hSocket;
	_HVLITENDATA_():hSocket(INVALID_SOCKET)
	{
		memset(szIP,0,sizeof(szIP));
	}
}HVLITENDATA;


class IHvLitenCMD
{
public:
	virtual ~IHvLitenCMD() {};
	virtual BOOL Start() = 0;
	virtual BOOL Stop() = 0;
	virtual BOOL Close() = 0;
	virtual HRESULT SetCallBack(HVAPI_HANDLE_EX hHandle,PVOID pFun = NULL,PVOID pUserData = NULL) = 0;
	virtual HRESULT SetCallBackEx(HVAPI_HANDLE_EX hHandle,INT iCallBackType,PVOID pFunc = NULL, PVOID pUserData = NULL) = 0;
	virtual HRESULT CloseListenHanle(HVAPI_HANDLE_EX hHandle) = NULL;
	virtual HVAPI_LISTEN_HANDLE_CONTEXT* GetHandlContext(char* szIP) = NULL;
	virtual HVAPI_HANDLE_EX OpenListenHanle(LPCSTR szIp) = NULL;
	static IHvLitenCMD* CreateInstance(int iPort,int iLitenNum=20);
	static IHvLitenCMD* GetInstance();
};


class CHvLitenCMD: public IHvLitenCMD
{
public:
	CHvLitenCMD();
	CHvLitenCMD(int iPort,int iLitenNum);
	virtual ~CHvLitenCMD();
	virtual BOOL Start();
	virtual BOOL Stop();
	virtual BOOL Close();
	virtual HRESULT SetCallBack(HVAPI_HANDLE_EX hHandle,PVOID pFun = NULL,PVOID pUserData = NULL);
	virtual HRESULT SetCallBackEx(HVAPI_HANDLE_EX hHandle,INT iCallBackType,PVOID pFunc = NULL, PVOID pUserData = NULL);
	virtual HRESULT CloseListenHanle(HVAPI_HANDLE_EX hHandle);
	virtual HVAPI_HANDLE_EX OpenListenHanle(LPCSTR szIp);
	virtual HVAPI_LISTEN_HANDLE_CONTEXT* GetHandlContext(char* szIP);
	BOOL Init();
	BOOL DoDealData(SOCKET hSocket);
	//BOOL GetXmlData(char* pRecvBuf,DWORD64* dwTime);
	HVAPI_LISTEN_HANDLE_CONTEXT* OpenHandle(LPCSTR szIP);
	BOOL CloseListenHandle(HVAPI_LISTEN_HANDLE_CONTEXT* handle);
	void ClearOneHandle(HVAPI_LISTEN_HANDLE_CONTEXT* handle);
	//BOOL DoDealData();
	//BOOL CheckIsHistory(HVAPI_LISTEN_HANDLE_CONTEXT* pContext,DWORD trigerID);
private:
	BOOL m_fExit;
	BOOL m_bInit;
	int m_iPort;
	int m_iLitenNum;
	HANDLE m_hThreadRevice;
	HANDLE m_hThreadDeal;
	SOCKET m_hSocket;
	CRITICAL_SECTION m_csConnection;
	std::vector<HVAPI_LISTEN_HANDLE_CONTEXT*> m_verLitenHANDLE;
	HVAPI_CALLBACK_LISTEN_CMD m_pCallFunc;
	HVAPI_CALLBACK_LISTEN_CMDEX m_pCallFuncEx;
	PVOID m_pUserData;
};
//extern IHvLitenCMD* gIHvLitenCMD;

#endif //_HV_CHVLISTENCMD_H_