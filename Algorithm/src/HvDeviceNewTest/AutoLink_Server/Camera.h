#pragma once

#define MAX_BIG_COUNT 5
#define MAX_LIST_COUNT 50

// 识别结果结构体
typedef struct _Result
{
	DWORD dwCarID;				    // 车辆ID
	DWORD64 dwTime;              // 车辆抓拍时间
	CHAR chPlate[32];                // 车牌号码
	CHAR chPlateColor[8];         // 车牌颜色

	PBYTE pbBinImage;		   // 车牌二值图
	DWORD dwBinSize;         // 车牌二值图大小

	//车牌小图
	WORD wSmallImgWidth;			//小图宽度
	WORD wSmallImgHeight;			//小图高度
	PBYTE pbSmallImg;				//车牌JPEG图
	DWORD dwSmallImgSize;			//车牌JPEG图的大小
	DWORD64 dwSmallImgTime;    //车牌小图时间
	
	//车辆大图
	DWORD BigCarID;
	IStream* BigImgStream[MAX_BIG_COUNT];      //车辆大图
	DWORD dwBigImgSize[MAX_BIG_COUNT];		//车辆大图大小
	RECT rcPlatePos[MAX_BIG_COUNT];                 //车牌坐标
	DWORD64 dwImageTimeMs[MAX_BIG_COUNT];          // 车辆大图时间
	int iImageCount;   // 大图数量
	
	int iSpeed;
	int iLimitSpeed;
	char chRoadName[128];
	char chRoadDirection[128];
	int iLand;
	char chCarColor[8];
	char chIllegalInfo[24];
	int iIllegalType;
	char chCarType[16];
	char chAppendInfo[2048];
	int iRedTimeMS;
	float fOverSpeedRate;
	
	// 红灯时间
	int iH;
	int iM;
	int iS;
	int iSS;
	
	_Result()
	{
		dwCarID = 0;
		dwTime = 0;
		pbBinImage = NULL;
		dwBinSize = 0;
		wSmallImgWidth = 0;
		wSmallImgHeight = 0;
		pbSmallImg = NULL;
		dwSmallImgSize = 0;
		BigCarID = 0;
		iImageCount = 0;
		iSpeed = 0;
		iLimitSpeed = 0;
		iLand = 0;
		iIllegalType = 0;
		iRedTimeMS= 0;
		fOverSpeedRate = 0;

		for (int i = 0; i < MAX_BIG_COUNT; i++)
		{
			BigImgStream[i] = NULL; 
			dwBigImgSize[i] = 0;
			dwImageTimeMs[i] = 0;
		}

		ZeroMemory(chAppendInfo, 2048);
	}

	~_Result()
	{
		if (pbBinImage != NULL)
		{
			delete[] pbBinImage;
			pbBinImage = NULL;
		}
		if (pbSmallImg != NULL)
		{
			delete[] pbSmallImg;
			pbSmallImg = NULL;
		}
		for (int i = 0; i < MAX_BIG_COUNT; i++)
		{
			if (BigImgStream[i] != NULL)
			{
				BigImgStream[i]->Release();
				BigImgStream[i] = NULL;
			}
		}
	}
}Result;

class CCamera
{
public:
	CCamera();
	~CCamera(void);

private:
	CLSID m_jpgClsid;
	HANDLE m_hThread;  // 监测状态线程
	HANDLE m_hHandleThread; // 结果保存线程
	
	Result* m_HvResult;   // 结果信息
	SAFE_MODE_INFO m_safeModeInfo;  // 历史结果信息
	int m_iBreakCount;  // 历史结果循环节点
	//int m_iDevType;  // 设备类型
	int m_iSyncTimeCount; // 同步时间间隔
	
    DEV_FILENAME m_FileNameInfo; // 文件命名信息
	CCommon m_Common; // 综合功能
	
	int m_iLogEnable; // 日志开关
	int m_iImgProcessEnable; // 图像处理开关

	int m_iFontSize; // 字体大小
	int m_iImgMode; // 图片保存模式
	DWORD m_dwFontColor; // 字体颜色
	DWORD m_dwFontEdgeColor; // 字体背景框颜色

	// 图片大小
	int m_iBigImgWidth;
	int m_iBigImgHeight;

	// 压缩选项
	int m_iCompressEnable;
	int m_iCompressQuality;  // 压缩质量
	int m_iCompressSubQuality; // 压缩质量步进
	int m_iCompressSize;   // 最终压缩尺寸

	// 车身截图选项
	int m_iFeatureEnable;
	int m_iFeatureWidth;
	int m_iFeatureHeigh;

	char m_chIllegalDir[1024];   // 违章路径
	char m_chNormalDir[1024]; // 非违章路径

	int Connect();
	int DisConnect();
	int SaveResult(Result* pHvResult);
	int ResolveXmlData(LPCSTR szResultInfo);
	int ImgProcess(Result* pHvResult, IStream* pStream, int iImgIndex, CString strText, IStream* pStreamOut);

	float CCamera::GetRedTime(int iRedTime, DWORD64 dwCarTime);

	int InflateCropRect(RECT& rectDst,
							const RECT &cPlatePos,
							const int& iImgWidth,
							const int& iImgHeight,
							const int& iCropWidth,
							const int& iCropHeight
						); // 计算截图位置大小

	int WriteIniFlie();
	int WriteHistoryIniFile();
	int ReadIniFlie();
	int ReadHistoryIniFile();

public:
	HVAPI_HANDLE_EX m_hHv;  // 设备连接句柄
	bool m_bExit; // 线程退出标记
	DWORD m_dwConnectStatus;   // 设备连接状态
	CRITICAL_SECTION m_csResult;
	CString m_strDevSN; // 设备编号
	CString m_strDevIP; // IP
	CString m_strDir;   // 目录
	int m_iDevType; // 设备类型
	int m_iPort;
	CList<Result*, Result*> m_HvResultList;

	int Open(CString strDevSN, CString strDir);
	int Close();
	DWORD GetStatus();
	int SyncTime();
	int HandleResult();
	int RecoConnect();
	int WriteLog(char* chText);

public:
	// 结果保存线程
	static DWORD HandleResultThread(LPVOID param)
	{
		if ( param == NULL)
			return  0;
		CCamera *pThis = (CCamera*)param;
		return pThis->HandleResult();
	}
	// 获取状态线程
	static DWORD RecoThread(LPVOID param)
	{
		if ( param == NULL)
			return  0;
		CCamera *pThis = (CCamera*)param;
		return pThis->RecoConnect();
	}
	// ==================以下是回调函数========================
	// 识别结果开始回调函数
	static int _cdecl RecordInfoBeginCallBack(PVOID pUserData, DWORD dwCarID)
	{
		if ( pUserData == NULL )
			return 0;

		CCamera* pThis = (CCamera*)pUserData;
		return pThis->RecordInfoBegin(dwCarID);
	}
	int RecordInfoBegin(DWORD dwCarID);

	// 识别结果结束回调函数
	static int _cdecl RecordInfoEndCallBack(PVOID pUserData, DWORD dwCarID)
	{
		if ( pUserData == NULL )
			return 0;

		CCamera* pThis = (CCamera*)pUserData;
		return pThis->RecordInfoEnd(dwCarID);
	}
	int RecordInfoEnd(DWORD dwCarID);

	// 车牌信息回调
	static int __cdecl GetPlateCallBack(PVOID pUserData, 
														DWORD dwCarID, 
														LPCSTR pcPlateNo, 
														LPCSTR pcAppendInfo, 
														DWORD dwRecordType,
														DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		CCamera* pThis = (CCamera*)pUserData;
		return pThis->GetPlate(dwCarID, pcPlateNo, pcAppendInfo, dwRecordType, dw64TimeMS);
	}
	int GetPlate(DWORD dwCarID, 
						LPCSTR pcPlateNo, 
						LPCSTR pcAppendInfo, 
						DWORD dwRecordType,
						DWORD64 dw64TimeMS);

	// 大图回调
	static int __cdecl GetBigImgCallBack(PVOID pUserData,
							DWORD dwCarID, 
							WORD  wImgType,
							WORD  wWidth,
							WORD  wHeight,
							PBYTE pbPicData,
							DWORD dwImgDataLen,
							DWORD dwRecordType,
							DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		CCamera* pThis = (CCamera*)pUserData;
		return pThis->GetBigImg(dwCarID, wImgType, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int GetBigImg(DWORD dwCarID, 
						WORD  wImgType,
						WORD  wWidth,
						WORD  wHeight,
						PBYTE pbPicData,
						DWORD dwImgDataLen,
						DWORD dwRecordType,
						DWORD64 dw64TimeMS);

	// 车牌小图回调
	static int __cdecl GetSmallImgCallBack(PVOID pUserData,
													  DWORD dwCarID,
													  WORD wWidth,
													  WORD wHeight,
													  PBYTE pbPicData,
													  DWORD dwImgDataLen,
													  DWORD dwRecordType,
													  DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		CCamera* pThis = (CCamera*)pUserData;
		return pThis->GetSmallImg(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int GetSmallImg(DWORD dwCarID,
							WORD wWidth,
							WORD wHeight,
							PBYTE pbPicData,
							DWORD dwImgDataLen,
							DWORD dwRecordType,
							DWORD64 dw64TimeMS);

	// 车牌二值图回调
	static int __cdecl GetBinImgCallBack(PVOID pUserData,
													   DWORD dwCarID,
													   WORD wWidth,
													   WORD wHeight,
													   PBYTE pbPicData,
													   DWORD dwImgDataLen,
													   DWORD dwRecordType,
													   DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		CCamera* pThis = (CCamera*)pUserData;
		return pThis->GetBinImg(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int GetBinImg(DWORD dwCarID,
						WORD wWidth,
						WORD wHeight,
						PBYTE pbPicData,
						DWORD dwImgDataLen,
						DWORD dwRecordType,
						DWORD64 dw64TimeMS);

};
