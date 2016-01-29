/*
 * TelnetSvr.h
 *
 *  Created on: May 9, 2011
 *      Author: ganzz
 */

#ifndef TELNETSVR_H_
#define TELNETSVR_H_
#include "config.h"
#if defined(_CAM_APP_) || defined(SINGLE_BOARD_PLATFORM)
#define DEFAULT_TELNETD_PORT 23 // HvTelnet端口号
#else
#define DEFAULT_TELNETD_PORT 9886
#endif

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Log.h"
#include "hvthreadbase.h"
#include "HvSockUtils.h"

class TelnetSvr : public CHvThreadBase
{
public:
	TelnetSvr();
	virtual ~TelnetSvr();

    // 取静态对象
	static TelnetSvr* GetInstance();

    Log* GetLog();

    // 以独立线程运行,并开启服务器进程
    virtual HRESULT Run(void* pvParam);

	// 关闭服务器进程
	void Close();

	struct SConn
	{
		HV_SOCKET_HANDLE hSock;
		sockaddr_in addr;
		Log* pLog;
	};

protected:
    virtual const char* GetName()
    {
        static char szName[] =  "TelnetSvr";
        return szName;
    }

	// 处理单个telnet连接函数
	static void* handle_conn(void* lp);

    // buf为已创建2048长度的空间，避免浪费
	static int send_main_menu(int hSock, char* buf);

	// buf为已创建2048长度的空间，避免浪费
	static int send_log_menu(int hSock, char* buf);

    // buf为已创建2048长度的空间，避免浪费
	static int send_rwreg_menu(int hSock, char* buf);

    // 处理具体telnet连接函数, buf为已创建2048长度的空间，避免浪费
	static int handle_input(SConn* pConn, char* buf);

	// buf为已创建2048长度的空间，避免浪费
	static int send_log(SConn* pConn, char* buf, int nType);

    // buf为已创建2048长度的空间，避免浪费
	static int send_hdd_info(int hSock, char* buf);

	// buf为已创建2048长度的空间，避免浪费
	static int send_mod_info(int hSock, char* buf);

	// buf为已创建2048长度的空间，避免浪费
	static int rw_reg_info(int hSock, char* buf);

	// buf为已创建2048长度的空间，避免浪费
	static int exec_user_cmd(int hSock, char* buf, char cmd);

    // UTF8 to GB2312
    static int u2g(char* input, int inputLen, char* output, int charOutPutLen);

    static int SendAll(HV_SOCKET_HANDLE sock, char * buf, int len);

    // 具体启动服务器的函数
	void run_svr();

    // 标识是否运行
    bool m_bRun;

    static unsigned short port;

	HV_SOCKET_HANDLE m_hSockSvr;
    Log m_log;

	static TelnetSvr* s_Instance;
};

#endif /* TELNETSVR_H_ */
