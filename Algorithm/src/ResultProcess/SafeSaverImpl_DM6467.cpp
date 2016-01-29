#include <sys/statfs.h>
#include "SafeSaverImpl_DM6467.h"
#include "hvutils.h"
#include "hvtarget_ARM.h"
#include "misc.h"
#include <vector>
#include <dirent.h>
#include "tinyxml.h"

#define  LOG_DIR "/log"
#define  RESULT_LIST_NUM  3
#define  MAX_INFO_BUFFER_SIZE (1024 * 1024)
#define  MAX_DATA_BUFFER_SIZE (5 * 1024 * 1024)

inline FILE *Hvfopen(const char *path, const char *mode)
{
    FILE *fp = fopen(path, mode);
    static int nTotalError = 0;
    if (fp)
    {
        SetStatLedOff();
        SetHddLedOn();
        nTotalError = 0;
    }
    else
    {
        if (strstr(mode, "w"))
        {
            SetStatLedOn();
            HV_Trace(5, "open file %s error, error string = %s\n", path, strerror(errno));
            //连续出错10次以上，怀疑硬盘有问题，需要分析是否要复位系统还是检测硬盘
            if (++nTotalError > 10)
            {
                //TODOxxxxx
                //HV_System("dm 4 0 /Result_disk");
                HV_Trace(5, "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhh  hard disk error\n");
            }
        }
    }
    return fp;
}

inline int Hvfclose(FILE *fp)
{
    int ret = 0;
    if (fp)
    {
        SetHddLedOff();
        ret = fclose(fp);
        if (ret)
        {
            HV_Trace(5, "close file error, error string = %s\n", strerror(errno));
        }
    }
    return ret;
}

//分割字符
static std::vector<std::string> SplitString(const char* szText, const char* szDelimiter)
{
    std::vector<std::string> list;

    const int iTextLen = strlen(szText);
    const int iDelemiterLen = strlen(szDelimiter);
    if (NULL == szText || iTextLen < 1 || NULL == szDelimiter || iDelemiterLen < 1)
    {
        return list;
    }

    char* pBuf = new char[iTextLen + 1];
    memset(pBuf, 0, iTextLen + 1);
    strcpy(pBuf, szText);
    char* pLast = pBuf;
    char* pTmp = strstr(pBuf, szDelimiter);
    while (pTmp)
    {
        *pTmp = 0;
        list.push_back(std::string(pLast));
        pLast = pTmp + iDelemiterLen;
        pTmp = strstr(pLast, szDelimiter);
    }
    if (pLast && *pLast)
    {
        list.push_back(std::string(pLast));
    }

    delete[] pBuf;
    pBuf = NULL;

    return list;
}

//获取指定目录下的目录列表
static std::vector<std::string> GetFolderList(const char* szPath)
{
    std::vector<std::string> list;
    if (NULL == szPath || strlen(szPath) >= MAX_PATH-3 || 0 != access(szPath, 0))
    {
        return list;
    }

    /*
    char szText[8 * 1024] = {0};
    sprintf(szText, "ls -F %s | grep /", szPath);
    FILE *pFile = popen(szText, "r");

    memset(szText, 0, sizeof(szText));
    if (pFile)
    {
        fread(szText, 1, sizeof(szText), pFile);
        pclose(pFile);
        pFile = NULL;

        list = SplitString(szText, "/\n");
    }*/
    DIR* pDir = opendir(szPath);
    if (pDir)
    {
        struct dirent* d_ent = NULL;
        struct stat sb;
        char szFullPath[MAX_PATH] = {0};
        while ( (d_ent = readdir(pDir)) )
        {
            if (strncmp(d_ent->d_name, ".", 1) == 0 || strncmp(d_ent ->d_name, "" ,1) == 0)
            {
                continue;
            }

            memset(szFullPath, 0, sizeof(szFullPath));
            if (szPath[strlen(szPath)-1] == '/')
            {
                snprintf(szFullPath, sizeof(szFullPath)-1, "%s%s", szPath, d_ent->d_name);
            }
            else
            {
                snprintf(szFullPath, sizeof(szFullPath)-1, "%s/%s", szPath, d_ent->d_name);
            }

            if (0 == stat(szFullPath, &sb))
            {
                if (S_ISDIR(sb.st_mode))
                {
                    list.push_back(d_ent->d_name);
                }
            }
        }
        closedir(pDir);
        pDir = NULL;
    }

    return list;
}

//------------------------------------------------------------

CSafeSaverDm6467Impl::CSafeSaverDm6467Impl()
{
    CreateSemaphore(&m_hSemQueCount, 0, RESULT_LIST_NUM);
    CreateSemaphore(&m_hSemQueCtrl, 1, 1);
    //创建信号量
    if ( 0 != CreateSemaphore(&m_hSemRead,1,1) )
    {
        HV_Trace(5, "<CSafeSaverDm6467Impl> m_hSemRead Create Failed!\n");
    }

    m_dwDiskCount = 6; //default

    m_fInitialized = false;
    m_fSaveRecord = false;
    m_fSaveVideo = false;

    // zhaopy
    m_dwRecordDiskBase = 0;
    m_dwRecordDiskCount = 2;
    m_iRecordCurDisk = -1;
    m_iRecordLastIndex = -1;
    memset(&m_rtLastRecord, 0, sizeof(REAL_TIME_STRUCT));

    m_dwVideoDiskBase = 2;
    m_dwVideoDiskCount = 2;
    m_iVideoCurDisk = -1;
    memset(&m_rtLastVideo, 0, sizeof(REAL_TIME_STRUCT));

    m_fCoverCtrlMode = false;

    m_iResultFreeSpacePerDisk = RECORD_FREEDISK_PER;
    m_iVideoFreeSpacePerDisk = VIDEO_FREEDISK_PER;

}

CSafeSaverDm6467Impl::~CSafeSaverDm6467Impl()
{
    DestroySemaphore(&m_hSemRead);
}

HRESULT CSafeSaverDm6467Impl::Init(const SSafeSaveParam* pParam)
{
    //黄国超增加，先判断当前硬盘的空间，500G时分4个区，1T时分1T
    FILE * fp = popen("fdisk -l|grep Linux|grep -v grep|wc -l", "r");
    int nVideoDisk = 2;
    if (fp)
    {
        char szCnt[255];
        fgets(szCnt, sizeof(szCnt), fp);
        fclose(fp);
        if (!strstr(szCnt, "7"))
        {
            m_dwDiskCount = 4;
            nVideoDisk = 2;
        }
        else
        {
            nVideoDisk = 4;
        }
        HV_Trace(5, "==================fdisk -l|grep Linux|grep -v grep|wc -l = %s, nVideoDisk = %d==================\n", szCnt, nVideoDisk);
    }
    else
    {
        HV_Trace(5, "==========fdisk -l|grep Linux|grep -v grep|wc -l error.==========\n");
    }
    // zhaopy 保存参数还不可以配置，默认结果和录像都打开.
    m_dwRecordDiskBase = 0;
    m_dwRecordDiskCount = nVideoDisk;
    m_iRecordCurDisk = -1;
    m_iRecordLastIndex = -1;
    memset(&m_rtLastRecord, 0, sizeof(REAL_TIME_STRUCT));

    m_dwVideoDiskBase = nVideoDisk;
    m_dwVideoDiskCount = 2;
    m_iVideoCurDisk = -1;
    memset(&m_rtLastVideo, 0, sizeof(REAL_TIME_STRUCT));

    m_fSaveRecord = true;
    m_fSaveVideo = true;
    m_hrDiskStatus = S_OK;
    m_fInitialized = true;

    //如果有初始化参数
    if (pParam && pParam->fCoverCtrl)
    {
        m_fCoverCtrlMode = true;
        m_fSaveVideo = false;
        if ( !m_fSaveVideo && m_fSaveRecord )
        {
            m_dwRecordDiskBase	    = 0;		            //用于保存记录的磁盘开始编号
            m_dwVideoDiskBase		= 0;		            //用于硬盘录像的磁盘开始编号
            m_dwRecordDiskCount	    = m_dwDiskCount;	    //保存记录的磁盘数
            m_dwVideoDiskCount		= 0;		            //保存录像的磁盘数
        }
        else if ( m_fSaveVideo && !m_fSaveRecord )
        {
            m_dwRecordDiskBase	    = 0;
            m_dwVideoDiskBase		= 0;
            m_dwRecordDiskCount	    = 0;
            m_dwVideoDiskCount		= m_dwDiskCount;
        }
        else if ( m_fSaveVideo && m_fSaveRecord )
        {
            m_dwRecordDiskBase	    = 0;
            m_dwVideoDiskBase		= m_dwDiskCount / 2;
            m_dwRecordDiskCount	    = m_dwDiskCount / 2;
            m_dwVideoDiskCount		= m_dwDiskCount / 2;
        }
    }

    Start(NULL);
    return S_OK;
}

int CSafeSaverDm6467Impl::GetActiveDisk(int iDiskBase, int iDiskCount, bool bVideo)
{
    int iRet = -1;

    DISK_SAVE_INFO cInfo;
    cInfo.iIsActiveDisk = 0;

    char szFilePath[MAX_PATH] = {0};
    sprintf(
        szFilePath,
        "%s/%s",
        LOG_DIR, bVideo ? DISK_VIDEO_FILENAME : DISK_RESULT_FILENAME
    );

    FILE* pfile = Hvfopen(szFilePath, "r");
    char szValue[16];
    if ( pfile != NULL )
    {
        fgets(szValue, 16, pfile);
        Hvfclose(pfile);
        pfile = NULL;
        return atoi(szValue);
    }

    return iRet;
}

bool CSafeSaverDm6467Impl::SaveActiveDisk(int iDiskBase, int iDiskCount, int iCurDisk,bool bVideo)
{
    bool fRet = true;

    DISK_SAVE_INFO cInfo;
    cInfo.iIsActiveDisk = 0;

    char szFilePath[MAX_PATH] = {0};
    sprintf(
        szFilePath,
        "%s/%s",
        LOG_DIR, bVideo ? DISK_VIDEO_FILENAME : DISK_RESULT_FILENAME
    );
    cInfo.iIsActiveDisk = iCurDisk;

    FILE* pfile = Hvfopen(szFilePath, "w");
    if ( pfile != NULL )
    {
        char szValue[16];
        sprintf(szValue, "%d\n", iCurDisk);
        fputs(szValue, pfile);
        Hvfclose(pfile);
        pfile = NULL;
    }

    return fRet;
}

int CSafeSaverDm6467Impl::FindHourPath(REAL_TIME_STRUCT* pRealtime, int iDiskBase, int iDiskCount)
{
    int iRet = -1;

    char szPrePath[MAX_PATH] = {0};

    for (int i = iDiskBase; i < (iDiskBase + iDiskCount); ++i)
    {
        sprintf(
            szPrePath,
            "%s/%d/%04d%02d%02d/%02d/",
            ROOT_DIR, i,
            pRealtime->wYear, pRealtime->wMonth, pRealtime->wDay,
            pRealtime->wHour
        );
        if ( IsDirectoryExist(szPrePath) )
        {
            iRet = i;
            break;
        }
    }

    return iRet;
}

int CSafeSaverDm6467Impl::FindDiskNumber(REAL_TIME_STRUCT* pRealtime, int iDiskBase, int iDiskCount, int iCurDisk, int iMinFreeDiskPer, bool bVideo)
{
    int iRet = -1;
    if ( pRealtime == NULL )
    {
        return iRet;
    }
    // 目录是否已经存在
    iRet = FindHourPath(pRealtime, iDiskBase, iDiskCount);
    if ( iRet != -1 )
    {
        HV_Trace(5, "FindDiskNumber ok. SaveActiveDisk=%d\n", iRet);
        if ( iRet != iCurDisk )
        {
            SaveActiveDisk(iDiskBase, iDiskCount, iRet, bVideo);
        }
        return iRet;
    }
    //
    if ( iCurDisk >= 0 )
    {
        iRet = iCurDisk;
        if ((GetDiskUsed(iCurDisk) < iMinFreeDiskPer
                || !IsEnoughSpaceToSaveOneHourData(iCurDisk, bVideo))
                && ChangeNextDiskEx(iDiskBase, iDiskCount, &iCurDisk))
        {
            iRet = iCurDisk;
            SaveActiveDisk(iDiskBase, iDiskCount, iCurDisk, bVideo);
        }
    }
    else
    {
        iRet = GetActiveDisk(iDiskBase, iDiskCount, bVideo);
        HV_Trace(5, "<saver> GetActiveDisk %d\n", iRet);
        // 第一次
        if ( iRet == -1 )
        {
            if ( ChangeNextDiskEx(iDiskBase, iDiskCount, &iCurDisk) )
            {
                iRet = iCurDisk;
                HV_Trace(5, "FindDiskNumber ok. SaveActiveDisk=%d\n", iRet);
                SaveActiveDisk(iDiskBase, iDiskCount, iCurDisk, bVideo);
            }
        }
    }

    return iRet;
}

int CSafeSaverDm6467Impl::GetFileCount(int iCurDisk, REAL_TIME_STRUCT* pRealtime)
{
    int iRet = 0;

    char szFileName[MAX_PATH] = {0};
    sprintf(
        szFileName,
        "%s/%d/%04d%02d%02d/%02d/%s",
        ROOT_DIR, iCurDisk,
        pRealtime->wYear, pRealtime->wMonth, pRealtime->wDay,
        pRealtime->wHour, FILE_COUNT_FILENAME
    );

    PATH_RECORD_COUNT cRecordCount;
    cRecordCount.iRecordCount = 0;
    FILE* pfile = Hvfopen(szFileName, "rb");
    if ( pfile != NULL )
    {
        fread(&cRecordCount, 1, sizeof(PATH_RECORD_COUNT), pfile);
        iRet = cRecordCount.iRecordCount;
        Hvfclose(pfile);
        pfile = NULL;
    }

    return iRet;
}

bool CSafeSaverDm6467Impl::SaveFileCount(int iCurDisk, REAL_TIME_STRUCT* pRealtime, int iRecordCount)
{
    bool fRet = true;

    char szFileName[MAX_PATH] = {0};
    sprintf(
        szFileName,
        "%s/%d/%04d%02d%02d/%02d/%s",
        ROOT_DIR, iCurDisk,
        pRealtime->wYear, pRealtime->wMonth, pRealtime->wDay,
        pRealtime->wHour, FILE_COUNT_FILENAME
    );

    PATH_RECORD_COUNT cRecordCount;
    cRecordCount.iRecordCount = iRecordCount;
    FILE* pfile = Hvfopen(szFileName, "wb");
    if ( pfile != NULL )
    {
        fwrite(&cRecordCount, 1, sizeof(PATH_RECORD_COUNT), pfile);
        Hvfclose(pfile);
        pfile = NULL;
    }
    else
    {
        fRet = false;
    }

    return fRet;
}

bool CSafeSaverDm6467Impl::IsSameHour(REAL_TIME_STRUCT* prtLeft, REAL_TIME_STRUCT* prtRight)
{
    if ( prtLeft == NULL || prtRight == NULL )
    {
        return false;
    }

    if ( prtLeft->wYear == prtRight->wYear
            && prtLeft->wMonth == prtRight->wMonth
            && prtLeft->wDay == prtRight->wDay
            && prtLeft->wHour == prtRight->wHour )
    {
        return true;
    }

    return false;
}

HRESULT CSafeSaverDm6467Impl::Add(int iType,
                                  DWORD32 dwTimeLow,
                                  DWORD32 dwTimeHigh,
                                  int iIndex,
                                  const CAMERA_INFO_HEADER *cihHeader,
                                  const unsigned char* pbInfo,
                                  const unsigned char* pbData,
                                  const char* szDevIno)
{
    HRESULT hr = E_FAIL;
    CAutoLock alock(&m_hSemQueCtrl);
    if (m_rsUseList.GetSize() < RESULT_LIST_NUM)
    {
        RESULTINFO * rs = NULL;
        if (m_rsUnuseList.GetSize() > 0)
        {
            rs = m_rsUnuseList.RemoveHead();
        }
        else
        {
            rs = new RESULTINFO;
            memset(rs, 0, sizeof(RESULTINFO));
        }
        if (rs->iInfoSize < (int)cihHeader->dwInfoSize)
        {
            rs->iInfoSize = MAX_INFO_BUFFER_SIZE;
            HV_Trace(5, "[SafeSaverImpl_DM6467]Info Size = %d\n", rs->iInfoSize);
            if (rs->pbInfo)
            {
                delete []rs->pbInfo;
            }
            rs->pbInfo = new unsigned char[rs->iInfoSize];
        }
        if (rs->iDataSize < (int)cihHeader->dwDataSize)
        {
            rs->iDataSize = MAX_DATA_BUFFER_SIZE;
            HV_Trace(5, "[SafeSaverImpl_DM6467]Data Size = %d\n", rs->iDataSize);
            if (rs->pbData)
            {
                delete []rs->pbData;
            }
            rs->pbData = new unsigned char[rs->iDataSize];
        }
        if (!rs->pbInfo || !rs->pbData)
        {
            if (rs->pbInfo)
            {
                delete []rs->pbInfo;
            }
            if (rs->pbData)
            {
                delete []rs->pbData;
            }
            delete rs;
            rs = NULL;
            hr = E_OUTOFMEMORY;
            HV_Trace(5, "not enough memory to queue result or video data, lost data.\n");
        }
        if (rs)
        {
            rs->iType = iType;
            rs->dwTimeLow = dwTimeLow;
            rs->dwTimeHigh = dwTimeHigh;
            rs->iIndex = iIndex;
            memcpy(&rs->cihHeader, cihHeader, sizeof(CAMERA_INFO_HEADER));
            if (rs->cihHeader.dwInfoSize > 0)
            {
                memcpy(rs->pbInfo, pbInfo, rs->cihHeader.dwInfoSize);
            }
            if (rs->cihHeader.dwDataSize > 0)
            {
                memcpy(rs->pbData, pbData, rs->cihHeader.dwDataSize);
            }
            if (szDevIno)
            {
                strncpy(rs->szDevIno, szDevIno, sizeof(rs->szDevIno));
            }
            else
            {
                rs->szDevIno[0] = '\0';
            }
            m_rsUseList.AddTail(rs);
            SemPost(&m_hSemQueCount);
            hr = S_OK;
        }
    }
    else
    {
        HV_Trace(5, "lost result because of the result list fulled\n");
    }
    return hr;
}

HRESULT CSafeSaverDm6467Impl::Run(void* pvParam)
{
    while (!m_fExit)
    {
        if (S_OK == SemPend(&m_hSemQueCount, 1000))
        {
            SemPend(&m_hSemQueCtrl);
            RESULTINFO * rs = m_rsUseList.RemoveHead();
            SemPost(&m_hSemQueCtrl);

            //识别结果
            if (rs->iType == 0)
            {
                SavePlateRecordInThread(rs->dwTimeLow,
                                        rs->dwTimeHigh,
                                        &rs->iIndex,
                                        &rs->cihHeader,
                                        rs->pbInfo,
                                        rs->pbData,
                                        rs->szDevIno);
            }
            //视频数据
            else if (rs->iType == 1)
            {
                SaveVideoRecordInThread(rs->dwTimeLow,
                                        rs->dwTimeHigh,
                                        &rs->cihHeader,
                                        rs->pbInfo,
                                        rs->pbData);
            }
            SemPend(&m_hSemQueCtrl);
            m_rsUnuseList.AddTail(rs);
            SemPost(&m_hSemQueCtrl);
        }
    }
    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::SavePlateRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
    const CAMERA_INFO_HEADER* pInfoHeader,
    const unsigned char* pbInfo,
    const unsigned char* pbData,
    const char* szDevIno  //设备信息(编号或者IP)
)
{
    return Add(0, dwTimeLow, dwTimeHigh, piIndex ? *piIndex : 0, pInfoHeader, pbInfo, pbData, szDevIno);
}

HRESULT CSafeSaverDm6467Impl::SavePlateRecordInThread(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
    const CAMERA_INFO_HEADER* pInfoHeader,
    const unsigned char* pbInfo,
    const unsigned char* pbData,
    const char* szDevIno  //设备信息(编号或者IP)
)
{
    CAutoLock autoLock(&m_hSemRead);
    int nStartTick = GetSystemTick();
    int nowCarID = piIndex ? ( *piIndex ) : 0;
    if ((pInfoHeader == NULL) || (pbInfo == NULL) || (pbData == NULL))
    {
        return E_POINTER;
    }

    //开始保存一个结果记录
    char szRecordFile[MAX_PATH] = {0};
    // 查找要保存的磁盘序号及编号
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    if (!IsSameHour(&realtime, &m_rtLastRecord) || !nowCarID || (0 > m_iRecordLastIndex))
    {
        if (m_fCoverCtrlMode)//连接多台一体机的情况下，每台设备都会要求换分区，只能允许第一台格式化下一个分区，
        {
            m_rtsChangeDisk = realtime;
        }
        SaveDirectoryMaxOccupySpace(&m_rtLastRecord, m_iRecordCurDisk, false);

        HV_Trace(5, "result disk free space setting is %d%%\n", m_iResultFreeSpacePerDisk);
        m_iRecordCurDisk = FindDiskNumber(
                               &realtime, m_dwRecordDiskBase, m_dwRecordDiskCount,
                               m_iRecordCurDisk, m_iResultFreeSpacePerDisk, false);
        if ( m_iRecordCurDisk < 0 )
        {
            HV_Trace(5, "<Disk saver>None disk can save!\n");
            return E_FAIL;
        }
        if (m_iRecordLastIndex < 0)
        {
            m_iRecordLastIndex = GetFileCount(m_iRecordCurDisk, &realtime);
            nowCarID = m_iRecordLastIndex;
        }
        sprintf(szRecordFile,
                "%s/%d/%04d%02d%02d/%02d/",
                ROOT_DIR,
                m_iRecordCurDisk,
                realtime.wYear, realtime.wMonth, realtime.wDay,
                realtime.wHour);
        MakeSureDirectoryPathExists(szRecordFile);
    }

    //默认
    if (NULL == szDevIno || strlen(szDevIno) <= 0)
    {
        sprintf(szRecordFile,
                "%s/%d/%04d%02d%02d/%02d/%05d.dat",
                ROOT_DIR,
                m_iRecordCurDisk,
                realtime.wYear, realtime.wMonth, realtime.wDay,
                realtime.wHour,
                nowCarID);
    }
    else
    {
        sprintf(szRecordFile,
                "%s/%d/%04d%02d%02d/%02d/%s/%05d.dat",
                ROOT_DIR,
                m_iRecordCurDisk,
                realtime.wYear, realtime.wMonth, realtime.wDay,
                realtime.wHour,
                szDevIno,
                nowCarID);
    }
    FILE* pfile = Hvfopen(szRecordFile, "wb");
    if ( pfile == NULL )
    {
        HV_Trace(5, "<Disk saver>Open %s failed!\n", szRecordFile);
        return E_FAIL;
    }

    if ( fwrite(pInfoHeader, 1, sizeof(CAMERA_INFO_HEADER), pfile) != sizeof(CAMERA_INFO_HEADER)
            || fwrite(pbInfo, 1, pInfoHeader->dwInfoSize, pfile) != pInfoHeader->dwInfoSize
            || fwrite(pbData, 1, pInfoHeader->dwDataSize, pfile) != pInfoHeader->dwDataSize)
    {
        Hvfclose(pfile);
        HV_Trace(5, "<Disk saver>Write %s failed!,error code = %s\n", szRecordFile, strerror(errno));
        return E_FAIL;
    }
    else
    {
        HV_Trace(5, "<Disk saver> write a record to HardDisk [%s], escape %d ms\n", szRecordFile, GetSystemTick() - nStartTick);
    }
    Hvfclose(pfile);
    pfile = NULL;
    SaveFileCount(m_iRecordCurDisk, &realtime, nowCarID+1);
    m_rtLastRecord = realtime;
    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::SavePlateRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
    CCameraDataReference* pRefData //数据引用
)
{
    return E_NOTIMPL;
}


HRESULT CSafeSaverDm6467Impl::GetPlateRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int& index,
    CAMERA_INFO_HEADER* pInfoHeader,
    unsigned char* pbInfo, const int iInfoLen,
    unsigned char* pbData, const int iDataLen,
    const char* szDevIno,  //设备信息(编号或者IP)
    int & nLastDiskID,
    const DWORD32 dwDataInfo    //指定某种数据信息类型，比如违章
)
{
    CAutoLock autoLock(&m_hSemRead);
    if ( pInfoHeader == NULL || pbInfo == NULL || pbData == NULL )
    {
        return E_POINTER;
    }

    FILE* pfile = NULL;
    char szRecordFile[MAX_PATH];
    HV_memset(szRecordFile, 0, MAX_PATH);
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);

    int iDisk = -1;

    //默认
    if (false == m_fCoverCtrlMode)
    {
        for ( int i = m_dwRecordDiskBase; i < (int)(m_dwRecordDiskBase + m_dwRecordDiskCount); ++i )
        {
            if (index < 0)
            {
                int iRecordIndex = GetFileCount(i, &realtime);
                if (iRecordIndex > 0)
                {
                    index = iRecordIndex;
                }
            }
            sprintf(szRecordFile, "%s/%d/%04d%02d%02d/%02d/%05d.dat",
                    ROOT_DIR,
                    i,
                    realtime.wYear, realtime.wMonth, realtime.wDay,
                    realtime.wHour,
                    index);

            if (!access(szRecordFile, 0))//先判断文件是否存在
            {
                pfile = Hvfopen(szRecordFile, "rb");  //尝试打开记录文件
                if ( pfile != NULL )
                {
                    nLastDiskID = iDisk;
                    iDisk = i;
                    break;
                }
            }
        }
    }
    else
    {
        std::vector<std::string> list;
        if (szDevIno && strlen(szDevIno) > 0)//指定设备列表
        {
            list = SplitString(szDevIno, ",");
        }
        bool fGetAll = list.empty();
        for ( int i = m_dwRecordDiskBase; i < (int)(m_dwRecordDiskBase + m_dwRecordDiskCount); ++i )
        {
            if (fGetAll)//获取小时目录下的设备目录
            {
                char szHourDir[256] = {0};
                sprintf(szHourDir, "%s/%d/%04d%02d%02d/%02d/",
                        ROOT_DIR,
                        i,
                        realtime.wYear, realtime.wMonth, realtime.wDay,
                        realtime.wHour
                       );
                list = GetFolderList(szHourDir);
            }
            if (list.empty())
            {
                continue;
            }

            std::vector<std::string>::const_iterator iter = list.begin();
            for (; iter!=list.end(); ++iter)
            {
                if (index < 0)
                {
                    int iRecordIndex = GetFileCount(i, &realtime);
                    if (iRecordIndex > 0)
                    {
                        index = iRecordIndex;
                    }
                }
                sprintf(szRecordFile, "%s/%d/%04d%02d%02d/%02d/%s/%05d.dat",
                        ROOT_DIR,
                        i,
                        realtime.wYear, realtime.wMonth, realtime.wDay,
                        realtime.wHour,
                        iter->c_str(),
                        index);

                if (0 == access(szRecordFile, 0))//先判断文件是否存在
                {
                    pfile = Hvfopen(szRecordFile, "rb");  //尝试打开记录文件
                    if ( pfile != NULL )
                    {
                        nLastDiskID = iDisk;
                        iDisk = i;
                        break;
                    }
                }
            }
            if ( iDisk != -1 )
            {
                break;
            }
        }
    }

    //如果找不到该条记录，则返回：E_RECORD_NONE
    if ( iDisk == -1 )
    {
        char szFileNameTmp[256];
        sprintf(szFileNameTmp, "%04d%02d%02d/%02d/%05d.dat",
                realtime.wYear, realtime.wMonth, realtime.wDay,
                realtime.wHour,
                index);
        HV_Trace(5, "<Disk saver> None file %s!\n", szFileNameTmp);
        return nLastDiskID >= 0 && GetFileCount(nLastDiskID, &realtime) > index ? E_FAIL : E_RECORD_NONE;
    }

    //将记录数据全部读出
    int iReadHeaderLen = fread(pInfoHeader, 1, sizeof(CAMERA_INFO_HEADER), pfile);
    if ( iReadHeaderLen != sizeof(CAMERA_INFO_HEADER) )
    {
        HV_Trace(5, "<Disk saver> Read header failed %s!\n", szRecordFile);
        Hvfclose(pfile);
        return E_FAIL;
    }
    if ( pInfoHeader->dwInfoSize > (DWORD32)iInfoLen || pInfoHeader->dwDataSize > (DWORD32)iDataLen )
    {
        HV_Trace(5, "<Disk saver> Out size info = %d, data = %d!\n", pInfoHeader->dwInfoSize, pInfoHeader->dwDataSize);
        Hvfclose(pfile);
        return E_FAIL;
    }
    if ( pInfoHeader->dwInfoSize != fread(pbInfo, 1, pInfoHeader->dwInfoSize, pfile)
            || pInfoHeader->dwDataSize != fread(pbData, 1, pInfoHeader->dwDataSize, pfile) )
    {
        HV_Trace(5, "<Disk saver> Read info and data failed %s!\n", pInfoHeader->dwInfoSize, pInfoHeader->dwDataSize, szRecordFile);
        Hvfclose(pfile);
        return E_FAIL;
    }
    Hvfclose(pfile);

    return S_OK;
}

//-------------------video----------------------
HRESULT CSafeSaverDm6467Impl::SaveVideoRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
    const CAMERA_INFO_HEADER* pInfoHeader,
    const unsigned char* pbInfo,
    const unsigned char* pbData
)
{
    return Add(1, dwTimeLow, dwTimeHigh, 0, pInfoHeader, pbInfo, pbData, NULL);
}

HRESULT CSafeSaverDm6467Impl::SaveVideoRecordInThread(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
    const CAMERA_INFO_HEADER* pInfoHeader,
    const unsigned char* pbInfo,
    const unsigned char* pbData
)
{
    CAutoLock autoLock(&m_hSemRead);
    int nStartTick = GetSystemTick();
    if ( pInfoHeader == NULL || pbInfo == NULL || pbData == NULL )
    {
        return E_POINTER;
    }
    //开始保存一个结果记录
    char szVideoFile[MAX_PATH] = {0};
    // 查找要保存的磁盘序号及编号
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    if ( !IsSameHour(&realtime, &m_rtLastVideo) )
    {
        SaveDirectoryMaxOccupySpace(&m_rtLastVideo, m_iVideoCurDisk, true);

        HV_Trace(5, "video disk free space setting is %d%%\n", m_iVideoFreeSpacePerDisk);
        m_iVideoCurDisk = FindDiskNumber(
                              &realtime, m_dwVideoDiskBase, m_dwVideoDiskCount,
                              m_iVideoCurDisk, m_iVideoFreeSpacePerDisk, true);
        if ( m_iVideoCurDisk < 0 )
        {
            HV_Trace(5, "<Disk saver>None disk can save!\n");
            return E_FAIL;
        }
        sprintf(
            szVideoFile, "%s/%d/%04d%02d%02d/%02d/",
            ROOT_DIR, m_iVideoCurDisk,
            realtime.wYear, realtime.wMonth, realtime.wDay,
            realtime.wHour);
        //确保路径中的目录都存在
        MakeSureDirectoryPathExists(szVideoFile);
    }
    sprintf(
        szVideoFile, "%s/%d/%04d%02d%02d/%02d/%04d%02d%02d%02d%02d%02d.vdo",
        ROOT_DIR, m_iVideoCurDisk,
        realtime.wYear, realtime.wMonth, realtime.wDay,
        realtime.wHour,
        realtime.wYear, realtime.wMonth, realtime.wDay,
        realtime.wHour, realtime.wMinute, realtime.wSecond);

    FILE* pfile = Hvfopen(szVideoFile, "wb");
    if ( pfile == NULL )
    {
        HV_Trace(5, "<Disk saver>Open %s failed!\n", szVideoFile);
        return E_FAIL;
    }

    if ( fwrite(pInfoHeader, 1, sizeof(CAMERA_INFO_HEADER), pfile) != sizeof(CAMERA_INFO_HEADER)
            || fwrite(pbInfo, 1, pInfoHeader->dwInfoSize, pfile) != pInfoHeader->dwInfoSize
            || fwrite(pbData, 1, pInfoHeader->dwDataSize, pfile) != pInfoHeader->dwDataSize)
    {
        Hvfclose(pfile);
        HV_Trace(5, "<Disk saver>Write %s failed!\n", szVideoFile);
        return E_FAIL;
    }
    else
    {
        HV_Trace(5, "<Disk saver> write a video to HardDisk [%s], escape %d ms\n", szVideoFile, GetSystemTick() - nStartTick);
    }
    Hvfclose(pfile);
    pfile = NULL;

    m_rtLastVideo = realtime;
    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::GetVideoRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
    CAMERA_INFO_HEADER* pInfoHeader,
    unsigned char* pbInfo, const int iInfoLen,
    unsigned char* pbData, const int iDataLen
)
{
    CAutoLock autoLock(&m_hSemRead);

    if ( pInfoHeader == NULL || pbInfo == NULL || pbData == NULL )
    {
        return E_POINTER;
    }

    FILE* pfile = NULL;
    char szVideoFile[MAX_PATH];
    HV_memset(szVideoFile, 0, MAX_PATH);
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);

    int iDisk = -1;
    for ( int i = (int)m_dwVideoDiskBase; i < (int)(m_dwVideoDiskBase + m_dwVideoDiskCount); ++i )
    {
        sprintf(szVideoFile, "%s/%d/%04d%02d%02d/%02d/%04d%02d%02d%02d%02d%02d.vdo",
                ROOT_DIR, i,
                realtime.wYear, realtime.wMonth, realtime.wDay,
                realtime.wHour,
                realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond);
        pfile = Hvfopen(szVideoFile, "rb");  //尝试打开记录文件
        if ( pfile != NULL )
        {
            iDisk = i;
            break;
        }
    }
    //如果找不到该条记录，则返回：E_RECORD_NONE
    if ( iDisk == -1 )
    {
        char szFileNameTmp[256];
        sprintf(szFileNameTmp, "%04d%02d%02d%02d%02d%02d.vdo",
                realtime.wYear, realtime.wMonth, realtime.wDay,
                realtime.wHour, realtime.wMinute, realtime.wSecond);
        HV_Trace(5, "<Disk saver,DiskBase=%d,DiskCount=%d> None file %s!\n", m_dwVideoDiskBase, m_dwVideoDiskCount, szFileNameTmp);
        return E_RECORD_NONE;
    }

    //将记录数据全部读出
    int iReadHeaderLen = fread(pInfoHeader, 1, sizeof(CAMERA_INFO_HEADER), pfile);
    if ( iReadHeaderLen != sizeof(CAMERA_INFO_HEADER) )
    {
        HV_Trace(5, "<Disk saver> Read header failed %s!\n", szVideoFile);
        Hvfclose(pfile);
        return E_FAIL;
    }
    if ( pInfoHeader->dwInfoSize > (DWORD32)iInfoLen || pInfoHeader->dwDataSize > (DWORD32)iDataLen )
    {
        HV_Trace(5, "<Disk saver> Out size info = %d, data = %d!\n", pInfoHeader->dwInfoSize, pInfoHeader->dwDataSize);
        Hvfclose(pfile);
        return E_FAIL;
    }
    if ( pInfoHeader->dwInfoSize != fread(pbInfo, 1, pInfoHeader->dwInfoSize, pfile)
            || pInfoHeader->dwDataSize != fread(pbData, 1, pInfoHeader->dwDataSize, pfile) )
    {
        HV_Trace(5, "<Disk saver> Read info and data failed %s!\n", pInfoHeader->dwInfoSize, pInfoHeader->dwDataSize, szVideoFile);
        Hvfclose(pfile);
        return E_FAIL;
    }
    Hvfclose(pfile);

    return S_OK;
}

//----------------------------------

HRESULT CSafeSaverDm6467Impl::SaveDetectData(CDetectData *pData)
{
    return E_NOTIMPL;
}

HRESULT CSafeSaverDm6467Impl::GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData* pData)
{
    return E_NOTIMPL;
}

HRESULT CSafeSaverDm6467Impl::GetLastCamStatus(DWORD32* pdwStatus)
{
    return E_NOTIMPL;
}

HRESULT CSafeSaverDm6467Impl::GetDiskStatus()
{
    return m_hrDiskStatus;
}

//-------------------------------------------------

HRESULT CSafeSaverDm6467Impl::LockRead()
{
    return (0 == SemPend(&m_hSemRead)) ? S_OK : E_FAIL;
}
HRESULT CSafeSaverDm6467Impl::UnLockRead()
{
    return (0 == SemPost(&m_hSemRead)) ? S_OK : E_FAIL;
}

int CSafeSaverDm6467Impl::GetDiskUsed(int nDiskID)
{
    char szText[128];
    sprintf(szText, "%s/%d", ROOT_DIR, nDiskID);
    struct statfs fs;
    if (!statfs(szText, &fs))
    {
        HV_Trace(5, "GetDiskUsed =  %d%%\n", 100*fs.f_bfree/fs.f_blocks);
        return 100*fs.f_bfree/fs.f_blocks;
    }
    HV_Trace(5, "GetDiskUsed %d error.\n", nDiskID);
    return -1;
}

bool CSafeSaverDm6467Impl::SaveDirectoryMaxOccupySpace(REAL_TIME_STRUCT* pRealtime, int iCurDisk, bool fVideo)
{
    if (iCurDisk < 0)
    {
        return false;
    }

    char szFileName[255];
    sprintf(
        szFileName,
        "%s/%d/%04d%02d%02d/%02d/",
        ROOT_DIR, iCurDisk,
        pRealtime->wYear, pRealtime->wMonth, pRealtime->wDay,
        pRealtime->wHour
    );

    int iDirSize = GetDirSize(szFileName);
    if (iDirSize > 0)
    {
        FILE* pfile = (fVideo ? Hvfopen("/log/vms.dat", "a+") : Hvfopen("/log/rms.dat", "a+"));
        if (pfile)
        {
            fseek(pfile, 0, SEEK_CUR);
            int iSize = 0;
            if (sizeof(iSize) != fread(&iSize, sizeof(iSize), 1, pfile))
            {
                if (iSize < iDirSize)
                {
                    HV_Trace(5, "Save directory size:%d\n", iDirSize);
                    fseek(pfile, 0, SEEK_CUR);
                    fwrite(&iDirSize, sizeof(iDirSize), 1, pfile);
                }
            }
            Hvfclose(pfile);
        }
    }
    else
    {
        HV_Trace(5, "get directory %s size failed.\n", szFileName);
    }

    return true;
}

bool CSafeSaverDm6467Impl::IsEnoughSpaceToSaveOneHourData(int nDiskID, bool fVideo)
{
    char szText[128];
    sprintf(szText, "%s/%d", ROOT_DIR, nDiskID);
    struct statfs fs;
    if (!statfs(szText, &fs))
    {
        FILE* pfile = (fVideo ? Hvfopen("/log/vms.dat", "rb") : Hvfopen("/log/rms.dat", "rb"));
        if (pfile)
        {
            int iSize;
            fread(&iSize, sizeof(iSize), 1, pfile);
            Hvfclose(pfile);
            return (iSize < (int)fs.f_bfree) ? (true) : (false);
        }
        else
        {
            return true;
        }
    }
    return false;
}

bool CSafeSaverDm6467Impl::FormatDisk(int nDiskID)
{
    char szCmd[255];
    sprintf(szCmd, "dm 3 %d %s", nDiskID, ROOT_DIR);
    HV_Trace(5, "Format Disk cmd=%s\n", szCmd);
    //格式化硬盘最大超时时间30分钟
    if (!HV_SystemWithTimeOut(szCmd, 1800000))
    {
        m_hrDiskStatus = S_OK;
        return true;
    }
    return false;
}

bool CSafeSaverDm6467Impl::ChangeNextDiskEx(int iDiskBase, int iDiskCount, int* piCurDisk)
{
    //切换分区
    if ( piCurDisk == NULL )
    {
        return false;
    }

    if ( *piCurDisk == -1 )
    {
        *piCurDisk = iDiskBase;
    }
    else
    {
        ++(*piCurDisk);
        *piCurDisk = ((*piCurDisk - iDiskBase) % (iDiskCount)) + iDiskBase;
    }

    //先判断该分区，是否已经有比当前时间更大的识别结果
    if (m_fCoverCtrlMode)
    {
        char szDisk[32] = {0};
        sprintf(szDisk, "%s/%d/",ROOT_DIR, *piCurDisk);
        std::vector<std::string> list = GetFolderList(szDisk);
        if (list.size())
        {
            char szHourTime[32] = {0};
            sprintf(szHourTime
                    ,"%04d%02d%02d"
                    ,m_rtsChangeDisk.wYear
                    ,m_rtsChangeDisk.wMonth
                    ,m_rtsChangeDisk.wDay
                   );

            std::vector<std::string>::const_iterator iter = list.begin();
            for (; iter!=list.end(); ++iter)
            {
                if (*iter >= std::string(szHourTime))
                {
                    HV_Trace(5, "Disk:%d has been formated before\n",*piCurDisk);
                    return true;
                }
            }
        }
    }

    //先格式化新分区
    if (!FormatDisk(*piCurDisk))
    {
        return false;
    }

    return true;
}

bool CSafeSaverDm6467Impl::IsDirectoryExist(char *pszPath)
{
    struct   stat buf;
    if (!stat(pszPath, &buf) && S_ISDIR(buf.st_mode))
    {
        return true;
    }
    return false;
}

//获取某个小时目录下的结果数量
HRESULT CSafeSaverDm6467Impl::GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh, DWORD32* pdwCount)
{
    CAutoLock autoLock(&m_hSemRead);

    if (NULL == pdwCount)
    {
        return E_FAIL;
    }
    *pdwCount = 0;

    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);

    for ( int i = 0; i < (int)m_dwDiskCount; ++i )
    {
        char szFileName[MAX_PATH] = {0};
        sprintf(szFileName
                ,"%s/%d/%04d%02d%02d/%02d/%s"
                ,ROOT_DIR
                ,i
                ,realtime.wYear
                ,realtime.wMonth
                ,realtime.wDay
                ,realtime.wHour
                ,FILE_COUNT_FILENAME
               );

        if (0 == access(szFileName, 0))
        {
            FILE* pfile = Hvfopen(szFileName, "rb");  //尝试打开记录文件
            if (pfile)
            {
                PATH_RECORD_COUNT cRecordCount;
                fread(&cRecordCount, 1, sizeof(PATH_RECORD_COUNT), pfile);
                *pdwCount = cRecordCount.iRecordCount;
                Hvfclose(pfile);
                pfile = NULL;
            }
            break;
        }
    }

    return S_OK;
}
