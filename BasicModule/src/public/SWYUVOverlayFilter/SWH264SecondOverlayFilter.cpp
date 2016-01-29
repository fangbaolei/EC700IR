#include "SWFC.h"
#include "SWH264SecondOverlayFilter.h"
//todo:临时做法
REGISTER_CLASS(CSWH264SecondOverlayFilter)

CSWH264SecondOverlayFilter::CSWH264SecondOverlayFilter()
	:CSWMessage(MSG_OVERLAY_H264_SECOND_BEGIN, MSG_OVERLAY_H264_SECOND_END)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
}

CSWH264SecondOverlayFilter::~CSWH264SecondOverlayFilter()
{
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264Enable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264Enable(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_fEnable;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264PlateEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264PlateEnable(0x%08x, 0x%08x)", wParam, lParam);
	return E_NOTIMPL;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264TimeEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264TimeEnable(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_fEnableTime;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264Text(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264Text(0x%08x, 0x%08x)", wParam, lParam);
	CHAR* szText = (CHAR *)lParam;

	// todo.
	// 需要转换换行符
	CSWString str = m_strOverlay;
	if( str.Find("\r") == -1 )
	{
		str.Replace("\n", "\r\n");
	}
	swpa_strcpy(szText, (LPCSTR)str);

	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264FontSize(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264FontSize(0x%08x, 0x%08x)", wParam, lParam);
	*(INT *)lParam = m_iFontSize;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264FontRBG(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264FontRBG(0x%08x, 0x%08x)", wParam, lParam);
	BYTE r, g, b;
	CSWUtils::YUV2RGB(m_iYColor, m_iUColor, m_iVColor, &r, &g, &b);
 //   *(INT *)lParam = SW_RGB(r, g, b);
    INT *iTmp = (INT *)lParam;
    iTmp[0] = m_iRColor;//r;
    iTmp[1] = m_iGColor;//g;
    iTmp[2] = m_iBColor;//b;

    SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264FontRBG m_iRColor: %d, m_iGColor: %d, m_iBColor: %d\n", m_iRColor, m_iGColor, m_iBColor);

	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDGetH264Pox(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDGetH264Pox(0x%08x, 0x%08x)", wParam, lParam);
	INT *iPos = (INT *)lParam;
	iPos[0] = m_iX;
	iPos[1] = m_iY;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264Enable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264Enable(0x%08x, 0x%08x)", wParam, lParam);
	m_fEnable = (INT)wParam;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264PlateEnable(WPARAM wParam, LPARAM lParam)
{
	return E_NOTIMPL;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264TimeEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264TimeEnable(0x%08x, 0x%08x)", wParam, lParam);
	m_fEnableTime = (INT)wParam;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264Text(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264Text(0x%08x, 0x%08x)", wParam, lParam);
	if (0 == swpa_strcmp((LPCSTR)wParam,"NULL"))
	{
		m_strOverlay.Clear();
	}
	else
	{
		m_strOverlay = (LPCSTR)wParam;
		// 替换特殊的换行符
		m_strOverlay.Replace("{0D0A}", "\n");
		m_strOverlay.Replace("\r\n", "\n");
	}
	
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264FontSize(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264FontSize(0x%08x, 0x%08x)", wParam, lParam);
	m_iFontSize = (INT)wParam;
	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264FontRBG(WPARAM wParam, LPARAM lParam)
{
    SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264FontRBG(0x%08x, 0x%08x)", wParam, lParam);
    INT *iTmp = (INT *)wParam;
    BYTE r = iTmp[0];//SW_R(wParam);
    BYTE g = iTmp[1];//SW_G(wParam);
    BYTE b = iTmp[2];//SW_B(wParam);

    m_iRColor = iTmp[0];
    m_iGColor = iTmp[1];
    m_iBColor = iTmp[2];


    SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264FontRBG %d %d %d", r, g ,b);
    CSWUtils::RGB2YUV(r, g, b, (BYTE *)&m_iYColor, (BYTE *)&m_iUColor, (BYTE *)&m_iVColor);

    SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264FontRBG m_iRColor: %d, m_iGColor: %d, m_iBColor: %d\n", m_iRColor, m_iGColor, m_iBColor);

	return S_OK;
}

HRESULT CSWH264SecondOverlayFilter::OnOSDSetH264Pox(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("CSWH264SecondOverlayFilter::OnOSDSetH264Pox(0x%08x, 0x%08x)", wParam, lParam);
	INT* iPos = (INT *)wParam;
	m_iX = iPos[0];
	m_iY = iPos[1];
	return S_OK;
}

