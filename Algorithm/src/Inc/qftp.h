#ifndef _QFTP_H_
#define _QFTP_H_

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <string>
#include <deque>

//#define __DEBUG_          // 打印调试信息宏
//#define PRINT_MESSAGE   // 打印服务器返回信息

#ifdef WIN32    // windows

    #include <winsock2.h>   // 必须放到windows.h前
    #pragma comment (lib,"ws2_32.lib")
    #include <windows.h>

    #ifndef WORD16
    typedef unsigned short WORD16;
    #endif
    #ifndef DWORD32
    typedef unsigned int DWORD32;
    #endif

    #define MSG_NOSIGNAL 0  // for send

    #ifdef __DEBUG_
    #define debug printf
    #else
    #define debug
    #endif

#else   // linux

    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    typedef char CHAR;
    typedef int INT;
    typedef unsigned int UINT;
    typedef int BOOL;
    typedef unsigned short WORD16;
    typedef unsigned int DWORD32;
    typedef struct sockaddr SOCKADDR;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef int SOCKET;

    #ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
    #endif

    #ifndef SOCKET_ERROR
    #define SOCKET_ERROR (-1)
    #endif

    #ifdef __DEBUG_
    #define debug(fmt...) printf(fmt)
    #else
    #define debug(fmt...)
    #endif

#endif  /* WIN32 */

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif
#ifdef __cplusplus
#ifndef NULL
#define NULL    0
#endif
#endif

//自定义错误码
// 通过打印各接口返回值，得到错误原因
#define ERROR_QFTP_BASE -1000
#define ERROR_QFTP_OK 0

#define ERROR_QFTP_UNKNOWNERROR                     ERROR_QFTP_BASE+0
#define ERROR_QFTP_CONNECT_INVALID                  ERROR_QFTP_BASE+1
#define ERROR_QFTP_LOGOUT_FAILED                    ERROR_QFTP_BASE+2
#define ERROR_QFTP_PARAMETER_INVALID                ERROR_QFTP_BASE+3
#define ERROR_QFTP_SOCKET_INITIALIZE_FAILED         ERROR_QFTP_BASE+4
#define ERROR_QFTP_CONNECT_FAILED                   ERROR_QFTP_BASE+5
#define ERROR_QFTP_CMD_ERROR                        ERROR_QFTP_BASE+6
#define ERROR_QFTP_FILEORDIR_NOT_FOUND              ERROR_QFTP_BASE+7
#define ERROR_QFTP_DATA_CONNECT_CLOSED              ERROR_QFTP_BASE+8

// select函数出错
#define ERROR_QFTP_SELECT_ERROR                 ERROR_QFTP_BASE+9
// 接收出错
#define ERROR_QFTP_RECV_ERROR                   ERROR_QFTP_BASE+10
// 发送出错
#define ERROR_QFTP_SEND_ERROR                   ERROR_QFTP_BASE+11
// 接收超时
#define ERROR_QFTP_RECV_TIMEOUT                 ERROR_QFTP_BASE+12
//发送超时
#define ERROR_QFTP_SEND_TIMEOUT                 ERROR_QFTP_BASE+13
// 用户或密码错误
#define ERROR_QFTP_USER_ERROR                   ERROR_QFTP_BASE+14
// 文件或目录未找到
#define ERROR_QFTP_NOT_FOUND                    ERROR_QFTP_BASE+15
// 删除文件出错
#define ERROR_QFTP_DEL_ERROR                    ERROR_QFTP_BASE+16
//目录无文件
#define ERROR_QFTP_NO_FILE_IN_DIR               ERROR_QFTP_BASE+17
#define MAX_CMD_LEN         256

using namespace std;

namespace nsFTP
{

class  CQFTP
{
public:
    // 默认为匿名登陆，超时单位MS 0表示阻塞模式
    CQFTP(const string& strHostName, const string& strUserName = "anonymous", const string& strPassWord = "yetpasswd", UINT nTimeout = 1000);
    ~CQFTP();
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
    INT LoginFtpServer(void);

    //说明：登出
    //成功返回：ERROR_QFTP_OK
    INT LogoutFtpServer(void);

    //说明：检测连接的有效性
    //连接有效返回：ERROR_QFTP_OK
    INT TestConnectValidity(void);

    ///////////////////// 新的接口
    // 读取FTP服务器上的文件到内存中
    // szFileName: FTP服务器上完整的路径及文件名(如“/array1/guest-share/user1/foo.dat”)
    // szBuffer: 缓冲区指针
    // dwLength：缓冲区大小
    // [OUT]dwDataLen：实际读取到的数据大小
    // 说明：当网络异常时，返回ERROR_QFTP_RECV_ERROR，其它情况返回0，
    // 如果指定文件不存在，dwDataLen值为0，如果缓冲区小于实际文件长度，dwDataLen为缓冲区大小
    INT ReadFileFromFtp(CHAR* szFileName, CHAR* szBuffer, DWORD32 dwLength, DWORD32& dwDataLen);

    // 保存数据到FTP中
    // szFileName: FTP服务器上完整的路径及文件名(如“/array1/guest-share/user1/foo.dat”)
    // szBuffer: 缓冲区指针
    // dwLength：缓冲区大小
    // 正常返回0，失败返回ERROR_QFTP_SEND_ERROR
    INT WriteFileToFtp(CHAR* szFileName, CHAR* szBuffer, DWORD32 dwLength);

    // 删除单个文件
    // szFileName: FTP服务器上完整的路径及文件名(如“/array1/guest-share/user1/foo.dat”)
    // 成功返回0，失败返回ERROR_QFTP_DEL_ERROR
    INT DeleteOneFile(CHAR* szFileName);

    // 让FTP一直处于连接状态
    // 成功返回0，失败返回连接无效ERROR_QFTP_CONNECT_INVALID
    // 该函数是产生数据传输，让FTP保持连接状态，但当网络异常时，返回失败
    INT KeepAlive(void);

    // 删除一个目录，该目录下不能存在目录
	INT DeleteOneDirectory(const CHAR* pbDirectory);
    // 以下函数为类内部使用
private:
    INT ConnectSocket(SOCKET& hConnectSocket, const CHAR* szIP, INT iPort);
    INT Send(SOCKET& socket, const CHAR* pszBuffer, INT nSize);
    INT Receive(SOCKET& socket, CHAR* pszBuffer, INT nSize);
    INT CloseSocket(SOCKET& hConnectSocket);

    INT RecvAnswer(SOCKET& hConnectSocket, CHAR* pszRecvBuf, INT iLen);
    INT SendCommand(SOCKET& hConnectSocket, const CHAR* pbCommand);

    INT ChangeDirectory(const CHAR* DirectoryPath);
    INT MakeDirectory(const CHAR* DirectoryPath);

    INT OpenPassiveDataConnection(SOCKET& hConnectSocket, SOCKET& hDataSocket, CHAR* pbCommand);
    INT GetPortForPASV(CHAR* szRecvBuf);
    INT SendData(SOCKET& hConnectSocket, CHAR* pbDataBuffer, INT nSize);
    INT SaveBufferToFtp(SOCKET& hConnectSocket, const CHAR* pbFileName, CHAR* pbBuffer, INT nSize, BOOL fAppend);
    INT SaveDataToFtp(string strFileFullName, CHAR* pbDataForSend, UINT uDataLen, BOOL fAppend=FALSE);

    INT GetFileFromFtp(const CHAR* pbRemoteFile, CHAR* szBuffer, DWORD32 dwBufLen);

    INT List(const CHAR* pbPath);

    INT GetFileName(SOCKET& s, deque<string>& qFileName);

	INT ListToDeque(const CHAR*  pbPath, deque<string>& qFileName);
private:

    SOCKET m_socketCtrlConnect;     // 控制(命令)通道socket
    INT    m_iLastRecvAnswerCode;   // 服务器返回代号，如226 550 200等
    UINT   m_wTimeout;              // 超时时间(单位为秒)
    CHAR   m_szRecvBuf[1024];       //接收缓冲区(发送命令返回的信息)
    string m_strFtpIP;              //用于存储IP地址
    string m_strUserName;           //用于存储用户名
    string m_strPassWord;           //用于存储口令
};  // class CQFTP

}   // namespace nsFTP

#endif /* _QFTP_H_ */
