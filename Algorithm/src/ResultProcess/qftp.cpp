// 该文件编码必须是WINDOWS-936格式
#include <ctime>
#include <iostream>
#include <fstream>
#include "qftp.h"

#ifdef CHIP_6467
#include "HvSockUtils.h"
#endif

using namespace std;

using namespace nsFTP;

CQFTP::CQFTP(const string& strFtpIp, const string& strUserName, const string& strPassWord, UINT nTimeout)
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
        CloseSocket(m_socketCtrlConnect);      // 直接关闭连接
        m_socketCtrlConnect = 0;
    }
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
    // 创建socket失败
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

	    // 设为非阻塞模式
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
		// 设为阻塞模式
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
#elif
	//一般的linux平台
	return ERROR_QFTP_CONNECT_INVALID;
#endif
}

// 接收数据，使用select，超时时间默认设为10s
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

// 发送数据，使用select，超时时间默认设为10s
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
        hConnectSocket = 0;  // 设为0
        return ERROR_QFTP_OK;
    }
    else
    {
        return ERROR_QFTP_PARAMETER_INVALID;
    }
}

/**
 * LoginFtpServer - 登陆服务器
 *
 * 返回值：
 * ERROR_QFTP_OK：正常
 * ERROR_QFTP_CONNECT_FAILED：连接失败
 * ERROR_QFTP_SEND_ERROR：发送命令失败
 * ERROR_QFTP_USER_ERROR：用户名或密码错误
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
    // 连接
    if (ERROR_QFTP_OK != ConnectSocket(m_socketCtrlConnect, m_strFtpIP.c_str(), 21))
    {
        iRetCode = ERROR_QFTP_CONNECT_FAILED;
        goto END;
    }
    // 连接后，得到欢迎信息
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);
    if ( 220 != m_iLastRecvAnswerCode )
    {
        goto END;
    }
    // 用户名
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
    // 密码 正确返回230，错误返回530
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
    // 一切正常
    iRetCode = ERROR_QFTP_OK;

END:
    if ( ERROR_QFTP_OK != iRetCode )
    {
        CloseSocket(m_socketCtrlConnect);
    }

    return iRetCode;
}

/**
 * LogoutFtpServer - 登出器
 *
 * 返回值：
 * ERROR_QFTP_OK：正常
 * ERROR_QFTP_SEND_ERROR：发送命令失败
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
        CloseSocket(m_socketCtrlConnect);  // 即使登出失败也关闭sokcet
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
        //打印出FTP服务器返回的具体应答内容
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

// 发送命令
// 适合RMD、DELE、CWD——550表示无此目录或文件，或对象不正确(如DELE dir)
// NLST、LIST——450表示无此目录或文件
// MKD——550表示目录已经存在
// 出错返回ERROR_QFTP_SEND_ERROR，成功返回响应码
INT CQFTP::SendCommand(SOCKET& hConnectSocket, const CHAR* pbCommand)
{
    INT iRet;
    iRet = Send(hConnectSocket, pbCommand, (INT)strlen(pbCommand));
    if (iRet < 0)
    {
        return iRet;
    }

    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);
    return m_iLastRecvAnswerCode;   // 直接返回响应码
}

// 创建目录
INT CQFTP::MakeDirectory(const CHAR* DirectoryPath)
{
    CHAR szCommand[256] = {0};

    sprintf(szCommand, "XMKD %s\r\n", DirectoryPath);
    return SendCommand(m_socketCtrlConnect, szCommand);
}

// 改变目录
INT CQFTP::ChangeDirectory(const CHAR* DirectoryPath)
{
    CHAR szCommand[256]= {0};

    sprintf(szCommand, "XCWD %s\r\n", DirectoryPath);

    return SendCommand(m_socketCtrlConnect, szCommand);
}

// 发送数据
INT CQFTP::SendData(SOCKET& hConnectSocket, CHAR* pbDataBuffer, INT nSize)
{
#define SENDSIZE 2*1024*1024
    INT nBytesSent = 0;
    INT nBytesThisTime;
    INT nBytesOneTime = SENDSIZE;   // 每次最多发SENDSIZE
    CHAR* pTmp = pbDataBuffer;

    do{
        if (nSize < SENDSIZE)
        {
            nBytesOneTime = nSize - nBytesSent;
        }
        nBytesThisTime = Send(hConnectSocket, pTmp, nBytesOneTime); // 一次发nBytesOneTime
        // 如果select出错、发送出错、发送超时，均返回发送出错
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

//从PASV命令返回应答信息中提取服务器的端口
//如：227 Entering Passive Mode (192,168,1,110,12,34).
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
* OpenPassiveDataConnection - 打开数据通道命令，如NLST/LIST/RETR、STOR/APPE
* hConnectSocket：命令通道socket，传入参数
* hDataSocket：创建的数据通道socket，传出参数
* pbCommand：与数据通道有关命令，如NLST/LIST/RETR、STOR/APPE
*
* 该函数只负责打开数据连接通道，如出错，返回前关闭该数据连接
*************************************************************************/
INT CQFTP::OpenPassiveDataConnection(SOCKET& hConnectSocket, SOCKET& hDataSocket, CHAR* pbCommand)
{
    WORD16 usPort;
    INT iRetCode = 0;
    const INT nPassiveLen = 128;
    CHAR szRecvPassiveBuffer[nPassiveLen] = {0};     // PASV命令返回信息，包含数据通道端口号

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
    // 得到数据通道端口号
    usPort = GetPortForPASV(szRecvPassiveBuffer);

    // 连接数据通道
    if (ERROR_QFTP_OK != ConnectSocket(hDataSocket, m_strFtpIP.c_str(), usPort))
    {
        iRetCode = ERROR_QFTP_CONNECT_FAILED;
        goto END;
    }

    // 发送命令
    iRetCode = Send(hConnectSocket, pbCommand, (INT)strlen(pbCommand));
    if (iRetCode < 0)
    {
        goto END;
    }
    // 如目录无文件
    // 在RecvAnswer中输出 226 Transfer complete，返回226，即关闭数据连接
    // 但调试本函数时(非RecvAnswer) g_szRecvBuf内容为前一次数据
    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);

    if (150 == m_iLastRecvAnswerCode || 125 == m_iLastRecvAnswerCode)  // 125 150 成功执行命令
    {
        iRetCode = ERROR_QFTP_OK;
        goto END;
    }
    else if (550 == m_iLastRecvAnswerCode || 450 == m_iLastRecvAnswerCode)   // 目录或文件不存在
    {
        iRetCode = ERROR_QFTP_FILEORDIR_NOT_FOUND;
        goto END;
    }
    else if (226 == m_iLastRecvAnswerCode)       // 针对目录无数据情况(不是出错，而是正常传输结束)
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

////////////////////////////////// 上传相关
/**
 * SaveBufferToFtp - 保存缓冲区数据到服务器
 *
 * hConnectSocket：命令通道socket
 * pbFileName：文件名称
 * pbBuffer：缓冲区
 * nSize：大小
 * fAppend：是否append(经测试，服务器不支持APPE命令)
 *
 * 返回值：
 * ERROR_QFTP_OK：正常
 * ERROR_QFTP_SEND_ERROR：发送命令/数据失败
 */
INT CQFTP::SaveBufferToFtp(SOCKET& hConnectSocket, const CHAR* pbFileName, CHAR* pbBuffer, INT nSize, BOOL fAppend)
{
    CHAR szCommand[256] = {0};
    SOCKET hDataSocket = 0;
    INT iRet;

    if (fAppend)
    {
        sprintf(szCommand, "APPE %s\r\n", pbFileName);   // 未测试
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

    // 创建数据通道
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

    // 关闭数据通道
    CloseSocket(hDataSocket);

    m_iLastRecvAnswerCode = RecvAnswer(hConnectSocket, NULL, 0);

    return ERROR_QFTP_OK;
}

/**
 * SaveDataToFtp - 保存数据到服务器
 *
 * strFileFullName：带完整路径的文件名称
 * pbDataForSend：缓冲区
 * uDataLen：缓冲区大小
 * fAppend：是否以附加形式保存(经测试，服务器不支持)
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
        strDirectory = strFileFullName.substr(0, iSlash+1); // 完整的_目录名_(如/xx/2011/08/15/14/)

    if (strDirectory.empty())   // 不带目录，直接上传
    {
        iRet = SaveBufferToFtp(m_socketCtrlConnect, strFileFullName.c_str(), pbDataForSend, uDataLen, fAppend);
        if (iRet < 0)
        {
            return iRet;
        }
        return ERROR_QFTP_OK;
    }
    string strTemp;
    iRet = ChangeDirectory(strDirectory.c_str());  // 尝试进入该目录(如果存在，就不用再创建)
    if (550 == iRet) // 目录不存在，创建后再上传
    {
        for (size_t i = 1; i < strDirectory.size(); i++)
        {
            if ('/' == strDirectory.at(i))  // 找到目录
            {
                strTemp = strDirectory.substr(0, i);
                MakeDirectory(strTemp.c_str());     // 如果存在，继续，不用处理
            }
        }
        // 创建完毕，保存
        iRet = SaveBufferToFtp(m_socketCtrlConnect, strFileFullName.c_str(), pbDataForSend, uDataLen, fAppend);
        if (iRet < 0)
        {
            return iRet;
        }
        return ERROR_QFTP_OK;
    }
    // 存在，直接上传(带完整路径)
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

// 写缓冲区内容到FTP中
// szFileName: 带完整路径的文件名
// szBuffer：缓冲区
// dwLength：缓冲区大小
// 成功返回0，失败返回ERROR_QFTP_SEND_ERROR
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
// 从服务器下载文件到内存中
// pbRemoteFile为带路径的文件
// 出错返回ERROR_QFTP_RECV_ERROR
// 没有找到文件返回ERROR_QFTP_NOT_FOUND
// 成功返回实际的长度(缓冲区及文件长取最小的那个)
INT CQFTP::GetFileFromFtp(const CHAR* pbRemoteFile, CHAR* szBuffer, DWORD32 dwBufLen)
{
    SOCKET hDataSocket;
    INT iRet = 0;
    CHAR szCommand[256] = {0};
    CHAR szDataBuffer[8*1024] = {0};    // 8KB 缓冲区

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

    // 打开数据通道
    iRet = OpenPassiveDataConnection(m_socketCtrlConnect, hDataSocket, szCommand);
    if (ERROR_QFTP_FILEORDIR_NOT_FOUND == iRet) // 没有找到文件
    {
        return ERROR_QFTP_NOT_FOUND;
    }

    DWORD32 dwFileSize = 0;
    // we get the message like  "150 Opening BINARY mode data connection for 1234.dat (29068 bytes)"
    // we need the file size: 29068
    sscanf(m_szRecvBuf, "%*[^(](%d[^ ]", &dwFileSize);  // 获得文件大小

    INT nNum = 0;
    CHAR* pTmp = szBuffer;
    INT nTmpSize = dwFileSize > dwBufLen ? dwBufLen : dwFileSize;
    INT nTrueSize = nTmpSize;

    if (dwFileSize <= 0)//有时FTP服务器并不一定返回150,而是125(只有成功提示，没有文件大小)
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
	    // 取小者
	    while (nTmpSize > 0)
	    {
	        nNum = Receive(hDataSocket, szDataBuffer, sizeof(szDataBuffer));
	        // select出错或接收出错，或超时(如断网)，直接返回，不重试
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

    // 关闭数据连接
    CloseSocket(hDataSocket);
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);

    return nTrueSize;
}

// 读FTP中指定文件到缓冲区中
// szFileName：FTP文件名
// szBuffer：缓冲区
// szBufLen：缓冲区大小
// [OUT]dwDataLen：实际读取的大小
// 读取文件失败或指定缓冲区小于实际文件长度，设置dwDataLen为缓冲区指定的大小，由调用者判断是否出错
// 指定缓冲区大于文件实际长度，返回0，并设置dwDataLen为实际文件长度
// 除了网络异常返回ERROR_QFTP_RECV_ERROR，其它都返回0
INT CQFTP::ReadFileFromFtp(CHAR* szFileName, CHAR* szBuffer, DWORD32 dwBufLen, DWORD32& dwDataLen)
{
    INT iRet = 0;
    iRet = GetFileFromFtp(szFileName, szBuffer, dwBufLen);
    if (ERROR_QFTP_RECV_ERROR == iRet)
    {
        return ERROR_QFTP_RECV_ERROR;
    }
    else if (ERROR_QFTP_NOT_FOUND == iRet)  // 没有找到文件，返回0，设置dwDataLen为0
    {
        dwDataLen = 0;
        return ERROR_QFTP_OK;
    }
    // 找到，设置真实读到的长度
    dwDataLen = iRet;

    return ERROR_QFTP_OK;
}

// 删除单个文件
// szFileName: FTP服务器上完整的路径及文件名(如“/array1/guest-share/user1/foo.dat”)
// 返回值：
// ERROR_QFTP_DEL_ERROR：文件不存在或删除对象不正确
// ERROR_QFTP_OK：成功删除文件
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
    else if (ERROR_QFTP_DATA_CONNECT_CLOSED == iRet)  // 目录无文件，直接返回ERROR_QFTP_NO_FILE_IN_DIR
    {
        return ERROR_QFTP_NO_FILE_IN_DIR;
    }

    //TODO 处理接收到的数据
    nNum = Receive(hDataSocket, szDataBuffer, sizeof(szDataBuffer));
    // select出错或接收出错，或超时(如断网)，直接返回，不重试
    if (nNum < 0)
    {
        return ERROR_QFTP_RECV_ERROR;
    }
    szDataBuffer[nNum] = '\0';
    debug("receive bufer: %s, num: %d\n", szDataBuffer, nNum);
    CloseSocket(hDataSocket);      // 关闭数据连接
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

// 获取目录或文件名称，保存到队列中
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
    strFile.append(pbTemp, nBytesThisTime); // 将接收到的数据添加到strFile中，注意需要指定字节数
    // 先解析一部分
    while (strFile.length())
    {
        size_t iCRLF = strFile.find('\n');
        if (iCRLF != string::npos)  // 找到
        {
            qFileName.push_back(strFile.substr(0, iCRLF-1));
            strFile.erase(0, iCRLF+1);            // 删除xxx\r\n
        }
        // 如果还有未以\r\n结束的文件名，在下面再解析，此处不作处理
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
                strFile.erase(0, iCRLF+1);            // 删除xxx\r\n
            }
            else
            {
                break;
            }
        }
    }   // end of while

    return ERROR_QFTP_OK;
}

// 列出指定目录下所有内容(目录或文件名称)，保存到队列中
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
    else if (ERROR_QFTP_DATA_CONNECT_CLOSED == ret)  // 目录无文件，直接返回ERROR_QFTP_NO_FILE_IN_DIR
    {
        return ERROR_QFTP_NO_FILE_IN_DIR;
    }
    else if (ERROR_QFTP_OK != ret)
    {
    	return ERROR_QFTP_CONNECT_FAILED;
    }

    GetFileName(hDataSocket, qFileName);    // 将文件列表保存到qFileName中
    CloseSocket(hDataSocket);               // 关闭数据连接
    m_iLastRecvAnswerCode = RecvAnswer(m_socketCtrlConnect, NULL, 0);

    return ERROR_QFTP_OK;
}

// 删除一个目录，该目录下不能存在目录
INT CQFTP::DeleteOneDirectory(const CHAR* pbDirectory)
{
    CHAR szCommand[256] = {0};
    deque<string> qFileName;
    deque<string> sDirectory;

    sprintf(szCommand, "XCWD %s\r\n", pbDirectory);

    int ret = SendCommand(m_socketCtrlConnect, szCommand);
    if (550 == ret) // 目录不存在
    {
        debug("%s: No such directory.\n", pbDirectory);
        return ERROR_QFTP_NOT_FOUND;
    }

    ret = ListToDeque("", qFileName);       // 列出该目录下的 _文件_放到qFileName队列中
    if (ERROR_QFTP_NOT_FOUND == ret
        || ERROR_QFTP_NO_FILE_IN_DIR == ret
       )   // 如果没有内容，即为空目录，不删除该目录
    {
        return ERROR_QFTP_OK;
    }

    while (qFileName.size() > 0)
    {
        ret = DeleteOneFile((CHAR*)qFileName.front().c_str());
        qFileName.pop_front();              // 弹出，继续取下一个
    }

    return ERROR_QFTP_OK;
}
