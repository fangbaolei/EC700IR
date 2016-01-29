#include "stdafx.h"
#include "atltime.h"
#include ".\hveconnecter.h"
#include "PlateFilter.h"
#include <gdiplus.h>

using namespace Gdiplus;

extern CString g_rgstrObjType[MAX_OBJ_TYPE];
extern void OutPutMessage(LPSTR szMsg);

CLSID	g_jpgClsid;
ULONG_PTR g_gdiplusToken = 0;
BOOL g_fGdiInitialized = FALSE;
BOOL g_fIsDrawPlateFrame = FALSE;


CHveConnecter::CHveConnecter(SINGLE_HVE_ELEMENT* pSingleHveEleMent, void* pPlateFilter, CDiskManager* DiskManager)
: m_pbSmallImage(NULL)
{
	m_pPlateFilter = pPlateFilter;
	m_pSingleHveEleMent = pSingleHveEleMent;
	m_DiskManager = DiskManager;
//	m_DiskManager->GetSaveImagePath(m_ImageSavePath);
	for(int i=0; i<10; i++)
	{
		m_dwLastCarriveTick[i] = 0;
	}
	m_iBigImageID = 0;
	m_hHv = NULL;
//	CreateThread(NULL, 0, AnalyseRecordThread, this, 0,NULL);
	CreateThread(NULL, 0, SaveImageThread, this, 0,NULL);
	if( !g_fGdiInitialized )
	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
		GetEncoderClsid(L"image/jpeg", &g_jpgClsid);
		g_fGdiInitialized = true;
	}
}

CHveConnecter::~CHveConnecter(void)
{
	if(m_hHv)
	{
		HVAPI_CloseEx(m_hHv);
		m_hHv = NULL;
	}
}

HRESULT CHveConnecter::SetCallBack()
{
	int i;
	for(i=0; i<3; i++)
	{
		if(S_OK == HVAPI_SetCallBackEx(m_hHv, OnInfoBeginProxy, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL))
		{
			break;
		}
		if(i >= 2) return E_FAIL;
	}

	for(i=0; i<3; i++)
	{
		if(S_OK == HVAPI_SetCallBackEx(m_hHv, OnPlateProxy, this, 0, CALLBACK_TYPE_RECORD_PLATE, NULL))
		{
			break;
		}
		if(i >= 2) return E_FAIL;
	}

	for(i=0; i<3; i++)
	{
		if(S_OK == HVAPI_SetCallBackEx(m_hHv, OnSmallImageProxy, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL))
		{
			break;
		}
		if(i >= 2) return E_FAIL;
	}

	for(i=0; i<3; i++)
	{
		if(S_OK == HVAPI_SetCallBackEx(m_hHv, OnBinaryImageProxy, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL))
		{
			break;
		}
		if(i >= 2) return E_FAIL;
	}

	for(i=0; i<3; i++)
	{
		if(S_OK == HVAPI_SetCallBackEx(m_hHv, OnBigImageProxy, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE,NULL))
		{
			break;
		}
		if(i >= 2) return E_FAIL;
	}
	

	for(i=0; i<3; i++)
	{
		if(S_OK == HVAPI_SetCallBackEx(m_hHv, OnInfoEndProxy, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL))
		{
			break;
		}
		if(i >= 2) return E_FAIL;
	}

	return S_OK;
}

int CHveConnecter::OnPlate(DWORD dwCarID, LPCSTR pcPlateNo,LPCSTR pcAppendInfo, DWORD64 dw64TimeMs)
{
//	OutPutMessage((LPSTR)pcPlateNo);
	char *tmpPtr;

	m_curRecord.dwCarID = dwCarID;
	m_curRecord.dw64TimeMs = dw64TimeMs;
	size_t len = strlen(pcPlateNo);
	tmpPtr = new char[len+1];
	memcpy(tmpPtr, pcPlateNo, len);
	tmpPtr[len] = '\0';
	m_curRecord.pcPlateNo = tmpPtr;

	len = strlen(pcAppendInfo);
	tmpPtr = new char[len+1];
	memcpy(tmpPtr, pcAppendInfo,len);
	tmpPtr[len] = '\0';
	m_curRecord.pcAppendInfo = tmpPtr;

	return 0;
}

int CHveConnecter::OnSmallImage(DWORD dwCarID, WORD wImageWidth, WORD wImageHeight, DWORD dwSize, PBYTE pbImage, DWORD64 dw64TimeMs)
{
	if(!pbImage)
	{
		return 1;
	}
	ImageInfo *pImageInfo = new ImageInfo;
	m_curRecord.pSmallImge = pImageInfo;

	PBYTE pbTmp = new BYTE[dwSize];
	memcpy(pbTmp, pbImage, dwSize);

	m_curRecord.pSmallImge->wImageWidth = wImageWidth;
	m_curRecord.pSmallImge->wImageHeight = wImageHeight;
	m_curRecord.pSmallImge->pbImage = pbTmp;
	m_curRecord.pSmallImge->dwSize = dwSize;

	return 0;
}


int CHveConnecter::OnBinaryImage(DWORD dwCarID, WORD wImageWidth, WORD wImageHeight, DWORD dwSize, PBYTE pbImage, DWORD64 dw64TimeMs)
{
	if(!pbImage)
	{
		return 1;
	}
	PBYTE pbTmp = new BYTE[dwSize];
	memcpy(pbTmp, pbImage, dwSize);
	ImageInfo *pImageInfo = new ImageInfo;

	m_curRecord.pBinImage = pImageInfo;
	m_curRecord.pBinImage->wImageWidth = wImageWidth;
	m_curRecord.pBinImage->wImageHeight = wImageHeight;
	m_curRecord.pBinImage->pbImage = pbTmp;
	m_curRecord.pBinImage->dwSize = dwSize;
	
	return 0;
}



int CHveConnecter::OnBigImage(DWORD dwCarID, WORD wImageWidth, WORD wImageHeight, WORD wType, DWORD dwSize, PBYTE pbImage, DWORD64 dw64TimeMs)
{
	if(!pbImage) 
	{
		return 1;
	}
	PBYTE pbTmp = new BYTE[dwSize];
	memcpy(pbTmp, pbImage, dwSize);
	ImageInfo *pImageInfo = new ImageInfo;

	switch(wType)
	{
	case 1:
		m_curRecord.pBigImage_1 = pImageInfo;
		m_curRecord.pBigImage_1->wImageWidth = wImageWidth;
		m_curRecord.pBigImage_1->wImageHeight = wImageHeight;
		m_curRecord.pBigImage_1->wType = wType;
		m_curRecord.pBigImage_1->pbImage = pbTmp;
		m_curRecord.pBigImage_1->dwSize = dwSize;
		break;
	case 2:
		m_curRecord.pBigImage_2 = pImageInfo;
		m_curRecord.pBigImage_2->wImageWidth = wImageWidth;
		m_curRecord.pBigImage_2->wImageHeight = wImageHeight;
		m_curRecord.pBigImage_2->wType = wType;
		m_curRecord.pBigImage_2->pbImage = pbTmp;
		m_curRecord.pBigImage_2->dwSize = dwSize;
		break;
	case 3:
		m_curRecord.pBigImage_3 = pImageInfo;
		m_curRecord.pBigImage_3->wImageWidth = wImageWidth;
		m_curRecord.pBigImage_3->wImageHeight = wImageHeight;
		m_curRecord.pBigImage_3->wType = wType;
		m_curRecord.pBigImage_3->pbImage = pbTmp;
		m_curRecord.pBigImage_3->dwSize = dwSize;
		break;
	case 4:
		m_curRecord.pBigImage_4 = pImageInfo;
		m_curRecord.pBigImage_4->wImageWidth = wImageWidth;
		m_curRecord.pBigImage_4->wImageHeight = wImageHeight;
		m_curRecord.pBigImage_4->wType = wType;
		m_curRecord.pBigImage_4->pbImage = pbTmp;
		m_curRecord.pBigImage_4->dwSize = dwSize;
		break;
	case 5:
		m_curRecord.pBigImage_5 = pImageInfo;
		m_curRecord.pBigImage_5->wImageWidth = wImageWidth;
		m_curRecord.pBigImage_5->wImageHeight = wImageHeight;
		m_curRecord.pBigImage_5->wType = wType;
		m_curRecord.pBigImage_5->pbImage = pbTmp;
		m_curRecord.pBigImage_5->dwSize = dwSize;
		break;
	default:

		break;
	}
	

	return 0;
}

void CHveConnecter::SaveLog(char* pszLog)
{
	FILE* pFile = fopen("ConnectLog.log", "at");
	if(pFile)
	{
		CString strInfo;
		CTime time = CTime::GetCurrentTime();
		strInfo.Format("%s:		%s\n", time.Format("[%Y-%m-%d %H:%M:%S]"), pszLog);
		fprintf(pFile, strInfo.GetBuffer());
		fclose(pFile);
	}
}

HRESULT CHveConnecter::GetAppendInfoElement(CString strAppendInfo)
{
	////----------------------------------------
		char szAppendInfo[4096] = {0};
		if(strAppendInfo.GetLength() < 4096)
		{
			HVAPIUTILS_ParsePlateXmlStringEx(strAppendInfo,szAppendInfo,4096);
		}
		else
		{
			OutPutMessage("附加信息长度越界...");
			return S_FALSE;
		}

	////----------------------------------------
	

	char* pchObj = NULL;
	pchObj = strstr(szAppendInfo, "车速");
	if(pchObj)
	{
		sscanf(pchObj, "车速:%d", &(m_curResult.iCarSpeed));
	}
	else
	{
		pchObj = strstr(szAppendInfo, "视频测速");
		if(pchObj)
		{
			sscanf(pchObj, "视频测速:%d", &(m_curResult.iCarSpeed));
		}
	}
	
	pchObj = strstr(szAppendInfo, "限速值");
	if(pchObj)
	{
		int iSpeedLimit;
		sscanf(pchObj, "限速值:%d", &iSpeedLimit);
		if(m_curResult.iCarSpeed > iSpeedLimit)
			m_curResult.iSpeeding = 1;
		else
			m_curResult.iSpeeding = 0;
	}


	pchObj = strstr(szAppendInfo, "车道");
//	OutPutMessage(pchObj);
	if(pchObj)
	{
		int iRoadIndex = 0;
		sscanf(pchObj, "车道:%d", &iRoadIndex);
		if(iRoadIndex >= 0 && iRoadIndex < 10)
		{
			m_curResult.iRoadNo = m_pSingleHveEleMent->rgiRoadNo[iRoadIndex];
			switch(m_curResult.iRoadNo)
			{
			case -1:
				m_curResult.iRoadNo = -2;
				break;
			case -2:
				m_curResult.iRoadNo = -1;
				break;
			case -3:
				m_curResult.iRoadNo = -4;
				break;
			case -4:
				m_curResult.iRoadNo = -3;
				break;
			}
		}
	}

	pchObj = strstr(szAppendInfo, "车辆类型");
	if(pchObj)
	{
		char szCarType[20] = {0};
		sscanf(pchObj, "车辆类型:%s", szCarType);
		for(int i=0; i<MAX_OBJ_TYPE; i++)
		{
			if(g_rgstrObjType[i].Find(szCarType) != -1)
			{
				m_curResult.iObjId = i;
				break;
			}
		}
	}

	pchObj = strstr(szAppendInfo, "压线");
	if(pchObj)
	{
		sscanf(pchObj, "压线:车道线%d", &m_curResult.iOverLine);
	}

	pchObj = strstr(szAppendInfo, "越线");
	if(pchObj)
	{
		sscanf(pchObj, "越线:车道线%d", &m_curResult.iCorssLine);
	}

	pchObj = strstr(szAppendInfo, "路口名称");
	if(pchObj)
	{
		sscanf(pchObj, "路口名称:%s", &m_curResult.szRoadName);
	}

	pchObj = strstr(szAppendInfo, "路口方向");
	if(pchObj)
	{
		sscanf(pchObj, "路口方向:%s", &m_curResult.szRoadDirect);
	}

	pchObj = strstr(szAppendInfo, "逆行");
	if(pchObj)
	{
		m_curResult.iReverseRun = 1;
	}

	pchObj = strstr(szAppendInfo, "紧急停车");
	if(pchObj)
	{
		sscanf(pchObj, "紧急停车%d", &m_curResult.iEmergencyStop);
	}
	m_curResult.iSpecialResult = (m_curResult.iOverLine + m_curResult.iCorssLine + m_curResult.iReverseRun + m_curResult.iEmergencyStop + m_curResult.iSpeeding ) == 0 ? 0 : 1;
	return S_OK;


}



int CHveConnecter::OnInfoBegin(DWORD dwCarID)
{
	memset(&(m_curRecord), 0, sizeof(m_curRecord));
	m_curRecord.dwCarID = dwCarID;
	return 0;
}

int CHveConnecter::OnInfoEnd(DWORD dwCarID)
{	
	if(m_pListRecord.GetCount()  >= MAX_RECORD_COUNT )
	{
		OutPutMessage("缓冲区已满 保存结果失败");
		return -1;
	}
	//m_csRecord.Lock();
	//m_pListRecord.AddTail(m_curRecord);
	//m_csRecord.Unlock();
	AnalyseRecord(&m_curRecord);

	return 0;
}




DWORD  CHveConnecter::AnalyseRecordThread(LPVOID lpParameter)
{

	CHveConnecter* pThis = (CHveConnecter*)lpParameter;
	while(1)
	{
		if(pThis->m_pListRecord.GetCount() >  0)
		{
			pThis->m_csRecord.Lock();
			RECORD_INFO cRecord = pThis->m_pListRecord.RemoveHead();
			pThis->m_csRecord.Unlock();
			if(cRecord.dw64TimeMs != 0)
			{
				pThis->AnalyseRecord(&cRecord);
			}
		}
		else
		{
			Sleep(1);
		}
	}
	return 0;
}

int CHveConnecter::AnalyseRecord(RECORD_INFO* cRecord)
{
	OnAnalysePlate(cRecord);
	OnAnalyseSmallImage(cRecord);
	OnAnalyseBigImage(cRecord);
	OnAnalyseBinaryImage(cRecord);

	((CPlateFilter*)m_pPlateFilter)->AddResult(m_curResult);
	memset(&m_curResult, 0, sizeof(m_curResult));

	return 0;
}


int CHveConnecter::OnAnalysePlate(RECORD_INFO* record)
{
	DWORD dwCarID = record->dwCarID;
	LPCSTR pcPlateNo = record->pcPlateNo;
	LPCSTR pcAppendInfo = record->pcAppendInfo;
	DWORD64 dw64TimeMs = record->dw64TimeMs;
	
	OutPutMessage((LPSTR)pcPlateNo);

	//-----------------------  Log -------------------
	CTime ctime(dw64TimeMs / 1000);
	CString strFileDir;
	CString strSavePath;
	strFileDir.Format("路段ID_%04d\\%s\\", m_pSingleHveEleMent->iLocationId, ctime.Format("%Y%m%d%H"));
	strSavePath.Format("%s%s.log", strFileDir, ctime.Format("%Y%m%d%H"));
//	m_DiskManager->SaveRecordLog(strSavePath.GetBuffer(), (char*)pcPlateNo, (DWORD32)strlen(pcPlateNo));
	m_DiskManager->SaveTextFile(strSavePath.GetBuffer(), (char*)pcPlateNo,(DWORD32)strlen(pcPlateNo) , strFileDir);
	//------------------------------------------------
//	return 1;
	strcpy(m_curResult.szPlateNo, pcPlateNo);
	HRESULT hr = E_FAIL;
	CString strAppendInfo;

	////-------------------- Old  -----------------
	//char* plateinfo = NULL;
	//if(m_hHv != NULL)
	//{
	//	hr = HV_GetPlateInfo(m_hHv, &plateinfo);
	//}
	//if(SUCCEEDED(hr))
	//{
	//	strAppendInfo = plateinfo;
	//	GetAppendInfoElement(strAppendInfo);
	//}
	////----------------- end ----------------------
	

	if(m_hHv != NULL)
	{
		strAppendInfo = pcAppendInfo;
		GetAppendInfoElement(strAppendInfo);
	}
	//----------- 将识别信息写入inf文件 ---------------
	char szAppendInfo[4096] = {0};
	if(strAppendInfo.GetLength() < 4096)
	{
		HVAPIUTILS_ParsePlateXmlStringEx(strAppendInfo,szAppendInfo,4096);

		CTime ctime(dw64TimeMs / 1000);
		CString strFileDir;
		CString strSavePath;
		strFileDir.Format("路段ID_%04d\\%s\\", m_pSingleHveEleMent->iLocationId, ctime.Format("%Y%m%d%H"));
		strSavePath.Format("%s%s_%06d.inf", strFileDir, ctime.Format("%Y%m%d%H%M%S"), dwCarID);
//		m_DiskManager->SaveAppendInfo(strSavePath.GetBuffer(),szAppendInfo,(DWORD32)strlen(szAppendInfo) , strFileDir);
		m_DiskManager->SaveTextFile(strSavePath.GetBuffer(),(char*)szAppendInfo,(DWORD32)strlen(szAppendInfo) , strFileDir);
	}

	//----------- end ---------------

	m_curResult.dw64CreateTimeMs = GetTickCount();
	m_curResult.iLocationId = m_pSingleHveEleMent->iLocationId;
	m_curResult.dw64TimeMs = dw64TimeMs;

	if(m_dwLastCarriveTick[m_curResult.iRoadNo] != 0)
	{
		m_curResult.iTimeHeadway = (int)(dw64TimeMs - m_dwLastCarriveTick[m_curResult.iRoadNo]);
	}
	else
	{
		m_curResult.iTimeHeadway = 10000;
	}

	if(m_curResult.iTimeHeadway < 0)
	{
		m_curResult.iTimeHeadway = -m_curResult.iTimeHeadway;
	}
	m_dwLastCarriveTick[m_curResult.iRoadNo] = dw64TimeMs;


	delete [] pcPlateNo;		
	delete [] pcAppendInfo;		

	return 0;
}

int CHveConnecter::OnAnalyseSmallImage(RECORD_INFO* record)
{
	if(!record->pSmallImge)
	{
		return 1;
	}
	DWORD dwCarID = record->dwCarID;
	WORD wImageWidth = record->pSmallImge->wImageWidth;
	WORD wImageHeight = record->pSmallImge->wImageHeight;
	DWORD dwSize = record->pSmallImge->dwSize;
	PBYTE pbImage = record->pSmallImge->pbImage;
	DWORD64 dw64TimeMs = record->dw64TimeMs;

	int iBufLen = wImageWidth * wImageHeight * 3 + 1024;
	int iBmpLen = iBufLen;
	BYTE *pbBuffer = new BYTE[iBufLen];
//	if(Yuv2BMP(pbBuffer, iBufLen, &iBmpLen, pbImage, wImageWidth, wImageHeight) == S_OK)
	if(HVAPIUTILS_SmallImageToBitmapEx(pbImage, wImageWidth, wImageHeight, pbBuffer, &iBmpLen) == S_OK)
	{
		CTime ctime(dw64TimeMs / 1000);
		CString strFileDir;
		CString strSavePath;
		strFileDir.Format("路段ID_%04d\\%s\\", m_pSingleHveEleMent->iLocationId, ctime.Format("%Y%m%d%H"));
		strSavePath.Format("%s%s_%06d.bmp", strFileDir, ctime.Format("%Y%m%d%H%M%S"), dwCarID);
		m_DiskManager->SaveFile(strSavePath.GetBuffer(), strFileDir);
		strcpy(m_curResult.szSmallImgePath, strFileDir.GetBuffer());
		
		record->pSmallImge->strSavePath = strSavePath;
		record->pSmallImge->strFileDir = strFileDir;
		record->pSmallImge->dwSize = iBmpLen;
		record->pSmallImge->pbImage = pbBuffer;

		m_pbSmallImage = new BYTE[iBmpLen];
		if(m_pbSmallImage)
		{
			memcpy(m_pbSmallImage, pbBuffer, iBmpLen);
		}
		m_csImageInfo.Lock();
		m_ListImageInfo.AddTail((record->pSmallImge));
		m_csImageInfo.Unlock();

		//if(m_DiskManager->SaveFile(strSavePath.GetBuffer(), pbBuffer, iBmpLen, strFileDir))
		//{
		//	strcpy(m_curResult.szSmallImgePath, strFileDir.GetBuffer());
		//}
		//else
		//{
		//	CString str = "保存小图失败:" + strSavePath;
		//	OutPutMessage("保存小图失败!");
		//	SaveLog(str.GetBuffer());
		//}
	}
	else
	{
		delete[] pbBuffer;		
	}

	delete[] pbImage;
	return 0;
}

int CHveConnecter::OnAnalyseBinaryImage(RECORD_INFO* record)
{
	if(!record->pBinImage) 
	{
		return 1;
	}

	DWORD dwCarID = record->dwCarID;
	WORD wImageWidth = record->pBinImage->wImageWidth;
	WORD wImageHeight = record->pBinImage->wImageHeight;
	DWORD dwSize = record->pBinImage->dwSize;
	PBYTE pbImage = record->pBinImage->pbImage;
	DWORD64 dw64TimeMs = record->dw64TimeMs;

	CTime ctime(dw64TimeMs / 1000);
	CString strFileDir;
	CString strSavePath;
	strFileDir.Format("路段ID_%04d\\%s\\", m_pSingleHveEleMent->iLocationId, ctime.Format("%Y%m%d%H"));
	strSavePath.Format("%s%s_%06d.bin", strFileDir, ctime.Format("%Y%m%d%H%M%S"), dwCarID);
	m_DiskManager->SaveFile(strSavePath.GetBuffer(), strFileDir);
	strcpy(m_curResult.szBinImagePath, strFileDir.GetBuffer());

	record->pBinImage->strSavePath.Format("%s", strSavePath.GetBuffer());
	record->pBinImage->strFileDir.Format("%s", strFileDir.GetBuffer());

	m_csImageInfo.Lock();
	m_ListImageInfo.AddTail((record->pBinImage));
	m_csImageInfo.Unlock();

	//strTmp.Format("ID:%d Address:%x  Len:%d Width: %d  Height:%d Time:%u SavePath:[%s] FileDir:[%s]", 
	//	record->dwCarID, record->pBinImage, record->pBinImage->dwSize,
	//	record->pBinImage->wImageWidth, record->pBinImage->wImageHeight,
	//	record->dw64TimeMs/1000, record->pBinImage->strSavePath, record->pBinImage->strFileDir
	//	);
	//SaveLog(strTmp.GetBuffer());

	//if(m_DiskManager->SaveFile(strSavePath.GetBuffer(), pbImage, dwSize, strFileDir))
	//{
	//	strcpy(m_curResult.szBinImagePath, strFileDir.GetBuffer());
	//}
	//else
	//{
	//	CString str = "保存二值图失败:" + strSavePath;
	//	OutPutMessage("保存二值图失败!");
	//	SaveLog(str.GetBuffer());
	//}

	//delete[] pbImage;		
	
	return 0;
}

int CHveConnecter::OnAnalyseBigImage(RECORD_INFO* record)
{
	if(record->pBigImage_1)
	{
		//SaveBigImage(record, record->pBigImage_1);

		const int MAX_COMBIMG_SIZE = 1024*1024;
		DWORD dwSize = MAX_COMBIMG_SIZE;
		WORD wWidth, wHeight;
		BYTE* pImage = new BYTE[MAX_COMBIMG_SIZE];
		bool bSucceed = false;
		if(!record->pSmallImge)
		{
			bSucceed = HV_CombineTwoImagesToOneImage(pImage, &dwSize, &wWidth, &wHeight,record->pBigImage_1->pbImage, record->pBigImage_1->dwSize,
				NULL, 0, m_curResult.szRoadName, m_curResult.szRoadDirect, m_curResult.szPlateNo,record->dw64TimeMs);
		}
		else
		{
			bSucceed = HV_CombineTwoImagesToOneImage(pImage, &dwSize, &wWidth, &wHeight,record->pBigImage_1->pbImage, record->pBigImage_1->dwSize,
			m_pbSmallImage, record->pSmallImge->dwSize, m_curResult.szRoadName, m_curResult.szRoadDirect, m_curResult.szPlateNo,record->dw64TimeMs);
			//delete []m_pbSmallImage;  
		}
		if(bSucceed == true)
		{
			delete[] record->pBigImage_1->pbImage;
			record->pBigImage_1->pbImage = pImage;
			record->pBigImage_1->dwSize = dwSize;
		}
		else
		{
			delete[] pImage;
		}
		SaveBigImage(record, record->pBigImage_1);
	}
	
	if(record->pBigImage_2)
	{
		const int MAX_COMBIMG_SIZE = 1024*1024;
		DWORD dwSize = MAX_COMBIMG_SIZE;
		WORD wWidth, wHeight;
		BYTE* pImage = new BYTE[MAX_COMBIMG_SIZE];
		bool bSucceed = false;
		if(!record->pSmallImge)
		{
			bSucceed = HV_CombineTwoImagesToOneImage(pImage, &dwSize, &wWidth, &wHeight,record->pBigImage_2->pbImage, record->pBigImage_2->dwSize,
				NULL, 0, m_curResult.szRoadName, m_curResult.szRoadDirect, m_curResult.szPlateNo,record->dw64TimeMs);
		}
		else
		{
			bSucceed = HV_CombineTwoImagesToOneImage(pImage, &dwSize, &wWidth, &wHeight,record->pBigImage_2->pbImage, record->pBigImage_2->dwSize,
			m_pbSmallImage, record->pSmallImge->dwSize, m_curResult.szRoadName, m_curResult.szRoadDirect, m_curResult.szPlateNo,record->dw64TimeMs);
			delete []m_pbSmallImage;  
		}
		if(bSucceed == true)
		{
			delete[] record->pBigImage_2->pbImage;
			record->pBigImage_2->pbImage = pImage;
			record->pBigImage_2->dwSize = dwSize;
		}
		else
		{
			delete[] pImage;
		}
		SaveBigImage(record, record->pBigImage_2);
	}

	if(record->pBigImage_3)
	{
		SaveBigImage(record, record->pBigImage_3);
	}

	if(record->pBigImage_4)
	{
		SaveBigImage(record, record->pBigImage_4);
	}

	if(record->pBigImage_5)
	{
		SaveBigImage(record, record->pBigImage_5);
	}

	return 0;
}


int CHveConnecter::SaveBigImage(RECORD_INFO* record,ImageInfo* pImageInfo)
{
	DWORD dwCarID = record->dwCarID;
	WORD wImageWidth = pImageInfo->wImageWidth;
	WORD wImageHeight = pImageInfo->wImageHeight;
	WORD wType = pImageInfo->wType;
	DWORD dwSize = pImageInfo->dwSize;
	PBYTE pbImage = pImageInfo->pbImage;
	DWORD64 dw64TimeMs = record->dw64TimeMs;	
	
	CTime ctime(dw64TimeMs / 1000);
	CString strFileDir;
	CString strSavePath;

	char *arImageType[6] = {"",
				"_BEST_SNAPSHOT",
				"_LAST_SNAPSHOT",
				"_BEGIN_CAPTURE",
				"_BEST_CAPTURE",
				"_LAST_CAPTURE"};

	strFileDir.Format("路段ID_%04d\\%s\\", m_pSingleHveEleMent->iLocationId, ctime.Format("%Y%m%d%H"));
//	strSavePath.Format("%s%s_%06d.jpg", strFileDir, ctime.Format("%Y%m%d%H%M%S"), dwCarID);
	strSavePath.Format("%s%s_%06d%s.jpg", strFileDir, ctime.Format("%Y%m%d%H%M%S"), dwCarID,arImageType[wType]);
	m_DiskManager->SaveFile(strSavePath.GetBuffer(), strFileDir);
	
	pImageInfo->strSavePath = strSavePath;
	pImageInfo->strFileDir = strFileDir;

	m_csImageInfo.Lock();
	m_ListImageInfo.AddTail(pImageInfo);
	m_csImageInfo.Unlock();

	switch(wType)
	{
	case 1:
		strcpy(m_curResult.szBigImagePath_1, strFileDir.GetBuffer());
		break;
	case 2:
		strcpy(m_curResult.szBigImagePath_2, strFileDir.GetBuffer());
		break;
	case 3:
		strcpy(m_curResult.szBigImagePath_3, strFileDir.GetBuffer());
		break;
	case 4:
		strcpy(m_curResult.szBigImagePath_4, strFileDir.GetBuffer());
		break;
	case 5:
		strcpy(m_curResult.szBigImagePath_5, strFileDir.GetBuffer());
		break;
	default:
		OutPutMessage("大图索引越界...");
		break;
	}
	
	//if(m_DiskManager->SaveFile(strSavePath.GetBuffer(), pbImage, dwSize, strFileDir))
	//{
	//	switch(wType)
	//	{
	//	case 1:
	//		strcpy(m_curResult.szBigImagePath_1, strFileDir.GetBuffer());
	//		break;
	//	case 2:
	//		strcpy(m_curResult.szBigImagePath_2, strFileDir.GetBuffer());
	//		break;
	//	case 3:
	//		strcpy(m_curResult.szBigImagePath_3, strFileDir.GetBuffer());
	//		break;
	//	case 4:
	//		strcpy(m_curResult.szBigImagePath_4, strFileDir.GetBuffer());
	//		break;
	//	case 5:
	//		strcpy(m_curResult.szBigImagePath_5, strFileDir.GetBuffer());
	//		break;
	//	default:
	//		OutPutMessage("大图索引越界...");
	//		break;
	//	}
	//}
	//else
	//{
	//	CString str = "保存大图失败:" + strSavePath;
	//	OutPutMessage("保存大图失败!");
	//	SaveLog(str.GetBuffer());
	//}

	//delete[] pbImage;		
	//delete pImageInfo; 

	return 0;

}


DWORD  CHveConnecter::SaveImageThread(LPVOID lpParameter)
{

	CHveConnecter* pThis = (CHveConnecter*)lpParameter;
	while(1)
	{
		if(pThis->m_ListImageInfo.GetCount() >  0)
		{

			pThis->m_csImageInfo.Lock();
			ImageInfo *pImageInfo = pThis->m_ListImageInfo.RemoveHead();
			pThis->m_csImageInfo.Unlock();

			pThis->m_DiskManager->SaveFile(pImageInfo->strSavePath.GetBuffer(), pImageInfo->pbImage, pImageInfo->dwSize, pImageInfo->strFileDir);
			
			delete[] pImageInfo->pbImage;
			delete pImageInfo;
		}
		else
		{
			Sleep(100);
		}
	}

	return 0;
}

int CHveConnecter::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; 
		}    
	}

	free(pImageCodecInfo);
	return -1; 
}

bool CHveConnecter::HV_CombineTwoImagesToOneImage(
									  PBYTE pbDstImageStream,		//输出数据，保存拼接结果
									  DWORD  *dwDstDataSize,		//拼接后图像尺寸
									  WORD	*wWidth,				//输出图片宽度
									  WORD  *wHight,				//输出图片高度
									  PBYTE pbSnapShot,				//输入全景图，输入待拼接图像1
									  DWORD dwSrcDataSize1,			//近景图尺寸
									  PBYTE pbCapture,				//输入车牌小图
									  DWORD dwSrcDataSize2,			//车牌小图尺寸
									  char  *StationNo,				//桩号
									  char  *TravelDirection,		//行驶方向
									  char  *PlateNo,				//车牌号
									  DWORD64 dw64TimeMs)			//时间)			
{
	if (pbCapture == NULL ||
		dwSrcDataSize2 == 0 )
	{
		LARGE_INTEGER liTemp = { 0 };
		IStream *pStream1 = NULL, *pStreamOut = NULL;

		CreateStreamOnHGlobal(NULL, TRUE, &pStream1);
		CreateStreamOnHGlobal(NULL, TRUE, &pStreamOut);

		if (pStream1 != NULL &&
			pStreamOut != NULL)
		{
			pStream1->Seek(liTemp, STREAM_SEEK_SET, NULL);
			pStream1->Write(pbSnapShot, dwSrcDataSize1, NULL);
			Bitmap InputBitmap1(pStream1);

			if (InputBitmap1.GetLastStatus() == Ok)
			{
				WORD uWidth, uBmp1Width = InputBitmap1.GetWidth();
				WORD uHight, uBmp1Hight = InputBitmap1.GetHeight();
				uWidth = uBmp1Width;
				uHight = uBmp1Hight;
				Bitmap OutputBitmap(uWidth, uHight, InputBitmap1.GetPixelFormat());
				if(OutputBitmap.GetLastStatus() == Ok)
				{
					Graphics gr( &OutputBitmap );
					if (gr.GetLastStatus() == Ok)
					{
					//	Rect rect1(0, 0, uBmp1Width, uBmp1Hight);
					//	Rect rect2(uBmp1Width, 0, uBmp2Width, uBmp2Hight);

						Rect rect1(0, 0, uBmp1Width, uBmp1Hight);

						RectF rect;
						FontFamily fontFamily(L"宋体");
						SolidBrush brush(Color(255,0, 0));
						PointF	   point(10, 10);
						Font	font(&fontFamily, 21.0);
						CTime	tTime( dw64TimeMs/1000);
						CString	strText;
						strText.Format( "车牌:%s\n抓拍时间:%s\n桩号:%s\n行驶方向:%s",
							PlateNo,
							tTime.Format( "%Y-%m-%d %H:%M:%S" ),
							StationNo,
							TravelDirection);
						CStringW wstrText(strText);
						
						gr.DrawImage(&InputBitmap1, rect1, 0, 0, uBmp1Width, uBmp1Hight, UnitPixel, NULL, NULL, NULL);
						gr.MeasureString(wstrText, -1, &font, point, &rect);
						gr.FillRectangle(&SolidBrush(Color(150, 255, 255, 255)), rect);
						gr.DrawString(wstrText, -1, &font, point, &brush);
						pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);
						OutputBitmap.Save(pStreamOut, &g_jpgClsid, NULL);

						ULARGE_INTEGER uiSize;
						pStreamOut->Seek(liTemp, STREAM_SEEK_CUR, &uiSize);

						if (*dwDstDataSize > uiSize.LowPart)
						{
							*dwDstDataSize = uiSize.LowPart;
						}
						*wWidth = uWidth;
						*wHight = uHight;
						pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);			
						pStreamOut->Read(pbDstImageStream, *dwDstDataSize, NULL);
					}
					if (pStream1)
					{
						pStream1->Release();
					}
					if (pStreamOut)
					{
						pStreamOut->Release();
					}
				}
			}
		}
	}
	else
	{

		LARGE_INTEGER liTemp = { 0 };
		IStream *pStream1 = NULL, *pStream2 = NULL, *pStreamOut = NULL;

		CreateStreamOnHGlobal(NULL, TRUE, &pStream1);
		CreateStreamOnHGlobal(NULL, TRUE, &pStream2);
		CreateStreamOnHGlobal(NULL, TRUE, &pStreamOut);

		if (pStream1 != NULL &&
			pStream2 != NULL &&
			pStreamOut != NULL)
		{
			pStream1->Seek(liTemp, STREAM_SEEK_SET, NULL);
			pStream1->Write(pbSnapShot, dwSrcDataSize1, NULL);
			Bitmap InputBitmap1(pStream1);

			pStream2->Seek(liTemp, STREAM_SEEK_SET, NULL);
			pStream2->Write(pbCapture, dwSrcDataSize2, NULL);
			Bitmap InputBitmap2(pStream2);

			if ((InputBitmap1.GetLastStatus() == Ok) &&
				(InputBitmap2.GetLastStatus() == Ok))
			{
				WORD uWidth, uBmp1Width = InputBitmap1.GetWidth(), uBmp2Width = InputBitmap2.GetWidth();
				WORD uHight, uBmp1Hight = InputBitmap1.GetHeight(), uBmp2Hight = InputBitmap2.GetHeight();
				uWidth = uBmp1Width;// + uBmp2Width;
				uHight = uBmp1Hight;

				Bitmap OutputBitmap(uWidth, uHight, InputBitmap1.GetPixelFormat());
				if (OutputBitmap.GetLastStatus() == Ok)
				{
					Graphics gr( &OutputBitmap );
					if (gr.GetLastStatus() == Ok)
					{
					//	Rect rect1(0, 0, uBmp1Width, uBmp1Hight);
					//	Rect rect2(uBmp1Width, 0, uBmp2Width, uBmp2Hight);

						Rect rect1(0, 0, uBmp1Width, uBmp1Hight);
						Rect rect2(10, 147, 412, 100);

						RectF rect;
						FontFamily fontFamily(L"宋体");
						SolidBrush brush(Color(255,0, 0));
						PointF	   point(10, 10);
						Font	font(&fontFamily, 21.0);
						CTime	tTime( dw64TimeMs/1000);
						CString	strText;
						strText.Format( "车牌:%s\n抓拍时间:%s\n桩号:%s\n行驶方向:%s",
							PlateNo,
							tTime.Format( "%Y-%m-%d %H:%M:%S" ),
							StationNo,
							TravelDirection);
						CStringW wstrText(strText);
						
						gr.DrawImage(&InputBitmap1, rect1, 0, 0, uBmp1Width, uBmp1Hight, UnitPixel, NULL, NULL, NULL);
						gr.DrawImage(&InputBitmap2, rect2, 0, 0, uBmp2Width, uBmp2Hight, UnitPixel, NULL, NULL, NULL);
						gr.MeasureString(wstrText, -1, &font, point, &rect);
						gr.FillRectangle(&SolidBrush(Color(150, 255, 255, 255)), rect);
						gr.DrawString(wstrText, -1, &font, point, &brush);
						pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);
						OutputBitmap.Save(pStreamOut, &g_jpgClsid, NULL);

						ULARGE_INTEGER uiSize;
						pStreamOut->Seek(liTemp, STREAM_SEEK_CUR, &uiSize);

						if (*dwDstDataSize > uiSize.LowPart)
						{
							*dwDstDataSize = uiSize.LowPart;
						}
						*wWidth = uWidth;
						*wHight = uHight;
						pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);			
						pStreamOut->Read(pbDstImageStream, *dwDstDataSize, NULL);
					}
				}
			}
		}
		if (pStream1)
		{
			pStream1->Release();
		}
		if (pStream2)
		{
			pStream2->Release();
		}
		if (pStreamOut)
		{
			pStreamOut->Release();
		}
	}

	
//	LeaveCriticalSection(&g_csTransImage);
	return true;
}
