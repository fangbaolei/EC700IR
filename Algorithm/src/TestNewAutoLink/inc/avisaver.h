#ifndef _INCLUDE_AVISAVER_H_
#define _INCLUDE_AVISAVER_H_

#include "resultsend.h"

////////////////////////////////////////////////////////////
//AVI安全保存接口(自维护),确保数据的完整
////////////////////////////////////////////////////////////


class CAviSaverData
{
public:
	FILE *pFile;
	//char szFileName[MAX_PATH];
	long nFileLen;
	int nBlocks;				// 总块数
	int nCurPos;				// 当前块发送位置
	bool fBegin;				// 已经开始发送
	unsigned short m_wYear;					// 年
	unsigned short m_wMon;					// 月
	unsigned short m_wDay;					// 日
	unsigned short m_wHour;					// 时
	unsigned short m_wMin;					// 分
	unsigned short m_wSec;					// 秒
	unsigned int m_dwAviLen;				// 长度
	DWORD64 dwAviTime;						// 时间长度

	CAviSaverData()
		: pFile(NULL)
	{
		//memset( pAvi, 0, sizeof(HVIO_Video));
	}

	~CAviSaverData()
	{
		//if (pFile != NULL)
		//{
		//	fclose(pFile);
		//	pFile = NULL;
		//}
	}

	CAviSaverData &operator =(const CAviSaverData &data)
	{
		pFile = data.pFile;
		nFileLen = data.nFileLen;

		m_wYear = data.m_wYear;					// 年
		m_wMon = data.m_wMon;					// 月
		m_wDay = data.m_wDay;					// 日
		m_wHour = data.m_wHour;					// 时
		m_wMin = data.m_wMin;					// 分
		m_wSec = data.m_wSec;					// 秒
		m_dwAviLen = data.m_dwAviLen;			// Avi时长
		dwAviTime = data.dwAviTime;

		//if( strlen(data.szFileName) > 0 )
		//{
		//	HV_memcpy(szFileName, data.szFileName, strlen(data.szFileName) + 1);
		//}

		return *this;
	}
};

class IAviSaver
{
public:
	virtual bool Init(const char* pszIniFile) = 0;
	virtual char* GetSavePath(__time64_t lTime, int nLen) = 0;						// 根据时间取保存路径
	virtual HRESULT GetAviFileByTime(__time64_t lTime, CAviSaverData* pData) = 0;

	virtual HRESULT SetCurWriteFile(char *pszWriteFile) = 0;						// 设置当前正在写的文件
	virtual HRESULT ClearCurWriteFile() = 0;										// 清除当前正在写的文件

	virtual HRESULT GetDiskStatus() = 0;
	virtual bool ThreadIsOk() = 0;

	//virtual HRESULT Init(LPCSTR lpszIniFile) = 0;
	//virtual HRESULT SaveData(CAviSaverData* pData) = 0;
	//virtual HRESULT GetPlateRecord(DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int& index,  int iType, CAviSaverData* pData) = 0;
	//virtual HRESULT GetDiskStatus() = 0;
	//virtual bool ThreadIsOk() = 0;
};

extern HRESULT GetAviSaver( IAviSaver** ppAviSaver );

#endif

