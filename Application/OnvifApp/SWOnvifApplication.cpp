/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "SWOnvifApplication.h"
#include "SWPTZParameter.h"



CREATE_PROCESS(CSWOnvifApp);


int CSWOnvifApp::m_UserSize = 0;
UserArray_t CSWOnvifApp::g_psUserlist[8];

CSWOnvifApp::CSWOnvifApp()
	: CSWMessage(MSG_ONVIF_BEGIN,MSG_ONVIF_END),
	m_fEnable(0),
	m_fAuthentEnable(0),
	m_fIsGetParam(FALSE)
{
	swpa_memset(&m_sPTZParam, 0, sizeof(ONVIF_PTZ_PARAM));	
}

CSWOnvifApp::~CSWOnvifApp()
{
}


HRESULT CSWOnvifApp::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
	
	if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
	{
		SW_TRACE_NORMAL("Err: failed to init CSWOnvifApp\n");
		return E_FAIL;
	}

	if (0 != RegisterPTZCallback(PTZControlCallback))
	{
		SW_TRACE_NORMAL("Err: failed to register PTZ callback\n");
		return E_FAIL;
	}

	if (0 != RegisterUserManage(UserManageCallback))
	{
		SW_TRACE_NORMAL("Err: failed to register UserManage callback\n");
		return E_FAIL;
	}

	if (0 != ReggisterGetUser(OnGetUSERInfo))
	{
		SW_TRACE_NORMAL("Err: failed to register GetUser callback\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSWOnvifApp::ReleaseInstance()
{
	if(FAILED(CSWApplication::ReleaseInstance()))
	{
		SW_TRACE_NORMAL("Err: failed to release CSWOnvifApp\n");
		return E_FAIL;
	}

	INT iRet = StopOnvif();

	if (iRet != 0)
	{
		SW_TRACE_NORMAL("Err: failed to stop onvif Service, errcode=%d\n", iRet);
		return E_FAIL;
	}
	else
	{
		SW_TRACE_NORMAL("Info: Onvif Service stopped!\n");
		return S_OK;
	}

	return S_OK;
}

HRESULT CSWOnvifApp::Run()
{
	BOOL fIsReady = TRUE;

	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMac[32] = {0};

	CSWFile cH264ResolutionFile;
	CSWFile cH264ResolutionFile_EX;

	BOOL fPTZInited = FALSE;


	while (!IsExited())
	{	
		if(fIsReady && m_fIsGetParam)
		{	
			if( 0 == swpa_tcpip_getinfo("eth0", szIP, 32, szMask, 32, szGateway, 32, szMac, 32)
				&& 0 != swpa_strcmp("0.0.0.0", szIP) )
			{	
			    swpa_thread_sleep_ms(2000);   //等待2s

		//		m_cMutex.Lock();
				int iAtmp[2] = {0};
				iAtmp[0] = m_fEnable;
				iAtmp[1] = m_fAuthentEnable;
		//		m_cMutex.Unlock();

				SW_TRACE_DEBUG("Run--m_fEnable:%d,m_fAuthentEnable:%d \n ",m_fEnable,m_fAuthentEnable);
				
				INT iRet = StartOnvif(25, 8080, 3702, 554, NULL,iAtmp);
				if (iRet != 0)
				{
					SW_TRACE_NORMAL("Err: failed to start onvif Service, errcode=%d\n", iRet);
					return E_FAIL;
				}
				fIsReady = FALSE;
			}
			else
			{
				SW_TRACE_NORMAL("ONVIF get ip:%s.", szIP);
			}
		}

		if(!m_fIsGetParam)
		{
			//此处休眠是为了让LPRAPP初始化完毕
			swpa_thread_sleep_ms(4000);
			HeartBeat();
			HRESULT hd = SendRemoteMessage(MSG_APP_GET_ONVIF_PARAM, NULL, 0);
            if(hd == S_OK)
			{
				SW_TRACE_DEBUG(" SendRemoteMessage(MSG_APP_GET_ONVIF_PARAM) OK\n");
			}

			continue;
		}

		

		if (!m_sPTZParam.fInited)
		{
			GetPTZInfo();
		}
		else if (!fPTZInited)
		{
			if (0 == SetPTZParam(&m_sPTZParam))
			{
				fPTZInited = TRUE;
			}
		}
		
		if( swpa_utils_file_exist("/tmp/.H264ResolutionInfo") && SUCCEEDED(cH264ResolutionFile.Open("/tmp/.H264ResolutionInfo", "r")) )
		{
			INT iResolution = 0;
			if (FAILED(cH264ResolutionFile.Read(&iResolution, sizeof(iResolution), 0)))
			{
				SW_TRACE_DEBUG("Err: failed to get H.264 resolution info\n");
				cH264ResolutionFile.Close();
			}
			else if (1080 != iResolution && 720 != iResolution)
			{
				SW_TRACE_DEBUG("Err: got invalid H.264 resolution info (%d)\n", iResolution);
				cH264ResolutionFile.Close();
			}
			else
			{
				
				StartOnvif_One(1080 == iResolution ? 1920 : 1280, 1080 == iResolution ? 1080 : 720);

				cH264ResolutionFile.Close();
				swpa_utils_file_delete("/tmp/.H264ResolutionInfo");
			}			
		}



		if( swpa_utils_file_exist("/tmp/.H264ResolutionInfo_Ex") && SUCCEEDED(cH264ResolutionFile_EX.Open("/tmp/.H264ResolutionInfo_Ex", "r")) )
		{
			INT iResolution_EX = 0;
			INT ResolutionHeight=0;
			INT ResolutionWidth=0;
		
			if (FAILED(cH264ResolutionFile_EX.Read(&iResolution_EX, sizeof(iResolution_EX), 0)))
			{
				SW_TRACE_DEBUG("Err: failed to get H.264 resolution info\n");
				cH264ResolutionFile_EX.Close();
			}
			else if (1080 != iResolution_EX && 720 != iResolution_EX && 576 != iResolution_EX && 480 != iResolution_EX)
			{
				SW_TRACE_DEBUG("Err: got invalid H.264 resolution info (%d)\n", iResolution_EX);
				cH264ResolutionFile_EX.Close();
			}
			else
			{
				
				if(1080==iResolution_EX)
				{
					ResolutionWidth=1920;
					ResolutionHeight=1080;
				}
				else if(720==iResolution_EX)
				{
					ResolutionWidth=1280;
					ResolutionHeight=720;
				}
				else if(576 == iResolution_EX)
				{
					ResolutionWidth=720;
					ResolutionHeight=576;
				}
				else if(480 == iResolution_EX)
				{
					ResolutionWidth=720;
					ResolutionHeight=480;
				}

				StartOnvif_EX(ResolutionWidth,ResolutionHeight);

				cH264ResolutionFile_EX.Close();
				swpa_utils_file_delete("/tmp/.H264ResolutionInfo_Ex");
				
			}
					
		}



		
		HeartBeat();		
		//PRINT("Info: HeartBea\n");
		swpa_thread_sleep_ms(2000);
	}

	return S_OK;
}

/**
 *@brief 取得代码的版本号 
 *@return 返回版本号,失败返回-1
 */
INT CSWOnvifApp::GetVersion(VOID)
{
	return CSWApplication::GetVersion(); 
}


HRESULT CSWOnvifApp::OnSetPTZInfo(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("Info: got SetPTZInfo msg...\n");

	if (NULL == pvBuffer || 0 >= iSize)
	{
		SW_TRACE_DEBUG("Err: invalid arg.\n");
		return E_INVALIDARG;
	}

	PTZ_INFO * psInfo = (PTZ_INFO*)pvBuffer;

	m_sPTZParam.fHomeSupported = psInfo->fHomeSupported;
	m_sPTZParam.fFixedHomePos = psInfo->fFixedHomePos;
	m_sPTZParam.iPresetCount = psInfo->iPresetCount;

	m_sPTZParam.fAbsoluteMoveSupport = psInfo->fAbsoluteMoveSupport;
	m_sPTZParam.iAbsolutePRangeMax = psInfo->iAbsolutePRangeMax;
	m_sPTZParam.iAbsolutePRangeMin = psInfo->iAbsolutePRangeMin;
	m_sPTZParam.iAbsoluteTRangeMax = psInfo->iAbsoluteTRangeMax;
	m_sPTZParam.iAbsoluteTRangeMin = psInfo->iAbsoluteTRangeMin;
	m_sPTZParam.iAbsoluteZRangeMax = psInfo->iAbsoluteZRangeMax;
	m_sPTZParam.iAbsoluteZRangeMin = psInfo->iAbsoluteZRangeMin;

	m_sPTZParam.fContinuousMoveSupport = psInfo->fContinuousMoveSupport;
	m_sPTZParam.iContinuousPSpeedMax = psInfo->iContinuousPSpeedMax;
	m_sPTZParam.iContinuousPSpeedMin = psInfo->iContinuousPSpeedMin;
	m_sPTZParam.iContinuousTSpeedMax = psInfo->iContinuousTSpeedMax;
	m_sPTZParam.iContinuousTSpeedMin = psInfo->iContinuousTSpeedMin;
	m_sPTZParam.iContinuousZSpeedMax = psInfo->iContinuousZSpeedMax;
	m_sPTZParam.iContinuousZSpeedMin = psInfo->iContinuousZSpeedMin;

	m_sPTZParam.fInited = TRUE;
	SW_TRACE_DEBUG("domeptz param(%d, %d, %d)\n", psInfo->iContinuousPSpeedMax, psInfo->iContinuousTSpeedMax, psInfo->iContinuousZSpeedMax);

	return S_OK;
}


HRESULT CSWOnvifApp::GetPTZInfo(VOID)
{
	SendRemoteMessage(MSG_APP_REMOTE_GET_PTZ_INFO, NULL, 0);
}

HRESULT CSWOnvifApp::OnSetUSERInfo(PVOID pvBuffer, INT iSize)
{
	char* cAtmp = (char*)pvBuffer;

//	SW_TRACE_DEBUG("GetUserList:get the username list : %s\n ",cAtmp);

	
	char cAcount[4] = {0};
	char* p_str_begin    = NULL;
    char* p_str_end      = NULL;

	p_str_begin = strstr(cAtmp,"UserCount=[");
	p_str_end = strstr(cAtmp,"]");
	if(p_str_begin != NULL && p_str_end != NULL)
	{
		memcpy(cAcount,p_str_begin+11,p_str_end-p_str_begin-11);
	}
	else
	{
		SW_TRACE_DEBUG("GetUserList:get the number of user failed   \n");
	}



	int isizeuser = atoi(cAcount);
	SW_TRACE_DEBUG("the isizeuser is %d \n",isizeuser);
    m_UserSize = isizeuser;

	
	int i;
	char cAUser[32] = {0};
	char cAauthority[32] = {0};
	char cAA[4] = {0};
	for(i =0;i<isizeuser;i++)
	{
		memset(g_psUserlist[i].user,0,32);
		memset(g_psUserlist[i].password,0,16);
		
		snprintf(cAUser,32,"<UserName%02d>",i);
		snprintf(cAauthority,32,"</UserName%02d>",i);
		
		p_str_begin = strstr(cAtmp,cAUser);
		p_str_end = strstr(cAtmp,cAauthority);
		if(p_str_begin != NULL && p_str_end != NULL)
		{
			memcpy(g_psUserlist[i].user,p_str_begin+strlen(cAUser),p_str_end-p_str_begin-strlen(cAUser));
		}
		else
		{
			SW_TRACE_DEBUG("__tds__GetUsers::get user name failed  \n");
		}

		memset(cAUser,0,32);
		memset(cAauthority,0,32);
		snprintf(cAUser,32,"<Passwd%02d>",i);
		snprintf(cAauthority,32,"</Passwd%02d>",i);
		p_str_begin = strstr(cAtmp,cAUser);
		p_str_end = strstr(cAtmp,cAauthority);
		if(p_str_begin != NULL && p_str_end != NULL)
		{
			memcpy(g_psUserlist[i].password,p_str_begin+strlen(cAUser),p_str_end-p_str_begin-strlen(cAUser));
		}
		else
		{
			SW_TRACE_DEBUG("__tds__GetUsers::get user password failed  \n");
		}
	
		memset(cAauthority,0,32);
		snprintf(cAauthority,32,"<UserAuthority%02d>=[",i);
		p_str_begin = strstr(cAtmp,cAauthority);
		if(p_str_begin != NULL)
		{
			memset(cAA,0,4);
			memcpy(cAA,p_str_begin+strlen(cAauthority),1);
			g_psUserlist[i].authority = atoi(cAA);
		}
		else
		{
			SW_TRACE_DEBUG("__tds__GetUsers::get user Authority failed  \n");
		}

	//	SW_TRACE_DEBUG("m_tUserInfo[%d].user=%s;password=%s;authority=%d \n",i,g_psUserlist[i].user,g_psUserlist[i].password,g_psUserlist[i].authority);
	}

	return S_OK;
}



HRESULT CSWOnvifApp::OnSetEnableInfo(PVOID pvBuffer, INT iSize)
{
	if(NULL == pvBuffer)
	{
		SW_TRACE_DEBUG("OnSetEnableInfo: invaild param \n  ",m_fEnable,m_fAuthentEnable);
		return E_FAIL;

	}
	ONVIF_PARAM_INFO* cAtmp = (ONVIF_PARAM_INFO*)pvBuffer;

//	m_cMutex.Lock();
	m_fEnable = cAtmp->iOnvifEnable;
	m_fAuthentEnable = cAtmp->iAuthenticateEnable;
//	m_cMutex.Unlock();

	m_fIsGetParam = TRUE;


	SW_TRACE_DEBUG("OnSetEnableInfo--m_fEnable:%d m_fAuthentEnable:%d !! \n  ",m_fEnable,m_fAuthentEnable);

	return S_OK;
}


/**
 *@brief 进程退出,让守护进程重启
 *@return S_OK;
*/
HRESULT CSWOnvifApp::OnOnvifRestart(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("Restart...\n");
	Exit(0x00);
	return S_OK;
}




int CSWOnvifApp::OnGetUSERInfo(void* pSize, void* pvArg)
{
	if(NULL == pSize || NULL == pvArg)
	{
		SW_TRACE_DEBUG("%s: paramter is null \n",__FUNCTION__);
		return E_INVALIDARG;
	}

	int* iSize = (int*)pSize;
	*iSize = m_UserSize;
	UserArray_t* pvBuffer = (UserArray_t*)pvArg;

	int i;
	for(i = 0; i < m_UserSize; i++)
	{
		memcpy(pvBuffer[i].user,g_psUserlist[i].user,32);
		memcpy(pvBuffer[i].password,g_psUserlist[i].password,16);
		pvBuffer[i].authority = g_psUserlist[i].authority;
	}
	
	return S_OK;
}




INT CSWOnvifApp::PTZControlCallback(const int iCmdID, void* pvArg)
{
	
	DWORD arrdwMsg[3] = {0};
	BOOL fSend = FALSE;
	INT* pArg = (INT*)pvArg;
	
	switch (iCmdID)
	{
		case PTZ_PAN:
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: start pan\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_START_PAN;
			arrdwMsg[1] = pArg[0];
			arrdwMsg[2] = pArg[1];
		}
		break;

		case PTZ_STOP_PAN:
		{
			SW_TRACE_DEBUG("domeptz: stop pan\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_STOP_PAN;
		}
		break;

		case PTZ_TILT:
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}

			SW_TRACE_DEBUG("domeptz: start tilt\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_START_TILT;
			arrdwMsg[1] = pArg[0];
			arrdwMsg[2] = pArg[1];
		}
		break;

		case PTZ_STOP_TILT:
		{
			SW_TRACE_DEBUG("domeptz: stop tilt\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_STOP_TILT;
		}
		break;

		case PTZ_PANTILT:
		{			
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: start pantilt\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_START_PAN_TILT;
			arrdwMsg[1] = pArg[0];
			arrdwMsg[2] = pArg[1];
		}
		break;

		case PTZ_STOP_PANTILT:
		{
			SW_TRACE_DEBUG("domeptz: stop pantilt\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_STOP_PAN_TILT;
		}
		break;

		case PTZ_ZOOM:
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: start zoom\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_START_ZOOM;  
			arrdwMsg[1] = pArg[0];
			arrdwMsg[2] = pArg[1];
		}
		break; 

		case PTZ_STOP_ZOOM:
		{
			SW_TRACE_DEBUG("domeptz: stop zoom\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_STOP_ZOOM;  
		} 
		break;

		case PTZ_SET_PANTILT: 
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: set pt coordinate\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_SET_PT_COORDINATE;
			arrdwMsg[1] = pArg[0] << 16 | pArg[1];
		}
		break;

		case PTZ_SET_ZOOM: 
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: set zoom\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_SET_ZOOM;
			arrdwMsg[1] = pArg[0];
		}
		break;

		case PTZ_PRESET_CALL:
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: call preset\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_CALL_PRESET_POS;
			arrdwMsg[1] = pArg[0];
		}
		break;

		
		case PTZ_PRESET_SET:
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: set preset\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_SET_PRESET_POS;
			arrdwMsg[1] = pArg[0];
			//arrdwMsg[2] = pArg[1]; //name -- unsupported
		}
		break;

		
		case PTZ_PRESET_REMOVE: 
		{
			if (NULL == pArg)
			{
				SW_TRACE_DEBUG("Err: invalid arg: pvArg == 0x%x\n", pvArg);
				return -1;
			}
			
			SW_TRACE_DEBUG("domeptz: remove preset\n");
			fSend = TRUE;
			arrdwMsg[0] = MSG_CLEAR_PRESET_POS;
			arrdwMsg[1] = pArg[0];
		}
		break;

		default:
			SW_TRACE_DEBUG("Warning: got PTZ cmd: %d\n", iCmdID);
			break;
		
	}
 
	if (fSend)
	{
		SendRemoteMessage(MSG_APP_REMOTE_PTZ_CONTROL, arrdwMsg, sizeof(arrdwMsg));
	}

	return 0;
}

INT CSWOnvifApp::UserManageCallback(const int len, void* pvArg)
{

	char* ctemp = (char*)pvArg;

	SW_TRACE_DEBUG("%s: ctemp is %s \n",__FUNCTION__,ctemp);
	
	SendRemoteMessage(MSG_USER_MANAGE, ctemp, len);

	return 0;
}



