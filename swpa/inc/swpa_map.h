#ifndef __SWPA_MAP_H__
#define __SWPA_MAP_H__
#ifdef __cplusplus
#define DEFAULT(val) = val
extern "C"
{
#else
#define DEFAULT(val)
#endif

/**
 *@brief 创建映射表
 *@param [in]int (*compare)(void* key1, void* key2)key比较的函数指针,-1表明小于,0表明相等，1表明大于
 *@return 成功返回映射表句柄，失败返回-1
 */
int swpa_map_create(int (*compare)(void*, void*) DEFAULT(0));

/**
 *@brief 删除映射表
 *@param [in] handle 映射表句柄
 *return 成功返回0，失败返回-1
 */
int swpa_map_destroy(int handle);

/**
 *@brief 添加一个节点
 *@param [in] handle 映射表句柄
 *@param [in] key 映射表键值
 *@param [in] value映射表的值
 *@return 成功返回0，失败返回-1
 */
int swpa_map_add(int handle, void* key, void* value);

/**
 *@brief 删除一个节点
 *@param [in] handle 映射表句柄
 *@param [in] key 映射表键值
 *@return 成功返回0，失败返回-1
 */
int swpa_map_delete(int handle, void* key);

/**
 *@brief 映射表的大小
 *@param [in] handle 映射表句柄
 *@return 返回映射表大小
 */
int swpa_map_size(int handle);

/**
 *@brief 查找满足key的节点
 *@param [in] handle 映射表句柄
 *@param [in] key 映射表键值
 *@return 成功返回节点的值，失败返回0
 */
void* swpa_map_find(int handle, void* key);

/**
 *@brief 打开映射表，准备遍历表
 *@param [in] handle 映射表句柄
 *@return 成功返回映射表头结点位置，失败返回-1
 */
int swpa_map_open(int handle);

/**
 *@brief 取得下一个节点
 *@param [in] handle 映射表句柄
 *@return 返回下一个节点位置
 */
int swpa_map_next(int handle);

/**
 *@brief 关闭映射表，停止遍历
 *@param [in] handle 映射表句柄
 *@return 成功返回0，失败返回-1
 */
int swpa_map_close(int handle);

/**
 *@brief 得到当前节点的key
 *@param [in] handle 映射表句柄
 *@param [in] pos 位置
 *@return 成功返回节点的key,失败返回0
 */
void* swpa_map_key(int handle, int pos);

/**
 *@brief 得到当前节点的value
 *@param [in] handle 映射表句柄
 *@param [in] pos 位置
 *@return 成功返回节点的value,失败返回0
 */
void* swpa_map_value(int handle, int pos);

#ifdef __cplusplus
}
#endif
#endif
