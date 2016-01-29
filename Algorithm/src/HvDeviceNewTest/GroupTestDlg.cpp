#include "stdafx.h"
#include "GroupTestDlg.h"
#include ".\grouptestdlg.h"
#include "shlwapi.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) {delete[] p; p = NULL;}
#endif

#define WM_UPDATE_INFO	(WM_USER+200)
#define WM_UPDATE_PLATE (WM_USER+220)
#define WM_UPDATE_VIDEO	(WM_USER+230)

#define WM_USERDEFMSG_GROUP (WM_USER + 103) 


void WriteLog(LPCSTR lpszLogInfo)
{
	CTime cTime = CTime::GetCurrentTime();
	CString cTimeStr;
	cTimeStr = cTime.Format("%Y-%m-%d_%H:%M:%S");
	FILE* fp = fopen("D:\\DLL_Review测试\\HvDeviceNewTest.log", "a+");
	if(fp)
	{
		fwrite(cTimeStr.GetBuffer(), cTimeStr.GetLength(), 1, fp);
		fwrite(lpszLogInfo, strlen(lpszLogInfo), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
}

CTestType::CTestType(char* szIP, CListCtrl* pListBox, DWORD dwListIndex, CString strSavePath, CWnd* pParent)
{
	m_ListBox = pListBox;
	m_dwIndex = dwListIndex;
	m_ListBox->SetItemText(m_dwIndex, 1, "测试类初始化成功");
	ZeroMemory(m_szIP, sizeof(m_szIP));
	memcpy(m_szIP, szIP, strlen(szIP));
	m_strSavePath = strSavePath;

	if(m_strSavePath != "")
	{
		m_ListBox->SetItemText(m_dwIndex, 1, "正在创建保存文件夹");
		if(!PathFileExists(m_strSavePath.GetBuffer()))
		{
			if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
			{
				m_strSavePath += "\\";
				m_strSavePath += szIP;
				m_strSavePath += "\\";
				if(!PathFileExists(m_strSavePath.GetBuffer()))
				{
					if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
					{
						m_fIsCanSave = TRUE;
					}
				}
				else
				{
					m_fIsCanSave = TRUE;
				}
			}
		}
		else
		{
			m_strSavePath += "\\";
			m_strSavePath += szIP;
			m_strSavePath += "\\";
			if(!PathFileExists(m_strSavePath.GetBuffer()))
			{
				if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
				{
					m_fIsCanSave = TRUE;
				}
			}
			else
			{
				m_fIsCanSave = TRUE;
			}
		}
	}

	m_pParent = pParent;
	InitializeCriticalSection(&m_csList);
	InitializeCriticalSection(&m_csVideoList);
	m_hDevice = NULL;
	m_hProceThread = NULL;
	m_dwStatus = 0;
	m_fIsThreadExit = FALSE;
	m_fIsProceResultThreadExit = FALSE;
	m_fIsProceVideoThreadExit = FALSE;
	m_fJPEGFps = 0.;
	m_fH264Fps = 0.;
	m_dwJPEGLastTick = GetTickCount();
	m_dwH264LastTick = GetTickCount();
	m_dwJPEGFrameCount = 0;
	m_dwH264FrameCount = 0;
	m_hProceResultDataThread = CreateThread(NULL, 0, ProceResultDataThread, this, 0, NULL);
	m_hProceVideoDataThread = CreateThread(NULL, 0, ProceVideoDataThread, this, 0, NULL);
	m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);
}

CTestType::~CTestType()
{
	if(m_hProceThread)
	{
		m_fIsThreadExit = TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}

	if(m_hProceResultDataThread)
	{
		m_fIsProceResultThreadExit = TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceResultDataThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceResultDataThread, 0);
		}
		CloseHandle(m_hProceResultDataThread);
		m_hProceResultDataThread = NULL;
	}

	if(m_hProceVideoDataThread)
	{
		m_fIsProceVideoThreadExit = TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceVideoDataThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceVideoDataThread, 0);
		}
		CloseHandle(m_hProceVideoDataThread);
		m_hProceVideoDataThread = NULL;
	}

	if(m_hDevice != NULL)
	{
		HVAPI_CloseEx(m_hDevice);
		m_hDevice = NULL;
	}
	
	while(!m_ResultList.IsEmpty())
	{
		GroupResultData cTmpInfo = m_ResultList.RemoveHead();
		if (cTmpInfo.pcPlateNo)
		{
			delete [] cTmpInfo.pcPlateNo;
			cTmpInfo.pcPlateNo = NULL;
		}

		if(cTmpInfo.pcAppendInfo)
		{
			delete[] cTmpInfo.pcAppendInfo;
			cTmpInfo.pcAppendInfo = NULL;
		}

		if(cTmpInfo.pImageData)
		{
			delete[] cTmpInfo.pImageData;
			cTmpInfo.pImageData = NULL;
		}
	}

	while(!m_VideoList.IsEmpty())
	{
		GroupVideoFrameData cTmpInfo = m_VideoList.RemoveHead();
		if(cTmpInfo.pVideoData)
		{
			delete[] cTmpInfo.pVideoData;
			cTmpInfo.pVideoData = NULL;
		}
		if(cTmpInfo.pszVideoExInfo)
		{
			delete[] cTmpInfo.pszVideoExInfo;
			cTmpInfo.pszVideoExInfo = NULL;
		}
	}

	DeleteCriticalSection(&m_csList);
}

DWORD WINAPI CTestType::ProceVideoDataThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return 10;
	}
	//static float fJPEGFPS = 0.;
	//static float fH264FPS = 0.;
	//static DWORD dwJPEGFrameCount = 0;
	//static DWORD dwH264FrameCount = 0;
	//static DWORD dwJPEGTicke = GetTickCount();
	//static DWORD dwH264Ticke = GetTickCount();
	CTestType* pTestType = (CTestType*)pParam;
	while(!pTestType->m_fIsProceVideoThreadExit)
	{
		if(pTestType->m_VideoList.IsEmpty())
		{
			Sleep(500);
			continue;
		}
		GroupVideoFrameData TmpVideoData;
		EnterCriticalSection(&pTestType->m_csVideoList);
		TmpVideoData = pTestType->m_VideoList.RemoveHead();
		LeaveCriticalSection(&pTestType->m_csVideoList);

		if(TmpVideoData.dwPackeType == 1)
		{
			if(TmpVideoData.dwVideoType == IMAGE_TYPE_JPEG_NORMAL)
			{
				//dwJPEGFrameCount++;
				char szInfo[128] = {0};
				sprintf(szInfo, "JPEG Stream: DateLen = %d, %fFPS", TmpVideoData.dwVideoDataLen, pTestType->m_fJPEGFps);
				pTestType->m_pParent->SendMessage(WM_UPDATE_VIDEO, pTestType->m_dwIndex, (LPARAM)szInfo);
			}
			else if(TmpVideoData.dwVideoType == IMAGE_TYPE_JPEG_LPR)
			{
				if(pTestType->m_fJPEGFps < 1)
				{
					char szInfo[128] = {0};
					sprintf(szInfo, "LPR Frame: DateLen = %d", TmpVideoData.dwVideoDataLen);
					pTestType->m_pParent->SendMessage(WM_UPDATE_VIDEO, pTestType->m_dwIndex, (LPARAM)szInfo);
				}
			}
		}
		else
		{
			if(TmpVideoData.dwVideoType == VIDEO_TYPE_H264_NORMAL_I
				|| TmpVideoData.dwVideoType == VIDEO_TYPE_H264_NORMAL_P)
			{
				//dwH264FrameCount++;
				char szInfo[128] = {0};
				sprintf(szInfo, "H264 Stream: DateLen = %d, %fFPS", TmpVideoData.dwVideoDataLen, pTestType->m_fH264Fps);
				pTestType->m_pParent->SendMessage(WM_UPDATE_VIDEO, pTestType->m_dwIndex, (LPARAM)szInfo);
			}
		}

		//if(dwH264FrameCount >= 50)
		//{
		//	DWORD dwCurrentTick = GetTickCount();
		//	fH264FPS = (float)((dwH264FrameCount)/((float)(dwCurrentTick - dwH264Ticke) / 1000.));
		//	dwH264Ticke = dwCurrentTick;
		//	dwH264FrameCount = 0;
		//}

		//if(dwJPEGFrameCount >= 50)
		//{
		//	DWORD dwCurrentTick = GetTickCount();
		//	fJPEGFPS = (float)((dwJPEGFrameCount)/((float)(dwCurrentTick - dwJPEGTicke) / 1000.));
		//	dwJPEGTicke = dwCurrentTick;
		//	dwJPEGFrameCount = 0;
		//}

		if(TmpVideoData.pVideoData)
		{
			delete[] TmpVideoData.pVideoData;
			TmpVideoData.pVideoData = NULL;
		}
		if(TmpVideoData.pszVideoExInfo)
		{
			delete[] TmpVideoData.pszVideoExInfo;
			TmpVideoData.pszVideoExInfo = NULL;
		}
	}
	return 0;
}

DWORD WINAPI CTestType::ProceResultDataThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return 10;
	}

	CTestType* pTestType = (CTestType*)pParam;
	while(!pTestType->m_fIsProceResultThreadExit)
	{
		if(pTestType->m_ResultList.IsEmpty())
		{
			Sleep(500);
			continue;
		}	

		GroupResultData TmpResultData;
		EnterCriticalSection(&pTestType->m_csList);
		TmpResultData = pTestType->m_ResultList.RemoveHead();
		LeaveCriticalSection(&pTestType->m_csList);

		switch(TmpResultData.iDataType)
		{
		case 1:
			{
				pTestType->m_pParent->SendMessage(WM_UPDATE_PLATE, pTestType->m_dwIndex, (LPARAM)TmpResultData.pcPlateNo);
				if(pTestType->m_fIsCanSave)
				{
					/*CString strSaveFileName;
					CTime cTime(TmpResultData.dw64TimeMS/1000);
					strSaveFileName.Format("%s%s_%d.xml", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), TmpResultData.dwCarID);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(TmpResultData.pcAppendInfo, strlen(TmpResultData.pcAppendInfo), 1, fp);
						fclose(fp);
					}

					strSaveFileName.Format("%sPlateInfo.txt", pTestType->m_strSavePath.GetBuffer());
					fp = fopen(strSaveFileName.GetBuffer(), "a+");
					if(fp)
					{
						fwrite(TmpResultData.pcPlateNo, strlen(TmpResultData.pcPlateNo), 1, fp);
						fwrite("\n", 1, 1, fp);
						fclose(fp);
					}*/
				}
			}
			break;
		case 2:
			{
				if(pTestType->m_fIsCanSave)
				{
					/*CString strSaveFileName;
					CTime cTime(TmpResultData.dw64TimeMS/1000);
					strSaveFileName.Format("%s%s_%d_%d.jpeg", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), TmpResultData.dwCarID, TmpResultData.dwImageType);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(TmpResultData.pImageData, TmpResultData.dwImageDataLen, 1, fp);
						fclose(fp);
					}*/
				}
			}
			break;
		case 3:
			{
				if(pTestType->m_fIsCanSave)
				{
					/*CString strSaveFileName;
					CTime cTime(TmpResultData.dw64TimeMS/1000);
					strSaveFileName.Format("%s%s_%d.bmp", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), TmpResultData.dwCarID);
					INT iDestBufLen = 1024 << 9;
					PBYTE DestData = new BYTE[iDestBufLen];
					if(DestData == NULL)
					{
						break;
					}
					if(HVAPIUTILS_SmallImageToBitmapEx(TmpResultData.pImageData,
						TmpResultData.wImageWidth, TmpResultData.wImageHeight, 
						DestData, &iDestBufLen) != S_OK)
					{
						if(DestData)
						{
							delete[] DestData;
							DestData = NULL;
						}
						break;
					}
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp && DestData)
					{
						fwrite(DestData, iDestBufLen, 1, fp);
						fclose(fp);
					}
					if(DestData)
					{
						delete[] DestData;
						DestData = NULL;
					}*/
				}
			}
			break;
		case 4:
			{
				if(pTestType->m_fIsCanSave)
				{
					/*CString strSaveFileName;
					CTime cTime(TmpResultData.dw64TimeMS/1000);
					strSaveFileName.Format("%s%s_%d.bin", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), TmpResultData.dwCarID);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(TmpResultData.pImageData, TmpResultData.dwImageDataLen, 1, fp);
						fclose(fp);
					}*/
				}
			}
			break;
		}

		if(TmpResultData.pcPlateNo)
		{
			delete[] TmpResultData.pcPlateNo;
			TmpResultData.pcPlateNo = NULL;
		}

		if(TmpResultData.pcAppendInfo)
		{
			delete[] TmpResultData.pcAppendInfo;
			TmpResultData.pcAppendInfo = NULL;
		}

		if(TmpResultData.pImageData)
		{
			delete[] TmpResultData.pImageData;
			TmpResultData.pImageData = NULL;
		}
	}

	return 0;
}

DWORD WINAPI CTestType::ProceThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return -1;
	}
	CTestType* pTestType = (CTestType*)pParam;
	while(!pTestType->m_fIsThreadExit)
	{
		switch(pTestType->m_dwStatus)
		{
		case 0:
			pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在连接识别器...");
			pTestType->m_hDevice = HVAPI_OpenEx(pTestType->m_szIP, NULL);
			if(pTestType->m_hDevice != NULL)
			{
				pTestType->m_dwStatus = 1;
			}
			break;
		case 1:
			pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在设置二值图回调...");
			if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnBinary, pTestType, 0, 
				CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL) == S_OK)
			{
				pTestType->m_dwStatus = 2;
			}
			break;
		case 2:
			pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在设置大图回调...");
			if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnBigImage, pTestType, 0, 
				CALLBACK_TYPE_RECORD_BIGIMAGE, NULL) == S_OK)
			{
				pTestType->m_dwStatus = 3;
			}
			break;
		case 3:
			pTestType->m_dwStatus = 4;
			break;
		case 4:
			pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在设置小图回调...");
			if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnSmall, pTestType, 0, 
				CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL) == S_OK)
			{
				pTestType->m_dwStatus = 5;
			}
			break;
		case 5:
			pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在设置车牌回调...");
			if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnPlate, 
				pTestType, 0, CALLBACK_TYPE_RECORD_PLATE, NULL) == S_OK)
			{
				pTestType->m_dwStatus = 6;
			}
			break;
		case 6:
			{
				DWORD dwStatus;
				DWORD dwReconnectTimes;
				CString strInfo = "";

				HVAPI_GetConnStatusEx(pTestType->m_hDevice, CONN_TYPE_RECORD, &dwStatus);
				switch(dwStatus)
				{
				case CONN_STATUS_UNKNOWN:
					strInfo += "连接状态:未知,";
					break;
				case CONN_STATUS_NORMAL:
					strInfo += "连接状态:正常,";
					break;
				case CONN_STATUS_DISCONN:
					strInfo += "连接状态:停止,";
					break;
				case CONN_STATUS_RECONN:
					strInfo += "连接状态:重连,";
					break;
				case CONN_STATUS_RECVDONE:
					strInfo += "连接状态:结束,";
					break;
				}

				HVAPI_GetReConnectTimesEx(pTestType->m_hDevice, CONN_TYPE_RECORD, &dwReconnectTimes, FALSE);
				CString strTemp;
				strTemp.Format("重连次数:%d", dwReconnectTimes);
				strInfo += strTemp;
				DWORD dwVideoReconnectTimes = 0;
				HVAPI_GetReConnectTimesEx(pTestType->m_hDevice, CONN_TYPE_IMAGE, &dwVideoReconnectTimes, FALSE);
				strTemp.Format("JPEG重连次数：%d", dwVideoReconnectTimes);
				strInfo += strTemp;
				HVAPI_GetReConnectTimesEx(pTestType->m_hDevice, CONN_TYPE_VIDEO, &dwVideoReconnectTimes, FALSE);
				strTemp.Format("H264重连次数：%d", dwVideoReconnectTimes);
				strInfo += strTemp;
				pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)strInfo.GetBuffer());
				Sleep(2000);
			}
			break;
		case 11:
			{
				pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在设置JPEG视频流回调...");
				int iDevType = DEV_TYPE_UNKNOWN;
				HVAPI_GetDevTypeEx(pTestType->m_szIP, &iDevType);
				if(iDevType == DEV_TYPE_HVSIGLE)
				{
					if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnJpegFrame, pTestType, 0, 
					CALLBACK_TYPE_JPEG_FRAME, "SetImgType,EnableRecogVideo[1]") == S_OK)
					{
						pTestType->m_dwStatus = 1;
					}
				}
				else
				{
					if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnJpegFrame, pTestType, 0, 
						CALLBACK_TYPE_JPEG_FRAME, NULL) == S_OK)
					{
						pTestType->m_dwStatus = 1;
					}
				}
			}
			break;
		case 12:
			pTestType->m_pParent->SendMessage(WM_UPDATE_INFO, pTestType->m_dwIndex, (LPARAM)"正在设置H264视频流回调...");
			if(HVAPI_SetCallBackEx(pTestType->m_hDevice, pTestType->OnH264Frame, pTestType, 0, 
				CALLBACK_TYPE_H264_VIDEO, NULL) == S_OK)
			{
				pTestType->m_dwStatus = 1;
			}
			break;
		}
	}
	return 0;
}

INT CTestType::OnPlate(PVOID pUserData, DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
	if(pUserData == NULL || pcAppendInfo == NULL || pcPlateNo == NULL)
	{
		WriteLog("Error Plate Info...");
		return -1;
	}
	CTestType* pTestType = (CTestType*)pUserData;
	if(pTestType->m_ResultList.IsFull())
	{
		return -1;
	}

	GroupResultData NewResultData;
	NewResultData.iDataType = 1;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dw64TimeMS;
	int iPlateStrLen = (int)strlen(pcPlateNo);
	int iAppendStrLen = (int)strlen(pcAppendInfo);
	NewResultData.pcPlateNo = new char[iPlateStrLen + 1];
	NewResultData.pcAppendInfo = new char[iAppendStrLen + 1];
	if(NewResultData.pcPlateNo == NULL || NewResultData.pcAppendInfo == NULL)
	{
		SAFE_DELETE(NewResultData.pcPlateNo);
		SAFE_DELETE(NewResultData.pcAppendInfo);
		return -1;
	}
	memcpy(NewResultData.pcPlateNo, pcPlateNo, iPlateStrLen);
	NewResultData.pcPlateNo[iPlateStrLen] = '\0';
	memcpy(NewResultData.pcAppendInfo, pcAppendInfo, iAppendStrLen);
	NewResultData.pcAppendInfo[iAppendStrLen] = '\0';
	EnterCriticalSection(&pTestType->m_csList);
	pTestType->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pTestType->m_csList);

	return 0;
}

INT CTestType::OnBigImage(PVOID pUserData, DWORD dwCarID, WORD wImgType, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS)
{
	if(pUserData == NULL || dwImgDataLen <= 0 || pbPicData == NULL)
	{
		WriteLog("Error Big Image Data...");
		return -1;
	}
	CTestType* pTestType = (CTestType*)pUserData;
	if(pTestType->m_ResultList.IsFull())
	{
		return -1;
	}
	GroupResultData NewResultData;
	NewResultData.iDataType = 2;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dw64TimeMS;
	NewResultData.dwImageType = wImgType;
	NewResultData.wImageWidth = wWidth;
	NewResultData.wImageHeight = wHeight;
	NewResultData.dwImageDataLen = dwImgDataLen;
	NewResultData.pImageData = new BYTE[dwImgDataLen];
	if(NewResultData.pImageData == NULL)
	{
		return -1;
	}
	memcpy(NewResultData.pImageData, pbPicData, dwImgDataLen);
	EnterCriticalSection(&pTestType->m_csList);
	pTestType->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pTestType->m_csList);

	return 0;
}

INT CTestType::OnSmall(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS)
{
	if(pUserData == NULL || pbPicData == NULL || dwImgDataLen <= 0)
	{
		WriteLog("Error Small Image Data...");
		return -1;
	}
	CTestType* pTestType = (CTestType*)pUserData;
	if(pTestType->m_ResultList.IsFull())
	{
		return -1;
	}
	GroupResultData NewResultData;
	NewResultData.iDataType = 3;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dwTimeMS;
	NewResultData.dwImageType = 0;
	NewResultData.wImageWidth = wWidth;
	NewResultData.wImageHeight = wHeight;
	NewResultData.dwImageDataLen = dwImgDataLen;
	NewResultData.pImageData = new BYTE[dwImgDataLen];
	if(NewResultData.pImageData == NULL)
	{
		return -1;
	}
	memcpy(NewResultData.pImageData, pbPicData, dwImgDataLen);
	EnterCriticalSection(&pTestType->m_csList);
	pTestType->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pTestType->m_csList);
	
	return 0;
}

INT CTestType::OnBinary(PVOID pUserData, DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dwTimeMS)
{
	if(pUserData == NULL || pbPicData == NULL || dwImgDataLen <= 0)
	{
		WriteLog("Error Binary Image Data...");
		return -1;
	}
	CTestType* pTestType = (CTestType*)pUserData;
	if(pTestType->m_ResultList.IsFull())
	{
		return -1;
	}
	GroupResultData NewResultData;
	NewResultData.iDataType = 4;
	NewResultData.dwCarID = dwCarID;
	NewResultData.dw64TimeMS = dwTimeMS;
	NewResultData.dwImageType = 0;
	NewResultData.wImageWidth = wWidth;
	NewResultData.wImageHeight = wHeight;
	NewResultData.dwImageDataLen = dwImgDataLen;
	NewResultData.pImageData = new BYTE[dwImgDataLen];
	if(NewResultData.pImageData == NULL)
	{
		return -1;
	}
	memcpy(NewResultData.pImageData, pbPicData, dwImgDataLen);
	EnterCriticalSection(&pTestType->m_csList);
	pTestType->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pTestType->m_csList);
	
	return 0;
}

INT CTestType::OnJpegFrame(PVOID pUserData, PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo)
{
	if(pUserData == NULL || pbImageData == NULL || szImageExtInfo == NULL
		|| dwImageDataLen <= 0)
	{
		return -1;
	}
	if(dwImageType != IMAGE_TYPE_JPEG_NORMAL &&
		dwImageType != IMAGE_TYPE_JPEG_LPR)
	{
		return 0;
	}

	CTestType* pTestType = (CTestType*)pUserData;

	if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
	{
		pTestType->m_dwJPEGFrameCount++;
		if(pTestType->m_dwJPEGFrameCount >= 50)
		{
			DWORD dwCurrentTick = GetTickCount();
			pTestType->m_fJPEGFps = (float)((pTestType->m_dwJPEGFrameCount)/((float)(dwCurrentTick - pTestType->m_dwJPEGLastTick) / 1000.));
			pTestType->m_dwJPEGLastTick = dwCurrentTick;
			pTestType->m_dwJPEGFrameCount = 0;
		}
	}

	if(pTestType->m_VideoList.IsFull())
	{
		return -1;
	}

	GroupVideoFrameData NewVideoData;
	NewVideoData.dwPackeType = 1;
	NewVideoData.dwVideoType = dwImageType;
	NewVideoData.dwVideoDataLen = dwImageDataLen;
	NewVideoData.dwVideoExInfoLen = (DWORD)strlen(szImageExtInfo);
	NewVideoData.pszVideoExInfo = new char[NewVideoData.dwVideoExInfoLen + 1];
	if(NewVideoData.pszVideoExInfo == NULL)
	{
		return -1;
	}
	NewVideoData.pVideoData = new BYTE[dwImageDataLen];
	if(NewVideoData.pVideoData == NULL)
	{
		delete[] NewVideoData.pszVideoExInfo;
		NewVideoData.pszVideoExInfo = NULL;
		return -1;
	}
	memcpy(NewVideoData.pszVideoExInfo, szImageExtInfo, NewVideoData.dwVideoExInfoLen);
	NewVideoData.pszVideoExInfo[NewVideoData.dwVideoExInfoLen] = '\0';
	memcpy(NewVideoData.pVideoData, pbImageData, dwImageDataLen);

	EnterCriticalSection(&pTestType->m_csVideoList);
	pTestType->m_VideoList.AddTail(NewVideoData);
	LeaveCriticalSection(&pTestType->m_csVideoList);

	return 0;
}

INT CTestType::OnH264Frame(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo)
{
	if(pUserData == NULL || pbVideoData == NULL || szVideoExtInfo == NULL
		|| dwVideoDataLen <= 0)
	{
		return -1;
	}
	if(dwVideoType != VIDEO_TYPE_H264_NORMAL_I &&
		dwVideoType != VIDEO_TYPE_H264_NORMAL_P)
	{
		return 0;
	}

	CTestType* pTestType = (CTestType*)pUserData;
	if(dwVideoType == VIDEO_TYPE_H264_NORMAL_I || dwVideoType == VIDEO_TYPE_H264_NORMAL_P)
	{
		pTestType->m_dwH264FrameCount++;
		if(pTestType->m_dwH264FrameCount >= 50)
		{
			DWORD dwCurrentTick = GetTickCount();
			pTestType->m_fH264Fps = (float)((pTestType->m_dwH264FrameCount)/((float)(dwCurrentTick - pTestType->m_dwH264LastTick) / 1000.));
			pTestType->m_dwH264LastTick = dwCurrentTick;
			pTestType->m_dwH264FrameCount = 0;
		}
	}
	if(pTestType->m_VideoList.IsFull())
	{
		return -1;
	}

	GroupVideoFrameData NewVideoData;
	NewVideoData.dwPackeType = 2;
	NewVideoData.dwVideoType = dwVideoType;
	NewVideoData.dwVideoDataLen = dwVideoDataLen;
	NewVideoData.dwVideoExInfoLen = (DWORD)strlen(szVideoExtInfo);
	NewVideoData.pszVideoExInfo = new char[NewVideoData.dwVideoExInfoLen + 1];
	if(NewVideoData.pszVideoExInfo == NULL)
	{
		return -1;
	}
	NewVideoData.pVideoData = new BYTE[dwVideoDataLen];
	if(NewVideoData.pVideoData == NULL)
	{
		delete[] NewVideoData.pszVideoExInfo;
		NewVideoData.pszVideoExInfo = NULL;
		return -1;
	}
	memcpy(NewVideoData.pszVideoExInfo, szVideoExtInfo, NewVideoData.dwVideoExInfoLen);
	NewVideoData.pszVideoExInfo[NewVideoData.dwVideoExInfoLen] = '\0';
	memcpy(NewVideoData.pVideoData, pbVideoData, dwVideoDataLen);

	EnterCriticalSection(&pTestType->m_csVideoList);
	pTestType->m_VideoList.AddTail(NewVideoData);
	LeaveCriticalSection(&pTestType->m_csVideoList);
	return 0;
}

IMPLEMENT_DYNAMIC(CGroupTestDlg, CDialog)

CGroupTestDlg::CGroupTestDlg(CWnd* pParent /* = NULL */)
: CDialog(CGroupTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if(pParent)
		m_pParentDlg = pParent;
	for(INT iIndex = 0; iIndex < 20; iIndex++)
		m_TestType[iIndex] = NULL;
}

CGroupTestDlg::~CGroupTestDlg()
{

}

void CGroupTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPControll);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST2, m_ListControll);
}

BEGIN_MESSAGE_MAP(CGroupTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CGroupTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	m_IPControll.SetAddress(0, 0, 0, 0);
	m_ListBox.ResetContent();
	m_Messge.Create(this);
	m_Messge.AddTool(GetDlgItem(IDC_BUTTON2), "TEST");
	m_Messge.SetDelayTime(0);
	m_Messge.SetTipBkColor(RGB(255, 255, 255));
	m_Messge.SetTipTextColor(RGB(255, 0, 0));
	m_Messge.Activate(TRUE);

	m_ListControll.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListControll.InsertColumn(0, "设备IP", LVCFMT_LEFT, 100);
	m_ListControll.InsertColumn(1, "当前状态", LVCFMT_LEFT, 160);
	m_ListControll.InsertColumn(2, "车牌", LVCFMT_LEFT, 80);
	m_ListControll.InsertColumn(3, "视频信息", LVCFMT_LEFT, 210);

	SetWindowText("新接口峰值测试");

	return TRUE;
}

void CGroupTestDlg::OnCancel()
{
	WORD wIndex;
	for(wIndex=0; wIndex<20; wIndex++)
	{
		if(m_TestType[wIndex] != NULL)
		{
			delete m_TestType[wIndex];
			m_TestType[wIndex] = NULL;
		}
	}

//	CDialog::OnCancel();
	DestroyWindow();

}


void CGroupTestDlg::PostNcDestroy()
{

//	CDialog::PostNcDestroy();
    delete this;
}


void CGroupTestDlg::OnBnClickedButton1()
{
	DWORD dwIP = 0;
	m_IPControll.GetAddress(dwIP);
	if(dwIP == 0 || dwIP == 0xFFFFFFFF)
	{
		MessageBox("非法IP", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	int iTotle = m_ListBox.GetCount();
	if(iTotle > 20)
	{
		MessageBox("测试工具最多只能支持同时连接20台设备");
		return;
	}
	for(int iIndex = 0; iIndex < iTotle; iIndex++)
	{
		if(m_ListBox.GetItemData(iIndex) == dwIP)
		{
			char szMsg[100] = {0};
			char szIP[20] = {0};
			m_ListBox.GetText(iIndex, szIP);
			sprintf(szMsg, "已添加%s的设备到列表，无需重复添加", szIP);
			MessageBox(szMsg);
			return;
		}
	}

	char szIP[20] = {0};
	sprintf(szIP, "%d.%d.%d.%d", (dwIP>>24)&0xFF,
		(dwIP>>16)&0xFF, (dwIP>>8)&0xFF, (dwIP)&0xFF);
	m_ListBox.AddString(szIP);
	m_ListBox.SetItemData(iTotle, dwIP);
}

void CGroupTestDlg::OnBnClickedButton2()
{
	BROWSEINFOA bi;
	ZeroMemory(&bi, sizeof(BROWSEINFOA));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = "保存路径";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	TCHAR szFolder[MAX_PATH<<1];
	szFolder[0] = _T('\0');
	if(pidl)
	{
		if(SHGetPathFromIDList(pidl, szFolder))
		{
			m_SaveFilePath = szFolder;
		}
	}
}

BOOL CGroupTestDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_MOUSEMOVE 
		&& pMsg->hwnd == GetDlgItem(IDC_BUTTON2)->GetSafeHwnd())
	{
		char szNewMessge[MAX_PATH] = {0};
		sprintf(szNewMessge, "当前保存路径\"%s\"", m_SaveFilePath);
		m_Messge.AddTool(GetDlgItem(IDC_BUTTON2), szNewMessge);
		m_Messge.RelayEvent(pMsg);
	}
	if(pMsg->message == WM_RBUTTONDBLCLK
		&& pMsg->hwnd == GetDlgItem(IDC_LIST1)->GetSafeHwnd())
	{
		DWORD dwIndex = m_ListBox.GetCurSel();
		m_ListBox.DeleteString(dwIndex);
	}

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CGroupTestDlg::OnBnClickedButton3()
{
	DWORD dwTotle = m_ListBox.GetCount();
	if(dwTotle <= 0 || dwTotle > 20)
	{
		MessageBox("设备列表为NULL");
		return;
	}
	//if(m_SaveFilePath == "")
	//{
	//	if(MessageBox("文件保存路径未设置，接收到的结果将不保存，是否确定启动测试？", "WARMMING", MB_YESNO) == IDNO)
	//	{
	//		return;
	//	}
	//}
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIST1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
	m_ListControll.DeleteAllItems();
	for(DWORD dwIndex=0; dwIndex<dwTotle; dwIndex++)
	{
		if(m_TestType[dwIndex] != NULL)
		{
			delete m_TestType[dwIndex];
			m_TestType[dwIndex] = NULL;
		}
		char szIP[20] = {0};
		m_ListBox.GetText(dwIndex, szIP);
		m_ListControll.InsertItem(dwIndex, "", 0);
		m_ListControll.SetItemText(dwIndex, 0, szIP);
		m_TestType[dwIndex] = new CTestType(szIP, &m_ListControll, dwIndex, m_SaveFilePath, this);
	}
}

void CGroupTestDlg::OnBnClickedButton4()
{
	WORD wIndex;
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	for(wIndex=0; wIndex<20; wIndex++)
	{
		if(m_TestType[wIndex] != NULL)
		{
			delete m_TestType[wIndex];
			m_TestType[wIndex] = NULL;
		}
	}
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
	GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	m_ListControll.DeleteAllItems();
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
}

void CGroupTestDlg::OnBnClickedButton8()
{
	int iTotleSel = m_ListBox.GetCount();
	int iIndex = m_ListBox.GetCurSel();
	if(iIndex >= 0 && iIndex < iTotleSel)
	{
		m_ListBox.DeleteString(iIndex);
	}
}

BOOL CGroupTestDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message)
	{
	case WM_UPDATE_INFO:
		m_ListControll.SetItemText((int)wParam, 1, (char*)lParam);
		break;
	case WM_UPDATE_PLATE:
		m_ListControll.SetItemText((int)wParam, 2, (char*)lParam);
		break;
	case WM_UPDATE_VIDEO:
		m_ListControll.SetItemText((int)wParam, 3, (char*)lParam);
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}


void CGroupTestDlg::OnOK()
{
    // 防止按回车退出
}


void CGroupTestDlg::OnClose()
{
	m_pParentDlg = NULL;

	WORD wIndex;
	for(wIndex=0; wIndex<20; wIndex++)
	{
		if(m_TestType[wIndex] != NULL)
		{
			delete m_TestType[wIndex];
			m_TestType[wIndex] = NULL;
		}
	}
	::SendMessage(AfxGetMainWnd()->m_hWnd,WM_USERDEFMSG_GROUP,1,0L);
	CDialog::OnClose();
}
