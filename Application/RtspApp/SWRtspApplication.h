/**
* @file
* @brief 
*
*/

#ifndef _SW_RTSP_APPLICATION_H_
#define _SW_RTSP_APPLICATION_H_

#include "SWApplication.h"
#include "SWThread.h"
#include "SWOnvifRtspParameter.h"
#include "SWMessage.h"

class CSWRtspApp : public CSWApplication, CSWMessage
{
	CLASSINFO(CSWRtspApp, CSWApplication);
public:
	
	/**
	* @brief 
	*/
	CSWRtspApp();

	/**
	* @brief 
	*/
	virtual ~CSWRtspApp();

	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	virtual HRESULT ReleaseInstance();

	virtual HRESULT Run();
	
	static HRESULT Breath(VOID)
	{
		return HeartBeat();
	}

protected:

	HRESULT OnRtspGetParam(PVOID pvBuffer, INT iSize);
    HRESULT OnRtspRestart(PVOID pvBuffer, INT iSize);
    //远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWRtspApp, CSWMessage)
	SW_REMOTE_MESSAGE_HANDLER(MSG_RTSP_SEND_PARAM, OnRtspGetParam)
	SW_REMOTE_MESSAGE_HANDLER(MSG_RTSP_APP_RESTART, OnRtspRestart)
    SW_END_REMOTE_MESSAGE_MAP()

private:	
	BOOL m_fIsGetParam;
	RTSP_PARAM_INFO m_cRtspParamInfo;
	INT GetVersion(VOID);
	
};


//extern CSWRtspApp theApp;


#endif //_SW_RTSP_APPLICATION_H_

