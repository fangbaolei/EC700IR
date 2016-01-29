#ifndef _TCPTERMINAL_H_
#define _TCPTERMINAL_H_

//终端服务进程
void TcpTerminalProcess();

//读终端数据
int ReadTcpTerminal( void *pRecvBuf, int iRecvLen );

//写终端数据
int WriteTcpTerminal( const void *pSendBuf, int iSendLen );

//写终端字符串
int WriteTcpTerminalString( const char *pSendBuf );

#endif
