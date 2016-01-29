#ifndef _TARGET_DEFINE_H
#define _TARGET_DEFINE_H

#define UNIQUE_ID_SUPPORT			1

/* 
	机制说明:系统硬件配置兼容性设计,为了保证底层接口设计的向下兼容性, 所以需要
		对硬件功能使用上作出相应的版本兼容设计,该兼容性设计是基于"主板+接口板"
		大致方案来确定的,主板的有关配置一般都只会有很大的变动,而接口板则根据
		需要在当前主板硬件基础上会有所不同,所以在整体硬件配置SYS_TARGET_CONFIG
		结构体中,只有从串口和GPIO会有所不同,而其余则不变(针对不同的大版本).
	硬件版本号取得:系统在初始化运行时会从CPLD中读取有关当前版本的信息,再从接口
		板上读取一些用户配置的接口状态,	从而整合生成当前硬件的具体版本号,如从
		CPLD中读得的版本号为"0x10",也即为1.0版(基本版本),从接口版读来的信息为
		0x03则表示当前版本号为1.0版,如果为0x02则为1.1版,如果为0x01则为1.2版,
		如果为0x00,则为1.3版.		
	兼容性使用:接口程序首先取得当前硬件的版本号,在一些接口函数的具体实现时
		会根据不同的配置而不同,上层用户不必理会具体的实现细节,如当前硬件接口
		支持触发输入输出,只需要调用相应的触发输入输出函数即可,如果当前硬件
		不支持触发输入输出则返回相应的错误值. */		
/*------------------------------------------------------------------------------------------------------*/
/* 串口功能枚举说明:用来指示串口外部接口功能使用,对于不同的串口接口功能使用,在初始化和使用时都会有所不同. */
typedef enum {
	UART_NOT_USED 		= 0,			//当前串口没有使用,以后对该串口的所有操作全部返回RET_VER_NOTSUPPORT错误.
	UART_RS232_CTRL		= 1,			//当前串口作为RS232使用,标准三线串口.
	UART_RS422_CTRL		= 1,			//RS422是单发多收的,所以在使用时可以当前一般的串口收发机制.
	UART_RS485_CTRL		= 2,			//RS485半工通讯时,是多发多收的,每次通讯时必有一个主设备,需要加入总线控制机制.
	UART_MODEM_CTRL		= 3				//全MODEM使用方式,初始化是不一样的,在全硬件流量控制情况下,操作与三线RS232一致.
} UART_COMM_TYPE;

/* 枚举描述:系统平台DSP所支持定时器宏定义. */
typedef enum {
	DSP_TIMER_DEV0 		= 0,
	DSP_TIMER_DEV1		= 1,
	DSP_TIMER_NUM		= 2
} DSP_TIMER_DEV;

#define DSP_IRQ_INT_START		4		//DSP硬件中断映射起始中断号.
#define DSP_IRQ_INT_CNT			15		//DSP硬件中断号最大数.

/* 枚举说明:该枚举为RTC时钟中断输出的时钟周期,因为1.0版本看门狗的工作时钟为该时钟,其基本工作时钟频率为32K,
RTC中断输出时钟频率初始化工作还在在Bootloader最开始处初始化. */
typedef enum {
	RTC_INT_1HZ 		= 0,
	RTC_INT_4KHZ		= 1,
	RTC_INT_8KHZ		= 2,
	RTC_INT_32KHZ		= 3
} RTC_INT_FREQUENCY;

/* 数据结构说明:该数据结构用来配置目标系统用户配置信息. */
typedef struct _SYS_GLOBAL_CONFIG {
    BOOL                fEmifInit;		//是否需要由用户程序去初始化硬件平台的EMIF接口.
    BOOL                fGpioInit;		//是否需要用户初始化GPIO片上外设.
    BOOL                fI2CInit;		//是否需要用户初始化I2C片上外设.
	BOOL				fTimerEnable;	//使用用户定时器,本库文件默认用户定时器为定时器2.
} SYS_GLOBAL_CONFIG;

/* 数据结构说明:该数据结构用来配置EMIF有关属性,将四类EMIFA接口属性(如数据访问时序的边缘有关属性),更为重要的是
SDRAM芯片的有关接口属性(如SDRAM内存页,读写时序属性).由于系统使用的原因,一般情况下,EMIFA接口配置会在Bootloader中
使用,而不会在正式程序运行时将其初始化(Bootloader是从EMIFA接口外设Flash中读取程序数据至SDRAM,因此时用户程序
在SDRAM中运行,有关初始化工作已在SDRAM中进行). */
typedef struct _EMIF_USER_CONFIG {
	DWORD32				dwGblCtl;		//EMIFA接口整体属性寄存器配置.
	DWORD32				dwCeCtl0;		//EMFIA CE0数据访问控制寄存器配置.
	DWORD32				dwCeCtl1;		//EMFIA CE1数据访问控制寄存器配置.
	DWORD32				dwCeCtl2;		//EMFIA CE2数据访问控制寄存器配置.
	DWORD32				dwCeCtl3;		//EMFIA CE3数据访问控制寄存器配置.
	DWORD32				dwSdCtl;		//EMIFA SDRAM外设控制寄存器配置.
	DWORD32				dwSdTim;		//EMIFA SDRAM 时序配置寄存器配置.
	DWORD32				dwSdExt;		//EMIFA SDRAM扩展寄存器配置.
	DWORD32				dwCeSec0;		//EMIFA CE0控制寄存器2配置.
	DWORD32				dwCeSec1;		//EMIFA CE1控制寄存器2配置.
	DWORD32				dwCeSec2;		//EMIFA CE2控制寄存器2配置.
	DWORD32				dwCeSec3;		//EMIFA CE3控制寄存器2配置.
} EMIF_USER_CONFIG;

/* 数据结构说明:该数据结构用来定义用户定时器有关属性,因为DSP定时器不一定会完全使用CPU分频而得到的时钟作为其基时钟,
也可使用外部输入时钟作为定时器工作时钟,所以会有定时器最小精度定时时间长度. */
typedef struct _TIMER_USER_CONFIG {	
	DWORD32 			dwTimerCtl;		//定时器控制寄存器配置.
	DWORD32				dwTimerPrd;		//定时器最小时间精度时使用工作时钟的周期数.
	DWORD32				dwTimerCnt;		//定时器当前工作时钟周期计数器.	
	DSP_TIMER_DEV		UserTimerDev;	//用户定时器索引号.
	DWORD32				dwTimerIRQ;		//用户定时器中断函数中断ID号.
	DWORD32				dwMilliseconds;	//定时器当前最小精度时间(单位为ms).
} TIMER_USER_CONFIG;

/* 数据结构说明:该数据结构用来配置串口的有关属性,不同的硬件版本对应于不同的串口使用情况. */
typedef struct _UART_USER_CONFIG {
	UART_COMM_TYPE		UartCommType;		//串口接口类型定义.
	DWORD32				dwUartBaudRate;		//串口的波特率.
	WORD16				wUartIcBuffLen;		//串口硬件收发缓冲区大小.
	WORD16				wFifoBuffLen;		//串口软件收发缓冲区大小.
} UART_USER_CONFIG;

/* 数据结构说明:该数据结构用来配置I2C外设的有关属性,如设备地址,EEPROM容量. */
typedef struct _I2C_USER_CONFIG {
	RTC_INT_FREQUENCY	RTCIntFrequecy;		//RTC中断输出时钟频率.
	DWORD32				dwEEPROMCapability;	//EEPROM的容量.
	DWORD32				dwPageSize;			//EEPROM的页面字节大小.
	BYTE8				bEEPROMDeviceAddr;	//EEPROM的设备地址,就I2C总线而言.
	BYTE8				bRTCDeviceAddr;		//RTC的设备地址,就I2C总线而言.
} I2C_USER_CONFIG;

/* 数据结构说明:该数据结构用来配置FLASH有关属性,用于控制FLASH访问. */
typedef struct _FLASH_USER_CONFIG {
	DWORD32				dwFlashCapability;	//FLASH的最大容量.
	DWORD32				dwFlashStartAddr;	//FLASH访问的起始地址.
	BOOL				fFlashProtected;	//FLASH是否需要加密保护.	
	DWORD32				dwFlashProtectedPos;//FLASH保护的起始地址.
	DWORD32				dwFlashProtectedLen;//FLASH保护的起始地址.
	DWORD32				dwFlashCacheSize;	//FLASH的CACHE操作缓冲区大小.
	DWORD32				dwFlashSectionSize;	//FLASH的Section大小;
} FLASH_USER_CONFIG;

/* 数据结构说明:该数据结构用来说明硬件平台的GPIO定义,以下只规定不同类型的GPIO,对于一些单项的GPIO定义则表明
硬件平台必有这个外设. */
typedef struct _GPIO_USER_CONFIG {
	DWORD32				dwWatchDogIO;		//当且仅当只有一个GPIO作来外部看门狗WDI信号.
	DWORD32				dwSpringInput;		//当前最多只有一个GPIO作来触发输入.
	DWORD32				dwSpringOutput;		//当前最多只有一个GPIO作为触发输出.
	DWORD32				dwIdSigIO;			//作为唯一ID芯片操作IO,该IO自动作为Input/Output来处理.
	DWORD32				dwWorkLEDIO;		//当前最多只有一个GPIO作来工作指示灯来使用.
	DWORD32				dwLedIO;			//将LED一体操作的,此时将LED灯作为一组,只有操作到该组内的GPIO才会有效.
	DWORD32				dwVerInputIO;		//硬件版本号的小版本号输入GPIO.
	DWORD32				dwDebugOutputIO;	//将GPIO作为DEBUG输出的放在一起操作(不包括LED灯),只有操作该组合内的GPIO才会有效,调试时使用.
} GPIO_USER_CONFIG;

/* 数据结构说明:该数据结构用来全局定义接口部分所使用的硬件配置. */
typedef struct _SYS_TARGET_CONFIG {
	DWORD32				dwHardwareVer;		//硬件版本号.
	DWORD32				dwDspDevCfg;		//DSP的片上设备配置.
	SYS_GLOBAL_CONFIG	SysGlobalCfg;		//全局系统配置.
	EMIF_USER_CONFIG	EmifUserCfg;		//EMFIA接口配置.
	FLASH_USER_CONFIG	FlashUserCfg;		//FLASH操作配置.
	TIMER_USER_CONFIG	TimerUserCfg;		//用户定时器配置.
	I2C_USER_CONFIG		I2CUserCfg;			//I2C总线外设配置.
	GPIO_USER_CONFIG	GPIOUserCfg;		//GPIO用户配置.
	UART_USER_CONFIG	MainUserCfg;		//主串口配置.	
	UART_USER_CONFIG	rgSlaveUserCfg[3];	//从串口配置.
} SYS_TARGET_CONFIG;


/* 函数说明:该函数用来设置当前硬件版本号,用于取得当前所有单项配置属性. */
extern HRESULT				SetTargetHardVer( 
	DWORD32 				dwHardVer				//平台硬件版本号.
);

/* 函数说明:以下函数均为取得各类子项配置的数据指针,这样做的好处是避免多一次数据复制. */
extern DWORD32 				*GetDspDevCfgPtr( void );
extern SYS_GLOBAL_CONFIG	*GetSysGlbCfgPtr( void );
extern EMIF_USER_CONFIG		*GetEmifCfgPtr( void );
extern FLASH_USER_CONFIG	*GetFlashCfgPtr( void );
extern TIMER_USER_CONFIG	*GetTimerCfgPtr( void );
extern I2C_USER_CONFIG		*GetI2CCfgPtr( void );
extern GPIO_USER_CONFIG		*GetGpioCfgPtr( void );
extern UART_USER_CONFIG		*GetMainUartCfgPtr( void );
extern UART_USER_CONFIG		*GetSlaveUartCfgPtr( void );

extern BYTE8 VdADDeviceAddr[3];

#endif /* #ifndef _TARGET_DEFINE_H */


