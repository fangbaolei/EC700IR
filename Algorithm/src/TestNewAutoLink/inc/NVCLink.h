#ifndef _NVCLINK_H_
#define _NVCLINK_H_

#include "..\protocol\tftp_ptl.h"

#define RCV_BEGIN_TIMEOUT 100	// 接收开始超时
#define RCV_TIMEOUT 500			// 接收超时
#define MAX_RECV_SIZE 2048	// UDP最大包长度

const DWORD32 JPEG_HEAD_SIZE = 595;

class CNVCLink
{
public:
	CNVCLink();
	~CNVCLink();
	HRESULT Connect(const char* pszIP);	
	HRESULT ReadRegValue(BYTE8 bAddr, WORD16& wValue);
	HRESULT WriteRegValue(BYTE8 bAddr, WORD16 wValue);
	HRESULT GetHeight(WORD16& wHeight);
	HRESULT GetWidth(WORD16& wWidth);
	HRESULT GetDevVer(WORD16& wDevVer);
	HRESULT GetDevType(WORD16& wDevType);
	HRESULT SaveConfigForever();
	HRESULT RestoreDefaultConfig();
	HRESULT SetImgInfo(WORD16 wWidth = 2048, WORD16 wHeight = 1536);
	HRESULT SetQuality(WORD16 wQuality = 11);
	HRESULT GetJpgImge(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, WORD16 wRes = 1);
#if (RUN_PLATFORM == PLATFORM_WINDOWS)
	static HRESULT SearchNvcCount(DWORD32& dwCount);
	static HRESULT GetNvcAddr(int iIndex, DWORD64& dw64MacAddr, DWORD32& dw32IP);
	static HRESULT SetNvcIPFromMac(DWORD64 dw64Mac, DWORD32 dw32IP);
#endif
private:
	CTftpPtl m_tftp_ptl;
	BYTE8* m_pbRevBuf;
	int m_iQuality;
	WORD16 m_wWidth;
	WORD16 m_wHeight;
	BYTE8 m_rgbJpegHeader[JPEG_HEAD_SIZE];
};

#endif
