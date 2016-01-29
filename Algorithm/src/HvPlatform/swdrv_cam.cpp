#include "swdrv.h"
#include "string.h"
#include <sys/ioctl.h>

#define SWDEV_NAME "/dev/swdev"
#define SWPCI_NAME "/dev/swpci"

/*extern "C"
{*/

static int SoftTriggerCapture();
static void SetFpgaStructContext(
    FPGA_STRUCT& cFpgaStruct,
    DWORD32 dwAddr,
    DWORD32 dwData
);

static int ClearFpgaTime();
static int SetGammaData(int rgiDataXY[8][2]);
static int SetTrafficLightEnhanceParam(
    int iHTh, int iLTh, int iSTh,
    int iColorFactor
);
static int SetTrafficLightEnhanceZone(
    int iId,
    int iX1, int iY1,
    int iX2, int iY2
);
static int SlaveSoftTriggerCapture();

//--------------------------------------------------------------------
//

int SwCamTrigger(int iMode)
{
    int iRet = 0;
    switch (iMode)
    {
    case SW_MODE_CAM_TRIGGER_SOFT_CAP :
        iRet = SoftTriggerCapture();
        break;

    default :
        iRet = EINVAL;
    }
    return iRet;
}

static int SoftTriggerCapture()
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));
    datas.count = 4;

#ifdef _CAMERA_PIXEL_500W_
    datas.regs[0].addr  = 0x905;
    datas.regs[0].data  = 1;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
    datas.regs[2].addr  = 0x905;
    datas.regs[2].data  = 0;
    datas.regs[3].addr  = 0x988;
    datas.regs[3].data  = 0;
#else
    datas.regs[0].addr  = 0x85;
    datas.regs[0].data  = 0x01;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
    datas.regs[2].addr  = 0x85;
    datas.regs[2].data  = 0x00;
    datas.regs[3].addr  = 0x88;
    datas.regs[3].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

//--------------------------------------------------------------------
//

int SwCamSetGain(int iGain)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    datas.count = 6;
    datas.regs[0].addr  = 0x90a;
    datas.regs[0].data  = 1;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
    datas.regs[2].addr  = 0x005;
    datas.regs[2].data  = iGain;
    datas.regs[3].addr  = 0x90a;
    datas.regs[3].data  = 0;
    datas.regs[4].addr  = 0x988;
    datas.regs[4].data  = 0;
    datas.regs[5].addr  = 0x005;
    datas.regs[5].data  = iGain;
#else
    datas.count = 1;
    datas.regs[0].addr  = 0x01;
    datas.regs[0].data  = iGain;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

//--------------------------------------------------------------------
//

int SwCamSetRGBGain(int iGainR, int iGainG, int iGainB)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

    iGainR &= 0x01ff;
    iGainG &= 0x01ff;
    iGainB &= 0x01ff;

#ifdef _CAMERA_PIXEL_500W_
    datas.count = 9;
    datas.regs[0].addr  = 0x90c;
    datas.regs[0].data  = iGainR;
    datas.regs[1].addr  = 0x90d;
    datas.regs[1].data  = iGainG;
    datas.regs[2].addr  = 0x90e;
    datas.regs[2].data  = iGainG;
    datas.regs[3].addr  = 0x90f;
    datas.regs[3].data  = iGainB;
    datas.regs[4].addr  = 0x91f;
    datas.regs[4].data  = iGainR;
    datas.regs[5].addr  = 0x920;
    datas.regs[5].data  = iGainG;
    datas.regs[6].addr  = 0x921;
    datas.regs[6].data  = iGainG;
    datas.regs[7].addr  = 0x922;
    datas.regs[7].data  = iGainB;
    datas.regs[8].addr  = 0x988;
    datas.regs[8].data  = 0;
#else
    int data04 = 0;
    int data05 = 0;

    // 0x gain1 gain0
    // 0x gain3 gain2
    //-----------------
    // 0x iGainG iGainB
    // 0x iGainR iGainG

    // for bt1120
    data04 |= iGainG;
    data04 <<= 9;
    data04 |= iGainB;

    data05 |= iGainR;
    data05 <<= 9;
    data05 |= iGainG;

    /*
    // for raw (bayer12)
    data04 |= iGainR;
    data04 <<= 9;
    data04 |= iGainG;

    data05 |= iGainG;
    data05 <<= 9;
    data05 |= iGainB;
    */

    datas.count = 2;
    datas.regs[0].addr  = 0x04;
    datas.regs[0].data  = data04;
    datas.regs[1].addr  = 0x05;
    datas.regs[1].data  = data05;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetShutter(int iShutter)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

    iShutter &= 0x00ffffff;

#ifdef _CAMERA_PIXEL_500W_
    datas.count = 2;
    datas.regs[0].addr  = 0x900;
    datas.regs[0].data  = iShutter;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0x00;
#else
    datas.count = 2;
    datas.regs[0].addr  = 0x80;
    datas.regs[0].data  = iShutter;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetCaptureGain(int iGain, int iMode)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    iGain &= 0x03ff;
    if ( iMode != SW_MODE_CAM_CAP_GAIN_DISENABLE )
    {
        // 将第28位置为1
        iGain |= 0x08000000;
    }

    datas.count = 3;
    datas.regs[0].addr  = 0x910;
    datas.regs[0].data  = iGain;
    datas.regs[1].addr  = 0x923;
    datas.regs[1].data  = iGain;
    datas.regs[2].addr  = 0x988;
    datas.regs[2].data  = 0;
#else
    int data72 = iGain & 0x03ff;
    if ( iMode != SW_MODE_CAM_CAP_GAIN_DISENABLE )
    {
        // 将第24位置为1
        data72 |= 0x00800000;
    }
    datas.count = 2;
    datas.regs[0].addr  = 0x72;
    datas.regs[0].data  = data72;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
#endif

    iRet = SwADWrite(datas);

    return iRet;
}

int SwCamSetCaptureRGBGain(int iGainR, int iGainG, int iGainB, int iMode)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    int data0 = (iGainR&0x01ff);
    int data1 = (iGainG&0x01ff);
    int data2 = (iGainG&0x01ff);
    int data3 = (iGainB&0x01ff);

    if ( iMode != SW_MODE_CAM_CAP_RGBGAIN_DISENABLE )
    {
        // 将第28位置为1
        data0 |= 0x08000000;
        data1 |= 0x08000000;
        data2 |= 0x08000000;
        data3 |= 0x08000000;
    }

    datas.count = 9;
    datas.regs[0].addr  = 0x911;
    datas.regs[0].data  = data0;
    datas.regs[1].addr  = 0x912;
    datas.regs[1].data  = data1;
    datas.regs[2].addr  = 0x913;
    datas.regs[2].data  = data2;
    datas.regs[3].addr  = 0x914;
    datas.regs[3].data  = data3;
    datas.regs[4].addr  = 0x924;
    datas.regs[4].data  = data0;
    datas.regs[5].addr  = 0x925;
    datas.regs[5].data  = data1;
    datas.regs[6].addr  = 0x926;
    datas.regs[6].data  = data2;
    datas.regs[7].addr  = 0x927;
    datas.regs[7].data  = data3;
    datas.regs[8].addr  = 0x988;
    datas.regs[8].data  = 0;
#else
    int data73 = 0;
    int data74 = 0;

    data73 |= (iGainG&0x01ff);
    data73 <<= 9;
    data73 |= (iGainB&0x01ff);

    data74 |= (iGainR&0x01ff);
    data74 <<= 9;
    data74 |= (iGainG&0x01ff);

    if ( iMode != SW_MODE_CAM_CAP_RGBGAIN_DISENABLE )
    {
        // 将第24位置为1
        data73 |= 0x00800000;
        data74 |= 0x00800000;
    }

    datas.count = 3;
    datas.regs[0].addr  = 0x73;
    datas.regs[0].data  = data73;
    datas.regs[1].addr  = 0x74;
    datas.regs[1].data  = data74;
    datas.regs[2].addr  = 0x88;
    datas.regs[2].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetCaptureShutter(int iShutter, int iMode)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    iShutter &= 0x00ffffff;
    if ( iMode != SW_MODE_CAM_CAP_SHUTTER_DISENABLE )
    {
        // 将第28位置为1
        iShutter |= 0x08000000;
    }

    datas.count = 2;
    datas.regs[0].addr  = 0x901;
    datas.regs[0].data  = iShutter;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
#else
    int data81 = 0;

    data81 = iShutter&0x00ffffff;

    if ( iMode != SW_MODE_CAM_CAP_SHUTTER_DISENABLE )
    {
        // 将第24位置为1
        data81 |= 0x00800000;
    }

    datas.count = 2;
    datas.regs[0].addr  = 0x81;
    datas.regs[0].data  = data81;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetFPSMode(int iMode)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    datas.count = 2;
    datas.regs[0].addr  = 0x906;
    datas.regs[0].data  = iMode&0x01;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
#else
    datas.count = 2;
    datas.regs[0].addr  = 0x86;
    datas.regs[0].data  = iMode&0x01;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetCaptureEdge(int iMode)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));
    datas.count = 2;

#ifdef _CAMERA_PIXEL_500W_
    datas.regs[0].addr  = 0x909;
    datas.regs[0].data  = iMode&0x01;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
#else
    datas.regs[0].addr  = 0x71;
    datas.regs[0].data  = iMode&0x01;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetGammaData(int** rgiDataXY)
{
    return SetGammaData((int (*)[2])rgiDataXY);
}

static int SetGammaData(int rgiDataXY[8][2])
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    DWORD32 dwDataTmp = 0;
    datas.count = 8 + 8 + 1;
    for ( int i = 0; i < 8; ++i )
    {
        datas.regs[i].addr  = 0x915 + i;
        datas.regs[i+8].addr  = 0x928 + i;
        dwDataTmp = ((rgiDataXY[i][0]&0xff)<<8) | (rgiDataXY[i][1]&0xff);
        datas.regs[i].data  = dwDataTmp;
        datas.regs[i+8].data = dwDataTmp;
    }
    datas.regs[16].addr = 0x988;
    datas.regs[16].data = 0;
#else
    datas.count = 9;
    for ( int i = 0; i < 8; ++i )
    {
        datas.regs[i].addr  = 0x75 + i;
        datas.regs[i].data  = ((rgiDataXY[i][0]&0xff)<<8) | (rgiDataXY[i][1]&0xff);
    }
    datas.regs[8].addr = 0x88;
    datas.regs[8].data = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamSetReg(DWORD32 dwAddr, DWORD32 dwValue)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

    SetFpgaStructContext(datas, dwAddr, dwValue);

#ifdef _CAMERA_PIXEL_500W_
    if ( 0x88 == dwAddr )
    {
        SetFpgaStructContext(datas, 0x988, dwValue);
    }
#else
    // do nothing
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

static void SetFpgaStructContext(
    FPGA_STRUCT& cFpgaStruct,
    DWORD32 dwAddr,
    DWORD32 dwData
)
{
    cFpgaStruct.regs[cFpgaStruct.count].addr  = dwAddr;
    cFpgaStruct.regs[cFpgaStruct.count].data  = dwData;
    cFpgaStruct.count++;
}

int SwCamSyncSignal(int iFlashRateEnable, int iCaptureEnable)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));
    datas.count = 2;

    int iValue = 0;
    int iFlashValue = 0;
    int iCaptureValue = 0;

    if ( 1 == iFlashRateEnable )
    {
        iFlashValue |= 0x01;
    }
    else
    {
        iFlashValue &= (~0x01);
    }

    if ( 1 == iCaptureEnable )
    {
        iCaptureValue |= 0x02;
    }
    else
    {
        iCaptureValue &= (~0x02);
    }

    iValue = (iFlashValue | iCaptureValue);

#ifdef _CAMERA_PIXEL_500W_
    datas.regs[0].addr  = 0x902;
    datas.regs[0].data  = iValue;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
#else
    datas.regs[0].addr  = 0x82;
    datas.regs[0].data  = iValue;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

int SwCamCtrl(int iCtrlID, BYTE8* pbData, DWORD32 dwLen)
{
    int iRet = 0;
    switch (iCtrlID)
    {
    case SW_CTRLID_FGPA_TIME_CLR :
        iRet = ClearFpgaTime();
        break;

    case SW_CTRLID_SLAVE_TRIGGER_CAP :
        //if (SW_DEVID_CAM_CTRL_SLAVE != iDevID)
        {
            iRet = EINVAL;
            break;
        }
        iRet = SlaveSoftTriggerCapture();
        break;

    case SW_CTRLID_TRAFFIC_LIGHT_ENHANCE_PARAM :
    {
        if (dwLen != sizeof(int)*4)
        {
            iRet = EINVAL;
            break;
        }
        int* piData = (int*)pbData;
        int iHTh = piData[0];
        int iLTh = piData[1];
        int iSTh = piData[2];
        int iColorFactor = piData[3];

        iRet = SetTrafficLightEnhanceParam(iHTh, iLTh, iSTh,
                                           iColorFactor);
    }
    break;

    case SW_CTRLID_TRAFFIC_LIGHT_ENHANCE_ZONE :
    {
        if (dwLen != sizeof(int)*5)
        {
            iRet = EINVAL;
            break;
        }
        int* piData = (int*)pbData;
        int iId = piData[0];
        int iX1 = piData[1];
        int iY1 = piData[2];
        int iX2 = piData[3];
        int iY2 = piData[4];
        iRet = SetTrafficLightEnhanceZone(iId, iX1, iY1,
                                          iX2, iY2);
    }
    break;

    default :
        iRet = EINVAL;
        break;
    }

    return iRet;
}

static int ClearFpgaTime()
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));
    datas.count = 4;

#ifdef _CAMERA_PIXEL_500W_
    datas.regs[0].addr  = 0x90b;
    datas.regs[0].data  = 0;
    datas.regs[1].addr  = 0x988;
    datas.regs[1].data  = 0;
    datas.regs[2].addr  = 0x90b;
    datas.regs[2].data  = 1;
    datas.regs[3].addr  = 0x988;
    datas.regs[3].data  = 0;
#else
    datas.regs[0].addr  = 0x7d;
    datas.regs[0].data  = 0x00;
    datas.regs[1].addr  = 0x88;
    datas.regs[1].data  = 0x00;
    datas.regs[2].addr  = 0x7d;
    datas.regs[2].data  = 0x01;
    datas.regs[3].addr  = 0x88;
    datas.regs[3].data  = 0x00;
#endif

    iRet = SwADWrite(datas);
    return iRet;
}

static int SetTrafficLightEnhanceParam(
    int iHTh, int iLTh, int iSTh,
    int iColorFactor
)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    SetFpgaStructContext(datas, 0x934, iHTh&0x3ffff);
    SetFpgaStructContext(datas, 0x935, iLTh&0x3fff);
    SetFpgaStructContext(datas, 0x936, iSTh&0x3fff);
    SetFpgaStructContext(datas, 0x937, iColorFactor&0x3ffff);
    SetFpgaStructContext(datas, 0x988, 0);
#else
    iRet = ENOSYS;
#endif

    if (iRet != 0)
    {
        return iRet;
    }

    iRet = SwADWrite(datas);
    return iRet;
}

static int SetTrafficLightEnhanceZone(
    int iId,
    int iX1, int iY1,
    int iX2, int iY2
)
{
    int iRet = 0;
    FPGA_STRUCT datas;
    bzero(&datas, sizeof(datas));

#ifdef _CAMERA_PIXEL_500W_
    SetFpgaStructContext(datas, 0x938, iId&0x1f);
    SetFpgaStructContext(datas, 0x939, iX1&0xfff);
    SetFpgaStructContext(datas, 0x93b, iY1&0xfff);
    SetFpgaStructContext(datas, 0x93a, iX2&0xfff);
    SetFpgaStructContext(datas, 0x93c, iY2&0xfff);
    SetFpgaStructContext(datas, 0x988, 0);
#else
    iRet = ENOSYS;
#endif

    if (iRet != 0)
    {
        return iRet;
    }

    iRet = SwADWrite(datas);
    return iRet;
}

static int SlaveSoftTriggerCapture()
{
    int fd = SwDevOpen(O_RDWR); // 打开
    int iRet = ioctl(fd, SWDEV_IOCTL_GET_PHOTO_FROM_SLAVE, 0);
    SwDevClose(fd);
    return iRet;
}
/*
}   // end of extern "C"*/
