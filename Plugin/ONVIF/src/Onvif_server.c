
/***********************************************************************

/*
 * onvif_server.c
 *
 *  Created on: 2014年7月14日
 *      Author: Yinj
 */
#include <stdio.h>
#include <pthread.h>
#include "stdsoap2.h"
#include "nsmap.h"
#include <uuid/uuid.h>
#include "onvifLib.h"
#include "onvif_function.h"
#include "httpda.h"

P_Trace g_Trace = NULL;
int size1=1;
int g_Enable = 0;
int g_AuthenticateEnable = 0;

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int idiscover_thread;
void* Fun_thread_discover(void* arg)
{
	struct soap soap;
	struct ip_mreq mcast;

	soap_init2(&soap, SOAP_IO_UDP|SOAP_IO_FLUSH, SOAP_IO_UDP|SOAP_IO_FLUSH);

	soap_set_namespaces(&soap, namespaces);
	//soap_set_mode(&soap, SOAP_C_UTFSTRING);

    printf("Fun_thread_discover g_onvif_dicover=%d\n",g_onvif_dicover);
	if(!soap_valid_socket(soap_bind(&soap, NULL, g_onvif_dicover, 10)))
	{
		soap_print_fault(&soap, stderr);
	}

	//mcast.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
	//mcast.imr_interface.s_addr = htonl(INADDR_ANY);
    mcast.imr_multiaddr.s_addr = htonl(inet_network("239.255.255.250"));
	mcast.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(soap.master, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast)) < 0)
	{
		printf("setsockopt error!\n");
		perror("239.255.255.250 - setsockopt error!");
		return 0;
	}

	while(idiscover_thread)
	{
//		pthread_mutex_lock(&mutex);

		printf("Accepting requests...\n");
		if( g_Trace != NULL )
        {
            g_Trace(5,"<onvif> Accepting requests...\n");
        }
		if(soap_serve(&soap))
		{
			soap_print_fault(&soap, stderr);
		}

		soap_destroy(&soap);
		soap_end(&soap);

//		pthread_mutex_unlock(&mutex);
	}

	soap_done(&soap);


	printf("---------Fun_thread_discover  exit");

	pthread_exit(0);
}



int iresponse_thread;
void* Fun_thread_response(void* arg)
{
	int m, s ,i = 0;
	struct soap add_soap;
	soap_init(&add_soap);
	soap_set_namespaces(&add_soap, namespaces);
	soap_register_plugin(&add_soap, http_da);     // http digest 鉴权方式插件注册
	
	m = soap_bind(&add_soap, NULL, g_onvif_response_port, 100);
	if (m < 0)
	{
		soap_print_fault(&add_soap, stderr);
		iresponse_thread = 0;
		exit(1);
	}
	fprintf(stderr, "Socket connection successful:master socket = %d\n", m);

	printf("enty Fun_thread_response\n");
	pthread_mutex_t mutex;
	while(iresponse_thread == 1 )
	{
		pthread_mutex_lock(&mutex);
		soap_set_namespaces(&add_soap, namespaces);
		s = soap_accept(&add_soap);
		if (s < 0)
		{
			soap_print_fault(&add_soap, stderr);
			fprintf(stderr, "Socket connection faild\n");
		}
		else
		{
			fprintf(stderr, "soap_accept from %s:%d\n", inet_ntoa(add_soap.peer.sin_addr), ntohs(add_soap.peer.sin_port));
			if( g_Trace != NULL )
	       	{
	                g_Trace(5,"<onvif> soap_accept from %s:%d\n", inet_ntoa(add_soap.peer.sin_addr), ntohs(add_soap.peer.sin_port));
	              }
		}
		
		fprintf(stderr,"---------------------------------------socket = %d, accepting the %d time\n", s, i++);
		if( g_Trace != NULL )
		{
	            g_Trace(5,"<onvif> ---------------------------------------socket = %d, accepting the %d time\n", s, i++);
		}
		soap_serve(&add_soap);
		soap_end(&add_soap);
//		soap_free(&add_soap);// done and free context

		// 在不断有onvif请求时不释放socket会导致设备无法打开文件（写心跳文件）而崩溃
		fprintf(stderr,"---------------------------------------closing socket %d\n", s);
		if( g_Trace != NULL )
		{
           		 g_Trace(5,"<onvif> ---------------------------------------closing socket %d\n", s);
		}
		close(s);
		usleep(50000); // close后需sleep 50ms

		pthread_mutex_unlock(&mutex);
	}
	printf("---------Fun_thread_response  exit");
	pthread_exit(0);
}




pthread_t g_tid_discover = 0;
pthread_t g_tid_response = 0;

//这个startOnvif函数应该在设备端中被调用，里面的形参应该是调用的时候被设备传入
int StartOnvif( int ivideoFramerate, int ionvifResponsePort, int ionvifDiscover, int irtspPort, P_Trace  prt ,int* ipAvg)
{
    if( g_tid_discover != 0 || g_tid_response != 0)
    {
        return -1;
    }

	if (NULL == ipAvg)
	{
		printf("StartOnvif -> ipAvg is NULL \n");
		return -1;
	}


	g_Enable = ipAvg[0];
	g_AuthenticateEnable = ipAvg[1];


	printf("StartOnvif--g_Enable:%d,g_AuthenticateEnable:%d \n",g_Enable,g_AuthenticateEnable);
	

    
    g_video_framerate = ivideoFramerate;
    g_onvif_response_port = ionvifResponsePort;
    g_onvif_dicover = ionvifDiscover;
    g_rtsp_port = irtspPort;
    g_Trace = prt;

    printf("StartOnvif g_onvif_dicover=%d\n",g_onvif_dicover);
    printf("StartOnvif g_onvif_response_port=%d\n",g_onvif_response_port);
    printf("StartOnvif g_rtsp_port=%d\n",g_rtsp_port);
    printf("StartOnvif g_video_width=%d\n",g_video_width);
    printf("StartOnvif g_video_height=%d\n",g_video_height);
    printf("StartOnvif g_video_framerate=%d\n",g_video_framerate);
    if( g_Trace != NULL )
    {
        g_Trace(5,"<onvif> StartOnvif g_onvif_dicover=%d\n",g_onvif_dicover);
        g_Trace(5,"<onvif> StartOnvif g_onvif_response_port=%d\n",g_onvif_response_port);
        g_Trace(5,"<onvif> StartOnvif g_rtsp_port=%d\n",g_rtsp_port);
        g_Trace(5,"<onvif> StartOnvif g_video_width=%d\n",g_video_width);
        g_Trace(5,"<onvif> StartOnvif g_video_height=%d\n",g_video_height);
        g_Trace(5,"<onvif> StartOnvif g_video_framerate=%d\n",g_video_framerate);
    }

    int err;
    idiscover_thread = 1;
    err = pthread_create(&g_tid_discover, NULL, Fun_thread_discover, NULL);
    if( err != 0 )
    {
        idiscover_thread = 0;
        printf(".....StartOnvif  fail\n");
        return err;
    }

    iresponse_thread = 1;
    err = pthread_create(&g_tid_response, NULL, Fun_thread_response, NULL);
    if( err != 0 )
    {
        iresponse_thread = 0;
        pthread_cancel(g_tid_discover);
        printf(".....StartOnvif  fail\n");
        return err;
    }
    printf(".....StartOnvif\n");
    return 0;
}

int StartOnvif_One( int iVideoWidth, int iVideoHeight)
{
	
    g_video_width = iVideoWidth;
    g_video_height = iVideoHeight;


	return 0;
}



int StartOnvif_EX( int iVideoWidth, int iVideoHeight)
{
	

	size1=2;


    g_video_width_ex=iVideoWidth;
	g_video_height_ex=iVideoHeight;


	return 0;
}


int StopOnvif( )
{
    int err;
    if(g_tid_discover != 0 )
    {
        idiscover_thread = 0;
        err = pthread_cancel(g_tid_discover);
        if( err != 0 )
        {
            return err;
        }
    }

    if(g_tid_response != 0 )
    {
        iresponse_thread = 0;
        err = pthread_cancel(g_tid_response);
        if( err != 0 )
        {
            return err;
        }
    }

	printf(".....StopOnvif\n");
    g_tid_discover = 0;
    g_tid_response = 0;
    //exit(254);
    return 0;
}


static UserManageCallback g_fUserManageCallback = 0;
int RegisterUserManage(UserManageCallback pUser)
{
	if (NULL == pUser)
	{
		printf("err: invalid callback func handle.\n");
		return -1;
	}

	if (0 == g_fUserManageCallback)
	{
		g_fUserManageCallback = pUser;
		
		return 0;
	}
	else
	{
		printf("err: callback func registered already.\n");
		return -1;
	}
}


int UserManage(const int len, void* pvArg)
{
	if (0 != g_fUserManageCallback)
	{
		return g_fUserManageCallback(len, pvArg);
	}

	return -1;
}


static GetUserCallback g_fGetUserCallback = 0;
int ReggisterGetUser(GetUserCallback pGetUser)
{
	if (NULL == pGetUser)
	{
		printf("err: invalid callback func handle.\n");
		return -1;
	}

	if (0 == g_fGetUserCallback)
	{
		g_fGetUserCallback = pGetUser;
		
		return 0;
	}
	else
	{
		printf("err: callback func registered already.\n");
		return -1;
	}
}

int GetUser(void* psize, void* pvArg)
{
	if (0 != g_fGetUserCallback)
	{
		return g_fGetUserCallback(psize, pvArg);
	}

	return -1;
}



static PTZControlCallback g_fPTZCallback = 0;
int RegisterPTZCallback(PTZControlCallback pfPTZ)
{
	if (NULL == pfPTZ)
	{
		printf("err: invalid callback func handle.\n");
		return -1;
	}

	if (0 == g_fPTZCallback)
	{
		g_fPTZCallback = pfPTZ;
		
		return 0;
	}
	else
	{
		printf("err: callback func registered already.\n");
		return -1;
	}
}


int PTZControl(const int iCmdID, void* pvArg)
{
	if (0 != g_fPTZCallback)
	{
		return g_fPTZCallback(iCmdID, pvArg);
	}

	return -1;
}


static ONVIF_PTZ_PARAM g_sPTZParam = {0};

int SetPTZParam(const ONVIF_PTZ_PARAM* psParam)
{
	if (NULL == psParam
		|| !psParam->fInited)
	{
		return -2;
	}

	//todo : lock first?? 
	
	g_sPTZParam.fInited = 0; //

	memcpy(&g_sPTZParam, psParam, sizeof(g_sPTZParam));

	g_sPTZParam.fInited = 1;

	return 0;
}

int GetPTZParam(ONVIF_PTZ_PARAM* psParam)
{
	if (NULL == psParam)
	{
		return -2;
	}

	if (g_sPTZParam.fInited)
	{
		memcpy(psParam, &g_sPTZParam, sizeof(*psParam));
	}

	if (!g_sPTZParam.fInited)
	{
		return -1;
	}

	return 0;
}


