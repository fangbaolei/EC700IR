#ifndef _HV_JPEG_PROCESSER_H_
#define _HV_JPEG_PROCESSER_H_

#include "swbasetype.h"
#include "HvInterface.h"
#include "swImage.h"

namespace HiVideo
{
	// jpeg压缩解压接口
	class IJpegProcesser
	{
	public:
		virtual ~IJpegProcesser() {};
		virtual BOOL DecodeJPEG2YUV(
			BYTE8* lpJpgBuffer,
			DWORD32 dwJpgBufferSize,
			BYTE8* lpYUVBuffer,
			DWORD32* lpdwWidth,
			DWORD32* lpdwHeight,
			DWORD32* lpdwNumberOfChannels
			) = 0;
		virtual BOOL EncodeYUV2JPEG(
			int iCompressionRadio,
			HV_COMPONENT_IMAGE* pImage,
			BYTE8** ppbJpegBuffer,
			DWORD32* pdwJpgBufferSize,
			BOOL fVScale = FALSE
			) = 0;
		static HRESULT CreateInstance(IJpegProcesser **ppTarget);
	};

	// jpeg保存接口
	class IJpegSaver
	{
	public:
		virtual ~IJpegSaver() {};
		enum {NORMAL_FILE = 0, STORAGE_FILE};
		virtual HRESULT SaveJpeg(
			char* pszMainPath,
			BYTE8* pbData, 
			DWORD32 dwSize, 
			int iFileType, 
			bool fTermOfHour
			) = 0;
		static HRESULT CreateInstance(IJpegSaver **ppTarget);
	};

	//保存视频接口
	class IVideoSaver
	{
	public:
		virtual ~IVideoSaver() {};
		virtual HRESULT SaveVideoData(const char* lpszPath, const char* lpszFileName,
			const unsigned char* pbData, const int& iSize) = 0;
	};
	HRESULT CreateVideoSaverInstance(IVideoSaver **ppTarget);
}

#endif
