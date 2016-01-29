// 该文件编码必须是WINDOWS-936格式

#include "HvUtilsClass.h"
#include "ControlFunc.h"

// 恢复InitHdd参数为默认值
/* ControlFunc.cpp */
extern int ResetInitHddParam();

CHddOpThread g_cHddOpThread;  // 全局唯一
int g_nHddCheckStatus = 1;

CHddOpThread::CHddOpThread()
{
    m_nStatusCode = 0;
    m_strStatusCode = "无";
    m_eHddOpType = HOT_UNKNOWN;
    m_iDiskCount = 2;
}

void CHddOpThread::ReadCheckReport(const char* szFileName, char* szReport, int& nReportLen)
{
    unsigned long ulFileSize = 0;
    if ( 0 == GetFileSize(szFileName, ulFileSize) )
    {
        ulFileSize = (int)ulFileSize > nReportLen ? nReportLen : ulFileSize;
        if ( ulFileSize > 0 )
        {
            ReadDataFromFile(szFileName, (unsigned char*)(szReport+nReportLen), ulFileSize);
            nReportLen += ulFileSize;
            szReport[nReportLen] = '\0';
            strcat(szReport, "\n---\n");
            nReportLen += strlen("\n---\n");
        }
    }
    nReportLen = ulFileSize;
}

void CHddOpThread::SetHddOpType(HddOpType eHddOpType, int iDiskCount)
{
    m_eHddOpType = eHddOpType;

    m_iDiskCount = iDiskCount;
    if( m_iDiskCount < 1 || m_iDiskCount > 10 )
    {
        m_iDiskCount = 2;
    }
}

HRESULT CHddOpThread::Run(void *pvParamter)
{
    HRESULT hResult = S_OK;
    g_nHddCheckStatus = 0;
    if ( HOT_FULL_INIT == m_eHddOpType )
    {
        Trace("------------ start CHddOpThread::Run is g_nHddCheckStatus =%d ------------\n", g_nHddCheckStatus);
        ResetInitHddParam();
        char szMsg[256] = {0};
        sprintf(szMsg, "dm 1 %d /Result_disk", m_iDiskCount);
        m_strStatusCode = "硬盘分区中";
        //等待12个小时
        if (!HV_SystemWithTimeOut(szMsg, 43200000))
        {
            g_nHddCheckStatus = 1;
            m_strStatusCode = "正常";
        }
        else
        {
            g_nHddCheckStatus = 2;
            m_strStatusCode = "硬盘分区错误";
        }
        Trace("------------ CHddOpThread::Run is finish.g_nHddCheckStatus =%d ------------\n", g_nHddCheckStatus);
    }
    else if ( HOT_INIT == m_eHddOpType )
    {
        Trace("------------ start CHddOpThread::Run is g_nHddCheckStatus =%d ------------\n", g_nHddCheckStatus);
        m_strStatusCode = "硬盘检测中";
        //等待12个小时
        if (!HV_SystemWithTimeOut("dm 2 0 /Result_disk", 43200000))
        {
            m_strStatusCode = "正常";
            g_nHddCheckStatus = 1;
        }
        else
        {
            g_nHddCheckStatus = 2;
            m_strStatusCode = "硬盘检测错误";
        }
        Trace("------------ CHddOpThread::Run is finish.g_nHddCheckStatus =%d, ------------\n", g_nHddCheckStatus);
    }
    else if (HOT_CHECKFACTOR == m_eHddOpType)
    {
        Trace("------------ start CHddOpThread::Run is g_nHddCheckStatus =%d ------------\n", g_nHddCheckStatus);
        ResetInitHddParam();
        m_strStatusCode = "全面检测硬盘中";
        //等待12小时
        if (!HV_SystemWithTimeOut("dm 5 0 /Result_disk", 43200000))
        {
            g_nHddCheckStatus = 1;
            m_strStatusCode = "正常";
        }
        else
        {
            g_nHddCheckStatus = 2;
            m_strStatusCode = "全面检测硬盘失败";
        }
        Trace("------------ CHddOpThread::Run is finish.g_nHddCheckStatus =%d, ------------\n", g_nHddCheckStatus);
    }
    else if (HOT_NOTHING == m_eHddOpType)
    {
        g_nHddCheckStatus = 3;
    }
    else
    {
        hResult = E_NOTIMPL;
    }
    SetBuzzerOn();
    HV_Sleep(200);
    SetBuzzerOff();
    return hResult;
}

int CHddOpThread::GetCurStatusCode()
{
    return m_nStatusCode;
}

const char * CHddOpThread::GetCurStatusCodeString(void)
{
    if (m_strStatusCode.IsEmpty())
    {
        return "";
    }
    return m_strStatusCode.GetBuffer();
}

void CHddOpThread::SetStatusCodeString(const char *szStatus)
{
    if(szStatus && strcmp(m_strStatusCode, szStatus))
    {
        m_strStatusCode =szStatus;
    }
}

int CHddOpThread::GetCheckReport(char* szReport, int size)
{
    ReadCheckReport("report.txt", szReport, size);
    if ( !size )
    {
        strcpy(szReport, "error.\n");
        return 0;
    }
    return 1;
}

CCheckDisk g_cCheckDisk;

CCheckDisk::CCheckDisk()
{
	m_iSts = 0;
	m_sCheckDiskReport.iResult = CDR_UNDETECTION;
}

CCheckDisk::~CCheckDisk()
{
}

int CCheckDisk::DoPartition()
{
	if (1 == m_iSts)
	{
		return CDR_PARTITIONING;
	}

	if (2 == m_iSts)
	{
		return CDR_DETECTING;
	}

	if (false == HaveDisk())
	{
		return CDR_NODISK;
	}

	m_iSts = 1;
	Start(this);
	HV_Sleep(100);

	return 0;
}

HRESULT CCheckDisk::Run(void *pvParamter)
{
	if (0 == m_iSts)
	{
		return S_FALSE;
	}

	g_nHddCheckStatus = 0;
	if (1 == m_iSts)//分区
	{
		if (!HV_SystemWithTimeOut("dm 1 2 /Result_disk", 43200000))
	    {
	  		g_nHddCheckStatus = 1;
	  		HV_Trace(5, "\n\n内置存储分区完成...\n\n");
	    }
	    else
	    {
	    	g_nHddCheckStatus = 2;
	    	HV_Trace(5, "\n\n内置存储分区失败...\n\n");
	    }
	}
	else if (2 == m_iSts)//检测
	{
		m_sCheckDiskReport.iResult = CDR_UNDETECTION;
		memset(m_sCheckDiskReport.szMsg, 0, sizeof(m_sCheckDiskReport.szMsg));
		if (!HV_SystemWithTimeOut("dm 7 0 /Result_disk", 43200000))
	    {
	    	HV_Sleep(100);
	    	srand(GetSystemTick());
    		char szWrite[64] = {0};
    		char szRead[64] = {0};
    		sprintf(szWrite, "%d", rand()%100000000);

	    	char szFile[256] = {0};
	    	sprintf(szFile, "/Result_disk/0/%s.txt", szWrite);
	    	FILE* pFile = fopen(szFile, "w");
	    	if (pFile)
	    	{
	    		fwrite(szWrite, 1, strlen(szWrite), pFile);
	    		fclose(pFile);
	    		pFile = NULL;
	    	}

			HV_Sleep(100);
	    	pFile = fopen(szFile, "r");
	    	if (pFile)
	    	{
	    		fread(szRead, 1, sizeof(szRead), pFile);
	    		fclose(pFile);
	    		pFile = NULL;
	    	}

	    	if (0 == strcmp(szWrite, szRead))
	    	{
	    		GetDiskInfo();
	    		g_nHddCheckStatus = 1;
	        	m_sCheckDiskReport.iResult = CDR_PASS;
	        	HV_Trace(5, "\n\n内置存储测试通过...\n\n");
	    	}
	    	else
	    	{
	    		g_nHddCheckStatus = 2;
	        	m_sCheckDiskReport.iResult = CDR_UNPASS;
	        	strcpy(m_sCheckDiskReport.szMsg, "读写校验失败");
	        	HV_Trace(5, "\n\n读写测试不通过...\n\n");
	    	}
	    	unlink(szFile);
	    }
	    else
	    {
	    	g_nHddCheckStatus = 2;
	        m_sCheckDiskReport.iResult = CDR_UNPASS;
	        strcpy(m_sCheckDiskReport.szMsg, "e2fsck 检测到异常");
	        HV_Trace(5, "\n\n内置存储测试不通过...\n\n");
	    }
	}

	HV_Sleep(100);
	m_fExit = true;
	m_iSts = 0;

	return S_OK;
}

int CCheckDisk::DoCheckDisk()
{
	if (1 == m_iSts)
	{
		return CDR_PARTITIONING;
	}

	if (2 == m_iSts)
	{
		return CDR_DETECTING;
	}

	if (0 == g_nHddCheckStatus)
	{
		return CDR_PARTITIONING;
	}

	if (false == HaveDisk())
	{
		return CDR_NODISK;
	}

	if (false == IsPartition())
	{
		return CDR_UNPARTITION;
	}

	m_iSts = 2;
	Start(this);
	HV_Sleep(100);

	return 0;
}

void CCheckDisk::GetCheckDisk(SCheckDiskReport& sCheckDiskReport)
{
	memcpy(&sCheckDiskReport, &m_sCheckDiskReport, sizeof(sCheckDiskReport));
}

bool CCheckDisk::IsPartition()
{
	char szShellCmd[128] = {0};
	strcpy(szShellCmd, "fdisk -l| grep \"/dev/sda1\"");
	FILE* pFile = popen(szShellCmd, "r");
	if (pFile)
	{
		char szInfo[128] = {0};
		fread(szInfo, 1, sizeof(szInfo), pFile);
		pclose(pFile);
		pFile = NULL;

		if (strlen(szInfo) > 3)
		{
			return true;
		}
	}

	HV_Trace(5, "Disk has not partition...\n");
	return false;
}

bool CCheckDisk::HaveDisk()
{
	char szShellCmd[128] = {0};
	strcpy(szShellCmd, "fdisk -l|wc -l");
	FILE* pFile = popen(szShellCmd, "r");
	if (pFile)
	{
		char szInfo[128] = {0};
		fread(szInfo, 1, sizeof(szInfo), pFile);
		pclose(pFile);
		pFile = NULL;

		if (strlen(szInfo) > 0 && atoi(szInfo) > 0)
		{
			return true;
		}
	}

	return false;
}

// 功能简述：将strInfo中的strsrc全部替换为strdst
static void string_replace(string& strInfo, const string& strsrc, const string& strdst)
{
    string::size_type pos=0;
    string::size_type srclen=strsrc.size();
    string::size_type dstlen=strdst.size();
    while ( (pos=strInfo.find(strsrc, pos)) != string::npos)
    {
        strInfo.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}

void CCheckDisk::GetDiskInfo()
{
	char szShellCmd[128] = {0};
	strcpy(szShellCmd, "df -h|grep \"/dev/sda\"|awk \'{print $2,$3,$4}\'");
	FILE* pFile = popen(szShellCmd, "r");
	if (pFile)
	{
		char szInfo[256] = {0};
		fread(szInfo, 1, sizeof(szInfo), pFile);
		pclose(pFile);
		pFile = NULL;

		string strInfo = szInfo;
		string_replace(strInfo, "\n", ";");
		strncpy(m_sCheckDiskReport.szMsg, strInfo.c_str(), sizeof(m_sCheckDiskReport.szMsg) - 1);
	}

	HV_Trace(5, "\n%s\n", m_sCheckDiskReport.szMsg);

	return;
}

