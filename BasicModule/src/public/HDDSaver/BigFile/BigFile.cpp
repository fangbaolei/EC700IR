/**
* @file BigFile.cpp 
* @brief implementation of CBigFile
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>	
#include "swpa.h"
#include "SWFC.h"

#include "BigFile.h"
#include <unistd.h>
#include <stdio.h>

class CSimpleFile
{
public:
	CSimpleFile()
	{
		m_fp = NULL; 
		swpa_memset(m_szFileName, 0, sizeof(m_szFileName));
	}
	virtual ~CSimpleFile()
	{
		if (NULL != m_fp)
		{
			Close(); 
			m_fp = NULL;
		}
	}
	
	HRESULT Open(LPCSTR szFileName, LPCSTR szMode)
	{
		if (NULL == szFileName || NULL == szMode)
		{
			SW_TRACE_DEBUG("Err: invalid arg.\n");
			return E_INVALIDARG;
		}

		if (NULL != m_fp)
		{
			Close(); 
		}

		m_fp = fopen(szFileName, szMode);
		if (NULL == m_fp)
		{
			SW_TRACE_DEBUG("Err: failed to open %s in %s mode.\n", szFileName, szMode);
			return E_FAIL;
		}

		swpa_strcpy(m_szFileName, szFileName);
		return S_OK;
	}
	
	HRESULT Close()
	{
		if (NULL != m_fp)
		{
			if (0 != fclose(m_fp))
			{
				SW_TRACE_DEBUG("Err: failed to close.\n");
				return E_FAIL;
			}

			m_fp = NULL;
		}

		return S_OK;
	}
	
	HRESULT Write(PVOID pvBuf, DWORD dwSize)
	{
		if (NULL == m_fp)
		{
			SW_TRACE_DEBUG("Err: no file opened for writing.\n");
			return E_FAIL;
		}

		INT iWrittenLen = fwrite(pvBuf, 1, dwSize, m_fp);
		if (iWrittenLen != dwSize)
		{
			SW_TRACE_DEBUG("Err: only wrote %dB, not %dB.\n", iWrittenLen, dwSize);
			return E_FAIL;
		}

		return S_OK;
	}
	
	HRESULT Read(PVOID pvBuf, DWORD dwSize)
	{
		if (NULL == m_fp)
		{
			SW_TRACE_DEBUG("Err: no file opened for reading.\n");
			return E_FAIL;
		}

		INT iReadLen = fread(pvBuf, 1, dwSize, m_fp);
		if (iReadLen != dwSize)
		{
			SW_TRACE_DEBUG("Err: only read %dB, not %dB.\n", iReadLen, dwSize);
			return E_FAIL;
		}

		return S_OK;
	}
	HRESULT Truncate(const DWORD dwSize)
	{
		if (0 == swpa_strlen(m_szFileName))
		{
			SW_TRACE_DEBUG("Err: no file opened for truncating.\n");
			return E_FAIL;
		}

		if (0 != truncate(m_szFileName, dwSize))
		{
			SW_TRACE_DEBUG("Err: failed to truncate %s to %dB.\n", m_szFileName, dwSize);
			return E_FAIL;
		}

		return S_OK;
	}
	
private:
	FILE* m_fp;
	CHAR m_szFileName[256];
};


#define PRINT SW_TRACE_DEBUG



#define  INDEX_FILE_NAME ".index.dat"
#define  INDEX_FILE_BAK_NAME ".index.dat.bak"

#define INDEX_SIZE 1024*512 //1024*1024/*MB*/ * 2
#define COMMIT_COUNT 20
#define MASTER_COMMIT_COUNT 5
#define COMMIT_INTERVAL 20000

#define RESERVED_PERSENTAGE 5/100
#define MIN_RESERVED_SIZE 1
#define MAX_RESERVED_SIZE 4

#define BIG_FILE_DEBUG 1

CBigFile::CBigFile()
	:m_fInitialize(FALSE)
	,m_dwOneFileSize(0)
	,m_dwTotalFileCount(0)
	,m_dwCommitCount(0)
	,m_dwMasteCommitCount(0)
	,m_dwLastCommitTime(0)
	,m_fUpdate(FALSE)
	,m_iCommitCount(0)
	,m_fIsRecord(TRUE)
	,m_iMapTableSize(512*1024)
	,m_pbIndexBuffer(NULL)
	,m_pbMapTableBuffer(NULL)
	,m_pcMemoryFactory(NULL)
	,m_pcMemIndexBuf(NULL)
	,m_pcMemMapTableBuf(NULL)
	,m_pcCommittingThread(NULL)
{

	swpa_memset(m_szDirectory, 0, FILE_PATH_STRING_LEN+1);
	
}

CBigFile::~CBigFile()
{
    UnInitialize();
	
    //pthread_mutex_destroy(&m_mutex);
}

HRESULT CBigFile::Initialize(const CHAR* szDirecory, LONGLONG llTotalSize, INT iFileSize, BOOL fRecord)
{
    if (!szDirecory || 0 == swpa_strlen(szDirecory))
    {
        PRINT( "Err: directory is empty.\n");
        return E_FAIL;
    }

    UnInitialize();

	CSWMemoryFactory * m_pcMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if (NULL == m_pcMemoryFactory)
	{
		PRINT("Err: failed to get pMemoryFactory instance\n");
		return E_FAIL;
	}

	CSWMemory* m_pcMemIndexBuf = m_pcMemoryFactory->Alloc(4 + INDEX_SIZE);
	if (NULL == m_pcMemIndexBuf)
	{
		PRINT("Err: no memory for m_pcMemIndexBuf\n");
		return E_OUTOFMEMORY;
	}

	m_pbIndexBuffer = (PBYTE)m_pcMemIndexBuf->GetBuffer();
	swpa_memset(m_pbIndexBuffer, 0, 4 + INDEX_SIZE);

	CSWMemory* m_pcMemMapTableBuf = m_pcMemoryFactory->Alloc(m_iMapTableSize);
	if (NULL == m_pcMemMapTableBuf)
	{
		PRINT("Err: no memory for m_pcMemMapTableBuf\n");
		return E_OUTOFMEMORY;
	}

	m_pbMapTableBuffer = (PBYTE)m_pcMemMapTableBuf->GetBuffer();
	swpa_memset(m_pbMapTableBuffer, 0, m_iMapTableSize);
	
	
	PRINT("llTotalSize = %lld\n", llTotalSize);
	
    swpa_strncpy(m_szDirectory, szDirecory, FILE_PATH_STRING_LEN);

    if (m_szDirectory[swpa_strlen(m_szDirectory) - 1] != '/')
    {
        swpa_strcat(m_szDirectory, "/");
    }
    MakeSureDirectoryExists(m_szDirectory);

    if (m_szDirectory[swpa_strlen(m_szDirectory) - 1] == '/')
    {
        m_szDirectory[swpa_strlen(m_szDirectory) - 1] = '\0';
    }

    if (swpa_utils_dir_exist(m_szDirectory))//if (access(szDirecory, 0))
    {
        PRINT("Err: Dir %s doesn't exist!\n", m_szDirectory);//PRINT( "directory %s access fail, error string:%s", szDirecory, strerror(errno));
        return E_FAIL;
    }

    //if (!IsValid())
    {
        //struct statfs fs;
        //if (statfs(szDirecory, &fs))
        //{
        //    PRINT( "directory %s statfs fail, error string:%s", szDirecory, strerror(errno));
        //     return E_FAIL;
        //}
        //LONGLONG llTotal = (LONGLONG)fs.f_bsize * (LONGLONG)fs.f_blocks;
        //
        
        LONGLONG llTotal = 0;
        PRINT("m_szDirectory = %s\n", m_szDirectory);
        if (SWPAR_OK != swpa_hdd_get_totalsize(m_szDirectory, &llTotal))
    	{
    		PRINT("Err: get %s available size failed!\n", m_szDirectory);
			return E_FAIL;
    	}

		PRINT( "Info: llTotal = %lld\n", llTotal);
		
    	if (llTotalSize > 0 && llTotalSize < llTotal)
        {
			llTotal = llTotalSize;
        }

        LONGLONG llFree = (llTotal*RESERVED_PERSENTAGE)/1024/1024/1024;
        if (llFree < MIN_RESERVED_SIZE)
        {
            llFree = MIN_RESERVED_SIZE;
        }
        else if (llFree > MAX_RESERVED_SIZE)
        {
            llFree = MAX_RESERVED_SIZE;
        }
        llFree *= 1024*1024*1024;
        llTotal -= llFree;

        if (llTotal < 1024*1024*1024)
        {
            PRINT( "Err: disk %s is too small (%lldB), initailzie failed\n", szDirecory, llTotal);
            return E_FAIL;
        }
        m_dwOneFileSize = iFileSize;
        m_dwTotalFileCount = llTotal/iFileSize;

		m_fIsRecord = fRecord;

		PRINT( "Info: Path:%s,size:%dG, one file size:%dK,count:%d\n", m_szDirectory, INT(llTotal/1024/1024/1024), m_dwOneFileSize/1024, m_dwTotalFileCount);

		if (FAILED(CFileQueue::Initialize(m_dwTotalFileCount, m_dwOneFileSize, fRecord ? FQ_TYPE_RESULT : FQ_TYPE_VIDEO)))
    	{
    		PRINT( "Err: failed to initailzie FileQueue\n");
            return E_FAIL;
    	}
    }

    CHAR szPath[FILE_PATH_STRING_LEN + 32 + 1];
    swpa_sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_NAME);
    if (swpa_utils_file_exist(szPath))
    {
#if 0    
		CSWFile cFile;
        if (SUCCEEDED(cFile.Open(szPath, "r")))
        {
            //PRINT( "Info: open file:%s", szPath);
            if (FAILED(cFile.Read(m_pbIndexBuffer, INDEX_SIZE + 4, NULL)))
        	{
        		PRINT( "Err: read file:%s, failed\n", szPath);
                cFile.Close();
                return E_FAIL;
        	}
            cFile.Close();

			
            DWORD dwCrc32;
            swpa_memcpy(&dwCrc32, m_pbIndexBuffer, 4);
            BOOL fOk = FALSE;
            if (dwCrc32 != CSWUtils::CalcCrc32(0, m_pbIndexBuffer + 4, INDEX_SIZE))
            {
                PRINT( "Info: crc32 check failed in %s, use %s\n", INDEX_FILE_NAME, INDEX_FILE_BAK_NAME);
                swpa_sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_BAK_NAME);
                if (SUCCEEDED(cFile.Open(szPath, "r")))
                {
                    PRINT( "Info: open file:%s\n", szPath);
					cFile.Read(m_pbIndexBuffer, INDEX_SIZE + 4, NULL);
                    cFile.Close();

                    swpa_memcpy(&dwCrc32, m_pbIndexBuffer, 4);
                    if (dwCrc32 == CSWUtils::CalcCrc32(0, m_pbIndexBuffer + 4, INDEX_SIZE))
                    {
                        fOk = TRUE;
                    }
                }
            }
            else
            {
                fOk = TRUE;
            }
			
            if (fOk)
            {
                m_fInitialize = (S_OK == CFileQueue::Load(m_pbIndexBuffer + 4, INDEX_SIZE));
            }
        }
#else
		CSimpleFile cSimFile;
        if (SUCCEEDED(cSimFile.Open(szPath, "r")))
        {
            //PRINT( "Info: open file:%s", szPath);
            BOOL fOk = TRUE;
            if (FAILED(cSimFile.Read(m_pbIndexBuffer, INDEX_SIZE + 4)))
        	{
        		PRINT( "Err: read file:%s, failed\n", szPath);
                cSimFile.Close();
                fOk = FALSE;//return E_FAIL;
        	}
            cSimFile.Close();

			
            DWORD dwCrc32;
            swpa_memcpy(&dwCrc32, m_pbIndexBuffer, 4);
            
            if (!fOk 
				|| dwCrc32 != CSWUtils::CalcCrc32(0, m_pbIndexBuffer + 4, INDEX_SIZE))
            {
            	fOk = FALSE;
                PRINT( "Info: crc32 check failed in %s, use %s\n", INDEX_FILE_NAME, INDEX_FILE_BAK_NAME);
                swpa_sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_BAK_NAME);
                if (SUCCEEDED(cSimFile.Open(szPath, "r")))
                {
                    PRINT( "Info: open file:%s\n", szPath);
					cSimFile.Read(m_pbIndexBuffer, INDEX_SIZE + 4);
                    cSimFile.Close();

                    swpa_memcpy(&dwCrc32, m_pbIndexBuffer, 4);
                    if (dwCrc32 == CSWUtils::CalcCrc32(0, m_pbIndexBuffer + 4, INDEX_SIZE))
                    {
                        fOk = TRUE;
                    }
                }
            }
            else
            {
                fOk = TRUE;
            }
			
            if (fOk)
            {
                m_fInitialize = (S_OK == CFileQueue::Load(m_pbIndexBuffer + 4, INDEX_SIZE));
            }
        }
#endif
    }

    if (!IsValid())
    {
        m_fInitialize = (S_OK == CFileQueue::Load(NULL, 0));
    }
	
    PRINT( "Info: initialize %s\n", IsValid() ? "ok" : "failed");

    if (IsValid())
    {
#if BIG_FILE_DEBUG == 1
        CFileQueue::Print();
#endif
		
		m_pcCommittingThread = new CSWThread();
		if (NULL == m_pcCommittingThread
			|| FAILED(m_pcCommittingThread->Start(SaveTableProxy, this)))
		{
			SW_TRACE_NORMAL("Err: failed to create thread to save table\n");
		}
    }
	
    return IsValid() ? S_OK : E_FAIL;
}


VOID CBigFile::UnInitialize(void)
{
	m_fInitialize = FALSE;
	
	while (m_fUpdate)
	{
		SW_TRACE_NORMAL("m_fUpdate = %d\n", m_fUpdate);
		CSWApplication::Sleep(1000);
	}

	SAFE_RELEASE(m_pcCommittingThread);
	
	if (NULL != m_pcMemoryFactory)
	{
		if (NULL != m_pcMemIndexBuf)
		{
			m_pcMemoryFactory->Free(m_pcMemIndexBuf);
			m_pcMemIndexBuf = NULL;
			m_pbIndexBuffer = NULL;
		}

		if (NULL != m_pcMemMapTableBuf)
		{
			m_pcMemoryFactory->Free(m_pcMemMapTableBuf);
			m_pcMemMapTableBuf = NULL;
			m_pbMapTableBuffer = NULL;
		}

		m_pcMemoryFactory = NULL;
	}
}



/*
@note: be sure this function be called in critical section!
*/
HRESULT CBigFile::WriteFile(DWORD dwStartIndex, DWORD dwEndIndex,
	BYTE *pbBuf, INT iSize, BOOL fPrintLog)
{
	
	if (dwEndIndex < dwStartIndex)
	{
		dwEndIndex += m_dwTotalFileCount;
	}


	CHAR szPath[256] = {0};
	
	for (INT i = dwStartIndex; i <= dwEndIndex && iSize > 0; i++)
	{
		INT index = (i % m_dwTotalFileCount);
		swpa_sprintf(szPath, "%s/%d/%d/%d.dat", m_szDirectory, (index/1000), ((index%1000)/100), ((index%1000)%100));
		if (fPrintLog)
		{
			PRINT( "Info: write file:%s\n", szPath);
			/*PRINT( "Info: start index:%d, end index:%d, now index:%d, write file:%s\n", 
				(dwStartIndex % m_dwTotalFileCount), (dwEndIndex % m_dwTotalFileCount), index, szPath);*/
		}

		MakeSureDirectoryExists(szPath);

#if 0
        CSWFile cFile;
		if (SUCCEEDED(cFile.Open(szPath, "w+")))
		{
			if (FAILED(cFile.IOCtrl(SWPA_FILE_IOCTL_TRANCATE, &m_dwOneFileSize)))
			{
				PRINT("Err: failed to trancate %s to %d\n", szPath, m_dwOneFileSize);
				cFile.Close();
				return E_FAIL;
			}

			INT iSizeTemp = iSize;
			if (iSizeTemp > m_dwOneFileSize)
			{
				iSizeTemp = m_dwOneFileSize;
			}

			cFile.Seek(0, SWPA_SEEK_SET, NULL);
			
			DWORD dwTickPrev = CSWDateTime::GetSystemTick();
            cFile.Write(pbBuf, iSizeTemp, NULL);
			cFile.Close();
			DWORD dwTimeMs = CSWDateTime::GetSystemTick() - dwTickPrev;

			if (0 == dwTimeMs)
				dwTimeMs = 1;
			
			DOUBLE dblSpeed = 0.0;
			static DOUBLE dblAveSpeed = 0.0;
			static DWORD dwCount = 0;
			
			dblSpeed = m_dwOneFileSize*1000.0/1024.0/dwTimeMs; /* write m_dwOneFileSize per time, no matter iSizeTemp */
			dblAveSpeed = dblAveSpeed/(dwCount + 1)*dwCount + dblSpeed/(dwCount + 1);
			dwCount++;
			
			if (fPrintLog && 0 == dwCount % 20)
			{
	            PRINT("Info: Write %d Byte, cost %d ms. Speed: %.2lf KB/S, Ave Speed: %.2lf KB/S\n", m_dwOneFileSize, dwTimeMs, dblSpeed, dblAveSpeed);
			}
			
			iSize -= iSizeTemp;
			pbBuf += iSizeTemp;
		}
#else	
		CSimpleFile cSimFile;
		if (SUCCEEDED(cSimFile.Open(szPath, "w+")))
		{
			if (FAILED(cSimFile.Truncate(m_dwOneFileSize)))
			{
				PRINT("Err: failed to trancate %s to %d\n", szPath, m_dwOneFileSize);
				return E_FAIL;
			}

			INT iSizeTemp = iSize;
			if (iSizeTemp > m_dwOneFileSize)
			{
				iSizeTemp = m_dwOneFileSize;
			}
			
			DWORD dwTickPrev = CSWDateTime::GetSystemTick();
            if (FAILED(cSimFile.Write(pbBuf, iSizeTemp)))
        	{
        		PRINT("Err: failed to write %s\n", szPath);
				return E_FAIL;
        	}
			cSimFile.Close();
			DWORD dwTimeMs = CSWDateTime::GetSystemTick() - dwTickPrev;

			if (0 == dwTimeMs)
				dwTimeMs = 1;
			
			DOUBLE dblSpeed = 0.0;
			static DOUBLE dblAveSpeed = 0.0;
			static DWORD dwCount = 0;
			
			dblSpeed = m_dwOneFileSize*1000.0/1024.0/dwTimeMs; /* write m_dwOneFileSize per time, no matter iSizeTemp */
			dblAveSpeed = dblAveSpeed/(dwCount + 1)*dwCount + dblSpeed/(dwCount + 1);
			dwCount++;
			
			if (fPrintLog && 0 == dwCount % 20)
			{
	            PRINT("Info: Write %d Byte, cost %d ms. Speed: %.2lf KB/S, Ave Speed: %.2lf KB/S\n", m_dwOneFileSize, dwTimeMs, dblSpeed, dblAveSpeed);
			}
			
			iSize -= iSizeTemp;
			pbBuf += iSizeTemp;
		}
#endif			
		else
		{	
			PRINT("Err: failed to open %s \n", szPath);

			return E_FAIL;
		}
		m_fUpdate = TRUE;
	}

	Commit(FALSE, m_fIsRecord);	

	return S_OK;
}


/*
@note: be sure this function be called in critical section!
*/
HRESULT CBigFile::UpdateMapTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
	DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
	DWORD dwCarID)
{
	
	DWORD dwStartIndex = 0, dwEndIndex = 0;
	
	if (S_OK == CFileQueue::Read(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, 0/*dwMapTableCarID*/, dwStartIndex, dwEndIndex))
	{
		dwEndIndex = dwEndIndex < dwStartIndex ? dwEndIndex + m_dwTotalFileCount : dwEndIndex;
		INT iSize = (dwEndIndex - dwStartIndex + 1) * m_dwOneFileSize;;
		
		if (iSize < m_iMapTableSize)
		{
			PRINT("Err: get invalid table size (%d), less than %d\n", iSize, m_iMapTableSize);
			return E_FAIL;
		}

		if (S_OK == ReadFile(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, 0/*dwMapTableCarID*/, (PVOID)m_pbMapTableBuffer, m_iMapTableSize, FALSE))
		{
			UINT uiCRC = 0;
			INT iCount = 0;

			swpa_memcpy(&uiCRC, m_pbMapTableBuffer, sizeof(uiCRC));
			swpa_memcpy(&iCount, m_pbMapTableBuffer + sizeof(uiCRC), sizeof(iCount));

			if (uiCRC != CSWUtils::CalcCrc32(0, m_pbMapTableBuffer + sizeof(uiCRC), m_iMapTableSize - sizeof(uiCRC)))
			{
				PRINT("Err: failed to check and verify map table of %4d/%02d/%02d %02d, restore it to default state\n",
					dwYear, dwMonth, dwDay, dwHour);
				
				swpa_memset(m_pbMapTableBuffer, 0, m_iMapTableSize);
				UINT uiCRC = CSWUtils::CalcCrc32(0, m_pbMapTableBuffer + sizeof(UINT), m_iMapTableSize - sizeof(UINT));
				swpa_memcpy(m_pbMapTableBuffer, &uiCRC, sizeof(uiCRC));

				iCount = 0;
			}

			INT iOffset = sizeof(uiCRC) + sizeof(iCount) + iCount * sizeof(dwCarID);
			if (m_iMapTableSize > iOffset + sizeof(dwCarID))
			{
				swpa_memcpy(m_pbMapTableBuffer + iOffset, &dwCarID, sizeof(dwCarID));
				
				iCount ++;
				swpa_memcpy(m_pbMapTableBuffer + sizeof(uiCRC), &iCount, sizeof(iCount));
				
				uiCRC = CSWUtils::CalcCrc32(0, m_pbMapTableBuffer + sizeof(uiCRC), m_iMapTableSize - sizeof(uiCRC));
				swpa_memcpy(m_pbMapTableBuffer, &uiCRC, sizeof(uiCRC));

				
				if (FAILED(WriteFile(dwStartIndex, dwEndIndex, m_pbMapTableBuffer, m_iMapTableSize, FALSE)))
				{
					PRINT("Err: failed to write map table file\n");
					return E_FAIL;
				}
			}
		}

		PRINT("Info: update map table -- done\n");
		
		return S_OK;
	}

	PRINT("Err: failed to update map table\n");
	
	return E_FAIL;
}


HRESULT CBigFile::Write(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
	DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
	PVOID pvBuffer, INT iSize, BOOL fIsNormal)
{
    if (!IsValid())
    {
        return E_NOTIMPL;
    }
    
	m_Mutex.Lock();

	DWORD dwCarID = 0, dwStartIndex = 0, dwEndIndex = 0;
	
	if (m_fIsRecord)
	{
		INT iMapTableSize = 0;

		if (0 == CFileQueue::GetFileCount(dwYear, dwMonth, dwDay, dwHour))
		{
			DWORD dwTableCarID = 0;
			if (S_OK == CFileQueue::Write(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, m_iMapTableSize, dwTableCarID, dwStartIndex, dwEndIndex))
			{
				PRINT("Info: write Record MapTable file: %4d/%02d/%02d/%02d:%02d:%02d : dwCarID = %lu, dwStartIndex=%lu, dwEndIndex=%lu\n", 
					dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwTableCarID, dwStartIndex, dwEndIndex );

				swpa_memset(m_pbMapTableBuffer, 0, m_iMapTableSize);
				UINT uiCRC = CSWUtils::CalcCrc32(0, m_pbMapTableBuffer + sizeof(UINT), m_iMapTableSize - sizeof(UINT));
				swpa_memcpy(m_pbMapTableBuffer, &uiCRC, sizeof(uiCRC));
				
				if (FAILED(WriteFile(dwStartIndex, dwEndIndex, m_pbMapTableBuffer, m_iMapTableSize, FALSE)))
				{
					PRINT("Info: Failed to write file\n");
					m_Mutex.Unlock();
					return E_FAIL;
				}
			}
			else
			{
				PRINT("Err: failed to write map table!");
				m_Mutex.Unlock();
				return E_FAIL;
			}
		}
	}
	
    
    if (S_OK == CFileQueue::Write(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, iSize, dwCarID, dwStartIndex, dwEndIndex))
    {
    	PRINT("Info: write %s file: %4d/%02d/%02d/%02d:%02d:%02d : dwCarID = %lu, dwStartIndex=%lu, dwEndIndex=%lu\n", 
			m_fIsRecord ? "Record" : "Video", dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID, dwStartIndex, dwEndIndex );
        
		if (FAILED(WriteFile(dwStartIndex, dwEndIndex, (PBYTE)pvBuffer, iSize)))
		{
			PRINT("Info: Failed to write file\n");
			m_Mutex.Unlock();
			return E_FAIL;
		}

		if (m_fIsRecord && !fIsNormal)
		{
			PRINT("Info: Record has Peccancy, update the mapping table\n");
			UpdateMapTable(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID);
		}
        
        m_Mutex.Unlock();
		
        return S_OK;
    }
	
    
    m_Mutex.Unlock();
	
    return E_FAIL;
}



HRESULT CBigFile::GetNext(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
	DWORD dwHour, DWORD dwMinute, DWORD dwSecond,
	INT iType, DWORD * pdwCarID)
{
	if (!IsValid())
    {
        return E_NOTIMPL;
    }

	if (!m_fIsRecord)
	{
		return S_OK;
	}

	if (NULL == pdwCarID)
	{
		PRINT( "Err: NULL == pdwCarID\n");
		return E_INVALIDARG;
	}

	if (0 == iType)//0:全部输出;
	{
		//*pdwCarID += 1;
		return S_OK;
	}

	m_Mutex.Lock();
	
	if (FAILED(ReadFile(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, 0/*dwMapTableCarID*/, m_pbMapTableBuffer, m_iMapTableSize, FALSE)))
	{
		PRINT( "Err: failed to read mapping table of %04d-%02d-%02d %02d!\n", dwYear, dwMonth, dwDay, dwHour);
		m_Mutex.Unlock();
		return E_INVALIDARG;
	}

	UINT uiCRC = 0;
	INT iCount = 0;
	swpa_memcpy(&uiCRC, m_pbMapTableBuffer, sizeof(uiCRC));
	swpa_memcpy(&iCount, m_pbMapTableBuffer + sizeof(uiCRC), sizeof(iCount));

	if (uiCRC != CSWUtils::CalcCrc32(0, m_pbMapTableBuffer + sizeof(uiCRC), m_iMapTableSize - sizeof(uiCRC)))
	{
		PRINT("Err: failed to check and verify map table of %4d/%02d/%02d %02d, restore it to default state??\n",
			dwYear, dwMonth, dwDay, dwHour);
		
		//swpa_memset(m_pbMapTableBuffer, 0, m_iMapTableSize);
		//uiCRC = CSWUtils::CalcCrc32(0, m_pbMapTableBuffer + sizeof(UINT), m_iMapTableSize - sizeof(UINT));
		//swpa_memcpy(m_pbMapTableBuffer, &uiCRC, sizeof(uiCRC));

		//iCount = 0;
		m_Mutex.Unlock();
		return E_FAIL;
	}

	switch (iType)
	{
		//1:只输出违章车辆;
		case 1:
		{
			//todo: Binary-Search??
			//DWORD dwCarID = *pdwCarID + 1;
			for (INT i = 0; i < iCount; i++)
			{
				DWORD dwID = 0;	
				DWORD dwOffset = sizeof(uiCRC) + sizeof(iCount) + i * sizeof(DWORD);
				
				swpa_memcpy(&dwID, m_pbMapTableBuffer + dwOffset, sizeof(dwID));
				
				if (dwID >= *pdwCarID + 1)
				{
					*pdwCarID = dwID - 1;
					//PRINT("Info: dwID= %d\n", dwID);
					m_Mutex.Unlock();
					return S_OK;
				}
			}
			
			PRINT("Err: found no record \n");
			
			m_Mutex.Unlock();
			return E_RECORD_NONE;
		}
		break;

		//2:只输出非违章车辆;
		case 2:
		{
			DWORD dwPrevID = 0, dwMinID = 0;;
			swpa_memcpy(&dwMinID, m_pbMapTableBuffer + sizeof(uiCRC) + sizeof(iCount), sizeof(dwMinID));

			//PRINT("Info: Current CarID: %lu\n", *pdwCarID);
			//PRINT("Info: Min P-CarID: %lu\n", dwMinID);
			if (*pdwCarID < dwMinID - 1)
			{
				m_Mutex.Unlock();
				return S_OK;
			}

			dwPrevID = dwMinID;
			

			for (INT i = 1; i < iCount; i++)
			{
				DWORD dwID = 0;
				DWORD dwOffset = sizeof(uiCRC) + sizeof(iCount) + i * sizeof(DWORD);
				swpa_memcpy(&dwID, m_pbMapTableBuffer + dwOffset, sizeof(dwID));
				
				if (1 < dwID - dwPrevID)
				{
					//PRINT("Info: dwID = %d, dwPrevID = %d\n", dwID, dwPrevID);
					if (dwPrevID > *pdwCarID)
					{
						*pdwCarID = dwPrevID/* + 1 - 1*/;
						//PRINT("Info: *pdwCarID = %d\n", *pdwCarID + 1);
						m_Mutex.Unlock();
						return S_OK;
					}
					else if(*pdwCarID > dwPrevID &&  *pdwCarID + 1 < dwID)
					{
						//PRINT("Info: *pdwCarID = %d\n", *pdwCarID + 1);
						m_Mutex.Unlock();
						return S_OK;
					}
				}

				dwPrevID = dwID;
			}

			DWORD dwMaxID = 0;
			
			swpa_memcpy(&dwMaxID, m_pbMapTableBuffer + sizeof(uiCRC) + sizeof(iCount) + (iCount - 1) * sizeof(DWORD), sizeof(dwMaxID));
			PRINT("Info: Current CarID: %lu\n", *pdwCarID);
			PRINT("Info: Max P-CarID: %lu\n", dwMaxID);
			if (*pdwCarID == dwMaxID)
			{
				*pdwCarID = dwMaxID + 1;
				m_Mutex.Unlock();
				return S_OK;
			}
			else if (*pdwCarID < dwMaxID)
			{
				*pdwCarID = dwMaxID/*+ 1 - 1 */;//== (dwMaxID+1) - 1
				m_Mutex.Unlock();
				return S_OK;
			}
			/*else
			{
				m_Mutex.Unlock();
				return S_OK;
			}*/
			
		}
		break;

		default :
		break;
	}

	m_Mutex.Unlock();
	
	return S_OK;
}




HRESULT CBigFile::GetSize(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
	DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
	DWORD dwCarID, INT * piSize)
{
	if (!IsValid())
    {
        return E_NOTIMPL;
    }

	if (NULL == piSize)
	{
		PRINT( "Err: NULL == piSize \n");
		return E_INVALIDARG;
	}

	*piSize = 0;
	DWORD dwStartIndex=0, dwEndIndex=0;
	HRESULT hr = S_OK;

	if (m_fIsRecord)
	{
		dwCarID += 1;
	}
	
	m_Mutex.Lock();
	if (S_OK == CFileQueue::Read(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID, dwStartIndex, dwEndIndex))
	{
		dwEndIndex = dwEndIndex < dwStartIndex ? dwEndIndex + m_dwTotalFileCount : dwEndIndex;
		*piSize = (dwEndIndex - dwStartIndex + 1) * m_dwOneFileSize;
	}
	else
	{
		hr = dwCarID < GetFileCount(dwYear, dwMonth, dwDay, dwHour) ? E_FAIL : E_RECORD_NONE;
	}
	m_Mutex.Unlock();

	return hr;
}



HRESULT CBigFile::Read(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
	DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
	DWORD dwCarID, PVOID pvBuffer, INT iSize)
{
	m_Mutex.Lock();
	 
	HRESULT hr = ReadFile(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID + (m_fIsRecord ? 1 : 0), pvBuffer, iSize);

	m_Mutex.Unlock();

	return hr;
}




/*
@note: be sure this function be called in critical section!
*/
HRESULT CBigFile::ReadFile(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
	DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
	DWORD dwCarID, PVOID pvBuffer, INT iSize, BOOL fPrintLog)
{
    if (!IsValid())
    {
        return E_NOTIMPL;
    }
    
    
    DWORD dwStartIndex = 0, dwEndIndex = 0;
    if (S_OK == CFileQueue::Read(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID, dwStartIndex, dwEndIndex))
    {
    	if (fPrintLog)
		{
	        PRINT( "Info: read %s file: %04d-%02d-%02d %02d:%02d:%02d(%d)[%d-%d] \n", 
				m_fIsRecord ? "Record" : "Video", dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID, dwStartIndex, dwEndIndex);
		}
        if (dwEndIndex < dwStartIndex)
        {
            dwEndIndex += m_dwTotalFileCount;
        }

        CHAR szPath[255];

        BYTE *pbBuf = (BYTE *)pvBuffer;
        for (INT i = dwStartIndex; i <= dwEndIndex && iSize > 0; i++)
        {
            INT index = (i % m_dwTotalFileCount);
            swpa_sprintf(szPath, "%s/%d/%d/%d.dat", m_szDirectory, (index/1000), ((index%1000)/100), ((index%1000)%100));

#if 0
            CSWFile cFile;
            if (SUCCEEDED(cFile.Open(szPath, "r")))
            {
            	if (fPrintLog)
        		{
                    PRINT( "Info: read file:%s \n", szPath);
                    //PRINT( "Info: start index:%d, end index:%d, now index:%d, read file:%s \n", dwStartIndex, (dwEndIndex % m_dwTotalFileCount), i, szPath);
                }
				INT iSizeTemp = iSize;
                if (iSizeTemp > m_dwOneFileSize)
                {
                    iSizeTemp = m_dwOneFileSize;
                }
				

				DWORD dwTickPrev = CSWDateTime::GetSystemTick();
				cFile.Read(pbBuf, iSizeTemp, NULL);
				cFile.Close();
				DWORD dwTimeMs = CSWDateTime::GetSystemTick() - dwTickPrev;
				
				DOUBLE dblSpeed = 0.0;
				static DOUBLE dblAveSpeed = 0.0;
				static DWORD dwCount = 0;

				if (0 == dwTimeMs)
					dwTimeMs = 1;
				
				dblSpeed = iSizeTemp*1000.0/1024.0/dwTimeMs; /* only read data of size iSizeTemp */
				dblAveSpeed = dblAveSpeed/(dwCount + 1)*dwCount + dblSpeed/(dwCount + 1);
				dwCount++;
				if (fPrintLog && 0 == dwCount % 20)
				{
	            	PRINT("Info: Read %d Byte, cost %d ms. Speed: %.2lf KB/S, Ave Speed: %.2lf KB/S\n", iSizeTemp, dwTimeMs, dblSpeed, dblAveSpeed);
				}
				
                iSize -= iSizeTemp;
                pbBuf += iSizeTemp;
            }
#else
			CSimpleFile cSimFile;
            if (SUCCEEDED(cSimFile.Open(szPath, "r")))
            {
            	if (fPrintLog)
        		{
                    PRINT( "Info: read file:%s \n", szPath);
                    //PRINT( "Info: start index:%d, end index:%d, now index:%d, read file:%s \n", dwStartIndex, (dwEndIndex % m_dwTotalFileCount), i, szPath);
                }
				INT iSizeTemp = iSize;
                if (iSizeTemp > m_dwOneFileSize)
                {
                    iSizeTemp = m_dwOneFileSize;
                }
				

				DWORD dwTickPrev = CSWDateTime::GetSystemTick();
				if (FAILED(cSimFile.Read(pbBuf, iSizeTemp)))
				{
					SW_TRACE_DEBUG("Err: failed to read %s.\n", szPath);
					return E_FAIL;
				}
				cSimFile.Close();
				DWORD dwTimeMs = CSWDateTime::GetSystemTick() - dwTickPrev;
				
				DOUBLE dblSpeed = 0.0;
				static DOUBLE dblAveSpeed = 0.0;
				static DWORD dwCount = 0;

				if (0 == dwTimeMs)
					dwTimeMs = 1;
				
				dblSpeed = iSizeTemp*1000.0/1024.0/dwTimeMs; /* only read data of size iSizeTemp */
				dblAveSpeed = dblAveSpeed/(dwCount + 1)*dwCount + dblSpeed/(dwCount + 1);
				dwCount++;
				if (fPrintLog && 0 == dwCount % 20)
				{
	            	PRINT("Info: Read %d Byte, cost %d ms. Speed: %.2lf KB/S, Ave Speed: %.2lf KB/S\n", iSizeTemp, dwTimeMs, dblSpeed, dblAveSpeed);
				}
				
                iSize -= iSizeTemp;
                pbBuf += iSizeTemp;
            }
#endif
            else
            {
                return dwCarID < GetFileCount(dwYear, dwMonth, dwDay, dwHour) ? E_FAIL : E_RECORD_NONE;
            }

        }
		
        return S_OK;
    }

	
    m_Mutex.Unlock();
	
    return dwCarID < GetFileCount(dwYear, dwMonth, dwDay, dwHour) ? E_FAIL : E_RECORD_NONE;
}



DWORD CBigFile::GetFileCount(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour)
{
    m_Mutex.Lock();
    DWORD dwCnt = CFileQueue::GetFileCount(dwYear, dwMonth, dwDay, dwHour);
    m_Mutex.Unlock();

	if (m_fIsRecord && dwCnt > 0)
	{
		dwCnt--; //the map table is not a Valid Record
	}
    
    return dwCnt;
}

HRESULT CBigFile::Commit(BOOL fForce, BOOL fIsRecord)
{
    m_Mutex.Lock();
    if (!IsValid())
    {
        m_Mutex.Unlock();
        return E_NOTIMPL;
    }

    if (m_fUpdate)
    {
        if( fIsRecord )
        {
            m_dwCommitCount = (m_dwCommitCount + 1)% COMMIT_COUNT;
        }
		
		//PRINT("Info: commit filequeue table\n");
        if (((fIsRecord && !m_dwCommitCount) || CSWDateTime::GetSystemTick() - m_dwLastCommitTime > COMMIT_INTERVAL || fForce)
                && S_OK == CFileQueue::Commit(m_pbIndexBuffer + 4, INDEX_SIZE))
        {
            PRINT( "Info: m_dwCommitCount:%d, m_dwLastCommitTime:%d, now:%d,fForce:%d\n", m_dwCommitCount, m_dwLastCommitTime, CSWDateTime::GetSystemTick(), fForce);

            m_dwCommitCount = 0;
            m_dwLastCommitTime = CSWDateTime::GetSystemTick();
            
            DWORD dwCrc32 = CSWUtils::CalcCrc32(0, m_pbIndexBuffer + 4, INDEX_SIZE);
            swpa_memcpy(m_pbIndexBuffer, &dwCrc32, 4);

            CHAR szPath[255];
            swpa_sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_NAME);

#if 0
			CSWFile cFile;
            if (SUCCEEDED(cFile.Open(szPath, "w+")))
            {
                PRINT( "Info: save index to %s\n", szPath);
                if (FAILED(cFile.Write(m_pbIndexBuffer, INDEX_SIZE + 4, NULL)))
            	{
            		PRINT( "Err: failed to write %s\n", szPath);
					m_Mutex.Unlock();
					return E_FAIL;
            	}
				else
				{
					cFile.Close();
					PRINT( "Info: save index to %s -- OK\n", szPath);
				}
            }
            m_dwMasteCommitCount = (m_dwMasteCommitCount + 1)% MASTER_COMMIT_COUNT;
            if (fForce || !m_dwMasteCommitCount)
            {
                swpa_sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_BAK_NAME);
                if (SUCCEEDED(cFile.Open(szPath, "w+")))
                {
                    PRINT( "Info: save index to %s\n", szPath);
                    if (FAILED(cFile.Write(m_pbIndexBuffer, INDEX_SIZE + 4, NULL)))
                	{
                		PRINT( "Err: failed to write %s\n", szPath);
						m_Mutex.Unlock();
						return E_FAIL;
                	}
                    cFile.Close();
					PRINT( "Info: save index to %s -- OK\n", szPath);
                }
            }
#else
			CSimpleFile cSimFile;
            if (SUCCEEDED(cSimFile.Open(szPath, "w")))
            {
                //PRINT( "Info: save index to %s\n", szPath);
                if (FAILED(cSimFile.Write(m_pbIndexBuffer, INDEX_SIZE + 4)))
            	{
            		PRINT( "Err: failed to write %s\n", szPath);
					m_Mutex.Unlock();
					return E_FAIL;
            	}
				else
				{
					cSimFile.Close();
					PRINT( "Info: save index to %s -- OK\n", szPath);
				}
            }
            m_dwMasteCommitCount = (m_dwMasteCommitCount + 1)% MASTER_COMMIT_COUNT;
            if (fForce || !m_dwMasteCommitCount)
            {
                swpa_sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_BAK_NAME);
                if (SUCCEEDED(cSimFile.Open(szPath, "w")))
                {
                    //PRINT( "Info: save index to %s\n", szPath);
                    if (FAILED(cSimFile.Write(m_pbIndexBuffer, INDEX_SIZE + 4)))
                	{
                		PRINT( "Err: failed to write %s\n", szPath);
						m_Mutex.Unlock();
						return E_FAIL;
                	}
                    cSimFile.Close();
					PRINT( "Info: save index to %s -- OK\n", szPath);
                }
            }
#endif
            m_iCommitCount++;
            if( m_iCommitCount > 10 )
            {
                m_iCommitCount = 0;
#if BIG_FILE_DEBUG == 1
                Print();
#endif
            }
            m_Mutex.Unlock();

			m_fUpdate = FALSE;
			
            return S_OK;
        }
    }
	
    m_Mutex.Unlock();
    
    return S_OK;
}

#if 0
HRESULT CBigFile::MakeSureDirectoryExists(const CHAR* pszPath)
{
	if (NULL == pszPath)
	{
		PRINT("Err: NULL == pszPath");
		return E_INVALIDARG;
	}

	CHAR szPathTemp[256] = {0};


	if (SWPAR_OK != swpa_utils_path_getdirname(pszPath, szPathTemp)
		|| 0 == swpa_strcmp(szPathTemp, ""))
	{
		return E_FAIL;
	}

	//create the parent directorys recursively
	if (SWPAR_OK != swpa_utils_dir_exist(szPathTemp))
	{
		if (SWPAR_OK != swpa_utils_dir_create(szPathTemp))
		{
			INT iStrlen = swpa_strlen(szPathTemp);
			if (0 >= iStrlen)
			{
				return E_FAIL;
			}
			szPathTemp[iStrlen-1] = '\0';
			MakeSureDirectoryExists(szPathTemp);
			
			return (SWPAR_OK == swpa_utils_dir_create(szPathTemp)) ? S_OK : E_FAIL; 
		}
		
	}

	return S_OK;
}
#else
HRESULT CBigFile::MakeSureDirectoryExists(const char* DirPath)
{
    if ( NULL == DirPath )
    {
        return false;
    }

    CSWString strPath(DirPath);
    int iSlashPos = 0;
    struct stat st;
    while ( true )
    {
        iSlashPos = strPath.Find("/", iSlashPos);
        if ( -1 == iSlashPos )
        {
            break;
        }
        else
        {
            if (stat((LPCSTR)strPath.Substr(0, iSlashPos+1), &st) >= 0 && S_ISDIR(st.st_mode))
            {
                ++iSlashPos;
                continue;
            }
            if ( 0 != mkdir((LPCSTR)strPath.Substr(0, iSlashPos+1), 0777) )
            {
                if ( 17 != errno )  // 17: File exists
                {
                    perror("mkdir");
                    return false;
                }
            }
            ++iSlashPos;
        }
    }

    return true;
}
#endif


PVOID CBigFile::SaveTableProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		return (PVOID)E_INVALIDARG;
	}

	((CBigFile*)pvArg)->SaveTable();
}


HRESULT CBigFile::SaveTable()
{
	DWORD dwPrevTick = CSWDateTime::GetSystemTick();

	SW_TRACE_DEBUG("Info: %s() started\n", __FUNCTION__);
	
	while (IsValid() || m_fUpdate)
	{
		if (!IsValid())
		{
			static DWORD dwCount = 0;
			if (FAILED(Commit(FALSE, FALSE)))
			{
				SW_TRACE_NORMAL("Err: failed to save table, trying again!!\n");
				dwCount ++;
			}
			else
			{
				dwCount = 0;
			}

			if (3 < dwCount)
			{
				SW_TRACE_NORMAL("Err: failed to save table, discards it\n");
				m_fUpdate = FALSE;
				break;
			}
		}
		
		DWORD dwTick = CSWDateTime::GetSystemTick();
	
		if (dwTick > dwPrevTick 
			&& dwTick - dwPrevTick > 60000)	//every minute
		{
			if (FAILED(Commit(FALSE, FALSE)))
			{
				SW_TRACE_NORMAL("Err: failed to save table!!\n");
			}
			else
			{
				dwPrevTick = dwTick;
			}
		}
		
		CSWApplication::Sleep(1000);
	}

	SW_TRACE_DEBUG("Info: %s() exited\n", __FUNCTION__);
	return S_OK;
}


