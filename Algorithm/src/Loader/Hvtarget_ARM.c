#include "hvtarget_ARM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>

int SwDevOpen(int flags)
{
    static int iFd = -1;
    int i = 0;

    if (iFd < 0)
    {
        for ( i = 0; i < 3; ++i )
        {
            iFd = open(SWDEV_NAME, O_RDWR);
            if (iFd < 0)
            {
                printf("open swdev error!\n");
                usleep(500*1000);
            }
            else
            {
                break;
            }
        }
        if (iFd < 0)
        {
            printf("open swdev error! exit(0)\n");
            exit(0);
        }
    }

    return iFd;
}

int SwDevClose(int fd)
{
    return 0;
}

int EEPROM_Write(
    DWORD32 addr, // 地址
    BYTE8 *data, // 数据
    DWORD32 len // 长度
)
{
    int fd, ret;
    DWORD32 rlen;
    BYTE8 *dst = data;
    EEPROM_STRUCT eeprom;

    if (data == NULL)
    {
        return -1;
    }

    fd = SwDevOpen(O_RDWR); // 打开

    rlen = len;

    bzero(&eeprom, sizeof(EEPROM_STRUCT));
    eeprom.len = EEPROM_AT24C1024_PAGE_SIZE - (addr % EEPROM_AT24C1024_PAGE_SIZE);
    if (eeprom.len > rlen)
    {
        eeprom.len = rlen;
    }
    eeprom.addr = addr;
    memcpy(eeprom.data, dst, eeprom.len);
    do
    {
        WdtHandshake();
        ret = ioctl(fd, SWDEV_IOCTL_EEPROM_WRITE, &eeprom);
        if (ret < 0)
        {
            printf("===write eeprom error!===\n");
            break;
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
        eeprom.addr = addr + (len - rlen);
        memcpy(eeprom.data, dst, eeprom.len);
    }
    while (rlen > 0);

    SwDevClose(fd);

    return ret;
}

// 喂狗
int WdtHandshake(void)
{
    int fd;
    fd = SwDevOpen(O_RDWR); // 打开
    if (ioctl(fd, SWDEV_IOCTL_WATCHDOG, 0) < 0)
    {
        printf("===WdtHandshake error!===\n");
    }
    SwDevClose(fd);
    return 0;
}

/*
// 读cpld
int CPLD_Read(unsigned char addr, unsigned char *data)
{
    int fd;
    int ret;
    int i;
    CPLD_STRUCT cpld;

    fd = SwDevOpen(SWDEV_NAME, O_RDWR); // 打开

    for (i = 0; i < 5; i++)
    {
        bzero(&cpld, sizeof(CPLD_STRUCT));
        cpld.addr = addr;
        ret = ioctl(fd, SWDEV_IOCTL_CPLD_READ, &cpld);
        if (ret < 0)
        {
            printf("===read cpld error!===\n");
        }
        else if (cpld.data == 0xFF)
        {
            printf("===read cpld value = 0xFF, retry!\n");
            continue;
        }
        else
        {
            *data = cpld.data;
            //printf("read cpld, addr: %x, data: %x\n", cpld.addr, cpld.data);
            break;
        }
    }

    if (cpld.data == 0xFF)
    {
        printf("===read cpld value error: 0xFF\n");
    }

    SwDevClose(fd);

    return ret;
}
*/

// 写cpld
int CPLD_Write(unsigned char addr, unsigned char data)
{
    int fd;
    int ret;
    CPLD_STRUCT cpld;

    fd = SwDevOpen(O_RDWR); // 打开

    bzero(&cpld, sizeof(CPLD_STRUCT));
    cpld.data = data;
    cpld.addr = addr;

    ret = ioctl(fd, SWDEV_IOCTL_CPLD_WRITE, &cpld);
    if (ret < 0)
    {
        printf("===write cpld error!===\n");
    }
    else
    {
        //printf("write cpld, addr: %x, data: %x\n", cpld.addr, cpld.data);
    }

    SwDevClose(fd);

    return ret;
}

// 使能看门狗
int EnableWatchDog()
{
    WdtHandshake();
    static int iFirst = 1;
    if (iFirst)
    {
        iFirst = 0;
        CPLD_Write(0x13, 1);
    }
    return 0;
}

