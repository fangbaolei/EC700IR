#pragma once

// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被覆盖。

/////////////////////////////////////////////////////////////////////////////
// CHvdeviceaxctrl1 包装类

class CHvdeviceaxctrl1 : public CWnd
{
protected:
	DECLARE_DYNCREATE(CHvdeviceaxctrl1)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x7F3E4C52, 0x224D, 0x415A, { 0x95, 0x68, 0xFD, 0x4B, 0xB3, 0x1, 0x67, 0xCF } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 特性
public:


// 操作
public:

// _DHvDeviceAx

// Functions
//

	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ConnectTo(LPCTSTR strAddr)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, strAddr);
	}
	long GetCarID()
	{
		long result;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString GetPlate()
	{
		CString result;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetPlateColor()
	{
		CString result;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetPlateInfo()
	{
		CString result;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long GetStatus(short iConnType)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, iConnType);
		return result;
	}
	long SaveH264Video(LPCTSTR strFilePath)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xd, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFilePath);
		return result;
	}
	long SaveJpegVideo(LPCTSTR strFileName)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFileName);
		return result;
	}
	long SavePlateBin2BMP(LPCTSTR strFileName)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFileName);
		return result;
	}
	long SavePlateBinImage(LPCTSTR strFileName)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFileName);
		return result;
	}
	long SavePlateImage(LPCTSTR strFileName)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFileName);
		return result;
	}
	long SaveSnapImage(LPCTSTR strFileName, short iImageType)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I2 ;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFileName, iImageType);
		return result;
	}
	void Disconnect()
	{
		InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	__int64 GetPlateResultTime()
	{
		__int64 result;
		InvokeHelper(0x14, DISPATCH_METHOD, VT_I8, (void*)&result, NULL);
		return result;
	}
	CString GetFileName(short iType)
	{
		CString result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, iType);
		return result;
	}
	long SetPathType(short iPathtType)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms, iPathtType);
		return result;
	}
	long SoftTriggerCaptureAndForceGetResult()
	{
		long result;
		InvokeHelper(0x18, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SendTriggerOut()
	{
		long result;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetH264FrameRateNum()
	{
		long result;
		InvokeHelper(0x1b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetH264FrameRateNum(long longH264FrameRateNum)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, longH264FrameRateNum);
		return result;
	}
	long InportNameListEx(LPCTSTR szWhiteNameList, long iWhiteListLen, LPCTSTR szBlackNameList, long iBlackListLen)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x1f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szWhiteNameList, iWhiteListLen, szBlackNameList, iBlackListLen);
		return result;
	}
	CString GetNameListEx()
	{
		CString result;
		InvokeHelper(0x20, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long InportNameList(signed char * szWhiteNameList, long iWhiteListLen, signed char * szBlackNameList, long iBlackListLen)
	{
		long result;
		static BYTE parms[] = VTS_PI1 VTS_I4 VTS_PI1 VTS_I4 ;
		InvokeHelper(0x21, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szWhiteNameList, iWhiteListLen, szBlackNameList, iBlackListLen);
		return result;
	}
	long GetNameList(signed char * szWhiteNameList, long * iWhiteListLen, signed char * szBlackNameList, long * iBlackListLen)
	{
		long result;
		static BYTE parms[] = VTS_PI1 VTS_PI4 VTS_PI1 VTS_PI4 ;
		InvokeHelper(0x22, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szWhiteNameList, iWhiteListLen, szBlackNameList, iBlackListLen);
		return result;
	}
	long TriggerSignal(long VideoID)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x23, DISPATCH_METHOD, VT_I4, (void*)&result, parms, VideoID);
		return result;
	}
	long TriggerAlarmSignal()
	{
		long result;
		InvokeHelper(0x24, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SaveIllegalVideo(LPCTSTR strFilePath)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x26, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strFilePath);
		return result;
	}
	CString GetIllegalReason()
	{
		CString result;
		InvokeHelper(0x27, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long GetCaptureImage(long VideoID, LPCTSTR strFileName)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x28, DISPATCH_METHOD, VT_I4, (void*)&result, parms, VideoID, strFileName);
		return result;
	}
	CString GetDevSN()
	{
		CString result;
		InvokeHelper(0x2a, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetDevMac()
	{
		CString result;
		InvokeHelper(0x2b, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}

	CString GetCusTomInfo()
	{
		CString result;
		InvokeHelper(0x2c, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}

	long SetCusTomInfo(LPCTSTR strCustomInfo)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x2d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strCustomInfo);
		return result;
	}



// Properties
//

long GetRecvH264Flag()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}
void SetRecvH264Flag(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}
long GetRecvJpegFlag()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}
void SetRecvJpegFlag(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}
long GetRecvPlateBinImageFlag()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}
void SetRecvPlateBinImageFlag(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}
long GetRecvPlateImageFlag()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}
void SetRecvPlateImageFlag(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}
long GetRecvSnapImageFlag()
{
	long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}
void SetRecvSnapImageFlag(long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}
long GetSaveH264VideoTime()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}
void SetSaveH264VideoTime(long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}
long GetRecvRecordFlag()
{
	long result;
	GetProperty(0x15, VT_I4, (void*)&result);
	return result;
}
void SetRecvRecordFlag(long propVal)
{
	SetProperty(0x15, VT_I4, propVal);
}
long GetShowH264VideoFlag()
{
	long result;
	GetProperty(0x1d, VT_I4, (void*)&result);
	return result;
}
void SetShowH264VideoFlag(long propVal)
{
	SetProperty(0x1d, VT_I4, propVal);
}
long GetRecvIllegalVideoFlag()
{
	long result;
	GetProperty(0x25, VT_I4, (void*)&result);
	return result;
}
void SetRecvIllegalVideoFlag(long propVal)
{
	SetProperty(0x25, VT_I4, propVal);
}
long GetHideIcon()
{
	long result;
	GetProperty(0x29, VT_I4, (void*)&result);
	return result;
}
void SetHideIcon(long propVal)
{
	SetProperty(0x29, VT_I4, propVal);
}


};
