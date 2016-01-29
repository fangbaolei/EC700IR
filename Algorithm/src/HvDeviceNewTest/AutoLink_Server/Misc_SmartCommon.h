// 名称：Common
// 功能：一个综合类
// 1. 字符替换--------------GetTrueString
		// 1). 支持的通配符：
				// $(DeviceNo)				设备编号
				// $(DeviceIP)				设备   IP
				// $(StreetName)			路口名称
				// $(StreetDirection)		路口方向
				// $(StreetNameID)		路口名称ID
				// $(StreetDirectionID)	路口方向ID

				// $(Year)				年
				// $(Month)			月
				// $(Day)				日
				// $(Hour)				时
				// $(Minute)			分
				// $(Second)			秒
				// $(MilliSecond)	毫秒

				// $(Date)				日期，格式：2013-12-10
				// $(Time)				时间，格式：13:46:50.207

				// $(PlateNo)			车牌
				// $(PlateType)		车牌类型
				// $(PlateColor)		车牌颜色
				// $(PlateColorCode)		车牌颜色编码

				// $(IllegalInfo)		违法信息
				// $(IllegalType)		违法类型
				// $(IllegalCode)	违法代码

				// $(Speed)			车速
				// $(LimitSpeed)	限速
				// $(OverSpeedRate)		超速百分比

				// $(CarType)		车辆类型
				// $(CarTypeCode)		车辆类型代码

				// $(CarColor)		车身颜色
				// $(CarColorCode)		车身颜色代码

				// $(CarNo, 08)     车牌ID，8为位数，0为补位前字符

				// $(BigImageID, 01)        图片ID，1为位数，0为补位前字符
				
				// $(BigImageID_Letter)     图片字母ID

				// $(RoadNum)		车道号

				// $(BigImageCount)     图片数量

				// $(NewLine)		回车换行

// 2. 流的操作:
		// 1). 图片流转数组--------------IStreamToByteArray
		// 2). 数组转图片流--------------ByteArrayToIStream
		// 3). 获取流的大小--------------GetStreamLength
		// 4). 获取当前工作路径--------------GetCurrentDir


#pragma once

#include <Shlwapi.h>
#pragma  comment(lib, "Shlwapi.lib")
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

typedef struct _ReplaceInfo
{
	CString strDeviceNo;            //设备编号
	CString strDeviceIP;              //设备IP

	CString strStreetName;     //路口名称
	CString strStreetDirection;           //路口方向
	CString strStreetNameID;           //路口名称ID
	CString strStreetDirectionID;        //路口方向ID

	DWORD64 dwTimeMs;       //时间

	CString strPlateNo;             //车牌号码
	CString strPlateType;           //车牌类型
	CString strPlateTypeCode;			//车牌类型编号
	CString strPlateColor;          //车牌颜色
	CString strPlateColorCode;       //车牌颜色编码


	int iRoadNumber;                //车道号

	CString strIllegalInfo;            //违法信息
	int iIllegalType;                   //违法类型
	CString strIllegalCode;         //违法代码

	int iSpeed;                     //车速
	int iLimitSpeed;            //限速
	float fltOverSpeedRate;       //超速百分比

	DWORD64 dwRedLightStartTimeMs;      // 红灯开始时刻

	CString strCarType; 		    // 车辆类型
	CString strCarTypeCode;         //车辆类型代码

	CString strCarColor;            //车身颜色
	CString strCarColorCode;            //车身颜色代码

	DWORD32 dwCarIndex;			//车牌ID

	int iBigImageCount;				//大图数量

	int iBigImageID;			//大图ID
	CString strBigImageID; //大图ID字母

	_ReplaceInfo()
	{
		strDeviceNo = strDeviceIP = strStreetName = strStreetDirection = strStreetNameID = strStreetDirectionID = strPlateNo = \
		strPlateType = strPlateTypeCode = strPlateColor = strPlateColorCode = strIllegalInfo = strIllegalCode = strCarType = \
		strCarTypeCode = strCarColor = strCarColorCode = _T("Unknown");

		dwTimeMs = 0;
		iRoadNumber = -1;
		iIllegalType = -1;

		iSpeed = -1;
		iLimitSpeed = -1;
		fltOverSpeedRate = 0.0;

		dwRedLightStartTimeMs = 0;

		dwCarIndex = -1;

		iBigImageCount = -1;
		iBigImageID = -1;
	}

}ReplaceInfo;


class CCommon
{
public:
	CCommon(void);
	~CCommon(void);

public:
	// 叠加字符替换
	CString GetTrueString(CString strFormat, ReplaceInfo& RpInfo);

	// 获取字符串替换的示例
	CString GetStringExample(CString strFormat);
	// 获取字符串替换的说明
	CString GetStringInfo(CString strFormat);

	// 保证路径存在
	BOOL MakeSurePathExists(CString strPath);

	// 图片流转数组
	int IStreamToByteArray(IStream* pStreamIn, BYTE* pImgBuff, int& iLen);

	// 数组写入流
	int ByteArrayToIStream(BYTE* pImgBuff, int iLen, IStream* pStreamOut);

	// 获取一个流的大小
	BOOL GetStreamLength(IStream* pStream, ULARGE_INTEGER* pulLenth);

	// 获取exe所在的路径（不一定是进程的当前路径）
	CString GetExeDir(void);

	// 产生一个[iStart,iEnd)区间内的随机数
	int Random(int iStart, int iEnd);

	// 取图片类型GLSID
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	// 路径检查
	int IsIllegalDir(CString strDir);
	
	// 文件名检查
	int IsIllegalFileName(CString strDir);

	// 获得两个时间相差的秒数
	int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);

private:
    void ReplaceNum(CString& strFormat, CString strArg, int iVal);

};
