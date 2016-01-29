#ifndef __SWPA_LIST_H__
#define __SWPA_LIST_H__

#ifdef __cplusplus
#define DEFAULT(val) = val
extern "C"
{
#else
#define DEFAULT(val)
#endif

/**
 *@brief 创建链表
 *@param [in] max_size 链表的最大大小，0表明不限制大小
 *@return 成功返回链表句柄，失败返回-1
 */
int swpa_list_create(int max_size DEFAULT(0));

/**
 *@brief 释放链表资源
 *@param [in] handle 链表句柄
 *@return 成功返回0，失败返回-1
 */
int swpa_list_destroy(int handle);

/**
 *@brief 向链表结尾添加一个节点
 *@param [in] handle 链表句柄
 *@param [in] value 节点的值
 *@return 成功返回新插入节点的位置，失败返回-1
 */
int swpa_list_add(int handle, void *value);

/**
 *@brief 向链表插入一个节点
 *@param [in] handle 链表句柄
 *@param [in] pos 节点位置
 *@param [in] value 节点的值
 *@return 成功返回节点位置，失败返回-1
 */
int swpa_list_insert(int handle, int pos, void *value);

/**
 *@brief 删除链表中的一个节点
 *@param [in] handle 链表句柄     
 *@param [in] pos 节点位置
 *@param [in] 成功返回0，失败返回-1
 */
int swpa_list_delete(int handle, int pos);

/**
 *@brief 从链表的头结点删除一个节点
 *@param [in] handle 链表句柄
 *@param [in] from_head 为0表明删除头节点，为1表明删除尾节点
 *@param [in] timeout -1表示阻塞锁定；0表示尝试锁定；大于0表示锁定超时时限。
 *@retval 成功返回节点的值，失败返回0
 */
void* swpa_list_remove(int handle, int from_head DEFAULT(0), int timeout DEFAULT(0));

/**
 *@brief 根据节点位置获得节点的值
 *@param [in] handle 链表句柄
 *@param [in] pos 节点位置
 *@return 成功返回节点值，失败返回0
 */
void* swpa_list_value(int handle, int pos);

/**
 *@brief 链表的大小
 *@param [in] handle 链表句柄
 *@return 返回链表节点的个数
 */
int swpa_list_size(int handle);

/**
 *@brief 遍历链表，遍历之前必须先open
 *@param [in] handle 链表句柄
 *@param [in] from_head 释放从头节点开始遍历，0表明从头节点开始，1表明从尾节点开始遍历
 *@return 成功返回头节点位置，失败返回-1
 */
int swpa_list_open(int handle, int from_head DEFAULT(0));

/**
 *@brief 链表节点下移一个节点
 *@brief handle 链表的句柄
 *@return 成功返回当前节点位置,失败返回-1
 */
int swpa_list_next(int handle);

/**
 *@brief 链表节点上移一个节点
 *@brief handle 链表句柄
 *@return 返回当前节点位置
 */
int swpa_list_prev(int handle);

/**
 *@brief 停止遍历
 *@brief handle 链表句柄
 *@return 成功返回0，失败返回-1
 */
int swpa_list_close(int handle);

#ifdef __cplusplus
}
#endif
#endif
