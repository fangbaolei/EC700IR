#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../../Loader/image.h"
#include "crypt.h"
#include "fastcrc32.h"
#include "hvtarget_ARM.h"

//#define _DEBUG_

#ifdef _DEBUG_
#define PRINTF printf
#else
#define PRINTF(...)
#endif

#define M_NUM 0x63BF7A29
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3, m4) (m1*(M_NUM&m2)*m3*m4)
#define ENCODE(m1, m2, m3, m4) (m1 ^ MAKE_XOR_CODE(m2,m3,m4,M_NUM))

static int decrypt_key_DSP(u8 *addr, u32 len)
{
    u32 k0 = 0x6B51445B;
    unsigned long pos = 0;
    u32 k1 = 0;
    u8 k2 = 0;
    k1 = CalcFastCrc32(0, (u8*)&k0, 4);
    k2 = MAKE_KEY(k1, (k1>>8), (k1>>16));
    while ( len-- )
    {
        *addr = ENCODE(*addr, k2, (unsigned char)pos, (k1>>24));
        addr++;
        pos++;
    }
    return 1;
}

static unsigned int g_rgdwDspKey[4] = {0};

void SetDspKey(unsigned int* rgNC, unsigned int* rgKey)
{
    unsigned int rgdwNC[4] = {0};
    unsigned int rgdwKey[4] = {0};
    memcpy(rgdwNC, rgNC, 16);
    memcpy(rgdwKey, rgKey, 16);

    // OrgKey -> ArmKey
    xxtea_decrypt((BYTE8*)rgdwKey, 16, rgdwNC);

    // ArmKey -> DspKey
    memcpy(g_rgdwDspKey, rgdwKey, 16);
    decrypt_key_DSP((u8*)g_rgdwDspKey, 16);
}

//static unsigned long key_HvEncryptFile[4] = {0x34D2AE9C, 0x60D2B613, 0x1CB85817, 0x91CA700D};

// 解密userdata
static int decrypt_userdata(u8 *data, u32 len)
{
    //return xxtea_decrypt(data, len, key_HvEncryptFile); // 内测版
    return xxtea_decrypt(data, len, g_rgdwDspKey);
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

        WdtHandshake();
    }

    // crc
    csum = 0;
    csum = CalcFastCrc32(csum, buf, data_len);

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

        WdtHandshake();
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
    u32 i = 0, ptrs, ret;
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
    csum = CalcFastCrc32(0, (u8 *)hdr, sizeof(image_header_t));
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
                PRINTF("%d Done!\n",i);
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

// 描述：从Flash加载DSP文件到内存并解密，解密后放在内存中的DSP文件的名字为随机名。
// szDspFileOut: 放在内存中的DSP文件的名字
int LoadDspFromFlash(char* szDspFileOut)
{
    char imgname[] = "/dev/swdev";  // 正式版
    //char imgname[] = "master.img";  // 读取DDR2中的master.img，调试用。

    u32 ret;
    int imgfd, tmpfd = -1;
    u8 dsp_filename[] = "/tmp/file.XXXXXX";

    imgfd = open(imgname, O_RDONLY); // 打开
    if (imgfd < 0)
    {
        PRINTF("open image file(%s) error!\n", imgname);
        goto error;
    }

    tmpfd = mkstemp(dsp_filename); // 打开
    if (tmpfd < 0)
    {
        PRINTF("open temp file error!\n");
        goto error;
    }
    PRINTF("temp file: %s\n", dsp_filename);
    strcpy(szDspFileOut, dsp_filename);

    ret = uimage((u8 *)imgfd, (u8 *)tmpfd, "firmware", 0);
    if (ret != 1)
    {
        goto error;
    }

    close(tmpfd);
    close(imgfd);

    PRINTF("LoadDspFromFlash is OK [.out filename: %s]\n", szDspFileOut);
    return 0;

error:
    close(tmpfd);
    close(imgfd);
    return -1;
}
