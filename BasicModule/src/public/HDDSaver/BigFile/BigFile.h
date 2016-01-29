/**
* @file BigFile.h 
* @brief CBigFile is used to read/write the result/video files
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/

#ifndef __BIGFILE_H__
#define __BIGFILE_H__

//#include "SWFC.h"
#include "FileQueue.h"



#define FILE_PATH_STRING_LEN 255


#ifndef E_RECORD_NONE
#define E_RECORD_NONE 0x80008901
#endif

#ifndef E_RECORD_WRITING
#define E_RECORD_WRITING 0x80008902
#endif

#ifndef E_RECORD_WAIT_TIMEOUT
#define E_RECORD_WAIT_TIMEOUT 0x80008903
#endif




class CBigFile : public CFileQueue
{
public:
    CBigFile();
	
    virtual ~CBigFile();
	
    HRESULT Initialize(const CHAR* szDirecory, LONGLONG llTotalSize = 0, INT iFileSize = 1024*1024, BOOL fRecord = TRUE);
	
    VOID UnInitialize(VOID);
	
    BOOL IsValid(VOID)
    {
        return m_fInitialize;
    }
	
    HRESULT Write(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour, DWORD dwMinute, DWORD dwSecond, PVOID pvBuffer, INT iSize, BOOL fIsNormal = TRUE);
	
    HRESULT Read(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour, DWORD dwMinute, DWORD dwSecond, DWORD dwCarID, PVOID pvBuffer, INT iSize);

	HRESULT GetSize(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour, DWORD dwMinute, DWORD dwSecond, DWORD dwCarID, INT * piSize);

	HRESULT GetNext(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour, DWORD dwMinute, DWORD dwSecond, INT iType, DWORD * pdwCarID);
	
	DWORD GetFileCount(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour);

	HRESULT Commit(BOOL fForce, BOOL fIsRecord);

	DWORD GetSingleFileSize()
	{
		return m_fInitialize ? m_dwOneFileSize : 0;
	}

private:
	
	HRESULT MakeSureDirectoryExists(const CHAR* pszPath);

	
	HRESULT UpdateMapTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
		DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
		DWORD dwCarID);

	
	HRESULT WriteFile(DWORD dwStartIndex, DWORD dwEndIndex,
		BYTE *pbBuf, INT iSize, BOOL fPrintLog = TRUE);
	

	HRESULT ReadFile(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
		DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
		DWORD dwCarID, PVOID pvBuffer, INT iSize, BOOL fPrintLog = TRUE);

	static PVOID SaveTableProxy(PVOID pvArg);

	HRESULT SaveTable();
	
private:
    //pthread_mutex_t m_mutex;
	CSWMutex	m_Mutex; 
    BOOL 		m_fInitialize;
    CHAR 		m_szDirectory[FILE_PATH_STRING_LEN+1];
    DWORD 		m_dwOneFileSize;
    DWORD 		m_dwTotalFileCount;
    PBYTE 		m_pbIndexBuffer;
    DWORD 		m_dwCommitCount;
    DWORD 		m_dwMasteCommitCount;
    DWORD 		m_dwLastCommitTime;
    BOOL    	m_fUpdate;
    INT 		m_iCommitCount;
	BOOL 		m_fIsRecord;
	INT			m_iMapTableSize;
	PBYTE		m_pbMapTableBuffer;

	CSWMemoryFactory * m_pcMemoryFactory;
	CSWMemory*	m_pcMemIndexBuf;
	CSWMemory*	m_pcMemMapTableBuf;

	CSWThread*	m_pcCommittingThread;
};
#endif


