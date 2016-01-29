#include "swpa.h"
#include "SWFC.h"
#include "SWDeamonApplication.h"

CREATE_PROCESS(CSWDeamonApp)
CSWDeamonApp::CSWDeamonApp()
{
	m_dwTotalPID = 0; 
	m_dwLastTime = 0;
	m_fPrint = FALSE;
	m_fDisableDog = TRUE;
	m_fExit = FALSE;
}

CSWDeamonApp::~CSWDeamonApp()
{
	m_clsFile.Close();
}

#ifdef DM8127
BOOL CSWDeamonApp::ResetKeyPressed(VOID)
{
	INT iPressed = 0; 
	if (SWPAR_OK == swpa_device_get_reset_key_event(&iPressed)
		&& 0 == iPressed)
	{
		return TRUE;
	}

	return FALSE;
}


HRESULT CSWDeamonApp::CheckResetKeyEvent(VOID)
{	
	static DWORD dwTick = 0;

	if (ResetKeyPressed())
	{
		if (0 == dwTick)
		{
			dwTick = CSWDateTime::GetSystemTick();
		}
	}
	else if (0 != dwTick)
	{
		DWORD dwCurTick = CSWDateTime::GetSystemTick();
		if (dwCurTick - dwTick >= 5*1000)
		{
			RestoreFactorySetting();
		}

		//reset 
		SW_TRACE_NORMAL("Info: reset device\n");
		while(true)
		{
			Sleep(100000);
		}

		dwTick = 0;
	}

	return S_OK;
}


HRESULT CSWDeamonApp::RestoreFactorySetting(VOID)
{
	CSWFile objAppParamFile;
	const CHAR* szParamFileName[] = 
		{
		"EEPROM/0/APP_PARAM"
		, "EEPROM/0/APP_PARAM_BAK"
		, "EEPROM/0/USER_MANAGER_INFO"
		, "/mnt/nand/DomeCameraSetting.dat" //球机参数
		};
	BYTE bInvalidData[256] = {0x0};

	SW_TRACE_NORMAL("Info: Clear App Param.....\n");

	for (INT iCount=0; iCount < sizeof(szParamFileName)/sizeof(szParamFileName[0]); iCount++)
	{
		if (FAILED(objAppParamFile.Open(szParamFileName[iCount], "w+")))
		{
			SW_TRACE_NORMAL("Err: failed to open %s, and failed to restore App Param\n", szParamFileName[iCount]);
			return E_FAIL;
		}
		else
		{
			if (FAILED(objAppParamFile.Write(bInvalidData, sizeof(bInvalidData))))
			{
				SW_TRACE_NORMAL("Err: failed to write %s, and failed to restore App Param\n", szParamFileName[iCount]);
				return E_FAIL;
			}
			objAppParamFile.Close();
		}
	}

	SW_TRACE_NORMAL("Info: Clear App Param......OK\n");

	if (SWPAR_OK != swpa_tcpip_setinfo("eth0", "100.100.100.101", "255.0.0.0", "100.100.1.1"))
	{
		SW_TRACE_NORMAL("Err: failed to set ip to 100.100.100.101\n");
		return E_FAIL;
	}

	//清除复位记录
	CSWFile cFile;
	if( S_OK == cFile.Open("EEPROM/0/APP_RESET_INFO", "w") )
	{
		DWORD dwSize = 0;
		if (FAILED(cFile.IOCtrl(SWPA_FILE_IOCTL_GET_MAX_SIZE, &dwSize)))
		{
			dwSize = 1024;
			SW_TRACE_NORMAL("Err: failed to get RESET_INFO file size, use %d!\n", dwSize);
		}

		BYTE* pbBuf = (BYTE*)swpa_mem_alloc(dwSize);
		if (NULL == pbBuf)
		{
			SW_TRACE_NORMAL("Err: failed to alloc mem\n");
		}
		else
		{
			swpa_memset(pbBuf, 0x0, dwSize);
			if (FAILED(cFile.Write(pbBuf, dwSize)))
			{
				SW_TRACE_NORMAL("Err: failed to clear RESET_INFO\n");
			}
			swpa_mem_free(pbBuf);
		}

		cFile.Close();
	}
	else
	{
		SW_TRACE_NORMAL("Err: failed to open RESET_INFO file!\n");
	}	
	
	SW_TRACE_NORMAL("Info: Restore Factory Setting......OK\n");
	
	return S_OK;
}

#endif

HRESULT CSWDeamonApp::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
	if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
	{
		return E_FAIL;
	}
	
#ifdef DM8127
	INT iFlag = 0;
	if (SWPAR_OK == swpa_device_get_default_param_flag(&iFlag)
		&& 0 != iFlag)
	{
		RestoreFactorySetting();
		if (SWPAR_OK != swpa_device_clear_default_param_flag())
		{
			SW_TRACE_NORMAL("Err: failed to clear default param flag\n");
		}
	}
#endif

	m_xmlDoc.LoadFile(GetCommandString("-i", "./deamon.xml"));
	if(NULL != m_xmlDoc.RootElement())
	{
		for(TiXmlElement *el = m_xmlDoc.RootElement()->FirstChildElement("Process"); NULL != el; el = el->NextSiblingElement("Process"))
		{
			CSWString strCmdLine;
			strCmdLine.Format("%s %s -f BLOCK//tmp/.running -p %d", el->Attribute("name"), el->Attribute("param") ? el->Attribute("param") : "" , m_dwTotalPID);
			SW_TRACE_NORMAL((LPCSTR)strCmdLine);
			el->SetAttribute("CmdLine", (LPCSTR)strCmdLine);
			el->SetAttribute("Pos", m_dwTotalPID);
			m_dwTotalPID++;
		}
	}
	if(m_dwTotalPID <= 0)
	{
		SW_TRACE_NORMAL("no process to deamon");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSWDeamonApp::ReleaseInstance()
{
	m_clsFile.Close();
	m_fExit = TRUE;
	for(TiXmlElement *el = m_xmlDoc.RootElement()->FirstChildElement("Process"); NULL != el; el = el->NextSiblingElement("Process"))
	{
		if(el->Attribute("PID"))
		{
			int iPID = swpa_atoi(el->Attribute("PID"));
			if(iPID > 0)
			{
				TerminateChildProcess(iPID);
			}
		}
	}

	m_cMonitorThread.Stop();
}

HRESULT CSWDeamonApp::Run()
{
	//先往心跳包文件
	if(E_FAIL == m_clsFile.Open("BLOCK//tmp/.running", "w+"))
	{
		return E_FAIL;
	}

	//启动子进程
	if (FAILED(StartApps()))
	{
		SW_TRACE_ERROR("Start App Failed,So restart system now!\n");
		//启动看门狗,等待复位
		EnableDog();
		while(true)
		{
			Sleep(100000);
		}
	}
	
	//启动看门狗
	EnableDog();	
	
	//监控子进程
	m_cMonitorThread.Start(MonitorProxy, this);
	DWORD dwTickLast = CSWDateTime::GetSystemTick();
	
	for(;;)
	{ 
		if(swpa_utils_file_exist("/tmp/dog.txt"))
		{
			DisableDog();
		}

		if (!m_fExit)
		{
			FeedDog();
			DWORD dwTickNow = CSWDateTime::GetSystemTick();
			if (dwTickNow - dwTickLast > 3000)
			{
				SW_TRACE_NORMAL("Warning: Free Dog time interval %d\n",dwTickNow - dwTickLast);
			}
			dwTickLast = dwTickNow;
		}

#ifdef DM8127
		CheckResetKeyEvent();
#endif
		
		Sleep(1000);
	}
	return S_OK;
}

HRESULT CSWDeamonApp::EnableDog(DWORD dwTimeOut)
{
#ifdef HAS_DOG		
	if (SWPAR_OK != swpa_watchdog_enable(dwTimeOut))
	{
		SW_TRACE_NORMAL("Err: failed to enable watchdog\n");
		return E_FAIL;
	}
	m_fDisableDog = FALSE;
	return S_OK;
#else
	SW_TRACE_NORMAL("EnableDog failed because of has not defined HAS_DOG\n");
	return E_FAIL;	
#endif	
}

HRESULT CSWDeamonApp::FeedDog(VOID)
{
#ifdef HAS_DOG		
	if(!m_fDisableDog)
	{
		swpa_watchdog_handshake();
		return S_OK;
	}
#else
	return E_FAIL;	
#endif
}

HRESULT CSWDeamonApp::DisableDog(VOID)
{
#ifdef HAS_DOG		
	if(!m_fDisableDog)
	{
		swpa_watchdog_disable();
		m_fDisableDog = TRUE;
	}
	return S_OK;
#else
	return E_FAIL;
#endif
}

HRESULT CSWDeamonApp::CheckProcess(TiXmlElement *el, INT *iExitCode)
{
	if(*iExitCode)
	{
		*iExitCode = 0;
	}
	DWORD dwPID = 0;
	if(el && el->Attribute("PID"))
	{
		dwPID = swpa_atoi(el->Attribute("PID"));
	}
	
	if(dwPID > 0)
	{
		//首先检查进程是否存在
		INT iExitCodeTmp;
		if(S_OK == WaitFor(dwPID, 800, &iExitCodeTmp))
		{
			SW_TRACE_ERROR("process %s is exited, kill code:0x%x,exit code:0x%x", (LPCSTR)el->Attribute("CmdLine"), (iExitCodeTmp & 0xFF00), (iExitCodeTmp & 0x00FF));
			if(iExitCode)
			{
				*iExitCode = iExitCodeTmp;
			}
			return E_ACCESSDENIED; 
		}
		
		//在检查心跳包是否正常
		INT iPos = swpa_atoi(el->Attribute("Pos"));
		DWORD dwCreateTime = swpa_atoi(el->Attribute("CreateTime"));
		DWORD dwTime = 0;
		DWORD dwInterval = 0;
		if(S_OK == m_clsFile.Seek(iPos*sizeof(DWORD), SEEK_SET) && S_OK == m_clsFile.Read(&dwTime, sizeof(DWORD)))
		{
			dwInterval = CSWDateTime::GetSystemTick();
			dwInterval = dwInterval > dwTime ? dwInterval - dwTime : 0;
		    if(dwCreateTime == dwTime && dwInterval < 120000 || dwCreateTime != dwTime && dwInterval < 30000)
			{
				if(CSWDateTime::GetSystemTick() - m_dwLastTime > 120000)
				{
					m_fPrint = TRUE;
					SW_TRACE_NORMAL("process %s is ok, now=%d, interval=%d\n", el->Attribute("CmdLine"), dwTime, dwInterval);
				}
				return  S_OK;
			}
		}
		
		//心跳包不正常，返回失败
		SW_TRACE_ERROR("process %s is dead, now=%d, heart beat time:%d, run time=%d, interval=%d\n", (LPCSTR)el->Attribute("CmdLine"), CSWDateTime::GetSystemTick(), dwTime, dwTime - dwCreateTime, dwInterval);
		*iExitCode = 0xFF00;
		return E_FAIL;
	}
	return S_OK;
}



HRESULT CSWDeamonApp::StartApps()
{
	for(TiXmlElement *el = m_xmlDoc.RootElement()->FirstChildElement("Process"); NULL != el; el = el->NextSiblingElement("Process"))
	{
		if((!el->Attribute("create") || swpa_atoi(el->Attribute("create")) > 0) && el->Attribute("CmdLine") && el->Attribute("Pos"))
		{ //写心跳包时间
			INT iPos = swpa_atoi(el->Attribute("Pos"));
			DWORD dwNow = CSWDateTime::GetSystemTick();
			m_clsFile.Seek(iPos*sizeof(DWORD), SEEK_SET);
			m_clsFile.Write(&dwNow, sizeof(DWORD));
			//创建进程
			DWORD dwPID = 0;
			if(S_OK == CreateChildProcess(el->Attribute("CmdLine"), &dwPID))
			{
				el->SetAttribute("PID", dwPID);
				el->SetAttribute("CreateTime", dwNow);
			}
			else
			{
				SW_TRACE_ERROR("CreateChildProcess %s failed!\n",el->Attribute("CmdLine"));
				return E_FAIL;
			}
		}
	}

	return S_OK;
}



PVOID CSWDeamonApp::MonitorProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		SW_TRACE_NORMAL("Err: invalid pvArg (NULL)\n");
		return NULL;
	}

	CSWDeamonApp* pThis = (CSWDeamonApp*)pvArg;
	pThis->Monitor();	
}


HRESULT CSWDeamonApp::Monitor()
{
	while (!m_fExit)
	{
		//监控子进程
		for(TiXmlElement *el = m_xmlDoc.RootElement()->FirstChildElement("Process"); NULL != el; el = el->NextSiblingElement("Process"))
		{
			INT iExitCode = 0;
			if(S_OK != CheckProcess(el, &iExitCode))
			{	//异常退出
				BOOL fReStart = TRUE;
				if((iExitCode & 0xFF00))
				{	//复位设备
					if(el->Attribute("error") && 1 == swpa_atoi(el->Attribute("error")))
					{	//通过休眠使得系统复位
						SW_TRACE_ERROR("restart reset device");
						m_fExit = TRUE;
						break;
					}
				}
				//正常退出
				else 
				{
					iExitCode &= 0x00FF;
					//根据退出码执行其他进程
					if(el->FirstChildElement("Exit"))
					{
						for(TiXmlElement *e = el->FirstChildElement("Exit"); NULL != e; e = e->NextSiblingElement("Exit"))
						{
							if(e->Attribute("value") && iExitCode == swpa_atoi(e->Attribute("value")))
							{	//重启其他进程
								if(e->Attribute("Process"))
								{
									for(TiXmlElement *elTmp = m_xmlDoc.RootElement()->FirstChildElement("Process"); NULL != elTmp; elTmp = elTmp->NextSiblingElement("Process"))
									{
										if(!swpa_strcmp(e->Attribute("Process"), elTmp->Attribute("name")))
										{	//写心跳包时间
											INT iPos = swpa_atoi(elTmp->Attribute("Pos"));
											DWORD dwNow = CSWDateTime::GetSystemTick();
											m_clsFile.Seek(iPos*sizeof(DWORD), SEEK_SET);
											m_clsFile.Write(&dwNow, sizeof(DWORD));
											//创建进程
											DWORD dwPID = 0;
											if(S_OK == CreateChildProcess(elTmp->Attribute("CmdLine"), &dwPID))
											{
												el->SetAttribute("PID", 0);
												elTmp->SetAttribute("PID", dwPID);
												elTmp->SetAttribute("CreateTime", dwNow);
												fReStart = FALSE;
											}										
										}
									}
								}
								else
								{
									//先主动执行reboot
									swpa_utils_shell("reboot -f", 0);
									
									//通过休眠使得系统复位
									SW_TRACE_ERROR("restart reset device");
									m_fExit = TRUE;
									fReStart = FALSE;
									break;
								}
							}
						}
					}
				}	
				
				if(fReStart)
				{ 
					//重启进程
					DWORD dwPID = swpa_atoi(el->Attribute("PID"));
					TerminateChildProcess(dwPID);
					
					char szName[255] = "";					
					swpa_utils_path_getfilename((LPCSTR)el->Attribute("name"), szName);
					CSWString strName;
					strName.Format("killall %s", szName);
					swpa_utils_shell((LPCSTR)strName, NULL);
										
					//写心跳包时间
				    INT iPos = swpa_atoi(el->Attribute("Pos"));
				    DWORD dwNow = CSWDateTime::GetSystemTick();
				    m_clsFile.Seek(iPos*sizeof(DWORD), SEEK_SET);
				    m_clsFile.Write(&dwNow, sizeof(DWORD));
					if(S_OK == CreateChildProcess(el->Attribute("CmdLine"), &dwPID))
					{
						SW_TRACE_NORMAL("restart process %s", el->Attribute("CmdLine"));
						el->SetAttribute("PID", dwPID);
						el->SetAttribute("CreateTime", dwNow);
					}
				}			
			}
		}

		if(m_fPrint)
		{
			m_dwLastTime = CSWDateTime::GetSystemTick();
			m_fPrint = FALSE;
		}

		CSWApplication::Sleep(500);
	}

	return S_OK;
}

