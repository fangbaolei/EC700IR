/*
* (C) Copyright 2000-2004
* DENX Software Engineering
* Wolfgang Denk, wd@denx.de
* All rights reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
* MA 02111-1307 USA
*/
#include "HvUpdatePackCreater.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef __WIN32__
#include <linux/types.h>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define WR_FILE_BUF_LEN 1024

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define MX (((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[p&3^e]^z))

////解密
////返回原始数据长度
////解密失败时长度信息无效,最大为 (输入长度/4*4 - 4)
//int xxtea_decrypt(
//                  u8* pbIn ,       //输入
//                  u32 nLenght ,   //输入长度
//                  u32* pKey       //128位密钥
//                  )
//{
//    u32* v;
//    u32* k;
//    int n;
//    u32 z,y,delta,sum,e;
//    int p,q;
//    u32 nBlockNum;
//    int nOriLenght;
//
//    if ( !pbIn || !pKey || nLenght < 8) return -1;
//    v=(u32*)pbIn;
//    k=pKey;
//    nBlockNum = nLenght>>2;
//
//    n = nBlockNum - 1;
//
//    z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
//    q = 6 + 52 / (n + 1);
//    sum = (u32)(q * delta);
//    while (sum != 0)
//    {
//        e = sum >> 2 & 3;
//        for (p = n; p > 0; p--)
//        {
//            z = v[p - 1];
//            y = (v[p] -= MX);
//        }
//        z = v[n];
//        y = (v[0] -= MX);
//        sum -= delta;
//    }
//
//    nOriLenght = *(int*)(pbIn + ((nBlockNum - 1)<<2)); //原始长度
//
//    return (nOriLenght <= ((nBlockNum - 1)<<2) )?nOriLenght:((nBlockNum - 1)<<2);
//}

int xxtea_encrypt(
                  u8* pbIn,
                  u32 nLenght,
                  u8* pbOut,
                  u32* pKey
                  )
{
    u32* v;
    u32* k;
    int n;
    u32 z,y,delta,sum,e;
    int p,q;
    u32 nBlockNum;

    if ( !pbIn || !pbOut || !pKey || nLenght < 8 ) return -1;
    v=(u32*)pbOut;
    k=pKey;

    nBlockNum=nLenght>>2;

    memcpy(pbOut,pbIn,nLenght);

    n = nBlockNum - 1;

    z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
    q = 6 + 52 / (n + 1);
    while (q-- > 0)
    {
        sum += delta;
        e = sum >> 2 & 3;
        for (p = 0; p < n; p++)
        {
            y = v[p + 1];
            z = (v[p] += MX);
        }
        y = v[0];
        z = (v[n] += MX);
    }

    return nLenght;
}

//int
//main_encrypt (int argc, char **argv)
//{
//    int ifd, ofd;
//    int len, file_len, mem_len;
//    u8 *rbuf, *wbuf, *tmp;
//    u8 key[16] = {0x00};
//    u32 KEY[4] = {0x12345678};
//    struct stat sbuf;
//
//    ifd = open(argv[1], O_RDONLY); // 打开
//    if (ifd < 0) {
//        printf("open input file(%s) error!\n", argv[1]);
//        return -1;
//    }
//
//    ofd = open(argv[2], O_RDWR | O_CREAT, 0666); // 打开
//    if (ofd < 0) {
//        printf("open output file(%s) error!\n", argv[2]);
//        return -1;
//    }
//
//    // get file size
//    if (fstat(ifd, &sbuf) < 0) {
//        printf ("Can't stat %s\n", argv[1]);
//        return -1;
//    }
//    file_len = sbuf.st_size;
//    mem_len = (file_len+3)/4*4;
//
//    rbuf = malloc(mem_len);
//    if (rbuf == NULL) {
//        printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
//        return -1;
//    }
//    memset(rbuf, 0, mem_len);
//
//    wbuf = malloc(mem_len);
//    if (wbuf == NULL) {
//        printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
//        return -1;
//    }
//    memset(wbuf, 0, mem_len);
//
//    // read to memory
//    tmp = rbuf;
//    do {
//        len = read(ifd, tmp, WR_FILE_BUF_LEN);
//        tmp += len;
//    } while (len > 0);
//
//    // encrypt
//    xxtea_encrypt(rbuf, mem_len, wbuf, KEY);
//
//#if 1
//    {
//        u8 *tbuf;
//
//        tbuf = malloc(mem_len);
//        if (tbuf == NULL) {
//            printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
//            return -1;
//        }
//        memset(tbuf, 0, mem_len);
//
//        memcpy(tbuf, wbuf, mem_len);
//        xxtea_decrypt(tbuf, mem_len, KEY);
//        len = memcmp(tbuf, rbuf, mem_len);
//        printf("memcmp = %d\n", len);
//    }
//#endif
//
//    // write to file
//    tmp = wbuf;
//    len = mem_len;
//    while (len > 0) {
//        write(ofd, tmp, len > WR_FILE_BUF_LEN ? WR_FILE_BUF_LEN : len);
//        len -= WR_FILE_BUF_LEN;
//        tmp += WR_FILE_BUF_LEN;
//    }
//
//    close(ifd);
//    close(ofd);
//
//    return 0;
//}

//------------------------------------------------------------------

unsigned long key_HvEncryptFile[4] = {0x34D2AE9C, 0x60D2B613, 0x1CB85817, 0x91CA700D};
int HvEncryptFile(char* szInFile, char* szOutFile)
{
    int ifd, ofd;
    int len, file_len, mem_len;
    u8 *rbuf, *wbuf, *tmp;
    struct stat sbuf;

    u32 KEY[4] = {0};
    memcpy(KEY, key_HvEncryptFile, sizeof(key_HvEncryptFile));  // key

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

    // get file size
    if (fstat(ifd, &sbuf) < 0) {
        printf ("Can't stat %s\n", szInFile);
        return -1;
    }
    file_len = sbuf.st_size;
    mem_len = (file_len+3)/4*4;

    rbuf = malloc(mem_len);
    if (rbuf == NULL) {
        printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
        return -1;
    }
    memset(rbuf, 0, mem_len);

    wbuf = malloc(mem_len);
    if (wbuf == NULL) {
        printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
        return -1;
    }
    memset(wbuf, 0, mem_len);

    // read to memory
    tmp = rbuf;
    do {
        len = read(ifd, tmp, WR_FILE_BUF_LEN);
        tmp += len;
    } while (len > 0);

    // encrypt
    xxtea_encrypt(rbuf, mem_len, wbuf, KEY);

//#if 1  // 用于验证加密后的数据是否等于加密前的数据
//    {
//        u8 *tbuf;
//
//        tbuf = malloc(mem_len);
//        if (tbuf == NULL) {
//            printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
//            return -1;
//        }
//        memset(tbuf, 0, mem_len);
//
//        memcpy(tbuf, wbuf, mem_len);
//        xxtea_decrypt(tbuf, mem_len, KEY);
//        len = memcmp(tbuf, rbuf, mem_len);
//        printf("memcmp = %d\n", len);
//    }
//#endif

    // write to file
    tmp = wbuf;
    len = mem_len;
    while (len > 0) {
        write(ofd, tmp, len > WR_FILE_BUF_LEN ? WR_FILE_BUF_LEN : len);
        len -= WR_FILE_BUF_LEN;
        tmp += WR_FILE_BUF_LEN;
    }

    close(ifd);
    close(ofd);

    return 0;
}

//---------------------------------------------------------------------------

#ifdef WIN32
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#endif

extern u32 crc32(u32 crc, const u8 *buf, u32 len);

#define M_NUM 0x63BF7A29
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3, m4) (m1*(M_NUM&m2)*m3*m4)
#define ENCODE(m1, m2, m3, m4) (m1 ^ MAKE_XOR_CODE(m2,m3,m4,M_NUM))

/*static */int crypt_key_DSP(u8 *addr, u32 len)
{
	u32 k0 = 0x6B51445B;
	unsigned long pos = 0;
    u32 k1 = 0;
    u8 k2 = 0;
    k1 = crc32(0, (u8*)&k0, 4);
    k2 = MAKE_KEY(k1, (k1>>8), (k1>>16));
    while ( len-- )
    {
        *addr = ENCODE(*addr, k2, (unsigned char)pos, (k1>>24));
		addr++;
        pos++;
    }
    return 1;
}

int HvEncryptFile_DSP(char* szInFile, char* szOutFile)
{
    int ifd, ofd;
    int len, file_len, mem_len;
    u8 *rbuf, *wbuf, *tmp;
    struct stat sbuf;

    u32 KEY[4] = {0};
    memcpy(KEY, key_HvEncryptFile, sizeof(key_HvEncryptFile));  // key

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

    // get file size
    if (fstat(ifd, &sbuf) < 0) {
        printf ("Can't stat %s\n", szInFile);
        return -1;
    }
    file_len = sbuf.st_size;
    mem_len = (file_len+3)/4*4;

    rbuf = malloc(mem_len);
    if (rbuf == NULL) {
        printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
        return -1;
    }
    memset(rbuf, 0, mem_len);

    wbuf = malloc(mem_len);
    if (wbuf == NULL) {
        printf("ERROR: malloc memory(size: %08x)!\n", mem_len);
        return -1;
    }
    memset(wbuf, 0, mem_len);

    // read to memory
    tmp = rbuf;
    do {
        len = read(ifd, tmp, WR_FILE_BUF_LEN);
        tmp += len;
    } while (len > 0);

    // encrypt
	crypt_key_DSP((u8*)&KEY, 16);
    xxtea_encrypt(rbuf, mem_len, wbuf, KEY);

    // write to file
    tmp = wbuf;
    len = mem_len;
    while (len > 0) {
        write(ofd, tmp, len > WR_FILE_BUF_LEN ? WR_FILE_BUF_LEN : len);
        len -= WR_FILE_BUF_LEN;
        tmp += WR_FILE_BUF_LEN;
    }

    close(ifd);
    close(ofd);

    return 0;
}
