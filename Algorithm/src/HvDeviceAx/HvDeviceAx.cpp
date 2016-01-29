// HvDeviceAx.cpp : Implementation of CHvDeviceAxApp and DLL registration.

#include "stdafx.h"
#include "HvDeviceAx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "comcat.h"
#include "strsafe.h"  
#include "ObjSafe.h"  


CHvDeviceAxApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x52C4A44D, 0xD2FF, 0x4CBA, { 0x8B, 0x58, 0x16, 0x6C, 0xEA, 0x92, 0xBE, 0x1E } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

const GUID CDECL CLSID_SafeItem =  
        { 0x7f3e4c52, 0x224d, 0x415a, { 0x95, 0x68, 0xfd, 0x4b, 0xb3, 0x1, 0x67, 0xcf } }; 



//libvlc_instance_t * g_vlc_ins = 0;
/*
const char * vlc_args[] =
{
    "-I",
    "dummy",
    "--ignore-config",
    "--extraintf=logger",
    "--verbose=2",
};*/


// Helper function to create a component category and associated
// description
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);

// Helper function to register a CLSID as belonging to a component
// category
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid);


HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
	ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;
	
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_ICatRegister,
                               (void**)&pcr);
	
	if (FAILED(hr))
		return hr;
	
	// Make sure the HKCR\Component Categories\{..catid...}
         // key is registered
	
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // english
	
	// Make sure the provided description is not too long.
    // Only copy the first 127 characters if it is
	
	int len = wcslen(catDescription);
    if (len>127)
		len = 127;
	
	wcsncpy(catinfo.szDescription, catDescription, len);
         // Make sure the description is null terminated
    catinfo.szDescription[len] = '\0';

     hr = pcr->RegisterCategories(1, &catinfo);
     pcr->Release();
	 return hr;
}


// Helper function to register a CLSID as belonging to a component
// category

HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	
	// Register your component categories information.
    ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_ICatRegister,
                               (void**)&pcr);
	
	if (SUCCEEDED(hr))
    {
		// Register this category as being "implemented" by
        // the class.
		
		CATID rgcatid[1] ;
        rgcatid[0] = catid;
        hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
    }
	
	if (pcr != NULL)
		pcr->Release();
	return hr;
}

//注销与CLSID中的相应implemented Categories项，一般用不到，因为其它程序可能也会用到这此项  
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)  
{  
    ICatRegister *pcr = NULL ;  
    HRESULT hr = S_OK ;  
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,   
            NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);  
  
    if (SUCCEEDED(hr))  
    {  
       CATID rgcatid[1] ;  
       rgcatid[0] = catid;  
       hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);  
    }  
  
    if (pcr != NULL)  
        pcr->Release();  
    return hr;  
}  



// CHvDeviceAxApp::InitInstance - DLL initialization

BOOL CHvDeviceAxApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
		//vlc init
		//g_vlc_ins = libvlc_new( sizeof(vlc_args)/sizeof(vlc_args[0]), vlc_args );

	}
	
	 
	
	return bInit;
}



// CHvDeviceAxApp::ExitInstance - DLL termination

int CHvDeviceAxApp::ExitInstance()
{
	// TODO: Add your own module termination code here.
	


	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);


	//创建脚本安全“补充”项，非CLSID中  
    HRESULT hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely scriptable!");  
    if (FAILED(hr))  
        return hr;  
  
    //创建初始化安全“补充”项，非CLSID中  
    hr = CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data!");  
    if (FAILED(hr))  
        return hr;  
  
    //设置控件CLSID中补充项的脚本安全项，与“补充”项中的脚本安全项对应  
    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);  
    if (FAILED(hr))  
        return hr;  
  
    //设置控件CLSID中补充项的初始化安全项，与“补充”项中的初始化安全项对应  
    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);  
    if (FAILED(hr))  
        return hr;  



	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// 删除控件初始化安全入口.  
    HRESULT hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);  
    if (FAILED(hr))  
        return hr;  
  
    // 删除控件脚本安全入口  
    hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);  
    if (FAILED(hr))  
        return hr;  


	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	
	
		
	return NOERROR;
}
