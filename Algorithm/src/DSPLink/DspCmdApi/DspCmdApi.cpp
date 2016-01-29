#include "DspCmdApi.h"
#include "DspLinkCmd.h"
#include "hvmodelstore.h"
#include "tracker.h"
#include "TiImageProcesser.h"

ITracker* g_pTrackerPhoto = NULL;
ITracker* g_pTrackerVideo = NULL;
CHvModelStore g_modelStore;
int g_nEddyType = 0;

#define REF_IMAGE_CBYCRY_WIDTH 2448
#define REF_IMAGE_CBYCRY_HEIGHT 2048
IReferenceComponentImage* g_pRefImageCbYCrY = NULL;
IReferenceComponentImage* g_pRefImageYUV422 = NULL;

extern HRESULT JPG2YUV(HV_COMPONENT_IMAGE imgJpeg, HV_COMPONENT_IMAGE& imgYuv);
extern HRESULT JPG2YUV_TI(HV_COMPONENT_IMAGE imgJpeg, HV_COMPONENT_IMAGE& imgYuv);
extern HRESULT YUV2JPG_TI(DWORD32 dwCompressRate, DWORD32 dwJpegType, JPEG_ENCODE_DATA *pData);
extern "C" void CbYCrY_1600_1200_Rotate90(const PBYTE8 pbSrc, PBYTE8 pbDst);
extern "C" int g_iErrCode;
char g_szTemp[] = "";
void ImageAddRef(IMG_MEM_OPER_LOG* pLog, IReferenceComponentImage* pRefImage)
{
	if ( pLog && pRefImage )
	{
		pLog->rgOperInfo[pLog->nCount].pMemAddr = pRefImage;
		pLog->rgOperInfo[pLog->nCount++].nOperFlag = IOF_ADD_REF;	
	}
}

void ImageRelease(IMG_MEM_OPER_LOG* pLog, IReferenceComponentImage* pRefImage)
{
	if ( pLog && pRefImage )
	{
		pLog->rgOperInfo[pLog->nCount].pMemAddr = pRefImage;
		pLog->rgOperInfo[pLog->nCount++].nOperFlag = IOF_RELEASE;
	}
}

int InitDspCmdLib_Photo(int nPlateRecogParamIndex, void* pvParam)
{
	// 设置全局参数
	LoadPlateRecogParam(nPlateRecogParamIndex);
	if (NULL == g_pTrackerPhoto)
	{
		CreateTracker_Photo(&g_pTrackerPhoto);
		if (g_pTrackerPhoto)
		{
			using namespace HvCore;
			IHvModel* pHvModel;
			g_modelStore.QueryInterface(IID_IHvModel, (void**)&pHvModel);
			g_pTrackerPhoto->SetHvModel(&g_modelStore);
			g_pTrackerPhoto->InitTracker((TRACKER_CFG_PARAM*)pvParam);
		}
		return (g_pTrackerPhoto != NULL ? 0 : -1);
	}
	else
	{
		return 1;
	}
}

int InitDspCmdLib_Video(int nPlateRecogParamIndex, int nLastLightType, int nLastPulseLevel,int nLastCplStatus, void* pvParam)
{
	// 设置全局参数
	LoadPlateRecogParam(nPlateRecogParamIndex);
	if (NULL == g_pTrackerVideo)
	{
		CreateTracker_Video(&g_pTrackerVideo);
		if (g_pTrackerVideo)
		{
			using namespace HvCore;
			IHvModel* pHvModel;
			g_modelStore.QueryInterface(IID_IHvModel, (void**)&pHvModel);
			g_pTrackerVideo->SetHvModel(&g_modelStore);
			g_pTrackerVideo->InitTracker((TRACKER_CFG_PARAM*)pvParam);
			g_pTrackerVideo->SetFirstLightType((LIGHT_TYPE)nLastLightType);
			g_pTrackerVideo->SetFirstPulseLevel(nLastPulseLevel);
			g_pTrackerVideo->SetFirstCplStatus(nLastCplStatus);
		}
		return (g_pTrackerVideo != NULL ? 0 : -1);
	}
	else
	{
		return 1;
	}
}

void ProcessDspCmd(unsigned char *pbInbuf, unsigned int nInlen, unsigned char *pbOutbuf, unsigned int nOutlen)
{
 	// 清空调试信息
	g_szDebugInfo[0] = 0;
	DSP_CMD_ID nCmdId = DSP_CMD_ID(*((DWORD32*)pbInbuf));
	if (sizeof(DSP_RESPOND_HEAD) > nOutlen)
	{
		HV_Trace(1, "\nDSPLink outbuf outof memery!\n");
		return;
	}
	// 清空输出内存
	memset(pbOutbuf, 0, nOutlen);
	DSP_RESPOND_HEAD* pRespondHead = (DSP_RESPOND_HEAD*)pbOutbuf;
	pRespondHead->dwCmdId = nCmdId;
	pRespondHead->iResult = -1;
	pRespondHead->iAppendDataSize = 0;

	switch (nCmdId) 
	{
	case DCI_IMAGE_EDDY_TYPE:
		{
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			// 判断参数长度是否正确
			if (*pnParamSize == sizeof(IMAGE_EDDYTYPE_PARAM))
			{
				pbPos += 4;
				IMAGE_EDDYTYPE_PARAM* pParam = (IMAGE_EDDYTYPE_PARAM*)pbPos;
				g_nEddyType = pParam->nEddyType;
				pRespondHead->iResult = S_OK;
			}
			else
			{
				// 输出错误信息
				sprintf(
					pRespondHead->szErrorInfo, 
					"Set image eddy type failed, the size of param error!!!\n"
					);
			}
		}
		break;

	case DCI_RECTIFY_TIME:
		{
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			// 判断参数长度是否正确
			if (*pnParamSize == sizeof(RECTIFY_TIME_PARAM))
			{
				pbPos += 4;
				RECTIFY_TIME_PARAM* pParam = (RECTIFY_TIME_PARAM*)pbPos;
				RectifySystemTick(pParam->dwArmCurTime);
				pRespondHead->iResult = S_OK;
			}
			else
			{
				// 输出错误信息
				sprintf(
					pRespondHead->szErrorInfo, 
					"Rectify time failed, the size of param error!!!\n"
					);
			}
		}
		break;

	case DCI_INIT_PHOTO_RECOGER:
		{
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			// 判断参数长度是否正确
			if (*pnParamSize == sizeof(INIT_PHOTO_RECOGER_PARAM))
			{
				pbPos += 4;
				INIT_PHOTO_RECOGER_PARAM* pParam = (INIT_PHOTO_RECOGER_PARAM*)pbPos;
				pRespondHead->iResult = InitDspCmdLib_Photo(pParam->nPlateRecogParamIndex, &(pParam->cTrackerCfgParam));
				
				if (pRespondHead->iResult == S_OK)
				{
					pRespondHead->iResult = CreateReferenceComponentImage(
						&g_pRefImageCbYCrY,
						HV_IMAGE_CbYCrY,
						REF_IMAGE_CBYCRY_WIDTH, REF_IMAGE_CBYCRY_HEIGHT,
						0, 0, 0, 
						"NULL"
						);

					if ( pRespondHead->iResult != S_OK )
					{
						// 输出错误信息
						sprintf(
							pRespondHead->szErrorInfo, 
							"Create g_pRefImageCbYCrY is failed!!!\n"
							);
					}
				}
				else
				{
					// 输出错误信息
					sprintf(
						pRespondHead->szErrorInfo, 
						"InitDspCmdLib_Photo is failed!!!\n"
						);
				}
				
			}
			else
			{
				// 输出错误信息
				sprintf(
					pRespondHead->szErrorInfo, 
					"Init photo recoger failed, the size of param error!!!\n"
					);
			}
		}
		break;

	case DCI_PROCESS_PHOTO:
		{
			DWORD32 dwDecodeTime = 0;
			DWORD32 dwRecogTime = 0;
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			pbPos += 4;
			// 判断参数长度是否正确
			PROCESS_PHOTO_PARAM* pParam = (PROCESS_PHOTO_PARAM*)pbPos;
			pbPos += *pnParamSize;
			int* pnDataSize = (int*)pbPos;
			pbPos += 4;
			PROCESS_PHOTO_DATA* pData = (PROCESS_PHOTO_DATA*)pbPos;
			if (*pnParamSize == sizeof(PROCESS_PHOTO_PARAM) && *pnDataSize == sizeof(PROCESS_PHOTO_DATA))
			{
				PROCESS_PHOTO_RESPOND* pProcessRespond = (PROCESS_PHOTO_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				pRespondHead->iAppendDataSize = sizeof(PROCESS_PHOTO_RESPOND);
				DWORD32 dwCurTick = GetSystemTick();
				pRespondHead->iResult = E_FAIL;

				if (pData->hvImage.nImgType == HV_IMAGE_JPEG)
				{
					pRespondHead->iResult = JPG2YUV(pData->hvImage, pData->hvImageYuv);
				}
				else if (pData->hvImage.nImgType == HV_IMAGE_BT1120)
				{
					/*
					YUV422ToCbYCrY_1600_1200_Fast(
						GetHvImageData(&pData->hvImage, 0), 
						GetHvImageData(&pData->hvImage, 1), 
						GetHvImageData(&pData->hvImageYuv, 0)
						);
					pData->hvImageYuv.iStrideWidth[0] = (pData->hvImageYuv.iWidth << 2);
					pData->hvImageYuv.iHeight >>= 1;
					pRespondHead->iResult = S_OK;
					*/

					pRespondHead->iResult = E_FAIL;
				}
				else
				{
					pRespondHead->iResult = S_FALSE;
				}

				dwDecodeTime = GetSystemTick() - dwCurTick;
				dwCurTick = GetSystemTick();

				if (pRespondHead->iResult == S_OK && g_pTrackerPhoto)
				{
					pRespondHead->iResult = g_pTrackerPhoto->ProcessPhoto(pData->hvImageYuv, &(pParam->prPram), pData, pProcessRespond);
				}
				dwRecogTime = GetSystemTick() - dwCurTick;
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}
			if (S_OK == pRespondHead->iResult && !strlen(g_szDebugInfo))
			{
				// 输出处理时间
				sprintf(
					pRespondHead->szErrorInfo, 
					"Decode time:%d\nRecog time:%d\n",
					dwDecodeTime,
					dwRecogTime
					);
			}
			else
			{
				// 输出错误信息
				memcpy(pRespondHead->szErrorInfo, g_szDebugInfo, 256);
			}
			pRespondHead->szErrorInfo[255] = 0;
		}
		break;

	case DCI_JPEG_ENCODE:
		{
			// zhaopy
			if( g_pTrackerVideo != NULL )
			{
				g_pTrackerVideo->CheckEvent();
			}

			DWORD32 dwEncodeTime = 0;
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			pbPos += 4;
			JPEG_ENCODE_PARAM* pParam = (JPEG_ENCODE_PARAM*)pbPos;
			pbPos += sizeof(JPEG_ENCODE_PARAM);
			int* pnDataSize = (int*)pbPos;
			pbPos += 4;
			JPEG_ENCODE_DATA* pData = (JPEG_ENCODE_DATA*)pbPos;

			if (*pnParamSize == sizeof(JPEG_ENCODE_PARAM) && *pnDataSize == sizeof(JPEG_ENCODE_DATA))
			{
				JPEG_ENCODE_RESPOND* pJpegEncodeRespond = (JPEG_ENCODE_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				pRespondHead->iAppendDataSize = sizeof(JPEG_ENCODE_RESPOND);

				DWORD32 dwCurTick = GetSystemTick();
				pRespondHead->iResult = YUV2JPG_TI(pParam->dwCompressRate, pParam->dwJpegDataType, pData);
				dwEncodeTime = GetSystemTick() - dwCurTick;

				pJpegEncodeRespond->dwJpegLen = pData->hvImageJpg.iWidth;
				if (S_OK == pRespondHead->iResult)
				{
					// 输出处理时间
					sprintf(
						pRespondHead->szErrorInfo, 
						"Encode time:%d\n", dwEncodeTime
						);
				}
				else
				{
					// 输出错误信息
					sprintf(pRespondHead->szErrorInfo, "Encode JPEG Error! Error Code:0x%08x\n", g_iErrCode);
				}
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}
			
			pRespondHead->szErrorInfo[255] = 0;
		}

		// zhaopy
		if( g_pTrackerVideo != NULL )
		{
			g_pTrackerVideo->CheckEvent();
		}
		break;

	case DCI_INIT_VIDEO_RECOGER:
		{
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			// 判断参数长度是否正确
			if (*pnParamSize == sizeof(INIT_VIDEO_RECOGER_PARAM))
			{
				pbPos += 4;
				INIT_VIDEO_RECOGER_PARAM* pParam = (INIT_VIDEO_RECOGER_PARAM*)pbPos;
				RectifySystemTick(pParam->dwArmTick);
				pRespondHead->iResult = InitDspCmdLib_Video(pParam->nPlateRecogParamIndex,
					pParam->nLastLightType,
					pParam->nLastPulseLevel,
					pParam->nLastCplStatus,
					&(pParam->cTrackerCfgParam)
					);
				if (pRespondHead->iResult == S_OK)
				{
					pRespondHead->iResult = CreateReferenceComponentImage(
						&g_pRefImageCbYCrY,
						HV_IMAGE_CbYCrY,
						REF_IMAGE_CBYCRY_WIDTH, REF_IMAGE_CBYCRY_HEIGHT,
						0, 0, 0, 
						"NULL"
						);

					if ( pRespondHead->iResult != S_OK )
					{
						// 输出错误信息
						sprintf(
							pRespondHead->szErrorInfo, 
							"Create g_pRefImageCbYCrY is failed!!!\n"
							);
					}
					//电子警察暂时不用二次解压,黄国超备注,2012-03-02
					if(GetCurrentParam()->g_PlateRcogMode != PRM_ELECTRONIC_POLICE)
					{
						pRespondHead->iResult = CreateReferenceComponentImage(
							&g_pRefImageYUV422,
							HV_IMAGE_YUV_422,
							1600, 600,
							0, 0, 0, 
							"NULL"
						);
					}
				}
				else
				{
					// 输出错误信息
					sprintf(
						pRespondHead->szErrorInfo, 
						"InitDspCmdLib_Video is failed!!!\n"
						);
				}
			}
			else
			{
				// 输出错误信息
				sprintf(
					pRespondHead->szErrorInfo, 
					"Init video recoger failed, the size of param error!!!\n"
					);
			}
		}
		break;

	case DCI_PROCESS_ONE_FRAME:
		{
			DWORD32 dwDecodeTime = 0;
			DWORD32 dwRecogTime = 0;
			unsigned char* pbPos = pbInbuf + 4;
			int* pnParamSize = (int*)pbPos;
			pbPos += 4;
			// 判断参数长度是否正确
			PROCESS_ONE_FRAME_PARAM* pParam = (PROCESS_ONE_FRAME_PARAM*)pbPos;
			RectifySystemTick(pParam->dwCurTick);
			pbPos += *pnParamSize;
			int* pnDataSize = (int*)pbPos;
			pbPos += 4;
			PROCESS_ONE_FRAME_DATA* pData = (PROCESS_ONE_FRAME_DATA*)pbPos;
			if (*pnParamSize == sizeof(PROCESS_ONE_FRAME_PARAM) && *pnDataSize == sizeof(PROCESS_ONE_FRAME_DATA))
			{
				PROCESS_ONE_FRAME_RESPOND* pProcessRespond = (PROCESS_ONE_FRAME_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				memset(pProcessRespond, 0, sizeof(PROCESS_ONE_FRAME_RESPOND));
				pRespondHead->iAppendDataSize = sizeof(PROCESS_ONE_FRAME_RESPOND);
				DWORD32 dwCurTick = GetSystemTick();
				pRespondHead->iResult = E_FAIL;

				if (pData->hvImage.nImgType == HV_IMAGE_JPEG)
				{
					if (g_nEddyType == 0)
					{
						// 由于GBA解码库解码300W图片快，而对200W图片解码慢
						// 因此300W图片使用GBA解码库，其它用TI解码库
						// huanggr 2011-12-01
						if (pData->hvImageYuv.nImgType == HV_IMAGE_CbYCrY)
						{
							pRespondHead->iResult = JPG2YUV_TI(pData->hvImage, pData->hvImageYuv);
							// 如解码失败，并且为200w图片，尝试用gba库再解码一次
							if (pRespondHead->iResult != S_OK /*&& pData->hvImage.iWidth <= 1600*/)
							{
								HV_COMPONENT_IMAGE imgYUV422;
								if (g_pRefImageYUV422 && g_pRefImageYUV422->GetImage(&imgYUV422) == S_OK)
								{
									pRespondHead->iResult = JPG2YUV(pData->hvImage, imgYUV422);
									pData->hvImageYuv = imgYUV422;
								}
							}
						}
						else
						{
							pRespondHead->iResult = JPG2YUV(pData->hvImage, pData->hvImageYuv);
						}
						if ( pRespondHead->iResult != S_OK )
						{
							sprintf(g_szDebugInfo, "JPG2YUV_TI is Error. ErrCode = 0x%08x\n", g_iErrCode);
						}
					}
					else
					{
						// TODO：更换GBA解码库后需支持旋转
						if ( 1600 == pData->hvImage.iWidth && 1200 == pData->hvImage.iHeight )
						{
							//Comment by Shaorg: 目前仅支持1600*1200规格的Jpeg图片的旋转。
							HV_COMPONENT_IMAGE imgCbYCrY;
							if (g_pRefImageCbYCrY->GetImage(&imgCbYCrY) == S_OK)
							{
								if ( S_OK == JPG2YUV_TI(pData->hvImage, imgCbYCrY) )
								{
									CbYCrY_1600_1200_Rotate90(
										GetHvImageData(&imgCbYCrY, 0), 
										GetHvImageData(&pData->hvImageYuv, 0)
									);

									pRespondHead->iResult = S_OK;
								}
							}
							if(pRespondHead->iResult == S_OK)
							{
								// 图片抽场
								pData->hvImageYuv.iStrideWidth[0] <<= 1;
								pData->hvImageYuv.iHeight >>= 1;
							}
						}
					}
				}
				else if (pData->hvImage.nImgType == HV_IMAGE_BT1120)
				{
					// Comment by Shaorg: 2048*1024为500w前端已经抽场的图片的规格。
					if ( pData->hvImage.iWidth != 2048 && pData->hvImage.iHeight != 1024 )
					{
						// 图片抽场
						pData->hvImageYuv.iStrideWidth[0] = (pData->hvImageYuv.iStrideWidth[0] << 1);
						pData->hvImageYuv.iStrideWidth[1] = (pData->hvImageYuv.iStrideWidth[1] << 1);
						pData->hvImageYuv.iHeight >>= 1;
					}
					pRespondHead->iResult = S_OK;
				}
				else if (pData->hvImage.nImgType == HV_IMAGE_BT1120_ROTATE_Y)
				{
					pData->hvImageYuv.iHeight >>= 1;
					pRespondHead->iResult = S_OK;
				}
				else if (pData->hvImage.nImgType == HV_IMAGE_BT1120_FIELD)
				{
					pRespondHead->iResult = S_OK;
				}
				else
				{
					pRespondHead->iResult = S_FALSE;
				}

				dwDecodeTime = GetSystemTick() - dwCurTick;
				dwCurTick = GetSystemTick();
				HRESULT hrTemp = pRespondHead->iResult;
				if (pRespondHead->iResult == S_OK && g_pTrackerVideo)
				{
					pRespondHead->iResult = g_pTrackerVideo->ProcessOneFrame(pParam, pData, pProcessRespond);
				}
				dwRecogTime = GetSystemTick() - dwCurTick;
				//sprintf(g_szDebugInfo, "before hr = 0x%x, after hr = 0x%x", hrTemp, pRespondHead->iResult);
				if (S_OK == pRespondHead->iResult && !strlen(g_szDebugInfo))
				{
					// 输出处理时间
					sprintf(
						pRespondHead->szErrorInfo, 
						"Decode time:%d\nRecog time:%d\n",
						dwDecodeTime,
						dwRecogTime
						);
				}
				else
				{
					// 输出错误信息
					memcpy(pRespondHead->szErrorInfo, g_szDebugInfo, 256);
				}
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}

			pRespondHead->szErrorInfo[255] = 0;
		}
		break;

	default:
		pRespondHead->dwCmdId = DCI_UNKNOWN;
	}
}
