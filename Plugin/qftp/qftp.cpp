// ¸ÃÎÄ¼ş±àÂë±ØĞëÊÇWINDOWS-936¸ñÊ½
#include <ctime>
#include <iostream>
#include <fstream>
#include "qftp.h"
#include "swbasetype.h"
#include "HvSockUtils.h"


#ifdef CHIP_6467
#include "HvSockUtils.h"
#endif

using namespace std;

using namespace nsFTP;

CQFTP::CQFTP(const string& strFtpIp,const string& strUserName, const string& strPassWord, UINT nTimeout)
        : m_socketCtrlConnect(0)
        , m_wTimeout(nTimeout)
        , m_strFtpIP(strFtpIp)
        , m_strUserName(strUserName)
        , m_strPassWord(strPassWord)
{

}

CQFTP::~CQFTP()
{
    if (0 != m_socketCtrlConnect)
    {
        CloseSocket(m_socketCtrlConnect);      // Ö±½Ó¹Ø±ÕÁ¬½Ó
        m_socketCtrlConnect = 0;
    }
}

HRESULT SvSetRecvTimeOut(const int& hSocket, int iMS)
{
    struct timeval uTime;
    uTime.tv_sec = iMS / 1000;
    uTime.tv_usec = (iMS % 1000) * 1000;
    int iRet = setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&uTime, sizeof(timeval));
    return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT SvSetSendTimeOut(const int& hSocket, int iMS)
{
    struct timeval uTime;
    uTime.tv_sec = iMS / 1000;
    uTime.tv_usec = (iMS % 1000) * 1000;
    int iRet = setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&uTime, sizeof(timeval));
    return (0 == iRet) ? S_OK : E_FAIL;
}

//æ³¨ï¼šLinuxä¸‹å¼‚æ­¥SOCKETçš„è¿æ¥å¹¶æ²¡æœ‰è¶…æ—¶çš„è¯´æ³•ï¼Œå› æ­¤åœ¨è°ƒç”¨è¯¥å‡½æ•°å¤±è´¥åè¯·è‡ªè¡ŒSleepä¸€æ®µæ—¶é—´ã€‚
HRESULT SvConnect(const int& hSocket, const char* pszIPv4, WORD16 wPort, int iTimeout/*=-1*/)
{
    if (INVALID_SOCKET == hSocket || NULL == pszIPv4 )	return E_INVALIDARG;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(wPort);
    addr.sin_addr.s_addr = inet_addr(pszIPv4);

    int iRtn = 0;
    if ( -1 == iTimeout )
    {
        iRtn = connect(hSocket, (sockaddr*)&addr, sizeof(addr));
        return (0 == iRtn) ? S_OK : E_FAIL;
    }

    HRESULT hr = E_FAIL;

    // è®¾ä¸ºéé˜»å¡æ¨¡å¼
    int flags = fcntl(hSocket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(hSocket, F_SETFL, flags);

    iRtn = connect(hSocket, (sockaddr*)&addr, sizeof(addr));

    if ( 0 == iRtn )
    {
        hr = S_OK;
    }
    else
    {
        fd_set w;
        FD_ZERO(&w);
        FD_SET(hSocket, &w);

        struct timeval timeout;
        timeout.tv_sec = iTimeout / 1000;
        timeout.tv_usec = (iTimeout % 1000) * 1000;

        int ret = select(hSocket + 1, 0, &w, 0, &timeout);
        if (ret <= 0)
        {
            hr = E_FAIL;
        }
        else if (!FD_ISSET(hSocket, &w))
        {
            hr = E_FAIL;
        }
        else
        {
            ret = 0;
            int errLen = sizeof(int);
            if (getsockopt(hSocket, SOL_SOCKET, SO_ERROR, &ret, (socklen_t*)&errLen) < 0
                || ret != 0)
            {
                hr = E_FAIL;
            }
            else
            {
                hr = S_OK;
            }
        }
    }

    // è®¾ä¸ºé˜»å¡æ¨¡å¼
    flags = fcntl(hSocket, F_GETFL, 0);
    flags &= (~O_NONBLOCK);
    fcntl(hSocket, F_SETFL, flags);
    return hr;
}

INT CQFTP::ConnectSocket(SOCKET& hConnectSocket, const CHAR* szIP, INT iPort)
{
#ifdef WIN32
    SOCKADDR_IN addr;
    INT err;
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1,1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0)
    {
        WSACleanup();
        return ERROR_QFTP_PARAMETER_INVALID;
    }
    if( LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1 )
    {
        WSACleanup();
        return ERROR_QFTP_PARAMETER_INVALID;
    }
#endif

    hConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
    // ´´½¨socketÊ§°Ü
    if ( INVALID_SOCKET == hConnectSocket )
    {
        return ERROR_QFTP_SOCKET_INITIALIZE_FAILED;
    }

#ifdef WIN32
	if (0 == m_wTimeout)
	{
		int iRtn = connect(hConnectSocket, (sockaddr*)&addr, sizeof(addr));
		if (iRtn != 0)
		{
			return ERROR_QFTP_CONNECT_INVALID;
		}
	}
	else
	{
		struct sockaddr_in addr;
		addr.sin_addr.s_addr = inet_addr(szIP);
	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(iPort);

	    // ÉèÎª·Ç×èÈûÄ£Ê½
		u_long cmd_value = 1;
		ioctlsocket(hConnectSocket, FIONBIO, &cmd_value);

		int iRtn = connect(hConnectSocket, (sockaddr*)&addr, sizeof(addr));
		if (iRtn != 0)
		{
			struct timeval timeout ;
			fd_set r;
			FD_ZERO( &r );
			FD_SET(m_skt, &r);
			timeout.tv_sec = 0;
			timeout.tv_usec = m_wTimeout * 1000;
			int ret = select(0, NULL, &r, NULL, &timeout);
			if (ret == 0 || ret == SOCKET_ERROR)
			{
				return ERROR_QFTP_CONNECT_INVALID;
			}
		}
		// ÉèÎª×èÈûÄ£Ê½
		u_long cmd_value = 0;
		ioctlsocket(hConnectSocket, FIONBIO, &cmd_value );
	}
    return ERROR_QFTP_OK;
#elif CHIP_6467
    if (S_OK == HvConnect(hConnectSocket, szIP, iPort, m_wTimeout))
    {
    	HvSetRecvTimeOut(hConnectSocket, m_wTimeout/2);
    	HvSetSendTimeOut(hConnectSocket, m_wTimeout);
    	return ERROR_QFTP_OK;
    }
    return ERROR_QFTP_CONNECT_INVALID;
	
#else
	//Ò»°ãµÄlinuxÆ½Ì¨
	 if (S_OK == SvConnect(hConnectSocket, szIP, iPort, m_wTimeout * 1000))
    {
    	SvSetRecvTimeOut(hConnectSocket, m_wTimeout/2 * 1000);
    	SvSetSendTimeOut(hConnectSocket, m_wTimeout * 1000);
    	return ERROR_QFTP_OK;
    }
	return ERROR_QFTP_CONNECT_INVALID;
	
#endif
}

// ½ÓÊÕÊı¾İ£¬Ê¹ÓÃselect£¬³¬Ê±Ê±¼äÄ¬ÈÏÉèÎª10s
INT CQFTP::Receive(SOCKET& socket, CHAR* pszBuffer, INT nSize)
{
    int nBytesReceived = recv(socket, pszBuffer, nSize, 0);
    if (nBytesReceived == SOCKET_ERROR)
    {
        debug("Receive error.\n");
        return ERROR_QFTP_RECV_ERROR;
    }

    return nBytesReceived;
}

// ·¢ËÍÊı¾İ£¬Ê¹ÓÃselect£¬³¬Ê±Ê±¼äÄ¬ÈÏÉèÎª10s
INT CQFTP::Send(SOCKET& socket, const CHAR* pszBuffer, INT nSize)
{
    int nBytesSent=send(socket, pszBuffer, nSize, MSG_NOSIGNAL);
    if (nBytesSent != nSize)
    {
    	return ERROR_QFTP_SEND_ERROR;
    }
    return nBytesSent;
}

INT CQFTP::CloseSocket(SOCKET& hConnectSocket)
{
    if ( hConnectSocket != 0 )
    {
#ifdef WIN32
        closesocket(hConnectSocket);
#else
        close(hConnectSocket);
#endif
        hConnectSocket = 0;  // ÉèÎª0
        return ERROR_QFTP_OK;
    }
    else
    {
        return ERROR_QFTP_PARAMETER_INVALID;
    }
}

/**
 * LoginFtpServer - µÇÂ½·şÎñÆ÷
 *
 * ·µ»ØÖµ£º
 * ERROR_QFTP_OK£ºÕı³£
 * ERROR_QFTP_CONNECT_FAILED£ºÁ¬½ÓÊ§°Ü
 * ERROR_QFTP_SEND_ERROR£º·¢ËÍÃüÁîÊ§°Ü
 * ERROR_QFTP_USER_ERROR£ºÓÃ»§Ãû»òÃÜÂë´íÎó
 *
 */
INT CQFTP::LoginFtpServer(void)
{
    INT iRetCode = ERROR_QFTP_UNKNOWNERROR;
    string strUser("USER ");
    string strPass("PASS ");
    strUser.append(m_strUserName);
    strUser.append("\r\n");

    strPass.append(m_strPassWord);
    strPass.append("\r\n");
	debug("want to LoginFtpServer\n");
    // Á¬½Ó
    if (ERROR_QFTP_OK != ConnectSocket(m_socketCtrlConnect, m_strFtpIP.c_str(), 21))
    {
        iRetCode = ERROR_QFTP_CONNECT_FAILED;
        goto END;
    }
    // Á¬½Óºó£¬µÃµ½»¶Ó­ĞÅÏ¢
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);
    if ( 220 != m_iLastRecvAnswerCode )
    {
        goto END;
    }
    // ÓÃ»§Ãû
    iRetCode = Send(m_socketCtrlConnect, strUser.c_str(), (INT)strUser.size());
    if (iRetCode < 0)
    {
        goto END;
    }
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);
    if (331 != m_iLastRecvAnswerCode)
    {
        iRetCode = m_iLastRecvAnswerCode;
        goto END;
    }
    // ÃÜÂë ÕıÈ··µ»Ø230£¬´íÎó·µ»Ø530
    iRetCode = Send(m_socketCtrlConnect, strPass.c_str(), (INT)strPass.size());
    if ( iRetCode < 0 )
    {
        goto END;
    }
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);
    if (230 != m_iLastRecvAnswerCode)
    {
        iRetCode = ERROR_QFTP_USER_ERROR;
        goto END;
    }
    // Ò»ÇĞÕı³£
    iRetCode = ERROR_QFTP_OK;

END:
    if ( ERROR_QFTP_OK != iRetCode )
    {
        CloseSocket(m_socketCtrlConnect);
    }

    return iRetCode;
}

/**
 * LogoutFtpServer - µÇ³öÆ÷
 *
 * ·µ»ØÖµ£º
 * ERROR_QFTP_OK£ºÕı³£
 * ERROR_QFTP_SEND_ERROR£º·¢ËÍÃüÁîÊ§°Ü
 *
 */
INT CQFTP::LogoutFtpServer()
{
     if (0 == m_socketCtrlConnect)
    {
        return ERROR_QFTP_PARAMETER_INVALID;
    }

    if ( Send(m_socketCtrlConnect, "QUIT\r\n", (INT)strlen("QUIT\r\n")) < 0 )
    {
        return ERROR_QFTP_SEND_ERROR;
    }
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);

    if (m_iLastRecvAnswerCode != 221 )
    {
        CloseSocket(m_socketCtrlConnect);  // ¼´Ê¹µÇ³öÊ§°ÜÒ²¹Ø±Õsokcet
        return ERROR_QFTP_LOGOUT_FAILED;
    }

    CloseSocket(m_socketCtrlConnect);

    return ERROR_QFTP_OK;
}

INT CQFTP::RecvAnswer(SOCKET& hConnectSocket, CHAR* pszRecvBuf, INT iLen)
{
    CHAR szRetCode[4];
    INT iRecvNum=0;
    INT iRecvBufferLen = 256;

    memset(m_szRecvBuf, 0, iRecvBufferLen);

    iRecvNum = Receive(hConnectSocket, m_szRecvBuf, iRecvBufferLen);
    if ( iRecvNum < 0 )
    {
        return iRecvNum;
    }
    else
    {
        memset(m_szRecvBuf + iRecvNum, 0, 1);

        if ( pszRecvBuf != NULL && iLen > iRecvNum )
        {
            strcpy(pszRecvBuf, m_szRecvBuf);
        }

        szRetCode[0] = m_szRecvBuf[0];
        szRetCode[1] = m_szRecvBuf[1];
        szRetCode[2] = m_szRecvBuf[2];
        szRetCode[3] = '\0';

#ifdef PRINT_MESSAGE
        //´òÓ¡³öFTP·şÎñÆ÷·µ»ØµÄ¾ßÌåÓ¦´ğÄÚÈİ
        debug("%s", m_szRecvBuf);
#endif
        return atoi(szRetCode);
    }
}

INT CQFTP::TestConnectValidity(void)
{

    if ( 0 == m_socketCtrlConnect )
    {
        return ERROR_QFTP_PARAMETER_INVALID;
    }

    int iRet = SendCommand(m_socketCtrlConnect, "NOOP \r\n");
    debug("socket: %d\n", m_socketCtrlConnect);
    if (200 == iRet)
    {
        return ERROR_QFTP_OK;
    }
    else
    {
        return ERROR_QFTP_CONNECT_INVALID;
    }
}

// ·¢ËÍÃüÁî
// ÊÊºÏRMD¡¢DELE¡¢CWD¡ª¡ª550±íÊ¾ÎŞ´ËÄ¿Â¼»òÎÄ¼ş£¬»ò¶ÔÏó²»ÕıÈ·(ÈçDELE dir)
// NLST¡¢LIST¡ª¡ª450±íÊ¾ÎŞ´ËÄ¿Â¼»òÎÄ¼ş
// MKD¡ª¡ª550±íÊ¾Ä¿Â¼ÒÑ¾­´æÔÚ
// ³ö´í·µ»ØERROR_QFTP_SEND_ERROR£¬³É¹¦·µ»ØÏìÓ¦Âë
INT CQFTP::SendCommand(SOCKET& hConnectSocket, const CHAR* pbCommand)
{
    INT iRet;
    iRet = Send(hConnectSocket, pbCommand, (INT)strlen(pbCommand));
    if (iRet < 0)
    {
        return iRet;
    }

    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);
    return m_iLastRecvAnswerCode;   // Ö±½Ó·µ»ØÏìÓ¦Âë
}

// ´´½¨Ä¿Â¼
INT CQFTP::MakeDirectory(const CHAR* DirectoryPath)
{
    CHAR szCommand[256] = {0};

    sprintf(szCommand, "XMKD %s\r\n", DirectoryPath);
    return SendCommand(m_socketCtrlConnect, szCommand);
}

// ¸Ä±äÄ¿Â¼
INT CQFTP::ChangeDirectory(const CHAR* DirectoryPath)
{
    CHAR szCommand[256]= {0};

    sprintf(szCommand, "XCWD %s\r\n", DirectoryPath);

    return SendCommand(m_socketCtrlConnect, szCommand);
}

// ·¢ËÍÊı¾İ
INT CQFTP::SendData(SOCKET& hConnectSocket, CHAR* pbDataBuffer, INT nSize)
{
#define SENDSIZE 2*1024*1024
    INT nBytesSent = 0;
    INT nBytesThisTime;
    INT nBytesOneTime = SENDSIZE;   // Ã¿´Î×î¶à·¢SENDSIZE
    CHAR* pTmp = pbDataBuffer;

    do{
        if (nSize < SENDSIZE)
        {
            nBytesOneTime = nSize - nBytesSent;
        }
        nBytesThisTime = Send(hConnectSocket, pTmp, nBytesOneTime); // Ò»´Î·¢nBytesOneTime
        // Èç¹ûselect³ö´í¡¢·¢ËÍ³ö´í¡¢·¢ËÍ³¬Ê±£¬¾ù·µ»Ø·¢ËÍ³ö´í
        if (ERROR_QFTP_SELECT_ERROR == nBytesThisTime || ERROR_QFTP_SEND_ERROR == nBytesThisTime || \
            ERROR_QFTP_SEND_TIMEOUT == nBytesThisTime)
        {
            return ERROR_QFTP_SEND_ERROR;
        }

        nBytesSent += nBytesThisTime;
        pTmp += nBytesThisTime;

        INT nLeft = nSize - nBytesSent;
        if (nLeft < nBytesOneTime)
        {
            nBytesOneTime = nLeft;
        }
    } while (nBytesSent < nSize);

    return ERROR_QFTP_OK;
}

//´ÓPASVÃüÁî·µ»ØÓ¦´ğĞÅÏ¢ÖĞÌáÈ¡·şÎñÆ÷µÄ¶Ë¿Ú
//Èç£º227 Entering Passive Mode (192,168,1,110,12,34).
INT CQFTP::GetPortForPASV(CHAR* szRecvBuf)
{
    INT nPort = 0;
    INT iPos1 = 0;
    INT iPos2 = 0;
    INT iPos3 = 0;
    INT iBufLen = 0;
    CHAR buf1[8] = {0};
    CHAR buf2[8] = {0};

    iBufLen = (INT)strlen(szRecvBuf);

    if ( '2'==szRecvBuf[0] && '2' == szRecvBuf[1] && '7'==szRecvBuf[2] )
    {
        for ( INT n=(iBufLen-1); n>=0; --n )
        {
            if ( ',' == szRecvBuf[n] )
            {
                if ( 0 == iPos2 )
                {
                    iPos2 = n;
                }
                else
                {
                    iPos1 = n;
                    break;
                }
            }
            else if ( ')' == szRecvBuf[n] )
            {
                iPos3 = n;
            }
        }

        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        memcpy(buf1, szRecvBuf+iPos1+1, iPos2-iPos1-1);
        memcpy(buf2, szRecvBuf+iPos2+1, iPos3-iPos2-1);

        nPort = atoi(buf1)*256 + atoi(buf2);

        return nPort;
    }

    return -1;
}

/**************************************************************************
* OpenPassiveDataConnection - ´ò¿ªÊı¾İÍ¨µÀÃüÁî£¬ÈçNLST/LIST/RETR¡¢STOR/APPE
* hConnectSocket£ºÃüÁîÍ¨µÀsocket£¬´«Èë²ÎÊı
* hDataSocket£º´´½¨µÄÊı¾İÍ¨µÀsocket£¬´«³ö²ÎÊı
* pbCommand£ºÓëÊı¾İÍ¨µÀÓĞ¹ØÃüÁî£¬ÈçNLST/LIST/RETR¡¢STOR/APPE
*
* ¸Ãº¯ÊıÖ»¸ºÔğ´ò¿ªÊı¾İÁ¬½ÓÍ¨µÀ£¬Èç³ö´í£¬·µ»ØÇ°¹Ø±Õ¸ÃÊı¾İÁ¬½Ó
*************************************************************************/
INT CQFTP::OpenPassiveDataConnection(SOCKET& hConnectSocket, SOCKET& hDataSocket, CHAR* pbCommand)
{
    WORD16 usPort;
    INT iRetCode = 0;
    const INT nPassiveLen = 128;
    CHAR szRecvPassiveBuffer[nPassiveLen] = {0};     // PASVÃüÁî·µ»ØĞÅÏ¢£¬°üº¬Êı¾İÍ¨µÀ¶Ë¿ÚºÅ

    iRetCode = Send(hConnectSocket, "PASV \r\n", (INT)strlen("PASV \r\n"));
    if (iRetCode < 0)
    {
        goto END;
    }
    // 227 Entering Passive Mode (172,18,222,100,6,170).
    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, szRecvPassiveBuffer, nPassiveLen);
    if (227 != m_iLastRecvAnswerCode)
    {
        iRetCode = ERROR_QFTP_CONNECT_FAILED;
        goto END;
    }
    // µÃµ½Êı¾İÍ¨µÀ¶Ë¿ÚºÅ
    usPort = GetPortForPASV(szRecvPassiveBuffer);

    // Á¬½ÓÊı¾İÍ¨µÀ
    if (ERROR_QFTP_OK != ConnectSocket(hDataSocket, m_strFtpIP.c_str(), usPort))
    {
        iRetCode = ERROR_QFTP_CONNECT_FAILED;
        goto END;
    }

    // ·¢ËÍÃüÁî
    iRetCode = Send(hConnectSocket, pbCommand, (INT)strlen(pbCommand));
    if (iRetCode < 0)
    {
        goto END;
    }
    // ÈçÄ¿Â¼ÎŞÎÄ¼ş
    // ÔÚRecvAnswerÖĞÊä³ö 226 Transfer complete£¬·µ»Ø226£¬¼´¹Ø±ÕÊı¾İÁ¬½Ó
    // µ«µ÷ÊÔ±¾º¯ÊıÊ±(·ÇRecvAnswer) g_szRecvBufÄÚÈİÎªÇ°Ò»´ÎÊı¾İ
    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);

    if (150 == m_iLastRecvAnswerCode || 125 == m_iLastRecvAnswerCode)  // 125 150 ³É¹¦Ö´ĞĞÃüÁî
    {
        iRetCode = ERROR_QFTP_OK;
        goto END;
    }
    else if (550 == m_iLastRecvAnswerCode || 450 == m_iLastRecvAnswerCode)   // Ä¿Â¼»òÎÄ¼ş²»´æÔÚ
    {
        iRetCode = ERROR_QFTP_FILEORDIR_NOT_FOUND;
        goto END;
    }
    else if (226 == m_iLastRecvAnswerCode)       // Õë¶ÔÄ¿Â¼ÎŞÊı¾İÇé¿ö(²»ÊÇ³ö´í£¬¶øÊÇÕı³£´«Êä½áÊø)
    {
        iRetCode = ERROR_QFTP_DATA_CONNECT_CLOSED;
        goto END;
    }

END:
    if (iRetCode != ERROR_QFTP_OK)
    {
        CloseSocket(hDataSocket);
    }
    return iRetCode;
}

////////////////////////////////// ÉÏ´«Ïà¹Ø
/**
 * SaveBufferToFtp - ±£´æ»º³åÇøÊı¾İµ½·şÎñÆ÷
 *
 * hConnectSocket£ºÃüÁîÍ¨µÀsocket
 * pbFileName£ºÎÄ¼şÃû³Æ
 * pbBuffer£º»º³åÇø
 * nSize£º´óĞ¡
 * fAppend£ºÊÇ·ñappend(¾­²âÊÔ£¬·şÎñÆ÷²»Ö§³ÖAPPEÃüÁî)
 *
 * ·µ»ØÖµ£º
 * ERROR_QFTP_OK£ºÕı³£
 * ERROR_QFTP_SEND_ERROR£º·¢ËÍÃüÁî/Êı¾İÊ§°Ü
 */
INT CQFTP::SaveBufferToFtp(SOCKET& hConnectSocket, const CHAR* pbFileName, CHAR* pbBuffer, INT nSize, BOOL fAppend)
{
    CHAR szCommand[256] = {0};
    SOCKET hDataSocket = 0;
    INT iRet;

    if (fAppend)
    {
        sprintf(szCommand, "APPE %s\r\n", pbFileName);   // Î´²âÊÔ
    }
    else
    {
        sprintf(szCommand, "STOR %s\r\n", pbFileName);
    }

    iRet = Send(hConnectSocket, "TYPE I\r\n", (INT)strlen("TYPE I\r\n"));
    if ( iRet < 0)
    {
        return ERROR_QFTP_SEND_ERROR;
    }
    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);
    if (200 != m_iLastRecvAnswerCode)
    {
        return ERROR_QFTP_SEND_ERROR;
    }

    // ´´½¨Êı¾İÍ¨µÀ
    iRet = OpenPassiveDataConnection(hConnectSocket, hDataSocket, szCommand);
    if (iRet < 0)
    {
        return ERROR_QFTP_SEND_ERROR;
    }

    iRet = SendData(hDataSocket, pbBuffer, nSize);
    if (iRet < 0)
    {
        return ERROR_QFTP_SEND_ERROR;
    }

    // ¹Ø±ÕÊı¾İÍ¨µÀ
    CloseSocket(hDataSocket);

    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);

    return ERROR_QFTP_OK;
}

/**
 * SaveDataToFtp - ±£´æÊı¾İµ½·şÎñÆ÷
 *
 * strFileFullName£º´øÍêÕûÂ·¾¶µÄÎÄ¼şÃû³Æ
 * pbDataForSend£º»º³åÇø
 * uDataLen£º»º³åÇø´óĞ¡
 * fAppend£ºÊÇ·ñÒÔ¸½¼ÓĞÎÊ½±£´æ(¾­²âÊÔ£¬·şÎñÆ÷²»Ö§³Ö)
 */
INT CQFTP::SaveDataToFtp(string strFileFullName, CHAR* pbDataForSend, UINT uDataLen, BOOL fAppend)
{
    INT iRet;
    if ( 0 == m_socketCtrlConnect || strFileFullName.empty() || NULL == pbDataForSend)
    {
        return ERROR_QFTP_PARAMETER_INVALID;
    }
    INT iDestPathLen = (INT)strFileFullName.size();
    if ( iDestPathLen >= (MAX_CMD_LEN-16) )
    {
        return ERROR_QFTP_PARAMETER_INVALID;
    }

    size_t iSlash;
    string strDirectory;
    iSlash = strFileFullName.find_last_of('/');
    if (iSlash != string::npos)
        strDirectory = strFileFullName.substr(0, iSlash+1); // ÍêÕûµÄ_Ä¿Â¼Ãû_(Èç/xx/2011/08/15/14/)

    if (strDirectory.empty())   // ²»´øÄ¿Â¼£¬Ö±½ÓÉÏ´«
    {
        iRet = SaveBufferToFtp(m_socketCtrlConnect, strFileFullName.c_str(), pbDataForSend, uDataLen, fAppend);
        if (iRet < 0)
        {
            return iRet;
        }
        return ERROR_QFTP_OK;
    }
    string strTemp;
    iRet = ChangeDirectory(strDirectory.c_str());  // ³¢ÊÔ½øÈë¸ÃÄ¿Â¼(Èç¹û´æÔÚ£¬¾Í²»ÓÃÔÙ´´½¨)
    if (550 == iRet) // Ä¿Â¼²»´æÔÚ£¬´´½¨ºóÔÙÉÏ´«
    {
        for (size_t i = 1; i < strDirectory.size(); i++)
        {
            if ('/' == strDirectory.at(i))  // ÕÒµ½Ä¿Â¼
            {
                strTemp = strDirectory.substr(0, i);
                MakeDirectory(strTemp.c_str());     // Èç¹û´æÔÚ£¬¼ÌĞø£¬²»ÓÃ´¦Àí
            }
        }
        // ´´½¨Íê±Ï£¬±£´æ
        iRet = SaveBufferToFtp(m_socketCtrlConnect, strFileFullName.c_str(), pbDataForSend, uDataLen, fAppend);
        if (iRet < 0)
        {
            return iRet;
        }
        return ERROR_QFTP_OK;
    }
    // ´æÔÚ£¬Ö±½ÓÉÏ´«(´øÍêÕûÂ·¾¶)
    else
    {
        iRet = SaveBufferToFtp(m_socketCtrlConnect, strFileFullName.c_str(), pbDataForSend, uDataLen, fAppend);
        if (iRet < 0)
        {
            return iRet;
        }
        return ERROR_QFTP_OK;
    }
    return ERROR_QFTP_OK;
}

// Ğ´»º³åÇøÄÚÈİµ½FTPÖĞ
// szFileName: ´øÍêÕûÂ·¾¶µÄÎÄ¼şÃû
// szBuffer£º»º³åÇø
// dwLength£º»º³åÇø´óĞ¡
// ³É¹¦·µ»Ø0£¬Ê§°Ü·µ»ØERROR_QFTP_SEND_ERROR
INT CQFTP::WriteFileToFtp(CHAR* szFileName, CHAR* szBuffer, DWORD32 dwLength)
{
    INT iRet;
    iRet = SaveDataToFtp(szFileName, szBuffer, dwLength, 0);
    if (iRet != ERROR_QFTP_OK)
    {
        return ERROR_QFTP_SEND_ERROR;
    }

    return ERROR_QFTP_OK;
}

/////////////////////////////////////////////////////////////////////
// ´Ó·şÎñÆ÷ÏÂÔØÎÄ¼şµ½ÄÚ´æÖĞ
// pbRemoteFileÎª´øÂ·¾¶µÄÎÄ¼ş
// ³ö´í·µ»ØERROR_QFTP_RECV_ERROR
// Ã»ÓĞÕÒµ½ÎÄ¼ş·µ»ØERROR_QFTP_NOT_FOUND
// ³É¹¦·µ»ØÊµ¼ÊµÄ³¤¶È(»º³åÇø¼°ÎÄ¼ş³¤È¡×îĞ¡µÄÄÇ¸ö)
INT CQFTP::GetFileFromFtp(const CHAR* pbRemoteFile, CHAR* szBuffer, DWORD32 dwBufLen)
{
    SOCKET hDataSocket;
    INT iRet = 0;
    CHAR szCommand[256] = {0};
    CHAR szDataBuffer[8*1024] = {0};    // 8KB »º³åÇø

    iRet = Send(m_socketCtrlConnect, "TYPE I\r\n", (INT)strlen("TYPE I\r\n"));
    if (iRet < 0)
    {
        return ERROR_QFTP_RECV_ERROR;
    }

    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);
    if (200 != m_iLastRecvAnswerCode)
    {
        return ERROR_QFTP_RECV_ERROR;
    }

    sprintf(szCommand, "RETR %s\r\n", pbRemoteFile);

    // ´ò¿ªÊı¾İÍ¨µÀ
    iRet = OpenPassiveDataConnection(m_socketCtrlConnect, hDataSocket, szCommand);
    if (ERROR_QFTP_FILEORDIR_NOT_FOUND == iRet) // Ã»ÓĞÕÒµ½ÎÄ¼ş
    {
        return ERROR_QFTP_NOT_FOUND;
    }

    DWORD32 dwFileSize = 0;
    // we get the message like  "150 Opening BINARY mode data connection for 1234.dat (29068 bytes)"
    // we need the file size: 29068
    sscanf(m_szRecvBuf, "%*[^(](%d[^ ]", &dwFileSize);  // »ñµÃÎÄ¼ş´óĞ¡

    INT nNum = 0;
    CHAR* pTmp = szBuffer;
    INT nTmpSize = dwFileSize > dwBufLen ? dwBufLen : dwFileSize;
    INT nTrueSize = nTmpSize;

    if (dwFileSize <= 0)//ÓĞÊ±FTP·şÎñÆ÷²¢²»Ò»¶¨·µ»Ø150,¶øÊÇ125(Ö»ÓĞ³É¹¦ÌáÊ¾£¬Ã»ÓĞÎÄ¼ş´óĞ¡)
    {
    	nTrueSize = 0;
    	nNum = Receive(hDataSocket, szDataBuffer, sizeof(szDataBuffer));
    	while (nNum > 0)
    	{
    		memcpy(szBuffer + nTrueSize, szDataBuffer, nNum);
    		nTrueSize += nNum;
    		nNum = Receive(hDataSocket, szDataBuffer, sizeof(szDataBuffer));
    	}
    }
    else
    {
	    // È¡Ğ¡Õß
	    while (nTmpSize > 0)
	    {
	        nNum = Receive(hDataSocket, szDataBuffer, sizeof(szDataBuffer));
	        // select³ö´í»ò½ÓÊÕ³ö´í£¬»ò³¬Ê±(Èç¶ÏÍø)£¬Ö±½Ó·µ»Ø£¬²»ÖØÊÔ
	        if (ERROR_QFTP_SELECT_ERROR == nNum || ERROR_QFTP_RECV_ERROR == nNum || ERROR_QFTP_RECV_TIMEOUT == nNum)
	        {
	            return ERROR_QFTP_RECV_ERROR;
	        }
	        if (nNum > nTmpSize)
	        {
	            nNum = nTmpSize;
	        }
	        memcpy(pTmp, szDataBuffer, nNum);
	        pTmp = pTmp + nNum;
	        nTmpSize -= nNum;
	    }
	}

    // ¹Ø±ÕÊı¾İÁ¬½Ó
    CloseSocket(hDataSocket);
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);

    return nTrueSize;
}

// ¶ÁFTPÖĞÖ¸¶¨ÎÄ¼şµ½»º³åÇøÖĞ
// szFileName£ºFTPÎÄ¼şÃû
// szBuffer£º»º³åÇø
// szBufLen£º»º³åÇø´óĞ¡
// [OUT]dwDataLen£ºÊµ¼Ê¶ÁÈ¡µÄ´óĞ¡
// ¶ÁÈ¡ÎÄ¼şÊ§°Ü»òÖ¸¶¨»º³åÇøĞ¡ÓÚÊµ¼ÊÎÄ¼ş³¤¶È£¬ÉèÖÃdwDataLenÎª»º³åÇøÖ¸¶¨µÄ´óĞ¡£¬ÓÉµ÷ÓÃÕßÅĞ¶ÏÊÇ·ñ³ö´í
// Ö¸¶¨»º³åÇø´óÓÚÎÄ¼şÊµ¼Ê³¤¶È£¬·µ»Ø0£¬²¢ÉèÖÃdwDataLenÎªÊµ¼ÊÎÄ¼ş³¤¶È
// ³ıÁËÍøÂçÒì³£·µ»ØERROR_QFTP_RECV_ERROR£¬ÆäËü¶¼·µ»Ø0
INT CQFTP::ReadFileFromFtp(CHAR* szFileName, CHAR* szBuffer, DWORD32 dwBufLen, DWORD32& dwDataLen)
{
    INT iRet = 0;
    iRet = GetFileFromFtp(szFileName, szBuffer, dwBufLen);
    if (ERROR_QFTP_RECV_ERROR == iRet)
    {
        return ERROR_QFTP_RECV_ERROR;
    }
    else if (ERROR_QFTP_NOT_FOUND == iRet)  // Ã»ÓĞÕÒµ½ÎÄ¼ş£¬·µ»Ø0£¬ÉèÖÃdwDataLenÎª0
    {
        dwDataLen = 0;
        return ERROR_QFTP_OK;
    }
    // ÕÒµ½£¬ÉèÖÃÕæÊµ¶Áµ½µÄ³¤¶È
    dwDataLen = iRet;

    return ERROR_QFTP_OK;
}

// É¾³ıµ¥¸öÎÄ¼ş
// szFileName: FTP·şÎñÆ÷ÉÏÍêÕûµÄÂ·¾¶¼°ÎÄ¼şÃû(Èç¡°/array1/guest-share/user1/foo.dat¡±)
// ·µ»ØÖµ£º
// ERROR_QFTP_DEL_ERROR£ºÎÄ¼ş²»´æÔÚ»òÉ¾³ı¶ÔÏó²»ÕıÈ·
// ERROR_QFTP_OK£º³É¹¦É¾³ıÎÄ¼ş
INT CQFTP::DeleteOneFile(CHAR* szFileName)
{
    INT iRet = 0;
    CHAR szCommand[256] = {0};

    sprintf(szCommand, "DELE %s\r\n", szFileName);

    iRet = SendCommand(m_socketCtrlConnect, szCommand);
    if (iRet == 550)
    {
        return ERROR_QFTP_DEL_ERROR;
    }

    return ERROR_QFTP_OK;
}

INT CQFTP::List(const CHAR* pbPath)
{
    SOCKET hDataSocket = 0;
    CHAR szCommand[256] = {0};
    CHAR szDataBuffer[16] = {0};
    INT iRet;
    INT nNum;

    sprintf(szCommand, "NLST %s\r\n", pbPath);
    iRet = OpenPassiveDataConnection(m_socketCtrlConnect, hDataSocket, szCommand);
    if (ERROR_QFTP_FILEORDIR_NOT_FOUND == iRet)
    {
        debug("List() %s: File or directory not found.\n",  pbPath);
        return ERROR_QFTP_NOT_FOUND;
    }
    else if (ERROR_QFTP_DATA_CONNECT_CLOSED == iRet)  // Ä¿Â¼ÎŞÎÄ¼ş£¬Ö±½Ó·µ»ØERROR_QFTP_NO_FILE_IN_DIR
    {
        return ERROR_QFTP_NO_FILE_IN_DIR;
    }

    //TODO ´¦Àí½ÓÊÕµ½µÄÊı¾İ
    nNum = Receive(hDataSocket, szDataBuffer, sizeof(szDataBuffer));
    // select³ö´í»ò½ÓÊÕ³ö´í£¬»ò³¬Ê±(Èç¶ÏÍø)£¬Ö±½Ó·µ»Ø£¬²»ÖØÊÔ
    if (nNum < 0)
    {
        return ERROR_QFTP_RECV_ERROR;
    }
    szDataBuffer[nNum] = '\0';
    debug("receive bufer: %s, num: %d\n", szDataBuffer, nNum);
    CloseSocket(hDataSocket);      // ¹Ø±ÕÊı¾İÁ¬½Ó
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);

    return ERROR_QFTP_OK;
}

INT CQFTP::KeepAlive()
{
    INT iRet = 0;

    iRet = List("/");
    if (ERROR_QFTP_OK != iRet)
        return ERROR_QFTP_CONNECT_INVALID;
    return ERROR_QFTP_OK;
}

// »ñÈ¡Ä¿Â¼»òÎÄ¼şÃû³Æ£¬±£´æµ½¶ÓÁĞÖĞ
INT CQFTP::GetFileName(SOCKET& s, deque<string>& qFileName)
{
    INT nBytesThisTime = 0;
    INT nSize = 1024;
    CHAR szDataBuffer[1024] = {0};
    PCHAR pbTemp = szDataBuffer;
    string strFile;

    nBytesThisTime = Receive(s, pbTemp, nSize); // first time

    if (ERROR_QFTP_RECV_ERROR == nBytesThisTime)
    {
        return ERROR_QFTP_RECV_ERROR;
    }
    strFile.append(pbTemp, nBytesThisTime); // ½«½ÓÊÕµ½µÄÊı¾İÌí¼Óµ½strFileÖĞ£¬×¢ÒâĞèÒªÖ¸¶¨×Ö½ÚÊı
    // ÏÈ½âÎöÒ»²¿·Ö
    while (strFile.length())
    {
        size_t iCRLF = strFile.find('\n');
        if (iCRLF != string::npos)  // ÕÒµ½
        {
            qFileName.push_back(strFile.substr(0, iCRLF-1));
            strFile.erase(0, iCRLF+1);            // É¾³ıxxx\r\n
        }
        // Èç¹û»¹ÓĞÎ´ÒÔ\r\n½áÊøµÄÎÄ¼şÃû£¬ÔÚÏÂÃæÔÙ½âÎö£¬´Ë´¦²»×÷´¦Àí
        else
        {
            break;
        }
    }
    while (nBytesThisTime != 0)
    {
        nBytesThisTime = Receive(s, pbTemp, nSize);

        if (ERROR_QFTP_RECV_ERROR == nBytesThisTime)
        {
            return ERROR_QFTP_RECV_ERROR;
        }

        strFile.append(pbTemp, nBytesThisTime); // append
        while (strFile.length())
        {
            size_t iCRLF = strFile.find('\n');
            if (iCRLF != string::npos)  // we got it
            {
                qFileName.push_back(strFile.substr(0, iCRLF-1));
                strFile.erase(0, iCRLF+1);            // É¾³ıxxx\r\n
            }
            else
            {
                break;
            }
        }
    }   // end of while

    return ERROR_QFTP_OK;
}

// ÁĞ³öÖ¸¶¨Ä¿Â¼ÏÂËùÓĞÄÚÈİ(Ä¿Â¼»òÎÄ¼şÃû³Æ)£¬±£´æµ½¶ÓÁĞÖĞ
INT CQFTP::ListToDeque(const CHAR*  pbPath, deque<string>& qFileName)
{
    SOCKET hDataSocket = 0;
    char szCommand[256] = {0};
    int ret;

    sprintf(szCommand, "NLST %s\r\n", pbPath);
    ret = OpenPassiveDataConnection(m_socketCtrlConnect, hDataSocket, szCommand);
    if (ERROR_QFTP_FILEORDIR_NOT_FOUND == ret)
    {
        debug("List() %s: File or directory not found.\n",  pbPath);
        return ERROR_QFTP_NOT_FOUND;
    }
    else if (ERROR_QFTP_DATA_CONNECT_CLOSED == ret)  // Ä¿Â¼ÎŞÎÄ¼ş£¬Ö±½Ó·µ»ØERROR_QFTP_NO_FILE_IN_DIR
    {
        return ERROR_QFTP_NO_FILE_IN_DIR;
    }
    else if (ERROR_QFTP_OK != ret)
    {
    	return ERROR_QFTP_CONNECT_FAILED;
    }

    GetFileName(hDataSocket, qFileName);    // ½«ÎÄ¼şÁĞ±í±£´æµ½qFileNameÖĞ
    CloseSocket(hDataSocket);               // ¹Ø±ÕÊı¾İÁ¬½Ó
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);

    return ERROR_QFTP_OK;
}

// É¾³ıÒ»¸öÄ¿Â¼£¬¸ÃÄ¿Â¼ÏÂ²»ÄÜ´æÔÚÄ¿Â¼
INT CQFTP::DeleteOneDirectory(const CHAR* pbDirectory)
{
    CHAR szCommand[256] = {0};
    deque<string> qFileName;
    deque<string> sDirectory;

    sprintf(szCommand, "XCWD %s\r\n", pbDirectory);

    int ret = SendCommand(m_socketCtrlConnect, szCommand);
    if (550 == ret) // Ä¿Â¼²»´æÔÚ
    {
        debug("%s: No such directory.\n", pbDirectory);
        return ERROR_QFTP_NOT_FOUND;
    }

    ret = ListToDeque("", qFileName);       // ÁĞ³ö¸ÃÄ¿Â¼ÏÂµÄ _ÎÄ¼ş_·Åµ½qFileName¶ÓÁĞÖĞ
    if (ERROR_QFTP_NOT_FOUND == ret
        || ERROR_QFTP_NO_FILE_IN_DIR == ret
       )   // Èç¹ûÃ»ÓĞÄÚÈİ£¬¼´Îª¿ÕÄ¿Â¼£¬²»É¾³ı¸ÃÄ¿Â¼
    {
        return ERROR_QFTP_OK;
    }

    while (qFileName.size() > 0)
    {
        ret = DeleteOneFile((CHAR*)qFileName.front().c_str());
        qFileName.pop_front();              // µ¯³ö£¬¼ÌĞøÈ¡ÏÂÒ»¸ö
    }

    return ERROR_QFTP_OK;
}
