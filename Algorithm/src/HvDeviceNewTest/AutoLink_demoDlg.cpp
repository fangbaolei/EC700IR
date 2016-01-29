// AutoLink_demoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <DbgHelp.h>
#include "AutoLink_demo.h"
#include "AutoLink_demoDlg.h"
#include ".\autolink_demodlg.h"

#include "tinyxml.h"
#include "tinystr.h"

#include "AutoLinkParam.h"

#include "HvDeviceNew.h"
#include "HvAutoLinkDevice.h"


#ifdef _DEBUG
#pragma comment(lib, "..\\HvDeviceDLL\\Debug\\HvDevice.lib")
#else
#pragma comment(lib, "..\\HvDeviceDLL\\Release\\HvDevice.lib")
#endif
#pragma comment(lib ,"Dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_DEV_SN_NUM 128

typedef struct tag_RecordCallBackInfo 
{
	char szDevSN[MAX_DEV_SN_NUM]; //设备信息
	DWORD   dwCurRecordCarID;	  //当前CarId
	DWORD64 dw64CurReocrdTime;    //当前结果时间
	int     iCurStatus;		     //-1 ：回调结束 ，0： 回调开始

	tag_RecordCallBackInfo()
	{
		memset(szDevSN , 0 , sizeof(szDevSN));
		dw64CurReocrdTime = 0;
		dwCurRecordCarID = 0;
		iCurStatus = -1;
	};
}RECORD_CALLBACK_INFO;

#define MAX_RECORD_CALLBACK_INFO_NUM 50
static RECORD_CALLBACK_INFO g_rgRecordCallBackInfo[MAX_RECORD_CALLBACK_INFO_NUM];
static INT g_iRecordCallBackInfoNum = 0;

typedef struct tagSTime
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMin;
	int nSec;
	int nMs;
} STime;

CString g_strDir;
bool g_fSaveFile = true;


#define DEV_STATUS_CONN		"连接"
#define DEV_STATUS_RECONN	"重连"
#define DEV_STATUS_DISCONN  "断开"
#define DEV_STATUS_DONE     "完成"
#define DEV_STATUS_UNKNOW   "未知"
#define DEV_STATUS_FIRSTCONN   "初始连接"
#define DEV_STATUS_OVERTIMECONN      "连接超时"


HRESULT WriteRecordCallBackInfo(RECORD_CALLBACK_INFO* pRecordCallBackInfo)
{
	if ( NULL == pRecordCallBackInfo )
	{
		return E_FAIL;
	}
	bool fFind = false;
	for (int i = 0 ; i< g_iRecordCallBackInfoNum ;++i)
	{
		if(0 == strcmp( g_rgRecordCallBackInfo[i].szDevSN ,pRecordCallBackInfo->szDevSN ) )
		{
			fFind = true;
			memcpy((void*)&g_rgRecordCallBackInfo[i] , pRecordCallBackInfo , sizeof(RECORD_CALLBACK_INFO));
			break;
		}
	}

	if (fFind)
	{
		return S_OK;
	}

	if ( g_iRecordCallBackInfoNum < MAX_RECORD_CALLBACK_INFO_NUM - 1 )
	{
		memcpy((void*)&g_rgRecordCallBackInfo[g_iRecordCallBackInfoNum++] ,pRecordCallBackInfo , sizeof(RECORD_CALLBACK_INFO));
	}

	return E_FAIL;
}

HRESULT ReadReocrdCallBackInfo( char* szDevSN ,RECORD_CALLBACK_INFO* pRecordCallBackInfo )
{
	if ( NULL == szDevSN || NULL ==  pRecordCallBackInfo )
	{
		return E_FAIL;
	}
	bool fFind = false;
	for (int i = 0 ; i< g_iRecordCallBackInfoNum ;++i)
	{
		if(0 == strcmp( g_rgRecordCallBackInfo[i].szDevSN ,szDevSN ) )
		{
			fFind = true;
			memcpy( pRecordCallBackInfo ,  (void*)&g_rgRecordCallBackInfo[i] , sizeof(RECORD_CALLBACK_INFO));
			break;
		}
	}

	if (fFind)
	{
		return S_OK;
	}
	return E_FAIL;
}

int GetTimeStrBySTime(const STime *pStime , CString &strTime ,const char* pType)
{
	CString strType(pType);
	CString strTemp;

	strTemp.Format("%d" ,pStime->nYear);
	strType.Replace("yyyy" ,strTemp);

	strTemp.Format("%2d" , pStime->nMonth);
	strTemp.Replace(' ' , '0');
	strType.Replace("mm",strTemp);

	strTemp.Format("%2d" , pStime->nDay);
	strTemp.Replace(' ' , '0');
	strType.Replace("dd",strTemp);

	strTemp.Format("%2d" ,pStime->nHour);
	strTemp.Replace(' ' , '0');
	strType.Replace("hh",strTemp);

	strTemp.Format("%2d" , pStime->nMin);
	strTemp.Replace(' ' , '0');
	strType.Replace("mi" , strTemp);

	strTemp.Format("%2d" , pStime->nSec);
	strTemp.Replace(' ' , '0');
	strType.Replace("ss",strTemp);

	strTemp.Format("%2d" , pStime->nMs);
	strTemp.Replace(' ' , '0');
	strType.Replace("ms",strTemp);

	strTime = strType;

	return 0;
}

int GetSTimeByDWORD64(STime &stime , DWORD64 dw64Time)
{
	CTime realtime(dw64Time/1000);
	time_t sec = (time_t)dw64Time/1000;
	if (sec<0)
	{
		sec = 0;
	}
	stime.nMs = (int)(dw64Time%1000);
//	struct tm *ts= localtime(&sec);

	stime.nYear = realtime.GetYear();
	stime.nMonth = realtime.GetMonth();
	stime.nDay = realtime.GetDay();
	stime.nHour = realtime.GetHour();
	stime.nMin = realtime.GetMinute();
	stime.nSec = realtime.GetSecond();

	return 0;

}


int FileSave(CString strFilePath ,CString strFileName, char* pFileData , int nFileDataLen, bool bTruncate)
{
	HRESULT hr = S_OK;
	static bool fFileSaveException = false;
	CFile file;

	TRY
	{
		if(!MakeSureDirectoryPathExists(strFilePath.GetBuffer()))
		{
			hr = E_FAIL;
		}
		strFilePath.ReleaseBuffer();

		if (S_OK == hr)
		{
			if (!bTruncate)
			{
				if(!file.Open(strFilePath + strFileName ,CFile:: modeWrite | CFile::modeCreate | CFile::modeNoTruncate))
				{
					hr = E_FAIL;
				}
			}
			else
			{
				if(!file.Open(strFilePath + strFileName ,CFile:: modeWrite | CFile::modeCreate))
				{
					hr = E_FAIL;
				}
			}
		}

		if (S_OK == hr)
		{
			file.SeekToEnd();
			file.Write(pFileData , nFileDataLen);
			fFileSaveException = false;
		}
	}
	CATCH( CFileException, pEx )
	{
		// Simply show an error message to the user.
		
		if (!fFileSaveException)
		{
			pEx->ReportError();
		}
		
		fFileSaveException = true;
	}
	AND_CATCH(CMemoryException, pEx)
	{
		fFileSaveException = true;
		AfxAbort( );
	}
	END_CATCH


	if (S_OK == hr)
	{
		file.Close();
	}

	
	return 0;
}
typedef BOOL (WINAPI *PGETDISKFREESPACEEX)(LPCSTR,PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);


HRESULT Hv_GetDiskFreeSpace(LPCSTR pszDrive ,__int64& iFreeBytes )
{
	PGETDISKFREESPACEEX pGetDiskFreeSpaceEx;
	__int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;
	DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
	BOOL fResult;

	pGetDiskFreeSpaceEx = (PGETDISKFREESPACEEX)GetProcAddress( GetModuleHandle("kernel32.dll"),
		"GetDiskFreeSpaceExA");

	if (pGetDiskFreeSpaceEx)
	{
		fResult = pGetDiskFreeSpaceEx (pszDrive,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);

		if( fResult )
		{	
			iFreeBytes = i64FreeBytes;
			return S_OK;
		}
	}
	else 
	{
		fResult = GetDiskFreeSpace (pszDrive, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);

		if(fResult)
		{
			iFreeBytes = dwFreeClusters*dwSectPerClust*dwBytesPerSect;
			return S_OK;
		}
	}

	return E_FAIL;
}

// CAutoLink_demoDlg 对话框

int FILE_OP_WriteDataToFile(const char* szFileName, const unsigned char* pbData, unsigned long ulDataSize)  
{  
	unsigned long ulWritedSize = 0;  
	FILE *fp = fopen(szFileName, "wb");  
	if ( fp )  
	{  
		ulWritedSize = (unsigned long)fwrite(pbData, 1, ulDataSize, fp);  
		fclose(fp);  
		return ( ulWritedSize == ulDataSize ) ? 0 : -1;  
	}  
	return -1;  
}  

int FILE_OP_AppendDataToFile(const char* szFileName, const unsigned char* pbData, unsigned long ulDataSize)  
{  
	unsigned long ulWritedSize = 0;  
	FILE *fp = fopen(szFileName, "awb");  
	if ( fp )  
	{  
		ulWritedSize = (unsigned long)fwrite(pbData, 1, ulDataSize, fp);  
		fclose(fp);  
		return ( ulWritedSize == ulDataSize ) ? 0 : -1;  
	}  
	return -1;  
}  


CAutoLink_demoDlg::CAutoLink_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoLink_demoDlg::IDD, pParent)
	, m_hServerHandle(NULL)
	, m_hShowThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoLink_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_devListCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_autoLinkPortEdit);
	DDX_Control(pDX, IDC_EDIT2, m_maxListenNumEdit);
	DDX_Control(pDX, IDC_BUTTON11, m_openAutoLinkBut);
	DDX_Control(pDX, IDC_BUTTON12, m_closeAutoLinkBut);
}

BEGIN_MESSAGE_MAP(CAutoLink_demoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton2)
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_FIND_DIR, OnBnClickedFindDir)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDBClick)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, OnRClickDevList)
	ON_COMMAND(ID__RECV_HISTORY_CONFIG, OnSetConfig)
END_MESSAGE_MAP()


// CAutoLink_demoDlg 消息处理程序

BOOL CAutoLink_demoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitDevListCtrl();

	m_maxListenNumEdit.SetWindowText("50");
	m_autoLinkPortEdit.SetWindowText("6665");
	((CEdit*)GetDlgItem(IDC_DIR))->SetWindowText("E:\\");
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoLink_demoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
/*
void ProcBigImage(CString strPath , CString strViolation , CString strTime, CString strCarId , PBYTE pbRecordData, DWORD dwLen, int nAddMode, BOOL fIsPeccancy)
{
	
	RECORD_IMAGE_GROUP cImageGroup;
	if(HVAPIUTILS_GetRecordImageAll(pbRecordData, dwLen, &cImageGroup) != S_OK)
	{
		return;
	}

	int nImageNum = 0;
	if(cImageGroup.pbImgDataBestSnapShot != NULL)
	{
		if((nAddMode == 1 ||
			nAddMode == 2 && fIsPeccancy == TRUE) && FALSE)
		{
			PBYTE pbImgChange = new BYTE[1024 << 11];
			if(pbImgChange != NULL)
			{
				int nBufferLen = (1024 << 11);
				memset(pbImgChange, 0, nBufferLen);
				int nRedLightCount = 0;
				PBYTE pbRedLightPos = new BYTE[20*sizeof(RECT)];
				if(pbRedLightPos != NULL)
				{
					RECORD_IMAGE cImg;
					cImg.cImgInfo = cImageGroup.cImgInfoBestSnapshot;
					cImg.pbImgData = cImageGroup.pbImgDataBestSnapShot;
					cImg.pbImgInfoEx = cImageGroup.pbImgInfoBestSnapShot;
					cImg.dwImgDataLen = cImageGroup.dwImgDataBestSnapShotLen;
					cImg.dwImgInfoLen = cImageGroup.dwImgInfoBestSnapShotLen;
					if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cImg, 20*sizeof(RECT), pbRedLightPos, nRedLightCount) == S_OK)
					{
						if(HVAPIUTILS_TrafficLightEnhance(cImageGroup.pbImgDataBestSnapShot, cImageGroup.dwImgDataBestSnapShotLen, nRedLightCount, pbRedLightPos, pbImgChange, nBufferLen, 0, 120, 80) == S_OK)
						{
							CString strFileNew;
							if(fIsPeccancy)
								strFileNew.Format(".\\HvApiDllTestResults\\违章结果\\%d_BestSnapShot.jpg", cImageGroup.cImgInfoBestSnapshot.dwCarID);
							else
								strFileNew.Format(".\\HvApiDllTestResults\\%d_BestSnapShot.jpg", cImageGroup.cImgInfoBestSnapshot.dwCarID);
							FILE_OP_WriteDataToFile(strFileNew.GetBuffer(), pbImgChange, nBufferLen);
						}
					}
					delete[] pbRedLightPos;
				}
				delete[] pbImgChange;
				pbImgChange = NULL;
			}
		}
		else
		{		
			CString strFileName;
			CString strImageNum;
			strImageNum.Format("%d",nImageNum);
			nImageNum++;
			strImageNum.Replace(' ' , '0');
			strFileName.Format("%s-%s-%s.jpg" ,strTime,strCarId,strImageNum );

			if(fIsPeccancy)
			{
				FileSave(strViolation , strFileName , (char*)cImageGroup.pbImgDataBestSnapShot , cImageGroup.dwImgDataBestSnapShotLen , true );
			}
			else
			{
				FileSave(strPath , strFileName ,(char*)cImageGroup.pbImgDataBestSnapShot , cImageGroup.dwImgDataBestSnapShotLen , true );
			}
				
			
		}
	}


	if(cImageGroup.pbImgDataLastSnapShot != NULL)
	{
		if((nAddMode == 1 ||
			(nAddMode == 2 && fIsPeccancy == TRUE )&& FALSE))
		{
			PBYTE pbImgChange = new BYTE[1024 << 11];
			if(pbImgChange != NULL)
			{
				int iBufferLen = (1024 << 11);
				memset(pbImgChange, 0, iBufferLen);
				int iRedLightCount = 0;
				PBYTE pbRedLightPos = new BYTE[20*sizeof(RECT)];
				if(pbRedLightPos != NULL)
				{
					RECORD_IMAGE cImg;
					cImg.cImgInfo = cImageGroup.cImgInfoLastSnapshot;
					cImg.pbImgData = cImageGroup.pbImgDataLastSnapShot;
					cImg.pbImgInfoEx = cImageGroup.pbImgInfoLastSnapShot;
					cImg.dwImgDataLen = cImageGroup.dwImgDataLastSnapShotLen;
					cImg.dwImgInfoLen = cImageGroup.dwImgInfoLastSnapShotLen;
					if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cImg, 20*sizeof(RECT), pbRedLightPos, iRedLightCount) == S_OK)
					{
						if(HVAPIUTILS_TrafficLightEnhance(cImageGroup.pbImgDataLastSnapShot, cImageGroup.dwImgDataLastSnapShotLen, iRedLightCount, pbRedLightPos, pbImgChange, iBufferLen, 0, 120, 80) == S_OK)
						{
							CString strFileNew;
							if(fIsPeccancy)
								strFileNew.Format(".\\HvApiDllTestResults\\违章结果\\%d_LastSnapShot.jpg", cImageGroup.cImgInfoLastSnapshot.dwCarID);
							else
								strFileNew.Format(".\\HvApiDllTestResults\\%d_LastSnapShot.jpg", cImageGroup.cImgInfoLastSnapshot.dwCarID);
							FILE_OP_WriteDataToFile(strFileNew.GetBuffer(), pbImgChange, iBufferLen);
						}
					}
					delete[] pbRedLightPos;
				}
				delete[] pbImgChange;
				pbImgChange = NULL;
			}
		}
		else
		{
			CString strFileName;
			CString strImageNum;
			strImageNum.Format("%d",nImageNum);
			nImageNum++;
			strImageNum.Replace(' ' , '0');
			strFileName.Format("%s-%s-%s.jpg" ,strTime,strCarId,strImageNum );

			if(fIsPeccancy)
			{
				FileSave(strViolation , strFileName ,(char*)cImageGroup.pbImgDataLastSnapShot , cImageGroup.dwImgDataLastSnapShotLen , true );
			}
			else
			{
				FileSave(strPath , strFileName ,(char*)cImageGroup.pbImgDataLastSnapShot , cImageGroup.dwImgDataLastSnapShotLen , true );
			}
		}
	}


	if(cImageGroup.pbImgDataBeginCapture != NULL)
	{
		if((nAddMode == 1 ||
			(nAddMode == 2 && fIsPeccancy == TRUE) ||
			nAddMode == 3 )&& FALSE)
		{
			PBYTE pbImgChange = new BYTE[1024 << 11];
			if(pbImgChange != NULL)
			{
				int iBufferLen = (1024 << 11);
				memset(pbImgChange, 0, iBufferLen);
				int iRedLightCount = 0;
				PBYTE pbRedLightPos = new BYTE[20*sizeof(RECT)];
				if(pbRedLightPos != NULL)
				{
					RECORD_IMAGE cImg;
					cImg.cImgInfo = cImageGroup.cImgInfoBeginCapture;
					cImg.pbImgData = cImageGroup.pbImgDataBeginCapture;
					cImg.pbImgInfoEx = cImageGroup.pbImgInfoBeginCapture;
					cImg.dwImgDataLen = cImageGroup.dwImgDataBeginCaptureLen;
					cImg.dwImgInfoLen = cImageGroup.dwImgInfoBeginCaptureLen;
					if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cImg, 20*sizeof(RECT), pbRedLightPos, iRedLightCount) == S_OK)
					{
						if(HVAPIUTILS_TrafficLightEnhance(cImageGroup.pbImgDataBeginCapture, cImageGroup.dwImgDataBeginCaptureLen, iRedLightCount, pbRedLightPos, pbImgChange, iBufferLen, 0, 120, 80) == S_OK)
						{
							CString strFileNew;
							if(fIsPeccancy)
								strFileNew.Format(".\\HvApiDllTestResults\\违章结果\\%d_BeginCapture.jpg", cImageGroup.cImgInfoBeginCapture.dwCarID);
							else
								strFileNew.Format(".\\HvApiDllTestResults\\%d_BeginCapture.jpg", cImageGroup.cImgInfoBeginCapture.dwCarID);
							FILE_OP_WriteDataToFile(strFileNew.GetBuffer(), pbImgChange, iBufferLen);
						}
					}
					delete[] pbRedLightPos;
				}
				delete[] pbImgChange;
				pbImgChange = NULL;
			}
		}
		else
		{
			CString strFileName;
			CString strImageNum;
			strImageNum.Format("%d",nImageNum);
			nImageNum++;
			strImageNum.Replace(' ' , '0');
			strFileName.Format("%s-%s-%s.jpg" ,strTime,strCarId,strImageNum );

			if(fIsPeccancy)
			{
				FileSave(strViolation , strFileName ,(char*)cImageGroup.pbImgDataBeginCapture , cImageGroup.dwImgDataBeginCaptureLen , true );
			}
			else
			{
				FileSave(strPath , strFileName ,(char*)cImageGroup.pbImgDataBeginCapture , cImageGroup.dwImgDataBeginCaptureLen , true );
			}
		}
	}

	if(cImageGroup.pbImgDataBestCapture != NULL)
	{
		if((nAddMode == 1 ||
			(nAddMode == 2 && fIsPeccancy == TRUE) ||
			nAddMode == 3)&& FALSE)
		{
			PBYTE pbImgChange = new BYTE[1024 << 11];
			if(pbImgChange != NULL)
			{
				int iBufferLen = (1024 << 11);
				memset(pbImgChange, 0, iBufferLen);
				int iRedLightCount = 0;
				PBYTE pbRedLightPos = new BYTE[20*sizeof(RECT)];
				if(pbRedLightPos != NULL)
				{
					RECORD_IMAGE cImg;
					cImg.cImgInfo = cImageGroup.cImgInfoBestCapture;
					cImg.pbImgData = cImageGroup.pbImgDataBestCapture;
					cImg.pbImgInfoEx = cImageGroup.pbImgInfoBestCapture;
					cImg.dwImgDataLen = cImageGroup.dwImgDataBestCaptureLen;
					cImg.dwImgInfoLen = cImageGroup.dwImgInfoBestCaptureLen;
					if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cImg, 20*sizeof(RECT), pbRedLightPos, iRedLightCount) == S_OK)
					{
						if(HVAPIUTILS_TrafficLightEnhance(cImageGroup.pbImgDataBestCapture, cImageGroup.dwImgDataBestCaptureLen, iRedLightCount, pbRedLightPos, pbImgChange, iBufferLen, 0, 120, 80) == S_OK)
						{
							CString strFileNew;
							if(fIsPeccancy)
								strFileNew.Format(".\\HvApiDllTestResults\\违章结果\\%d_BestCapture.jpg", cImageGroup.cImgInfoBestCapture.dwCarID);
							else
								strFileNew.Format(".\\HvApiDllTestResults\\%d_BestCapture.jpg", cImageGroup.cImgInfoBestCapture.dwCarID);
							FILE_OP_WriteDataToFile(strFileNew.GetBuffer(), pbImgChange, iBufferLen);
						}
					}
					delete[] pbRedLightPos;
				}
				delete[] pbImgChange;
				pbImgChange = NULL;
			}
		}
		else
		{
			CString strFileName;
			CString strImageNum;
			strImageNum.Format("%d",nImageNum);
			nImageNum++;
			strImageNum.Replace(' ' , '0');
			strFileName.Format("%s-%s-%s.jpg" ,strTime,strCarId,strImageNum );

			if(fIsPeccancy)
			{
				FileSave(strViolation , strFileName ,(char*)cImageGroup.pbImgDataBestCapture , cImageGroup.dwImgDataBestCaptureLen , true );
			}
			else
			{
				FileSave(strPath , strFileName ,(char*)cImageGroup.pbImgDataBestCapture , cImageGroup.dwImgDataBestCaptureLen , true );
			}
			
		}
	}

	if(cImageGroup.pbImgDataLastCapture != NULL)
	{
		if((nAddMode == 1 ||
			(nAddMode == 2 && fIsPeccancy == TRUE) ||
			nAddMode == 3) && FALSE)
		{
			PBYTE pbImgChange = new BYTE[1024 << 11];
			if(pbImgChange != NULL)
			{
				int iBufferLen = (1024 << 11);
				memset(pbImgChange, 0, iBufferLen);
				int iRedLightCount = 0;
				PBYTE pbRedLightPos = new BYTE[20*sizeof(RECT)];
				if(pbRedLightPos != NULL)
				{
					RECORD_IMAGE cImg;
					cImg.cImgInfo = cImageGroup.cImgInfoLastCapture;
					cImg.pbImgData = cImageGroup.pbImgDataLastCapture;
					cImg.pbImgInfoEx = cImageGroup.pbImgInfoLastCapture;
					cImg.dwImgDataLen = cImageGroup.dwImgDataLastCaptureLen;
					cImg.dwImgInfoLen = cImageGroup.dwImgInfoLastCaptureLen;
					if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cImg, 20*sizeof(RECT), pbRedLightPos, iRedLightCount) == S_OK)
					{
						if(HVAPIUTILS_TrafficLightEnhance(cImageGroup.pbImgDataLastCapture, cImageGroup.dwImgDataLastCaptureLen, iRedLightCount, pbRedLightPos, pbImgChange, iBufferLen, 0, 120, 80) == S_OK)
						{
							CString strFileNew;
							if(fIsPeccancy)
								strFileNew.Format(".\\HvApiDllTestResults\\违章结果\\%d_LastCapture.jpg", cImageGroup.cImgInfoLastCapture.dwCarID);
							else
								strFileNew.Format(".\\HvApiDllTestResults\\%d_LastCapture.jpg", cImageGroup.cImgInfoLastCapture.dwCarID);
							FILE_OP_WriteDataToFile(strFileNew.GetBuffer(), pbImgChange, iBufferLen);
						}
					}
					delete[] pbRedLightPos;
				}
				delete[] pbImgChange;
				pbImgChange = NULL;
			}
		}
		else
		{
			CString strFileName;
			CString strImageNum;
			strImageNum.Format("%d",nImageNum);
			nImageNum++;
			strImageNum.Replace(' ' , '0');
			strFileName.Format("%s-%s-%s.jpg" ,strTime,strCarId,strImageNum );

			if(fIsPeccancy)
			{
				FileSave(strViolation , strFileName ,(char*)cImageGroup.pbImgDataLastCapture , cImageGroup.dwImgDataLastCaptureLen , true );
			}
			else
			{
				FileSave(strPath , strFileName ,(char*)cImageGroup.pbImgDataLastCapture , cImageGroup.dwImgDataLastCaptureLen , true );
			}
			
		}
	}
	
}

*/
DWORD GetPlateNumByXmlExtInfo(char* szXmlExtInfo, char* szPlateNum,char* szPlateAdditionInfo ,char* szCarId , DWORD32* pdwTimeLow, DWORD32* pdwTimeHigh)
{
	BOOL fHadTimeInfo = FALSE;
	TiXmlDocument cXmlDoc;
	const TiXmlElement* pElementname;
	if ( cXmlDoc.Parse(szXmlExtInfo) )
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement )
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if ( pElementResultSet )
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if ( pElementResult )
				{
					const TiXmlElement* pElementPlateName = pElementResult->FirstChildElement("PlateName");
					if ( pElementPlateName )
					{
						if ( 0 == strcmp("无车牌", pElementPlateName->GetText()) )
						{
							if (NULL != szPlateNum)
							{
								strcpy(szPlateNum, "  无车牌");  // “无车牌的情况下需要在前面加上两个空格”
							}
							
							//m_fIsNoPlateCar = true;
						}
						else
						{
							if (NULL != szPlateNum)
							{
								//m_fIsNoPlateCar = false;
								strcpy(szPlateNum, pElementPlateName->GetText());
							}
						}

						// 识别结果时间
						pElementname = pElementResult->FirstChildElement("TimeLow");
						if (pElementname)
						{
							fHadTimeInfo = TRUE;
							sscanf(pElementname->Attribute("value"), "%u", pdwTimeLow);
						}

						pElementname = pElementResult->FirstChildElement("TimeHigh");
						if (pElementname)
						{
							sscanf(pElementname->Attribute("value"), "%u", pdwTimeHigh);
						}

						pElementname = pElementResult->FirstChildElement("CarID");
						if (pElementname)
						{
							if (NULL != szCarId)
							{
								strcpy(szCarId , pElementname->Attribute("value"));
							}
							
						}

						// 新XML附加信息中加入了"chnname"中文节点，方便以后拓展
						if (fHadTimeInfo)
						{
							const char *pszChnName;
							const char *pszValue;
							pElementname = pElementResult->FirstChildElement();
							while (pElementname)
							{
								pszChnName = pElementname->Attribute("chnname");
								pszValue = pElementname->Attribute("value");
								if (pszChnName && pszValue)
								{
									// 事件检测附加信息直接取value
									if (strstr(pszChnName, "事件检测") != NULL)
									{
										strcat(szPlateAdditionInfo, "\r\n");
										strcat(szPlateAdditionInfo, pszValue);
										char* pPeccancyType = strstr(pszValue,"违章:是");

										if (NULL != pPeccancyType)
										{
											char szPeccancyType[128];
											memset(szPeccancyType , 0 , 128);
											sscanf(pPeccancyType ,"违章:是<%s>\n"  ,szPeccancyType);
											//m_strPeccancyType.Format("违章:是<%s" , szPeccancyType );
										}
									}
									else
									{
										strcat(szPlateAdditionInfo, "\r\n");
										strcat(szPlateAdditionInfo, pszChnName);
										strcat(szPlateAdditionInfo, ":");
										strcat(szPlateAdditionInfo, pszValue);
										if (0 == strcmp(pszChnName,"车道"))
										{
											//m_strRoad.Format("%s" , pszValue );
										}
										else if (0 == strcmp(pszChnName,"路口名称"))
										{
											//m_strSection.Format("%s" , pszValue);
										}
										else if (0 == strcmp(pszChnName,"路口方向"))
										{
											//m_strDirection.Format("%s" , pszValue);
										}
										else if (0 == strcmp(pszChnName,"摄像机亮度等级"))
										{
											//m_strLightState.Format("摄像机亮度等级: %s" , pszValue);
										}
									}
								}
								pElementname = pElementname->NextSiblingElement();
							}
						}
						else // 不包含chnname中文节点则需要逐个进行解析
						{
							//-------------添加CarId 获取-------------------------------
							pElementname = pElementResult->FirstChildElement("CarID");
							if (pElementname)
							{
								strcpy(szCarId, pElementname->Attribute("value"));
							}
							//----------------------------------------------------------

							pElementname = pElementResult->FirstChildElement("ReverseRun");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车辆逆向行驶:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("VideoScaleSpeed");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "视频测速:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("SpeedLimit");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "限速值:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("ScaleSpeedOfDistance");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "距离测量误差比例:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("ObservedFrames");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "有效帧数:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("Confidence");
							if ( pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "平均可信度:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("FirstCharConf");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "首字可信度:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("CarArriveTime");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车辆检测时间:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("CarType");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车辆类型:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("CarColor");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车身颜色:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("RoadNumber");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车道:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
								//m_strRoad.Format("%s" , pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("BeginRoadNumber");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "起始车道号:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("StreetName");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "路口名称:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
								//m_strSection.Format("%s" ,pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("StreetDirection");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "路口方向:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
								//m_strDirection.Format("%s" ,pElementname->Attribute("value"));

							}

							pElementname = pElementResult->FirstChildElement("EventCheck");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("FrameName");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "视频帧名:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("PlateLightType");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "摄相机亮度级别:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("AmbientLight");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "环境亮度:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("PlateLight");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车牌亮度:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("PlateVariance");
							if (pElementname)
							{
								strcat(szPlateAdditionInfo, "\r\n");
								strcat(szPlateAdditionInfo, "车牌对比度:");
								strcat(szPlateAdditionInfo, pElementname->Attribute("value"));
							}
						}
						return 0;
					}
				}
			}
		}
	}

	return -1;
}


HRESULT GetExInfo(LPCSTR szResultInfo, DWORD32& dwProceFrameTimes, DWORD32& dwTimeHight,
											  DWORD32& dwTimeLow, DWORD32& dwIndex)
{
	if(szResultInfo == NULL) return FALSE;
	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szResultInfo) )
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if(pRootElement)
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if(pElementResultSet)
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if(pElementResult)
				{
					const TiXmlElement* pElementname;
					pElementname = pElementResult->FirstChildElement("TimeHigh");
					if(pElementname)
					{
						dwTimeHight = atoi(pElementname->Attribute("value"));
					}

					pElementname = pElementResult->FirstChildElement("TimeLow");
					if(pElementname)
					{
						dwTimeLow = atoi(pElementname->Attribute("value"));
					}

					pElementname = pElementResult->FirstChildElement("ProcTime");
					if(pElementname)
					{
						dwProceFrameTimes = atoi(pElementname->Attribute("value"));
					}

					pElementname = pElementResult->FirstChildElement("CarID");
					if(pElementname)
					{
						dwIndex = atoi(pElementname->Attribute("value"));
					}
				}
			}
		}
	}
	return S_OK;
}
int OnRecordBegin(PVOID pUserData, DWORD dwCarID)
{
	if ( NULL == pUserData )
	{
		return -1;
	}
	RECORD_CALLBACK_INFO cRecordCallBackInfo;
	if ( strlen((char*)pUserData) < MAX_DEV_SN_NUM)
	{
		strcpy(cRecordCallBackInfo.szDevSN  ,(char*)pUserData );
	}
	else
	{
		return -1;
	}
	cRecordCallBackInfo.iCurStatus = 0;
	WriteRecordCallBackInfo(&cRecordCallBackInfo);
	return 0;
}

int OnRecordEnd(PVOID pUserData, DWORD dwCarID)
{
	if ( NULL == pUserData )
	{
		return -1;
	}
	RECORD_CALLBACK_INFO cRecordCallBackInfo;
	if ( strlen((char*)pUserData) < MAX_DEV_SN_NUM)
	{
		strcpy(cRecordCallBackInfo.szDevSN  ,(char*)pUserData );
	}
	else
	{
		return -1;
	}
	cRecordCallBackInfo.iCurStatus = -1;
	WriteRecordCallBackInfo(&cRecordCallBackInfo);
	return 0;
}
int OnBigImage(PVOID pUserData, DWORD dwCarID,  
			   WORD  wImgType, WORD  wWidth,
			   WORD  wHeight, PBYTE pbPicData,
			   DWORD dwImgDataLen,DWORD dwRecordType, 
			   DWORD64 dw64TimeMS)
{
	CString strMsg;
	CString strMsg2;
	RECORD_CALLBACK_INFO cRecordCallBackInfo;

	char szDevSN[128];
	ZeroMemory(szDevSN ,sizeof(szDevSN));

	if (NULL != pUserData)
	{
		if (strlen((char*)pUserData) <128)
		{
			strcpy(szDevSN ,(char*)pUserData );
		}
	}
	
	//保存时间统一时间
	if ( S_OK ==  ReadReocrdCallBackInfo(szDevSN ,&cRecordCallBackInfo ) && 
		0 == cRecordCallBackInfo.iCurStatus && 
		dwCarID == cRecordCallBackInfo.dwCurRecordCarID )
	{
		dw64TimeMS = cRecordCallBackInfo.dw64CurReocrdTime;
	}
	//名字
	CString fileName , strCarId , strPath ,strViolationPath , strHour;
	STime sTime;
	CString strTime;
	GetSTimeByDWORD64(sTime , dw64TimeMS);
	GetTimeStrBySTime(&sTime ,strTime ,"yyyymmddhhmiss");

	strPath = g_strDir +"\\";
	strViolationPath = g_strDir +"\\";

	GetTimeStrBySTime(&sTime , strHour , "hh");
	CString strDevSN(szDevSN);
	if (!strDevSN.IsEmpty())
	{
		strPath += strDevSN;
		strPath += "\\";
		strViolationPath += strDevSN;
		strViolationPath += "\\";
	}
	strViolationPath += "违章结果\\";

	CString strTimePath;
	GetSTimeByDWORD64(sTime , dw64TimeMS);
	GetTimeStrBySTime(&sTime ,strTimePath ,"yyyymmdd");

	strPath += strTimePath;
	strPath += "\\";
	strViolationPath += strTimePath ;
	strViolationPath += "\\";

	strPath += strHour;
	strPath += "\\";
	strViolationPath += strHour ;
	strViolationPath += "\\";

	fileName = strTime + "-";
	strCarId.Format("%8d",dwCarID);
	strCarId.Replace(' ','0');
	fileName +=  strCarId;

	CTime cTime(dw64TimeMS/1000);
	CTime cTime1(2011, 12, 8, 17, 0, 0);

	int nImageNum = 0;
	switch(wImgType)
	{
	case RECORD_BIGIMG_BEST_SNAPSHOT:
		nImageNum = 0;
		break;
	case RECORD_BIGIMG_LAST_SNAPSHOT:
		nImageNum = 1;
		break;
	case RECORD_BIGIMG_BEGIN_CAPTURE:
		nImageNum = 2;
		break;
	case RECORD_BIGIMG_BEST_CAPTURE:
		nImageNum = 3;
		break;
	case RECORD_BIGIMG_LAST_CAPTURE:
		nImageNum = 4;
		break;
	}
	CString strFileName;
	CString strImageNum;

	strImageNum.Format("%d",nImageNum);
	strImageNum.Replace(' ' , '0');
	strFileName.Format("%s-%s-%s.jpg" ,strTime,strCarId,strImageNum );

	__int64 i64FreeSpace = 0 ;
	bool fIsPeccancy =false;
	
	if( g_fSaveFile )
	{
		if(fIsPeccancy)
		{
			FileSave(strViolationPath , strFileName , (char*)pbPicData , dwImgDataLen , true );
		}
		else
		{
			FileSave(strPath , strFileName ,(char*)pbPicData , dwImgDataLen , true );
		}
	}
	return 0;
}
int OnPlate(PVOID pUserData, DWORD dwCarID,
			LPCSTR pcPlateNo, LPCSTR pcAppendInfo,
			DWORD dwRecordType,
			DWORD64 dw64TimeMS )
{
	CString strMsg;
	CString strMsg2;

 	DWORD32 dwProcTimes = 0;
 	DWORD32 dwTimeHight = 0;
 	DWORD32 dwTimeLow = 0;
    DWORD32 dwIndex = dwCarID;
	RECORD_CALLBACK_INFO cRecordCallBackInfo;

	char szDevSN[128];
	ZeroMemory(szDevSN ,sizeof(szDevSN));

	if (NULL != pUserData)
	{
		if (strlen((char*)pUserData) <MAX_DEV_SN_NUM)
		{
			strcpy(szDevSN ,(char*)pUserData );
			strcpy(cRecordCallBackInfo.szDevSN  ,(char*)pUserData );
		}
	}

	cRecordCallBackInfo.dw64CurReocrdTime = dw64TimeMS;
	cRecordCallBackInfo.dwCurRecordCarID = dwCarID;
	cRecordCallBackInfo.iCurStatus = 0;
	WriteRecordCallBackInfo(&cRecordCallBackInfo);

	GetExInfo(pcAppendInfo, dwProcTimes, dwTimeHight, dwTimeLow, dwIndex );

	DWORD64 dw64FrameTime = dw64TimeMS;
	DEV_CONFIG devConfig;
	ReadConfig(szDevSN , &devConfig);

	if (devConfig.fEnableRecvHistory)
	{
		STime sTime ;
		GetSTimeByDWORD64(sTime , dw64FrameTime);
		char rgBeginTime[64];
		ZeroMemory(rgBeginTime , sizeof(rgBeginTime));
		sprintf(rgBeginTime , "%4d.%2d.%2d_%2d" , sTime.nYear , sTime.nMonth , sTime.nDay , sTime.nHour);

		CString strBeginTime(rgBeginTime);
		strBeginTime.Replace(' ' , '0');
		strcpy(devConfig.szBeginTime , strBeginTime);

		devConfig.nIndex = dwCarID;
	}

	WriteConfig(szDevSN , &devConfig);

	//名字
	CString fileName , strCarId , strPath ,strViolationPath , strHour;
	STime sTime;
	CString strTime;
	GetSTimeByDWORD64(sTime , dw64FrameTime);
	GetTimeStrBySTime(&sTime ,strTime ,"yyyymmddhhmiss");

	strPath = g_strDir +"\\";
	strViolationPath = g_strDir +"\\";

	GetTimeStrBySTime(&sTime , strHour , "hh");
	CString strDevSN(szDevSN);
	if (!strDevSN.IsEmpty())
	{
		strPath += strDevSN;
		strPath += "\\";
		strViolationPath += strDevSN;
		strViolationPath += "\\";
	}
	strViolationPath += "违章结果\\";

	CString strTimePath;
	GetSTimeByDWORD64(sTime , dw64FrameTime);
	GetTimeStrBySTime(&sTime ,strTimePath ,"yyyymmdd");

	strPath += strTimePath;
	strPath += "\\";
	strViolationPath += strTimePath ;
	strViolationPath += "\\";

	strPath += strHour;
	strPath += "\\";
	strViolationPath += strHour ;
	strViolationPath += "\\";

	fileName = strTime + "-";
	strCarId.Format("%8d",dwCarID);
	strCarId.Replace(' ','0');
	fileName +=  strCarId;

	CTime cTime(dw64FrameTime/1000);
	CTime cTime1(2011, 12, 8, 17, 0, 0);

	//strMsg.Format("HvCamCallbackRecord: 0x%x, 0x%x, %d, 0x%x, 0x%x [%s]\n", pUserData, pbRecordData, dwLen, dwType, szExtInfo, szPlateBuf);

	char szPlateBuf[128];
	ZeroMemory(szPlateBuf , sizeof(szPlateBuf));

	if ( 0 == strcmp("无车牌",pcPlateNo) )
	{
		strcpy(szPlateBuf, "  无车牌");  // “无车牌的情况下需要在前面加上两个空格”
	}
	else
	{
		strcpy(szPlateBuf,pcPlateNo);
	}

	bool fIsPeccancy = false;
	if(strstr(pcAppendInfo, "违章:是") != NULL)
	{
		fIsPeccancy = true;
	}
	fIsPeccancy = false;
	if(g_fSaveFile)
	{
		if (fIsPeccancy)
		{
			FileSave(strViolationPath ,fileName +".txt",szPlateBuf ,strlen(szPlateBuf) , true );
		}
		else
		{

			FileSave(strPath ,fileName +".txt",szPlateBuf ,strlen(szPlateBuf) , true );
		}	
	}

	char szPlateAdditionInfo[12*1024];
	ZeroMemory(szPlateAdditionInfo , sizeof(szPlateAdditionInfo));
	DWORD32 dw32TimeL ,dw32TimeH;

	int iRet = GetPlateNumByXmlExtInfo((char*)pcAppendInfo , NULL ,szPlateAdditionInfo, NULL ,&dw32TimeL , &dw32TimeH);
	if (-1 == iRet)
	{
		return -1;
	}

	CString strXml(szPlateAdditionInfo);
	CString strFileName = fileName +".inf";
	CStdioFile cPlateAdditionFile;

	if (g_fSaveFile)
	{
		if(fIsPeccancy)
		{
			cPlateAdditionFile.Open(strViolationPath + strFileName , CFile::modeCreate|CFile::modeWrite , NULL );
			cPlateAdditionFile.Write(szPlateAdditionInfo ,strXml.GetLength()+1 );
			cPlateAdditionFile.Close();
			//FileSave(strViolationPath , strFileName , (char*)szPlateAdditionInfo , strXml.GetLength()+1 , true );
		}
		else
		{
			cPlateAdditionFile.Open(strPath + strFileName , CFile::modeCreate|CFile::modeWrite , NULL );
			cPlateAdditionFile.Write(szPlateAdditionInfo ,strXml.GetLength()+1 );
			cPlateAdditionFile.Close();
			//FileSave(strPath , strFileName ,(char*)szPlateAdditionInfo ,strXml.GetLength()+1 , true );
		}
	}

}
int OnSmall(PVOID pUserData, DWORD dwCarID,
			WORD wWidth, WORD wHeight,
			PBYTE pbPicData, DWORD dwImgDataLen,
			DWORD dwRecordType,
			DWORD64 dwTimeMS)
{
	CString strMsg;
	CString strMsg2;

	DWORD32 dwProcTimes = 0;
	DWORD32 dwTimeHight = 0;
	DWORD32 dwTimeLow = 0;
	DWORD32 dwIndex = dwCarID;
	static const int c_iSmallBufSize = 128*1024;

	char szDevSN[128];
	ZeroMemory(szDevSN ,sizeof(szDevSN));

	if (NULL != pUserData)
	{
		if (strlen((char*)pUserData) <128)
		{
			strcpy(szDevSN ,(char*)pUserData );
		}
	}


	//名字
	CString fileName , strCarId , strPath ,strViolationPath , strHour;
	STime sTime;
	CString strTime;
	GetSTimeByDWORD64(sTime , dwTimeMS);
	GetTimeStrBySTime(&sTime ,strTime ,"yyyymmddhhmiss");

	strPath = g_strDir +"\\";
	strViolationPath = g_strDir +"\\";

	GetTimeStrBySTime(&sTime , strHour , "hh");
	CString strDevSN(szDevSN);
	if (!strDevSN.IsEmpty())
	{
		strPath += strDevSN;
		strPath += "\\";
		strViolationPath += strDevSN;
		strViolationPath += "\\";
	}
	strViolationPath += "违章结果\\";

	CString strTimePath;
	GetSTimeByDWORD64(sTime , dwTimeMS);
	GetTimeStrBySTime(&sTime ,strTimePath ,"yyyymmdd");

	strPath += strTimePath;
	strPath += "\\";
	strViolationPath += strTimePath ;
	strViolationPath += "\\";

	strPath += strHour;
	strPath += "\\";
	strViolationPath += strHour ;
	strViolationPath += "\\";

	fileName = strTime + "-";
	strCarId.Format("%8d",dwIndex);
	strCarId.Replace(' ','0');
	fileName +=  strCarId;

	CTime cTime(dwTimeMS/1000);
	CTime cTime1(2011, 12, 8, 17, 0, 0);

	if (g_fSaveFile)
	{
		char* pbBmpData = new char[ c_iSmallBufSize ];
		int nBmpDataLen = c_iSmallBufSize;
		if ( NULL == pbBmpData )
		{
			return -1;
		}
		
		if ( S_OK != HVAPIUTILS_SmallImageToBitmap(pbPicData, wWidth, wHeight, (PBYTE)pbBmpData, &nBmpDataLen) )
		{
			return -1;
		}

		CString strFileName = fileName +".bmp";
		bool fIsPeccancy = false;

		if(fIsPeccancy)
		{
			FileSave(strViolationPath , strFileName , (char*)pbBmpData , nBmpDataLen , true );
		}
		else
		{
			FileSave(strPath , strFileName ,(char*)pbBmpData , nBmpDataLen , true );
		}
		delete[] pbBmpData;
	}
	return 0;
}
int OnBinary(PVOID pUserData, DWORD dwCarID,
			 WORD wWidth, WORD wHeight, 
			 PBYTE pbPicData, DWORD dwImgDataLen,
			 DWORD dwRecordType, 
			 DWORD64 dwTimeMS)
{
	CString strMsg;
	CString strMsg2;

	DWORD32 dwProcTimes = 0;
	DWORD32 dwTimeHight = 0;
	DWORD32 dwTimeLow = 0;
	DWORD32 dwIndex = dwCarID;

	char szDevSN[128];
	ZeroMemory(szDevSN ,sizeof(szDevSN));

	if (NULL != pUserData)
	{
		if (strlen((char*)pUserData) <128)
		{
			strcpy(szDevSN ,(char*)pUserData );
		}
	}


	//名字
	CString fileName , strCarId , strPath ,strViolationPath , strHour;
	STime sTime;
	CString strTime;
	GetSTimeByDWORD64(sTime , dwTimeMS);
	GetTimeStrBySTime(&sTime ,strTime ,"yyyymmddhhmiss");

	strPath = g_strDir +"\\";
	strViolationPath = g_strDir +"\\";

	GetTimeStrBySTime(&sTime , strHour , "hh");
	CString strDevSN(szDevSN);
	if (!strDevSN.IsEmpty())
	{
		strPath += strDevSN;
		strPath += "\\";
		strViolationPath += strDevSN;
		strViolationPath += "\\";
	}
	strViolationPath += "违章结果\\";

	CString strTimePath;
	GetSTimeByDWORD64(sTime , dwTimeMS);
	GetTimeStrBySTime(&sTime ,strTimePath ,"yyyymmdd");

	strPath += strTimePath;
	strPath += "\\";
	strViolationPath += strTimePath ;
	strViolationPath += "\\";

	strPath += strHour;
	strPath += "\\";
	strViolationPath += strHour ;
	strViolationPath += "\\";

	fileName = strTime + "-";
	strCarId.Format("%8d",dwIndex);
	strCarId.Replace(' ','0');
	fileName +=  strCarId;

	CTime cTime(dwTimeMS/1000);
	CTime cTime1(2011, 12, 8, 17, 0, 0);

	CString strFileName = fileName +".bin";
	bool fIsPeccancy = false;
	if (g_fSaveFile)
	{
		if(fIsPeccancy)
		{
			FileSave(strViolationPath , strFileName , (char*)pbPicData , dwImgDataLen , true );
		}
		else
		{
			FileSave(strPath , strFileName ,(char*)pbPicData , dwImgDataLen , true );

		}
	}

	return 0;
}
/*
INT CDECL HvCamCallbackRecord(PVOID pUserData, PBYTE pbRecordData, DWORD dwLen, DWORD dwType, LPCSTR szExtInfo)
{
	if ( RECORD_TYPE_NORMAL == dwType || RECORD_TYPE_HISTORY == dwType )
	{
		CString strMsg;
		CString strMsg2;

		DWORD32 dwProcTimes = 0;
		DWORD32 dwTimeHight = 0;
		DWORD32 dwTimeLow = 0;
		DWORD32 dwIndex = 0;

		char szDevSN[128];
		ZeroMemory(szDevSN ,sizeof(szDevSN));

		if (NULL != pUserData)
		{
			if (strlen((char*)pUserData) <128)
			{
				strcpy(szDevSN ,(char*)pUserData );
			}
		}
		GetExInfo(szExtInfo, dwProcTimes, dwTimeHight, dwTimeLow, dwIndex );

		DWORD64 dw64FrameTime = ((DWORD64)(dwTimeHight)<<32) | dwTimeLow;
		DEV_CONFIG devConfig;
		ReadConfig(szDevSN , &devConfig);

		if (devConfig.fEnableRecvHistory)
		{
			STime sTime ;
			GetSTimeByDWORD64(sTime , dw64FrameTime);
			char rgBeginTime[64];
			ZeroMemory(rgBeginTime , sizeof(rgBeginTime));
			sprintf(rgBeginTime , "%4d.%2d.%2d_%2d" , sTime.nYear , sTime.nMonth , sTime.nDay , sTime.nHour);

			CString strBeginTime(rgBeginTime);
			strBeginTime.Replace(' ' , '0');
			strcpy(devConfig.szBeginTime , strBeginTime);

			devConfig.nIndex = dwIndex;
		}

		WriteConfig(szDevSN , &devConfig);

		//名字
		CString fileName , strCarId , strPath ,strViolationPath , strHour;
		STime sTime;
		CString strTime;
		GetSTimeByDWORD64(sTime , dw64FrameTime);
		GetTimeStrBySTime(&sTime ,strTime ,"yyyymmddhhmiss");

		strPath = g_strDir +"\\";
		strViolationPath = g_strDir +"\\";

		GetTimeStrBySTime(&sTime , strHour , "hh");
		CString strDevSN(szDevSN);
		if (!strDevSN.IsEmpty())
		{
			strPath += strDevSN;
			strPath += "\\";
			strViolationPath += strDevSN;
			strViolationPath += "\\";
		}
		strViolationPath += "违章结果\\";

		CString strTimePath;
		GetSTimeByDWORD64(sTime , dw64FrameTime);
		GetTimeStrBySTime(&sTime ,strTimePath ,"yyyymmdd");

		strPath += strTimePath;
		strPath += "\\";
		strViolationPath += strTimePath ;
		strViolationPath += "\\";

		strPath += strHour;
		strPath += "\\";
		strViolationPath += strHour ;
		strViolationPath += "\\";

		fileName = strTime + "-";
		strCarId.Format("%8d",dwIndex);
		strCarId.Replace(' ','0');
		fileName +=  strCarId;

		CTime cTime(dw64FrameTime/1000);
		CTime cTime1(2011, 12, 8, 17, 0, 0);

		if(true)
		{

			char szPlateBuf[256];
			if ( S_OK == HVAPIUTILS_GetPlateString(szExtInfo, szPlateBuf, sizeof(szPlateBuf)) )
			{
				strMsg.Format("HvCamCallbackRecord: 0x%x, 0x%x, %d, 0x%x, 0x%x [%s]\n", pUserData, pbRecordData, dwLen, dwType, szExtInfo, szPlateBuf);

				if ( 0 == strcmp("无车牌",szPlateBuf) )
				{
					if (NULL != szPlateBuf)
					{
						strcpy(szPlateBuf, "  无车牌");  // “无车牌的情况下需要在前面加上两个空格”
					}

				}

				BOOL fIsPeccancy = false;
				RECORD_IMAGE cImg;
				if(strstr(szExtInfo, "违章:是") != NULL)
				{
					fIsPeccancy = true;
				}
				if (fIsPeccancy)
				{
					FileSave(strViolationPath ,fileName +".txt",szPlateBuf ,strlen(szPlateBuf) , true );
				}
				else
				{
					FileSave(strPath ,fileName +".txt",szPlateBuf ,strlen(szPlateBuf) , true );
				}

				ProcBigImage(strPath , strViolationPath ,strTime ,strCarId , pbRecordData, dwLen, 0, fIsPeccancy);

				if ( S_OK == HVAPIUTILS_GetRecordImage(pbRecordData, dwLen, RECORD_IMAGE_SMALL_IMAGE, &cImg) )
				{
					CTime cTime(cImg.cImgInfo.dw64TimeMs / 1000);
					CString strTime = cTime.Format("%c");
					strMsg2.Format("[%x,%d,%d,%d,%d,{%s}]\n", 
						cImg.pbImgData, 
						cImg.dwImgDataLen, 
						cImg.cImgInfo.dwCarID, 
						cImg.cImgInfo.dwWidth, 
						cImg.cImgInfo.dwHeight, 
						strTime);
					//OutputDebugString(strMsg2);

					CString strFileName;
					if(fIsPeccancy)
						strFileName.Format(".\\HvApiDllTestResults\\违章结果\\%s_%d_small.bmp", cTime.Format("%Y%m%d%H%M%S"), dwIndex);
					else
						strFileName.Format(".\\HvApiDllTestResults\\%s_%d_small.bmp", cTime.Format("%Y%m%d%H%M%S"), dwIndex);

					static char pbBmpData[32*1024] = {0};
					int nBmpDataLen = sizeof(pbBmpData);
					if ( S_OK == HVAPIUTILS_SmallImageToBitmap(cImg.pbImgData, cImg.cImgInfo.dwWidth, cImg.cImgInfo.dwHeight, (PBYTE)pbBmpData, &nBmpDataLen) )
					{
						CString strFileName = fileName +".bmp";
						if(fIsPeccancy)
						{
							FileSave(strViolationPath , strFileName , (char*)pbBmpData , nBmpDataLen , true );
						}
						else
						{
							FileSave(strPath , strFileName ,(char*)pbBmpData , nBmpDataLen , true );
						}
					}
				}
				if ( S_OK == HVAPIUTILS_GetRecordImage(pbRecordData, dwLen, RECORD_IMAGE_BIN_IMAGE, &cImg) )
				{
					CTime cTime(cImg.cImgInfo.dw64TimeMs / 1000);
					CString strTime = cTime.Format("%c");
					strMsg2.Format("[%x,%d,%d,%d,%d,{%s}]\n", 
						cImg.pbImgData, 
						cImg.dwImgDataLen, 
						cImg.cImgInfo.dwCarID, 
						cImg.cImgInfo.dwWidth, 
						cImg.cImgInfo.dwHeight, 
						strTime);
					//OutputDebugString(strMsg2);


					static char pbBmpData[32*1024] = {0};
					int nBmpDataLen = sizeof(pbBmpData);
					if ( S_OK == HVAPIUTILS_BinImageToBitmap(cImg.pbImgData, (PBYTE)pbBmpData, &nBmpDataLen) )
					{
						CString strFileName = fileName +".bin";
						if(fIsPeccancy)
						{
							FileSave(strViolationPath , strFileName , (char*)pbBmpData , nBmpDataLen , true );
						}
						else
						{
							FileSave(strPath , strFileName ,(char*)pbBmpData , nBmpDataLen , true );
						}

					}
				}

				char szPlateAdditionInfo[1024<<6];
				ZeroMemory(szPlateAdditionInfo , sizeof(szPlateAdditionInfo));
				DWORD32 dw32TimeL , dw32TimeH;
				int iRet = GetPlateNumByXmlExtInfo((char*)szExtInfo , NULL ,szPlateAdditionInfo, NULL ,&dw32TimeL , &dw32TimeH);
				if (-1 == iRet)
				{
					return -1;
				}

				CString strXml(szPlateAdditionInfo);
				CString strFileName = fileName +".inf";
				if(fIsPeccancy)
				{
					FileSave(strViolationPath , strFileName , (char*)szPlateAdditionInfo , strXml.GetLength()+1 , true );
				}
				else
				{
					FileSave(strPath , strFileName ,(char*)szPlateAdditionInfo ,strXml.GetLength()+1 , true );
				}
			}
			else
			{
				strMsg = "HVAPIUTILS_GetPlateString is Error!!!\n";
			}

			//OutputDebugString(strMsg);


		}
	}
	else if ( RECORD_TYPE_STRING == dwType )
	{
		CString strMsg;
		strMsg.Format("RECORD_TYPE_STRING: pUserData = 0x%x, pbRecordData = [%s], dwLen = %d, dwType = 0x%x, szExtInfo = [%s]\n",
			pUserData, 
			pbRecordData, 
			dwLen, 
			dwType, 
			szExtInfo);
		//OutputDebugString(strMsg);
	}
	else if ( RECORD_TYPE_INLEFT == dwType )
	{
		CString strMsg;
		strMsg.Format("RECORD_TYPE_INLEFT: pUserData = 0x%x, pbRecordData = [%s], dwLen = %d, dwType = 0x%x, szExtInfo = [%s]\n",
			pUserData, 
			pbRecordData, 
			dwLen, 
			dwType, 
			szExtInfo);
		//OutputDebugString(strMsg);
	}

	

	return 0;
}
*/
//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CAutoLink_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

INT CDECL HvCamCallbackGetherInfo(PVOID pUserData, PBYTE pbInfoData, DWORD dwInfoLen)
{
	if(pUserData == NULL) return -1;
	CString strMsg;
	strMsg.Format("%s", (LPSTR)pbInfoData);
	CAutoLink_demoDlg* pDlg = (CAutoLink_demoDlg*)pUserData;

	return 0;
}
bool IsDigitStr( CString str )
{
	bool digitFlag = true;
	int i  = str.GetLength();
	while (i--)
	{
		if (!isdigit(str.GetAt(i)))
		{
			digitFlag = false;
			break;

		}

	}
	return digitFlag;
}

static DWORD WINAPI UpdateClientInfor(LPVOID lpParameter)
{

	CAutoLink_demoDlg* pDlg = (CAutoLink_demoDlg* )lpParameter;

	char szRecordConStatus[128];
	ZeroMemory(szRecordConStatus , sizeof(szRecordConStatus));

	INT nRecordConStatus = CONN_STATUS_UNKNOWN;
	int nReConnectCount = 0;

	int nDevListLen = 0;
	int nDevListBufLen = 128*100;
	char* pDevListBuf = new char[nDevListBufLen];
	
	char* pszSN = NULL;
	char* pszTemp = NULL;
	__int64 i64FreeDiskSpace = 0;
	const __int64 c_i64MinFreeDiskSpace = 30 * 1024 * 1024;

	while(!pDlg->m_fExit)
	{
		Sleep(1000);

		//检查硬盘空间
		if( g_fSaveFile )
		{
			if ( S_OK == Hv_GetDiskFreeSpace(g_strDir , i64FreeDiskSpace)
				&& i64FreeDiskSpace < c_i64MinFreeDiskSpace 
				)
			{
				g_fSaveFile = false;
				pDlg->SetWindowText("AutoLinkServer_硬盘空间不足！！");
			}
		}
		else 
		{
			if ( S_OK == Hv_GetDiskFreeSpace(g_strDir , i64FreeDiskSpace)
				&& i64FreeDiskSpace > c_i64MinFreeDiskSpace )
			{
				g_fSaveFile = true;
				pDlg->SetWindowText("AutoLinkServer");
			}
		}
		
		
		if (S_OK != HVAPI_GetDeviceListSize(nDevListLen , NULL))
		{
			continue;
		}
		if (nDevListBufLen < nDevListLen)
		{
			if (NULL != pDevListBuf)
			{
				delete pDevListBuf;
				pDevListBuf = NULL;
			}
			nDevListBufLen = nDevListLen;
			pDevListBuf = new char[nDevListBufLen];
		}

		if (S_OK == HVAPI_GetDeviceList(pDevListBuf , nDevListBufLen , NULL))
		{
			if (NULL == pDevListBuf)
			{
				continue;
			}
			pszSN = pDevListBuf;
			while(pszTemp = strstr(pszSN ,";"))
			{
				*pszTemp = '\0';
				if(S_OK != HVAPI_GetDeviceStatus(pszSN ,nRecordConStatus , nReConnectCount , NULL ))
				{
					nRecordConStatus = CONN_STATUS_UNKNOWN;
				}
				switch (nRecordConStatus)
				{
					case CONN_STATUS_UNKNOWN:
						strcpy( szRecordConStatus,DEV_STATUS_UNKNOW);
						break;
					case CONN_STATUS_NORMAL:
						strcpy( szRecordConStatus,DEV_STATUS_CONN);
						break;
					case CONN_STATUS_DISCONN:
						strcpy( szRecordConStatus,DEV_STATUS_DISCONN);
						break;
					case CONN_STATUS_RECONN:
						strcpy( szRecordConStatus,DEV_STATUS_RECONN);
						break;
					case CONN_STATUS_RECVDONE:
						strcpy( szRecordConStatus,DEV_STATUS_DONE);
						break;
					case CONN_STATUS_CONNFIRST:
						strcpy( szRecordConStatus,DEV_STATUS_FIRSTCONN);
						break;
					case CONN_STATUS_CONNOVERTIME:
						strcpy( szRecordConStatus,DEV_STATUS_OVERTIMECONN);
						break;
					default:
						strcpy( szRecordConStatus,DEV_STATUS_UNKNOW);
						break;
				}
				INT Conn_port = 0 ;
				char* dwIP = new char[128];
			    HVAPI_GetDeviceInfoAutoLink(pszSN, NULL, dwIP,128 ,Conn_port);
				pDlg->ShowClientInfor(pszSN,szRecordConStatus,nReConnectCount ,dwIP, Conn_port);
				pszSN = pszTemp+1;
				delete[] dwIP;
			}
		}
	}

	if (NULL != pDevListBuf)
	{
		delete pDevListBuf;
		pDevListBuf = NULL;
	}

	return 0;
}

void CAutoLink_demoDlg::OnBnClickedButton1()
{
	CString strMaxListenNum;
	CString strAutoLinkPort;
	CString strError= "error:";
	HRESULT hr = S_OK;

	m_maxListenNumEdit.GetWindowText(strMaxListenNum);
	if (!IsDigitStr(strMaxListenNum) || strMaxListenNum.IsEmpty())
	{
		strError = "最大监听数：非法值！";
		hr = E_FAIL;
	}

	m_autoLinkPortEdit.GetWindowText(strAutoLinkPort);
	if (!IsDigitStr(strAutoLinkPort)|| strAutoLinkPort.IsEmpty())
	{
		strError += "端口号：非法值！";
		hr = E_FAIL;
	}
	if (atoi(strMaxListenNum) <= 0 && atoi(strAutoLinkPort) <= 0)
	{
		strError = "最大监听数：非法值！";
		hr = E_FAIL;
	}

	
	CString strDir;
	GetDlgItem(IDC_DIR)->GetWindowText(strDir);
	DWORD attr;   
	attr = GetFileAttributes(strDir);   
	if(attr == (DWORD)-1 && attr & FILE_ATTRIBUTE_DIRECTORY)
	{
		strError += "保存目录有误！";
		hr = E_FAIL;
	}
	else
	{
		g_strDir = strDir;
	}
	
	if (S_OK != hr)
	{
		AfxMessageBox(strError);
		return ;
	}
	
	HVAPI_UnLoadMonitor();
	if (S_OK != HVAPI_LoadMonitor(atoi(strMaxListenNum) ,NULL ))
	{
		AfxMessageBox("LoadMonitor Fail!");
		return ;
	}
	
	if (S_OK != HVAPI_OpenServer(atoi(strAutoLinkPort), LISTEN_TYPE_RECORD , NULL))
	{
		AfxMessageBox("Open Record Listen Error!");
		return ;
	}
	if (NULL == m_hShowThread)
	{
		m_fExit = FALSE;
		m_hShowThread = CreateThread(NULL, 0, UpdateClientInfor, this, 0, NULL);
	}

	m_autoLinkPortEdit.EnableWindow(FALSE);
	m_maxListenNumEdit.EnableWindow(FALSE);
	m_openAutoLinkBut.EnableWindow(FALSE);
	m_closeAutoLinkBut.EnableWindow(TRUE);
	GetDlgItem(IDC_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_FIND_DIR)->EnableWindow(FALSE);

}

void CAutoLink_demoDlg::OnBnClickedButton2()
{
	m_autoLinkPortEdit.EnableWindow(TRUE);
	m_maxListenNumEdit.EnableWindow(TRUE);
	m_openAutoLinkBut.EnableWindow(TRUE);
	m_closeAutoLinkBut.EnableWindow(FALSE);
	GetDlgItem(IDC_DIR)->EnableWindow(TRUE);
	GetDlgItem(IDC_FIND_DIR)->EnableWindow(TRUE);

	m_fExit = TRUE;
	if (NULL != m_hShowThread)
	{
		WaitForSingleObject(m_hShowThread , 1000);
		CloseHandle(m_hShowThread);
		m_hShowThread = NULL;
	}

	HVAPI_CloseServer(LISTEN_TYPE_RECORD);
	HVAPI_UnLoadMonitor();
	
	m_devListCtrl.DeleteAllItems();
	m_devListCtrl.UpdateWindow();

	m_connDevList.RemoveAll();

}

HRESULT CAutoLink_demoDlg::InitDevListCtrl()
{
	m_devListCtrl.DeleteAllItems();

	LVCOLUMN lv;
	lv. mask=LVCF_TEXT|LVCF_FMT|LVCF_WIDTH ;
	lv. fmt=LVCFMT_CENTER ;

	lv.pszText="设备编号";
	lv.cchTextMax=sizeof(lv.pszText);
	lv.iSubItem =0;
	lv.iOrder =0;
	lv.cx=148;
	m_devListCtrl.InsertColumn(0,&lv);

	lv.pszText="设备IP";
 	lv.cchTextMax=sizeof(lv.pszText);
 	lv.iSubItem =1;
 	lv.iOrder =1;
	lv.cx = 100;
 
 	m_devListCtrl.InsertColumn(1,&lv);

	lv.pszText="状态";
	lv.cx = 65;
	lv.cchTextMax=sizeof(lv.pszText);
	lv.iSubItem =1;
	lv.iOrder =1;

	m_devListCtrl.InsertColumn(2,&lv);

// 	lv.pszText="结果数";
// 	lv.cchTextMax=sizeof(lv.pszText);
// 	lv.iSubItem =1;
// 	lv.iOrder =1;
// 
// 	m_devListCtrl.InsertColumn(2,&lv);


 	lv.pszText="重连次数";
 	lv.cchTextMax=sizeof(lv.pszText);
 	lv.iSubItem =1;
 	lv.iOrder =1;
	lv.cx = 65;
 
 	m_devListCtrl.InsertColumn(3,&lv);

 	lv.pszText="连接端口";
 	lv.cchTextMax=sizeof(lv.pszText);
 	lv.iSubItem =1;
 	lv.iOrder =1;
	lv.cx = 80;
 
 	m_devListCtrl.InsertColumn(4,&lv);

// 	lv.pszText="数据信息：";
// 	lv.cchTextMax=sizeof(lv.pszText);
// 	lv.iSubItem =1;
// 	lv.iOrder =1;
// 
// 	m_devListCtrl.InsertColumn(3,&lv);
	
	m_devListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES);


	return 0;
}

HRESULT CAutoLink_demoDlg::ShowClientInfor( char* pSN, char* pStatus,int nReConnectCount , char* dwIP, int port)
{
	if (m_fExit || NULL == pSN)
	{
		return S_OK;
	}
	if (0 == strcmp(pSN ,""))
	{
		return S_OK;
	}

	int iNetDiskCount = m_devListCtrl.GetItemCount();

	CString strDevSN;
	CString strReConnectCount;
	CString strdwIP;
	CString strPort;
	strReConnectCount.Format("%d" , nReConnectCount);
	strPort.Format("%d", port);
	for (int i = 0; i<iNetDiskCount ; i++)
	{
		strDevSN = m_devListCtrl.GetItemText(i , 0);

		if (0 == strcmp(pSN , strDevSN))
		{
			m_devListCtrl.SetItemText(i , 2 , pStatus);
			m_devListCtrl.SetItemText(i , 3 , strReConnectCount);
			m_devListCtrl.SetItemText(i , 1 , dwIP);
			m_devListCtrl.SetItemText(i , 4 , strPort);
			return S_OK;
		}
	}

	int iItemRow = m_devListCtrl.InsertItem(0 , pSN);
	m_devListCtrl.SetItem(iItemRow ,2 , LVIF_TEXT , pStatus , 0 , 0 ,0 ,0 );
	m_devListCtrl.SetItemText(i , 3 , strReConnectCount);

	return S_OK;
}

void CAutoLink_demoDlg::OnClose()
{
	m_fExit = TRUE;
	if (NULL != m_hShowThread)
	{
		WaitForSingleObject(m_hShowThread , 2000);
		CloseHandle(m_hShowThread);
		m_hShowThread = NULL;
	}

	HVAPI_CloseServer(LISTEN_TYPE_RECORD);
	HVAPI_UnLoadMonitor();

	m_devListCtrl.DeleteAllItems();
	CDialog::OnClose();
}

void CAutoLink_demoDlg::OnCancel()
{

	DestroyWindow();
//	CDialog::OnCancel();
}

void CAutoLink_demoDlg::PostNcDestroy()
{
//	CDialog::PostNcDestroy();
    delete this;
}



	
void CAutoLink_demoDlg::deleteSrc()
{
	m_fExit = TRUE;
	if (NULL != m_hShowThread)
	{
		WaitForSingleObject(m_hShowThread , 2000);
		CloseHandle(m_hShowThread);
		m_hShowThread = NULL;
	}

	HVAPI_CloseServer(LISTEN_TYPE_RECORD);
	HVAPI_UnLoadMonitor();

	m_devListCtrl.DeleteAllItems();
}


void CAutoLink_demoDlg::OnBnClickedFindDir()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   bBinfo; 
	memset(&bBinfo,   0,   sizeof(BROWSEINFO));     //定义结构并初始化 
	bBinfo.hwndOwner=m_hWnd;//设置对话框所有者句柄 

	bBinfo.lpszTitle= "请选择结果保存目录： "; 
	bBinfo.ulFlags   =   BIF_RETURNONLYFSDIRS;   //设置标志只允许选择目录         
	LPITEMIDLIST   lpDlist; 

	CString dir;
	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串 
	lpDlist=SHBrowseForFolder(&bBinfo)   ;   //显示选择对话框 
	if(lpDlist!=NULL) 
	{ 
		SHGetPathFromIDList(lpDlist,dir.GetBuffer(255));//把项目标识列表转化成目录 
		dir.ReleaseBuffer();
		GetDlgItem(IDC_DIR)->SetWindowText(dir);
	}
}

HRESULT MakeConnCmd(DEV_CONFIG* pDevConfig , char* pCmd , int& nCmdLen)
{
	
	char szConnCmd[512];
	ZeroMemory(szConnCmd ,sizeof(szConnCmd));
	
	sprintf(szConnCmd , "DownloadRecord,"
		"Enable[%d],"
		"BeginTime[%s],"
		"Index[%d],"
		"EndTime[%s],"
		"DataInfo[%d]"
		,pDevConfig->fEnableRecvHistory
		,pDevConfig->szBeginTime
		,pDevConfig->nIndex
		,pDevConfig->szEndTime
		,pDevConfig->fOnlyViolation
		);
	if (nCmdLen > (int)strlen(szConnCmd))
	{
		strcpy(pCmd , szConnCmd);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}

}

void CAutoLink_demoDlg::OnDBClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//// TODO: Add your control notification handler code here
	*pResult = 0;
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (-1 != pNMListView->iItem)
	{
		CString strDevSn = m_devListCtrl.GetItemText(pNMListView->iItem , 0);
		CString strConnStatus = m_devListCtrl.GetItemText(pNMListView->iItem , 2);


		if (0 == strcmp(strConnStatus, DEV_STATUS_UNKNOW) ||
			0 == strcmp(strConnStatus , DEV_STATUS_DISCONN) ||
			0 == strcmp(strConnStatus , DEV_STATUS_OVERTIMECONN) ||
			0 == strcmp(strConnStatus , DEV_STATUS_FIRSTCONN))
		{
			HVAPI_HANDLE_EX AutoLinkHandle = HVAPI_OpenAutoLink(strDevSn , NULL);
			if (NULL == AutoLinkHandle)
			{
				AfxMessageBox("连接失败！");
			}
			else
			{
				DEV_HANDLE * pDevHandle = new DEV_HANDLE;
				if (NULL == pDevHandle)
				{
					AfxMessageBox("分配失败！");
					return ;
				}
				DEV_CONFIG devConfig;
				ReadConfig(strDevSn.GetBuffer() ,&devConfig );
				strDevSn.ReleaseBuffer();

				int nConnCmdLen = 512;
				char szConnCmd[512];
				ZeroMemory(szConnCmd , sizeof(szConnCmd));
				MakeConnCmd(&devConfig , szConnCmd ,nConnCmdLen);

				pDevHandle->handle = AutoLinkHandle;
				strcpy(pDevHandle->szDevSN , strDevSn);
				
				HVAPI_SetCallBackEx(AutoLinkHandle , OnRecordBegin , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOBEGIN , szConnCmd);
				HVAPI_SetCallBackEx(AutoLinkHandle , OnPlate , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_PLATE , szConnCmd);
				HVAPI_SetCallBackEx(AutoLinkHandle , OnSmall , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_SMALLIMAGE , szConnCmd);
				HVAPI_SetCallBackEx(AutoLinkHandle , OnBinary , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_BINARYIMAGE , szConnCmd);
				HVAPI_SetCallBackEx(AutoLinkHandle , OnBigImage , pDevHandle->szDevSN ,0, CALLBACK_TYPE_RECORD_BIGIMAGE , szConnCmd);
				HVAPI_SetCallBackEx(AutoLinkHandle , OnRecordEnd , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOEND , szConnCmd);
				m_connDevList.AddHead(pDevHandle);
				AfxMessageBox("设置回调成功！");
			}
		}
		else if (0 == strcmp(strConnStatus , DEV_STATUS_DONE))
		{
			DEV_HANDLE * pDevHandle = NULL;
			POSITION pos = m_connDevList.GetHeadPosition();
			while (pos)
			{
				pDevHandle = m_connDevList.GetAt(pos);

				if (NULL != pDevHandle)
				{
					if (0 == strcmp(pDevHandle->szDevSN ,strDevSn ))
					{
						DEV_CONFIG devConfig;
						ReadConfig(pDevHandle->szDevSN ,&devConfig );
					
						int nConnCmdLen = 512;
						char szConnCmd[512];
						ZeroMemory(szConnCmd , sizeof(szConnCmd));
						MakeConnCmd(&devConfig , szConnCmd ,nConnCmdLen);
						
						HVAPI_SetCallBackEx(pDevHandle->handle , OnRecordBegin , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOBEGIN , szConnCmd);
						HVAPI_SetCallBackEx(pDevHandle->handle , OnPlate , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_PLATE , szConnCmd);
						HVAPI_SetCallBackEx(pDevHandle->handle , OnSmall , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_SMALLIMAGE , szConnCmd);
						HVAPI_SetCallBackEx(pDevHandle->handle , OnBinary , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_BINARYIMAGE , szConnCmd);
						HVAPI_SetCallBackEx(pDevHandle->handle , OnBigImage , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_BIGIMAGE , szConnCmd);
						HVAPI_SetCallBackEx(pDevHandle->handle , OnRecordEnd , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOEND , szConnCmd);

						break;
					}
				}
				m_connDevList.GetNext(pos);

			}
		}
		else if (0 == strcmp(strConnStatus , DEV_STATUS_CONN ) || 
					0 == strcmp(strConnStatus , DEV_STATUS_RECONN ))
		{
			DEV_HANDLE * pDevHandle = NULL;
			POSITION pos = m_connDevList.GetHeadPosition();
			while (pos)
			{
				pDevHandle = m_connDevList.GetAt(pos);

				if (NULL != pDevHandle)
				{
					if (0 == strcmp(pDevHandle->szDevSN ,strDevSn ))
					{
						if (S_OK == HVAPI_CloseEx(pDevHandle->handle))
						{
							AfxMessageBox("成功断开连接！");
							m_connDevList.RemoveAt(pos);

							delete pDevHandle;
							pDevHandle = NULL;
							break;
						}
						else
						{
							AfxMessageBox("断开连接失败！");
							continue;
						}
					}
				}
				m_connDevList.GetNext(pos);
				
			}
		}
	}

	return ;
}



void CAutoLink_demoDlg::OnRClickDevList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (-1 != pNMListView->iItem)
	{
		CString strDevSN = m_devListCtrl.GetItemText(pNMListView->iItem , 0);

		int iSubMenuNum = 0;
		CMenu menu, *pSubMenu;
		menu.LoadMenu(IDR_MENU1);
		pSubMenu = menu.GetSubMenu(iSubMenuNum);

		POINT oPoint;
		GetCursorPos(&oPoint);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN ,oPoint.x , oPoint.y ,this);
	}
	
}

void CAutoLink_demoDlg::OnSetConfig()
{
	POSITION pos = m_devListCtrl.GetFirstSelectedItemPosition();
	int iRow = m_devListCtrl.GetNextSelectedItem(pos);
	CString strDevSN = m_devListCtrl.GetItemText(iRow ,0);

	AutoLinkParam autoLinkParamDlg;

	DEV_CONFIG devConfig;
	char szDevSN[128];
	ZeroMemory(szDevSN , sizeof(szDevSN));

	strcpy(szDevSN , strDevSN);
	ReadConfig(szDevSN , &devConfig);

	bool fSetParam = FALSE;
	if (S_OK != autoLinkParamDlg.InitParamDialog(szDevSN, &devConfig ,&fSetParam))
	{
		AfxMessageBox("SetConfig fail!");
		return ;
	}
	
	autoLinkParamDlg.DoModal();

	if (fSetParam)
	{
		WriteConfig(szDevSN , &devConfig);

		int nConnCmdLen = 512;
		char szConnCmd[512];
		ZeroMemory(szConnCmd , sizeof(szConnCmd));
		MakeConnCmd(&devConfig , szConnCmd ,nConnCmdLen);

		DEV_HANDLE * pDevHandle = NULL;
		POSITION pos = m_connDevList.GetHeadPosition();
		BOOL fFind = FALSE;
		while (pos)
		{
			pDevHandle = m_connDevList.GetAt(pos);

			if (NULL != pDevHandle)
			{
				if (0 == strcmp(pDevHandle->szDevSN , szDevSN ))
				{
					HVAPI_SetCallBackEx(pDevHandle->handle , OnRecordBegin , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOBEGIN , szConnCmd);
					HVAPI_SetCallBackEx(pDevHandle->handle , OnPlate , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_PLATE , szConnCmd);
					HVAPI_SetCallBackEx(pDevHandle->handle , OnSmall , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_SMALLIMAGE , szConnCmd);
					HVAPI_SetCallBackEx(pDevHandle->handle , OnBinary , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_BINARYIMAGE , szConnCmd);
					HVAPI_SetCallBackEx(pDevHandle->handle , OnBigImage , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_BIGIMAGE , szConnCmd);
					HVAPI_SetCallBackEx(pDevHandle->handle , OnRecordEnd , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOEND , szConnCmd);

					fFind = TRUE;
					break;
				}
			}
			m_connDevList.GetNext(pos);
		}

		if (!fFind)
		{
			HVAPI_HANDLE_EX AutoLinkHandle = HVAPI_OpenAutoLink(szDevSN ,  NULL);
			if (NULL == AutoLinkHandle)
			{
				AfxMessageBox("连接失败！");
			}
			else
			{
				DEV_HANDLE * pDevHandle = new DEV_HANDLE;
				if (NULL == pDevHandle)
				{
					AfxMessageBox("分配失败！");
					return ;
				}

				pDevHandle->handle = AutoLinkHandle;
				strcpy(pDevHandle->szDevSN , szDevSN);

				HVAPI_SetCallBackEx(pDevHandle->handle , OnRecordBegin , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOBEGIN , szConnCmd);
				HVAPI_SetCallBackEx(pDevHandle->handle , OnPlate , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_PLATE , szConnCmd);
				HVAPI_SetCallBackEx(pDevHandle->handle , OnSmall , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_SMALLIMAGE , szConnCmd);
				HVAPI_SetCallBackEx(pDevHandle->handle , OnBinary , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_BINARYIMAGE , szConnCmd);
				HVAPI_SetCallBackEx(pDevHandle->handle , OnBigImage , pDevHandle->szDevSN ,0,  CALLBACK_TYPE_RECORD_BIGIMAGE , szConnCmd);
				HVAPI_SetCallBackEx(pDevHandle->handle , OnRecordEnd , pDevHandle->szDevSN , 0, CALLBACK_TYPE_RECORD_INFOEND , szConnCmd);

				m_connDevList.AddHead(pDevHandle);
				AfxMessageBox("连接成功！");
			}
		}
	}
}




void CAutoLink_demoDlg::OnOK()
{
    // 防止按回车退出
}


BOOL CAutoLink_demoDlg::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
 } 
