#pragma once
#include <afxtempl.h>
#include <atlstr.h>
#include <afxmt.h>
#include "HvDeviceNew.h"
//#include "hvthreadbase.h"
#include "DiskManager.h"
#include "afxmt.h"


#define MAX_OBJ_TYPE	20
#define MAX_RECORD_COUNT	100

typedef struct _SINGLE_HVE_ELEMENT
{
	CString strAddr;	//HVE IP地址
	int iLocationId;	//所在路段ID
	int rgiRoadNo[10];	//所覆盖的车道号
	_SINGLE_HVE_ELEMENT()
	{
		iLocationId = -1;
		for(int i=0; i<10; i++)
		{
			rgiRoadNo[i] = -1;
		}
	}
}SINGLE_HVE_ELEMENT;

typedef struct _RESULT_ELEMENT
{
	DWORD64		dw64CreateTimeMs; //识别结果生成时间 用于判断长时间无车牌来时 将结果写入数据库 
	int			iLocationId;
	DWORD64		dw64TimeMs;
	char		szPlateNo[20];
	int			iRoadNo;
	char        szRoadName[25];
	char        szRoadDirect[10];
	int			iCarSpeed;
	int			iObjId;
	int			iTimeHeadway;
	int			iOverLine;
	int			iCorssLine;
	int			iEmergencyStop;
	int			iReverseRun;
	int			iSpeeding;	//该成员为需求添加进的变量 是否超速
	int			iSpecialResult;
	char		szBigImagePath_1[256];
	char		szBigImagePath_2[256];
	char		szBigImagePath_3[256];
	char		szBigImagePath_4[256];
	char		szBigImagePath_5[256];
	char		szSmallImgePath[256];
	char		szBinImagePath[256];
	char		szVideoPath[256];
	_RESULT_ELEMENT()
	{
		memset(this, 0, sizeof(_RESULT_ELEMENT));
	}
}RESULT_ELEMENT;

typedef struct _ImageInfo
{
	WORD		wImageWidth;
	WORD		wImageHeight;
	WORD		wType;
	DWORD		dwSize;
	PBYTE		pbImage;
	CString		strSavePath;
	CString		strFileDir;
	_ImageInfo()
	{
		wImageWidth = 0;
		wImageHeight = 0;
	    wType = 0;
		dwSize = 0;
		pbImage = 0;
		strcpy(strSavePath.GetBuffer(),"");
		strcpy(strFileDir.GetBuffer(),"");
	}
}ImageInfo;


typedef struct _RECORD_INFO
{
	DWORD			dwCarID;
	LPCSTR			pcPlateNo;
	LPCSTR			pcAppendInfo;
	DWORD64			dw64TimeMs;
	ImageInfo*		pBigImage_1;
	ImageInfo*		pBigImage_2;
	ImageInfo*		pBigImage_3;
	ImageInfo*		pBigImage_4;
	ImageInfo*		pBigImage_5;
	ImageInfo*		pSmallImge;
	ImageInfo*		pBinImage;
	PBYTE			pbVideo;
	_RECORD_INFO()
	{
		memset(this, 0, sizeof(_RECORD_INFO));
	}
}RECORD_INFO;

class CHveConnecter
{
public:
	CHveConnecter(SINGLE_HVE_ELEMENT* pSingleHveEleMent, void* pPlateFilter, CDiskManager* DiskManager);
	~CHveConnecter(void);

	static int OnInfoBeginProxy(void* pFirstParameter, DWORD dwCarID)
	{
		if(pFirstParameter == NULL)
		{
			return 0;
		}
		return ((CHveConnecter*)pFirstParameter)->OnInfoBegin(dwCarID);
	}

	static int OnInfoEndProxy(void* pFirstParameter, DWORD dwCarID)
	{
		if(pFirstParameter == NULL)
		{
			return 0;
		}
		return ((CHveConnecter*)pFirstParameter)->OnInfoEnd(dwCarID);
	}

	static int OnPlateProxy(void* pFirstParameter, DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo,DWORD dwRecordType, DWORD64 dw64TimeMs)
	{
		if(pFirstParameter == NULL)
		{
			return 0;
		}
		return ((CHveConnecter*)pFirstParameter)->OnPlate(dwCarID, pcPlateNo,pcAppendInfo,dw64TimeMs);
	}
	static int OnSmallImageProxy(void *pFirstParameter, DWORD dwCarID, WORD wImageWidth, WORD wImageHigh,
		PBYTE pbImage, DWORD dwSize,DWORD dwRecordType, DWORD64 dw64TimeMs)
	{
		if(pFirstParameter == NULL)
		{
			return 0;
		}
		return ((CHveConnecter*)pFirstParameter)->OnSmallImage(dwCarID, wImageWidth, wImageHigh, dwSize, pbImage, dw64TimeMs);
	}
	static int OnBinaryImageProxy(void *pFirstParameter, DWORD dwCarID, WORD wImageWidth, WORD wImageHigh,
		PBYTE pbImage, DWORD dwSize, DWORD dwRecordType, DWORD64 dw64TimeMs)
	{
		if(pFirstParameter == NULL)
		{
			return 0;
		}
		return ((CHveConnecter*)pFirstParameter)->OnBinaryImage(dwCarID, wImageWidth, wImageHigh, dwSize, pbImage, dw64TimeMs);
	}
	static int OnBigImageProxy(void *pFirstParameter, DWORD dwCarID, WORD wType, WORD wImageWidth, WORD wImageHigh,
		PBYTE pbImage, DWORD dwSize, DWORD dwRecordType, DWORD64 dw64TimeMs)
	{
		if(pFirstParameter == NULL)
		{
			return 0;
		}
		return ((CHveConnecter*)pFirstParameter)->OnBigImage(dwCarID, wImageWidth, wImageHigh, wType,
			dwSize, pbImage, dw64TimeMs);
	}
private:
	int m_iBigImageID;
	int m_iCurCarID;
	void* m_pPlateFilter;
	RESULT_ELEMENT m_curResult;
	DWORD64 m_dwLastCarriveTick[10];
//	char m_ImageSavePath[MAX_PATH];
	CDiskManager* m_DiskManager;
public:
	HVAPI_HANDLE_EX m_hHv;
	SINGLE_HVE_ELEMENT* m_pSingleHveEleMent;

	void SaveLog(char* pszLog);
	HRESULT	SetCallBack();	
	HRESULT GetAppendInfoElement(CString strAppendInfo);

	int OnPlate(DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD64 dw64TimeMs);
	
	int OnSmallImage(DWORD dwCarID, WORD wImageWidth, WORD wImageHeight, 
		DWORD dwSize, PBYTE pbImage, DWORD64 dw64TimeMs);

	int OnBinaryImage(DWORD dwCarID, WORD wImageWidth, WORD wImageHeight, 
		DWORD dwSize, PBYTE pbImage, DWORD64 dw64TimeMs);

	int OnBigImage(DWORD dwCarID, WORD wImageWidth, WORD wImageHeight, 
		WORD wType, DWORD dwSize, PBYTE pbImage, DWORD64 dw64TimeMs);


//	int GetImageSavePath(DWORD dwCarID, DWORD64 dw64TimeMs);

	int OnInfoBegin(DWORD dwCarID);
	int OnInfoEnd(DWORD dwCarID);
//	BOOL SaveToBuf(LPSTR szSaveName, PBYTE pbImage, DWORD32 dwImgSize, CString& strSavePath);
private:
	RECORD_INFO m_curRecord;
	CList<RECORD_INFO> m_pListRecord;
	CCriticalSection m_csRecord;
public:
	static DWORD WINAPI  AnalyseRecordThread(LPVOID lpParameter);
	static DWORD WINAPI  SaveImageThread(LPVOID lpParameter);
	int AnalyseRecord(RECORD_INFO* cRecord);
	int OnAnalysePlate(RECORD_INFO* record);

	int OnAnalyseSmallImage(RECORD_INFO* cRecord);
	int OnAnalyseBinaryImage(RECORD_INFO* cRecord);
	int OnAnalyseBigImage(RECORD_INFO* cRecord);
	int SaveBigImage(RECORD_INFO* record,ImageInfo* pImageInfo);
private:
	CList<ImageInfo *> m_ListImageInfo;
	CCriticalSection m_csImageInfo;
	bool HV_CombineTwoImagesToOneImage(
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
									  DWORD64 dw64TimeMs);    		//时间)
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
public:
	BYTE *m_pbSmallImage;   //用于保存小图数据 防止小图入队保存后内存释放 导致图片叠加是获取的小图指针为非法地址
};

