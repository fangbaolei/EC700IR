/*
 * DSPLink.h
 *
 *  Created on: 2009-9-4
 *      Author: Administrator
 */

#ifndef DSPLINK_H_
#define DSPLINK_H_

#include "DSPLinkType.h"

#ifdef _ARM_APP_
#include <cmem.h>
#include "slw.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DSPLinkBuffer_ {
	void *addr; // virtual address for linux(arm)
	unsigned int phys; // physics address for dsp
	unsigned int len;  // len of the buffer
}DSPLinkBuffer;

/*Driver memeobj*/
typedef struct _tagDSPLinkMem{
	Uint32	addr;
	Uint32	value;
	Uint32	size;
}DSPLinkMem,*DSPLinkMemHandle;

/*DSP Link resgister*/
typedef struct _tagDSPLinkReg{
	Uint32	value;
	Uint32	type;
	Uint32	status;
}DSPLinkReg,*DSPLinkRegHandle;

enum{
	INVALID = 0,
	VALID
};

#ifndef _ARM_APP_
typedef struct _tagDSPLinkObj{
#ifndef _DSP_
	/*ARM side*/
	struct 	semaphore	mutex;
	SEM_Obj				sem_msg;
#else

	#ifndef _NO_DSPBIOS_
	/*DSP side*/
	LCK_Handle			mutex;
	SEM_Handle			sem_msg;
	#endif

#endif /*_DSP_*/
	Uint32				trigerInt;
	DSPLinkRegHandle	armReg;
	DSPLinkRegHandle	dspReg;
}DSPLinkObj,*DSPLinkHandle;
#endif /*_ARM_APP_*/

/* DSPLink CMD */
enum{
	SET_TRI_INT = 0,
	SET_ARM_MSG_REG,
	SET_DSP_MSG_REG,
	GET_REG,
	SET_REG,
	DATA_COPY,
	GET_STATUS
};

/*ARM BOOT DSP Register*/
#define		DSPLINK_PTSTAT			(0x01C41128)
#define		DSPLINK_MDCTL1			(0x01C41A04)
#define		DSPLINK_PTCMD			(0x01C41120)
#define		DSPLINK_MDSTAT1			(0x01C41804)
#define		DSPLINK_MDSTAT2			(0x01C41808)
#define		DSPLINK_MDSTAT3			(0x01C4180C)
#define		DSPLINK_DSPBOOTADDR		(0x01C40008)

/*
 * User need update this section sync to DSP/BIOS mem
 */
#define		TASK_STACK_SIZE	(4096) // DSP任务的stack size
#define 	DSP_ENTRY_POINT	(0x8FFFF000) // OUT文件加载和运行的地址

#ifndef _NO_DSPBIOS_
    #ifndef _CAM_APP_
        #define 	DSPLINK_MEM_ARM	(0x11827100)
        #define		DSPLINK_MEM_DSP	(0x11827200)
	#else
        /* CamApp's Arm side*/
        #define 	DSPLINK_MEM_ARM	(0x1182f000)
        #define		DSPLINK_MEM_DSP	(0x1182f800)
	#endif
#else
    /* CamApp's Dsp side*/
	#define 	DSPLINK_MEM_ARM	(0x1182f000)
	#define		DSPLINK_MEM_DSP	(0x1182f800)
#endif

/*ARM <-> DSP interrupt register*/
#define 	DSPINTARM		(0x01c40074)
#define		ARMINTDSP		(0x01C40064)
#define		DSPINTCLR		(0x01c40068)

//DSP端的5号硬中断寄存器地址。注：由于TI的H.264库的原因，无法响应中断，只能采用轮询中断寄存器的方式。
#define 	DSP5HWIREG		(0x01c40060)


/**
 * @brief DSP的回调函数
 * @param inbuf 输入数据
 * @param inlen 输入数据的长度
 * @param outbuf 输出数据
 * @param outlen 输出数据的长度
 * @return 无
 */
typedef void (*MsgDispatch)(unsigned char *inbuf,
	unsigned int inlen,
	unsigned char *outbuf,
	unsigned int outlen);

#ifndef _DSP_
/**
 * @brief 初始化DSP并加载OUT文件到DSP运行
 * @param OUT文件的文件名
 * @return 无
 */
void DSPLinkSetup(char *DspFile);

/**
 * @brief 调用DSP处理
 * @param input 输入数据的结构体
 * @param output 输入出数据的结构体
 * @param timeout 超时时间
 * @return ==0 成功, !=0 失败
 */
int do_process(DSPLinkBuffer *input, DSPLinkBuffer *output, long timeout_ms);
#else
/**
 * @brief 处理DSP与ARM的通信,当ARM调用DSP时就调用回调函数
 * @param dispatch 回调函数
 * @return 无
 */
void DSPLinkSetup(MsgDispatch dispatch);
#endif /* _DSP_ */

int CreateDSPBuffer(DSPLinkBuffer *obj, Uint32 len);
void FreeDSPBuffer(DSPLinkBuffer *obj);
void ExitDSPLink(void);

#ifdef __cplusplus
}
#endif

#endif /* DSPLINK_H_ */
