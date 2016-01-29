///////////////////////////////////////////////////////////
//  CSWApplication.cpp
//  Implementation of the Class CSWApplication
//  Created on:      28-二月-2013 14:09:46
//  Original author: zhouy
///////////////////////////////////////////////////////////
#include "swpa.h"
#include "SWFC.h"
#include "SWApplication.h"

CSWString CSWApplication::m_strExeName;
INT  CSWApplication::m_iPos = -1;
BOOL CSWApplication::m_fExit = FALSE;
INT  CSWApplication::m_iExitCode = 0;
INT  CSWApplication::m_iHandle = -1;
/**
 * @brief 构造函数
 */
CSWApplication::CSWApplication(){

}

/**
 * @brief 析构函数
 */
CSWApplication::~CSWApplication(){

}

/**
 *@brief 取得代码的版本号 
 *@return 返回版本号,失败返回-1
 */
INT CSWApplication::GetVersion(VOID)
{
#ifdef VERSION
	return VERSION;
#else
	return -1;
#endif	
}

/**
 *@brief 取得DSP代码的版本号 
 *@return 返回DSP版本号,失败返回-1
 */
INT CSWApplication::GetDSPVersion(VOID)
{
#ifdef DSPVERSION
	return DSPVERSION;
#else
	return -1;
#endif
}

/**
 *@brief 程序名
 *@return 返回程序全路径名
 */
LPCSTR CSWApplication::GetExeName(VOID)
{
	return (LPCSTR)m_strExeName;
}

/**
 * @brief 创建子进程
 * 
 * @param [in] szName : 子进程名
 * @param [out] pdwProcessID : 子进程的ID
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::CreateChildProcess(const CHAR* szName, DWORD* pdwProcessID){

    int attr = 0;


    if ( 0 == swpa_process_create(
        (int*)pdwProcessID,
        &attr,
        (char*)szName,
        NULL) )
    {
        return S_OK;
    }

	return E_FAIL;
}

/**
 *@brief 判断进程是否已经执行了Exit函数
 *@return 退出则返回S_OK，其他值为错误代码
 */
BOOL CSWApplication::IsExited(VOID)
{
	return m_fExit;
}

/**
 * @brief 进程退出函数
 * 
 * @param  [in] sdwExitCode : 退出码
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::Exit(INT sdwExitCode){

		if(!m_fExit)
		{
			m_fExit = TRUE;
    	m_iExitCode = sdwExitCode;
    }
    return S_OK;
}

INT CSWApplication::GetExitCode(VOID)
{
	return m_iExitCode;
}

/**
 *@brief 心跳包命令
 *@return 成功返回S_OK其他值代表失败	 
 */
HRESULT CSWApplication::HeartBeat(DWORD dwTime)
{
	if(-1 != m_iPos)
	{
		CSWFile file;
		if(S_OK == file.Open((LPCSTR)GetCommandString("-f", "BLOCK//tmp/.running"), "w+"))
		{
			file.Seek(m_iPos*sizeof(DWORD), 0);
			file.Write(&dwTime, sizeof(DWORD));
			file.Close();
			return S_OK;
		}
	}
	return E_FAIL;
}

/**
 * @brief 初始化进程实例
 * 
 * @param  [in] wArgc : 进程参数个数
 * @param  [in] szArgv : 进程参数串
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::InitInstance(const WORD wArgc, const CHAR** szArgv){

	m_strExeName = szArgv[0];
	if(-1 == m_iHandle)
	{
		m_iHandle = swpa_map_create(OnCompare);
	}
	INT i = 1;
	while(i < wArgc)
	{
		if(szArgv[i][0] == '-' && i < wArgc -1)
		{			
			swpa_map_add(m_iHandle, new CSWString(szArgv[i]), new CSWString(szArgv[i + 1]));
			if(!swpa_strcmp(szArgv[i], "-p"))
			{
				m_iPos = swpa_atoi(szArgv[i+1]);
			}
			i+= 2;
		}
		else
		{
			i++;
		}
	}
	return S_OK;
}

/**
 *@brief 比较2个key的大小
 *@param pKey1 键值1
 *@param pKey1 键值2
 *@return pKey1 < pKey2,返回-1;pKey1 = pKey2,返回0;pKey1 > pKey2,返回1
 */
INT CSWApplication::OnCompare(PVOID pKey1, PVOID pKey2)
{
	CSWString* strKey1 = (CSWString *)pKey1;
	CSWString* strKey2 = (CSWString *)pKey2;
	return strKey1->Compare(*strKey2);
}

/**
 * @brief 分析并执行命令
 * 
 * @param  [in] wArgc : 进程参数个数
 * @param  [in] szArgv : 进程参数串
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
 
LPCSTR CSWApplication::GetCommandString(LPCSTR szOption, LPCSTR szDefault)
{
	CSWString *strValue = NULL;
	if(-1 != m_iHandle)
	{
		CSWString *strOpt = new CSWString(szOption);
		strValue = (CSWString *)swpa_map_find(m_iHandle, strOpt);
		strOpt->Release();
	}
	return NULL == strValue ? szDefault : (LPCSTR)*strValue;
}

/**
 * @brief 释放实例
 * 
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::ReleaseInstance(){

	if(-1 != m_iHandle)
	{
		int pos = swpa_map_open(m_iHandle);
		if(-1 != pos)
		{
			while(-1 != pos) 
			{
				CSWString *strKey = (CSWString *)swpa_map_key(m_iHandle, pos);
				CSWString *strValue = (CSWString *)swpa_map_value(m_iHandle, pos);
				
				strKey->Release();
				strValue->Release();
				
				pos = swpa_map_next(m_iHandle);
			}
			swpa_map_close(m_iHandle);
		}
	}
	SW_TRACE_NORMAL("pragram exit.\n");
	//exit(m_iExitCode);
	return S_OK;
}

/**
 * @brief 进程主体函数，子类需重载该函数实现子类自身功能
 * 
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::Run(){

	return S_OK;
}

/**
 * @brief 异常处理函数
 * @param [in] iSignalNo异常信号
 */
HRESULT CSWApplication::OnException(INT iSignalNo)
{
	return Exit(iSignalNo);
}

/**
 * @brief 进程睡眠
 * 
 * @param  [in] dwMS : 睡眠时间，单位为毫秒
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::Sleep(const DWORD dwMS){

#ifdef WIN32
    ::Sleep(dwMS);
#else
    swpa_thread_sleep_ms(dwMS);
#endif

	return  S_OK;
}

/**
 * @brief 系统调用函数
 * 
 * @param  [in] szCmd : 调用系统执行的命令
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::System(const CHAR* szCmd){

    int iProcID = 0;
    int iProcAttr = 0;
    int iExitCode = 0;

    if ( 0 == swpa_process_create(
        &iProcID,
        &iProcAttr,
        (char*)szCmd,
        NULL) )
    {
    		WaitFor(iProcID, -1, &iExitCode);
        return !iExitCode ? S_OK : E_FAIL;
    }

	return E_FAIL;
}

/**
 * @brief 终止子进程
 * 
 * @param  [in] dwPID : 子进程ID
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWApplication::TerminateChildProcess(const DWORD dwPID){

	if ( 0 == swpa_process_kill(dwPID) )
    {
        return S_OK;
    }

	return E_FAIL;
}

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
HRESULT CSWApplication::WaitFor(const DWORD& dwPID, const INT& iTimeOut, INT* iExitCode){

		INT kill_code = 0, exit_code = 0;
    if ( 0 == swpa_process_wait(dwPID, iTimeOut, &kill_code, &exit_code) )
    {
    		if(iExitCode)
        {
        	*iExitCode = (kill_code << 8) | (exit_code);
        }
        return S_OK;
    }

	return E_FAIL;
}


TiXmlElement *CSWApplication::GetElement(TiXmlElement *pElement, LPCSTR szPath)
{
	CSWString strTmp;
	for(LPCSTR ch = szPath; pElement && *ch != '\0'; ch++)
	{
		if(*ch != '\\')
		{
			strTmp += *ch;
		}
		else
		{
			pElement = pElement->FirstChildElement((LPCSTR)strTmp);
			strTmp = "";
		}			
	}

	if(!strTmp.IsEmpty() && swpa_strcmp((LPCSTR)strTmp, szPath))
	{
		pElement = pElement->FirstChildElement((LPCSTR)strTmp);
	}
	return pElement;
}

