/**
* @file SWTransmittingOpt.h
* @brief some common enum and definition 
* 
*/
#ifndef _SW_TRANSMITTING_OPT_H_
#define _SW_TRANSMITTING_OPT_H_

enum {
	TRANSMITTING_HISTORY = 0,
	TRANSMITTING_REALTIME,		
	
	TRANSMITTING_TYPE_COUNT
};


enum
{
	DATATYPE_IMAGE = 0,
	DATATYPE_VIDEO,
	DATATYPE_RECORD,

	DATATYPE_COUNT
};


enum {
	HISTORY_TRANSMITTING_NOTSTARTED= 0,
	HISTORY_TRANSMITTING_ONGOING,	
	HISTORY_TRANSMITTING_FINISHED,
	
	HISTORY_TRANSMITTING_COUNT
};

typedef struct _HISTORY_FILE_TRANSMITTING_PARAM
{
	SWPA_DATETIME_TM* psBeginTime;
	SWPA_DATETIME_TM* psEndTime;
	DWORD dwCarID;

	_HISTORY_FILE_TRANSMITTING_PARAM()
	{
		psBeginTime = psEndTime = NULL;
		dwCarID = 0;
	}
	
} HISTORY_FILE_TRANSMITTING_PARAM;

typedef struct _HISTORY_GB28181_TRANSMITTING_PARAM
{
    SWPA_DATETIME_TM* psBeginTime;
    SWPA_DATETIME_TM* psEndTime;
    DWORD dwCarID;
    BOOL  fBackward;

    _HISTORY_GB28181_TRANSMITTING_PARAM()
    {
        psBeginTime = psEndTime = NULL;
        dwCarID = 0;
        fBackward = FALSE;
    }

} HISTORY_GB28181_TRANSMITTING_PARAM;
#endif  //_SW_TRANSMITTING_OPT_H_

