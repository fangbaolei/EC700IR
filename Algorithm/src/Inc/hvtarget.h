#ifndef _HVTARGET_H
#define _HVTARGET_H

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

#include "dspsys_def.h"

typedef struct _SYS_WORK_TIMER {
	int iGetType;
	DWORD32 dwMSCountLow;
	DWORD32 dwMSCountHigh;
} SYS_WORK_TIMER;

extern SYS_WORK_TIMER SysWorkTimer;

/* 枚举描述:系统支持串口号宏定义. */
typedef enum {
	SYS_DEBUG_COM 		= 0,
	SYS_CTRL_COM		= 1,
	MAX_SYS_COM_NUM		= 2
} SYS_COM_PORT;

/* 枚举描述:串口终端用户输入关键字符标志,以下为返回类型常量定义. */
typedef enum {
	USER_KEY_ESC		= 1,
	USER_KEY_BACKSPACE 	= 2,
	USER_KEY_ENTER		= 4
} USER_KEY_VALUE;

/* 枚举描述:DSP的启动方式. */
typedef enum {
	DSP_BOOT_EMULATION	= 0,
	DSP_BOOT_HPI		= 1,
	DSP_BOOT_RESERVED	= 2,
	DSP_BOOT_EMIFA		= 3
} DSP_BOOT_MODE;

/* 枚举描述:DSP的内存映射方式. */
typedef enum {
	DSP_MEMMAP_EXTERNAL = 0,			/* 外部存储器地址从0开始. */
	DSP_MEMMAP_INTERNAL	= 1				/* 内部存储器地址从0开始. */
} DSP_MEMMAP_MODE;

/* 数据结构说明:该数据结构各域用来指示DSP的有关片上设备信息以及DSP版本型号信息. */
typedef struct _DSP_CHIP_INFO {
	BYTE8				bCpuTypeId;		/* DSP产品类型ID号.											*/
	BYTE8 				bCpuVerId;		/* DSP版本ID号.                                             */
	BYTE8				bEndianMode;	/* DSP正在使用的字节序,为1表示为低字节序,为0表示为高字节序. */
	BYTE8				bVpConfig;		/* DSP的片上外设VP端口使用情况,bit0--VP0,bit1-VP1,bit2--VP2 */
	DSP_BOOT_MODE		DspBootMode;	/* DSP当前所使用的启动模式.									*/
	DSP_MEMMAP_MODE		DspMemMapMode;	/* DSP的内存映射模式(在没有使用L2 Cache的情况下),			*/
										/* 为0表示为外部地址从0开始,为1表示内部地址从0开始.			*/
	BOOL				fEmacEnable;	/* DSP的片上外设EMAC是否使能.                               */
	BOOL				fPciEnable;		/* DSP的片上外设PCI是否使能.	                            */
} DSP_CHIP_INFO;

/* 数据结构说明:该数据结构用来指示目标板的系统级信息. */
typedef struct _TARGET_SYSTEM_INFO {
	DWORD32 			dwTargetTypeId;	/* 目标板的硬件平台类型版本号.								*/
	DSP_CHIP_INFO		DspChipInfo;	/* 有关DSP片上外设使能配置以及当前使用DSP模式有关信息.	    */
} TARGET_SYSTEM_INFO;


extern DWORD32 g_dwLed;

extern int g_iResetFlag;

__inline int GetResetFlag()
{
	return g_iResetFlag;
}

__inline void SetResetFlag()
{
	g_iResetFlag = 1;
}

/* 基本操作API声明及接口说明. */
/*------------------------------------------------------------------------------------------------------*/
/* 函数说明:该函数为毫秒为单位的延迟,延迟时间为"dwDelayTime * 1ms ".函数无返回值. */
extern void Target_DelayMS(
	DWORD32 					dwDelayTime		//延迟MS次数.
);

/*
函数说明:该函数用来取得目标系统硬件版本号和外设的在关信息.
函数返回值:
	返回S_OK表示取信息成功;
	返回E_POINTER表示pSysInfo为INVALID_POINTER; */
extern HRESULT GetTargetSysInfo(
	TARGET_SYSTEM_INFO 			*pSysInfo		//指向取得平台有关信息数据.
);

/* 函数说明:该函数用来关闭全局中断,函数返回关闭全局中断之前的全局中断使能. */
extern DWORD32 CloseGlobalInterrupt( void );

/* 函数说明:该函数用来恢复全局中断,根据传入的全局中断使能情况. */
extern void RestoreGlobalInterrupt( DWORD32 dwGie );

/* 函数说明:关闭全局中断使能. */
extern void CloseAllInterrupt( void );

/* 函数说明:使能全局中断. */
extern void EnableAllInterrupt( void );

/* 函数说明:该函数用来点亮或者熄灭系统平台的工作指示灯,如当前LED灯为亮则调用该函数之后灯即灭,函数无返回值. */
extern void WorkLedLight( void );

/* 函数说明:该函数用来点亮或者熄灭系统平台的LAN灯,如当前LED灯为亮则调用该函数之后灯即灭,函数无返回值. */
extern void LanLedLight( void );
extern void SetLanLedOn( void );
extern void SetLanLedOff( void );
/* 函数说明:该函数用来点亮或者熄灭系统平台的Hard指示灯,如当前LED灯为亮则调用该函数之后灯即灭,函数无返回值. */
extern void HardLedLight( void );
extern void SetHardLedOn( void );
extern void SetHardLedOff( void );
/* 函数说明:该函数用来点亮或者熄灭系统平台的Alarm1指示灯,如当前LED灯为亮则调用该函数之后灯即灭,函数无返回值. */
extern void Alarm1LedLight( void );
extern void SetAlarm1LedOn( void );
extern void SetAlarm1LedOff( void );
/* 函数说明:该函数用来点亮或者熄灭系统平台的Alarm2指示灯,如当前LED灯为亮则调用该函数之后灯即灭,函数无返回值. */
extern void Alarm2LedLight( void );
extern void SetAlarm2LedOn( void );
extern void SetAlarm2LedOff( void );
/* 函数说明:该函数用来让峰鸣器响.函数无返回值. */
extern void SetBuzzerOn( void );
/* 函数说明:该函数用来让峰鸣器不响.函数无返回值. */
extern void SetBuzzerOff( void );

/* 函数说明:该函数用来点亮系统平台上作为工作指示灯.函数无形参,函数无返回值. */
extern void SetWorkLedOn( void );

/* 函数说明:该函数用来熄灭系统平台上作为工作指示灯.函数无形参,函数无返回值. */
extern void SetWorkLedOff( void );

// 上海交计独有的LED灯操作
extern void WorkLedLight_Jiaoji( void );
extern void SetWorkLedOn_Jiaoji( void );
extern void SetWorkLedOff_Jiaoji( void );
extern void SetHardLedOn_Jiaoji( void );
extern void SetHardLedOff_Jiaoji( void );
extern void Lan1LedLight_Jiaoji( void );
extern void SetLan1LedOn_Jiaoji( void );
extern void SetLan1LedOff_Jiaoji( void );
extern void SetLan2LedOn_Jiaoji( void );
extern void SetLan2LedOff_Jiaoji( void );
extern void SetAlm1LedOn_Jiaoji( void );
extern void SetAlm1LedOff_Jiaoji( void );
extern void SetAlm2LedOn_Jiaoji( void );
extern void SetAlm2LedOff_Jiaoji( void );
extern void Link1LedLight_Jiaoji( void );
extern void Link2LedLight_Jiaoji( void );
extern void Link3LedLight_Jiaoji( void );
extern void Link4LedLight_Jiaoji( void );
extern void Link5LedLight_Jiaoji( void );
extern void Link6LedLight_Jiaoji( void );
extern void Link7LedLight_Jiaoji( void );
extern void Link8LedLight_Jiaoji( void );
extern void SetLink1LedOn_Jiaoji( void );
extern void SetLink1LedOff_Jiaoji( void );
extern void SetLink2LedOn_Jiaoji( void );
extern void SetLink2LedOff_Jiaoji( void );
extern void SetLink3LedOn_Jiaoji( void );
extern void SetLink3LedOff_Jiaoji( void );
extern void SetLink4LedOn_Jiaoji( void );
extern void SetLink4LedOff_Jiaoji( void );
extern void SetLink5LedOn_Jiaoji( void );
extern void SetLink5LedOff_Jiaoji( void );
extern void SetLink6LedOn_Jiaoji( void );
extern void SetLink6LedOff_Jiaoji( void );
extern void SetLink7LedOn_Jiaoji( void );
extern void SetLink7LedOff_Jiaoji( void );
extern void SetLink8LedOn_Jiaoji( void );
extern void SetLink8LedOff_Jiaoji( void );
extern void Stat1LedLight_Jiaoji( void );
extern void Stat2LedLight_Jiaoji( void );
extern void Stat3LedLight_Jiaoji( void );
extern void Stat4LedLight_Jiaoji( void );
extern void Stat5LedLight_Jiaoji( void );
extern void Stat6LedLight_Jiaoji( void );
extern void Stat7LedLight_Jiaoji( void );
extern void Stat8LedLight_Jiaoji( void );
extern void SetStat1LedOn_Jiaoji( void );
extern void SetStat1LedOff_Jiaoji( void );
extern void SetStat2LedOn_Jiaoji( void );
extern void SetStat2LedOff_Jiaoji( void );
extern void SetStat3LedOn_Jiaoji( void );
extern void SetStat3LedOff_Jiaoji( void );
extern void SetStat4LedOn_Jiaoji( void );
extern void SetStat4LedOff_Jiaoji( void );
extern void SetStatk5LedOn_Jiaoji( void );
extern void SetStat5LedOff_Jiaoji( void );
extern void SetStat6LedOn_Jiaoji( void );
extern void SetStat6LedOff_Jiaoji( void );
extern void SetStat7LedOn_Jiaoji( void );
extern void SetStat7LedOff_Jiaoji( void );
extern void SetStat8LedOn_Jiaoji( void );
extern void SetStat8LedOff_Jiaoji( void );

/* 函数说明:该函数用来将WDT机制狗的喂狗当前状态置为低.函数无形参,函数无返回值. */
extern void ClearWDT( void );

/* 函数说明:该函数用来将WDT机制狗的喂狗当前状态置为高.函数无形参,函数无返回值. */
extern void SetWDT( void );

/* 函数说明:该函数用来将WDT机制狗的状态有个跳变. 函数无形参,函数无返回值. */
extern void ResWDT( void );

/* 函数说明:该函数用来取得系统平台触发输入的当前状态值.函数无形参,函数返回值的最低位有效. */
extern BYTE8 GetTriggerIn( void );

/* 函数说明:该函数用来取得系统平台触发输入的当前状态值.函数无形参,函数返回值的最低位有效. */
extern BYTE8 GetTriggerIn_2( void );

/* 函数说明:该函数用来根据bSetBit位值设置触发输出的状态.函数无返回值. */
extern void SetTriggerOut(
	BYTE8 						bSetBit			//设置值,只有最后一位有效.
);

/* 函数说明:该函数用来初始化当前系统LED灯的MASK.*/
extern void InitLedMask();

/* 函数说明:该函数用来取得当前系统LED灯的MASK.函数直接返回该MASK. */
extern DWORD32 GetLedMask( void );

/* 函数说明:该函数用来点亮平台上使用dwLedMask所标识的GPIO指示灯(使用DSP的GPIO引脚的灯). */
extern void SetLedOn(
	DWORD32 					dwLedMask		//LED灯的GPIO MASK.
);

/* 函数说明:该函数用来熄灭平台上使用dwLedMask所标识的GPIO指示灯(使用DSP的GPIO引脚的灯). */
extern void SetLedOff(
	DWORD32 					dwLedMask		//LED灯的GPIO MASK.
);

/* 函数说明:函数直接返回硬件平台跳线输入. */
extern DWORD32 GetJumperInput( void );

/* 函数说明:该函数用来取得硬件平台用户拔码开关输入值.函数无形参.函数直接返回该值. */
extern DWORD32 GetUserConfigPort( void );
/*------------------------------------------------------------------------------------------------------------------------------------*/




/* 外设操作函数API声明及接口说明. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
函数说明:该函数用来取得ID芯片的ID号,该ID具体意义由该API使用者解释.
函数返回值:
	返回S_OK表示读取成功.
	返回E_POINTER表示pbIdValue为INVALID_POINTER.
	返回E_NOTIMPL表示当前硬件平台不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示只返回ID号的部分数据或者dwBytesToRead超过实际唯一ID数据长度.	*/
extern HRESULT GetUniqueId(
	PBYTE8						pbIdValue,		//指向取得的ID号内容.
	DWORD32						dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead	//实际读到的数据字节长度,为NULL则不填.
);

#ifdef _HV_PRODUCT
/*
函数说明:该函数用来从FLASH中读取数据,当前硬件平台所有对FLASH的读操作必须通过该函数才能保证从FLASH读
	取正确/有效.
函数返回值:
	返回S_OK表示读取成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,读取的数据超出FLASH当前容量等.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取FLASH出错. */
extern HRESULT FLASH_Read(
	DWORD32 					dwSrcPos,		//从FLASH读取数据的起始位置.
	PBYTE8						pbMemDst, 		//指向内存中放置读到的数据.
	BYTE8						bReadType,		//FLASH读取模式,为0时表示为透明读,为1为加密读
	DWORD32						dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead	//实际读到的数据字节长度,为NULL则不填.

);

/*
函数说明:该函数用来向FLASH中写入数据,当前硬件平台所有对FLASH的写操作必须通过该函数才能保证向FLASH中写入
	数据正确/有效(但是所写的FLASH页区域必须先被ERASE过).
函数返回值:
	返回S_OK表示写入操作成功.
	返回E_POINTER表示pbMemSrc为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,写入的数据超出FLASH当前容量等.
	返回E_NOTIMPL表示当前FLASH写不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写操作FLASH出错. */
extern HRESULT FLASH_Write(
	const PBYTE8 				pbMemSrc,		//指向所要读取的内存数据.
	DWORD32 					dwDstPos,		//FLASH写入数据目标起始位置.
	BYTE8 						bWriteType,		//FLASH写模式,0为透明写,1为加密写
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32 					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);

/*
函数说明:该函数用来擦除FLASH块数据,机制将其存储最小单元全部置为1,FLASH的写操作只能对最小单元进行1->0的
	写操作,而不可进行0->1的操作,所以在写入数据之前需要对其存储块先进行擦除操作,擦除的最小单位为块.
函数返回值:
	返回S_OK表示写入操作成功.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_NOTIMPL表示当前FLASH擦除不支持.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写操作FLASH出错. */
extern HRESULT FLASH_Erase(
	DWORD32						dwStartPos, 	//需要擦除FLASH块首位置.
	DWORD32 					dwLength		//当前需要擦除长度,以字节为单位.
);

extern HRESULT FLASH_RandomWrite(
	DWORD32 					dwFlashAddr,
	BYTE8 						*pSrcData,
	int 						iLen
);
#endif

/*
函数说明:该函数用来设置FLASH存储时所使用的加密算子.
函数返回值:
	返回S_OK表示设置成功.
	返回E_POINTER表示pbSrcCode为INVALID_POINTER. */
extern HRESULT SetSecurityCode(
	const BYTE8					*pbSrcCode,		//指向作为加密算子的数据.
	DWORD32						dwCodeLen		//运算加密算子的数据字节长度.
);


/* 初始化EEPROM */
extern HRESULT EEPROM_Init();

/* 读取EEPROM密区数据 */

extern HRESULT EEPROM_FreeRead(
	DWORD32 					dwReadPos,			//读取EEPROM的首位置.
	PBYTE8						pbMemDst,			//指向读出的数据.
	DWORD32 					dwBytesToRead,		//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead		//实际读取的数据字节长度,为NULL则不填.
);

/* 读取EEPROM加密区数据 */
extern HRESULT EEPROM_EncryptRead(
	DWORD32						dwReadPos,		//读取EEPROM的首位置.
	PBYTE8 						pbMemDst, 		//指向读出的数据.
	DWORD32						dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead		//实际读取的数据字节长度,为NULL则不填.
);

/* 写入EEPROM非加密区 */
extern HRESULT EEPROM_FreeWrite(
	const PBYTE8				pbMemSrc, 		//指向写入的数据.
	DWORD32 					dwWritePos, 	//写入EEPROM的首地址.
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);

/* 写入EEPROM加密区 */
extern HRESULT EEPROM_EncryptWrite(
	const PBYTE8				pbMemSrc, 		//指向写入的数据.
	DWORD32 					dwWritePos, 	//写入EEPROM的首地址.
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);

/*
函数说明:该函数用来从EEPROM中读取一定数据的数据.
函数返回值:
	返回S_OK表示读取EEPROM操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取EEPROM出错. */
/*
extern HRESULT EEPROM_Read(
	DWORD32 					dwReadPos,		//读取EEPROM的首位置.
	PBYTE8						pbMemDst,		//指向读出的数据.
	DWORD32 					dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead	//实际读取的数据字节长度,为NULL则不填.
);
*/

/*
函数说明:该函数用来向EEPROM中写入一定量数据.
函数返回值:
	返回S_OK表示写入EEPROM操作成功.
	返回E_POINTER表示pbMemSrc为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写入EEPROM出错. */
extern HRESULT EEPROM_Write(
	const PBYTE8				pbMemSrc,		//指向写入的数据.
	DWORD32 					dwWritePos, 	//写入EEPROM的首地址.
	DWORD32 					dwBytesToWrite, //期望写入的数据字节长度.
	PDWORD32					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);

/*
函数说明:该函数用来读取实时时钟芯片中的实时时间值(含日期),精确到秒,但是当前的实时时钟芯片年误差为100分钟左右.
函数返回值:
	返回S_OK表示读取RTC日期时间成功.
	返回E_POINTER表示pRealTime为INVALID_POINTER.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取RTC日期时间出错. */
extern HRESULT RTC_GetDateTime(
	REAL_TIME_STRUCT 			*pRealTime		//指向读出的时间.
);

/*
函数说明:该函数用来设置实时时钟芯片时间(包含日期),精确到秒.其数据串的各字节意义见上描述.根据用户所设置的数据
	写入至RTC相关时间日期寄存器中.
函数返回值:
	返回S_OK表示设置RTC日期时间成功.
	返回E_POINTER表示pbTimeVal为INVALID_POINTER.
	返回E_INVALIDARG表示传入的实时时间数据非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示设置RTC日期时间出错. */
extern HRESULT RTC_SetDateTime(
	const REAL_TIME_STRUCT 		*pRealTime		//指向设置的时间.
);

/*
函数说明:该函数用来向ComPort串口的发送队列中填写字符,如果当前该串口端口发送队列满则会阻塞等待定时中断
	(定时中断将发送队列中的数据发送至串口端口的硬件FIFO中),只有在所要填定的发送字符中遇到'\0'时才会
	停止向发送队列中填写数据.
	该函数只可在TSK或者SWI中使用,否则可能会陷入死等状态.
函数返回值:
	返回S_OK表示向串口端口写入数据成功.
	返回E_POINTER表示pszString为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示向串口端口写入数据失败. */
extern HRESULT ComWriteStr(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	const PCHAR					pszString		//指向所要写入的字符串,该串以'\0'为结束标志
);

/*
函数说明:该函数用来向ComPort串口的发送队列中填写定长度数据,如果当前该串口端口发送队列满则会
	阻塞等待定时中断(定时中断将发送队列中的数据发送至串口端口的硬件FIFO中).
	该函数只可在TSK或者SWI中使用,否则可能会陷入死等状态.
函数返回值:
	返回S_OK表示向串口端口写入数据成功.
	返回E_POINTER表示pbData为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示向串口端口写入数据失败. */
extern HRESULT ComWriteData(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	const PBYTE8				pbData,			//指向所要写入的数据
	DWORD32						dwBytesToWrite,	//期望写入的数据字节个数.
	PDWORD32					pdwBytesWrite	//实际写入的数据字节个数,为NULL则不填.
);

/*
函数说明:该函数用来从ComPort串口的接收队列中读取字符串,如果当前该串口端口接收队列为空则会阻塞等待定时中断
	(定时中断会该串口端口的硬件FIFO中读取数据).该函数只有在遇到与回车键才会正常中断当前读取字符串操作,
	在读取字符串长度超过传入最大长度时,该函数不会向pszString中填入任何数据(后面接收的全部丢弃).
	该函数只可在TSK或者SWI中使用,否则可能会陷入死等状态.
函数返回值:
	返回S_OK表示从串口端口读取数据成功.
	返回E_POINTER表示pszString为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,绱诙丝诜欠?
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示从串口端口读取的数据长度超过用户所设定的最大数据长度. */
extern HRESULT ComReadStr(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	PCHAR 						pszString,		//指向读出的字符串数据.
	DWORD32						dwMaxBytesToRead,//充许读入的最大字符数目.
	PDWORD32					pdwBytesRead	//实际读到的字符长度,包括回车键,如果为NULL则不填.
);

/*
函数说明:该函数用来从ComPort串口的接收队列中读取字符数据,如果当前该串口端口接收队列为空直接返回当前实际读到
	的数据个数.定时中断(在中断函数中会该串口端口的硬件FIFO中读取数据).
函数返回值:
	返回S_OK表示从串口端口读取数据成功.
	返回E_POINTER表示pbData为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示从串口端口读取的数据个数少于dwBytesToRead所指定的个数. */
extern HRESULT ComReadData(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	PBYTE8						pbData,			//指向读出的数据
	DWORD32						dwBytesToRead,	//期望读入的字符数目.
	PDWORD32					pdwBytesRead	//实际读到的字符长度,如果为NULL则不填.
);

/*
函数说明:该函数用来清串口的软硬件
*/
extern HRESULT ClearComData(
	SYS_COM_PORT 				ComPort 		//表示当前操作的串口端口号.
);

/*
函数说明:该函数用来向串口写入字符串,是以字符串口有结束字符'\0'为中止标志的.
	该函数可以在任何地方调用,但是必须要明确该发送程序是一直等待硬件发送结束后才返回的.
函数返回值:
	返回S_OK表示向串口端口写入数据成功.
	返回E_POINTER表示pszString为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示向串口端口写入数据失败. */
extern HRESULT COM_WriteStr(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	const PCHAR					pszString		//指向所要写入的字符串数据.
);

/*
函数说明:该函数用来向串口写入定长度数据.
	该函数可以在任何地方调用,但是必须要明确该发送程序是一直等待硬件发送结束后才返回的.
函数返回值:
	返回S_OK表示向串口端口写入数据成功.
	返回E_POINTER表示pszString为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示向串口端口写入数据失败. */
extern HRESULT COM_WriteData(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	const PBYTE8				pbData,			//指向所要写入的数据
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32					pdwBytesWrite	//实际写入的数据字节长度,如果为NULL则不填.
);

/*
函数说明:该函数用来从串口读取字符串,该函数只有在遇到回车键时才会正常返回.在读取字符串长度超过传入最大长度时,
	该函数不会向pszString中填入任何数据(后面接收的全部丢弃).
	该函数可以在任何地方调用,但是是轮询串口端口硬件FIFO是否接收到字符的,所以其他事务都被当前操作抢占.
函数返回值:
	返回S_OK表示从串口端口读取数据成功.
	返回E_POINTER表示pszString为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示从串口端口读取数据失败. */
extern HRESULT COM_ReadStr(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	PCHAR 						pszString,		//指向读出的字符串数据.
	DWORD32						dwMaxBytesToRead,//充许读入的最大字符数目.
	PDWORD32					pdwBytesRead	//实际读到的字符长度,包括回车键,如果为NULL则不填.
);

/*
函数说明:数用来从串口读取数据,以读到期望得到的数据长度为中止条件.
	该函数可以在任何地方调用,但是是轮询端谟布﨔IFO是否接收到字符的,所以其他事务都被当前操作抢占.
函数返回值:
	返回S_OK表示从串口端口读取数据成功.
	返回E_POINTER表示pbData或者pdwLen为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如串口端口非法.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示从串口端口读取数据失败. */
extern HRESULT COM_ReadData(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	PBYTE8						pbData,			//指向读出的数据
	DWORD32						dwBytesToRead,	//期望读入的字符数目.
	PDWORD32					pdwBytesRead	//实际读到的字符长度,如果为NULL则不填.
);

/*
函数说明:该函数从串口端口读取一字符,函数直接返回该字符.
	该函数可以在任何地方调用,但是是轮询串口端口硬件FIFO是否接收到字符的,所以其他事务都被当前操作抢占. */
extern CHAR COM_GetChar(
	SYS_COM_PORT 				ComPort 		//表示当前操作的串口端口号.
);

/*
函数说明:该函数向串口端口输出一个字符,函数无返回值.
	该函数可以在任何地方调用,但是必须要明确该发送程序是一直等待硬件发送结束后才返回的. */
extern void COM_PutChar(
	SYS_COM_PORT 				ComPort, 		//表示当前操作的串口端口号.
	CHAR						chVal			//当前要发送的数据.
);
/*------------------------------------------------------------------------------------------------------------------------------------*/



/*
函数说明:该函数用来判断当前的视频端口是否被使用.
函数返回值:
	返回S_OK表示取缓冲区个数成功.
	返回E_INVALIDARG表示传入的参数有错,该端口系统当前不支持.
	返回E_POINTER表示pfPresent为INVALID_POINTER.
	返回S_FALSE表示当前的视频端口未使用. */
extern HRESULT VIDEO_IsPortPresent(
	int 				iPort,
	PBOOL 				pfPresent
);


/*------------------------------------------------------------------------------------------------------------------------------------*/
// ATA IDE 硬盘接口
extern HRESULT ATA_ReadSector(
	DWORD32					dwDeviceID,
	DWORD32					dwSectorStartLBA,
	PBYTE8					pbMemDst,
	DWORD32					dwSectorsToRead,
	PDWORD32				pdwReadSectors
);

extern HRESULT ATA_WriteSector(
	DWORD32					dwDeviceID,
	PBYTE8					pbMemSrc,
	DWORD32					dwSectorStartLBA,
	DWORD32					dwSectorsToWrite,
	PDWORD32				pdwWriteSectors
);

extern HRESULT ATA_HdCheck( void );
extern HRESULT ATA_HdReadId(
	PCHAR				pszString,
	DWORD32				dwBytesToRead,
	PDWORD32			pdwReadBytes
);
extern HRESULT ATA_Reset( void );
/*------------------------------------------------------------------------------------------------------------------------------------*/

// SATA 接口.
/*------------------------------------------------------------------------------------------------------------------------------------*/
extern HRESULT SATA_ReadSector(
	DWORD32					dwDeviceID,
	DWORD32					dwSectorStartLBA,
	PBYTE8					pbMemDst,
	DWORD32					dwSectorsToRead,
	PDWORD32				pdwReadSectors
);

extern HRESULT SATA_ReadSectors(
	DWORD32					dwDeviceID,
	DWORD32					dwSectorStartLBA,
	PBYTE8					pbMemDst,
	DWORD32					dwSectorsToRead,
	PDWORD32				pdwReadSectors
);
extern HRESULT SATA_WriteSector(
	DWORD32					dwDeviceID,
	PBYTE8					pbMemSrc,
	DWORD32					dwSectorStartLBA,
	DWORD32					dwSectorsToWrite,
	PDWORD32				pdwWriteSectors
);

extern HRESULT SATA_WriteSectors(
	DWORD32			dwDeviceID,
	PBYTE8			pbMemSrc,
	DWORD32			dwSectorStartLBA,
	DWORD32			dwSectorsToWrite,
	PDWORD32		pdwWriteSectors
);

extern HRESULT SATA_HdCheck( PBYTE8 pVar );
extern HRESULT SATA_HdReadId(
	PCHAR				pszString,
	DWORD32				dwBytesToRead,
	PDWORD32			pdwReadBytes
);
extern HRESULT SATA_Reset( void );
extern int SATA_GetStatus();
extern WORD16 SATA_GetDiskSize();
/*------------------------------------------------------------------------------------------------------------------------------------*/

extern HRESULT NAND_ReadFlashId(
	PBYTE8					pbID,
	DWORD32					dwBytesToRead,
	PDWORD32				pdwReadBytes
);


//读函数
extern HRESULT NANDFLASH_Read(
	DWORD32 					dwSrcPos,		//从FLASH读取数据的起始位置.
	PBYTE8						pbMemDst, 		//指向内存中放置读到的数据.
	BYTE8						bReadType,		//FLASH读取模式,为0时表示为透明读,为1为加密读
	DWORD32						dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead	//实际读到的数据字节长度,为NULL则不?

);

//页写函数。执行此函数之前必须保证页内所有字节必须全为0xFF，否则必须先擦除
extern HRESULT NANDFLASH_Write(
	const PBYTE8 				pbMemSrc,		//指向所要读取的内存数据.
	DWORD32 					dwDstPos,		//FLASH写入数据目标起始位置.
	BYTE8 						bWriteType,		//FLASH写模式,0为透明写,1为加密写
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32 					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);

//页写函数。执行此函数之前必须保证页内所有字节必须全为0xFF，否则必须先擦除
extern HRESULT NANDFLASH_WritePage(
	const PBYTE8 				pbMemSrc,		//指向所要读取的内存数据.
	DWORD32 					dwDstPage,		//FLASH写入数据目标起始位置.
	BYTE8 						bWriteType,		//FLASH写模式,0为透明写,1为加密写
	DWORD32						dwPagesToWrite,	//期望写入的数据字节长度.
	PDWORD32 					pdwPagesWrite	//实际写入的数据字节长度,为NULL则不填.
);

//块擦除函数
extern HRESULT NANDFLASH_Erase(
	DWORD32						dwStartPos, 	//需要擦除FLASH块首位置.
	DWORD32 					dwLength		//当前需要擦除长度,以字节为单位.
);

//Blank 检查函数
extern HRESULT NANDFLASH_Verify(
	DWORD32						dwStartPos, 	//需要检查的FLASH块首位置.
	DWORD32 					dwLength		//当前需要检查的长度,以字节为单位.
);

/*
函数说明:该函数用来将将CPU的一些BUG总体性修改,故而该函数需要在主线程初始化时调用.
*/
extern HRESULT InitCPURegisterOk( void );

/*
函数说明:该函数用来检测当前是否是VVD的接口板.
*/
extern HRESULT IsVVDInterfaceBoard( void );

/* 函数说明:该函数用来根据bSetBit位值设置DPIO 3个触发输出的状态.函数无返回值. */
extern void SetTriggerOutDPIO(
	BYTE8 	bPos,				// 位置
	BYTE8 	bSetBit			// 设置值,只有最后一位有效(0或1)
);

#ifdef __cplusplus
	}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef _HVTARGET_H */
