#include "HvDeviceUtils.h"
#include "LinuxBmp.h"
#include "inifile.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "ximage.h"

#include<string>
#undef max
#include<vector>

#pragma pack(4)

static int HvMakeInfoReq(char* szXmlBuf, int nInfoCount,
char** szInfoName, char** szInfoText );

struct SXmlAttr
{
   std::string strName;
   std::string  strValue;
   SXmlAttr()
   {
       strName = "";
       strValue = "";
   }
};

struct SXmlCmd
{
    std::string strCmdName;
    std::vector<SXmlAttr> listAttr;

    SXmlCmd()
    {
        strCmdName="";
        listAttr.clear();
    }
};

typedef std::vector<SXmlCmd>  XmlCmdList;


ThreadMutex g_mutexLog;

//Get True Path(No include filename)
static char* GetProcessTruePath(char* path, int size = 4096)
{
    char *tempath = ( char* )malloc(size);
    chdir("./");
    getcwd(path, size);
    chdir(tempath);
    delete tempath;
    return path;
}

const int BUFFER_SIZE = 3;
bool CopyFile(char *lpExistingFileName, char *lpNewFileName, bool bFailExists)
{
    if (lpExistingFileName == NULL)
    return false;
    if (lpNewFileName == NULL)
     return false;

    FILE * from_fd =  NULL, *to_fd =  NULL;
    int bytes_read = 0, bytes_write = 0;

    char buffer[BUFFER_SIZE];
    char *ptr = NULL;

    from_fd = fopen(lpExistingFileName, "r");
    if (from_fd== NULL)
        return false;

    to_fd = fopen(lpNewFileName, "w");
    if ( to_fd == NULL)
        return false;

    while (bytes_read = fread(buffer, 1, BUFFER_SIZE, from_fd))
    {
        if ((bytes_read == -1) && errno != EINTR)
            break;
        else if (bytes_read > 0)
        {
            ptr = buffer;
            while (bytes_write =fwrite(ptr, 1, bytes_read, to_fd))
            {
                if ((bytes_write == -1) && (errno != EINTR))
                    break;
                else if (bytes_write == bytes_read)
                   break;
                else if (bytes_write > 0)
                {
                    ptr += bytes_write;
                    bytes_read -= bytes_write;
                }
            }
            if (bytes_write == -1)
                break;
        }

        bytes_read = 0;
    }
    fclose(from_fd);
    fclose(to_fd);
    return true;
}

static unsigned long GetTickCount()
{
   // tms tm;
    //return times;
    unsigned long currentTime;
    struct timeval current;
    gettimeofday(&current, NULL);
    currentTime = current.tv_sec * 1000 + current.tv_usec/1000;

    return currentTime;
}

void WriteLog(const char *pszLog)
{
   const int NOT_FIND = -99;
	 g_mutexLog.lock();

	//Éú³ÉLOGÎÄŒþÃû
	const int MAXPATH = 260;
	char szFileName[MAXPATH] = {0};
	char szIniName[MAXPATH] = {0};
	char szBakName[MAXPATH] = {0};
    GetProcessTruePath(szFileName, MAXPATH);
	strncpy(szIniName, szFileName, MAXPATH);
	strncpy(szBakName, szFileName, MAXPATH);
	strncat(szFileName, "/HvDevice.log", MAXPATH - 1);
	strncat(szIniName, "/HvDevice.ini", MAXPATH - 1);
	strncat(szBakName, "/HvDevice.bak", MAXPATH - 1);

	int iEnable = GetPrivateProfileInt("LogInf", "Enable", NOT_FIND, szIniName);
	if (NOT_FIND == iEnable)
	{
		iEnable = 1;
		char szEnable[10];
		sprintf(szEnable, "%d", iEnable);
		 WritePrivateProfileString("LogInf", "Enable", szEnable, szIniName);
	}

	if (iEnable)
	{
		int iHoldDays = GetPrivateProfileInt("LogInf", "HoldDays", NOT_FIND, szIniName);
		if (iHoldDays <= 0)
		{
			iHoldDays = 1;
			char szHoldDays[10];
			sprintf(szHoldDays, "%d", iHoldDays);
			 WritePrivateProfileString("LogInf", "HoldDays", szHoldDays, szIniName);
		}

		//È¡µÃµ±Ç°µÄŸ«È·ºÁÃëµÄÊ±Œä
		static time_t starttime = time(NULL);
		static unsigned long starttick = GetTickCount();
		unsigned long dwNowTick = GetTickCount() - starttick;
		time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
		struct tm *pTM = localtime(&nowtime);
		unsigned long dwMS = dwNowTick % 1000;

		int iYDay = GetPrivateProfileInt("LogInf", "YDay", NOT_FIND, szIniName);
		if (NOT_FIND == iYDay)
		{
			iYDay = pTM->tm_yday;
			char szYDay[20];
			sprintf(szYDay, "%d", iYDay);
			WritePrivateProfileString("LogInf", "YDay", szYDay, szIniName);
		}

		//È·¶šÊÇ·ñÖØÐŽÎÄŒþ
		FILE *pfile = NULL;
		if ((pTM->tm_yday - iYDay < iHoldDays) && (pTM->tm_yday - iYDay >= 0))
		{
			pfile = fopen(szFileName, "a+");
		}
		else
		{
			//CopyFile(szFileName, szBakName, FALSE);
			pfile = fopen(szFileName, "w");
			char szYDay[20];
			sprintf(szYDay, "%d", pTM->tm_yday);
			WritePrivateProfileString("LogInf", "YDay", szYDay, szIniName);
		}

		//ÐŽÈëLOGÐÅÏ¢
		if (pfile != NULL)
		{
			fprintf(pfile, "%04d-%02d-%02d %02d:%02d:%02d:%03d %s\n",
				pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
				pTM->tm_hour, pTM->tm_min, pTM->tm_sec, dwMS,
				pszLog);
			fclose(pfile);
		}
	}

    // g_IsemWriteLog->Post(ISemaphore::NOWAIT);
    g_mutexLog.unlock();
}



bool InitWSA()
{
    return true;
}
bool UnInitWSA()
{
    return true;
}

int RecvAll( int socket, char *pBuffer, int iRecvLen )
{
    char *pTemp = pBuffer;
    int iRst = iRecvLen;
    CAMERA_CMD_RESPOND* cbTemp = (CAMERA_CMD_RESPOND*)pBuffer;
    while ( iRecvLen > 0 )
    {
        int iTemp = recv( socket, pTemp, iRecvLen, 0 );

        if ( iTemp < 0 )
        {
          //  int irr = errno;
          //  strerror(errno);
            if ( errno ==  EINTR /*|| errno == EAGAIN || errno == EWOULDBLOCK*/)
            {
                continue ;
            }
            return iTemp;
        }
        if (  iTemp == 0 )
            break;
        pTemp += iTemp;
        iRecvLen -= iTemp;
    }
    return iRst - iRecvLen;
}

int RecvAll(int socket, char *pBuffer, int iRecvLen , int& iRealRecvLen) {
	char *pTemp = pBuffer;
	int iRst = iRecvLen;

	int iBlockLen = 10240;

	while (iRecvLen > 0)
	{
		int iTemp = recv(socket, pTemp, iRecvLen>iBlockLen?iBlockLen:iRecvLen, 0);
		//int iTemp = recv( socket, pTemp, iRecvLen, 0 );

        if ( iTemp < 0 )
        {
          //  int irr = errno;
          //  strerror(errno);
            if ( errno ==  EINTR /*|| errno == EAGAIN || errno == EWOULDBLOCK*/)
            {
                continue ;
            }
            return iTemp;
        }

		if (iTemp == 0)
			break;
		pTemp += iTemp;
		iRecvLen -= iTemp;
		iRealRecvLen = iRst - iRecvLen;
	}
	return iRst - iRecvLen;
}






const int MAX_HV_COUNT = 1000;
const unsigned short GETIP_COMMAND_PORT = 6666;
const DWORD32 GETIP_COMMAND = 0x00000101;
const DWORD32 SETIP_COMMAND = 0x00000102;
static int g_iDeviceCount=0;
static unsigned char g_aDeviceAddr[MAX_HV_COUNT][18+32];


int ForceCloseSocket(int &iNetSocket)
{
    if ( iNetSocket == 0)
        return 0;
    shutdown(iNetSocket, SHUT_RDWR);
    int iRet = close(iNetSocket);
    iNetSocket = 0;
    return iRet;
}

//
HRESULT SearchHVDeviceCount(DWORD32 *pdwCount)
{
    if ( pdwCount == NULL)
        return E_FAIL;

    int updSocket = -1;
    updSocket = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( updSocket < 0 )
        return E_FAIL;

    int flags = fcntl(updSocket, F_GETFL, 0);
    fcntl(updSocket, F_SETFL, flags&~O_NONBLOCK);

  /*  bool bBroadcast = true;
    if ( 0  !=   setsockopt(updSocket, SOL_SOCKET, SO_BROADCAST, (const void*)&bBroadcast, sizeof(bool))  )
        return E_FAIL;
*/
    int bBroadcast = 1;
    if ( 0  !=   setsockopt(updSocket, SOL_SOCKET, SO_BROADCAST, (const void*)&bBroadcast, sizeof(bBroadcast))  )
        return E_FAIL;

    struct timeval timeout ;
    timeout.tv_sec =2;
    timeout.tv_usec =  0;

     //if ( setsockopt(updSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&timeout, sizeof(struct timeval))  != 0 )
       //     return -1;
    if ( setsockopt(updSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval))  != 0 )
            return -1;

    struct sockaddr_in addrRemote;
    memset(&addrRemote, 0, sizeof(addrRemote));
  //  addrRemote.sin_addr.S_un.S_addr = 0xffffffff;
    addrRemote.sin_addr.s_addr = htonl(0xffffffff);
    addrRemote.sin_family = AF_INET;
    short sPort = GETIP_COMMAND_PORT;
    addrRemote.sin_port = htons(sPort);

    int iLen = sizeof(GETIP_COMMAND);

    if ( iLen  != sendto(updSocket, (char*)&GETIP_COMMAND, iLen, 0, (struct sockaddr *)&addrRemote, sizeof(addrRemote)) )
        return E_FAIL;


   // int iCount = 0;
    g_iDeviceCount = 0;
    while ( true )
    {
        /*
        struct timeval tv;
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(updSocket, &rset);
        int iTemp = select(updSocket+1, &rset, NULL, NULL, &tv);
        if (iTemp == -1)
            break;
        if ( iTemp== 0 )
            break;
        */
        struct sockaddr_in addrRemote;
         int iAddlen = sizeof(addrRemote);
        char chBuffer[256] = {0};
		int iRet = recvfrom(updSocket, (char*)chBuffer, sizeof(chBuffer), 0, (struct sockaddr *)&addrRemote, (socklen_t *)&iAddlen);

		 if ( iRet == -1 )
          {
              //printf("strerror: %d, %s\n", errno, strerror(errno));
             break;
          }
        if ( iRet == 0 )
            break ;
        if (iRet == E_HANDLE || iRet == E_POINTER)
            break ;


        if ( iRet == 22 || iRet==22+32)
        {
        //     iCount++;
            DWORD32 dwCommand;
            memcpy(&dwCommand, chBuffer, sizeof(dwCommand));
         //   printf("%d\n" , dwCommand);
            if ( dwCommand == GETIP_COMMAND )
            {
                if ( (chBuffer[9] & 0xFF) == 0xB6 || (chBuffer[9]&0xFF)==0xB7
                    || (chBuffer[9]&0xFF) == 0xB8 || (chBuffer[9]&0xFF) == 0xB9 )
                {
                     int i=0;
                     for (i=0; i<g_iDeviceCount; i++)
                    {
                       if ( memcmp(&chBuffer[4], g_aDeviceAddr[i], 6) == 0)
                       {
                           break ;
                       }
                    }//end for

                    if (i == g_iDeviceCount)
                    {
                        memset(g_aDeviceAddr[g_iDeviceCount], 0, 18+32);
                        memcpy(g_aDeviceAddr[g_iDeviceCount], &chBuffer[4], iRet - sizeof(dwCommand));

                        g_iDeviceCount++;
                        if ( g_iDeviceCount == MAX_HV_COUNT )
                            break ;
                    }
                }

           }//if
        }// if
    }//while

   // printf("DeviceCount:%d\n", iCount);
    // close socketint nSendLen = send( m_hSocket, (char*)pv, cb, MSG_NOSIGNAL);
    ForceCloseSocket(updSocket);
    usleep(50000);
    *pdwCount  = g_iDeviceCount;
    return S_OK;
}

HRESULT GetHVDeviceAddr(int iIndex,
                                                    DWORD64 *pdw64MacAddr,
                                                    DWORD32 *pdwIP,
                                                    DWORD32 *pdwMask,
                                                    DWORD32 *pdwGateway)
 {
     if ( iIndex >= g_iDeviceCount || iIndex < 0 )
        return E_FAIL;
    if (pdw64MacAddr != NULL)
        memcpy(pdw64MacAddr, g_aDeviceAddr[iIndex], 6);
    if (pdwIP != NULL)
        memcpy(pdwIP, &g_aDeviceAddr[iIndex][6], 4);
    if (pdwMask != NULL)
        memcpy(pdwMask, &g_aDeviceAddr[iIndex][6+4], 4);
    if (pdwGateway)
        memcpy(pdwGateway, &g_aDeviceAddr[iIndex][6+4+4], 4);
    return S_OK;


}

HRESULT SetIPFromMac(DWORD64  dw64MacAddr, DWORD dwIP,DWORD dwMask, DWORD dwGateway)
{
    int iSock = socket(PF_INET, SOCK_DGRAM, 0);
    if ( iSock  == -1 )
        return E_FAIL;

     int bBroadcast = 1;
    if ( 0  !=   setsockopt(iSock , SOL_SOCKET, SO_BROADCAST, (const void*)&bBroadcast, sizeof(bBroadcast))  )
        return E_FAIL;

    struct sockaddr_in addrRemote;
    memset(&addrRemote, 0, sizeof(addrRemote));
    addrRemote.sin_addr.s_addr = htonl(0xffffffff);
    addrRemote.sin_family = AF_INET;
    addrRemote.sin_port = htons(GETIP_COMMAND_PORT);

    char chTemp[22] = {0};
    memcpy(&chTemp[0], &SETIP_COMMAND, 4);
    memcpy(&chTemp[4], &dw64MacAddr, 6);
    memcpy(&chTemp[10], &dwIP, 4);
    memcpy(&chTemp[14], &dwMask, 4);
    memcpy(&chTemp[18], &dwGateway, 4);

    int iLen = sizeof(chTemp);
    int iRtn = sendto(iSock, (const void *)chTemp, iLen, 0, (struct sockaddr *)&addrRemote, sizeof(addrRemote));
    if (iRtn != iLen)
        return E_FAIL;

/*
    int iRst = E_FAIL;
    struct timeval tv;
    tv.tv_sec = 4;
    tv.tv_usec = 0;
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(iSock, &rset);
    int iTemp  =select(iSock+1, &rset, NULL, NULL, &tv);
    if ( iTemp != 0 && iTemp != -1 )
    {
        struct sockaddr_in addrRemote;
        int iaddrLen = sizeof(addrRemote);
        char acBuf[20] = {0};
        int iRecvLen = recvfrom(iSock, acBuf, sizeof(acBuf),0,(sockaddr*)&addrRemote, (socklen_t*)&iaddrLen);
        if ( iRecvLen == 4 )
            iRst = 0;
    }*/

  //    struct sockaddr_in addrRemote;
   int iRst = E_FAIL;
    int iaddrLen = sizeof(addrRemote);
      char acBuf[20] = {0};
      int iRecvLen = recvfrom(iSock, acBuf, sizeof(acBuf),0,(sockaddr*)&addrRemote, (socklen_t*)&iaddrLen);
     if ( iRecvLen == 4 )
        iRst = 0;

    ForceCloseSocket(iSock);
    usleep(50000);

    //arp
    system("sudo arp -n|awk");
  // if ( iTemp == 0 )
  if ( iRst != 0 )
        return S_FALSE;
    return iRst;
}

HRESULT HvGetDeviceExteInfo(int iIndex, LPSTR lpExtInfo, int iBufLen)
{
      if ( iIndex >= g_iDeviceCount || iIndex < 0 )
        return E_FAIL;

    if ( lpExtInfo == NULL || iBufLen<=0  )
        return E_FAIL;
   // int iSerStrinLen = (int)strlen( (char*)g_aDeviceAddr[iIndex] );
   int iSerStrinLen = 32;
    if ( iSerStrinLen<=0 || iBufLen <= iSerStrinLen)
        return E_FAIL;
    memcpy((char*)lpExtInfo, (char*)&g_aDeviceAddr[iIndex][18], iSerStrinLen);
    return S_OK;
}


bool ConnectCamera( char *szIp, WORD wPort, int &hSocket, int iTryTimes)
{
    if ( hSocket !=  -1)
    {
        ForceCloseSocket(hSocket);
        hSocket = -1;
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if ( hSocket == -1)
        return false;

    struct timeval tv;
    tv.tv_sec = 8;
    tv.tv_usec=0;
    //if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval))  != 0 )
     //       return -1;

    if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval))  != 0 )
            return -1;

   // int val = 1;
    //setsockopt(hSocket, SOL_SOCKET,SO_REUSEADDR, (void*)&val, sizeof(val));

    int val = 1;
    setsockopt(hSocket, SOL_SOCKET,SO_REUSEADDR, (char*)&val, sizeof(val));

	int flags = fcntl(hSocket, F_GETFL, 0);
    fcntl(hSocket, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(wPort);
    addr.sin_addr.s_addr = inet_addr(szIp);

    do
    {
       if (  connect(hSocket, (struct sockaddr*)&addr, sizeof(addr) ) != 0 )
       {
           struct timeval timeout ;
			fd_set r;
			FD_ZERO( &r );
			FD_SET(hSocket, &r);
			timeout.tv_sec =  4;
			timeout.tv_usec =0;
			int ret = select(hSocket+ 1,NULL, &r,  NULL, &timeout);
			if ( ret > 0 )
             {
                 if ( FD_ISSET(hSocket, &r) )
                 {
                     int ierror = 0;
                     socklen_t  len  = sizeof(int);
                     getsockopt(hSocket, SOL_SOCKET,  SO_ERROR,  &ierror , &len);
                     if ( ierror == 0 )
                        break;
                 }

             }
       }
       else
       {
           break;
       }
    }while (iTryTimes-- > 0 );

    if ( iTryTimes < 0 )
    {
        char szLog[256] = { 0 };
        sprintf(szLog, "[HvDevice] %s Cmd Link Fail ", szIp);
        ForceCloseSocket(hSocket);
        return false;
    }

    flags = fcntl(hSocket, F_GETFL, 0);
    fcntl(hSocket, F_SETFL, flags & ~O_NONBLOCK);

    char szLog[256] = { 0 };
    sprintf(szLog, "[HvDevice] %s Cmd Link Success ", szIp);
    WriteLog(szLog);
    return true;
}

bool IsNewProtocol(char *szIP)
{
    if ( szIP == NULL )
        return false;
    const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hsocketcmd = -1;
    CAMERA_CMD_HEADER  cmdHearder;
    CAMERA_CMD_RESPOND cmdRespond;

    if ( !ConnectCamera(szIP, wPort, hsocketcmd, 0 ) )
     {
            return false;
     }

    bool fIsOK = false;
    cmdHearder.dwID = CAMERA_PROTOCAL_COMMAND;
    cmdHearder.dwInfoSize = 0;

    int iHeaderLen = sizeof(cmdHearder);
    if ( iHeaderLen  !=  send( hsocketcmd, (char*)&cmdHearder, iHeaderLen, MSG_NOSIGNAL) )
    {
        ForceCloseSocket(hsocketcmd);
        return fIsOK;
    }
    int iRespondLen = sizeof(cmdRespond);
   if ( iRespondLen !=  RecvAll(hsocketcmd, (char *)&cmdRespond, iRespondLen ) )
   {
       ForceCloseSocket(hsocketcmd);
        return fIsOK;
   }

   fIsOK =  ( cmdRespond.dwID == CAMERA_PROTOCAL_COMMAND );
   ForceCloseSocket(hsocketcmd);
   return fIsOK;
}

HRESULT HvGetXmlProtocolVersion(char* szIP, DWORD *pdwVersionType)
{
    if ( szIP == NULL || pdwVersionType == NULL )
        return E_FAIL;
    int hSocketCmd = -1;
    CAMERA_CMD_HEADER cCmdHeader;
    CAMERA_CMD_RESPOND cCmdRespond;
    if (!ConnectCamera(szIP, CAMERA_CMD_LINK_PORT, hSocketCmd))
    {
        return E_FAIL;
    }
    cCmdHeader.dwID  = CAMERA_PROTOCAL_COMMAND;
    cCmdHeader.dwInfoSize = 0;
    if ( send (hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0 ) != sizeof(cCmdHeader) )
    {
        ForceCloseSocket(hSocketCmd);
        hSocketCmd = INVALID_SOCKET;
        return E_FAIL;
    }

    if ( recv( hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0 ) != sizeof(cCmdRespond) )
    {
        ForceCloseSocket(hSocketCmd);
        hSocketCmd = INVALID_SOCKET;
        return E_FAIL;
    }
    ForceCloseSocket(hSocketCmd);
    if ( cCmdRespond.dwID != CAMERA_PROTOCAL_COMMAND)
    {
        return E_FAIL;
    }
    *pdwVersionType = 1;
    return S_OK;
}

bool ExecXmlExtCmd(char* szIP, char* szXmlCmd, char* szRetBuf, int& nBufLen)
{
	if ( strlen(szXmlCmd) > 16*1024 )
	{
		return false;
	}

	const int iBufLen = (1024 << 10);
	char* pbTmpXmlBuf = new char[iBufLen];
	if(pbTmpXmlBuf == NULL)
	{
		return FALSE;
	}
	memset(pbTmpXmlBuf, 0, sizeof(pbTmpXmlBuf));

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if ( ConnectCamera(szIP, wPort, hSocketCmd) )
	{
		cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
		cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE(pbTmpXmlBuf);
			return false;
		}
		if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize, 0) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE(pbTmpXmlBuf);
			return false;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
				&& 0 == cCmdRespond.dwResult
				&& 0 < cCmdRespond.dwInfoSize )
			{
				if ( cCmdRespond.dwInfoSize == RecvAll(hSocketCmd, (char*)pbTmpXmlBuf, cCmdRespond.dwInfoSize) )
				{
					if ( nBufLen > (int)cCmdRespond.dwInfoSize )
					{
						nBufLen = cCmdRespond.dwInfoSize;
						memcpy(szRetBuf, pbTmpXmlBuf, cCmdRespond.dwInfoSize);
						memset(szRetBuf+cCmdRespond.dwInfoSize, 0, 1);

						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE(pbTmpXmlBuf);
						return true;
					}
					else
					{
						nBufLen = cCmdRespond.dwInfoSize;
						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE(pbTmpXmlBuf);
						return false;
					}
				}
				else
				{
					nBufLen = 0;
					ForceCloseSocket(hSocketCmd);
					SAFE_DELETE(pbTmpXmlBuf);
					return false;
				}
			}
		}
		else
		{
			nBufLen = 0;
			ForceCloseSocket(hSocketCmd);
		}
	}
	SAFE_DELETE(pbTmpXmlBuf);
	return false;
}



static int HvMakeXmlCmdReq(char *szXmlBuf, char *szCmdName, int nArgCount, char ** szArgID, char **szArgValue)
{
    TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "1.0");

	doc.LinkEndChild(pXmlRootElement);

	TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
	pXmlRootElement->LinkEndChild(pXmlElementCmd);

	TiXmlText *pXmlCmdText = new TiXmlText(szCmdName);
	pXmlElementCmd->LinkEndChild(pXmlCmdText);

	if (nArgCount != 0)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdArg");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		for (int i=0; i<nArgCount; i++)
		{
			TiXmlElement *pXmlElementID = new TiXmlElement("Value");
			pXmlElementCmd->LinkEndChild(pXmlElementID);

			pXmlElementID->SetAttribute("id", szArgID[i]);

			if (szArgValue[i] == NULL)
				continue;

			TiXmlText *pXmlIDText = new TiXmlText(szArgValue[i]);
			pXmlElementID->LinkEndChild(pXmlIDText);
		}
	}


	TiXmlPrinter printer;
	doc.Accept(&printer);

	int len = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), len);
	szXmlBuf[len] = '\0';

	return len;

}

static int HvMakeXmlCmdReq2(const XmlCmdList &listXmlCmd, char *szXmlBuf)
{
    TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "2.0");

	doc.LinkEndChild(pXmlRootElement);

	std::vector<SXmlCmd>::const_iterator iter = listXmlCmd.begin();
	for(; iter!=listXmlCmd.end(); ++iter)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		//ÃüÁîÃû³Æ
		TiXmlText *pXmlCmdText = new TiXmlText(iter->strCmdName.c_str());
		pXmlElementCmd->LinkEndChild(pXmlCmdText);

		//²ÎÊý(ÊôÐÔ)
		std::vector<SXmlAttr>::const_iterator iterAttr = iter->listAttr.begin();
		for(; iterAttr!=iter->listAttr.end(); ++iterAttr)
		{
			if (iterAttr->strName.size() > 0 && iterAttr->strValue.size() > 0)
			{
				pXmlElementCmd->SetAttribute(iterAttr->strName.c_str(), iterAttr->strValue.c_str());
			}
		}
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), iLen);
	szXmlBuf[iLen] = '\0';

	return iLen;
}

HRESULT HvMakeXmlCmdByString2(const char* inXmlOrStrBuf,  int nInlen, char *szOutXmlBuf, int &nOutlen)
{
    TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(inXmlOrStrBuf))
	{
		memcpy(szOutXmlBuf, inXmlOrStrBuf, nInlen);
		nOutlen = nInlen;
		return S_OK;
	}
	bool bGet = false;

	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	char* pszCmd = pszInBufCopy;
	XmlCmdList listXmlCmd;
	while(pszCmd)
	{
		int iBufLen = (int)strlen(pszCmd);
		char* pTmpCmd = new char[iBufLen + 1];//pszCmd;
		memcpy(pTmpCmd, pszCmd, iBufLen);
		pTmpCmd[iBufLen] = '\0';
		char* pTmpCmdEnd = strstr(pTmpCmd, ";");
		if(pTmpCmdEnd)
		{
			*pTmpCmdEnd = '\0';
		}
		int nCount = 0;
		char* pszCmdName = pTmpCmd;
		char* pValueID[64];
		char* pValueText[64];
		char* pTemp;
		while(pTmpCmd = strchr(pTmpCmd, ','))
		{
			*pTmpCmd = '\0';
			pValueID[nCount] = pTmpCmd + 1;
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2±íÊŸ±ØÐèÒªÓÐÒ»žö×Ö·û
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2±íÊŸ±ØÐèÒªÓÐÒ»žö×Ö·û
			{
				goto getone;
			}
			pValueText[nCount] = pTemp + 1;
			nCount++;

			*pTemp = '\0';
			*pTmpCmd = '\0';
			pTmpCmd++;
		}

getone:
		bGet = TRUE;
		SXmlCmd  sXmlCmd;
		sXmlCmd.strCmdName = pszCmdName;
		for (int i=0; i<nCount; ++i)
		{
			if (pValueID[i] && pValueText[i])
			{
				SXmlAttr  sXmlAttr;
				sXmlAttr.strName = pValueID[i];
				sXmlAttr.strValue = pValueText[i];
				sXmlCmd.listAttr.push_back(sXmlAttr);
			}
		}
		listXmlCmd.push_back(sXmlCmd);

		pszCmd = strstr(pszCmd, ";");
		if(pszCmd)
		{
			pszCmd += 1;
		}
	}

	nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
	SAFE_DELETE_ARG(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}

HRESULT HvMakeXmlCmdByString(bool fNewProtocol, const char* inXmlOrStrBuf,  int nInlen, char *szOutXmlBuf, int &nOutlen)
{
   	if(fNewProtocol)
	{
		return HvMakeXmlCmdByString2(inXmlOrStrBuf, nInlen, szOutXmlBuf, nOutlen);
	}
	TiXmlDocument cXmlDoc;

	if ( cXmlDoc.Parse(inXmlOrStrBuf) )
	{
		memcpy(szOutXmlBuf, inXmlOrStrBuf, nInlen);
		nOutlen = nInlen;
		return S_OK;
	}

	bool bGet = false;
	// ÎªŽŽœšXml×Œ±ž²ÎÊý¿ÕŒä
	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	int nCount = 0;
	char* pCmdName = pszInBufCopy;
	// ŒÙ¶šÃüÁîµÄ²ÎÊýžöÊý²»³¬¹ý64žö¡£
	char* pValueID[64];
	char* pValueText[64];
	char* pTemp;
	// ÅÐ¶ÏŒ°œâÎöXmlÀàÐÍ×Ö·ûŽ®
	while( pszInBufCopy = strchr(pszInBufCopy, ',') )
	{
		*pszInBufCopy = '\0';
		pValueID[nCount] = pszInBufCopy + 1;
		if ( ((pTemp = strchr(pszInBufCopy+2, '[')) == NULL)     // +2±íÊŸ±ØÐèÒªÓÐÒ»žö×Ö·û
			|| ((pszInBufCopy = strchr(pTemp+2, ']')) == NULL) ) // +2±íÊŸ±ØÐèÒªÓÐÒ»žö×Ö·û
		{
			goto done;
		}
		pValueText[nCount] = pTemp + 1;
		nCount++;

		*pTemp = '\0';
		*pszInBufCopy = '\0';
		pszInBufCopy++;
	}
	bGet = true;
done:
	if (bGet)
	{
		if (!fNewProtocol)
		{
			nOutlen = HvMakeXmlCmdReq(szOutXmlBuf, pCmdName, nCount, pValueID, pValueText);
		}
		else
		{
			XmlCmdList listXmlCmd;
			SXmlCmd  sXmlCmd;
			sXmlCmd.strCmdName = pCmdName;
			for (int i=0; i<nCount; ++i)
			{
				if (pValueID[i] && pValueText[i])
				{
					SXmlAttr  sXmlAttr;
					sXmlAttr.strName = pValueID[i];
					sXmlAttr.strValue = pValueText[i];
					sXmlCmd.listAttr.push_back(sXmlAttr);
				}
			}
			listXmlCmd.push_back(sXmlCmd);
			nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
		}
	}

	SAFE_DELETE_ARG(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;

}


HRESULT HvSendXmlCmd(char *szIP, LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT *piRetLen, int sktSend)
{
    if ( NULL == szIP || NULL == szCmd || NULL == szRetBuf || iBufLen <=0)
        return E_FAIL;
    const int iMaxLen = (1024 << 4);
    char *pszXmlBuf = new char[iMaxLen];
    int iXmlBufLen = iMaxLen;
    if ( pszXmlBuf == NULL )
        return E_FAIL;

    memset(pszXmlBuf, 0, iMaxLen);
    TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // 检查szCmd是否是XML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(IsNewProtocol(szIP), szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE(pszXmlBuf);
			return E_FAIL;
		}
		szCmd = (char*)pszXmlBuf;
	}
    bool fRet = ExecXmlExtCmdEx(szIP, (char*)szCmd, (char *)szRetBuf, iBufLen,sktSend);
    if ( piRetLen )
        *piRetLen = iBufLen;
    SAFE_DELETE_ARG(pszXmlBuf);
    return (true == fRet) ? S_OK : E_FAIL;
}

HRESULT HvParseXmlCmdRespRetcode(char* szXmlBuf, char* szCommand)
{
     HRESULT rst = E_FAIL;
 TiXmlDocument cXmlDoc;
 if ( cXmlDoc.Parse(szXmlBuf) )
 {
  const char* pszTextGet = NULL;
  TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
  if ( pCurElement == NULL ) goto done;
  TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
  if (pAttrib==NULL)
  {
   goto done;
  }
  if (0 == strcmp(pAttrib->Value(), "1.0"))   // 默认认为第一个属性就是ver
  { // 查找返回值
   pCurElement = pCurElement->FirstChildElement("RetCode");
   assert(pCurElement != NULL);
   pszTextGet = pCurElement->GetText();
  }
  else if (0 == strcmp(pAttrib->Value(), "2.0"))   // 默认认为第一个属性就是ver
  {
   for (;pCurElement; pCurElement = pCurElement->NextSiblingElement())
   {
    if (pCurElement->Attribute("RetCode") && pCurElement->GetText()
    && !strcasecmp(pCurElement->GetText(), szCommand))
    {
     pszTextGet = pCurElement->Attribute("RetCode");
    }
   }
  }
  if(NULL != pszTextGet && !atoi(pszTextGet))
  {
   rst = S_OK;
  }
 }
done:
 return rst;
}

HRESULT HvParseXmlCmdRespRetcode2(char*szXmlBuf, char* szCmdName,
char* szCmdValueName, char* szCmdValueText)
{
    if ( szXmlBuf == NULL ||szCmdName == NULL || szCmdValueName == NULL ||  szCmdValueText== NULL )
        return E_FAIL;

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szXmlBuf) == NULL  )
        return E_FAIL;

    const char* pszTextGet = NULL;
    TiXmlElement *pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
    if ( pCurElement == NULL )
        return E_FAIL;
    TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
    if ( pAttrib == NULL )
        return E_FAIL;
    if ( strcmp(pAttrib->Value(), "2.0") != 0 )
        return E_FAIL;

    HRESULT hr = E_FAIL;
    pCurElement = pCurElement->FirstChildElement();
    while ( pCurElement )
    {
        if ( strcmp( pCurElement->GetText(), szCmdName) == 0 )
        {
            pszTextGet = pCurElement->Attribute(szCmdValueName);
            if ( pszTextGet )
            {
                memcpy(szCmdValueText, pszTextGet, strlen(pszTextGet)+1);
                hr = S_OK;
                break;
            }
        }
    }
    return hr;
}

HRESULT HvParseXmlInfoRespValue(char*szXmlBuf, char* szInfoName,
char* nInfoValueName, char* szInfoValueText)
{
    HRESULT rst = E_FAIL;
    if ( szXmlBuf == NULL )
        return rst;

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szXmlBuf) == NULL )
        return rst;

    const char *szTextGet = NULL;
    TiXmlElement *pCurElement = cXmlDoc.FirstChildElement("HvInfoRespond");
    if ( pCurElement == NULL )
        return rst;
    TiXmlAttribute *pAttrib = pCurElement->FirstAttribute();
    if ( pAttrib == NULL || 0 != strcmp(pAttrib->Value(), "1.0") )
        return rst;

    for (pCurElement = pCurElement->FirstChildElement("Info"); pCurElement != NULL;
    pCurElement = pCurElement->NextSiblingElement("Info"))
    {
        pAttrib = pCurElement->FirstAttribute();
        szTextGet = pAttrib->Value();
        if ( strncasecmp(szInfoName, szTextGet, strlen(szTextGet)+1) == 0 )
        {
            TiXmlElement *pCurValueElement = NULL;
            for (pCurValueElement= pCurElement->FirstChildElement("Value"); pCurValueElement != NULL;
                    pCurValueElement = pCurValueElement->NextSiblingElement("Value"))
            {
                  pAttrib = pCurValueElement->FirstAttribute();
                 szTextGet = pAttrib->Value();
                 if ( strncasecmp(nInfoValueName, szTextGet, strlen(szTextGet)+1) == 0 )
                 {
                     szTextGet = pCurValueElement->GetText();
                     if ( szTextGet == NULL )
                        return rst;
                    memcpy(szInfoValueText, szTextGet, strlen(szTextGet)+1);
                    rst = S_OK;
                    break;
                 }
            }//FOR
        }//if
    }//for

    return rst;
}

static int HvMakeXmlInfoReq(char* szXmlBuf, int nInfoCount,
char** szInfoName, char** szInfoText )
{
    TiXmlDocument doc;
    TiXmlDeclaration *pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    doc.LinkEndChild(pDecl);

    TiXmlElement *pXmlRootElement = new TiXmlElement("HvInfo");
    pXmlRootElement->SetAttribute("ver", "1.0");
    doc.LinkEndChild(pXmlRootElement);
    for  ( int i=0; i<nInfoCount; i++ )
    {
        TiXmlElement *pXmlElementInfo = new TiXmlElement("Info");
        pXmlRootElement->LinkEndChild(pXmlElementInfo);

        pXmlElementInfo->SetAttribute("name", szInfoName[i]);
        if ( (szInfoText == NULL ) || (szInfoText[i] == NULL ))
            continue ;
        TiXmlText *pXmlIDText = new TiXmlText(szInfoText[i]);
        pXmlElementInfo->LinkEndChild(pXmlIDText);
    }
    TiXmlPrinter printer;
    doc.Accept(&printer);
    int len = (int)printer.Size();
    memcpy(szXmlBuf, printer.CStr(), len);
    szXmlBuf[len] = '\0';
    return len;
}

HRESULT HvMakeXmlInfoByString(bool fNewProtocol, const char*inXmlOrStrBuf, int nInlen,
char* szOutXmlBuf, int &nOutlen)
{
    bool bGet = false;
    char* pszInBufCopy = new char[nInlen+1];
    char* pszInBufCopyOld = pszInBufCopy;
    if ( pszInBufCopy == NULL )
        return E_FAIL;
    memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
    pszInBufCopy[nInlen] = '\0';
    int nCount = 1;
    char* pInfoName[64] = {pszInBufCopy};
    while( pszInBufCopy = strchr(pszInBufCopy, ','))
    {
        *pszInBufCopy = '\0';
        pszInBufCopy++;
        if ( *pszInBufCopy == '\0')
            break;
        pInfoName[nCount] = pszInBufCopy;
        nCount++;
    }//while

    if ( !fNewProtocol )
    {
        nOutlen = HvMakeXmlInfoReq(szOutXmlBuf, nCount, pInfoName, NULL );
    }
    else
    {
        XmlCmdList listXmlCmd;
        for (int i=0; i<nCount; ++i )
        {
            if ( pInfoName[i] )
            {
                SXmlCmd sXmlCmd;
                sXmlCmd.strCmdName = pInfoName[i];
                listXmlCmd.push_back(sXmlCmd);
            }
        }//for
        nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
    }
    delete[] pszInBufCopyOld;
    pszInBufCopyOld = NULL;
    return S_OK;
}

bool ExecXmlExtCmdEx(char *szIP, char *szXmlCmd, char *szRetBuf, int &iBufLen, int sktSend)
{
    if ( szIP == NULL || szXmlCmd == NULL )
        return false;

    if ( strlen(szXmlCmd) > (1024 << 4) )
        return false;

    bool fIsTempSocket = true;
    const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hSocketCmd = INVALID_SOCKET;
    CAMERA_CMD_HEADER cmdHearder;
    CAMERA_CMD_RESPOND cmdRespond;
    if ( sktSend == INVALID_SOCKET )
    {
        if ( !ConnectCamera(szIP, wPort, hSocketCmd) )
            return false;
        sktSend = hSocketCmd;
    }
    else
    {
        hSocketCmd = sktSend;
        fIsTempSocket = false;
    }

    struct timeval tv;
    tv.tv_sec = 12;
    tv.tv_usec=0;
    setsockopt(hSocketCmd, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval));
    bool fIsOK = false;
    cmdHearder.dwID = CAMERA_XML_EXT_CMD;
    cmdHearder.dwInfoSize = (int )strlen(szXmlCmd) + 1;
    int iHeaderLen = sizeof(cmdHearder);
    if ( iHeaderLen  !=  send( hSocketCmd, (const char*)&cmdHearder, iHeaderLen, MSG_NOSIGNAL) )
    {
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    int iValue = send( hSocketCmd, (const char*)szXmlCmd, cmdHearder.dwInfoSize, MSG_NOSIGNAL);
    if ( cmdHearder.dwInfoSize  !=   iValue )
    {
        int error = errno;
        perror("Send");
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    int iRespondLen = sizeof(cmdRespond);
    if (  iRespondLen != RecvAll(hSocketCmd, (char*)&cmdRespond, iRespondLen) )
    {
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    if ( CAMERA_XML_EXT_CMD  != cmdRespond.dwID  ||  0 != cmdRespond.dwResult  ||  cmdRespond.dwInfoSize < 0 )
    {
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    const int iMaxLen = (1024 << 10 );
    char *pszTmpXmlBuf = new char[iMaxLen];
     if ( pszTmpXmlBuf == NULL )
        return false;
    memset(pszTmpXmlBuf, 0, iMaxLen);

    int iRetLen = RecvAll(hSocketCmd, pszTmpXmlBuf, cmdRespond.dwInfoSize);
    if ( cmdRespond.dwInfoSize != iRetLen )
    {
        iBufLen = 0;
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(pszTmpXmlBuf);
        return false;
    }

    if ( iBufLen >  (int)cmdRespond.dwInfoSize )
    {
        memcpy(szRetBuf, pszTmpXmlBuf, cmdRespond.dwInfoSize );
        szRetBuf[cmdRespond.dwInfoSize] = 0;
        iBufLen = cmdRespond.dwInfoSize;
        fIsOK = true;
    }
    else
    {
        int iTmpLen =  iBufLen-1;
        memcpy(szRetBuf, pszTmpXmlBuf, iTmpLen );
        szRetBuf[iTmpLen] = 0;
        fIsOK = true;
    }

      if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(pszTmpXmlBuf);

    return  fIsOK;
}


void HvSafeCloseThread(pthread_t  &pthreadHandle)
{
    if ( pthreadHandle !=  0 )
    {
        pthread_join(pthreadHandle, NULL);
        pthreadHandle = 0;
    }
}

typedef struct _IMAGE_INFO
{
    DWORD32 dwCarID;
    DWORD32 dwImgType;
    DWORD32 dwImgWidth;
    DWORD32 dwImgHeight;
    DWORD32 dwTimeLow;
    DWORD32 dwTimeHigh;
    DWORD32 dwEddyType;
    RECT rcPlate;
    RECT rcRedLightPos[20];
    INT iRedLightCount;
    RECT rcFacePos[20];
    int nFaceCount;
    _IMAGE_INFO()
    {
        memset(this, 0, sizeof(*this));
    }
}IMAGE_INFO;

HRESULT HvGetRecordImage(PBYTE pbRecordData, DWORD dwRecordDataLen, RECORD_IMAGE_GROUP_EX* pcRecordImage)
{
    if ( pcRecordImage == NULL || pbRecordData == NULL || dwRecordDataLen < 0 )
    {
        WriteLog("HvGetRecordImage-- pcRecordImage == NULL or Other");
        return E_FAIL;
    }

    IMAGE_INFO cImageInfo;
    DWORD dwImgInfoLen = 0;
    DWORD dwImgDataLen = 0;
    PBYTE pbTempData = pbRecordData;
    PBYTE pbTempInfo = NULL;
    DWORD dwRemainLen = dwRecordDataLen;
    memset(pcRecordImage, 0, sizeof(RECORD_IMAGE_GROUP_EX));
    while ( dwRemainLen > 0 )
    {
        memcpy(&dwImgInfoLen, pbTempData, 4);
        pbTempData += 4;
        if  ( dwImgInfoLen > dwRemainLen || dwImgInfoLen <= 0)
        {
            char chLog[260] = {0};
            sprintf(chLog, "HvGetRecordImage-- dwImgInfoLen > dwRemainLen , dwImgInfoLen=%x, dwRemainLen = %x", dwImgInfoLen,dwRemainLen );
            WriteLog(chLog);
            return E_FAIL;
        }

        long iBuffLen = 0;
        iBuffLen = (sizeof(IMAGE_INFO) < dwImgInfoLen) ? sizeof(IMAGE_INFO) : dwImgInfoLen;
        memcpy(&cImageInfo, pbTempData, iBuffLen);
        pbTempInfo = pbTempData;
        pbTempData += dwImgInfoLen;

        memcpy(&dwImgDataLen, pbTempData, 4 );
        pbTempData+=4;
        if ( dwImgDataLen > dwRemainLen )
        {
            char chLog[260] = {0};
            sprintf(chLog, "HvGetRecordImage-- dwImgDataLen > dwRemainLen , dwImgDataLen=%x, dwRemainLen = %x", dwImgDataLen,dwRemainLen );
            WriteLog(chLog);
            return E_FAIL;
        }

        switch (cImageInfo.dwImgType)
        {
            case RECORD_IMAGE_BEST_SNAPSHOT:
                {
                    pcRecordImage->cBestSnapshot.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cBestSnapshot.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cBestSnapshot.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs<<=32;
                    pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cBestSnapshot.pbImgData = pbTempData;
                    pcRecordImage->cBestSnapshot.pbImgInfo = pbTempInfo;
                    pcRecordImage->cBestSnapshot.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cBestSnapshot.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cBestSnapshot.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cBestSnapshot.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));

                }
            break;
            case RECORD_IMAGE_LAST_SNAPSHOT:
             {
                    pcRecordImage->cLastSnapshot.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cLastSnapshot.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cLastSnapshot.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cLastSnapshot.pbImgData = pbTempData;
                    pcRecordImage->cLastSnapshot.pbImgInfo = pbTempInfo;
                    pcRecordImage->cLastSnapshot.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cLastSnapshot.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cLastSnapshot.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));
                }
            break;

            case RECORD_IMAGE_BEGIN_CAPTURE:
             {
                    pcRecordImage->cBeginCapture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cBeginCapture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cBeginCapture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cBeginCapture.pbImgData = pbTempData;
                    pcRecordImage->cBeginCapture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cBeginCapture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cBeginCapture.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cBeginCapture.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cBeginCapture.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));
                }
            break;
            case RECORD_IMAGE_BEST_CAPTURE:
             {
                    pcRecordImage->cBestCapture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cBestCapture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cBestCapture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cBestCapture.pbImgData = pbTempData;
                    pcRecordImage->cBestCapture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cBestCapture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cBestCapture.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cBestCapture.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cBestCapture.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));

                }
            break;
            case RECORD_IMAGE_LAST_CAPTURE:
             {
                    pcRecordImage->cLastCapture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cLastCapture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cLastCapture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cLastCapture.pbImgData = pbTempData;
                    pcRecordImage->cLastCapture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cLastCapture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cLastCapture.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cLastCapture.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cLastCapture.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));

                }
            break;
            case RECORD_IMAGE_SMALL_IMAGE:
             {
                    pcRecordImage->cPlatePicture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cPlatePicture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cPlatePicture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;
                      pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs <<=32;
                    pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cPlatePicture.pbImgData = pbTempData;
                    pcRecordImage->cPlatePicture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cPlatePicture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cPlatePicture.dwImgInfoLen = dwImgInfoLen;
                }
            break;
            case RECORD_IMAGE_BIN_IMAGE:
             {
                    pcRecordImage->cPlateBinary.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cPlateBinary.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cPlateBinary.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;
                    pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cPlateBinary.pbImgData = pbTempData;
                    pcRecordImage->cPlateBinary.pbImgInfo = pbTempInfo;
                    pcRecordImage->cPlateBinary.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cPlateBinary.dwImgInfoLen = dwImgInfoLen;
                }
            break;
            default :
            break;
        }
        pbTempData+=dwImgDataLen;
        dwRemainLen -= (8+dwImgInfoLen+dwImgDataLen);
    }

    return S_OK;
}



////////////////////////////

// ---------------------------------------------------------------------


static inline int SetInt32ToByteArray(BYTE *pbDest, INT32 i32Src)
{
	memcpy(pbDest, &i32Src, sizeof(i32Src));
	return sizeof(i32Src);
}

static inline int SetInt16ToByteArray(BYTE *pbDest, INT16 i16Src)
{
	memcpy(pbDest, &i16Src, sizeof(i16Src));
	return sizeof(i16Src);
}

// 描述:	转换YUV数据到RGB
// 参数:	pbDest			输出RGB数据的缓冲区指针;
//			pbSrc			输入YUV数据的缓冲区指针;
//			iSrcWidth		图像宽度;
//			iSrcHeight		图像高度;
//			iBGRStride		RGB数据每行的步长;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
static HRESULT Yuv2Rgb(
					   BYTE *pbDest,
					   BYTE *pbSrc,
					   int iSrcWidth,
					   int iSrcHeight,
					   int iBGRStride
					   )
{
	bool fBottomUp = true;
	if (iSrcHeight < 0)
	{
		iSrcHeight = -iSrcHeight;
		fBottomUp = false;
	}
	int x, y;
	unsigned char *pY = reinterpret_cast< unsigned char* >(pbSrc);
	unsigned char *pCb = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight;
	unsigned char *pCr = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight + (iSrcWidth >> 1) * iSrcHeight;
	for (y = 0; y < iSrcHeight; y++)
		for (x = 0; x < iSrcWidth; x++)
		{
			int iY = *(pY + y * iSrcWidth + x);
			int iCb = *(pCb + y * (iSrcWidth >> 1) + (x >> 1));
			int iCr = *(pCr + y * (iSrcWidth >> 1) + (x >> 1));
			int iR = static_cast< int >(1.402 * (iCr - 128) + iY);
			int iG = static_cast< int >(-0.34414 * (iCb - 128) - 0.71414 * (iCr - 128) + iY);
			int iB = static_cast< int >(1.772 * (iCb - 128) + iY);
			if (iR > 255)
				iR = 255;
			if (iR < 0)
				iR = 0;
			if (iG > 255)
				iG = 255;
			if (iG < 0)
				iG = 0;
			if (iB > 255)
				iB = 255;
			if (iB < 0)
				iB = 0;
			if (fBottomUp)
			{
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 ] = iB;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 2 ] = iR;
			}
			else
			{
				pbDest[ y * iBGRStride + x * 3 ] = iB;
				pbDest[ y * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ y * iBGRStride + x * 3 + 2 ] = iR;
			}
		}
		return S_OK;
}

HRESULT Yuv2BMP(
				BYTE *pbDest,
				int iDestBufLen,
				int *piDestLen,
				BYTE *pbSrc,
				int iSrcWidth,
				int iSrcHeight
				)
{
	DWORD32 dwOffSet = 54;
	DWORD32 dwRBGStride = ((iSrcWidth * 24 + 31) >> 5) << 2;
	DWORD32 dwRgbSize = dwRBGStride * iSrcHeight;
	*piDestLen = dwOffSet + dwRgbSize;
	if (*piDestLen > iDestBufLen)
	{
		*piDestLen = 0;
		return E_FAIL;
	}

	*(pbDest++) = 'B';										//BMP文件标志.
	*(pbDest++) = 'M';
	pbDest += SetInt32ToByteArray(pbDest, *piDestLen);		//BMP文件大小.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//reserved
	pbDest += SetInt32ToByteArray(pbDest, dwOffSet);		//文件头偏移量.
	pbDest += SetInt32ToByteArray(pbDest, 0x28);			//biSize.
	pbDest += SetInt32ToByteArray(pbDest, iSrcWidth);		//biWidth.
	pbDest += SetInt32ToByteArray(pbDest, iSrcHeight);		//biHeight.
	pbDest += SetInt16ToByteArray(pbDest, 1);				//biPlanes.
	pbDest += SetInt16ToByteArray(pbDest, 24);				//biBitCount.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biCompression.
	pbDest += SetInt32ToByteArray(pbDest, dwRgbSize);		//biSizeImage.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biXPerMeter.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biYPerMeter.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biClrUsed.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biClrImportant.
	Yuv2Rgb(pbDest, pbSrc, iSrcWidth, iSrcHeight, dwRBGStride);
	return S_OK;
}

// ---------------------------------------------------------------------

const int BIN_BIT_COUNT = 8;
const int BIN_WIDTH = 112;
const int BIN_HEIGHT = 20;
const int BIN_STRIDE = BIN_WIDTH / BIN_BIT_COUNT;
const int BIN_BYTE_COUNT = BIN_HEIGHT * BIN_STRIDE;

static void RgbBin2ByteBin(PBYTE m_rgbBinImage, PBYTE pbByteBinImage)
{
	for (int y = 0; y < BIN_HEIGHT; y++)
	{
		BYTE *pSrc = &m_rgbBinImage[(BIN_HEIGHT - 1) * BIN_STRIDE];
		BYTE *pDest = pbByteBinImage;
		for (int y = 0; y < BIN_HEIGHT; y++)
		{
			memcpy(pDest, pSrc, BIN_STRIDE);
			pSrc -= BIN_STRIDE;
			pDest += BIN_STRIDE;
		}
		//字节中前后bit位互换
		for (int i = 0; i < BIN_BYTE_COUNT; i++)
		{
			BYTE bTemp = pbByteBinImage[i];
			BYTE bTemp2 = 0;
			bTemp2 |= (bTemp & 0x80) >> 7;
			bTemp2 |= (bTemp & 0x40) >> 5;
			bTemp2 |= (bTemp & 0x20) >> 3;
			bTemp2 |= (bTemp & 0x10) >> 1;
			bTemp2 |= (bTemp & 0x08) << 1;
			bTemp2 |= (bTemp & 0x04) << 3;
			bTemp2 |= (bTemp & 0x02) << 5;
			bTemp2 |= (bTemp & 0x01) << 7;
			pbByteBinImage[i] = bTemp2;
		}
	}
}

static BOOL BinImage2BitmapData(int iCX, int iCY, BYTE* pbByteBinImage, char* pbBitmapData, int* piBitmapDataLen)
{
	BOOL bSuccess=TRUE;
	int iByteBinImageLen = BIN_BYTE_COUNT;
	if ( iCX != BIN_WIDTH || iCY != BIN_HEIGHT || pbByteBinImage == NULL || piBitmapDataLen == NULL )
	{
		bSuccess = FALSE;
	}
	else
	{
		int iStride = ((BIN_STRIDE + 3) >> 2) << 2;
		int iBmpDataSize = iStride * BIN_HEIGHT;

		BITMAPFILEHEADER bmfHdr;
		bmfHdr.bfType = 0x4d42;
		bmfHdr.bfSize = iBmpDataSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
		bmfHdr.bfReserved1 = 0;
		bmfHdr.bfReserved2 = 0;
		bmfHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);

		BITMAPINFO *pInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
		pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pInfo->bmiHeader.biWidth = BIN_WIDTH;
		pInfo->bmiHeader.biHeight = BIN_HEIGHT;
		pInfo->bmiHeader.biPlanes = 1;
		pInfo->bmiHeader.biBitCount = 1;
		pInfo->bmiHeader.biCompression = BI_RGB;
		pInfo->bmiHeader.biSizeImage = 0;
		pInfo->bmiHeader.biXPelsPerMeter = 0;
		pInfo->bmiHeader.biYPelsPerMeter = 0;
		pInfo->bmiHeader.biClrUsed = 0;
		pInfo->bmiHeader.biClrImportant = 0;

		pInfo->bmiColors[0].rgbBlue = 0;
		pInfo->bmiColors[0].rgbGreen = 0;
		pInfo->bmiColors[0].rgbRed = 0;
		pInfo->bmiColors[0].rgbReserved = 0;
		pInfo->bmiColors[1].rgbBlue = 255;
		pInfo->bmiColors[1].rgbGreen = 255;
		pInfo->bmiColors[1].rgbRed = 255;
		pInfo->bmiColors[1].rgbReserved = 0;

		BYTE *pBmpData = (BYTE*)malloc(iBmpDataSize);
		memset(pBmpData, 0, iBmpDataSize);
		BYTE *pSrc = pbByteBinImage;
		BYTE *pDest = pBmpData;
		for (int i = 0; i < BIN_HEIGHT; i++)
		{
			memcpy(pDest, pSrc, BIN_STRIDE);
			pDest += iStride;
			pSrc += BIN_STRIDE;
		}

		memcpy(pbBitmapData, &bmfHdr, sizeof(BITMAPFILEHEADER));
		pbBitmapData += sizeof(BITMAPFILEHEADER);
		*piBitmapDataLen += sizeof(BITMAPFILEHEADER);

		memcpy(pbBitmapData, pInfo, sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
		pbBitmapData += sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
		*piBitmapDataLen += sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);

		memcpy(pbBitmapData, pBmpData, iBmpDataSize);
		pbBitmapData += iBmpDataSize;
		*piBitmapDataLen += iBmpDataSize;

		free(pBmpData);
		free(pInfo);
	}

	return bSuccess;
}

void Bin2BMP(PBYTE pbBinData, PBYTE pbBmpData, INT& nBmpLen)
{
	PBYTE pbByteBinImage[BIN_BYTE_COUNT] = {0};
	RgbBin2ByteBin(pbBinData, (PBYTE)pbByteBinImage);

	int iBitmapDataLen = 0;
	BinImage2BitmapData(BIN_WIDTH, BIN_HEIGHT, (PBYTE)pbByteBinImage, (char*)pbBmpData, &iBitmapDataLen);
	nBmpLen = iBitmapDataLen;
}

// ---------------------------------------------------------------------

int BuildHvCmdXml(
    char* pXmlBuf,
    char* pCmdName,
    int iArgCount,
    const char rgszName[][IVN_MAX_SIZE],
    const char rgszValue[][IVN_MAX_SIZE]
)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "2.0");

	doc.LinkEndChild(pXmlRootElement);

	TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
	pXmlRootElement->LinkEndChild(pXmlElementCmd);

	TiXmlText *pXmlCmdText = new TiXmlText(pCmdName);
	pXmlElementCmd->LinkEndChild(pXmlCmdText);

	if (iArgCount != 0)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdArg");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		for (int i=0; i<iArgCount; i++)
		{
			TiXmlElement *pXmlElementID = new TiXmlElement("Value");
			pXmlElementCmd->LinkEndChild(pXmlElementID);

			pXmlElementID->SetAttribute("id", rgszName[i]);

			if (0 == strcmp(rgszValue[i] ,""))
				continue;

			TiXmlText *pXmlIDText = new TiXmlText(rgszValue[i]);
			pXmlElementID->LinkEndChild(pXmlIDText);
		}
	}


	TiXmlPrinter printer;
	doc.Accept(&printer);

	int len = (int)printer.Size();
	memcpy(pXmlBuf, printer.CStr(), len);
	pXmlBuf[len] = '\0';

	return len;
}


HRESULT GetParamStringFromXml(
							  const TiXmlElement* pCmdArgElement,
							  const char *pszParamName,
							  char *pszParamValue,
							  int nParamValueSize
							  )
{
	if ( pCmdArgElement == NULL
		|| pszParamName == NULL
		|| pszParamValue == NULL
		|| nParamValueSize <= 0 )
	{
		return E_POINTER;
	}

	bool fFind = false;
	const TiXmlElement* pValueElement = pCmdArgElement->FirstChildElement("Value");
	while ( pValueElement )
	{
		if ( 0 == strcmp(pszParamName, pValueElement->Attribute("id")) )
		{
			if ( strlen(pValueElement->GetText()) < (size_t)nParamValueSize )
			{
				fFind = true;
				strcpy(pszParamValue, pValueElement->GetText());
				break;
			}
		}
		pValueElement = pValueElement->NextSiblingElement();
	}

	return fFind ? S_OK : E_FAIL;
}

HRESULT GetParamIntFromXml(
						   const TiXmlElement* pCmdArgElement,
						   const char *pszParamName,
						   int *pnParamValue
						   )
{
	if (pCmdArgElement == NULL || pszParamName == NULL || pnParamValue == NULL)
	{
		return E_POINTER;
	}

	bool fFind = false;
	const TiXmlElement* pValueElement = pCmdArgElement->FirstChildElement("Value");
	while ( pValueElement )
	{
		if ( 0 == strcmp(pszParamName, pValueElement->Attribute("id")) )
		{
			fFind = true;
			(*pnParamValue) = atoi(pValueElement->GetText());
			break;
		}
		pValueElement = pValueElement->NextSiblingElement();
	}

	return fFind ? S_OK : E_FAIL;
}

bool ExecXmlExtCmd(char* szXmlCmd, char* szRetBuf,
					 int& iBufLen, SOCKET sktSend)
{
	if ( strlen(szXmlCmd) > (1024 << 4) )
	{
		return false;
	}
	const int iMaxLen = (1024 << 10);
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
	{
		return false;
	}
	memset(pszTmpXmlBuf, 0, iMaxLen);
	bool fIsTempSocket = true;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;
	if(sktSend == INVALID_SOCKET)
	{
		return false;
	}
	else
	{
		hSocketCmd = sktSend;
		fIsTempSocket = false;
	}
	bool fIsOk = false;
	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
	int isend0 = send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) ;
	if ( sizeof(cCmdHeader) != isend0)
	{
		if(fIsTempSocket)
		{
			ForceCloseSocket(hSocketCmd);
		}
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return false;
	}

	int isend = send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize, 0);
	if ( cCmdHeader.dwInfoSize !=  isend)
	{
		//int iTemp = WSAGetLastError();
		if(fIsTempSocket)
		{
			ForceCloseSocket(hSocketCmd);
		}
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return false;
	}
	if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
	{
		if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult
			&& 0 < cCmdRespond.dwInfoSize )
		{
			int iRecvLen = RecvAll(hSocketCmd, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
			if ( cCmdRespond.dwInfoSize ==  iRecvLen)
			{
				if ( iBufLen > (int)cCmdRespond.dwInfoSize )
				{
					memcpy(szRetBuf, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
					memset(szRetBuf+cCmdRespond.dwInfoSize, 0, 1);
					iBufLen = cCmdRespond.dwInfoSize;
					if(fIsTempSocket)
					{
						ForceCloseSocket(hSocketCmd);
					}
					SAFE_DELETE(pszTmpXmlBuf);
					return true;
				}
				else
				{
					iBufLen = cCmdRespond.dwInfoSize;
					if(fIsTempSocket)
					{
						ForceCloseSocket(hSocketCmd);
					}
					SAFE_DELETE_ARG(pszTmpXmlBuf);
					return false;
				}
			}
			else
			{
				if(fIsTempSocket)
				{
					ForceCloseSocket(hSocketCmd);
				}
				SAFE_DELETE_ARG(pszTmpXmlBuf);
				return false;
			}
		}
		fIsOk = true;
	}

	if(fIsTempSocket)
	{
		ForceCloseSocket(hSocketCmd);
	}

	SAFE_DELETE_ARG(pszTmpXmlBuf);
	return fIsOk;
}


HRESULT HvSendXmlCmd(LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT* piRetLen, DWORD dwXmlVersion, SOCKET sktSend)
{
	if (NULL == szCmd || NULL == szRetBuf || iBufLen <= 0 )
	{
		return E_FAIL;
	}
	const int iMaxLen = (1024 << 4);
	char* pszXmlBuf = new char[iMaxLen];  // 16KB
	int iXmlBufLen = iMaxLen;
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	memset(pszXmlBuf, 0, iMaxLen);
	TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // Œì²észCmdÊÇ·ñÊÇXML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(true , szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
		szCmd = (char*)pszXmlBuf;
	}
	//printf("%s----\n", szCmd);
	bool fRet = ExecXmlExtCmd((char*)szCmd, (char*)szRetBuf, iBufLen, sktSend);
	if ( piRetLen )
	{
		*piRetLen = iBufLen;
	}
	SAFE_DELETE_ARG(pszXmlBuf);
	return (true == fRet) ? S_OK : E_FAIL;
}


HRESULT HvSetRecvTimeOut( const SOCKET& hSocket, int iMS )
{
	struct timeval uTime;
	uTime.tv_sec = iMS / 1000;
	uTime.tv_usec = (iMS % 1000) * 1000;
	int iRet = setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&uTime, sizeof(timeval));
	return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvSetSendTimeOut( const SOCKET& hSocket, int iMS )
{
    /*
    struct timeval tv;
    tv.tv_sec = 8;
    tv.tv_usec=0;
    if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval))  != 0 )
            return -1;*/

	struct timeval uTime;
	uTime.tv_sec = iMS / 1000;
	uTime.tv_usec = (iMS % 1000) * 1000;
	int iRet = setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&uTime, sizeof(timeval));
	return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvListen( const SOCKET& hSocket, int nPort, int backlog )
{
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(nPort);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	int iOpt = 1;
	int iOptLen = sizeof(iOpt);
	setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);

	int iRet = -1;
	iRet = bind(hSocket, (sockaddr*)&sa, sizeof(struct sockaddr));
	if ( 0 != iRet ) return E_FAIL;

	iRet = listen(hSocket, backlog);
	return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvAccept( const SOCKET& hSocket, SOCKET& hNewSocket, int iTimeout/*=-1*/ )
{
	if (INVALID_SOCKET == hSocket)	return E_INVALIDARG;

	struct sockaddr_in addr;
	//size_t addrlen = sizeof(addr);
	socklen_t addrlen = sizeof(addr); // 64bit

	if ( -1 == iTimeout )
	{
		hNewSocket = accept(hSocket, (struct sockaddr*)&addr, &addrlen);
		return (INVALID_SOCKET!=hNewSocket) ? S_OK : E_FAIL;
	}

	HRESULT hr = E_FAIL;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(hSocket, &r);

	struct timeval timeout;
	timeout.tv_sec = iTimeout / 1000;
	timeout.tv_usec = (iTimeout % 1000) * 1000;

	int ret = select(hSocket+1, &r, NULL, NULL, &timeout);
	if (ret == 1)
	{
		hNewSocket = accept(hSocket, (struct sockaddr*)&addr, &addrlen);
		hr = ( INVALID_SOCKET != hNewSocket ) ? S_OK : E_FAIL;
	}
	else if (ret == 0)
	{
		hr = S_FALSE;
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

SOCKET HvCreateSocket( int af/*=AF_INET*/, int type/*=SOCK_STREAM*/, int protocol/*=0*/ )
{
	return socket(af, type, protocol);
}

//红灯加红
HRESULT HvEnhanceTrafficLight(PBYTE pbSrcImg, DWORD dwiSrcImgDataLen, int iRedLightCount,
							  PBYTE pbRedLightPos, PBYTE pbDestImgBuf, DWORD& dwDestImgBufLen,
							  INT iBrightness, INT iHueThreshold, INT iCompressRate)
{
	if(pbSrcImg == NULL || dwiSrcImgDataLen <= 0) return E_FAIL;
	if(pbDestImgBuf == NULL || dwDestImgBufLen <= 0) return E_FAIL;
	if(pbRedLightPos == NULL && sizeof(pbRedLightPos)/sizeof(RECT) < iRedLightCount)return E_FAIL;

	RECT* pcRect = new RECT[iRedLightCount*sizeof(RECT)];
	if(pcRect == NULL)return E_FAIL;
	memcpy(pcRect, pbRedLightPos, iRedLightCount*sizeof(RECT));
	if(iBrightness < -255) iBrightness = -255;
	if(iBrightness > 255) iBrightness = 255;

	CxImage imgSrc(pbSrcImg, dwiSrcImgDataLen, CXIMAGE_FORMAT_UNKNOWN);
	if(!imgSrc.IsValid())
	{
	    fprintf(stderr, "%s\n", imgSrc.GetLastError());
	    return E_FAIL;
	}
	int iWidth = imgSrc.GetWidth();
	int iHeight = imgSrc.GetHeight();

	if(iRedLightCount < 1)
	{
		pcRect[0].left = 0;
		pcRect[0].top = 0;
		pcRect[0].right = iWidth;
		pcRect[0].bottom = iHeight;
		iRedLightCount = 1;
	}

	for(int i=0; i<iRedLightCount; i++)
	{
		RECT cTempRect;
		LONG lTemp;
		cTempRect.left = pcRect[i].left;
		cTempRect.top = pcRect[i].top;
		cTempRect.right = pcRect[i].right;
		cTempRect.bottom = pcRect[i].bottom;

		if(cTempRect.left > cTempRect.right)
		{
			lTemp = cTempRect.left;
			cTempRect.left = cTempRect.right;
			cTempRect.right = lTemp;
		}

		if(cTempRect.top > cTempRect.bottom)
		{
			lTemp = cTempRect.top;
			cTempRect.top = cTempRect.bottom;
			cTempRect.bottom = lTemp;
		}

		int iRectWidht, iRectHeight;
		iRectWidht = cTempRect.right - cTempRect.left;
		iRectHeight = cTempRect.bottom - cTempRect.top;
		if(iRectWidht <= 0 || iRectHeight <= 0)
		{
			continue;
		}
		CxImage imgCrop, imgH, imgS, imgV;

        if(!imgSrc.Crop(cTempRect.left, cTempRect.top, cTempRect.right, cTempRect.bottom, &imgCrop))
//		if(!imgSrc.Crop(cTempRect, &imgCrop))
		{
			fprintf(stderr, "Crop:%s\n", imgSrc.GetLastError());
			continue;
		}
		if(!imgCrop.SplitHSL(&imgH, &imgS, &imgV))
		{
			fprintf(stderr, "SplitHSL:%s\n", imgSrc.GetLastError());
			continue;
		}

		RGBQUAD h, s, v;
		int iSumH = 0, iSumV = 0, iSumS = 0;
		int iPixelCount(0);

		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				h = imgH.GetPixelColor(x, y);
				s = imgS.GetPixelColor(x, y);
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue > 40 && v.rgbBlue < 230)
				{
					iSumH += h.rgbBlue;
					iSumS += s.rgbBlue;
					iSumV += v.rgbBlue;
					iPixelCount++;
				}
			}
		}
		if(iPixelCount == 0)
		{
			continue;
		}
		int iAvgH = iSumH / iPixelCount;
		if(iAvgH > iHueThreshold)
		{
			continue;
		}
		int iAvgV = iSumV / iPixelCount;
		int iAvgS = iSumS / iPixelCount;

		iSumV = 0;
		iPixelCount = 0;
		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue >= iAvgV)
				{
					iSumV += v.rgbBlue;
					iPixelCount++;
				}
			}
		}
		int iHeightAvgV = iAvgV;
		if(iPixelCount != 0) iHeightAvgV = iSumV / iPixelCount;

		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				h = imgH.GetPixelColor(x, y);
				s = imgS.GetPixelColor(x, y);
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue <= 20 || s.rgbBlue < 40 || (h.rgbBlue > 60 && h.rgbBlue < 230)) continue;
				if(v.rgbBlue >= iHeightAvgV)
				{
					v.rgbRed = (BYTE)(v.rgbBlue - (v.rgbBlue-iHeightAvgV)*0.4);
					v.rgbGreen = v.rgbRed;
					v.rgbBlue = v.rgbRed;
//					imgV.SetPixelColor(x, y, v);
					COLORREF crV=RGB(v.rgbRed,v.rgbGreen,v.rgbBlue);
                    imgV.SetPixelColor(x, y, crV);

					s.rgbRed = iAvgS;
					s.rgbGreen = s.rgbRed;
					s.rgbBlue = s.rgbRed;
//					imgS.SetPixelColor(x, y, s);
					COLORREF crS=RGB(s.rgbRed,s.rgbGreen,s.rgbBlue);
					imgS.SetPixelColor(x, y, crS);
				}
				h.rgbBlue = 0;
				h.rgbGreen = 0;
				h.rgbRed = 0;
//				imgH.SetPixelColor(x, y, h);
				COLORREF crH=RGB(h.rgbRed,h.rgbGreen,h.rgbBlue);
				imgH.SetPixelColor(x, y, crH);
			}
		}
		imgCrop.Combine(&imgH, &imgS, &imgV, NULL, 1);
		imgSrc.Mix(imgCrop, CxImage::OpDstCopy, -cTempRect.left, cTempRect.bottom-iHeight);
	}
	delete[] pcRect;
	if(iBrightness)
	{
		imgSrc.Light(iBrightness);
	}
	long lsize = 0;
	BYTE* pbuffer = NULL;
	imgSrc.SetJpegQuality(iCompressRate);
	if(!imgSrc.Encode(pbuffer, lsize, CXIMAGE_FORMAT_JPG))
	{
	    fprintf(stderr, "Encode:%s\n", imgSrc.GetLastError());
	    return E_FAIL;
	}
	if(lsize > (int)dwDestImgBufLen)
	{
		imgSrc.FreeMemory(pbuffer);
		dwDestImgBufLen = lsize;
		return E_FAIL;
	}
	memcpy(pbDestImgBuf, pbuffer, lsize);
	dwDestImgBufLen = lsize;
	imgSrc.FreeMemory(pbuffer);
	return S_OK;

}
