/**
   Author:	wujf
   Date:	2015/03/25
*/
#ifndef __SWMUXER_H__
#define __SWMUXER_H__


#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "SWFC.h"
#include "swpa.h"
#include "PS_Define.h"

//Stream ID
#define PROGRAM_STREAM_MAP_ID	(0xBC)
#define PRIVATE_STREAM_1_ID		(0xBD)
#define PADDING_STREAM_ID		(0xBE)
#define PRIVATE_STREAM_2_ID		(0xBF)
#define AUDIO_STREAM_ID(N)		(0xC0 + N)	//110x xxxx audio stream number x xxxx
#define VIDEO_STREAM_ID(N)		(0xE0 + N)  //1110 xxxx video stream number xxxx
#define ECM_STREAM_ID			(0xF0)
#define EMM_STREAM_ID			(0xF1)
#define DSM_CC_STREAM_ID		(0xF2)
#define ISO_IEC_13522_STREAM_ID (0xF3)
#define RESERVED_STREAM_ID(N)	(0xF0 + N)  //0100 - 1110 are reserved
#define PROGRAM_STREAM_DIRECTORY (0xFF)

/**
   数据块类型，其中前3种类型和RtpParser传入类型一致
*/
typedef enum _BLOCK_TYPE_T
{
	BLOCK_TYPE_H264_H265_P_FRAME = 0,
	BLOCK_TYPE_H264_H265_IDR_FRAME,
	BLOCK_TYPE_H264_H265_B_FRAME,
	BLOCK_TYPE_PES_HEADER,
	BLOCK_TYPE_PSM_HEADER,
	BLOCK_TYPE_SYS_HEADER,
	BLOCK_TYPE_PACK_HEADER,
	BLOCK_TYPE_TS_HEADER,
	BLOCK_TYPE_RTP_HEADER,
	BLOCK_TYPE_UNDEFINE,
}BLOCK_TYPE_T;

//数据块链表
typedef struct _BLOCK_T BLOCK_T;
struct _BLOCK_T
{
	BLOCK_T *pNext;
	BLOCK_T *pPrev;
	CHAR	*pBuffer;
	unsigned int	iBuffer;
	BLOCK_TYPE_T	iType;
};

//按位写入缓存结构
typedef struct _BIT_BUFFER_T
{
    unsigned int    iSize;
    unsigned int    iData;
    unsigned char 	iMask;
    unsigned char 	*pData;
} BIT_BUFFER_T;


class CSWMuxer 
{
public:
	CSWMuxer();
	virtual ~CSWMuxer();

/**
   @brief 初始化复用器
   @param [in] pParam 根据不同复用器使用不同参数
*/
	virtual INT InitMuxer(VOID *pParam);

/**
   @brief 复用函数
   @param [in] pBlock 待复用数据块
*/
	virtual INT Mux(BLOCK_T **pBlock);

public:
/**
   @brief 初始化按位写入的缓存
   @param [in] pBuffer 待写入的缓存结构
   @param [in] iSize 待写入的缓存大小
   @param [in] pData 待写入的缓存
*/
	INT BitInitWrite(BIT_BUFFER_T *pBuffer, INT iSize, VOID *pData);

/**
   @brief 按位写入缓存
   @param [in] pBuffer 待写入的缓存结构
   @param [in] iCount 写入多少比特位
   @param [in] iBits 写入的值
*/
	VOID BitWrite(BIT_BUFFER_T *pBuffer, INT iCount, unsigned long int iBits);

/**
   @brief 添加数据块到链表
   @param [in] [out] pBlock 待添加的链表，添加后的链表
   @parma [in] pAppBlock 要添加的数据块
*/
	VOID BlockChainAppend(BLOCK_T **pBlock, BLOCK_T *pAppBlock);

/**
   @brief 释放数据块链表,所有数据都释放
   @param [in] pBlock 释放的数据块链表
*/
	VOID BlockChainFree(BLOCK_T *pBlock);
};

#endif 
