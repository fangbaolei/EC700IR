// 该文件编码必须是WINDOWS-936格式
#include "DataCtrl.h"
#include "HvCameraLinkOpt.h"
#include "tinyxml.h"
#include "SafeSaverImpl_CF.h"
#include "hvtarget_ARM.h"

const int APEND_INFO_SIZE   = 32 * 1024;            //附加信息长度
const int DEV_MAX_DIFF_TIME = 3 * 60 * 1000;        //3分钟

const DWORD32 MAX_INFO_SIZE = 64 * 1024;
const DWORD32 MAX_DATA_SIZE = 1.5 * 1024 * 1024;

// 扩展信息头
#ifndef BLOCK_HEADER
typedef struct tag_block
{
	DWORD32 dwID;
	DWORD32 dwLen;
}
BLOCK_HEADER;
#endif


#ifdef PC_TEST
void TelnetOutputDataString(int nLevel, char* fmt, ...)
{
}
void SetHddLedOn()
{
}
void SetHddLedOff()
{
}
#endif

HRESULT ForceSend(DWORD32 dwVideoID)
{
    return S_OK;
}

static ISafeSaver* g_pcSafeSaver = NULL;

static void FormatTime(char* szFileName, DWORD32 dwTimeLow, DWORD32 dwTimeHigh)
{
	if (NULL == szFileName)
	{
		return;
	}
	REAL_TIME_STRUCT rtFileTime;
	ConvertMsToTime(dwTimeLow, dwTimeHigh, &rtFileTime);
	DWORD32 dwMSecond = dwTimeLow%1000;

	sprintf(szFileName
		,"%04d%02d%02d%02d%02d%02d%03d"
		,rtFileTime.wYear
		,rtFileTime.wMonth
		,rtFileTime.wDay
		,rtFileTime.wHour
		,rtFileTime.wMinute
		,rtFileTime.wSecond
		,dwMSecond
		);
}

static HRESULT RecordCallBack(PVOID pUserData                          //用户上下文
                              ,const char* szDev                        //设备标识(编号或IP)
                              ,DWORD32 dwTimeLow
                              ,DWORD32 dwTimeHigh
                              ,const CAMERA_INFO_HEADER* pInfoHeader
                              ,const unsigned char* pbInfo
                              ,const unsigned char* pbData
                              )
{
    if (g_pcSafeSaver)
    {
        int iIndex = 0;
        g_pcSafeSaver->SavePlateRecord(dwTimeLow, dwTimeHigh, &iIndex, pInfoHeader, pbInfo, pbData, szDev);
    }
    return 0;
}

static HRESULT GetXmlValue(const char* szAppendInfo, const char* szName, char* szBuf, int iBufLen, const int fAttr)
{
	if ( NULL == szAppendInfo || NULL == szBuf || NULL == szName )
	{
		return E_FAIL;
	}
	szBuf[0] = 0;

	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szAppendInfo) )
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement )
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if ( pElementResultSet )
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if ( pElementResult )
				{
					const TiXmlElement* pElementFoundName = pElementResult->FirstChildElement(szName);
					if ( pElementFoundName )
					{
					    if (fAttr)
					    {
					        const TiXmlAttribute* attr = pElementFoundName->FirstAttribute();
					        for (; attr; attr=attr->Next())
                            {
                                if (0 == strcmp(attr->Name(), "value"))
                                {
                                    if ( iBufLen > (int)strlen(attr->Value()) )
                                    {
                                        strcpy(szBuf, attr->Value());
							            return S_OK;
                                    }
                                }
                            }
					    }
						else if( iBufLen > (int)strlen(pElementFoundName->GetText()) )
						{
							strcpy(szBuf, pElementFoundName->GetText());
							return S_OK;
						}
					}
				}
			}
		}
	}
	//cXmlDoc.SaveFile("/home/linux/mnt/2.xml");
	return E_FAIL;
}

static HRESULT ModifyXmlValue(const char* szDev, const DWORD32 dwCarID, char* szAppendInfo, const int iMaxInfoLen)
{
	if ( NULL == szAppendInfo || NULL == szDev )
	{
		return E_FAIL;
	}

	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szAppendInfo) )
	{
        TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement )
		{
			TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if ( pElementResultSet )
			{
				TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if ( pElementResult )
				{
				    TiXmlElement* pValue = new TiXmlElement("CoverCtrlDev");
                    TiXmlText* pText = new TiXmlText(szDev);
                    if (pValue && pText)
                    {
                        pValue->LinkEndChild(pText);
                        pElementResult->LinkEndChild(pValue);
                    }

                    TiXmlElement* pElementCarIDName = pElementResult->FirstChildElement("CarID");
					if ( pElementCarIDName )
					{
				        TiXmlAttribute* attr = pElementCarIDName->FirstAttribute();
				        for (; attr; attr=attr->Next())
                        {
                            if (0 == strcmp(attr->Name(), "value"))
                            {
                                char szCarID[8] = {0};
                                sprintf(szCarID, "%u", dwCarID);
                                attr->SetValue(szCarID);
                                break;
                            }
                        }
					}
				}
			}
		}

		TiXmlPrinter cTxPr;
        //cTxPr.SetStreamPrinting();
        cXmlDoc.Accept(&cTxPr);
        strncpy(szAppendInfo, cTxPr.CStr(), iMaxInfoLen - 1);

        return S_OK;
	}

	return E_FAIL;
}

//连接socket
static bool ConnectSocket(const char* szIp, DWORD wPort, HV_SOCKET_HANDLE& hSocket)
{
    if ( S_OK == HvConnect(hSocket, szIp, wPort, 4000))
    {
        HvSetRecvTimeOut(hSocket, 6000);
        HvSetSendTimeOut(hSocket, 4000);
        return true;
    }

    return false;
}

//关闭socket
static void ForceCloseSocket(HV_SOCKET_HANDLE &hSocket)
{
	if ( hSocket == INVALID_SOCKET )
	{
		return;
	}

    CloseSocket(hSocket);
    hSocket = INVALID_SOCKET;
}

SResultInfo::SResultInfo()
{
    strDev = "";
    strPlateNO = "";
    dwTime = 0;
}

SResultInfo::SResultInfo(const SResultInfo& rs)
{
	strDev = rs.strDev;
	strPlateNO = rs.strPlateNO;
	dwTime = rs.dwTime;
}

SResultInfo& SResultInfo::operator=(const SResultInfo& rs)
{
	strDev = rs.strDev;
	strPlateNO = rs.strPlateNO;
	dwTime = rs.dwTime;

	return *this;
}

//接收识别结果的线程
HRESULT CRecvRecordThread::Run(void *pvParamter)
{
    CHvBox* pHvBox = (CHvBox*)pvParamter;

    unsigned char* pbInfo   = new unsigned char[MAX_INFO_SIZE];
    unsigned char* pbData   = new unsigned char[MAX_DATA_SIZE];
    char szAppendInfo[APEND_INFO_SIZE] = {0}; // 32KB

    CAMERA_INFO_HEADER cInfoHeader;
    BLOCK_HEADER cBlockHeader;
    while ( !m_fExit )
    {
        if ( CONN_STATUS_NORMAL == pHvBox->m_dwRecordConnStatus )
		{
			if ( GetSystemTick() - pHvBox->m_dwRecvRecordThreadLastTick > 6000 )
			{
			    HV_Trace(5, "<BoxInfo>Record connection of %s time out, try to reconnect ...\n", pHvBox->m_szIP);
				pHvBox->m_dwRecordConnStatus = CONN_STATUS_RECONN;
			}
		}
		else if ( CONN_STATUS_RECONN == pHvBox->m_dwRecordConnStatus || CONN_STATUS_DISCONN == pHvBox->m_dwRecordConnStatus )
		{
			HV_Sleep(1000);
			continue;
		}
        memset(&cInfoHeader, 0 ,sizeof(cInfoHeader));

        if ( sizeof(cInfoHeader) == RecvAll(pHvBox->m_sktRecord, (char*)&cInfoHeader, sizeof(cInfoHeader)) )
	    {
			pHvBox->m_dwRecvRecordThreadLastTick = GetSystemTick();

			if ( cInfoHeader.dwType == CAMERA_THROB )  // 心跳包
			{
				continue;
			}

			if (cInfoHeader.dwInfoSize > MAX_INFO_SIZE || cInfoHeader.dwDataSize > MAX_DATA_SIZE)
			{
			    HV_Trace(3, "InfoSize or DataSize is too big, reconnect...");
			    pHvBox->m_dwRecordConnStatus = CONN_STATUS_RECONN;
			    break;
			}

			if( cInfoHeader.dwInfoSize > 0 )
			{
				if ( cInfoHeader.dwInfoSize != RecvAll(pHvBox->m_sktRecord, (char*)pbInfo, cInfoHeader.dwInfoSize) )
				{
					HV_Sleep(1000);
					continue;
				}
			}

			if( cInfoHeader.dwDataSize > 0 )
			{
				if ( cInfoHeader.dwDataSize != RecvAll(pHvBox->m_sktRecord, (char*)pbData, cInfoHeader.dwDataSize) )
				{
					HV_Sleep(1000);
					continue;
				}

				if( cInfoHeader.dwType == CAMERA_RECORD ) //识别结果流
				{
					// 数据流附加信息解析
					memset(szAppendInfo, 0, sizeof(szAppendInfo));
					unsigned char* pbTemp = pbInfo;
					for( int i = 0; i < (int)cInfoHeader.dwInfoSize; )
					{
						memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if( cBlockHeader.dwID == BLOCK_XML_TYPE )  // 识别结果XML附加信息
						{
                            memcpy(szAppendInfo, pbTemp, cBlockHeader.dwLen);
                            if (strstr(szAppendInfo, "PlateName"))
                            {
                                break;
                            }
                            memset(szAppendInfo, 0, sizeof(szAppendInfo));
						}

						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if ( (int)strlen(szAppendInfo) < 128 )
					{
					    continue;
					}

                    DWORD32 dwTimeLow = 0;
                    DWORD32 dwTimeHigh = 0;

					SResultInfo sResultInfo;
                    sResultInfo.strDev = pHvBox->m_szIP;
                    sResultInfo.dwTime = GetSystemTick();

                    char szBuf[64] = {0};
                    if (S_OK == GetXmlValue(szAppendInfo, "PlateName", szBuf, sizeof(szBuf)-1, false))
                    {
                        sResultInfo.strPlateNO = szBuf;
                    }
                    if (S_OK == GetXmlValue(szAppendInfo, "TimeLow", szBuf, sizeof(szBuf)-1, true))
                    {
                        sscanf(szBuf, "%u", &dwTimeLow);
                    }
                    if (S_OK == GetXmlValue(szAppendInfo, "TimeHigh", szBuf, sizeof(szBuf)-1, true))
                    {
                        sscanf(szBuf, "%u", &dwTimeHigh);
                    }

                    DWORD32 dwTimeLowThis = 0;
                    DWORD32 dwTimeHighThis = 0;
                    GetSystemTime(&dwTimeLowThis, &dwTimeHighThis);
                    int iTimeDiff = dwTimeLowThis - dwTimeLow;
                    if (abs(iTimeDiff) > DEV_MAX_DIFF_TIME)
                    {
                        HV_Trace(5, "设备[%s]与控制器的时间相差太大，请同步时间\n", pHvBox->m_szIP);
                        continue;
                    }

                    char szTime[32] = {0};
                    FormatTime(szTime, dwTimeLow, dwTimeHigh);
                    HV_Trace(5, "<BoxInfo>Dev:%s, Time:%s, PlateNO:%s\n", pHvBox->m_szIP, szTime, sResultInfo.strPlateNO.c_str());

                    SemPend(&CHvBox::m_pSemRecord);
					if ( CHvBox::m_pRecordCallBack && CHvBox::AddResultInfo(sResultInfo) )
					{
                        //添加到列表中
                        bool fAddToList = false;
                        for (int i=0; i<MAX_RECORD_BLEN; ++i)
                        {
                            if (false == CHvBox::m_szRecordData[i].fUsed)
                            {
                                CHvBox::m_szRecordData[i].fUsed = true;
                                strcpy(CHvBox::m_szRecordData[i].szIP, pHvBox->m_szIP);
                                CHvBox::m_szRecordData[i].dwTimeLow = dwTimeLow;
                                CHvBox::m_szRecordData[i].dwTimeHigh = dwTimeHigh;
                                CHvBox::m_szRecordData[i].cInfoHeader = cInfoHeader;

                                if (NULL == CHvBox::m_szRecordData[i].pbInfo)
                                {
                                    CHvBox::m_szRecordData[i].pbInfo = new unsigned char[MAX_INFO_SIZE];
                                }
                                if (NULL == CHvBox::m_szRecordData[i].pbData)
                                {
                                    CHvBox::m_szRecordData[i].pbData = new unsigned char[MAX_DATA_SIZE];
                                }
                                if (NULL == CHvBox::m_szRecordData[i].pbInfo || NULL == CHvBox::m_szRecordData[i].pbData)
                                {
                                    HV_Trace(5, "<BoxInfo>memory have been used up!\n");
                                    HV_Exit(HEC_FAIL, "<BoxInfo>memory have been used up!");
                                }

                                memcpy(CHvBox::m_szRecordData[i].pbInfo, pbInfo, cInfoHeader.dwInfoSize);
                                memcpy(CHvBox::m_szRecordData[i].pbData, pbData, cInfoHeader.dwDataSize);

                                fAddToList = true;
                                CHvBox::m_listDataPos.push_back(i);
                                break;
                            }
                        }

                        if (false == fAddToList)
                        {
                            HV_Trace(5, "<BoxInfo>List is full, lost data\n");
                        }
					}
					SemPost(&CHvBox::m_pSemRecord);
				}
			}
        }
	}

	m_fExit = TRUE;

	SAFE_DELETE(pbInfo);
	SAFE_DELETE(pbData);

	return S_OK;
}

//监控线程
HRESULT CMonitorThread::Run(void *pvParamter)
{
    CHvBox* pHvBox = (CHvBox*)pvParamter;
    while ( !m_fExit )
    {
        if (CHvBox::m_pRecordCallBack)
        {
            if (CONN_STATUS_UNKNOWN == pHvBox->m_dwRecordConnStatus || CONN_STATUS_RECONN == pHvBox->m_dwRecordConnStatus)
    		{
                ForceCloseSocket(pHvBox->m_sktRecord);
                pHvBox->m_sktRecord = HvCreateSocket();
    		    HV_Sleep(100);

    			if (ConnectSocket(pHvBox->m_szIP, CAMERA_RECORD_LINK_PORT, pHvBox->m_sktRecord))
    			{
    				pHvBox->m_dwRecordConnStatus = CONN_STATUS_NORMAL;
    				pHvBox->m_dwRecvRecordThreadLastTick = GetSystemTick();
    				if (pHvBox->m_cRecvRecordThread.m_fExit)
    				{
                        pHvBox->m_cRecvRecordThread.Start(pvParamter);
    				}
    				pHvBox->m_dwTryCount = 0;
    				HV_Trace(5, "<BoxInfo>Connected to record connection of %s\n", pHvBox->m_szIP);
    			}
    			else
    			{
    			    ++pHvBox->m_dwTryCount;
    			    HV_Sleep(5000);
    			}
    		}
        }
        else
        {
            if (CONN_STATUS_NORMAL == pHvBox->m_dwRecordConnStatus || CONN_STATUS_RECONN == pHvBox->m_dwRecordConnStatus)
            {
                pHvBox->m_dwRecordConnStatus = CONN_STATUS_DISCONN;
                ForceCloseSocket(pHvBox->m_sktRecord);
                HV_Trace(5, "Disconnected from record connection of %s\n", pHvBox->m_szIP);
            }
        }

		HV_Sleep(1000);
    }

    m_fExit = TRUE;
    return S_OK;
}

HRESULT CSaveThread::Run(void *pvParamter)
{
    char szAppendInfo[APEND_INFO_SIZE] = {0}; // 32KB
    BLOCK_HEADER cBlockHeader;

    m_sRecordData.pbInfo = new unsigned char[MAX_INFO_SIZE];
    m_sRecordData.pbData = new unsigned char[MAX_DATA_SIZE];

    if (NULL == m_sRecordData.pbInfo || NULL == m_sRecordData.pbData)
    {
        HV_Trace(5, "<BoxInfo>memory have been used up!\n");
        HV_Exit(HEC_FAIL, "<BoxInfo>memory have been used up!");
    }

    while ( !m_fExit )
    {
        m_sRecordData.fUsed = false;

        SemPend(&CHvBox::m_pSemRecord);
        if(CHvBox::m_listDataPos.size())
        {
            int i = CHvBox::m_listDataPos.front();
            if (CHvBox::m_szRecordData[i].fUsed)
            {
                m_sRecordData.fUsed = true;
                strcpy(m_sRecordData.szIP, CHvBox::m_szRecordData[i].szIP);
                m_sRecordData.dwTimeLow = CHvBox::m_szRecordData[i].dwTimeLow;
                m_sRecordData.dwTimeHigh = CHvBox::m_szRecordData[i].dwTimeHigh;
                m_sRecordData.cInfoHeader = CHvBox::m_szRecordData[i].cInfoHeader;
                memcpy(m_sRecordData.pbInfo, CHvBox::m_szRecordData[i].pbInfo, m_sRecordData.cInfoHeader.dwInfoSize);
                memcpy(m_sRecordData.pbData, CHvBox::m_szRecordData[i].pbData, m_sRecordData.cInfoHeader.dwDataSize);
                CHvBox::m_szRecordData[i].fUsed = false;
            }
            CHvBox::m_listDataPos.pop_front();
        }
        SemPost(&CHvBox::m_pSemRecord);

        if (m_sRecordData.fUsed)
        {
            //修正CarID
            DWORD32 dwAppendBlockLen = 0;
		    unsigned char* pAppendBlock = NULL;
			// 数据流附加信息解析
			memset(szAppendInfo, 0, sizeof(szAppendInfo));
			unsigned char* pbTemp = m_sRecordData.pbInfo;
			for( int i = 0; i < (int)m_sRecordData.cInfoHeader.dwInfoSize; )
			{
				memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
				pbTemp += sizeof(BLOCK_HEADER);
				i += sizeof(BLOCK_HEADER);

				if( cBlockHeader.dwID == BLOCK_XML_TYPE )  // 识别结果XML附加信息
				{
                    memcpy(szAppendInfo, pbTemp, cBlockHeader.dwLen);
                    if (strstr(szAppendInfo, "PlateName"))
                    {
                        pAppendBlock = pbTemp;
                        dwAppendBlockLen = cBlockHeader.dwLen;
                        break;
                    }
                    memset(szAppendInfo, 0, sizeof(szAppendInfo));
				}

				pbTemp += cBlockHeader.dwLen;
				i += cBlockHeader.dwLen;
			}

			//Add info to szAppendInfo
            if (pAppendBlock
                && g_pcSafeSaver
                && strlen(szAppendInfo) > 0
                && dwAppendBlockLen - strlen(szAppendInfo) > 64
                )
            {
                DWORD32 dwCarID = 0;
                g_pcSafeSaver->GetHourCount(m_sRecordData.dwTimeLow, m_sRecordData.dwTimeHigh, &dwCarID);
                ModifyXmlValue(m_sRecordData.szIP ,dwCarID ,szAppendInfo, sizeof(szAppendInfo));
                memcpy(pAppendBlock, szAppendInfo, dwAppendBlockLen);
            }
            else
            {
                HV_Trace(5, "<BoxInfo>Can't add dev info to append info set\n");
            }

            CHvBox::m_pRecordCallBack(CHvBox::m_pUserData
                                      ,m_sRecordData.szIP
			                          ,m_sRecordData.dwTimeLow
			                          ,m_sRecordData.dwTimeHigh
			                          ,&m_sRecordData.cInfoHeader
			                          ,m_sRecordData.pbInfo
			                          ,m_sRecordData.pbData
			                          );
        }
        else
        {
            HV_Sleep(300);
        }
    }

    SAFE_DELETE(m_sRecordData.pbInfo);
    SAFE_DELETE(m_sRecordData.pbData);
    m_fExit = TRUE;
    return S_OK;
}

CHvBox::CHvBox(const char* szIP)
{
    if (0 == m_dwInstanceCount)
    {
        CreateSemaphore(&m_pSemRecord, 1, 1);
        m_cSaveThread.Start(NULL);
    }
    ++m_dwInstanceCount;
    memset(m_szIP, 0, sizeof(m_szIP));
    strncpy(m_szIP, szIP, sizeof(m_szIP));

    m_sktRecord = INVALID_SOCKET;

    memset(m_szRecordConnCmd, 0, sizeof(m_szRecordConnCmd));
    m_dwRecordConnStatus = CONN_STATUS_UNKNOWN;

    m_dwRecvRecordThreadLastTick = 0;

    m_fOpen = false;
    m_dwTryCount = 0;
}

CHvBox::~CHvBox()
{
    Close();
    if (--m_dwInstanceCount == 0)
    {
        m_cSaveThread.Stop(-1);
        for (int i=0; i<MAX_RECORD_BLEN; ++i)
        {
            if (m_szRecordData[i].pbInfo)
            {
                SAFE_DELETE(m_szRecordData[i].pbInfo);
            }
            if (m_szRecordData[i].pbData)
            {
                SAFE_DELETE(m_szRecordData[i].pbData);
            }
        }
        DestroySemaphore(&m_pSemRecord);
    }
}

//连接设备
int CHvBox::Open()
{
    if (m_fOpen)
    {
        return S_OK;
    }

    m_cMonitorThread.Start(this);

    m_fOpen = true;

    return S_OK;
}

//断开设备连接
void CHvBox::Close()
{
    if (false == m_fOpen)
    {
        return;
    }

    m_cMonitorThread.Stop(-1);
    m_cRecvRecordThread.Stop(-1);

	ForceCloseSocket(m_sktRecord);
	m_dwRecordConnStatus = CONN_STATUS_UNKNOWN;

	m_fOpen = false;
}

//设置回调函数
HRESULT CHvBox::SetCallBack(PVOID pFunc, PVOID pUserData, const char* szConnCmd)
{
    SemPend(&m_pSemRecord);
    m_pRecordCallBack = (RECORD_CALLBACK)pFunc;
    m_pUserData = pUserData;
    SemPost(&m_pSemRecord);

    return S_OK;
}

//获取连接状态
int CHvBox::GetConnStatus(DWORD* pdwConnStatus)
{
    if ( NULL == pdwConnStatus )
	{
		return E_FAIL;
	}

	*pdwConnStatus = m_dwRecordConnStatus;

	return S_OK;
}

//清除过期的记录
void CHvBox::ClearTimeOutResult()
{
    std::vector<SResultInfo>::iterator iter = m_listResultInfo.begin();
    for (; iter!=m_listResultInfo.end(); )
    {
        if (GetSystemTick() - iter->dwTime > CHvBox::m_dwRemainTime)
        {
            m_listResultInfo.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

//增加一条记录
bool CHvBox::AddResultInfo(const SResultInfo& sResultInfo)
{
    ClearTimeOutResult();

    bool fFind = false;
    std::vector<SResultInfo>::const_iterator iter = m_listResultInfo.begin();
    for (iter; iter!=m_listResultInfo.end(); ++iter)
    {
        if (sResultInfo.strPlateNO == iter->strPlateNO && sResultInfo.strDev != iter->strDev)
        {
            int iDiff = sResultInfo.dwTime - iter->dwTime;
            HV_Trace(5, "<BoxInfo> Both %s and %s have result: %s (diff:%d)\n"
                    ,sResultInfo.strDev.c_str()
                    ,iter->strDev.c_str()
                    ,sResultInfo.strPlateNO.c_str()
                    ,iDiff
                    );
            fFind = true;
            break;
        }
    }

    if (fFind)
    {
        return false;
    }

    m_listResultInfo.push_back(sResultInfo);
    return true;
}

DWORD                        CHvBox::m_dwInstanceCount      = 0;         //实例个数
HV_SEM_HANDLE                CHvBox::m_pSemRecord;
PVOID                        CHvBox::m_pUserData            = NULL;
RECORD_CALLBACK              CHvBox::m_pRecordCallBack      = NULL;      //识别结果回调函数
ListResultInfo               CHvBox::m_listResultInfo;                   //记录最近的结果信息
DWORD                        CHvBox::m_dwRemainTime         = 10000;     //记录保留时间
SRecordData                  CHvBox::m_szRecordData[MAX_RECORD_BLEN];
std::list<int>               CHvBox::m_listDataPos;
CSaveThread                  CHvBox::m_cSaveThread;                      //保存线程

CHvBoxHolder::CHvBoxHolder()
{
    m_fInit = false;
    for (int i=0; i<MAX_HV_BOX; ++i)
    {
        m_szPHvBox[i] = NULL;
    }
}

CHvBoxHolder::~CHvBoxHolder()
{
    for (int i=0; i<MAX_HV_BOX; ++i)
    {
        if (m_szPHvBox[i])
        {
            delete m_szPHvBox[i];
            m_szPHvBox[i] = NULL;
        }
    }
}

//初始化函数
HRESULT CHvBoxHolder::Init(const ModuleParams* pModuleParams, ISafeSaver* pcSafeSaver)
{
    if (m_fInit)
    {
        return S_OK;
    }

    if (pcSafeSaver)
    {
        g_pcSafeSaver = pcSafeSaver;
    }

    if (pModuleParams)
    {
        CHvBox::m_dwRemainTime = pModuleParams->cDevParam.iFilterTime * 1000;
        if (pModuleParams->cDevParam.iDevCount > 0)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP1) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP1));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 1)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP2) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP2));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 2)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP3) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP3));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 3)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP4) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP4));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 4)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP5) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP5));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 5)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP6) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP6));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 6)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP7) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP7));
            }
        }
        if (pModuleParams->cDevParam.iDevCount > 7)
        {
            if (strlen(pModuleParams->cDevParam.szDevIP8) > 7)
            {
                m_setDev.insert(std::string(pModuleParams->cDevParam.szDevIP8));
            }
        }
    }
#ifdef PC_TEST
    m_setDev.insert(std::string("172.18.111.65"));
#endif
    int i = 0;
    std::set<std::string>::const_iterator iter = m_setDev.begin();
    for (; iter!=m_setDev.end(); ++iter)
    {
        m_szPHvBox[i] = new CHvBox(iter->c_str());
        m_szPHvBox[i]->Open();
        HV_Trace(5 ,"<BoxInfo> Index:%d IP:%s Init OK ...\n", i, iter->c_str());
        if (++i >= MAX_HV_BOX)
        {
            break;
        }
    }

    if (i > 0)
    {
        CHvBox::SetCallBack((PVOID)RecordCallBack, NULL, NULL);
    }

    m_fInit = true;

    return S_OK;
}

//打印连接状态
void CHvBoxHolder::ShowStatus()
{
    for (int i=0; i<MAX_HV_BOX; ++i)
    {
        if (m_szPHvBox[i])
        {
            DWORD dwConnStatus = -1;
            m_szPHvBox[i]->GetConnStatus(&dwConnStatus);
            HV_Trace(5, "<BoxInfo> Index:%d, IP:%s, Status:%d\n", i+1, m_szPHvBox[i]->m_szIP, dwConnStatus);
        }
    }
}
