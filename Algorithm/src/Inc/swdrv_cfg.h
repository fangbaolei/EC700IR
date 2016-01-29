#ifndef SWDRV_CFG_H_INCLUDED
#define SWDRV_CFG_H_INCLUDED

#include "errno.h"
#include "swbasetype.h"
#include "slw_dev.h"
#include "slw_pci.h"
#include <sys/ioctl.h>

#define SWDEV_NAME "/dev/swdev"
#define SWPCI_NAME "/dev/swpci"

#define SW_DATA_AD FPGA_STRUCT

//---------
// swdrv_base
#define SW_DEVID_DEV_DEF        0x1

#define SW_DEVID_CPLD_DEF       0x0

//---------
#define SW_DEVID_AD_DEF         0x0
#define SW_DEVID_AD_500M        0x1

//---------
#define SW_DEVID_EEPROM_DEF     0x0


//---------
// swdrv_devtrl
#define SW_DEVID_WTD_DEF        0x0

#define SW_MODE_WTD_ENABLE      0x1
#define SW_MODE_WTD_RESET       0x2

//---------
#define SW_DEVID_LIGHT_WORK_LED_DEF        0x1

#define SW_DEVID_LIGHT_ALL_LED_SINGLE      0xF0
#define SW_DEVID_LIGHT_WORK_LED_SINGLE     0xF1
#define SW_DEVID_LIGHT_LAN_LED_SINGLE_1    0xF2
#define SW_DEVID_LIGHT_LAN_LED_SINGLE_2    0xF3
#define SW_DEVID_LIGHT_HDD_LED_SINGLE      0xF4
#define SW_DEVID_LIGHT_STAT_LED_SINGLE     0xF5

#define SW_MODE_LIGHT_ON         0x1
#define SW_MODE_LIGHT_OFF        0x2
#define SW_MODE_LIGHT_FLASH      0x3

//---------
#define SW_DEVID_BUZZ_SINGLE            0x1

#define SW_MODE_BUZZ_ON                 0x1
#define SW_MODE_BUZZ_OFF                0x2

//---------
#define SW_DEVID_CRYPT_DEF              0x0

#define SW_OP_CRYPT_VERIFY              0x1
#define SW_MODE_CRYPT_WRITE             0x0
#define SW_MODE_CRYPT_READ              0x80

// SwGetSysStatus()
// devid
#define SW_DEVID_SYSSTAT_TEMPERATURE    0x01

//---------
// swdrv_cam

#define SW_DEVID_CAM_GAIN_DEF           0x0
#define SW_DEVID_CAM_GAIN_500M          0x1

//---------
#define SW_DEVID_CAM_RGBGAIN_DEF     0x0
#define SW_DEVID_CAM_RGBGAIN_500M    0x1

#define SW_DEVID_CAM_SHUTTER_DEF     0x0
#define SW_DEVID_CAM_SHUTTER_500M    0x1

//---------
#define SW_DEVID_CAM_TRIGGER_DEF     0x0
#define SW_DEVID_CAM_TRIGGER_500M    0x1

#define SW_MODE_CAM_TRIGGER_SOFT_CAP      0x0

//---------
#define SW_DEVID_CAM_CAP_GAIN_DEF     0x0
#define SW_DEVID_CAM_CAP_GAIN_500M    0x1

#define SW_MODE_CAM_CAP_GAIN_ENABLE       0x0
#define SW_MODE_CAM_CAP_GAIN_DISENABLE       0x1

//---------
#define SW_DEVID_CAM_CAP_RGBGAIN_DEF     0x0
#define SW_DEVID_CAM_CAP_RGBGAIN_500M    0x1

#define SW_MODE_CAM_CAP_RGBGAIN_ENABLE          0x0
#define SW_MODE_CAM_CAP_RGBGAIN_DISENABLE       0x1

//---------
#define SW_DEVID_CAM_CAP_SHUTTER_DEF     0x0
#define SW_DEVID_CAM_CAP_SHUTTER_500M    0x1

#define SW_MODE_CAM_CAP_SHUTTER_ENABLE          0x0
#define SW_MODE_CAM_CAP_SHUTTER_DISENABLE       0x1

//---------
#define SW_DEVID_CAM_FPS_DEF     0x0
#define SW_DEVID_CAM_FPS_500M    0x1

#define SW_MODE_CAM_FPS_ENABLE          0x0
#define SW_MODE_CAM_FPS_DISENABLE       0x1

//---------
#define SW_DEVID_CAM_CAP_EDGE_DEF     0x0
#define SW_DEVID_CAM_CAP_EDGE_500M    0x1

//---------
#define SW_DEVID_CAM_GAMMA_DATA_DEF     0x0
#define SW_DEVID_CAM_GAMMA_DATA_500M    0x1

//---------
#define SW_DEVID_CAM_SEG_DEF     0x0
#define SW_DEVID_CAM_SEG_500M    0x1

//---------
#define SW_DEVID_CAM_SYNCSIGN_DEF     0x0
#define SW_DEVID_CAM_SYNCSIGN_500M    0x1

#define SW_MODE_CAM_SYNCSIGN_FLASHRATE_ENABLE          0x0
#define SW_MODE_CAM_SYNCSIGN_FLASHRATE_DISENABLE       0x1

#define SW_MODE_CAM_SYNCSIGN_CAP_ENABLE          0x0
#define SW_MODE_CAM_SYNCSIGN_CAP_DISENABLE       0x1

//---------
#define SW_DEVID_CAM_CTRL_DEF          0x0
#define SW_DEVID_CAM_CTRL_500W          0x1
#define SW_DEVID_CAM_CTRL_SLAVE        0x2

#define SW_CTRLID_FGPA_TIME_CLR             0x1
#define SW_CTRLID_SLAVE_TRIGGER_CAP         0x2
#define SW_CTRLID_TRAFFIC_LIGHT_ENHANCE_PARAM     0x3
#define SW_CTRLID_TRAFFIC_LIGHT_ENHANCE_ZONE     0x4

//---------
// swdrv_pci

#define SW_DEVID_PCI_DEF          0x0

#define SW_MODE_SERIAL_IOFLUSH  0x00
#define SW_MODE_SERIAL_IFLUSH   0x01
#define SW_MODE_SERIAL_OFLUSH   0x02

#ifndef _CAMERA_PIXEL_500W_
#define SWDEV_IOCTL_AD9974_WRITE    -1
#endif

#endif // #ifndef SWDRV_CFG_H_INCLUDED
