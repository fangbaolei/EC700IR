//////////////////////////////////////////////////////////////////////////////
//Module Name:
//
//    WDTDll.h  
//
//Abstract:   WDT interface.
//
//Environment:
//
//    Windows2000/Windows XP
//
//Author:    mz.yang  may, 2006
//
//////////////////////////////////////////////////////////////////////////////

#define WDT_WORK_IN_RESET				0
#define	WDT_WORK_IN_INTERRUPT			1	

#define WDT_COUNT_UNIT_SECOND			0
#define WDT_COUNT_UNIT_MINUTE			1

#define IRQ_SEARCH_AUTO				   -1

#define IRQ_SEARCH_AUTO_OK				1
#define IRQ_SEARCH_AUTO_ERROR			0

#define IRQ_VALID						1
#define IRQ_INVALID						0

#define OVERFLOW_EVENT_SHUTDOWN 		0
#define OVERFLOW_EVENT_REBOOT			1
#define OVERFLOW_EVENT_SET_EVENT		2
#define OVERFLOW_EVENT_SEND_MESSAGE		3
#define OVERFLOW_EVENT_CALL_FUNC		4

////////////////////////////////////////////////////////////////////////////////

// User function point type.
#ifndef CallBackFunc
	typedef void (_stdcall * CallBackFunc)(void *pContext);
#endif

////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Initial and set WDT working mode.
//
//Parameters:
//	mode - value of mode,"0" is working in reset,"1" is 
//			working in interrupt.
//	irq  - IRQ number.when "mode" set to "0",this parameter is invalid and can  
//		   set to any value.
//		-1		: Search IRQ number automatic from system.
//		0 ~ 15	: User give IRQ number.
//
//Return Value:
//	Reset mode:
//		0 : Function called failed.
//		1 : Function called succeed.
//	Interrupt mode:
//		Serch IRQ number automatic:	
//			0		: No interrupt resource can use in the system.
//			3 ~ 15	: IRQ number(isa IRQ : 3,4,5,6,7,9,10,11,12,14,15).
//		User set IRQ:
//			0		: Invalid IRQ number.
//			1		: Valid IRQ number.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall EvocWDTSetMode(int mode,int irq);


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Enable WDT,it will be start counting down.
//
//Parameters:
//	unit - WDT counting unit. 
//		0 : second
//		1 : minutes
//	time - WDT overflow time,1 <= time <=255.	
//
//Return Value:
//	true  : Function called failed.
//	false : Function called succeed.
//			
//////////////////////////////////////////////////////////////////////////
bool _stdcall EvocEnableWDT(int unit,int time);


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Disnable WDT,stop WDT counting down.
//
//Parameters:
//	None.
//
//Return Value:
//	true  : Function called failed.
//	false : Function called succeed.
//			
//////////////////////////////////////////////////////////////////////////
bool _stdcall EvocDisnableWDT();


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Wait for WDT overflow,and deal with overflow.
//
//Parameters:
//	Operate - the way to deal with WDT overflow.
//		0 : shutdown.
//		1 : reboot.
//		2 : set event.
//		3 : send message.
//		4 : call function.
//	hWDTEvent - Event handle of user created.
//	MyFunc    - User callback function pointer.
//	pContext  - User callback function parameter.
//
//Return Value:
//	true  : Function called failed.
//	false : Function called succeed.
//////////////////////////////////////////////////////////////////////////
bool _stdcall WaitForWDTOverflow(int Operate,HANDLE hWDTEvent,CallBackFunc MyFunc,void *pContext);


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Read WDT current count value.
//	Note: this function is use for W83977F/AF,W83627HF/THF/EHF/DHF super IO,
//		  user can't get WDT current count value from SCH311x super IO.
//
//Parameters:
//	None.
//
//Return Value:
//	WDT current count value.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall ReadCurrentCountData();


/////////////////////////////////////////////////////////////////////////
//Function Description:
//	Get WDT hardware ID.
//
//Parameters:
//	None.
//
//Return Value:
//	WDT hardware ID.
//			
//////////////////////////////////////////////////////////////////////////
int _stdcall GetWDTHardwareID();
