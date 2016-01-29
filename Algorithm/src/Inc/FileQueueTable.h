
#ifndef _FILEQUEUE_TABLE_H_
#define _FILEQUEUE_TABLE_H_

#include "hvutils.h"
#include <deque>


typedef struct _STORAGEINFO
{
    WORD16 wRepeatTimes;
    WORD16 wBlockCnt;
} STORAGEINFO;


typedef struct _RECORDNODE{
    DWORD32 dwYear;
    DWORD32 dwMonth;
    DWORD32 dwDay;
    DWORD32 dwHour;

    //block range in HDD
    DWORD32 dwDataStartID;
    DWORD32 dwDataEndID;

    //file range
    DWORD32 dwFileStartIndex;
    DWORD32 dwFileEndIndex;

    //记录file与block对应关系的压缩表
    //用[占用块数相同的文件个数][占用的块数]这样的结构进行压缩存储
    std::deque<STORAGEINFO*> dqInfo;
    DWORD32 dwInfoLen;

}RECORDNODE;


class CFileQueueTable
{
public:
    CFileQueueTable(){}
    virtual ~CFileQueueTable(){};
    virtual HRESULT LoadTable(BYTE8* pbBuf, const DWORD32 dwLen);

    virtual HRESULT SaveTable(BYTE8* pbBuf, const DWORD32 dwLen);

    virtual HRESULT PrintTable();

    virtual HRESULT ReadTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay,
                   DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond,
                   DWORD32 dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)=0;

    virtual HRESULT WriteTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay,
               DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond,
               DWORD32 dwSize, DWORD32 &dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)=0;

    virtual DWORD32 GetFileCount(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour)=0;


protected:

    HRESULT Clear();

    DWORD32 m_dwTRICKNUMBER;// = 0xEFDCCDFE;  标志一条记录的开始

    DWORD32 m_dwTotalBlockCount; // 块总数
    DWORD32 m_dwOneBlockSize;    //单块大小
    DWORD32 m_dwRemained;       //剩余可用块数
    DWORD32 m_dwOffset;         //下一个可用块的偏移量
    DWORD32 m_dwThreshHold;     //预留，目前无用

    std::deque<RECORDNODE*> m_dqRecords; //保存记录的双向表
    //DWORD32 m_dwRecordLen;    //表长，可换由调用deque.size()获取

};


#endif //_FILEQUEUE_TABLE_H_

