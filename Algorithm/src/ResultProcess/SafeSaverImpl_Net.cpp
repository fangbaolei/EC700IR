// 该文件编码必须是WINDOWS-936格式
#include "SafeSaverImpl_Net.h"
#include "tinyxml.h"
#include "qftp.h"
#include "HvUtilsClass.h"

#include "misc.h"
#include <sys/statfs.h>
#include <sys/mount.h>
#include <dirent.h>

using namespace nsFTP;

#ifdef _CAMERA_PIXEL_500W_
static const DWORD32 RECORD_FILE_SIZE   = DWORD32(2.5 * 1024 * 1024);    //一个结果文件的最大大小
static const DWORD32 MAX_WRITE_BUF_LEN  = 3;                             //最大写缓存
#else
static const DWORD32 RECORD_FILE_SIZE   = 2.5 * 1024 * 1024;               //一个结果文件的最大大小
static const DWORD32 MAX_WRITE_BUF_LEN  = 3;                             //最大写缓存
#endif

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

static CQFTP* g_pFTP = NULL;

/* ControlFunc.cpp */
extern int ResetInitHddParam();

/* HvUtilsClass.cpp */
extern int g_nHddCheckStatus;

/* ControlFunc.cpp */
extern "C" bool g_fHvReset;

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
    if (NULL == g_pFTP)
    {
        return E_FAIL;
    }

    int iRet = g_pFTP->ReadFileFromFtp(szFileName, szBuffer, dwBufLen, dwDataLen);
    if (ERROR_QFTP_RECV_ERROR != iRet)
    {
        return S_OK;
    }

    return E_FAIL;
}

static HRESULT CfWriteFile(char* szFileName, char* szData, DWORD32 dwDataLen)
{
    if (NULL == g_pFTP)
    {
        return E_FAIL;
    }

    int iRet = g_pFTP->WriteFileToFtp(szFileName, szData, dwDataLen);
    if (ERROR_QFTP_SEND_ERROR != iRet)
    {
        return S_OK;
    }

    return E_FAIL;
}

static HRESULT CfDeleteFile(char* szFileName)
{
    if (NULL == g_pFTP)
    {
        return E_FAIL;
    }
    if (ERROR_QFTP_OK != g_pFTP->TestConnectValidity())
    {
        if (ERROR_QFTP_OK != g_pFTP->LoginFtpServer())
        {
            return E_FAIL;
        }
    }
    g_pFTP->DeleteOneFile(szFileName);
    return S_OK;
}

static HRESULT CfDeleteFolder(char* szPath)
{
    if (NULL == g_pFTP)
    {
        return E_FAIL;
    }
    if (ERROR_QFTP_OK != g_pFTP->TestConnectValidity())
    {
        if (ERROR_QFTP_OK != g_pFTP->LoginFtpServer())
        {
            return E_FAIL;
        }
    }
    g_pFTP->DeleteOneDirectory(szPath);
    return S_OK;
}

static HRESULT CfReadFileNfs(char* szFileName, char* szBuffer, DWORD32 dwBufLen, DWORD32& dwDataLen)
{
    FILE* pFile = fopen(szFileName, "rb");
    if (pFile)
    {
        dwDataLen = fread(szBuffer, 1, dwBufLen, pFile);
        fclose(pFile);
        pFile = NULL;
        return S_OK;
    }
    return E_FAIL;
}

static HRESULT CfWriteFileNfs(char* szFileName, char* szData, DWORD32 dwDataLen)
{
    MakeSureDirectoryPathExists(szFileName);
    FILE* pFile = fopen(szFileName, "wb");
    if (pFile)
    {
        DWORD32 dwWritten = fwrite(szData, 1, dwDataLen, pFile);
        fclose(pFile);
        pFile = NULL;
        if (dwWritten == dwDataLen)
        {
            return S_OK;
        }
    }
    return E_FAIL;
}

static HRESULT CfDeleteFileNfs(char* szFileName)
{
    unlink(szFileName);
    return S_OK;
}

static HRESULT CfDeleteFolderNfs(char* szPath)
{
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
                if (S_ISREG(sb.st_mode))
                {
                    unlink(szFullPath);
                }
            }
        }
        closedir(pDir);
        pDir = NULL;
    }
    return S_OK;
}

CSafeSaverNetImpl::CSafeSaverNetImpl()
{
    //创建信号量
    if ( 0 != CreateSemaphore(&m_hSem,1,1) )
    {
        HV_Trace(5, "<SafeSaverNet> m_hSem Create Failed!\n");
    }
    if ( 0 != CreateSemaphore(&m_hSemQue,1,1) )
    {
        HV_Trace(5, "<SafeSaverNet> m_hSemQue Create Failed!\n");
    }
    if ( 0 != CreateSemaphore(&m_hSemQueCount, 0, MAX_WRITE_BUF_LEN))
    {
        HV_Trace(5, "<SafeSaver> m_hSemQueCount Create Failed!\n");
    }
    memset(&m_rtsLastVideoTime, 0, sizeof(m_rtsLastVideoTime));
    m_hrDiskStatus = E_FAIL;
    m_sBuf.addr = NULL;
}

CSafeSaverNetImpl::~CSafeSaverNetImpl()
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

HRESULT CSafeSaverNetImpl::Init(const SSafeSaveParam* pParam)
{
    if (S_OK == m_hrDiskStatus)
    {
        return S_OK;
    }

    if (NULL == pParam || pParam->iNetDiskSpace < 1)
    {
        HV_Trace(5, "<SafeSaverNet> pParam invalid!\n");
        return E_FAIL;
    }

    memcpy(&m_sParam, pParam, sizeof(m_sParam));

    Start(NULL);

    return S_OK;
}

HRESULT CSafeSaverNetImpl::SavePlateRecord(
    DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
    const CAMERA_INFO_HEADER* pInfoHeader,
    const unsigned char* pbInfo,
    const unsigned char* pbData,
    const char* szDevIno  //设备信息(编号或者IP)
)
{
    return E_NOTIMPL;
}

HRESULT CSafeSaverNetImpl::SavePlateRecord(
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
        HV_Trace(5, "<SafeSaverNet>pCameraData is NULL, 保存结果失败 \n");
        return E_FAIL;
    }

    DWORD32 dwInLen = sizeof(CAMERA_INFO_HEADER) + pCameraData->header.dwInfoSize + pCameraData->header.dwDataSize;
    if (dwInLen > RECORD_FILE_SIZE)
    {
        HV_Trace(5, "<SafeSaverNet>保存结果失败，文件大小超长:%u\n", dwInLen);
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


HRESULT CSafeSaverNetImpl::GetPlateRecord(
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

    SemPend(&m_hSem);
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

HRESULT CSafeSaverNetImpl::SaveVideoRecord(
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
    static bool fFirst = true;
    if (fFirst)
    {
        fFirst = false;
        return E_FAIL;
    }
    DWORD32 dwInLen = sizeof(CAMERA_INFO_HEADER) + pInfoHeader->dwInfoSize + pInfoHeader->dwDataSize;
    if (dwInLen > VIDEO_FILE_SIZE)
    {
        HV_Trace(5, "<SafeSaverNet>保存录像失败，文件大小超长:%u\n", dwInLen);
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
        HV_Trace(5, "<SafeSaverNet>申请内存空间引用失败\n");
        return E_OUTOFMEMORY;
    }

    CCameraDataReference* pRefData = NULL;
    if (S_OK != CCameraDataReference::CreateCameraDataReference(&pRefData))
    {
        SAFE_RELEASE(pRefMemory);
        HV_Trace(5, "<SafeSaverNet>创建数据内存引用失败\n");
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

HRESULT CSafeSaverNetImpl::GetVideoRecord(
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

    SemPend(&m_hSem);
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
HRESULT CSafeSaverNetImpl::GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh, DWORD32* pdwCount)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }
#if FIX_FILE_VER == 1
    *pdwCount = m_cCfRecord.GetHourCount(dwTimeLow, dwTimeHigh);
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
HRESULT CSafeSaverNetImpl::FindNextVideo(DWORD32& dwTimeLow, DWORD32& dwTimeHigh)
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
    sprintf(szFileName, "/Result_disk/video/file/%04d%02d%02d/%02d/%02d/%02d", realtime.wYear, realtime.wMonth, realtime.wDay, realtime.wHour, realtime.wMinute, realtime.wSecond);
    ConvertTimeToMs(&realtime, &dwTimeLow, &dwTimeHigh);
    return !access(szFileName, 0)  ? S_OK : E_FAIL;
#endif
}

HRESULT CSafeSaverNetImpl::SaveDetectData(CDetectData *pData)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverNetImpl::GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData *pData)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverNetImpl::GetDiskStatus()
{
    return m_hrDiskStatus ;
}

HRESULT CSafeSaverNetImpl::GetLastCamStatus(DWORD32* pdwStatus)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverNetImpl::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    if (S_OK != m_hrDiskStatus)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSafeSaverNetImpl::PutData(const int iDataType //数据类型 0:识别结果 1:录像
                                   ,const DWORD32 dwTimeLow
                                   ,const DWORD32 dwTimeHigh
                                   ,const DWORD32 dwDataLen
                                   ,int* piIndex
                                   ,CCameraDataReference* pRefData
                                  )
{
    int iTryTimes = 3;
    bool fIsFull = false;
    while(m_listCfData.size() >= MAX_WRITE_BUF_LEN)
    {
       if( iTryTimes-- < 0 )
       {
           fIsFull = true;
           break;
       }
       HV_Sleep(200);
    }

    if (fIsFull)
    {
        HV_Trace(5, "<SafeSaverNet>队列已满,写失败 \n");
        g_cHddOpThread.SetStatusCodeString("异常");
        return E_FAIL;
    }

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
    SemPost(&m_hSemQueCount);
    return S_OK;
}

HRESULT CSafeSaverNetImpl::GetData(const int iDataType //数据类型 0:识别结果 1:录像
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
        //读取文件失败，重连网络硬盘
        ReconnectNFS();
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
        HV_Trace(5, "<SafeSaverNet>读取缓存空间不够 \n");
        return E_FAIL;
    }
#if FIX_FILE_VER == 1
    if (dwDataLen != sizeof(CAMERA_INFO_HEADER) + dwInfoSize + dwDataSize)
    {
        HV_Trace(5, "<SafeSaverNet>文件(%d)长度异常 \n", index);
        return E_FAIL;
    }
#endif
    memcpy(pbInfo, pGetData + sizeof(CAMERA_INFO_HEADER), dwInfoSize);
    memcpy(pbData, pGetData + sizeof(CAMERA_INFO_HEADER) + dwInfoSize, dwDataSize);
#if FIX_FILE_VER == 1 || FIX_FILE_VER == 3
    //打印信息
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

    HV_Trace(5, "<SafeSaverNet>Read %s : %s %u (%u) %d \n"
             ,szDataType
             ,szFileName
             ,dwDataLen
             ,m_dwLastDataTime - dwBeginTime
             ,index
            );

    return S_OK;
}

HRESULT CSafeSaverNetImpl::CopyCfData(SCfData& sCfData)
{
    CCameraData* pCameraData = sCfData.pRefData->GetItem();
    DWORD32 dwDataLen = 0;
    memcpy((char*)m_sBuf.addr + dwDataLen, &pCameraData->header, sizeof(pCameraData->header));
    dwDataLen += sizeof(pCameraData->header);
    memcpy((char*)m_sBuf.addr + dwDataLen, pCameraData->pbInfo, pCameraData->header.dwInfoSize);
    dwDataLen += pCameraData->header.dwInfoSize;
    memcpy((char*)m_sBuf.addr + dwDataLen, pCameraData->pbData, pCameraData->header.dwDataSize);
    dwDataLen += pCameraData->header.dwDataSize;

    if (0 == sCfData.iDataType)//识别结果,修正CarID
    {
        BLOCK_HEADER cBlockHeader;
        char* pXML = NULL;
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
                pXML = pbTemp;
                if (strstr(pXML, "违章:是"))
                {
                    sCfData.dwDataInfo = 1;
                }
                break;
            }

            pbTemp += cBlockHeader.dwLen;
            i += cBlockHeader.dwLen;
        }

        TiXmlDocument xmlDoc;
        xmlDoc.Parse(pXML);
        if (pXML && xmlDoc.RootElement())
        {
            TiXmlElement* pElement = xmlDoc.RootElement()->FirstChildElement("ResultSet");
            if (pElement)
            {
                pElement = pElement->FirstChildElement("Result");
            }
            TiXmlElement* pElementCarId = NULL;
            TiXmlElement* pElementTimeLow = NULL;
            TiXmlElement* pElementTimeHigh = NULL;
            if (pElement)
            {
                pElementCarId = pElement->FirstChildElement("CarID");
                pElementTimeLow = pElement->FirstChildElement("TimeLow");
                pElementTimeHigh = pElement->FirstChildElement("TimeHigh");
            }
            if (pElementTimeLow)
            {
                const TiXmlAttribute* attr = pElementTimeLow->FirstAttribute();
                for (; attr; attr=attr->Next())
                {
                    if (0 == strcmp(attr->Name(), "value"))
                    {
                        sscanf(attr->Value(), "%u", &sCfData.dwTimeLow);
                        break;
                    }
                }
            }
            if (pElementTimeHigh)
            {
                const TiXmlAttribute* attr = pElementTimeHigh->FirstAttribute();
                for (; attr; attr=attr->Next())
                {
                    if (0 == strcmp(attr->Name(), "value"))
                    {
                        sscanf(attr->Value(), "%u", &sCfData.dwTimeHigh);
                        break;
                    }
                }
            }
            if (pElementCarId)
            {
#if FIX_FILE_VER == 1
                DWORD32 dwCarID = m_cCfRecord.GetHourCount(sCfData.dwTimeLow, sCfData.dwTimeHigh);
#else
                DWORD32 dwCarID;
                GetHourCount(sCfData.dwTimeLow, sCfData.dwTimeHigh, &dwCarID);
                sCfData.dwIndex = dwCarID;
#endif
                char szValue[32] = {0};
                sprintf(szValue, "%u", dwCarID);
                pElementCarId->SetAttribute("value", szValue);

                TiXmlPrinter cTxPr;
                xmlDoc.Accept(&cTxPr);
                strcpy(pXML, cTxPr.CStr());
            }
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

bool CSafeSaverNetImpl::ReconnectNFS(void)
{
    return true;
    /*
    if (m_sParam.iNetDiskType == 0)
    {
        std::string strNFS = m_sParam.szNFS;
        if (strNFS[0] != '/')
        {
            strNFS = "/" + strNFS;
        }
        char szCMD[256] = {0};
        sprintf(szCMD, "mount -t nfs %s:%s %s %s"
                ,m_sParam.szNetDiskIP
                ,strNFS.c_str()
                ,ROOT_DIR
                ,m_sParam.szNFSParam
               );
        return !HV_System("umount -f /Result_disk") && !HV_SystemWithTimeOut(szCMD, 3600000);
    }
    return false;
    */
}

HRESULT CSafeSaverNetImpl::Run(void *pvParamter)
{
    if (false == m_sParam.fSaveRecord && false == m_sParam.fSaveVideo)
    {
        return E_FAIL;
    }

    SConstFileSysParam sCfParamRecord;
    SConstFileSysParam sCfParamVideo;
    sCfParamRecord.fFormat = m_sParam.fFormat;
    sCfParamVideo.fFormat = m_sParam.fFormat;

    bool fMounted = false;
    //const char szMountFlag[] = "/nfs_mounted/";
    if (access(ROOT_DIR, 0) == 0)
    {
        umount2(ROOT_DIR , MNT_FORCE);
        //fMounted = true;
    }
    //群辉硬盘
    if (m_sParam.iNetDiskType == 0)
    {
        sCfParamRecord.fnReadFile = CfReadFileNfs;
        sCfParamRecord.fnWriteFile = CfWriteFileNfs;
        sCfParamRecord.fnDeleteFile = CfDeleteFileNfs;
        sCfParamRecord.fnDeleteFolder = CfDeleteFolderNfs;
        sCfParamVideo.fnReadFile = CfReadFileNfs;
        sCfParamVideo.fnWriteFile = CfWriteFileNfs;
        sCfParamVideo.fnDeleteFile = CfDeleteFileNfs;
        sCfParamVideo.fnDeleteFolder = CfDeleteFolderNfs;
        sprintf(sCfParamRecord.szRootDir, "/%s", ROOT_DIR);
        sprintf(sCfParamVideo.szRootDir, "/%s", ROOT_DIR);
        mkdir(ROOT_DIR"/", 0777);
    }
    else if (m_sParam.iNetDiskType == 1)//Buffalo
    {
        sCfParamRecord.fnReadFile = CfReadFile;
        sCfParamRecord.fnWriteFile = CfWriteFile;
        sCfParamRecord.fnDeleteFile = CfDeleteFile;
        sCfParamRecord.fnDeleteFolder = CfDeleteFolder;
        sCfParamVideo.fnReadFile = CfReadFile;
        sCfParamVideo.fnWriteFile = CfWriteFile;
        sCfParamVideo.fnDeleteFile = CfDeleteFile;
        sCfParamVideo.fnDeleteFolder = CfDeleteFolder;
        const int iTmp = strlen(m_sParam.szFtpRoot);
        if (iTmp > 0)
        {
            if (m_sParam.szFtpRoot[iTmp - 1] == '/')
            {
                m_sParam.szFtpRoot[iTmp - 1] = 0;
            }
        }
        sprintf(sCfParamRecord.szRootDir, "%s/%s", m_sParam.szFtpRoot, m_sParam.szNetDiskUser);
        sprintf(sCfParamVideo.szRootDir, "%s/%s", m_sParam.szFtpRoot, m_sParam.szNetDiskUser);
    }
    else
    {
        return E_FAIL;
    }

    sCfParamVideo.fHourFix = true;
    sCfParamRecord.fHourInfo = true;
    strcpy(sCfParamRecord.szDataType, "r");
    strcpy(sCfParamVideo.szDataType, "v");

    bool fVideoNewHour = false;
    unsigned long long llUser = (long long)m_sParam.iNetDiskSpace * 1024 * 1024 * 1024;
#if FIX_FILE_VER == 1
    if (m_sParam.fFormat)
    {
        ResetInitHddParam();

        sCfParamRecord.dwMaxFileSize = RECORD_FILE_SIZE;
        sCfParamVideo.dwMaxFileSize = VIDEO_FILE_SIZE;

        if (m_sParam.fSaveRecord && false == m_sParam.fSaveVideo)
        {
            sCfParamRecord.dwMaxFileCount = llUser / sCfParamRecord.dwMaxFileSize;
        }
        else if (false == m_sParam.fSaveRecord && m_sParam.fSaveVideo)
        {
            sCfParamVideo.dwMaxFileCount = llUser / sCfParamVideo.dwMaxFileSize;
        }
        else
        {
            llUser /= 2;
            sCfParamRecord.dwMaxFileCount = llUser / sCfParamRecord.dwMaxFileSize;
            sCfParamVideo.dwMaxFileCount = llUser / sCfParamVideo.dwMaxFileSize;
            sCfParamVideo.dwFileBegin = sCfParamRecord.dwMaxFileCount;
        }
    }
#else
    if (m_sParam.fSaveRecord && m_sParam.fSaveVideo)
    {
        llUser /= 2;
    }
#endif
    while (!m_fExit)
    {
        if (S_OK == m_hrDiskStatus)
        {
             g_cHddOpThread.SetStatusCodeString("正常");
            m_bigFileRecord.Commit(false, false);
            m_bigFileVideo.Commit(false, false);
        }

        if (g_fHvReset)//系统要求复位了
        {
            SemPend(&m_hSem);
#if FIX_FILE_VER == 1
            if (S_OK == m_hrDiskStatus)
            {
                m_cCfRecord.Submit(1);
                m_cCfVideo.Submit(1);
            }
#endif
            m_hrDiskStatus = E_FAIL;
            SemPost(&m_hSem);
            break;
        }

        if (S_OK != m_hrDiskStatus)
        {
            //尝试登陆
            if (m_sParam.iNetDiskType == 0)
            {
                if (false == fMounted)
                {
                    std::string strNFS = m_sParam.szNFS;
                    if (strNFS[0] != '/')
                    {
                        strNFS = "/" + strNFS;
                    }
                    char szCMD[256] = {0};
                    sprintf(szCMD, "mount -t nfs %s:%s %s %s"
                            ,m_sParam.szNetDiskIP
                            ,strNFS.c_str()
                            ,ROOT_DIR
                            ,m_sParam.szNFSParam
                           );
                    if (0 != HV_SystemWithTimeOut(szCMD, 3600000))
                    {
                        HV_Sleep(10 * 1000);
                        continue;
                    }
                    fMounted = true;
                    // mkdir(szMountFlag, 0777);
                }
                else
                {
                    FILE* pFile = fopen(ROOT_DIR"/test.txt", "wb");
                    if (pFile)
                    {
                        fclose(pFile);
                        pFile = NULL;
                    }
                    else
                    {
                        HV_Sleep(3000);
                        continue;
                    }
                }
            }
            else
            {
                if (NULL == g_pFTP)
                {
                    g_pFTP = new CQFTP(m_sParam.szNetDiskIP, m_sParam.szNetDiskUser, m_sParam.szNetDiskPwd, 5000);
                }
                g_pFTP->LogoutFtpServer();
                if (ERROR_QFTP_OK != g_pFTP->LoginFtpServer())
                {
                    HV_Sleep(2000);
                    continue;
                }
            }

            bool fSaveRecord = m_sParam.fSaveRecord;
            bool fSaveVideo = m_sParam.fSaveVideo;
            //登陆成功后
            if (fSaveRecord)
            {
#if FIX_FILE_VER == 1
                if (S_OK != m_cCfRecord.Init(&sCfParamRecord))
                {
                    if (sCfParamRecord.fFormat)
                    {
                        HV_Trace(5, "<SafeSaverNet>识别结果存储格式化失败(%s)\n", m_sParam.szNetDiskUser);
                    }
                    else
                    {
                        HV_Trace(5, "<SafeSaverNet>识别结果存储初始化失败(%s)\n", m_sParam.szNetDiskUser);
                    }
                    fSaveRecord = false;
                }
                else
                {
                    if (sCfParamRecord.fFormat)
                    {
                        HV_Trace(5, "<SafeSaverNet>识别结果存储格式化成功(%s)\n", m_sParam.szNetDiskUser);
                    }
                    else
                    {
                        HV_Trace(5, "<SafeSaverNet>识别结果存储初始化成功(%s)\n", m_sParam.szNetDiskUser);
                    }
                }
#elif FIX_FILE_VER == 2
                char szPath[255];
                sprintf(szPath, "%s/record/", ROOT_DIR);
                if (!m_fixRecord.Initialize(szPath, llUser, m_sParam.iFileSize*1024))
                {
                    fSaveRecord = false;
                }
#elif FIX_FILE_VER == 3
                char szPath[255];
                sprintf(szPath, "%s/record/", ROOT_DIR);
                if (S_OK != m_bigFileRecord.Initialize(szPath, llUser, m_sParam.iFileSize*1024, true))
                {
                    fSaveRecord = false;
                }
#endif
            }
            if (fSaveVideo)
            {
#if FIX_FILE_VER == 1
                if (S_OK != m_cCfVideo.Init(&sCfParamVideo))
                {
                    if (sCfParamVideo.fFormat)
                    {
                        HV_Trace(5, "<SafeSaverNet>录像存储格式化失败(%s)\n", m_sParam.szNetDiskUser);
                    }
                    else
                    {
                        HV_Trace(5, "<SafeSaverNet>录像存储初始化失败(%s)\n", m_sParam.szNetDiskUser);
                    }
                    fSaveVideo = false;
                }
                else
                {
                    if (sCfParamVideo.fFormat)
                    {
                        HV_Trace(5, "<SafeSaverNet>录像存储格式化成功(%s)\n", m_sParam.szNetDiskUser);
                    }
                    else
                    {
                        HV_Trace(5, "<SafeSaverNet>录像存储初始化成功(%s)\n", m_sParam.szNetDiskUser);
                    }
                }
#elif FIX_FILE_VER == 2
                char szPath[255];
                sprintf(szPath, "%s/video/", ROOT_DIR);
                if (!m_fixVideo.Initialize(szPath, llUser, m_sParam.iFileSize*1024))
                {
                    fSaveVideo = false;
                }
#elif FIX_FILE_VER == 3
                char szPath[255];
                sprintf(szPath, "%s/video/", ROOT_DIR);
                if (S_OK != m_bigFileVideo.Initialize(szPath, llUser, m_sParam.iFileSize*1024, false))
                {
                    fSaveVideo = false;
                }
#endif
            }
            sCfParamRecord.fFormat = false;
            sCfParamVideo.fFormat = false;

            //如果都失败了
            if ((true == m_sParam.fSaveRecord && false == fSaveRecord) || (true == m_sParam.fSaveVideo && false == fSaveVideo))
            {
                ReconnectNFS();
                HV_Sleep(10000);
                continue;
            }

            if (NULL == m_sBuf.addr)
            {
                //这里需要用new来创建内存，防止共享内存不足
                //CreateShareMemOnPool(&m_sBuf, RECORD_FILE_SIZE);
                m_sBuf.addr = new char[RECORD_FILE_SIZE];
                m_sBuf.len = RECORD_FILE_SIZE;
                if (NULL == m_sBuf.addr)
                {
                    HV_Trace(5, "<SafeSaverNet>申请共享内存失败\n");
                    break;
                }
            }

            HV_Sleep(2000);
            SemPend(&m_hSem);
            m_hrDiskStatus = S_OK;
            SemPost(&m_hSem);
            g_nHddCheckStatus = 1;
            fVideoNewHour = true;
        }
        else
        {
            SemPend(&m_hSemQueCount, 500);
            SemPend(&m_hSemQue);
            bool fIsEmpty = m_listCfData.empty();
            SemPost(&m_hSemQue);
            if (fIsEmpty)
            {
#if FIX_FILE_VER == 1
                //空闲时，提交索引表等
                SemPend(&m_hSem);
                if (S_OK == m_hrDiskStatus)
                {
                    if (S_OK != m_cCfRecord.Submit(3))
                    {
                        m_hrDiskStatus = E_FAIL;
                    }
                }
                if (S_OK == m_hrDiskStatus)
                {
                    if (S_OK != m_cCfVideo.Submit(60))
                    {
                        m_hrDiskStatus = E_FAIL;
                    }
                }
                SemPost(&m_hSem);
                HV_Sleep(3);

                if (m_sParam.iNetDiskType)
                {
                    if (GetSystemTick() - m_dwLastDataTime > 60 * 1000)
                    {
                        SemPend(&m_hSem);
                        if (S_OK == m_hrDiskStatus)
                        {
                            g_pFTP->TestConnectValidity();
                            if (0 != g_pFTP->KeepAlive())
                            {
                                m_hrDiskStatus = E_FAIL;
                            }
                            m_dwLastDataTime = GetSystemTick();
                        }
                        SemPost(&m_hSem);
                    }
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
            SemPend(&m_hSem);
            CopyCfData(sCfData);
            SAFE_RELEASE(sCfData.pRefData);
            if (S_OK != m_hrDiskStatus)
            {
                SemPost(&m_hSem);
                continue;
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
                //写文件失败，重新连接网络硬盘
                ReconnectNFS();
                if (E_FAIL == hr)
                {
                    m_hrDiskStatus = E_FAIL;
                }
                HV_Trace(5, "<SafeSaverNet>写入文件失败 \n");
                SemPost(&m_hSem);
                continue;
            }
            else
            {
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
                m_hrDiskStatus = E_FAIL;
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
                m_hrDiskStatus = E_FAIL;
            }
#endif
            m_dwLastDataTime = GetSystemTick();
            SemPost(&m_hSem);

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
            HV_Trace(5, "<SafeSaverNet>write %s : %s %u (%u) %d \n"
                     ,szDataType
                     ,szFileName
                     ,sCfData.dwDataLen
                     ,m_dwLastDataTime - dwBeginTime
                     ,sCfData.dwIndex
                    );
        }
    }

    if (S_OK == m_hrDiskStatus)
    {
        m_bigFileRecord.Commit(true, false);
        m_bigFileVideo.Commit(true, false);
    }
    m_fExit = TRUE;
    return S_OK;
}


