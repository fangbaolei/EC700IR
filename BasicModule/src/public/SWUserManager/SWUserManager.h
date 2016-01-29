/*
 * SWUserManager.h
 *
 *  Created on: 2014年1月6日
 *      Author: qinjj
 */

#ifndef __SW_USERMANAGER_H__
#define __SW_USERMANAGER_H__
#include "SWObject.h"
#include "SWMessage.h"

#define ACOUNT_NUM				8 		///< How many acounts which are stored in system.
#define USER_LEN_MAX			32 		///< Maximum of acount username length.
#define MIN_USER_LEN			4 		///< Maximum of acount username length.
#define PASSWORD_LEN_MAX		16 		///< Maximum of acount password length.
#define MIN_PASSWORD_LEN		4 		///< Maximum of acount password length.
#define PASSWORD_ZERO           0       ///< minimum length of password>

/**
* @brief IPCAM user account data.
*/
typedef struct _Acount_t
{
	CHAR	user[USER_LEN_MAX];			///< username
	char	password[PASSWORD_LEN_MAX];	///< password
	DWORD	authority;					///< user authority

    _Acount_t()
    {
        memset(user, '\0', USER_LEN_MAX);
        memset(password, '\0', PASSWORD_LEN_MAX);
        authority = 0x1;
    };

}Acount_t;

class CSWUserManager: public CSWMessage, public CSWObject {
	CLASSINFO(CSWUserManager, CSWObject)
public:
	CSWUserManager();
	virtual ~CSWUserManager();

protected:
	/**
	* @brief MSG_USER_LOGIN 用户登录校验
	* @param [in] wParam 登录用户结构体指针
	* @param [out] lParam 登录用户权限，-1登录失败
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnUserLogin(WPARAM wParam,LPARAM lParam);

	/**
	* @brief MSG_USER_GETLIST 获取用户信息列表
	* @param [in] wParam 保留
	* @param [out] lParam 登录用户列表结构体数组指针
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnUserGetList(WPARAM wParam,LPARAM lParam);

	/**
	* @brief MSG_USER_GETLIST 获取用户信息列表以及密码
	* @param [in] wParam 保留
	* @param [out] lParam 登录用户列表结构体数组指针
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnGetListUsername_Passwd(WPARAM wParam,LPARAM lParam);


	/**
	* @brief MSG_USER_ADD 增加用户
	* @param [in] wParam 超级用户信息和增加用户信息结构体数组
	* @param [out] lParam 保留
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnUserAdd(WPARAM wParam,LPARAM lParam);

	/**
	* @brief MSG_USER_DEL 删除用户
	* @param [in] wParam 超级用户信息和删除用户信息结构体数组
	* @param [out] lParam 保留
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnUserDell(WPARAM wParam,LPARAM lParam);

	/**
	* @brief MSG_USER_CHANGE 用户变更
	* @param [in] wParam 超级用户信息或当前用户和新用户信息结构体数组
	* @param [out] lParam 保留
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnUserChange(WPARAM wParam,LPARAM lParam);

	/**
	* @brief MSG_USER_RESTORE_DEFAULT 用户信息恢复默认
	* @param [in] wParam 保留
	* @param [out] lParam 保留
	* @return 成功返回S_OK，其他值代表失败
	*/
	HRESULT OnUserRetoreDefault(WPARAM wParam,LPARAM lParam);

	/**
     *@brief 用户管理
     *@param [PVOID] pvBuffer 用户控制参数:1、获取用户列表；2、增加用户；3、修改用户密码；4、删除用户
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnUserManage_ONVIF(PVOID pvBuffer, INT iSize);

	/**
	 *@brief 获取用户密码明文
	 *@param [in] wParam 用户名
	 *@param [out] lParam 用户密码
     *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnUserGetPassword(WPARAM wParam, LPARAM lParam);


	//消息映射宏定义
	SW_BEGIN_MESSAGE_MAP(CSWUserManager, CSWMessage)
	   SW_MESSAGE_HANDLER(MSG_USER_LOGIN, OnUserLogin)
	   SW_MESSAGE_HANDLER(MSG_USER_GETLIST, OnUserGetList)
	   SW_MESSAGE_HANDLER(MSG_USER_ADD, OnUserAdd)
	   SW_MESSAGE_HANDLER(MSG_USER_DEL, OnUserDell)
	   SW_MESSAGE_HANDLER(MSG_USER_CHANGE, OnUserChange)
	   SW_MESSAGE_HANDLER(MSG_USER_RESTORE_DEFAULT, OnUserRetoreDefault)
		SW_MESSAGE_HANDLER(MSG_USER_GETPASSWORD, OnUserGetPassword)
	SW_END_MESSAGE_MAP()


	

	//远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWUserManager, CSWMessage)
		SW_REMOTE_MESSAGE_HANDLER(MSG_USER_MANAGE, OnUserManage_ONVIF)
    SW_END_REMOTE_MESSAGE_MAP();

private:
   	//用户信息数据段头部
#define USER_DATA_SYNC 0x47
#define USER_DATA_VALID	0xD1

   	typedef struct _tUserInfoHead
   	{
		BYTE bSync;		//同步标志
	   	BYTE bValid;		//数据有效标志
	   	WORD	wDataLen;	//有效数据长度
	   	_tUserInfoHead()
	   	{
		   	bSync = USER_DATA_SYNC;
		   	bValid = USER_DATA_VALID;
		   	wDataLen = 0;
		};
   	}tUserInfoHead;

   	Acount_t m_tUserInfo[ACOUNT_NUM];		//用户信息
   	Acount_t m_tSuperUser;				//超级用户，用户名、密码固定，不可修改，不在用户列表中

   	BOOL m_fLoadInfo;

   	HRESULT LoadUserInfo(VOID);			//Read from EEPROM
   	HRESULT CommitUserInfo(VOID);		//Write to EEPROM

   	VOID RestoreDefault(VOID);

	BOOL UserInfoCheck(Acount_t *pCount);
   	BOOL AdminCheck(Acount_t *pCount);
	BOOL SuperUserCheck(Acount_t *pCount);
   
};
REGISTER_CLASS(CSWUserManager)
#endif /* __SW_USERMANAGER_H__ */
