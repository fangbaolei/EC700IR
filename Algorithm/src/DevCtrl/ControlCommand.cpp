#include "ControlCommand.h"
#include "ControlFunc.h"

using namespace HvSys;

static IResultIO* g_pIResultIO = NULL;
static const int RESPOND_BUF_LEN = 1024;
static const int PARAM_FILE_LEN = 256 * 1024;

int GetTimeCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if( pCmdInfo->dwArgLen != 0 ) return E_INVALIDARG;

	SYS_INFO cInfo;
	cInfo.nType = INFO_SYS_TIME;

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = NOT_SEND_RESULT;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = GetSysInfo(&cInfo);
	cRespond.dwArgLen = 8;

	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	DWORD32 rgTime[2] =
	{
		cInfo.Info.SysTime.dwTimeLow,
		cInfo.Info.SysTime.dwTimeHigh
	};

	if (FAILED(pCmdLink->SendData(rgTime, cRespond.dwArgLen, NULL)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

int GetIDCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen != 0 ) return E_INVALIDARG;

	//申请内存
	CFastMemAlloc cStack;
	DWORD32* pBuf = (DWORD32*)cStack.StackAlloc( RESPOND_BUF_LEN, FALSE );

	if ( pBuf == NULL ) return E_OUTOFMEMORY;
	HV_memset(pBuf, 0, RESPOND_BUF_LEN);

	//执行操作,填写回应信息
	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = NOT_SEND_RESULT;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;

	SYS_INFO cInfo;

	cInfo.nType = INFO_SYS_ID;
	HRESULT hr = GetSysInfo(&cInfo);

	BOOL fOK = TRUE;
	int nSysNameLen = 0;
	if ( SUCCEEDED(hr) )
	{
		pBuf[0] = cInfo.Info.SysID.IDLow;
		pBuf[1] = cInfo.Info.SysID.IDHigh;

		cInfo.nType = INFO_SYS_NAME;
		cInfo.Info.SysName.pbNameBuf = (char*)(pBuf+2);
		cInfo.Info.SysName.nLen = RESPOND_BUF_LEN - 8;

		hr = GetSysInfo(&cInfo);

		if ( SUCCEEDED(hr) )
		{
			nSysNameLen = cInfo.Info.SysName.nLen;
		}
		else
		{
			fOK = FALSE;
		}
	}
	else
	{
		fOK = FALSE;
	}

	if ( fOK )
	{
		cRespond.nResult = S_OK;
		cRespond.dwArgLen = 8 + nSysNameLen;
	}
	else
	{
		cRespond.nResult = E_FAIL;
		cRespond.dwArgLen = 0;
	}

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	//传送结果数据
	if (FAILED(pCmdLink->SendData(pBuf, cRespond.dwArgLen, NULL)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

int SetSafeMode( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen < 18 || pCmdInfo->dwArgLen > 1024 ) return E_INVALIDARG;

	//申请接收缓存
	CFastMemAlloc cStack;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc( pCmdInfo->dwArgLen, FALSE );
	if ( pBuf == NULL ) return E_OUTOFMEMORY;
	HV_memset( pBuf, 0, pCmdInfo->dwArgLen );

	//接收参数数据
	if ( FAILED( pCmdLink->ReceiveData( pBuf, pCmdInfo->dwArgLen, NULL) ) )
	{
		return NET_FAILED;
	}

	DWORD32 dwTimeLow = *(DWORD32*)(pBuf);
	DWORD32 dwTimeHigh = *(DWORD32*)(pBuf + 4);
	DWORD32 index = *(DWORD32*)(pBuf + 8);
	unsigned int uIP = *(DWORD32*)(pBuf + 12);
	unsigned short usPort = *(WORD16*)(pBuf + 16);
	usPort = Hv_ntohs(usPort);
	int iPageIndex = -1;
	int iPageSize = 0;
	if( pCmdInfo->dwArgLen == 24 )
	{
		iPageIndex = *(WORD16*)(pBuf + 18);
		iPageSize = *(int*)(pBuf + 20);
	}

	//执行操作,填写回应信息
	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.dwArgLen = 0;

	if( g_pIResultIO != NULL )
	{
		cRespond.nResult = g_pIResultIO->SetSafeMode( uIP, usPort, dwTimeLow, dwTimeHigh, index, iPageIndex, iPageSize);
	}
	else
	{
		cRespond.nResult = (int)E_FAIL;
	}

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

int SetFilterCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen != 11 ) return E_INVALIDARG;

	//申请接收缓存
	CFastMemAlloc cStack;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc( pCmdInfo->dwArgLen, FALSE );
	if ( pBuf == NULL ) return E_OUTOFMEMORY;
	HV_memset( pBuf, 0, pCmdInfo->dwArgLen );

	//接收参数数据
	if ( FAILED( pCmdLink->ReceiveData( pBuf, pCmdInfo->dwArgLen, NULL) ) )
	{
		return NET_FAILED;
	}

	unsigned int uIP = *(DWORD32*)(pBuf);
	unsigned short usPort = *(WORD16*)(pBuf + 4);
	usPort = Hv_ntohs(usPort);
	unsigned short usVideoID = *(WORD16*)(pBuf + 6);
	unsigned short usStreamID = *(WORD16*)(pBuf + 8);
	BYTE8 ucSwitch = *(pBuf + 10);

	//执行操作,填写回应信息
	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.dwArgLen = 0;

	if( g_pIResultIO != NULL )
	{
		if ( ucSwitch )
		{
			cRespond.nResult = g_pIResultIO->SetStreamID( uIP, usPort, usVideoID, usStreamID );
		}
		else
		{
			cRespond.nResult = g_pIResultIO->DeleteStreamID( uIP, usPort, usVideoID, usStreamID );
		}
	}
	else
	{
		cRespond.nResult = (int)E_FAIL;
	}

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
	    Trace("pCmdLink->SendRespond is failed!!!\n");
		return NET_FAILED;
	}
	return S_OK;
}

int GetVersionCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if( pCmdInfo->dwArgLen != 0 ) return E_INVALIDARG;

	//执行操作,填写回应信息
	SYS_INFO cInfo;
	cInfo.nType = INFO_SYS_VERSION;

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = NOT_SEND_RESULT;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = GetSysInfo(&cInfo);
	cRespond.dwArgLen = 4;

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	//传送结果数据
	if (FAILED(pCmdLink->SendData(&cInfo.Info.SysVersion.dwVersion, cRespond.dwArgLen, NULL)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

int GetVersionCommandString( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
    const char* PSZ_DSP_BUILD_NO = "v0.1";  //临时

	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if( pCmdInfo->dwArgLen != 0 ) return E_INVALIDARG;

	//执行操作,填写回应信息

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = S_OK;
	cRespond.dwArgLen = strlen(PSZ_DSP_BUILD_NO) + 1;

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	char szVersion[256];
	HV_memset(szVersion,0,256);
	memcpy(szVersion,PSZ_DSP_BUILD_NO,strlen(PSZ_DSP_BUILD_NO));

	//传送结果数据
	if (FAILED(pCmdLink->SendData(szVersion, cRespond.dwArgLen, NULL)))
	{
		return NET_FAILED;
	}
	return S_OK;
}

int GetVideoNameCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	CFastMemAlloc cStack;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(32,FALSE);
	if (pBuf == NULL) return E_OUTOFMEMORY;
	HV_memset(pBuf, 0, 32);

	sprintf((char*)(pBuf + 1), "Not Used.");
	pBuf[0] = (BYTE8)strlen("Not Used.");

	DWORD32 dwVideoCount = 1;

	//执行操作,填写回应信息
	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = NOT_SEND_RESULT;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = S_OK;
	cRespond.dwArgLen = 4 + 1 + pBuf[0];

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	if (FAILED(pCmdLink->SendData(&dwVideoCount, 4, NULL)))
	{
		return NET_FAILED;
	}

	//传送结果数据
	if (FAILED(pCmdLink->SendData(pBuf, cRespond.dwArgLen - 4, NULL)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

//设置系统选项命令处理
int SetOptionCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen < 4 ) return E_INVALIDARG;

	DWORD32 dwOptID = 0;

    //接收OptID
	if (FAILED(pCmdLink->ReceiveData(&dwOptID, 4, NULL)))
	{
		return NET_FAILED;
	}

	UINT nParamLen =  pCmdInfo->dwArgLen - 4;

	CFastMemAlloc cStack;
	BYTE8* pbParamDat = (BYTE8*)cStack.StackAlloc(RESPOND_BUF_LEN, FALSE);
	if ( pbParamDat == NULL ) return E_OUTOFMEMORY;
	HV_memset(pbParamDat, 0, RESPOND_BUF_LEN);

	if ( (nParamLen > 0) && FAILED(pCmdLink->ReceiveData(pbParamDat, nParamLen, NULL)))
	{
		return NET_FAILED;
	}

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.dwArgLen = 0;

	SYS_INFO cInfo;

	switch ( dwOptID & 0xffff0000)
	{
	case HV_OPT_FONTMODE:
		if ( nParamLen < 4 )
		{
			cRespond.nResult = E_FAIL;
		}
		else
		{
			cInfo.nType = INFO_FONTMODE;
			cInfo.Info.FontMode.dwMode = *(DWORD32*)pbParamDat;
			cRespond.nResult = SetSysInfo( &cInfo );
		}
		break;

	case HV_OPT_WORKMODE:
		if ( nParamLen < 4 )
		{
			cRespond.nResult = E_FAIL;
		}
		else
		{
			cInfo.nType = INFO_WORKMODE;
			cInfo.Info.WorkMode.dwMode = *(DWORD32*)pbParamDat;
			cRespond.nResult = SetSysInfo( &cInfo );
		}
		break;

	default:
		cRespond.nResult = E_NOTIMPL;
		break;
	}

    //传送结果数据
	if ( FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

//读取系统选项命令处理
int GetOptionCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	//接收OptID
	if ( pCmdInfo->dwArgLen != 4 ) return E_INVALIDARG;	//无其他附加参数

	DWORD32 dwOptID;

	if (FAILED(pCmdLink->ReceiveData(&dwOptID, 4, NULL)))
	{
		return NET_FAILED;
	}

	//执行操作,填写回应信息
	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;

	//返回数据用内存
	CFastMemAlloc cStack;
	DWORD32* rgRespond = (DWORD32*)cStack.StackAlloc(RESPOND_BUF_LEN, FALSE);
	if ( rgRespond == NULL ) return E_OUTOFMEMORY;
	HV_memset(rgRespond, 0, RESPOND_BUF_LEN);

	SYS_INFO cInfo;

	switch ( dwOptID  & 0xffff0000 )
	{
	case HV_OPT_RESETCOUNT:
		{
			cInfo.nType = INFO_RESET_COUNT;

			cRespond.nResult = GetSysInfo(&cInfo);
			cRespond.dwArgLen = 4;
			rgRespond[0] = cInfo.Info.ResetCount.dwCount;
		}
		break;

	case HV_OPT_RESETMODE:
		{
			cInfo.nType = INFO_BOOTMODE;

			cRespond.nResult = GetSysInfo(&cInfo);
			cRespond.dwArgLen = 4;
			rgRespond[0] = cInfo.Info.BootMode.dwMode;
		}
		break;

	case HV_OPT_PRODUCTNAME:
		{
			cInfo.nType = INFO_PRODUCT_NAME;
			cInfo.Info.ProductName.pbNameBuf = (char*)rgRespond;
			cInfo.Info.ProductName.nLen = RESPOND_BUF_LEN;

			cRespond.nResult = GetSysInfo(&cInfo);

			cRespond.dwArgLen = (DWORD32)cInfo.Info.ProductName.nLen;
		}
		break;

	case HV_OPT_FONTMODE:
		{
			cInfo.nType = INFO_FONTMODE;

			cRespond.nResult = GetSysInfo(&cInfo);
			cRespond.dwArgLen = 4;

			rgRespond[0] = cInfo.Info.FontMode.dwMode;
		}
		break;

	case HV_OPT_FONTMODE_COUNT:
		{
			cInfo.nType = INFO_FONTMODE;

			cRespond.nResult = GetSysInfo(&cInfo);
			cRespond.dwArgLen = 4;

			rgRespond[0] = cInfo.Info.FontMode.dwModeCount;
		}
		break;

	case HV_OPT_FONTMODE_INDEX:
		{
			cInfo.nType = INFO_FONTMODE_LIST;

			cInfo.Info.FontModeList.nMode = dwOptID & 0x0000ffff;
			cInfo.Info.FontModeList.pbListBuf = (char*)rgRespond;
			cInfo.Info.FontModeList.nLen = RESPOND_BUF_LEN;

			cRespond.nResult = GetSysInfo( &cInfo );
			cRespond.dwArgLen = cInfo.Info.FontModeList.nLen;
		}
		break;

    case HV_OPT_WORKMODE:
		{
			cInfo.nType = INFO_WORKMODE;

			cRespond.nResult = GetSysInfo(&cInfo);
			cRespond.dwArgLen = 4;

			rgRespond[0] = cInfo.Info.WorkMode.dwMode;
		}
		break;

	case HV_OPT_WORKMODE_COUNT:
		{
			cInfo.nType = INFO_WORKMODE;

			cRespond.nResult = GetSysInfo(&cInfo);
			cRespond.dwArgLen = 4;

			rgRespond[0] = cInfo.Info.WorkMode.dwModeCount;
		}
		break;

	case HV_OPT_WORKMODE_INDEX:
		{
			cInfo.nType = INFO_WORKMODE_LIST;

			cInfo.Info.WorkModeList.nMode = dwOptID & 0x0000ffff;
			cInfo.Info.WorkModeList.pbListBuf = (char*)rgRespond;
			cInfo.Info.WorkModeList.nLen = RESPOND_BUF_LEN;

			cRespond.nResult = GetSysInfo( &cInfo );
			cRespond.dwArgLen = cInfo.Info.WorkModeList.nLen;
		}
		break;

	case HV_OPT_SYSLED:
		{
			cInfo.nType = INFO_SYS_LED;
			cRespond.nResult = GetSysInfo( &cInfo );
			rgRespond[0] = cInfo.Info.SysLed.dwFlag;
			cRespond.dwArgLen = 4;
		}
		break;

	case HV_OPT_FILELIST:
		{
			cInfo.nType = INFO_FILELIST;
			cInfo.Info.FileList.dwFlag = dwOptID;
			cInfo.Info.FileList.pListBuf = rgRespond;
			cRespond.nResult = GetSysInfo( &cInfo );
			cRespond.dwArgLen = cInfo.Info.FileList.nLen;
		}
		break;

	default:
		cRespond.nResult = E_NOTIMPL;
		cRespond.dwArgLen = 0;
		break;
	}

	if ( FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	if( cRespond.dwArgLen != 0 )
	{
		if ( FAILED(pCmdLink->SendData(rgRespond, cRespond.dwArgLen, NULL)))
		{
			return NET_FAILED;
		}
	}

	return S_OK;
}

int GetConnectedIPCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen != 0 ) return E_INVALIDARG;

	static const int iMaxConnect = 10;
	DWORD32 dwIPCount = 0;
	DWORD32 dwaIP[iMaxConnect] = {0};
	DWORD32 dwaType[iMaxConnect] = {0};

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = GetConnectedIP(&dwIPCount, dwaIP, dwaType);
	cRespond.dwArgLen = 4 + 4 * 8;

	if ( FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	if ( FAILED(pCmdLink->SendData( &dwIPCount, 4, NULL )))
	{
		return NET_FAILED;
	}

	if ( FAILED(pCmdLink->SendData( dwaIP, MAX_CONNECT_COUNT * 4, NULL )))
	{
		return NET_FAILED;
	}

	if ( FAILED(pCmdLink->SendData( dwaType, MAX_CONNECT_COUNT * 4, NULL )))
	{
		return NET_FAILED;
	}

	return S_OK;
}

int GetIniFileCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	CFastMemAlloc cStack;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(PARAM_FILE_LEN + 1, FALSE);
	if ( pBuf == NULL ) return E_OUTOFMEMORY;
	HV_memset(pBuf, 0, PARAM_FILE_LEN + 1);

	UINT nLen = PARAM_FILE_LEN;

	HV_CMD_RESPOND cRespond;
	cRespond.nResult = GetIniFile((char*)pBuf, &nLen);
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.dwArgLen = (DWORD32)nLen;

	if ( FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	if ( FAILED(pCmdLink->SendData( pBuf, cRespond.dwArgLen, NULL ) ) )
	{
		return NET_FAILED;
	}

	return S_OK;
}

int SetIniFileCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen < 4 ) return E_INVALIDARG;

	DWORD32 dwIniFileLen = 0;

	if ( FAILED( pCmdLink->ReceiveData( &dwIniFileLen, 4, NULL ) ) ||
		dwIniFileLen != (pCmdInfo->dwArgLen - 4)	)
	{
		return NET_FAILED;
	}

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.dwArgLen = 0;

	CFastMemAlloc cStack;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(dwIniFileLen + 1, FALSE);
	if (pBuf == NULL) return E_OUTOFMEMORY;
	HV_memset(pBuf, 0, dwIniFileLen + 1);

	if ( FAILED(pCmdLink->ReceiveData(pBuf, dwIniFileLen, NULL)))
	{
		return NET_FAILED;
	}
	cRespond.nResult = SetIniFile((char*)pBuf, dwIniFileLen);

	if ( FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}
	return S_OK;
}

int ResetCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen != 4 ) return E_INVALIDARG;

	//接收参数数据
	DWORD32 dwResetMode;
	if ( FAILED( pCmdLink->ReceiveData( &dwResetMode, 4, NULL) ) )
	{
		return NET_FAILED;
	}

	//执行操作,填写回应信息
	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = S_OK;
	cRespond.dwArgLen = 0;

	//传送回应信息
	HRESULT hr = pCmdLink->SendRespond(&cRespond);

    //考虑到复位的时间也许较长，所以先发回应，然后再真正实现复位操作。
    //attendtion: 这个机制需要注意。
	ResetHv(dwResetMode);

	return SUCCEEDED(hr)?S_OK:NET_FAILED;
}

int SetTimeCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;

	if ( pCmdInfo->dwArgLen != 8 ) return E_INVALIDARG;

    //申请内存
    CFastMemAlloc cStack;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(pCmdInfo->dwArgLen, FALSE);
	if (pBuf == NULL) return E_OUTOFMEMORY;
	memset(pBuf, 0, pCmdInfo->dwArgLen);

	//接收参数数据
	if ( FAILED( pCmdLink->ReceiveData( pBuf, 8, NULL) ) )
	{
		return NET_FAILED;
	}

	DWORD32* rgTime = (DWORD32*)pBuf;

	//执行操作,填写回应信息
	SYS_INFO cInfo;
	cInfo.nType = INFO_SYS_TIME;
	cInfo.Info.SysTime.dwTimeLow = rgTime[0];
	cInfo.Info.SysTime.dwTimeHigh = rgTime[1];

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = SetSysInfo(&cInfo);
	cRespond.dwArgLen = 0;

	//传送回应信息
	if (FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

//强制出大图
int ForceSendCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
	if ( pCmdInfo == NULL || pCmdLink == NULL ) return E_INVALIDARG;
	if( pCmdInfo->dwArgLen != 0 &&  pCmdInfo->dwArgLen != 4 ) return E_INVALIDARG;

	DWORD32 dwVideoID = 0;
	if ( pCmdInfo->dwArgLen == 4
		&& FAILED( pCmdLink->ReceiveData( &dwVideoID, pCmdInfo->dwArgLen, NULL ) ) )
	{
		return NET_FAILED;
	}

	//g_dwSignalTime = GetSystemTick();

	HV_CMD_RESPOND cRespond;
	cRespond.dwFlag = 0;
	cRespond.dwCmdID = pCmdInfo->dwCmdID;
	cRespond.nResult = ForceSend(dwVideoID);
	cRespond.dwArgLen = 0;

	if ( FAILED(pCmdLink->SendRespond(&cRespond)))
	{
		return NET_FAILED;
	}

	return S_OK;
}

// 命令
CMD_MAP_ITEM g_CmdFuncMap[] =
{
	//公有命令
	{ GET_TIME_COMMAND, GetTimeCommand },
	{ SET_TIME_COMMAND, SetTimeCommand },
	{ GET_ID_COMMAND, GetIDCommand },
	{ SET_SAFEMODE_COMMAND, SetSafeMode },
	{ SET_FILTER_COMMAND, SetFilterCommand },
	{ GET_VER_COMMAND, GetVersionCommand },
	{ GET_VERSION_COMMAND, GetVersionCommandString},
	{ GET_VIDEO_NAME_COMMAND, GetVideoNameCommand },
	{ GET_OPTION_COMMAND, GetOptionCommand },
	{ SET_OPTION_COMMAND, SetOptionCommand },
	{ GET_CONNECTED_IP, GetConnectedIPCommand },
	{ GET_INIFILE_COMMAND, GetIniFileCommand },
	{ SET_INIFILE_COMMAND, SetIniFileCommand },
	{ RESET_COMMAND, ResetCommand },
	{ FORCE_SEND_COMMAND, ForceSendCommand },
};

static int g_nCmdItemCount = ARRSIZE(g_CmdFuncMap);

HRESULT ExecuteCommand( HV_CMD_INFO* pCmdInfo, ICmdDataLink* pCmdLink )
{
    int nCmdth = -1;///

	HV_CMD_FUNC fp = NULL;

	for ( int i = 0; i < g_nCmdItemCount; i++)
	{
		if ( g_CmdFuncMap[i].dwCmdID == pCmdInfo->dwCmdID )
		{
			fp = g_CmdFuncMap[i].fpCmdFunc;
			nCmdth = i;///
			break;
		}
	}
	if (fp == NULL)
	{
	    HV_Trace(5, "dwCmdID [0x%04x] No exist!\n", pCmdInfo->dwCmdID);///
	    return E_NOTIMPL;
	}

	int iRet = (*fp)(pCmdInfo, pCmdLink);
	if ( FAILED(iRet) )
	{
	    HV_Trace(5, "%dth Command exec failed\n", nCmdth);
	}

	return iRet;

	//return (*fp)(pCmdInfo, pCmdLink);
}

HRESULT InitCtrlCommand(IResultIO* pResultIO)
{
    assert( NULL != pResultIO );
	g_pIResultIO = pResultIO;

	return S_OK;
}
