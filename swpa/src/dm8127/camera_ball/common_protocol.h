/**
 * @file   common_protocol.h
 * @author 
 * @date   Tue Jun 17 09:20:40 2014
 * 
 * @brief  api for serial port device of posix.
 * 
 * 
 */

#ifndef __COMMON_PROTOCOL_H
#define __COMMON_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif


#define _DEBUG

// 模拟，不实际传输
//#define __LL_DEBUG

// 传输时打印命令
//#define __DUMP_CMD

// 第6级是内部调试等级
#define DEFAULT_RANK 7

#define com_debug(rank, fmt, ...) \
    do {\
        if (rank <= DEFAULT_RANK) \
        {\
            printf(fmt, ##__VA_ARGS__); \
        }\
    }while(0)

#define com_print(fmt, ...) \
    do {\
        printf(fmt, ##__VA_ARGS__); \
    }while(0)


#define _LOCK(_lock)	\
if (0 !=_lock && 0 != swpa_sem_pend(&_lock, -1))\
{\
	com_print("Err: failed to lock %s! [%d]\n", #_lock, -1);\
	return -1;	\
}\
//printf("[zydebug] lock 0x%x @ %s:L%d\n", _lock, __FUNCTION__, __LINE__);


#define _UNLOCK(_lock)	\
/*printf("[zydebug] unlock 0x%x @ %s:L%d\n", _lock, __FUNCTION__, __LINE__);*/ \
if (0 !=_lock && 0 != swpa_sem_post(&_lock))\
{\
	com_print("Err: failed to unlock %s! [%d]\n", #_lock, -1);\
	return -1;	\
}


#define _CREATE_LOCK(_lock) \
\
if (0 ==_lock && 0 != swpa_sem_create(&_lock, 1, 1))\
{\
	com_print("Err: failed to create lock %s! [%d]\n", #_lock, -1);\
	return -1;	\
}\


#define _DELETE_LOCK(_lock) \
if (0 !=_lock && 0 != swpa_sem_delete(&_lock))\
{\
	com_print("Err: failed to delete lock %s! [%d]\n", #_lock, -1);\
	return -1;	\
}



/// 成功
#define COM_SUCCESS       0
/// 失败
#define COM_FAILURE       -1
/// 超时
#define COM_TIMEOUT       -6
/// 命令执行失败
#define COM_CMDERROR      -3

/* size of the local packet buffer */
#define COM_INPUT_BUFFER_SIZE     32

/// 串口协议接口
typedef struct _COM_interface
{
    /// 串口设备名称
    const char* device;
    /// 设备地址
    uint32_t address;
    /// 广播标志
    uint32_t broadcast;

    /// 串口设备描述符
    int port_fd;
    /// 串口接收数据缓冲区
    unsigned char ibuf[COM_INPUT_BUFFER_SIZE];
    /// 串口接收数据缓冲区大小
    uint32_t bytes;
    /// 串口接收数据包类型
    uint32_t type;
} COMInterface_t;

typedef struct _COM_interface *pCOMInterface_t;

/// 命令接收包结构体
typedef struct _COM_packet
{
    /// 命令包缓冲区
    unsigned char bytes[32];
    /// 命令包缓冲区大小
    uint32_t length;
} COMPacket_t;
/** 
 * 打开串口
 * 
 * @param device_name [in] 串口设备名称
 * @param baudrate [in] 波特率
 * @param data_bits [in] 数据位
 * @param parity [in] 奇偶校验性
 * @param stop_bits [in] 停止位
 * 
 * @return 成功：串口设备描述符  失败：-1
 */
int32_t com_open_serial(const char *device_name, int baudrate, int data_bits, int parity, int stop_bits);
/** 
 * 关闭串口
 * 
 * @param fd [in]  串口设备描述符
 * 
 * @return 成功：0  失败：-1
 */
int32_t com_close_serial(int fd);
/** 
 * 读取串口缓冲区剩余的数据
 * 
 * @param fd [in] 串口设备描述符
 * @param buffer [out] 缓冲区
 * @param buffer_size [out] 缓冲区大小
 * 
 * @return 
 */
int32_t com_unread_bytes(int fd, unsigned char *buffer, int *buffer_size);
/** 
 * 
 * 
 * @param fd [in] 串口设备描述符
 * @param buffer [in] 要发送的数据的缓冲区
 * @param buffer_size [in] 缓冲区大小
 * 
 * @return 
 */
int32_t com_write_packet_data(int fd, unsigned char *buffer, int buffer_size);

/** 
 * 
 * 
 * @param fd [in] 串口设备描述符
 * @param buffer [out] 缓冲区
 * @param buffer_size [in/out] 缓冲区大小(传出参数表示实际读取到的数量)
 * @param terminator [in] 结束符(eg: 0xff)
 * 
 * @return 
 */
int32_t com_get_packet_terminator(int fd, unsigned char *buffer, int *buffer_size, int terminator);
/** 
 * 
 * 
 * @param fd [in] 串口设备描述符
 * @param buffer [out] 缓冲区
 * @param buffer_size [in/out] (传出参数表示实际读取到的数量)
 * 
 * @return 
 */
int32_t com_get_packet(int fd, unsigned char *buffer, int *buffer_size);

#ifdef __cplusplus
};
#endif

#endif /* __COMMON_PROTOCOL_H */
