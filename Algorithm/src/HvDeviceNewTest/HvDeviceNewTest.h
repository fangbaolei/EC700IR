// HvDeviceNewTest.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号


// CHvDeviceNewTestApp:
// 有关此类的实现，请参阅 HvDeviceNewTest.cpp
//

class CHvDeviceNewTestApp : public CWinApp
{
public:
	CHvDeviceNewTestApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CHvDeviceNewTestApp theApp;
