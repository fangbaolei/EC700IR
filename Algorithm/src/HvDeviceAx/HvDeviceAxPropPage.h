#pragma once

// HvDeviceAxPropPage.h : Declaration of the CHvDeviceAxPropPage property page class.


// CHvDeviceAxPropPage : See HvDeviceAxPropPage.cpp for implementation.

class CHvDeviceAxPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CHvDeviceAxPropPage)
	DECLARE_OLECREATE_EX(CHvDeviceAxPropPage)

// Constructor
public:
	CHvDeviceAxPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_HVDEVICEAX };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

