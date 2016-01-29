// HvDeviceAxPropPage.cpp : Implementation of the CHvDeviceAxPropPage property page class.

#include "stdafx.h"
#include "HvDeviceAx.h"
#include "HvDeviceAxPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CHvDeviceAxPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CHvDeviceAxPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CHvDeviceAxPropPage, "HVDEVICEAX.HvDeviceAxPropPage.1",
	0x9d9008ce, 0xd2f2, 0x4960, 0x93, 0xac, 0x75, 0x54, 0x95, 0x2e, 0x5, 0x42)



// CHvDeviceAxPropPage::CHvDeviceAxPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CHvDeviceAxPropPage

BOOL CHvDeviceAxPropPage::CHvDeviceAxPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_HVDEVICEAX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CHvDeviceAxPropPage::CHvDeviceAxPropPage - Constructor

CHvDeviceAxPropPage::CHvDeviceAxPropPage() :
	COlePropertyPage(IDD, IDS_HVDEVICEAX_PPG_CAPTION)
{
}



// CHvDeviceAxPropPage::DoDataExchange - Moves data between page and properties

void CHvDeviceAxPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CHvDeviceAxPropPage message handlers
