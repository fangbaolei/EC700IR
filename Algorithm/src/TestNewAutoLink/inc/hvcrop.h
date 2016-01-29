#ifndef _HV_CROP_H
#define _HV_CROP_H

#include "HvUtils.h"

extern wchar_t g_wszCropDir[];
extern int g_nCropID;

void SaveGrayImage( 
				   wchar_t* szPath, 
				   void* pbImg, 
				   int nWidth, 
				   int nHeight,
				   int nStrideWidth
				   );

void SaveImage( 
			   wchar_t* szPath, 
			   HV_COMPONENT_IMAGE* pImg
			   );

void SaveText(
			  const char* szPath,
			  char* szMsg
			  );

void SaveDebugText(
			  const char* szPath,
			  char* szMsg,
			  ...
			  );

#endif
