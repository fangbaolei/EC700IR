#ifndef _HVDEVICEUTILS_H_
#define _HVDEVICEUTILS_H_

#include "HvDeviceDLL.h"
#include "HvDeviceCommDef.h"


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)							\
	if ( p )									\
	{											\
		delete p;								\
		p = NULL;								\
	}
#endif

#define IVN_MAX_SIZE 128

#ifndef SAFE_DELETE_ARG
#define SAFE_DELETE_ARG(p)						\
	if(p)										\
	{											\
		delete[] p;								\
		p = NULL;								\
	}
#endif

//结果图片信息结构体
typedef struct _RECORD_IMAGE_INFO_EX
{
	RECT rcPlate;
	RECT rcFacePos[20];                     //人脸坐标
	int nFaceCount;                         //人脸数
	DWORD32 dwCarID;
	DWORD32 dwWidth;
	DWORD32 dwHeight;
	DWORD64 dw64TimeMS;
}RECORD_IMAGE_INFO_EX;

//结果图片结构体
typedef struct _RECORD_IMAGE_EX
{
	RECORD_IMAGE_INFO_EX	cImgInfo;
	PBYTE pbImgInfo;
	PBYTE pbImgData;
	DWORD dwImgInfoLen;
	DWORD dwImgDataLen;
}RECORD_IMAGE_EX;

//视频结构体
typedef struct _RECORD_VIDEO_ILLEGAL
{
	PBYTE pbVideoInfo;
	PBYTE pbVideoData;
	DWORD dwVideoInfoLen;
	DWORD dwVideoDataLen;
	DWORD dwCarID;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD64 dw64TimeMS;
}RECORD_VIDEO_ILLEGAL;

//结果图片集结构体
typedef struct _RECORD_IMAGE_GROUP_EX
{
	RECORD_IMAGE_EX	cBestSnapshot;
	RECORD_IMAGE_EX	cLastSnapshot;
	RECORD_IMAGE_EX	cBeginCapture;
	RECORD_IMAGE_EX	cBestCapture;
	RECORD_IMAGE_EX	cLastCapture;
	RECORD_IMAGE_EX	cPlatePicture;
	RECORD_IMAGE_EX	cPlateBinary;
	RECORD_VIDEO_ILLEGAL cIllegalVideo; // 多帧视频数据
}RECORD_IMAGE_GROUP_EX;

typedef struct _PACK_RESUME_HEADER
{
	DWORD32 dwType;
	DWORD32 dwInfoLen;
	DWORD32 dwDataLen;

}PACK_RESUME_HEADER;


#define MAX_INFOR_LEN 3*1024*1024
#define MAX_DATA_LEN 10*1024*1024

#define DEFAULT_INFOR_LEN 1024*1024
#define DEFAULT_DATA_LEN  3*1024*1024

#define PROTOCOL_MERCURY	1
#define PROTOCOL_EARTH	2

typedef enum
{
	PACK_TYPE_HEADER 
	, PACK_TYPE_INFO
	, PACK_TYPE_DATA

}PACK_TYPE;

//断点续传记录（单个结果接收情况记录）
typedef struct _PACK_RESUME_CACHE
{
	PACK_RESUME_HEADER header;
	BOOL fVailHeader;			

	CHAR* pInfor;
	INT nMaxInforLen;
	INT nInforLen;
	BOOL fVailInfor;

	CHAR* pData;
	INT nMaxDataLen;
	INT nDataLen;
	BOOL fVailData;

	INT nDataOffset;
	INT nInfoOffset;

	_PACK_RESUME_CACHE()
		: nInforLen(0)
		, nDataLen(0)
		, fVailData(FALSE)
		, fVailInfor(FALSE)
		, fVailHeader(FALSE)
		, pInfor(NULL)
		, nMaxInforLen(0)
		, pData(NULL)
		, nMaxDataLen(0)
		, nDataOffset(0)
		, nInfoOffset(0)
	{
		ZeroMemory((void*)&header , sizeof(header));

	}

}PACK_RESUME_CACHE;

// 水星协议命令输入/输出的类型
typedef enum {
	XML_CMD_TYPE_NULL,       // 既无入参也无出参
	XML_CMD_TYPE_CUSTOM,     // 由代码自行定义参数格式
	XML_CMD_TYPE_INT,
	XML_CMD_TYPE_DOUBLE,
	XML_CMD_TYPE_FLOAT,
	XML_CMD_TYPE_BOOL,
	XML_CMD_TYPE_DWORD,
	XML_CMD_TYPE_STRING,
	XML_CMD_TYPE_BIN,
	XML_CMD_TYPE_INTARRAY1D,
	XML_CMD_TYPE_INTARRAY2D
} XML_CMD_TYPE;

typedef struct _CXmlParseInfo
{
	CHAR		szKeyName[128];
	CHAR		szKeyValue[128];
	INT			nKeyValueLen;
	XML_CMD_TYPE eKeyType; 
	_CXmlParseInfo()
	{
		szKeyName[0]='\0';
		szKeyValue[0]='\0';
		nKeyValueLen = 128;
		eKeyType = XML_CMD_TYPE_NULL;
	}
}CXmlParseInfo;

class CCSLockEx
{
public:
	CCSLockEx(CRITICAL_SECTION * pCS, bool fLock = true)
	{
		m_pCS = pCS;
		m_fLock = false;
		if (fLock)
		{
			Lock();
		}
	}
	virtual ~CCSLockEx()
	{
		Unlock();
	}
	void Lock(void)
	{
		if (!m_fLock)
		{
			m_fLock = true;
			EnterCriticalSection(m_pCS);
		}
	}
	void Unlock(void)
	{
		if (m_fLock)
		{
			LeaveCriticalSection(m_pCS);
			m_fLock = false;
		}
	}
private:
	CRITICAL_SECTION *m_pCS;
	bool m_fLock;
};

class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION * pCS)
	{
		if ( pCS != NULL )
		{
			m_pCS = pCS;
			EnterCriticalSection(m_pCS);
		}
	}
	virtual ~CAutoLock()
	{
		if ( m_pCS!= NULL )
		{
			LeaveCriticalSection(m_pCS);
			m_pCS = NULL;
		}

	}

private:
	CRITICAL_SECTION *m_pCS;

};


#define HX_TYPE		    "TYPE"
#define HX_TYPE_INT     "INT"
#define HX_TYPE_STRING  "STRING"
#define HX_TYPE_BIN     "BIN"
#define HX_TYPE_FLOAT   "FLOAT"
#define HX_CMDNAME		"CmdName"
#define HX_VALUE		"Value"
#define HX_RECODE		"Value"
#define HX_RETMSG		"RetMsg"

// 初始化WSA
bool InitWSA();

// 反初始化WSA
bool UnInitWSA();

// 创建SOCKET
SOCKET HvCreateSocket(int af=AF_INET, int type=SOCK_STREAM, int protocol=0);

// 设置接收超时
HRESULT HvSetRecvTimeOut(const SOCKET& hSocket, int iMS);

// 设置发送超时
HRESULT HvSetSendTimeOut(const SOCKET& hSocket, int iMS);

// 监听一体机连接
HRESULT HvListen(const SOCKET& hSocket, int nPort, int backlog);

// 等待一体机连接
HRESULT HvAccept(const SOCKET& hSocket, SOCKET& hNewSocket, int iTimeout/*=-1*/);



//初始化GDI
void GdiInitialized(void);

void LoadIniConfig(void);

// 连接一体机
bool ConnectCamera(char* szIp, WORD wPort, SOCKET& hSocket, int iTryTimes = 8, int iReciveTimeOutMS = 20000 );

// 强制关闭socket
int ForceCloseSocket(SOCKET &s);

// 写日志
void HV_WriteLog(const char *pszLog);

// 安全关闭线程
void HvSafeCloseThread(HANDLE& hThread);

// 完整接收socket上的数据
int RecvAll(SOCKET socket, char *pBuffer, int iRecvLen);

// 完整接收socket上的数据,并返回真正接到的长度
int RecvAll(SOCKET socket, char *pBuffer, int iRecvLen , int& iRealRecvLen) ;

// 发送执行xml协议命令
bool ExecXmlExtCmd(char* szIP, char* szXmlCmd, char* szRetBuf, int& nBufLen);

//发送执行XML协议命令
bool ExecXmlExtCmd(char* szXmlCmd, char* szRetBuf,
				   int& iBufLen, SOCKET sktSend);


//获取协议版本
PROTOCOL_VERSION GetProtocolVersion(char* szIP);

//获取XML协议类型
//HRESULT HvGetXmlProtocolVersion(char* szIP, DWORD* pdwVersionType);

//获取设备扩展信息
HRESULT HvGetDeviceExteInfo(int iIndex, LPSTR lpExtInfo, int iBufLen);
// 生成Xml数据
int BuildHvCmdXml(
			char* pXmlBuf,
			char* pCmdName,
			int iArgCount,
			const char rgszName[][IVN_MAX_SIZE],
			const char rgszValue[][IVN_MAX_SIZE]
				  );
 // 从HvXml协议信息中 获取指定参数的 Vaule(String)
HRESULT GetParamStringFromXml(
			const TiXmlElement* pCmdArgElement,
			const char *pszParamName,
			char *pszParamValue,
			int nParamValueSize
			);

// 从HvXml协议信息中 获取指定参数的 Vaule(int) 
HRESULT GetParamIntFromXml(
			const TiXmlElement* pCmdArgElement,
			const char *pszParamName,
			int *pnParamValue
			);

// 从XML中解析 指定参数
HRESULT	HvXmlParse( CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount );
// 从XML中解析 指定参数
HRESULT	HvXmlParseMercury( const CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount );

HRESULT HvGetNonGreedyMatchBetweenBeginLabelAndEndLabel(const CHAR* szBeginLabel,const  CHAR* szEndLabel,const  CHAR* pszRetMsg, INT nRetMsgLen, CHAR* szJieQuReslut, INT nLen );

// 从XML中解析 指定参数  onlyfor 
HRESULT	HvXmlParseMercuryMulti( CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount );

// 判断及解析Xml或Xml字符串命令，并生成Xml数据；szOutXmlBuf必需有足够的空间
HRESULT HvMakeXmlCmdByString(
	PROTOCOL_VERSION emProtocolVersion,
	const char* inXmlOrStrBuf, 
	int nInlen, 
	char* szOutXmlBuf, 
	int& nOutlen
);

// 判断及解析Xml或Xml字符串命令，并生成Xml数据；szOutXmlBuf必需有足够的空间
HRESULT HvMakeXmlCmdByString1(
	const char* inXmlOrStrBuf, 
	int nInlen, 
	char* szOutXmlBuf, 
	int& nOutlen
);

// 判断及解析Xml或Xml字符串信息，并生成Xml数据；szOutXmlBuf必需有足够的空间
HRESULT HvMakeXmlCmdByString2(
	const char* inXmlOrStrBuf,
	int nInlen, 
	char* szOutXmlBuf, 
	int& nOutlen);

// 水星协议，判断及解析Xml或Xml字符串信息，并生成Xml数据；szOutXmlBuf必需有足够的空间
HRESULT HvMakeXmlCmdByString3(
	const char* inXmlOrStrBuf, 
	int nInlen, 
	char* szOutXmlBuf, 
	int& nOutlen);

// 判断及解析Xml或Xml字符串信息，并生成Xml数据；szOutXmlBuf必需有足够的空间
HRESULT HvMakeXmlInfoByString(
    PROTOCOL_VERSION emProtocolVersion,
	const char* inXmlOrStrBuf, 
	int nInlen, 
	char* szOutXmlBuf, 
	int& nOutlen
);

// 从HvXml协议命令执行返回的结果中解析出返回码
HRESULT HvParseXmlCmdRespRetcode(char* szXmlBuf, char* szCommand);

// 从HvXml协议命令执行返回的结果中解析出返回文本信息
HRESULT HvParseXmlCmdRespMsg(char* szXmlBuf, char* szMsgID, char* szMsgOut);

//从XML2.0协议信息命令执行返回的结果中解析出指定信息
HRESULT HvParseXmlCmdRespRetcode2(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText);

//从XML2.0协议信息命令执行返回的结果中解析出指定信息
HRESULT HvParseXmlCmdRespRetcode2Adv(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText, int* pnCmdValueTextLen);

//从XML3.0协议信息命令执行返回的结果中解析出指定信息
HRESULT HvParseXmlCmdRespRetcode3(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText);

//解析connectIP字符串
HRESULT ParseConnectIP(const CHAR* szIn, const CHAR* szPre, CHAR* szOut, INT *pnszOut);

// 从HvXml协议信息获取执行返回的结果中解析出指定信息
HRESULT HvParseXmlInfoRespValue(
	char* szXmlBuf,
	char* szInfoName, 
	char* nInfoValueName,
	char* szInfoValueText
);

// 描述:	转换YUV数据成BMP格式
// 参数:	pbDest			输出BMP数据的缓冲区指针;
//			iDestBufLen		输出缓冲区大小
//			piDestLen		实际输出数据大小
//			pbSrc			输入YUV数据的缓冲区指针;
//			iSrcWidth		图像宽度;
//			iSrcHeight		图像高度;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
HRESULT Yuv2BMP(
	BYTE* pbDest,
	int iDestBufLen,
	int* piDestLen,
	BYTE* pbSrc,
	int iSrcWidth,
	int iSrcHeight
);

// 二值图数据转换为BMP格式
void Bin2BMP(PBYTE pbBinData, PBYTE pbBmpData, INT& nBmpLen);

// 搜索局域网内所有的视频处理设备，返回设备数目
HRESULT SearchHVDeviceCount(DWORD32 *pdwCount);

// 根据索引查询设备ip地址
HRESULT GetHVDeviceAddr(
	int iIndex,
	DWORD64 *pdw64MacAddr,
	DWORD32 *pdwIP,
	DWORD32 *pdwMask,
	DWORD32 *pdwGateway
);

// 通过设备MAC地址修改设备的IP地址
HRESULT SetIPFromMac(DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway);

// 将IP地址由二进制转换为点分十进制字符串形式
BOOL MyGetIpString(DWORD dwIP, LPSTR lpszIP);

// 将MAC地址由二进制转换为'-'分十六进制字符串形式
BOOL MyGetMacString(DWORD64 dwMac, LPSTR lpszMac, DWORD dwDevType);

//发送命令
HRESULT HvSendXmlCmd(LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT* piRetLen, DWORD dwXmlVersion, SOCKET sktSend);

//通过指定的SOCKET发送Xml命令
HRESULT HvSendXmlCmd(char* szIP, LPCSTR szCmd, LPSTR szRetBuf,
					 INT iBufLen, INT* piRetLen, SOCKET sktSend);
//通过指定的SOCKET执行Xml命令
bool ExecXmlExtCmdEx(char* szIP, char* szXmlCmd, char* szRetBuf,
					 int& iBufLen, SOCKET sktSend);

//通过指定的SOCKET执行Xml命令（水星协议）
bool ExecXmlExtCmdMercury(char* szIP, char* szXmlCmd, char* szRetBuf,
					 int& iBufLen, SOCKET sktSend);

HRESULT HvGetRecordImage_Mercury(const char* szAppendInfo, PBYTE pbRecordData, DWORD dwRecordDataLen, RECORD_IMAGE_GROUP_EX* pcRecordImage);
HRESULT HvGetRecordImage(PBYTE pbRecordData, DWORD dwRecordDataLen, RECORD_IMAGE_GROUP_EX* pcRecordImage);

HRESULT HvEnhanceTrafficLight(PBYTE pbSrcImg, DWORD dwiSrcImgDataLen, int iRedLightCount,
							  PBYTE pbRedLightPos, PBYTE pbDestImgBuf, DWORD& dwDestImgBufLen,
							  INT iBrightness, INT iHueThreshold, INT iCompressRate);

HRESULT ZBase64Encode( unsigned char* pSrcData, int nSrcByte ,  char* pDestData ,int* pnDestByte );
HRESULT ZBase64Decode( unsigned char* pSrcData, int nSrcByte ,  char* pDestData ,int* pnDestByte );

HRESULT ExecuteCmd( CHAR* pIP , CAMERA_COMMAND_TYPE eCmdId 
				   , CHAR* pCmdData , INT nCmdByte
				   , CHAR* pRetData , INT* pnRetByte  );
HRESULT SearchDeviceEx( CDevBasicInfo* rgDevInfo , INT* pnDevCount );

HRESULT HvGetFirstParamNameFromXmlCmdAppendInfoMap(const CHAR* szCmdName, CHAR * szParamName, INT nParamNameLen);

HRESULT HvGetParamNameFromXmlCmdAppendInfoMap(const CHAR* szCmdName,  CXmlParseInfo* pXmlParseInfo, INT nXmlParseInfoNum);

int HvGetXmlForSetNameList( const CHAR* szValue, INT iValueLen, CHAR *szXmlBuff, INT iBuffLen);
bool HvGetNamListFromXML(const CHAR* szXmlBuff, CHAR* szWhiteNameList, CHAR* szBlackNameList);
int HvGetXmlForTriggerPlate( const CHAR* szValue, INT iValueLen, CHAR *szXmlBuff, INT iBuffLen);
bool HvGetRetMsgFromXml(const CHAR* szXmlBuff, CHAR* szRetMsg);
int HvGetXmlOfStringType(const CHAR* szCMD, const CHAR* szClass, const CHAR* szValue, CHAR *szXmlBuff);

#endif // _HVDEVICEUTILS_H_
