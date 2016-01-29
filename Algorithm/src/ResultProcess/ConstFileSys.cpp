// 该文件编码必须是WINDOWS-936格式
#include "ConstFileSys.h"
#include <stdio.h>
#include <vector>

#ifdef WIN32
#include <io.h>
#include <time.h>
#endif

//以下用于合法性检查
#define MAX_FILE_COUNT_VALID(a)   (a >= 1 && a < 1024 * 1024 * 1024)
#define MAX_FILE_SIZE_VALID(a)    (a >= 1024 && a < 100 * 1024 * 1024)

#ifdef WIN32
/*static int HV_Trace(const int iLevel, char* szfmt, ...)
{
	va_list arglist;
	int iRetVal = 0;

	char szBuff[256] = {0};

	va_start( arglist, szfmt );
	iRetVal = vsprintf( szBuff, szfmt, arglist );
	va_end( arglist );
    ShowMessage(szBuff);
	return iRetVal;
}*/

/* 数据结构说明:用来定义实时时间. */
typedef struct _REAL_TIME_STRUCT {
	WORD16				wYear;			//年数.
	WORD16				wMonth;			//月数.
	WORD16				wDay;			//号数.
	WORD16				wWeekNum;		//当前日期的星期数.
	WORD16				wHour;			//小数数,24小时制.
	WORD16				wMinute;		//小时数.
	WORD16				wSecond;		//秒数.
	WORD16				wMSecond;		//毫秒数.
} REAL_TIME_STRUCT;

void ConvertMsToTime(
					 DWORD32 			dwMSCountLow,
					 DWORD32 			dwMSCountHigh,
                     REAL_TIME_STRUCT	*pRealTime
					 )
{
    DWORD64 dw64TimeMs = dwMSCountHigh;
    dw64TimeMs = (dw64TimeMs << 32) | dwMSCountLow;
	long t = dw64TimeMs/1000;
    struct tm * time = localtime(&t);

	pRealTime->wYear = time->tm_year+1900;
	pRealTime->wMonth = time->tm_mon+1;
	pRealTime->wDay = time->tm_mday;
	pRealTime->wHour = time->tm_hour;
	pRealTime->wMinute = time->tm_min;
	pRealTime->wSecond = time->tm_sec;
	pRealTime->wMSecond = dw64TimeMs % 1000;
}
#else
#endif

static bool CompareIndex(const SHourIndex& sHourIndex1, const SHourIndex& sHourIndex2)
{
	if (sHourIndex1.dwHour < sHourIndex2.dwHour)
	{
		return true;
	}
	else if (sHourIndex1.dwHour  > sHourIndex2.dwHour)
	{
		return false;
	}
	else
	{
		if (sHourIndex1.dwHourIndex < sHourIndex2.dwHourIndex)
		{
			return true;
		}
	}

	return false;
}

CConstFileSys::CConstFileSys()
{
	m_dwBufLen = 64 * 1024;
	m_pBuffer = new char[m_dwBufLen];
	Clear();
}

CConstFileSys::~CConstFileSys()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
}

HRESULT CConstFileSys::Init(const SConstFileSysParam* pParam)
{
    Clear();

    if (NULL == pParam)
    {
        return E_FAIL;
    }
    if (NULL == pParam->fnReadFile
       || NULL == pParam->fnWriteFile
       || NULL == pParam->fnDeleteFile
       )
    {
        HV_Trace(5, "<ConstFileSys>读写函数为空\n");
        return E_FAIL;
    }

    m_fnReadFile = pParam->fnReadFile;
    m_fnWriteFile = pParam->fnWriteFile;
    m_fnDeleteFile = pParam->fnDeleteFile;
    m_fHourFix = pParam->fHourFix;
    m_fHourInfo = pParam->fHourInfo;

#ifdef WIN32
    sprintf(m_szDataDir, "%s\\data", pParam->szRootDir);
    sprintf(m_szLogDir, "%s\\log", pParam->szRootDir);
    sprintf(m_szHourIndexFile, "%s\\hour_%s.idx", m_szLogDir,pParam->szDataType);
    sprintf(m_szHourIndexFileBak, "%s\\hour_%s.idx.bak", m_szLogDir,pParam->szDataType);
    sprintf(m_szSysTableFile, "%s\\sys_%s.tbl", m_szLogDir,pParam->szDataType);
#else
    sprintf(m_szDataDir, "%s/data", pParam->szRootDir);
    sprintf(m_szLogDir, "%s/log", pParam->szRootDir);
    sprintf(m_szHourIndexFile, "%s/hour_%s.idx", m_szLogDir,pParam->szDataType);
    sprintf(m_szHourIndexFileBak, "%s/hour_%s.idx.bak", m_szLogDir,pParam->szDataType);
    sprintf(m_szSysTableFile, "%s/sys_%s.tbl", m_szLogDir,pParam->szDataType);
#endif

    if (pParam->fFormat)
    {
        m_fnDeleteFile(m_szHourIndexFile);
        m_fnDeleteFile(m_szHourIndexFileBak);
        m_fnDeleteFile(m_szSysTableFile);
        //MakeSureDirectoryPathExists(m_szHourIndexFile);
        static bool fDeleteFolder = false;
        if (false == fDeleteFolder && pParam->fnDeleteFolder)
        {
        	fDeleteFolder = true;
        	pParam->fnDeleteFolder(m_szLogDir);
        }

        m_sSysTable.dwMaxFileCount = pParam->dwMaxFileCount;
        m_sSysTable.dwMaxFileSize = pParam->dwMaxFileSize;
        m_sSysTable.dwFileBegin = pParam->dwFileBegin;

        if (S_OK != CheckSysTable())
        {
            return E_FAIL;
        }

        memcpy(m_pBuffer, &m_sSysTable, sizeof(m_sSysTable));
        if (S_OK != m_fnWriteFile(m_szSysTableFile, m_pBuffer, sizeof(m_sSysTable)))
        {
            HV_Trace(5, "<ConstFileSys>写入系统表失败...\n");
            return E_FAIL;
        }

        HV_Trace(5, "<ConstFileSys>定长存储系统初始化完成(MAX_FILE:%u, MAX_FILE_SIZE:%u)"
                ,m_sSysTable.dwMaxFileCount
                ,m_sSysTable.dwMaxFileSize
                );
    }
    else
    {
        DWORD32 dwDataLen = 0;
        m_fnReadFile(m_szSysTableFile, m_pBuffer, m_dwBufLen, dwDataLen);
        if (dwDataLen >= m_dwBufLen)
        {
        	HV_Trace(5, "<ConstFileSys>系统表过大...\n");
            return E_FAIL;
        }
        if (dwDataLen == sizeof(m_sSysTable))
        {
            memcpy(&m_sSysTable, m_pBuffer, sizeof(m_sSysTable));
            //检查系统表
            if (S_OK != CheckSysTable())
            {
                return E_FAIL;
            }
        }
        else
        {
            HV_Trace(5, "<ConstFileSys>系统表不存在，定长存储系统可能未初始化...\n");
            return E_FAIL;
        }

        //读小时索引表
        bool fHourIndex = false;
        dwDataLen = 0;
        m_fnReadFile(m_szHourIndexFile, m_pBuffer, m_dwBufLen, dwDataLen);
        if (dwDataLen >= m_dwBufLen)
        {
        	HV_Trace(5, "<ConstFileSys>小时索引表过大...\n");
            return E_FAIL;
        }
        if (dwDataLen > 0 && 0 == dwDataLen%sizeof(SHourIndex))
        {
            for (int i=0; i<(int)dwDataLen; i+=sizeof(SHourIndex))
            {
                SHourIndex sHourIndex;
                memcpy(&sHourIndex, m_pBuffer + i, sizeof(sHourIndex));
                m_listHourIndex.push_back(sHourIndex);
            }
            fHourIndex = true;
        }
        //再读取备份表
        if (false == fHourIndex)
        {
            dwDataLen = 0;
            m_fnReadFile(m_szHourIndexFileBak, m_pBuffer, m_dwBufLen, dwDataLen);
            if (dwDataLen >= m_dwBufLen)
	        {
	        	HV_Trace(5, "<ConstFileSys>小时索引备份表过大...\n");
	            return E_FAIL;
	        }
            if (dwDataLen > 0 && 0 == dwDataLen%sizeof(SHourIndex))
            {
                for (int i=0; i<(int)dwDataLen; i+=sizeof(SHourIndex))
                {
                    SHourIndex sHourIndex;
                    memcpy(&sHourIndex, m_pBuffer + i, sizeof(sHourIndex));
                    m_listHourIndex.push_back(sHourIndex);
                }
                fHourIndex = true;
            }

            //更新主表
            if (fHourIndex)
            {
                if (S_OK != UpdateHourIndex())
                {
                    return E_FAIL;
                }
                HV_Trace(5, "<ConstFileSys>根据小时索引备份表恢复主表...\n");
            }
        }
    }

    char szFileBegin[MAX_PATH] = {0};
    char szFileEnd[MAX_PATH] = {0};
    GetFileByIndex(0, szFileBegin, sizeof(szFileBegin));
    GetFileByIndex(m_sSysTable.dwMaxFileCount-1, szFileEnd, sizeof(szFileEnd));

    HV_Trace(5, "<ConstFileSys>[%s]开始文件:%s 结束文件:%s\n", pParam->szDataType, szFileBegin, szFileEnd);

    m_fSysOk = true;

    return S_OK;
}

DWORD32 CConstFileSys::GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh)
{
    DWORD32 dwHour = TimeToHour(dwTimeLow, dwTimeHigh);

    DWORD32 dwHourCount = 0;
    std::vector<SHourIndex>::const_iterator iter = m_listHourIndex.begin();
    for(; iter!=m_listHourIndex.end(); ++iter)
    {
        if (iter->dwHour == dwHour)
        {
            dwHourCount += iter->dwCount;
        }
    }

    return dwHourCount;
}

HRESULT CConstFileSys::GetNextItem(const SCfItem& sCfItem, SCfItem& sCfItemNext)
{
	if (S_OK != GetStatus())
    {
        return E_FAIL;
    }

	std::vector<SHourIndex> list = m_listHourIndex;
	//按时间排序
	std::sort(list.begin(), list.end(), CompareIndex);
	DWORD32 dwNowHour = TimeToHour(sCfItem.dwTimeLow, sCfItem.dwTimeHigh);
	DWORD32 dwNextIndex = sCfItem.dwIndex + 1;
	DWORD32 dwNextHour = 0;

	std::vector<SHourIndex>::const_iterator iter = list.begin();
    for(; iter!=list.end(); ++iter)
    {
    	if (dwNextHour >= dwNowHour)
    	{
    		break;
    	}
        if (dwNowHour == iter->dwHour)//当前小时的下一个
        {
        	//当前段
        	if (dwNextIndex >= iter->dwHourIndex && dwNextIndex < iter->dwHourIndex + iter->dwCount)
        	{
        		dwNextHour = iter->dwHour;
        	}
        	else if (iter->dwHourIndex > dwNextIndex)//比当前大的段
        	{
        		dwNextHour = iter->dwHour;
        		dwNextIndex = iter->dwHourIndex;
        	}
        }
        else if (dwNowHour < iter->dwHour)//比当前小时大
        {
        	dwNextHour = iter->dwHour;
        	dwNextIndex = iter->dwHourIndex;
        }
    }

    if (dwNextHour >= dwNowHour)//有下一个
    {
    	REAL_TIME_STRUCT rtsTime;
	    rtsTime.wYear = dwNextHour / 1000000;
	    dwNextHour = dwNextHour % 1000000;
	    rtsTime.wMonth = dwNextHour / 10000;
	    dwNextHour = dwNextHour % 10000;
	    rtsTime.wDay = dwNextHour / 100;
	    dwNextHour = dwNextHour % 100;
	    rtsTime.wHour = dwNextHour;
	    sCfItemNext.dwIndex = dwNextIndex;
	    ConvertTimeToMs(&rtsTime, &sCfItemNext.dwTimeLow, &sCfItemNext.dwTimeHigh);
	    return S_OK;
    }

    return S_FALSE;
}

HRESULT CConstFileSys::DeleteFirstHour()
{
    DWORD32 dwDeleteHour = m_listHourIndex.front().dwHour;

    std::vector<SHourIndex>::iterator iter = m_listHourIndex.begin();
    while (iter!=m_listHourIndex.end())
    {
        if (iter->dwHour == dwDeleteHour)
        {
            HV_Trace(5, "<ConstFileSys> Recycle(%u %u)", dwDeleteHour, iter->dwCount);
            iter = m_listHourIndex.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if (m_fHourInfo)
    {
    	char szFile[MAX_PATH] = {0};
		GetHourInfoFile(dwDeleteHour, false, szFile, sizeof(szFile));
		m_fnDeleteFile(szFile);
    }

    return S_OK;
}

HRESULT CConstFileSys::WriteFile(const DWORD32 dwTimeLow
                                ,const DWORD32 dwTimeHigh
                                ,const DWORD32 dwDataInfo          //数据信息(1:违章数据)
                                ,DWORD32& dwIndex
                                ,char* szBuf
                                ,const DWORD32 dwDataLen
                                ,const bool fForceNewHour/* = false*/
                                )
{
    if (NULL == szBuf || dwDataLen < 1)
    {
        return E_INVALIDARG;
    }

    if (S_OK != GetStatus())
    {
        return E_FAIL;
    }

    if (dwDataLen > m_sSysTable.dwMaxFileSize)
    {
        HV_Trace(5, "<ConstFileSys>写文件大小:%u, 超过单个文件的最大值:%u...\n", dwDataLen, m_sSysTable.dwMaxFileSize);
        return E_INVALIDARG;
    }

    DWORD32 dwHour = TimeToHour(dwTimeLow, dwTimeHigh);

    //文件已经存在，则直接替换
    if (m_fHourFix)
    {
    	std::vector<SHourIndex>::const_iterator iter = m_listHourIndex.begin();
	    for(; iter!=m_listHourIndex.end(); ++iter)
	    {
	        if (iter->dwHour == dwHour
	            && dwIndex >= iter->dwHourIndex
	            && dwIndex < iter->dwHourIndex + iter->dwCount
	           )
	        {
	        	char szFileName[MAX_PATH] = {0};
            	DWORD32 dwFileIndex = iter->dwFileIndex + (dwIndex - iter->dwHourIndex);
            	GetFileByIndex(dwFileIndex, szFileName, sizeof(szFileName));
            	if (S_OK != m_fnWriteFile(szFileName, szBuf, dwDataLen))
			    {
			        m_fSysOk = false;
			        HV_Trace(5, "<ConstFileSys>写入文件:%s失败...\n", szFileName);
			        return E_FAIL;
			    }
			    return S_OK;
	        }
		}
    }
    //删除比当前大的文件
    if (m_fHourFix)
    {
    	std::vector<SHourIndex>::iterator iter = m_listHourIndex.begin();
	    while (iter!=m_listHourIndex.end())
	    {
	        if (iter->dwHour > dwHour
	        	|| ((iter->dwHour == dwHour) && (dwIndex < iter->dwHourIndex))
	        	)
	        {
	        	iter = m_listHourIndex.erase(iter);
	        }
	        else
	        {
	        	++iter;
	        }
		}
    }

	bool fForceUpdate = false;
    DWORD32 dwLastHour = 0;         //最后一个小时
    DWORD32 dwNextFileIndex = 0;    //下一个文件的存储位置
    SHourIndex& sFirstHour = m_listHourIndex.front();
    SHourIndex& sLastHour = m_listHourIndex.back();

    //1.确定下一个存储位置
    if (m_listHourIndex.size())
    {
        dwLastHour = sLastHour.dwHour;

        DWORD32 dwFirstFileIndex = sFirstHour.dwFileIndex;
        DWORD32 dwLastFileIndex = sLastHour.dwFileIndex + sLastHour.dwCount - 1;

        //已被占用的文件数量
        DWORD32 dwUsedFile = (dwLastFileIndex + m_sSysTable.dwMaxFileCount - dwFirstFileIndex) % m_sSysTable.dwMaxFileCount + 1;

        if (dwUsedFile >= m_sSysTable.dwMaxFileCount)
        {
            //删除第一个小时的数据
            DeleteFirstHour();
            fForceUpdate = true;
        }

        if (m_listHourIndex.size())
        {
            dwNextFileIndex = (sLastHour.dwFileIndex + sLastHour.dwCount)%m_sSysTable.dwMaxFileCount;
        }
    }

    //2.是否要新增小时索引
	if (m_listHourIndex.empty()
	   || (dwLastHour != dwHour)
	   || fForceNewHour
	   )
    {
        SHourIndex sNewHour;
        sNewHour.dwHour = dwHour;
        sNewHour.dwFileIndex = dwNextFileIndex;
        if (fForceNewHour || m_fHourFix)//创建新小时，都有外部指定第一个记录的小时索引
        {
        	sNewHour.dwHourIndex = dwIndex;
        }
        else
        {
        	sNewHour.dwHourIndex = GetHourCount(dwTimeLow, dwTimeHigh);
        }
        dwIndex = sNewHour.dwHourIndex;
        sNewHour.dwCount = 1;
        m_listHourIndex.push_back(sNewHour);
    }
    else
    {
        dwIndex = sLastHour.dwHourIndex + sLastHour.dwCount;
        ++sLastHour.dwCount;
    }

    char szNextFileName[MAX_PATH] = {0};
    GetFileByIndex(dwNextFileIndex, szNextFileName, sizeof(szNextFileName));

    if (S_OK != m_fnWriteFile(szNextFileName, szBuf, dwDataLen))
    {
        m_fSysOk = false;
        HV_Trace(5, "<ConstFileSys>写入文件:%s失败...\n", szNextFileName);
        return E_FAIL;
    }
    //指定文件大小
    if(truncate(szNextFileName, m_sSysTable.dwMaxFileSize))
    {
        HV_Trace(5, "指定文件%s大小%d失败,错误代码:%s", szNextFileName, m_sSysTable.dwMaxFileSize, strerror(errno));
    }
	//需要更新小时信息表
    if (m_fHourInfo)
    {
    	if (dwIndex < sizeof(m_sWriteHourInfo.szInfo))
    	{
    		GetHourInfo(dwHour, m_sWriteHourInfo);
    		unsigned char* pIndex = m_sWriteHourInfo.szInfo + dwIndex;
    		unsigned char ucDataInfo = (unsigned char)dwDataInfo;
			if (*pIndex != ucDataInfo)
			{
				*pIndex = ucDataInfo;
				if (S_OK != UpdateHourInfo())
				{
				    return E_FAIL;
				}
			}
    	}
    }

	++m_dwLastCount;
    //更新表
    if (fForceUpdate)
    {
    	  std::vector<SHourIndex>::const_iterator iter = m_listHourIndex.begin();
        for(; iter!=m_listHourIndex.end(); ++iter)
        {
            if(iter == m_listHourIndex.end()-1)
            {
                if( iter->dwCount != sLastHour.dwCount )
                {
                    SHourIndex& sHour = m_listHourIndex.back();
                    sHour.dwCount = sLastHour.dwCount;
                    HV_Trace(5,"Count:%d",iter->dwCount);
                }
                break;
            }
        }

	    if (S_OK != Update())
	    {
	        return E_FAIL;
	    }
	    m_dwLastCount = 0;
	}

    return S_OK;
}

const SSysTable& CConstFileSys::GetSysTable() const
{
    return m_sSysTable;
}

const std::vector<SHourIndex>& CConstFileSys::GetHourIndex() const
{
    return m_listHourIndex;
}

HRESULT CConstFileSys::Update()
{
    //更新小时索引表
    if (S_OK != UpdateHourIndex())
    {
        return E_FAIL;
    }

    //更新小时索引备份表
    if (S_OK != UpdateHourIndexBak())
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CConstFileSys::ReadFile(const DWORD32 dwTimeLow
                               ,const DWORD32 dwTimeHigh
                               ,const DWORD32 dwDataInfo          //数据信息(1:违章数据)
                               ,DWORD32& dwIndex
                               ,char* szBuf
                               ,const DWORD32 dwBufLen
                               ,DWORD32& dwDataLen
                               )
{
    if (S_OK != GetStatus())
    {
        return E_FAIL;
    }

    DWORD32 dwHour = TimeToHour(dwTimeLow, dwTimeHigh);

	//指定类型的数据，比如违章的数据
	if (m_fHourInfo && dwDataInfo)
	{
		if (dwIndex >= sizeof(m_sReadHourInfo.szInfo))
		{
			return S_OK;
		}
		if (dwHour == m_sWriteHourInfo.dwHour)
		{
			memcpy(&m_sReadHourInfo, &m_sWriteHourInfo, sizeof(m_sReadHourInfo));
		}
		else
		{
			GetHourInfo(dwHour, m_sReadHourInfo);
		}
		bool fFind = false;
		unsigned char ucDataInfo = (unsigned char)dwDataInfo;
		for (int i=int(dwIndex); i<int(sizeof(m_sReadHourInfo.szInfo)); ++i)
		{
			unsigned char* pIndex = m_sReadHourInfo.szInfo + i;
			if (*pIndex == ucDataInfo)
			{
				fFind = true;
				dwIndex = i;
				break;
			}
		}
		if (false == fFind)
		{
			return S_OK;
		}
	}

    std::vector<SHourIndex>::const_iterator iter = m_listHourIndex.begin();
    for(; iter!=m_listHourIndex.end(); ++iter)
    {
        if (iter->dwHour == dwHour
            && dwIndex >= iter->dwHourIndex
            && dwIndex < iter->dwHourIndex + iter->dwCount
           )
        {
            char szFileName[MAX_PATH] = {0};
            DWORD32 dwFileIndex = iter->dwFileIndex + (dwIndex - iter->dwHourIndex);
            GetFileByIndex(dwFileIndex, szFileName, sizeof(szFileName));

            if (S_OK != m_fnReadFile(szFileName, szBuf, dwBufLen, dwDataLen))
            {
                HV_Trace(5, "<ConstFileSys>读取文件:%s失败...\n", szFileName);
                return E_FAIL;
            }

            return S_OK;
        }
	}

    return S_OK;
}

HRESULT CConstFileSys::UpdateHourIndex()
{
    if (m_listHourIndex.empty())
    {
        m_fnDeleteFile(m_szHourIndexFile);
        return S_OK;
    }

    std::vector<SHourIndex>::const_iterator iter = m_listHourIndex.begin();
    DWORD32 dwDataLen = 0;
    for(; iter!=m_listHourIndex.end(); ++iter)
    {
        const SHourIndex sHourIndex = *iter;
        memcpy(m_pBuffer + dwDataLen, &sHourIndex, sizeof(sHourIndex));
        dwDataLen += sizeof(sHourIndex);
        if (dwDataLen >= m_dwBufLen)
        {
        	m_fSysOk = false;
        	HV_Trace(5, "<ConstFileSys>更新小时索引表失败(表过大)...\n");
        	return E_FAIL;
        }
    }

    if (S_OK != m_fnWriteFile(m_szHourIndexFile, m_pBuffer, dwDataLen))
    {
        m_fSysOk = false;
        HV_Trace(5, "<ConstFileSys>更新小时索引表失败...\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CConstFileSys::UpdateHourIndexBak()
{
    if (m_listHourIndex.empty())
    {
        m_fnDeleteFile(m_szHourIndexFileBak);
        return S_OK;
    }

    std::vector<SHourIndex>::const_iterator iter = m_listHourIndex.begin();
    DWORD32 dwDataLen = 0;
    for(; iter!=m_listHourIndex.end(); ++iter)
    {
        const SHourIndex sHourIndex = *iter;
        memcpy(m_pBuffer + dwDataLen, &sHourIndex, sizeof(sHourIndex));
        dwDataLen += sizeof(sHourIndex);
        if (dwDataLen >= m_dwBufLen)
        {
        	m_fSysOk = false;
        	HV_Trace(5, "<ConstFileSys>更新小时索引备份表失败(表过大)...\n");
        	return E_FAIL;
        }
    }

    if (S_OK != m_fnWriteFile(m_szHourIndexFileBak, m_pBuffer, dwDataLen))
    {
        m_fSysOk = false;
        HV_Trace(5, "<ConstFileSys>更新小时索引备份表失败...\n");
        return E_FAIL;
    }

    return S_OK;
}

//根据系统文件索引等到文件路径
HRESULT CConstFileSys::GetFileByIndex(const DWORD32 dwFileIndex
                                     ,char* szBuf
                                     ,const DWORD32 dwBufLen
                                     )
{
    if (NULL == szBuf || dwBufLen < 1)
    {
        return E_FAIL;
    }

	DWORD32 dwDataFileIndex = dwFileIndex%m_sSysTable.dwMaxFileCount + m_sSysTable.dwFileBegin;
    DWORD32 dwFolder1 = dwDataFileIndex/(100 * 100);
    DWORD32 dwFolder2 = dwDataFileIndex%(100 * 100) / 100;

#ifdef WIN32
    snprintf(szBuf, dwBufLen-1, "%s\\%d\\%d\\%08d.dat", m_szDataDir, dwFolder1, dwFolder2, dwDataFileIndex);
#else
    snprintf(szBuf, dwBufLen-1, "%s/%d/%d/%08d.dat", m_szDataDir, dwFolder1, dwFolder2, dwDataFileIndex);
#endif

    return S_OK;
}

//转换时间为具体的小时数
DWORD32 CConstFileSys::TimeToHour(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh)
{
    REAL_TIME_STRUCT rtsTime;
    ConvertMsToTime(dwTimeLow, dwTimeHigh, &rtsTime);
    DWORD32 dwHour = 0;
    char szTime[32] = {0};
    sprintf(szTime, "%04d%02d%02d%02d", rtsTime.wYear, rtsTime.wMonth, rtsTime.wDay, rtsTime.wHour);
    sscanf(szTime, "%u", &dwHour);

    return dwHour;
}

HRESULT CConstFileSys::GetStatus()
{
    if (false == m_fSysOk)
    {
        return E_FAIL;
    }

    /*if (access(m_szSysTableFile, 0))
    {
        m_fSysOk = false;
        return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CConstFileSys::CheckSysTable()
{
    //检查值是否合理
    if ( false == MAX_FILE_COUNT_VALID(m_sSysTable.dwMaxFileCount)
        || false == MAX_FILE_SIZE_VALID(m_sSysTable.dwMaxFileSize)
        )
    {
        m_fSysOk = false;
        HV_Trace(5, "<ConstFileSys>定长存储系统表参数无效...\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CConstFileSys::Clear()
{
	m_fSysOk = false;
	m_fHourFix = false;
	m_fHourInfo = false;
	m_dwLastCount = 0;
	m_listHourIndex.clear();
    memset(m_szDataDir, 0, sizeof(m_szDataDir));
    memset(m_szLogDir, 0, sizeof(m_szLogDir));
    memset(m_szHourIndexFile, 0, sizeof(m_szHourIndexFile));
    memset(m_szHourIndexFileBak, 0, sizeof(m_szHourIndexFileBak));
    memset(m_szSysTableFile, 0, sizeof(m_szSysTableFile));
    memset(&m_sWriteHourInfo, 0, sizeof(m_sWriteHourInfo));
    memset(&m_sReadHourInfo, 0, sizeof(m_sReadHourInfo));

    m_fnReadFile = NULL;
    m_fnWriteFile = NULL;
    m_fnDeleteFile = NULL;

    return S_OK;
}

HRESULT CConstFileSys::GetHourInfoFile(const DWORD32 dwHour
									  ,const bool fBak
                                      ,char* szBuf
                                      ,const DWORD32 dwBufLen
                                      )
{
	if (NULL == szBuf || dwBufLen < 1)
    {
        return E_FAIL;
    }

#ifdef WIN32
	if (fBak)
	{
		snprintf(szBuf, dwBufLen-1, "%s\\%d.df.bak", m_szLogDir, dwHour);
	}
	else
	{
    	snprintf(szBuf, dwBufLen-1, "%s\\%d.df", m_szLogDir, dwHour);
    }
#else
	if (fBak)
	{
		snprintf(szBuf, dwBufLen-1, "%s/%d.df.bak", m_szLogDir, dwHour);
	}
	else
	{
    	snprintf(szBuf, dwBufLen-1, "%s/%d.df", m_szLogDir, dwHour);
    }
#endif

    return S_OK;
}

HRESULT CConstFileSys::GetHourInfo(const DWORD32 dwHour, SHourInfo& sHourInfo)
{
	if (dwHour == sHourInfo.dwHour)
	{
		return S_OK;
	}

	memset(&sHourInfo, 0, sizeof(sHourInfo));
	sHourInfo.dwHour = dwHour;

	char szFile[MAX_PATH] = {0};
	GetHourInfoFile(dwHour, false, szFile, sizeof(szFile));

	//读小时索引表
    DWORD32 dwDataLen = 0;
    m_fnReadFile(szFile, m_pBuffer, m_dwBufLen, dwDataLen);
    if (dwDataLen == sizeof(sHourInfo.szInfo))
    {
    	memcpy(sHourInfo.szInfo, m_pBuffer, dwDataLen);
    }

    return S_OK;
}

HRESULT CConstFileSys::UpdateHourInfo()
{
	char szFile[MAX_PATH] = {0};
	GetHourInfoFile(m_sWriteHourInfo.dwHour, false, szFile, sizeof(szFile));

	if (S_OK != m_fnWriteFile(szFile, (char*)m_sWriteHourInfo.szInfo, sizeof(m_sWriteHourInfo.szInfo)))
	{
		m_fSysOk = false;
		HV_Trace(5, "<ConstFileSys>更新小时信息表失败...\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CConstFileSys::Submit(const DWORD32 dwMinCount)
{
	if (m_dwLastCount >= dwMinCount)
	{
		if (S_OK != Update())
	    {
	        return E_FAIL;
	    }
	    m_dwLastCount = 0;
	}

	return S_OK;
}



