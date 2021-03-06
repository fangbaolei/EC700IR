#ifndef __SW_H264_SECOND_OVERLAY_FILTER_H__
#define __SW_H264_SECOND_OVERLAY_FILTER_H__
#include "SWMessage.h"
#include "SWYUVOverlayFilter.h"

class CSWH264SecondOverlayFilter : public CSWYUVOverlayFilter,CSWMessage
{
	CLASSINFO(CSWH264SecondOverlayFilter, CSWYUVOverlayFilter)
public:
	CSWH264SecondOverlayFilter();
	virtual ~CSWH264SecondOverlayFilter();
protected:
	HRESULT OnOSDGetH264Enable(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDGetH264PlateEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDGetH264TimeEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDGetH264Text(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDGetH264FontSize(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDGetH264FontRBG(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDGetH264Pox(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264Enable(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264PlateEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264TimeEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264Text(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264FontSize(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264FontRBG(WPARAM wParam, LPARAM lParam);
	HRESULT OnOSDSetH264Pox(WPARAM wParam, LPARAM lParam);

protected:
	//��Ϣӳ���
	SW_BEGIN_MESSAGE_MAP(CSWH264SecondOverlayFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_ENABLE,       OnOSDGetH264Enable)
    SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_PLATE_ENABLE, OnOSDGetH264PlateEnable)
    SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_TIME_ENABLE,  OnOSDGetH264TimeEnable)
    SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_TEXT,         OnOSDGetH264Text)
    SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_FONNTSIZE,    OnOSDGetH264FontSize)
    SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_FONT_RBG,     OnOSDGetH264FontRBG)
    SW_MESSAGE_HANDLER(MSG_OSD_GET_H264_SECOND_POX,          OnOSDGetH264Pox)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_ENABLE,       OnOSDSetH264Enable)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_PLATE_ENABLE, OnOSDSetH264PlateEnable)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_TIME_ENABLE,  OnOSDSetH264TimeEnable)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_TEXT,         OnOSDSetH264Text)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_FONNTSIZE,    OnOSDSetH264FontSize)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_FONT_RBG,     OnOSDSetH264FontRBG)
    SW_MESSAGE_HANDLER(MSG_OSD_SET_H264_SECOND_POX,          OnOSDSetH264Pox)
	SW_END_MESSAGE_MAP()	
};
//REGISTER_CLASS(CSWH264SecondOverlayFilter)
#endif
