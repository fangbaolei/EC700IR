#pragma once
#include <Windows.h>



#pragma comment(lib, "./lib/avutil.lib")
#pragma comment(lib, "./lib/avcodec.lib")
#pragma comment(lib, "./lib/avformat.lib")
#pragma comment(lib, "./lib/swscale.lib")
#pragma comment(lib, "./lib/DShow.lib")

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")



extern "C" __declspec(dllimport) bool DSShowVideo(HWND hParent, int width, int height ,char * pImage, int size);
extern "C" __declspec(dllimport) HANDLE DSInitializeH264Decode(int width, int height);
//extern "C" __declspec(dllexport) bool DSDecodeH264(HANDLE hH264DecodeHandle, char * pH264Image, int iH264Size, char *pRGBImage, int iRGBSize);
//extern "C" __declspec(dllexport) void DSReleaseH264Decode(HANDLE hH264DecodeHandle);
extern "C" __declspec(dllexport) void DSShowReleaseVideo(HWND hParent);


#include "video_render.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// CFFmpeg

class CFFmpeg : public CWnd
{
	DECLARE_DYNAMIC(CFFmpeg)

public:
	CFFmpeg();
	virtual ~CFFmpeg();

public:
	AVStream* m_video_st;
	AVFormatContext* m_video_oc;
	BOOL m_fOpenFile;

	int H264VideoSaverInit();
	int H264VideoSaverOpen(const char* szOutputFile, int iFrameNum, int iWith, int iHeight);
	int H264VideoSaverWirteOneFrame(PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame);
	int H264VideoSaverClose();

protected:
	DECLARE_MESSAGE_MAP()
};


//Ω‚¬Îœ‘ æ¿‡
class CH264FrameDecode
{
public:
	
	CH264FrameDecode(void);
	~CH264FrameDecode(void);
	
	static BOOL H264Env_Init();
	static BOOL H264Env_Release();
	BOOL Init();
	BOOL Release();

	BOOL H264_Decode(const PBYTE pSrcData, const DWORD dwDataLen,  int * pnWidth, int * pnHeight, HWND hWnd);

	

private:

	AVCodecContext  *m_pCodecCtx;
	AVFrame  *m_pavfFrame;
	AVPacket m_AVPkt;
	HWND m_showhWnd;

	video_render *m_pVideoRender;
	int m_iWidth;
	int m_iHeight;
};





static int ANSIToUTF8(const char *pszCode, char *UTF8code)
{
	WCHAR Unicode[512]={0}; 
	char utf8[512]={0};

	// read char Lenth
	int nUnicodeSize = MultiByteToWideChar(CP_ACP, 0, pszCode, (int)strlen(pszCode), Unicode, sizeof(Unicode)); 

	// read UTF-8 Lenth
	int nUTF8codeSize = WideCharToMultiByte(CP_UTF8, 0, Unicode, nUnicodeSize, UTF8code, sizeof(Unicode), NULL, NULL); 

	// convert to UTF-8 
	MultiByteToWideChar(CP_UTF8, 0, utf8, nUTF8codeSize, Unicode, sizeof(Unicode)); 

	return nUTF8codeSize;
}


