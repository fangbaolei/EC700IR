#pragma once
#include "hvvartype.h"
#include "hvthreadbase.h"
#include "hvsocket.h"

const int LISTEN_PORT = 9999; // 抓拍图像侦听端口
const int MAX_RECV_THREAD = 10;
const int MAX_RECV_BUF_LEN = 1048576; // 1M
const int MAX_VIDEO_DATA_LEN = 4194304; // 4M

// 抓拍相机数据接收线程
class CCapCamRecvThread : public CHvThreadBase
{
public:
	CCapCamRecvThread(HiVideo::ISocket* pSocket);
	~CCapCamRecvThread(void);
	// CHvThreadBase Interface
	virtual HRESULT Run(void* pvParam);
private:
	HiVideo::ISocket* m_pSocket;
};


#if (RUN_PLATFORM == PLATFORM_WINDOWS)
//
// 抓拍相机数据接收线程
class CNCCamRecvThread : public CHvThreadBase
{
public:
	CNCCamRecvThread(void);
	~CNCCamRecvThread(void);
	// CHvThreadBase Interface
	virtual HRESULT Run(void* pvParam);

public:

	static const int MAX_CAM_IP_COUNT = 50;
	typedef struct
	{
		BYTE	CamId;			//相机ID
		BYTE	CamIp[4];	//相机IP地址
	}CAMLIST;

	typedef struct
	{
		DWORD32 dwCamCount;
		DWORD32 rgdwSignalTypeNum[MAX_CAM_IP_COUNT];
		CAMLIST   rgCamList[MAX_CAM_IP_COUNT];
	}CAMSIGNAL;

	CAMSIGNAL m_camsignal;

	typedef UINT (__cdecl *CALLBACK_PROC)(LPVOID);

	typedef bool (WINAPI *MYFUNC1 )(BYTE *);
	typedef bool (WINAPI *MYFUNC2 )();
	typedef bool (WINAPI *MYFUNC3 )(BYTE *);
	typedef bool (WINAPI *MYFUNC4 )(BYTE *,BYTE *);
	typedef bool (WINAPI *MYFUNC5 )(BYTE,CAMLIST *);
	typedef bool (WINAPI *MYFUNC6 )(BYTE *);
	typedef bool (WINAPI *MYFUNC7 )(BYTE *,BYTE);
	typedef bool (WINAPI *MYFUNC8 )(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC9 )(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC10)(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC11)(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC12)(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC13)(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC14)(BYTE *,BYTE);
	typedef bool (WINAPI *MYFUNC15)(BYTE *,BYTE);
	typedef bool (WINAPI *MYFUNC16)(BYTE *,BYTE);
	typedef bool (WINAPI *MYFUNC17)(BYTE *,BYTE);
	typedef bool (WINAPI *MYFUNC18)(BYTE *);
	typedef bool (WINAPI *MYFUNC19)(BYTE *);
	typedef bool (WINAPI *MYFUNC20)(BYTE *,BYTE *);
	typedef bool (WINAPI *MYFUNC21)(BYTE *,BYTE *);
	typedef int (WINAPI *MYFUNC22)(CALLBACK_PROC,LPVOID);
	typedef bool (WINAPI *MYFUNC23)(BYTE *);
	typedef int (WINAPI *MYFUNC24)(CALLBACK_PROC,LPVOID);
	typedef bool (WINAPI *MYFUNC25)(BYTE*,WORD *,WORD *,BYTE*);
	typedef int (WINAPI *MYFUNC26)(CALLBACK_PROC,LPVOID);
	typedef bool (WINAPI *MYFUNC27)(BYTE*,BYTE*);
	typedef bool (WINAPI *MYFUNC28)(BYTE*,BYTE*);
	typedef bool (WINAPI *MYFUNC29)(HDC,WORD,WORD,WORD,WORD,BYTE *);
	typedef bool (WINAPI *MYFUNC30)(LPCTSTR,WORD,WORD,BYTE *);
	typedef bool (WINAPI *MYFUNC31)(LPCTSTR,WORD,WORD,BYTE,BYTE *);
	typedef bool (WINAPI *MYFUNC32)(BYTE,LPCTSTR);
	typedef bool (WINAPI *MYFUNC33)(BYTE *,BYTE);
	typedef bool (WINAPI *MYFUNC34)(BYTE *,BOOL,BYTE,BYTE,BYTE);
	typedef bool (WINAPI *MYFUNC35)(BYTE *,BOOL);
	typedef bool (WINAPI *MYFUNC36)(BYTE *,WORD);
	typedef bool (WINAPI *MYFUNC37)(BYTE*,DWORD*,WORD,WORD,BYTE,BYTE *);

	MYFUNC1 	NC_SystemInit	;			
	MYFUNC2 	NC_SystemFree	;			
	MYFUNC3 	NC_Reset		;			
	MYFUNC4 	NC_GetActip		;
	MYFUNC5 	NC_SetCamlist	;			
	MYFUNC6 	NC_GetCaminfo	;			
	MYFUNC7 	NC_SetAWB		;			
	MYFUNC8 	NC_SetR			;			
	MYFUNC9 	NC_SetG			;
	MYFUNC10	NC_SetB			;			
	MYFUNC11	NC_SetExpor		;			
	MYFUNC12	NC_SetGain		;			
	MYFUNC13	NC_SetBlk		;			
	MYFUNC14	NC_SetPrvMode	;			
	MYFUNC15	NC_SetLut		;			
	MYFUNC16	NC_Setprive		;			
	MYFUNC17	NC_Setflash		;			
	MYFUNC18	NC_SetSave		;			
	MYFUNC19	NC_SetSnapShot	;			
	MYFUNC20	NC_SetCamip		;			
	MYFUNC21	NC_SetServerip	;				
	MYFUNC22	NC_CaminfoCallback;
	MYFUNC23	NC_GetCaminfobuf;			
	MYFUNC24	NC_CamPreviewCallback;
	MYFUNC25	NC_GetCamPreviewbuf;
	MYFUNC26	NC_CamPhotoCallback;
	MYFUNC27	NC_GetCamRawbuf;			
	MYFUNC28	NC_GetCamPhotobuf;
	MYFUNC29	NC_DrawDisplay;
	MYFUNC30	NC_SaveImgBMP;
	MYFUNC31	NC_SaveImgJPG;
	MYFUNC32	NC_Updatefireware;			
	MYFUNC33	NC_SetNetMode;	
	MYFUNC34	NC_SetAEC;
	MYFUNC35	NC_SetAFC;
	MYFUNC36	NC_SetPAGDELAY;
	MYFUNC37	NC_BMP2JPG;


	UINT CaminfoProc();
	UINT CamPhotoProc();
	UINT CamPreviewProc();

	int AddCamIp(DWORD32 dwSignalTypeNum, BYTE* pbIp);

private:
	static const int IMAGEWIDTH = 1360;
	static const int IMAGEHIGTH = 1024;
	static const int IMAGESIZE = 1360*1024;

	HINSTANCE   m_hInst;

	typedef struct{
		BYTE	HAND[4];		//标记头
		DWORD	       CAMID;			//相机ID
		DWORD 	FVER;			//FPGA版本
		DWORD 	DVER;			//DSP版本
		BYTE	CAMDMAC[6];		//相机MAC
		BYTE	CAMIPAddress[4];	//相机IP地址
		BYTE	SERDMAC[6];		//服务器MAC
		BYTE	SERIPAddress[4];	//服务器IP地址
		WORD	sourceport;		//源端口
		WORD	targetport;		//目的端口
		WORD	Total_Pixel;	//总像素
		WORD	Width;			//原始宽
		WORD	Height;			//原始高
		WORD	R;
		WORD	G;
		WORD	B;
		WORD	exp;			//曝光
		WORD	gain;			//增益
		WORD	blckl;			//黑电平
		BYTE	mode;			//模式
		BYTE	lut;			//查找表
		WORD	shaketime;		//握手时间
		WORD	shaketimeov;	//超时时间
		BYTE	previwen;		//预览
		BYTE	flashen;		//抓拍补光灯
		BYTE	AWB;			//自动白平衡
		BYTE	AFE;			//自动曝光
		BYTE	NETMODE;		//网络模式
		BYTE	AECTOP;			//最大曝光值
		BYTE	AGCTOP;			//最大增益值
		BYTE	BLC;			//曝光补偿值
		WORD	PAGDELAY;		//数据包间延时
		BYTE	AFL;			//自动抓拍补光灯
	}CAMINFO;
	CAMINFO * m_pCamInfo;  //相机属性值

	typedef struct
	{
		WORD	Imgtype;			//图象类型
		WORD	Width;				//图象宽
		WORD	Height;				//图象高
		WORD	Carspeed1;			//车速1
		WORD	Carspeed2;			//车速2
		WORD	Carlengh1;			//车长1
		WORD	Carlengh2;			//车长2
		WORD	Radspeed;			//雷达车速
		WORD	Radlengh;			//雷达车长
		WORD	Res ;				//保留
		BYTE	ImageData[IMAGESIZE*3];    //数据区 
	}IMGinfo;
	IMGinfo* m_pImgInfo;

	BYTE* m_pbInfoBuff;
	BYTE* m_pbPhotoBuff;
	BYTE* m_pbJpegBuff;

	bool m_fInitialized;
};
#else
//
// 抓拍相机数据接收线程
class CNCCamRecvThread : public CHvThreadBase
{
public:
	CNCCamRecvThread(void);
	~CNCCamRecvThread(void)
	{
		m_fExit = true;
		StopThread(-1);
	};
	// CHvThreadBase Interface
	virtual HRESULT Run(void* pvParam)
	{
		m_fExit = true;
		return E_FAIL;
	}
};
#endif

// 抓拍相机侦听线程
class CCapCamThread : public HiVideo::IRunable
{
public:
	CCapCamThread(void);
	~CCapCamThread(void);
	HRESULT Create();
	HRESULT Close();
	bool IsConnected();
	bool ThreadIsOk();
	virtual HRESULT Run(void* pvParamter);
protected:
	bool m_fRunControl;
	bool m_fRunStatus;
	HiVideo::IThread* m_pThread;
	CCapCamRecvThread* m_rgpRecvThread[MAX_RECV_THREAD];

#ifndef _NML_DEVICE 
	char m_rgbRecvBuf[MAX_RECV_BUF_LEN];
	char m_rgbVideoData[MAX_VIDEO_DATA_LEN];
#endif

	DWORD32 m_dwCurVideoPos;
};
