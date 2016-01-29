/**
* @file SWH264SourceFilter.h 
* @brief H264采集Filter
* @copyright Signalway All Rights Reserved
* @author zhaopy
* @date 2013-04-02
* @version 1.0
*/

#ifndef _SWH264_SOURCEFILTER_H_
#define _SWH264_SOURCEFILTER_H_

#include "SWBaseFilter.h"
#include "SWFilterStruct.h"
#include "SWMessage.h"
#include "tinyxml.h"

class CSWH264SourceFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWH264SourceFilter, CSWBaseFilter)
public:
	CSWH264SourceFilter();
	virtual ~CSWH264SourceFilter();


	virtual HRESULT Run();
	virtual HRESULT Stop();

public:
	/**
   *@brief 初始化
   */
	HRESULT Initialize(BOOL fEnableOverlay, PVOID pvParam);
	/**
   *@brief 设置帧率
   */
	HRESULT SetFps(INT iFps);
	/**
   *@brief 设置码率
   */
	HRESULT SetBitrate(INT iBitrate);

	/**
   *@brief 显示菜单
   */
	HRESULT OnInitMenu(WPARAM wParam, LPARAM lParam);
	/**
   *@brief 设置H264字符叠加
   */
	HRESULT OnSetCharOverlay(WPARAM wParam, LPARAM lParam);
	/**
   *@brief 读取DM368心跳包信息。
   */
	HRESULT OnGetHeartbeat(WPARAM wParam, LPARAM lParam);
	/**
   *@brief 同步DM368时间。
   */
	HRESULT OnSyncTime(WPARAM wParam, LPARAM lParam);
	/**
   *@brief CVBS菜单左按键。
   */
	HRESULT OnMenuLeft(WPARAM wParam, LPARAM lParam);
	/**
   *@brief CVBS菜单右按键。
   */
	HRESULT OnMenuRight(WPARAM wParam, LPARAM lParam);
	/**
   *@brief CVBS菜单上按键。
   */
	HRESULT OnMenuUp(WPARAM wParam, LPARAM lParam);
	/**
   *@brief CVBS菜单下按键。
   */
	HRESULT OnMenuDwon(WPARAM wParam, LPARAM lParam);
	/**
   *@brief CVBS菜单确定按键。
   */
	HRESULT OnMenuOK(WPARAM wParam, LPARAM lParam);
	/**
   *@brief CVBS菜单取消按键。
   */
	HRESULT OnMenuCancel(WPARAM wParam, LPARAM lParam);
	/**
   *@brief 取DM368版本信息。
   */
	HRESULT OnGetVersion(WPARAM wParam, LPARAM lParam);
	/**
   *@brief 设置H264参数。
   */
	HRESULT OnSetH264Param(WPARAM wParam, LPARAM lParam);

	static VOID OnH264Proxy(PVOID pvParam);

protected:	
	// H264接收线程
	HRESULT OnH264();

private:
	VOID Clear();
	
	HRESULT SetResolution(INT iResolution);

	// 数据解析
	HRESULT GetFrameInfo(PBYTE pbInfo, const DWORD& dwInfoSize, PDWORD pdwTick, PDWORD pdwFrameType, PDWORD pdwWidth, PDWORD pdwHeight);
	// 命令连接
	HRESULT ConnectCtrl();
	// 断开命令连接
	HRESULT DisconnectCtrl();
	// 发送命令
	HRESULT SendCtrlCmd(const DWORD& dwId, const DWORD& dwInfoSize, const PBYTE pbInfo, PDWORD pdwRespondSize);
	// 生成菜单XML
	HRESULT CreateMenuXML(TiXmlDocument& cXMLDoc);

	// 重启368
	HRESULT ResetDM368();
	// 初始化DM368
	HRESULT InitDM368();

protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWH264SourceFilter, CSWBaseFilter)
		SW_DISP_METHOD(SetFps, 1)
		SW_DISP_METHOD(Initialize, 2)
		SW_DISP_METHOD(SetBitrate, 1)
	SW_END_DISP_MAP()
	//消息映射宏
	SW_BEGIN_MESSAGE_MAP(CSWH264SourceFilter, CSWMessage)
	SW_MESSAGE_HANDLER(MSG_DM368_INIT_MENU, OnInitMenu)
	SW_MESSAGE_HANDLER(MSG_DM368_CHAR_OVERLAY, OnSetCharOverlay)
	SW_MESSAGE_HANDLER(MSG_DM368_HEARTBEAT, OnGetHeartbeat)
	SW_MESSAGE_HANDLER(MSG_DM368_SYNC_TIME, OnSyncTime)
	SW_MESSAGE_HANDLER(MSG_DM368_MENU_LEFT, OnMenuLeft)
	SW_MESSAGE_HANDLER(MSG_DM368_MENU_RIGHT, OnMenuRight)
	SW_MESSAGE_HANDLER(MSG_DM368_MENU_UP, OnMenuUp)
	SW_MESSAGE_HANDLER(MSG_DM368_MENU_DWON, OnMenuDwon)
	SW_MESSAGE_HANDLER(MSG_DM368_MENU_OK, OnMenuOK)
	SW_MESSAGE_HANDLER(MSG_DM368_MENU_CANCEL, OnMenuCancel)
	SW_MESSAGE_HANDLER(MSG_DM368_GET_VERSION, OnGetVersion)
	SW_END_MESSAGE_MAP()
private:
	BOOL m_fEnableOverlay;
	int m_iFps;	// 帧率： -1 ：不改变帧率。
	int m_iBitrate; // 码率
	int m_iWidth; // 图片宽
	int m_iHeight;// 图片高

	CSWMemoryFactory* m_pMemoryFactory; 
	BOOL m_fInitialized;
	CSWThread m_cThreadH264;
	CSWTCPSocket m_cSocketCtrl;
	CSWMutex m_cCtrlMutex;
	CSWTCPSocket m_cSocketH264;
	CHAR m_szCtrlIp[32];
	WORD m_wCtrlPort;
	CHAR m_szH264Ip[32];
	WORD m_wH264Port;


	static const INT s_dwSyncTime = 60 * 60 * 1000;	// 同步368时间间隔。
	DWORD m_dwLastSyncTime;

	H264_OVERLAY_INFO m_cOverlayInfo;
	H264_PARAM m_cH264Param;

	DWORD m_dwResolution;
	

	// cmd
	enum
	{
		CMD_INIT_MENU = 0xFFCC0020,
		CMD_CHAR_OVERLAY = 0xFFCC0010,
		CMD_HEARTBEAT = 0xFFCC0001,
		CMD_SYNC_TIME = 0xFFCC0004,
		CMD_MENU_CTRL = 0xFFCC0021,
		CMD_GET_VERSION = 0xFFCC0002,
		CMD_SET_H264_PARAM = 0xFFCC0030
	};

	// key
	enum
	{
		KEY_LEFT = 75,
		KEY_UP = 72,
		KEY_RIGHT = 77,
		KEY_DWON = 80,
		KEY_OK = 13,
		KEY_CANCEL = 27
	};
};
REGISTER_CLASS(CSWH264SourceFilter)

#endif

