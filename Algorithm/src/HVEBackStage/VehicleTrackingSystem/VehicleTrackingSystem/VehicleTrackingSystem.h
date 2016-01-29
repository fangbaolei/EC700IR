#pragma once
#include "resource.h"	

class CVehicleTrackingSystemApp : public CWinApp
{
public:
	CVehicleTrackingSystemApp();

	public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CVehicleTrackingSystemApp theApp;
