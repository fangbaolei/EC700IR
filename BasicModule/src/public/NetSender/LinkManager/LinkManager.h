/**
* @file LinkManager.h 
* @brief 链接管理模块
*
* 管理设备的链接，包括主动链接和被动链接\n
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-11
* @version 1.0
*/

#ifndef _LINK_MANAGER_H_
#define _LINK_MANAGER_H_

#include "SWStream.h"
#include "SWTCPSocket.h"
#include "SWThread.h"


typedef HRESULT (*PF_LINK_CALLBACK)(CSWStream * pStream, PVOID pvArg, PVOID pvHandshakeBuf);


class CLinkManager : public CSWObject
{
	CLASSINFO(CLinkManager, CSWObject)
public:

	/*
	* @brief 构造函数
	*/
	CLinkManager();

	/*
	* @brief 析构函数
	*/
	virtual ~CLinkManager();

	/*
	* @brief 初始化函数
	* @param [in] wPassiveLinkPort : 被动链接的端口
	* @param [in] wActiveLinkPort : 主动链接的端口
	* @param [in] pArrayDstIp : 主动链接的IP
	* @param [in] fSafeSaverEnabled : 是否有可靠性存储
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT Initialize(const WORD wLocalPort, const WORD wDstPort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled);
	
	/*
	* @brief 获取网络链接管理模块实例
	* @param [in] wPassiveLinkPort : 被动链接的端口
	* @param [in] wActiveLinkPort : 主动链接的端口
	* @param [in] pArrayDstIp : 主动链接的IP列表
	* @param [in] dwDstIpCount : 主动链接的IP数目
	* @retval NULL : 获取失败
	* @retval CLinkManager单实例指针
	*/
	//static CLinkManager* GetInstance(const WORD wPassiveLinkPort, const WORD wActiveLinkPort,  const CHAR ** pArrayDstIp, const DWORD dwDstIpCount);

	/*
	* @brief 释放一个链接
	* @param [in] pStream : 与该链接关联的流指针
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT ReleaseLink(CSWStream * pStream);

	/*
	* @brief 设置链接创建成功的回调函数
	* @param [in] pfCallback : 回调函数指针
	* @param [in] pvArg : 回调函数参数指针
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT SetLinkCallback(PF_LINK_CALLBACK pfCallback, PVOID pvArg);

	/*
	* @brief 启动主动链接
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT StartActiveLink(VOID);

	/*
	* @brief 停止主动链接
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT StopActiveLink(VOID);

	/*
	* @brief 启动被动链接
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT StartPassiveLink(VOID);

	/*
	* @brief 停止被动链接
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT StopPassiveLink(VOID);

	/*
	* @brief 设置最大链接数
	* @param [in] dwMaxCount : 链接数
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT SetMaxLinkCount(const DWORD dwMaxCount);

	/*
	* @brief 获取最大链接数
	* @retval 非负整数 : 最大链接数
	*/
	DWORD GetMaxLinkCount(VOID);

	/*
	* @brief 获取当前链接数
	* @retval 非负整数 : 最大链接数
	*/
	DWORD GetLinkCount(VOID);

	/*
	* @brief 开启用户登录模式
	* @retval 
	*/
	HRESULT EnableUserLogin(VOID);

	/*
	* @brief 关闭用户登录模式
	* @retval 
	*/
	HRESULT DisableUserLogin(VOID);

	/*
	* @brief 开启身份验证
	* @retval 
	*/
	HRESULT EnableIdentityVerification(VOID);

	/*
	* @brief 关闭身份验证
	* @retval 
	*/
	HRESULT DisableIdentityVerification(VOID);
	

	/*
	* @brief 取链接状态字符串
	* @retval 
	*/
	CSWString GetConnectInfo(VOID);

	/*
	* @brief 更新发送数据的流，用于链接临时断开后重连上来的链接
	* @retval 
	*/
	HRESULT UpdateStream(CSWStream* pOldStream, CSWStream** ppNewStream);

	


private:	

	
	HRESULT SendHandshakeMsg();

	/*
	* @brief 推送链接到设置了回调函数的模块
	* @param [in] pTCPSocket : LinkManager建立的一个TCP链接
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败	
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT PushLink(CSWTCPSocket* pTCPSocket);

	/*
	* @brief 主动链接主函数
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败	
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT OnActiveLink();

	/*
	* @brief 主动链接主函数的静态代理函数
	*/
	static VOID* OnActiveLinkProxy(VOID * pvArg);

	/*
	* @brief 被动链接主函数
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败	
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT OnPassiveLink();

	/*
	* @brief 被动链接主函数的静态代理函数
	*/
	static VOID* OnPassiveLinkProxy(VOID * pvArg);

	/*
	* @brief 判断设备是否已经与特定IP的特定端口连接
	* @param [in] szIp : IP
	* @param [in] wPort : 端口
	* @retval TRUE : 已连接
	* @retval FALSE : 未连接
	*/
	BOOL IsConnected(const CHAR* szIp, const WORD wPort);

	
	HRESULT GetDefaultHandshakeXml(PBYTE* pbBuf, DWORD* pdwLen);

	
	HRESULT ActiveLinkHandshake(CSWTCPSocket * pSockClient);

private:
	
#define _IP_LEN 32

	enum {
		STATE_READY = 0,
		STATE_RUNNING,
		STATE_STOPPED,
	
		STATE_COUNT
	};
	
	typedef struct __LINK_INFO
	{
		CHAR 		szIp[_IP_LEN];
		WORD		wPort;
		CSWStream* 	pStream;
		BOOL		fIsActiveLink;
		CSWStream* 	pPrevStream;
		BOOL		fIsRealTimeLink;
	}_LINK_INFO;

	typedef struct __CALLBACK_INFO
	{
		PF_LINK_CALLBACK 	pfCallback;
		PVOID				pvCallbackArg;
	}_CALLBACK_INFO;
	
	BOOL		m_fInited;
	DWORD		m_dwActiveLinkState;
	DWORD		m_dwPassiveLinkState;
	
	BOOL 		m_fNeedVerification;
	BOOL		m_fUserLogin;

	WORD		m_wLocalPort;
	WORD		m_wDstPort;
	BOOL		m_fSafeSaver;
	CHAR*		m_pszDstIp;
	DWORD		m_dwMaxLinkCount;
	DWORD		m_dwLinkCount;
	
	CSWList<_LINK_INFO*>		m_lstLinks;	
	CSWList<_CALLBACK_INFO*>	m_lstCallbacks;
	CSWMutex*	m_pCallbackLock;
	
	CSWThread*	m_pActiveLinkThread;
	CSWThread*	m_pPassiveLinkThread;	

	// zhaopy
	CSWMutex m_cMutex;
	CSWString m_strConnectInfo;
};


#endif //_LINK_MANAGER_H_

