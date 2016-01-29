// 该文件编码必须是WINDOWS-936格式
#include "SafeSaverImpl_CF.h"
#include <dirent.h>
#include <sys/statfs.h>
#include "misc.h"
#include <string>
#include "HvUtilsClass.h"

static const DWORD32 MAX_WRITE_BUF_LEN  = 8;                             //最大写缓存
static const DWORD32 RECORD_FILE_SIZE   = 2.5 * 1024 * 1024;               //一个结果文件的最大大小
static const DWORD32 VIDEO_FILE_SIZE    = DWORD32(1.5 * 1024 * 1024);    //一个视频文件的最大大小

// 扩展信息头
#ifndef BLOCK_HEADER
typedef struct tag_block
{
    DWORD32 dwID;
    DWORD32 dwLen;
}
BLOCK_HEADER;
#endif

/* HvUtilsClass.cpp */
extern int g_nHddCheckStatus;

/* ControlFunc.cpp */
extern "C" bool g_fHvReset;

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


inline FILE *Hvfopen(const char *path, const char *mode)
{
    FILE *fp = fopen(path, mode);
    if (fp)
    {
        SetStatLedOff();
        SetHddLedOn();
    }
    else
    {
        if (strstr(mode, "w"))
        {
            SetStatLedOn();
            HV_Trace(5, "open file %s error, error string = %s\n", path, strerror(errno));
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

static bool IsSameHour(REAL_TIME_STRUCT* prtLeft, REAL_TIME_STRUCT* prtRight)
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

static HRESULT CfReadFile(char* szFileName, char* szBuffer, DWORD32 dwBufLen, DWORD32& dwDataLen)
{
    FILE* pFile = Hvfopen(szFileName, "rb");
    if (pFile)
    {
        dwDataLen = fread(szBuffer, 1, dwBufLen, pFile);
        Hvfclose(pFile);
        pFile = NULL;
        return S_OK;
    }
    return E_FAIL;
}

static HRESULT CfWriteFile(char* szFileName, char* szData, DWORD32 dwDataLen)
{
    int iTryTimes = 3;
    while (--iTryTimes >= 0)
    {
        MakeSureDirectoryPathExists(szFileName);
        FILE* pFile = Hvfopen(szFileName, "wb");
        if (pFile)
        {
            DWORD32 dwWritten = fwrite(szData, 1, dwDataLen, pFile);
            Hvfclose(pFile);
            pFile = NULL;
            if (dwWritten == dwDataLen)
            {
                return S_OK;
            }
        }
        HV_Sleep(10);
    }
    return E_FAIL;
}

static HRESULT CfDeleteFile(char* szFileName)
{
    unlink(szFileName);
    return S_OK;
}

static HRESULT CfDeleteFolder(char* szPath)
{
    return S_OK;
}

CSafeSaverDm6467Impl::CSafeSaverDm6467Impl()
{
    //创建信号量
    if ( 0 != CreateSemaphore(&m_hSem,1,1) )
    {
        HV_Trace(5, "<SafeSaver> m_hSem Create Failed!\n");
    }
    if ( 0 != CreateSemaphore(&m_hSemQue,1,1) )
    {
        HV_Trace(5, "<SafeSaver> m_hSemQue Create Failed!\n");
    }
    if ( 0 != CreateSemaphore(&m_hSemQueCount, 0, MAX_WRITE_BUF_LEN))
    {
        HV_Trace(5, "<SafeSaver> m_hSemQueCount Create Failed!\n");
    }
    memset(&m_rtsLastVideoTime, 0, sizeof(m_rtsLastVideoTime));
    m_hrDiskStatus = E_FAIL;
    m_sBuf.addr = NULL;
}

CSafeSaverDm6467Impl::~CSafeSaverDm6467Impl()
{
    DestroySemaphore(&m_hSemQue);
    DestroySemaphore(&m_hSem);
    DestroySemaphore(&m_hSemQueCount);
    if (m_sBuf.addr != NULL)
    {
        delete [] (unsigned char*)m_sBuf.addr;
        m_sBuf.addr = NULL;
    }
}

HRESULT CSafeSaverDm6467Impl::Init(const SSafeSaveParam* pParam)
{
    if (S_OK == m_hrDiskStatus)
    {
        return S_OK;
    }

    if (NULL == pParam)
    {
        HV_Trace(5, "<SafeSaver> pParam invalid!\n");
        return E_FAIL;
    }

    memcpy(&m_sParam, pParam, sizeof(m_sParam));

    Start(NULL);

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
    return E_NOTIMPL;
}

HRESULT CSafeSaverDm6467Impl::SavePlateRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
    CCameraDataReference* pRefData //数据引用
)
{
    if (false == m_sParam.fSaveRecord)
    {
        return E_FAIL;
    }
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }
    if (NULL == pRefData)
    {
        return E_FAIL;
    }
    CCameraData* pCameraData = pRefData->GetItem();
    if (NULL == pCameraData)
    {
        HV_Trace(5, "<SafeSaver>pCameraData is NULL, 保存结果失败 \n");
        return E_FAIL;
    }

    DWORD32 dwInLen = sizeof(CAMERA_INFO_HEADER) + pCameraData->header.dwInfoSize + pCameraData->header.dwDataSize;
    if (dwInLen > RECORD_FILE_SIZE)
    {
        HV_Trace(5, "<SafeSaver>保存结果失败，文件大小超长:%u\n", dwInLen);
        return E_FAIL;
    }

    SemPend(&m_hSemQue);
    //放到队列里
    HRESULT hr = PutData(0
                         ,dwTimeLow
                         ,dwTimeHigh
                         ,dwInLen
                         ,piIndex
                         ,pRefData
                        );
    SemPost(&m_hSemQue);
    SemPost(&m_hSemQueCount);

    return hr;
}


HRESULT CSafeSaverDm6467Impl::GetPlateRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int& index,
    CAMERA_INFO_HEADER* pInfoHeader,
    unsigned char* pbInfo, const int iInfoLen,
    unsigned char* pbData, const int iDataLen,
    const char* szDevIno,  //设备信息(编号或者IP)
    int & nLastDiskID,
    const DWORD32 dwDataInfo
)
{
    if (false == m_sParam.fSaveRecord)
    {
        return E_FAIL;
    }

    SemPend(&m_hSemQue);
    bool fIsEmpty = m_listCfData.empty();
    SemPost(&m_hSemQue);

    if (!fIsEmpty)
    {
        return E_RECORD_WRITING;
    }

    if (0 != SemPend(&m_hSem, 60000))
    {
        HV_Trace(5, "<SafeSaver>结果读取失败 (等待超时)\n");
        return E_RECORD_WAIT_TIMEOUT;
    }
    HRESULT hr = GetData(0
                         ,dwTimeLow
                         ,dwTimeHigh
                         ,dwDataInfo
                         ,index
                         ,pInfoHeader
                         ,pbInfo
                         ,iInfoLen
                         ,pbData
                         ,iDataLen
                         ,szDevIno
                        );
    SemPost(&m_hSem);

    return hr;
}

HRESULT CSafeSaverDm6467Impl::SaveVideoRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
    const CAMERA_INFO_HEADER* pInfoHeader,
    const unsigned char* pbInfo,
    const unsigned char* pbData
)
{
    if (false == m_sParam.fSaveVideo)
    {
        return E_FAIL;
    }
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }
    DWORD32 dwInLen = sizeof(CAMERA_INFO_HEADER) + pInfoHeader->dwInfoSize + pInfoHeader->dwDataSize;
    if (dwInLen > VIDEO_FILE_SIZE)
    {
        HV_Trace(5, "<SafeSaver>保存录像失败，文件大小超长:%u\n", dwInLen);
        return E_FAIL;
    }

    DWORD32 dwMemoryLen = pInfoHeader->dwDataSize;
    if (dwMemoryLen < 1024 * 1024)//优先占用1M的共享内存
    {
        dwMemoryLen = 1024 * 1024;
    }
    IReferenceMemory* pRefMemory = NULL;
    if (S_OK != CreateReferenceMemory(&pRefMemory, dwMemoryLen))
    {
        HV_Trace(5, "<SafeSaver>申请内存空间引用失败\n");
        return E_OUTOFMEMORY;
    }

    CCameraDataReference* pRefData = NULL;
    if (S_OK != CCameraDataReference::CreateCameraDataReference(&pRefData))
    {
        SAFE_RELEASE(pRefMemory);
        HV_Trace(5, "<SafeSaver>创建数据内存引用失败\n");
        return E_OUTOFMEMORY;
    }
    CCameraData* pCameraData = pRefData->GetItem();
    if (NULL == pCameraData)
    {
        SAFE_RELEASE(pRefMemory);
        pRefData->Release();
        return E_OUTOFMEMORY;
    }
    memcpy(&pCameraData->header, pInfoHeader, sizeof(CAMERA_INFO_HEADER));
    pCameraData->header.dwInfoSize = pInfoHeader->dwInfoSize;
    pCameraData->pbInfo = new BYTE8[pInfoHeader->dwInfoSize];
    memcpy(pCameraData->pbInfo, pbInfo, pInfoHeader->dwInfoSize);
    pCameraData->header.dwDataSize = pInfoHeader->dwDataSize;
    pCameraData->pbData = NULL;
    pCameraData->pRefMemory = pRefMemory;
    pCameraData->pRefMemory->AddRef();
    pCameraData->pRefMemory->GetData(&pCameraData->pbData);
    memcpy(pCameraData->pbData, pbData, pInfoHeader->dwDataSize);
    SAFE_RELEASE(pRefMemory);
    SemPend(&m_hSemQue);
    //放到队列里
    HRESULT hr = PutData(1
                         ,dwTimeLow
                         ,dwTimeHigh
                         ,dwInLen
                         ,NULL
                         ,pRefData
                        );
    SemPost(&m_hSemQue);
    SemPost(&m_hSemQueCount);
    SAFE_RELEASE(pRefData);

    return hr;

}

HRESULT CSafeSaverDm6467Impl::GetVideoRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
    CAMERA_INFO_HEADER* pInfoHeader,
    unsigned char* pbInfo, const int iInfoLen,
    unsigned char* pbData, const int iDataLen
)
{
    if (false == m_sParam.fSaveVideo)
    {
        return E_FAIL;
    }

    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    int index = realtime.wMinute * 60 + realtime.wSecond;

    if (0 != SemPend(&m_hSem, 60000))
    {
        HV_Trace(5, "<SafeSaver>录像读取失败 (等待超时)\n");
        return E_RECORD_WAIT_TIMEOUT;
    }
    HRESULT hr = GetData(1
                         ,dwTimeLow
                         ,dwTimeHigh
                         ,0
                         ,index
                         ,pInfoHeader
                         ,pbInfo
                         ,iInfoLen
                         ,pbData
                         ,iDataLen
                         ,NULL
                        );
    SemPost(&m_hSem);

    return hr;
}

//获取某个小时目录下的结果数量
HRESULT CSafeSaverDm6467Impl::GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh, DWORD32* pdwCount)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }
#if FIX_FILE_VER == 1
    if (0 != SemPend(&m_hSem, 2000))
    {
        return E_FAIL;
    }
    *pdwCount = m_cCfRecord.GetHourCount(dwTimeLow, dwTimeHigh);
    SemPost(&m_hSem);
#elif FIX_FILE_VER == 2
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    char szFileName[255];
    sprintf(szFileName, "%04d%02d%02d/%02d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour);
    *pdwCount = m_fixRecord.GetFileCount(szFileName);
#elif FIX_FILE_VER == 3
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    *pdwCount = m_bigFileRecord.GetFileCount(realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour);
#else
    *pdwCount = 0;
#endif
    return S_OK;
}

//查找下一个视频文件
HRESULT CSafeSaverDm6467Impl::FindNextVideo(DWORD32& dwTimeLow, DWORD32& dwTimeHigh)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }
#if FIX_FILE_VER == 1
    if (0 != SemPend(&m_hSem, 2000))
    {
        return E_FAIL;
    }

    SCfItem sCfItem;
    SCfItem sCfItemNext;
    sCfItem.dwTimeLow = dwTimeLow;
    sCfItem.dwTimeHigh = dwTimeHigh;
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    sCfItem.dwIndex = realtime.wMinute * 60 + realtime.wSecond;

    HRESULT hr = m_cCfVideo.GetNextItem(sCfItem, sCfItemNext);
    if (S_OK == hr)
    {
        dwTimeLow = sCfItemNext.dwTimeLow;
        dwTimeHigh = sCfItemNext.dwTimeHigh;

        REAL_TIME_STRUCT realtime;
        ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
        realtime.wMinute = sCfItemNext.dwIndex / 60;
        realtime.wSecond = sCfItemNext.dwIndex % 60;
        ConvertTimeToMs(&realtime, &dwTimeLow, &dwTimeHigh);
    }
    SemPost(&m_hSem);
    return hr;
#else
    TimeMsAddOneSecond(dwTimeLow, dwTimeHigh);
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    char szFileName[255];
    sprintf(szFileName, "/Result_disk/1/file/%04d%02d%02d/%02d/%02d/%02d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond);
    ConvertTimeToMs(&realtime, &dwTimeLow, &dwTimeHigh);
    return !access(szFileName, 0)  ? S_OK : E_FAIL;
#endif
}

HRESULT CSafeSaverDm6467Impl::SaveDetectData(CDetectData *pData)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData *pData)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::GetDiskStatus()
{
    return m_hrDiskStatus ;
}

HRESULT CSafeSaverDm6467Impl::GetLastCamStatus(DWORD32* pdwStatus)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::PutData(const int iDataType //数据类型 0:识别结果 1:录像
                                      ,const DWORD32 dwTimeLow
                                      ,const DWORD32 dwTimeHigh
                                      ,const DWORD32 dwDataLen
                                      ,int* piIndex
                                      ,CCameraDataReference* pRefData
                                     )
{
    if (m_listCfData.size() >= MAX_WRITE_BUF_LEN)
    {
        HV_Trace(5, "<SafeSaver>队列已满,写失败 \n");
        g_cHddOpThread.SetStatusCodeString("异常");
        return E_FAIL;
    }
    g_cHddOpThread.SetStatusCodeString("正常");

    SCfData sCfData;
    sCfData.iDataType = iDataType;
    if (piIndex)
    {
        sCfData.dwIndex = *piIndex;
    }
    sCfData.dwTimeLow = dwTimeLow;
    sCfData.dwTimeHigh = dwTimeHigh;
    sCfData.dwDataLen = dwDataLen;
    sCfData.pRefData = pRefData;
    sCfData.pRefData->AddRef();
    m_listCfData.push_back(sCfData);

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::GetData(const int iDataType //数据类型 0:识别结果 1:录像
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
                                     )
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_NOTIMPL;
    }

    DWORD32 dwDataLen = 0;
    char* pGetData = (char*)m_sBuf.addr;

    DWORD32 dwBeginTime = GetSystemTick();
#if FIX_FILE_VER == 1
    CConstFileSys* pCf = NULL;
    if (0 == iDataType)
    {
        pCf = &m_cCfRecord;
    }
    else
    {
        pCf = &m_cCfVideo;
    }
    //如果小于0，则从该小时的最大记录开始
    if (index < 0)
    {
        if (0 == iDataType)//识别结果
        {
            index = (int)m_cCfRecord.GetHourCount(dwTimeLow, dwTimeHigh);
        }
        else
        {
            GetHourCount(dwTimeLow, dwTimeHigh, index);
        }
    }

    DWORD32 dwIndex = (DWORD32)index;
    HRESULT hr = pCf->ReadFile(dwTimeLow
                               ,dwTimeHigh
                               ,dwDataInfo
                               ,dwIndex
                               ,pGetData
                               ,m_sBuf.len
                               ,dwDataLen
                              );
    index = (int)dwIndex;
    if (S_OK != hr)
    {
        m_hrDiskStatus = E_FAIL;
        return E_FAIL;
    }

    if (0 == dwDataLen)
    {
        return E_RECORD_NONE;
    }
#elif FIX_FILE_VER == 2
    CFixFileStream * file = NULL;
    //根据时间生成文件
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    char szFileName[255];

    if (0 == iDataType)
    {
        file = &m_fixRecord;
        sprintf(szFileName, "%04d%02d%02d/%02d/%d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, index);
    }
    else
    {
        file = &m_fixVideo;
        sprintf(szFileName, "%04d%02d%02d/%02d/%02d/%02d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond);
    }

    if (!file->IsValid())
    {
        return E_NOTIMPL;
    }

    if (!file->Read(szFileName, pGetData, m_sBuf.len))
    {
        if (0 == iDataType)
        {
            sprintf(szFileName, "%04d%02d%02d/%02d/%d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, index + 1);
            if (!access(szFileName, 0))
            {
                return E_FAIL;
            }
        }
        return E_RECORD_NONE;
    }
#elif FIX_FILE_VER == 3
    CBigFile * file = NULL;
    //根据时间生成文件
    REAL_TIME_STRUCT realtime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &realtime);
    if (0 == iDataType)
    {
        file = &m_bigFileRecord;
    }
    else
    {
        file = &m_bigFileVideo;
    }
    if (!file->IsValid())
    {
        return E_NOTIMPL;
    }
    HRESULT hr = file->Read(realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond, index, pGetData, m_sBuf.len);
    if (S_OK != hr)
    {
        return hr;
    }
#endif
    m_dwLastDataTime = GetSystemTick();

    memcpy(pInfoHeader, pGetData, sizeof(CAMERA_INFO_HEADER));

    DWORD32 dwInfoSize = pInfoHeader->dwInfoSize;
    DWORD32 dwDataSize = pInfoHeader->dwDataSize;
    if (iInfoLen < (int)dwInfoSize || iDataLen < (int)dwDataSize)
    {
        HV_Trace(5, "<SafeSaver>读取缓存空间不够 \n");
        return E_FAIL;
    }
#if FIX_FILE_VER == 1
    if (dwDataLen < sizeof(CAMERA_INFO_HEADER) + dwInfoSize + dwDataSize)
    {
        HV_Trace(5, "<SafeSaver>文件(%d)长度异常[%d-->%d]\n", index, dwDataLen, sizeof(CAMERA_INFO_HEADER) + dwInfoSize + dwDataSize);
        return E_FAIL;
    }
#endif
    memcpy(pbInfo, pGetData + sizeof(CAMERA_INFO_HEADER), dwInfoSize);
    memcpy(pbData, pGetData + sizeof(CAMERA_INFO_HEADER) + dwInfoSize, dwDataSize);

    //打印信息
#if FIX_FILE_VER == 1 || FIX_FILE_VER == 3
    char szFileName[MAX_PATH] = {0};
#endif
    FormatTime(szFileName, dwTimeLow, dwTimeHigh);
    char szDataType[16] = {0};
    if (0 == iDataType)
    {
        strcpy(szDataType, "record");
    }
    else
    {
        strcpy(szDataType, "video");
    }

    HV_Trace(5, "<SafeSaver>Read %s : %s %u (%u) %d \n"
             ,szDataType
             ,szFileName
             ,dwDataLen
             ,m_dwLastDataTime - dwBeginTime
             ,index
            );

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::CopyCfData(SCfData& sCfData)
{
    CCameraData* pCameraData = sCfData.pRefData->GetItem();
    DWORD32 dwDataLen = 0;
    memcpy((char*)m_sBuf.addr + dwDataLen, &pCameraData->header, sizeof(pCameraData->header));
    dwDataLen += sizeof(pCameraData->header);
    memcpy((char*)m_sBuf.addr + dwDataLen, pCameraData->pbInfo, pCameraData->header.dwInfoSize);
    dwDataLen += pCameraData->header.dwInfoSize;
    memcpy((char*)m_sBuf.addr + dwDataLen, pCameraData->pbData, pCameraData->header.dwDataSize);
    dwDataLen += pCameraData->header.dwDataSize;

    if (0 == sCfData.iDataType)//识别结果,判断是否违章
    {
        BLOCK_HEADER cBlockHeader;
        char* pAppendInfo = NULL;
        char* pbTemp = (char*)m_sBuf.addr + sizeof(CAMERA_INFO_HEADER);
        //修正数据类型
        DWORD32 dwRecordType = CAMERA_RECORD_HISTORY;
        memcpy(pbTemp+8, &dwRecordType, 4);

        for (int i = 0; i < (int)pCameraData->header.dwInfoSize; )
        {
            memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
            pbTemp += sizeof(BLOCK_HEADER);
            i += sizeof(BLOCK_HEADER);

            if (cBlockHeader.dwID == BLOCK_XML_TYPE)  // 识别结果XML附加信息
            {
                pAppendInfo = pbTemp;
                if (strstr(pAppendInfo, "违章:是"))
                {
                    sCfData.dwDataInfo = 1;
                }
                const char* pCarID = strstr(pAppendInfo, "<CarID");
                if (pCarID)
                {
                    const char* pEnd = strstr(pCarID, ">");
                    if (pEnd)
                    {
                        char szCarID[32] = {0};
#if FIX_FILE_VER == 1
                        DWORD32 dwCarID = m_cCfRecord.GetHourCount(sCfData.dwTimeLow, sCfData.dwTimeHigh);
#else
                        DWORD32 dwCarID;
                        GetHourCount(sCfData.dwTimeLow, sCfData.dwTimeHigh, &dwCarID);
                        sCfData.dwIndex = dwCarID;
#endif
                        sprintf(szCarID, "<CarID value=\"%d\" />", dwCarID);
                        int iPos = pCarID - pAppendInfo;
                        int iNum = pEnd - pCarID + 1;
                        std::string strAppendInfo = pAppendInfo;
                        strAppendInfo.replace(iPos, iNum, std::string(szCarID));
                        memset(pAppendInfo, 0, cBlockHeader.dwLen);
                        strncpy(pAppendInfo, strAppendInfo.c_str(), cBlockHeader.dwLen-1);
                    }
                }
                break;
            }

            pbTemp += cBlockHeader.dwLen;
            i += cBlockHeader.dwLen;
        }
    }
    else
    {
        REAL_TIME_STRUCT rtsTime;
        ConvertMsToTime(sCfData.dwTimeLow, sCfData.dwTimeHigh, &rtsTime);
        sCfData.dwIndex = rtsTime.wMinute * 60 + rtsTime.wSecond;
    }

    return S_OK;
}

HRESULT CSafeSaverDm6467Impl::Run(void *pvParamter)
{
    if (false == m_sParam.fSaveRecord && false == m_sParam.fSaveVideo)
    {
        return E_FAIL;
    }

    while (1 != g_nHddCheckStatus)
    {
        HV_Sleep(500);
    }
    HV_Sleep(500);
#if FIX_FILE_VER == 1
    SConstFileSysParam sCfParamRecord;
    SConstFileSysParam sCfParamVideo;
    sCfParamRecord.fnReadFile = CfReadFile;
    sCfParamRecord.fnWriteFile = CfWriteFile;
    sCfParamRecord.fnDeleteFile = CfDeleteFile;
    sCfParamRecord.fnDeleteFolder = CfDeleteFolder;
    sCfParamVideo.fnReadFile = CfReadFile;
    sCfParamVideo.fnWriteFile = CfWriteFile;
    sCfParamVideo.fnDeleteFile = CfDeleteFile;
    sCfParamVideo.fnDeleteFolder = CfDeleteFolder;
    sCfParamVideo.fHourFix = true;
    sCfParamRecord.fHourInfo = true;
    strcpy(sCfParamRecord.szDataType, "r");
    strcpy(sCfParamVideo.szDataType, "v");
    sCfParamRecord.dwMaxFileSize = RECORD_FILE_SIZE;
    sCfParamVideo.dwMaxFileSize = VIDEO_FILE_SIZE;
#endif
    char szDisk1[32] = {0};
    char szDisk2[32] = {0};
    strcpy(szDisk1, "/Result_disk/0");
    strcpy(szDisk2, "/Result_disk/1");

    bool fHasDisk1 = false;
    bool fHasDisk2 = false;
    if ( m_sParam.fSaveRecord && m_sParam.fSaveVideo )
    {
#if FIX_FILE_VER == 1
        strcpy(sCfParamRecord.szRootDir, szDisk1);
        strcpy(sCfParamVideo.szRootDir, szDisk2);
#endif
        fHasDisk1 = true;
        fHasDisk2 = true;
    }
    else if ( m_sParam.fSaveRecord && !m_sParam.fSaveVideo )
    {
#if FIX_FILE_VER == 1
        strcpy(sCfParamRecord.szRootDir, szDisk1);
#endif
        fHasDisk1 = true;
    }
    else if ( !m_sParam.fSaveRecord && m_sParam.fSaveVideo )
    {
#if FIX_FILE_VER == 1
        strcpy(sCfParamVideo.szRootDir, szDisk1);
#endif
        fHasDisk1 = true;
    }

    strcat(szDisk1, "/");
    strcat(szDisk2, "/");
    while ( (fHasDisk1 && access(szDisk1, 0)) || (fHasDisk2 && access(szDisk2, 0)) )
    {
        HV_Sleep(2000);
    }
    HV_Sleep(2000);

    if (m_sParam.fSaveRecord)
    {
#if FIX_FILE_VER == 1
        struct statfs fs;
        if (!statfs(szDisk1, &fs))
        {
            unsigned long long llTotal = (long long)fs.f_bsize * (long long)fs.f_blocks;
            if (llTotal > 1024 * 1024 * 1024)
            {
                sCfParamRecord.dwMaxFileCount = llTotal / RECORD_FILE_SIZE;
            }
            else
            {
                HV_Trace(5, "<SafeSaver>分区%s容量太小\n", sCfParamRecord.szRootDir);
                m_sParam.fSaveRecord = false;
            }
        }
        else
        {
            HV_Trace(5, "<SafeSaver>获取%s容量失败\n", sCfParamRecord.szRootDir);
            m_sParam.fSaveRecord = false;
        }
#elif FIX_FILE_VER == 2
        if (!m_fixRecord.Initialize(szDisk1, 0, m_sParam.iFileSize*1024))
        {
            m_sParam.fSaveRecord = false;
        }
#elif FIX_FILE_VER == 3
        if (S_OK != m_bigFileRecord.Initialize(szDisk1, 0, m_sParam.iFileSize*1024, true))
        {
            m_sParam.fSaveRecord = false;
        }
#endif
    }
    if (m_sParam.fSaveVideo)
    {
#if FIX_FILE_VER == 1
        struct statfs fs;
        if (!statfs(szDisk2, &fs))
        {
            unsigned long long llTotal = (long long)fs.f_bsize * (long long)fs.f_blocks;
            if (llTotal > 1024 * 1024 * 1024)
            {
                sCfParamVideo.dwMaxFileCount = llTotal / VIDEO_FILE_SIZE;
            }
            else
            {
                HV_Trace(5, "<SafeSaver>分区%s容量太小\n", sCfParamVideo.szRootDir);
                m_sParam.fSaveVideo = false;
            }
        }
        else
        {
            HV_Trace(5, "<SafeSaver>获取%s容量失败\n", sCfParamVideo.szRootDir);
            m_sParam.fSaveVideo = false;
        }
#elif FIX_FILE_VER == 2
        if (!m_fixVideo.Initialize(m_sParam.fSaveRecord  ? szDisk2 : szDisk1, 0, m_sParam.iFileSize*1024))
        {
            m_sParam.fSaveVideo = false;
        }
#elif FIX_FILE_VER == 3
        if (S_OK != m_bigFileVideo.Initialize(m_sParam.fSaveRecord  ? szDisk2 : szDisk1, 0, m_sParam.iFileSize*1024, false))
        {
            m_sParam.fSaveVideo = false;
        }
#endif
    }
#if FIX_FILE_VER == 1
    if (m_sParam.fSaveRecord)
    {
        bool fDone = false;
        if (S_OK != m_cCfRecord.Init(&sCfParamRecord))
        {
            HV_Trace(5, "<SafeSaver>格式化识别结果定长存储系统...\n");
            sCfParamRecord.fFormat = true;
            if (S_OK == m_cCfRecord.Init(&sCfParamRecord))
            {
                fDone = true;
            }
        }
        else
        {
            fDone = true;
        }
        if (fDone)
        {
            HV_Trace(5, "<SafeSaver>识别结果存储初始化成功\n");
        }
        else
        {
            m_sParam.fSaveRecord = false;
            HV_Trace(5, "<SafeSaver>识别结果存储初始化失败\n");
        }
    }
    if (m_sParam.fSaveVideo)
    {
        bool fDone = false;
        if (S_OK != m_cCfVideo.Init(&sCfParamVideo))
        {
            HV_Trace(5, "<SafeSaver>格式化录像定长存储系统...\n");
            sCfParamVideo.fFormat = true;
            if (S_OK == m_cCfVideo.Init(&sCfParamVideo))
            {
                fDone = true;
            }
        }
        else
        {
            fDone = true;
        }
        if (fDone)
        {
            HV_Trace(5, "<SafeSaver>录像存储初始化成功\n");
        }
        else
        {
            m_sParam.fSaveVideo = false;
            HV_Trace(5, "<SafeSaver>录像存储初始化失败\n");
        }
    }
#endif
    //如果都失败了
    if (false == m_sParam.fSaveRecord && false == m_sParam.fSaveVideo)
    {
        HV_Trace(5, "初始化定长存储失败");
        return E_FAIL;
    }

    if (NULL == m_sBuf.addr)
    {
        //这里需要用new来创建内存，防止共享内存不足
        //CreateShareMemOnPool(&m_sBuf, RECORD_FILE_SIZE);
        m_sBuf.addr = new char[RECORD_FILE_SIZE];
        m_sBuf.len = RECORD_FILE_SIZE;
        if (NULL == m_sBuf.addr)
        {
            HV_Trace(5, "<SafeSaver>申请共享内存失败\n");
            return E_FAIL;
        }
    }

    HV_Sleep(1000);
    SemPend(&m_hSem);
    m_hrDiskStatus = S_OK;
    SemPost(&m_hSem);

    bool fVideoNewHour = true;
#if 0
    srand((int)time(0));

    REAL_TIME_STRUCT rt;
    DWORD32 dwL, dwH;
    GetSystemTime(&dwL, &dwH);
#endif
    while (!m_fExit)
    {
        if (S_OK == m_hrDiskStatus)
        {
            m_bigFileRecord.Commit(false, false);
            m_bigFileVideo.Commit(false, false);
        }

        if (g_fHvReset)//系统要求复位了
        {
            SemPend(&m_hSem);
            m_hrDiskStatus = E_FAIL;
            SemPost(&m_hSem);
            break;
        }
        SemPend(&m_hSemQueCount, 500);
        SemPend(&m_hSemQue);
        bool fIsEmpty = m_listCfData.empty();
        SemPost(&m_hSemQue);
        if (fIsEmpty)
        {
#if 0
            TimeMsAddOneSecond(dwL, dwH);
            ConvertMsToTime(dwL, dwH, &rt);
            char szFileName[255];
            sprintf(szFileName, "%04d%02d%02d/%02d/%02d/%02d", rt.wYear, rt.wMonth, rt.wDay, rt.wHour, rt.wMinute, rt.wSecond);
            int s = rand()%(3*RECORD_FILE_SIZE);
            if (s > 0)
            {
                m_fixVideo.Write(szFileName, NULL, s);
            }
#endif
            continue;
        }

        //将队列中的数据写入存储中
        SemPend(&m_hSemQue);
        SCfData sCfData = m_listCfData.front();
        m_listCfData.pop_front();
        SemPost(&m_hSemQue);

        //拷贝和保持数据
        CAutoLock cAutoLock(&m_hSem);
        CopyCfData(sCfData);
        SAFE_RELEASE(sCfData.pRefData);
        if (S_OK != m_hrDiskStatus)
        {
            break;
        }

        bool fForceNewHour = false;
        DWORD32 dwBeginTime = GetSystemTick();
#if FIX_FILE_VER == 1
        CConstFileSys* pCf = NULL;
        if (0 == sCfData.iDataType)
        {
            pCf = &m_cCfRecord;
        }
        else
        {
            pCf = &m_cCfVideo;
            if (false == fVideoNewHour)
            {
                //判断录像如果出现了间隔，则要创建新段
                REAL_TIME_STRUCT rtsVideoTime;
                ConvertMsToTime(sCfData.dwTimeLow, sCfData.dwTimeHigh, &rtsVideoTime);
                if (IsSameHour(&rtsVideoTime, &m_rtsLastVideoTime))
                {
                    int iNow = rtsVideoTime.wMinute * 60 + rtsVideoTime.wSecond;
                    int iLast = m_rtsLastVideoTime.wMinute * 60 + m_rtsLastVideoTime.wSecond;
                    if (iNow - iLast > 5)
                    {
                        fVideoNewHour = true;
                    }
                }
            }
            fForceNewHour = fVideoNewHour;
        }
        HRESULT hr = pCf->WriteFile(sCfData.dwTimeLow
                                    ,sCfData.dwTimeHigh
                                    ,sCfData.dwDataInfo
                                    ,sCfData.dwIndex
                                    ,(char*)m_sBuf.addr
                                    ,sCfData.dwDataLen
                                    ,fForceNewHour
                                   );
        if (S_OK != hr)
        {
            HV_Trace(5, "<SafeSaver>写入文件失败 \n");
            if (E_FAIL == hr)
            {
                m_hrDiskStatus = E_FAIL;
                break;
            }
            continue;
        }
        else
        {
            hr = pCf->Submit(1);
            if (S_OK != hr)
            {
                m_hrDiskStatus = E_FAIL;
                HV_Trace(5, "<SafeSaver>提交索引表失败 \n");
                break;
            }
            if (sCfData.iDataType)
            {
                ConvertMsToTime(sCfData.dwTimeLow, sCfData.dwTimeHigh, &m_rtsLastVideoTime);
                fVideoNewHour = false;
            }
        }
#elif FIX_FILE_VER == 2
        CFixFileStream* file = NULL;
        //根据时间生成文件
        REAL_TIME_STRUCT realtime;
        ConvertMsToTime(sCfData.dwTimeLow, sCfData.dwTimeHigh, &realtime);
        char szFileName[255];
        if (0 == sCfData.iDataType)
        {
            file = &m_fixRecord;
            sprintf(szFileName, "%04d%02d%02d/%02d/%d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, sCfData.dwIndex);
        }
        else
        {
            file = &m_fixVideo;
            sprintf(szFileName, "%04d%02d%02d/%02d/%02d/%02d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond);
        }

        if (!file->Write(szFileName, m_sBuf.addr, sCfData.dwDataLen))
        {
        }
#elif FIX_FILE_VER == 3
        CBigFile* file = NULL;
        //根据时间生成文件
        REAL_TIME_STRUCT realtime;
        ConvertMsToTime(sCfData.dwTimeLow, sCfData.dwTimeHigh, &realtime);
        if (0 == sCfData.iDataType)
        {
            file = &m_bigFileRecord;
        }
        else
        {
            file = &m_bigFileVideo;
        }

        if (S_OK != file->Write(realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond, m_sBuf.addr, sCfData.dwDataLen))
        {
        }
#endif
        m_dwLastDataTime = GetSystemTick();

        //打印信息
#if FIX_FILE_VER == 1 || FIX_FILE_VER == 3
        char szFileName[MAX_PATH] = {0};
#endif
        FormatTime(szFileName, sCfData.dwTimeLow, sCfData.dwTimeHigh);
        char szDataType[16] = {0};
        if (0 == sCfData.iDataType)
        {
            strcpy(szDataType, "record");
        }
        else
        {
            strcpy(szDataType, "video");
        }
        HV_Trace(5, "<SafeSaver>Write %s : %s %u (%u) %d \n"
                 ,szDataType
                 ,szFileName
                 ,sCfData.dwDataLen
                 ,m_dwLastDataTime - dwBeginTime
                 ,sCfData.dwIndex
                );
    }

    HV_Trace(5, "<save>Save thread exit.\n");
    m_bigFileRecord.Commit(true, false);
    m_bigFileVideo.Commit(true, false);

    m_fExit = TRUE;
    return S_OK;
}


