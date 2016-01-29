#ifndef __SWMESSAGESTRUCT_H__
#define __SWMESSAGESTRUCT_H__

/******************************************************************************

				  版权所有 (C), 2015-2023, sw

 ******************************************************************************
  文 件 名   : SWMessageStruct.h
  版 本 号   : 初稿
  作	者   : 姓名
  生成日期   : 2015年8月14日
  最近修改   :
  功能描述   : 控制消息相关的结构体定义
  函数列表   :
  修改历史   :
  1.日    期   : 2015年8月14日
	作    者   : 姓名
	修改内容   : 初稿

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件   								*
 *----------------------------------------------*/
#include "SWFC.h"

/*----------------------------------------------*
 * 宏定义   							  *
 *----------------------------------------------*/
#define MSG_SHM_PROCESS_LEN   200	  //应用程序名的最大长度
#define MSG_SHM_KEY_FILE_LEN  (MSG_SHM_PROCESS_LEN+5)  //key文件的最大长度

/*----------------------------------------------*
 * 常量定义 							*
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量   						  *
 *----------------------------------------------*/

/* BEGIN: Modified by huangdch, 2015/9/5 
*这个值需要根据平台需要进行修改,最大支持6个*/
#define MSG_SHM_PROCESS_NUMBER	    6                     //需要进程间交换的最大进程数
#define MSG_SHM_NODE_NUMBER 	6   					  //每个应用程序所建的共享内存节点数
#define MSG_SHM_SEM_TOTAIL  	(MSG_SHM_NODE_NUMBER*2+2) //信号总数

/*只在消息最大和最小值范围对应的共享内存key值
通过Id的编号可以查找到对应的共享内存快的key值
*/
typedef struct _STMessageRangNode
{
	DWORD dwKey; //共享内存key值
	DWORD dwStartID; //消息Id的min值
	DWORD dwEndID; //消息Id的max值
}STMessageRangNode;

/*平台相关的进程消息节点:记录整个平台各个进程的相关信息
*/
typedef struct _STProcessManageNode
{
	DWORD iProcessId; //进程Id,可以在接收到广播消息后更新这个字段,目前未用到
	CHAR szProcessName[MSG_SHM_PROCESS_LEN];//应用程序名称
	DWORD dwKey; //根据这个key值可以查找到对应的共享内存块的消息
	void* pShm; //共享内存块连接指针 dwKey
	CSWMutex* pMutex; //锁的连接指针 
	CSWSemaphore* pSem[MSG_SHM_SEM_TOTAIL];  //信号量的连接指针数组
}STProcessManageNode;

typedef struct _STProcessManageCur
{
	DWORD iProcessId; //进程Id,可以在接收到广播消息后更新这个字段,目前未用到
	CHAR szProcessName[MSG_SHM_PROCESS_LEN];//应用程序名称

	DWORD dwKey; //根据这个key值可以查找到对应的共享内存块的消息

	void* pShm; //共享内存块连接指针 dwKey
}STProcessManageCur;

/* BEGIN: Modified by huangdch, 2015/8/31 
*进程间通信消息id发布,一般从80000开始编号*/
//enum
//{
//	SHM_PROCESS_MEM_PHONE	= 80010, //摄像机
//	SHM_PROCESS_MEM_CAPTURE	= 80030, //采集
//	SHM_PROCESS_MEM_ENCODE	= 80050, //压缩
//	SHM_PROCESS_MEM_SEND	= 80070, //发送
//	
//	SHM_PROCESS_NUMBER	 = 4 //对应的应用程序的进程数,要保证这个值是正确的
//};


/*************************************************************************
*其它进程间通信的消息结构体可以从这里开始发布
**************************************************************************/

typedef struct _STSendMig
{
	int a;
	int b;
}STSendMig;


typedef struct _STCaptureMig
{
	int a;
	int b;
}STCaptureMig;


typedef struct _STEncodeMig
{
	int a;
	int b;
}STEncodeMig;


typedef struct _STPhoneMig
{
	int a;
	int b;
}STPhoneMig;


#endif

