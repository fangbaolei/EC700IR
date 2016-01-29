/**
* @file swpa_tcpip.h
* @brief 网络操作
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含网络的相关操作。例如获取以及设置IP地址、获取MAC地址等等。
*/

#ifndef _SWPA_TCPIP_H_
#define _SWPA_TCPIP_H_

#ifdef __cplusplus
extern "C"
{
#endif

// 网卡工作模式
typedef enum 
{
    ETH_AUTONEG = 0, // 自动协商
    ETH_1000_FULL,   // 千兆全双工
    ETH_100_FULL,    // 百兆全双工
    ETH_100_HALF,    // 百兆半双工
    ETH_10_FULL,     // 十兆全双工
    ETH_10_HALF,     // 十兆半双工
    ETH_UNKONW,      // 未知
} eth_type;

/**
* @brief 获取网络信息
* @param [in] eth 网络适配器名称
* @param [out] ip 网络IP地址缓存区
* @param [in] len_ip 网络IP地址缓存区长度。建议为16字节。
* @param [out] mask 子网掩码缓存区
* @param [in] len_mask 子网掩码缓存区长度。建议为16字节。
* @param [out] mac 网络MAC地址缓存区
* @param [in] len_mac 网络MAC地址缓存区长度。建议为32字节。
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_getinfo(
    const char* eth,
    char* ip,
    int len_ip,
    char* mask,
    int len_mask,
    char* gateway,
    int len_gateway,
    char* mac,
    int len_mac
);

/**
* @brief 设置网络信息
* @param [in] eth 网络适配器名称
* @param [in] ip 网络IP地址
* @param [in] mask 网络子网掩码
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_setinfo(
    const char* eth,
    const char* ip,
    const char* mask,
    const char* gateway
);

/**
* @brief 获取网络信息
* @param [in] eth 网络适配器名称
* @param [out] ip 网络IP地址缓存区
* @param [in] len_ip 网络IP地址缓存区长度。建议为16字节。
* @param [out] mask 子网掩码缓存区
* @param [in] len_mask 子网掩码缓存区长度。建议为16字节。
* @param [out] gateway网关缓存区
* @param [in] gateway网关缓存区长度。建议为16字节。
* @param [out] mac 网络MAC地址缓存区
* @param [in] len_mac 网络MAC地址缓存区长度。建议为32字节。
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_getinfo_with_id(
    const char* eth,
    int id,
    char* ip,
    int len_ip,
    char* mask,
    int len_mask,
    char* gateway,
    int len_gateway,
    char* mac,
    int len_mac
);

/**
* @brief 设置网络信息
* @param [in] eth 网络适配器名称
* @param [in] ip 网络IP地址
* @param [in] mask 网络子网掩码
* @param [in] id 多IP通道
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_setinfo_with_id(
    const char* eth,
    const int  id,
    const char* ip,
    const char* mask,
    const char* gateway
);


/**
* @brief 获取网关
* @param [in] eth 网络适配器名称
* @param [out] gw 网关网络IP地址缓存区
* @param [in] len 网关网络IP地址缓存区长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_getgateway(const char* eth, char* gw, int len);

/**
* @brief 设置网关
* @param [in] eth 网络适配器名称
* @param [in] gw 网关网络IP地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_setgateway(const char* eth, const char* gw);

/**
* @brief 获取网络MTU值
* @param [in] eth 网络适配器名称
* @param [out] mtu 网络MTU值
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_getmtu(const char* eth, unsigned int* mtu);

/**
* @brief 设置网络MTU值
* @param [in] eth 网络适配器名称
* @param [in] mtu 网络MTU值
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_setmtu(const char* eth, unsigned int mtu);

/**
* @brief 使能网络适配器
* @param [in] eth 网络适配器名称
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_enable(const char* eth);

/**
* @brief 静止网络适配器
* @param [in] eth 网络适配器名称
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_disable(const char* eth);

/**
* @brief 获取网络适配器连接状态
* @param [out] linked 连接状态。0：已断开；1：已连接。
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_getphystatus(const char* eth, int* linked);

/**
* @brief IP地址、子网掩码、网关合法性检查
* @param [in] ip_str IP地址字符串
* @param [in] net_mask_str 子网掩码字符串
* @param [in] gateway_str 网关字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_tcpip.h
*/
int swpa_tcpip_checkinfo(const char *ip_str, const char *net_mask_str, const char *gateway_str);

/**
* @brief 获取网卡工作模式
* @param [in] devname 网络适配器名称
* @param [out] type 网卡工作模式，定义见eth_type
* @param [out] autoneg 是否自动协商使能，一般不使用
* @retval 0 成功
* @retval -1 失败
*/
int swpa_tcpip_ethtool_gset(const char* devname, eth_type* type, int* autoneg);

/**
* @brief 设置网卡工作模式
* @param [in] devname 网络适配器名称
* @param [in] type 网卡工作模式，定义见eth_type
* @retval 0 成功
* @retval -1 失败
* @note 在清楚了解当前网络环境情况下才进行正确的设置，否则会出现异常，
        一般系统启动时已经设置好网卡工作模式，不再需要手动设置了。
        在百兆网络中，手动指定千兆全双工，是无法连接网络的，
        在强大的M$系统中测试亦如是。
*/
int swpa_tcpip_ethtool_sset(const char* devname, eth_type type);

#ifdef __cplusplus
}
#endif

#endif

