/**
* @file swpa_socket.h
* @brief 网络套接字
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含标准网络套接字socket的一组函数。
*/

#ifndef _SWPA_SOCKET_H_
#define _SWPA_SOCKET_H_

#include <sys/socket.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#define SWPA_SOCKET_T int
//#define SWPA_SOCKADDR sockaddr_in

// 阻塞套接字
#define SWPA_SOCK_STREAM            SOCK_STREAM // 1
// 非阻塞套接字
#define SWPA_SOCK_DGRAM             SOCK_DGRAM // 2

//本地套接字
#define SWPA_AF_UNIX				AF_UNIX // 1
// 网络协议
#define SWPA_AF_INET                AF_INET // 2

//
#define SWPA_EAGAIN                 11

#define SWPA_SOL_SOCKET             SOL_SOCKET // 1


#define SWPA_SO_RCVTIMEO            SO_RCVTIMEO // 20
#define SWPA_SO_SNDTIMEO            SO_SNDTIMEO // 21
#define SWPA_SO_TIMEOUT             SO_TIMEOUT // 10060
#define SWPA_SO_BROADCAST           SO_BROADCAST // 6
#define SWPA_SO_REUSEADDR           SO_REUSEADDR // 2

#define SWPA_SO_SNDBUF				SO_SNDBUF // 7
#define SWPA_SO_RCVBUF				SO_RCVBUF // 8


typedef struct SWPA_SOCKADDR_UN
{
	int sun_family;
	char sun_path[128];
}SWPA_SOCKADDR_UN,*LPSWPA_SOCKADDR_UN;

typedef struct SWPA_SOCKADDR
{
    int port;
    char ip[32];
}SWPA_SOCKADDR,*LPSWPA_SOCKADDR;

typedef struct _SWPA_SOCKET_ATTR_T
{
    int af;
    int type;
    int protocol;
}SWPA_SOCKET_ATTR_T;

typedef struct SWPA_TIMEVAL
{
    long tv_sec;
    long tv_usec;

}SWPA_TIMEVAL;


#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief 创建套接字
* @param [out] skt 套接字结构体指针
* @param [in] attr 套接字属性
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_create(SWPA_SOCKET_T* skt, const SWPA_SOCKET_ATTR_T* attr);

/**
* @brief 删除套接字
* @param [in] skt 套接字结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_delete(SWPA_SOCKET_T skt);

/**
* @brief 绑定套接字
* @param [in] skt 套接字结构体
* @param [in] addr 要绑定的网络地址
* @param [in] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_bind(
    SWPA_SOCKET_T skt,
    const struct SWPA_SOCKADDR* addr,
    unsigned int addrlen
);

/**
* @brief 绑定本地套接字
* @param [in] skt 套接字结构体
* @param [in] addr 要绑定的本地地址
* @param [in] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_bind_un(
    SWPA_SOCKET_T skt,
    const struct SWPA_SOCKADDR_UN* addr,
    unsigned int addrlen
);

/**
* @brief 监听套接字
* @param [in] skt 套接字结构体
* @param [in] backlog 套接字监听队列所允许的最大长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_listen(SWPA_SOCKET_T skt, int backlog);

/**
* @brief 接受套接字
* @param [in] skt 套接字结构体
* @param [out] addr 对方网络地址
* @param [out] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_accept(
    SWPA_SOCKET_T skt,
    struct SWPA_SOCKADDR* addr,
    unsigned int* addrlen,
    SWPA_SOCKET_T *outSock
);

/**
* @brief 连接套接字
* @param [in] skt 套接字结构体
* @param [out] addr 目标网络地址
* @param [out] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_connect(
    SWPA_SOCKET_T skt,
    const struct SWPA_SOCKADDR* addr,
    unsigned int addrlen
);

/**
* @brief 连接本地套接字
* @param [in] skt 套接字结构体
* @param [out] addr 目标本地地址
* @param [out] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_connect_un(
    SWPA_SOCKET_T skt,
    const struct SWPA_SOCKADDR_UN* addr,
    unsigned int addrlen
);

/**
* @brief 面向有连接的套接字发送数据
* @param [in] skt 套接字结构体
* @param [in] buf 数据内容
* @param [in] len 数据长度
* @param [out] succ_send_len 已成功发送的数据长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_send(
    SWPA_SOCKET_T skt,
    const void* buf,
    unsigned int len,
    unsigned int* succ_send_len
);

/**
* @brief 面向无连接的套接字发送数据
* @param [in] skt 套接字结构体
* @param [in] buf 数据内容
* @param [in] len 数据长度
* @param [out] succ_send_len 已成功发送的数据长度
* @param [in] dest_addr 目的地址结构体指针
* @param [in] addrlen 目的地址结构体长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_sendto(
    SWPA_SOCKET_T skt,
    const void* buf,
    unsigned int len,
    unsigned int* succ_send_len,
    const struct SWPA_SOCKADDR* dest_addr,
    unsigned int addrlen
);

/**
* @brief 面向有连接的套接字接收数据
* @param [in] skt 套接字结构体
* @param [out] buf 数据接收缓存区
* @param [in,out] len 传入buf缓存区长度，传出成功接收到的数据长度。
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_recv(
    SWPA_SOCKET_T skt,
    void* buf,
    unsigned int* len
);

/**
* @brief 面向无连接的套接字接收数据
* @param [in] skt 套接字结构体
* @param [out] buf 数据接收缓存区
* @param [in,out] len 传入buf缓存区长度，传出成功接收到的数据长度。
* @param [out] src_addr 源主机地址结构体指针
* @param [out] addrlen 源主机地址结构体长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_recvfrom(
    SWPA_SOCKET_T skt,
    void* buf,
    unsigned int* len,
    struct SWPA_SOCKADDR* src_addr,
    unsigned int* addrlen
);

/**
* @brief 设置套接字
* @param [in] skt 套接字结构体
* @param [in] cmd 设置命令种类
* @param [in] ... 设置命令参数
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_ioctl(SWPA_SOCKET_T skt, int cmd, void* argv);

/**
* @brief 设置套接字
* @param [in] skt 套接字结构体
* @param [in] level 设置的网络层
* @param [in] optname 设置的选项
* @param [in] optval 代表欲设置的值
* @param [in] optlen 为optval的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_opt( SWPA_SOCKET_T skt, int level,int optname
                    ,const void * optval, int optlen );
/**
* @brief 设置套接字
* @param [in] skt 套接字结构体
* @param [in] level 设置的网络层
* @param [in] optname 设置的选项
* @param [out] optval 该选项被设置的值
* @param [in/out] optlen 为optval的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_getopt( SWPA_SOCKET_T skt, int level,int optname
                    ,const void * optval, int *optlen );
/**
* @brief 获取套接字的对方网络地址
* @param [in] skt 套接字结构体
* @param [out] addr 对方的网络地址
* @param [out] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_getpeername(
    SWPA_SOCKET_T skt,
    struct SWPA_SOCKADDR* addr,
    unsigned int* addrlen
);

/**
* @brief 获取套接字的本机网络地址
* @param [in] skt 套接字结构体
* @param [out] addr 本机的网络地址
* @param [out] addrlen 地址结构体的长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_getsockname(
    SWPA_SOCKET_T skt,
    struct SWPA_SOCKADDR* addr,
    unsigned int* addrlen
);

/**
* @brief 将网络地址结构体转为点分十进制字符串形式
* @param [in] addr 网络地址结构体
* @param [out] str 字符串网络地址。样例："192.168.255.255"
* @param [in] len 缓存区str的长度。建议为16字节
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_addr2string(const struct SWPA_SOCKADDR* addr, char* str, int len);

/**
* @brief 将点分十进制字符串形式的网络地址转为结构体
* @param [in] str 字符串网络地址
* @param [out] addr 网络地址结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
int swpa_socket_string2addr(const char* str, unsigned short sin_port, struct SWPA_SOCKADDR* addr);

/**
*
* @brief 获取最好错误码
* @retval 返回当前错误码
* @see swpa_socket.h
*/

int swpa_socket_get_lastererrocode( );

#ifdef __cplusplus
}
#endif

#endif

