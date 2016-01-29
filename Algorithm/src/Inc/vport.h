/* 
    该文档为视频端口设置宏定义和接口声明.
    胡中华于2004年10月15日10:24作出修改.
*/
#ifndef _VPORT_H
#define _VPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "std.h"
#include "csl.h"
#include <iom.h>
#include "edc.h"

/* 有关VP端口的属性设置宏定义. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
/* VP 视频场工作模式. */
#define VPORT_FLDOP_FLD1              	0x1    //场1 CAP/DIS ONLY.
#define VPORT_FLDOP_FLD2              	0x2    //场2 CAP/DIS ONLY.
#define VPORT_FLDOP_FRAME             	0x4    //帧 CAP/DIS.
#define VPORT_FLDOP_PROGRESSIVE       	0x5

/* VP端口操作模式. */
#define VPORT_MODE_BT656_8BIT        	0x0
#define VPORT_MODE_BT656_10BIT       	0x1
#define VPORT_MODE_RAW_8BIT          	0x2
#define VPORT_MODE_RAW_10BIT         	0x3
#define VPORT_MODE_YCBCR_8BIT        	0x4
#define VPORT_MODE_YCBCR_10BIT       	0x5
#define VPORT_MODE_RAW_16BIT         	0x6
#define VPORT_MODE_RAW_20BIT	     	0x7
                                     	
/* VP端口外部引脚极性. */            	
#define VPORT_POLARITY_ACTIVE_HIGH   	0x00
#define VPORT_POLARITY_ACTIVE_LOW    	0x11
                
/* VP端口垂直缩放模式,对于捕足而言是缩小1/2,对于显示而言是放大2. */
#define VPORT_SCALING_DISABLE        	0x0
#define VPORT_SCALING_ENABLE         	0x1

/* VP端口色度重采样属性设置,对于捕捉而言是由 4:2:2 => 4:2:0,对于显示而言是4:2:0 => 4:2:2. */
#define VPORT_RESMPL_DISABLE         	0x0
#define VPORT_RESMPL_ENABLE          	0x1          
                                           
/* VP端口视频帧数据是分开放置,还是综合到一起. */                                      
#define VPORT_FLDS_SEPARATED         	0
#define VPORT_FLDS_MERGED				1
/*------------------------------------------------------------------------------------------------------------------------------------*/





/* VP端口IOM驱动程序相关接口和控制命令集宏定义 */
/*------------------------------------------------------------------------------------------------------------------------------------*/
/* IOM设备驱动程序所支持的控制命令集. */
#define VPORT_CMD_RESET                 0x10000001  //VP端口复位命令.
#define VPORT_CMD_CONFIG_PORT           0x10000002  //VP端口配置命令.
#define VPORT_CMD_CONFIG_CHAN           0x10000003  //VP端口通道配置命令.
#define VPORT_CMD_START                 0x10000004  //启动VP端口操作.
#define VPORT_CMD_STOP                  0x10000005  //停止VP端口操作.
#define VPORT_CMD_SET_VINTCB            0x10000006  //设置VP端口中断Callback函数.
#define VPORT_CMD_DUND_RECOVER          0x10000007  //强制VP从显示FIFO下溢状态恢复.
#define VPORT_CMD_COVR_RECOVER          0x10000007  //强制VP从捕捉FIFO上溢状态恢复.
#define VPORT_CMD_SET_BUFFNUM			0x10000008  //设置缓冲区个数.
#define VPORT_CMD_GET_BUFFNUM			0x10000009  //设置缓冲区个数.

/* EDC控制命令基参数 */
#define VPORT_CMD_EDC_BASE              0x80000000
#define VPORT_CMD_EDC_CONFIG			( VPORT_CMD_EDC_BASE + EDC_CONFIG )		//config基本命令.		Video Interface IC Contorl Command.
#define VPORT_CMD_EDC_RESET				( VPORT_CMD_EDC_BASE + EDC_RESET )		//reset基本命令.        Video Interface IC Contorl Command.
#define VPORT_CMD_EDC_START				( VPORT_CMD_EDC_BASE + EDC_START )		//start基本命令.        Video Interface IC Contorl Command.
#define VPORT_CMD_EDC_STOP				( VPORT_CMD_EDC_BASE + EDC_STOP )		//stop基本命令.         Video Interface IC Contorl Command.
#define VPORT_CMD_EDC_GET_CONFIG		( VPORT_CMD_EDC_BASE + EDC_GET_CONFIG )	//getconfig基本命令.    Video Interface IC Contorl Command.
#define VPORT_CMD_EDC_GET_STATUS		( VPORT_CMD_EDC_BASE + EDC_CONFIG )		//getstatus基本命令.    Video Interface IC Contorl Command.


#define VPORT_MAX_NUM_FRMBUFS           20          // 视频帧最大缓冲区个数

/* 错误Callback函数原型.对于一个通道/设备实例而言,应只有一个VP端口全局中断Callback函数,否则多次调用(覆盖以前的设置)只会以最后一次为准. */
typedef void ( *VPORT_IntCallBack )( Arg id, Arg mask );

/* VP端口中断屏蔽标志意义. */
#define VPORT_INT_COVR                  0x0002      //捕捉FIFO上溢中断.
#define VPORT_INT_CCMP                  0x0004      //捕捉完成中断.
#define VPORT_INT_SERR                  0x0008      //同步错误中断.
#define VPORT_INT_VINT1                 0x0010      //场1中断.
#define VPORT_INT_VINT2                 0x0020      //场2中断.
#define VPORT_INT_SFD                   0x0040      //短场侦测中断.
#define VPORT_INT_LFD                   0x0080      //长场侦测中断.
#define VPORT_INT_STC                   0x0400      //系统时钟中断.
#define VPORT_INT_TICK                  0x0800      //时钟跳中断.
#define VPORT_INT_DUND                  0x1000      //显示FIFO下溢中断.
#define VPORT_INT_DCMP                  0x2000      //显示完成中断.
#define VPORT_INT_DCNA                  0x4000      //显示完成但是没有回应中断.

/* 视频端口Callback函数设置数据结构. */                                           
typedef struct VPORT_VIntCbParams{
    Int                 cbArg;          //当前该中断Callback函数为何具体的通道或者设备实例.
    VPORT_IntCallBack   vIntCbFxn;      //指向具体的Callback函数.
    Uint16              vIntMask;       //使用该中断Callback函数的中断类型限制.
    Uint16              vIntLine;       //场水平线中断水平线值.
    Int                 irqId;          //中断ID.    
} VPORT_VIntCbParams;


/* VP端口属性数据结构. */
typedef struct VPORT_PortParams{
    Bool                dualChanEnable; //VP端口FIFO是否使用双通道.
    Uns                 vc1Polarity;    //VP端口控制引脚1极性.
    Uns                 vc2Polarity;    //VP端口控制引脚2极性.
    Uns                 vc3Polarity;    //VP端口控制引脚3极性.
    EDC_Fxns*           edcTbl[2];      //外部EDC控制函数集指针,为每个通道分配一个EDC操作函数表.
} VPORT_PortParams;
/*------------------------------------------------------------------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _VPORT_H */
