#pragma once

// HvDeviceAx.h : main header file for HvDeviceAx.DLL

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols


// CHvDeviceAxApp : See HvDeviceAx.cpp for implementation.

class CHvDeviceAxApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

