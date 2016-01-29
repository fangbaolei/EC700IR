/***********************************************************************
** Copyright (C) GUOG. All rights reserved.
** Author			: guog
** Date				: 2013.09.17
** Name				: onvif_function.h
** Version			: 1.0
** Description			: 定义一些onvif模块需要用到的功能函数
** Modify Record		:
1:Creat on 2013.09.17

***********************************************************************/

#ifndef _ONVIF_FUNCTION_H_
#define _ONVIF_FUNCTION_H_



extern int size1;		//profiles的大小默认为1，有两路H264码流时设置为2

int g_video_width;
int g_video_width_ex;
int g_video_height;
int g_video_height_ex;

int g_video_framerate;
int g_onvif_response_port;
int g_onvif_dicover;
int g_rtsp_port;


int netGetIp(char *pInterface, unsigned int *ip);

int netGetMac(char *pInterface, unsigned char *pMac);


#endif

