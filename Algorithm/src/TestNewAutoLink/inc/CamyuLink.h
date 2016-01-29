#ifndef _CAMYU_LINK_H_
#define _CAMYU_LINK_H_

#include "CamyuLinkOpt.h"
#include "hvsocket.h"
#include "hvinterface.h"
#include "hvthreadbase.h"
#include "hvvartype.h"


const int MAX_RECV_BUF_SIZE = 1024 * 1024;

class CCamyuLink
{
public:
	CCamyuLink();
	~CCamyuLink();
	// 获取图片开始
	HRESULT GetImageStart(const char* pszIP);	
	// 获取图片开始Ex，调用了该函数就不会调用GetImageStart
	HRESULT GetImageStartEx(const char* pszIP, int iPort);	
	// 获取图片结束
	HRESULT GetImageStop();
	// 获取一帧图片
	HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime);
	// 在不接收相机数据的情况下,定时调用该函数来保持连接
	HRESULT ProcessData();
	// 获取当前帧类型，指定大华摄像机码流I帧或P帧 
	int GetCurFrameType();
	//获取当前车道
	int GetCurRoadNum();
	HRESULT GetImageWidthAndHeight(DWORD32& dwWidth, DWORD32& dwHeight);
	// 控制相机开始
	HRESULT CtrtCamStart(const char* pszIP);
	// 控制相机结束
	HRESULT CtrtCamStop();
	// 发送命令至相机
	HRESULT SendCommand(DWORD32 dwCmdNo, DWORD32 dwParam1 = 0, DWORD32 dwParam2 = 0);
private:
	HRESULT GetOneFrame(char* pcBuf, DWORD32* pdwSize, bool fMustData);
	HiVideo::ISocket* m_psktData;
	HiVideo::ISocket* m_psktCmd;
	HvCore::IHvStream* m_pstmData;
	HvCore::IHvStream* m_pstmCmd;
	char* m_pcTmpData;
	int m_iCurFrameType; // 大华摄像机码流类型
	int m_iRoadNum;		//当前车道号
	DWORD32 m_dwHeight;
	DWORD32 m_dwWidth;

	
};

#endif
