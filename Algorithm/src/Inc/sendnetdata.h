/**
* @file		sendnetdata.h
* @version	1.0
* @brief	结果发送API
*/

#ifndef _SENDNETDATA_H_
#define _SENDNETDATA_H_

#include "hvutils.h"
#include "swimageobj.h"
#include "trackerdef.h"
#include "resultsend.h"

#include "safesaver.h"

bool GetHVIOSmallImg(
    HVIO_SmallImage* pcSmallImage,
    const HV_COMPONENT_IMAGE &image
);

bool GetHVIOBinaryImg(
    HVIO_SmallImage* pcBinaryImg,
    const HV_COMPONENT_IMAGE &image
);

bool GetHVIOBigImg(
    HVIO_BigImage* pcBigImg,
    const HV_COMPONENT_IMAGE &image,
    unsigned char* pbOffsetInfo = NULL,
    WORD16 wOffsetSize = 0
);

int SendInfoBegin( IResultIO* pResultIO, DWORD32 dwVideoID );

int SendInfoEnd( IResultIO* pResultIO, DWORD32 dwVideoID );

int SendCarEnterLeaveFlag(
    IResultIO* pResultIO,
    DWORD32 dwVideoID,
    WORD16 wInLeftFlag,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
    );

int SendResultText(
    IResultIO* pResultIO,
    DWORD32 dwVideoID,
    const char *pResult,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
    );

int SendBigImg(
    IResultIO* pResultIO,
    IReferenceComponentImage *pBigImg,
    WORD16 wImageID,
    DWORD32 dwVideoID,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

int SendSmallImg(
    IResultIO* pResultIO,
    IReferenceComponentImage *pSmallImg,
    DWORD32 dwVideoID,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

int SendBinaryImg(
    IResultIO* pResultIO,
    IReferenceComponentImage *pBinImg,
    DWORD32 dwVideoID,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

// 发送视频
HRESULT SendVideo(
    IResultIO* pResultIO,
	DWORD32 dwVideoID,
	BYTE8* pImgData,
	DWORD32 dwImgSize,
	DWORD32* pdwSendCount,
	DWORD32 dwImgTime,
	int nRectCount = 0,
	HV_RECT *pRect = NULL
);

//发送识别结果
int SendResult(
    IResultIO* pResultIO,
    const char *pPlateStr,
    RESULT_IMAGE_STRUCT *pResultImage,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

//发送字符串
HRESULT SendString(
    IResultIO* pResultIO,
    WORD16 wVideoID,
    WORD16 wStreamID,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    const char *pString
);

//安全发送结果（即：在发送之前先缓存结果，防止发送意外失败而导致的结果丢失）
int SafeSendResult(
    IResultIO* pResultIO,
    ISafeSaver* pSafeSaver,
    const char *pPlateStr,
    RESULT_IMAGE_STRUCT *pResultImage,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

/* SaveResultToHardDisk.cpp */
// 保存结果到本地硬盘上
extern HRESULT SaveResultToHardDisk(
    ISafeSaver* pSafeSaver,
    const char* pPlateStr,
    PBYTE8 pbRecord,
    DWORD32 dwRecordSize,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

#endif
