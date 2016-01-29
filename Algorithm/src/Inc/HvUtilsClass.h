#ifndef _HVUTILSCLASS_H_
#define _HVUTILSCLASS_H_

#include <stdio.h>
#include <sys/wait.h>
#include "config.h"
#include "misc.h"
#include "hvthreadbase.h"
#include "ICMP.h"
//-------------------------------------------------

#ifdef _USE_SEED_DEBUG_
/* DSPLink.c */
extern int g_fDebugStart;
#endif

//临时加入的一个类：用来随时退出程序。
class CUserExitThread : public CHvThreadBase
{
public:
    virtual const char* GetName()
    {
        static char szName[] =  "CUserExitThread";
        return szName;
    }

    virtual HRESULT Run(void *pvParamter)
    {
#if !defined(_HVCAM_PLATFORM_RTM_)
        Trace("CUserExitThread is Running...\n");

        while ( !m_fExit )
        {
            char c = getchar();
            if ( 'q' == c )
            {
                HV_Exit(HEC_SUCC|HEC_RESET_DEV, "ExitDebug");
            }
#ifdef _USE_SEED_DEBUG_
            else if ( 'g' == c )
            {
                g_fDebugStart = 1;
            }
#endif // _USE_SEED_DEBUG_
        }
        Trace("CUserExitThread is Exiting...\n");
        HV_Exit(HEC_FAIL, "UserExit");
#endif // #if !defined(_HVCAM_PLATFORM_RTM_) || defined(SINGLE_BOARD_PLATFORM)

        return S_OK;
    };
};

//-------------------------------------------------

#define PING_GATEWAY_T 10000  // ping网关周期。单位：毫秒

// Ping网关线程。用途：每隔一定周期ping一次网关，目的是为了更新ARP表。
class CPingThread : public CHvThreadBase
{
public:
    void SetIPAddress(char * szIP, char * szNetmask, char* pszGateWay)
    {
        int a,b,c,d, e,f,g,h, i,j,k,l;
        if (4 == sscanf(szIP, "%d.%d.%d.%d", &a, &b, &c, &d)
                &&4 == sscanf(szNetmask, "%d.%d.%d.%d", &e, &f, &g, &h)
                &&4 == sscanf(pszGateWay, "%d.%d.%d.%d", &i, &j, &k, &l)
                && !i && !j && !k && !l)
        {
            i = a & e;
            j = b & f;
            k = c & g;
            l = 1;
        }
        else
        {
            i = 172;
            j = 18;
            k = 10;
            l = 49;
        }
        sprintf(szGateWay, "%d.%d.%d.%d", i, j, k, l);
    }

    virtual const char* GetName()
    {
        static char szName[] = "CPingThread";
        return szName;
    }

    virtual HRESULT Run(void *pvParamter)
    {
        Trace("CPingThread is Running...\n");

        CICMP cICMP;
        int num = 0;
        static char szPingCmd[64] = {"test"};

        while ( !m_fExit )
        {
            HV_Trace(5, "Ping Gateway===%s.\n", szGateWay);

            cICMP.setId(getpid());
            cICMP.setSeq(++num);
            cICMP.send_icmp(szGateWay, szPingCmd, strlen(szPingCmd));

            HV_Sleep(PING_GATEWAY_T);
        }

        Trace("CPingThread is Exiting.\n");
        return S_OK;
    };

private:
    char szGateWay[16];
};

//-------------------------------------------------

typedef enum tagHddOpType
{
    HOT_UNKNOWN,
    HOT_FULL_INIT,  // 完全初始化硬盘（分区，格式化，检测，生成检测报告）
    HOT_INIT,       // 初始化硬盘（检测，生成检测报告，挂载分区）
    HOT_CHECKFACTOR,// 检测扇区
    HOT_NOTHING     // 不进行任何硬盘操作
} HddOpType;

// 硬盘操作线程
class CHddOpThread : public CHvThreadBase
{
public:
    CHddOpThread();

    virtual const char* GetName()
    {
        static char szName[] = "CHddOpThread";
        return szName;
    }

    virtual HRESULT Run(void *pvParamter);

    void SetHddOpType(HddOpType eHddOpType, int iDiskCount);
    int GetCheckReport(char* szReport, int size);
    int GetCurStatusCode();
    const char * GetCurStatusCodeString(void);
    void SetStatusCodeString(const char *szStatus);
private:
    void ReadCheckReport(const char* szFileName, char* szReport, int& nReportLen);
    HRESULT AnalyticCheckReport();

    int m_nStatusCode;  //TODO:需要去除MagicNum
    CHvString m_strStatusCode;
    HddOpType m_eHddOpType;
    int m_iDiskCount;
};

/* HvUtilsClass.cpp */
extern CHddOpThread g_cHddOpThread;

enum ECdrReport
{
	CDR_UNKNOWN = 0,
	CDR_UNDETECTION,      //未检测
	CDR_UNPARTITION,  		//硬盘未分区
	CDR_PARTITIONING, 	//硬盘分区中
	CDR_DETECTING,    		//检测中
	CDR_PASS,          	//检测通过
	CDR_UNPASS,        	//检测失败
	CDR_NODISK,        	//没有硬盘
	CDR_COUNT,
};

struct SCheckDiskReport
{
	int    	iResult;    //返回值
	char    szMsg[256]; //消息

	SCheckDiskReport()
	{
		memset(this, 0 ,sizeof(*this));
	}
};

//供生产匹配检测硬盘或者U盘
class CCheckDisk : public CHvThreadBase
{
public:
	CCheckDisk();
	~CCheckDisk();

	virtual const char* GetName()
	{
		static char szName[] = "CCheckDisk";
		return szName;
	}

	virtual HRESULT Run(void *pvParamter);

	//分区
	int DoPartition();

	//触发检测
	int DoCheckDisk();

	//读取检测结果
	void GetCheckDisk(SCheckDiskReport& sCheckDiskReport);

protected:
	bool IsPartition();
	bool HaveDisk();
	void GetDiskInfo();

private:
	SCheckDiskReport    m_sCheckDiskReport;
	int                 m_iSts;  //0空闲 1分区中 2检测中
};

extern CCheckDisk g_cCheckDisk;

#endif  // _HVUTILSCLASS_H_
