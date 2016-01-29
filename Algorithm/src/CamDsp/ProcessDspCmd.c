#include <stdio.h>
#include <string.h>
#include "CamDsp.h"
#include "StringOverlay.h"

int g_iErrCode = 0;  // 全局错误信息码
BOOL g_fIsInitGammaTable = FALSE;
unsigned char g_rgGammaValue[256];

void GammaProcess(HV_COMPONENT_IMAGE *phvImageYuv, H264_ENCODE_PARAM *pParam)
{
	int i = 0;
	int nYLen = 0;
	unsigned char *pbYData = NULL;
	if (g_fIsInitGammaTable == FALSE)
	{
		float fltRatio1 = (float)pParam->iAdjustPointY / pParam->iAdjustPointX;
		float fltRatio2 = (float)(255 - pParam->iAdjustPointY) / (255 - pParam->iAdjustPointX);
		for (i = 0; i <= pParam->iAdjustPointX; ++i)
		{
			g_rgGammaValue[i] = (unsigned char)(i * fltRatio1);
		}
		for (i = pParam->iAdjustPointX + 1; i < 256; ++i)
		{
			g_rgGammaValue[i] = (unsigned char)((i - pParam->iAdjustPointX) * fltRatio2 + pParam->iAdjustPointY);
		}
		g_fIsInitGammaTable = TRUE;
	}

	nYLen = phvImageYuv->iHeight * phvImageYuv->iWidth;
	pbYData = (unsigned char*)phvImageYuv->rgImageData[0].phys;
	for (i = 0; i < nYLen; ++i)
	{
		pbYData[i] = g_rgGammaValue[pbYData[i]];
	}
}

void ProcessDspCmd(unsigned char *pbInbuf, unsigned int nInlen, unsigned char *pbOutbuf, unsigned int nOutlen)
{
	DSP_RESPOND_HEAD* pRespondHead = NULL;

	DSP_CMD_ID nCmdId = (DSP_CMD_ID)(*((DWORD32*)pbInbuf));

	if (sizeof(DSP_RESPOND_HEAD) > nOutlen)
	{
		return;
	}

	g_iErrCode = 0;
	memset(pbOutbuf, 0, nOutlen);  // 清空输出内存

	pRespondHead = (DSP_RESPOND_HEAD*)pbOutbuf;
	pRespondHead->dwCmdId = nCmdId;
	pRespondHead->iResult = -1;
	pRespondHead->iAppendDataSize = 0;

	switch (nCmdId)
	{
	case DCI_JPEG_ENCODE:
		{
			DWORD32 dwEncodeTime = 0;
			DWORD32 dwCurTick = 0;
			unsigned char* pbPos = NULL;
			int* pnParamSize = NULL;
			JPEG_ENCODE_PARAM* pParam = NULL;
			int* pnDataSize = NULL;
			JPEG_ENCODE_DATA* pData = NULL;

			pbPos = pbInbuf + 4;
			pnParamSize = (int*)pbPos;
			pbPos += 4;
			pParam = (JPEG_ENCODE_PARAM*)pbPos;
			pbPos += sizeof(JPEG_ENCODE_PARAM);
			pnDataSize = (int*)pbPos;
			pbPos += 4;
			pData = (JPEG_ENCODE_DATA*)pbPos;

			if (*pnParamSize == sizeof(JPEG_ENCODE_PARAM) && *pnDataSize == sizeof(JPEG_ENCODE_DATA))
			{
				JPEG_ENCODE_RESPOND* pJpegEncodeRespond = (JPEG_ENCODE_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				pRespondHead->iAppendDataSize = sizeof(JPEG_ENCODE_RESPOND);
				dwCurTick = CLK_getltime();
				pRespondHead->iResult = YUV2JPG_TI(pParam, pData, pJpegEncodeRespond);
				dwEncodeTime = CLK_getltime() - dwCurTick;
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}
			if (S_OK == pRespondHead->iResult)
			{
				// 输出处理时间
				sprintf(
					pRespondHead->szErrorInfo, 
					"JpegEncode time:%d\n", dwEncodeTime
					);
			}
			else
			{
				// 输出错误信息
				sprintf(pRespondHead->szErrorInfo, "DCI_JPEG_ENCODE ErrCode = [0x%08x]\n", g_iErrCode);
			}
			pRespondHead->szErrorInfo[255] = 0;
		}
		break;

	case DCI_H264_ENCODE:
		{
			DWORD32 dwEncodeTime = 0;
			DWORD32 dwCurTick = 0;
			unsigned char* pbPos = NULL;
			int* pnParamSize = NULL;
			H264_ENCODE_PARAM* pParam = NULL;
			int* pnDataSize = NULL;
			H264_ENCODE_DATA* pData = NULL;

			pbPos = pbInbuf + 4;
			pnParamSize = (int*)pbPos;
			pbPos += 4;
			pParam = (H264_ENCODE_PARAM*)pbPos;
			pbPos += sizeof(H264_ENCODE_PARAM);
			pnDataSize = (int*)pbPos;
			pbPos += 4;
			pData = (H264_ENCODE_DATA*)pbPos;

			if (*pnParamSize == sizeof(H264_ENCODE_PARAM) && *pnDataSize == sizeof(H264_ENCODE_DATA))
			{
				H264_ENCODE_RESPOND* pH264EncodeRespond = (H264_ENCODE_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				pRespondHead->iAppendDataSize = sizeof(H264_ENCODE_RESPOND);
				dwCurTick = CLK_getltime();
				if (pParam->fEnableH264BrightnessAdjust)
				{
					GammaProcess(&pData->hvImageYuv, pParam);
				}
				pRespondHead->iResult = YUV2H264_TI(pParam, pData, pH264EncodeRespond);
				dwEncodeTime = CLK_getltime() - dwCurTick;
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}
			if (S_OK == pRespondHead->iResult)
			{
				// 输出处理时间
				sprintf(
					pRespondHead->szErrorInfo, 
					"H264Encode time:%d\n",
					dwEncodeTime
					);
			}
			else
			{
				// 输出错误信息
				sprintf(pRespondHead->szErrorInfo, "DCI_H264_ENCODE ErrCode = [0x%08x]\n", g_iErrCode);
			}
			pRespondHead->szErrorInfo[255] = 0;
		}
		break;

		case DCI_STRING_OVERLAYINIT:
		{
			unsigned char* pbPos = NULL;
			int* pnParamSize = NULL;
			STRING_OVERLAY_PARAM* pParam = NULL;
			int* pnDataSize = NULL;
			STRING_OVERLAY_DATA* pData = NULL;

			pbPos = pbInbuf + 4;
			pnParamSize = (int*)pbPos;
			pbPos += 4;
			pParam = (STRING_OVERLAY_PARAM*)pbPos;
			pbPos += sizeof(STRING_OVERLAY_PARAM);
			pnDataSize = (int*)pbPos;
			pbPos += 4;
			pData = (STRING_OVERLAY_DATA*)pbPos;

			if (*pnParamSize == sizeof(STRING_OVERLAY_PARAM) && *pnDataSize == sizeof(STRING_OVERLAY_DATA))
			{
				STRING_OVERLAY_RESPOND* pRespond = (STRING_OVERLAY_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				pRespondHead->iAppendDataSize = sizeof(STRING_OVERLAY_RESPOND);
				pRespondHead->iResult = InitStringOverlay(pParam, pData, pRespond);
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}
			if (S_OK != pRespondHead->iResult)
			{
				// 输出错误信息
				sprintf(pRespondHead->szErrorInfo, "DCI_STRING_OVERLAYINIT ErrCode = [0x%08x]\n", g_iErrCode);
			}
			pRespondHead->szErrorInfo[255] = 0;

			break;
		}
	case DCI_CAM_DSP:
		{
			DWORD32 dwEncodeTime = 0;
			DWORD32 dwCurTick = 0;
			unsigned char* pbPos = NULL;
			int* pnParamSize = NULL;
			CAM_DSP_PARAM* pParam = NULL;
			int* pnDataSize = NULL;
			CAM_DSP_DATA* pData = NULL;

			pbPos = pbInbuf + 4;
			pnParamSize = (int*)pbPos;
			pbPos += 4;
			pParam = (CAM_DSP_PARAM*)pbPos;
			pbPos += sizeof(CAM_DSP_PARAM);
			pnDataSize = (int*)pbPos;
			pbPos += 4;
			pData = (CAM_DSP_DATA*)pbPos;

			if (*pnParamSize == sizeof(CAM_DSP_PARAM) && *pnDataSize == sizeof(CAM_DSP_DATA))
			{
				CAM_DSP_RESPOND* pCamDspRespond = (CAM_DSP_RESPOND*)(pbOutbuf + sizeof(DSP_RESPOND_HEAD));
				pRespondHead->iAppendDataSize = sizeof(CAM_DSP_RESPOND);
				dwCurTick = CLK_getltime();
				pRespondHead->iResult = CamDspProc(pParam, pData, pCamDspRespond);
				dwEncodeTime = CLK_getltime() - dwCurTick;
			}
			else
			{
				sprintf(pRespondHead->szErrorInfo, "\nDSPLink protocol not match!\n");
			}
			if (S_OK == pRespondHead->iResult)
			{
				// 输出处理时间
				sprintf(
					pRespondHead->szErrorInfo, 
					"CamDspProc time:%d\n",
					dwEncodeTime
					);
			}
			else
			{
				// 输出错误信息
				sprintf(pRespondHead->szErrorInfo, "DCI_CAM_DSP ErrCode = [0x%08x]\n", g_iErrCode);
			}
			pRespondHead->szErrorInfo[255] = 0;
		}
		break;

	default:
		pRespondHead->dwCmdId = DCI_UNKNOWN;
	}
}
