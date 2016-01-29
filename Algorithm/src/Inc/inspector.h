#pragma once

#include "swimage.h"
#include "swobjbase.h"

struct CRecognitionResult
{
	BYTE8 bResult;
	float fltProb;
};

enum IMAGE_POSITION
{
	ipOriginalPlate,	ipRectified,	ipSegmented,
	ipSegment1,         ipSegment2,     ipSegment3,
	ipSegment4,         ipSegment5,     ipSegment6,
	ipSegment7,         ipSegment8,     ipSegment9,
	ipSegment10,        ipSegment11,    ipSegment12,
	ipSegment13,        ipSegment14,    ipSegment15,
	ipChar0,			ipChar1,		ipChar2,
	ipChar3,			ipChar4,		ipChar5,
	ipCharMax,			ipBlank0,		ipBlank1,
	ipTrack0,			ipTrack1,		ipTrack2,
	ipTrack3,			ipTrackMax,		ipCount
};

class IInspector
{
public:
	STDMETHOD(ShowComponentImage)(
		int nPos,
		const HV_COMPONENT_IMAGE *pImage,
		PVOID pvInfo = NULL
	)=0;
	STDMETHOD(ShowGrayImage)(
		int nPos,
		const HV_COMPONENT_IMAGE *pImage,
		PVOID pvInfo = NULL
	)=0;
	STDMETHOD(SmartSave)(
		 const HV_COMPONENT_IMAGE &image
	)=0;
	STDMETHOD(SaveAnalyzeImg)(
		const char* szSavePath,
		const HV_COMPONENT_IMAGE &image,
		BOOL fSaveAsGray = FALSE
	)=0;
};

