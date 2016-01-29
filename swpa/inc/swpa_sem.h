/**
* @file swpa_sem.h
* @brief 信号量管�?
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含信号量、互斥体、读写锁这类同步对象的操作函数�?
*/

#ifndef _SWPA_SEM_H_
#define _SWPA_SEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief 创建信号�?
* @param [out] sem 信号量结构体指针
* @param [in] init 信号量初始�?
* @param [in] init 信号量最大�?
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
* @note 该信号量为线程级别�?
*/
int swpa_sem_create(int* sem, unsigned int init, unsigned int max);

/*****************************************************************************
 �� �� ��  : swpa_processsem_create
 ��������  : ���̼��ź��������ӿ�
 �������  : int* sem  �ź����������ؾ��         
             unsigned int init �ź�����ʼֵ 
             unsigned int max  �ź������ֵ 
             const int sem_id  �ź���Ψһ��ʶ 
 �������  : ��
 �� �� ֵ  : int 0�����ɹ� ��0����ʧ��
 ���ú���  : 
 ע������  : sem_id�ǽ��̼��ź�����Ψһ��ʶ,��ͬ���ź���Ҫ��֤Ψһ��
 
 �޸���ʷ      :
  1.��    ��   : 2015��9��4��
    ��    ��   : huangdch
    �޸�����   : �����ɺ���

*****************************************************************************/
int swpa_processsem_create(int* sem, unsigned int init, unsigned int max, const int sem_id);

/**
* @brief 锁定信号�?
* @param [in] sem 信号量结构体指针
* @param [in] timeout 锁定超时。单位：毫秒
* @retval 0 成功或可�?
* @retval -1 失败或不可锁
* @see swpa_sem.h
* @note 当timeout�?1时，表示阻塞锁定；当timeout等于0时，表示尝试锁定；当timeout大于0时，即表示锁定超时时限�?
*/
int swpa_sem_pend(int* sem, int timeout);

/**
* @brief 解锁信号�?
* @param [in] sem 信号量结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_sem_post(int* sem);

/**
 *@brief 获得信号量个�?
 * @param [in] sem 信号量结构体指针
 @retval 返回信号量个�?
 */
int swpa_sem_value(int* sem);

/**
* @brief 删除信号�?
* @param [in] sem 信号量结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_sem_delete(int* sem);

/**
* @brief 创建互斥�?
* @param [out] mutex 互斥体结构体指针
* @param [in] name 互斥体的名称，不需要跨进程时可填NULL�?
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
* @note 其中的name主要用于跨进程互斥时，以便让另一个进程能通过名称获取到该互斥体，所以请自行保证该名称的唯一性�?
*/
int swpa_mutex_create(int* mutex, const char* name);

/*****************************************************************************
 �� �� ��  : swpa_processmutex_create
 ��������  : ���̼����Ĵ����ӿ�
 �������  : int* mutex �������̼�������صľ��     
             const int mutex_id ���̼����ı�ʶ
 �������  : ��
 �� �� ֵ  : int 0�ɹ� ��0ʧ��
 ���ú���  : 
 ע������  : ��ͬ�Ľ�����mutex_id��Ψһ��
 
 �޸���ʷ      :
  1.��    ��   : 2015��9��4��
    ��    ��   : huangdch
    �޸�����   : �����ɺ���

*****************************************************************************/
int swpa_processmutex_create(int* mutex, const int mutex_id);

/**
* @brief 锁定互斥�?
* @param [in] mutex 互斥体结构体指针
* @param [in] timeout 锁定超时。单位：毫秒
* @retval 0 成功或可�?
* @retval -1 失败或不可锁
* @see swpa_sem.h
* @note 当timeout�?1时，表示阻塞锁定；当timeout等于0时，表示尝试锁定；当timeout大于0时，即表示锁定超时时限�?
*/
int swpa_mutex_lock(int* mutex, int timeout);

/**
* @brief 解锁互斥�?
* @param [in] mutex 互斥体结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_mutex_unlock(int* mutex);

/**
* @brief 删除互斥�?
* @param [in] mutex 互斥体结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_mutex_delete(int* mutex);

/**
* @brief 创建读写�?
* @param [out] rwlock 读写锁结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_rwlock_create(int* rwlock, const char* name);

/**
* @brief 锁定读写�?
* @param [in] rwlock 读写锁结构体指针
* @param [in] timeout 锁定超时。单位：毫秒
* @param [in] is_read_mode 指明是否是以读模式锁定，否则为写模式�?
* @retval 0 成功或可�?
* @retval -1 失败或不可锁
* @see swpa_sem.h
* @note 当timeout�?1时，表示阻塞锁定；当timeout等于0时，表示尝试锁定；当timeout大于0时，即表示锁定超时时限�?
*/
int swpa_rwlock_lock(int* rwlock, int timeout, unsigned int is_read_mode);

/**
* @brief 解锁读写�?
* @param [in] rwlock 读写锁结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_rwlock_unlock(int* rwlock);

/**
* @brief 删除读写�?
* @param [in] rwlock 读写锁结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_sem.h
*/
int swpa_rwlock_delete(int* rwlock);

#ifdef __cplusplus
}
#endif

#endif

