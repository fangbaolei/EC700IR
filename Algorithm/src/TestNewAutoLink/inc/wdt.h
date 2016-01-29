/////////////////////////////////////////////////////////////////////////
//Module Name:
//
//    WDT.h  
//
//Abstract:  
//    WDT interface.
//
//Environment:
//    Windows NT/2000/XP
//
//Author:
//    mz.yang 2008.9
//
//Revision history:  
//	  mz.yang  2008.9    - Created.Support W83977F/AF,W83627HF/THF/EHF/DHG,
//						   SCH311x,IT8712.
//    mz.yang  2009.3    - Update.
//						   (1) Add WDT error code.
//						   (2) Add WDTUninstallEvent interface.
//						   (3) Support F81216D/DG.
//
/////////////////////////////////////////////////////////////////////////

#ifndef _WDT_H_
#define _WDT_H_

#include <windows.h>

// WDT working mode
#define WDT_MODE_RESET						1
#define	WDT_MODE_INTERRUPT					2	

// WDT counter counting unit
#define WDT_COUNTER_UNIT_SECOND				1
#define WDT_COUNTER_UNIT_MINUTE				2
#define WDT_COUNTER_UNIT_MS					3

// WDT IRQ set mode
#define WDT_IRQ_SEARCH_AUTO				   -1

// WDT overflow operation
#define WDT_OVERFLOW_EVENT_SHUTDOWN 		1
#define WDT_OVERFLOW_EVENT_REBOOT			2
#define WDT_OVERFLOW_EVENT_SET_EVENT		3
#define WDT_OVERFLOW_EVENT_SEND_MESSAGE		4
#define WDT_OVERFLOW_EVENT_CALL_FUNC		5

// WDT error code
#define WDT_ERROR_SUCCESS					0
#define WDT_ERROR_FAILED				   -1
#define WDT_ERROR_INVALID_PARAMETER		   -2
#define WDT_ERROR_NO_FREE_IRQ_RESOURCE	   -3
#define WDT_ERROR_IRQ_CONFLICT             -4
#define WDT_ERROR_NOT_CREATE               -5
#define WDT_ERROR_NOT_SUPPORT              -6
#define WDT_ERROR_DEVICE_NOT_FOUND         -7

// WDT overflow message name
#define WDT_OVERFLOW_MESSAGE_NAME "WDTOverflowMessage"

// WDT overflow message ID
UINT  WM_WDT_OVERFLOW_MESSAGE;

// User call back function pointer type.
#ifndef WDTCallBack
	typedef void (_stdcall * WDTCallBack)(void *pContext);
#endif


//*****************************************************************************
//
// EVOCIO Driver init routine. 2007.7
//
//*****************************************************************************
/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Install EVOCIO driver dynamic, and open device.
//
//Parameters:
//	none.
//
//Return Value:
//	true  : init succeed.
//	false : init failed.	
//			
//////////////////////////////////////////////////////////////////////////
bool _stdcall InitEVOCIO();

/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Close device, and remove EVOCIO driver.
//
//Parameters:
//	none.
//
//Return Value:
//	void.
//			
//////////////////////////////////////////////////////////////////////////
void _stdcall ShutdownEVOCIO();

//*****************************************************************************
//
// Super IO WDT interface routine. 2008.9
//
//*****************************************************************************
/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Create WDT.
//Parameters:
//
//  Mode        - WDT working mode:
//					1  - cause system reset when WDT time out.
//					2  - cause an INT when WDT time out.
//	Irq			- IRQ number:
//					WDT_IRQ_SEARCH_AUTO  - Search system free IRQ automatic.
//					3,4,5,7,9,12,14,15   - WDT work in interrupt mode.this value
//						                   is WDT IRQ resource,when WDT time out,
//                                         it will cause a interrupt,call
//						                   WDTInstallEvent() to register WDT time 
//						                   out operation.
//
//Return Value:
//	Reset mode:
//		WDT_ERROR_SUCCESS               - Create WDT successful.
//      WDT_ERROR_DEVICE_NOT_FOUND      - Not found WDT device.
//		WDT_ERROR_INVALID_PARAMETER     - Invalid parameters.
//	Interrupt mode:
//		Serch IRQ number automatic:	
//			0 ~ 15	                    - Create WDT successful and the value is the WDT
//					                      actual used IRQ number(isa IRQ : 3,4,5,6,7,9,
//                                        10,11,14,15).
//			WDT_ERROR_INVALID_PARAMETER	- Invalid parameters.
//			WDT_ERROR_NO_FREE_IRQ_RESOURCE - No interrupt resource can use in the system.
//          WDT_ERROR_DEVICE_NOT_FOUND  - Not found WDT device.
//          WDT_ERROR_FAILED            - Create WDT failed.
//		User set IRQ:
//			WDT_ERROR_SUCCESS           - Setup WDT successful.
//			WDT_ERROR_INVALID_PARAMETER - Invalid parameters.
//			WDT_ERROR_IRQ_CONFLICT      - The IRQ number is conflict with others device.
//          WDT_ERROR_DEVICE_NOT_FOUND  - Not found WDT device.
//          WDT_ERROR_FAILED            - Create WDT failed.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTCreate(int Mode,int Irq);


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Delete WDT.
//
//Parameters:
// None.
//
//Return Value:
//	WDT_ERROR_SUCCESS		- Destroy WDT successful.
//	WDT_ERROR_FAILED		- Destroy WDT failed.	
//  WDT_ERROR_NOT_CREATE    - WDT not created.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTDestroy();


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Enable WDT.
//
//Parameters:
//	Unit		- WDT counter unit:
//					0 : second.
//					1 : minute.
//  Time		- Time value.
//
//Return Value:
//	WDT_ERROR_SUCCESS		        - Enable WDT successful.
//	WDT_ERROR_INVALID_PARAMETER		- Invalid parameters.	
//  WDT_ERROR_NOT_CREATE            - WDT not created.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTEnable(int Unit,int Time);


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Disable WDT.
//
//Parameters:
//	none.
//
//Return Value:
//	WDT_ERROR_SUCCESS		- Disable WDT successful.
//	WDT_ERROR_FAILED		- Disable WDT failed.	
//  WDT_ERROR_NOT_CREATE    - WDT not created.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTDisable();


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Install WDT overflow event.
//
//Parameters:
//  Operation		- the way to deal with WDT time out.
//						1 : shutdown system.
//						2 : reboot system.
//						3 : set event point by "Event" parameter.
//						4 : send message named "WDT".
//						5 : call callback function point by "CallBackFunc".
//	EventHandle		- Event handle of user created.
//	CallBackFunc    - User callback function pointer.
//	pContext		- User callback function parameter.
//
//Return Value:
//	WDT_ERROR_SUCCESS		- Install WDT Overflow event successful.
//	WDT_ERROR_FAILED		- Install WDT Overflow event failed.
//  WDT_ERROR_NOT_CREATE    - WDT not created.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTInstallEvent(int Operation,HANDLE EventHandle,WDTCallBack CallBackFunc,void *pContext);


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Uninstall WDT overflow event.
//
//Parameters:
//	none.
//
//Return Value:
//	WDT_ERROR_SUCCESS		- Uninstall WDT Overflow event successful.
//	WDT_ERROR_FAILED		- Uninstall WDT Overflow event failed.
//  WDT_ERROR_NOT_CREATE    - WDT not created.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTUninstallEvent();


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Get WDT counter current counting value.
//
//Parameters:
//	none.
//
//Return Value:
//	0 ~ 255					- WDT counter current value.
//	WDT_ERROR_NOT_SUPPORT	- Hardware not support.	
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall WDTReadCounterVal();


#endif