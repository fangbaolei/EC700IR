/**
* @file
* @brief 
*
*/

#ifndef _SW_ONVIF_APPLICATION_H_
#define _SW_ONVIF_APPLICATION_H_

#include "SWApplication.h"
#include "SWThread.h"
#include "SWMessage.h"
#include "onvifLib.h"


//ONVIF 参数相关
typedef struct _ONVIF_PARAM_INFO
{
     int iOnvifEnable;
     int iAuthenticateEnable;                                   //监权 0 不使能  1 使能
     _ONVIF_PARAM_INFO()
     {
          iOnvifEnable = 0;
          iAuthenticateEnable = 0;
     }
}ONVIF_PARAM_INFO;


class CSWOnvifApp : public CSWApplication,CSWMessage
{
	CLASSINFO(CSWOnvifApp, CSWApplication);
public:
	
	/**
	* @brief 
	*/
	CSWOnvifApp();

	/**
	* @brief 
	*/
	virtual ~CSWOnvifApp();

	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	virtual HRESULT ReleaseInstance();

	virtual HRESULT Run();
	
	static HRESULT Breath(VOID)
	{
		return HeartBeat();
	}

protected:

	/**
     *@brief 获取球机云台配置
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT GetPTZInfo(VOID);

	/**
     *@brief 设置球机云台配置
     *@param [PVOID] pvBuffer PTZ控制参数
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnSetPTZInfo(PVOID pvBuffer, INT iSize);

	/**
     *@brief 发送用户名密码
     *@param [PVOID] pvBuffer 用户名密码
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnSetUSERInfo(PVOID pvBuffer, INT iSize);

	/**
     *@brief 获取用户名密码
     *@param [PVOID] pvBuffer 用户名密码
     *@param [PVOID] iSize 用户个数
     *@return 成功返回S_OK,其他值为错误代码
     */
	static int OnGetUSERInfo(void* pSize, void* pvArg);

	HRESULT OnSetEnableInfo(PVOID pvBuffer, INT iSize);

    HRESULT OnOnvifRestart(PVOID pvBuffer, INT iSize);


	
	//远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWOnvifApp, CSWMessage)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REMOTE_SET_PTZ_INFO, OnSetPTZInfo)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REMOTE_SET_USER_INFO, OnSetUSERInfo)
		SW_REMOTE_MESSAGE_HANDLER(MSG_ONVIF_SET_PARAM, OnSetEnableInfo)
		SW_REMOTE_MESSAGE_HANDLER(MSG_ONVIF_APP_RESTART, OnOnvifRestart)
    SW_END_REMOTE_MESSAGE_MAP();

	
private:	
	INT GetVersion(VOID);

	static INT PTZControlCallback(const int iCmdID, void* pvArg);
	static INT UserManageCallback(const int iCmdID, void* pvArg);

	ONVIF_PTZ_PARAM m_sPTZParam;
	
	static UserArray_t g_psUserlist[8];
	static int	m_UserSize;
	int m_fEnable;
	int m_fAuthentEnable;

	bool m_fIsGetParam;

	CSWMutex m_cMutex;


};





#endif //_SW_ONVIF_APPLICATION_H_

