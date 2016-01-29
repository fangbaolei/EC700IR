// 该文件编码必须是WINDOWS-936格式
#ifndef __DATA_CTRL_H__
#define __DATA_CTRL_H__

#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include "hvthreadbase.h"
#include "HvSockUtils.h"
#include "CameraLinkBase.h"
#include "LoadParam.h"

//连接状态
const int CONN_STATUS_UNKNOWN   = 0;	//未知
const int CONN_STATUS_NORMAL    = 1;	//正常
const int CONN_STATUS_DISCONN   = 2;	//断开
const int CONN_STATUS_RECONN    = 3;	//重连中

const int MAX_HV_BOX            = 8;
const int MAX_RECORD_BLEN       = 30;

//识别结果回调函数
typedef HRESULT (*RECORD_CALLBACK)(PVOID pUserData                          //用户上下文
                                  ,const char* szDev                         //设备标识(编号或IP)
                                  ,DWORD32 dwTimeLow
                                  ,DWORD32 dwTimeHigh
                                  ,const CAMERA_INFO_HEADER* pInfoHeader
                                  ,const unsigned char* pbInfo
                                  ,const unsigned char* pbData
                                  );

struct SResultInfo
{
    std::string    strDev;          //哪个设备
    std::string    strPlateNO;      //号码
    DWORD          dwTime;          //时间
    SResultInfo();
	SResultInfo(const SResultInfo& rs);
	SResultInfo& operator=(const SResultInfo& rs);
};

struct SRecordData
{
    bool                fUsed;
    char                szIP[32];
    DWORD32             dwTimeLow;
    DWORD32             dwTimeHigh;
    CAMERA_INFO_HEADER  cInfoHeader;
    unsigned char*      pbInfo;
    unsigned char*      pbData;

    SRecordData()
    {
        memset(this, 0, sizeof(*this));
    }
};

typedef std::vector<SResultInfo> ListResultInfo;

//接收识别结果的线程
class CRecvRecordThread : public CHvThreadBase
{
public:
    virtual HRESULT Run(void *pvParamter);
};

//监控线程
class CMonitorThread : public CHvThreadBase
{
public:
    virtual HRESULT Run(void *pvParamter);
};

//保存线程
class CSaveThread : public CHvThreadBase
{
public:
    virtual HRESULT Run(void *pvParamter);

private:
    SRecordData     m_sRecordData;
};

class CHvBoxHolder;

class CHvBox
{
public:
    CHvBox(const char* szIP);

    ~CHvBox();

public:
    //打开设备
    HRESULT Open();

    //关闭设备
    void Close();

    //获取连接状态
	HRESULT GetConnStatus(DWORD* pdwConnStatus);

	//设置数据流接收回调函数
    static HRESULT SetCallBack(PVOID pFunc, PVOID pUserData, const char* szConnCmd);

	friend class CRecvRecordThread;
	friend class CMonitorThread;
	friend class CHvBoxHolder;
	friend class CSaveThread;

protected:
    //清除过期的记录
    static void ClearTimeOutResult();

    //增加一条记录
    static bool AddResultInfo(const SResultInfo& sResultInfo);

private:
    static DWORD                            m_dwInstanceCount;              //实例个数
    static HV_SEM_HANDLE                    m_pSemRecord;
    static PVOID                            m_pUserData;
    static RECORD_CALLBACK                  m_pRecordCallBack;              //识别结果回调函数
    static ListResultInfo                   m_listResultInfo;               //记录最近的结果信息
    static DWORD                            m_dwRemainTime;                 //记录保留时间

    static SRecordData                      m_szRecordData[MAX_RECORD_BLEN];
    static std::list<int>                   m_listDataPos;
    static CSaveThread                      m_cSaveThread;                  //保存线程

private:
    char                                    m_szIP[32];                     //设备IP
	HV_SOCKET_HANDLE                        m_sktRecord;
	char                                    m_szRecordConnCmd[128];
	DWORD                                   m_dwRecordConnStatus;

	CRecvRecordThread                       m_cRecvRecordThread;
	CMonitorThread                          m_cMonitorThread;

	DWORD                                   m_dwRecvRecordThreadLastTick;

	bool                                    m_fOpen;
	DWORD                                   m_dwTryCount;                   //尝试重连的次数
};

typedef CHvBox* PHvBox;

class CHvBoxHolder
{
public:
    CHvBoxHolder();
    ~CHvBoxHolder();

    //初始化函数
    HRESULT Init(const ModuleParams* pModuleParams, ISafeSaver* pcSafeSaver);

    //打印连接状态
    void ShowStatus();
private:
    PHvBox                                   m_szPHvBox[MAX_HV_BOX];
    std::set<std::string>                    m_setDev;                      //设备列表
    bool                                     m_fInit;                       //是否初始化了
};
#endif
