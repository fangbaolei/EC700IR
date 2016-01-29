/*
 * DSPLink.c
 *
 *  Created on: 2009-9-4
 *      Author: Administrator
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "DSPLink.h"
#include "coff.h"
#include "config.h"
#include "hvtarget_ARM.h"
#include "sys/ioctl.h"
#include "HvExit.h"

/* interface.c */
extern DWORD32 GetSystemTick( void );

/* hvutils.h */
extern __inline int HV_Trace(int nRank, char* szfmt, ...);

/* LoadDspFromFlash.c */
extern int LoadDspFromFlash(char* szDspFileOut);

#ifdef _USE_SEED_DEBUG_
int g_fDebugStart = 0;
#endif

static int fd = -1;
static DSPLinkBuffer DSPLinkBufferObject;
static int DSPLINK_BUFFER_OBJECT_LEN = 1024*1024;

int CreateDSPBuffer(DSPLinkBuffer *obj, Uint32 len)
{
    if (obj == NULL)
    {
        printf("CreateDSPBuffer obj is NULL!\n");
        return -1;
    }
    obj->addr = CMEM_alloc(len, &CMEM_DEFAULTPARAMS);
    if (obj->addr == NULL)
    {
        printf("CMEM_alloc is error!\n");
        return -1;
    }
    obj->phys = CMEM_getPhys(obj->addr);
    obj->len = len;

    /*
    printf("--CreateDSPBuffer--\n");
    printf("addr: %08x\n", obj->addr);
    printf("phys: %08x\n", obj->phys);
    printf("len:  %0d\n", obj->len);
    */

    return 0;
}

void FreeDSPBuffer(DSPLinkBuffer *obj)
{
    if (obj)
    {
        if (obj->addr)
        {
            /*
            printf("--FreeDSPBuffer--\n");
            printf("addr: %08x\n", obj->addr);
            printf("phys: %08x\n", obj->phys);
            printf("len:  %0d\n", obj->len);
            */

            CMEM_free(obj->addr, &CMEM_DEFAULTPARAMS);
            obj->addr = NULL;
        }
    }
}

static Uint32 InitDSPLink(Uint32 len)
{
    if (DSPLinkBufferObject.addr)
    {
        return 0;
    }

    CreateDSPBuffer(&DSPLinkBufferObject, len);
    if (DSPLinkBufferObject.addr == NULL)
    {
        printf("Create DSPLinkBufferObject error\n");
        return 1;
    }

    return 0;
}

void ExitDSPLink(void)
{
    FreeDSPBuffer(&DSPLinkBufferObject);

    CMEM_exit();
}

static Uint32 DSPLinkWrite(long timeout_ms)
{
    fd_set writefd;
    struct timeval timeout;

    FD_ZERO(&writefd);
    FD_SET(fd,&writefd);

    timeout.tv_sec  = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    DWORD32 dwStartTime = 0;
    dwStartTime = GetSystemTick();

    select(fd + 1,NULL,&writefd,NULL, &timeout);

    int nLinkStatus = -100;
    if (0 == ioctl(fd,GET_STATUS,&nLinkStatus))
    {
        if(nLinkStatus != (int)VALID && nLinkStatus != -100)
        {
            HV_Trace(5,"DSPLink Status Err: %d\n",nLinkStatus);
        }
    }

    if (FD_ISSET(fd, &writefd) <= 0)
    {
        DWORD32 dwEndTime = GetSystemTick();
        HV_Trace(5, "DSP_LINK WRITE TIMEOUT. ts:%d, te:%d, dis:%d, timeout:%d\n",
                 dwStartTime, dwEndTime, dwEndTime-dwStartTime, timeout_ms);
        return 1;
    }
    else
    {
        if ( 0 == write(fd, (char*)DSPLinkBufferObject.phys, 4) )
        {
            return 0;
        }
        else
        {
            perror("DSPLinkWrite::write");
            return 2;
        }
    }
}

/**
 * @brief 等待DSP处理完成
 * @param timeout_ms 超时时间
 * @return ==0 成功, !=0 失败
 */
static Uint32 DSPLinkRead(long timeout_ms)
{
    int iRet = 0;
    int value = -1;
    fd_set readfd;
    struct timeval timeout;

    FD_ZERO(&readfd);
    FD_SET(fd,&readfd);

    timeout.tv_sec  = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    DWORD32 dwStartTime = 0;
    dwStartTime = GetSystemTick();

    select(fd + 1,&readfd,NULL,NULL, &timeout);

    int nLinkStatus = -100;
    if(0 == ioctl(fd,GET_STATUS,&nLinkStatus))
    {
        if(nLinkStatus != (int)VALID && nLinkStatus != -100)
        {
            HV_Trace(5,"DSPLink Status Err: %d\n",nLinkStatus);
        }
    }

    if (FD_ISSET(fd, &readfd) <= 0)
    {
        DWORD32 dwEndTime = GetSystemTick();
        HV_Trace(5, "DSP_LINK READ TIMEOUT. ts:%d, te:%d, dis:%d, timeout:%d\n",
                 dwStartTime, dwEndTime, dwEndTime-dwStartTime, timeout_ms);
        return 1;
    }
    else
    {
        iRet = read(fd, &value, 4);
        if ( iRet != value )
        {
            perror("DSPLinkRead::read");
            HV_Trace(5, "read:[ret = 0x%x, v = 0x%x]\n", iRet, value);
            return 2;
        }
        else
        {
            return 0;
        }
    }
}

static void WriteDspProg(int fd,Uint32 dst,Uint32 src,Uint32 sz)
{
    DSPLinkMem		mem;
    mem.addr = dst;
    mem.value= src;
    mem.size = sz;
    ioctl(fd, DATA_COPY, &mem);
}

static void ClearDspDDR2()
{
#ifdef _CAMERA_PIXEL_500W_
    int iZeroBufCount = 32;
    Uint32 dst = 0x8E000000; // [32M] 500w DSP's DDR2 start addr
#else
    int iZeroBufCount = 64;
    Uint32 dst = 0x8C000000; // [64M] 200w DSP's DDR2 start addr
#endif
    int iZeroBufLen = 1024*1024; // 1MB
    unsigned char* pbZeroBuf = (unsigned char*)malloc(iZeroBufLen);
    Uint32 src = (Uint32)pbZeroBuf;
    int i = 0;

    if ( pbZeroBuf != NULL )
    {
        memset(pbZeroBuf, 0, iZeroBufLen);

        HV_Trace(5, "ClearDspDDR2 start...\n");
        for ( i = 0; i < iZeroBufCount; ++i )
        {
            WriteDspProg(fd, dst, src, iZeroBufLen);
            dst += iZeroBufLen;
        }
        HV_Trace(5, "ClearDspDDR2 finish\n");

        free(pbZeroBuf);
        pbZeroBuf = NULL;
    }
    else
    {
        HV_Trace(5, "<ClearDspDDR2> malloc is failed!\n");
    }
}

#define DDR2_ATTR			( 0x01848200 ) // DDR2 memory attribute reg
#define CACHE_L2CFG         ( 0x01840000 )
#define CACHE_L2INV         ( 0x01845008 )
#define CACHE_L1PCFG        ( 0x01840020 )
#define CACHE_L1PINV        ( 0x01845028 )
#define CACHE_L1DCFG        ( 0x01840040 )
#define CACHE_L1DINV        ( 0x01845048 )

static void ClearDSPCache(int fd)
{
    DSPLinkMem		reg;
    int				i;

    for (i = 0; i < 0x40; i++)
    {
        reg.addr = DDR2_ATTR + i * 4;
        reg.value = 0;
        ioctl(fd,SET_REG,&reg);
    }

    //L1P invalid
    reg.addr = CACHE_L1PINV;
    reg.value= 1;
    ioctl(fd,SET_REG,&reg);

    //L1P on max size
    reg.addr = CACHE_L1PCFG;
    reg.value= 7;
    ioctl(fd,SET_REG,&reg);

    //L1D invalid
    reg.addr = CACHE_L1DINV;
    reg.value= 1;
    ioctl(fd,SET_REG,&reg);

    //L1D on max size
    reg.addr = CACHE_L1DCFG;
    reg.value= 7;
    ioctl(fd,SET_REG,&reg);

    //L2D invalid
    reg.addr = CACHE_L2INV;
    reg.value= 1;
    ioctl(fd,SET_REG,&reg);

    //L2D on min size
    reg.addr = CACHE_L2CFG;
    reg.value= 0;
    ioctl(fd,SET_REG,&reg);
}

void DSPLinkSetup(char *DspFile)
{
    DSPLinkMem		reg;

    close(fd);
    fd = open("/dev/DSPLink",O_RDWR,S_IRUSR | S_IWUSR);
    ClearDspDDR2();

    /*
     * step 1:clear the MDCTL1:LRST to 0
     */
    printf("\n============================\nstep0:clear the lrst.\n");
    reg.addr    = DSPLINK_MDCTL1;
    reg.value   = ioctl(fd,GET_REG,reg.addr);
    reg.value  &= ~(1<<8);
    ioctl(fd,SET_REG,&reg);

    /*disable cache*/
    printf("step1:clear L2 cache.\n");
    ClearDSPCache(fd);

    reg.addr    = DSPLINK_MDSTAT1;
    reg.value   = ioctl(fd,GET_REG,reg.addr);
    if ( (reg.value&0xffff) != 0x1e03 )
    {
        printf("step1:reg [MDSTAT1] is abnormal!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "step1:reg [MDSTAT1] is abnormal!");
    }

    /*
     * step 2:set DSPBOOTADDR
     */
    printf("step2:set entry point.\n");
    reg.addr    = DSPLINK_DSPBOOTADDR;
    reg.value   = DSP_ENTRY_POINT;
    ioctl(fd,SET_REG,&reg);

    /*
     *step3:load dsp program
     */
    printf("step3:load dsp program.\n");
    if ( 0 == strcmp("LOAD_DSP_FROM_FLASH", DspFile) )
    {
        char szDspFile[32] = {0};
        // 从Flash加载DSP文件到内存并解密。
        if ( 0 != LoadDspFromFlash(szDspFile) )
        {
            printf("LoadDspFromFlash is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "LoadDspFromFlash is error!");
        }
        if ( -1 == C6xDspLoad(fd,szDspFile,WriteDspProg) )
        {
            printf("C6xDspLoad is error![%s]\n", szDspFile);

            char szInfoTmp[64];
            sprintf(szInfoTmp, "C6xDspLoad is error![%s]", szDspFile);
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, szInfoTmp);
        }
        remove(szDspFile);  // 删除留在内存中的已解密的DSP程序。
    }
    else
    {
        if ( -1 == C6xDspLoad(fd,DspFile,WriteDspProg) )
        {
            printf("C6xDspLoad is error![%s]\n", DspFile);

            char szInfoTmp[64];
            sprintf(szInfoTmp, "C6xDspLoad is error![%s]", DspFile);
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, szInfoTmp);
        }
    }

    /*
     *step4:set dsplink arg
     */
    printf("step4:set dsplink env.\n");
    ioctl(fd,SET_TRI_INT,ARMINTDSP);
    ioctl(fd,SET_ARM_MSG_REG,DSPLINK_MEM_ARM);
    ioctl(fd,SET_DSP_MSG_REG,DSPLINK_MEM_DSP);

    /*
     * step 5:set MDCTL1:LRST to 1
     */
    printf("step5:start DSP.\n");
    reg.addr    = DSPLINK_MDCTL1;
    reg.value   = ioctl(fd,GET_REG,reg.addr);
    reg.value  |= (1<<8);
    ioctl(fd,SET_REG,&reg);

    //wait the dsp routine has boot
    printf("step6:wait dsp load ...\n");
#ifdef _USE_SEED_DEBUG_
    printf("<USE_SEED_DEBUG> waiting debug start ...\n");
    while ( 0 == g_fDebugStart )
    {
        usleep(1000);
    }
#endif
    WdtHandshake();
    while (DSPLinkRead(1000))
    {
        static int iRetryTimes = 3;
        if ( iRetryTimes > 0 )
        {
            WdtHandshake();
            --iRetryTimes;
            printf("step6:wait dsp load timeout ...\n");
        }
        else
        {
            printf("step6:wait dsp load is failed!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "step6:wait dsp load is failed!");
        }
    }

    // init cmem
    printf("step7:init dsp param ...\n");
    if (InitDSPLink(DSPLINK_BUFFER_OBJECT_LEN))
    {
        printf("InitDSPLink is error!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "InitDSPLink is error!");
    }

    printf("step8:dsp load success.\n============================\n");
}

/**
 * @brief 调用DSP处理
 * @param input 输入数据的结构体
 * @param output 输入出数据的结构体
 * @param timeout 超时时间
 * @return ==0 成功, !=0 失败
 */
int do_process(DSPLinkBuffer *input, DSPLinkBuffer *output, long timeout_ms)
{
    Uint32 dwRet = 0;

    DSPLinkBuffer* bufs = (DSPLinkBuffer*)DSPLinkBufferObject.addr;

    if (input == NULL || output == NULL || bufs == NULL)
    {
        return 1;
    }

    memcpy((void *)&bufs[0], (void *)input, sizeof(DSPLinkBuffer));
    memcpy((void *)&bufs[1], (void *)output, sizeof(DSPLinkBuffer));

    dwRet = DSPLinkWrite(timeout_ms);
    if ( 0 == dwRet )
    {
        dwRet = DSPLinkRead(timeout_ms);
        if ( 0 == dwRet )
        {
            return 0;
        }
        else
        {
            HV_Trace(5, "DSPLinkRead return %d\n", dwRet);
            return -1;
        }
    }
    else
    {
        HV_Trace(5, "DSPLinkWrite return %d\n", dwRet);
        return -1;
    }
}

int CheckHDVICP01()
{
    DSPLinkMem reg;

    reg.addr = DSPLINK_MDSTAT2;
    reg.value = ioctl(fd,GET_REG, reg.addr);
    if ( (reg.value&0xffff) != 0x1f03 )
    {
        return -1;
    }

    reg.addr = DSPLINK_MDSTAT3;
    reg.value = ioctl(fd,GET_REG, reg.addr);
    if ( (reg.value&0xffff) != 0x1f03 )
    {
        return -1;
    }

    return 0;
}
