#ifndef __SW_CENTAURUS_H264_ENCODE_FILTER_H__
#define __SW_CENTAURUS_H264_ENCODE_FILTER_H__
#include "SWBaseFilter.h"

class CSWCentaurusH264EncodeFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWCentaurusH264EncodeFilter, CSWBaseFilter)
public:
	CSWCentaurusH264EncodeFilter();
	virtual ~CSWCentaurusH264EncodeFilter();
protected:
	virtual HRESULT Receive(CSWObject* obj);
	virtual HRESULT Run();
	virtual HRESULT Stop();
	
protected:
	
	HRESULT Initialize(PVOID pvParam);

	/**
	*@brief H264压缩回调函数
	*
	*/
	INT PackH264Frame(VOID *pvDataStruct);	
	static INT PackH264FrameCallback(VOID *pvContext, INT iType, VOID *pvDataStruct);	

	HRESULT SendH264();
	static PVOID SendH264Proxy(PVOID pvArg);

	HRESULT OnSetH264BitRate(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetH264BitRate(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264IFrameInterval(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetH264IFrameInterval(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264RateControl(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264VbrDuration(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264VbrSensitivity(WPARAM wParam, LPARAM lParam);

	HRESULT OnSetH264SecondBitRate(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetH264SecondBitRate(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264SecondIFrameInterval(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetH264SecondIFrameInterval(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264SecondRateControl(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264SecondVbrDuration(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264SecondVbrSensitivity(WPARAM wParam, LPARAM lParam);


	SW_BEGIN_MESSAGE_MAP(CSWCentaurusH264EncodeFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_SET_H264_BITRATE, OnSetH264BitRate)
		SW_MESSAGE_HANDLER(MSG_GET_H264_BITRATE, OnGetH264BitRate)
		SW_MESSAGE_HANDLER(MSG_SET_H264_I_FRAME_INTERVAL, OnSetH264IFrameInterval)
		SW_MESSAGE_HANDLER(MSG_GET_H264_I_FRAME_INTERVAL, OnGetH264IFrameInterval)
		SW_MESSAGE_HANDLER(MSG_SET_H264_RATE_CONTROL, OnSetH264RateControl)
		SW_MESSAGE_HANDLER(MSG_SET_H264_VBR_DURATION, OnSetH264VbrDuration)
		SW_MESSAGE_HANDLER(MSG_SET_H264_VBR_SENSITIVITY, OnSetH264VbrSensitivity)
		SW_MESSAGE_HANDLER(MSG_SET_H264_SECOND_BITRATE, OnSetH264SecondBitRate)
		SW_MESSAGE_HANDLER(MSG_GET_H264_SECOND_BITRATE, OnGetH264SecondBitRate)
		SW_MESSAGE_HANDLER(MSG_SET_H264_SECOND_I_FRAME_INTERVAL, OnSetH264SecondIFrameInterval)
		SW_MESSAGE_HANDLER(MSG_GET_H264_SECOND_I_FRAME_INTERVAL, OnGetH264SecondIFrameInterval)
		SW_MESSAGE_HANDLER(MSG_SET_H264_SECOND_RATE_CONTROL, OnSetH264SecondRateControl)
		SW_MESSAGE_HANDLER(MSG_SET_H264_SECOND_VBR_DURATION, OnSetH264SecondVbrDuration)
		SW_MESSAGE_HANDLER(MSG_SET_H264_SECOND_VBR_SENSITIVITY, OnSetH264SecondVbrSensitivity)

	SW_END_MESSAGE_MAP()
	
private:
	BOOL IsInited() {return m_fInited;};

	/**
	*@brief 打印运行状态信息，如H264压缩率、高宽等
	*
	*/
	HRESULT PrintRunningInfo(CSWImage* pImage);
	
	HRESULT SetResolution(INT iResolution, INT ChannelNum );

	SW_BEGIN_DISP_MAP(CSWCentaurusH264EncodeFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 1)
	SW_END_DISP_MAP();
private:
 	BOOL m_fInited;
	H264_PARAM m_sH264Param;

	DWORD m_dwQueueSize;
	CSWList<CSWImage*> m_lstH264Frame;
	//CSWMutex m_mutexLock;
	CSWSemaphore * m_pSemaLock;
	CSWSemaphore m_semaFrame;
	CSWThread	m_cSendThread;
	IMAGE_EXT_INFO m_cExtInfo;		//帧扩展信息，如RGB


	unsigned char m_rgbData[256];
	INT m_iChannelId;				//通道号，0-主H264,2-辅H264
};
REGISTER_CLASS(CSWCentaurusH264EncodeFilter)
#endif //__SW_CENTAURUS_H264_ENCODE_FILTER_H__

