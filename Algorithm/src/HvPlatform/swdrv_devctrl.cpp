#include "swdrv.h"
#include <sys/ioctl.h>
#include <pthread.h>

// 看门狗使能
#define CPLD_WTD_ENABLE 0x13

// LED地址
#ifdef SINGLE_BOARD_PLATFORM
#define CPLD_LED_ADDR 0x0b
#else
#define CPLD_LED_ADDR 0x0e
#endif

// LED寄存器位定义
#define BUZZER_BIT 0x80
#define HDD_BIT 0x20
#define STAT_BIT 0x10
#define LAN1_BIT 0x08
#define LAN2_BIT 0x04
#define WORK_BIT 0x02

static BOOL g_fCanLightUp = FALSE;
static pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C"
{

    static int EnableWTD();
    static int ResetWTD();

// 点LED
    static void SetAllLightOn(int iDevID);
    static void SetAllLightOff(int iDevID);
    static void SetAllLightFlash(int iDevID);
    static void SetWorkLedOn(int iDevID);
    static void SetWorkLedOff(int iDevID);
    static void SetWorkLedFlash(int iDevID);
    static void SetLanLedOn(int iDevID);
    static void SetLanLedOff(int iDevID);
    static void SetLanLedFlash(int iDevID);
    static void SetHddLedOn(int iDevID);
    static void SetHddLedOff(int iDevID);
    static void SetHddLedFlash(int iDevID);
    static void SetStatLedOn(int iDevID);
    static void SetStatLedOff(int iDevID);
    static void SetStatLedFlash(int iDevID);

// 蜂鸣器
    static void SetBuzzerOn();
    static void SetBuzzerOff();
    static int GetCpuNearTemp(int* piTemp);

// verify.c
    extern "C" int verify_crypt(void);
    extern "C" int write_crypt(unsigned int* rgNC);

//--------------------------------------------------------------------
// 看门狗

    int SwWTDCtrl(int iMode)
    {
        int iRet = EINVAL;
        switch (iMode)
        {
        case SW_MODE_WTD_ENABLE :
            iRet = EnableWTD();
            break;

        case SW_MODE_WTD_RESET :
            iRet = ResetWTD();
            break;

        default :
            break;
        }

        return iRet;
    }

//看门狗使能
    static int EnableWTD()
    {
        int iRet = -1;
        iRet = SwCpldWrite(CPLD_WTD_ENABLE, 1);
        return iRet;
    }

//看门狗复位
    static int ResetWTD()
    {
        int fd;
        int iRet = EBUSY;

        fd = SwDevOpen(O_RDWR);
        if (fd >= 0)
        {
            iRet = ioctl(fd, SWDEV_IOCTL_WATCHDOG, 0);
            SwDevClose(fd);
        }

        return iRet;
    }

//////////////////////////////////////////////////////////////////////
// 系统状态外设
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//点灯
    int SwSetLight(int iDevID, int iMode)
    {
        int iRet = 0;
        switch (iDevID)
        {
        case SW_DEVID_LIGHT_ALL_LED_SINGLE :
            switch (iMode)
            {
            case SW_MODE_LIGHT_ON :
                SetAllLightOn(iDevID);
                break;

            case SW_MODE_LIGHT_OFF :
                SetAllLightOff(iDevID);
                break;

            case SW_MODE_LIGHT_FLASH :
                SetAllLightFlash(iDevID);
                break;

            default :
                iRet = EINVAL;
            }
            break;

        case SW_DEVID_LIGHT_WORK_LED_DEF:
        case SW_DEVID_LIGHT_WORK_LED_SINGLE :
            switch (iMode)
            {
            case SW_MODE_LIGHT_ON :
                SetWorkLedOn(iDevID);
                break;

            case SW_MODE_LIGHT_OFF :
                SetWorkLedOff(iDevID);
                break;

            case SW_MODE_LIGHT_FLASH :
                SetWorkLedFlash(iDevID);
                break;

            default :
                iRet = EINVAL;
            }
            break;

        case SW_DEVID_LIGHT_LAN_LED_SINGLE_1 :
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_2 :
            switch (iMode)
            {
            case SW_MODE_LIGHT_ON :
                SetLanLedOn(iDevID);
                break;

            case SW_MODE_LIGHT_OFF :
                SetLanLedOff(iDevID);
                break;

            case SW_MODE_LIGHT_FLASH :
                SetLanLedFlash(iDevID);
                break;

            default :
                iRet = EINVAL;
            }
            break;

        case SW_DEVID_LIGHT_HDD_LED_SINGLE :
            switch (iMode)
            {
            case SW_MODE_LIGHT_ON :
                SetHddLedOn(iDevID);
                break;

            case SW_MODE_LIGHT_OFF :
                SetHddLedOff(iDevID);
                break;

            case SW_MODE_LIGHT_FLASH :
                SetHddLedFlash(iDevID);
                break;

            default :
                iRet = EINVAL;
            }
            break;

        case SW_DEVID_LIGHT_STAT_LED_SINGLE :
            switch (iMode)
            {
            case SW_MODE_LIGHT_ON :
                SetStatLedOn(iDevID);
                break;

            case SW_MODE_LIGHT_OFF :
                SetStatLedOff(iDevID);
                break;

            case SW_MODE_LIGHT_FLASH :
                SetStatLedFlash(iDevID);
                break;

            default :
                iRet = EINVAL;
            }
            break;

        default :
            iRet = EINVAL;
        }

        return iRet;
    }

    static void SetAllLightOn(int iDevID)
    {
        g_fCanLightUp = FALSE;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            bTemp |= (HDD_BIT|STAT_BIT|LAN1_BIT|LAN2_BIT|WORK_BIT);
            SwCpldWrite(CPLD_LED_ADDR, bTemp);
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetAllLightOff(int iDevID)
    {
        if (!g_fCanLightUp)
        {
            volatile BYTE8 bTemp;
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                bTemp &= ~(HDD_BIT|STAT_BIT|LAN1_BIT|LAN2_BIT|WORK_BIT);
                SwCpldWrite(CPLD_LED_ADDR, bTemp);
            }
            pthread_mutex_unlock(&g_Mutex);
            g_fCanLightUp = TRUE;
        }
    }

    static void SetAllLightFlash(int iDevID)
    {
        g_fCanLightUp = FALSE;
        volatile BYTE8 bTemp, bMask;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            bMask = HDD_BIT|STAT_BIT|LAN1_BIT|LAN2_BIT|WORK_BIT;
            bTemp = ((~bTemp) & bMask) | (bTemp & ~bMask);
            SwCpldWrite(CPLD_LED_ADDR, bTemp);
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetWorkLedOn(int iDevID)
    {
        switch (iDevID)
        {
        case SW_DEVID_LIGHT_WORK_LED_DEF :
            SwCpldWrite(CPLD_LED_ADDR, 3);
            break;

        case SW_DEVID_LIGHT_WORK_LED_SINGLE :
        {
            if (!g_fCanLightUp) return;
            volatile BYTE8 bTemp;
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                SwCpldWrite(CPLD_LED_ADDR, bTemp |= WORK_BIT);
            }
            pthread_mutex_unlock(&g_Mutex);
        }
        break;
        default :
            break;
        }
    }

    static void SetWorkLedOff(int iDevID)
    {
        switch (iDevID)
        {
        case SW_DEVID_LIGHT_WORK_LED_DEF :
            SwCpldWrite(CPLD_LED_ADDR, 0);
            break;
        case SW_DEVID_LIGHT_WORK_LED_SINGLE :
        {
            if (!g_fCanLightUp) return;
            volatile BYTE8 bTemp;
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                SwCpldWrite(CPLD_LED_ADDR, bTemp &= (~WORK_BIT));
            }
            pthread_mutex_unlock(&g_Mutex);
        }
        break;
        default :
            break;
        }
    }

    static void SetWorkLedFlash(int iDevID)
    {
        switch (iDevID)
        {
        case SW_DEVID_LIGHT_WORK_LED_DEF :
        {
            // 第0位：主板调试灯
            // 第1位：工作指示灯
            static BYTE8 bTemp = 0;
            bTemp = (0 == bTemp) ? (3) : (0);
            SwCpldWrite(CPLD_LED_ADDR, bTemp);
        }
        break;
        case SW_DEVID_LIGHT_WORK_LED_SINGLE :
        {
            if (!g_fCanLightUp) return;
            volatile BYTE8 bTemp;
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                SwCpldWrite(CPLD_LED_ADDR, ((~bTemp) & WORK_BIT) | (bTemp & (~WORK_BIT )));
            }
            pthread_mutex_unlock(&g_Mutex);
        }
        break;
        default :
            break;
        }
    }

    static void SetLanLedOn(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;

        switch (iDevID)
        {
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_1:
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_2:
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                BYTE8 bBit = (iDevID == SW_DEVID_LIGHT_LAN_LED_SINGLE_1)
                             ? LAN1_BIT : LAN2_BIT;
                SwCpldWrite(CPLD_LED_ADDR, bTemp |= bBit);
            }
            pthread_mutex_unlock(&g_Mutex);
            break;

        default :
            break;
        }
    }

    static void SetLanLedOff(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;

        switch (iDevID)
        {
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_1:
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_2:
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                BYTE8 bBit = (iDevID == SW_DEVID_LIGHT_LAN_LED_SINGLE_1)
                             ? LAN1_BIT : LAN2_BIT;
                SwCpldWrite(CPLD_LED_ADDR, bTemp &= (~bBit));
            }
            pthread_mutex_unlock(&g_Mutex);
            break;

        default :
            break;
        }
    }

    static void SetLanLedFlash(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        switch (iDevID)
        {
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_1:
        case SW_DEVID_LIGHT_LAN_LED_SINGLE_2:
            pthread_mutex_lock(&g_Mutex);
            if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
            {
                BYTE8 bBit = (iDevID == SW_DEVID_LIGHT_LAN_LED_SINGLE_1)
                             ? LAN1_BIT : LAN2_BIT;
                SwCpldWrite(CPLD_LED_ADDR, ((~bTemp) & bBit) | (bTemp & (~bBit )));
            }
            pthread_mutex_unlock(&g_Mutex);
            break;

        default :
            break;
        }
    }

    static void SetHddLedOn(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, bTemp |= HDD_BIT);
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetHddLedOff(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, bTemp &= (~HDD_BIT));
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetHddLedFlash(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR,
                        ((~bTemp) & HDD_BIT) | (bTemp & (~HDD_BIT )));
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetStatLedOn(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, bTemp |= STAT_BIT);
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetStatLedOff(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, bTemp &= (~STAT_BIT));
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetStatLedFlash(int iDevID)
    {
        if (!g_fCanLightUp) return;
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, ((~bTemp) & STAT_BIT) | (bTemp & (~STAT_BIT )));
        }
        pthread_mutex_unlock(&g_Mutex);
    }

//--------------------------------------------------------------------
// 蜂鸣器
    int SwSetBuzz(int iMode)
    {
        int iRet = 0;
        switch (iMode)
        {
        case SW_MODE_BUZZ_ON :
            SetBuzzerOn();
            break;

        case SW_MODE_BUZZ_OFF :
            SetBuzzerOff();
            break;

        default :
            iRet = EINVAL;
        }
        return iRet;
    }

    static void SetBuzzerOn()
    {
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead( CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, bTemp |= BUZZER_BIT);
        }
        pthread_mutex_unlock(&g_Mutex);
    }

    static void SetBuzzerOff()
    {
        volatile BYTE8 bTemp;
        pthread_mutex_lock(&g_Mutex);
        if (0 == SwCpldRead(CPLD_LED_ADDR, (unsigned char*)&bTemp))
        {
            SwCpldWrite(CPLD_LED_ADDR, bTemp &= (~BUZZER_BIT));
            // 关闭蜂鸣器后LED才允许点亮，否则有可能会造成蜂鸣器长响
            //g_fCanLightUp = TRUE;
        }
        pthread_mutex_unlock(&g_Mutex);
    }

//--------------------------------------------------------------------
// //系统状态信息
    int SwGetSysStatus(int iDevID, DWORD32* pdwRetStat)
    {
        int iRet = 0;
        switch (iDevID)
        {
        case SW_DEVID_SYSSTAT_TEMPERATURE :
            iRet = GetCpuNearTemp((int*)pdwRetStat);
            break;

        default :
            iRet = EINVAL;
            break;
        }
        return iRet;
    }

// 获取CPU附近的温度
    static int GetCpuNearTemp(int* piTemp)
    {
        TEMP_STRUCT cTemp;
        int fd = 0;
        int ret = EINVAL;

        if ( NULL == piTemp )
        {
            return ret;
        }

        fd = SwDevOpen(O_RDWR);

        if ( ioctl(fd, SWDEV_IOCTL_LM75_GET_TEMP, &cTemp) >= 0 )
        {
            *piTemp = cTemp.Temp;
            if ( 1 == cTemp.TempPolar )
            {
                *piTemp = 0 - *piTemp;
            }

            ret = 0;
        }

        SwDevClose(fd);

        return ret;
    }

//////////////////////////////////////////////////////////////////////
// 加密芯片操作
//////////////////////////////////////////////////////////////////////

// 加密芯片控制
    int CryptDevCtrl(int iDevID, int iOp)
    {
        int iRet = 0;
        switch (iDevID)
        {
        case SW_DEVID_CRYPT_DEF:
            switch (iOp)
            {
            case SW_OP_CRYPT_VERIFY :
                iRet = verify_crypt();
                break;

            default :
                iRet = EINVAL;
                break;
            }
            break;

        default :
            iRet = EINVAL;
            break;
        }
        return iRet;
    }

//写加密芯片
    int SwCryptDevWrite(int iMode, BYTE8* pbBuf, DWORD32 dwLen)
    {
        return write_crypt((unsigned int*)pbBuf);
    }

//读加密芯片
    int SwCryptDevRead(int iMode, BYTE8* pbBuf, DWORD32* pdwLen)
    {
        return ENOSYS;
    }

//////////////////////////////////////////////////////////////////////
// 物理网卡操作
//////////////////////////////////////////////////////////////////////
    int SwNetGetPhyStatus(int fd, PHY_STATUS_STRUCT* pPhyStat)
    {
#ifdef _CAMERA_PIXEL_500W_
        return ENOSYS;
#else
        return ioctl(fd, SWDEV_IOCTL_GET_PHY_STATUS, pPhyStat);
#endif
    }

}   // end of extern "C"
