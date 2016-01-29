// 该文件编码必须是WINDOWS-936格式
#ifndef _SAFESAVERIMPL_NET_H_
#define _SAFESAVERIMPL_NET_H_

#include "safesaver.h"
#include "ConstFileSys.h"
#include "FixFileStream.h"
#include "BigFile.h"
#include <string>
#include <list>
#include "DspLinkMemBlocks.h"

class CSafeSaverNetImpl : public ISafeSaver,public CHvThreadBase
{
public:
    CSafeSaverNetImpl(void);
    ~CSafeSaverNetImpl(void);

	virtual HRESULT Run(void *pvParamter);

public:
    //ISafeSaver接口方法
    HRESULT Init(const SSafeSaveParam* pParam);

    HRESULT SavePlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
        const CAMERA_INFO_HEADER* pInfoHeader,
        const unsigned char* pbInfo,
        const unsigned char* pbData,
        const char* szDevIno  //设备信息(编号或者IP)
    );

    virtual HRESULT SavePlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
        CCameraDataReference* pRefData //数据引用
    );

    HRESULT GetPlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int& index,
        CAMERA_INFO_HEADER* pInfoHeader,
        unsigned char* pbInfo, const int iInfoLen,
        unsigned char* pbData, const int iDataLen,
        const char* szDevIno,  //设备信息(编号或者IP)
        int & nLastDiskID,
        const DWORD32 dwDataInfo
    );

    HRESULT SaveVideoRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
        const CAMERA_INFO_HEADER* pInfoHeader,
        const unsigned char* pbInfo,
        const unsigned char* pbData
    );

    HRESULT GetVideoRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
        CAMERA_INFO_HEADER* pInfoHeader,
        unsigned char* pbInfo, const int iInfoLen,
        unsigned char* pbData, const int iDataLen
    );

    //获取某个小时目录下的结果数量
    HRESULT GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh, DWORD32* pdwCount);

    HRESULT SaveDetectData(CDetectData *pData);

    HRESULT GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData *pData);

    HRESULT GetDiskStatus();

    HRESULT GetLastCamStatus(DWORD32* pdwStatus);

    HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);

    //查找下一个视频文件
    HRESULT FindNextVideo(DWORD32& dwTimeLow, DWORD32& dwTimeHigh);

private:

	HRESULT PutData(const int iDataType //数据类型 0:识别结果 1:录像
					,const DWORD32 dwTimeLow
				    ,const DWORD32 dwTimeHigh
				    ,const DWORD32 dwDataLen
				    ,int* piIndex
				    ,CCameraDataReference* pRefData
					);

	HRESULT GetData(const int iDataType //数据类型 0:识别结果 1:录像
				   ,DWORD32 dwTimeLow
				   ,DWORD32 dwTimeHigh
				   ,DWORD32 dwDataInfo
				   ,int& index
    			   ,CAMERA_INFO_HEADER* pInfoHeader
    			   ,unsigned char* pbInfo
    			   ,const int iInfoLen
				   ,unsigned char* pbData
				   ,const int iDataLen
				   ,const char* szDevIno  //设备信息(编号或者IP)
				   );

	//拷贝和修正数据
	HRESULT CopyCfData(SCfData& sCfData);
  bool    ReconnectNFS(void);
private:

    HV_SEM_HANDLE               m_hSem;                   //存储系统的锁
    HV_SEM_HANDLE               m_hSemQue;				   //队列的锁
    HV_SEM_HANDLE               m_hSemQueCount;            //队列信号量
    std::list<SCfData>          m_listCfData;

	SSafeSaveParam			    m_sParam;			 		//初始化参数

#if FIX_FILE_VER == 1
    CConstFileSys               m_cCfRecord;           	  //定长存储系统
    CConstFileSys               m_cCfVideo;           		//定长存储系统
#elif FIX_FILE_VER == 2
    CFixFileStream              m_fixRecord;
    CFixFileStream              m_fixVideo;
#elif FIX_FILE_VER == 3
    CBigFile                    m_bigFileRecord;
    CBigFile                    m_bigFileVideo;
#endif
    DSPLinkBuffer               m_sBuf;
    HRESULT                     m_hrDiskStatus;     		//硬盘状态
    DWORD32                     m_dwLastDataTime;
    REAL_TIME_STRUCT            m_rtsLastVideoTime;       //最后一次保存录像的时间

};

#endif
