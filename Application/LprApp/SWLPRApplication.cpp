#include <unistd.h>
#include "swpa.h"
#include "SWFC.h"
#include "SWLPRApplication.h"
#include "SWFilterStruct.h"
#include "SWCameraControlParameter.h"
#include "SWGB28181Parameter.h"
#include "SWOnvifRtspParameter.h"
#include "SWCarLeft.h"
#include "SWNetOpt.h"
#include <time.h>



/*
int IntegralPreprocessForCompactA(
	unsigned int* pdwItgImage,
	unsigned int* pdwItgSqImage,
	unsigned short wItgWidth,
	unsigned short wItgHeight,
	unsigned short nItgStrideWidth,
	unsigned char* pImageData
)
{
	unsigned int* pdwItgOrigin;
	unsigned int* pdwFastItg;
	unsigned int* pdwFastItgUp;
	unsigned int* pdwItgSqOrigin;
	unsigned int* pdwFastItgSq;
	unsigned int* pdwFastItgSqUp;
	unsigned int dwItgValue, dwItgSqValue;
	unsigned char* pbImageOrigin;
	unsigned char bTemp;
	int i, j;
	int iSrcStep;

	if (pdwItgImage == NULL || pdwItgSqImage == NULL)
	{
		return 1;
	}

	pdwFastItg = (unsigned int*)malloc(nItgStrideWidth * sizeof(unsigned int));
	pdwFastItgUp = (unsigned int*)malloc(nItgStrideWidth * sizeof(unsigned int));
	pdwFastItgSq = (unsigned int*)malloc(nItgStrideWidth * sizeof(unsigned int));
	pdwFastItgSqUp = (unsigned int*)malloc(nItgStrideWidth * sizeof(unsigned int));

	pbImageOrigin = pImageData;
	iSrcStep = 1;

	pdwItgOrigin = pdwItgImage;
	pdwItgSqOrigin = pdwItgSqImage;

	wItgWidth += 1;
	wItgHeight += 1;

	// 将积分图的第一行数据置为0.
	memset( (void*)pdwFastItgUp, 0, wItgWidth * sizeof( unsigned int ) );
	memset( (void*)pdwFastItgSqUp, 0, wItgWidth * sizeof( unsigned int ) );

    memcpy((unsigned char*)pdwItgOrigin, (unsigned char*)pdwFastItgUp, wItgWidth * sizeof(unsigned int));
    memcpy((unsigned char*)pdwItgSqOrigin, (unsigned char*)pdwFastItgSqUp, wItgWidth * sizeof(unsigned int));

	pdwItgOrigin += nItgStrideWidth;
	pdwItgSqOrigin += nItgStrideWidth;

	dwItgValue = 0;
	dwItgSqValue = 0;

	unsigned int* pdwTemp1;
	unsigned int* pdwFastItgTemp1;
	unsigned int* pdwFastItgSqTemp1;
	unsigned int* pdwFastItgUpTemp;
	unsigned int* pdwFastItgSqUpTemp;
	unsigned int dwTemp1;//, dwTemp2;
	unsigned char bTemp1, bTemp2, bTemp3, bTemp4;

	unsigned short wWidthNew = wItgWidth;
	if (wItgWidth & 3 )
	{
		wWidthNew = (wItgWidth & ~3);
	}
	// 进行积分图运算
	for ( i = 1; i < wItgHeight; ++ i )
	{
		pdwFastItg[0] = 0;
		pdwFastItgSq[0] = 0;

		pdwTemp1 = ( unsigned int* )pbImageOrigin;
		pdwFastItgTemp1 = pdwFastItg;
		pdwFastItgSqTemp1 = pdwFastItgSq;
		pdwFastItgUpTemp = pdwFastItgUp;
		pdwFastItgSqUpTemp = pdwFastItgSqUp;

		for ( j = 1; j < wWidthNew; j += 4 )
		{
			dwTemp1 = *( pdwTemp1 ++ );
			//dwTemp2 = *( pdwTemp1 ++ );

			bTemp1 = ( dwTemp1 ) & 0xFF;
			dwItgValue += bTemp1;
			dwItgSqValue += bTemp1 * bTemp1;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);

			bTemp2 = ( dwTemp1 >> 8 ) & 0xFF;
			dwItgValue += bTemp2;
			dwItgSqValue += bTemp2 * bTemp2;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);

			bTemp3 = ( dwTemp1 >> 16 ) & 0xFF;
			dwItgValue += bTemp3;
			dwItgSqValue += bTemp3 * bTemp3;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);

			bTemp4 = ( dwTemp1 >> 24 ) & 0xFF;
			dwItgValue += bTemp4;
			dwItgSqValue += bTemp4 * bTemp4;
			*(++pdwFastItgTemp1) = dwItgValue + *(++pdwFastItgUpTemp);
			*(++pdwFastItgSqTemp1) = dwItgSqValue + *(++pdwFastItgSqUpTemp);
		}
		for ( ; j < wItgWidth; ++ j )
		{
			bTemp = pbImageOrigin[(j - 1) * iSrcStep];
			dwItgValue += bTemp;
			dwItgSqValue += bTemp * bTemp;

			pdwFastItg[j] = dwItgValue + pdwFastItgUp[j];
			pdwFastItgSq[j] = dwItgSqValue + pdwFastItgSqUp[j];
		}

		unsigned int* pdwTemp1 = pdwFastItgUp;
		unsigned int* pdwTemp2 = pdwFastItgSqUp;
		pdwFastItgUp = pdwFastItg;
		pdwFastItgSqUp = pdwFastItgSq;
		pdwFastItg = pdwTemp1;
		pdwFastItgSq = pdwTemp2;

		memcpy((unsigned char*)pdwItgOrigin, (unsigned char*)pdwFastItgUp, wItgWidth * sizeof(unsigned int));
		memcpy((unsigned char*)pdwItgSqOrigin, (unsigned char*)pdwFastItgSqUp, wItgWidth * sizeof(unsigned int));

		pdwItgOrigin += nItgStrideWidth;
		pdwItgSqOrigin += nItgStrideWidth;
		pbImageOrigin += wItgWidth;

		dwItgValue = 0;
		dwItgSqValue = 0;
	}

	free(pdwFastItg);
	free(pdwFastItgUp);
	free(pdwFastItgSq);
	free(pdwFastItgSqUp);

	return( 0 );
}
*/

CREATE_PROCESS(CSWLPRApplication)
CSWLPRApplication::CSWLPRApplication():CSWMessage(MSG_APP_START, MSG_APP_END)
{
	CALL_MESSAGE_REGISTER_FUNCTION();
	m_pParam = NULL;
	m_hObject = swpa_map_create(OnMSGCompare);
	memset( m_rgdwNC, 0, sizeof(DWORD) * 4 );
	m_fDSPTrigger = TRUE;
    m_iFlashDiffFlag = 0;

	m_fHardDiskReady = FALSE;
	m_fNetDiskMounted = FALSE;
	swpa_memset(m_szHDDPath, 0, sizeof(m_szHDDPath));
	m_dwDevType = DEVTYPE_VENUS;

 	m_MaxSdErrNum = 5;
	m_SdErrNum = 0;
	m_fIsAppReady = FALSE;
}

CSWLPRApplication::~CSWLPRApplication()
{
}

HRESULT CSWLPRApplication::InitInstance(const WORD wArgc, const CHAR** szArgv)
{		
	SW_TRACE_NORMAL("start application initialize...\n");	

	// 共享内存提前初始化
	//CSWMemoryFactory* pMemoryFactoryTemp = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
/*
	unsigned int uWidth = 1920;
	unsigned int uHeight = 1080;
	unsigned int* pbA = (unsigned int*)malloc((uWidth + 1) * (uHeight + 1) * sizeof(unsigned int));
	unsigned int* pbB = (unsigned int*)malloc((uWidth + 1) * (uHeight + 1) * sizeof(unsigned int));
	unsigned char* pbData = (unsigned char*)malloc((uWidth + 1) * (uHeight + 1));

	SW_TRACE_DEBUG("=======<<<<<<<<<<<<  IntegralPreprocessForCompact >>>>>>>>>>============");

	if( pbA != NULL && pbB != NULL && pbData != NULL )
	{
		unsigned int uTimeMs = CSWDateTime::GetSystemTick();
		IntegralPreprocessForCompactA(pbA,pbB,uWidth,uHeight,uWidth+1, pbData);
		SW_TRACE_DEBUG("IntegralPreprocessForCompactA relay : %d.",
				CSWDateTime::GetSystemTick() - uTimeMs);
		free(pbA);
		free(pbB);
		free(pbData);
	}
	else
	{
		SW_TRACE_DEBUG("malloc failed!!!!!!!!!!");
	}
	SW_TRACE_DEBUG("=======<<<<<<<<<<<<  IntegralPreprocessForCompact end >>>>>>>>>>=========");
*/

	HRESULT hr = CSWApplication::InitInstance(wArgc, szArgv);
	if(FAILED(hr))
	{
		return hr;
	}
	WriteResetInfo();
	if(NULL == GetCommandString("-test"))
	{
		INT iMode = 0;
		if(!swpa_device_get_resetmode(&iMode) && iMode == 2)
		{
			SW_TRACE_NORMAL("current state is upgrade, exit and run upgrade mode");
			Exit(1);
			return E_FAIL;
		}
	}

	//初始化脚本
	if(NULL != GetCommandString("-p"))
	{
		swpa_utils_shell("echo 2 > /proc/sys/vm/overcommit_memory", NULL);
		swpa_utils_shell("echo 70 > /proc/sys/vm/overcommit_ratio", NULL);
		swpa_utils_shell("echo 3 > /proc/cpu/alignment", NULL);
		swpa_utils_shell("/etc/reloadko.sh", NULL);
	}

	//初始化类工厂
	ClassInitialize();
	//初始化本地类工厂
	LOAD_CLASS(CSWLPRParameter);
	LOAD_CLASS(CSWLPREPoliceParameter);
	LOAD_CLASS(CSWLPRCapFaceParameter);
	LOAD_CLASS(CSWLPRTollGateParameter);
	LOAD_CLASS(CSWLPRHvcParameter);

	LOAD_CLASS(CSWLPRVenusTollGateParameter);
	LOAD_CLASS(CSWLPRVenusTrafficGateParameter);
	LOAD_CLASS(CSWLPRVenusEPoliceParameter);
	LOAD_CLASS(CSWLPRVenusTrafficGateEC700Parameter);
	LOAD_CLASS(CSWEPoliceCarLeft);
	LOAD_CLASS(CSWTrafficGateCarLeft);
	LOAD_CLASS(CSWTollGateCarLeft);
	LOAD_CLASS(CSWPeccancyParkingCarLeft);
	LOAD_CLASS(CSWHvcCarLeft);
    LOAD_CLASS(SWLPRVenusCameraParameter);
	LOAD_CLASS(CSWLPRJupiterDomeCameraParameter);

	//通过命令行设定设备类型
	CSWString strDevType = GetCommandString("-devtype");
	if(!strDevType.IsEmpty())
	{
	
		if (0 == swpa_strcmp((LPCSTR)strDevType, "Venus"))
		{
			m_dwDevType = DEVTYPE_VENUS;
		}
		else if (0 == swpa_strcmp((LPCSTR)strDevType, "Jupiter"))
		{
			m_dwDevType = DEVTYPE_JUPITER;
		}
	}
	
	//解析命令行参数
	CSWString strXML = GetCommandString("-i", "./LprApp.xml");
	m_xmlDoc.LoadFile(strXML);
	if(NULL == m_xmlDoc.RootElement())
	{
		SW_TRACE_DEBUG("%s is an invalid xml file\n", (LPCSTR)strXML);
		return E_FAIL;
	}

	InitAT88();
		
	//初始化工作参数
	if(FAILED(InitParameter()))
	{
		SW_TRACE_DEBUG("InitParameter failed\n");
		return E_FAIL;
	}
	
	//设置日志输出等级
	//CSWLog::SetLevel(GetParam().Get().nLogLevel);
	
	//NTP时间同步
	NtpTimeSync();
	
	//设置时区
	OnSetNTPTimeZone((WPARAM)GetParam().Get().cResultSenderParam.iTimeZone, 0);
	
	//初始化xml表中的替换规则
	if(FAILED(InitVariant()))
	{
		SW_TRACE_DEBUG("InitVariant failed\n");
		return E_FAIL;
	}
	
	//通过命令行修改设备IP
	CSWString strIP = GetCommandString("-IP");
	if(!strIP.IsEmpty())
	{
		swpa_tcpip_setinfo("eth0", (LPCSTR)strIP, 
			GetParam().Get().cTcpipCfgParam.szNetmask, GetParam().Get().cTcpipCfgParam.szGateway);    
	}
	
	//通过命令行修改摄像机IP
	strIP = GetCommandString("-c");
	if(!strIP.IsEmpty())
	{
		swpa_strcpy(GetParam().Get().cCamCfgParam.szIP, (LPCSTR)strIP);
	}


	// zhaopy
	// 每次启动都需要RESET FPGA
    //FPGA的复位由load程序复位，只有在加载时才复位，不加载无需复位
	if( GetParam().Get().nWorkModeIndex == PRM_ELECTRONIC_POLICE )
	{
		INT iRet = swpa_device_reset(SWPA_DEVICE_CAMERA_FPGA);
		SW_TRACE_DEBUG("<LPRApplication>Reset fpga return = %d.\n", iRet);
		swpa_thread_sleep_ms(100);
	}

	HeartBeat();
#ifdef DM8127



	int imx = ( GetParam().Get().cCamCfgParam.iCamType != 0 )?JPEG_DEC:GetParam().Get().nMCFWUsecase;

	IPNCCreateConfig IPNCConfig;
	INT iResolution = GetParam().Get().cCamAppParam.iResolution;
	INT iResolutionSecond = GetParam().Get().cCamAppParam.iResolutionSecond;
	INT iMaxBitRate = (GetParam().Get().cCamAppParam.iMaxBitRate) << 10;
	INT iMaxBitRateSecond = (GetParam().Get().cCamAppParam.iMaxBitRateSecond) << 10;
	INT iSecondVideoFrameRate = GetParam().Get().cCamAppParam.iFrameRateSecond;
	if(iMaxBitRate < GetParam().Get().cCamAppParam.iTargetBitRate)
		iMaxBitRate = GetParam().Get().cCamAppParam.iTargetBitRate;
	if(iMaxBitRateSecond < GetParam().Get().cCamAppParam.iTargetBitRateSecond)
		iMaxBitRateSecond = GetParam().Get().cCamAppParam.iTargetBitRateSecond;
	iResolutionSecond = (iResolutionSecond < iResolution) ? iResolution : iResolutionSecond;

    SW_TRACE_DEBUG("<LPRApplication> init videoConfig, iMaxBitRate = %d, GetParam().Get().cCamAppParam.iTargetBitRate = %d.\n",
                   iMaxBitRate, GetParam().Get().cCamAppParam.iTargetBitRate);

	swpa_memset(&IPNCConfig, 0, sizeof(IPNCCreateConfig));
	IPNCConfig.imx 				= imx;
	IPNCConfig.videoConfig		= (0 == iResolution)?SWPA_VIDEO_RES_1080P:SWPA_VIDEO_RES_720P;
	IPNCConfig.videoConfigSecond= iResolutionSecond;
	IPNCConfig.secondVideoFrameRate = iSecondVideoFrameRate;
	IPNCConfig.verbose		    = GetParam().Get().cCamCfgParam.iIPNCLogOutput;
	IPNCConfig.maxBitRate       = iMaxBitRate;
	IPNCConfig.secondMaxBitRate = iMaxBitRateSecond;

	swpa_ipnc_create(&IPNCConfig);
	// swpa_ipnc_create(imx
	// 	,(0 == iResolution)?SWPA_VIDEO_RES_1080P:SWPA_VIDEO_RES_720P
	// 	,(iResolutionSecond < iResolution) ? iResolution : iResolutionSecond
	// 	,GetParam().Get().cCamAppParam.iFrameRateSecond
	// 	,GetParam().Get().cCamCfgParam.iIPNCLogOutput);

	//todo:临时做法，防止升级不同的前端版本
	//保存前端类型和前端类型校验
	INT iSensorType = swpa_get_sensor_type();

	INT iSensorTypeInFile = 0;
	SW_TRACE_NORMAL("Check Sensor !!!!!!!!!!!!!!!!!!!!!!!!!\n");

	CHAR *szSensorNameMap[] = {"IMX178","IMX185","IMX174","IMX249", "ICX816"};
#define GET_SENSOR_NAME(ID) ((0<=ID && ID<SENSOR_MAX)?szSensorNameMap[ID]:"Unknown")
	if (FAILED( ReadSensorType(&iSensorTypeInFile)))
	{
		SW_TRACE_NORMAL("The first save sensor [%s] type to file\n",GET_SENSOR_NAME(iSensorType));
		if (FAILED(SaveSensorType(iSensorType)))
		{
			SW_TRACE_NORMAL("**********Save Sensor Type File Failed*********\n");
		}
	}
	else
	{
		if (iSensorType != iSensorTypeInFile)
		{
			SW_TRACE_NORMAL("###### Sensor is [%s],But fpga program support [%s] only #####",
				GET_SENSOR_NAME(iSensorTypeInFile),GET_SENSOR_NAME(iSensorType));
			OnResetDevice((WPARAM)3,0);//boot to backup system
		}
		else
		{
			SW_TRACE_NORMAL("Sensor type [%s] check OK!",GET_SENSOR_NAME(iSensorType));
		}
	}
#endif

	HeartBeat();

	//通过命令行指定存储设备的路径
	CSWString strHDDPath = GetCommandString("-hdd", "/dev/sd*");
	if(!strHDDPath.IsEmpty())
	{
		swpa_strcpy(m_szHDDPath, (LPCSTR)strHDDPath);
	}

	
	//初始化程序用到的各种对象
	if(FAILED(InitObject()))
	{
		SW_TRACE_DEBUG("InitObject failed\n");
		return E_FAIL;
	}
	
	HeartBeat();

	//初始化硬盘
	if (FAILED(InitHdd()))
	{
		SW_TRACE_DEBUG("InitHdd failed\n");
		return E_FAIL;
	}
	//如果格式化硬盘，则忽略下面的Filter初始化和挂接
	if(GetParam().Get().cResultSenderParam.fInitHdd)
	{
		//只支持对设备上的固态存储介质做分区和格式化
		if (2 == GetParam().Get().cResultSenderParam.iDiskType) // "2" means HDD
		{
			return S_OK;
		}
		else
		{
			GetParam().UpdateEnum("\\HvDsp\\Misc", "InitHdd", 0);
			GetParam().Commit();
			GetParam().Get().cResultSenderParam.fInitHdd = 0;
		}
	}
	
	HeartBeat();

	//初始化数据流模块
	if(FAILED(InitFilter()))
	{
		SW_TRACE_DEBUG("InitFilter failed\n");
		return E_FAIL;
	}
	// 判断是否需要恢复出厂默认。
	// 目前此状态没法自动恢复。todo.
	if( swpa_device_get_reset_flag() == 0 )
	{
		SW_TRACE_DEBUG("<LPRApplication>reset factory flag == 0.\n");
		return OnRestoreFactory(0,0);
	}
	//设置sd卡储存出错记录队列
	m_lstSdErrRecord.SetMaxCount(m_MaxSdErrNum);
	//执行Filter
	return CSWBaseGraph::Run();
}

HRESULT CSWLPRApplication::InitAT88()
{
	// 生成AT88 NC
	swpa_utils_srand(CSWDateTime::GetSystemTick());
	for(int i = 0; i < 4; ++i)
	{
		m_rgdwNC[i] = swpa_utils_rand();
	}
	UINT uWrite = 7;
	if( 0 == swpa_device_crypt_init() 
		&& 0 == swpa_device_crypt_write(0x0, (BYTE*)m_rgdwNC, &uWrite) )
	{
		SW_TRACE_DEBUG("<App>Init AT88 ok.");
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CSWLPRApplication::VerifyCrypt()
{
	HRESULT hr = S_OK;

	// 认证
	static int iCheckCount = 0;
	static int iVerifyCrypt = 0;
	static const int VERIFY_CRYPT_PARAM = 518400;
	swpa_utils_srand(CSWDateTime::GetSystemTick());
	iCheckCount++;
	if ( iCheckCount >= iVerifyCrypt )
	{
		SW_TRACE_DEBUG("start <App>Check AT88.");
		BYTE rgbNc[7];
		UINT uRead = 7;
		INT iRet = swpa_device_crypt_read(0x0, rgbNc, &uRead);
		if( 0 == iRet
			&& swpa_memcmp((BYTE*)m_rgdwNC, (BYTE*)rgbNc, 7) == 0 )
		{
			SW_TRACE_DEBUG("<App>Check AT88 ok.");
		}
		else if(!swpa_utils_file_exist("dog.txt"))
		{
			SW_TRACE_DEBUG("<App>Check AT88 failed! reset.");
			hr = E_FAIL;
		}

		iCheckCount = 0;
		iVerifyCrypt = ( rand() * VERIFY_CRYPT_PARAM / RAND_MAX );
		iVerifyCrypt += VERIFY_CRYPT_PARAM;
	}
	return hr;
}

HRESULT CSWLPRApplication::CheckRebootEvent(void)
{
	if (FALSE == GetParam().Get().cAutoRebootParam.fEnable)
	{
		return S_OK;
	}
	
	//车辆统计
#define CAR_COUNT_TM 10

	static DWORD dwCarCountPMin[CAR_COUNT_TM] = {0};
	static INT iIndex = 1;
	static DWORD dwLast10MinCarCount = 0;
	static DWORD dwTotalCarCountLast = 0;
	
	DWORD dwMinuteCur = CSWDateTime::GetSystemTick()/1000/60;
	static DWORD dwMinuteLast = dwMinuteCur;
	if (dwMinuteCur != dwMinuteLast)		/*严格一分钟统计一次*/
	{
		DWORD dwTotalCarCount = 0;
		CSWMessage::SendMessage(MSG_RECOGNIZE_GET_CARLEFT_COUNT,0,(LPARAM)&dwTotalCarCount);
		DWORD dwThisMinCarCount = dwTotalCarCount - dwTotalCarCountLast;
		DWORD dwFirstMinCarCount = (iIndex > 9)?dwCarCountPMin[(iIndex -10)%CAR_COUNT_TM]:0;	//第十分钟前的一分钟内的车辆数
		dwCarCountPMin[iIndex%CAR_COUNT_TM] = dwThisMinCarCount;
		dwLast10MinCarCount += (dwThisMinCarCount - dwFirstMinCarCount);

		dwTotalCarCountLast = dwTotalCarCount;
		dwMinuteLast = dwMinuteCur;
		iIndex++;
		
		BOOL fIsNight = 0;
		static INT iIsNightCount = 0;
		CSWMessage::SendMessage(MSG_AUTO_CONTROL_GET_ENVPERIOD,0,(LPARAM)&fIsNight);
		iIsNightCount = (TRUE == fIsNight) ? (iIsNightCount+1) : 0;		/*确保是夜间*/
		
		//夜晚判断条件，自动控制模块和系统时间
		if (iIsNightCount > 6*60 /*进入夜晚六个小时后一般认为车会比较少*/
			&& (dwMinuteCur/60 > GetParam().Get().cAutoRebootParam.iSysUpTimeLimit))/*超过运行时限*/
		{
			if (dwLast10MinCarCount < GetParam().Get().cAutoRebootParam.iCarLeftCountLimit/*总车辆数小于门限arg*/ 
				&& (dwThisMinCarCount <= dwLast10MinCarCount/CAR_COUNT_TM))/*最近一分钟小于等于平均值*/
			{
				//retboot system
				SW_TRACE_NORMAL("Info: ==========Reboot System=============\n");
				SW_TRACE_NORMAL("uptime:%dh%m,Traffic info:last ten min %d,last min %d\n",
					dwMinuteCur/60,dwMinuteCur%60,dwLast10MinCarCount,dwThisMinCarCount);
				OnResetDevice((WPARAM)2,0);
			}
			else if (1/*总运行时间大于多长*/)
			{
				//reboot system
				//OnResetDevice((WPARAM)2,0);
				SW_TRACE_DEBUG("Warning: Should be reboot system,"
				"but traffic busy.Car <%d,%d>\n",dwLast10MinCarCount,dwThisMinCarCount);
			}
		}
#if 1		//debug info
		CSWString strDebugInfo;
		strDebugInfo.Format("Traffic Info: ");
		for (INT i=iIndex; i<CAR_COUNT_TM+iIndex; i++)
		{
			CHAR szInfo[64];
			swpa_sprintf(szInfo,"%d ",dwCarCountPMin[i%CAR_COUNT_TM]);
			strDebugInfo.Append(szInfo);
		}
		SW_TRACE_DEBUG("-- %s <%s> dwLast10MinCarCount:%d--",
			(const CHAR *)strDebugInfo,(TRUE == fIsNight)?"Night":"Day",dwLast10MinCarCount);
#endif	
	}

	return S_OK;
}

HRESULT CSWLPRApplication::WriteResetInfo()
{
	CSWFile cFile;
	BOOL fInit = FALSE;
	DWORD dwMaxCount = 30;
	DWORD dwOneRecordSize = 32;

	if( S_OK == cFile.Open("EEPROM/0/APP_RESET_INFO", "w+") )
	{
		// read header
		if( S_OK == cFile.Read(&m_cResetInfoHeader, sizeof(m_cResetInfoHeader), NULL) )
		{
			// check flag
			if( m_cResetInfoHeader.dwFlag == RESET_INFO_FLAG )
			{
				UINT uiCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);
				if( uiCRC != m_cResetInfoHeader.dwCRC )
				{
					fInit = TRUE;
				}
			}
			else
			{
				fInit = TRUE;
			}

			if( fInit )
			{
				SW_TRACE_DEBUG("<reset>Init reset info.\n");
				swpa_memset(&m_cResetInfoHeader, 0, sizeof(m_cResetInfoHeader));
				m_cResetInfoHeader.dwFlag = RESET_INFO_FLAG;
				m_cResetInfoHeader.dwCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);
				cFile.Seek(0, SWPA_SEEK_SET, NULL);
				cFile.Write(&m_cResetInfoHeader, sizeof(m_cResetInfoHeader), NULL);
			}

			SW_TRACE_DEBUG("<reset>header.flag:0x%08x,count:%d,head:%d,tail:%d,crc:0x%08x.\n",
				m_cResetInfoHeader.dwFlag, m_cResetInfoHeader.dwCount, 
				m_cResetInfoHeader.dwHead, m_cResetInfoHeader.dwTail, m_cResetInfoHeader.dwCRC);

			PBYTE pbData = (PBYTE)swpa_mem_alloc(1024);
			if( pbData != NULL )
			{
				INT index = 0;
				swpa_memset(pbData, 0, 1024);
				m_cResetInfoHeader.dwCount++;
				DWORD dwPos = m_cResetInfoHeader.dwTail % dwMaxCount;
				m_cResetInfoHeader.dwTail++;
				m_cResetInfoHeader.dwTail = m_cResetInfoHeader.dwTail % dwMaxCount;
				if( m_cResetInfoHeader.dwHead == m_cResetInfoHeader.dwTail )
				{
					m_cResetInfoHeader.dwHead++;
					m_cResetInfoHeader.dwHead = m_cResetInfoHeader.dwHead % dwMaxCount;
				}
				m_cResetInfoHeader.dwCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);

				swpa_memcpy(pbData, &m_cResetInfoHeader, sizeof(m_cResetInfoHeader));
				index += sizeof(m_cResetInfoHeader);

				cFile.Read(pbData + index, 1024 - sizeof(m_cResetInfoHeader), NULL);

				CSWDateTime cTime;
				CHAR szInfo[32] = {0};
				swpa_sprintf(szInfo, "%08d:%04d/%02d/%02d %02d:%02d:%02d", 
					m_cResetInfoHeader.dwCount, cTime.GetYear(), cTime.GetMonth(), 
					cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
				
				DWORD dwOffset = sizeof(m_cResetInfoHeader) + dwPos * dwOneRecordSize;
				swpa_memcpy(pbData + dwOffset, szInfo, dwOneRecordSize);

				cFile.Seek(0, SWPA_SEEK_SET, NULL);
				cFile.Write(pbData, 1024, NULL);

				swpa_mem_free(pbData);
			}
		}
		cFile.Close();
	}
	else
	{
		SW_TRACE_DEBUG("<reset>open file failed.\n");
	}

	return S_OK;
}
INT CSWLPRApplication::GetResetCount()
{
	CSWFile cFile;
	int iResetCount = 0;
	BOOL fInit = FALSE;
	if( S_OK == cFile.Open("EEPROM/0/APP_RESET_INFO", "r") )
	{
		// read header
		if( S_OK == cFile.Read(&m_cResetInfoHeader, sizeof(m_cResetInfoHeader), NULL) )
		{
			// check flag
			if( m_cResetInfoHeader.dwFlag == RESET_INFO_FLAG )
			{
				UINT uiCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);
				if( uiCRC == m_cResetInfoHeader.dwCRC )
				{
					SW_TRACE_DEBUG("<reset>header.flag:0x%08x,count:%d,head:%d,tail:%d,crc:0x%08x.\n",
						m_cResetInfoHeader.dwFlag, m_cResetInfoHeader.dwCount, 
						m_cResetInfoHeader.dwHead, m_cResetInfoHeader.dwTail, m_cResetInfoHeader.dwCRC);
					iResetCount = m_cResetInfoHeader.dwCount;
				}
				else
				{
					fInit = TRUE;
				}
			}
			else
			{
				fInit = TRUE;
			}

			// 读取时不初始化。
			/*
			if( fInit )
			{
				SW_TRACE_DEBUG("<reset>Init reset info.\n");
				swpa_memset(&m_cResetInfoHeader, 0, sizeof(m_cResetInfoHeader));
				m_cResetInfoHeader.dwFlag = RESET_INFO_FLAG;
				m_cResetInfoHeader.dwCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);
				cFile.Seek(0, SWPA_SEEK_SET, NULL);
				cFile.Write(&m_cResetInfoHeader, sizeof(m_cResetInfoHeader), NULL);
			}*/
		}
		cFile.Close();
	}
	else
	{
		SW_TRACE_DEBUG("<reset>open file failed.\n");
	}
	return iResetCount;
}

CSWString CSWLPRApplication::GetResetInfo()
{
	CSWString strInfo = "";

	CSWFile cFile;
	BOOL fInit = FALSE;
	DWORD dwMaxCount = 30;
	DWORD dwOneRecordSize = 32;

	if( S_OK == cFile.Open("EEPROM/0/APP_RESET_INFO", "r") )
	{
		// read header
		if( S_OK == cFile.Read(&m_cResetInfoHeader, sizeof(m_cResetInfoHeader), NULL) )
		{
			// check flag
			if( m_cResetInfoHeader.dwFlag == RESET_INFO_FLAG )
			{
				UINT uiCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);
				if( uiCRC != m_cResetInfoHeader.dwCRC )
				{
					fInit = TRUE;
				}
			}
			else
			{
				fInit = TRUE;
			}

			if( fInit )
			{
				// 读取时不初始化。
				/*
				SW_TRACE_DEBUG("<reset>Init reset info.\n");
				swpa_memset(&m_cResetInfoHeader, 0, sizeof(m_cResetInfoHeader));
				m_cResetInfoHeader.dwFlag = RESET_INFO_FLAG;
				m_cResetInfoHeader.dwCRC = CSWUtils::CalcCrc32(0, (PBYTE)(&m_cResetInfoHeader), sizeof(m_cResetInfoHeader) - 4);
				cFile.Seek(0, SWPA_SEEK_SET, NULL);
				cFile.Write(&m_cResetInfoHeader, sizeof(m_cResetInfoHeader), NULL);
				*/
				cFile.Close();
				return strInfo;
			}
			else
			{
				SW_TRACE_DEBUG("<reset>header.flag:0x%08x,count:%d,head:%d,tail:%d,crc:0x%08x.\n",
					m_cResetInfoHeader.dwFlag, m_cResetInfoHeader.dwCount, 
					m_cResetInfoHeader.dwHead, m_cResetInfoHeader.dwTail, m_cResetInfoHeader.dwCRC);
				PBYTE pbData = (PBYTE)swpa_mem_alloc(1024);
				if( pbData != NULL )
				{
					INT index = 0;
					swpa_memset(pbData, 0, 1024);
					index += sizeof(m_cResetInfoHeader);
					cFile.Read(pbData + index, 1024 - sizeof(m_cResetInfoHeader), NULL);
					DWORD dwHead = m_cResetInfoHeader.dwHead;
					DWORD dwTail = m_cResetInfoHeader.dwTail;
					char szInfo[32] = {0};

					while(dwHead != dwTail)
					{
						dwTail += dwMaxCount;
						dwTail--;
						dwTail = dwTail % dwMaxCount;
						DWORD dwOffset = sizeof(m_cResetInfoHeader) + dwTail * dwOneRecordSize;
						
						if( dwOffset > 1024 - dwOneRecordSize - 1)
						{
							break;
						}
						swpa_memcpy(szInfo, pbData + dwOffset, dwOneRecordSize);
						szInfo[dwOneRecordSize] = 0;
						strInfo.Append(szInfo);
						strInfo.Append("\n");
					}

					swpa_mem_free(pbData);
				}
			}
		}
		cFile.Close();
	}

	return strInfo;
}


HRESULT CSWLPRApplication::ReleaseInstance()
{
    /*
	//系统复位的话，直接程序退出，以减小复位时间
	if (2 == CSWApplication::GetExitCode())
	{	
		DeinitHdd();
		
		SW_TRACE_NORMAL("Reboot system quick!!!!!!");
		return 0;
    }*/

	DWORD dwTimes = 0;

	CSWThread cThread;
	cThread.Start(OnReleaseInstance, this);
	dwTimes = 0;
	while(S_OK == cThread.IsValid() && dwTimes++ < 10)
	{
		HeartBeat();
		Sleep(800);
	}
	for(int pos = swpa_map_open(m_hObject); -1 != pos; pos = swpa_map_next(m_hObject))
	{
		CSWString *pstrName = (CSWString *)swpa_map_key(m_hObject, pos);
		CSWObject *obj = (CSWObject *)swpa_map_value(m_hObject, pos);
		pstrName->Release();
		obj->Release();
	}
	swpa_map_close(m_hObject);
	swpa_map_destroy(m_hObject);
	if(NULL != m_pParam)
	{	//todo,删除此变量会导致kill 9,暂时不删除
		//delete m_pParam;
		m_pParam = NULL;
	}


#ifdef DM8127
	//CSWMemoryFactory::FreeAllShareMem(CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY));
	
	SW_TRACE_NORMAL("swpa ipnc close begin!\n");
	swpa_ipnc_close();
	dwTimes = 0;
	while(dwTimes++ < 4)
	{
		HeartBeat();
		Sleep(800);
	}
	SW_TRACE_NORMAL("swpa ipnc close finish!\n");
#endif

	DeinitHdd();
	return CSWApplication::ReleaseInstance();
}

PVOID CSWLPRApplication::OnReleaseInstance(PVOID pvParam)
{
	CSWLPRApplication *pThis = (CSWLPRApplication *)pvParam;
	SendRemoteMessage(MSG_RTSP_APP_RESTART, NULL, 0);		//停止RTSP进程
	pThis->Stop();
	return 0;
}

HRESULT CSWLPRApplication::Run()
{
	SW_TRACE_NORMAL("\nstart running...\n");
	
	DWORD dwTotal, dwFree, dwTimes = 0;
	CSWMemoryFactory* pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
			
#ifdef HAS_DOG		
	swpa_alarm_led_off();
#endif

	//判断各个模块的状态
	INT iLed = 0;
	swpa_alarm_led_off();

	//如果是启动计数已经禁用，不用等待30s即可响应工作模式，在正常复位启动时减少等待时间
	INT iFlag = -1;
	if(SWPAR_OK == swpa_device_get_bootcount_flag(&iFlag)
		&& iFlag == 0)
	{
		m_fIsAppReady = TRUE;
	}
	
	while(!IsExited() && (GetState() == FILTER_RUNNING || GetParam().Get().cResultSenderParam.fInitHdd))
	{	//点灯
#ifdef HAS_DOG
		((++iLed)%2) ? swpa_heart_led_on() : swpa_heart_led_off();
#endif			
		//休眠500MS
		Sleep(500);	
		
		//喂狗
		HeartBeat();
		
		//内存使用情况
		if(!((dwTimes++) % 20))
		{

			pMemoryFactory->GetTotalSize(&dwTotal);
			pMemoryFactory->GetAvailableSize(&dwFree);
			dwTotal /= (1024 * 1024);
			dwFree /= (1024 * 1024);

			CHAR szInfo[256] = {0};
			CHAR szMsg[256] = {0};

			swpa_sprintf(szInfo, "shared memory total:%dM, free:%dM. status:%s.", 
				dwTotal, dwFree, swpa_sharedmem_check() == 0 ? "OK" : "Warning");
			
			SW_TRACE_NORMAL("--- %s ARM-A8 CPU:%d%% Mem:%d%%. ---", szInfo, swpa_utils_get_cpu_usage(), swpa_utils_get_mem_usage());
			swpa_sprintf(szMsg, "系统:%s", szInfo);
			m_strDM6467Status = szMsg;

			CSWString strInfo;
			if( !m_strDM6467Status.IsEmpty() )
			{
				strInfo.Append(m_strDM6467Status);
				strInfo.Append(";\n");
			}
			if( !m_strH264Status.IsEmpty() )
			{
				strInfo.Append(m_strH264Status);
				strInfo.Append(";\n");
			}
			if( !m_strVPIFStatus.IsEmpty() )
			{
				strInfo.Append(m_strVPIFStatus);
				strInfo.Append(";\n");
			}
			if( !m_strRecogStatus.IsEmpty() )
			{
				strInfo.Append(m_strRecogStatus);
				strInfo.Append(";\n");
			}
			if( !m_strResultNetStatus.IsEmpty() )
			{
				strInfo.Append(m_strResultNetStatus);
				strInfo.Append(";\n");
			}
/*
			if( !m_strJpegNetStatus.IsEmpty() )
			{
				strInfo.Append(m_strJpegNetStatus);
				strInfo.Append(";\n");
			}
			if( !m_strH264NetStatus.IsEmpty() )
			{
				strInfo.Append(m_strH264NetStatus);
				strInfo.Append(";\n");
			}

			if( !m_strRTSPConnStatus.IsEmpty() )
			{
				strInfo.Append(m_strRTSPConnStatus);
				strInfo.Append(";\n");
			}
			if( !m_strRTSPSecondConnStatus.IsEmpty() )
			{
				strInfo.Append(m_strRTSPSecondConnStatus);
				strInfo.Append(";\n");
			}
*/
			if( !m_strResultHDDStatus.IsEmpty() )
			{
				strInfo.Append(m_strResultHDDStatus);
				strInfo.Append(";\n");
			}
			if( !m_strH264HDDStatus.IsEmpty() )
			{
				strInfo.Append(m_strH264HDDStatus);
				strInfo.Append(";\n");
			}
			if( !m_strCoilStatus.IsEmpty() )
			{
				strInfo.Append(m_strCoilStatus);
				strInfo.Append(";\n");
			}
			if( !m_strFTPStatus.IsEmpty() )
			{
				strInfo.Append(m_strFTPStatus);
				strInfo.Append(";\n");
			}

			//SW_TRACE_NORMAL("\n=======================================\n%s\n=======================================", (const CHAR*)strInfo);
			m_strRunStatus = strInfo;

			//swpa_ipnc_printState();

		}
		
		if( S_OK != VerifyCrypt() )
		{
			break;
		}
		
	
		//运行30秒之后，置启动次数为0,并禁止启动次数的更新。
		if(dwTimes == 60)
		{
			SetBootCountInfo();
			m_fIsAppReady = TRUE;
		}

//		if( dwTimes % 4 == 0 )
//		{
//			SW_TRACE_DEBUG("<app>running...");
//		}

		//NTP校时
		//NtpTimeSync();
		if(swpa_utils_file_exist("/tmp/ntp_ok"))
		{
			swpa_utils_file_delete("/tmp/ntp_ok");
#if DM6467			
			SendMessage(MSG_DM368_SYNC_TIME,0,0);
#endif
		}

		BOOL fIsInitHdd = FALSE;
		if ((2 == GetParam().Get().cResultSenderParam.iDiskType)
			&& (GetParam().Get().cResultSenderParam.fInitHdd))
		{
			fIsInitHdd = TRUE;
			if (2 == swpa_hdd_query(0))
			{
				GetParam().UpdateEnum("\\HvDsp\\Misc", "InitHdd", 0);
				GetParam().Commit();
				GetParam().Get().cResultSenderParam.fInitHdd = 0;
				SW_TRACE_NORMAL("Info: Hdd init -- OK, restart LprApp...\n");
				if(1 == swpa_utils_file_exist("/mnt/nand/SdErrFile"))
				{
					SW_TRACE_NORMAL("Info: remove SdErrFile ...\n");
					swpa_utils_file_delete("/mnt/nand/SdErrFile");
				}
				Exit(2); //restart
			}
			else if ( 3 == swpa_hdd_query(0))
			{
				SW_TRACE_NORMAL("Info: Hdd init -- failed, restart LprApp...\n");
				if(1 == swpa_utils_file_exist("/mnt/nand/SdErrFile"))
				{
					SW_TRACE_NORMAL("Info: remove SdErrFile ...\n");
					swpa_utils_file_delete("/mnt/nand/SdErrFile");
				}
				Exit(2); //restart
			}
		}

		static int siErrorCount = 0;
		INT iSleepStatus = 0;
		CSWMessage::SendMessage(MSG_GET_SLEEP_STATUS, 0, (LPARAM)&iSleepStatus);
		//一体机协议才做这个检查，测试协议不做检查
		if (0 == GetParam().Get().cCamCfgParam.iCamType 
			&& !fIsInitHdd 	//格式化硬盘时不做检查
			&& !iSleepStatus //木星: 球机睡眠状态不做检查
			&& 0 == (dwTimes % 4))	//2s
		{
			FLOAT fltJpegFps = 0.0, fltH264Fps = 0.0;
			CSWMessage::SendMessage(MSG_SOURCE_GET_JPEG_RAW_FPS, 0, (LPARAM)&fltJpegFps);
			CSWMessage::SendMessage(MSG_SOURCE_GET_H264_RAW_FPS, 0, (LPARAM)&fltH264Fps);
			if (fltJpegFps < 1.0 || fltH264Fps < 1.0)
			{
				SW_TRACE_NORMAL("===== Image Source Error:Jpeg Fps:%.1f,H264 Fps:%.1f =====\n",fltJpegFps,fltH264Fps);
				++siErrorCount;
				if( siErrorCount > 3 )
				{
					OnResetDevice((WPARAM)2,0);
				}

				//swpa_ipnc_printState();
				swpa_ipnc_printDetailedStatistics();
				swpa_ipnc_printBufferStatistics();
			}
			else
			{
				siErrorCount = 0;
			}
		}

		if (dwTimes % 20 == 0)		//10s
		{
			//自动复位检查
			CheckRebootEvent();
			/*CSWDateTime cTime;
			if(cTime.GetHour()==2&&cTime.GetMinute()==30)
			{
				SW_TRACE_DEBUG("OnResetDevice 2:30\n");
				OnResetDevice((WPARAM)2,0);
			}*/
		}

	}

	//程序正常退出，置启动次数为0
	if (0 == GetExitCode())
	{
		SetBootCountInfo();
	}
	
	SW_TRACE_NORMAL("\nApp exit...\n");
	return S_OK;
}

// 启动计数
#ifdef DM6467
VOID CSWLPRApplication::SetBootCountInfo()
{
	SW_TRACE_DEBUG("<app>set boot count.");
	if( 0 == swpa_device_set_bootcount(0)
		&& 0 == swpa_device_set_bootcount_flag(0) )
	{
		SW_TRACE_DEBUG("<app>set boot count 0,0 ok.");
	}
	else
	{
		SW_TRACE_DEBUG("<app>set boot count 0,0 failed.");
	}
}
#endif

#ifdef DM8127
VOID CSWLPRApplication::SetBootCountInfo()
{
	SW_TRACE_DEBUG("<app>set boot count.");
	if( 0 == swpa_device_set_normal_bootcount(0)
		&& 0 == swpa_device_set_bootcount_flag(0) )
	{
		SW_TRACE_DEBUG("<app>set boot count 0,0 ok.");
	}
	else
	{
		SW_TRACE_DEBUG("<app>set boot count 0,0 failed.");
	}
}
#endif


HRESULT CSWLPRApplication::OnCommand(CSWBaseFilter* pFilter, DWORD dwID, DWORD wParam, DWORD lParam)
{
	CSWObject **pCarLeft = (CSWObject **)wParam;
    if(!swpa_strcmp(pFilter->Name(), "CSWRecognizeTransformFilter"))
	{		
		*pCarLeft = CreateObjectByVariable("carleft", "CSWEPoliceCarLeft");
	}
    else if(!swpa_strcmp(pFilter->Name(), "CSWRecognizeTransformTollGateFilter"))
    {
        *pCarLeft = CreateObjectByVariable("carleft", "CSWEPoliceCarLeft");
    }
	else if(!swpa_strcmp(pFilter->Name(), "CSWMatchTransformFilter"))
	{
		*pCarLeft = CreateObjectByVariable("carleft", "CSWEPoliceCarLeft");
	}
	else if(!swpa_strcmp(pFilter->Name(), "CSWRecognizeTransformPTFilter"))
	{
	    *pCarLeft = CreateObjectByVariable("carleft", "CSWHvcCarLeft");
	}
	else if(!swpa_strcmp(pFilter->Name(), "CSWRecognizeTransformEPFilter"))
	{
	    *pCarLeft = CreateObjectByVariable("carleft", "CSWEPoliceCarLeft");
	}
	else if(!swpa_strcmp(pFilter->Name(), "CSWRecognizeTransformPPFilter"))
	{
		*pCarLeft = CreateObjectByVariable("carleft", "CSWPeccancyParkingCarLeft");
	}
	else if(!swpa_strcmp(pFilter->Name(), "CSWRecognizeTGTransformFilter"))
	{
		*pCarLeft = CreateObjectByVariable("carleft", "CSWEPoliceCarLeft");
	}
	else
	{
		*pCarLeft = CreateObjectByVariable("carleft", "CSWCarLeft");
	}
	
	if(IsDecendant(CSWCarLeft, *pCarLeft))
	{
		((CSWCarLeft *)(*pCarLeft))->SetParameter(&GetParam().Get().cTrackerCfgParam, NULL);
	}
	return S_OK;
}

CSWLPRParameter& CSWLPRApplication::GetParam(VOID)
{
	return *m_pParam;
}

CSWObject * CSWLPRApplication::CreateObjectByVariable(LPCSTR szVar, LPCSTR szDefaultClassName)
{
	CSWString strClassName = szDefaultClassName;
	TiXmlElement *el = m_xmlDoc.FirstChildElement("LprApp")->FirstChildElement("VarList");
	if(el && (el = el->FirstChildElement(szVar)) && el->Attribute("class"))
	{
		strClassName = el->Attribute("class");
	}
	return CREATE_OBJECT(strClassName);
}

INT CSWLPRApplication::OnMSGCompare(PVOID key1, PVOID key2)
{
	CSWString *pstrTmp1 = (CSWString*)key1;
	CSWString *pstrTmp2 = (CSWString*)key2;
	return pstrTmp1->Compare(*pstrTmp2);
}

HRESULT CSWLPRApplication::InitVariant(VOID)
{
	TiXmlElement *el = NULL;
	//替换$(CaptureFilter)
	if((el = m_xmlDoc.FirstChildElement("LprApp")->FirstChildElement("FilterList")))
	{
		for(TiXmlElement *pTmp = el->FirstChildElement("Filter"); NULL != pTmp; pTmp = pTmp->NextSiblingElement("Filter"))
		{
			if(pTmp->Attribute("class") && !swpa_strcmp(pTmp->Attribute("class"), "$(CaptureFilter)"))
			{
				//临时代码
				switch(GetParam().Get().cCamCfgParam.iCamType)
				{
#if defined(DM6467)
				case 0 : pTmp->SetAttribute("class", "CSWVPIFSourceFilter"); break;
#elif defined(DM8127)
				case 0 : pTmp->SetAttribute("class", "CSWCentaurusSourceFilter"); break;
#endif					
				case 1 : pTmp->SetAttribute("class", "CSWNetSourceFilter"); break;
				}
			}
		}
	}	
	return S_OK;
}

HRESULT CSWLPRApplication::InitParameter(VOID)
{
	CSWString strParam = "CSWLPREPoliceParameter";
	if( m_pParam == NULL )
	{
		m_pParam = (CSWLPRParameter *)CreateObjectByVariable("param", (LPCSTR)strParam);
	}

	CSWString strXMLFile = GetCommandString("-e", "EEPROM/0/APP_PARAM");
	SW_TRACE_DEBUG("Parameter config file:%s", (LPCSTR)strXMLFile);
	
	// NFS方式下不能设置IP，当前直接注释掉参数的加载。
	// todo.
	
	if(NULL == m_pParam || S_OK != m_pParam->Initialize((PSTR)(LPCSTR)strXMLFile))
	{
		SW_TRACE_DEBUG("invalid parameter, m_pParam(%s)=0x%08x\n", (LPCSTR)strParam, m_pParam);
		return E_FAIL;
	}
	m_pParam->Commit();
	SW_TRACE_DEBUG("init parameter, m_pParam(%s)=0x%08x ok.\n", (LPCSTR)m_pParam->Name(), m_pParam);
	return S_OK;
}

HRESULT CSWLPRApplication::InitObject(VOID)
{
	HRESULT hr = S_OK;
	TiXmlElement *el = m_xmlDoc.FirstChildElement("LprApp")->FirstChildElement("ObjectList");
	if(NULL == el)
	{
		SW_TRACE_DEBUG("Can not found ObjectList node.\n");
	}
	else
	{
		for(TiXmlElement *pTmp = el->FirstChildElement("Object"); NULL != pTmp; pTmp = pTmp->NextSiblingElement("Object"))
		{	//判断是否要创建，默认为创建
			if(!pTmp->Attribute("create") || 1 == swpa_atoi(pTmp->Attribute("create")))
			{
				CSWObject *pMSG = CREATE_OBJECT(pTmp->Attribute("class"));
				if(NULL != pMSG)
				{
					SW_TRACE_DEBUG("create Object %s success\n", pMSG->Name());
					if(SUCCEEDED(OnObjectInitialize(pMSG)))
					{
						CSWString *pstrName = new CSWString(pTmp->Attribute("class"));
						if(pTmp->Attribute("name"))
						{
							*pstrName = pTmp->Attribute("name");
						}
						
						if(!pstrName->IsEmpty())
						{
							SAFE_ADDREF(pMSG);
							SAFE_ADDREF(pstrName);
							swpa_map_add(m_hObject, pstrName, pMSG);
							SW_TRACE_DEBUG("initialize Object %s success\n", pMSG->Name());
						}
						else
						{
							SW_TRACE_DEBUG("initialize Object %s failed\n", pMSG->Name());
						}
						SAFE_RELEASE(pstrName);
					}
					else
					{
						SW_TRACE_DEBUG("initialize Object %s failed\n", pMSG->Name());
					}
					SAFE_RELEASE(pMSG);
				}
				else
				{
					SW_TRACE_DEBUG("create Object %s failed\n", pTmp->Attribute("class"));
				}
			}
		}
	}
	return hr;
}

HRESULT CSWLPRApplication::OnCameralControlMSG(CSWObject *pObject)
{

	// zhaopy
	// 每次启动都需要RESET FPGA
	//INT iRet = swpa_device_reset(SWPA_DEVICE_CAMERA_FPGA);
	//SW_TRACE_DEBUG("<LPRApplication>Reset fpga return = %d.\n", iRet);


	CAMERAPARAM_INFO info;

	info.iDiffCorEnable        = GetParam().Get().cCamAppParam.iEnableCCDSignalCheck;
	// 抓拍图的差异校正只有在使能了独立增益参数后才使能。
	info.iCapiffCorEnable      = GetParam().Get().cCamAppParam.iEnableCaptureGain;

	info.iAGCEnable            = GetParam().Get().cCamAppParam.iAGCEnable;
	info.iAWBEnable            = GetParam().Get().cCamAppParam.iAWBEnable;
	info.iAGCTh                = GetParam().Get().cCamAppParam.iAGCTh;
	info.iAGCShutterLOri       = GetParam().Get().cCamAppParam.iAGCShutterLOri;
	info.iAGCShutterHOri       = GetParam().Get().cCamAppParam.iAGCShutterHOri;
	info.iAGCGainLOri          = GetParam().Get().cCamAppParam.iAGCGainLOri;
	info.iAGCGainHOri          = GetParam().Get().cCamAppParam.iAGCGainHOri;
	info.iAGCScene			   = GetParam().Get().cCamAppParam.iAGCScene;
	info.iGainR                = GetParam().Get().cCamAppParam.iGainR;
	info.iGainG                = GetParam().Get().cCamAppParam.iGainG;
	info.iGainB                = GetParam().Get().cCamAppParam.iGainB;
	info.iGain                 = GetParam().Get().cCamAppParam.iGain;
    info.iShutter              = GetParam().Get().cCamAppParam.iShutter;
    info.iCaptureGainR         = GetParam().Get().cCamAppParam.iCaptureGainR;
	info.iCaptureGainG         = GetParam().Get().cCamAppParam.iCaptureGainG;
	info.iCaptureGainB         = GetParam().Get().cCamAppParam.iCaptureGainB;
	info.iCaptureGain          = GetParam().Get().cCamAppParam.iCaptureGain;
	info.iCaptureShutter       = GetParam().Get().cCamAppParam.iCaptureShutter;
	info.iEnableDCAperture     = GetParam().Get().cCamAppParam.iEnableDCAperture;
	info.iEnableSharpen        = GetParam().Get().cCamAppParam.iEnableSharpen;
	info.iSharpenCapture       = GetParam().Get().cCamAppParam.iSharpenCapture;
	info.iSharpenThreshold     = GetParam().Get().cCamAppParam.iSharpenThreshold;
	info.iColorMatrixMode      = GetParam().Get().cCamAppParam.iColorMatrixMode;
	info.iContrastValue        = GetParam().Get().cCamAppParam.iContrastValue;
	info.iSaturationValue      = GetParam().Get().cCamAppParam.iSaturationValue;
	info.iTemThreshold         = GetParam().Get().cCamAppParam.iTemThreshold;
	info.iHueThreshold         = GetParam().Get().cCamAppParam.iHueThreshold;
	info.iHazeRemovalMode      = GetParam().Get().cCamAppParam.iHazeRemovalMode;
	info.iHazeRemovalThreshold = GetParam().Get().cCamAppParam.iHazeRemovalThreshold;
	info.iCaptureAutoParamEnable = GetParam().Get().cCamAppParam.iCaptureAutoParamEnable;
	info.iCaptureRGBEnable     = GetParam().Get().cCamAppParam.iEnableCaptureGainRGB;
	info.iCaptureShutterEnable = GetParam().Get().cCamAppParam.iEnableCaptureShutter;
	info.iCaptureGainEnable    = GetParam().Get().cCamAppParam.iEnableCaptureGain;
	info.iFlashRateSynSignalEnable = GetParam().Get().cCamAppParam.iFlashRateSynSignalEnable;
	info.iFlashRateMultiplication = GetParam().Get().cCamAppParam.iFlashRateMultiplication;
	info.iFlashRateOutputType = GetParam().Get().cCamAppParam.iFlashRateOutputType;
	info.iFlashRatePolarity = GetParam().Get().cCamAppParam.iFlashRatePolarity;
	info.iFlashRatePulseWidth = GetParam().Get().cCamAppParam.iFlashRatePulseWidth / 100;

	info.iCaptureSynSignalEnable = GetParam().Get().cCamAppParam.iCaptureSynSignalEnable;
	info.iCaptureSynCoupling = GetParam().Get().cCamAppParam.iCaptureSynCoupling;
	info.iCaptureSynOutputType = GetParam().Get().cCamAppParam.iCaptureSynOutputType;
	info.iCaptureSynPolarity = GetParam().Get().cCamAppParam.iCaptureSynPolarity;
	info.iCaptureSynPulseWidth = GetParam().Get().cCamAppParam.iCaptureSynPulseWidth / 100;

	swpa_memcpy(info.rgiAGCZone, GetParam().Get().cCamAppParam.rgiAGCZone, sizeof(info.rgiAGCZone));

	info.iExternalSyncMode = GetParam().Get().cCamAppParam.iExternalSyncMode;
	info.iSyncRelay = GetParam().Get().cCamAppParam.iSyncRelay;

	//TG IO
	info.iCaptureEdgeOne = info.iCaptureEdgeTwo = GetParam().Get().cCamAppParam.iCaptureEdge;
	info.iFlashDifferentLaneExt = GetParam().Get().cCamAppParam.iFlashDifferentLaneExt;

    info.iSaturationEnable = GetParam().Get().cCamAppParam.iSaturationEnable;
    info.iContrastEnalbe = GetParam().Get().cCamAppParam.iContrastEnalbe;
    info.iWDRStrength = GetParam().Get().cCamAppParam.iWDRValue;
    switch (GetParam().Get().cCamAppParam.iSNFTNFMode)
    {
    case 1:
        info.fTNFEnable = 1;
        info.fSNFEnable = 0;
        break;
    case 2:
        info.fTNFEnable = 0;
        info.fSNFEnable = 1;
        break;
    case 3:
        info.fTNFEnable = 1;
        info.fSNFEnable = 1;
        break;
    case 0:
    default:
        info.fTNFEnable = 0;
        info.fSNFEnable = 0;
        break;
    }
    info.iDenoiseState = GetParam().Get().cCamAppParam.iTNFSNFValue;
    info.fGrayImageEnable = GetParam().Get().cCamAppParam.iGrayImageEnable;
    info.iCVBSMode = GetParam().Get().cCamAppParam.iCVBSMode;
    info.iGammaEnable = GetParam().Get().cCamAppParam.iGammaEnable;
    info.iGammaValue = GetParam().Get().cCamAppParam.iGammaValue;
    info.iEdgeValue = GetParam().Get().cCamAppParam.iEdgeValue;

	//补光灯控制 EXP IO
	info.iLEDPolarity = GetParam().Get().cCamAppParam.iLEDPolarity;
	info.iLEDOutputType = GetParam().Get().cCamAppParam.iLEDOutputType;
	info.iLEDPluseWidth = GetParam().Get().cCamAppParam.iLEDPluseWidth;
	info.iFilterSwitchState = GetParam().Get().cTrackerCfgParam.nCtrlCpl;
	
	// 红灯加红的参数
	info.iHThreshold = 0x9000;
	info.iLThreshold = 0x999;
	info.iSThreshold = 0x2000;
	info.iColorFactor = 0x66;
	info.iLumLThreshold = GetParam().Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight ? 0x2d00 : 0x3fff;
	info.iLumLFactor = 0x166;
	info.iLumHFactor = 0x0;

	// 触发输出 F1 IO
	info.fEnableTriggerOut = GetParam().Get().cResultSenderParam.fEnableTriggerOut;
	info.nTriggerOutNormalStatus = GetParam().Get().cResultSenderParam.nTriggerOutNormalStatus;
	info.nCaptureSynOutputType = GetParam().Get().cCamAppParam.iCaptureSynOutputType;

	info.iColorGradation=GetParam().Get().cCamAppParam.iColorGradation;

	//ALM IO
	info.iALMPolarity = GetParam().Get().cCamAppParam.iALMPolarity;
	info.iALMOutputType = GetParam().Get().cCamAppParam.iALMOutputType;
	
	//使用F1和ALM口做闪光灯输出
	info.iFlashEnable 		= GetParam().Get().cCamAppParam.iFlashEnable;
	info.iFlashPolarity 	= GetParam().Get().cCamAppParam.iFlashPolarity;
	info.iFlashOutputType 	= GetParam().Get().cCamAppParam.iFlashOutputType;
	info.iFlashCoupling		= GetParam().Get().cCamAppParam.iFlashCoupling;
	info.iFlashPluseWidth	= GetParam().Get().cCamAppParam.iFlashPluseWidth;

	//F1输出模式
	info.iF1OutputType = GetParam().Get().cCamAppParam.iF1OutputType;

	//图像增强
	info.fColorMatrixEnable = GetParam().Get().cCamAppParam.iColorMatrixMode;

	//相机工作模式
	info.iCameraWorkMode = GetParam().Get().nCameraWorkMode;

	//白平衡模式
	info.iAWBMode = GetParam().Get().cCamAppParam.iAWBWorkMode;
	// 只有是一体机时才去初始化。
	if( GetParam().Get().cCamCfgParam.iCamType == 0 )
	{		
		//设置gamma
		SendMessage(MSG_SET_GAMMA, (WPARAM)GetParam().Get().cCamAppParam.rgiGamma);
		SendMessage(MSG_SET_GAMMA_MODE, (WPARAM)GetParam().Get().cCamAppParam.iGammaMode);

		// 红灯加红区域设置
		const int MAX_RED_RECT_COUNT = 8;
		SW_RECT rgcRect[MAX_RED_RECT_COUNT];
		swpa_memset(rgcRect, 0, sizeof(SW_RECT) * MAX_RED_RECT_COUNT);
		INT iRectCount = MAX_RED_RECT_COUNT;
		GetRedRectInfo(rgcRect, iRectCount);

		// 金星在此设置红绿灯坐标
		// 在相机初始化时进行红灯加红设置
		for(int i = 0; i < iRectCount && i < 8; ++i)
		{
			memcpy(&info.rgRedLightRect[i], &rgcRect[i], sizeof(SW_RECT));
		}
		info.nRedLightCount = iRectCount;

		//设置其他默认参数
		HRESULT hr = SendMessage(MSG_INITIAL_PARAM, (WPARAM)&info);

		hr = SendMessage(MSG_SET_REDLIGHT_RECT, (WPARAM)rgcRect, (LPARAM)MAX_RED_RECT_COUNT);

		swpa_memset(rgcRect, 0, sizeof(SW_RECT) * MAX_RED_RECT_COUNT);
		iRectCount = MAX_RED_RECT_COUNT;
		GetGreenRectInfo(rgcRect, iRectCount);

		hr = SendMessage(MSG_SET_GREENLIGHT_RECT, (WPARAM)rgcRect, (LPARAM)MAX_RED_RECT_COUNT);
		return hr;
	}
	else
	{
		return S_OK;
	}
}

HRESULT CSWLPRApplication::OnCameraControl2AMSG(CSWObject *pObject)
{
	CAMERAPARAM_INFO info;

	info.iDiffCorEnable        = GetParam().Get().cCamAppParam.iEnableCCDSignalCheck;
	// 抓拍图的差异校正只有在使能了独立增益参数后才使能。
	info.iCapiffCorEnable      = GetParam().Get().cCamAppParam.iEnableCaptureGain;

	info.iAGCEnable            = GetParam().Get().cCamAppParam.iAGCEnable;
	info.iAWBEnable            = GetParam().Get().cCamAppParam.iAWBEnable;
	info.iAGCTh                = GetParam().Get().cCamAppParam.iAGCTh;
	info.iAGCShutterLOri       = GetParam().Get().cCamAppParam.iAGCShutterLOri;
	info.iAGCShutterHOri       = GetParam().Get().cCamAppParam.iAGCShutterHOri;
	info.iAGCGainLOri          = GetParam().Get().cCamAppParam.iAGCGainLOri;
	info.iAGCGainHOri          = GetParam().Get().cCamAppParam.iAGCGainHOri;
	info.iAGCScene			   = GetParam().Get().cCamAppParam.iAGCScene;
	info.iGainR                = GetParam().Get().cCamAppParam.iGainR;
	info.iGainG                = GetParam().Get().cCamAppParam.iGainG;
	info.iGainB                = GetParam().Get().cCamAppParam.iGainB;
	info.iGain                 = GetParam().Get().cCamAppParam.iGain;
	info.iShutter              = GetParam().Get().cCamAppParam.iShutter;
	info.iCaptureGainR         = GetParam().Get().cCamAppParam.iCaptureGainR;
	info.iCaptureGainG         = GetParam().Get().cCamAppParam.iCaptureGainG;
	info.iCaptureGainB         = GetParam().Get().cCamAppParam.iCaptureGainB;
	info.iCaptureGain          = GetParam().Get().cCamAppParam.iCaptureGain;
	info.iCaptureShutter       = GetParam().Get().cCamAppParam.iCaptureShutter;
	info.iEnableDCAperture     = GetParam().Get().cCamAppParam.iEnableDCAperture;
	info.iEnableSharpen        = GetParam().Get().cCamAppParam.iEnableSharpen;
	info.iSharpenCapture       = GetParam().Get().cCamAppParam.iSharpenCapture;
	info.iSharpenThreshold     = GetParam().Get().cCamAppParam.iSharpenThreshold;
	info.iColorMatrixMode      = GetParam().Get().cCamAppParam.iColorMatrixMode;
	info.iContrastValue        = GetParam().Get().cCamAppParam.iContrastValue;
	info.iSaturationValue      = GetParam().Get().cCamAppParam.iSaturationValue;
	info.iTemThreshold         = GetParam().Get().cCamAppParam.iTemThreshold;
	info.iHueThreshold         = GetParam().Get().cCamAppParam.iHueThreshold;
	info.iHazeRemovalMode      = GetParam().Get().cCamAppParam.iHazeRemovalMode;
	info.iHazeRemovalThreshold = GetParam().Get().cCamAppParam.iHazeRemovalThreshold;
	info.iCaptureAutoParamEnable = GetParam().Get().cCamAppParam.iCaptureAutoParamEnable;
	info.iCaptureRGBEnable     = GetParam().Get().cCamAppParam.iEnableCaptureGainRGB;
	info.iCaptureShutterEnable = GetParam().Get().cCamAppParam.iEnableCaptureShutter;
	info.iCaptureGainEnable    = GetParam().Get().cCamAppParam.iEnableCaptureGain;
	info.iFlashRateSynSignalEnable = GetParam().Get().cCamAppParam.iFlashRateSynSignalEnable;
	info.iFlashRateMultiplication = GetParam().Get().cCamAppParam.iFlashRateMultiplication;
	info.iFlashRateOutputType = GetParam().Get().cCamAppParam.iFlashRateOutputType;
	info.iFlashRatePolarity = GetParam().Get().cCamAppParam.iFlashRatePolarity;
	info.iFlashRatePulseWidth = GetParam().Get().cCamAppParam.iFlashRatePulseWidth / 100;

	info.iCaptureSynSignalEnable = GetParam().Get().cCamAppParam.iCaptureSynSignalEnable;
	info.iCaptureSynCoupling = GetParam().Get().cCamAppParam.iCaptureSynCoupling;
	info.iCaptureSynOutputType = GetParam().Get().cCamAppParam.iCaptureSynOutputType;
	info.iCaptureSynPolarity = GetParam().Get().cCamAppParam.iCaptureSynPolarity;
	info.iCaptureSynPulseWidth = GetParam().Get().cCamAppParam.iCaptureSynPulseWidth / 100;

	swpa_memcpy(info.rgiAGCZone, GetParam().Get().cCamAppParam.rgiAGCZone, sizeof(info.rgiAGCZone));

	info.iExternalSyncMode = GetParam().Get().cCamAppParam.iExternalSyncMode;
	info.iSyncRelay = GetParam().Get().cCamAppParam.iSyncRelay;

	//TG IO
	info.iCaptureEdgeOne = info.iCaptureEdgeTwo = GetParam().Get().cCamAppParam.iCaptureEdge;
	info.iFlashDifferentLaneExt = GetParam().Get().cCamAppParam.iFlashDifferentLaneExt;

    info.iSaturationEnable = GetParam().Get().cCamAppParam.iSaturationEnable;
    info.iContrastEnalbe = GetParam().Get().cCamAppParam.iContrastEnalbe;
    info.iWDRStrength = GetParam().Get().cCamAppParam.iWDRValue;
    switch (GetParam().Get().cCamAppParam.iSNFTNFMode)
    {
    case 1:
        info.fTNFEnable = 1;
        info.fSNFEnable = 0;
        break;
    case 2:
        info.fTNFEnable = 0;
        info.fSNFEnable = 1;
        break;
    case 3:
        info.fTNFEnable = 1;
        info.fSNFEnable = 1;
        break;
    case 0:
    default:
        info.fTNFEnable = 0;
        info.fSNFEnable = 0;
        break;
    }
    info.iDenoiseState = GetParam().Get().cCamAppParam.iTNFSNFValue;
    info.fGrayImageEnable = GetParam().Get().cCamAppParam.iGrayImageEnable;
    info.iCVBSMode = GetParam().Get().cCamAppParam.iCVBSMode;
    info.iGammaEnable = GetParam().Get().cCamAppParam.iGammaEnable;
    info.iGammaValue = GetParam().Get().cCamAppParam.iGammaValue;
    info.iEdgeValue = GetParam().Get().cCamAppParam.iEdgeValue;

	//补光灯控制 EXP IO
	info.iLEDPolarity = GetParam().Get().cCamAppParam.iLEDPolarity;
	info.iLEDOutputType = GetParam().Get().cCamAppParam.iLEDOutputType;
	info.iLEDPluseWidth = GetParam().Get().cCamAppParam.iLEDPluseWidth;
	info.iFilterSwitchState = GetParam().Get().cTrackerCfgParam.nCtrlCpl;
	
	// 红灯加红的参数
	info.iHThreshold = 0x9000;
	info.iLThreshold = 0x999;
	info.iSThreshold = 0x2000;
	info.iColorFactor = 0x66;
	info.iLumLThreshold = GetParam().Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight ? 0x2d00 : 0x3fff;
	info.iLumLFactor = 0x166;
	info.iLumHFactor = 0x0;

	// 触发输出 F1 IO
	info.fEnableTriggerOut = GetParam().Get().cResultSenderParam.fEnableTriggerOut;
	info.nTriggerOutNormalStatus = GetParam().Get().cResultSenderParam.nTriggerOutNormalStatus;
	info.nCaptureSynOutputType = GetParam().Get().cCamAppParam.iCaptureSynOutputType;

	//ALM IO
	info.iALMPolarity = GetParam().Get().cCamAppParam.iALMPolarity;
	info.iALMOutputType = GetParam().Get().cCamAppParam.iALMOutputType;
	
	//使用F1和ALM口做闪光灯输出
	info.iFlashEnable 		= GetParam().Get().cCamAppParam.iFlashEnable;
	info.iFlashPolarity 	= GetParam().Get().cCamAppParam.iFlashPolarity;
	info.iFlashOutputType 	= GetParam().Get().cCamAppParam.iFlashOutputType;
	info.iFlashCoupling		= GetParam().Get().cCamAppParam.iFlashCoupling;
	info.iFlashPluseWidth	= GetParam().Get().cCamAppParam.iFlashPluseWidth;

	//F1输出模式
	info.iF1OutputType = GetParam().Get().cCamAppParam.iF1OutputType;

	//图像增强
	info.fColorMatrixEnable = GetParam().Get().cCamAppParam.iColorMatrixMode;

	//相机工作模式
	info.iCameraWorkMode = GetParam().Get().nCameraWorkMode;

	//白平衡模式
	info.iAWBMode = GetParam().Get().cCamAppParam.iAWBWorkMode;
	// 只有是一体机时才去初始化。
	if( GetParam().Get().cCamCfgParam.iCamType == 0 )
	{		
		//设置gamma
		SendMessage(MSG_SET_GAMMA, (WPARAM)GetParam().Get().cCamAppParam.rgiGamma);
		SendMessage(MSG_SET_GAMMA_MODE, (WPARAM)GetParam().Get().cCamAppParam.iGammaMode);

		// 红灯加红区域设置
		const int MAX_RED_RECT_COUNT = 8;
		SW_RECT rgcRect[MAX_RED_RECT_COUNT];
		swpa_memset(rgcRect, 0, sizeof(SW_RECT) * MAX_RED_RECT_COUNT);
		INT iRectCount = MAX_RED_RECT_COUNT;
		GetRedRectInfo(rgcRect, iRectCount);

		// 金星在此设置红绿灯坐标
		// 在相机初始化时进行红灯加红设置
		for(int i = 0; i < iRectCount && i < 8; ++i)
		{
			memcpy(&info.rgRedLightRect[i], &rgcRect[i], sizeof(SW_RECT));
		}
		info.nRedLightCount = iRectCount;

		//设置其他默认参数
		HRESULT hr = SendMessage(MSG_INITIAL_PARAM, (WPARAM)&info);

		hr = SendMessage(MSG_SET_REDLIGHT_RECT, (WPARAM)rgcRect, (LPARAM)MAX_RED_RECT_COUNT);

		swpa_memset(rgcRect, 0, sizeof(SW_RECT) * MAX_RED_RECT_COUNT);
		iRectCount = MAX_RED_RECT_COUNT;
		GetGreenRectInfo(rgcRect, iRectCount);

		hr = SendMessage(MSG_SET_GREENLIGHT_RECT, (WPARAM)rgcRect, (LPARAM)MAX_RED_RECT_COUNT);
		return hr;
	}
	else
	{
		return S_OK;
	}
}


HRESULT CSWLPRApplication::OnDomeCameraControlMSG(CSWObject *pObject)
{
	CAMERAPARAM_INFO sInfo;
	swpa_memset(&sInfo, 0, sizeof(sInfo));

	sInfo.iGainR                = GetParam().Get().cCamAppParam.iGainR;
	sInfo.iGainG                = GetParam().Get().cCamAppParam.iGainG;
	sInfo.iGainB                = GetParam().Get().cCamAppParam.iGainB;
	sInfo.iGain                 = GetParam().Get().cCamAppParam.iGain;
	sInfo.iShutter              = GetParam().Get().cCamAppParam.iShutter;
	sInfo.iIris					= GetParam().Get().cCamAppParam.iIris;
	sInfo.iAutoFocus			= GetParam().Get().cCamAppParam.iAutoFocus;
	sInfo.iAWBMode				= GetParam().Get().cCamAppParam.iAWBMode;
	sInfo.iGammaValue			= GetParam().Get().cCamAppParam.iGammaValue;
	sInfo.iWDREnable			= GetParam().Get().cCamAppParam.iWDREnable ;
	sInfo.iAEMode				= GetParam().Get().cCamAppParam.iAEMode;
	sInfo.iSaturationValue		= GetParam().Get().cCamAppParam.iSaturationValue;
	sInfo.iNRLevel				= GetParam().Get().cCamAppParam.iNRLevel;
	sInfo.iSharpenThreshold		= GetParam().Get().cCamAppParam.iSharpenThreshold;
	sInfo.iEdgeValue			= GetParam().Get().cCamAppParam.iEdgeValue;

	CSWMessage::SendMessage(MSG_INITIAL_DOMECAMERA, (WPARAM)&sInfo, 0);

	return S_OK;
}


HRESULT CSWLPRApplication::GetRedRectInfo(SW_RECT* pRect, INT& iRectCount)
{
	if( pRect == NULL )
	{
		return E_INVALIDARG;
	}

	INT iMaxCount = iRectCount;
	iRectCount = 0;

	if( GetParam().Get().cTrackerCfgParam.cTrafficLight.nLightCount > 0 )
	{
		int iLightLeft = 0,iLightTop = 0,iLightRight = 0,iLightBottom = 0;
		int iRLightLeft = 0,iRLightTop = 0, iRLightRight = 0 , iRLightBottom=0;
		int iLightNum = 0, iLightType = 0;
		int iLightInterval = 0;
		char szLightInfor[256] = {0};
		int i = 0 , j = 0;

		for ( i = 0; i<GetParam().Get().cTrackerCfgParam.cTrafficLight.nLightCount; i++ )
		{
			if( iRectCount >= iMaxCount )
			{
				break;
			}
			sscanf(GetParam().Get().cTrackerCfgParam.cTrafficLight.rgszLightPos[i] ,
				"(%d,%d,%d,%d),%d,%d,%s" ,
				&iLightLeft , &iLightTop , &iLightRight , &iLightBottom ,
				&iLightNum , &iLightType , szLightInfor);
			if ( iLightNum <= 0 )
			{
				continue;
			}

			iLightTop*=2;
			iLightBottom*=2;
			if ( iLightType == 0 )
			{
				iLightInterval = (iLightRight - iLightLeft+1)/iLightNum;
			}
			else
			{
				iLightInterval = (iLightBottom - iLightTop+1)/iLightNum;
			}

			for ( j = 1; j <=iLightNum ; j++ )
			{
				if ( szLightInfor[j*2 - 1] != '1' )
				{
					continue;
				}
				if ( iLightType == 0 )
				{
					iRLightLeft = iLightLeft + (j-1) * (iLightInterval + 1);
					iRLightRight = iRLightLeft + iLightInterval;

					iRLightTop = iLightTop;
					iRLightBottom = iLightBottom;
				}
				else
				{
					iRLightLeft = iLightLeft;
					iRLightRight = iLightRight;

					iRLightTop = iLightTop + (j-1) * ( iLightInterval + 1);
					iRLightBottom = iRLightTop + iLightInterval;
				}
				//如果图片旋转，则需要修正坐标
				// todo.
				pRect[iRectCount].left = iRLightLeft;
				pRect[iRectCount].top = iRLightTop;
				pRect[iRectCount].right = iRLightRight;
				pRect[iRectCount].bottom = iRLightBottom;
				++iRectCount;

				if( iRectCount >= iMaxCount )
				{
					break;
				}
			}
		}
	}

	return S_OK;
}


HRESULT CSWLPRApplication::GetGreenRectInfo(SW_RECT* pRect, INT& iRectCount)
{
	if( pRect == NULL )
	{
		return E_INVALIDARG;
	}

	INT iMaxCount = iRectCount;
	iRectCount = 0;

	if( GetParam().Get().cTrackerCfgParam.cTrafficLight.nLightCount > 0 )
	{
		int iLightLeft = 0,iLightTop = 0,iLightRight = 0,iLightBottom = 0;
		int iRLightLeft = 0,iRLightTop = 0, iRLightRight = 0 , iRLightBottom=0;
		int iLightNum = 0, iLightType = 0;
		int iLightInterval = 0;
		char szLightInfor[256] = {0};
		int i = 0 , j = 0;

		for ( i = 0; i<GetParam().Get().cTrackerCfgParam.cTrafficLight.nLightCount; i++ )
		{
			if( iRectCount >= iMaxCount )
			{
				break;
			}
			sscanf(GetParam().Get().cTrackerCfgParam.cTrafficLight.rgszLightPos[i] ,
				"(%d,%d,%d,%d),%d,%d,%s" ,
				&iLightLeft , &iLightTop , &iLightRight , &iLightBottom ,
				&iLightNum , &iLightType , szLightInfor);
			if ( iLightNum <= 0 )
			{
				continue;
			}

			iLightTop*=2;
			iLightBottom*=2;
			if ( iLightType == 0 )
			{
				iLightInterval = (iLightRight - iLightLeft+1)/iLightNum;
			}
			else
			{
				iLightInterval = (iLightBottom - iLightTop+1)/iLightNum;
			}

			for ( j = 1; j <=iLightNum ; j++ )
			{
				if ( szLightInfor[j*2 - 1] != '3' ) //绿灯
				{
					continue;
				}
				if ( iLightType == 0 )
				{
					iRLightLeft = iLightLeft + (j-1) * (iLightInterval + 1);
					iRLightRight = iRLightLeft + iLightInterval;

					iRLightTop = iLightTop;
					iRLightBottom = iLightBottom;
				}
				else
				{
					iRLightLeft = iLightLeft;
					iRLightRight = iLightRight;

					iRLightTop = iLightTop + (j-1) * ( iLightInterval + 1);
					iRLightBottom = iRLightTop + iLightInterval;
				}
				//如果图片旋转，则需要修正坐标
				// todo.
				pRect[iRectCount].left = iRLightLeft;
				pRect[iRectCount].top = iRLightTop;
				pRect[iRectCount].right = iRLightRight;
				pRect[iRectCount].bottom = iRLightBottom;
				++iRectCount;

				if( iRectCount >= iMaxCount )
				{
					break;
				}
			}
		}
	}

	return S_OK;
}



HRESULT CSWLPRApplication::OnNetCommandProcessInitialize(CSWObject *pObject)
{
	return pObject->Invoke("Run");
}

HRESULT CSWLPRApplication::InitFilter(VOID)
{	
	TiXmlElement *el = m_xmlDoc.FirstChildElement("LprApp")->FirstChildElement("FilterList");
	if(NULL == el)
	{
		SW_TRACE_DEBUG("Can not found FilterList node.\n");
		return E_FAIL;
	}	
	
	//创建Filter
	CSWBaseFilter *pFilter = NULL, *pConnectedFilter = NULL;
	for(TiXmlElement *pTmp = el->FirstChildElement("Filter"); NULL != pTmp; pTmp = pTmp->NextSiblingElement("Filter"))
	{	//判断是否要创建，默认为创建
		if(pTmp->Attribute("create") && 0 == swpa_atoi(pTmp->Attribute("create")))
		{
			continue;
		}
		
		if(NULL != (pFilter = CreateFilter(pTmp)))
		{
			CSWString strName = pTmp->Attribute("class");                
			if(pTmp->Attribute("name"))
			{
				strName = pTmp->Attribute("name");
			}			
			SW_TRACE_DEBUG("create filter %s success\n", (LPCSTR)strName);
			//初始化Filter
			if(FAILED(AddFilter(strName, pFilter)))
			{
				SW_TRACE_DEBUG("AddFilter %s failed, remove it.\n", (LPCSTR)strName);
				RemoveFilter(strName);
			}
			pFilter->Release();
		}
		else
		{
			SW_TRACE_DEBUG("create filter %s failed\n", pTmp->Attribute("class"));
		}
	}
	SW_TRACE_DEBUG("connect filter...\n");
	//连接Filter
	for(TiXmlElement *pTmp = el->FirstChildElement("Filter"); NULL != pTmp; pTmp = pTmp->NextSiblingElement("Filter"))
	{	//查询Filter
		CSWString strName = pTmp->Attribute("class");                
		if(pTmp->Attribute("name"))
		{
			strName = pTmp->Attribute("name");
		}
		if(NULL != (pFilter= QueryFilter(strName)))
		{	//初始化Filter
			if(FAILED(OnObjectInitialize(pFilter)))
			{
				SW_TRACE_DEBUG("initialize %s failed, remove it.\n", (LPCSTR)strName);
				RemoveFilter(strName);
			}
			else
			{
				//一个输出
				if(pTmp->Attribute("ConnectFilter"))
				{
					if(NULL != (pConnectedFilter = QueryFilter(pTmp->Attribute("ConnectFilter"))))
					{
						INT iConnectedPin = 0;
						if(pTmp->Attribute("Pin"))
						{
							iConnectedPin = swpa_atoi(pTmp->Attribute("Pin"));
						}
						//连接两个Pin
						HRESULT hr = Connect(pFilter->GetOut(0), pConnectedFilter->GetIn(iConnectedPin));
						SW_TRACE_DEBUG("connect %s[0] => %s[%d] %s\n", pFilter->GetName(), pConnectedFilter->GetName(), iConnectedPin, hr == S_OK ? "success.": "failed.");
						//释放资源
						pConnectedFilter->Release();
					}
				}
				//多个输出
				else
				{
					for(TiXmlElement *pChild = pTmp->FirstChildElement("Pin"); NULL != pChild; pChild = pChild->NextSiblingElement("Pin"))
					{	//查到连接的Filter
						if(pChild->Attribute("ConnectFilter") && NULL != (pConnectedFilter = QueryFilter(pChild->Attribute("ConnectFilter"))))
						{	
							//获取两个Filter对应的Pin
							INT iPin = swpa_atoi(pChild->Attribute("value"));
							INT iConnectedPin = 0;
							if(pChild->Attribute("Pin"))
							{
								iConnectedPin = swpa_atoi(pChild->Attribute("Pin"));
							}
							//连接两个Pin
							HRESULT hr = Connect(pFilter->GetOut(iPin), pConnectedFilter->GetIn(iConnectedPin));
							SW_TRACE_DEBUG("connect %s[%d] => %s[%d] %s\n", pFilter->GetName(), iPin, pConnectedFilter->GetName(), iConnectedPin, hr == S_OK ? "success.": "failed.");
							//释放资源
							pConnectedFilter->Release();
						}
					}
				}
			}
			pFilter->Release();
		}
	}
	return S_OK;
}


VOID CSWLPRApplication::OnDSPAlarm(PVOID pvParam, INT iInfo)
{
//    // 连续两次抓拍在 80 ms 以内则过滤掉
//    {
//        static DWORD dwLastCapTime = 0;
//        DWORD dwDiff = CSWDateTime::GetSystemTick() - dwLastCapTime;
//        dwLastCapTime = CSWDateTime::GetSystemTick();
//        if (dwDiff <= 80)
//        {
//            return ;
//        }
//    }

	CSWLPRApplication *pThis = (CSWLPRApplication *)pvParam;
    INT iLane = 0;//(iInfo & 0xFF000000) >> 24;
	INT iSEQ = (iInfo & 0x00FFFFFF);
	
    if(!pThis->GetParam().Get().cResultSenderParam.iFlashOneByOne)
	{
        //iLane = 7;
        iLane = 3;
	}
	else
	{
//        iLane = (1 << iLane);
        // 利用原来分车道闪的功能来控制闪光灯的轮闪
        iLane = pThis->m_iFlashDiffFlag ? 1 : 2;
        pThis->m_iFlashDiffFlag = !pThis->m_iFlashDiffFlag;
	}	
    //SW_TRACE_DEBUG("DSPAlarm, Lane:%d, SEQ:%d,DSPTrigger:%d\n", iLane, iSEQ, pThis->m_fDSPTrigger);

	if(pThis->m_fDSPTrigger)
	{
		PostMessage(MSG_SET_SOFT_TOUCH, (iLane << 24)|(iSEQ));
	}
}

CSWBaseFilter *CSWLPRApplication::CreateFilter(TiXmlElement *element)
{
	CSWBaseFilter *pFilter = NULL;
	if(swpa_strcmp(CSWBaseFilter::Class(), element->Attribute("class")))
	{
		pFilter = CREATE_FILTER(element->Attribute("class"));
	}
	else
	{
		INT iIn = -1, iOut = -1;
			
		if(element->Attribute("In"))
		{
			iIn = swpa_atoi(element->Attribute("In"));
		}
		else
		{
			SW_TRACE_DEBUG("CSWBaseFilter lost in pin. failed to create the filter\n");
		}
		
		if(element->Attribute("Out"))
		{
			iOut = swpa_atoi(element->Attribute("Out"));
		}
		else
		{
			SW_TRACE_DEBUG("CSWBaseFilter lost out pin. failed to create the filter\n");
		}
		
		CSWString strFormat;
		if(element->Attribute("Format"))
		{
			strFormat = element->Attribute("Format");
		}
		else
		{
			SW_TRACE_DEBUG("CSWBaseFilter lost format. failed to create the filter\n");
		}
		
		if(iIn >0 && iOut > 0 && !strFormat.IsEmpty())
		{
			pFilter = new CSWBaseFilter(iIn, iOut);
			for(INT i = 0; i < pFilter->GetInCount(); i++)
			{
				pFilter->GetIn(i)->AddObject(CLASSIDBYNAME((LPCSTR)strFormat));
			}
			for(INT i = 0; i < pFilter->GetOutCount(); i++)
			{
				pFilter->GetOut(i)->AddObject(CLASSIDBYNAME((LPCSTR)strFormat));
			}
		}
	}
	return pFilter;
}

HRESULT CSWLPRApplication::OnH264HDDTransformFilterInitialize(CSWObject *pObject)
{
	//todo: 后续移至保存模块
	LONGLONG llTotalSize = 0;

	DWORD dwFileSize = 0;
	if (GetParam().Get().cResultSenderParam.fSaveVideo)
	{
		INT iTryCount = 10;

		SW_TRACE_DEBUG("SetVideoSaveEnable:%d\n",GetParam().Get().cGb28181.fVideoSaveEnable);
		if (FAILED(pObject->Invoke("SetVideoSaveEnable", GetParam().Get().cGb28181.fVideoSaveEnable)))
		{
			SW_TRACE_DEBUG("Err: failed to config H264HDD!\n");
		}
		
		//create folder
		if (SWPAR_OK != swpa_utils_dir_exist("/Result"))
		{
			if (SWPAR_OK != swpa_utils_dir_create("/Result"))
			{
				SW_TRACE_DEBUG("Err: failed to create /Result/\n");
				goto DO_INIT;//return E_FAIL;
			}

			if (SWPAR_OK != swpa_utils_dir_create("/Result/0"))
			{
				SW_TRACE_DEBUG("Err: failed to create /Result/0\n");
				goto DO_INIT;//return E_FAIL;
			}
		}

		if (0 == GetParam().Get().cResultSenderParam.iDiskType)// nfs, 0:网络硬盘
		{
			CHAR szCMD[256] = {0};

			if (!m_fNetDiskMounted)
			{			
				if (NULL == GetParam().Get().cResultSenderParam.szNetDiskIP
					|| 0 == swpa_strlen(GetParam().Get().cResultSenderParam.szNetDiskIP))
				{
					SW_TRACE_DEBUG("Err: failed to get Net Disk IP \n");
					goto DO_INIT;//return E_FAIL;
				}

				if (NULL == GetParam().Get().cResultSenderParam.szNFS
					|| 0 == swpa_strlen(GetParam().Get().cResultSenderParam.szNFS))
				{
					SW_TRACE_DEBUG("Err: failed to get Net Disk NFS Path \n");
					goto DO_INIT;//return E_FAIL;
				}


				SW_TRACE_DEBUG("Info: szNFSParam = %s\n", GetParam().Get().cResultSenderParam.szNFSParam);
				
				swpa_snprintf(szCMD, sizeof(szCMD)-1, "mount -t nfs %s:%s /Result/ %s", 
					GetParam().Get().cResultSenderParam.szNetDiskIP,
					GetParam().Get().cResultSenderParam.szNFS,
					GetParam().Get().cResultSenderParam.szNFSParam);
				while (SWPAR_OK != swpa_utils_shell(szCMD, NULL))
				{
					if (0 >= iTryCount--)
					{
						SW_TRACE_NORMAL("Err: failed to mount Net Disk\n");
						goto DO_INIT;
					}

					SW_TRACE_DEBUG("Info: mounting Net Disk...\n");
					swpa_hdd_umount("/Result/");
					CSWApplication::Sleep(1000);
					HeartBeat();
				}

				SW_TRACE_NORMAL("Info: Net Disk mount OK\n");

				m_fNetDiskMounted = TRUE;
			}

			llTotalSize = GetParam().Get().cResultSenderParam.iNetDiskSpace; //GB
			if (0 >= llTotalSize)
			{
				SW_TRACE_DEBUG("Err: failed to get Net Disk's space\n");
				goto DO_INIT;//return E_FAIL;
			}

			llTotalSize *= 1024*1024*1024; //convert GB to Byte

			if (GetParam().Get().cResultSenderParam.fIsSafeSaver)
			{
				llTotalSize /= 2;
			}
			
		}
		else if (2 == GetParam().Get().cResultSenderParam.iDiskType) //2:固态硬盘
		{
			if (GetParam().Get().cResultSenderParam.fInitHdd)
			{
				goto DO_INIT;
			}

			if (1 == swpa_utils_file_exist("/mnt/nand/SdErrFile"))
			{
				goto SD_BROKEN;
			}
			
					
			CHAR szPartition[64]={0};
			swpa_strcpy(szPartition, m_szHDDPath);
			swpa_strcat(szPartition, "1");

			if (SWPAR_OK != swpa_hdd_umount(szPartition)
				&& SWPAR_OK != swpa_hdd_umount("/Result/0/"))
			{
				SW_TRACE_DEBUG("Err: failed to umount h264 hdd partition\n");
			}

			CSWApplication::Sleep(1000);
		
			
			
			
			//CHAR szCmd[256] = {0};
			//swpa_sprintf(szCmd, "mount -l | grep /Result/0 | grep %s", szPartition);
			//if (SWPAR_OK == swpa_utils_shell(szCmd, NULL))
			//{
			//	SW_TRACE_NORMAL("Info: /Result/0 already mounted \n");
			//}
			//else
			{
				//mount
				while (iTryCount--)
				{
					if (0 != swpa_hdd_mount((const CHAR*)szPartition, "/Result/0", NULL))
					{
						swpa_thread_sleep_ms(500);
						swpa_hdd_umount("/Result/0/");
						swpa_hdd_umount(szPartition);
						swpa_thread_sleep_ms(500);
						HeartBeat();
					}
					else
					{
						SW_TRACE_NORMAL("Info: mount %s to /Result/0 -- OK\n", szPartition);
						//尝试写硬盘
						CSWString strTmp = "test";
						int iTryTime = 0;
						while(iTryTime++ < 3)
						{
							CSWFile file;
							if(S_OK == file.Open("/Result/0/.test", "w+") && S_OK == file.Write((LPVOID)(LPCSTR)strTmp, strTmp.Length()))
							{
								break;
							}
						}
						if(iTryTime >= 3)
						{							
							goto DO_INIT;
						}
						break;
					}
				}
				
				if (0 >= iTryCount)
				{
					SW_TRACE_DEBUG("Err: failed to mount %s to /Result/0\n", szPartition);
					goto DO_INIT;//return E_FAIL;
				}
			}

#ifdef DM8127
			swpa_thread_sleep_ms(3000);
#else
			swpa_thread_sleep_ms(10000);
#endif
			HeartBeat();

			iTryCount = 0;
			do
			{
#ifdef DM8127
				swpa_thread_sleep_ms(1000);
#else
				swpa_thread_sleep_ms(2000);
#endif
				HeartBeat();
				swpa_hdd_get_totalsize("/Result/0/", &llTotalSize);
				SW_TRACE_NORMAL("Info: llTotalSize = %lld\n", llTotalSize);
				
			} while (llTotalSize < 1*1024*1024*1024 && iTryCount++ < 10) ;

			if (10 <= iTryCount)
			{
				goto DO_INIT;
			}
		}
		
		dwFileSize = 1024*(DWORD)GetParam().Get().cResultSenderParam.iFileSize; //KB
		
		if (FAILED(pObject->Invoke("Initialize", "/Result/0", llTotalSize, dwFileSize, GetParam().Get().cResultSenderParam.iSaveVideoType)))
		{
			SW_TRACE_DEBUG("Err: failed to initialize H264HDD!\n");
			goto DO_INIT;//return E_FAIL;
		}

		if (FAILED(pObject->Invoke("SetSaveType", GetParam().Get().cResultSenderParam.iSaveSafeType))
			|| FAILED(pObject->Invoke("SetSendInterval", GetParam().Get().cResultSenderParam.iSendHisVideoSpace*100))
			)
		{
			SW_TRACE_DEBUG("Err: failed to config H264HDD!\n");
			goto DO_INIT;//return E_FAIL;
		}
		else
		{
			CSWFile file("BLOCK//tmp/.h264_save", "w");
		}
		return S_OK;
DO_INIT:
		if(1 == swpa_utils_file_exist("/tmp/.h264_save"))
		{
			SW_TRACE_NORMAL("Reboot in %s\n", __FUNCTION__);
			Exit(2);
		}
		else
		{
			m_strH264HDDStatus = "H264存储:异常";	
		}
	}

	SW_TRACE_NORMAL("Info: safe video disabled\n");    
	if (FAILED(pObject->Invoke("Initialize", 0, 0, 0, 0))
		|| FAILED(pObject->Invoke("SetSendInterval", GetParam().Get().cResultSenderParam.iSendHisVideoSpace*100))
		)
	{
		SW_TRACE_DEBUG("Err: failed to initialize H264HDD!\n");
		return E_FAIL;
	}

	return S_OK;

SD_BROKEN:
	m_strH264HDDStatus = "H264存储:SD卡状态异常，无法恢复，请更换或格式化SD卡";  
	SW_TRACE_NORMAL("Err: has file /mnt/nand/SdErrFile, need to format sd card!\n");
	if (FAILED(pObject->Invoke("Initialize", 0, 0, 0, 0))
		|| FAILED(pObject->Invoke("SetSendInterval", GetParam().Get().cResultSenderParam.iSendHisVideoSpace*100))
		)
	{
		SW_TRACE_DEBUG("Err: failed to initialize H264HDD!\n");
		return E_FAIL;
		}
	return S_OK;
}


HRESULT CSWLPRApplication::OnH264NetRenderFilterInitialize(CSWObject *pObject)
{
	return pObject->Invoke("Initialize"	                
	                );
}



HRESULT CSWLPRApplication::OnResultHDDTransformFilterInitialize(CSWObject *pObject)
{
	//todo: 后续移至保存模块
	LONGLONG llTotalSize = 0;
	DWORD dwFileSize = 0;
	
	if (GetParam().Get().cResultSenderParam.fIsSafeSaver)
	{
		INT iTryCount = 10;

		//create folder
		if (SWPAR_OK != swpa_utils_dir_exist("/Result"))
		{
			if (SWPAR_OK != swpa_utils_dir_create("/Result"))
			{
				SW_TRACE_DEBUG("Err: failed to create /Result/\n");
				goto DO_INIT;//return E_FAIL;
			}

			if (SWPAR_OK != swpa_utils_dir_create("/Result/1"))
			{
				SW_TRACE_DEBUG("Err: failed to create /Result/1\n");
				goto DO_INIT;//return E_FAIL;
			}
		}

		if (0 == GetParam().Get().cResultSenderParam.iDiskType)// nfs, 0:网络硬盘
		{
			CHAR szCMD[256] = {0};

			if (!m_fNetDiskMounted)
			{				
				if (NULL == GetParam().Get().cResultSenderParam.szNetDiskIP
					|| 0 == swpa_strlen(GetParam().Get().cResultSenderParam.szNetDiskIP))
				{
					SW_TRACE_DEBUG("Err: failed to get Net Disk IP \n");
					goto DO_INIT;//return E_FAIL;
				}

				if (NULL == GetParam().Get().cResultSenderParam.szNFS
					|| 0 == swpa_strlen(GetParam().Get().cResultSenderParam.szNFS))
				{
					SW_TRACE_DEBUG("Err: failed to get Net Disk NFS Path \n");
					goto DO_INIT;//return E_FAIL;
				}

				if (NULL == GetParam().Get().cResultSenderParam.szNFSParam)
				{
					SW_TRACE_DEBUG("(NULL == GetParam().Get().cResultSenderParam.szNFSParam)\n");
				}

				swpa_snprintf(szCMD, sizeof(szCMD)-1, "mount -t nfs %s:%s /Result/ %s", 
					GetParam().Get().cResultSenderParam.szNetDiskIP,
					GetParam().Get().cResultSenderParam.szNFS,
					GetParam().Get().cResultSenderParam.szNFSParam);
				while (SWPAR_OK != swpa_utils_shell(szCMD, NULL))
				{
					if (0 >= iTryCount--)
					{
						SW_TRACE_NORMAL("Err: failed to mount Net Disk\n");
						goto DO_INIT;
					}

					SW_TRACE_DEBUG("Info: mounting Net Disk...\n");
					swpa_hdd_umount("/Result/");
					CSWApplication::Sleep(1000);
					HeartBeat();
				}

				m_fNetDiskMounted = TRUE;
			}

			llTotalSize = GetParam().Get().cResultSenderParam.iNetDiskSpace; //GB
			if (0 >= llTotalSize)
			{
				SW_TRACE_DEBUG("Err: failed to get Net Disk's space\n");
				goto DO_INIT;//return E_FAIL;
			}

			llTotalSize *= 1024*1024*1024; //convert GB to Byte

			if (GetParam().Get().cResultSenderParam.fSaveVideo)
			{
				llTotalSize /= 2;
			}
			
		}
		else if (2 == GetParam().Get().cResultSenderParam.iDiskType) //2:固态硬盘
		{
			if (GetParam().Get().cResultSenderParam.fInitHdd)
			{
				goto DO_INIT;
			}

			if (1 == swpa_utils_file_exist("/mnt/nand/SdErrFile"))
			{
				goto SD_BROKEN;
			}
			
			CHAR szPartition[64]={0};
			swpa_strcpy(szPartition, m_szHDDPath);
			swpa_strcat(szPartition, "1");

			if (GetParam().Get().cResultSenderParam.fSaveVideo)
			{
				swpa_strcpy(szPartition, m_szHDDPath);
				swpa_strcat(szPartition, "2");
			}
			
			if (SWPAR_OK != swpa_hdd_umount(szPartition)
				&& SWPAR_OK != swpa_hdd_umount("/Result/1/"))
			{
				SW_TRACE_DEBUG("Err: failed to umount result hdd partition\n");
			}

			CSWApplication::Sleep(1000);
			HeartBeat();

			//CHAR szCmd[256] = {0};
			//swpa_sprintf(szCmd, "mount -l | grep /Result/1 | grep %s", szPartition);
			//if (SWPAR_OK == swpa_utils_shell(szCmd, NULL))
			//{
			//	SW_TRACE_NORMAL("Info: /Result/1 already mounted \n");
			//}
			//else
			{
				//mount
				iTryCount = 10;
				while (iTryCount--)
				{
					if (0 != swpa_hdd_mount((const CHAR*)szPartition, "/Result/1", NULL))
					{
						swpa_thread_sleep_ms(500);
						swpa_hdd_umount("/Result/1/");
						swpa_hdd_umount(szPartition);
						swpa_thread_sleep_ms(500);
						HeartBeat();
					}
					else
					{
						SW_TRACE_NORMAL("Info: mount %s to /Result/1 -- OK\n", szPartition);
						//尝试写硬盘
						CSWString strTmp = "test";
						int iTryTime = 0;
						while(iTryTime++ < 3)
						{
							CSWFile file;
							if(S_OK == file.Open("/Result/1/.test", "w+") && S_OK == file.Write((LPVOID)(LPCSTR)strTmp, strTmp.Length()))
							{
								break;
							}
						}
						if(iTryTime >= 3)
						{
							goto DO_INIT;
						}						
						break;
					}
				}
				
				if (0 >= iTryCount)
				{
					SW_TRACE_NORMAL("Err: failed to mount %s to /Result/1\n", szPartition);
					goto DO_INIT;//return E_FAIL;
				}			

#ifdef DM8127
			swpa_thread_sleep_ms(3000);
#else
			swpa_thread_sleep_ms(10000);
#endif
				HeartBeat();
			}

			iTryCount = 0;
			do
			{
#ifdef DM8127
				swpa_thread_sleep_ms(1000);
#else
				swpa_thread_sleep_ms(2000);
#endif
				HeartBeat();
				swpa_hdd_get_totalsize("/Result/1", &llTotalSize);
				SW_TRACE_NORMAL("Info: llTotalSize = %lld\n", llTotalSize);
				
			} while (llTotalSize < 1*1024*1024*1024 && iTryCount++ < 10) ;

			if (10 <= iTryCount)
			{
				goto DO_INIT;
			}
		}

		dwFileSize = 1024*(DWORD)GetParam().Get().cResultSenderParam.iFileSize; //KB

		if (FAILED(pObject->Invoke("Initialize", "/Result/1", llTotalSize, dwFileSize, GetParam().Get().cResultSenderParam.iSaveVideoType)))
		{
			SW_TRACE_DEBUG("Err: failed to initialize ResultHDD!\n");
			goto DO_INIT;//return E_FAIL;
		}		

		if (FAILED(pObject->Invoke("SetSaveType", GetParam().Get().cResultSenderParam.iSaveSafeType))
			|| FAILED(pObject->Invoke("SetOutputType", GetParam().Get().cResultSenderParam.iOutputOnlyPeccancy))
			|| FAILED(pObject->Invoke("SetSendInterval", GetParam().Get().cResultSenderParam.iSendRecordSpace*100))
			|| FAILED(pObject->Invoke("SetFilterUnSurePeccancy", GetParam().Get().cResultSenderParam.iFilterUnSurePeccancy))
			)
		{
			SW_TRACE_DEBUG("Err: failed to config ResultHDD!\n");
			goto DO_INIT;//return E_FAIL;
		}
		else
		{
			CSWFile file("BLOCK//tmp/.result_save", "w");
		}
		return S_OK;
DO_INIT:
		if(1 == swpa_utils_file_exist("/tmp/.result_save"))
		{
			SW_TRACE_NORMAL("Reboot in %s\n", __FUNCTION__);
			Exit(2);
		}		
		else
		{
			m_strResultHDDStatus = "结果存储:异常"; 
		}
	}
	
	SW_TRACE_NORMAL("Info: safe saver disabled\n");
	if (FAILED(pObject->Invoke("Initialize", 0, 0, 0, 0))
		|| FAILED(pObject->Invoke("SetOutputType", GetParam().Get().cResultSenderParam.iOutputOnlyPeccancy))
		|| FAILED(pObject->Invoke("SetSendInterval", GetParam().Get().cResultSenderParam.iSendRecordSpace*100))
		|| FAILED(pObject->Invoke("SetFilterUnSurePeccancy", GetParam().Get().cResultSenderParam.iFilterUnSurePeccancy))
		)
	{
		SW_TRACE_DEBUG("Err: failed to initialize ResultHDD!\n");
		return E_FAIL;
	}

	return S_OK;
	
SD_BROKEN:
	m_strResultHDDStatus = "结果存储:SD卡状态异常，无法恢复，请更换或格式化SD卡";  
	SW_TRACE_NORMAL("Err: has file /mnt/nand/SdErrFile, need to format sd card!\n");
	if (FAILED(pObject->Invoke("Initialize", 0, 0, 0, 0))
		|| FAILED(pObject->Invoke("SetOutputType", GetParam().Get().cResultSenderParam.iOutputOnlyPeccancy))
		|| FAILED(pObject->Invoke("SetSendInterval", GetParam().Get().cResultSenderParam.iSendRecordSpace*100))
		|| FAILED(pObject->Invoke("SetFilterUnSurePeccancy", GetParam().Get().cResultSenderParam.iFilterUnSurePeccancy))
		)
	{
		SW_TRACE_DEBUG("Err: failed to initialize ResultHDD!\n");
		return E_FAIL;
	}

	return S_OK;

}



HRESULT CSWLPRApplication::OnResultNetRenderFilterInitialize(CSWObject *pObject)
{
	
	if (GetParam().Get().cResultSenderParam.cAutoLinkParam.fAutoLinkEnable)
	{
		SW_TRACE_DEBUG("Info: AutoLink enabled!\n");
		SW_TRACE_DEBUG("Info: AutoLinkIP = %s!\n", GetParam().Get().cResultSenderParam.cAutoLinkParam.szAutoLinkIP);
		SW_TRACE_DEBUG("Info: AutoLinkPort = %d!\n", GetParam().Get().cResultSenderParam.cAutoLinkParam.iAutoLinkPort);
		
		return pObject->Invoke("Initialize"
	                    , CAMERA_RECORD_LINK_PORT
	                    , GetParam().Get().cResultSenderParam.cAutoLinkParam.iAutoLinkPort
	                    , GetParam().Get().cResultSenderParam.cAutoLinkParam.szAutoLinkIP
	                    , GetParam().Get().cResultSenderParam.fIsSafeSaver
	                );
	}
	else
	{
		SW_TRACE_DEBUG("Info: AutoLink disabled!\n");
		
		return pObject->Invoke("Initialize"
	                    , CAMERA_RECORD_LINK_PORT
	                    , 0
	                    , 0
	                    , 0
	                );
	}
	
}

HRESULT CSWLPRApplication::OnCSWResultFtpRenderFilterInitialize(CSWObject *pObject)
{
 	SW_TRACE_DEBUG("OnCSWResultFtpRenderFilterInitialize\n");
    if (GetParam().Get().cResultSenderParam.cFtpParam.fFtpEnable)
	{
	
		SW_TRACE_DEBUG("Info: FTP enabled!\n");
		SW_TRACE_DEBUG("Info: FTPIP = %s!\n", GetParam().Get().cResultSenderParam.cFtpParam.szFtpIP);
		
		return pObject->Invoke("Initialize"
	                    , GetParam().Get().cResultSenderParam.cFtpParam.szFtpIP
	                    , GetParam().Get().cResultSenderParam.cFtpParam.szUserName
	                    , GetParam().Get().cResultSenderParam.cFtpParam.szPassWord
	                    , GetParam().Get().cResultSenderParam.cFtpParam.iTimeOut
	                );               
	}
	else
	{
		SW_TRACE_DEBUG("Info: FTP disabled!\n");
		return pObject->Invoke("Initialize"
	                    ,0
	                    ,0
	                    ,0
	                    ,0
	                );
	}
	return S_OK;
}

HRESULT CSWLPRApplication::OnJPEGNetRenderFilterInitialize(CSWObject *pObject)
{
	return pObject->Invoke("Initialize"
	                     
	                );
}


HRESULT CSWLPRApplication::OnNetSourceFilterInitialize(CSWObject *pObject)
{
	CAMERA_INFO cCameraInfo;
	swpa_strcpy(cCameraInfo.szIp, GetParam().Get().cCamCfgParam.szIP);
	cCameraInfo.iCameraType = 0;
	cCameraInfo.wPort = GetParam().Get().cCamCfgParam.iCamPort;
	return pObject->Invoke("Initialize"
	                     , (PVOID)(&cCameraInfo)
	                );
}

HRESULT CSWLPRApplication::OnH264SourceFilterInitialize(CSWObject *pObject)
{
	H264_FILTER_PARAM cParam;

	cParam.cOverlayInfo = GetParam().Get().cOverlay.cH264Info;
	cParam.cOverlayInfo.iTextLen = swpa_strlen(cParam.cOverlayInfo.szInfo) + 1;
	
	cParam.cH264Param.iForceFrameType = -1;
	cParam.cH264Param.iTargetBitrate = GetParam().Get().cCamAppParam.iTargetBitRate;
	cParam.cH264Param.intraFrameInterval = GetParam().Get().cCamAppParam.iIFrameInterval;
	cParam.cH264Param.iRateControl = GetParam().Get().cCamAppParam.iRateControl;
	
	switch (GetParam().Get().cCamAppParam.iResolution)
	{
		case 1:
			cParam.cH264Param.iResolution = 720;
		break;

		case 2:
			cParam.cH264Param.iResolution = 1080;
		break;

		default:
			cParam.cH264Param.iResolution = 1080;
		break;
	}
	
	return pObject->Invoke("Initialize", GetParam().Get().cOverlay.fH264Eanble, (PVOID)(&cParam));
}

HRESULT CSWLPRApplication::OnCentaurusH264EncodeFilterInitialize(CSWObject *pObject)
{
	H264_FILTER_PARAM cParam;
	if (!swpa_strcmp(((CSWBaseFilter *)pObject)->GetName(),"CSWCentaurusH264SecondEncodeFilter"))
	{
		SW_TRACE_DEBUG("set h264 encode filter channel 2\n");
		cParam.iChannelNum = 2;
		cParam.cOverlayInfo = GetParam().Get().cOverlay.cH264SecondInfo;
		cParam.cH264Param.iTargetBitrate = GetParam().Get().cCamAppParam.iTargetBitRateSecond;
		cParam.cH264Param.intraFrameInterval = GetParam().Get().cCamAppParam.iIFrameIntervalSecond;
		cParam.cH264Param.iRateControl = GetParam().Get().cCamAppParam.iRateControlSecond;
		if(0 == GetParam().Get().cCamAppParam.iResolutionSecond)
		{
			cParam.cH264Param.iResolution=1080;	
		}
		else if(1 == GetParam().Get().cCamAppParam.iResolutionSecond)
		{
			cParam.cH264Param.iResolution=720;		
		}
		else if(2 == GetParam().Get().cCamAppParam.iResolutionSecond)
		{
			cParam.cH264Param.iResolution=540;
		}
		else if(3 == GetParam().Get().cCamAppParam.iResolutionSecond)
		{
			cParam.cH264Param.iResolution=576;
		}
		else if(4 == GetParam().Get().cCamAppParam.iResolutionSecond)
		{
			cParam.cH264Param.iResolution=480;
		}
	}
	else
	{
		SW_TRACE_DEBUG("set h264 encode filter channel 0\n");
		cParam.iChannelNum = 0;
		cParam.cOverlayInfo = GetParam().Get().cOverlay.cH264Info;
		cParam.cH264Param.iTargetBitrate = GetParam().Get().cCamAppParam.iTargetBitRate;
		cParam.cH264Param.intraFrameInterval = GetParam().Get().cCamAppParam.iIFrameInterval;
		cParam.cH264Param.iResolution = (0 == GetParam().Get().cCamAppParam.iResolution) ? 1080 : 720;
		cParam.cH264Param.iRateControl = GetParam().Get().cCamAppParam.iRateControl;
	}
	
	cParam.cOverlayInfo.iTextLen = swpa_strlen(cParam.cOverlayInfo.szInfo) + 1;
	
	cParam.cH264Param.iForceFrameType = -1;
	//分辨率在ipc初始化时设置
	
//	cParam.cH264Param.iResolution = (0 == GetParam().Get().cCamAppParam.iResolution) ? 1080 : 720;

	return pObject->Invoke("Initialize", (PVOID)(&cParam));

}
HRESULT CSWLPRApplication::OnCentaurusJpegEncodeFilterInitialize(CSWObject *pObject)
{
	return pObject->Invoke("Initialize", (PVOID)NULL);
}

HRESULT CSWLPRApplication::OnRecognizeTransformFilterInitialize(CSWObject *pObject)
{
	INT iParam[3] = {-1, 0, 0};
	SendMessage(MSG_AUTO_CONTROL_READPARAM, (WPARAM)iParam);
	pObject->Invoke("Initialize"
	                     , GetParam().Get().nWorkModeIndex
	                     , iParam[0]
	                     , iParam[1]
	                     , iParam[2]
	                     , (PVOID)&GetParam().Get().cTrackerCfgParam
	                );

	pObject->Invoke("RegisterCallBackFunction", (PVOID)OnDSPAlarm, (PVOID)this);	

	if( GetParam().Get().cCamCfgParam.iDynamicTriggerEnable == 1 )
	{
		GetParam().Get().cCamCfgParam.iDynamicTriggerEnable = 2;
		SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER, GetParam().Get().cCamCfgParam.iDynamicTriggerEnable);	
	}
	
	CSWCarLeft::SetRoadInfo(GetParam().Get().cResultSenderParam.szStreetName, GetParam().Get().cResultSenderParam.szStreetDirection);
	CSWCarLeft::SetOutputMode(
						GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
						GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
						GetParam().Get().cResultSenderParam.iOutputCaptureImage );
	CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);

	pObject->Invoke("SetReverseRunFilterFlag",
		GetParam().Get().cTrackerCfgParam.fFilterReverseEnable
        );

	return S_OK;
}

HRESULT CSWLPRApplication::OnRecognizeTransformTollGateFilterInitialize(CSWObject *pObject)
{
    INT iParam[3] = {-1, 0, 0};
    SendMessage(MSG_AUTO_CONTROL_READPARAM, (WPARAM)iParam);
    pObject->Invoke("Initialize"
                         , GetParam().Get().nWorkModeIndex
                         , iParam[0]
                         , iParam[1]
                         , iParam[2]
                         , (PVOID)&GetParam().Get().cTrackerCfgParam
                    );

    pObject->Invoke("RegisterCallBackFunction", (PVOID)OnDSPAlarm, (PVOID)this);

    if( GetParam().Get().cCamCfgParam.iDynamicTriggerEnable == 1 )
    {
        GetParam().Get().cCamCfgParam.iDynamicTriggerEnable = 2;
        SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER, GetParam().Get().cCamCfgParam.iDynamicTriggerEnable);
    }

    CSWCarLeft::SetRoadInfo(GetParam().Get().cResultSenderParam.szStreetName, GetParam().Get().cResultSenderParam.szStreetDirection);
    CSWCarLeft::SetOutputMode(
                        GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
                        GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
                        GetParam().Get().cResultSenderParam.iOutputCaptureImage );
    CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);

    pObject->Invoke("SetReverseRunFilterFlag",
        GetParam().Get().cTrackerCfgParam.fFilterReverseEnable
        );

    return S_OK;
}

HRESULT CSWLPRApplication::OnRecognizeTransformEPFilterInitialize(CSWObject *pObject)
{
	INT iParam[3] = {-1, 0, 0};
	SendMessage(MSG_AUTO_CONTROL_READPARAM, (WPARAM)iParam);
	pObject->Invoke("Initialize"
						 , GetParam().Get().nWorkModeIndex
						 , iParam[0]
						 , iParam[1]
						 , iParam[2]
						 , (PVOID)&GetParam().Get().cTrackerCfgParam
					);

	pObject->Invoke("RegisterCallBackFunction", (PVOID)OnDSPAlarm, (PVOID)this);

	if( GetParam().Get().cCamCfgParam.iDynamicTriggerEnable == 1 )
	{
		GetParam().Get().cCamCfgParam.iDynamicTriggerEnable = 2;
		SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER, GetParam().Get().cCamCfgParam.iDynamicTriggerEnable);
	}

	CSWCarLeft::SetRoadInfo(GetParam().Get().cResultSenderParam.szStreetName, GetParam().Get().cResultSenderParam.szStreetDirection);
	CSWCarLeft::SetOutputMode(
						GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
						GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
						GetParam().Get().cResultSenderParam.iOutputCaptureImage );
	CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);

	pObject->Invoke("SetReverseRunFilterFlag",
		GetParam().Get().cTrackerCfgParam.fFilterReverseEnable
		);

	// todo.
	// 不能动态 生效。
	pObject->Invoke("SetGB28181Enable", GetParam().Get().cGb28181.fEnalbe);
	return S_OK;
}

HRESULT CSWLPRApplication::OnRecognizeTransformPPFilterInitialize(CSWObject *pObject)
{
	SW_TRACE_DEBUG("OnRecognizeTransformPPFilterInitialize start.\n");
	INT iParam[3] = {-1, 0, 0};
	SendMessage(MSG_AUTO_CONTROL_READPARAM, (WPARAM)iParam);
	pObject->Invoke("Initialize"
						 , GetParam().Get().nWorkModeIndex
						 , iParam[0]
						 , iParam[1]
						 , iParam[2]
					, (PVOID)&GetParam().Get().cTrackerCfgParam
					, (PVOID)&GetParam().Get().cMatchParam
					);

	pObject->Invoke("RegisterCallBackFunction", (PVOID)OnDSPAlarm, (PVOID)this);

	if( GetParam().Get().cCamCfgParam.iDynamicTriggerEnable == 1 )
	{
		GetParam().Get().cCamCfgParam.iDynamicTriggerEnable = 2;
		SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER, GetParam().Get().cCamCfgParam.iDynamicTriggerEnable);
	}

	CSWCarLeft::SetRoadInfo(GetParam().Get().cResultSenderParam.szStreetName, GetParam().Get().cResultSenderParam.szStreetDirection);
	CSWCarLeft::SetOutputMode(
						GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
						GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
						GetParam().Get().cResultSenderParam.iOutputCaptureImage );
	CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);

	// pObject->Invoke("SetReverseRunFilterFlag",
	// 	GetParam().Get().cTrackerCfgParam.fFilterReverseEnable
	// 	);

	// todo.
	// 不能动态 生效。
	pObject->Invoke("SetGB28181Enable", GetParam().Get().cGb28181.fEnalbe);
	SW_TRACE_DEBUG("OnRecognizeTransformPPFilterInitialize done.\n");
	return S_OK;
}

HRESULT CSWLPRApplication::OnRecognizeTransformPTFilterInitialize(CSWObject *pObject)
{
    pObject->Invoke("Initialize",
        GetParam().Get().nWorkModeIndex,
        (PVOID)GetParam().Get().cImgFrameParam.rgstrHVCParm[0],
        (PVOID)&GetParam().Get().cTrackerCfgParam
        );

    pObject->Invoke("SetTollEvasionDetectingFlag",
        GetParam().Get().cResultSenderParam.iUseRushRule
        );

    pObject->Invoke("SetTailgatingTimeThreshold",
        GetParam().Get().cResultSenderParam.iTimeIn
        );

    pObject->Invoke("SetBarrierNormalState",
        !GetParam().Get().cResultSenderParam.iBarrierStatus
        );

     if (GetParam().Get().cResultSenderParam.iEnableBackupIO
        || GetParam().Get().cResultSenderParam.iUseRushRule)
    {
        swpa_camera_io_backupio_enable(1);
    }

    CSWCarLeft::SetOutputMode(
        GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
        GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
        GetParam().Get().cResultSenderParam.iOutputCaptureImage
        );
    CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);
    return S_OK;
}

HRESULT CSWLPRApplication::OnRecognizeTGTransformFilterInitialize(CSWObject *pObject)
{
	INT iParam[3] = {-1, 0, 0};
	SendMessage(MSG_AUTO_CONTROL_READPARAM, (WPARAM)iParam);
	pObject->Invoke("Initialize"
	                     , GetParam().Get().nWorkModeIndex
	                     , iParam[0]
	                     , iParam[1]
	                     , iParam[2]
	                     , (PVOID)&GetParam().Get().cTrackerCfgParam
	                     , GetParam().Get().cCamCfgParam.iMinPlateLight
	                     , GetParam().Get().cCamCfgParam.iMaxPlateLight
	                );

	pObject->Invoke("RegisterCallBackFunction", (PVOID)OnDSPAlarm, (PVOID)this);	

	if( GetParam().Get().cCamCfgParam.iDynamicTriggerEnable == 1 )
	{
		//GetParam().Get().cCamCfgParam.iDynamicTriggerEnable = 2;
		SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER, GetParam().Get().cCamCfgParam.iDynamicTriggerEnable);	
	}
	
	CSWCarLeft::SetRoadInfo(GetParam().Get().cResultSenderParam.szStreetName, GetParam().Get().cResultSenderParam.szStreetDirection);
	CSWCarLeft::SetOutputMode(
						GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
						GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
                        GetParam().Get().cResultSenderParam.iOutputCaptureImage,
                        GetParam().Get().cResultSenderParam.iLastSnapshotCropOutput,
                        GetParam().Get().cResultSenderParam.iCropWidthLevel,
                        GetParam().Get().cResultSenderParam.iCropHeightLevel);
	CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);

	pObject->Invoke("SetReverseRunFilterFlag",
		GetParam().Get().cTrackerCfgParam.fFilterReverseEnable
        );

	return S_OK;
}

HRESULT CSWLPRApplication::OnCentaurusSourceFilterInitialize(CSWObject *pObject)
{

	return pObject->Invoke("Initialize",
        GetParam().Get().cCamAppParam.iCVBSExport,
        GetParam().Get().cCamAppParam.iCVBSCropStartX,
        GetParam().Get().cCamAppParam.iCVBSCropStartY
        );

}

HRESULT CSWLPRApplication::OnJPEGEncodeTransformFilterInitialize(CSWObject *pObject)
{

	INT iValue = (0 == GetParam().Get().cCamAppParam.iJpegAutoCompressEnable) 
		? GetParam().Get().cCamAppParam.iJpegCompressRate : GetParam().Get().cCamAppParam.iJpegExpectSize;


	return pObject->Invoke("Initialize"
					 , GetParam().Get().cCamAppParam.iJpegType
					 , GetParam().Get().cCamAppParam.iJpegAutoCompressEnable
					 , iValue
					 , GetParam().Get().cCamAppParam.iJpegCompressRateL
					 , GetParam().Get().cCamAppParam.iJpegCompressRateH
					 , GetParam().Get().cCamAppParam.iJpegCompressRateCapture
					 , &GetParam().Get().cOverlay.cJPEGInfo
				);

}

HRESULT CSWLPRApplication::OnVPIFSourceFilterInitialize(CSWObject *pObject)
{
	return pObject->Invoke("Initialize");
}

HRESULT CSWLPRApplication::OnMatchTransformFilterInitialize(CSWObject *pObject)
{
	m_fDSPTrigger=GetParam().Get().cCamCfgParam.iDynamicTriggerEnable>0?TRUE:FALSE;
	return pObject->Invoke("Initialize", (PVOID)&GetParam().Get().cMatchParam, (PVOID)&GetParam().Get().cDevParam,m_fDSPTrigger);
}

HRESULT CSWLPRApplication::OnAutoControlRenderFilterInitialize(CSWObject *pObject)
{
	pObject->Invoke("SetNightThresholdArg",
	    GetParam().Get().cCamCfgParam.iNightShutterThreshold,
	    GetParam().Get().cCamCfgParam.iNightAvgYThreshold,
        GetParam().Get().cCamCfgParam.iDuskAvgYThreshold,
	    GetParam().Get().cCamAppParam.iAGCShutterHOri
	    );
	
	// 如果设置了CPL的调节等级是13级，就会只要是白天都要使能偏振镜
	// 偏振镜的控制目前在DSP端实现，其实不太合理。
	int iEnableCpl = (GetParam().Get().cTrackerCfgParam.nCtrlCpl == 1 && GetParam().Get().cTrackerCfgParam.nLightTypeCpl == 13) ? 1 : 0;
	pObject->Invoke("Enable"
								, GetParam().Get().cCamCfgParam.iDynamicCfgEnable
								, GetParam().Get().cCamCfgParam.iEnableAGC
								, GetParam().Get().cCamCfgParam.iMinPSD
								, GetParam().Get().cCamCfgParam.iMaxPSD
								, iEnableCpl
								, GetParam().Get().cTrackerCfgParam.nMinPlateBrightness
								, GetParam().Get().cTrackerCfgParam.nMaxPlateBrightness
					 );
					 
	for(int i = 0; i < 14; i++)
	{
		pObject->Invoke("SetLevel"
									, i
									, GetParam().Get().cCamCfgParam.irgAGCLimit[i]
									, GetParam().Get().cCamCfgParam.irgExposureTime[i]
									, GetParam().Get().cCamCfgParam.irgGain[i]
						 );
	}

	//抓拍参数
	pObject->Invoke("SetAutoCaptureParam",
						GetParam().Get().cCamCfgParam.iCaptureAutoParamEnable,
						GetParam().Get().cCamCfgParam.iCaptureDayShutterMax,
						GetParam().Get().cCamCfgParam.iCaptureDayGainMax,
						GetParam().Get().cCamCfgParam.iCaptureNightShutterMax,
						GetParam().Get().cCamCfgParam.iCaptureNightGainMax
						);

    pObject->Invoke("SetCaptureShutterGain",
                    GetParam().Get().cCamAppParam.iCaptureShutter,
                    GetParam().Get().cCamAppParam.iCaptureGain);

    pObject->Invoke("SetDayNightShutterHOri",
                    GetParam().Get().cCamAppParam.iAGCDayNightShutterControl,
                    GetParam().Get().cCamAppParam.iAGCShutterHOri,
                    GetParam().Get().cCamAppParam.iAGCNightShutterHOri,
                    GetParam().Get().cCamAppParam.iAGCGainHOri,
                    GetParam().Get().cCamAppParam.iAGCNightGainHOri,
                    GetParam().Get().cCamAppParam.iAGCShutterLOri);
    INT I = 0;

	return S_OK;
}

HRESULT CSWLPRApplication::OnResultFilterInitialize(CSWObject *pObject)
{
	return pObject->Invoke("InitRule", (PVOID)&GetParam().Get().cResultSenderParam.cProcRule);
}

HRESULT CSWLPRApplication::OnTriggerOutInitialize(CSWObject *pObject)
{
    return pObject->Invoke("InitTrigger",
        GetParam().Get().cResultSenderParam.fEnableTriggerOut,
        GetParam().Get().cResultSenderParam.nTriggerOutNormalStatus,
        GetParam().Get().cResultSenderParam.nTriggerOutPlusWidth,
        GetParam().Get().cCamAppParam.iCaptureSynOutputType);
}

HRESULT CSWLPRApplication::OnEPoliceEventCheckerTransformFilter(CSWObject *pObject)
{
	return pObject->Invoke("Initialize", GetParam().Get().cResultSenderParam.iCheckEventTime*60000);
}


HRESULT CSWLPRApplication::OnONVIFControlInitialize(CSWObject *pObject)
{
	if (FAILED( pObject->Invoke("Initialize"))
		|| FAILED(pObject->Invoke("Run")))
	{
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CSWLPRApplication::OnRTSPTransformFilterInitialize(CSWObject *pObject)	
{
	if (!swpa_strcmp(((CSWBaseFilter *)pObject)->GetName(),"CSWRTSPSecondTransformFilter"))
	{
		SW_TRACE_DEBUG("set rtsp transform filter channel 2\n");
		return pObject->Invoke("Initialize",2);
	}
	else
	{
		SW_TRACE_DEBUG("set rtsp transform filter channel 0\n");
		return pObject->Invoke("Initialize",0);
	}
}

HRESULT CSWLPRApplication::OnJPEGOverlayFilterInitialize(CSWObject *pObject)
{
	BYTE r, g, b, y, u, v;
	b = (BYTE)(GetParam().Get().cOverlay.cJPEGInfo.iColor & 0x000000FF);
	g = (BYTE)((GetParam().Get().cOverlay.cJPEGInfo.iColor & 0x0000FF00) >> 8);
	r = (BYTE)((GetParam().Get().cOverlay.cJPEGInfo.iColor & 0x00FF0000) >> 16);
	CSWUtils::RGB2YUV(r, g, b, &y, &u, &v);
	CHAR szDevID[64] = {0};
	ReadCustomizedDevName(szDevID);

    DWORD dwArrayRGB[3] = {255, 0, 0};
    dwArrayRGB[0] = r;
    dwArrayRGB[1] = g;
    dwArrayRGB[2] = b;

	return pObject->Invoke("Initialize"
	                      , GetParam().Get().cOverlay.cJPEGInfo.fEnable
	                      , GetParam().Get().cOverlay.cJPEGInfo.fEnableTime
	                      , GetParam().Get().cOverlay.cJPEGInfo.szInfo
	                      , GetParam().Get().cOverlay.cJPEGInfo.iFontSize
	                      , GetParam().Get().cOverlay.cJPEGInfo.iX
	                      , GetParam().Get().cOverlay.cJPEGInfo.iY
	                      , y
	                      , u
	                      , v
	                      , szDevID
                           , GetParam().Get().cTrackerCfgParam.iSpeedLimit
                           , dwArrayRGB
	                      );
}

HRESULT CSWLPRApplication::OnH264OverlayFilterInitialize(CSWObject *pObject)
{
	BYTE r, g, b, y, u, v;
	b = (BYTE)(GetParam().Get().cOverlay.cH264Info.dwColor & 0x000000FF);
	g = (BYTE)((GetParam().Get().cOverlay.cH264Info.dwColor & 0x0000FF00) >> 8);
	r = (BYTE)((GetParam().Get().cOverlay.cH264Info.dwColor & 0x00FF0000) >> 16);
	CSWUtils::RGB2YUV(r, g, b, &y, &u, &v);
	CHAR szDevID[64] = {0};
	ReadCustomizedDevName(szDevID);

    DWORD dwArrayRGB[3] = {255, 0, 0};
    dwArrayRGB[0] = r;
    dwArrayRGB[1] = g;
    dwArrayRGB[2] = b;

	return pObject->Invoke("Initialize"
	                      , GetParam().Get().cOverlay.fH264Eanble
	                      , GetParam().Get().cOverlay.cH264Info.fEnableTime
	                      , GetParam().Get().cOverlay.cH264Info.szInfo
	                      , GetParam().Get().cOverlay.cH264Info.iFontSize
	                      , GetParam().Get().cOverlay.cH264Info.iTopX
	                      , GetParam().Get().cOverlay.cH264Info.iTopY
	                      , y
	                      , u
	                      , v
	                      , szDevID
                           , GetParam().Get().cTrackerCfgParam.iSpeedLimit
                           , dwArrayRGB
	                      );
}

HRESULT CSWLPRApplication::OnH264SecondOverlayFilterInitialize(CSWObject *pObject)
{
	BYTE r, g, b, y, u, v;
	
	b = (BYTE)(GetParam().Get().cOverlay.cH264SecondInfo.dwColor & 0x000000FF);
	g = (BYTE)((GetParam().Get().cOverlay.cH264SecondInfo.dwColor & 0x0000FF00) >> 8);
	r = (BYTE)((GetParam().Get().cOverlay.cH264SecondInfo.dwColor & 0x00FF0000) >> 16);
	CSWUtils::RGB2YUV(r, g, b, &y, &u, &v);
	CHAR szDevID[64] = {0};
	ReadCustomizedDevName(szDevID);
	return pObject->Invoke("Initialize"
	                      , GetParam().Get().cOverlay.fH264SecondEnable
	                      , GetParam().Get().cOverlay.cH264SecondInfo.fEnableTime
	                      , GetParam().Get().cOverlay.cH264SecondInfo.szInfo
	                      , GetParam().Get().cOverlay.cH264SecondInfo.iFontSize
	                      , GetParam().Get().cOverlay.cH264SecondInfo.iTopX
	                      , GetParam().Get().cOverlay.cH264SecondInfo.iTopY
	                      , y
	                      , u
	                      , v
	                      , szDevID
                           , GetParam().Get().cTrackerCfgParam.iSpeedLimit
	                      );
}

HRESULT CSWLPRApplication::OnGBH264TransformFilterInitialize(CSWObject *pObject)
{
 	
    HRESULT hd = pObject->Invoke("Initialize");
    SW_TRACE_DEBUG("OnGBH264TransformFilterInitialize %d",hd);
    return hd;
}
HRESULT CSWLPRApplication::OnH264RTPFilterInitialize(CSWObject *pObject)
{

    HRESULT hd = pObject->Invoke("Initialize");
    SW_TRACE_DEBUG("OnH264RTPFilterInitialize %d",hd);
    return hd;
}

HRESULT CSWLPRApplication::OnJPEGCachTransformFilterInitialize(CSWObject *pObject)
{
    INT iParam[3] = {-1, 0, 0};
    SendMessage(MSG_AUTO_CONTROL_READPARAM, (WPARAM)iParam);
    pObject->Invoke("Initialize"
                         , GetParam().Get().nWorkModeIndex
                         , iParam[0]
                         , iParam[1]
                         , iParam[2]
                         , (PVOID)&GetParam().Get().cTrackerCfgParam
                    );

    pObject->Invoke("RegisterCallBackFunction", (PVOID)OnDSPAlarm, (PVOID)this);

    if( GetParam().Get().cCamCfgParam.iDynamicTriggerEnable == 1 )
    {
        GetParam().Get().cCamCfgParam.iDynamicTriggerEnable = 2;
        SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER, GetParam().Get().cCamCfgParam.iDynamicTriggerEnable);
    }

    return S_OK;
}

HRESULT CSWLPRApplication::OnCSWDomeRockerControlMSGInitialize(CSWObject *pObject)
{
    return pObject->Invoke("Initialize"
                    , GetParam().Get().cDomeRockerParam.fRockerEnable
                    , GetParam().Get().cDomeRockerParam.iBaudrate
                    , GetParam().Get().cDomeRockerParam.iProtocol
                   );
}



HRESULT CSWLPRApplication::OnCSWH264QueueRenderFilterInitialize(CSWObject *pObject)
{
	 return pObject->Invoke("Initialize");
}

HRESULT CSWLPRApplication::OnCSWResultVideoTransformFilterInitialize(CSWObject *pObject)
{
	 return pObject->Invoke("Initialize");
}

HRESULT CSWLPRApplication::OnMD5EncryptFilterInitialize(CSWObject *pObject)
{
	char szKey[1024] = {0};	
	int iKeyLen = 8;
	BOOL fEnable = TRUE;
	return pObject->Invoke("InitMD5EncryptFilter", fEnable, (PVOID)szKey, iKeyLen);
}
HRESULT CSWLPRApplication::OnCSWExtendDataTransformFilterInitialize(CSWObject *pObject)
{
	return pObject->Invoke("Initialize", NULL);
}

HRESULT CSWLPRApplication::OnSetTime(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	SWPA_DATETIME_TM *tm = (SWPA_DATETIME_TM *)wParam;
	if(NULL != tm)
	{
		CSWDateTime dt;
		dt.SetTime(*tm);
#ifdef	DM8127		//金星有时区文件，已经是GMT8
		swpa_utils_shell("hwclock -w  --utc", NULL);
#else
		swpa_utils_shell("hwclock -w", NULL);
#endif

#if DM6467
		// 同步368的时间
		SendMessage(MSG_DM368_SYNC_TIME,0,0);
#endif
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetTime(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetTime(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;

	SWPA_DATETIME_TM* pTime = (SWPA_DATETIME_TM*)lParam;

	
	if(NULL != pTime)
	{
		SWPA_DATETIME_TM tm;
		CSWDateTime dt;
		dt.GetTime(&tm);

		pTime->year = tm.year;
		pTime->month = tm.month;
		pTime->day = tm.day;
		pTime->hour = tm.hour;
		pTime->min = tm.min;
		pTime->sec = tm.sec;
		pTime->msec = tm.msec;

		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetTime(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnSetIP(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	CHAR **szNetParam = (CHAR **)wParam;

	if(NULL != szNetParam)
	{
		CHAR szTmp[4][255];
		CSWString strIp = (const CHAR*)szNetParam[0];
		CSWString strMask = (const CHAR*)szNetParam[1];
		CSWString strGateway = (const CHAR*)szNetParam[2];

		SW_TRACE_DEBUG("<LPRApplication> onsetip %s %s %s.\n", (const CHAR*)strIp, (const CHAR*)strMask, (const CHAR*)strGateway );
		if( 0 == swpa_tcpip_getinfo("eth0", szTmp[0], 255, szTmp[1], 255, szTmp[2], 255, szTmp[3], 255))
		{
			if( swpa_strcmp((const CHAR*)strIp, szTmp[0]) == 0 
				&& swpa_strcmp((const CHAR*)strMask, szTmp[1]) == 0 
				&& swpa_strcmp((const CHAR*)strGateway, szTmp[2]) == 0)
			{
				SW_TRACE_DEBUG("<LPRApplication> onsetip same.\n" );
				hr = S_OK;
			}
			else if (SWPAR_OK != swpa_tcpip_checkinfo(szNetParam[0], szNetParam[1], szNetParam[2]))
			{
				SW_TRACE_DEBUG("Err: check ip info failed\n");
	            hr = E_FAIL;
			}
			else if( 0 == swpa_device_write_ipinfo(szNetParam[0], szNetParam[1], szNetParam[2]) )
			{
				GetParam().UpdateString("System[系统]\\TcpipCfg[网络配置]", "CommIPAddr", (const CHAR*)strIp);
				GetParam().UpdateString("System[系统]\\TcpipCfg[网络配置]", "CommMask", (const CHAR*)strMask);
				GetParam().UpdateString("System[系统]\\TcpipCfg[网络配置]", "CommGateway", (const CHAR*)strGateway);
				GetParam().Commit();
				hr = S_OK;
				//Exit(2);// reset automatilcally
			}
			else
			{
				SW_TRACE_DEBUG("<LPRApplication> failed to save ip info!\n" );
				hr = E_FAIL;
			}
		}		
	}

	SW_TRACE_DEBUG("CSWLPRApplication::OnSetIP(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);

	return hr;
}

HRESULT CSWLPRApplication::OnGetIP(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	char **szNetParam = (char **)lParam;
	if(NULL != szNetParam && !swpa_tcpip_getinfo("eth0", szNetParam[0], 255, szNetParam[1], 255, szNetParam[2], 255, szNetParam[3], 255))
	{
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetIP(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}


HRESULT SdBroken()
{
	INT fd = -1;
	INT ret_size = 0;
	char buf[10] = "SD Broken";
	fd = swpa_file_open("/mnt/nand/SdErrFile","w+");
	if(fd > 0)
	{
		swpa_file_seek(fd, 0, SWPA_SEEK_SET);
		swpa_file_write(fd, buf, 10, &ret_size);
		if (1 != swpa_utils_file_exist("/mnt/nand/SdErrFile"))
		{
			SW_TRACE_DEBUG("Err: write to /mnt/nand/SdErrFile failes!\n");
			return S_FALSE;
		}
	}
	else
	{
		SW_TRACE_DEBUG("Err: open /mnt/nand/SdErrFile failes!\n");
		return S_FALSE;
	}	
	return S_OK;
}

HRESULT CSWLPRApplication::OnReInitHDD(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("in the OnReInitHDD\n");
	INT iret;
	static DWORD dwPrevID = 0xff;
	static DWORD dwPrevTick = 0;
	BOOL fDoReInit = FALSE;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();
	
	if (dwCurTick > dwPrevTick + 30 * 1000
		|| dwPrevID == (DWORD)wParam)
	{
		fDoReInit = TRUE;
	}

	if (fDoReInit)
	{
		dwPrevTick = dwCurTick;
		dwPrevID = (DWORD)wParam;
		CSWBaseFilter* pH264HddFilter = QueryFilter("CSWH264HDDTransformFilter");
		CSWBaseFilter* pResultFilter = QueryFilter("CSWResultHDDTransformFilter");

		SdErrRecord* sdErrRecord = (SdErrRecord*)swpa_mem_alloc(sizeof(struct SdErrRecord));
		sdErrRecord->id = m_SdErrNum;
		m_SdErrNum++;
		sdErrRecord->time = dwCurTick;
		m_lstSdErrRecord.AddTail(sdErrRecord);

		SW_TRACE_DEBUG("SD arleady write err %u\n",m_SdErrNum);
		
		if(pH264HddFilter)
		{
			if (FAILED(pH264HddFilter->Invoke("ClearBuffer")))
			{
				SW_TRACE_DEBUG("Err: failed to clear H264HDD buffer!\n");
			}
			CSWApplication::Sleep(1000);
		}

		if(pResultFilter)
		{
			if (FAILED(pResultFilter->Invoke("ClearBuffer")))
			{
				SW_TRACE_DEBUG("Err: failed to clear ResultHDD buffer!\n");
			}
			CSWApplication::Sleep(1000);
		}

		if(m_MaxSdErrNum <= m_SdErrNum)
		{
			SdErrRecord* FirstSdErr = m_lstSdErrRecord.GetHead();
			SdErrRecord* LastSdErr = m_lstSdErrRecord.GetTail();
			//一天有86400000毫秒
			SW_TRACE_DEBUG("Sd reinti num has out!\n");
			if(LastSdErr->time - FirstSdErr->time < 86400000)
			{
				SW_TRACE_DEBUG("Because of the sd card Broken,close SD Storage!!\n");

				SdBroken();

				m_strH264HDDStatus = "H264存储:SD卡状态异常，无法恢复，请更换或格式化SD卡";
				m_strResultHDDStatus = "结果存储:SD卡状态异常，无法恢复，请更换或格式化SD卡";  
				
				while (!m_lstSdErrRecord.IsEmpty())
				{
					SdErrRecord* SdErr = m_lstSdErrRecord.RemoveHead();
					swpa_mem_free(SdErr);
				}
				iret = swpa_device_sd_reinit();
				return S_OK;
			}
			m_lstSdErrRecord.RemoveHead();
			swpa_mem_free(FirstSdErr);
		}
		
		iret = swpa_device_sd_reinit();
		if(iret != 0)
			return S_FALSE;
			
		if(pH264HddFilter)
		{			
			OnH264HDDTransformFilterInitialize(pH264HddFilter);
			SAFE_RELEASE(pH264HddFilter);
		}

		
		if(pResultFilter)
		{
			OnResultHDDTransformFilterInitialize(pResultFilter);
			SAFE_RELEASE(pResultFilter);
		}
	}
	else
	{
		SW_TRACE_DEBUG("Info: do nothing!\n");
	}

	return S_OK;
}




HRESULT CSWLPRApplication::OnSetXML(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetXML(0x%08x, 0x%08x)", wParam, lParam);
	HRESULT hr = E_FAIL;
	RTSP_PARAM_INFO cRtspParamInfo;
	ONVIF_PARAM_INFO cOnvifParamInfo;
	swpa_memcpy(&cRtspParamInfo, &GetParam().Get().cRtspParam, sizeof(RTSP_PARAM_INFO));
	swpa_memcpy(&cOnvifParamInfo,&GetParam().Get().cOnvifParam,sizeof(ONVIF_PARAM_INFO));
	char *szXML = (char *)wParam;
	if(NULL != szXML)
	{
		hr = GetParam().LoadFromXml(szXML, swpa_strlen(szXML) + 1);
		if(S_OK == hr)
		{
			GetParam().Commit();
			// 动态修改识别参数
			InitParameter();
			SendMessage(MSG_RECOGNIZE_MODIFY_PARAM,
					(WPARAM)&GetParam().Get().cTrackerCfgParam,0);

			SendMessage(MSG_RECOGNIZE_MODIFY_PECCANCYPARKING_PARAM,
						(WPARAM)&GetParam().Get().cMatchParam, 0);
			SendMessage(MSG_RECOGNIZE_ENABLE_TRIGGER,
				(WPARAM)GetParam().Get().cCamCfgParam.iDynamicTriggerEnable, 0);
			
			//auto conctl
			CSWBaseFilter* pFilter = QueryFilter("CSWAutoControlRenderFilter");
			if(pFilter)
			{
				OnAutoControlRenderFilterInitialize(pFilter);
				SAFE_RELEASE(pFilter);
			}
		
			//外总控
			pFilter = QueryFilter("CSWMatchTransformFilter");
			if(pFilter)
			{
				OnMatchTransformFilterInitialize(pFilter);
				SAFE_RELEASE(pFilter);
			}
			//后处理
			pFilter = QueryFilter("CSWResultFilter");
			if(pFilter)
			{
				OnResultFilterInitialize(pFilter);
				SAFE_RELEASE(pFilter);
			}
			
			//路口名称，方向
			CSWCarLeft::SetRoadInfo(GetParam().Get().cResultSenderParam.szStreetName, GetParam().Get().cResultSenderParam.szStreetDirection);
			CSWCarLeft::SetOutputMode(
								GetParam().Get().cResultSenderParam.iBestSnapshotOutput,
								GetParam().Get().cResultSenderParam.iLastSnapshotOutput,
                                GetParam().Get().cResultSenderParam.iOutputCaptureImage,
                                GetParam().Get().cResultSenderParam.iLastSnapshotCropOutput,
                                GetParam().Get().cResultSenderParam.iCropWidthLevel,
                                GetParam().Get().cResultSenderParam.iCropHeightLevel);
			CSWCarLeft::SetOutputHSLFlag(GetParam().Get().cTrackerCfgParam.fProcessPlate_BlackPlate_Enable);

			// 红灯加红
			// 红灯加红区域设置
			const int MAX_RED_RECT_COUNT = 8;
			SW_RECT rgcRect[MAX_RED_RECT_COUNT];
			swpa_memset(rgcRect, 0, sizeof(SW_RECT) * MAX_RED_RECT_COUNT);
			INT iRectCount = MAX_RED_RECT_COUNT;
			GetRedRectInfo(rgcRect, iRectCount);

			SW_TRACE_DEBUG("<onsetxml>red rectcount:%d.", iRectCount);
			for(int i = 0; i < iRectCount; ++i)
			{
				SW_TRACE_DEBUG("<onsetxml red>%d:(%d,%d,%d,%d).", i,
						rgcRect[i].left, rgcRect[i].top, rgcRect[i].right, rgcRect[i].bottom);
			}
			SendMessage(MSG_SET_REDLIGHT_RECT, (WPARAM)rgcRect, MAX_RED_RECT_COUNT);

			int iLumLThreshold = GetParam().Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight ? 0x2d00 : 0x3fff;
			SendMessage(MSG_SET_TRAFFIC_LIGTH_LUM_TH, (WPARAM)iLumLThreshold, 0);

			//绿灯加绿
			swpa_memset(rgcRect, 0, sizeof(SW_RECT) * MAX_RED_RECT_COUNT);
			iRectCount = MAX_RED_RECT_COUNT;
			GetGreenRectInfo(rgcRect, iRectCount);

			SW_TRACE_DEBUG("<onsetxml>green rectcount:%d.", iRectCount);
			for(int i = 0; i < iRectCount; ++i)
			{
				SW_TRACE_DEBUG("<onsetxml green>%d:(%d,%d,%d,%d).", i,
						rgcRect[i].left, rgcRect[i].top, rgcRect[i].right, rgcRect[i].bottom);
			}
			SendMessage(MSG_SET_GREENLIGHT_RECT, (WPARAM)rgcRect, MAX_RED_RECT_COUNT);

			iLumLThreshold = GetParam().Get().cTrackerCfgParam.cTrafficLight.fEnhanceRedLight ? 0x2d00 : 0x3fff;
			SendMessage(MSG_SET_TRAFFIC_GREEN_LIGTH_LUM_TH, (WPARAM)iLumLThreshold, 0);

			//重启RTSP APP
			if(0 != swpa_memcmp(&cRtspParamInfo, &GetParam().Get().cRtspParam, sizeof(RTSP_PARAM_INFO)))
			{
				SendRemoteMessage(MSG_RTSP_APP_RESTART, NULL, 0);
			}


			if(0 != swpa_memcmp(&cOnvifParamInfo, &GetParam().Get().cOnvifParam, sizeof(ONVIF_PARAM_INFO)))
			{
				SendRemoteMessage(MSG_ONVIF_APP_RESTART, NULL, 0);
			}
			
			//球机保存图像冻结状态
			SendMessage(MSG_SAVE_FREEZEFLAG, 0, 0);
		}
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetXML(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetXML(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	CSWString *strXML = (CSWString *)lParam;
	if(NULL != strXML)
	{
		hr = GetParam().ToXml(*strXML);
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetXML(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetWorkModeCount(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	INT* iCount = (INT *)lParam;
	if(NULL != iCount)
	{
		*iCount = 1;
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetWorkModeCount(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetWorkMode(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
#if 1
    //INT* iWorkMode = (INT *)wParam;
    INT iWorkMode;
    CHAR *strWorkMode = (CHAR *)lParam;
	if(NULL != strWorkMode)
	{
		CSWString strMode;
        hr = GetParam().GetWorkMode(iWorkMode, strMode);
		if( hr == S_OK )
		{
			swpa_strcpy(strWorkMode, (const CHAR*)strMode);
		}
	}
#else
	INT* iWorkMode = (INT *)lParam;
	if (NULL != iWorkMode)
	{
		*iWorkMode = 0;
		hr = S_OK;
	}
#endif

	SW_TRACE_DEBUG("CSWLPRApplication::OnGetWorkMode(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetWorkModeFromIndex(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;	
	INT* iWorkMode = (INT *)wParam;

	char *strWorkMode = (CHAR *)lParam;
	if(NULL != iWorkMode && NULL != strWorkMode)
	{
		CSWString strMode;
		hr = GetParam().GetWorkMode(*iWorkMode, strMode);
		if( hr == S_OK )
		{
			swpa_strcpy(strWorkMode, (const CHAR*)strMode);
		}
	}

	SW_TRACE_DEBUG("CSWLPRApplication::OnGetWorkModeFromIndex(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnResetDevice(WPARAM wParam, LPARAM lParam)
{
	INT iResetCode = (INT)wParam;
	SW_TRACE_DEBUG("CSWLPRApplication::OnResetDevice(0x%08x, 0x%08x)", (DWORD)wParam, (DWORD)lParam);
	HeartBeat(CSWDateTime::GetSystemTick() + 60000);
	
	
	if(NULL == GetCommandString("-test") && iResetCode == 1)
	{
		swpa_device_set_resetmode(2);//set resetmode to Upgrade-Mode
	}
	else if(NULL == GetCommandString("-test") && iResetCode == 3)
	{
#ifdef DM6467	
		swpa_device_set_bootcount(4);
#endif
#ifdef DM8127
        swpa_device_set_bootsystem(1);
        swpa_device_set_backup_bootcount(0);
		swpa_device_set_bootcount_flag(1);
#endif
		iResetCode = 2;//force to reboot to Backup system
	}

	SW_TRACE_DEBUG("Aready In OnResetDevice!\n");
	SendRemoteMessage(MSG_GB28181_QUIT,NULL,0);

	//如果是球机停止转动
	CSWMessage::SendMessage(MSG_TOUCH_DOME_CAMERA, 0, 0);
	
	
	return Exit(iResetCode);
}

HRESULT CSWLPRApplication::OnRestoreDefault(WPARAM wParam, LPARAM lParam)
{
	GetParam().Clear();
	GetParam().InitSystem();
	GetParam().Commit();

	// 如果是球机需要恢复球机的参数
	CSWMessage::SendMessage(MSG_RESETDEFAULT_DOMECAMERA,NULL,NULL);

	SW_TRACE_DEBUG("CSWLPRApplication::OnRestoreDefault(0x%08x, 0x%08x)", wParam, lParam);
	return S_OK;
}

HRESULT CSWLPRApplication::OnRestoreFactory(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnRestoreFactory(0x%08x, 0x%08x)", wParam, lParam);

	//清除参数
	GetParam().Clear();
	//GetParam().Commit();
	
	CSWFile objAppParamFile;
	const CHAR* szParamFileName[] = {"EEPROM/0/APP_PARAM", "EEPROM/0/APP_PARAM_BAK"};
	BYTE bInvalidData[1024] = {0x0};

	SW_TRACE_NORMAL("Info: Clear App Param.....\n");

	for (INT iCount=0; iCount < 2; iCount++)
	{
		if (FAILED(objAppParamFile.Open(szParamFileName[iCount], "w")))
		{
			SW_TRACE_NORMAL("Err: failed to open %s\n", szParamFileName[iCount]);
			return E_FAIL;
		}
		else
		{
			if (FAILED(objAppParamFile.Write(bInvalidData, sizeof(bInvalidData))))
			{
				SW_TRACE_NORMAL("Err: failed to write %s\n", szParamFileName[iCount]);
				return E_FAIL;
			}
			objAppParamFile.Close();
		}
	}

	SW_TRACE_NORMAL("Info: Clear App Param.....OK\n");

	// 恢复IP出厂默认。
	CHAR aszNetInfo[4][256] = {{0}};
	CHAR* pszNetInfo[4];
	pszNetInfo[0] = (CHAR*)swpa_mem_alloc(64);
	pszNetInfo[1] = (CHAR*)swpa_mem_alloc(64);
	pszNetInfo[2] = (CHAR*)swpa_mem_alloc(64);
	pszNetInfo[3] = (CHAR*)swpa_mem_alloc(64);
	swpa_strcpy(pszNetInfo[0], "100.100.100.101");
	swpa_strcpy(pszNetInfo[1], "255.0.0.0");
	swpa_strcpy(pszNetInfo[2], "100.100.1.1");
	swpa_strcpy(pszNetInfo[3], "NULL");
	OnSetIP((WPARAM)pszNetInfo,0);
	swpa_mem_free(pszNetInfo[0]);
	swpa_mem_free(pszNetInfo[1]);
	swpa_mem_free(pszNetInfo[2]);
	swpa_mem_free(pszNetInfo[3]);


	//清除复位记录
	CSWFile cFile;
	if( S_OK == cFile.Open("EEPROM/0/APP_RESET_INFO", "w") )
	{
		DWORD dwSize = 0;
		if (FAILED(cFile.IOCtrl(SWPA_FILE_IOCTL_GET_MAX_SIZE, &dwSize)))
		{
			dwSize = 1024;
			SW_TRACE_DEBUG("CSWLPRApplication::OnRestoreFactory: failed to get RESET_INFO file size, use %d!\n", dwSize);
		}

		BYTE* pbBuf = (BYTE*)swpa_mem_alloc(dwSize);
		if (NULL == pbBuf)
		{
			SW_TRACE_DEBUG("CSWLPRApplication::OnRestoreFactory: failed to alloc mem\n");
		}
		else
		{
			swpa_memset(pbBuf, 0x0, dwSize);
			if (FAILED(cFile.Write(pbBuf, dwSize)))
			{
				SW_TRACE_DEBUG("CSWLPRApplication::OnRestoreFactory: failed to clear RESET_INFO\n");
			}
			swpa_mem_free(pbBuf);
		}

		cFile.Close();
	}
	else
	{
		SW_TRACE_DEBUG("CSWLPRApplication::OnRestoreFactory: failed to open RESET_INFO file!\n");
	}
	
	//用户信息恢复默认
	CSWMessage::SendMessage(MSG_USER_RESTORE_DEFAULT,NULL,NULL);
	
	// 如果是球机需要恢复球机的参数
	CSWMessage::SendMessage(MSG_RESTOREFACTORY_DOMECAMERA,NULL,NULL);

	// 重启系统
	//Exit(2);// reset automatilcally
	return S_OK;
}

/*
HRESULT CSWLPRApplication::OnSetCharacterEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetCharacterEnable(0x%08x, 0x%08x)", wParam, lParam);
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetCharacterEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetCharacterEnable(0x%08x, 0x%08x)", wParam, lParam);
	return S_OK;
}
*/

HRESULT CSWLPRApplication::OnGetHvName(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	char* strName = (char*)lParam;
	if(NULL != strName)
	{
		swpa_strcpy(strName, "Mercury-Video-Beta");
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetHvName(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetHvID(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	INT* iID = (INT*)lParam;
	if(NULL != iID)
	{
		*iID = 0;
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetHvID(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetProductName(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	CHAR *strName = (CHAR *)lParam;
	if(NULL != strName)
	{
		char szName[256];
		UINT uiLen = 256;

		if( 0 == swpa_device_read_sn(szName, &uiLen) )
		{
			swpa_strcpy(strName, szName);
			strName[uiLen] = 0;
			hr = S_OK;
		}
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetProductName(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetVideoCount(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	INT* iCount = (INT *)lParam;
	if(NULL != iCount)
	{
		*iCount = 0;
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetVideoCount(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;	
}

HRESULT CSWLPRApplication::OnGetVersion(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	CHAR *szVersion = (char *)lParam;
	swpa_strcpy(szVersion, GetParam().GetVersion());
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetVersion(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;	
}

HRESULT CSWLPRApplication::OnGetVersionString(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	CHAR *strVersion = (CHAR *)lParam;
	if(NULL != strVersion)
	{
		swpa_sprintf(strVersion, "DSP:%d,SWFC:%d,BaseModule:%d, swpa:%d", GetDSPVersion(), CSWApplication::GetVersion(), CSWBaseGraph::GetVersion(), swpa_version());
		//strVersion->Format("DSP:%d,SWFC:%d,BaseModule:%d, swpa:%d", GetDSPVersion(), CSWApplication::GetVersion(), CSWBaseGraph::GetVersion(), swpa_version());
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetVersionString(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetConnectedIP(WPARAM wParam, LPARAM lParam)
{
	CHAR* szInfo = (CHAR*)wParam;
	if( szInfo != NULL )
	{
		CSWString strInfo;
		strInfo.Append(m_strResultNetStatus);
		strInfo.Append("\n");
		strInfo.Append(m_strJpegNetStatus);
		strInfo.Append("\n");
		strInfo.Append(m_strH264NetStatus);
		strInfo.Append("\n");
		strInfo.Append(m_strRTSPConnStatus);
		strInfo.Append("\n");
		strInfo.Append(m_strRTSPSecondConnStatus);
		strInfo.Append("\n");
		swpa_strcpy(szInfo, (const CHAR*)strInfo);
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetConnectedIP(0x%08x, 0x%08x)\n", wParam, lParam);
	return S_OK;
}

HRESULT CSWLPRApplication::OnHDDFDisk(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	DWORD dwPartitionCount = (DWORD)wParam;

#ifdef DM8127
	INT iSDCardStatus = -1;
	if (SWPAR_OK != swpa_device_get_sdcard_status(&iSDCardStatus)
		|| 0 != iSDCardStatus)
	{
		SW_TRACE_DEBUG("CSWLPRApplication::OnHDDFDisk: No SD Card found!\n");
		return E_FAIL;
	}
#endif

	if (0 == dwPartitionCount || 2 < dwPartitionCount)
	{
		SW_TRACE_DEBUG("CSWLPRApplication::OnHDDFDisk: invalide partition count(%d)\n", dwPartitionCount);
		hr = E_INVALIDARG;
	}
	else
	{
		CSWFile cPartInfoFile;
		if (FAILED(cPartInfoFile.Open("/tmp/HDDPartInfo.dat", "w"))
			|| FAILED(cPartInfoFile.Write(&dwPartitionCount, sizeof(dwPartitionCount))))
		{
			SW_TRACE_DEBUG("CSWLPRApplication::OnHDDFDisk: failed to save hdd partition info!\n");
			hr = E_FAIL;
		}
		else if (FAILED(GetParam().UpdateEnum("\\HvDsp\\Misc", "InitHdd", 1))
			|| FAILED(GetParam().Commit()))
		{
			SW_TRACE_DEBUG("CSWLPRApplication::OnHDDFDisk: failed to fdisk and format HDD!\n");
			hr = E_FAIL;
		}
		else
		{
			Exit(0);//restart LprApp
		}

	}

	SW_TRACE_DEBUG("CSWLPRApplication::OnHDDFDisk(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	
	return hr;
}

HRESULT CSWLPRApplication::OnHDDCheck(WPARAM wParam, LPARAM lParam)
{
	INT iMode = (INT)wParam;
	HRESULT hr = !swpa_hdd_check(iMode) ? S_OK : E_FAIL;
	SW_TRACE_DEBUG("CSWLPRApplication::OnHDDCheck(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnHDDStatus(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	LPCSTR szDev = "/dev/sda";
    CHAR *strStatus = (CHAR*)lParam;
	
	if (NULL == strStatus)
    {
    	hr = E_INVALIDARG;
        SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
		return hr;
    }
	
#ifdef DM8127
	INT iSDCardStatus = -1;
	if (SWPAR_OK != swpa_device_get_sdcard_status(&iSDCardStatus) 
		|| (0 != iSDCardStatus))
	{
		SW_TRACE_DEBUG("[SD card] iSDCardStatus = %d\n", iSDCardStatus);
		swpa_strcpy(strStatus, "状态: 未找到SD卡设备");
        hr = S_OK;
		SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
		return hr;
	}
	
	szDev = "/dev/mmcblk0";
#endif

	INT iHDDFdiskFlag = swpa_hdd_query(0);
	if (1 == iHDDFdiskFlag)
	{
		swpa_strcat(strStatus, "状态: 格式化中");
		hr = S_OK;
	}
	else if (2 == iHDDFdiskFlag)
	{
		swpa_strcat(strStatus, "状态: 格式化完成");
		hr = S_OK;
	}
	else if (3 == iHDDFdiskFlag)
	{
		swpa_strcat(strStatus, "状态: 格式化失败！");
		hr = S_OK;
	}
	else if (4 == iHDDFdiskFlag)
	{
		swpa_strcat(strStatus, "状态: 分区失败！");
		hr = S_OK;
	}
	else
	{
#if 0	
		swpa_strcat(strStatus, "状态: 正常使用中\n");
		LONGLONG llSize = -1;
	
		if (SWPAR_OK != swpa_hdd_get_totalsize(szDev, &llSize))
		{		
			SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus: failed to get total size\n");
			swpa_strcat(strStatus, "总大小: 未知\n");
		}
		else
		{
			CHAR szInfo[256] = {0};
			swpa_sprintf(szInfo, "总大小: %lldMB = %lldGB\n", llSize>>20, llSize>>30);
			swpa_strcat(strStatus, szInfo);
		}

		llSize = -1;

		for (INT iPartNo = 1; iPartNo < 9; iPartNo++)
		{
			CHAR szPartName[32] = {0};
#ifdef DM6467			
			swpa_sprintf(szPartName, "/dev/sda%d", iPartNo);
#endif
#ifdef DM8127			
			swpa_sprintf(szPartName, "/dev/mmcblk0p%d", iPartNo);
#endif

			if (!swpa_utils_file_exist(szPartName))
			{
				break;
			}

			LONGLONG llSizeTmp = 0;
			if (SWPAR_OK != swpa_hdd_get_availsize(szPartName, &llSizeTmp))
			{
				SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus: failed to get available size of %s\n", szPartName);
				//break;
			}

			llSize = (-1 == llSize) ? 0 : llSize;
			llSize += llSizeTmp;
		}

		if (0 >= llSize)
		{		
			SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus: failed to get available size\n");
			swpa_strcat(strStatus, "可用大小: 未知\n");
		}
		else
		{
			CHAR szInfo[256] = {0};
			swpa_sprintf(szInfo, "可用大小: %lldMB = %lldGB\n", llSize>>20, llSize>>30);
			swpa_strcat(strStatus, szInfo);
		}
#else
		DWORD dwTotalSize = 0, dwAvailSize = 0;
		if ( !m_strH264HDDStatus.IsEmpty()
			|| !m_strResultHDDStatus.IsEmpty())
		{
			swpa_strcat(strStatus, "状态: 正常使用中\n");
			
			DWORD dwTempTotal = 0, dwTempAvail = 0;
			if (!m_strH264HDDStatus.IsEmpty())
			{
				CHAR szStoreInfo[128] = {0};
				m_strH264HDDStatus.Copy(szStoreInfo, m_strH264HDDStatus.Length() - CSWString("H264存储:").Length(), CSWString("H264存储:").Length());
				SW_TRACE_DEBUG("Info: H264 Store info: %s\n", szStoreInfo);
				swpa_sscanf(szStoreInfo, "%dMB / %dMB", &dwTempAvail, &dwTempTotal );
				dwTotalSize += dwTempTotal;
				dwAvailSize += dwTempAvail;
			}

			dwTempTotal = 0; dwTempAvail = 0;
			if (!m_strResultHDDStatus.IsEmpty())
			{
				CHAR szStoreInfo[128] = {0};
				m_strResultHDDStatus.Copy(szStoreInfo, m_strResultHDDStatus.Length() - CSWString("结果存储:").Length(), CSWString("结果存储:").Length());
				SW_TRACE_DEBUG("Info: result Store info: %s\n", szStoreInfo);
				swpa_sscanf(szStoreInfo, "%dMB / %dMB", &dwTempAvail, &dwTempTotal );
				dwTotalSize += dwTempTotal;
				dwAvailSize += dwTempAvail;
			}

			CHAR szInfo[256] = {0};
			LONGLONG llSize = -1;
			if (SWPAR_OK != swpa_hdd_get_totalsize(szDev, &llSize))
			{		
				SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus: failed to get total size\n");
			}
			else
			{
				dwTotalSize = llSize >> 20;
			}
			swpa_sprintf(szInfo, "总大小: %dMB = %dGB\n", dwTotalSize, dwTotalSize>>10);
			swpa_strcat(strStatus, szInfo);

			swpa_sprintf(szInfo, "可用大小: %dMB = %dGB\n", dwAvailSize, dwAvailSize>>10);
			swpa_strcat(strStatus, szInfo);
		}
		else
		{
			swpa_strcat(strStatus, "状态: 正常使用中\n");
			LONGLONG llSize = -1;
	
			if (SWPAR_OK != swpa_hdd_get_totalsize(szDev, &llSize))
			{		
				SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus: failed to get total size\n");
				swpa_strcat(strStatus, "总大小: 未知\n");
			}
			else
			{
				CHAR szInfo[256] = {0};
				swpa_sprintf(szInfo, "总大小: %lldMB = %lldGB\n", llSize>>20, llSize>>30);
				swpa_strcat(strStatus, szInfo);
			}
		}
#endif
		hr = S_OK;
	}

	
	SW_TRACE_DEBUG("CSWLPRApplication::OnHDDStatus(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnResetCount(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	INT *iCount = (INT *)lParam;
	if(NULL != iCount)
	{
		*iCount = GetResetCount();
		hr = S_OK;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnResetCount(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnResetReport(WPARAM wParam, LPARAM lParam)
{
	CHAR *buffer = (CHAR *)lParam;
	HRESULT hr = S_OK;
	if( buffer != NULL )
	{
		CSWString strResetInfo;

		char szName[256];
		UINT uiLen = 256;
		if( 0 == swpa_device_read_sn(szName, &uiLen) )
		{
			strResetInfo.Append(szName);
			strResetInfo.Append("\n");
		}
		strResetInfo.Append(GetResetInfo());

		SW_TRACE_DEBUG("<OnResetReport>%d.\n %s.\n", strResetInfo.Length(), (const CHAR*)strResetInfo);
        swpa_strncpy(buffer, (const CHAR*)strResetInfo,16*1024-1);//string 消息 16*1024
	}
	else
	{
		hr = E_INVALIDARG;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnResetReport(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnResetMode(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	INT *iMode = (INT *)lParam;
	if(NULL != iMode)
	{
		/* only when bootcount is disabled (means run over 30sec after upgrading or mode-switching), 
		    then return the correct ResetMode */
		INT iFlag = -1;
		if(TRUE == m_fIsAppReady		//该标记存放在EEPROM中,不能太频繁的读取EEPROM
			/* SWPAR_OK == swpa_device_get_bootcount_flag(&iFlag)
			&& iFlag == 0*/)
		{
			*iMode = 0;
			hr = S_OK;
		}
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnResetMode(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetDevType(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	CHAR *strName = (CHAR*)lParam;
	if(NULL != strName)
    {
#ifdef DM8127
		
		swpa_strcpy(strName, "Unknown"); //Unknown

		if (IsVenus())
		{

			INT iSensor = swpa_get_sensor_type();
	        if (SENSOR_IMX185 == iSensor )
	        {
				swpa_strcpy(strName, "PCC200"); //200W
	    	}
			else if (SENSOR_IMX178 == iSensor)
			{
				swpa_strcpy(strName, "PCC600"); //600W
			}
			else if (SENSOR_IMX174 == iSensor || SENSOR_IMX249 == iSensor)
			{
				swpa_strcpy(strName, "PCC200B");//200w Global shutter
			}
			else if (SENSOR_ICX816 == iSensor)
			{
				swpa_strcpy(strName, "EC700");	//CCD 680W
			}
		    else
		    {
		        SW_TRACE_NORMAL("Err: got Unknown DevType: %d\n", iSensor);
				hr = E_FAIL;
		    }
		}
		else if (IsJupiter())
		{
			swpa_strcpy(strName, "SDC200");
		}
		else
		{
			//for others
		}
#endif
#ifdef DM6467
        swpa_strcpy(strName, "EC300");
#endif		
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetDevType(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnGetCpuTemperature(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = E_FAIL;
	INT* iTemp = (INT *)lParam;
	if(NULL != iTemp)
	{
		hr = swpa_info_get_cpu_temperature(iTemp) == 0 ? S_OK : E_FAIL;
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetCpuTemperature(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}

HRESULT CSWLPRApplication::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 0 )
	{
		return E_INVALIDARG;
	}

	CSWString strInfo = (CHAR*)wParam;
	if( strInfo.Length() > 0 )
	{
		if( strInfo.Find("H264采集") != -1)
		{
			m_strH264Status = strInfo;
		}
		else if( strInfo.Find("VPIF采集") != -1 )
		{
			m_strVPIFStatus = strInfo;
		}
		else if( strInfo.Find("识别") != -1 )
		{
			m_strRecogStatus = strInfo;
		}
		else if( strInfo.Find("结果链接") != -1)
		{
			m_strResultNetStatus = strInfo;
		}
		else if( strInfo.Find("JPEG链接") != -1)
		{
			m_strJpegNetStatus = strInfo;
		}
		else if( strInfo.Find("H264链接") != -1)
		{
			m_strH264NetStatus = strInfo;
		}
		else if( strInfo.Find("RTSP链接") != -1)
		{
			m_strRTSPConnStatus = strInfo;
		}
		else if( strInfo.Find("RTSP第二路链接") != -1)
		{
			m_strRTSPSecondConnStatus = strInfo;
		}
		else if( strInfo.Find("H264存储") != -1)
		{
			m_strH264HDDStatus = strInfo;
		}
		else if( strInfo.Find("结果存储") != -1)
		{
			m_strResultHDDStatus = strInfo;
		}
		else if( strInfo.Find("FTP状态") != -1)
		{
			m_strFTPStatus = strInfo;
		}
	}

	return S_OK;
}

HRESULT CSWLPRApplication::OnGetRunStatus(WPARAM wParam, LPARAM lParam)
{
	if( lParam == 0 )
	{
		return E_INVALIDARG;
	}

	CHAR* pszInfo = (CHAR*)lParam;
	if( !m_strRunStatus.IsEmpty() )
	{
		swpa_strcpy(pszInfo, (const CHAR*)m_strRunStatus);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CSWLPRApplication::OnRadarTrigger(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	if(!m_fDSPTrigger)
	{
		DWORD dwLane = wParam;
		DWORD iInfo = (DWORD)lParam;
		INT iSEQ = (iInfo & 0x00FFFFFF);
		INT iLane = 0;
		iLane |= (0x1 << (24 + dwLane));	
		SW_TRACE_DEBUG("RadarAlarm, Lane:%u, SEQ:%d\n", dwLane, 0);	
		PostMessage(MSG_SET_SOFT_TOUCH, iLane|iSEQ);
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnRadarTrigger(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
	return hr;
}


HRESULT CSWLPRApplication::OnCoilStatus(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
#if 0
	//线圈的状态
	m_strCoilStatus.Format("线圈1:%s 抓拍:%s;线圈2:%s 抓拍:%s;线圈3:%s 抓拍:%s;线圈4:%s 抓拍:%s"
		, (wParam & 0x01) ? "异常" : "正常", (lParam & 0x01) ? "触发" : "不触发"
		, (wParam & 0x02) ? "异常" : "正常", (lParam & 0x02) ? "触发" : "不触发"
		, (wParam & 0x04) ? "异常" : "正常", (lParam & 0x04) ? "触发" : "不触发"
		, (wParam & 0x08) ? "异常" : "正常", (lParam & 0x08) ? "触发" : "不触发"
	);
	
	//没有线圈触发
	if(lParam == 0)
	{
		m_fDSPTrigger = TRUE;
	}
	//有线圈触发且有其中的一个线圈触发损坏了，则视频触发
	else if((wParam & lParam))
	{
		m_fDSPTrigger = TRUE;
	}
	//线圈触发抓拍
	else
	{
		m_fDSPTrigger = FALSE;
	}
	
	SW_TRACE_DEBUG("CSWLPRApplication::OnCoilStatus(0x%08x, 0x%08x)[%s] return 0x%08x\n", wParam, lParam, (LPCSTR)m_strCoilStatus, hr);
#endif
	return hr;
}

HRESULT CSWLPRApplication::OnCoilTrigger(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
#if 0
	if(!m_fDSPTrigger)
	{
		INT iLane = wParam / 2;		
		if(!GetParam().Get().cResultSenderParam.iFlashDifferentLane)
		{
			iLane = 7;
		}
		else
		{
			iLane = (1 << iLane);
		}	
		SW_TRACE_DEBUG("CoilAlarm, Lane:%d, SEQ:%d\n", iLane, 0);	
		SendMessage(MSG_SET_SOFT_TOUCH, iLane << 24);
	}
	SW_TRACE_DEBUG("CSWLPRApplication::OnCoilTrigger(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
#endif
	return hr;
}

HRESULT CSWLPRApplication::OnGetBlackBoxMessage(WPARAM wParam, LPARAM lParam)
{
#define SINGLE_LOG_LEN 256

    if( lParam == 0 )
    {
        return E_INVALIDARG;
    }

    CHAR* pszMessage = (CHAR*)lParam;
    if (NULL == pszMessage)
    {
        SW_TRACE_DEBUG("Err: NULL == pszMessage\n");
        return E_INVALIDARG;
    }
    swpa_strcpy(pszMessage, "(NULL)\n");

    CSWFile cBlackBoxFile;
    
    if (FAILED(cBlackBoxFile.Open("EEPROM/0/BLACK_BOX", "r")))
    {
        SW_TRACE_DEBUG("Err: failed to open file\n");
        return E_FAIL;
    }

    //get and check header
    //arrHeader[0] - offset
    //arrHeader[1] - overwritten flag
    //arrHeader[2] - crc
    DWORD arrHeader[3] = {0};
    if (FAILED(cBlackBoxFile.Seek(0, SWPA_SEEK_SET)))
    {    
        SW_TRACE_DEBUG("failed to seek blackbox to SEEK_SET.\n");
        return E_FAIL;
    }
    if (FAILED(cBlackBoxFile.Read(arrHeader, sizeof(arrHeader))))
    {
        SW_TRACE_DEBUG("failed to read blackbox header.\n");
        return E_FAIL;
    }

    
    SW_TRACE_DEBUG("BlackBox arrHeader = [%d, %d, 0x%x]\n", arrHeader[0], arrHeader[1], arrHeader[2]);

    DWORD dwCrc = CSWUtils::CalcCrc32(0, (PBYTE)arrHeader, sizeof(arrHeader[0]) + sizeof(arrHeader[1]));
    if (dwCrc != arrHeader[2])
    {
        cBlackBoxFile.Close();
        return S_OK;
    }

    DWORD dwSize = 0;
    CHAR * pszBuf = NULL;
    if (arrHeader[1] > 0) /* overwritten */
    {
        if (FAILED(cBlackBoxFile.IOCtrl(SWPA_FILE_IOCTL_GET_MAX_SIZE, &dwSize)))
        {
            SW_TRACE_DEBUG("Err: failed to get file total size\n");
            return E_FAIL;
        }
    }
    else
    {
        dwSize = arrHeader[0];
    }
    
    dwSize -= SINGLE_LOG_LEN;

    //SW_TRACE_DEBUG("Info: BlackBox file size = %lu\n", dwSize);

    pszBuf = (CHAR*)swpa_mem_alloc(dwSize + 1);//the first SINGLE_LOG_LEN is reserved for header
    if (NULL == pszBuf)
    {
        SW_TRACE_DEBUG("Err: no memory for pszBuf\n");
        return E_OUTOFMEMORY;
    }        
    swpa_memset(pszBuf, 0, dwSize + 1);

    DWORD dwOffset = 0;
    if (arrHeader[1] > 0) /* overwritten */
    {
        if (FAILED(cBlackBoxFile.Seek(arrHeader[0], SWPA_SEEK_SET)))
        {    
            SW_TRACE_DEBUG("failed to seek blackbox to SEEK_SET + %d.\n", arrHeader[0]);
            SAFE_MEM_FREE(pszBuf);
            return E_FAIL;
        }

        DWORD dwReadLen = 0;
        if (FAILED(cBlackBoxFile.Read((VOID*)pszBuf, dwSize + SINGLE_LOG_LEN - arrHeader[0],  &dwReadLen))
            || dwReadLen != dwSize + SINGLE_LOG_LEN - arrHeader[0])
        {
            SW_TRACE_DEBUG("Err: failed to read blackbox file\n");
            SAFE_MEM_FREE(pszBuf);
            return E_FAIL;
        }

        dwOffset = dwReadLen;
    }
    
    if (FAILED(cBlackBoxFile.Seek(SINGLE_LOG_LEN, SWPA_SEEK_SET)))
    {    
        SW_TRACE_DEBUG("failed to seek blackbox to SEEK_SET+%d.\n", SINGLE_LOG_LEN);
        SAFE_MEM_FREE(pszBuf);
        return E_FAIL;
    }

    DWORD dwReadLen = 0;
    if (FAILED(cBlackBoxFile.Read((VOID*)(pszBuf+dwOffset), arrHeader[0]-SINGLE_LOG_LEN,  &dwReadLen))
        || dwReadLen != arrHeader[0]-SINGLE_LOG_LEN)
    {
        SW_TRACE_DEBUG("Err: failed to read blackbox file\n");
        SAFE_MEM_FREE(pszBuf);
        return E_FAIL;
    }

    BOOL fBlank = FALSE;
    char *szDst = pszMessage;
    char *szSrc = pszBuf;
    while(dwSize-- > 0)
    {
        *szDst = *szSrc++;
        if(*szDst == '\0' || *szDst == 0)
        {
            if(!fBlank)
            {
                fBlank = TRUE;
                *szDst = '\n';
                szDst++;    
            }            
        }
        else
        {
            fBlank = FALSE;
            szDst++;    
        }    
    }
    //printf("\n==================\n%s\n==================\n", pszMessage);
    
    SAFE_MEM_FREE(pszBuf);
    cBlackBoxFile.Close();    
    return S_OK;
}


HRESULT CSWLPRApplication::NtpTimeSync()
{
	//NTP时间同步
	if (GetParam().Get().cResultSenderParam.fEnableNtp)
	{
		char szCmd[255]={0};
		INT iSyncInterval = GetParam().Get().cResultSenderParam.iNtpSyncInterval;
		if (iSyncInterval < 300)
		{
			iSyncInterval = 300;
			SW_TRACE_DEBUG("Info: ntpdate interval must be longer then 300sec.\n");
		}

		if (0 != swpa_strcmp(GetParam().Get().cResultSenderParam.szNtpServerIP, ""))
		{
			swpa_sprintf(szCmd, "ntp_sync %d %s", iSyncInterval, GetParam().Get().cResultSenderParam.szNtpServerIP);
			swpa_utils_shell(szCmd, NULL);
		}
		else
		{
			SW_TRACE_NORMAL("Err: ntpdate server unknown.\n");
		}
	}
	else
	{
		swpa_utils_file_delete("/tmp/.running_ntp");
	}

	return S_OK;
}


HRESULT CSWLPRApplication::InitHdd(VOID)
{
	DWORD dwPartitionCount = 0;
	BOOL fForce = FALSE;

	CSWFile cPartInfoFile;
	if (SUCCEEDED(cPartInfoFile.Open("/tmp/HDDPartInfo.dat", "r"))
		&& SUCCEEDED(cPartInfoFile.Read(&dwPartitionCount, sizeof(dwPartitionCount)))
		)
	{
		SW_TRACE_DEBUG("Info: read from HDDPartInfo: %d\n", dwPartitionCount);
		fForce = TRUE;
	}
	else
	{
		dwPartitionCount += GetParam().Get().cResultSenderParam.fSaveVideo ? 1 : 0;
		dwPartitionCount += GetParam().Get().cResultSenderParam.fIsSafeSaver ? 1 : 0;
	}
	
	if (2 == GetParam().Get().cResultSenderParam.iDiskType // "2" means HDD
		|| fForce) 
	{	
		if (GetParam().Get().cResultSenderParam.fInitHdd)
		{
			SW_TRACE_DEBUG("Info: dwPartitionCount = %d\n", dwPartitionCount);
			if (0 == dwPartitionCount)
			{
				GetParam().UpdateEnum("\\HvDsp\\Misc", "InitHdd", 0);
				GetParam().Commit();
				GetParam().Get().cResultSenderParam.fInitHdd = 0;
			}
			else
			{
				CHAR szPartitionCount[8] = {0};
				swpa_sprintf(szPartitionCount, "%d", dwPartitionCount);
				while (SWPAR_OK != swpa_hdd_command(0, szPartitionCount))
				{
					SW_TRACE_NORMAL("Info: initializing Hdd...\n");
					CSWApplication::Sleep(500);
				}

				return S_OK;
			}
		}
		
		if (!m_fHardDiskReady
			&& (GetParam().Get().cResultSenderParam.fSaveVideo || GetParam().Get().cResultSenderParam.fIsSafeSaver))
		{
			/*while (SWPAR_OK != swpa_camera_init_reset_usb_disk())
			{
				SW_TRACE_NORMAL("Info: waiting HDD gets ready\n");
				swpa_thread_sleep_ms(1000);
			}
			*/
			m_fHardDiskReady = TRUE;
			
			SW_TRACE_NORMAL("Info: HDD is ready\n");
		}
	}
	
	return S_OK;
}


HRESULT CSWLPRApplication::DeinitHdd(VOID)
{
	if (m_fHardDiskReady)
	{
		if (SWPAR_OK != swpa_hdd_umount("/Result/0"))
		{
			SW_TRACE_DEBUG("Err: failed to umount /Result/0\n");
			return E_FAIL;
		}
		CSWApplication::Sleep(1000);
		if (SWPAR_OK != swpa_hdd_umount("/Result/1"))
		{
			SW_TRACE_DEBUG("Err: failed to umount /Result/1\n");
			return E_FAIL;
		}
		CSWApplication::Sleep(1000);
	}

	if (m_fNetDiskMounted)
	{
		if (SWPAR_OK != swpa_hdd_umount("/Result"))
		{
			SW_TRACE_DEBUG("Err: failed to umount /Result\n");
			return E_FAIL;
		}
		CSWApplication::Sleep(1000);
	}

	return S_OK;
}

//读取备份系统的版本号
HRESULT CSWLPRApplication::ReadBackupSystemVersion(CHAR* pszVersion)
{
	CHAR szFileName[64];
#define HV_BAKSYS_VERSION_PATH       "/mnt/nand/BakSysVersion"

	if (NULL == pszVersion)
	{
		SW_TRACE_NORMAL("Err: NULL == pszVersion\n");
		return E_INVALIDARG;
	}
	
	if (SWPAR_OK != swpa_utils_dir_exist(HV_BAKSYS_VERSION_PATH))
	{
		SW_TRACE_NORMAL("Err: No BakSys Version info...\n");
		return E_FAIL;
	}

	swpa_sprintf(szFileName,"%s/Version.dat", HV_BAKSYS_VERSION_PATH);

	CSWFile cVerFile;
	if (FAILED(cVerFile.Open(szFileName, "r")))
	{
		SW_TRACE_NORMAL("Err: open %s failed\n",szFileName);
		return E_FAIL;
	}

	CHAR szVersion[32] = {0};
	if (FAILED(cVerFile.Read(szVersion, sizeof(szVersion)-1)))
	{
		SW_TRACE_NORMAL("Err: Read backup sys version info failed\n");
		cVerFile.Close();
		return E_FAIL;
	}

	swpa_strcpy(pszVersion,"3.1.0.");
	swpa_strcat(pszVersion, szVersion);
	cVerFile.Close();

	SW_TRACE_NORMAL("Info: BakSys Version %s", pszVersion);

	return S_OK;
}


#define HV_SENSOR_TYPE_PATH       "/mnt/nand/SensorType"
HRESULT CSWLPRApplication::ReadSensorType(INT *piSensorType)
{
	CHAR szFileName[64];
	
	if (NULL == piSensorType)
	{
		SW_TRACE_NORMAL("Err: NULL == piSensorType\n");
		return E_INVALIDARG;
	}
	
	if (SWPAR_OK != swpa_utils_dir_exist(HV_SENSOR_TYPE_PATH))
	{
		SW_TRACE_NORMAL("Err: No Sensor Type info...\n");
		return E_FAIL;
	}
	swpa_sprintf(szFileName,"%s/SensorType.dat", HV_SENSOR_TYPE_PATH);
	
	CSWFile cVerFile;
	if (FAILED(cVerFile.Open(szFileName, "r")))
	{
		SW_TRACE_NORMAL("Err: open %s failed\n",szFileName);
		return E_FAIL;
	}
	
	CHAR szSensorType[32] = {0};
	if (FAILED(cVerFile.Read(szSensorType, sizeof(szSensorType)-1)))
	{
		SW_TRACE_NORMAL("Err: Read Sensor Type info failed\n");
		cVerFile.Close();
		return E_FAIL;
	}
	
	*piSensorType = swpa_atoi(szSensorType);

	cVerFile.Close();
	
	SW_TRACE_NORMAL("Info: Sensor Type %d", *piSensorType);

	return S_OK;
}

HRESULT CSWLPRApplication::SaveSensorType(INT iSensorType)
{
    CHAR szFileName[64];

    swpa_sprintf(szFileName,"%s/SensorType.dat", HV_SENSOR_TYPE_PATH);

	//文件存在则不再修改，因此要确保第一次保存的前端类型信息是正确的
	if (swpa_utils_file_exist(szFileName))
	{
		SW_TRACE_NORMAL("Sensor type file %s is exist!\n",szFileName);
		return S_OK;
	}

    //目录不存在，创建
    if (SWPAR_OK != swpa_utils_dir_exist(HV_SENSOR_TYPE_PATH))
    {
        if (SWPAR_OK != swpa_utils_dir_create(HV_SENSOR_TYPE_PATH)) 
        {
            SW_TRACE_DEBUG("Err: mkdir %s failed\n",HV_SENSOR_TYPE_PATH);
            return E_FAIL;
        }
    }

	CSWFile cVerFile;
	if (FAILED(cVerFile.Open(szFileName, "w")))
    {
        SW_TRACE_DEBUG("Err: open %s failed\n",szFileName);
        return E_FAIL;
    }

	CHAR szSensorType[32] = {0};
	swpa_snprintf(szSensorType, sizeof(szSensorType), "%d", iSensorType);
	SW_TRACE_NORMAL("Info: SensorType str = %s, strlen(str) = %d\n", szSensorType, swpa_strlen(szSensorType));
	if (FAILED(cVerFile.Write(szSensorType, sizeof(szSensorType))))
	{
		SW_TRACE_NORMAL("Err: Write SensorType info to %s failed\n",szFileName);
		cVerFile.Close();
        return E_FAIL;
	}

	cVerFile.Close();

	sync();

    SW_TRACE_NORMAL("Info: Save SensorType %s To File %s -- OK!", szSensorType, szFileName);

    return S_OK;	
}

HRESULT CSWLPRApplication::OnGetBackUpVersion(WPARAM wParam, LPARAM lParam)
{
    SW_TRACE_DEBUG("CSWLPRApplication::OnGetBackUpVersion(0x%08x, 0x%08x)", wParam, lParam);
    CHAR *szVersion = (CHAR *)lParam;
    
	if (FAILED(ReadBackupSystemVersion(szVersion)))
	{
		swpa_strcpy(szVersion, "Unknown");
	}
	
    return S_OK;
}


//获取net包版本号
HRESULT CSWLPRApplication::OnGetNetPackageVersion(WPARAM wParam, LPARAM lParam)
{
    SW_TRACE_DEBUG("CSWLPRApplication::OnGetNetPackageVersion(0x%08x, 0x%08x)", wParam, lParam);
    CHAR *szVersion = (CHAR *)lParam;
    
	swpa_strcpy(szVersion, "Unknown");

	if (swpa_utils_file_exist("/var/www/net/net_package_ver"))
	{
		CSWFile cFile;
		CHAR szVer[256] = {0};
		if (FAILED(cFile.Open("/var/www/net/net_package_ver", "r")))
		{
			SW_TRACE_DEBUG("Err: failed to read /var/www/net/net_package_ver\n");
			return E_FAIL;
		}
		else
		{
			cFile.Read(szVer, sizeof(szVer)-1);
		}

		swpa_strcpy(szVersion, szVer);
		return S_OK;
	}
	else
	{
		SW_TRACE_DEBUG("Err: no /var/www/net/net_package_ver file found\n");
		return E_FAIL;
	}
	
    return S_OK;
}


//获取固件包版本号
HRESULT CSWLPRApplication::OnGetFirmwareVersion(WPARAM wParam, LPARAM lParam)
{
    SW_TRACE_DEBUG("CSWLPRApplication::OnGetFirmwareVersion(0x%08x, 0x%08x)", wParam, lParam);
    CHAR *szVersion = (CHAR *)lParam;

	if (swpa_utils_file_exist("/firmware_ver"))
	{
		CSWFile cFile;
		CHAR szVer[256] = {0};
		if (FAILED(cFile.Open("/firmware_ver", "r")))
		{
			SW_TRACE_DEBUG("Err: failed to read /firmware_ver\n");
			swpa_strcpy(szVersion, "Unknown");
		}
		else
		{
			cFile.Read(szVer, sizeof(szVer)-1);
		}

		swpa_strncpy(szVersion, szVer, swpa_strlen(szVer)-1);
	}
	else
	{
		SW_TRACE_DEBUG("Err: no /firmware_ver file found\n");
		swpa_strcpy(szVersion, "Unknown");
	}

	swpa_strcat(szVersion, "; mcfw:");
	
	CHAR szMCFWVersionInfo[512] = {0};
	if (SWPAR_FAIL == swpa_ipnc_control(0, CMD_GET_VERSION, (void*)szMCFWVersionInfo , sizeof(szMCFWVersionInfo), 0))
	{
		SW_TRACE_NORMAL("Err: failed to get mcfw version info.\n");
		swpa_sprintf(szMCFWVersionInfo, "Unknown\n");
	}
	else
	{
		DWORD dwMcfwA8 = 0, dwMcfwVpss = 0, dwMcfwVideo = 0, dwMcfwDsp = 0;
		DWORD dwDomeVersion = 0;
		CHAR chCamInfo[16] = {0};
		BOOL fGetDomeVersion = FALSE;

		swpa_sscanf(szMCFWVersionInfo 
			, "[Linux  ]mcfw:r%d\n[m3vpss ]mcfw:r%d\n[m3video]mcfw:r%d\n[c6xdsp ]mcfw:r%d\n"
			, &dwMcfwA8, &dwMcfwVpss, &dwMcfwVideo, &dwMcfwDsp );

		//木星获取球机的固件版本
		if(GetParam().Get().nWorkModeIndex == PRM_DOME_CAMERA)
		{
			if(S_OK == (CSWMessage::SendMessage(MSG_GET_DOME_VERSION ,0, (LPARAM)&dwDomeVersion)) && S_OK == CSWMessage::SendMessage(MSG_GET_CAMERA_VERSION, 0, (LPARAM)chCamInfo))
			{
				fGetDomeVersion = TRUE;
			}

		}
		if(fGetDomeVersion)
		{
			swpa_sprintf(szMCFWVersionInfo
						 , "A8:r%d VPSS:r%d VIDEO:r%d DSP:r%d Dome:r%d Cam:r%04x"
						 , dwMcfwA8, dwMcfwVpss, dwMcfwVideo, dwMcfwDsp, 
						 dwDomeVersion, (chCamInfo[4] << 8) | chCamInfo[5]);
		}
		else
		{
			swpa_sprintf(szMCFWVersionInfo
						 , "A8:r%d VPSS:r%d VIDEO:r%d DSP:r%d"
						 , dwMcfwA8, dwMcfwVpss, dwMcfwVideo, dwMcfwDsp);


		}
	}

	
	swpa_strcat(szVersion, szMCFWVersionInfo);
	swpa_strcat(szVersion, "; 2a:");
	CHAR sz2AAppVersionInfo[64];
	if (S_OK != SendMessage(MSG_2A_GET_VERSION, sizeof(sz2AAppVersionInfo), (LPARAM)sz2AAppVersionInfo))
	{
	
		SW_TRACE_NORMAL("Err: failed to get 2AApp version info.\n");
		swpa_sprintf(sz2AAppVersionInfo, "Unknown\n");
	}
	
	swpa_strcat(szVersion, sz2AAppVersionInfo);
	return S_OK;
}



#define GET_VERSION(TAG_STR)\
{\
	CHAR *szVersion = (CHAR *)lParam;\
	swpa_strcpy(szVersion, "unknown");\
	\
	CHAR szVerInfo[512] = {0};\
	INT iLen = sizeof(szVerInfo);\
	if (SWPAR_OK == swpa_info_get_firmware_version(szVerInfo, &iLen))\
	{\
		SW_TRACE_DEBUG("Info: szVerInfo = %s\n", szVerInfo);\
		CHAR * szSub = swpa_strstr(szVerInfo, TAG_STR);\
		if (NULL != szSub)\
		{\
			CHAR * szEnd = swpa_strstr(szSub, ";");\
			CHAR szVer[256] = {0};\
			swpa_strncpy(szVer, szSub+swpa_strlen(TAG_STR)+1, (DWORD)szEnd - (DWORD)szSub  - swpa_strlen(TAG_STR) - 1);\
			SW_TRACE_DEBUG("Info: Ver = %s\n", szVer);\
			swpa_strcpy(szVersion, szVer);\
		}\
	}\
	else\
	{\
		SW_TRACE_DEBUG("Err: failed to get firmware version info\n");\
	}\
}

HRESULT CSWLPRApplication::OnGetFPGAVersion(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetFPGAVersion(0x%08x, 0x%08x)", wParam, lParam);
	GET_VERSION("camera_fpga");	
	
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetKernelVersion(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetKernelVersion(0x%08x, 0x%08x)", wParam, lParam);
	GET_VERSION("kernel");	
	
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetUBootVersion(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetUBootVersion(0x%08x, 0x%08x)", wParam, lParam);

#ifdef DM8127
	GET_VERSION("uboot");
#endif

#ifdef DM6467
	GET_VERSION("uboot_dm6467");
#endif
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetUBLVersion(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetUBLVersion(0x%08x, 0x%08x)", wParam, lParam);
#ifdef DM8127
	CHAR *szVersion = (CHAR *)lParam;
	swpa_strcpy(szVersion, "Not supported");
#endif

#ifdef DM6467
	GET_VERSION("ubl_dm6467");
#endif
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetSN(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetSN(0x%08x, 0x%08x)", wParam, lParam);
    CHAR *szSN = (CHAR *)lParam;
    swpa_strcpy(szSN, "unknown");
    UINT dwLen = 128;
    INT iRet = swpa_device_read_sn(szSN, &dwLen);
    if (SWPAR_OK != iRet)
    {
        SW_TRACE_NORMAL("Err: failed to get device serial number\n");
        return E_FAIL;
    }
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetCPU(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetCPU(0x%08x, 0x%08x)", wParam, lParam);
	INT* iCPU = (INT *)lParam;
	if (NULL == iCPU)
	{
		return E_INVALIDARG;
	}
	*iCPU = swpa_utils_get_cpu_usage();
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetRAM(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetRAM(0x%08x, 0x%08x)", wParam, lParam);
	INT* iMemory = (INT *)lParam;
	if (NULL == iMemory)
	{
		return E_INVALIDARG;
	}
	*iMemory = swpa_utils_get_mem_usage();
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetLogLevel(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetLogLevel(0x%08x, 0x%08x)", wParam, lParam);
	INT* iLevel = (INT *)lParam;
	*iLevel = CSWLog::GetLevel();
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetLogLevel(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetLogLevel(0x%08x, 0x%08x)", wParam, lParam);
	CSWLog::SetLevel((INT)wParam);
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetNTPServerIP(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetNTPServerIP(0x%08x, 0x%08x)", wParam, lParam);
	CHAR *szIP = (CHAR *)lParam;
	swpa_strcpy(szIP, GetParam().Get().cResultSenderParam.szNtpServerIP);
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetNTPServerIP(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetNTPServerIP(0x%08x, 0x%08x)", wParam, lParam);
	CHAR *szIP = (CHAR *)wParam;
	swpa_strcpy(GetParam().Get().cResultSenderParam.szNtpServerIP, szIP);

    NtpTimeSync();
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetNTPEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetNTPEnable(0x%08x, 0x%08x)", wParam, lParam);
	INT* iEnable = (INT *)lParam;
	*iEnable = GetParam().Get().cResultSenderParam.fEnableNtp;
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetNTPEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetNTPEnable(0x%08x, 0x%08x)", wParam, lParam);
	GetParam().Get().cResultSenderParam.fEnableNtp = (INT)wParam;

	NtpTimeSync();
	
	return S_OK;
}
HRESULT CSWLPRApplication::OnGetNTPTimeZone(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetNTPTimeZone(0x%08x, 0x%08x)", wParam, lParam);
	DWORD *pdwTimeZone = (DWORD *)lParam;
	if (NULL == pdwTimeZone)
	{
		return E_INVALIDARG;
	}

	*pdwTimeZone =  CSWDateTime::GetTimeZone();
	
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetNTPTimeZone(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetNTPTimeZone(0x%08x, 0x%08x)", wParam, lParam);
	DWORD dwTimeZone = (DWORD)wParam;

	return CSWDateTime::SetTimeZone(dwTimeZone);
}

HRESULT CSWLPRApplication::OnGetNTPInterval(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetNTPInterval(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = GetParam().Get().cResultSenderParam.iNtpSyncInterval;
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetNTPInterval(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetNTPInterval(0x%08x, 0x%08x)", wParam, lParam);
    GetParam().Get().cResultSenderParam.iNtpSyncInterval = (INT)wParam;

	NtpTimeSync();
	
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetHDDCheckReport(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetHDDCheckReport(0x%08x, 0x%08x)", wParam, lParam);

    CHAR *szReport = (CHAR *)lParam;
    swpa_strcpy(szReport, "unknown");
#ifdef DM8127
    swpa_strcpy(szReport, "正常");
#endif
    return S_OK;
}


#define HV_CUSTOMIZED_INFO_PATH       "/mnt/nand/CustomizedInfo"
//保存设备别名
HRESULT CSWLPRApplication::SaveCustomizedDevName(LPCSTR szDevName)
{
    CHAR szFileName[64];

	if (NULL == szDevName)
	{
		SW_TRACE_DEBUG("Err: szDevName(%p) is invalid\n", szDevName);
        return E_INVALIDARG;
	}

    swpa_sprintf(szFileName,"%s/DevName.txt", HV_CUSTOMIZED_INFO_PATH);

    //目录不存在，创建
    if (SWPAR_OK != swpa_utils_dir_exist(HV_CUSTOMIZED_INFO_PATH))
    {
        if (SWPAR_OK != swpa_utils_dir_create(HV_CUSTOMIZED_INFO_PATH)) 
        {
            SW_TRACE_DEBUG("Err: mkdir %s failed\n",HV_CUSTOMIZED_INFO_PATH);
            return E_FAIL;
        }
    }

	CSWFile cFile;
	if (FAILED(cFile.Open(szFileName, "w")))
    {
        SW_TRACE_DEBUG("Err: open %s failed\n",szFileName);
        return E_FAIL;
    }

	CHAR szName[64] = {0};
	swpa_snprintf(szName, sizeof(szName), "%s", szDevName);
	SW_TRACE_NORMAL("Info: DevName str = %s, len = %d\n", szName, swpa_strlen(szName));
	if (FAILED(cFile.Write(szName, sizeof(szName))))
	{
		SW_TRACE_NORMAL("Err: Write DevName to %s failed\n",szFileName);
		cFile.Close();
        return E_FAIL;
	}

	cFile.Close();

    SW_TRACE_NORMAL("Info: Save DevName %s To File %s -- OK!", szName, szFileName);

    return S_OK;	
}



//读取设备别名
HRESULT CSWLPRApplication::ReadCustomizedDevName(LPSTR pszName)
{
	CHAR szFileName[64];

	if (NULL == pszName)
	{
		SW_TRACE_NORMAL("Err: NULL == pszName\n");
		return E_INVALIDARG;
	}
	
	if (SWPAR_OK != swpa_utils_dir_exist(HV_CUSTOMIZED_INFO_PATH))
	{
		SW_TRACE_NORMAL("Err: No customized devname info...\n");
		return E_NOTIMPL;
	}

	swpa_sprintf(szFileName,"%s/DevName.txt", HV_CUSTOMIZED_INFO_PATH);

	CSWFile cFile;
	if (FAILED(cFile.Open(szFileName, "r")))
	{
		SW_TRACE_NORMAL("Err: open %s failed\n",szFileName);
		return E_NOTIMPL;
	}

	CHAR szDevName[64] = {0};
	if (FAILED(cFile.Read(szDevName, sizeof(szDevName))))
	{
		SW_TRACE_NORMAL("Err: Read customized devname info failed\n");
		cFile.Close();
		return E_FAIL;
	}

	cFile.Close();
	
	swpa_strcpy(pszName, szDevName);

	SW_TRACE_NORMAL("Info: customized dev name: %s", pszName);

	return S_OK;
}


HRESULT CSWLPRApplication::OnSetCustomizedDevName(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetCustomizedDevName(0x%08x, 0x%08x)", wParam, lParam);

    CHAR *szName = (CHAR *)wParam;
	if (NULL == szName 
		|| 64 <= swpa_strlen(szName))
	{
		SW_TRACE_NORMAL("Err: invalid arg %s\n", NULL == szName ? "NULL pointer" : szName);
		return E_INVALIDARG;
	}

	if (FAILED(SaveCustomizedDevName(szName)))
	{
		SW_TRACE_NORMAL("Err: failed to set customized dev name to %s\n", szName);
		return E_FAIL;
	}
	CSWMessage::SendMessage(MSG_OSD_SET_JPEG_DEV_ID,(WPARAM)szName,0);	//将修改动态设置到字符叠加模块
	CSWMessage::SendMessage(MSG_OSD_SET_H264_DEV_ID,(WPARAM)szName,0);
    return S_OK;
}


HRESULT CSWLPRApplication::OnGetCustomizedDevName(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetCustomizedDevName(0x%08x, 0x%08x)", wParam, lParam);

    CHAR *szName = (CHAR *)lParam;
	if (NULL == szName)
	{
		SW_TRACE_NORMAL("Err: invalid arg %s\n", NULL == szName ? "NULL pointer" : szName);
		return E_INVALIDARG;
	}

	HRESULT hr = ReadCustomizedDevName(szName);
	if (E_NOTIMPL == hr)
	{
		swpa_strcpy(szName, "Unnamed");
		SaveCustomizedDevName("Unnamed");
	}
	else if (FAILED(hr)) //other errors
	{
		SW_TRACE_NORMAL("Err: failed to get customized dev name\n");
		swpa_strcpy(szName, "Get Fail");
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CSWLPRApplication::OnSetAWBWorkMode(WPARAM wParam, LPARAM lParam)
{
	INT iValue = (INT)wParam;
    if (iValue < 0 || iValue > 2)
	{
		return E_FAIL;
	}
	GetParam().Get().cCamAppParam.iAWBWorkMode = iValue;
	CSWMessage::SendMessage(MSG_SET_AWB_MODE, iValue, 0);
	GetParam().UpdateEnum("\\CamApp", "AWBWorkMode", iValue);
	GetParam().Commit();
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetAWBWorkMode(WPARAM wParam, LPARAM lParam)
{
	INT* piValue = (INT*)lParam;
	if (piValue == NULL)
	{
		return E_INVALIDARG;
	}
	*piValue = GetParam().Get().cCamAppParam.iAWBWorkMode;
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetAutoControlCammeraAll(WPARAM wParam, LPARAM lParam)
{
	INT iValue = (INT)wParam;
	if (iValue < 0 || iValue > 1)
	{
		return E_FAIL;
	}
	GetParam().Get().cCamAppParam.iAutoControlCammeraAll = iValue;
	GetParam().UpdateEnum("\\CamApp", "AutoControlAll", iValue);
	GetParam().Commit();
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetAutoControlCammeraAll(WPARAM wParam, LPARAM lParam)
{	
  	INT* piValue = (INT*)lParam;
  	if (piValue == NULL)
	{
		return E_INVALIDARG;
	}
  	*piValue = GetParam().Get().cCamAppParam.iAutoControlCammeraAll;
	return S_OK;
}

HRESULT CSWLPRApplication::OnSetH264Resolution(WPARAM wParam, LPARAM lParam)
{
	INT iValue = (INT)wParam;
	if (iValue < 0 || iValue > 2)
	{
		return E_FAIL;
	}
	GetParam().Get().cCamAppParam.iResolution = iValue;
	GetParam().UpdateEnum("\\CamApp", "Resolution", iValue);
	GetParam().Commit();
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetH264Resolution(WPARAM wParam, LPARAM lParam)
{
	INT* piValue = (INT*)lParam;	
	if (piValue == NULL)
	{
		return E_INVALIDARG;
	}
  	*piValue = GetParam().Get().cCamAppParam.iResolution;
	return S_OK;
}

HRESULT CSWLPRApplication::OnGetGB28181Param(PVOID pvBuffer, INT iSize)
{
	if (NULL == m_pParam)
	{
		return E_FAIL;
	}
    if(!GetParam().Get().cGb28181.fEnalbe)
    {
		SW_TRACE_DEBUG("GB28181 is not enabled!\n");
		return S_FALSE;
    }
	
    HRESULT hd = S_OK;
	GB28181_info info;
    swpa_memset(&info,0,sizeof(info));
    info.enable = GetParam().Get().cGb28181.fEnalbe;
  
    swpa_strcpy(info.server_id,    GetParam().Get().cGb28181.szServerID);
    if(swpa_strlen(GetParam().Get().cGb28181.szServerRegion))
        swpa_strcpy(info.server_region,GetParam().Get().cGb28181.szServerRegion );
    else
         swpa_strncpy(info.server_region,info.server_id,10);
    swpa_strcpy(info.server_ip,    GetParam().Get().cGb28181.szServerIP);
    swpa_sprintf(info.server_port,"%d",GetParam().Get().cGb28181.iServerPort);
    swpa_strcpy(info.ipc_id,      GetParam().Get().cGb28181.szIPC_ID);
    if(swpa_strlen(GetParam().Get().cGb28181.szIPC_Region))
        swpa_strcpy(info.ipc_region,GetParam().Get().cGb28181.szIPC_Region );
    else
         swpa_strncpy(info.ipc_region,info.ipc_id,10);

    swpa_strcpy(info.ipc_username,"00000000002");
    swpa_strcpy(info.ipc_pwd,GetParam().Get().cGb28181.szPassWord);
    swpa_strcpy(info.ipc_alarm,    GetParam().Get().cGb28181.szAlarmID);
    swpa_sprintf(info.ipc_port,"%d",GetParam().Get().cGb28181.iIPC_Port);
    swpa_sprintf(info.mediaSend_port,"%d",GetParam().Get().cGb28181.iIPC_VideoPort);
    info.mediaSend_FPS = 25;
    info.mediaSend_wight = 1920;
    info.mediaSend_height = 1080;
    OnGetSN(NULL,(LPARAM)info.szDeviceName);//设备型号
    swpa_strcpy(info.szManufacturer,  GetParam().Get().cGb28181.szManufacturer);
    OnGetDevType(NULL,(LPARAM)info.szModel);
    OnGetVersion(NULL,(LPARAM)info.szFirmware);//设备固件版本
    swpa_strcpy(info.device_encode,"ON");
    swpa_strcpy(info.device_record,"OFF");
    swpa_strcpy(info.szBlock,    GetParam().Get().cGb28181.szBlock);
    swpa_strcpy(info.szAddress,    GetParam().Get().cGb28181.szAddress);
    swpa_strcpy(info.szOwner,    GetParam().Get().cGb28181.szOwner);
    swpa_strcpy(info.szCivilCode,    GetParam().Get().cGb28181.szCivilCode);
    swpa_strcpy(info.szLongitude,  GetParam().Get().cGb28181.szLongitude);
    swpa_strcpy(info.szLatitude,    GetParam().Get().cGb28181.szLatitude);
    swpa_strcpy(info.device_guard,"OFFDUTY");


    SW_TRACE_NORMAL("server_id = %s\n",info.server_id);
    SW_TRACE_NORMAL("server_region = %s\n",info.server_region);
    SW_TRACE_NORMAL("server_ip = %s\n",info.server_ip);
    SW_TRACE_NORMAL("ipc_id = %s\n",info.ipc_id);
    SW_TRACE_NORMAL("ipc_region = %s\n",info.ipc_region);
    SW_TRACE_NORMAL("ipc_username = %s\n",info.ipc_username);
    SW_TRACE_NORMAL("ipc_pwd = %s\n",info.ipc_pwd);
    SW_TRACE_NORMAL("ipc_alarm = %s\n",info.ipc_alarm);
    SW_TRACE_NORMAL("ipc_port = %s\n",info.ipc_port);
    SW_TRACE_NORMAL("mediaSend_port = %s\n",info.mediaSend_port);
    SW_TRACE_NORMAL("szManufacturer = %s\n",info.szManufacturer);
    SW_TRACE_NORMAL("szModel = %s\n",info.szModel);
    SW_TRACE_NORMAL("szFirmware = %s\n",info.szFirmware);
    SW_TRACE_NORMAL("device_encode = %s\n",info.device_encode);
    SW_TRACE_NORMAL("device_record = %s\n",info.device_record);
    SW_TRACE_NORMAL("szOwner = %s\n",info.szOwner);
    SW_TRACE_NORMAL("szCivilCode = %s\n",info.szCivilCode);
    SW_TRACE_NORMAL("szBlock = %s\n",info.szBlock);
    SW_TRACE_NORMAL("szAddress = %s\n",info.szAddress);
    SW_TRACE_NORMAL("szLongitude = %s\n",info.szLongitude);
    SW_TRACE_NORMAL("szLatitude = %s\n",info.szLatitude);
    SW_TRACE_NORMAL("device_guard = %s\n",info.device_guard);


    hd = SendRemoteMessage(MSG_GB28181_PARAMTER_CHANGE_ALARM, &info, sizeof(info));
	SW_TRACE_DEBUG("SendRemoteMessage(MSG_GB28181_PARAMTER_CHANGE_ALARM, %x08x, %d) -- %s\n",
		&info, sizeof(info), (hd == S_OK) ? "OK" : "FAILED");

    return hd;

}

HRESULT CSWLPRApplication::OnReBoot(PVOID pvBuffer, INT iSize)
{
    SW_TRACE_DEBUG("CSWLPRApplication::OnReBoot\n");
    return OnResetDevice((WPARAM)2,0);
}

HRESULT CSWLPRApplication::OnSendRtspParam(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnSendRtspParam \n");
	RTSP_PARAM_INFO pRtspParamInfo;
	HRESULT hr = S_OK;

	if(m_pParam == NULL)
		return S_FALSE;

	swpa_memset(&pRtspParamInfo, 0, sizeof(RTSP_PARAM_INFO));
	swpa_memcpy(&pRtspParamInfo, &GetParam().Get().cRtspParam, sizeof(RTSP_PARAM_INFO));

	hr = SendRemoteMessage(MSG_RTSP_SEND_PARAM, &pRtspParamInfo, sizeof(RTSP_PARAM_INFO));
	if(hr != S_OK)
	{
		SW_TRACE_DEBUG(" SendRemoteMessage(MSG_GET_RTSP_PARAM error !");
	}

	return hr;
}





HRESULT CSWLPRApplication::OnGetOnvifParam(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("CSWLPRApplication::OnGetOnvifParam \n");

	if(m_pParam == NULL)
	{
		SW_TRACE_DEBUG("CSWLPRApplication::OnGetOnvifParam m_pParam is NULL !!! \n");
		return S_FALSE;
	}
		
	//ONVIF 参数相关
	
	ONVIF_PARAM_INFO onvifparam;
	onvifparam.iOnvifEnable = GetParam().Get().cOnvifParam.iOnvifEnable;;
	onvifparam.iAuthenticateEnable = GetParam().Get().cOnvifParam.iAuthenticateEnable;
	

	HRESULT hr = S_OK;
	hr = SendRemoteMessage(MSG_ONVIF_SET_PARAM, &onvifparam, sizeof(ONVIF_PARAM_INFO));
	if(hr != S_OK)
	{
		SW_TRACE_DEBUG(" SendRemoteMessage(MSG_ONVIF_SET_PARAM error !");
	}

	return hr;
}

HRESULT CSWLPRApplication::OnSetDenoiseStatus(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;
	PDWORD pdwDenoiseStatus = (PDWORD)pvBuffer;
	SW_TRACE_DEBUG("CSWLPRApplication::OnSetDenoiseStatus [%d] [%d]\n",
		pdwDenoiseStatus[0],pdwDenoiseStatus[1]);
	
	DWORD dwTnfEnable = pdwDenoiseStatus[0];
	DWORD dwSnfEnable = pdwDenoiseStatus[1];

	HRESULT hr = SendMessage(MSG_SET_TNF_STATE, dwTnfEnable, 0);
	if (hr != S_OK)
	{
		return hr;
	}
	hr = SendMessage(MSG_SET_SNF_STATE, dwSnfEnable, 0);

	return hr;
}

BOOL CSWLPRApplication::IsVenus(VOID)
{
	return (DEVTYPE_VENUS == m_dwDevType);
}

BOOL CSWLPRApplication::IsJupiter(VOID)
{
	return (DEVTYPE_JUPITER == m_dwDevType);
}


