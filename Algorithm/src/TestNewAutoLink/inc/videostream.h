#ifndef _VIDEOSTREAM_INCLUDED__
#define _VIDEOSTREAM_INCLUDED__

#include "config.h"
#include "swBaseType.h"
#include "swObjBase.h"
#include "swImage.h"
#include "VideoBaseType.h"

class IVideoStream
{
public:
	virtual ~IVideoStream(){};
	IVideoStream(){};

	STDMETHOD(Initialize)(
		int 				iPort,
		VideoOptionParam	*pParam,
		int 				iSegId
	) = 0;
	
	STDMETHOD( GetOneFrame )(
		HV_COMPONENT_IMAGE 	*pImage,
		VP_IMAGE_INFO		*pImageInfo
	) = 0;
	
	STDMETHOD( FreeOneFrame )(
		HV_COMPONENT_IMAGE 	*pImage,
		VP_IMAGE_INFO		*pImageInfo
	) = 0;
	
	STDMETHOD( SetCallBack )(
		VIDEO_CALLBACK 		pfnCaptureCallback, 
		PVOID 				pvUserData
	) = 0;
	
	STDMETHOD( GetPortStatus )( PBOOL pfStatus ) = 0;
	
	STDMETHOD( GetBuffNum )( PINT piNum ) = 0;
	STDMETHOD( SetBuffNum )( int iNum ) = 0;	
	
	STDMETHOD( GetBrightness )( PINT piBrightness ) = 0;
	STDMETHOD( SetBrightness )( int iBrightness ) = 0;
	STDMETHOD( GetSaturation )( PINT piSaturation, int iType ) = 0;
	STDMETHOD( SetSaturation )( int iSaturation, int iType ) = 0;
	STDMETHOD( GetContrast )( PINT piContrast ) = 0;
	STDMETHOD( SetContrast )( int iContrast ) = 0;
	STDMETHOD( GetHue )( PINT piHue ) = 0;
	STDMETHOD( SetHue )( int iHue ) = 0;	
	STDMETHOD( GetStatus )(
		PDWORD32 		pdwValue,
		DWORD32			dwFirstLen,
		PDWORD32		pdwReadLen
	) = 0;
	STDMETHOD( ResetDevice )( PBOOL pfStatus ) = 0;
	STDMETHOD( ResetDriver )( PBOOL pfStatus ) = 0;
};

HRESULT CreateVideoStreamInstance( IVideoStream** ppVideo );

#endif // _VIDEOSTREAM_INCLUDED__
