#pragma once

// 安全信息结构体
typedef struct _SAFE_MODE_INFO
{
	char szDevSN[128];
	int iEanbleSafeMode;
	char szBeginTime[256];
	char szEndTime[256];
	int index;
	int DataInfo;
}SAFE_MODE_INFO;

// 文件名信息
typedef struct _DEV_FILENAME
{
	int iBigImgEnable;
	int iSmallImgEnable;
	int iBinEnable;
	int iInfoEnable;
	int iPlateEnable;
	int iPlateNoEnable;

	char chBigImgFileName[1024];
	char chSmallImgFileName[1024];
	char chBinFileName[1024];
	char chInfoFileName[1024];
	char chPlateFileName[1024];
	char chPlateNoFileName[1024];

	_DEV_FILENAME()
	{
		iBigImgEnable = 0;
		iSmallImgEnable = 0;
		iBinEnable = 0;
		iInfoEnable = 0;
		iPlateEnable = 0;
		iPlateNoEnable = 0;

		memset(chBigImgFileName, 0, 1024);
		memset(chSmallImgFileName, 0, 1024);
		memset(chBinFileName, 0, 1024);
		memset(chInfoFileName, 0, 1024);
		memset(chPlateFileName, 0, 1024);
		memset(chPlateNoFileName, 0, 1024);
	}

	~_DEV_FILENAME()
	{
		iBigImgEnable = 0;
		iSmallImgEnable = 0;
		iBinEnable = 0;
		iInfoEnable = 0;
		iPlateEnable = 0;
		iPlateNoEnable = 0;

		memset(chBigImgFileName, 0, 1024);
		memset(chSmallImgFileName, 0, 1024);
		memset(chBinFileName, 0, 1024);
		memset(chInfoFileName, 0, 1024);
		memset(chPlateFileName, 0, 1024);
		memset(chPlateNoFileName, 0, 1024);
	}
}DEV_FILENAME;
