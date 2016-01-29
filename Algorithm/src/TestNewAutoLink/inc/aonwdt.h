//	aonWdt.h
//
//	Aaeon Watchdog Support
//	Copyright(C)Aaeon Technology Inc., 2005
//	Unauthorized access prohibit
//

#ifndef __AONWDT_H__
#define __AONWDT_H__

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifdef AAEON_EXPORTS
#define DLLAPI __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

//
//Open Watchdog Instance
//
//	Input:	reserved		//reserved must be zero
//	return: Nonzero, the instance handle, if success, 
//			NULL if failed
//
//			Using GetLastError() to get error codes
//			Possible error codes are 
//				ERROR_SERVICE_LOGON_FAILED
//				ERROR_FILE_NOT_FOUND
//				ERROR_IO_PENDING
//	
DLLAPI HANDLE
aaeonWdtOpen(
	DWORD reserved		//reserved must be zero
);

//
//Close Watchdog Instance
//
//	Input:	hInst is the instance handle
//	return: TRUE if success, 
//			FALSE if failed
//
DLLAPI BOOL
aaeonWdtClose(
	HANDLE hInst
);

//
//Get Device ID
//
//	Input:	hInst is the instance handle
//	return: device id, LOWORD is the Major ID, HIWORD is the Subid.
//
DLLAPI WORD
aaeonWdtGetDevID(
	HANDLE hInst
);

//
//Get Device Version
//
//	Input:	hInst is the instance handle
//	Return: device version, LOWORD is the major version, HIWORD is the minor version
//
DLLAPI WORD
aaeonWdtGetDevVer(
	HANDLE hInst
);

//
//Get Device Name
//
//	Input:	hInst is the instance handle
//	Return:	Device name in zero-terminated string format
//
DLLAPI LPCTSTR
aaeonWdtGetDevName(
	HANDLE hInst
);

//
//Set Enable
//
//	Input:	hInst is the instance handle
//			bEnable = TRUE if Enable, FALSE if Disable
//
DLLAPI HRESULT
aaeonWdtSetEnable(
	HANDLE hInst, 
	BOOL bEnable
);

//
//Get Enable
//
//	Input:	hInst is the instance handle
//	return:	*pbEnable
//
DLLAPI HRESULT
aaeonWdtGetEnable(
	HANDLE hInst, 
	BOOL* pbEnable
);

//
//Set Count Mode
//
//	Input:	hInst is the instance handle
//			bMinute is TRUE if Minute mode, FALSE if second mode.
//
DLLAPI HRESULT
aaeonWdtSetCountMode(
	HANDLE hInst, 
	BOOL bMinute
);

//
//Get Count mode
//
//	Input:	hInst is the instance handle
//	Return:	*pbMinute is the current count mode
//
DLLAPI HRESULT
aaeonWdtGetCountMode(
	HANDLE hInst, 
	BOOL* pbMinute
);

//
//Set Timeout Count
//
//	Input:	hInst is the instance handle
//			tTimeout is the count
//
DLLAPI HRESULT
aaeonWdtSetTimeoutCount(
	HANDLE hInst, 
	DWORD tTimeout
);

//
//Get Timeout count
//
//	Input:	hInst is the instance handle
//	Return:	*ptTimeout is the timeout count
//
DLLAPI HRESULT
aaeonWdtGetTimeoutCount(
	HANDLE hInst, 
	DWORD* ptTimeout
);

//
//Get Instant Timeout count
//
//	Input:	hInst is the instance handle
//	Return:	*ptTimeout is the Instant timeout count
//
DLLAPI HRESULT
aaeonWdtGetCurTimeoutCount(
	HANDLE hInst, 
	DWORD* ptTimeout
);

//
//Set Mouse Reset Enable
//
//	Input:	hInst is the instance handle
//	Return:	bEnable is TRUE if Enable, FALSE if Disable
//
DLLAPI HRESULT
aaeonWdtSetMouseResetEnable(
	HANDLE hInst, 
	BOOL bEnable
);

//
//Get Mouse Reset Enable
//
//	Input:	hInst is the instance handle
//	Return:	*pbEnable is TRUE if Enable, FALSE if Disable.
//
DLLAPI HRESULT
aaeonWdtGetMouseResetEnable(
	HANDLE hInst, 
	BOOL* pbEnable
);

//
//Set Keyboard Reset Enable
//
//	Input:	hInst is the instance handle
//	Return:	bEnable is TRUE if Enable, FALSE if Disable
//
DLLAPI HRESULT
aaeonWdtSetKeyboardResetEnable(
	HANDLE hInst, 
	BOOL bEnable
);

//
//Get Keyboard Reset Enable
//
//	Input:	hInst is the instance handle
//	Return:	*pbEnable is TRUE if Enable, FALSE if Disable.
//
DLLAPI HRESULT
aaeonWdtGetKeyboardResetEnable(
	HANDLE hInst, 
	BOOL* pbEnable
);

//
//Set Timeout status
//
//	Input:	hInst is the instance handle
//			bTimeout is TRUE if Timeout, FALSE if Down counting.
//
DLLAPI HRESULT
aaeonWdtSetTimeoutStatus(
	HANDLE hInst, 
	BOOL bTimeout
);

//
//Get Timeout status
//
//	Input:	hInst is the instance handle
//	Return:	*pbTimeout is TRUE if Timeout, FALSE if Down counting.
//
DLLAPI HRESULT
aaeonWdtGetTimeoutStatus(
	HANDLE hInst, 
	BOOL* pbTimeout
);

//
//Handshake with Watchdog to reload the Timeout count into the down counter.
//
//	Input:	hInst is the instance handle
//
DLLAPI HRESULT
aaeonWdtHandshake(
	HANDLE hInst
);

//
//Force watchdog timeout immediately
//
//	Input:	hInst is the instance handle
//
DLLAPI HRESULT
aaeonWdtForceTimeout(
	HANDLE hInst
);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__AONWDT_H__
