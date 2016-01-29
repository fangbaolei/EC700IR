#include "HvUpdatePackCreater.h"

#define M_NUM 0x75DA63BF
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3) (m1*(M_NUM&m2)*m3)
#define ENCODE(m1, m2, m3) (m1 ^ MAKE_XOR_CODE(m2,m3,M_NUM))

// 加密
void CryptoByMAC(char* data, int len, unsigned char* keyMAC, unsigned long* pos)
{
    unsigned char k0 = MAKE_KEY(keyMAC[0], keyMAC[1], keyMAC[2]);
    while ( len-- )
    {
        *data++ = ENCODE(*data, k0, (unsigned char)*pos);
        (*pos)++;
    }
}

// 解密
void DecryptByMAC(char* data, int len, unsigned char* keyMAC, unsigned long* pos)
{
    CryptoByMAC(data, len, keyMAC, pos);
}

#include <stdio.h>
#include <stdlib.h>

#ifndef __WIN32__
#include <linux/types.h>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WR_FILE_BUF_LEN 1024

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

int CryptoByMAC_File(char* szInFile, char* szOutFile, unsigned char* keyMAC)
{
    int ifd, ofd;
    int len;
    u8 buf[WR_FILE_BUF_LEN];
    unsigned long position = 0;

    ifd = open(szInFile, O_RDONLY); // 打开
    if (ifd < 0) {
        printf("open input file(%s) error!\n", szInFile);
        return -1;
    }

    ofd = open(szOutFile, O_RDWR | O_CREAT, 0666); // 打开
    if (ofd < 0) {
        printf("open output file(%s) error!\n", szOutFile);
        return -1;
    }

    do {
        len = read(ifd, buf, WR_FILE_BUF_LEN);
        CryptoByMAC(buf, len, keyMAC, &position);
        write(ofd, buf, len);
    } while (len > 0);

    close(ifd);
    close(ofd);

    return 0;
}
