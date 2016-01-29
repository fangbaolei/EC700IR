#ifndef _HVDEVICEUTILS_H_
#define _HVDEVICEUTILS_H_

#include "tinyxml.h"
#include "HvDeviceEx.h"
#include "HvCameraType.h"
#include "HVAPI_HANDLE_CONTEXT_EX.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  if (p)  {delete p; p = NULL ;  }
#endif

#define IVN_MAX_SIZE 128

#ifndef SAFE_DELETE_ARG
#define SAFE_DELETE_ARG(p)   if  ( p ) {delete[] p; p = NULL; }
#endif

typedef struct _RECORD_IMAGE_INFO_EX
{
    RECT rcPlate;
    RECT rcFacePos[20];
    int nFaceCount;
    DWORD32 dwCarID;
    DWORD32 dwWidth;
    DWORD32 dwHeight;
    DWORD64 dw64TimeMs;
}_RECORD_IMAGE_INFO_EX;

typedef struct _RECORD_IMAGE_EX
{
    _RECORD_IMAGE_INFO_EX cImgInfo;
    PBYTE pbImgInfo;
    PBYTE pbImgData;
    DWORD dwImgInfoLen;
    DWORD dwImgDataLen;
}RECORD_IMAGE_EX;

typedef struct _RECORD_IMAGE_GROUP_EX
{
    RECORD_IMAGE_EX  cBestSnapshot;
    RECORD_IMAGE_EX   cLastSnapshot;
    RECORD_IMAGE_EX  cBeginCapture;
    RECORD_IMAGE_EX cBestCapture;
    RECORD_IMAGE_EX cLastCapture;
    RECORD_IMAGE_EX cPlatePicture;
    RECORD_IMAGE_EX cPlateBinary;
}RECORD_IMAGE_GROUP_EX;




bool InitWSA();
bool UnInitWSA();

// ŽŽœšSOCKET
SOCKET HvCreateSocket(int af=PF_INET, int type=SOCK_STREAM, int protocol=0);

// ÉèÖÃœÓÊÕ³¬Ê±
HRESULT HvSetRecvTimeOut(const SOCKET& hSocket, int iMS);

// ÉèÖÃ·¢ËÍ³¬Ê±
HRESULT HvSetSendTimeOut(const SOCKET& hSocket, int iMS);

// ŒàÌýÒ»Ìå»úÁ¬œÓ
HRESULT HvListen(const SOCKET& hSocket, int nPort, int backlog);

// µÈŽýÒ»Ìå»úÁ¬œÓ
HRESULT HvAccept(const SOCKET& hSocket, SOCKET& hNewSocket, int iTimeout/*=-1*/);


void WriteLog(const char *pszLog);

int RecvAll( int socket, char *pBuffer, int iRecvLen );

// ÍêÕûœÓÊÕsocketÉÏµÄÊýŸÝ,²¢·µ»ØÕæÕýœÓµœµÄ³€¶È
int RecvAll(int socket, char *pBuffer, int iRecvLen , int& iRealRecvLen) ;
//int RecvAll(SOCKET socket, char *pBuffer, int iRecvLen , int& iRealRecvLen) ;

// ·¢ËÍÖŽÐÐxmlÐ­ÒéÃüÁî
bool ExecXmlExtCmd(char* szIP, char* szXmlCmd, char* szRetBuf, int& nBufLen);

//·¢ËÍÖŽÐÐXMLÐ­ÒéÃüÁî
bool ExecXmlExtCmd(char* szXmlCmd, char* szRetBuf,
				   int& iBufLen, SOCKET sktSend);



//
bool IsNewProtocol(char *szIP);
HRESULT SearchHVDeviceCount(DWORD32 *pdwCount);
HRESULT GetHVDeviceAddr(int iIndex,
                                                    DWORD64 *pdw64MacAddr,
                                                    DWORD32 *pdwIP,
                                                    DWORD32 *pdwMask,
                                                    DWORD32 *pdwGateway);
HRESULT SetIPFromMac(DWORD64  dw64MacAddr, DWORD dwIP,DWORD dwMask, DWORD dwGateway);

//»ñÈ¡XMLÐ­ÒéÀàÐÍ
//HRESULT HvGetXmlProtocolVersion(char* szIP, DWORD* pdwVersionType);

//»ñÈ¡Éè±žÀ©Õ¹ÐÅÏ¢
//HRESULT HvGetDeviceExteInfo(int iIndex, LPSTR lpExtInfo, int iBufLen);
// Éú³ÉXmlÊýŸÝ
int BuildHvCmdXml(
			char* pXmlBuf,
			char* pCmdName,
			int iArgCount,
			const char rgszName[][IVN_MAX_SIZE],
			const char rgszValue[][IVN_MAX_SIZE]
				  );
 // ŽÓHvXmlÐ­ÒéÐÅÏ¢ÖÐ »ñÈ¡Öž¶š²ÎÊýµÄ Vaule(String)
HRESULT GetParamStringFromXml(
			const TiXmlElement* pCmdArgElement,
			const char *pszParamName,
			char *pszParamValue,
			int nParamValueSize
			);

// ŽÓHvXmlÐ­ÒéÐÅÏ¢ÖÐ »ñÈ¡Öž¶š²ÎÊýµÄ Vaule(int)
HRESULT GetParamIntFromXml(
			const TiXmlElement* pCmdArgElement,
			const char *pszParamName,
			int *pnParamValue
			);



HRESULT HvGetDeviceExteInfo(int iIndex, LPSTR lpExtInfo, int iBufLen);

HRESULT HvGetXmlProtocolVersion(char* szIP, DWORD *pdwVersionType);

bool ConnectCamera( char *szIp, WORD wPort, int &hSocket, int iTryTimes=0);

HRESULT HvGetRecordImage(PBYTE pbRecordData, DWORD dwRecordDataLen, RECORD_IMAGE_GROUP_EX* pRecordImag);

HRESULT HvMakeXmlInfoByString(bool fNewProtocol, const char*inXmlOrStrBuf, int nInlen,
char* szOutXmlBuf, int &nOutlen);
HRESULT HvMakeXmlCmdByString(bool fNewProtocol, const char* inXmlOrStrBuf,  int nInlen, char *szOutXmlBuf, int &nOutlen);
HRESULT HvMakeXmlCmdByString( const char* inXmlOrStrBuf,  int nInlen, char *szOutXmlBuf, int &nOutlen);
HRESULT HvParseXmlCmdRespRetcode2(char*szXmlBuf, char* szCmdName,
char* nInfoValueName, char* szInfoValueText);
HRESULT HvParseXmlCmdRespRetcode(char* szXmlBuf, char* szCommand);

HRESULT HvParseXmlInfoRespValue(char*szXmlBuf, char* szInfoName,
char* nCmdValueName, char* szCmdValueText);

HRESULT HvSendXmlCmd(char *szIP, LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT *piRetLen, int sktSend);

HRESULT HvSendXmlCmd(LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT* piRetLen, DWORD dwXmlVersion, SOCKET sktSend);
bool ExecXmlExtCmdEx(char *szIP, char *szXmlCmd, char *szRetBuf, int &iBufLen, int sktSend);
//·¢ËÍÖŽÐÐXMLÐ­ÒéÃüÁî
bool ExecXmlExtCmd(char* szXmlCmd, char* szRetBuf,
				   int& iBufLen, SOCKET sktSend);


void HvSafeCloseThread(pthread_t  &pthreadHandle);
int ForceCloseSocket(int &iNetSocket);

HRESULT Yuv2BMP(
				BYTE *pbDest,
				int iDestBufLen,
				int *piDestLen,
				BYTE *pbSrc,
				int iSrcWidth,
				int iSrcHeight
				);

void Bin2BMP(PBYTE pbBinData, PBYTE pbBmpData, INT& nBmpLen);

HRESULT HvEnhanceTrafficLight(PBYTE pbSrcImg, DWORD dwiSrcImgDataLen, int iRedLightCount,
							  PBYTE pbRedLightPos, PBYTE pbDestImgBuf, DWORD& dwDestImgBufLen,
							  INT iBrightness, INT iHueThreshold, INT iCompressRate);

#endif // _HVDEVICEUTILS_H_

