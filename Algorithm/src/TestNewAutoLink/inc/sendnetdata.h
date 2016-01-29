#ifndef _SENDNETDATA_H_
#define _SENDNETDATA_H_

#include "swImageObj.h"
#include "TrackerCallback.h"
#include "resultsend.h"

#define MAX_PLATE_POS_COUNT 5

void InitProtocol();

extern DWORD32 g_dwNowCarID;

int SendInfoBegin( DWORD32 dwID );
int SendInfoEnd( DWORD32 dwID );
int SendCarEnterLeaveFlag( DWORD32 dwID, WORD16 fEnterLeave, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh );
int SendResultText( DWORD32 dwID, char *pResult, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh );
int SendAppendInfo( DWORD32 dwID, char *pAppendInfo, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh );

int SendBigImg( DWORD32 dwID, IReferenceComponentImage *pBigImg, WORD16 wImageID,
					DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh, const HiVideo::CRect *prcPlate, BOOL fVScale = FALSE, 			  
					UINT nRedLightCount = 0,
					HV_RECT* rgRedLightPos = NULL);

int SendSmallImg( DWORD32 dwID,IReferenceComponentImage *pSmallImg, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh );
int SendBinaryImg( DWORD32 dwID, IReferenceComponentImage *pBinImg, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh );
int SendBin( DWORD32 dwID, WORD16 wStreamID, WORD16 wBinSize, const unsigned char *pBuffer);

void ProcessStdCommPtl();

bool GetHVIOSmallImg(HVIO_SmallImage* pcSmallImage, const HV_COMPONENT_IMAGE &image);
bool GetHVIOBinaryImg(HVIO_SmallImage* pcBinaryImg, const HV_COMPONENT_IMAGE &image);
bool GetHVIOBigImg( HVIO_BigImage* pcBigImg, const HV_COMPONENT_IMAGE &image, unsigned char* pbOffsetInfo = NULL, WORD16 wOffsetSize = 0 );

//发送检测结果
void SendResult(
				char *pPlateStr,
				char *pAppendInfo,
				RESULT_IMAGE_STRUCT *pResultImage,
				DWORD32 dwTimeMsLow,
				DWORD32 dwTimeMsHigh,
				const HiVideo::CRect *prcBestPlate,
				const HiVideo::CRect *prcLastPlate,
				UINT nRedLightCount = 0,
				HV_RECT* rgRedLightPos = NULL,
				bool fIsPeccecy = false,
				const HiVideo::CRect * prcSecondPos = NULL
				);

//保存结果
bool SafeSave(
			  char *pPlateStr,
			  char *pAppendInfo,
			  RESULT_IMAGE_STRUCT *pResultImage,
			  DWORD32 dwTimeMsLow,
			  DWORD32 dwTimeMsHigh,
			  const HiVideo::CRect *prcBestPlate,
			  const HiVideo::CRect *prcLastPlate,
			  UINT nRedLightCount = 0,
			  HV_RECT* rgRedLightPos = NULL,
			  bool fIsPeccecy = false,
			  const HiVideo::CRect * prcSecondPos = NULL
			  );

//发送可靠性结果
HRESULT SafeSendResult(
					   char *pPlateStr,
					   char *pAppendInfo,
					   CARLEFT_INFO_STRUCT *pCarLeftInfo,
					   UINT nRedLightCount = 0,   //红灯的数量
					   HV_RECT* rgRedLightPos = NULL	//红灯的坐标(用于红灯的加红)
					   );
#endif
