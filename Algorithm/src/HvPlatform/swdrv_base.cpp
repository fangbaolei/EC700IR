#include "swdrv.h"
#include "swdrv_base.h"

#include "string.h"
#include <sys/ioctl.h>
#include <vector>
#include "hvthread.h"
#include "HvExit.h"

#ifdef _DEBUG_SWDRV_BASE
#define PRINTF printf
#else
#define PRINTF(...)
#endif

static HV_SEM_HANDLE g_hSemAD;

extern "C"
{
//--------------------------------------------------------------------
// swdev

int SwDevOpen(int iFlags)
{
    static int iFd = -1;
    int i = 0;

    if (iFd < 0)
    {
        for ( i = 0; i < 3; ++i )
        {
            iFd = open(SWDEV_NAME, iFlags);
            if (iFd < 0)
            {
                printf("open swdev error! retry again...\n");
                usleep(500*1000);
            }
            else
            {
                break;
            }
        }
        if (iFd < 0)
        {
            printf("open swdev error! exit\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "SwDevOpen is Error!!!");
        }
    }

    return iFd;
}

int SwDevClose(int fd)
{
    return 0;
}

//--------------------------------------------------------------------

int SwCpldWrite(DWORD32 dwAddr, BYTE8 bData)
{
    int fd;
    int ret;
    CPLD_STRUCT cpld;

    fd = SwDevOpen(O_RDWR); // 打开

    bzero(&cpld, sizeof(CPLD_STRUCT));
    cpld.data = bData;
    cpld.addr = dwAddr;

    ret = ioctl(fd, SWDEV_IOCTL_CPLD_WRITE, &cpld);
    if (ret < 0)
    {
        printf("===write cpld error!===\n");
        HvDebugStateInfo("Write CPLD error");
    }
    else
    {
        //printf("write cpld, addr: %x, data: %x\n", cpld.addr, cpld.data);
    }

    SwDevClose(fd);

    return ret;
}

int SwCpldRead(DWORD32 dwAddr, BYTE8* pbData)
{
    int fd;
    int ret;
    int i;
    CPLD_STRUCT cpld;

    fd = SwDevOpen(O_RDWR); // 打开

    for (i = 0; i < 5; i++)
    {
        bzero(&cpld, sizeof(CPLD_STRUCT));
        cpld.addr = dwAddr;
        ret = ioctl(fd, SWDEV_IOCTL_CPLD_READ, &cpld);
        if (ret < 0)
        {
            printf("===read cpld error!===\n");
            HvDebugStateInfo("Read CPLD error");
        }
        else if (cpld.data == 0xFF)
        {
            printf("===read cpld value = 0xFF, retry!\n");
            continue;
        }
        else
        {
            *pbData = cpld.data;
            //printf("read cpld, addr: %x, data: %x\n", cpld.addr, cpld.data);
            break;
        }
    }

    if (cpld.data == 0xFF)
    {
        printf("===read cpld value error: 0xFF\n");
        HvDebugStateInfo("Read CPLD value is 0xFF");
        ret = -1;
    }

    SwDevClose(fd);

    return ret;
}

int SwEepromRead(DWORD32 dwAddr, BYTE8* pbBuf, DWORD32 dwLen)
{
    int fd, ret;
    DWORD32 rlen;
    BYTE8 *dst = pbBuf;
    EEPROM_STRUCT eeprom;

    if (pbBuf == NULL)
    {
        return -1;
    }

    if (dwLen == 0)
    {
        return 0;
    }

    fd = SwDevOpen(O_RDWR); // 打开

    rlen = dwLen;

    bzero(&eeprom, sizeof(EEPROM_STRUCT));
    eeprom.len = EEPROM_AT24C1024_PAGE_SIZE - (dwAddr % EEPROM_AT24C1024_PAGE_SIZE);
    if (eeprom.len > rlen)
    {
        eeprom.len = rlen;
    }

    eeprom.addr = dwAddr;
    int iSleepLen = 0;
    do
    {
        ret = ioctl(fd, SWDEV_IOCTL_EEPROM_READ, &eeprom);
        if (ret < 0)
        {
            printf("===read eeprom error!===\n");
            break;
        }
        memcpy(dst, eeprom.data, eeprom.len);
        //每1k就休眠10ms
        iSleepLen += rlen;
        if(iSleepLen >= 1024)
        {
            iSleepLen = 0;
            HV_Sleep(10);//休眠10ms
        }
        rlen -= eeprom.len;
        dst += eeprom.len;

        bzero(&eeprom, sizeof(EEPROM_STRUCT));

        if (rlen < EEPROM_AT24C1024_PAGE_SIZE)
        {
            eeprom.len = rlen;
        }
        else
        {
            eeprom.len = EEPROM_AT24C1024_PAGE_SIZE;
        }

        eeprom.addr = dwAddr + (dwLen - rlen);
    }
    while (rlen > 0);

    SwDevClose(fd);

    return ret;
}

int SwEepromWrite(DWORD32 dwAddr, BYTE8* pbBuf, DWORD32 dwLen)
{
    int fd, ret;
    DWORD32 rlen;
    BYTE8 *dst = pbBuf;
    EEPROM_STRUCT eeprom;

    if (pbBuf == NULL)
    {
        return -1;
    }

    fd = SwDevOpen(O_RDWR); // 打开

    rlen = dwLen;

    bzero(&eeprom, sizeof(EEPROM_STRUCT));
    eeprom.len = EEPROM_AT24C1024_PAGE_SIZE - (dwAddr % EEPROM_AT24C1024_PAGE_SIZE);
    if (eeprom.len > rlen)
    {
        eeprom.len = rlen;
    }
    eeprom.addr = dwAddr;
    memcpy(eeprom.data, dst, eeprom.len);
    int iSleepLen = 0;
    do
    {
        ret = ioctl(fd, SWDEV_IOCTL_EEPROM_WRITE, &eeprom);
        if (ret < 0)
        {
            printf("===write eeprom error!===\n");
            break;
        }
        //每1k就休眠10ms
        iSleepLen += rlen;
        if(iSleepLen >= 1024)
        {
            iSleepLen = 0;
            HV_Sleep(10);//休眠10ms
        }
        rlen -= eeprom.len;
        dst += eeprom.len;

        bzero(&eeprom, sizeof(EEPROM_STRUCT));
        if (rlen < EEPROM_AT24C1024_PAGE_SIZE)
        {
            eeprom.len = rlen;
        }
        else
        {
            eeprom.len = EEPROM_AT24C1024_PAGE_SIZE;
        }
        eeprom.addr = dwAddr + (dwLen - rlen);
        memcpy(eeprom.data, dst, eeprom.len);
    }
    while (rlen > 0);

    SwDevClose(fd);

    return ret;
}

int SwADWrite(SW_DATA_AD adData)
{
    int fd;
    int ret;
    int iSwDev;

#ifdef _CAMERA_PIXEL_500W_
    iSwDev = SWDEV_IOCTL_AD9974_WRITE;
#else
    iSwDev = SWDEV_IOCTL_AD9949_WRITE;
#endif

    static bool s_fInitAD = false;
    if (!s_fInitAD)
    {

        if (HiVideo::CreateSemaphore(&g_hSemAD, 1, 1) != 0)
        {
            PRINTF("<SwADWrite>::CreateSemaphore failed!\n");
            return -1;
        }

        s_fInitAD = true;
    }

    HiVideo::SemPend(&g_hSemAD);

    fd = SwDevOpen(O_RDWR); // 打开

    ret = ioctl(fd, iSwDev, &adData);

    if (ret < 0)
    {
        printf("=== SwADWrite is error![%d] ===\n", ret);
        char buf[30];
        sprintf(buf,"SwADWrite is error![%d]",ret);
        HvDebugStateInfo(buf);
    }

    SwDevClose(fd);

    HiVideo::SemPost(&g_hSemAD);
    return ret;
}

//----------------------------------

}   // end of extern "C"


