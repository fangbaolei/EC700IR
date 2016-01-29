#ifndef __SW_PTZ_PARAMETER_H__
#define __SW_PTZ_PARAMETER_H__

#include "SWBaseType.h"

typedef struct __PTZ_INFO
{ 
	BOOL fHomeSupported;
	BOOL fFixedHomePos;

	BOOL fAbsoluteMoveSupport;
	INT iAbsolutePRangeMax;
	INT iAbsolutePRangeMin;
	INT iAbsoluteTRangeMax;
	INT iAbsoluteTRangeMin;
	INT iAbsoluteZRangeMax;
	INT iAbsoluteZRangeMin;

	BOOL fContinuousMoveSupport;
	INT iContinuousPSpeedMax;
	INT iContinuousPSpeedMin;
	INT iContinuousTSpeedMax;
	INT iContinuousTSpeedMin;
	INT iContinuousZSpeedMax;
	INT iContinuousZSpeedMin;

	INT iPresetCount;
}PTZ_INFO, *LP_PTZ_INFO;


#endif
