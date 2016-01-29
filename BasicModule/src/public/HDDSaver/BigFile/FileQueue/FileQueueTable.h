/**
* @file FileQueueTable.h 
* @brief CFileQueue is used to save the result/video file mapping table
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/


#ifndef _FILEQUEUE_TABLE_H_
#define _FILEQUEUE_TABLE_H_

#include "SWList.h"
#include "SWFC.h"



typedef struct _STORAGEINFO
{
    WORD wRepeatTimes;
    WORD wBlockCnt;
} STORAGEINFO;


typedef struct _RECORDNODE{
    DWORD dwYear;
    DWORD dwMonth;
    DWORD dwDay;
    DWORD dwHour;

    //block range in HDD
    DWORD dwDataStartID;
    DWORD dwDataEndID;

    //file range
    DWORD dwFileStartIndex;
    DWORD dwFileEndIndex;

    //记录file与block对应关系的压缩表
    //用[占用块数相同的文件个数][占用的块数]这样的结构进行压缩存储
    CSWList<STORAGEINFO*, 4096> lstInfo;
    DWORD dwInfoLen;

}RECORDNODE;


class CFileQueueTable
{

public:
	
    CFileQueueTable(){}
    virtual ~CFileQueueTable(){};
    virtual HRESULT LoadTable(BYTE* pbBuf, const DWORD dwLen);

    virtual HRESULT SaveTable(BYTE* pbBuf, const DWORD dwLen);

    virtual HRESULT PrintTable();

    virtual HRESULT ReadTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay,
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond,
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)=0;

    virtual HRESULT WriteTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay,
               DWORD dwHour, DWORD dwMinute, DWORD dwSecond,
               DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)=0;

    virtual DWORD GetFileCount(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour)=0;

	
	DWORD GetTotalCount()
	{
		return m_dwTotalBlockCount;
	}
	
	
	DWORD GetAvailableCount()
	{
		return m_dwRemained;
	}
	
	
	DWORD GetOffset()
	{
		return m_dwOffset;
	}

protected:

    HRESULT Clear();

    DWORD m_dwTRICKNUMBER;// = 0xEFDCCDFE;  标志一条记录的开始

    DWORD m_dwTotalBlockCount; // 块总数
    DWORD m_dwOneBlockSize;    //单块大小
    DWORD m_dwRemained;       //剩余可用块数
    DWORD m_dwOffset;         //下一个可用块的偏移量
    DWORD m_dwThreshHold;     //预留，目前无用

    CSWList<RECORDNODE*, 40960> m_lstRecords; //保存记录的双向表
    //DWORD m_dwRecordLen;    //表长，可换由调用deque.size()获取

};


#endif //_FILEQUEUE_TABLE_H_


