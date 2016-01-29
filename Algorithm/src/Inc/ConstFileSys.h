// 该文件编码必须是WINDOWS-936格式
#ifndef _CONST_FILE_SYS_H_
#define _CONST_FILE_SYS_H_

#include <vector>
#ifdef WIN32
#include <vcl.h>
#else
#include "hvutils.h"
#endif

#ifdef WIN32
#ifndef WORD16
typedef unsigned short WORD16;
#endif
#ifndef DWORD32
typedef unsigned int DWORD32;
#endif
#endif

typedef HRESULT (*CF_ReadFile)(char* szFileName, char* szBuffer, DWORD32 dwBufLen, DWORD32& dwDataLen);
typedef HRESULT (*CF_WriteFile)(char* szFileName, char* szData, DWORD32 dwDataLen);
typedef HRESULT (*CF_DeleteFile)(char* szFileName);
typedef HRESULT (*CF_DeleteFolder)(char* szPath);


//系统表
struct SSysTable
{
    DWORD32         dwMaxFileCount;         //最大文件数量
    DWORD32         dwMaxFileSize;          //一个文件最大大小
    DWORD32         dwFileBegin;            //该数据类型的第一个文件起始编号
    DWORD32         dwExtend1;              //扩展字段1

    SSysTable()
    {
        memset(this, 0, sizeof(*this));
    }
};

//定长文件系统的小时索引表
struct SHourIndex
{
    DWORD32         dwHour;                 //小时，比如11102213表示2011年10月23号13时
    DWORD32         dwFileIndex;            //在该数据类型的文件系统中的index
    DWORD32         dwHourIndex;            //在该小时中的index,非特殊情况下都为0
    DWORD32         dwCount;                //该小时的文件数量

    SHourIndex()
    {
        memset(this, 0, sizeof(*this));
    }
};

//一个数据项
struct SCfItem
{
    DWORD32         dwTimeLow;
    DWORD32         dwTimeHigh;
    DWORD32         dwIndex;

    SCfItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

//小时信息
struct SHourInfo
{
    DWORD32         dwHour;                 //小时，比如11102213表示2011年10月23号13时
    unsigned char   szInfo[4096];

    SHourInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};


//初始化结构
struct SConstFileSysParam
{
    bool            fFormat;                //是否格式化
    DWORD32         dwMaxFileCount;         //格式化时,设置最大文件数量
    DWORD32         dwMaxFileSize;          //格式化时,设置一个文件最大大小
    char            szRootDir[MAX_PATH];    //存储系统的根目录
    DWORD32         dwFileBegin;            //该数据类型的第一个文件起始编号
    char            szDataType[8];          //数据类型 record, video
    CF_ReadFile     fnReadFile;             //读函数
    CF_WriteFile    fnWriteFile;            //写函数
    CF_DeleteFile   fnDeleteFile;           //删除函数
    CF_DeleteFolder fnDeleteFolder;         //删除目录的函数
    bool            fHourFix; 
    bool            fHourInfo;				  //是否要记录小时的记录信息

    SConstFileSysParam()
    {
        memset(this, 0, sizeof(*this));
    }
};

//定长文件系统
class CConstFileSys
{
public:
    CConstFileSys();
    ~CConstFileSys();

    //初始化
    HRESULT Init(const SConstFileSysParam* pParam);

    //根据时间存储文件
    HRESULT WriteFile(const DWORD32 dwTimeLow
                     ,const DWORD32 dwTimeHigh
                     ,const DWORD32 dwDataInfo          //数据信息(1:违章数据)
                     ,DWORD32& dwIndex
                     ,char* szBuf
                     ,const DWORD32 dwDataLen
                     ,const bool fForceNewHour = false
                     );

    //根据时间读取文件
    HRESULT ReadFile(const DWORD32 dwTimeLow
                    ,const DWORD32 dwTimeHigh
                    ,const DWORD32 dwDataInfo          //数据信息(1:违章数据)
                    ,DWORD32& dwIndex
                    ,char* szBuf
                    ,const DWORD32 dwBufLen
                    ,DWORD32& dwDataLen
                    );

    //获取某个小时的文件数量
    DWORD32 GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh);

	//根据当前项获取下一个项
    HRESULT GetNextItem(const SCfItem& sCfItem, SCfItem& sCfItemNext);

    //获取存储系统状态
    HRESULT GetStatus();

    //获取系统表
    const SSysTable& GetSysTable() const;

    //获取小时索引
    const std::vector<SHourIndex>& GetHourIndex() const;

    //未提交的个数大于dwMinCount时候，提交索引相关的表
    HRESULT Submit(const DWORD32 dwMinCount);

protected:

	//更新小时索引表等
	HRESULT Update();
	
    //更新小时索引表
    HRESULT UpdateHourIndex();

    //更新小时索引备份表
    HRESULT UpdateHourIndexBak();

    //根据系统文件索引等到文件路径
    HRESULT GetFileByIndex(const DWORD32 dwFileIndex
                          ,char* szBuf
                          ,const DWORD32 dwBufLen
                          );

    //转换时间为具体的小时数
    DWORD32 TimeToHour(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh);

    //当存储满后，删除第一个小时的结果
    HRESULT DeleteFirstHour();

    //检查系统表是否正常
    HRESULT CheckSysTable();

	//清空
    HRESULT Clear(); 

    //根据小时获取小时信息文件
    HRESULT GetHourInfoFile(const DWORD32 dwHour
    					   ,const bool fBak
                           ,char* szBuf
                           ,const DWORD32 dwBufLen
                           );
                           
    //加载某个小时的信息
    HRESULT GetHourInfo(const DWORD32 dwHour, SHourInfo& sHourInfo);

	//更新当前操作小时的信息
    HRESULT UpdateHourInfo();

private:
    bool                                    m_fSysOk;                       //文件系统状态正常
    std::vector<SHourIndex>                 m_listHourIndex;                //小时索引
    SSysTable                               m_sSysTable;                    //系统表
    bool                                    m_fHourFix;                     //小时的数据固定  
    bool                                    m_fHourInfo;				     //是否要记录小时的记录信息
    SHourInfo                               m_sWriteHourInfo;              //当前写小时的信息
    SHourInfo                               m_sReadHourInfo;               //当前读小时的信息
	DWORD32                                 m_dwLastCount;                 //记录从上次提交索引表到现在的记录个数
	
    char                                    m_szDataDir[MAX_PATH];          //保存数据的根目录
    char                                    m_szLogDir[MAX_PATH];           //保存索引和日志的跟目录
    char                                    m_szHourIndexFile[MAX_PATH];    //小时索引表文件
    char                                    m_szHourIndexFileBak[MAX_PATH]; //小时索引表的备份文件
    char                                    m_szSysTableFile[MAX_PATH];     //系统表文件

	DWORD32                                 m_dwBufLen;                      //缓存大小
    char*                                   m_pBuffer;                       //读写缓存
    CF_ReadFile                             m_fnReadFile;
    CF_WriteFile                            m_fnWriteFile;
    CF_DeleteFile                           m_fnDeleteFile;

};

#endif
