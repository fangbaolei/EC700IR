/* 
    该文档为视频捕捉端口设置宏定义和接口声明.
    胡中华于2004年10月15日11:24作出修改.
*/
#ifndef _VPORTCAP_H
#define _VPORTCAP_H

#ifdef __cplusplus
extern "C" {
#endif
#include "vport.h"



/* 有关VP 捕捉端口的属性设置宏定义. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
/* 输入的场信号是否需要反向. */
#define   VPORTCAP_FINV_DISABLE             0x0			
#define   VPORTCAP_FINV_ENABLE              0x1

/* VP端口扫描线计数器复位位置模式宏定义. */
#define   VPORTCAP_VRST_START_VBLK          0x0 
#define   VPORTCAP_VRST_START_VSYNC         0x0            
#define   VPORTCAP_VRST_EAV_V0              0x0

#define   VPORTCAP_VRST_END_VBLK            0x1 
#define   VPORTCAP_VRST_END_VSYNC           0x1 
#define   VPORTCAP_VRST_EAV_V1              0x1


/* VP端口像素计数器复位位置模式宏定义. */
#define   VPORTCAP_HRST_EAV                 0x0 
#define   VPORTCAP_HRST_START_HBLK          0x0
#define   VPORTCAP_HRST_START_HSYNC         0x0 

#define   VPORTCAP_HRST_SAV                 0x1 
#define   VPORTCAP_HRST_END_HBLK            0x1
#define   VPORTCAP_HRST_END_HSYNC           0x1 

/* VP端口场检测模式设置寄存器. */
#define   VPORTCAP_FLDD_DISABLE             0x0             //使用EAV,SAV数据来检测出场信号.
#define   VPORTCAP_FLDD_ENABLE              0x1             //由场信号输入引脚的逻辑得出.

/* VP捕捉外部控制模式设置. */
#define   VPORTCAP_EXC_DISABLE              0x0             //使用数据流上的EAV,SAV代码来检测相关信号.   
#define   VPORTCAP_EXC_ENABLE               0x1             //由HCTL,VCTL,FID信号引脚输入来获取控制逻辑.

/* VP捕捉端口(10位方式)时的包格式设置. */
#define	   VPORTCAP_BPK_10BIT_ZERO_EXTENDED 0x0
#define    VPORTCAP_BPK_10BIT_SIGN_EXTENDED 0x1
#define    VPORTCAP_BPK_10BIT_DENSE         0x2


/* VP驱动程序缓冲区管理机制. */
#define 	VP_DEVICE_NEW_FRAME				0
#define 	VP_DEVICE_BUFFER				1
#define 	VP_DEVICE_ASYMMETRY_QUEUE		2

/* IOM设备驱动程序外部接口数据结构定义和声明. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
/* VP端口作为捕捉端口使用时的属性设置数据结构定义,这些有关设置的宏定义见Vport.h文件. */
typedef struct {
    /* -------------------------------VP端口功能属性.----------------------- */
    Int     cmode;                          //视频捕捉模式,
    Int     fldOp;                          //捕捉对象设置,是捕捉场1/2或者帧,Bit2:用来指示是否用帧,Bit1:场2,Bit0:场1, 为1是使能为0是禁止.
    Int     scale;                          //水平缩放设置
    Int     resmpl;                         //色度重新采样设置,即将4:2:2重采样成4:2:0
    Int     bpk10Bit;                       //10位包模式设置.
    Int     hCtRst;                         //水平扫描线计数器复位模式设置.
    Int     vCtRst;                         //垂直线(采样点)计数器复位模式设置.
    Int     fldDect;                        //视频捕捉场检测模式设置.
    Int     extCtl;                         //视频捕捉时钟逻辑控制设置.
    Int     fldInv;                         //场信号反向设置.
    
    Uint16  fldXStrt1;                      //场1 X起始像素点.
    Uint16  fldYStrt1;                      //场1 Y起始像素点.
    Uint16  fldXStrt2;                      //场2 X起始像素点.
    Uint16  fldYStrt2;                      //场2 Y起始像素点.
    Uint16  fldXStop1;                      //场1 X结束像素点.
    Uint16  fldYStop1;                      //场1 Y结束像素点. 
    Uint16  fldXStop2;                      //场2 X结束像素点.
    Uint16  fldYStop2;                      //场2 Y结束像素点.
    Uint16  thrld;                          //产生DMA事件的阈值(以两字为单位).
    
    /* -------------------------------VP数据帧属性.------------------------- */
    Int     numFrmBufs;                     //为IOM设备驱动分配多少个帧缓冲区,用来保存视频捕捉数据.
    Int     alignment;                      //帧缓冲区在由segId所设置的内存块中的起始.
    Int     mergeFlds;                      //设置是否需要合并场1/2的数据.
    Int     segId;                          //用来存放视频捕捉数据的内存块名称.

    /* -------------------------------EDMA优先级设置.----------------------- */
    Int     edmaPri;                        //EDMA优先级设置.

    /* -------------------------------VP端口ID设置.------------------------- */
    Int     irqId;                          //中断ID设置
    
    /* -------------------------------VP缓冲区模式------------------------- */
    Int		queueMode;						//用来指示VP缓冲器缓冲模式.
    Int		iCtrlNumFrmBufs;				//不对称缓冲区管理缓冲区数目.
} VPORTCAP_Params;

extern IOM_Fxns VPORTCAP_Fxns;




#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _VPORTCAP_H */
