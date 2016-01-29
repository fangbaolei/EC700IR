

#ifndef __SLW_CPLD_DEV__
#define __SLW_CPLD_DEV__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw_gpio.h"
#include "slw_spi.h"


#define GPIO_WATCHDOG  5

enum {
  CPLD_LAN2_LED = 0,
  CPLD_STAT_LED,
  CPLD_WORK_LED,
  CPLD_LAN1_LED,
  CPLD_HARD_LED,
  CPLD_BUZZER,
  CPLD_MainBoard_LED1,
  CPLD_MainBoard_LED2,
  CPLD_RELAY,
  CPLD_USB_POWER,
  CPLD_MAX_DEV
};

#define CPLD_MAX_LED CPLD_RELAY

// CPLD设备
typedef struct _cpld_dev
{
	Uint8 index;
	Uint8 addr; // 地址
	Uint8 bit;  // 位移
} cpld_dev;

// 函数说明:该函数用来将WDT机制狗的状态有个跳变
// 函数无形参
// 函数无返回值
void ResWDT(void);

// 该函数用来读取CPLD的版本号(0x20)
void get_cpld_version(Uint8* version);

// 函数说明:控制CPLD设备的开关
// 函数形参: dev为设备序号
//           on_off 0为关,非0为开
// 函数无返回值
void set_cpld_dev(Uint8 dev, Uint8 on_off);

// 函数说明:控制CPLD的LED设备闪烁,如为亮则调用该函数之后灯即灭
// 函数形参:dev为设备序号
// 函数无返回值
void cpld_led_blink(Uint8 dev);

#ifdef __cplusplus
}
#endif

#endif // __SLW_CPLD_DEV__
