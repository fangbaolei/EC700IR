/*
 * SWCOMTestDevice.cpp
 *
 *  Created on: 2013Äê12ÔÂ4ÈÕ
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWCOMTestDevice.h"

#define DEBUG_TEST_COM_DEVICE
#ifdef DEBUG_TEST_COM_DEVICE
#define SW_TRACE_DEBUG SW_TRACE_NORMAL
#endif

CSWCOMTestDevice::CSWCOMTestDevice(DEVICEPARAM *pParam):CSWBaseDevice(pParam)
{
	// TODO Auto-generated constructor stub
}

CSWCOMTestDevice::~CSWCOMTestDevice() {
	// TODO Auto-generated destructor stub
}

HRESULT CSWCOMTestDevice::Run(VOID)
{
	BYTE buf[4096];	//???

	typedef struct
	{
		BYTE bySync;		//0x47
		BYTE byVersion;		//0x01
		WORD wDataLen;		//Data Length,Little Endian
	}tComTestDataHead;
	//example
	//47 01 04 00 FF FF FF FF
	
	tComTestDataHead tDatahead;

	SW_TRACE_DEBUG("<CSWCOMTestDevice> running....................................\n");

	while(S_OK == IsValid())
	{
		if (S_OK == Read(&tDatahead,sizeof(tDatahead))
				&& 0x47 == tDatahead.bySync)
		{
			swpa_thread_sleep_ms(500);
			if (0x01 != tDatahead.byVersion
					|| tDatahead.wDataLen > 4096)
			{
				tDatahead.byVersion = 0x01;
				tDatahead.wDataLen = 0;
				Write(&tDatahead,sizeof(tDatahead),1);
				SW_TRACE_DEBUG("Data head err ver 0x%2x data len %d..................\n",
						tDatahead.byVersion,tDatahead.wDataLen);
				continue;
			}
			SW_TRACE_NORMAL("Need Read Test COM Data len %d............\n",tDatahead.wDataLen);
			Read(&buf,tDatahead.wDataLen,1);
			SW_TRACE_NORMAL("Write Data head len %d.............\n",sizeof(tDatahead));
			Write(&tDatahead,sizeof(tDatahead),1);

			Write(&buf,tDatahead.wDataLen,1);

			SW_TRACE_NORMAL("Info: Respond COM test data finish, data len %d!........\n",tDatahead.wDataLen);
		}

		//swpa_thread_sleep_ms(500);
	}
	return S_OK;
}
