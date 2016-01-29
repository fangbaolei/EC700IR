
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "eXosip2/eXosip.h"
#include <osipparser2/osip_parser.h>
#include "SWFC.h"
#include "swpa.h"
#include "SWGB28181Application.h"
#include <errno.h>


#define PROG_VER  "1.0"
#define UA_STRING "XLW v" PROG_VER

CREATE_PROCESS(CSWGB28181App);


static void _print_media(sdp_media_t * medi)
{
    SW_TRACE_DEBUG("============== media =============\n");
    SW_TRACE_DEBUG("m_media='%s'\n",medi->m_media);
    SW_TRACE_DEBUG("m_port='%s'\n",medi->m_port);
    SW_TRACE_DEBUG("m_number_of_port='%s'\n",medi->m_number_of_port);
    SW_TRACE_DEBUG("m_proto='%s'\n",medi->m_proto);
    SW_TRACE_DEBUG("m_payloads='%s'\n",osip_list_get(&medi->m_payloads,0));
    SW_TRACE_DEBUG("i_info='%s\n",medi->i_info);
    sdp_connection_t * sc=(sdp_connection_t *)osip_list_get(&medi->c_connections,0);
    if(sc)
        SW_TRACE_DEBUG("c_connections='%s#%s'\n",sc->c_addrtype,sc->c_addr);
    sdp_bandwidth_t * sb=(sdp_bandwidth_t *)osip_list_get(&medi->b_bandwidths,0);
    if(sb)
        SW_TRACE_DEBUG("b_bandwidths='%s#%s'\n",sb->b_bwtype,sb->b_bandwidth);
    if(medi->k_key)
        SW_TRACE_DEBUG("k_key='%s#%s\n",medi->k_key->k_keytype,medi->k_key->k_keydata);
    sdp_attribute_t * sa=(sdp_attribute_t *)osip_list_get(&medi->a_attributes,0);
    if(sa)
        SW_TRACE_DEBUG("a_att_field='%s',a_att_value='%s'\n",sa->a_att_field,sa->a_att_value);
}

static void _print_sdp(sdp_message * sdp)
{
    SW_TRACE_DEBUG("============== sdp =============\n");
    SW_TRACE_DEBUG("s='%s'\n",sdp->s_name);
    SW_TRACE_DEBUG("i_info=%s\n",sdp->i_info);
    sdp_attribute_t * sa=(sdp_attribute_t *)osip_list_get(&sdp->a_attributes,0);
    if(sa)
        SW_TRACE_DEBUG("a_att_field='%s',a_att_value='%s'\n",sa->a_att_field,sa->a_att_value);
}


static void printf_osip_message(osip_message_t *message)
{
    static unsigned long i = 0;
    CHAR *buf;
    if(message)
    {
        osip_message_to_str(message,&buf,&(message->message_length));
        SW_TRACE_DEBUG("osip_message：%d\n%s\n",i,buf);
        osip_free(buf);
        i++;
    }
}


CSWGB28181App::CSWGB28181App()
	:CSWMessage(MSG_GB28181_BEGIN, MSG_GB28181_END)
{
    SW_TRACE_DEBUG("%s %d\n",__FUNCTION__,__LINE__);
    m_regid = 0;
    m_status=0;

    m_keepalive_last_ticket = 0;
    m_register_last_ticket = 0;
    m_ParamGet = FALSE;
    m_bGuard = FALSE;
    m_bAlarmReset = FALSE;
    m_quit = 0;
	m_speed = 1.0;
	m_gb28181_refresh= FALSE;
	t_thread = TRUE;
}

CSWGB28181App::~CSWGB28181App()
{
}


HRESULT CSWGB28181App::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
    if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
    {
        SW_TRACE_DEBUG("Err: failed to init CSWGB28181App\n");
        return E_FAIL;
    }

    BOOL fIsReady = FALSE;

    CHAR szIP[32] = {0};
    CHAR szMask[32] = {0};
    CHAR szGateway[32] = {0};
    CHAR szMac[32] = {0};

    while (!IsExited())
    {
        if( !fIsReady )
        {
            HeartBeat();
            if( 0 == swpa_tcpip_getinfo("eth0", szIP, 32, szMask, 32, szGateway, 32, szMac, 32) && 0 != swpa_strcmp("0.0.0.0", szIP) )
            {
                SW_TRACE_DEBUG("%s:CSWGB28181App get ip:%s.", __FUNCTION__,szIP);

				do 
				{
					HeartBeat();

					swpa_thread_sleep_ms(2000);
	                if(SUCCEEDED(SendRemoteMessage(MSG_APP_GET_28181_PARAM,NULL,0)))
	                {
	                    SW_TRACE_DEBUG("%s:Send MSG_APP_GET_28181_PARAM -- ok\n", __FUNCTION__);
	                }
	                else
	                {
	                    SW_TRACE_DEBUG("%s:Send MSG_APP_GET_28181_PARAM -- failed\n", __FUNCTION__);
	                }
					
					swpa_thread_sleep_ms(10000);
				} while (!IsExited() && !m_ParamGet);

				if (IsExited())
				{
					break;
				}
				
                

				GB28181_info info;
				m_cMutexLock.Lock();
				swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
				//lzl:获取GB28181的开关值
				SW_TRACE_DEBUG("%s:info.enable= %d In InitInstance \n",__FUNCTION__,info.enable);
				if(info.enable==0)
				{
					SW_TRACE_DEBUG("%s:info.enable= %d GB28181 Switch is off \n",__FUNCTION__,info.enable);
					//return S_FALSE;
					swpa_thread_sleep_ms(2000);
					continue;
				}
                 //lzl:获取GB28181的开关值
                m_cMutexLock.Unlock();

				if(eXosip_init()!=0)				//无论开关是否打开，执行成功之后就不再执行
                {
                    SW_TRACE_DEBUG("eXosip init fail\n");
                    continue;
                }

                if(eXosip_listen_addr(IPPROTO_UDP, NULL,swpa_atoi(info.ipc_port), AF_INET, 0)!=0)
                {
                    SW_TRACE_DEBUG("eXosip listen fail\n");
                    eXosip_quit();
                    continue;
                }
                
                int value = 0;
                eXosip_set_option(EXOSIP_OPT_UDP_KEEP_ALIVE,&value);
                eXosip_set_user_agent(UA_STRING);
                ParamterInit();
                fIsReady = TRUE;
                break;
            }
            else
            {
                SW_TRACE_DEBUG("loop CSWGB28181App get ip:%s.", szIP);
            }
            HeartBeat();
            swpa_thread_sleep_ms(1000);
        }
    }
    SW_TRACE_DEBUG("CSWGB28181App InitInstance success\n");

    return S_OK;
}

HRESULT CSWGB28181App::ReleaseInstance()
{
	//lzl:测试是否进入GB28181的ReleaseInstance
	SW_TRACE_NORMAL("Aready In GB28181 ReleaseInstance!\n");

    if(FAILED(CSWApplication::ReleaseInstance()))
    {
        SW_TRACE_DEBUG("Err: failed to release CSWGB28181App\n");
        return E_FAIL;
    }

    eXosip_quit();
    INT iRet = eXosip_execute();
    if (iRet != 0)
    {
        SW_TRACE_DEBUG("Err: failed to stop CSWGB28181App Service, errcode=%d\n", iRet);
        return E_FAIL;
    }
    else
    {
        SW_TRACE_DEBUG("Info: CSWGB28181App Service stopped!\n");
        return S_OK;
    }

    return S_OK;
}

HRESULT CSWGB28181App::Run()
{
    while (!IsExited() && !m_quit)
    {
        OnLoop();
        HeartBeat();
        swpa_thread_sleep_ms(2000);
    }
    return S_OK;
}

/**
 *@brief 取得代码的版本号
 *@return 返回版本号,失败返回-1
 */
INT CSWGB28181App::GetVersion(VOID)
{
    return CSWApplication::GetVersion();
}

VOID CSWGB28181App::ParamterInit()
{
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    m_realtimethread.set_FPS(info.mediaSend_FPS);
  	m_realtimethread.set_Size(info.mediaSend_wight,info.mediaSend_height);
    m_realtimethread.set_PackMode(1);

	m_historythread.set_FPS(info.mediaSend_FPS);
  	m_historythread.set_Size(info.mediaSend_wight,info.mediaSend_height);
    m_historythread.set_PackMode(1);
	
    m_reset = 0;

}

osip_message_t *CSWGB28181App::build_message()
{
    osip_message_t *message= NULL;
    CHAR fromuser[256];
    CHAR proxy[256];
    CHAR route[256];
    memset(&route,0,sizeof(route));

    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    /*    sSW_TRACE_DEBUG(fromuser,"sip:%s@%s",ipc_id,ipc_ip);
    sSW_TRACE_DEBUG(proxy,"sip:%s:%s",server_ip,server_ip);		*/	//服务器URL
    swpa_sprintf(fromuser,"sip:%s@%s",info.ipc_id,info.ipc_region);
    swpa_sprintf(proxy,"sip:%s@%s",info.server_id,info.server_region);
    swpa_sprintf(route,"<sip:%s:%s;lr>",info.server_ip,info.server_port);
    INT ret= eXosip_message_build_request(&message,"MESSAGE",proxy,fromuser,route);

    SW_TRACE_DEBUG("%s:ret is %d \n",__FUNCTION__,ret);

    return message;

}

INT CSWGB28181App::do_register()
{
    if(m_status == 1)
        return 0;


    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    eXosip_register_remove(m_regid);
    CHAR fromuser[256];
    CHAR proxy[256];
    CHAR route[256];
    SW_TRACE_DEBUG( "%s:ipc_id= %s ipc_region = %s\n",__FUNCTION__,info.ipc_id,info.ipc_region);
    SW_TRACE_DEBUG( "%s:server_id = %s server_region = %s server_ip=%s server_port = %s\n",__FUNCTION__,info.server_id,
																	info.server_region,info.server_ip,info.server_port);
    SW_TRACE_DEBUG( "%s:ipc_username = %s ipc_pwd = %s\n",__FUNCTION__,info.ipc_username,info.ipc_pwd);

    swpa_sprintf(fromuser,"sip:%s@%s",  info.ipc_id,    info.ipc_region);
    swpa_sprintf(proxy,"sip:%s@%s",     info.server_id, info.server_region);
    swpa_sprintf(route,"<sip:%s:%s;lr>",info.server_ip, info.server_port);
    eXosip_clear_authentication_info();
    eXosip_add_authentication_info(info.ipc_username, info.ipc_username,info.ipc_pwd, NULL, NULL);
    osip_message_t *reg = NULL;
    m_regid =eXosip_register_build_initial_register(fromuser, proxy, NULL,_SIP_EXPIRES_TIME,&reg);
    osip_message_set_route(reg,route);
    if(eXosip_register_send_register(m_regid, reg)!=0)
    {
    	SW_TRACE_DEBUG( "do_register failed \n");
    	//Sleep(30000);//注册失败，则休眠30秒
        return -1;
    }
    SW_TRACE_DEBUG( "do_register success \n");
    m_register_last_ticket=CSWDateTime::GetSystemTick();
    return 0;
}

INT CSWGB28181App::do_unregister()
{
    if(m_status==0)
        return 0;
    SW_TRACE_DEBUG( "%s:start... \n",__FUNCTION__);
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    eXosip_register_remove(m_regid);
    CHAR fromuser[256];
    CHAR proxy[256];
    CHAR route[256];
    swpa_sprintf(fromuser,"sip:%s@%s",  info.ipc_id,    info.ipc_region);
    swpa_sprintf(proxy,"sip:%s@%s",     info.server_id, info.server_region);
    swpa_sprintf(route,"<sip:%s:%s;lr>",info.server_ip, info.server_port);
    eXosip_clear_authentication_info();
    eXosip_add_authentication_info(info.ipc_username, info.ipc_username,info.ipc_pwd, NULL, NULL);
    osip_message_t *reg = NULL;
    m_regid = eXosip_register_build_initial_register(fromuser, proxy, NULL,0,&reg);
    osip_message_set_route(reg,route);
    if(eXosip_register_send_register(m_regid, reg)!=0)
    {
    	SW_TRACE_DEBUG("do_unregister failed!\n");
        return -1;
    }
    SW_TRACE_DEBUG( "do_unregister success \n");
    m_reset = 0;
    m_status = 0;
    return 0;
}

INT CSWGB28181App::do_preview(eXosip_event *event,LPCSTR  ssrc)
{
    SW_TRACE_DEBUG( "%s:start... \n",__FUNCTION__);
    osip_message_t *answer = NULL;
    INT ret = eXosip_call_build_answer (event->tid, 200, &answer);
    if (ret == 0)
    {
        CHAR ss[128];
        swpa_sprintf(ss, "");
        if(swpa_strlen(ssrc)>0)
        {
            m_historythread.set_ssrc(ssrc);
			m_realtimethread.set_ssrc(ssrc);
        }
        GB28181_info info;
        m_cMutexLock.Lock();
        swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
        m_cMutexLock.Unlock();

        CHAR szIP[20];
        if(eXosip_guess_localip(AF_INET, szIP, 20)!=0)
        {
            return -1;
        }

        CHAR tmp[4096];
        swpa_snprintf (tmp, 4096,
                       "v=0\r\n"
                       "o=%s 0 0 IN IP4 %s\r\n"
                       "s=XLIPCAM\r\n"
                       "c=IN IP4 %s\r\n"
                       "t=0 0\r\n"
                       "m=video 0 RTP/AVP 96\r\n"
                       "a=rtpmap:96 PS/90000\r\n"
                       "y=%s\r\n",info.ipc_id,szIP,szIP,ss);
        osip_message_set_body (answer, tmp, swpa_strlen(tmp));
        osip_message_set_content_type (answer,"application/sdp");
        ret = eXosip_call_send_answer (event->tid, 200, answer);
		if(ret == 0 )
		{
			SW_TRACE_DEBUG("%s: anwser 200 OK  \n",__FUNCTION__);
		}

    }
	
    return ret?-1:0;
}


INT CSWGB28181App::do_catalog(eXosip_event *event,LPCSTR sn)
{
    osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("%s:message = NULL\n",__FUNCTION__);
        return -1;
    }
    CHAR szIP[20];
    if(eXosip_guess_localip(AF_INET, szIP, 20)!=0)
    {
        return -1;
    }

    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    CHAR tmp[4096];
    swpa_memset(tmp,0,4096);
    swpa_snprintf(tmp, 4096, "<?xml version=\"1.0\"?>\r\n"
                  "<Response>\r\n"
                  "<CmdType>Catalog</CmdType>\r\n"/*命令类型*/
                  "<SN>%s</SN>\r\n"/*命令序列号*/
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<SumNum>2</SumNum>\r\n"/*查询结果总数*/
                  "<DeviceList Num=\"1\">\r\n"/*设备目录项列表*/
                  "<Item>\r\n"
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<Name>%s</Name>\r\n"/*设备/区域/系统名称*/
                  "<Manufacturer>%s</Manufacturer>\r\n"/*设备厂商*/
                  "<Model>%s</Model>\r\n"/*设备型号*/
                  "<Owner>%s</Owner>\r\n"/*设备归属*/
                  "<CivilCode>%s</CivilCode>\r\n"/*行政区域*/
                  "<Block>%s</Block>\r\n"/*警区*/
                  "<Address>%s</Address>\r\n"/*安装地址*/
                  "<Parental>0</Parental>\r\n"/*是否有子设备*/
                  "<SafetyWay>0</SafetyWay>\r\n"/*信令安全模式/0为不采用/2为S/MIME签名方式/3为S/MIME加密签名同时采用方式/4为数字摘要方式*/
                  "<RegisterWay>1</RegisterWay>\r\n"/*注册方式/1为符合sip3261标准的认证注册模式/2为基于口令的双向认证注册模式/3为基于数字证书的双向认证注册模式*/
                  "<Secrecy>0</Secrecy>\r\n"/*保密属性/0为不涉密/1为涉密*/
                  "<IPAddress>%s</IPAddress>\r\n"/*设备/区域/系统IP地址*/
                  "<Port>%s</Port>\r\n"/*设备/区域/系统端口*/
                  "<Password>%s</Password>\r\n"/*设备口令*/
                  "<Status>ON</Status>\r\n"/*设备状态*/
                  "<Longitude>%s</Longitude>\r\n"/*经度*/
                  "<Latitude>%s</Latitude>\r\n"/*纬度*/
                  "</Item>\r\n"
                  "</DeviceList>\r\n"
                  "</Response>\r\n",
                  sn,
                  info.ipc_id,

                  info.ipc_id,
                  info.szDeviceName,
                  "北京信路威科技股份有限公司",//info.szManufacturer,
                  info.szModel,
                  info.szOwner,
                  info.szCivilCode,
                  info.szBlock,
                  info.szAddress,
                  szIP,
                  info.ipc_port,
                  info.ipc_pwd,
                  info.szLongitude,
                  info.szLatitude
                  );


    osip_message_set_body (message, tmp, swpa_strlen(tmp));
    osip_message_set_content_type (message,"Application/MANSCDP+xml");

    INT ret = eXosip_message_send_request(message); 
    
    message = build_message();
    swpa_memset(tmp,0,4096);
    swpa_snprintf(tmp, 4096, "<?xml version=\"1.0\"?>\r\n"
                  "<Response>\r\n"
                  "<CmdType>Catalog</CmdType>\r\n"/*命令类型*/
                  "<SN>%s</SN>\r\n"/*命令序列号*/
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<SumNum>2</SumNum>\r\n"/*查询结果总数*/
                  "<DeviceList Num=\"1\">\r\n"/*设备目录项列表*/
                  "<Item>\r\n"
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<Name>%s</Name>\r\n"/*设备/区域/系统名称*/
                  "<Manufacturer>%s</Manufacturer>\r\n"/*设备厂商*/
                  "<Model>%s</Model>\r\n"/*设备型号*/
                  "<Owner>%s</Owner>\r\n"/*设备归属*/
                  "<CivilCode>%s</CivilCode>\r\n"/*行政区域*/
                  "<Block>%s</Block>\r\n"/*警区*/
                  "<Address>%s</Address>\r\n"/*安装地址*/
                  "<Parental>0</Parental>\r\n"/*是否有子设备*/
                  "<SafetyWay>0</SafetyWay>\r\n"/*信令安全模式/0为不采用/2为S/MIME签名方式/3为S/MIME加密签名同时采用方式/4为数字摘要方式*/
                  "<RegisterWay>1</RegisterWay>\r\n"/*注册方式/1为符合sip3261标准的认证注册模式/2为基于口令的双向认证注册模式/3为基于数字证书的双向认证注册模式*/
                  "<Secrecy>0</Secrecy>\r\n"/*保密属性/0为不涉密/1为涉密*/
                  "<IPAddress>%s</IPAddress>\r\n"/*设备/区域/系统IP地址*/
                  "<Port>%s</Port>\r\n"/*设备/区域/系统端口*/
                  "<Password>%s</Password>\r\n"/*设备口令*/
                  "<Status>ON</Status>\r\n"/*设备状态*/
                  "<Longitude>%s</Longitude>\r\n"/*经度*/
                  "<Latitude>%s</Latitude>\r\n"/*纬度*/
                  "</Item>\r\n"
                  "</DeviceList>\r\n"
                  "</Response>\r\n",
                  sn,
                  info.ipc_id,

                  info.ipc_alarm,
                  info.ipc_alarm,
                  "北京信路威科技股份有限公司",//info.szManufacturer,
                  info.szModel,
                  info.szOwner,
                  info.szCivilCode,
                  info.szBlock,
                  info.szAddress,
                  szIP,
                  info.ipc_port,
                  info.ipc_pwd,
                  info.szLongitude,
                  info.szLatitude
                  );
    osip_message_set_body (message, tmp, swpa_strlen(tmp));
    osip_message_set_content_type (message,"Application/MANSCDP+xml");

    INT ret0 = eXosip_message_send_request(message);

    if( ret == 0 && ret0 == 0 )
    {
    	SW_TRACE_DEBUG("%s:message send success\n",__FUNCTION__);
    	return 0;
    } 
    else
    {
    	SW_TRACE_DEBUG("%s:message send Failed\n",__FUNCTION__);
    	return -1;
    }
}

INT CSWGB28181App::send_alarm(LPCSTR szAlarmTime,LPCSTR szAlarmMsg)
{
    SW_TRACE_DEBUG("%s: AlarmTime = %s,AlarmMsg = %s\n",__FUNCTION__,szAlarmTime,szAlarmMsg);
    if(m_status==0)
        return 0;

    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();
    if(m_bGuard == FALSE)//非补防状态不发送告警
	{
		SW_TRACE_DEBUG("%s:m_bGuard=%d",__FUNCTION__,m_bGuard);
        return 0;
	}

    osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("%s:message = NULL\n",__FUNCTION__);
        return -1;
    }


    CHAR tmp[4096];
    swpa_snprintf (tmp, 4096,
                   "<?xml version=\"1.0\"?>\n"
                   "<Notify>\n"
                   "<CmdType>Alarm</CmdType>\n"
                   "<SN>1</SN>\n"
                   "<DeviceID>%s</DeviceID>\n"
                   "<AlarmPriority>1</AlarmPriority>\n"//1为 一级警情，2为二级警情，3为三级警情，4为四级警情
                   "<AlarmTime>%s</AlarmTime>\n"
                   "<AlarmMethod>2</AlarmMethod>\n"//1为电话报警，2为设备报警，3为短信报警，4为GPS报警，5为视频报警，6为设备故障报警，7其它报警
                   //"<AlarmDescription>%s<AlarmDescription>\n" //报警内容描述（可选）
                   "</Notify>\n",info.ipc_alarm,szAlarmTime);//,szAlarmMsg);


    osip_message_set_body (message, tmp, swpa_strlen(tmp));
    osip_message_set_content_type (message,"Application/MANSCDP+xml");

    int ret = eXosip_message_send_request(message);

    if(ret != 0)
    {
        SW_TRACE_DEBUG("%s:send message failed",__FUNCTION__);
        return ret;
    }
    SW_TRACE_DEBUG("%s:告警发送1！%s %s [before printf_osip_message]\n",__FUNCTION__,szAlarmTime,szAlarmMsg);
    printf_osip_message(message);
    m_bAlarmReset = TRUE;
    return 0;

}

INT CSWGB28181App::do_guard(LPCSTR guardcmd,LPCSTR sn)
{
    if(swpa_strcmp(guardcmd,"SetGuard") == 0)
    {
        SW_TRACE_DEBUG("布防\n");
        m_bGuard = TRUE;
    }
    else if(swpa_strcmp(guardcmd,"ResetGuard") == 0)
    {
        SW_TRACE_DEBUG("撤防\n");
        m_bGuard = FALSE;
    }
    else
    {
        SW_TRACE_DEBUG("ERROR!LINE=%d,str=%s\n",__LINE__,guardcmd);
        return -1;
    }
	
    return  ans_devicecontrol_response("OK",sn);
}

INT CSWGB28181App::do_alarmreset(LPCSTR alarmcmd,LPCSTR sn)
{
    SW_TRACE_DEBUG("告警复位设置\n");
    //


    ///
    return ans_devicecontrol_response("OK",sn);
}

INT CSWGB28181App::do_recordcmd(LPCSTR recordcmd,LPCSTR sn)
{
    if(swpa_strcmp(recordcmd,"Record") == 0)
    { 
        //开始手动录像
        SW_TRACE_DEBUG("%s:start manual Record \n",__FUNCTION__);
        DWORD dwrecord = 1;  
        SendRemoteMessage(MSG_H264HDD_FILTER_GB28181_VIDEOSAVING_ENABLE,&dwrecord,sizeof(DWORD));
    }
    else if(swpa_strcmp(recordcmd,"StopRecord") == 0)
    {
        //停止手动录像
        SW_TRACE_DEBUG("%s:stop manual Record \n",__FUNCTION__);
        DWORD dwrecord = 0;  
        SendRemoteMessage(MSG_H264HDD_FILTER_GB28181_VIDEOSAVING_ENABLE,&dwrecord,sizeof(DWORD));
    }
    else
    {
        SW_TRACE_DEBUG("%s:ERROR!,recordcmd=%s\n",__FUNCTION__,recordcmd);
        return -1;
    }
    
    return ans_devicecontrol_response("OK",sn);
}

INT CSWGB28181App::do_device_info(LPCSTR sn)
{
    osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("message = NULL\n");
        return -1;
    }
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    CHAR tmp[4096];
    swpa_snprintf (tmp, 4096,
                   "<?xml version=\"1.0\"?>\n"
                   "<Response>\n"
                   "<CmdType>DeviceInfo</CmdType>\n"
                   "<SN>%s</SN>\n"
                   "<DeviceID>%s</DeviceID>\n"
                   "<Result>OK</Result>\n"
                   "<DeviceType>IPC</DeviceType>\n" //DeviceType  demo中含有，文档未见说明
                   "<Manufacturer>%s</Manufacturer>\n"//设备生产商（可选）
                   "<Model>%s</Model>\n"//设备型号（可选）!!!!???
                   "<Firmware>%s</Firmware>\n" //设备固件版本（可选）
                   "<Channel>%d</Channel>\n" //视频输入通道数（可选）
                   "</Response>\n",sn,
                   info.ipc_id,
                   "北京信路威科技股份有限公司",//info.szManufacturer,
                   info.szModel,
                   info.szFirmware,
                   1);

    osip_message_set_body (message, tmp, swpa_strlen(tmp));
    osip_message_set_content_type (message,"Application/MANSCDP+xml");
    SW_TRACE_DEBUG("%s:[before printf_osip_message]\n",__FUNCTION__);
    printf_osip_message(message);
    return eXosip_message_send_request(message);
}

INT CSWGB28181App::ans_devicecontrol_response(LPCSTR state,LPCSTR sn)//OK or ERROR
{
    if(swpa_strcmp(state,"OK") || swpa_strcmp(state,"ERROR"))
    {
        osip_message_t *message = build_message();
        if(message == NULL)
        {
            SW_TRACE_DEBUG("%s:message = NULL\n",__FUNCTION__);
            return -1;
        }
        GB28181_info info;
        m_cMutexLock.Lock();
        swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
        m_cMutexLock.Unlock();

        CHAR tmp[4096];
        swpa_snprintf (tmp, 4096,
                       "<?xml version=\"1.0\"?>\n"
                       "<Response>\n"
                       "<CmdType>DeviceControl</CmdType>\n"
                       "<SN>%s</SN>\n"
                       "<DeviceID>%s</DeviceID>\n"
                       "<Result>%s</Result>\n"
                       "</Response>\n",sn,info.ipc_alarm,state);
        osip_message_set_body (message, tmp, swpa_strlen(tmp));
        osip_message_set_content_type (message,"Application/MANSCDP+xml");

        INT ret = eXosip_message_send_request(message);
    	SW_TRACE_DEBUG("%s:[before printf_osip_message]\n",__FUNCTION__);
        printf_osip_message(message);
        if(ret != 0)
        {
            SW_TRACE_DEBUG("%s:send message failed",__FUNCTION__);
            return ret;
        }
        m_bAlarmReset = FALSE;
        return 0;
    }
    return -1;

}

INT	CSWGB28181App::ans_devicestatus__response(LPCSTR sn)
{

    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
    dt.GetTime(&tm);


    CHAR time[64];
    swpa_sprintf(time,"%d-%02d-%02dT%02d:%02d:%02d.%03d",tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec,tm.msec);

    SW_TRACE_DEBUG("%s:now time = %s\n",__FUNCTION__,time);


    osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("message = NULL\n");
        return -1;
    }
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    CHAR tmp[4096];
    swpa_snprintf(tmp, 4096, "<?xml version=\"1.0\"?>\r\n"
                  "<Response>\r\n"
                  "<CmdType>DeviceStatus</CmdType>\r\n"/*命令类型*/
                  "<SN>%s</SN>\r\n"/*命令序列号*/
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<Result>OK</Result>\r\n"/*查询结果标志*/
                  "<Online>ONLINE</Online>\r\n"/*是否在线/ONLINE/OFFLINE*/
                  "<Status>OK</Status>\r\n"/*是否正常工作*/
                  "<Encode>%s</Encode>\r\n"/*是否编码*/
                  "<Record>%s</Record>\r\n"/*是否录像*/
                  "<DeviceTime>%s</DeviceTime>\r\n"/*设备时间和日期*/
                  "<Alarmstatus Num=\"1\">\r\n"/*报警设备状态列表*/
                  "<Item>\r\n"
                  "<DeviceID>%s</DeviceID>\r\n"/*报警设备编码*/
                  "<DutyStatus>%s</DutyStatus>\r\n"/*报警设备状态*/
                  "</Item>\r\n"
                  "</Alarmstatus>\r\n"
                  "</Response>\r\n",
                  sn,
                  info.ipc_id,
                  info.device_encode,
                  info.device_record,
                  time,
                  info.ipc_id,
                  m_bAlarmReset?"ALARM":(m_bGuard?"ONDUTY":"OFFDUTY"));
    osip_message_set_body (message, tmp, swpa_strlen(tmp));

    osip_message_set_content_type (message,"Application/MANSCDP+xml");
    SW_TRACE_DEBUG("%s:[before printf_osip_message]\n",__FUNCTION__);
    printf_osip_message(message);
    INT ret = eXosip_message_send_request(message);
    if(ret != 0)
    {
        SW_TRACE_DEBUG("%s:send message failed",__FUNCTION__);
        return ret;
    }
    return 0;

}

INT	CSWGB28181App::ans_VideoParamConfig__response(LPCSTR sn)
{
	osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("message = NULL\n");
        return -1;
    }
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();
    
     CHAR tmp[4096];
    swpa_snprintf(tmp, 4096, "<?xml version=\"1.0\" encoding=\"GB2312\" ?>\r\n"
                  "<Response>\r\n"
                  "<CmdType>ConfigDownload</CmdType>\r\n"/*命令类型*/
                  "<SN>%s</SN>\r\n"/*命令序列号*/
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<VideoParamConfig Num=\"1\">\r\n"
                  "<Item>\r\n"
                  "<StreamName>MainFormat</StreamName>\r\n"
                  "<VideoFormat>2</VideoFormat>\r\n"
                  "<Resolution>4</Resolution>\r\n"
                  "<FrameRate>12</FrameRate>\r\n"
                  "<BitRateType>1</BitRateType>\r\n"
                  "<VideoBitRate>2949</VideoBitRate>\r\n"
                  "</Item>\r\n"
                  "</VideoParamConfig>\r\n"
                  "</Response>\r\n",/*查询结果标志*/
                  sn,
                  info.ipc_id);
    osip_message_set_body (message, tmp, swpa_strlen(tmp));

    osip_message_set_content_type (message,"Application/MANSCDP+xml");
    SW_TRACE_DEBUG("%s:[before printf_osip_message]\n",__FUNCTION__);
    printf_osip_message(message);
    INT ret = eXosip_message_send_request(message);
    if(ret != 0)
    {
        SW_TRACE_DEBUG("%s:send message failed",__FUNCTION__);
        return ret;
    }
    return 0;
}

INT	CSWGB28181App::ans_AudioParamConfig__response(LPCSTR sn)
{
	 	osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("message = NULL\n");
        return -1;
    }
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();
    
     CHAR tmp[4096];
    swpa_snprintf(tmp, 4096, "<?xml version=\"1.0\" encoding=\"GB2312\" ?>\r\n"
                  "<Response>\r\n"
                  "<CmdType>ConfigDownload</CmdType>\r\n"/*命令类型*/
                  "<SN>%s</SN>\r\n"/*命令序列号*/
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<AudioParamConfig Num=\"1\">\r\n"
                  "<Item>\r\n"
                  "<StreamName>MainFormat</StreamName>\r\n"
                  "<AudioFormat>1</AudioFormat>\r\n"
                  "<AudioBitRate>8</AudioBitRate>\r\n"
                  "<SamplingRate>1</SamplingRate>\r\n"
                  "</Item>\r\n"
                  "</AudioParamConfig>\r\n"
                  "</Response>\r\n",/*查询结果标志*/
                  sn,
                  info.ipc_id);
    osip_message_set_body (message, tmp, swpa_strlen(tmp));

    osip_message_set_content_type (message,"Application/MANSCDP+xml");
    SW_TRACE_DEBUG("%s:[before printf_osip_message]\n",__FUNCTION__);
    printf_osip_message(message);
    INT ret = eXosip_message_send_request(message);
    if(ret != 0)
    {
        SW_TRACE_DEBUG("%s:send message failed",__FUNCTION__);
        return ret;
    }
    return 0;
}

INT	CSWGB28181App::ans_BasicParam__response(LPCSTR sn)
{
		osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("message = NULL\n");
        return -1;
    }
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();
    
     CHAR tmp[4096];
    swpa_snprintf(tmp, 4096, "<?xml version=\"1.0\" encoding=\"GB2312\" ?>\r\n"
                  "<Response>\r\n"
                  "<CmdType>ConfigDownload</CmdType>\r\n"/*命令类型*/
                  "<SN>%s</SN>\r\n"/*命令序列号*/
                  "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                  "<BasicParam>\r\n"
                  "<Name>SWHV-PCC</Name>\r\n"
                  "<DeviceID>%s</DeviceID>\r\n"
                  "<SIPServerID>%s</SIPServerID>\r\n"
                  "<SIPServerIP>%s</SIPServerIP>\r\n"
                  "<SIPServerPort>%s</SIPServerPort>\r\n"
                  "<DomainName>3402000000</DomainName>\r\n"
                  "<Expiration>3600</Expiration>\r\n"
                  "<Password>%s</Password>\r\n"
                  "<HeartBeatInterval>60</HeartBeatInterval>\r\n"
                  "<HeartBeatCount>3</HeartBeatCount>\r\n"
                  "</BasicParam>\r\n"
                  "</Response>\r\n",/*查询结果标志*/
                  sn,info.ipc_id,info.ipc_id,info.server_id,info.server_ip,info.server_port,info.ipc_pwd);
    osip_message_set_body (message, tmp, swpa_strlen(tmp));

    osip_message_set_content_type (message,"Application/MANSCDP+xml");
    SW_TRACE_DEBUG("%s:[before printf_osip_message]\n",__FUNCTION__);
    printf_osip_message(message);
    INT ret = eXosip_message_send_request(message);
    if(ret != 0)
    {
        SW_TRACE_DEBUG("%s:send message failed",__FUNCTION__);
        return ret;
    }
    return 0;
}

INT CSWGB28181App::do_keepalive()
{
    if(m_status==0)
        return 0;
    osip_message_t *message = build_message();
    if(message == NULL)
    {
        SW_TRACE_DEBUG("message = NULL\n");
        return -1;
    }
    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();

    CHAR tmp[4096];
    swpa_snprintf (tmp, 4096,
                   "<?xml version=\"1.0\"?>\n"
                   "<Notify>\n"
                   "<CmdType>Keepalive</CmdType>\n"
                   "<SN>43</SN>\n"
                   "<DeviceID>%s</DeviceID>\n"
                   "<Status>OK</Status>\n"
                   "</Notify>\n",info.ipc_id);

    osip_message_set_body (message, tmp, swpa_strlen(tmp));
    osip_message_set_content_type (message,"Application/MANSCDP+xml");
    if(eXosip_message_send_request(message)!=0)
        return -1;
    return 0;
}

void CSWGB28181App::on_refresh_alive()
{
    m_keepalive_last_ticket=CSWDateTime::GetSystemTick();
}

BOOL CSWGB28181App::is_alive(unsigned long timeout)
{
    if(m_status==0)
        return TRUE;
    unsigned long ticket=CSWDateTime::GetSystemTick();

    if(_SIP_KEEPALIVE_TIME == timeout)
    {

    	if(ticket-m_keepalive_last_ticket > timeout && m_gb28181_refresh)
	    {
		    m_gb28181_refresh = FALSE;
	        return FALSE;
	    }
    }
    if(_SIP_KEEPALIVE_TIME*3 == timeout)
    {
    	if(ticket-m_keepalive_last_ticket>timeout )
    	{
    		SW_TRACE_DEBUG( "In is_alive ;timeout is:%d m_keepalive_last_ticket is  %d ticket is %d ticket-m_keepalive_last_ticket=%d!\n",timeout,m_keepalive_last_ticket,ticket,ticket-m_keepalive_last_ticket);
    		return FALSE;
    	}
    }
    return TRUE;
}


BOOL CSWGB28181App::is_register_alive(unsigned long timeout)
{
    if(m_status==0)
        return TRUE;
    unsigned long ticket=CSWDateTime::GetSystemTick();
    if(ticket-m_register_last_ticket>timeout)
        return FALSE;
    return TRUE;
}


INT CSWGB28181App::on_close_preview()
{
    SW_TRACE_DEBUG("on_close_preview:start...\n");
	t_thread = FALSE;
    GB28181_Control_Param Control_param;
	Control_param.Messege_ID = GB28181_CMD_BYE;
    SendRemoteMessage(MSG_GB2818_TRANSFORM_COMMAND,&Control_param,sizeof(GB28181_Control_Param));
	return 0;
}

INT	CSWGB28181App::on_process_register_success(eXosip_event * event)
{
    if(event==NULL)
        return 0;
    osip_header_t * h;
    if(osip_message_get_date(event->response,0,&h)==-1)
    {
        SW_TRACE_DEBUG("%s:get date failed! \n",__FUNCTION__);
        return 0;
    }
    SW_TRACE_DEBUG("%s:Date=%s\n",__FUNCTION__,SAFE_STR_PTR(h->hvalue));
    
    // 时间同步
	SWPA_DATETIME_TM sRealTime;
	swpa_memset(&sRealTime, 0, sizeof(sRealTime));
    INT iY, iMon, iD, iH, iM, iS, iTZ=8;//水星iTZ=8 金星=0
    #ifdef DM8127
        iTZ=0;
    #endif
	swpa_sscanf(SAFE_STR_PTR(h->hvalue), "%4d-%2d-%2dT%2d:%2d:%2d",// UTC%d", 2014-07-29T09:17:48
			&iY, &iMon, &iD, &iH, &iM, &iS);//, &iTZ);
	sRealTime.year = (short)iY;
	sRealTime.month = (short)iMon;
	sRealTime.day = (short)iD;
	sRealTime.hour = (short)iH;
	sRealTime.min = (short)iM;
	sRealTime.sec = (short)iS;
	sRealTime.msec = 0;
    SW_TRACE_DEBUG("<settime>%d-%d-%d %d %d %d.\n",iY, iMon, iD, iH, iM, iS);
	if (-12 > iTZ || +12 < iTZ)
	{
		SW_TRACE_DEBUG("Err: invalid TimeZone value %d\n");
	}
	else
	{
		SWPA_TIME sTime;
		
		swpa_datetime_tm2time(sRealTime, &sTime);
		sTime.sec -= 3600*iTZ;
		swpa_datetime_time2tm(sTime, &sRealTime);
	    SW_TRACE_DEBUG("CSWLPRApplication::OnSetTime %d-%02d-%02d %02d:%02d:%02d:%03d",sRealTime.year,
	                   sRealTime.month,sRealTime.day,sRealTime.hour,sRealTime.min,sRealTime.sec,sRealTime.msec);
//		SendRemoteMessage(MSG_APP_SETTIME,
//			&sRealTime,
//			NULL);		
		CSWDateTime dt;
		dt.SetTime(sRealTime);
#ifdef	DM8127		//金星有时区文件，已经是GMT8
		swpa_utils_shell("hwclock -w  --utc", NULL);
#else
		swpa_utils_shell("hwclock -w", NULL);
#endif


	}
	
    if(osip_message_get_expires(event->response,0,&h)==-1)
    {
        SW_TRACE_DEBUG("error! %d\n",__LINE__);
        return 0;
    }
    SW_TRACE_DEBUG("Expires:%s\n",SAFE_STR_PTR(h->hvalue));
    if(swpa_atoi(SAFE_STR_PTR(h->hvalue)) > 0)
    {
        m_status=1;
        m_register_last_ticket=CSWDateTime::GetSystemTick();
		SW_TRACE_DEBUG("In register success !");
        do_keepalive();
        on_refresh_alive();
    }
    else
    {
        //m_reset=0;
        SendRemoteMessage(MSG_APP_REBOOT,NULL,0);
        Sleep(20000); //注销等待重启.
    }
    return 0;
}

INT CSWGB28181App::on_process_call_close(eXosip_event *event)
{
    if(event==NULL)
        return -1;

	SW_TRACE_DEBUG("%s: start..\n",__FUNCTION__);	

	if(m_historythread.remove_listelment(event->did))
	{
		SW_TRACE_DEBUG("on_process_call_close: m_historythread.Stop\n");	
		m_historythread.Stop();
	}
		
	if(m_realtimethread.remove_listelment(event->did))
	{
		SW_TRACE_DEBUG("on_process_call_close: m_realtimethread.Stop\n");	
		m_realtimethread.Stop();
	}
	
	on_close_preview();
    return 0;
}

void CSWGB28181App::SendVideoEndMessage()
{
	SW_TRACE_DEBUG("%s: begin\n",__FUNCTION__);
	t_thread = TRUE;
	while(t_thread)
	{
		swpa_thread_sleep_ms(50);
		if( TRUE == m_historythread.get_Historystopflag())
		{
			osip_message_t *message = build_message();

			CHAR tmp[4096];
			swpa_memset(tmp,0,4096);
			swpa_snprintf (tmp, 4096,
							"<?xml version=\"1.0\"?>\n"
							"<Notify>\n"
							"<CmdType>MediaStatus</CmdType>\n"/*命令类型*/	                
							"<SN>%s</SN>\n"/*命令序列号*/  //sn
							"<DeviceID>%s</DeviceID>\n"
							"<NotifyType>121</NotifyType>\n"
							"</Notify>\n",
							"17431",
							m_gbinfo.ipc_id
							//"34020000001310000001" // ？设备ID是111，这里是录像通道子ID 131
							);

			SW_TRACE_DEBUG("send MediaStatus=%s\n",tmp);
			osip_message_set_body (message, tmp, swpa_strlen(tmp));
			osip_message_set_content_type (message,"Application/MANSCDP+xml");
			
			SW_TRACE_DEBUG("SendVideoEndMessage number:%s\n",m_call_id_number);  
			osip_call_id_set_number (message->call_id, m_call_id_number);
			eXosip_message_send_request(message);

			t_thread = FALSE;
		}
	}
}

LPVOID CSWGB28181App::SendVideoEndMessageThread(LPVOID pArg)
{
	if (pArg)
	{
		CSWGB28181App * pThis = (CSWGB28181App*)pArg;
		pThis->SendVideoEndMessage();
	}
	return 0;
}

INT CSWGB28181App::on_process_invite(eXosip_event *event)
{
    if(event==NULL)
        return -1;

    osip_content_type_t * ct=osip_message_get_content_type(event->request);
    if(ct==NULL)
        return -1;
    CHAR * tmp=NULL;
    if(osip_content_type_to_str(ct,&tmp)!=0)
        return -1;


	DWORD tempIP;
    CHAR* buf = NULL;
	client_info* Client_info = new client_info;
	
    osip_message_to_str(event->request,&buf,&(event->request->message_length));
    SW_TRACE_DEBUG("CSWGB28181App::on_process_invite ,message=%s\n",buf);

	   
    if(osip_strcasecmp(SAFE_STR_PTR(tmp),"application/sdp")==0)
    {
        osip_free(tmp);
		
        CHAR    *p_xml_body     = buf;
        CHAR    *p_str_begin    = NULL;
        CHAR    *p_str_end      = NULL;

        CHAR    playcmd[32];
        CHAR    videoSendIP[32];
        CHAR    videoSendPort[32];
        CHAR    ssrc[32];
        CHAR    str_time[128];
        swpa_memset(ssrc,0,32);
        swpa_memset(str_time,0,128);
        
        p_str_begin = swpa_strstr(p_xml_body, "y=");/*查找字符串"<CmdType>"*/
        if (p_str_begin)
	        p_str_end   = swpa_strstr(p_str_begin, "\n");
	    else
	    	SW_TRACE_DEBUG("CSWGB28181App::on_process_invite: y=NULL \n");
		
        if(p_str_begin != NULL && p_str_end != NULL)
        {
	   
            swpa_memcpy(ssrc, p_str_begin+2, p_str_end-p_str_begin-2);
        }
        
        p_str_begin = swpa_strstr(buf, "s=");
        if (p_str_begin)
	        p_str_end   = swpa_strstr(p_str_begin, "\r\n");
	   
        if(p_str_begin != NULL && p_str_end != NULL)
        {
            swpa_memset(playcmd,0,32);
            swpa_memcpy(playcmd, p_str_begin+2, p_str_end-p_str_begin-2);
        }
		else
		{
			SW_TRACE_DEBUG("CSWGB28181App::on_process_invite:s=NULL \n");
		}
        
        p_str_begin = swpa_strstr(buf, "c=IN IP4 ");
        if (p_str_begin)
	        p_str_end   = swpa_strstr(p_str_begin, "\r\n");
	   
        if(p_str_begin != NULL && p_str_end != NULL)
        {
            swpa_memset(videoSendIP,0,32);
            swpa_memcpy(videoSendIP, p_str_begin+9, p_str_end-p_str_begin-9);
        }

        p_str_begin = swpa_strstr(buf, "m=video ");
        if (p_str_begin)
	        p_str_end   = swpa_strstr(p_str_begin, " RTP/AVP");
	    
        if(p_str_begin != NULL && p_str_end != NULL)
        {
            swpa_memset(videoSendPort,0,32);
            swpa_memcpy(videoSendPort, p_str_begin+8, p_str_end-p_str_begin-8);
        }
	
	    // 要在osip_free(buf)之前搜索
        p_str_begin = swpa_strstr(p_xml_body, "t=");/*查找字符串"<CmdType>"*/
        if (p_str_begin)
	        p_str_end   = swpa_strstr(p_str_begin, "\n");
	    
        if(p_str_begin != NULL && p_str_end != NULL)
        {
            swpa_memcpy(str_time, p_str_begin+2, p_str_end-p_str_begin-2);
			SW_TRACE_DEBUG("CSWGB28181App::on_process_invite:str_time [%s]\n",SAFE_STR_PTR(str_time));
        }

		tempIP = swpa_atol(videoSendPort);
		Client_info->did = event->did;
		Client_info->videoSendPort = tempIP;
		swpa_strcpy(Client_info->videoSendIP,videoSendIP);
	        
        osip_free(buf);
		if (swpa_strstr(SAFE_STR_PTR(playcmd),"Playback") !=0
         || swpa_strstr(SAFE_STR_PTR(playcmd),"Download") !=0 )
      	{
			if(S_OK != m_realtimethread.IsValid()  && S_OK != m_historythread.IsValid())		//判断是否有线程存在	
			{
				m_historythread.set_GBAPPlist(Client_info);
				
				if( NULL != event->request && NULL != event->request->call_id )		
	          	{
	          		memset(m_call_id_number,0,128);
	          		strcpy(m_call_id_number,osip_call_id_get_number (event->request->call_id));
	          		SW_TRACE_DEBUG("CSWGB28181App::on_process_invite:Playback number:%s\n",m_call_id_number);         	
	          	}
	         	else
	        	{
	        		SW_TRACE_DEBUG("Playback NULL \n");
	        	}
	        
		        CHAR    sBeginTime[32];
		        CHAR    sEndTime[32];
		        swpa_memset(sBeginTime,0,32);
		        swpa_memset(sEndTime,0,32);
		        
		        swpa_strncpy(sBeginTime,str_time,10);
		        swpa_strncpy(sEndTime,str_time+11,10);
		        SW_TRACE_DEBUG("CSWGB28181App::on_process_invite:sBeginTime:%s,sEndTime:%s\n",sBeginTime,sEndTime);
		        
		        SWPA_TIME tBeginTime;
		        SWPA_TIME tEndTime;
		        tBeginTime.sec = swpa_atoi(sBeginTime);
		        tBeginTime.msec = 0;
		        tEndTime.sec = swpa_atoi(sEndTime);
		        tEndTime.msec = 0;
		        
		        SW_TRACE_DEBUG("CSWGB28181App::on_process_invite:tBeginTime:%d,tEndTime:%d\n",tBeginTime.sec,tEndTime.sec);
		        
		        swpa_datetime_time2tm(tBeginTime,&Control_param.sBeginTime);
		        swpa_datetime_time2tm(tEndTime,&Control_param.sEndTime);

				Control_param.sBeginTime.msec = 0;
				Control_param.sEndTime.msec = 0;
		        if( swpa_strstr(SAFE_STR_PTR(playcmd),"Playback")!=0 )
		        {
	      			Control_param.Messege_ID = GB28181_CMD_PLAYBACK;
	      	  	}
	      	  	else
	    	  	{
	 	  		    Control_param.Messege_ID = GB28181_CMD_DOWNLOAD;	
	    	  	}
				Control_param.fltScale = 1.0;

				SW_TRACE_DEBUG("%s: Messege_ID = %d,Channel_ID = %d, speed = %f,begintime: %04d/%02d/%02d %02d:%02d:%02d:%02d endtime: %04d/%02d/%02d %02d:%02d:%02d:%02d \n",__FUNCTION__,
						Control_param.Messege_ID,Control_param.Channel_ID,Control_param.fltScale,
						Control_param.sBeginTime.year,Control_param.sBeginTime.month,Control_param.sBeginTime.day,
						Control_param.sBeginTime.hour,Control_param.sBeginTime.min,Control_param.sBeginTime.sec,
						Control_param.sBeginTime.msec,Control_param.sEndTime.year,Control_param.sEndTime.month,
						Control_param.sEndTime.day,Control_param.sEndTime.hour,Control_param.sEndTime.min,
						Control_param.sEndTime.sec,Control_param.sEndTime.msec);
				
			    SendRemoteMessage(MSG_GB2818_TRANSFORM_COMMAND,&Control_param,sizeof(GB28181_Control_Param));

				//视频播放结束发送结束消息
	        	int hThread = 0;
	        	swpa_thread_create(&hThread, 64 * 1024, 0, (LPVOID (*)(LPVOID))SendVideoEndMessageThread, this);

				if(S_OK != m_historythread.Start())  
				{
					SW_TRACE_DEBUG("%s:historythread start failed \n",__FUNCTION__);
					return -1;
				}
				
				if(0 == do_preview(event,ssrc))
				{
					SW_TRACE_DEBUG("%s:send the SDP info success \n",__FUNCTION__);
					return 0;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				SW_TRACE_DEBUG("%s:realtimethread or historythread is exit,do nothing \n",__FUNCTION__);
				return -1;	
			}
      	}
      	else if (swpa_strstr(SAFE_STR_PTR(playcmd),"Play")!=0)
        {
			if(S_OK != m_historythread.IsValid())
			{
				m_realtimethread.set_GBAPPlist(Client_info);

				SWPA_TIME tTime;
				tTime.sec = 0;
				tTime.msec = 0;

				Control_param.fltScale = 1.0;
				swpa_datetime_time2tm(tTime,&Control_param.sBeginTime);
	    	    swpa_datetime_time2tm(tTime,&Control_param.sEndTime);
				Control_param.Messege_ID = GB28181_CMD_PLAY;

				SW_TRACE_DEBUG("%s: Messege_ID = %d,Channel_ID = %d, speed = %f,begintime: %04d/%02d/%02d %02d:%02d:%02d:%02d endtime: %04d/%02d/%02d %02d:%02d:%02d:%02d \n",__FUNCTION__,
						Control_param.Messege_ID,Control_param.Channel_ID,Control_param.fltScale,
						Control_param.sBeginTime.year,Control_param.sBeginTime.month,Control_param.sBeginTime.day,
						Control_param.sBeginTime.hour,Control_param.sBeginTime.min,Control_param.sBeginTime.sec,
						Control_param.sBeginTime.msec,Control_param.sEndTime.year,Control_param.sEndTime.month,
						Control_param.sEndTime.day,Control_param.sEndTime.hour,Control_param.sEndTime.min,
						Control_param.sEndTime.sec,Control_param.sEndTime.msec);
				
 			    SendRemoteMessage(MSG_GB2818_TRANSFORM_COMMAND,&Control_param,sizeof(GB28181_Control_Param));
				
				if(S_OK != m_realtimethread.IsValid())
				{
					if(S_OK != m_realtimethread.Start())  
					{
						SW_TRACE_DEBUG("%s:m_realtimethread start failed \n",__FUNCTION__);
						return -1;
					}
				}
				
				if (0 == do_preview(event,ssrc))
	            {
	            	SW_TRACE_DEBUG("%s:send the SDP info success \n",__FUNCTION__);
	            	return 0;
	            }
	            else
	            {
	            	return -1;
	            }
			}
			else
			{
				SW_TRACE_DEBUG("%s:realtimethread or historythread is exit,do nothing \n",__FUNCTION__);
				return -1;
			}	
        }
    }
    else
    {
    	osip_free(buf);
        osip_free(tmp);
        return -1;
    }

    return -1;
}

BOOL CSWGB28181App::check_connect()
{

    GB28181_info info;
    m_cMutexLock.Lock();
    swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
    m_cMutexLock.Unlock();
    if(m_reset)
    {
        do_unregister();
        return TRUE;
    }
    if(info.enable == FALSE)
    {
        do_unregister();
    }
    else
    {
        do_register();
    }
    return TRUE;
}

void CSWGB28181App::check_keepalive()
{	
    if(!is_alive(_SIP_KEEPALIVE_TIME))
    {
    	SW_TRACE_DEBUG( "In check_keepalive Line is:  %d the next line is do_keepalive!",__LINE__);
        do_keepalive();
    }
	
    if(!is_alive(_SIP_KEEPALIVE_TIME*3))
    {
        do_unregister();
    }

	
    if(!is_register_alive((_SIP_EXPIRES_TIME*1000)/4))
    {
        if(m_regid != 0)
            do_RepeatRegister();
    }

}

INT CSWGB28181App::do_RepeatRegister()
{
    if(m_regid == 0)
        return -1;

    eXosip_clear_authentication_info();
    osip_message_t *reg = NULL;
    eXosip_register_build_register(m_regid,_SIP_EXPIRES_TIME,&reg);
    if(eXosip_register_send_register(m_regid, reg)!=0)
        return -1;
    SW_TRACE_DEBUG( "do_RepeatRegister22 success \n");
    return 0;

}

INT CSWGB28181App::on_process_message(eXosip_event *event)
{
    if(event==NULL)
    {
        return -1;
    }
    else if (!MSG_IS_MESSAGE(event->request))
    {
    	if (MSG_IS_INFO(event->request))
    	{
		    SW_TRACE_DEBUG("on_process_message got INFO, return 200 ok\n");
		    //eXosip_message_send_answer (event->tid, 200,NULL);
		    //on_process_message_RTSP(event); 
		    return 0;
    	}
    	else if (MSG_IS_BYE(event->request))
    	{
		    SW_TRACE_DEBUG("on_process_message got BYE, return 200 ok\n");
		    //int ret = eXosip_message_send_answer (event->tid, 200,NULL);
		    //SW_TRACE_DEBUG("got BYE ret= %d\n",ret);
		  //  return on_process_call_close(event);
		  	return 0;
    	}
    	return -1;
    }
    else
    {
	    SW_TRACE_DEBUG("on_process_message\n");
	    osip_body_t *body=NULL;
	    if(osip_message_get_body(event->request,0, &body)<0)
	        return -1;
	    if(body==NULL||body->body==NULL)
	        return -1;
	        
	    CHAR    *p_xml_body     = body->body;
	    CHAR    *p_str_begin    = NULL;
	    CHAR    *p_str_end      = NULL;
	    CHAR    cmdtype[32];
	    CHAR    sn[32];
	    swpa_memset(cmdtype,0,sizeof(cmdtype));
	    swpa_memset(sn,0,sizeof(sn));
	    p_str_begin = swpa_strstr(p_xml_body, "<CmdType>");/*查找字符串"<CmdType>"*/
	    p_str_end   = swpa_strstr(p_xml_body, "</CmdType>");
	    if(p_str_begin != NULL && p_str_end != NULL)
	        swpa_memcpy(cmdtype, p_str_begin+9, p_str_end-p_str_begin-9);/*保存<CmdType>到xml_cmd_type*/
	
	
	    p_str_begin = swpa_strstr(p_xml_body, "<SN>");/*查找字符串"<SN>"*/
	    p_str_end   = swpa_strstr(p_xml_body, "</SN>");
	    if(p_str_begin != NULL && p_str_end != NULL)
	        swpa_memcpy(sn, p_str_begin+4, p_str_end-p_str_begin-4);/*保存<SN>到xml_cmd_sn*/
	
	     
	    if(swpa_strcmp(cmdtype,"DeviceControl")==0)
	    {
	        SW_TRACE_DEBUG("DeviceControl \n");
	        //SW_TRACE_DEBUG("p_xml_body %s\n",p_xml_body);
	        //设备控制(PTZ、录像控制、报警布撤防、报警复位、远程启动)
	        CHAR ptzcmd[32];
	        CHAR guardcmd[32];
	        CHAR alarmresetcmd[32];
	        CHAR boot[32];
	        CHAR recordcmd[32];
	        swpa_memset(ptzcmd,0,sizeof(ptzcmd));
	        swpa_memset(guardcmd,0,sizeof(guardcmd));
	        swpa_memset(alarmresetcmd,0,sizeof(alarmresetcmd));
	        swpa_memset(boot,0,sizeof(boot));
	        swpa_memset(recordcmd,0,sizeof(recordcmd));
	        
	        p_str_begin = swpa_strstr(p_xml_body, "<PTZCmd>");/*查找字符串"<PTZCmd>"*/
	        p_str_end   = swpa_strstr(p_xml_body, "</PTZCmd>");
	        if(p_str_begin != NULL && p_str_end != NULL)
	            swpa_memcpy(ptzcmd, p_str_begin+8, p_str_end-p_str_begin-8);/*保存<PTZCmd>ptzcmd*/
	        //printf("%s %d\n",ptzcmd,__LINE__);
	
	        p_str_begin = swpa_strstr(p_xml_body, "<GuardCmd>");/*查找字符串"<GuardCmd>"*///布撤防
	        p_str_end   = swpa_strstr(p_xml_body, "</GuardCmd>");
	        if(p_str_begin != NULL && p_str_end != NULL)
	            swpa_memcpy(guardcmd, p_str_begin+10, p_str_end-p_str_begin-10);/*保存<GuardCmd>guardcmd*/
	        //printf("%s %d\n",guardcmd,__LINE__);
	
	
	        p_str_begin = swpa_strstr(p_xml_body, "<AlarmCmd>");/*查找字符串"<AlarmCmd>"*/
	        p_str_end   = swpa_strstr(p_xml_body, "</AlarmCmd>");//报警复位
	        if(p_str_begin != NULL && p_str_end != NULL)
	            swpa_memcpy(alarmresetcmd, p_str_begin+10, p_str_end-p_str_begin-10);/*保存<AlarmCmd>alarmresetcmd*/
	        //printf("%s %d\n",alarmresetcmd,__LINE__);
	
	        p_str_begin = swpa_strstr(p_xml_body, "<TeleBoot>");/*查找字符串"<TeleBoot>"*/
	        p_str_end   = swpa_strstr(p_xml_body, "</TeleBoot>");//远程启动
	         if(p_str_begin != NULL && p_str_end != NULL)
	            swpa_memcpy(boot, p_str_begin+10, p_str_end-p_str_begin-10);/*保存<TeleBoot>boot*/
	        //printf("%s %d\n",boot,__LINE__);
	        
	        p_str_begin = swpa_strstr(p_xml_body, "<RecordCmd>");/*查找字符串"<RecordCmd>"*/
	        p_str_end   = swpa_strstr(p_xml_body, "</RecordCmd>");//开始手动录像
	         if(p_str_begin != NULL && p_str_end != NULL)
	            swpa_memcpy(recordcmd, p_str_begin+11, p_str_end-p_str_begin-11);/*保存<RecordCmd>Record*/
	        
	        //SW_TRACE_DEBUG("recordcmd %s\n",recordcmd);
	        
	
	        if(swpa_strlen(ptzcmd) != 0)
	        {
	            SW_TRACE_DEBUG("ptzcmd  %s\n",ptzcmd);
	            eXosip_message_send_answer (event->tid, 200,NULL);
	            return do_ptz(ptzcmd);
	        }
	        else if(swpa_strlen(guardcmd) != 0)
	        {
	            SW_TRACE_DEBUG("guardcmd  %s\n",guardcmd);
	            eXosip_message_send_answer (event->tid, 200,NULL);
	            return do_guard(guardcmd,sn);
	        }
	        else if(swpa_strlen(alarmresetcmd) != 0)
	        {
	            SW_TRACE_DEBUG("alarmresetcmd %s\n",alarmresetcmd);
	            eXosip_message_send_answer (event->tid, 200,NULL);
	            return do_alarmreset(alarmresetcmd,sn);
	        }
	        else if(swpa_strlen(boot) != 0)
	        {
	            SW_TRACE_DEBUG("boot %s\n",boot);
	            eXosip_message_send_answer (event->tid, 200,NULL);
	            m_reset = 1;
	            // do_unregister();
	
	            //todo
	
	            return 0;
	        }
	        else if(swpa_strlen(recordcmd) != 0)
	        {
	            SW_TRACE_DEBUG("recordcmd %s\n",recordcmd);
	            eXosip_message_send_answer (event->tid, 200,NULL);
	            return do_recordcmd(recordcmd,sn);
	        }
	        else
	            return -1;
	    }
	    else if(swpa_strcmp(cmdtype,"Catalog")==0)
	    {
	        eXosip_message_send_answer (event->tid, 200,NULL);
	        return do_catalog(event,sn);
	    }
	    else if(swpa_strcmp(cmdtype,"Alarm")==0)
	    {
	        SW_TRACE_DEBUG("告警输出返回\n");
	        eXosip_message_send_answer (event->tid, 200,NULL);
	        return 0;
	    }
	    else if(swpa_strcmp(cmdtype,"DeviceInfo")==0)//设备信息查询
	    {
	        SW_TRACE_DEBUG("设备信息查询\n");
	        eXosip_message_send_answer (event->tid, 200,NULL);
	        return do_device_info(sn);
	    }
	    else if(swpa_strcmp(cmdtype,"DeviceStatus")==0)//设备信息查询
	    {
	        SW_TRACE_DEBUG("设备状态查询\n");
	        eXosip_message_send_answer (event->tid, 200,NULL);
	        return ans_devicestatus__response(sn);
	    }
	    else if(swpa_strcmp(cmdtype,"RecordInfo")==0)//设备信息查询
	    {
	        SW_TRACE_DEBUG("录像查询\n");
	        eXosip_message_send_answer (event->tid, 200,NULL);
	        
	        CHAR sStartTime[64];
	        CHAR sEndTime[64];
	        swpa_memset(sStartTime,0,sizeof(sStartTime));
	        swpa_memset(sEndTime,0,sizeof(sEndTime));
	        p_str_begin = swpa_strstr(p_xml_body, "<StartTime>"); 
	        p_str_end   = swpa_strstr(p_xml_body, "</StartTime>");
	        if(p_str_begin != NULL && p_str_end != NULL)
	        {
	            swpa_memcpy(sStartTime, p_str_begin+11, p_str_end-p_str_begin-11);
	        }
	        else
	        {
	       		return -1;
	        }
	        p_str_begin = swpa_strstr(p_xml_body, "<EndTime>"); 
	        p_str_end   = swpa_strstr(p_xml_body, "</EndTime>");
	        if(p_str_begin != NULL && p_str_end != NULL)
	        {
	        	  swpa_memcpy(sEndTime, p_str_begin+9, p_str_end-p_str_begin-9);
	        }
	        else
	        {
	        	  return -1;
	        }
	            
	        SWPA_DATETIME_TM tStartTime,tEndTime;
	        
	        SW_TRACE_DEBUG("%s:sStartTime=%s,sEndTime=%s\n",__FUNCTION__,sStartTime,sEndTime);
	        
	        swpa_sscanf(sStartTime,"%d-%d-%dT%d:%d:%d",&tStartTime.year,&tStartTime.month,&tStartTime.day,
	        	&tStartTime.hour,&tStartTime.min,&tStartTime.sec);
	        swpa_sscanf(sEndTime,"%d-%d-%dT%d:%d:%d",&tEndTime.year,&tEndTime.month,&tEndTime.day,
	        	&tEndTime.hour,&tEndTime.min,&tEndTime.sec);
	        
	        SW_TRACE_DEBUG("tStartTime %d-%d-%dT%d:%d:%d \n",
	        	tStartTime.year,tStartTime.month,tStartTime.day,
	        	tStartTime.hour,tStartTime.min,tStartTime.sec);
			
	        SW_TRACE_DEBUG("tEndTime %d-%d-%dT%d:%d:%d \n",
	        	tEndTime.year,tEndTime.month,tEndTime.day,
	        	tEndTime.hour,tEndTime.min,tEndTime.sec);
	        	
	        CSWDateTime dtStartTime(&tStartTime),dtEndTime(&tEndTime);
			
	        return ans_deviceRecordInfo__response(sn,&dtStartTime,&dtEndTime);
	    }
	    else if(swpa_strcmp(cmdtype,"ConfigDownload")==0)
	    {
	    		SW_TRACE_DEBUG("配置查询\n");
	        eXosip_message_send_answer (event->tid, 200,NULL);
	        
	        CHAR    configType[32];
	        swpa_memset(configType,0,sizeof(configType));
	        p_str_begin = swpa_strstr(p_xml_body, "<ConfigType>");
			    p_str_end   = swpa_strstr(p_xml_body, "</ConfigType>");
			    if(p_str_begin != NULL && p_str_end != NULL)
			    {
			    		swpa_memcpy(configType, p_str_begin+12, p_str_end-p_str_begin-12);
			    		if(swpa_strcmp(configType,"VideoParamConfig")==0)
			    		{
			    				return ans_VideoParamConfig__response(sn);
			    		}
			    		else if(swpa_strcmp(configType,"AudioParamConfig")==0)
			    		{
			    				return ans_AudioParamConfig__response(sn);
			    		}
			    		else if(swpa_strcmp(configType,"BasicParam")==0)
			    		{
			    				return ans_BasicParam__response(sn);
			    		}
			    		else
			    		{
			    			SW_TRACE_DEBUG("unknown configType \n");
			    		}
			    }
			    else
			    {
			    		SW_TRACE_DEBUG("NULL configType \n");
			    }
	    }
	    else
	    {
	        SW_TRACE_DEBUG("NULL cmdtpey \n");
	    }
	}

    return -1;
}

INT CSWGB28181App::on_process_message_ack(eXosip_event *event)
{
	SW_TRACE_DEBUG("%s:on_refresh_alive\n",__FUNCTION__);
	m_gb28181_refresh = TRUE;
    on_refresh_alive();
    return 0;
}


INT CSWGB28181App::HexStringToBytes(LPCSTR strCmd, BYTE* pbCmd, INT iLen)
{
	if (NULL == strCmd || NULL == pbCmd || 0 == iLen)
	{	
		return -1;
	}

	CHAR* pcFirstChar = (CHAR*)strCmd;
	CHAR* pcSecondChar = pcFirstChar+1;
	INT iCount = 0;

	while ('\0' != *pcFirstChar && '\0' != *pcSecondChar)
	{
		BYTE bHigh = 0, bLow = 0;
		if (*pcFirstChar>='0' && *pcFirstChar<='9')
		{
			bHigh = *pcFirstChar - '0';
		}
		else if (*pcFirstChar>='a' && *pcFirstChar<='f')
		{
			bHigh = *pcFirstChar - 'a' + 10;
		}
		else if (*pcFirstChar>='A' && *pcFirstChar<='F')
		{
			bHigh = *pcFirstChar - 'A' + 10;
		}
		else
		{
			return iCount;
		}

		if (*pcSecondChar>='0' && *pcSecondChar<='9')
		{
			bLow = *pcSecondChar - '0';
		}
		else if (*pcSecondChar>='a' && *pcSecondChar<='f')
		{
			bLow = *pcSecondChar - 'a' + 10;
		}
		else if (*pcSecondChar>='A' && *pcSecondChar<='F')
		{
			bLow = *pcSecondChar - 'A' + 10;
		}
		else
		{
			return iCount;
		}

		*(pbCmd+iCount) = (BYTE)((bHigh << 4) | (bLow & 0xF));
		iCount ++;
		if (iCount >= iLen)
		{	
			return iCount;
		}

		pcFirstChar += 2;
		pcSecondChar += 2;
	}

	return iCount;
}



INT CSWGB28181App::do_ptz(LPCSTR ptzcmd)
{

    BYTE cmd[128] = {0};
	SW_TRACE_DEBUG("cmd = %s\n", ptzcmd);
	
    INT len=HexStringToBytes(ptzcmd, cmd, 128);

	SW_TRACE_DEBUG("cmd = %s, len = %d\n", ptzcmd, len);

    //长度不符
    if(len!=8)
        return -1;

    //起始码
    if(cmd[0]!=0xA5)
        return -1;


    INT pan_speed=cmd[4];
    INT titl_speed=cmd[5];
    INT zoom_speed=0x5;//HIBYTE(cmd[6]);
    pan_speed=pan_speed*100/255;
    titl_speed=titl_speed*100/255;
    zoom_speed=zoom_speed*100/15;

	DWORD arrdwMsg[3] = {0};
    BOOL need_action=TRUE;
    switch(cmd[3] & 0x0F)
    {
    case 0x01:
        //右
        SW_TRACE_DEBUG("PZTZ 右\n");
        arrdwMsg[0] = MSG_START_PAN;
		arrdwMsg[1] = 1;
		arrdwMsg[2] = pan_speed;
        break;
    case 0x02:
        //左
        SW_TRACE_DEBUG("PZTZ 左\n");
        arrdwMsg[0] = MSG_START_PAN;
		arrdwMsg[1] = 0;
		arrdwMsg[2] = pan_speed;
        break;
    case 0x04:
        //下
        SW_TRACE_DEBUG("PZTZ 下\n");
        arrdwMsg[0] = MSG_START_TILT;
		arrdwMsg[1] = 0;
		arrdwMsg[2] = titl_speed;
        break;
    case 0x05:
        //右下
        SW_TRACE_DEBUG("PZTZ 右下\n");
        arrdwMsg[0] = MSG_START_PAN_TILT;
		arrdwMsg[1] = 2;
		arrdwMsg[2] = pan_speed;
        break;
    case 0x06:
        //左下
        SW_TRACE_DEBUG("PZTZ 左下\n");
        arrdwMsg[0] = MSG_START_PAN_TILT;
		arrdwMsg[1] = 0;
		arrdwMsg[2] = pan_speed;
        break;
    case 0x08:
        //上
        SW_TRACE_DEBUG("PZTZ 上\n");
        arrdwMsg[0] = MSG_START_TILT;
		arrdwMsg[1] = 1;
		arrdwMsg[2] = titl_speed;
        break;
    case 0x09:
        //右上
        SW_TRACE_DEBUG("PZTZ 右上\n");
		arrdwMsg[0] = MSG_START_PAN_TILT;
		arrdwMsg[1] = 3;
		arrdwMsg[2] = pan_speed;
        break;
    case 0x0A:
        //左上
        SW_TRACE_DEBUG("PZTZ 左上\n");
		arrdwMsg[0] = MSG_START_PAN_TILT;
		arrdwMsg[1] = 1;
		arrdwMsg[2] = pan_speed;
        break;
    default:
        need_action=FALSE;
        break;
    }

    if(need_action)
	{
		SendRemoteMessage(MSG_APP_REMOTE_PTZ_CONTROL, arrdwMsg, sizeof(arrdwMsg));
	}
	
    //执行zoom控制
    //memset(&ptz_ctrl,0,sizeof(ptz_ctrl));
    //ptz_ctrl.u8ProtoType = 0xff;
    need_action=TRUE;
    switch(cmd[3] & 0x30)
    {
    case 0x10:
        //放大
        SW_TRACE_DEBUG("PZTZ 放大\n");
        arrdwMsg[0] = MSG_START_ZOOM;
		arrdwMsg[1] = 0;
		arrdwMsg[2] = 0;
        break;
    case 0x20:
        //缩小
        SW_TRACE_DEBUG("PZTZ 缩小\n");
        arrdwMsg[0] = MSG_START_ZOOM;
		arrdwMsg[1] = 1;
		arrdwMsg[2] = 0;
        break;
    default:
        need_action=FALSE;
        break;
    }

    if(need_action)
	{
		SendRemoteMessage(MSG_APP_REMOTE_PTZ_CONTROL, arrdwMsg, sizeof(arrdwMsg));
	}

    if(cmd[3]==0x00)
    {
        //停止
        SW_TRACE_DEBUG("PZTZ 停止\n");
        DWORD arrdwMsg[3] = {0};
		arrdwMsg[0] = MSG_STOP_PAN;
        SendRemoteMessage(MSG_APP_REMOTE_PTZ_CONTROL, arrdwMsg, sizeof(arrdwMsg));
		arrdwMsg[0] = MSG_STOP_ZOOM;
		SendRemoteMessage(MSG_APP_REMOTE_PTZ_CONTROL, arrdwMsg, sizeof(arrdwMsg));
    }
    
    return 0;
}

INT CSWGB28181App::on_process_message_RTSP(eXosip_event *event)
{
	osip_message_t *message = event->request;
	CHAR 	*buf;
	CHAR    *p_str_begin    = NULL;
	CHAR    *p_str_end      = NULL;
	CHAR    Scale[32];
	CHAR    sPauseTime[32];
	CHAR	m_cseq[32];
	CHAR	range[32];
	INT		rang_int;	
	BOOL    m_direction = TRUE;            //记录历史视频暂停之前播放的方向   
	SWPA_TIME tBeginTime;	 				//时间转换
	SWPA_DATETIME_TM sgetTime;   
	

	GB28181_Control_Param Control_param_t;
	swpa_memset(&Control_param_t,0,sizeof(GB28181_Control_Param));
	
	SW_TRACE_DEBUG("%s:message:%d\n",__FUNCTION__,message);
	if(message)
	{
		eXosip_message_send_answer (event->tid, 200,NULL);

		osip_message_to_str(message,&buf,&(message->message_length));
		SW_TRACE_DEBUG("osip_message：%s\n",buf);

		if(swpa_strstr(buf,"PLAY")!= NULL)
		{
			p_str_begin = swpa_strstr(buf, "Scale:");		//向前播放
			if(NULL != p_str_begin)   					
			{
				Control_param_t.Messege_ID = GB28181_CMD_FORWARD;
				p_str_end = swpa_strstr(p_str_begin, "\n");
					
				if(p_str_begin != NULL && p_str_end != NULL)
				swpa_memcpy(Scale, p_str_begin+6, p_str_end-p_str_begin-6);
				Control_param_t.fltScale = swpa_atof(Scale);

				m_historythread.get_Timerecord(&sgetTime);
				swpa_memcpy(&Control_param_t.sBeginTime,&sgetTime,sizeof(SWPA_DATETIME_TM));
				swpa_memcpy(&Control_param_t.sEndTime,&Control_param.sEndTime,sizeof(SWPA_DATETIME_TM));

				Control_param_t.sBeginTime.msec = 0;
				Control_param_t.sEndTime.msec = 0;
				
				m_speed = Control_param_t.fltScale;
				m_direction = TRUE;

			}
			SW_TRACE_DEBUG("fltScale:%f\n",Control_param_t.fltScale);
			
			p_str_begin = swpa_strstr(buf,"Scale: -");
			if(NULL != p_str_begin)   					
			{
				Control_param_t.Messege_ID = GB28181_CMD_BACKAWAY;			//向后播放
				p_str_end = swpa_strstr(p_str_begin, "\n");
				if(p_str_begin != NULL && p_str_end != NULL)
				swpa_memcpy(Scale, p_str_begin+8, p_str_end-p_str_begin-8);
				
				Control_param_t.fltScale = swpa_atof(Scale);
				swpa_memcpy(&Control_param_t.sBeginTime,&Control_param.sBeginTime,sizeof(SWPA_DATETIME_TM));
				m_historythread.get_Timerecord(&sgetTime);
				swpa_memcpy(&Control_param_t.sEndTime,&sgetTime,sizeof(SWPA_DATETIME_TM));
				Control_param_t.sBeginTime.msec = 0;
				Control_param_t.sEndTime.msec = 0;
				m_speed = Control_param_t.fltScale;
				m_direction = FALSE;
			}
			
			p_str_begin = swpa_strstr(buf,"Range: npt=");
			if(NULL != p_str_begin)
			{
				if(p_str_begin != NULL)
				{
					Control_param_t.Messege_ID = GB28181_CMD_DRAG;		//拖放
					p_str_end = swpa_strstr(p_str_begin, "-");
				}
				if(p_str_begin != NULL && p_str_end != NULL)
					swpa_memcpy(range, p_str_begin+11, p_str_end-p_str_begin-11);

				rang_int = swpa_atoi(range);

				swpa_datetime_tm2time(Control_param.sBeginTime,&tBeginTime);
				tBeginTime.sec += rang_int;
				tBeginTime.msec = 0;
				swpa_datetime_time2tm(tBeginTime, &Control_param_t.sBeginTime);
				swpa_memcpy(&Control_param_t.sEndTime,&Control_param.sEndTime,sizeof(SWPA_DATETIME_TM));	

				Control_param_t.sBeginTime.msec = 0;
				Control_param_t.sEndTime.msec = 0;					//毫秒都设置为0

				Control_param_t.fltScale = m_speed;
			}


			if(swpa_strstr(buf,"Range: npt=now-")!= NULL)
			{
				if(m_direction)					//	恢复播放，向前播放
				{
					Control_param_t.Messege_ID =  GB28181_CMD_FORWARD;
					swpa_memcpy(&Control_param_t.sBeginTime,&m_pause_time,sizeof(SWPA_DATETIME_TM));
					swpa_memcpy(&Control_param_t.sEndTime,&Control_param.sEndTime,sizeof(SWPA_DATETIME_TM));
					Control_param_t.sBeginTime.msec = 0;
					Control_param_t.sEndTime.msec = 0;
					
				}
				else
				{
					Control_param_t.Messege_ID =  GB28181_CMD_BACKAWAY;
					swpa_memcpy(&Control_param_t.sBeginTime,&Control_param.sBeginTime,sizeof(SWPA_DATETIME_TM));
					swpa_memcpy(&Control_param_t.sEndTime,&m_pause_time,sizeof(SWPA_DATETIME_TM));
					Control_param_t.sBeginTime.msec = 0;
					Control_param_t.sEndTime.msec = 0;
				}

				Control_param_t.fltScale = m_speed;

				m_historythread.set_Pause(FALSE);

			}



		}
		else if(swpa_strstr(buf,"PAUSE")!= NULL)
		{

			m_historythread.get_Timerecord(&sgetTime);
			swpa_memcpy(&m_pause_time,&sgetTime,sizeof(SWPA_DATETIME_TM));
			Control_param_t.Messege_ID = GB28181_CMD_PAUSE;
			m_historythread.set_Pause(TRUE);

		} 

		SW_TRACE_DEBUG("%s: Messege_ID = %d,Channel_ID = %d, speed = %f,begintime: %04d/%02d/%02d %02d:%02d:%02d:%02d endtime: %04d/%02d/%02d %02d:%02d:%02d:%02d \n",__FUNCTION__,
						Control_param_t.Messege_ID,Control_param_t.Channel_ID,Control_param_t.fltScale,
						Control_param_t.sBeginTime.year,Control_param_t.sBeginTime.month,Control_param_t.sBeginTime.day,
						Control_param_t.sBeginTime.hour,Control_param_t.sBeginTime.min,Control_param_t.sBeginTime.sec,
						Control_param_t.sBeginTime.msec,Control_param_t.sEndTime.year,Control_param_t.sEndTime.month,
						Control_param_t.sEndTime.day,Control_param_t.sEndTime.hour,Control_param_t.sEndTime.min,
						Control_param_t.sEndTime.sec,Control_param_t.sEndTime.msec);
		
		SendRemoteMessage(MSG_GB2818_TRANSFORM_COMMAND,&Control_param_t,sizeof(GB28181_Control_Param));
	  	osip_free(buf);    
	}
}

void CSWGB28181App::on_event()
{
    eXosip_event_t *event; 
	
    if (!(event = eXosip_event_wait(1, 50)))
        return ;

    SW_TRACE_DEBUG("%s: event type = %d \n",__FUNCTION__,event->type);
    printf_osip_message(event->request);

	//	event->request->to->url->username
	//SW_TRACE_DEBUG("username = %s",event->request->to->url->username);
    
    static int sId = event->tid;
    switch (event->type)
    {
    case EXOSIP_REGISTRATION_NEW:					
        SW_TRACE_DEBUG("%s:received new EXOSIP_REGISTRATION\n",__FUNCTION__);
        break;
    case EXOSIP_REGISTRATION_SUCCESS:
        SW_TRACE_DEBUG("%s:EXOSIP_EXOSIP_REGISTRATION_SUCCESS\n",__FUNCTION__);
        on_process_register_success(event);
        break;
    case EXOSIP_REGISTRATION_FAILURE:
    	SW_TRACE_DEBUG("%s:EXOSIP_REGISTRATION_FAILURE\n",__FUNCTION__);
		Sleep(5000);
        if(event->response && event->response->status_code==401)
        {
            osip_message_t *reg = NULL;
            eXosip_lock();
            osip_header_t * h;
            if(osip_message_get_expires(event->request,0,&h)==-1)
            {
                SW_TRACE_DEBUG("401 get expires error! %d\n",__LINE__);
                eXosip_unlock();
                break;
            }
            SW_TRACE_DEBUG("401 Expires:%s\n",SAFE_STR_PTR(h->hvalue));


            INT ret =eXosip_clear_authentication_info();

            GB28181_info info;
            m_cMutexLock.Lock();
            swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
            m_cMutexLock.Unlock();

            
            eXosip_add_authentication_info(info.ipc_username, info.ipc_username, info.ipc_pwd, "MD5", NULL);
            eXosip_register_build_register(event->rid, swpa_atoi(SAFE_STR_PTR(h->hvalue)),&reg);//
            eXosip_register_send_register(event->rid,reg);
            eXosip_unlock();
        }
        break;
    case EXOSIP_REGISTRATION_TERMINATED:
        SW_TRACE_DEBUG("%s:EXOSIP_REGISTRATION terminated\n");
        break;
    case EXOSIP_CALL_INVITE:
		SW_TRACE_DEBUG("CSWGB28181App::on_event: EXOSIP_CALL_INVITE event->tid = %d\n",event->tid);
		SW_TRACE_DEBUG("CSWGB28181App::on_event: EXOSIP_CALL_INVITE event->did = %d\n",event->did);
			
		if(on_process_invite(event)!=0)
		{
		    eXosip_message_send_answer (event->tid, 401, NULL);
		    sId = event->tid;
		}
		break;
    case EXOSIP_CALL_REINVITE:
        SW_TRACE_DEBUG("EXOSIP_EXOSIP_CALL_REINVIT\n");
        break;
    case EXOSIP_CALL_PROCEEDING:
        break;
    case EXOSIP_CALL_RINGING:
        break;
    case EXOSIP_CALL_ANSWERED:
        break;
    case EXOSIP_CALL_REDIRECTED:
        break;
    case EXOSIP_CALL_REQUESTFAILURE:
        break;
    case EXOSIP_CALL_SERVERFAILURE:
        break;
    case EXOSIP_CALL_GLOBALFAILURE:
        break;
    case EXOSIP_CALL_ACK://15
        SW_TRACE_DEBUG("EXOSIP_EXOSIP_CALL_ACK: ACK received for 200ok to INVITE\n");
        break;
    case EXOSIP_CALL_CANCELLED:
        break;
    case EXOSIP_CALL_TIMEOUT:
        break;
    case EXOSIP_CALL_MESSAGE_NEW://18
    	SW_TRACE_DEBUG("EXOSIP_CALL_MESSAGE_NEW\n");
        on_process_message_RTSP(event);
        break;
    case EXOSIP_CALL_MESSAGE_PROCEEDING:
        break;
    case EXOSIP_CALL_MESSAGE_ANSWERED:
        break;
    case EXOSIP_CALL_MESSAGE_REDIRECTED:
        break;
    case EXOSIP_CALL_MESSAGE_REQUESTFAILURE://22
        break;
    case EXOSIP_CALL_MESSAGE_SERVERFAILURE://23
        break;
    case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:
        break;
    case EXOSIP_CALL_CLOSED://25
    	SW_TRACE_DEBUG("CSWGB28181App::on_event: EXOSIP_CALL_CLOSED event->tid = %d\n",event->tid);
        SW_TRACE_DEBUG("CSWGB28181App::on_event: EXOSIP_CALL_CLOSED event->did = %d\n",event->did);	
        on_process_call_close(event);
        break;
    case EXOSIP_CALL_RELEASED:
        SW_TRACE_DEBUG("(对方或服务器正忙!)\n");
        break;
    case EXOSIP_MESSAGE_NEW:
		SW_TRACE_DEBUG("event->tid = %d\n",event->tid);
		SW_TRACE_DEBUG("event->did = %d\n",event->did);
        on_refresh_alive();        
        if(on_process_message(event)!=0)
        {    
        	eXosip_message_send_answer (event->tid, 401,NULL);
        }
        SW_TRACE_DEBUG("%s %d\n",__FUNCTION__,__LINE__);
        break;
    case EXOSIP_MESSAGE_PROCEEDING:
        break;
    case EXOSIP_MESSAGE_ANSWERED:	
        on_process_message_ack(event);
        break;
    case EXOSIP_MESSAGE_REDIRECTED:
        break;
    case EXOSIP_MESSAGE_REQUESTFAILURE: 
        break;
    case EXOSIP_MESSAGE_SERVERFAILURE:
        break;
    case EXOSIP_MESSAGE_GLOBALFAILURE:
        break;
    case EXOSIP_SUBSCRIPTION_UPDATE:
        break;
    case EXOSIP_SUBSCRIPTION_CLOSED:
        break;
    case EXOSIP_SUBSCRIPTION_NOANSWER:
        break;
    case EXOSIP_SUBSCRIPTION_PROCEEDING:
        break;
    case EXOSIP_SUBSCRIPTION_ANSWERED:
        break;
    case EXOSIP_SUBSCRIPTION_REDIRECTED:
        break;
    case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
        break;
    case EXOSIP_SUBSCRIPTION_SERVERFAILURE:
        break;
    case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:
        break;
    case EXOSIP_SUBSCRIPTION_NOTIFY:
        break;
    case EXOSIP_SUBSCRIPTION_RELEASED:
        break;
    case EXOSIP_IN_SUBSCRIPTION_NEW:
        /////////////
        break;
    case EXOSIP_IN_SUBSCRIPTION_RELEASED:
        break;
    case EXOSIP_NOTIFICATION_NOANSWER:
        break;
    case EXOSIP_NOTIFICATION_PROCEEDING:
        break;
    case EXOSIP_NOTIFICATION_ANSWERED:
        break;
    case EXOSIP_NOTIFICATION_REDIRECTED:
        break;
    case EXOSIP_NOTIFICATION_REQUESTFAILURE:
        break;
    case EXOSIP_NOTIFICATION_SERVERFAILURE:
        break;
    case EXOSIP_NOTIFICATION_GLOBALFAILURE:
        break;
    case EXOSIP_EVENT_COUNT:
        break;
    default:
        break;
    }
	
    eXosip_event_free(event);
}

INT CSWGB28181App::OnLoop()
{
    check_connect();
    on_event();
    SW_TRACE_DEBUG("%s %d\n",__FUNCTION__,__LINE__);
    check_keepalive();
    SW_TRACE_DEBUG("%s %d\n",__FUNCTION__,__LINE__);
    on_event();
    SW_TRACE_DEBUG("%s %d\n",__FUNCTION__,__LINE__);
    return 0;
}



HRESULT CSWGB28181App::OnSendAlarm(PVOID pvBuffer, INT iSize)
{
    if(pvBuffer == NULL && iSize != sizeof(GB28181_AlarmInfo))
    {
        SW_TRACE_DEBUG("CSWGB28181App::OnParamterChange Paramter error ");
        return S_FALSE;
    }
    GB28181_AlarmInfo *info = (GB28181_AlarmInfo *)pvBuffer;

    //SW_TRACE_DEBUG("CSWGB28181App::OnSendAlarm %s %s\n",info->szAlarmTime,info->szAlarmMsg);
    send_alarm(info->szAlarmTime,info->szAlarmMsg);
    return S_OK;
}
HRESULT CSWGB28181App::OnGB28181Quit(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("In CSWGB28181:%s \n",__FUNCTION__);
	m_quit = 1;
	return S_OK;
}
HRESULT CSWGB28181App::OnParamterChange(PVOID pvBuffer, INT iSize)
{
    SW_TRACE_DEBUG("%s:Paramter\n ",__FUNCTION__);
    if(pvBuffer == NULL && iSize != sizeof(GB28181_info))
    {
        SW_TRACE_DEBUG("%s:Paramter error\n",__FUNCTION__);
        return S_FALSE;
    }
    m_ParamGet = TRUE;
    GB28181_info *info = (GB28181_info *)pvBuffer;
    m_cMutexLock.Lock();

	SW_TRACE_NORMAL("enable = %d\n",info->enable);
	
    SW_TRACE_DEBUG("%s:server_id = %s\n",__FUNCTION__,info->server_id);
    SW_TRACE_DEBUG("%s:server_region = %s\n",__FUNCTION__,info->server_region);
    SW_TRACE_DEBUG("%s:server_ip = %s\n",__FUNCTION__,info->server_ip);
    SW_TRACE_DEBUG("%s:server_ip = %s\n",__FUNCTION__,info->server_ip);
    SW_TRACE_DEBUG("%s:ipc_id = %s\n",__FUNCTION__,info->ipc_id);
    SW_TRACE_DEBUG("%s:ipc_region = %s\n",__FUNCTION__,info->ipc_region);
    SW_TRACE_DEBUG("%s:ipc_username = %s\n",__FUNCTION__,info->ipc_username);
    SW_TRACE_DEBUG("%s:ipc_pwd = %s\n",__FUNCTION__,info->ipc_pwd);
    SW_TRACE_DEBUG("%s:ipc_alarm = %s\n",__FUNCTION__,info->ipc_alarm);
    SW_TRACE_DEBUG("%s:ipc_port = %s\n",__FUNCTION__,info->ipc_port);
    SW_TRACE_DEBUG("%s:mediaSend_port = %s\n",__FUNCTION__,info->mediaSend_port);
    SW_TRACE_DEBUG("%s:szManufacturer = %s\n",__FUNCTION__,info->szManufacturer);
    SW_TRACE_DEBUG("%s:szModel = %s\n",__FUNCTION__,info->szModel);
    SW_TRACE_DEBUG("%s:szFirmware = %s\n",__FUNCTION__,info->szFirmware);
    SW_TRACE_DEBUG("%s:device_encode = %s\n",__FUNCTION__,info->device_encode);
    SW_TRACE_DEBUG("%s:device_record = %s\n",__FUNCTION__,info->device_record);
    SW_TRACE_DEBUG("%s:szOwner = %s\n",__FUNCTION__,info->szOwner);
    SW_TRACE_DEBUG("%s:szCivilCode = %s\n",__FUNCTION__,info->szCivilCode);
    SW_TRACE_DEBUG("%s:szBlock = %s\n",__FUNCTION__,info->szBlock);
    SW_TRACE_DEBUG("%s:szAddress = %s\n",__FUNCTION__,info->szAddress);
    SW_TRACE_DEBUG("%s:szLongitude = %s\n",__FUNCTION__,info->szLongitude);
    SW_TRACE_DEBUG("%s:szLatitude = %s\n",__FUNCTION__,info->szLatitude);


    if(swpa_strcmp(info->server_id,m_gbinfo.server_id)  ||
            swpa_strcmp(info->server_region,m_gbinfo.server_region) ||
            swpa_strcmp(info->server_ip,m_gbinfo.server_ip) ||
            swpa_strcmp(info->server_port,m_gbinfo.server_port) ||
            swpa_strcmp(info->ipc_id,m_gbinfo.ipc_id) ||
            swpa_strcmp(info->ipc_region,m_gbinfo.ipc_region) ||
            swpa_strcmp(info->ipc_username,m_gbinfo.ipc_username) ||
            swpa_strcmp(info->ipc_pwd,m_gbinfo.ipc_pwd)
            )
    {
        SW_TRACE_DEBUG("%s:the key paramter has change,go ot do_unregister \n",__FUNCTION__);
        this->m_reset = 1;										//传入的参数和原来的不一致，先注销，然后再注册
    }
    SW_TRACE_DEBUG("%s:Manufacturer=%s,Latitude=%s,Longitude=%s \n",__FUNCTION__,info->szManufacturer,info->szLatitude,info->szLongitude);

    swpa_memcpy(&m_gbinfo,info,sizeof(GB28181_info));//TODO LOCK
    m_cMutexLock.Unlock();
    return S_OK;
}

INT CSWGB28181App::ans_deviceRecordInfo__response(LPCSTR sn,CSWDateTime* pdtStartTime,
															CSWDateTime* pdtEndTime)
{

	osip_message_t *message = build_message();
	if(message == NULL)
	{
		SW_TRACE_DEBUG("%s:message = NULL\n",__FUNCTION__);
		return -1;
	}

    CHAR strTemp[10240];			//从文件中读取的最大长度，10k
    swpa_memset(strTemp,0,10240);		
		
    INT numCount = 0;
    DWORD ireadLen = 0;
    INT ifileLen = 0;
    CHAR    *p_str_begin    = NULL;
    CHAR    *p_str_end      = NULL;
    CHAR    *p_Item_begin    = NULL;
    CHAR    *p_Item_end      = NULL;
    
    SW_TRACE_DEBUG("%s:Open  /Result/0/StartEndTime r\n",__FUNCTION__);
    for( int read_i = 0; read_i < 10; read_i++ )
	{
	
		FILE *pDD = fopen("/Result/0/StartEndTime","r");
		if( pDD != NULL )
		{
			fseek(pDD,0L,SEEK_END); 
			ifileLen = ftell(pDD);
			fseek(pDD,0L,SEEK_SET); 

			SW_TRACE_DEBUG("%s:the length of /Result/0/StartEndTime = %d \n",__FUNCTION__,ifileLen);

			ireadLen = fread(strTemp,1,ifileLen,pDD);
			SW_TRACE_DEBUG("%s:pDD = %d,ireadLen=%d,errno=%d\n",__FUNCTION__,pDD,ireadLen,errno);
			SW_TRACE_DEBUG("strTemp=%s\n",strTemp);
			fclose(pDD);
			pDD = NULL;
		}
		if( ireadLen >= ifileLen && ireadLen > 0 ) 
		{
			break;
		}
	}
		
	if( ireadLen <= 82)			//没有存放开始时间和结束时间
	{
		SW_TRACE_DEBUG("Read  not  \n");
    	return -1; 
	}
		  
	GB28181_info info;
	m_cMutexLock.Lock();
	swpa_memcpy(&info,&m_gbinfo,sizeof(GB28181_info));
	m_cMutexLock.Unlock();

	CHAR Item[1024];
	swpa_memset(Item,0,1024);
	INT istep = 0;
	CHAR *p_strStart = NULL;
	INT ICount = 0;
	INT iSum = 0;
		
	p_strStart = strTemp;
	SW_TRACE_DEBUG("%s:p_strStart=%s\n",__FUNCTION__,p_strStart);
			
	while( NULL != swpa_strstr(p_strStart, "<StartTime>") )		//统计录像的个数	
	{
		CHAR sStartTime[64];
		CHAR sEndTime[64];
		swpa_memset(sStartTime,0,sizeof(sStartTime));
		swpa_memset(sEndTime,0,sizeof(sEndTime));
		p_str_begin = swpa_strstr(p_strStart, "<StartTime>"); 
		p_str_end   = swpa_strstr(p_strStart, "</StartTime>");
		if(p_str_begin != NULL && p_str_end != NULL)
		{	
			swpa_memcpy(sStartTime, p_str_begin+11, p_str_end-p_str_begin-11);
			p_Item_begin = p_str_begin;
		}
		else
		{
			SW_TRACE_DEBUG("Find <StartTime> err\n");
			break; 
		}
		
		p_str_begin = swpa_strstr(p_strStart, "<EndTime>"); 
		p_str_end   = swpa_strstr(p_strStart, "</EndTime>");
		if(p_str_begin != NULL && p_str_end != NULL)
		{
			swpa_memcpy(sEndTime, p_str_begin+9, p_str_end-p_str_begin-9);
			p_Item_end = p_str_end;
		}
		else
		{
			SW_TRACE_DEBUG("Find <EndTime> err\n");
			break;
		}
          
		SWPA_DATETIME_TM tttStartTime,tttEndTime;
		SW_TRACE_DEBUG("sStartTime=%s,sEndTime=%s\n",sStartTime,sEndTime);
      
		swpa_sscanf(sStartTime,"%d-%d-%dT%d:%d:%d",
			&tttStartTime.year,&tttStartTime.month,&tttStartTime.day,
			&tttStartTime.hour,&tttStartTime.min,&tttStartTime.sec);
		swpa_sscanf(sEndTime,"%d-%d-%dT%d:%d:%d",
			&tttEndTime.year,&tttEndTime.month,&tttEndTime.day,
			&tttEndTime.hour,&tttEndTime.min,&tttEndTime.sec);
      	
		SW_TRACE_DEBUG("tttStartTime %d-%d-%dT%d:%d:%d \n",
			tttStartTime.year,tttStartTime.month,tttStartTime.day,
			tttStartTime.hour,tttStartTime.min,tttStartTime.sec);
			
		SW_TRACE_DEBUG("tttEndTime %d-%d-%dT%d:%d:%d \n",
			tttEndTime.year,tttEndTime.month,tttEndTime.day,
			tttEndTime.hour,tttEndTime.min,tttEndTime.sec);
			
		CSWDateTime  dtItemStartTime(&tttStartTime),dtItemEndTime(&tttEndTime);
      
		if( (*pdtStartTime < dtItemStartTime && dtItemEndTime < *pdtEndTime )
		|| ( *pdtStartTime == dtItemStartTime && dtItemEndTime < *pdtEndTime )
		|| ( *pdtStartTime < dtItemStartTime && dtItemEndTime == *pdtEndTime )
		|| ( *pdtStartTime == dtItemStartTime && dtItemEndTime == *pdtEndTime ))
		{
			ICount++;														
		}
		
		istep = p_Item_end-p_Item_begin+10;
		p_strStart = p_strStart + istep+2;
	}


	iSum = ICount;
	istep = 0;
	ICount = 0;
	p_strStart = strTemp;
	swpa_memset(Item,0,1024);
	SW_TRACE_DEBUG("%s:the number of record = %d \n",__FUNCTION__,iSum);

	while( NULL != swpa_strstr(p_strStart, "<StartTime>") )
	{
		CHAR temp[128];
		swpa_memset(temp,0,128);

		CHAR sStartTime[64];
		CHAR sEndTime[64];
		swpa_memset(sStartTime,0,sizeof(sStartTime));
		swpa_memset(sEndTime,0,sizeof(sEndTime));
		p_str_begin = swpa_strstr(p_strStart, "<StartTime>"); 
		p_str_end   = swpa_strstr(p_strStart, "</StartTime>");
		if(p_str_begin != NULL && p_str_end != NULL)
		{	
			swpa_memcpy(sStartTime, p_str_begin+11, p_str_end-p_str_begin-11);
			p_Item_begin = p_str_begin;
		}
		else
		{
			SW_TRACE_DEBUG("Find <StartTime> err\n");
			break; 
		}
		
		p_str_begin = swpa_strstr(p_strStart, "<EndTime>"); 
		p_str_end   = swpa_strstr(p_strStart, "</EndTime>");
		if(p_str_begin != NULL && p_str_end != NULL)
		{
			swpa_memcpy(sEndTime, p_str_begin+9, p_str_end-p_str_begin-9);
			p_Item_end = p_str_end;
		}
		else
		{
			SW_TRACE_DEBUG("Find <EndTime> err\n");
			break;
		}
	  
		SWPA_DATETIME_TM tttStartTime,tttEndTime;
		SW_TRACE_DEBUG("sStartTime=%s,sEndTime=%s\n",sStartTime,sEndTime);

		swpa_sscanf(sStartTime,"%d-%d-%dT%d:%d:%d",
			&tttStartTime.year,&tttStartTime.month,&tttStartTime.day,
			&tttStartTime.hour,&tttStartTime.min,&tttStartTime.sec);
		swpa_sscanf(sEndTime,"%d-%d-%dT%d:%d:%d",
			&tttEndTime.year,&tttEndTime.month,&tttEndTime.day,
			&tttEndTime.hour,&tttEndTime.min,&tttEndTime.sec);
			
		SW_TRACE_DEBUG("tttStartTime %d-%d-%dT%d:%d:%d \n",
			tttStartTime.year,tttStartTime.month,tttStartTime.day,
			tttStartTime.hour,tttStartTime.min,tttStartTime.sec);
			
		SW_TRACE_DEBUG("tttEndTime %d-%d-%dT%d:%d:%d \n",
			tttEndTime.year,tttEndTime.month,tttEndTime.day,
			tttEndTime.hour,tttEndTime.min,tttEndTime.sec);
			
		CSWDateTime  dtItemStartTime(&tttStartTime),dtItemEndTime(&tttEndTime);

	   if( (*pdtStartTime < dtItemStartTime && dtItemEndTime < *pdtEndTime )
   		|| ( *pdtStartTime == dtItemStartTime && dtItemEndTime < *pdtEndTime )
   		|| ( *pdtStartTime < dtItemStartTime && dtItemEndTime == *pdtEndTime )
   		|| ( *pdtStartTime == dtItemStartTime && dtItemEndTime == *pdtEndTime )
   		)
		{
			swpa_memcpy(temp, p_Item_begin, p_Item_end-p_Item_begin+10);

			swpa_sprintf(Item,"%s<Item>\r\n"
							 "<DeviceID>%s</DeviceID>\r\n"
							 "<Name>1310000002</Name>\r\n"
							 "<FilePath>%d1310000002</FilePath>\r\n"
							 "<Address>Address1</Address>\r\n"
							 "%s\r\n"
							 "<Secrecy>0</Secrecy>\r\n"
							 "<Type>all</Type>\r\n"
							 "<RecorderID>%d1310000002</RecorderID>\r\n"
							 "</Item>\r\n"
							 ,Item
							 ,"34020000001310000001"//,info.ipc_id
							 ,ICount
							 ,temp
							 ,ICount);
			SW_TRACE_DEBUG("Item=%s\n",Item);
				
			ICount++;

			INT iSelNum = 2;
			if( ( ICount%iSelNum == 0 ) || ( ICount%iSelNum != 0 && iSum == ICount ))
			{
				INT Inum = (ICount%iSelNum == 0) ? iSelNum : iSum%iSelNum;
				message = build_message();

				CHAR tmp[4096];
				swpa_memset(tmp,0,4096);
				swpa_snprintf (tmp, 4096,
									"<?xml version=\"1.0\"?>\r\n"
									"<Response>\r\n"
									"<CmdType>RecordInfo</CmdType>\r\n"/*命令类型*/	                
									"<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/ //info.ipc_id
									"<SumNum>%d</SumNum>\r\n"  //ICount
									"<SN>%s</SN>\r\n"/*命令序列号*/  //sn
									"<RecordList Num=\"%d\">\r\n"  //ICount
									"%s"                        //Item
									"</RecordList>\r\n"
									"</Response>\r\n",
									info.ipc_id,
									iSum,
									sn,
									Inum,
									Item
									);

				SW_TRACE_DEBUG("send RecordInfo=%s\n",tmp);
				osip_message_set_body (message, tmp, swpa_strlen(tmp));
				osip_message_set_content_type (message,"Application/MANSCDP+xml");
				eXosip_message_send_request(message);
				swpa_memset(Item,0,1024);
			}
		}
	   
		istep = p_Item_end-p_Item_begin+10;
		p_strStart = p_strStart + istep+2;
	}
	  
	SW_TRACE_DEBUG("%s:query RecordInfo success \n",__FUNCTION__);
	return 0;
}
