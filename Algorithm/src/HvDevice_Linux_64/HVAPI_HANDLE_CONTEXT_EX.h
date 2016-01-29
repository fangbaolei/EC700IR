#ifndef _HVAPI_HANDLE_CONTEXT_EX_H_
#define _HVAPI_HANDLE_CONTEXT_EX_H_

#include "HvDeviceCommon.h"
#include "HvDeviceUtils.h"

typedef struct _PACK_RESUME_HEADER
{
	DWORD32 dwType;
	DWORD32 dwInfoLen;
	DWORD32 dwDataLen;
}PACK_RESUME_HEADER;


#define MAX_INFOR_LEN 3*1024*1024
#define MAX_DATA_LEN 10*1024*1024

#define DEFAULT_INFOR_LEN 1024*1024
#define DEFAULT_DATA_LEN  3*1024*1024

typedef enum
{
	PACK_TYPE_HEADER
	, PACK_TYPE_INFO
	, PACK_TYPE_DATA
}PACK_TYPE;

typedef struct _PACK_RESUME_CACHE
{
	PACK_RESUME_HEADER header;
	BOOL fVailHeader;

	CHAR* pInfor;
	INT nMaxInforLen;
	INT nInforLen;
	BOOL fVailInfor;

	CHAR* pData;
	INT nMaxDataLen;
	INT nDataLen;
	BOOL fVailData;

	INT nDataOffset;
	INT nInfoOffset;

	_PACK_RESUME_CACHE()
		: nInforLen(0)
		, nDataLen(0)
		, fVailData(FALSE)
		, fVailInfor(FALSE)
		, fVailHeader(FALSE)
		, pInfor(NULL)
		, nMaxInforLen(0)
		, pData(NULL)
		, nMaxDataLen(0)
		, nDataOffset(0)
		, nInfoOffset(0)
	{
		//ZeroMemory((void*)&header , sizeof(header));
		memset((void*)&header ,0,  sizeof(header));
	}

}PACK_RESUME_CACHE;



typedef struct _HVAPI_CALLBACK_SET
{
    struct _HVAPI_CALLBACK_SET *pNext;
    INT iVideoID;

    HVAPI_CALLBACK_RECORD_INFOBEGIN  pOnRecordBegin;
    PVOID pOnRecordBeginParam;

    HVAPI_CALLBACK_RECORD_INFOEND pOnRecordEnd;
    PVOID pOnRecordEndParam;

    HVAPI_CALLBACK_RECORD_PLATE pOnPlate;
    PVOID pOnPlateParam;

    HVAPI_CALLBACK_RECORD_BIGIMAGE pOnBigImage;
    PVOID pOnBigImageParam;

    HVAPI_CALLBACK_RECORD_SMALLIMAGE pOnSmallImage;
    PVOID pOnSmallImageParam;

    HVAPI_CALLBACK_RECORD_BINARYIMAGE pOnBinaryImage;
    PVOID pOnBinaryImageParam;

    HVAPI_CALLBACK_STRING pOnString;
    PVOID pOnStringParam;

    HVAPI_CALLBACK_JPEG pOnJpegFrame;
    PVOID pOnJpegFrameParam;

    HVAPI_CALLBACK_H264  pOnH264;
    PVOID pOnH264Param;

    HVAPI_CALLBACK_HISTORY_VIDEO pOnHistoryVideo;
    PVOID pOnHistoryVideoParam;

    _HVAPI_CALLBACK_SET() : pNext(NULL)
    ,pOnRecordBegin(NULL)
    ,pOnRecordBeginParam(NULL)
    ,pOnRecordEnd(NULL)
    ,pOnRecordEndParam(NULL)
    ,pOnPlate(NULL)
    ,pOnPlateParam(NULL)
    ,pOnBigImage(NULL)
    ,pOnBigImageParam(NULL)
    ,pOnSmallImage(NULL)
    ,pOnSmallImageParam(NULL)
    ,pOnBinaryImage(NULL)
    ,pOnBinaryImageParam(NULL)
    ,pOnString(NULL)
    ,pOnStringParam(NULL)
    ,pOnJpegFrame(NULL)
    ,pOnJpegFrameParam(NULL)
    ,pOnH264(NULL)
    ,pOnH264Param(NULL)
    ,pOnHistoryVideo(NULL)
    ,pOnHistoryVideoParam(NULL)
    ,iVideoID(0)
    {
    }
}HVAPI_CALLBACK_SET;

typedef struct _HVAPI_HANDLE_CONTEXT_EX
{
    DWORD dwOpenType;
    char szVersion[8];
    char szIP[16];
    char szDevSN[256];


    //record
    int sktRecord;
    char szRecordConnCmd[128];
    DWORD  dwRecordConnStatus;
    BOOL fIsRecvHistoryRecord;
    pthread_t pthreadRecvRecord;
    BOOL fIsThreadRecvRecordExit;

    //IMAGE
    int sktImage;
    char szImageConnCmd[128];
    DWORD dwImageConnStatus;
    pthread_t pthreadRecvImage;
    BOOL fIsThreadRecvImageExit;

	DWORD	dwEnhanceRedLightFlag;			//识别结果红灯加红标志
	INT		iBigPicBrightness;				//识别结果大图增亮
	INT		iBigPicHueThrshold;				//识别结果大图红灯阀值
	INT		iBigPicCompressRate;			//识别结果大图红灯加红图片压缩品质

    //VIDEO
    int sktVideo;
    char szVideoConnCmd[128];
    DWORD dwVideoConnStatus;
    BOOL fIsConnectHistoryVideo;
    pthread_t pthreadRecvVideo;
    BOOL fIsThreadRecvVideoExit;

    DWORD	dwVideoEnhanceRedLightFlag;		//历史录像红灯加红处理
	INT		iBrightness;					//历史录像增亮
	INT		iHueThrshold;					//历史录像红灯色度阀值
	INT		iCompressRate;					//历史录像红灯加红图片压缩品质

    pthread_t pthreadStatusMonitor;
    BOOL fStatusMonotorExit;


    HVAPI_CALLBACK_SET *pCallBackSet;

    DWORD dwRecordStreamTick;
    DWORD dwImageStreamTick;
    DWORD dwVideoStreamTick;

    char szRecordBeginTimeStr[14];
    char szRecordEndTimeStr[14];
    DWORD dwRecordStartIndex;
    DWORD dwHistroyRecordFlag;
    INT iRecordDataInfo;

    char szVideoBeginTimeStr[20];
    char szVideoEndTimeStr[20];

    DWORD dwRecordReconnectTimes;
    DWORD dwImageReconnectTimes;
    DWORD dwVideoReconnectTimes;

    RECT rcPlate[5];                                    //车牌坐标
    RECT rcFacePos[5][20];                        // 人脸坐标
    int nFaceCount[5];

    bool fNewProtocol;

    BOOL  fAutoLink;
    BOOL fVailPackResumeCache;
    PACK_RESUME_CACHE* pPackResumeCache;

    _HVAPI_HANDLE_CONTEXT_EX()
      :sktRecord(-1)
    ,dwRecordConnStatus(CONN_STATUS_UNKNOWN)
     ,fIsRecvHistoryRecord(false)
    ,pthreadRecvRecord(0)
    ,fIsThreadRecvRecordExit(true)
    ,sktImage(-1)
    ,dwImageConnStatus(CONN_STATUS_UNKNOWN)
    ,pthreadRecvImage(0)
     ,fIsThreadRecvImageExit(true)
    ,sktVideo(-1)
    ,dwVideoConnStatus(CONN_STATUS_UNKNOWN)
    ,fIsConnectHistoryVideo(FALSE)
    , pthreadRecvVideo(0)
    ,fIsThreadRecvVideoExit(true)
    ,pCallBackSet(NULL)
    ,dwRecordStreamTick(0)
    ,dwImageStreamTick(0)
    ,dwVideoStreamTick(0)
    ,dwRecordStartIndex(0)
    ,dwHistroyRecordFlag(0)
    ,iRecordDataInfo(-1)
    ,dwRecordReconnectTimes(0)
    ,dwImageReconnectTimes(0)
    ,dwVideoReconnectTimes(0)
   ,pthreadStatusMonitor(0)
    ,fStatusMonotorExit(false)
    ,fAutoLink(false)
    ,fVailPackResumeCache(false)
    ,pPackResumeCache(NULL)
    , dwEnhanceRedLightFlag(0)    //图片红灯加红
    , dwVideoEnhanceRedLightFlag(0)  //视频红灯加红
    {

        memset(szVersion, 0, sizeof(szVersion));
        memset(szIP, 0, sizeof(szIP));
        memset(szVideoConnCmd, 0, 128);
        memset(szRecordConnCmd, 0, 128);
        memset( szImageConnCmd, 0, 128);

        memset(  szRecordBeginTimeStr, 0, 14);
        memset( szRecordEndTimeStr, 0, 14);

        memset(  szVideoBeginTimeStr, 0, 20);
        memset( szVideoEndTimeStr, 0, 20);

        memset(szDevSN, 0, 256);

        memset(rcFacePos, 0, sizeof(rcFacePos));

    }

}HVAPI_HANDLE_CONTEXT_EX;


typedef struct ThreadMutex
{
    ThreadMutex()
    {
        pthread_mutex_init(&mtx, NULL);
    }
    ~ThreadMutex()
    {
        pthread_mutex_destroy(&mtx);
    }
    inline void lock()
    {

        pthread_mutex_lock(&mtx);
       //    printf("Enter %d\n", ++i);
    }
    inline void unlock()
    {
        pthread_mutex_unlock(&mtx);
     //     printf("Leave %d\n", ++t);
    }
    pthread_mutex_t mtx;
 //   int i;
 //   int t;
}ThreadMutex;



#endif // _HVAPI_HANDLE_CONTEXT_EX_H_
