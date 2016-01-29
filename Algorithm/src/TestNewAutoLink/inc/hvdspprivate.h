#ifndef _HVDSP_PRIVATE_H
#define _HVDSP_PRIVATE_H

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */

#include "dspsys_def.h"
#include "dsp64xdefine.h"
#include "hvtarget.h"

/* Jpeg压缩类私为内部函数定义. */
/*------------------------------------------------------------------------------------------------------*/
/*
函数说明:该函数用来初始化JPEG压缩运算时所使用的栈空间分配.
函数返回S_OK表示初始化成功.*/
extern HRESULT Jpeg_MemInit(
	int 						iInHeap, 		//片内堆名称.
	int 						iExtHeap 		//片外SDRAM堆名称.
);
/*------------------------------------------------------------------------------------------------------*/

/* FLASH文件系统类私为内部函数定义. */
/*------------------------------------------------------------------------------------------------------*/
/*
函数说明:该函数用来将文件系统工作机制初始化,输入的dwInitFileNum为0时表示将所有有关
	文件系统中的文件头读至内存,以便于文件系统操作机制实现.
函数返回值:
	返回S_OK表示初始化成功.
	返回E_FAIL表示硬件底层操作出现异常.
	返回S_FALSE表示初始化失败,如当前没有平台没有文件系统等. */
extern HRESULT FILE_InitFileSystem( 
	DWORD32 					dwInitFileNum		//初始化文件数目.
);

/*
函数说明:该函数用来初始化FLASH CACHE操作数据结构.
函数返回值:
	返回S_OK表示初始化成功.
	返回E_POINTER表示pFlashCfg为INVALID_POINTER.
	返回E_FAIL表示初始化失败,此后对FLASH CACHE操作方式全部失效. */
extern HRESULT InitFlash( 
	FLASH_USER_CONFIG			*pFlashCfg
);

/*
函数说明:该函数用来从FLASH中读取数据,由于该FLASH读操作不需要任何命令类型码写操作,
	所以可以直接对FLASH的任意空间进行读操作.
函数返回值:
	返回S_OK表示读取成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,读取的数据超出FLASH当前容量等.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取FLASH出错. */
extern HRESULT FLASH_CacheRead( 
	DWORD32 					dwSrcPos,		//从FLASH读取数据的起始位置.
	PBYTE8						pbMemDst, 		//指向内存中放置读到的数据.
	DWORD32						dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead	//实际读到的数据字节长度,为NULL则不填.
);

/* 
函数说明:该函数用来向FLASH中写入数据,FLASH的地址空间为连续的地址空间,但是FLASH的读写
	操作必须要使用专用的FLASH读写封装函数,否则会出现不可预知的结果.
函数返回值:
	返回S_OK表示写入操作成功.
	返回E_POINTER表示pbMemSrc,为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,写入的数据超出FLASH当前容量等.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写操作FLASH出错. */
extern HRESULT FLASH_CacheWrite( 
	const PBYTE8 				pbMemSrc,		//指向所要读取的内存数据.
	DWORD32 					dwDstPos,		//FLASH写入数据目标起始位置.
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32 					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);


/*
函数说明:该函数用来将FLASH CACHE中的数据写入至FLASH中, FLASH的地址空间为连续的地址空间,
	但是FLASH在写操作时,如果当前的写操作所涉及的数据非常敏感时,应在执行写操作之后,
	接着执行一次FlushFlash操作,以使得将FLASH CACHE中的内容真正更新至外部FLASH中.
函数返回值:
	返回S_OK表示操作成功.
	返回E_FAIL表示操作失败.	 */
extern HRESULT FlushFlash( void );

/*
函数说明:读取FLASH ROM芯片的设备ID号.
函数返回值:
	返回S_OK表示读取成功.
	返回E_POINTER表示pdwFlashID为INVALID_POINTER.
	返回E_FAIL表示读取ID芯片的ID号失败.	*/
extern HRESULT FLASH_readID(
	PDWORD32					pdwFlashID		//指向所取得的ID内容.
);

/*------------------------------------------------------------------------------------------------------*/


/* 基本接口类私为内部接口函数定义. */
/*------------------------------------------------------------------------------------------------------*/
typedef void * TIMER_HANDLE;			/* DSP片上用户时钟模块的操作句柄. */
typedef void * I2C_HANDLE;				/* DSP片上外设I2C模块的操作句柄. */
typedef void * GPIO_HANDLE;				/* DSP片上外设GPIO模块的操作句柄. */

/* 数据结构说明:该数据结构相关域用来指示目标板系统外设的操作句柄. */
typedef struct SYS_DEVICE_HANDLE {
	I2C_HANDLE 					hSysI2C;		/* I2C设备句柄.												*/
	GPIO_HANDLE					hSysGpio;		/* GPIO设备句柄.                                            */
	TIMER_HANDLE				hSysTimer;		/* 用户时钟设备句柄.                                        */
} SYS_DEVICE_HANDLE;

/*
函数说明:该函数用来初始化目标系统,初始化DSP片上外设工作环境,以及其他外设工作环境配置.该函数一次将系统
	初始化工作做完,并且取回有关硬件平台的信息.该函数为自动从当前平台的版本号中取出当前的硬件版本信息,
	并且根据当前硬件版本的不同而选择不同的初始化方式.
函数返回值:
	返回S_OK表示初始化成功;
	返回S_FALSE表示初始化失败;
	返回E_FAIL则表示为硬件系统级错误.*/
extern HRESULT InitHvSysTarget(
	SYS_DEVICE_HANDLE			*pSysHandle		//指向设备句柄,为NULL时则不传回设备句柄.	
);

extern HRESULT InitHvSysTargetPrivate( void );

/* 
函数说明:函数是在周期函数中执行的(软中断函数/时钟中断中,故而在周期函数不可关闭全局中断或者
	使能全局中断使能标志位.函数通过检测两个串口的发送队列以及串口IC的FIFO状态来确定当前是否
	应向串口IC发送数据.
函数返回值:
	返回S_OK表示成功.
	返回E_FAIL表示发送数据出现异常,如当前系统串口都没有初始化. */
extern HRESULT SendDataToUart( void );

/*
函数说明:函数是在周期函数中执行的(软中断函数/时钟中断中,故而在周期函数不可关闭全局中断或者
	使能全局中断使能标志位.函数通过检测串口IC接收FIFO状态来确定当前是否应从串口IC的FIFO读取数据.
函数返回值:
	返回S_OK表示成功.
	返回E_FAIL表示发送数据出现异常,如当前系统串口都没有初始化. */
extern HRESULT ReadDataFromUart( void );

/*
函数说明:用来初始化硬件平台的DSP片上外设兼容寄存器.
函数返回值:
	返回S_OK表示初始化成功;
	返回E_FAIL表示系统级异常(寄存器设置值读出判断不一致). */
extern HRESULT InitDeviceConfig( 
	DWORD32 					dwDeviceCfg		//片上外设兼容性设置值.
);

/*
函数说明:该函数用来初始化DM642的EMIFA接口.
函数返回值:
	返回S_OK表示初始化成功;
	返回E_FAIL表示初始化失败(寄存器设置值读出判断不一致).
	返回E_POINTER表示pSysInfo为INVALID_POINTER; */
extern HRESULT InitSystemEmif( 
	const EMIF_USER_CONFIG		*pEmifConfig	//指向EMIF配置数据.
);

/* 
函数说明:该函数用来初始化DSP片上外设I2C模块.当前的平台所使用的DSP I2C模块端口只有一个即为I2C_PORT0,
	本函数用来初始化该I2C端口.
函数返回值:
	返回S_OK表示初始化成功;
	返回E_FAIL表示初始化失败;
	返回E_POINTER表示pHandle为INVALID_POINTER; */
extern HRESULT InitTargetI2CBus( 
	I2C_HANDLE					*pHandle		//指向I2C设备句柄.
);

/*
函数说明:该函数用来初始化DSP片上外设GPIO,将当前平台上的作为GPIO来使用的相关GPIO引脚功能使能,
	在初始化成功之后,所有对GPIO的操作均需使用该GPIO设备句柄.DSP的GPIO有16个,每个IO对应于16位
	中的一位,输入的MASK参数中如果某位为1则表示设置当前IO为该功能.
函数返回值:
	返回S_OK表示初始化成功;
	返回S_FALSE表示初始化失败;
	返回E_POINTER表示pHandle为INVALID_POINTER; */
extern HRESULT InitTargetGPIO(
	GPIO_HANDLE					*pHandle,		//指向GPIO设备句柄.
	DWORD32						dwGpioInputMask,//用来标识GPIO作为输入的MASK.
	DWORD32						dwGpioOutputMask,//用来标识GPIO作为输出的MASK.
	DWORD32						dwGpioInOutMask	//用来标识GPIO作为输入输出的MASK.
);

/* 
函数说明:该函数用来初始化DSP片上时钟,当前平台DSP所支持的时钟为时钟0,时钟1.
	在初始化成功之后,所有对用户时钟的操作均需要该时钟设备句柄.
函数返回值:
	返回S_OK表示初始化成功;
	返回E_FAIL表示初始化失败;
	返回E_POINTER表示pHandle或者pTimerConfig为INVALID_POINTER;
	返回E_INVALIDARG表示参数错误,如时钟设备号不合法或者硬件中断映射号不合法. */
extern HRESULT InitTargetTimer(
	TIMER_HANDLE				*pHandle,	
	const TIMER_USER_CONFIG		*pTimerConfig
);

/*
函数说明:该函数用来初始化目标板串口,将其作为简单三线串口来使用.只有正确成功地初始化串口之后,
	对串口的操作才有可能会成功.
函数返回值:
	返回S_OK表示初始化成功;
	返回S_FALSE表示初始化失败;
	返回E_INVALIDARG表示参数错误. */
extern HRESULT COM_InitPortNormal(
	SYS_COM_PORT				ComPort,		//当前所要初始化的串口号.
	DWORD32 					dwBaudRate		//串口波特率.
);

/*
函数说明:该函数用来初始化目标板串口,将其作为Modem口来使用.只有正确成功地初始化串口之后,
	对串口的操作才有可能会成功.
函数返回值:
	返回S_OK表示初始化成功;
	返回S_FALSE表示初始化失败;
	返回E_INVALIDARG表示参数错误. */
extern HRESULT COM_InitPortModem(
	SYS_COM_PORT				ComPort,		//当前所要初始化的串口号.
	DWORD32 					dwBaudRate		//串口波特率.
);

/*
函数说明:该函数用来初始化系统串口功能,根据硬件版本号的不同而选用不同的初始化方式.
函数返回值:
	返回S_OK表示初始化成功.
	返E_FAIL表示初始化失败. */
extern HRESULT InitSysUart( void );

/*
函数说明:该函数用来取得硬件版本号细分配置值. 
函数返回值:
	返回S_OK表示取值成功.
	返回E_POINTER表示pbConfigVal为INVALID_POINTER. */
extern HRESULT GetHardVerConfig( 
	PBYTE8						pbConfigVal
);

/* 
函数说明:用来启动hHandel设备句柄所指向的时钟.
函数返回值:
	返回S_OK表示操作成功.
	返回E_HANDLE表示hHandle为INVALID_HANDLE. */
extern HRESULT StartTimer( 
	TIMER_HANDLE				hHandle			//当前需要启动时钟的设备句柄.
);

/* 
函数说明:该函数返回由hHandle所指向的时钟当前走时秒数,该读取用户时钟操作会先将该时钟暂停.
函数返回值:
	返回S_OK表示读取成功.
	返回E_POINTER表示pdwSecondTime为INVALID_POINTER.
	返回E_NOTIMPL表示用户时钟没有初始化. */
extern HRESULT GetSecond(
	PDWORD32					pdwSecondTime	//指向取得的秒数.
);

/* 
函数说明:该函数返回由hHandle所指向的时钟当前走时秒数,该读取用户时钟操作会先将该时钟暂停.
函数返回值:
	返回S_OK表示读取成功.
	返回E_POINTER表示pdwMSecondTime为INVALID_POINTER.
	返回E_NOTIMPL表示用户时钟没有初始化. */
extern HRESULT GetMSecond(
	PDWORD32					pdwMSecondTime	//指向取得的秒数.
);

/*
函数说明:该函数用来取得目标系统硬件版本号和外设的在关信息.
函数返回值:
	返回S_OK表示取信息成功;
	返回E_POINTER表示pSysHandle为INVALID_POINTER; */
extern HRESULT GetSysDevHandle(
	SYS_DEVICE_HANDLE			*pSysHandle		//指向外设设备句柄数据.
);

/* 函数说明:该函数为10NS为单位的延迟,延迟时间为"dwDelayTime * 10ns ".函数无返回值. */
extern void Target_DelayTNS(
	DWORD32 					dwDelayTime		//延迟10NS次数.
);
/* 函数说明:该函数为微秒为单位的延迟,延迟时间为"dwDelayTime * 1us ".函数无返回值. */
extern void Target_DelayUS(
	DWORD32 					dwDelayTime		//延迟US次数.
);


/* 
函数说明:该函数用来从硬件平台的DSP地址空间取得一地址数据,函数不检查要取数据地址是否合法,
只将dwEmifAddr地址的数据读回而已,而对该地址的读操作是否对当前系统正常工作产生影响,本函数不
提供任何保护措施.函数直接返回读到的值. */
#define GetEmifData( dwEmifAddr )	( *( ( volatile DWORD32 * )( dwEmifAddr ) ) )

/* 
函数说明:该函数用来向硬件平台的DSP地址空间一地址写入数据,函数不检查要写入数据的地址是否合法,
只将数据dwSetData写入到dwEmifAddr地址,而对该地址的写操作是否对当前系统正常工作产生影响,本函数不
提供任何保护措施.函数无返回值.*/
#define SetEmifData( dwEmifAddr, dwSetData )	*( ( volatile DWORD32 * )( dwEmifAddr ) ) = ( dwSetData )

/*
函数说明:该函数用来从I2C设备(不包括EEPROM)中读取一些寄存器的值.
函数返回值:
	返回S_OK表示读取I2C设备操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如ID号不符要求.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取I2C设备出错. */
extern HRESULT I2C_ReadDevReg(
	BYTE8 						bSlaveDevAddr,	//I2C从设备设备地址.
	BYTE8 						bDevSubAddr,	//读取I2C设备寄存器首地址.
	PBYTE8 						pbMemDst,		//指向读出的数据.
	BYTE8						bBytesToRead,	//期望读取的数据字节长度.
	PBYTE8 						pbBytesRead		//指向实际读取的数据字节长度,如为NULL则不填.
);

/*
函数说明:该函数用来向I2C设备(不包括EEPROM)寄存器写入数据.
函数返回值:
	返回S_OK表示写入I2C设备操作成功.
	返回E_POINTER表示pbMemSrc为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错,如ID号不符要求.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写入I2C设备出错. */
extern HRESULT I2C_WriteDevReg(
	BYTE8 						bSlaveDevAddr,	//I2C从设备设备地址.
	const PBYTE8				pbMemSrc, 		//指向需要写入至I2C设备的数据.
	BYTE8 						bDevSubAddr, 	//写入I2C设备寄存器首地址.
	BYTE8						bBytesToWrite,	//期望写入的数据字节长度.
	PBYTE8 						pbBytesWrite	//指向实际写入的数据字节长度,如为NULL则不填.
);

/*
函数说明:该函数用来初始化时钟的中断输出时钟频率.
函数返回值:
	返回S_OK表示操作成功.
	返回E_FAIL表示操作出现系统级异常.
	返回S_FALSE表示操作失败. */
extern HRESULT InitRTCInt(
	RTC_INT_FREQUENCY 			IntFrequency
);
/*------------------------------------------------------------------------------------------------------------------------------------*/

/*
函数说明:该函数用来从DSP的EMAC模块上取得MAC地址值.
函数返回值:
	返回S_OK表示读取成功.
	返回E_FAIL表示读取寄存器失败.
	返回E_INVALIDARG表示参数不合法.
	返回E_POINTER表示pbData为NULL> */
extern HRESULT GetEtherMacAddr( 
	PBYTE8				pbData,
	DWORD32				dwSize,
	PDWORD32			pdwLen
);

extern void FLASH_SetSection( DWORD32 dwExtAddr );
extern FLASH_USER_CONFIG *g_pFlashCfg;

/* 初始化EEPROM */
extern HRESULT EEPROM_Init();

extern HRESULT ReadFlashParam(BYTE8* pbDat, DWORD32 dwLen);
extern HRESULT WriteFlashParam(BYTE8* pbDat, DWORD32 dwLen);

extern HRESULT CheckVideoPort( int iPort, BYTE8 *pbVariable );
extern HRESULT CheckUserConfigDefault( BYTE8 *pbVariable );
extern HRESULT SetUserConfigVariable( void );

/*
函数说明:该函数用来从EEPROM中读取一定数据的数据.
函数返回值:
	返回S_OK表示读取EEPROM操作成功.
	返回E_POINTER表示pbMemDst为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示读取EEPROM出错. */
extern HRESULT EEPROM_CryptRead(
	DWORD32						dwReadPos,		//读取EEPROM的首位置.
	PBYTE8 						pbMemDst, 		//指向读出的数据.
	DWORD32						dwBytesToRead,	//期望读取的数据字节长度.
	PDWORD32					pdwBytesRead		//实际读取的数据字节长度,为NULL则不填.
);
/*
函数说明:该函数用来向EEPROM中写入一定量数据.
函数返回值:
	返回S_OK表示写入EEPROM操作成功.
	返回E_POINTER表示pbMemSrc为INVALID_POINTER.
	返回E_INVALIDARG表示传入的参数有错.
	返回E_FAIL表示硬件底层操作出错.
	返回S_FALSE表示写入EEPROM出错. */
extern HRESULT EEPROM_CryptWrite(
	const PBYTE8				pbMemSrc, 		//指向写入的数据.
	DWORD32 					dwWritePos, 	//写入EEPROM的首地址.
	DWORD32						dwBytesToWrite,	//期望写入的数据字节长度.
	PDWORD32					pdwBytesWrite	//实际写入的数据字节长度,为NULL则不填.
);

void SetUart485Trans( int iPort );
void SetUart485Recv( int iPort );


HRESULT PCI_Init( void );

extern BYTE8 GetCpldVersion( void );

extern PBYTE8 GetDMAResultMemoryPtr( void );

extern PBYTE8 GetDMAVideoMemoryPtr( void );

extern PBYTE8 GetDMAReadMemoryPtr( void );
#ifdef __cplusplus
	}
#endif	//#ifdef __cplusplus

#endif //#ifndef _HVDSP_PRIVATE_H
