/*
 * DSPLink.c
 *
 *  Created on: 2009-9-4
 *      Author: Administrator
 */
#include <std.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sem.h>
#include <lck.h>
#include <tsk.h>
#include <csl_intc.h>
#include <csl_cache.h>
#include "DSPLink.h"	

static DSPLinkObj linkObj;
static void tskMessageDispatch(MsgDispatch dispatch);

extern int extHeap;
extern int intHeap;

void DSPLinkSetup(MsgDispatch dispatch)
{
	TSK_Attrs taskAttr = TSK_ATTRS;
	TSK_Handle taskHandle = NULL;

	linkObj.armReg = (DSPLinkRegHandle)DSPLINK_MEM_ARM;
	linkObj.dspReg = (DSPLinkRegHandle)DSPLINK_MEM_DSP;
	linkObj.trigerInt = DSPINTARM;
	linkObj.armReg->status = VALID;
	linkObj.dspReg->status = VALID;

	linkObj.mutex  = LCK_create(NULL);
	linkObj.sem_msg= SEM_create(0,NULL);

	/*
	create TSK Receive thread
	taskAttr.priority  = 3;
	taskAttr.stacksize = TASK_STACK_SIZE;
	*/

	taskAttr.priority = 6;
	taskAttr.stacksize = 32760;
	taskAttr.stackseg = extHeap;
	taskAttr.stack = NULL;
	taskAttr.environ = NULL;
	taskAttr.name = "Process Cmd thread";
	taskAttr.exitflag = TRUE;

	taskHandle = TSK_create(tskMessageDispatch, &taskAttr, (void*)dispatch);
	if ( NULL != taskHandle )
	{
		//indicat the arm the dsp routine has boot
		DSPLinkWrite(0);
	}
}

void tskMessageDispatch(MsgDispatch dispatch)
{
	Uint32 msg;
	DSPLinkBuffer *bufs;
	DSPLinkBuffer *input;
	DSPLinkBuffer *output;
	unsigned char *inbuf;
	unsigned char *outbuf;
	unsigned int inlen;
	unsigned int outlen;

	printf("hello dsplink.\n");

	while(1)
	{
		while((*((volatile unsigned int *)DSP5HWIREG))==0);
	  	*((volatile unsigned int *)DSPINTCLR)|=0x1;

		CACHE_wbInvAllL2(CACHE_WAIT);
		msg = linkObj.dspReg->value; //read the arm's message
		
		if(dispatch)
		{
			bufs   = (DSPLinkBuffer *)msg;
			input  = &bufs[0];
			output = &bufs[1];
			inbuf  = (unsigned char *)input->phys;
			outbuf = (unsigned char *)output->phys;
			inlen  = input->len;
			outlen = output->len;

			dispatch(inbuf, inlen, outbuf, outlen);
		}

		CACHE_wbInvAllL2(CACHE_WAIT);
		DSPLinkWrite(msg);
	}
}

/**
 * @brief 向ARM发送信息
 * @param value 发送的值
 * @return ==0 成功, !=0 失败
 */
int DSPLinkWrite(Uint32 value)
{
	//set message
	linkObj.armReg->value = value;

	//trigger arm
	*(volatile Uint32*)linkObj.trigerInt |= 0x1;

	return 0;
}
