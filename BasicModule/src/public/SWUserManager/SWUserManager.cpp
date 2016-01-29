/*
 * SWUserManager.cpp
 *
 *  Created on: 2014年1月6日
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWUserManager.h"

#define USER_INFO_FILE "EEPROM/0/USER_MANAGER_INFO"

#define DEFAULT_ADMIN_NAME 			"admin"
#define DEFAULT_ADMIN_PASSWD		"admin"
#define DEFAULT_ADMIN_AUTHORITY 	0x5

#define DEFAULT_ROOT_NAME			"root"
#define DEFAULT_ROOT_PASSWD			"Venus2014"
#define DEFAULT_ROOT_AUTHORITY 		0x3


CSWUserManager::CSWUserManager()
	: CSWMessage( MSG_USER_BEGIN , MSG_USER_END )
{
	// TODO Auto-generated constructor stub
	m_fLoadInfo = FALSE;
	swpa_strcpy(m_tSuperUser.user, DEFAULT_ROOT_NAME);
	CHAR szSN[256] = {0};
	UINT uLen = 255;
	swpa_device_read_sn(szSN, &uLen);
	if (0!= swpa_strlen(szSN) && NULL != swpa_strstr(szSN, "JPTCAM"))
	{
		swpa_strcpy(m_tSuperUser.password, "Jupiter2014");
	}
	else
	{
		swpa_strcpy(m_tSuperUser.password, DEFAULT_ROOT_PASSWD);
	}
	m_tSuperUser.authority = DEFAULT_ROOT_AUTHORITY;
}

CSWUserManager::~CSWUserManager()
{
	// TODO Auto-generated destructor stub
}

VOID GetStrValue(const CHAR *szSrc,const CHAR *szKey,CHAR *szValue,INT iStrLenMax)
{
	if (NULL == szSrc || NULL == szKey || NULL == szValue)
	{
		SW_TRACE_NORMAL(" GetStrValue Args err!\n");
		return ;
	}
	CSWString strTemp = szSrc;
	CSWString strValue;
	int iIndex = strTemp.Find(szKey);
	int iEnd = 0;
	if( iIndex >= 0 )
	{
		iIndex += swpa_strlen(szKey) + 1; // 包含 [
		iEnd = strTemp.Find("]", iIndex);
		strValue = strTemp.Substr(iIndex, iEnd - iIndex);
	}
	swpa_strncpy(szValue,(const CHAR *)strValue,iStrLenMax);

	return ;
}

HRESULT CSWUserManager::OnUserLogin(WPARAM wParam,LPARAM lParam)
{
	Acount_t aCount;
	Acount_t *pCount = &aCount;
	CHAR *szValue = (CHAR *)wParam;

	INT *iAuthority = (INT *)lParam;
	if (NULL == szValue)
	{
		SW_TRACE_NORMAL("OnUserLogin arg is NULL\n");
		return E_FAIL;
	}
	
	*iAuthority = -1;
	BOOL fLoginSuccess = FALSE;
	
	HRESULT hr = S_OK;

	if (FALSE == m_fLoadInfo)
	{
		hr = LoadUserInfo();
		if (S_OK == hr)
		{
			m_fLoadInfo = TRUE;
		}
		else if (E_OBJ_NO_INIT == hr)
		{
			OnUserRetoreDefault(0,0);
			m_fLoadInfo = TRUE;
		}
		else
		{
			return hr;
		}
	}

	GetStrValue(szValue,"UserName=",pCount->user,USER_LEN_MAX -1);
	GetStrValue(szValue,"Password=",pCount->password,PASSWORD_LEN_MAX -1);

	if (FALSE == UserInfoCheck(pCount))
	{
		SW_TRACE_NORMAL("User info check failed\n");
		return E_INVALIDARG;
	}

	for (int i=0; i<ACOUNT_NUM; i++)
	{
		if ((0 == swpa_strcmp(m_tUserInfo[i].user,pCount->user))
				&& (0 == swpa_strcmp(m_tUserInfo[i].password,pCount->password)))
		{
			*iAuthority = m_tUserInfo[i].authority;
			fLoginSuccess = TRUE;
			break;
		}
	}
	//超级用户不在用户列表中
	if (FALSE == fLoginSuccess && TRUE == SuperUserCheck(pCount))
	{
		SW_TRACE_DEBUG("Super user login Success!\n");
		fLoginSuccess = TRUE;
		*iAuthority = m_tSuperUser.authority;
	}
	
	if (TRUE == fLoginSuccess)
		SW_TRACE_DEBUG("Found it! User name:%s,authority %d\n",pCount->user,*iAuthority);
	else
		SW_TRACE_DEBUG("User name:%s Login Failed\n",pCount->user);
	
	return S_OK;
}

HRESULT CSWUserManager::OnUserGetList(WPARAM wParam,LPARAM lParam)
{
	CHAR *pValue = (CHAR *)lParam;
	if (NULL == pValue)
	{
		SW_TRACE_NORMAL("OnUserGetList arg is NULL\n");
		return E_FAIL;
	}
	
	//UserCount=[2],UserName00=[admin],UserAuthority00=[1],UserName01=[usr1], UserAuthority01=[1]
	if (FALSE == m_fLoadInfo)
	{
		SW_TRACE_NORMAL("OnUserGetList Please Login first!\n");
		return E_FAIL;
	}
	CHAR szUserInfo[128];
	//user count 
	INT iUserCount = 0;
	for (int i=0; i<ACOUNT_NUM; i++)
	{
		if (swpa_strlen(m_tUserInfo[i].user) != 0)
		{
			iUserCount++;
		}
	}

	swpa_sprintf(pValue,"UserCount=[%d]",iUserCount);
	INT iNameIndex = 0;
	for (int i=0; i<ACOUNT_NUM; i++)
	{
		if (0 == swpa_strlen(m_tUserInfo[i].user))
		{
			continue;
		}
		swpa_snprintf(szUserInfo,128,",UserName%02d=[%s],UserAuthority%02d=[%d]",
			iNameIndex,m_tUserInfo[i].user,iNameIndex,m_tUserInfo[i].authority);

		swpa_strcat(pValue,szUserInfo);
		iNameIndex++;
	}

	SW_TRACE_DEBUG("User Info Get List success\n");
	return S_OK;
}

HRESULT CSWUserManager::OnGetListUsername_Passwd(WPARAM wParam,LPARAM lParam)
{
	CHAR *pValue = (CHAR *)lParam;
	if (NULL == pValue)
	{
		SW_TRACE_NORMAL("OnUserGetList arg is NULL\n");
		return E_FAIL;
	}
	
	//UserCount=[2],UserName00=[admin],UserAuthority00=[1],UserName01=[usr1], UserAuthority01=[1]
	if (FALSE == m_fLoadInfo)
	{
		SW_TRACE_NORMAL("OnUserGetList Please Login first!\n");
		return E_FAIL;
	}
	CHAR szUserInfo[128];
	//user count 
	INT iUserCount = 0;
	for (int i=0; i<ACOUNT_NUM; i++)
	{
		if (swpa_strlen(m_tUserInfo[i].user) != 0)
		{
			iUserCount++;
		}
	}

	swpa_sprintf(pValue,"UserCount=[%d]",iUserCount);
	INT iNameIndex = 0;
	for (int i=0; i<ACOUNT_NUM; i++)
	{
		if (0 == swpa_strlen(m_tUserInfo[i].user))
		{
			continue;
		}
		swpa_snprintf(szUserInfo,128,"<UserName%02d>%s</UserName%02d>,<UserAuthority%02d>=[%d],<Passwd%02d>%s</Passwd%02d>",
			iNameIndex,m_tUserInfo[i].user,iNameIndex,iNameIndex,m_tUserInfo[i].authority,iNameIndex,m_tUserInfo[i].password,iNameIndex);

		swpa_strcat(pValue,szUserInfo);
		iNameIndex++;
	}

	SW_TRACE_DEBUG("User Info Get username and password success\n");
	return S_OK;
}




HRESULT CSWUserManager::OnUserAdd(WPARAM wParam,LPARAM lParam)
{
	Acount_t tAdmin;
	Acount_t tAddUser;
	BOOL fAddSuccess = FALSE;
	CHAR *szValue = (CHAR *)wParam;
	if (NULL == szValue)
	{
		SW_TRACE_NORMAL("OnUserAdd arg is NULL\n");
		return E_FAIL;
	}
	//UserName=[admin ],Password=[9999], AddUserName=[usr ],AddUserPassword=[123456 ],AddUserAuthority=[1]
	if (FALSE == m_fLoadInfo)
	{
		SW_TRACE_NORMAL("OnUserAdd Please Login first!\n");
		return E_FAIL;
	}
	GetStrValue(szValue,"UserName=",tAdmin.user,USER_LEN_MAX-1);
	GetStrValue(szValue,"Password=",tAdmin.password,PASSWORD_LEN_MAX-1);

	GetStrValue(szValue,"AddUserName=",tAddUser.user,USER_LEN_MAX-1);
	GetStrValue(szValue,"AddUserPassword=",tAddUser.password,PASSWORD_LEN_MAX-1);

	CHAR szAddUserAuthority[32];
	GetStrValue(szValue,"AddUserAuthority=",szAddUserAuthority,sizeof(szAddUserAuthority)-1);
	tAddUser.authority = swpa_atoi(szAddUserAuthority);
	
	if (FALSE == UserInfoCheck(&tAdmin)
		|| FALSE == UserInfoCheck(&tAddUser))
	{
		SW_TRACE_NORMAL("User info check failed\n");
		return E_INVALIDARG;
	}
	
	SW_TRACE_DEBUG("OnUserAdd:tAdmin.user = %s ,passwd = %s AddUserName= %s ,AdduserPassword= %s \n",
					tAdmin.user,tAdmin.password,tAddUser.user,tAddUser.password);
	//check admin
	if (FALSE == AdminCheck(&tAdmin) && FALSE == SuperUserCheck(&tAdmin))
	{
		SW_TRACE_NORMAL("Admin check failed\n");
		return E_FAIL;
	}

	for (int i=0; i<ACOUNT_NUM; i++)
	{
		//相同用户名
		if (swpa_strlen(m_tUserInfo[i].user) > 0
				&& 0 == swpa_strcmp(tAddUser.user,m_tUserInfo[i].user))
		{
			SW_TRACE_NORMAL("The same user name: %s!\n",tAddUser.user);
			fAddSuccess = FALSE;
			break;
		}

		if (0 == swpa_strlen(m_tUserInfo[i].user))
		{
			memcpy(&m_tUserInfo[i],&tAddUser,sizeof(m_tUserInfo[i]));
			fAddSuccess = TRUE;
			break;
		}
	}
	
	//commit
	return (FALSE == fAddSuccess)? E_FAIL : CommitUserInfo();
}

HRESULT CSWUserManager::OnUserDell(WPARAM wParam,LPARAM lParam)
{
	//UserName=[admin], Password=[9999], DelUserName=[usr]
	//Acount_t *pAcount = (Acount_t *)wParam;
	CHAR *szValue = (CHAR *)wParam;

	if (NULL == szValue)
	{
		SW_TRACE_NORMAL("OnUserDell arg is NULL\n");
		return E_FAIL;
	}

	Acount_t tAdmin;
	CHAR szDelUserName[USER_LEN_MAX];
	
	if (FALSE == m_fLoadInfo)
	{
		SW_TRACE_NORMAL("OnUserDell Please Login first!\n");
		return E_FAIL;
	}

	GetStrValue(szValue,"UserName=",tAdmin.user,USER_LEN_MAX-1);
	GetStrValue(szValue,"Password=",tAdmin.password,PASSWORD_LEN_MAX-1);
	GetStrValue(szValue,"DelUserName=",szDelUserName,USER_LEN_MAX-1);
	
	if (FALSE == UserInfoCheck(&tAdmin))
	{
		SW_TRACE_NORMAL("User info check failed\n");
		return E_INVALIDARG;
	}

	//check admin
	if (FALSE == AdminCheck(&tAdmin) && FALSE == SuperUserCheck(&tAdmin))
	{
		SW_TRACE_NORMAL("Admin check failed\n");
		return E_FAIL;
	}
	
	BOOL fDelSuccess = FALSE;

	for (int i=1; i<ACOUNT_NUM; i++)	//0 为admin不能删除不能添加
	{
		if (0 == swpa_strcmp(szDelUserName,m_tUserInfo[i].user) && i != 0)
		{
			Acount_t tAcount;// = NONE_ACOUT;
			memcpy(&m_tUserInfo[i],&tAcount,sizeof(tAcount));
			fDelSuccess = TRUE;
			break;
		}
	}
	SW_TRACE_NORMAL("Delete User %s,%s!\n",szDelUserName,(TRUE == fDelSuccess)?"Success":"Failed");
	
	return (TRUE == fDelSuccess)?CommitUserInfo():E_FAIL;
}

HRESULT CSWUserManager::OnUserGetPassword(WPARAM wParam, LPARAM lParam)
{
	const INT cniMaxLen = 127;
	CHAR *pUserName = (CHAR *)wParam;
	CHAR *pPassword = (CHAR *)lParam;
	HRESULT hr = S_OK;
	BOOL fIsNormalUser = FALSE;

	if (FALSE == m_fLoadInfo)
	{
		hr = LoadUserInfo();
		if (S_OK == hr)
		{
			m_fLoadInfo = TRUE;
		}
		else if (E_OBJ_NO_INIT == hr)
		{
			OnUserRetoreDefault(0,0);
			m_fLoadInfo = TRUE;
		}
		else
		{
			return hr;
		}
	}
	//普通用户
	for (int i=0; i<ACOUNT_NUM; i++)
	{
		if (0 == swpa_strlen(m_tUserInfo[i].user) ||
			0 != swpa_strncmp(pUserName, m_tUserInfo[i].user, swpa_strlen(m_tUserInfo[i].user)))
		{
			continue;
		}

		INT iPasswordLen = swpa_strlen(m_tUserInfo[i].password);
		iPasswordLen = iPasswordLen > cniMaxLen ? cniMaxLen : iPasswordLen;

		swpa_memcpy(pPassword, m_tUserInfo[i].password, iPasswordLen);
		fIsNormalUser = TRUE;
		break;
	}

	//超级用户
	if((!fIsNormalUser) && 
	   (0 == swpa_strncmp(m_tSuperUser.user, pUserName, swpa_strlen(m_tSuperUser.user))))
	{
		INT iPasswordLen = swpa_strlen(m_tSuperUser.password);
		iPasswordLen = iPasswordLen > cniMaxLen ? cniMaxLen : iPasswordLen;

		swpa_memcpy(pPassword, m_tSuperUser.password, iPasswordLen);
	}

	return S_OK;
}

HRESULT CSWUserManager::OnUserChange(WPARAM wParam,LPARAM lParam)
{
	//Acount_t *pAcount = (Acount_t *)wParam;
	//UserName=[admin], Password=[9999], ModUserName=[admin], ModUserPassword=[123456], ModUserAuthority=[1]"
	CHAR *szValue = (CHAR *)wParam;

	if (NULL == szValue)
	{
		SW_TRACE_NORMAL("OnUserChange arg is NULL\n");
		return E_FAIL;
	}

	Acount_t tAdmin;
	Acount_t tChange;
	
	if (FALSE == m_fLoadInfo)
	{
		SW_TRACE_NORMAL("OnUserChange Please Login first!\n");
		return E_FAIL;
	}

	GetStrValue(szValue,"UserName=",tAdmin.user,USER_LEN_MAX-1);
	GetStrValue(szValue,"Password=",tAdmin.password,PASSWORD_LEN_MAX-1);

	GetStrValue(szValue,"ModUserName=",tChange.user,USER_LEN_MAX-1);
	GetStrValue(szValue,"ModUserPassword=",tChange.password,PASSWORD_LEN_MAX-1);

	CHAR szModUserAuthority[32];
	GetStrValue(szValue,"ModUserAuthority=",szModUserAuthority,sizeof(szModUserAuthority)-1);
	tChange.authority = swpa_atoi(szModUserAuthority);

	if (FALSE == UserInfoCheck(&tAdmin)
		|| FALSE == UserInfoCheck(&tChange))
	{
		SW_TRACE_NORMAL("User info check failed\n");
		return E_INVALIDARG;
	}
	BOOL fChangeSuccess = FALSE;

	SW_TRACE_DEBUG("UserName= %s,Password= %s,ModUserName=%s,ModUserPassword= %s \n ",
					tAdmin.user,tAdmin.password,tChange.user,tChange.password);
	
	//check admin
	if (TRUE == AdminCheck(&tAdmin) || TRUE == SuperUserCheck(&tAdmin))	//admin
	{
		for (int i=0; i<ACOUNT_NUM; i++)
		{
			if (0 == swpa_strcmp(tChange.user,m_tUserInfo[i].user))
			{
				memcpy(&m_tUserInfo[i],&tChange,sizeof(m_tUserInfo[i]));
				fChangeSuccess = TRUE;
				break;
			}
		}
	}
	else		//user self
	{
		for (int i=0; i<ACOUNT_NUM; i++)
		{
			if (0 == swpa_strcmp(tAdmin.user,m_tUserInfo[i].user) 
				&& 0 == swpa_strcmp(tAdmin.password,m_tUserInfo[i].password)
				&& 0 == swpa_strcmp(tAdmin.user,tChange.user))	//不能修改用户名
			{
				memcpy(&m_tUserInfo[i],&tChange,sizeof(m_tUserInfo[i]));
				fChangeSuccess = TRUE;
				break;
			}
		}
	}

	SW_TRACE_NORMAL("Change User %s password %s!\n",
		tChange.user,(TRUE == fChangeSuccess)?"Success":"Failed");

	return (TRUE == fChangeSuccess)?CommitUserInfo():E_FAIL;
}

HRESULT CSWUserManager::OnUserRetoreDefault(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_NORMAL("#########Retore User Info Default.......\n");
	RestoreDefault();

	return CommitUserInfo();
}


HRESULT CSWUserManager::OnUserManage_ONVIF(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF\n");

    
    char* pdwMsg = (char*)pvBuffer;
	char cACmd[4] = {0};
	char cUserAuthority[4] = {0};
	char cAUsername[32] = {0};
	char cAPassword[16] = {0};

	CHAR    *p_str_begin    = NULL;
    CHAR    *p_str_end      = NULL;

	
	SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF recevice is : %s  \n",pdwMsg);

	p_str_begin = strstr(pdwMsg,"<CMD>");
	p_str_end = strstr(pdwMsg,"</CMD>");
	if(p_str_begin != NULL && p_str_end != NULL)
	{
		memcpy(cACmd,p_str_begin+5,p_str_end-p_str_begin-5);
	}
	else
	{
		SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF CMD is NULL  \n");
	}

	p_str_begin = strstr(pdwMsg,"<UserName>");
	p_str_end = strstr(pdwMsg,"</UserName>");
	if(p_str_begin != NULL && p_str_end != NULL)
	{
		memcpy(cAUsername,p_str_begin+10,p_str_end-p_str_begin-10);
	}
	else
	{
		SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF cAUsername is NULL  \n");
	}

	p_str_begin = strstr(pdwMsg,"<PassWord>");
	p_str_end = strstr(pdwMsg,"</PassWord>");
	if(p_str_begin != NULL && p_str_end != NULL)
	{
		memcpy(cAPassword,p_str_begin+10,p_str_end-p_str_begin-10);
	}
	else
	{
		SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF cAPassword is NULL  \n");
	}

	p_str_begin = strstr(pdwMsg,"<UserAuthority>");
	p_str_end = strstr(pdwMsg,"</UserAuthority>");
	if(p_str_begin != NULL && p_str_end != NULL)
	{
		memcpy(cUserAuthority,p_str_begin+15,p_str_end-p_str_begin-15);
	}
	else
	{
		SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF UserAuthority is NULL  \n");
	}

	
//	swpa_sscanf(pdwMsg,"CMD=[%s];UserName=[%s];PassWord=[%s]\r\n",cACmd,cAUsername,cAPassword);

	int iCmd = swpa_atoi(cACmd);
	int iUserAuthority = swpa_atoi(cUserAuthority);

//	SW_TRACE_DEBUG("CSWUserManager::OnUserManage_ONVIF recevice is : CMD = %d username = %s passwd = %s UserAuthority = %d\n",
//									iCmd,cAUsername,cAPassword,iUserAuthority);


    // 先用admin 登陆
    int iAuthor = DEFAULT_ADMIN_AUTHORITY;
    char cALogin[64] = "UserName=[admin],Password=[admin]"; 
	if(S_OK != OnUserLogin((WPARAM)cALogin,(LPARAM)&iAuthor))
	{
		SW_TRACE_DEBUG("admin login failed\n");
	}


	switch(iCmd)
	{
		case 1:
			{
				// 获取用户列表
				char cAtmp[1024] = {0};
				if(S_OK != OnGetListUsername_Passwd(0,(LPARAM)cAtmp))
				{
					SW_TRACE_DEBUG("%s: get user list faield \n",__FUNCTION__);
					return -1;
				}

			//	SW_TRACE_DEBUG("get the usernamelist is %s \n",cAtmp);

				SendRemoteMessage( MSG_APP_REMOTE_SET_USER_INFO, cAtmp, sizeof(cAtmp));
			}
			break;
		case 2:
			{	
				//添加用户
				char cAAdduser[128] = {0}; 
				swpa_snprintf(cAAdduser,128,"UserName=[admin];Password=[admin];AddUserName=[%s];AddUserPassword=[%s];AddUserAuthority=[%d]\r\n",
																			   cAUsername,cAPassword,iUserAuthority);

				if(S_OK != OnUserAdd((WPARAM)cAAdduser,0))
				{
					SW_TRACE_DEBUG("%s: add user faield \n",__FUNCTION__);
					return -1;
				}
			}
			break;
		case 3:
			{
				//删除用户
				char cADeluser[128] = {0};
				swpa_snprintf(cADeluser,128,"UserName=[admin];Password=[admin];DelUserName=[%s]\r\n",cAUsername);

				if(S_OK != OnUserDell((WPARAM)cADeluser,0))
				{
					SW_TRACE_DEBUG("%s: delete user faield \n",__FUNCTION__);
					return -1;
				}
			}
			break;
		case 4:
			{
				//修改用户密码
				char cAModefiy[128] = {0};
				swpa_snprintf(cAModefiy,128,"UserName=[admin];Password=[admin];ModUserName=[%s];ModUserPassword=[%s];ModUserAuthority=[%d]\r\n",
																			       cAUsername,cAPassword,iUserAuthority);
				if(S_OK != OnUserChange((WPARAM)cAModefiy,0))
				{
					SW_TRACE_DEBUG("%s: delete user faield \n",__FUNCTION__);
					return -1;
				}
			}
			break;
		default:
			SW_TRACE_DEBUG("%s:CMD is invalid!!! \n",__FUNCTION__);
			break;
	}
	
	return 0;
}


HRESULT CSWUserManager::LoadUserInfo(VOID)
{
	SW_TRACE_NORMAL("Load User Info from EEPROM......\n");
	CSWFile * pSWFile;

	//open 
	pSWFile = new CSWFile();
	if (NULL == pSWFile)
	{
		SW_TRACE_NORMAL("Err: failed to alloc memory for pSWFile\n");
		return E_OUTOFMEMORY;
	}

	if (FAILED(pSWFile->Open(USER_INFO_FILE, "r")))
	{
		SW_TRACE_NORMAL("Err: failed to opne %s with r mode \n", USER_INFO_FILE);
		SAFE_DELETE(pSWFile);
		return E_FAIL;
	}

	//read head
	tUserInfoHead datahead;
	if (FAILED(pSWFile->Read(&datahead,sizeof(datahead))))
	{
		SW_TRACE_NORMAL("Err: Read User Info head failed\n");
		pSWFile->Close();
		SAFE_DELETE(pSWFile);
		return E_FAIL;
	}

	//check head
	if (datahead.bSync != USER_DATA_SYNC 
		|| datahead.bValid != USER_DATA_VALID
		|| datahead.wDataLen != sizeof(m_tUserInfo))
	{
		SW_TRACE_NORMAL("Warning: EEPROM User Info data is not init\n");
		pSWFile->Close();
		SAFE_DELETE(pSWFile);
		return E_OBJ_NO_INIT;
	}

	//read data
	if (FAILED(pSWFile->Read(m_tUserInfo,sizeof(m_tUserInfo))))
	{
		SW_TRACE_NORMAL("Err: Read User Info data Failed\n");
		pSWFile->Close();
		SAFE_DELETE(pSWFile);
		return E_FAIL;
	}

	//close delete CSWFile
	pSWFile->Close();
	SAFE_DELETE(pSWFile);

	return S_OK;
}

HRESULT CSWUserManager::CommitUserInfo(VOID)
{
	//open file
	CSWFile * pSWFile;
	pSWFile = new CSWFile();
	if (NULL == pSWFile)
	{
		SW_TRACE_NORMAL("Err: failed to alloc memory for pSWFile\n");
		return E_OUTOFMEMORY;
	}

	if (FAILED(pSWFile->Open(USER_INFO_FILE, "w")))
	{
		SW_TRACE_NORMAL("Err: failed to opne %s with w mode \n", USER_INFO_FILE);
		SAFE_DELETE(pSWFile);
		return E_FAIL;
	}
	//write head
	tUserInfoHead datahead;
	datahead.wDataLen = sizeof(m_tUserInfo);
	if (FAILED(pSWFile->Write(&datahead,sizeof(datahead))))
	{
		SW_TRACE_NORMAL("Err: User Info head Write Failed\n");
		pSWFile->Close();
		SAFE_DELETE(pSWFile);
		return E_FAIL;
	}

	//write data
	if (FAILED(pSWFile->Write(m_tUserInfo,sizeof(m_tUserInfo))))
	{
		SW_TRACE_NORMAL("Err: User Info data Write Failed\n");
		pSWFile->Close();
		SAFE_DELETE(pSWFile);
		return E_FAIL;
	}

	//close file
	pSWFile->Close();
	SAFE_DELETE(pSWFile);

	return S_OK;
}

VOID CSWUserManager::RestoreDefault(VOID)
{
	Acount_t tAdmin_Acount;// = ADMIN_ACOUT;
	swpa_strcpy(tAdmin_Acount.user,DEFAULT_ADMIN_NAME);
	swpa_strcpy(tAdmin_Acount.password,DEFAULT_ADMIN_PASSWD);
	tAdmin_Acount.authority = DEFAULT_ADMIN_AUTHORITY;
	memcpy(&m_tUserInfo[0],&tAdmin_Acount,sizeof(tAdmin_Acount));

	for (int i=1; i<ACOUNT_NUM; i++)
	{
		Acount_t tAcount;// = NONE_ACOUT;
		memcpy(&m_tUserInfo[i],&tAcount,sizeof(tAcount));
	}
}

BOOL CSWUserManager::UserInfoCheck(Acount_t *pAcount)
{
	if (NULL == pAcount)
	{
		return FALSE;
	}
	
	INT iUserNameLen = swpa_strlen(pAcount->user);
	INT iPassWordLen = swpa_strlen(pAcount->password);

	if (iUserNameLen > USER_LEN_MAX - 1 || iUserNameLen < MIN_USER_LEN)
	{
		SW_TRACE_NORMAL("User name len %d is not allow\n",iUserNameLen);
		return FALSE;
	}

	if (iPassWordLen > USER_LEN_MAX - 1 || iPassWordLen < MIN_USER_LEN)
	{
		SW_TRACE_NORMAL("User password len %d is not allow\n",iPassWordLen);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CSWUserManager::AdminCheck(Acount_t *pAcount)
{
	if (NULL == pAcount)
	{
		SW_TRACE_NORMAL("AdminCheck arg is NULL\n");
		return FALSE;
	}

	if (0 == swpa_strcmp(pAcount->user,m_tUserInfo[0].user) 
		&& 0 == swpa_strcmp(pAcount->password,m_tUserInfo[0].password))
	{
		SW_TRACE_DEBUG("Admin check Success!\n");
		return TRUE;
	}
	
	SW_TRACE_DEBUG("Admin check failed!\n");
	return FALSE;
}

BOOL CSWUserManager::SuperUserCheck(Acount_t *pAcount)
{
	if (NULL == pAcount)
	{
		SW_TRACE_NORMAL("AdminCheck arg is NULL\n");
		return FALSE;
	}
	
	if (0 == swpa_strcmp(pAcount->user, m_tSuperUser.user) 
		&& 0 == swpa_strcmp(pAcount->password, m_tSuperUser.password))
	{
		SW_TRACE_DEBUG("Super User check Success!\n");
		return TRUE;
	}

	SW_TRACE_DEBUG("Admin check failed!\n");
	return FALSE;
}

