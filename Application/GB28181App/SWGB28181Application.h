/**
* @file
* @brief 
*
*/

#ifndef _CSWGB28181APP_H_
#define _CSWGB28181APP_H_

#include "SWApplication.h"
#include "SWMessage.h"
#include "SWGBRtpThread.h"
#include "SWMutex.h"
#include "SWGB28181Parameter.h"
#include <osip2/osip_mt.h>
#include <eXosip2/eXosip.h>



#define SAFE_STR_PTR(a)				a?a:""
#define _SIP_EXPIRES_TIME			(60*60) //1小时，3600秒
#define _SIP_KEEPALIVE_TIME			(60*1000) //60秒


class CSWGB28181App : public CSWApplication,CSWMessage
{	
    CLASSINFO(CSWGB28181App, CSWApplication)
    public:
        /**
        * @brief
        */
        CSWGB28181App();
    /**
    * @brief
    */
    virtual ~CSWGB28181App();
    virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
    virtual HRESULT ReleaseInstance();

    virtual HRESULT Run();

    static HRESULT Breath(VOID)
    {
        return HeartBeat();
    }
private:
    INT GetVersion(VOID);


protected:
    VOID                        ParamterInit();
    osip_message_t*              build_message();
    GB28181_info                m_gbinfo;
    virtual INT					OnLoop();
    VOID						on_event();
    INT							on_process_message(eXosip_event *event);
    INT							on_process_message_ack(eXosip_event *event);
    INT							on_process_invite(eXosip_event *event);
    INT							on_process_call_close(eXosip_event *event);
    INT							on_process_register_success(eXosip_event * event);
    BOOL						check_connect();
    VOID						check_keepalive();
    INT                         do_RepeatRegister();

    INT							do_ptz(LPCSTR  ptzcmd);
    INT							do_register();
    INT							do_unregister();
    INT							do_keepalive();
    INT							do_preview(eXosip_event *event,LPCSTR ssrc);
    INT							do_catalog(eXosip_event *event,LPCSTR  sn);
    INT                         do_guard(LPCSTR  guardcmd,LPCSTR sn);
    INT                         do_alarmreset(LPCSTR  guardcmd,LPCSTR sn);
    INT                         do_recordcmd(LPCSTR recordcmd,LPCSTR sn);
    INT                         do_device_info(LPCSTR sn);
    INT                         ans_devicecontrol_response(LPCSTR state,LPCSTR sn);// state ( OK or ERROR )
    INT							ans_devicestatus__response(LPCSTR sn);
    INT							ans_VideoParamConfig__response(LPCSTR sn);
    INT							ans_AudioParamConfig__response(LPCSTR sn);
    INT							ans_BasicParam__response(LPCSTR sn);
    INT                         ans_deviceRecordInfo__response(LPCSTR sn,CSWDateTime* pdtStartTime,CSWDateTime* pdtEndTime);
    INT                         send_alarm(LPCSTR szAlarmTime,LPCSTR szAlarmMsg);
    BOOL						is_alive(unsigned long timeout);
    BOOL                        is_register_alive(unsigned long timeout);
    VOID						on_refresh_alive();
    INT						on_close_preview();
    INT             on_process_message_RTSP(eXosip_event *event);
    VOID 						SendVideoEndMessage();
    INT							HexStringToBytes(LPCSTR strCmd, BYTE* szCmd, INT iLen);

protected:

    HRESULT OnSendAlarm         (PVOID pvBuffer, INT iSize);
    HRESULT OnParamterChange    (PVOID pvBuffer, INT iSize);
    HRESULT OnGB28181Quit(PVOID pvBuffer, INT iSize);
    //远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWGB28181App, CSWMessage)
    SW_REMOTE_MESSAGE_HANDLER(MSG_GB28181_SEND_ALARM,           OnSendAlarm)
    SW_REMOTE_MESSAGE_HANDLER(MSG_GB28181_PARAMTER_CHANGE_ALARM,OnParamterChange)
    SW_REMOTE_MESSAGE_HANDLER(MSG_GB28181_QUIT,OnGB28181Quit)
    SW_END_REMOTE_MESSAGE_MAP()
	static LPVOID SendVideoEndMessageThread(LPVOID pArg);


public:
    INT							m_status;					//0:未启动，1:启动
    INT                         m_reset;
    char            m_call_id_number[128];
    INT			  m_quit;
    INT           m_gb28181_refresh;
    
protected:
	BOOL 						t_thread;				//发送结束消息线程标志位
    unsigned long				m_keepalive_last_ticket;
    unsigned long				m_register_last_ticket;
    class SWGBRtpThread         m_realtimethread;
    class SWGBRtpThread         m_historythread;
    INT                         m_regid;
    CSWMutex                    m_cMutexLock;
    BOOL                        m_ParamGet;//标记是否从LRPAPP获取到参数
    BOOL                        m_bGuard;
    BOOL                        m_bAlarmReset;
    CSWList<client_info *>      m_Client_info_list;
    GB28181_Control_Param       Control_param;          //用于记录历史视频播放的开始时间和结束时间
    SWPA_DATETIME_TM            m_pause_time;           //用于记录历史视频播放暂停的时间
    FLOAT                       m_speed;                //记录暂停前的播放速度
    

};

#endif

