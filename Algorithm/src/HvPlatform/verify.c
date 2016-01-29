#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/types.h>
#include "slw_dev.h"
#include "fastcrc32.h"
#include "hvtarget_ARM.h"
#include "swdrv.h"

//#define _DEBUG_

#ifdef _DEBUG_
#define PRINTF printf
#define DUMP   dump
#else
#define PRINTF(...)
#define DUMP(...)
#endif

#ifdef _DEBUG_
void dump(unsigned char *buf, int len)
{
    int i, j, n;
    int line = 16;
    char c;

    n = len / line;
    if (len % line)
    {
        n++;
    }

    for (i=0; i<n; i++)
    {
        printf("0x%08x: ", buf+i*line);

        for (j=0; j<line; j++)
        {
            if ((i*line+j) < len)
            {
                printf("%02x ", buf[i*line+j]);
            }
            else
            {
                printf("   ");
            }
        }

        printf("  ");
        for (j=0; j<line && (i*line+j)<len; j++)
        {
            if ((i*line+j) < len)
            {
                c = buf[i*line+j];
                printf("%c", c > ' ' && c < '~' ? c : '.');
            }
            else
            {
                printf("   ");
            }
        }

        printf("\n");
    }
}
#else
#define dump
#endif

/* fastcrc32.cpp */
extern unsigned int crc_table[256];

//nLen>=4
void crypt_1(void* pDat, int nLen)
{
    unsigned char* pTMP = (unsigned char *)pDat;
    unsigned char nTMP = 0;
    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i = 0; i < nLen; i++)
    {
        nTMP ^= *pTMP;
        pTMP ++;
    }

    unsigned int nCRC = CalcFastCrc32(nTMP, (unsigned char*)pDat, nLen);
    memcpy(pDat, &nCRC, 4);
}

void crypt_2(void* pDat, int nLen)
{
    unsigned char* pTMP = (unsigned char *)pDat;
    unsigned char nTMP = 0;
    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i = 0; i < nLen; i++)
    {
        nTMP ^= *pTMP;
        pTMP ++;
    }

    memcpy(pDat, &crc_table[nTMP], 4);
    unsigned int nCRC = CalcFastCrc32(0, (unsigned char*)pDat, nLen);
    memcpy(pDat, &nCRC, 4);
}

void crypt_3(void* pDat, int nLen)
{
    unsigned char* pTMP = (unsigned char *)pDat;
    unsigned char nTMP = 0;
    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i = 0; i < nLen; i++)
    {
        nTMP ^= *pTMP;
        pTMP ++;
    }

    memcpy(pDat, &crc_table[nTMP], 4);
    unsigned int nCRC = CalcFastCrc32(crc_table[nTMP], (unsigned char*)pDat, nLen);
    memcpy(pDat, &nCRC, 4);
}

void crypt_4(void* pDat, int nLen)
{
    unsigned int nCrc0 = CalcFastCrc32(0, (unsigned char*)pDat, nLen);
    unsigned int nCrc1 = CalcFastCrc32(0, (unsigned char*)pDat + 1, nLen-1);
    unsigned int nCRC = nCrc0 ^ nCrc1;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    memcpy(pDat, &nCRC, 4);
}

void crypt_5(void* pDat, int nLen)
{
    unsigned char* pTMP = (unsigned char *)pDat;
    unsigned char nTMP = 0;

    unsigned char* pMask = (unsigned char*)&crc_table[nTMP];
    unsigned char* pDatBuf = (unsigned char*)pDat;

    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i = 0; i < nLen; i++)
    {
        nTMP ^= *pTMP;
        pTMP ++;
    }

    pDatBuf[0] ^= pMask[0];
    pDatBuf[1] ^= pMask[2];
    pDatBuf[2] ^= pMask[1];
    pDatBuf[3] ^= pMask[3];
}

void crypt_6(void* pDat, int nLen)
{
    unsigned char* pDatBuf = (unsigned char*)pDat;
    unsigned int nSum = 0;
    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i = 0; i < nLen; i++)
    {
        nSum += pDatBuf[i];
    }

    nSum *= nSum;
    nSum ^= 0x31adc9ed;
    memcpy(pDat, &nSum, 4);
}

void crypt_7(void* pDat, int nLen)
{
    unsigned int hash = *(int*)pDat;
    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i=0; i<nLen; ++i)
    {
        hash *= 33;
        hash += crc_table[i];
    }

    memcpy(pDat, &hash, 4);
}

void crypt_8(void* pDat, int nLen)
{
    unsigned char* pDatBuf = (unsigned char*)pDat;
    unsigned int p = 16777619u;
    unsigned int hash = 2166136261u;
    int i;

    PRINTF("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

    for (i=0; i<nLen; i++)
    {
        hash = (hash ^ pDatBuf[i]) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;

    memcpy(pDat, &hash, 4);
}

typedef void (*crypt_fun)(void* pDat, int nLen);
crypt_fun all_crypt_funs[] = {crypt_1, crypt_2, crypt_3, crypt_4, crypt_5, crypt_6, crypt_7, crypt_8};
crypt_fun curr_crypt_fun   = NULL;

static unsigned int g_NC[4] = {0};
static void GetNC(u8* data, u32* len)
{
    memcpy(data, g_NC, 16);
    *len = 16;
}

static int g_iVerifyInit = 0;

// 第一次初始化
// 写入加密数据
int write_crypt(unsigned int* rgNC)
{
    int fd;
    int ret;
    AT88SC_STRUCT at88sc;

    srand((int)time(0)); // 初始化随机数种子
    curr_crypt_fun = all_crypt_funs[rand() % (sizeof(all_crypt_funs) / sizeof(crypt_fun))];

    memcpy(g_NC, rgNC, 16);

    // crypt
    GetNC(at88sc.data, &at88sc.len);  // get Nc
    DUMP(at88sc.data, at88sc.len);
    curr_crypt_fun(at88sc.data, at88sc.len);
    DUMP(at88sc.data, at88sc.len);

    fd = SwDevOpen(O_RDWR); // 打开

    // write crypt data to 0 userzone
    at88sc.addr = 0;
    at88sc.len  = 0x07;
    ret = ioctl(fd, SWDEV_IOCTL_AT88SC_EEPROM_CRYTP_WRITE, &at88sc);
    if (ret < 0)
    {
        PRINTF("===write at88sc error!===\n");
    }
    else
    {
        PRINTF("write at88sc, addr: %02x, len: %d\n", at88sc.addr, at88sc.len);
        DUMP(&at88sc, sizeof(AT88SC_STRUCT));
    }

    SwDevClose(fd);

    g_iVerifyInit = 1;
    return ret;
}

// 随机进行循环
// 验证加密数据
int verify_crypt(void)
{
    int fd;
    int ret;
    AT88SC_STRUCT nc;
    AT88SC_STRUCT at88sc;

    if ( 0 == g_iVerifyInit )
    {
        return 1;
    }

	fd = SwDevOpen(O_RDWR); // 打开

    // get data
    bzero(&at88sc, sizeof(AT88SC_STRUCT));
    at88sc.addr = 0x00;
    at88sc.len  = 0x07;
    ret = ioctl(fd, SWDEV_IOCTL_AT88SC_EEPROM_CRYTP_READ, &at88sc);
    if (ret < 0)
    {
        PRINTF("===read at88sc error!===\n");
    }
    else
    {
        PRINTF("read at88sc, addr: %02x, len: %d\n", at88sc.addr, at88sc.len);
        DUMP(&at88sc, sizeof(AT88SC_STRUCT));
    }

    SwDevClose(fd);

    // crypt
    GetNC(nc.data, &nc.len);  // get Nc
    DUMP(nc.data, nc.len);
    curr_crypt_fun(nc.data, nc.len);
    DUMP(nc.data, nc.len);

    return (memcmp(nc.data, at88sc.data, 0x07) != 0) ? (-1) : (0);
}

// 获取nc和key
int get_nc_key(unsigned int (*rgdwNC)[4], unsigned int (*prgdwKey)[4])
{
    int fd = 0;
    int ret = 0;
    AT88SC_STRUCT nc;
    AT88SC_STRUCT key;

    fd = SwDevOpen(O_RDWR); // 打开

    bzero(&nc, sizeof(AT88SC_STRUCT));
    nc.data[0] = 0x19;
    nc.data[1] = 0x07;
    nc.len = 0x07;
    ret = ioctl(fd, SWDEV_IOCTL_AT88SC_STD_READ, &nc);
    if (ret < 0)
    {
        PRINTF("SWDEV_IOCTL_AT88SC_STD_READ is error!\n");
        SwDevClose(fd);
        return -1;
    }

    ret = ioctl(fd, SWDEV_IOCTL_AT88SC_EEPROM_INIT, NULL);
    if (ret < 0)
    {
        PRINTF("SWDEV_IOCTL_AT88SC_EEPROM_INIT is error!\n");
        SwDevClose(fd);
        return -1;
    }

    bzero(&key, sizeof(AT88SC_STRUCT));
    key.addr = 0x40;
    key.len  = 0x10;
    ret = ioctl(fd, SWDEV_IOCTL_AT88SC_EEPROM_CRYTP_READ, &key);
    if (ret < 0)
    {
        PRINTF("SWDEV_IOCTL_AT88SC_EEPROM_CRYTP_READ is error!\n");
        SwDevClose(fd);
        return -1;
    }

    SwDevClose(fd);

    memcpy(rgdwNC, nc.data, 16);
    memcpy(prgdwKey, key.data, 16);

    return 0;
}
