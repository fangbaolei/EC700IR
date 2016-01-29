/*
 * message.c
 *
 *  Created on: 2009-9-5
 *      Author: Administrator
 */
#include <std.h>
#include <stdio.h>
#include <stdlib.h>
#include <tsk.h>
#include <sem.h>
#include <DSPLink.h>
#include "DmaCopyApi.h"

/**
 * @brief 回调函数
 * @param inbuf 输入数据
 * @param inlen 输入数据的长度
 * @param outbuf 输出数据
 * @param outlen 输出数据的长度
 * @return 无
 */
void MessageProc(
	unsigned char *inbuf,
	unsigned int inlen,
	unsigned char *outbuf,
	unsigned int outlen
)
{
	static int iFlagInitDma = 0;
	if ( 0 == iFlagInitDma )
	{
		iFlagInitDma = 1;
	}

	ProcessDspCmd(inbuf, inlen, outbuf, outlen);
}
