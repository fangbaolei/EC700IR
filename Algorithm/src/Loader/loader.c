#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/statfs.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <netinet/in.h>
#include "hvtarget_ARM.h"
#include "HvDebugStateInfo.h"
#include "image.h"
//#define _DEBUG_

#ifdef _DEBUG_
#define PRINTF printf
#define DUMP dump
#else
#define PRINTF(...)
#define DUMP(...)
#endif

#define SWDEV_NAME "/dev/swdev"

u8 arm_filename[] = "/tmp/file.XXXXXX";

int g_fNotRun = 0;

#ifdef _DEBUG_
static void dump(unsigned char *buf, int len)
{
    int i, j, n;
    int line = 16;
    char c;

    n = len / line;
    if (len % line)
        n++;

    for (i=0; i<n; i++)
    {
        printf("0x%08x: ", buf+i*line);

        for (j=0; j<line; j++)
        {
            if ((i*line+j) < len)
                printf("%02x ", buf[i*line+j]);
            else
                printf("   ");
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
                printf("   ");
        }

        printf("\n");
    }
}
#else
#define dump
#endif

/* fastcrc32.cpp */
extern unsigned int crc_table[256];

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
u32 crc32(u32 crc, u8 *buf, u32 len)
{
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
        DO8(buf);
        len -= 8;
    }
    if (len)
    {
        do
        {
            DO1(buf);
        }
        while (--len);
    }
    return crc ^ 0xffffffffL;
}

#define MX (((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[p&3^e]^z))

//解密
//返回原始数据长度
//解密失败时长度信息无效,最大为 (输入长度/4*4 - 4)
int xxtea_decrypt(
    u8* pbIn ,       //输入
    u32 nLenght ,   //输入长度
    u32* pKey       //128位密钥
)
{
    u32* v;
    u32* k;
    int n;
    u32 z,y,delta,sum,e;
    int p,q;
    u32 nBlockNum;
    int nOriLenght;

    if ( !pbIn || !pKey || nLenght < 8) return -1;
    v=(u32*)pbIn;
    k=pKey;
    nBlockNum = nLenght>>2;

    n = nBlockNum - 1;

    z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
    q = 6 + 52 / (n + 1);
    sum = (u32)(q * delta);
    while (sum != 0)
    {
        e = sum >> 2 & 3;
        for (p = n; p > 0; p--)
        {
            z = v[p - 1];
            y = (v[p] -= MX);
        }
        z = v[n];
        y = (v[0] -= MX);
        sum -= delta;
    }

    nOriLenght = *(int*)(pbIn + ((nBlockNum - 1)<<2)); //原始长度

    return (nOriLenght <= ((nBlockNum - 1)<<2) )?nOriLenght:((nBlockNum - 1)<<2);
}

//static unsigned long key_HvEncryptFile[4] = {0x34D2AE9C, 0x60D2B613, 0x1CB85817, 0x91CA700D};

int decrypt_userdata(u8 *data, u32 len)
{
    u32 rgdwKey[4] = {0};
    u32 rgdwNC[4] = {0};
    if ( 0 == get_nc_key(&rgdwNC, &rgdwKey) )
    {
        xxtea_decrypt((u8*)rgdwKey, 16, (u32*)rgdwNC);
        xxtea_decrypt(data, len, (u32*)rgdwKey);  // 正式版
        //xxtea_decrypt(data, len, key_HvEncryptFile);  // 内部调试版
        return 0;
    }
    return -1;
}

int copy_image(image_header_t *hdr, u8 *src, u8 *dst)
{
    u32 data_len  = ntohl(hdr->ih_size);
    u32 data_csum = ntohl(hdr->ih_dcrc);
    u32 csum;

#ifdef CONFIG_COMMANDS
    if (((u32)src) & 0x80000000)
    {
        // from ddr
        memcpy(dst, src, data_len);
    }
    else
    {
        // from flash
        PRINTF("eeprom_read(%x, %p, %p, %x)\n", 0, src, dst, data_len);
        if (eeprom_read(0, src, dst, data_len))
        {
            PRINTF("ERROR:read flash failed\n");
            return -1;
        }
    }

    // make sure checksum intact
    csum = crc32(0, dst, data_len);
#else
#define WR_FILE_BUF_LEN (1024)
    int len;
    u8 *buf, *tmp;

    buf = malloc(data_len);
    if (buf == NULL)
    {
        PRINTF("ERROR: malloc memory(size: %08x)!\n", data_len);
        return -1;
    }

    // read to memory
    tmp = buf;
    len = data_len;
    while (len > 0)
    {
        read((int)src, tmp, len > WR_FILE_BUF_LEN ? WR_FILE_BUF_LEN : len);

        len -= WR_FILE_BUF_LEN;
        tmp += WR_FILE_BUF_LEN;
    }

    // crc
    csum = 0;
    csum = crc32(csum, buf, data_len);

    // decrypt
    decrypt_userdata(buf, data_len);

    // write to file
    tmp = buf;
    len = data_len;
    while (len > 0)
    {
        write((int)dst, tmp, len > WR_FILE_BUF_LEN ? WR_FILE_BUF_LEN : len);
        len -= WR_FILE_BUF_LEN;
        tmp += WR_FILE_BUF_LEN;
    }
#endif // CONFIG_COMMANDS

    PRINTF("crc32: %08x => dcrc: %08x\n", data_csum, csum);
    if (data_csum != csum)
    {
        PRINTF ("ERROR: Image file data is damaged!\n");
        return -1;
    }

    return 1;
}

int uimage(u8 *src, u8 *dst, u8 *name, u8 cpy_hdr)
{
    image_header_t img_hdr;
    image_header_t *hdr = &img_hdr;
    u32 csum, hdr_csum, data_csum, data_len;
    u32 i, ptrs, ret;
    u8 *data;

#ifdef CONFIG_COMMANDS
    if (((u32)src) & 0x80000000)
    {
        // from ddr
        hdr = (image_header_t *)src;
    }
    else
    {
        // from flash
        PRINTF("eeprom_read(%x, %p, %p, %x)\n", 0, src, (u8 *)hdr, sizeof(image_header_t));
        if (eeprom_read(0, src, (u8 *)hdr, sizeof(image_header_t)))
        {
            PRINTF("ERROR:read flash failed\n");
            return -1;
        }
    }
#else
    read((int)src, hdr, sizeof(image_header_t));
#endif // CONFIG_COMMANDS

    if (ntohl(hdr->ih_magic) != IH_MAGIC)
    {
        PRINTF("\n-------image_header-----------\n");
        PRINTF("ih_magic: %08x\n",	ntohl(hdr->ih_magic));
        PRINTF("ih_hcrc: %08x\n",	ntohl(hdr->ih_hcrc));
        PRINTF("ih_time: %08x\n",	ntohl(hdr->ih_time));
        PRINTF("ih_size: %08x\n",	ntohl(hdr->ih_size));
        PRINTF("ih_load: %08x\n",	ntohl(hdr->ih_load));
        PRINTF("ih_ep: %08x\n",	    ntohl(hdr->ih_ep));
        PRINTF("ih_dcrc: %08x\n",	ntohl(hdr->ih_dcrc));
        PRINTF("ih_os: %02x\n",	    hdr->ih_os);
        PRINTF("ih_arch: %02x\n",	hdr->ih_arch);
        PRINTF("ih_type: %02x\n",	hdr->ih_type);
        PRINTF("ih_comp: %02x\n",	hdr->ih_comp);
        PRINTF("ih_name: %s\n",	    hdr->ih_name);

        PRINTF("ERROR: Invalid image file!\n");
        return -1;
    }

    hdr_csum  = ntohl(hdr->ih_hcrc);
    data_len  = ntohl(hdr->ih_size);
    data_csum = ntohl(hdr->ih_dcrc);

    hdr->ih_hcrc = 0;	// clear for re-calculation
    csum = crc32(0, (u8 *)hdr, sizeof(image_header_t));
    PRINTF("crc32: %08x => hcrc: %08x\n", csum, hdr_csum);
    if (hdr_csum != csum)
    {
        PRINTF ("ERROR: Image file header is damaged\n");
        return -1;
    }
    hdr->ih_hcrc = htonl(hdr_csum);

    switch (hdr->ih_type)
    {
    case IH_TYPE_STANDALONE:
    case IH_TYPE_KERNEL:
    case IH_TYPE_RAMDISK:
    case IH_TYPE_MULTI:
    case IH_TYPE_FIRMWARE:
    case IH_TYPE_FILESYSTEM:
        break;
    default:
        PRINTF("ERROR: Invalid Image TYPE\n");
        return -1;
    }

    switch (hdr->ih_os)
    {
    case IH_OS_LINUX:
        break;
    default:
        PRINTF("ERROR: Invalid Image OS\n");
        return -1;
    }

#ifdef CONFIG_COMMANDS
    if (strcmp(hdr->ih_name, name) == 0)
    {
        u32 *len_ptr;

        data = src + sizeof(image_header_t);

        if (cpy_hdr)
        {
            memcpy(dst, hdr, sizeof(image_header_t));

            dst += sizeof(image_header_t);
        }

        if (IH_TYPE_MULTI == hdr->ih_type)
        {
            ret = copy_image(hdr, data, dst);
            if (ret == -1)
                return -1;

            if (cpy_hdr)
            {
                return ret;
            }

            len_ptr = (u32 *)(data);
            for (i=0; len_ptr[i]; ++i)   // null pointer terminates list
            {
                PRINTF("len[%d]: %x\n", i, ntohl(len_ptr[i]));
            }

            ptrs = i + 1; // null pointer terminates list
            data = data + ptrs * sizeof(u32);
            data_len = data_len - (ptrs * sizeof(u32));

            memcpy(dst, data, data_len);

            return 1;
        }
        else
        {
            return copy_image(hdr, data, dst);
        }
    }
#else
    if (strcmp(hdr->ih_name, name) == 0)
    {
        if (cpy_hdr)
        {
            write((int)dst, hdr, sizeof(image_header_t));
        }

        if (IH_TYPE_MULTI == hdr->ih_type)
        {
            u32 len_t[32];

            i = 0;
            read((int)src, &len_t[i], sizeof(u32));
            PRINTF("len[%d]: %x\n", i, ntohl(len_t[i]));
            for (; len_t[i]; ++i)
            {
                read((int)src, &len_t[i], sizeof(u32));
                PRINTF("len[%d]: %x\n", i, ntohl(len_t[i]));
            }

            if (cpy_hdr)
            {
                write((int)dst, len_t, (i + 1) * sizeof(u32));
            }
        }

        return copy_image(hdr, src, dst);
    }
#endif // CONFIG_COMMANDS

    if (IH_TYPE_MULTI == hdr->ih_type)
    {
        int pos, size;
        u32 *len_ptr;
        u32 len_t[32];

        PRINTF("Loading multiple files...\n");

#ifdef CONFIG_COMMANDS
        if (((u32)src) & 0x80000000)
        {
            // from ddr
            len_ptr = (u32 *)(src + sizeof(image_header_t));
        }
        else
        {
            // from flash
            data = src + sizeof(image_header_t);
            PRINTF("eeprom_read(%x, %p, %p, %x)\n", 0, len_t, dst, 32 * sizeof(u32));
            if (eeprom_read(0, len_t, dst, 32 * sizeof(u32)))
            {
                PRINTF("ERROR:read flash failed\n");
                return -1;
            }
            len_ptr = (u32 *)(len_t);
        }
#else
        len_ptr = (u32 *)(len_t);
        read((int)src, &len_ptr[0], sizeof(u32));
#endif // CONFIG_COMMANDS

        // determine number of images first (to calculate image offsets)
        for (i=0; len_ptr[i]; ++i)  	// null pointer terminates list
        {
            PRINTF("len[%d]: %x\n", i, ntohl(len_ptr[i]));
#ifndef CONFIG_COMMANDS
            read((int)src, &len_ptr[i+1], sizeof(u32));
#endif // CONFIG_COMMANDS
        }

        ptrs = i + 1; // null pointer terminates list
        pos = sizeof(image_header_t) + ptrs * sizeof(u32);
        data = src + pos;

        for (i=0; len_ptr[i]; ++i)
        {
            size = ntohl(len_ptr[i]);

#ifdef CONFIG_COMMANDS
            ret = uimage(data, dst, name, cpy_hdr);
#else
            ret = uimage(src, dst, name, cpy_hdr);
#endif // CONFIG_COMMANDS
            if (-1 == ret)
            {
                PRINTF("ERROR: failed loading image file %d!\n",i);
                return -1;
            }
            if (1 == ret)
            {
                PRINTF("Done!\n",i);
                return 1;
            }
            //pad  to double word aligned
            size += 3;
            size &= ~3;
            data += size;
        }

        PRINTF("ERROR: failed! Cannot find the image!\n");
        return 0;
    }

#ifndef CONFIG_COMMANDS
    data_len += 3;
    data_len &= ~3;
    lseek((int)src, data_len, SEEK_CUR);
    PRINTF("++len[%d]: %x++\n", i, data_len);
#endif // CONFIG_COMMANDS

    return 0;
}

static unsigned long GetTickCount( void )
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned long dwTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return dwTime;
}

int load_fpga(char *filename)
{
    FILE *fd = NULL;
    int len, n, ret;
    unsigned char *data, *t;
    REG_STRUCT load_addr;
    unsigned long dwStart, dwEnd;

    dwStart = GetTickCount();

    fd = fopen(filename, "rb+");
    if (fd == NULL)
    {
        PRINTF("fopen(%s) error!\n", filename);
        return -1;
    }

    fseek(fd, 0, SEEK_END);
    len = ftell(fd);
    if (len == 0)
    {
        PRINTF("File read failed!\n");
        fclose(fd);
        return -1;
    }
    fseek(fd, 0, SEEK_SET);
    PRINTF("file size = %08x\n", len);

    t = data = malloc(len);
    if (data == NULL)
    {
        PRINTF("malloc(%08x) error!\n", len);
        fclose(fd);
        return -1;
    }

    while (len > 0)
    {
        n = fread(t, 1, 64*1024, fd);
        t += n;
        len -= n;
    }

    fclose(fd);

    dwEnd = GetTickCount();
    PRINTF("process time = %d[%d - %d]\n", dwEnd - dwStart, dwEnd, dwStart);

    fd = (FILE*)SwDevOpen(O_RDWR); // 打开
    if (fd < 0)
    {
        PRINTF("open swdev error!\n");
        return -1;
    }
    load_addr.addr = (u32)data;
    load_addr.data = (u32)data;
    dwStart = GetTickCount();
    ret = ioctl((int)fd, SWDEV_IOCTL_LOAD_FPGA_FROM_JTAG, &load_addr);
    dwEnd = GetTickCount();
    if (ret < 0)
    {
        PRINTF("===load fpga ioctl error!===\n");
    }
    PRINTF("process time = %d[%d - %d]\n", dwEnd - dwStart, dwEnd, dwStart);

    SwDevClose((int)fd);

    free(data);

    return ret;
}

int loader(char *imgname)
{
    int imgfd, tmpfd;
    u8 fpga_filename[] = "/tmp/file.XXXXXX";
    //u8 arm_filename[] = "/tmp/file.XXXXXX";
    u8 dsp_filename[] = "/tmp/file.XXXXXX";
    u8 buf[256];
    u32 len;
    u32 ret;
    // fpga
    imgfd = open(imgname, O_RDONLY); // 打开Flash镜像
    if (imgfd < 0)
    {
        PRINTF("open image file(%s) error!\n", imgname);
        goto error;
    }

    tmpfd = mkstemp(fpga_filename);
    if (tmpfd < 0)
    {
        PRINTF("open temp file error!\n");
        goto error;
    }
    PRINTF("1--------temp file: %s\n", fpga_filename);

    ret = uimage((u8 *)imgfd, (u8 *)tmpfd, "fpga", 0);
    if (ret == 1)
    {
        PRINTF("load fpga...\n");
        load_fpga(fpga_filename);
    }

    close(imgfd);
    close(tmpfd);
    remove(fpga_filename);

    // arm
    imgfd = open(imgname, O_RDONLY); // 打开Flash镜像
    if (imgfd < 0)
    {
        PRINTF("open image file(%s) error!\n", imgname);
        goto error;
    }

    tmpfd = mkstemp(arm_filename);
    if (tmpfd < 0)
    {
        PRINTF("open temp file error!\n");
        goto error;
    }
    PRINTF("2--------temp file: %s\n", arm_filename);

    ret = uimage((u8 *)imgfd, (u8 *)tmpfd, "tools", 0);
    if (ret != 1)
    {
        goto error;
    }

    close(imgfd);
    close(tmpfd);

#ifdef LOAD_ARM_DSP  // 由loader负责载入DSP
    // dsp
    imgfd = open(imgname, O_RDONLY); // 打开Flash镜像
    if (imgfd < 0)
    {
        PRINTF("open image file(%s) error!\n", imgname);
        goto error;
    }

    tmpfd = mkstemp(dsp_filename);
    if (tmpfd < 0)
    {
        PRINTF("open temp file error!\n");
        goto error;
    }
    PRINTF("3--------temp file: %s\n", dsp_filename);

    ret = uimage((u8 *)imgfd, (u8 *)tmpfd, "firmware", 0);
    if (ret != 1)
    {
        goto error;
    }

    close(imgfd);
    close(tmpfd);
#endif

    chmod(arm_filename, S_IRWXU);

#ifdef LOAD_ARM_DSP
    return execl(arm_filename, arm_filename, dsp_filename, (char*)NULL);
#else
    if (g_fNotRun)
    {
        remove(arm_filename);
    }
    else
    {
        return 0;
    }
#endif

    return 0;

error:
    close(imgfd);
    close(tmpfd);
    exit(255);
    return 0;
}

#define IP_ADDR               0x40C // IP地址 (4byte)
#define GATEWAY_ADDR          0x410 // 网关地址 (4byte)
#define NETMASK_ADDR          0x414 // 子网掩码 (4byte)

// 注：主机字节序
inline BOOL MyGetIpString(DWORD32 dwIP, LPSTR lpszIP)
{
    if ( lpszIP == NULL ) return FALSE;

    BYTE8* pByte=(BYTE8*)&dwIP;
    if ( -1 == sprintf(lpszIP, "%d.%d.%d.%d", pByte[3],pByte[2],pByte[1],pByte[0]) )
    {
        return FALSE;
    }
    return TRUE;
}

int EEPROM_Read(
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
    do
    {
        ret = ioctl(fd, SWDEV_IOCTL_EEPROM_READ, &eeprom);
        if (ret < 0)
        {
            PRINTF("===read eeprom error!===\n");
            break;
        }
        memcpy(dst, eeprom.data, eeprom.len);

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
    }
    while (rlen > 0);

    SwDevClose(fd);

    return ret;
}

int GetUbootNetAddr(DWORD32* pdwIP, DWORD32* pdwNetmask, DWORD32* pdwGateway)
{
    if ( 0 == EEPROM_Read(IP_ADDR, (BYTE8*)pdwIP, 4)
            && 0 == EEPROM_Read(GATEWAY_ADDR, (BYTE8*)pdwGateway, 4)
            && 0 == EEPROM_Read(NETMASK_ADDR, (BYTE8*)pdwNetmask, 4) )
    {
        return 0;
    }
    return -1;
}

void SetIP()
{
    //set ip
    DWORD32 dwIP=0,dwMask=0,dwGateway=0;
    GetUbootNetAddr(&dwIP, &dwMask, &dwGateway);
    dwIP = ntohl(dwIP);
    dwMask = ntohl(dwMask);
    dwGateway = ntohl(dwGateway);

    char szIP[64] = {0};
    char szMask[64] = {0};
    char szGateWay[64] = {0};

    MyGetIpString(dwIP, szIP);
    MyGetIpString(dwMask, szMask);
    MyGetIpString(dwGateway, szGateWay);

    char szCmd[256] = {0};
    sprintf(szCmd,"ifconfig eth0 %s netmask %s", szIP, szMask);
    printf("set ip_addr: %s\n", szCmd);
    system(szCmd);
}

static void ShowStartInfo(int argc, char *argv[])
{
    int i = 0;
    printf("loader build datetime:[%s %s]\n", __DATE__, __TIME__);
    for (i=0; i<argc; i++)
    {
        printf("argv[%d] = [%s]\n", i, argv[i]);
    }
}
int ReadDataFromFile(const char* szFileName, unsigned char* pbData, unsigned long ulDataSize)
{
    int err = 0;
    if (!( err = access(szFileName, 0)))
    {
        unsigned long ulReadedSize = 0;
        int fp = open(szFileName, O_RDONLY);
        if ( fp )
        {
            ulReadedSize = pread(fp, pbData, ulDataSize, 0);
            close(fp);
            return ulReadedSize;
        }
        else
        {
            perror("fopen file error.\n");
        }
    }
    else
    {
        printf("Warning:Can't access file, code = %d.\n", err);
    }
    return -1;
}
int GetSystemTick(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    DWORD32 dwTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return dwTime;
}
// 获取复位次数(长度：4字节)
int GetResetCount(int* piResetCount)
{
    return EEPROM_Read(RESET_COUNTER_ADDR, (BYTE8*)piResetCount, 4);
}
int WriteResetReport(const int iType)
{
    /* 数据结构说明:修改自实时时间结构体，将第4个成员Uint16最高3位作为复位类型使用. */
    typedef struct
    {
        Uint16 wYear;    //年数.
        Uint16 wMonth;   //月数.
        Uint16 wDay;     //号数.
        Uint16 wResetType; //复位类型（最高3位）
        Uint16 wHour;    //小数数,24小时制.
        Uint16 wMinute;  //小时数.
        Uint16 wSecond;  //秒数.
        Uint16 wMSecond; //毫秒数.
    } RESET_RECORD_STRUCT;

    int iResetCount = 0;
    int iWriteIndex = 0;
    static time_t t = 0;
    static struct tm* tt = NULL;
    RESET_RECORD_STRUCT cRealTime;
    WdtHandshake();
    if ( GetResetCount(&iResetCount) < 0 )
    {
        printf("<ReadResetReport> GetResetCount is FAILED!\n");
        return -1;
    }
    else
    {
        printf("iResetCount=%d\n", iResetCount);
        //if ( iResetCount >= 0 )
        {
            t = time(NULL);
            tt = gmtime(&t);

            iWriteIndex = iResetCount % 64;
            if (iWriteIndex < 0)
            {
                iWriteIndex = 0;
            }

            cRealTime.wYear     = tt->tm_year + 1900;
            cRealTime.wMonth    = tt->tm_mon + 1;
            cRealTime.wDay      = tt->tm_mday;
            cRealTime.wHour     = tt->tm_hour;
            cRealTime.wMinute   = tt->tm_min;
            cRealTime.wSecond   = tt->tm_sec;

            cRealTime.wResetType = (iType&0x07)<<13;
            WdtHandshake();
            if ( EEPROM_Write(START_TIME_TABLE_ADDR+iWriteIndex*sizeof(RESET_RECORD_STRUCT), (Uint8*)&cRealTime, sizeof(RESET_RECORD_STRUCT)) < 0 )
            {
                printf("<ReadResetReport> EEPROM_Write is FAILED!\n");
                return -1;
            }
            else
            {
                printf("<ReadResetReport> EEPROM_Write success.\n");
            }
            WdtHandshake();
            iResetCount++;
            EEPROM_Write(RESET_COUNTER_ADDR, (BYTE8*)&iResetCount, 4);
        }
        return 0;
    }
}
int main(int argc, char *argv[])
{
    ShowStartInfo(argc, argv);
    if (argc == 1)
    {
        load_fpga("./fpga.xsvf");
        SetIP();
        system("/etc/reloadko.sh");
        return 0;
    }

    if (argc == 3 && strcmp(argv[2],"NotRun") == 0)
    {
        g_fNotRun = 1;
        SetIP();
        loader(argv[1]);
        return 0;
    }

    int fRestartSystem = 0;
    struct timeval tvStart, tvEnd;
    char szLine[255];

    //后台运行脚本
    system("dm.sh &");

    //从Flash中载入ARM端程序，并进行解密
    loader(argv[1]);
    system("/etc/reloadko.sh");

    struct mytv
    {
        int iTick;
        int fMaster;
    }tmp;

    memset(&tmp, 0, sizeof(tmp));

    int pid = 0;
    while (!fRestartSystem || !tmp.fMaster)
    {
        gettimeofday(&tvStart, NULL);
        unlink("/.running");
        //如果是上一个进程的PID,则发送kill命令
        if(pid)
        {
            kill(pid, SIGINT);
        }
        pid = fork();
        if (!pid)
        {
            //杀死格式化进程
            system("killall fdisk");
            usleep(1000000);
            system("killall mkfs.ext3");
            //启动应用层
            printf("start load application....\n");
            execl(arm_filename, arm_filename, "LOAD_DSP_FROM_FLASH", (char*)NULL);
            return 0;
        }
        else
        {
            EnableWatchDog();  //使能看门狗
            gettimeofday(&tvEnd, NULL);
            printf("Start waiting child process, time=%d ms\n", 1000*(tvEnd.tv_sec - tvStart.tv_sec) + (tvEnd.tv_usec - tvStart.tv_usec)/1000);
            //记录下第一次等待的时间
            gettimeofday(&tvStart, NULL);
            int stat;
            int iTimes = 0;
            while (!waitpid(-1, &stat, WNOHANG))
            {
                tmp.iTick = 0;
                ReadDataFromFile("/.running", (unsigned char *)&tmp, sizeof(tmp));
                iTimes++;
                if (((0 == tmp.iTick) && (iTimes < 200)) || (GetSystemTick() - tmp.iTick < 120000))
                {
                    WdtHandshake();
                }
                else
                {
                    HvDebugStateInfo("child process is dead");
                    printf("now=%d, iTime = %d, child process is dead, restart system...\n", GetSystemTick(), tmp.iTick);
                    fRestartSystem = 1;
                    break;
                }
                if (!((iTimes++)%40))
                {
                    printf("now=%d,last=%d,escape=%d\n", GetSystemTick(), tmp.iTick, GetSystemTick() - tmp.iTick);
                }
                //等待500毫秒
                usleep(500000);
            }
            gettimeofday(&tvEnd, NULL);
            //子进程仅在启动至少30秒之后且正常终止，此时才能[重载程序]，否则[复位设备]
            int iExitCode = WEXITSTATUS(stat);
            if (!fRestartSystem
                    && !WTERMSIG(stat)
                    && !(iExitCode & 0x80)
                    && (!(iExitCode & 0x01) || (tvEnd.tv_sec - tvStart.tv_sec > 30)))
            {
                WdtHandshake();
                fRestartSystem = 0;
                //等待5秒钟后再加载驱动
                int i;
                for (i = 0; i < 10; i++)
                {
                    usleep(500000);
                    WdtHandshake();
                }
                //加载驱动识别，复位设备
                WdtHandshake();
                if (0 != system("/etc/reloadko.sh"))
                {
                    HvDebugStateInfo("load driver error.restart system");
                    printf("load driver error.restart system.\n");
                    fRestartSystem = 1;
                }
            }
            //子进程异常终止
            else
            {
                fRestartSystem = 1;
            }

            WdtHandshake();
            time_t t = time(NULL);
            struct tm *tt = localtime(&t);
            sprintf(szLine, "[%04d-%02d-%02d %02d:%02d:%02d] run time=[%ds], exit code=[%d], kill_code=[%d], action=[%s].\n" ,
                    tt->tm_year + 1900,
                    tt->tm_mon + 1,
                    tt->tm_mday,
                    tt->tm_hour,
                    tt->tm_min,
                    tt->tm_sec,
                    tvEnd.tv_sec - tvStart.tv_sec,
                    WEXITSTATUS(stat),
                    WTERMSIG(stat),
                    fRestartSystem ? "restart system" : "restart application");

            //打印出详细退出信息到串口
            printf("\n\n");
            printf("%s", szLine);
            system("cat exit.txt");
            printf("\n\n");

            if (0 != WTERMSIG(stat))
            {
                //将Killed类型的退出信息添加到黑盒子
                WdtHandshake();
                sprintf(szLine, "Killed:time=[%ds],exit=[%d],kill=[%d]",
                    tvEnd.tv_sec - tvStart.tv_sec,
                    WEXITSTATUS(stat),
                    WTERMSIG(stat));
                HvDebugStateInfo(szLine);
            }

            if (!fRestartSystem)
            {
                if (WTERMSIG(stat))
                {
                    WriteResetReport(2);
                }
                else
                {
                    WriteResetReport(1);
                }
            }

            //将详细退出信息记录到日志文件
            WdtHandshake();
            /*先不写日志了，写日志会导致狗超时复位
            system("mkdir /log;mount /dev/sda3 /log");
            struct statfs fs;
            if (!statfs("/log", &fs) && 100*fs.f_bfree/fs.f_blocks < 10)
            {
                system("rm -f core* *.log*");
            }
            WdtHandshake();
            FILE* fp = fopen("/log/reset.log", "a+");
            if (fp)
            {
                fputs(szLine, fp);
                fclose(fp);
            }
            WdtHandshake();
            system("cat exit.txt >> /log/reset.log;mv log.txt /log/log.txt.`date +\"%Y%m%d%H%M%S\"`;umount /dev/sda3");
            */
        }
    } // end while (!fRestartSystem)

    return 0;
}
