///////////////////////////////////////////////////////////
//  CSWApplication.h
//  Implementation of the Class CSWApplication
//  Created on:      28-二月-2013 14:09:46
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_73A6F873_747D_4169_B684_79143D3C979C__INCLUDED_)
#define EA_73A6F873_747D_4169_B684_79143D3C979C__INCLUDED_
#include <signal.h>
#include "SWObject.h"
#include "SWDateTime.h"
#include "swpa_thread.h"

#define CREATE_PROCESS(cls)\
	cls *theApp = NULL;\
	VOID OnSignal(INT iSignal)\
	{\
		SW_TRACE_DEBUG("receive a signal %d\n", iSignal);\
		if(iSignal != 17 && iSignal != 13)\
			theApp->OnException(iSignal);\
	}\
	INT main( INT argc, const char **argv)\
	{\
		signal(SIGINT, OnSignal);\
		theApp = new cls;\
		CSWApplication *pApp = theApp;\
		if(S_OK == pApp->InitInstance(argc, argv))\
		{\
			pApp->Run();\
			pApp->ReleaseInstance();\
		}\
		pApp->Exit(0);\
		INT iExitCode = pApp->GetExitCode();\
		delete pApp;\
		return iExitCode;\
	}

/**
 * @brief 应用程序的基类
 * 建立应用程序的框架，提供管理应用程序及管理子进程的方法。
 * 所有应用程序都需继承该类。
 */
class CSWApplication : public CSWObject
{
CLASSINFO(CSWApplication,CSWObject)
public:
	/**
	 * @brief 构造函数
	 */
	CSWApplication();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWApplication();
	
	/**
	 *@brief 取得代码的版本号 
	 *@return 返回版本号,失败返回-1
	 */
	static  INT GetVersion(VOID);
	
	/**
	 *@brief 取得DSP代码的版本号 
	 *@return 返回DSP版本号,失败返回-1
	 */
	static  INT GetDSPVersion(VOID);
	
	/**
	 *@brief 程序名
	 *@return 返回程序全路径名
	 */
	static LPCSTR GetExeName(VOID);

	/**
	 * @brief 创建子进程
	 * 
	 * @param [in] szName : 子进程名
	 * @param [out] pdwProcessID : 子进程的ID
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT CreateChildProcess(const CHAR* szName, DWORD* pdwProcessID);
	/**
	 *@brief 判断进程是否已经执行了Exit函数
	 *@return 退出则返回TRUE否则返回FALSE
	 */
	static BOOL IsExited(VOID);
	/**
	 * @brief 进程退出函数
	 * 
	 * @param  [in] sdwExitCode : 退出码
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT Exit(INT iExitCode);
	/**
	 *@brief 获得本进程的退出码
	 *@return 退出码
	 */
	static INT GetExitCode(VOID);
	/**
	 *@brief 心跳包命令
	 *@return 成功返回S_OK其他值代表失败	 
	 */
	static HRESULT HeartBeat(DWORD dwTime = CSWDateTime::GetSystemTick());
	/**
	 * @brief 初始化进程实例
	 * 
	 * @param  [in] wArgc : 进程参数个数
	 * @param  [in] szArgv : 进程参数串
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */	 
	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	/**
	 * @brief 分析并执行命令
	 * 
	 * @param  [in] wArgc : 进程参数个数
	 * @param  [in] szArgv : 进程参数串
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static LPCSTR GetCommandString(LPCSTR szOption, LPCSTR szDefault=NULL);
	/**
	 * @brief 释放实例
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT ReleaseInstance();
	/**
	 * @brief 进程主体函数，子类需重载该函数实现子类自身功能
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Run();
	
	/**
	 * @brief 异常处理函数
	 * @param [in] iSignalNo异常信号
	 */
	virtual HRESULT OnException(INT iSignalNo);
	 
	/**
	 * @brief 进程睡眠
	 * 
	 * @param  [in] dwMS : 睡眠时间，单位为毫秒
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT Sleep(const DWORD dwMS);
	/**
	 * @brief 系统调用函数
	 * 
	 * @param  [in] szCmd : 调用系统执行的命令
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT System(const CHAR* szCmd);
	/**
	 * @brief 终止子进程
	 * 
	 * @param  [in] dwPID : 子进程ID
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT TerminateChildProcess(const DWORD dwPID);
	/**
	 * @brief 等待子进程退出
	 * 
	 * @param  [in] dwPID : 子进程ID
	 * @param  [in] iTimeOut : 等待超时限制，单位为毫秒
     * @param  [out] iExitCode : 退出码
	 * @return
	 * - S_OK : 子进程成功退出
	 * - E_FAIL : 子进程未成功退出，失败。
	 */
	static HRESULT WaitFor(const DWORD& dwPID, const INT& iTimeOut, INT* iExitCode = NULL);
	
	/**
	 * @brief 解析XML
	 * @param [in] pElement xml节点
	 * @param [in] szPath 路径
	 * @return 成功返回该节点的指针，失败返回NULL
	 */
	static TiXmlElement *GetElement(TiXmlElement *pElement, LPCSTR szPath);
protected:
	/**
	 * @brief 引用计数加1
	 */
	virtual DWORD AddRef(){return 0;}

	/**
	 * @brief 引用计数减1，并在计数为0时销毁掉该对象
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Release(){return S_OK;}
	/**
	 *@brief 比较2个key的大小
	 *@param pKey1 键值1
	 *@param pKey1 键值2
	 *@return pKey1 < pKey2,返回-1;pKey1 = pKey2,返回0;pKey1 > pKey2,返回1
	 */
	static INT OnCompare(PVOID pKey1, PVOID pKey2);
private:
	static CSWString m_strExeName;
	static INT  m_iPos;
	static BOOL m_fExit;
	static INT  m_iExitCode;
  static INT  m_iHandle;
};
#endif // !defined(EA_73A6F873_747D_4169_B684_79143D3C979C__INCLUDED_)

