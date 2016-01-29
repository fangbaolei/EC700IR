// 该文件编码必须是WINDOWS-936格式
#include "LoadParam.h"
#include "HvParamIO.h"
#include "hvtarget_ARM.h"
#include "HvPciLinkApi.h"
#include "ControlFunc.h"
#include "DataCtrl.h"

static CEprFileStorage g_cFileStorage(PARAM_START_ADDR, PARAM_SIZE);
CParamStore g_cParamStore;
ModuleParams g_cModuleParams;

HRESULT LoadModuleParam(ModuleParams& cModuleParams)
{
    HRESULT hr = E_FAIL;

    hr = g_cFileStorage.Initialize("");
    if ( S_OK != hr && S_FALSE != hr )
    {
        HV_Trace(5, "<g_cFileStorage.Initialize> Error! hr = [0x%08x]\n", hr);
        return hr;
    }

    g_cParamStore.Initialize(&g_cFileStorage, true);

    WdtHandshake();

    // 读取UBoot使用的网络参数，将其设置到应用层，并同步到应用层的网络设置参数中。
    SetIpFromEEPROM();

    WdtHandshake();

    LoadTcpipParam(&g_cParamStore, cModuleParams.cTcpipCfgParam);
    LoadCamAppParam(&g_cParamStore, cModuleParams.cCamAppParam);
    LoadDeviceInfoParam(&g_cParamStore, cModuleParams.cDeviceInfoParam);
    LoadFrontPannelParam(&g_cParamStore, cModuleParams.cFrontPannelParam);

    g_cParamStore.Save(HvCore::PSM_SIMPLE);

    return S_OK;
}

//--------------------------------------------------------------------------

int LoadTcpipParam(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam
)
{
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\TcpipCfg", "CommIPAddr",
        cTcpipCfgParam.szIp, 31, //TODO:字符长度按照实际填写
        "IP地址", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\TcpipCfg", "CommMask",
        cTcpipCfgParam.szNetmask, 31,
        "子网掩码", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\TcpipCfg", "CommGateway",
        cTcpipCfgParam.szGateway, 31,
        "网关", "", CUSTOM_LEVEL
    );

    return 0;
}

//--------------------------------------------------------------------------

int LoadCamAppParam(
    CParamStore* pcParamStore,
    CamAppParam& cCamAppParam
)
{
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "TraceRank",
        &g_nTraceRank,
        5, 1, 5,
        "调试信息输出等级", "1:研发级 3:工程级 5:用户级",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SendType",
        &cCamAppParam.iSendType,
        cCamAppParam.iSendType, -1, 0,
        "流传输模式", "-1:不传输 0:HvCam传输协议",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegStream",
        &cCamAppParam.iJpegStream,
        cCamAppParam.iJpegStream, 0, 1,
        "Jpeg流使能,如同时使能H264流，则强制关闭Jpeg流", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "H264Stream",
        &cCamAppParam.iH264Stream,
        cCamAppParam.iH264Stream, 0, 1,
        "H.264流使能", "",
        CUSTOM_LEVEL);

    if (1 == cCamAppParam.iH264Stream)
    {
        cCamAppParam.iJpegStream = 0;
    }

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegCompressRate",
        &cCamAppParam.iJpegCompressRate,
        cCamAppParam.iJpegCompressRate, 1, 100,
        "视频流Jpeg压缩品质", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegCompressRateCapture",
        &cCamAppParam.iJpegCompressRateCapture,
        cCamAppParam.iJpegCompressRateCapture, 1, 100,
        "抓拍Jpeg压缩品质", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "IFrameInterval",
        &cCamAppParam.iIFrameInterval,
        cCamAppParam.iIFrameInterval, 0, 15,
        "H.264流I帧间隔", "",
        CUSTOM_LEVEL);

    /*HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "FrameRate",
        &cCamAppParam.iFrameRate,
        cCamAppParam.iFrameRate, 1, 30,
        "H.264流帧率", "",
        INNER_LEVEL);*/

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "TargetBitRate",
        &cCamAppParam.iTargetBitRate,
        cCamAppParam.iTargetBitRate, 512*1024, 8*1024*1024,
        "H.264流输出比特率", "单位：bps（比特每秒）",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AGCTh",
        &cCamAppParam.iAGCTh,
        cCamAppParam.iAGCTh, 0, 255,
        "AGC测光基准值", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AGCShutterLOri",
        &cCamAppParam.iAGCShutterLOri,
        cCamAppParam.iAGCShutterLOri, 0, 54000,
        "AGC快门调节下限", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AGCShutterHOri",
        &cCamAppParam.iAGCShutterHOri,
        cCamAppParam.iAGCShutterHOri, 0, 54000,
        "AGC快门调节上限", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AGCGainLOri",
        &cCamAppParam.iAGCGainLOri,
        cCamAppParam.iAGCGainLOri, 72, 360,
        "AGC增益调节下限", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AGCGainHOri",
        &cCamAppParam.iAGCGainHOri,
        cCamAppParam.iAGCGainHOri, 72, 360,
        "AGC增益调节上限", "",
        CUSTOM_LEVEL);


    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "GainR",
        &cCamAppParam.iGainR,
        cCamAppParam.iGainR, 0, 255,
        "R增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "GainG",
        &cCamAppParam.iGainG,
        cCamAppParam.iGainG, 0, 255,
        "G增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "GainB",
        &cCamAppParam.iGainB,
        cCamAppParam.iGainB, 0, 255,
        "B增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Gain",
        &cCamAppParam.iGain,
        cCamAppParam.iGain, 0, 360,
        "增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Shutter",
        &cCamAppParam.iShutter,
        cCamAppParam.iShutter, 0, 54000,
        "快门", "单位：us",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureGainR",
        &cCamAppParam.iCaptureGainR,
        cCamAppParam.iCaptureGainR, 0, 255,
        "抓拍R增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureGainG",
        &cCamAppParam.iCaptureGainG,
        cCamAppParam.iCaptureGainG, 0, 255,
        "抓拍G增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureGainB",
        &cCamAppParam.iCaptureGainB,
        cCamAppParam.iCaptureGainB, 0, 255,
        "抓拍B增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureGain",
        &cCamAppParam.iCaptureGain,
        cCamAppParam.iCaptureGain, 0, 360,
        "抓拍增益", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureShutter",
        &cCamAppParam.iCaptureShutter,
        cCamAppParam.iCaptureShutter, 0, 54000,
        "抓拍快门", "单位：us",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableCaptureGainRGB",
        &cCamAppParam.iEnableCaptureGainRGB,
        cCamAppParam.iEnableCaptureGainRGB, 0, 1,
        "抓拍RGB增益使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableCaptureGain",
        &cCamAppParam.iEnableCaptureGain,
        cCamAppParam.iEnableCaptureGain, 0, 1,
        "抓拍增益使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableCaptureShutter",
        &cCamAppParam.iEnableCaptureShutter,
        cCamAppParam.iEnableCaptureShutter, 0, 1,
        "抓拍快门使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CameraSideInstall",
        &cCamAppParam.fIsSideInstall,
        cCamAppParam.fIsSideInstall, 0, 1,
        "前端镜头侧装", "0:正装 1:侧装",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegExpectSize",
        &cCamAppParam.iJpegExpectSize,
        cCamAppParam.iJpegExpectSize, 0, 1024*1024,
        "Jpeg图片期望大小", "单位：Byte（字节）注：为0则表示忽略该参数",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegType",
        &cCamAppParam.iJpegType,
        cCamAppParam.iJpegType, 0, 1,
        "Jpeg图片格式", "0:YUV分开，1:YUV混合",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegCompressRateLow",
        &cCamAppParam.iJpegCompressRateL,
        cCamAppParam.iJpegCompressRateL, 1, 100,
        "Jpeg压缩率自动调节下限", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "JpegCompressRateHigh",
        &cCamAppParam.iJpegCompressRateH,
        cCamAppParam.iJpegCompressRateH, 1, 100,
        "Jpeg压缩率自动调节上限", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "OutputFrameRate",
        &cCamAppParam.iOutputFrameRate,
        cCamAppParam.iOutputFrameRate, 0, 15,
        "相机输出帧率", "相机视频输出的最大帧率",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AGCEnable",
        &cCamAppParam.iAGCEnable,
        cCamAppParam.iAGCEnable, 0, 1,
        "AGC使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AWBEnable",
        &cCamAppParam.iAWBEnable,
        cCamAppParam.iAWBEnable, 0, 1,
        "AWB使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "FlashRateSynSignalEnable",
        &cCamAppParam.iFlashRateSynSignalEnable,
        cCamAppParam.iFlashRateSynSignalEnable, 0, 1,
        "频闪同步信号使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureSynSignalEnable",
        &cCamAppParam.iCaptureSynSignalEnable,
        cCamAppParam.iCaptureSynSignalEnable, 0, 1,
        "抓拍同步信号使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "ENetSyn",
        &cCamAppParam.iENetSyn,
        cCamAppParam.iENetSyn, 0, 1,
        "电网同步使能", "0:15fps 1:12.5fps",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "CaptureEdge",
        &cCamAppParam.iCaptureEdge,
        cCamAppParam.iCaptureEdge, 0, 3,
        "触发抓拍沿", "0:外部下降沿触发 1:外部上升沿触发 2: 上升沿下降沿都触发 3：不触发",
        CUSTOM_LEVEL);

    cCamAppParam.iFlashDifferentLaneExt = 0;
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "FlashDifferentLaneEx",
        &cCamAppParam.iFlashDifferentLaneExt,
        cCamAppParam.iFlashDifferentLaneExt, 0, 1,
        "闪光灯分车道闪(硬触发)", "0:不分车道闪 1:分车道闪",
        PROJECT_LEVEL);

    ///以下5个参数不对外公开
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Character_X",
        &cCamAppParam.iX,
        cCamAppParam.iX, 0, 1600,
        "字符叠X坐标", "注：正装时最大宽度为1600，侧装时为1200，不能设置为边沿值",
        INNER_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Character_Y",
        &cCamAppParam.iY,
        cCamAppParam.iY, 0, 1600,
        "字符叠Y坐标", "注：正装时最大宽度为1200，侧装时为1600，不能设置为边沿值",
        INNER_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Character_DateFormat",
        &cCamAppParam.iDateFormat,
        cCamAppParam.iDateFormat, 0, 1,
        "日期格式", "0: '/' 1: '-'",
        INNER_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Character_FontColor",
        &cCamAppParam.iFontColor,
        cCamAppParam.iFontColor, 0, 0xffffff,
        "字体颜色", "字体颜色为RGB格式",
        INNER_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "Character_LatticeLen",
        &cCamAppParam.iLatticeLen,
        cCamAppParam.iLatticeLen, 0, 0xffff,
        "路段信息点阵长度", "此参数是附加的字符信息的长度，由软件设置，无需手动修改",
        INNER_LEVEL);
    //////////////////////////

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableCharacter",
        &cCamAppParam.iEnableCharacterOverlap,
        cCamAppParam.iEnableCharacterOverlap, 0, 1,
        "字符叠加使能", "0:不使能 1:使能",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableFixedLight",
        &cCamAppParam.iEnableFixedLight,
        cCamAppParam.iEnableFixedLight, 0, 1,
        "字符叠加字体固定亮度", "0:不固定(随场景亮度而变化) 1:固定",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SafeSaver",
        &cCamAppParam.fIsSafeSaver,
        cCamAppParam.fIsSafeSaver, 0, 1,
        "安全存储使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SaveVideo",
        &cCamAppParam.fSaveVideo,
        cCamAppParam.fSaveVideo, 0, 1,
        "录像保存使能", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "InitHdd",
        &cCamAppParam.fInitHdd,
        cCamAppParam.fInitHdd, 0, 1,
        "初始化硬盘", "对硬盘进行存储初始化。注：一次性有效",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "OneFileSize",
        &cCamAppParam.iFileSize,
        cCamAppParam.iFileSize, 1, 1024*1024,
        "定长存储单个文件大小,(单位K)", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SaveSafeType",
        &cCamAppParam.iSaveSafeType,
        cCamAppParam.iSaveSafeType, 0, 1,
        "固态硬盘存储方式", "0:没有客户端连接时才存储 1:一直存储",
        CUSTOM_LEVEL);

    cCamAppParam.iDiskType = 2;
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "DiskType",
        &cCamAppParam.iDiskType,
        cCamAppParam.iDiskType, 0, 2,
        "硬盘类型 (0:群辉网络硬盘 2:固态硬盘)", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableH264BrightnessAdjust",
        &cCamAppParam.fEnableH264BrightnessAdjust,
        cCamAppParam.fEnableH264BrightnessAdjust, 0, 1,
        "使能H264亮度调节", "0:不使能 1:使能",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AdjustPointX",
        &cCamAppParam.iAdjustPointX,
        cCamAppParam.iAdjustPointX, 1, 254,
        "H264亮度调节点X值", "",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "AdjustPointY",
        &cCamAppParam.iAdjustPointY,
        cCamAppParam.iAdjustPointY, 0, 255,
        "H264亮度调节点Y值", "",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "NetDiskIP",
        cCamAppParam.szNetDiskIP, sizeof(cCamAppParam.szNetDiskIP),
        "网络存储的IP地址", "",
        CUSTOM_LEVEL);
/*
    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "FtpRoot",
        cCamAppParam.szFtpRoot, sizeof(cCamAppParam.szFtpRoot),
        "网络存储的FTP根路径(FTP方式存储时有效)", "",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "NetDiskUser",
        cCamAppParam.szNetDiskUser, sizeof(cCamAppParam.szNetDiskUser),
        "网络存储的用户名", "",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "NetDiskPwd",
        cCamAppParam.szNetDiskPwd, sizeof(cCamAppParam.szNetDiskPwd),
        "网络存储的用户密码", "",
        CUSTOM_LEVEL);
*/
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "NetDiskSpace",
        &cCamAppParam.iNetDiskSpace,
        cCamAppParam.iNetDiskSpace, 1, 3696,
        "网络存储的容量,单位G (存储初始化时有意义)", "",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "NFS",
        cCamAppParam.szNFS, sizeof(cCamAppParam.szNFS),
        "NFS路径", "",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp\\NetDisk", "NFSParam",
        cCamAppParam.szNFSParam, sizeof(cCamAppParam.szNFSParam),
        "NFS挂载参数", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "AvgY",
        &cCamAppParam.iAvgY, cCamAppParam.iAvgY,
        0, 255,
        "晚上亮度阈值", "低于此阈值的才进行Gamma调整", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X0",
        &cCamAppParam.rgiGamma[0][0], cCamAppParam.rgiGamma[0][0],
        0, 255,
        "X0", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y0",
        &cCamAppParam.rgiGamma[0][1], cCamAppParam.rgiGamma[0][1],
        0, 255,
        "Y0", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X1",
        &cCamAppParam.rgiGamma[1][0], cCamAppParam.rgiGamma[1][0],
        0, 255,
        "X1", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y1",
        &cCamAppParam.rgiGamma[1][1], cCamAppParam.rgiGamma[1][1],
        0, 255,
        "Y1", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X2",
        &cCamAppParam.rgiGamma[2][0], cCamAppParam.rgiGamma[2][0],
        0, 255,
        "X2", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y2",
        &cCamAppParam.rgiGamma[2][1], cCamAppParam.rgiGamma[2][1],
        0, 255,
        "Y2", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X3",
        &cCamAppParam.rgiGamma[3][0], cCamAppParam.rgiGamma[3][0],
        0, 255,
        "X3", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y3",
        &cCamAppParam.rgiGamma[3][1], cCamAppParam.rgiGamma[3][1],
        0, 255,
        "Y3", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X4",
        &cCamAppParam.rgiGamma[4][0], cCamAppParam.rgiGamma[4][0],
        0, 255,
        "X4", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y4",
        &cCamAppParam.rgiGamma[4][1], cCamAppParam.rgiGamma[4][1],
        0, 255,
        "Y4", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X5",
        &cCamAppParam.rgiGamma[5][0], cCamAppParam.rgiGamma[5][0],
        0, 255,
        "X5", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y5",
        &cCamAppParam.rgiGamma[5][1], cCamAppParam.rgiGamma[5][1],
        0, 255,
        "Y5", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X6",
        &cCamAppParam.rgiGamma[6][0], cCamAppParam.rgiGamma[6][0],
        0, 255,
        "X6", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y6",
        &cCamAppParam.rgiGamma[6][1], cCamAppParam.rgiGamma[6][1],
        0, 255,
        "Y6", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "X7",
        &cCamAppParam.rgiGamma[7][0], cCamAppParam.rgiGamma[7][0],
        0, 255,
        "X7", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\Gamma", "Y7",
        &cCamAppParam.rgiGamma[7][1], cCamAppParam.rgiGamma[7][1],
        0, 255,
        "Y7", "", CUSTOM_LEVEL
    );
    //校验gamma是否合理
    bool fValid = true;
    for (int i = 1; i < 8; i++)
    {
        if (cCamAppParam.rgiGamma[i - 1][0] > cCamAppParam.rgiGamma[i][0]
                || cCamAppParam.rgiGamma[i - 1][0] < 0 || cCamAppParam.rgiGamma[i - 1][0] > 255
                || cCamAppParam.rgiGamma[i][0] < 0 || cCamAppParam.rgiGamma[i][0] > 255
                || cCamAppParam.rgiGamma[i - 1][1] < 0 || cCamAppParam.rgiGamma[i - 1][1] > 255
                || cCamAppParam.rgiGamma[i][1] < 0 || cCamAppParam.rgiGamma[i][1] > 255)
        {
            fValid = false;
            break;
        }
    }
    if (!fValid)
    {
        for (int i = 0; i < 8; i++)
        {
            cCamAppParam.rgiGamma[i][0] = cCamAppParam.rgiDefGamma[i][0];
            cCamAppParam.rgiGamma[i][1] = cCamAppParam.rgiDefGamma[i][1];
        }
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X0", cCamAppParam.rgiGamma[0][0], cCamAppParam.rgiGamma[0][0], 0, 255, "X0", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y0", cCamAppParam.rgiGamma[0][1], cCamAppParam.rgiGamma[0][1], 0, 255, "Y0", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X1", cCamAppParam.rgiGamma[1][0], cCamAppParam.rgiGamma[1][0], 0, 255, "X1", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y1", cCamAppParam.rgiGamma[1][1], cCamAppParam.rgiGamma[1][1], 0, 255, "Y1", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X2", cCamAppParam.rgiGamma[2][0], cCamAppParam.rgiGamma[2][0], 0, 255, "X2", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y2", cCamAppParam.rgiGamma[2][1], cCamAppParam.rgiGamma[2][1], 0, 255, "Y2", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X3", cCamAppParam.rgiGamma[3][0], cCamAppParam.rgiGamma[3][0], 0, 255, "X3", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y3", cCamAppParam.rgiGamma[3][1], cCamAppParam.rgiGamma[3][1], 0, 255, "Y3", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X4", cCamAppParam.rgiGamma[4][0], cCamAppParam.rgiGamma[4][0], 0, 255, "X4", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y4", cCamAppParam.rgiGamma[4][1], cCamAppParam.rgiGamma[4][1], 0, 255, "Y4", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X5", cCamAppParam.rgiGamma[5][0], cCamAppParam.rgiGamma[5][0], 0, 255, "X5", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y5", cCamAppParam.rgiGamma[5][1], cCamAppParam.rgiGamma[5][1], 0, 255, "Y5", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X6", cCamAppParam.rgiGamma[6][0], cCamAppParam.rgiGamma[6][0], 0, 255, "X6", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y6", cCamAppParam.rgiGamma[6][1], cCamAppParam.rgiGamma[6][1], 0, 255, "Y6", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "X7", cCamAppParam.rgiGamma[7][0], cCamAppParam.rgiGamma[7][0], 0, 255, "X7", "", CUSTOM_LEVEL);
        HvParamWriteInt(&g_cParamStore, "\\CamApp\\Gamma", "Y7", cCamAppParam.rgiGamma[7][1], cCamAppParam.rgiGamma[7][1], 0, 255, "Y7", "", CUSTOM_LEVEL);
    }

    // zhaopy auto...
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "Enable",
        &cCamAppParam.iEnableAutoCapture, cCamAppParam.iEnableAutoCapture,
        0, 1,
        "动态抓拍参数开关", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "ShutterCloudy",
        &cCamAppParam.iShutterCloudy, cCamAppParam.iShutterCloudy,
        50, 50000,
        "阴天曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "GainCloudy",
        &cCamAppParam.iGainCloudy, cCamAppParam.iGainCloudy,
        72, 320,
        "阴天增益", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "ShutterDay",
        &cCamAppParam.iShutterDay, cCamAppParam.iShutterDay,
        50, 50000,
        "晴天曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "GainDay",
        &cCamAppParam.iGainDay, cCamAppParam.iGainDay,
        72, 320,
        "晴天增益", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "ShutterBLight",
        &cCamAppParam.iShutterBLight, cCamAppParam.iShutterBLight,
        50, 50000,
        "逆光曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "GainBLight",
        &cCamAppParam.iGainBLight, cCamAppParam.iGainBLight,
        72, 320,
        "逆光增益", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "ShutterFLight",
        &cCamAppParam.iShutterFLight, cCamAppParam.iShutterFLight,
        50, 50000,
        "顺光曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "GainFLight",
        &cCamAppParam.iGainFLight, cCamAppParam.iGainFLight,
        72, 320,
        "顺光增益", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "ShutterNight",
        &cCamAppParam.iShutterNeight, cCamAppParam.iShutterNeight,
        50, 50000,
        "晚上曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\AutoCapture", "GainNight",
        &cCamAppParam.iGainNeight, cCamAppParam.iGainNeight,
        72, 320,
        "晚上增益", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "EnableSharpen",
        &cCamAppParam.iEnableSharpen,
        cCamAppParam.iEnableSharpen, 0, 1,
        "图像锐化使能开关", "0：不使能 1：使能",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SharpenCapture",
        &cCamAppParam.iSharpenCapture,
        cCamAppParam.iSharpenCapture, 0, 1,
        "锐化抓拍帧", "0：锐化所有帧 1：锐化抓拍帧",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SharpenThreshold",
        &cCamAppParam.iSharpenThreshold,
        cCamAppParam.iSharpenThreshold, 0, 100,
        "图像锐度值", "范围：[0~100]",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "ContrastValue",
        &cCamAppParam.iContrastValue,
        cCamAppParam.iContrastValue, -100, 100,
        "图像对比度", "范围：[-100~100]",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp", "SaturationValue",
        &cCamAppParam.iSaturationValue,
        cCamAppParam.iSaturationValue, -100, 100,
        "图像饱和度", "范围：[-100~100]",
        CUSTOM_LEVEL);

    float fltTemp = 1.3;
    HvParamReadFloatWithWrite(
        pcParamStore, "\\CamApp", "fltAWBRawRGain",
        &fltTemp, fltTemp, 0.0, 2.0,
        "AWBRawRGain", "...",
        PROJECT_LEVEL);
    cCamAppParam.iAWBRawRGain = (int)(fltTemp * 16384);

    fltTemp = 1.0;
    HvParamReadFloatWithWrite(
        pcParamStore, "\\CamApp", "fltAWBRawGrGain",
        &fltTemp, fltTemp, 0.0, 2.0,
        "AWBRawGrGain", "...",
        PROJECT_LEVEL);
    cCamAppParam.iAWBRawGrGain = (int)(fltTemp * 16384);

    fltTemp = 1.0;
    HvParamReadFloatWithWrite(
        pcParamStore, "\\CamApp", "fltAWBRawGbGain",
        &fltTemp, fltTemp, 0.0, 2.0,
        "AWBRawGbGain", "...",
        PROJECT_LEVEL);
    cCamAppParam.iAWBRawGbGain = (int)(fltTemp * 16384);

    fltTemp = 1.5;
    HvParamReadFloatWithWrite(
        pcParamStore, "\\CamApp", "fltAWBRawBGain",
        &fltTemp, fltTemp, 0.0, 2.0,
        "AWBRawBGain", "...",
        PROJECT_LEVEL);
    cCamAppParam.iAWBRawBGain = (int)(fltTemp * 16384);

    // TODO: 这里有个Bug，在HvSetup端无法更改这个String型的参数。
    /*
    HvParamReadStringWithWrite(
        pcParamStore, "\\CamApp", "SafeSaverInitStr",
        cCamAppParam.szSafeSaverInitStr, sizeof(cCamAppParam.szSafeSaverInitStr),
        "安全储存器初始化参数", "",
        CUSTOM_LEVEL);
    */
    return 0;
}

int LoadDeviceInfoParam(
    CParamStore* pcParamStore,
    DeviceInfoParam& cDeviceInfoParam
)
{
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System", "CommMac",
        cDeviceInfoParam.szMac, 31,
        "MAC地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System", "CommDevNo",
        cDeviceInfoParam.szSn, 127,
        "设备编号", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System", "Version",
        cDeviceInfoParam.szVer, 63,
        "版本信息", "", CUSTOM_LEVEL
    );

    return 0;
}

int LoadFrontPannelParam(
    CParamStore* pcParamStore,
    FrontPannelParam& cFrontPannelParam
)
{
    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel", "PannelStyle",
        &cFrontPannelParam.iUsedAutoControllMode,
        cFrontPannelParam.iUsedAutoControllMode, 0, 1,
        "控制板类型，0为拨码板，1为数控板", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel", "AutoRun",
        &cFrontPannelParam.iAutoRunFlag,
        cFrontPannelParam.iAutoRunFlag, 0, 1,
        "控制板运行方式，0为手动运行，1为自动运行", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel", "DivFrequency",
        &cFrontPannelParam.iDivFrequency,
        cFrontPannelParam.iDivFrequency, 1, 8,
        "分频系数", "",
        CUSTOM_LEVEL);

    if(cFrontPannelParam.iDivFrequency != 1
        && cFrontPannelParam.iDivFrequency != 2
        && cFrontPannelParam.iDivFrequency != 4
        && cFrontPannelParam.iDivFrequency != 8)
        {
            cFrontPannelParam.iDivFrequency = 1;
            HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel", "DivFrequency",
            cFrontPannelParam.iDivFrequency,
            cFrontPannelParam.iDivFrequency,
            1, 8, "分频系数", "",
            CUSTOM_LEVEL);
        }

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel", "OutPutDelay",
        &cFrontPannelParam.iOutPutDelay,
        cFrontPannelParam.iOutPutDelay, 0, 150,
        "输出延迟", "1对应0.1MS",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel", "PulseWidthMin",
        &cFrontPannelParam.iPulseWidthMin,
        cFrontPannelParam.iPulseWidthMin, 1, 72,
        "频闪脉宽最小值", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel", "PulseWidthMax",
        &cFrontPannelParam.iPulseWidthMax,
        cFrontPannelParam.iPulseWidthMax, 9, 80,
        "频闪脉宽最大值", "",
        CUSTOM_LEVEL);

    if (cFrontPannelParam.iPulseWidthMax - cFrontPannelParam.iPulseWidthMin < 8)
    {
        cFrontPannelParam.iPulseWidthMin = 5;
        cFrontPannelParam.iPulseWidthMax = 42;
        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel", "PulseWidthMin",
            cFrontPannelParam.iPulseWidthMin,
            cFrontPannelParam.iPulseWidthMin,
            1, 72, "频闪脉宽最小值", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel", "PulseWidthMax",
            cFrontPannelParam.iPulseWidthMax,
            cFrontPannelParam.iPulseWidthMax,
            9, 80, "频闪脉宽最大值", "",
            CUSTOM_LEVEL);
    }

    cFrontPannelParam.iPulseStep = (int)((cFrontPannelParam.iPulseWidthMax -
                                          cFrontPannelParam.iPulseWidthMin) / 7.);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "PulseWidth",
        &cFrontPannelParam.iFlash1PulseWidth,
        cFrontPannelParam.iFlash1PulseWidth, 100, 4000,
        "脉宽", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "Polarity",
        &cFrontPannelParam.iFlash1Polarity,
        cFrontPannelParam.iFlash1Polarity, 0, 1,
        "极性，0为负脉冲，1为正脉冲", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "Coupling",
        &cFrontPannelParam.iFlash1Coupling,
        cFrontPannelParam.iFlash1Coupling, 0, 1,
        "耦合，0为不耦合到频闪，1为耦合到频闪", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "ResistorMode",
        &cFrontPannelParam.iFlash1ResistorMode,
        cFrontPannelParam.iFlash1ResistorMode, 0, 1,
        "触发类型，0为短路触发，1为电平触发", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "PulseWidth",
        &cFrontPannelParam.iFlash2PulseWidth,
        cFrontPannelParam.iFlash2PulseWidth, 100, 4000,
        "脉宽", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "Polarity",
        &cFrontPannelParam.iFlash2Polarity,
        cFrontPannelParam.iFlash2Polarity, 0, 1,
        "极性，0为负脉冲，1为正脉冲", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "Coupling",
        &cFrontPannelParam.iFlash2Coupling,
        cFrontPannelParam.iFlash2Coupling, 0, 1,
        "耦合，0为不耦合到频闪，1为耦合到频闪", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "ResistorMode",
        &cFrontPannelParam.iFlash2ResistorMode,
        cFrontPannelParam.iFlash2ResistorMode, 0, 1,
        "触发类型，0为短路触发，1为电平触发", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "PulseWidth",
        &cFrontPannelParam.iFlash3PulseWidth,
        cFrontPannelParam.iFlash3PulseWidth, 100, 4000,
        "脉宽", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "Polarity",
        &cFrontPannelParam.iFlash3Polarity,
        cFrontPannelParam.iFlash3Polarity, 0, 1,
        "极性，0为负脉冲，1为正脉冲", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "Coupling",
        &cFrontPannelParam.iFlash3Coupling,
        cFrontPannelParam.iFlash3Coupling, 0, 1,
        "耦合，0为不耦合到频闪，1为耦合到频闪", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "ResistorMode",
        &cFrontPannelParam.iFlash3ResistorMode,
        cFrontPannelParam.iFlash3ResistorMode, 0, 1,
        "触发类型，0为短路触发，1为电平触发", "",
        CUSTOM_LEVEL);

    return 0;
}

//-------------------------------------------------------------------

int SaveAGCEnable(int iAGCEnable, BOOL fSaveNow/*=FALSE*/)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp", "AGCEnable",
        iAGCEnable,
        iAGCEnable, 0, 1,
        "AGC使能", "",
        CUSTOM_LEVEL);

    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }

    return 0;
}

int SaveAGCTh(int iAGCTh, BOOL fSaveNow/*=FALSE*/)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp", "AGCTh",
        iAGCTh,
        iAGCTh, 0, 255,
        "AGC测光基准值", "",
        CUSTOM_LEVEL);

    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }

    return 0;
}

int SaveGain(int iGain, BOOL fSaveNow/*=FALSE*/)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp", "Gain",
        iGain,
        iGain, 0, 360,
        "增益", "",
        CUSTOM_LEVEL);

    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }

    return 0;
}

int SaveGamma(int rgiDataXY[8][2], BOOL fSaveNow/*=FALSE*/)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X0",
        rgiDataXY[0][0],
        rgiDataXY[0][0], 0, 255,
        "X0", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y0",
        rgiDataXY[0][1],
        rgiDataXY[0][1], 0, 255,
        "Y0", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X1",
        rgiDataXY[1][0],
        rgiDataXY[1][0], 0, 255,
        "X1", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y1",
        rgiDataXY[1][1],
        rgiDataXY[1][1], 0, 255,
        "Y1", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X2",
        rgiDataXY[2][0],
        rgiDataXY[2][0], 0, 255,
        "X2", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y2",
        rgiDataXY[2][1],
        rgiDataXY[2][1], 0, 255,
        "Y2", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X3",
        rgiDataXY[3][0],
        rgiDataXY[3][0], 0, 255,
        "X3", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y3",
        rgiDataXY[3][1],
        rgiDataXY[3][1], 0, 255,
        "Y3", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X4",
        rgiDataXY[4][0],
        rgiDataXY[4][0], 0, 255,
        "X4", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y4",
        rgiDataXY[4][1],
        rgiDataXY[4][1], 0, 255,
        "Y4", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X5",
        rgiDataXY[5][0],
        rgiDataXY[5][0], 0, 255,
        "X5", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y5",
        rgiDataXY[5][1],
        rgiDataXY[5][1], 0, 255,
        "Y5", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X6",
        rgiDataXY[6][0],
        rgiDataXY[6][0], 0, 255,
        "X6", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y6",
        rgiDataXY[6][1],
        rgiDataXY[6][1], 0, 255,
        "Y6", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "X7",
        rgiDataXY[7][0],
        rgiDataXY[7][0], 0, 255,
        "X7", "",
        CUSTOM_LEVEL);

    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\Gamma", "Y7",
        rgiDataXY[7][1],
        rgiDataXY[7][1], 0, 255,
        "Y7", "",
        CUSTOM_LEVEL);

    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }

    return 0;
}

int SaveShutter(int iShutter, BOOL fSaveNow/*=FALSE*/)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp", "Shutter",
        iShutter,
        iShutter, 0, 54000,
        "快门", "",
        CUSTOM_LEVEL);

    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }

    return 0;
}

HRESULT SaveModuleParam(ModuleParams& cModuleParams)
{
    if ( g_pCamApp->AGCIsEnable() )
    {
        g_pCamApp->GetNowAGCShutterGain(
            cModuleParams.cCamAppParam.iShutter,
            cModuleParams.cCamAppParam.iGain
        );
    }

    if ( g_pCamApp->AWBIsEnable() )
    {
        g_pCamApp->GetNowAWBGain(
            cModuleParams.cCamAppParam.iGainR,
            cModuleParams.cCamAppParam.iGainG,
            cModuleParams.cCamAppParam.iGainB
        );
    }

    HRESULT hr = S_OK;

    hr |= g_cParamStore.SetInt("\\CamApp", "SendType", cModuleParams.cCamAppParam.iSendType);
    hr |= g_cParamStore.SetInt("\\CamApp", "JpegStream", cModuleParams.cCamAppParam.iJpegStream);
    hr |= g_cParamStore.SetInt("\\CamApp", "H264Stream", cModuleParams.cCamAppParam.iH264Stream);
    hr |= g_cParamStore.SetInt("\\CamApp", "JpegCompressRate", cModuleParams.cCamAppParam.iJpegCompressRate);
    hr |= g_cParamStore.SetInt("\\CamApp", "JpegCompressRateCapture", cModuleParams.cCamAppParam.iJpegCompressRateCapture);
    hr |= g_cParamStore.SetInt("\\CamApp", "AGCTh", cModuleParams.cCamAppParam.iAGCTh);
    hr |= g_cParamStore.SetInt("\\CamApp", "AGCShutterLOri", cModuleParams.cCamAppParam.iAGCShutterLOri);
    hr |= g_cParamStore.SetInt("\\CamApp", "AGCShutterHOri", cModuleParams.cCamAppParam.iAGCShutterHOri);
    hr |= g_cParamStore.SetInt("\\CamApp", "AGCGainLOri", cModuleParams.cCamAppParam.iAGCGainLOri);
    hr |= g_cParamStore.SetInt("\\CamApp", "AGCGainHOri", cModuleParams.cCamAppParam.iAGCGainHOri);
    hr |= g_cParamStore.SetInt("\\CamApp", "GainR", cModuleParams.cCamAppParam.iGainR);
    hr |= g_cParamStore.SetInt("\\CamApp", "GainG", cModuleParams.cCamAppParam.iGainG);
    hr |= g_cParamStore.SetInt("\\CamApp", "GainB", cModuleParams.cCamAppParam.iGainB);
    hr |= g_cParamStore.SetInt("\\CamApp", "Gain", cModuleParams.cCamAppParam.iGain);
    hr |= g_cParamStore.SetInt("\\CamApp", "Shutter", cModuleParams.cCamAppParam.iShutter);
    hr |= g_cParamStore.SetInt("\\CamApp", "CaptureGainR", cModuleParams.cCamAppParam.iCaptureGainR);
    hr |= g_cParamStore.SetInt("\\CamApp", "CaptureGainG", cModuleParams.cCamAppParam.iCaptureGainG);
    hr |= g_cParamStore.SetInt("\\CamApp", "CaptureGainB", cModuleParams.cCamAppParam.iCaptureGainB);
    hr |= g_cParamStore.SetInt("\\CamApp", "CaptureGain", cModuleParams.cCamAppParam.iCaptureGain);
    hr |= g_cParamStore.SetInt("\\CamApp", "CaptureShutter", cModuleParams.cCamAppParam.iCaptureShutter);
    hr |= g_cParamStore.SetInt("\\CamApp", "EnableCaptureGainRGB", cModuleParams.cCamAppParam.iEnableCaptureGainRGB);
    hr |= g_cParamStore.SetInt("\\CamApp", "EnableCaptureGain", cModuleParams.cCamAppParam.iEnableCaptureGain);
    hr |= g_cParamStore.SetInt("\\CamApp", "EnableCaptureShutter", cModuleParams.cCamAppParam.iEnableCaptureShutter);
    hr |= g_cParamStore.SetInt("\\CamApp", "JpegExpectSize", cModuleParams.cCamAppParam.iJpegExpectSize);
    hr |= g_cParamStore.SetInt("\\CamApp", "JpegType", cModuleParams.cCamAppParam.iJpegType);
    hr |= g_cParamStore.SetInt("\\CamApp", "AGCEnable", cModuleParams.cCamAppParam.iAGCEnable);
    hr |= g_cParamStore.SetInt("\\CamApp", "AWBEnable", cModuleParams.cCamAppParam.iAWBEnable);
    hr |= g_cParamStore.SetInt("\\CamApp", "FlashRateSynSignalEnable", cModuleParams.cCamAppParam.iFlashRateSynSignalEnable);
    hr |= g_cParamStore.SetInt("\\CamApp", "CaptureSynSignalEnable", cModuleParams.cCamAppParam.iCaptureSynSignalEnable);
    hr |= g_cParamStore.SetInt("\\CamApp", "ENetSyn", cModuleParams.cCamAppParam.iENetSyn);
    hr |= g_cParamStore.SetInt("\\CamApp", "EnableCharacter", cModuleParams.cCamAppParam.iEnableCharacterOverlap);
    hr |= g_cParamStore.SetInt("\\CamApp", "EnableFixedLight", cModuleParams.cCamAppParam.iEnableFixedLight);

    if ( S_OK == hr )
    {
        return g_cParamStore.Save();
    }
    else
    {
        return E_FAIL;
    }
}

int SaveControllPannelPulseWidthRange(
    int iPulseWidthMin,
    int iPulseWidthMax,
    BOOL fSaveNow/*=FALSE*/
)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\FrontPannel", "PulseWidthMin",
        iPulseWidthMin,
        iPulseWidthMin, 1, 72,
        "频闪脉宽最小值", "",
        CUSTOM_LEVEL);
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\FrontPannel", "PulseWidthMax",
        iPulseWidthMax,
        iPulseWidthMax, 9, 80,
        "频闪脉宽最大值", "",
        CUSTOM_LEVEL);
    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }
    return 0;
}

int SaveControllPannelFlashConfig(
    int iChannel,
    int iPolarity,
    int iTriggerType,
    int iPulseWidth,
    int iCoupling,
    BOOL fSaveNow/*=FALSE*/
)
{
    switch (iChannel)
    {
    case 0:
        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "PulseWidth",
            iPulseWidth,
            iPulseWidth, 100, 4000,
            "脉宽", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "Polarity",
            iPolarity,
            iPolarity, 0, 1,
            "极性，0为负脉冲，1为正脉冲", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "Coupling",
            iCoupling,
            iCoupling, 0, 1,
            "耦合，0为不耦合到频闪，1为耦合到频闪", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash1Info", "ResistorMode",
            iTriggerType,
            iTriggerType, 0, 1,
            "触发类型，0为短路触发，1为电平触发", "",
            CUSTOM_LEVEL);
        break;
    case 1:
        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "PulseWidth",
            iPulseWidth,
            iPulseWidth, 100, 4000,
            "脉宽", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "Polarity",
            iPolarity,
            iPolarity, 0, 1,
            "极性，0为负脉冲，1为正脉冲", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "Coupling",
            iCoupling,
            iCoupling, 0, 1,
            "耦合，0为不耦合到频闪，1为耦合到频闪", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash2Info", "ResistorMode",
            iTriggerType,
            iTriggerType, 0, 1,
            "触发类型，0为短路触发，1为电平触发", "",
            CUSTOM_LEVEL);
        break;
    case 2:
        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "PulseWidth",
            iPulseWidth,
            iPulseWidth, 100, 4000,
            "脉宽", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "Polarity",
            iPolarity,
            iPolarity, 0, 1,
            "极性，0为负脉冲，1为正脉冲", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "Coupling",
            iCoupling,
            iCoupling, 0, 1,
            "耦合，0为不耦合到频闪，1为耦合到频闪", "",
            CUSTOM_LEVEL);

        HvParamWriteInt(
            &g_cParamStore, "\\CamApp\\FrontPannel\\Flash3Info", "ResistorMode",
            iTriggerType,
            iTriggerType, 0, 1,
            "触发类型，0为短路触发，1为电平触发", "",
            CUSTOM_LEVEL);
        break;
    }
    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }
    return 0;
}

int SaveControllPannelAutoRunStatus(int iStatus, BOOL fSaveNow/*=FALSE*/)
{
    HvParamWriteInt(
        &g_cParamStore, "\\CamApp\\FrontPannel", "AutoRun",
        iStatus,
        iStatus, 0, 1,
        "控制板运行方式，0为手动运行，1为自动运行", "",
        CUSTOM_LEVEL);
    if ( fSaveNow )
    {
        g_cParamStore.Save();
    }
    return 0;
}
