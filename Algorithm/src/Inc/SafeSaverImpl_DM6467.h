#ifndef _INCLUDE_SAFESAVERIMPL_DM6467_H_
#define _INCLUDE_SAFESAVERIMPL_DM6467_H_

#include "safesaver.h"
#define DISK_TYPE "sda"                 /**< 硬盘类型，有效值：sda或hda */
#define PARTITION_TYPE "ext2"           /**< 分区格式，默认值：ext2 */

#define DISK_RESULT_FILENAME "Result.dat"
#define DISK_VIDEO_FILENAME "Video.dat"
#define FILE_COUNT_FILENAME "FileCount.dat"   /**< 可靠性识别结果保存记录每个小时文件数，每个小时一个文件，存放在小时目录 */
#define SS_VIDEO_INFO_FILE "v.dat"      /**< 可靠性视频保存信息文件，每个分区一个文件 */
#define SAVE_DISK_COUNT 4               /**< 可靠性保存硬盘分区数 */
#define SAVE_MAX_USE 50         /**< 可靠性保存每个分区最大使用率 */
#define SAVE_MAXDAY_COUNT 2     /**< 可靠性保存每个分区保存最大天数 */

// zhaopy video temp
const int MAX_DATA_COUNT = 3;

enum RECORD_FILEHEAD_FLAG
{
    RFF_Dm6467 = 0xffff0002  // 文件版本标志，无独立的附加信息
};

#define MAX_VIDEO_DATA_SIZE (1024*1024*1)

typedef struct tag_VIDEO_FILEHEAD
{
    DWORD32 dwVideoDataSize;
} VIDEO_FILEHEAD;

// 视频记录块数据类型
enum VideoDataType
{
    VDT_H264 = 1,
};

typedef struct tag_VIDEO_BLOCK  // 视频记录块
{
    int fUsed;
    int iVideoDataType;
    int iVideoDataSize;
    DWORD32 dwLowTime;
    DWORD32 dwHighTime;

    tag_VIDEO_BLOCK()
    {
        Clear();
    }

    void Clear()
    {
        fUsed = FALSE;
        iVideoDataType = 0;
        iVideoDataSize = 0;
        dwLowTime = dwHighTime = 0;
    }

} VIDEO_BLOCK;

// “可靠性识别结果保存信息文件”内容结构
typedef struct tagSafeSaveInfo
{
    int iIsActiveDisk;
}
DISK_SAVE_INFO;

// 每个小时结果数
typedef struct tagPathRecordCount
{
    int iRecordCount;
}
PATH_RECORD_COUNT;

// “可靠性视频保存信息文件”内容结构
typedef struct tagVideoSafeSaveInfo
{
    DWORD32 dwDurationSecond;
    DWORD32 dwStartTimeMsLow;
    DWORD32 dwStartTimeMsHigh;
} VideoSafeSaveInfo;

class CAutoLock
{
public:
    CAutoLock(HV_SEM_HANDLE * hSem)
    {
        m_hSem = hSem;
        if (m_hSem)
        {
            SemPend(m_hSem);
        }
    }
    virtual ~CAutoLock()
    {
        if (m_hSem)
        {
            SemPost(m_hSem);
        }
    }
private:
    HV_SEM_HANDLE * m_hSem;
};

typedef struct tagRESULTINFO
{
    int iType;
    DWORD32 dwTimeLow;
    DWORD32 dwTimeHigh;
    int iIndex;
    CAMERA_INFO_HEADER cihHeader;
    unsigned char* pbInfo;
    int iInfoSize;
    unsigned char* pbData;
    int iDataSize;
    char szDevIno[255];

}RESULTINFO;

class CSafeSaverDm6467Impl : public ISafeSaver, public CHvThreadBase
{
public:
    CSafeSaverDm6467Impl(void);
    ~CSafeSaverDm6467Impl(void);

public:
    //ISafeSaver接口方法
    virtual HRESULT Init(const SSafeSaveParam* pParam);
    void SetFreeSpacePerDisk(int iResultFreeSpacePerDisk, int iVideoFreeSpacePerDisk)
    {
        m_iResultFreeSpacePerDisk = iResultFreeSpacePerDisk;
        m_iVideoFreeSpacePerDisk = iVideoFreeSpacePerDisk;
    }
    HRESULT Add(int iType,
                DWORD32 dwTimeLow,
                DWORD32 dwTimeHigh,
                int iIndex,
                const CAMERA_INFO_HEADER *cihHeader,
                const unsigned char* pbInfo,
                const unsigned char* pbData,
                const char* szDevIno);

    HRESULT Run(void* pvParam);

    HRESULT SavePlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
        const CAMERA_INFO_HEADER* pInfoHeader,
        const unsigned char* pbInfo,
        const unsigned char* pbData,
        const char* szDevIno  //设备信息(编号或者IP)
    );

    HRESULT SavePlateRecordInThread(
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
        int & nLastDiskID, //上一条记录的分区号
        const DWORD32 dwDataInfo    //指定某种数据信息类型，比如违章
    );

    HRESULT SaveVideoRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
        const CAMERA_INFO_HEADER* pInfoHeader,
        const unsigned char* pbInfo,
        const unsigned char* pbData
    );

    HRESULT SaveVideoRecordInThread(
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

    HRESULT SaveDetectData(CDetectData *pData);
    HRESULT GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData *pData);

    HRESULT GetDiskStatus();
    HRESULT GetLastCamStatus(DWORD32* pdwStatus);

    HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        return E_NOTIMPL;
    }

    // 从硬盘获取当前保存记录索引
    // 返回值 -1: 错误; 其他：当前索引
    int GetCurRecordIndex()
    {
        return m_iRecordLastIndex;
    }

    //获取某个小时目录下的结果数量
    virtual HRESULT GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh, DWORD32* pdwCount);

private:
    bool SaveFileCount(int iCurDisk, REAL_TIME_STRUCT* pRealtime, int iRecordCount);
    int GetFileCount(int iCurDisk, REAL_TIME_STRUCT* pRealtime);
    bool SaveActiveDisk(int iDiskBase, int iDiskCount, int iCurDisk, bool bVideo);
    int GetActiveDisk(int iDiskBase, int iDiskCount, bool bVideo);

    int FindDiskNumber(REAL_TIME_STRUCT* pRealtime, int iDiskBase, int iDiskCount, int iCurDisk, int iMinFreeDiskPer, bool bVideo);
    int FindHourPath(REAL_TIME_STRUCT* pRealtime, int iDiskBase, int iDiskCount);
    bool IsSameHour(REAL_TIME_STRUCT* prtLeft, REAL_TIME_STRUCT* prtRight);

    HRESULT LockRead();
    HRESULT UnLockRead();


    bool IsDirectoryExist(char *pszPath);
    /*
    *  格式化硬盘分区
    *  nDiskID: 分区ID，0为第一个分区，1为第二个分区，以此类推
    **/
    bool FormatDisk(int nDiskID);

    /*
    *  获取硬盘分区使用率
    *  nDiskID: 分区ID，0为第一个分区，1为第二个分区，以此类推
    *  return: 硬盘分区使用率，0～100，-1表示获取失败
    **/
    int GetDiskUsed(int nDiskID);

    /**
    *  换小时时调用，保存当前识别结果或者录像一个小时数据最大占有空间量
    *  pRealtime 更换小时之前最大的时间
    *  fVideo 是否是视频数据
    *  return: 成功返回true失败返回false
    **/
    bool SaveDirectoryMaxOccupySpace(REAL_TIME_STRUCT* pRealtime, int iCurDisk, bool fVideo);

    /**
    * 获取硬盘分区当前是否能够存够一个小时的数据
    * nDiskID: 分区ID，0为第一个分区，1为第二个分区，以此类推
    *  return: true 够一个小时，1不够一个小时
    **/
    bool IsEnoughSpaceToSaveOneHourData(int nDiskID, bool fVideo);

    /*
    *  更换下一个存储分区
    *  return: true成功，否则失败
    **/
    bool ChangeNextDiskEx(int iDiskBase, int iDiskCount, int* piCurDisk);
private:
    HV_SEM_HANDLE m_hSemQueCount;
    HV_SEM_HANDLE m_hSemQueCtrl;
    CHvList<RESULTINFO *> m_rsUseList;
    CHvList<RESULTINFO *> m_rsUnuseList;
private:
    DWORD32  m_dwDiskCount;
    // plate
    static const int RECORD_FREEDISK_PER = 10;

    DWORD32 m_dwRecordDiskBase;
    DWORD32 m_dwRecordDiskCount;
    int m_iRecordCurDisk;
    int m_iRecordLastIndex;
    REAL_TIME_STRUCT m_rtLastRecord;
    // video
    static const int VIDEO_FREEDISK_PER = 10;

    DWORD32 m_dwVideoDiskBase;
    DWORD32 m_dwVideoDiskCount;
    int m_iVideoCurDisk;
    int m_iVideoLastIndex;
    REAL_TIME_STRUCT m_rtLastVideo;

private:
    bool m_fInitialized;
    bool m_fSaveRecord;
    bool m_fSaveVideo;
    bool m_fDiskIsOk;
    HRESULT m_hrDiskStatus;
    int m_iResultFreeSpacePerDisk;
    int m_iVideoFreeSpacePerDisk;
    HV_SEM_HANDLE m_hSemRead; // 为了保证对硬盘数据的读取与写入是原子性的。
    bool m_fCoverCtrlMode;    //段面控制器的存储模式
    REAL_TIME_STRUCT m_rtsChangeDisk; //导致切换到下一分区的识别结果的时间
public:
    // zhaopy
    HRESULT GetVideoRecordInfo(DWORD32& dwDurationSecond, DWORD32& dwStartTimeMsLow, DWORD32& dwStartTimeMsHigh)
    {
        return E_NOTIMPL;
    }
};

#endif
