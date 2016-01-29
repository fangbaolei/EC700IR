#ifndef _SAFESAVER_H_
#define _SAFESAVER_H_

#include "resultsend.h"
#include "CameraLinkBase.h"

//-------------------------------------
//安全保存接口(自维护),确保数据的完整
//-------------------------------------

#ifndef E_RECORD_NONE
#define E_RECORD_NONE 0x80008901
#endif

#ifndef E_RECORD_WRITING
#define E_RECORD_WRITING 0x80008902
#endif

#ifndef E_RECORD_WAIT_TIMEOUT
#define E_RECORD_WAIT_TIMEOUT 0x80008903
#endif

#ifndef ROOT_DIR
#define ROOT_DIR "/Result_disk"         //硬盘的挂载路径
#endif

#define FIX_FILE_VER 3

//一个存储单位结构
struct SCfData
{
	int                   iDataType;    //数据类型 0:识别结果 1:录像
	DWORD32			      dwTimeLow;
	DWORD32 			  dwTimeHigh;
	DWORD32               dwDataInfo;   //数据信息，比如违章的数据
	CCameraDataReference* pRefData;     //数据引用
	DWORD32               dwIndex;      //小时索引
	DWORD32               dwDataLen;    //数据大小

	SCfData()
	{
		memset(this, 0, sizeof(*this));
	}
};

const DWORD32 MAX_BIG_IMG_COUNT = 5;  //大图数量最大值

class CDetectData
{
public:
    HVIO_String* pString;
    CDetectData()
    {
        pString = new HVIO_String;
        memset(pString, 0, sizeof(HVIO_String));
    }
    ~CDetectData()
    {
        SAFE_DELETE(pString);
    }
};

//初始化参数
struct SSafeSaveParam
{
    bool    fFormat;                //格式化
    bool    fSaveRecord;
    bool    fSaveVideo;
    bool    fCoverCtrl;
    int     iNetDiskType;           //网络硬盘的类型
    char    szNetDiskIP[32];        //网络存储的IP
    char    szFtpRoot[32];          //FTP root dir
    char    szNetDiskUser[32];      //网络存储的用户
    char    szNetDiskPwd[32];
    int     iNetDiskSpace;
    char    szNFS[32];              //网络硬盘NFS路径
    char    szNFSParam[255];        //网络硬盘挂载的参数
    int     iFileSize;              //单个文件的大小

    SSafeSaveParam()
    {
        memset(this, 0, sizeof(*this));
        iFileSize = 512;
        strcpy(szNFSParam, "-o timeo=1,soft,tcp,nolock,retry=1");
    }
};

//安全保存接口
class ISafeSaver
{
public:
    virtual HRESULT Init(const SSafeSaveParam* pParam) = 0;

    virtual HRESULT SavePlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
        const CAMERA_INFO_HEADER* pInfoHeader,
        const unsigned char* pbInfo,
        const unsigned char* pbData,
        const char* szDevIno  //设备信息(编号或者IP)
    ) = 0;

    virtual HRESULT SavePlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int* piIndex,
        CCameraDataReference* pRefData //数据引用
    ) = 0;

    virtual HRESULT GetPlateRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int& index,
        CAMERA_INFO_HEADER* pInfoHeader,
        unsigned char* pbInfo, const int iInfoLen,
        unsigned char* pbData, const int iDataLen,
        const char* szDevIno,  //设备信息(编号或者IP)
        int & nLastDiskID, //上一个记录的分区索引
        const DWORD32 dwDataInfo    //指定某种数据信息类型，比如违章
    ) = 0;

    virtual HRESULT SaveVideoRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
        const CAMERA_INFO_HEADER* pInfoHeader,
        const unsigned char* pbInfo,
        const unsigned char* pbData
        ) = 0;

    virtual HRESULT GetVideoRecord(
        DWORD32 dwTimeLow, DWORD32 dwTimeHigh,
        CAMERA_INFO_HEADER* pInfoHeader,
        unsigned char* pbInfo, const int iInfoLen,
        unsigned char* pbData, const int iDataLen
        ) = 0;


    virtual HRESULT SaveDetectData(CDetectData *pData) = 0;
    virtual HRESULT GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData *pData) = 0;

    virtual HRESULT GetDiskStatus() = 0;
    virtual HRESULT GetLastCamStatus(DWORD32* pdwStatus) = 0;
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;

    //获取某个小时目录下的结果数量
    virtual HRESULT GetHourCount(const DWORD32 dwTimeLow, const DWORD32 dwTimeHigh, DWORD32* pdwCount) = 0;

    //查找下一个视频文件
    virtual HRESULT FindNextVideo(DWORD32& dwTimeLow, DWORD32& dwTimeHigh) = 0;

    virtual ~ISafeSaver() {};
};

#endif
