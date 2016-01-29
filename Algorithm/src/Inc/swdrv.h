#ifndef SWDRV_H_INCLUDED
#define SWDRV_H_INCLUDED
 //errno.h
#include "swdrv_cfg.h"

typedef void (*PCI_RECV_DATA_CALLBACK_FUNC)(int unused);

// 复位次数地址
#define RESET_COUNTER_ADDR    0x400 // 复位计数器 (4byte)

// 2~3K
#define START_TIME_TABLE_ADDR 0x800 // 启动时间表 (1Kbyte)
#define START_TIME_TABLE_LEN  0x400




#ifdef __cplusplus
extern "C"
{
#endif
    // CPLD c
    int SwCpldRead(DWORD32 dwAddr, BYTE8* pbData);
    int SwCpldWrite(DWORD32 dwAddr, BYTE8 bData);

    // AD芯片 c
    int SwADWrite(SW_DATA_AD adData);

    // 看门狗 c
    int SwWTDCtrl(int iMode);

    // 系统外设
    int SwSetLight(int iDevID,int iMode);       // LED灯
    int SwSetBuzz(int iMode);     	 // 蜂鸣器
    int SwGetSysStatus(int iDevID, DWORD32* pdwRetStat);  //系统状态

    // 加密芯片操作  at88
    int SwCryptDevCtrl(int iOp);      //加密芯片控制
    int SwCryptDevWrite(int iMode, BYTE8* pbBuf, DWORD32 dwLen);   //写加密芯片
    int SwCryptDevRead(int iMode, BYTE8* pbBuf, DWORD32* pdwLen);  //读加密芯片

    // EEPROM c
    int EEPROM_Lock();
    int EEPROM_UnLock();
    int SwEepromRead(DWORD32 dwAddr, BYTE8* pbBuf, DWORD32 dwLen);
    int SwEepromWrite(DWORD32 dwAddr, BYTE8* pbBuf, DWORD32 dwLen);

    //相机前端 c
    int SwCamTrigger(int iMode);
    int SwCamSetGain(int iGain);
    int SwCamSetRGBGain(int iGainR, int iGainG, int iGainB);
    int SwCamSetShutter(int iShutter);
    int SwCamSetCaptureGain(int iGain, int iMode);
    int SwCamSetCaptureRGBGain(int iGainR, int iGainG, int iGainB, int iMode);
    int SwCamSetCaptureShutter(int iShutter, int iMode);
    int SwCamSetFPSMode(int iMode);
    int SwCamSetCaptureEdge(int iMode);
    int SwCamSetGammaData(int** rgiDataXY);
    int SwCamSetReg(DWORD32 dwAddr, DWORD32 dwValue);
    int SwCamSyncSignal(int iFlashRateEnable, int iCaptureEnable);

    // 相机前端扩展接口, 扩展其它特定操作，如FPGA时间清除、从端软触发等
    int SwCamCtrl(int iCtrlID, BYTE8* pbData, DWORD32 dwLen);

    // PCI c
    int SwPciOpen(); 	// 返回PCI设备句柄
    int SwPciClose(int fd);
    int SwInitPciRWBuffers(int fd, swpci_buffer* pBuf);
    int SwPciPutRecvBuf(int fd, swpci_buffer* pBuf);
    int SwPciGetSendBuf(int fd, swpci_buffer* pBuf);
    int SwPciPutSendBuf(int fd, swpci_buffer* pBuf);
    int SwPciFreeSendBuf(int fd, swpci_buffer* pBuf);

    // 打开关闭系统设备 rw
    int SwDevOpen(int iFlags);  // 返回/dev/swdev设备句柄
    int SwDevClose(int fd);

    // 串口
    int SwSerialOpen(const char* szDevName); 	// 返回串口设备句柄
    int SwSerialClose(int fd);

    int SwSerialSetAttr(
        int fd,
        int iBaudrate,
        int iDatasize,
        int iParity,
        int iStopBit);

    int SwSerialSend(int fd, BYTE8* pBuf, DWORD32 dwLen);
    int SwSerialRecv(int fd, BYTE8* pBuf, DWORD32 dwLen);
    int SwSerialFlush(int fd, int iMode);

    // DMA c
    int SwDmaCopy1D(
        int fd,
        BYTE8* pbDstPhys,
        BYTE8* pbSrcPhys,
        DWORD32 dwSize
    );

    int SwDmaCopy2D(
        int fd,
        BYTE8* pbDstPhys, int iDstStride,
        BYTE8* pbSrcPhys, int iSrcStride,
        int iWidth, int iHeight
    );

    int SwDmaRotationYUV(
        int fd,
        BYTE8* pbSrcImgY,
        BYTE8* pbSrcImgUV,
        BYTE8* pbDstImgY,
        BYTE8* pbDstImgUV,
        int iWidth,
        int iHeight);

    int SwDmaCopyUV(
        int fd,
        BYTE8* pbSrcImgUV,
        BYTE8* pbDstImgU,
        BYTE8* pbDstImgV,
        int iWidth,
        int iHeight);

    int SwDmaConvImage();
    int SwDmaCopyJpeg();
    int SwDmaCopyH264();
    int SwDmaCopyYUV();
    int SwDmaPCIWrite();
    int SwDmaPCIRead();

    // 网络 c
    int SwNetGetPhyStatus(int fd, PHY_STATUS_STRUCT* pPhyStat);
#ifdef __cplusplus
}
#endif

#endif // SWDRV_H_INCLUDED
