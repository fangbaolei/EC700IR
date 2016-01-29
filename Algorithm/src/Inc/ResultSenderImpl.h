/**
* @file		ResultSenderImpl.h
* @version	1.0
* @brief	结果发送模块实现
*/

#ifndef _ResultSenderImpl_h_
#define _ResultSenderImpl_h_

#include "IResultSender.h"
#include "DataLinkImpl.h"
#include "sendnetdata.h"
#include "HvResultFilter.h"
#include "ImgGatherer.h"
#include "AutoLinkParam.h"

#ifndef MAX_STREETINFO_LENGTH
#define MAX_STREETINFO_LENGTH 255
#endif

/**
* @brief 结果发送模块参数结构体
*/
typedef struct _ResultSenderParam
{
    //结果后处理模块的相关参数
    PROCESS_RULE cProcRule;     /**< 后处理规则 */

    BOOL fInitHdd;              /**< 是否初始化硬盘 */

    //可靠性保存模块
    BOOL fIsSafeSaver;            /**< 是否启动安全存储 */
    char szSafeSaverInitStr[64];  /**< 安全储存器初始化参数 */  //暂时不用，预留
    int  iRecordFreeSpacePerDisk;
    int  iVideoFreeSpacePerDisk;

    //发送视频相关参数
    int iSaveVideo;         /**< 保存视频数据 */
    int iVideoDisplayTime;  /**< 发送视频时间间隔 */
    int iDrawRect;          /**< 发送红框标识 */
    int iEddyType;          /**< 图片旋转 */

    //附加信息模块参数
    BOOL fOutputAppendInfo;     //输出车牌附加信息开关
    BOOL fOutputObservedFrames; //输出有效帧数开关
    BOOL fOutputCarArriveTime;  //输出检测时间开关
    BOOL fOutputFilterInfo; //输出后处理信息
    char szStreetName[MAX_STREETINFO_LENGTH];       //路口名称
    char szStreetDirection[MAX_STREETINFO_LENGTH];  //路口方向

    int iOutputOnlyPeccancy;//1:只输出违章车辆 2:只输出非违章结果 3:接收历史数据时,非违章输出文本,违章输出文本和图片
    int iSendRecordSpace; //结果发送间隔(*100ms)
    int iSendHisVideoSpace;//历史录像发送间隔(*100ms)

    int iSpeedLimit;    //速度上限值(km/h)

    int iCheckEventTime;    //事件检测周期(单位:分钟)

    int iSaveType;    //结果保存方式
    int iWidth;         //截图宽度
    int iHeight;        //截图高度
    int iBestSnapshotOutput;    //主视频最清晰大图输出
    int iLastSnapshotOutput;    //主视频最后大图输出
    int iOutputCaptureImage;    //非违章车辆输出三张抓拍图

    int iFilterUnSurePeccancy;  //过滤不合格的违章记录
    int nOutputPeccancyType;    //输出违章类型方式
    char szPeccancyPriority[256]; //违章类型优先级

    float fltCarAGCLinkage;  //车流量与AGC联动开关，0为关，非0表示单位时间车流量少于该值时启用AGC，大于则关闭
    int iAWBLinkage;        //环境亮度与AWB联动开关
    int nRedPosBrightness;

    int iEnablePanoramicCamera;     //全景相机使能开关
    char szPanoramicCameraIP[20];   //全景相机IP

    CAP_CAM_PARAM cCapCamParam; //抓拍相机参数
    AUTO_LINK_PARAM cAutoLinkParam; //主动连接参数

    BOOL fMTUSetEnable;

    //“ini文件发送”命令使用
    //int nExportParamRank;   /**< 输出参数等级 */
    //BOOL nExportRankInfo;   /**< 输出等级信息 */

	int iCapMode;  //抓拍模式 (0:抓拍一次; 1:超速违章抓拍2次; 2:全部抓拍2次)
	int iFlashDifferentLane;  //闪光灯分车道闪

    _ResultSenderParam()
    {
        iBestSnapshotOutput = 1;
        iLastSnapshotOutput = 1;
        iOutputCaptureImage = 0;
#ifndef SINGLE_BOARD_PLATFORM
        fIsSafeSaver = 0;
#else
		fIsSafeSaver = 1;
#endif
        iRecordFreeSpacePerDisk = 10;
        iVideoFreeSpacePerDisk = 10;
        iVideoDisplayTime = 500;
        iDrawRect = 1;
        iEddyType = 0;
        iSpeedLimit = 1000;
        iSaveType = 0;
        iWidth = 720;
        iHeight = 288;
        iCheckEventTime = 1;
        iFilterUnSurePeccancy = 1;
        fOutputAppendInfo = TRUE;
        fOutputObservedFrames = TRUE;
        fOutputCarArriveTime = FALSE;
        strcpy(szSafeSaverInitStr, "NULL");
        strcpy(szStreetName, "NULL");
        strcpy(szStreetDirection, "NULL");
        iOutputOnlyPeccancy = 0;
        iSendRecordSpace = 5;
        iSendHisVideoSpace = 1;
        nOutputPeccancyType = 0;
        fltCarAGCLinkage = 0.0f;
        iAWBLinkage = 0;
        fOutputFilterInfo = 0;
        fInitHdd = FALSE;
        fMTUSetEnable = FALSE;
        strcpy(szPeccancyPriority, "逆行-非机动车道-闯红灯-不按车道行驶-压线-超速");
		iCapMode = 0;
		iFlashDifferentLane = 0;
    };

} ResultSenderParam;

class CResultSender : public IResultSender
{
public:
    CResultSender();
    ~CResultSender();

public:
    HRESULT Init(
        const ResultSenderParam& cParam,
        CDataLinkCtrlThread* pcDataLinkCtrlThread
    );

public:
    HRESULT PutResult(
        LPCSTR szResultInfo,
        LPVOID lpcData
    );

    HRESULT PutVideo(
        DWORD32* pdwSendCount,
        LPVOID lpFrameData,
        int nRectCount = 0,
        HV_RECT *pRect = NULL
    );

    HRESULT PutString(
        WORD16 wVideoID,
        WORD16 wStreamID,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        const char *pString
    );

    HRESULT PutCarEnterLeaveFlag(
        DWORD32 dwVideoID,
        WORD16 wInLeftFlag,
        DWORD32 dwTimeMsLow,
        DWORD32 dwTimeMsHigh
    );

    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);

protected:
    bool fInited;
    CDataLinkCtrlThread* m_pcDataLinkCtrlThread;
    CHvResultFilter m_resultFilter;

public:
    //安全存储器
    CSafeSaverDm6467Impl* m_pcSafeSaver;
    ISafeSaver* m_pSafeSaver;

public:
    static ResultSenderParam m_cParam;  /**< 结果发送模块总参数 */
};

#endif
