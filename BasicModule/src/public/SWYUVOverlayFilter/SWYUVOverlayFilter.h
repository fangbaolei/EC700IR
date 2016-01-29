#ifndef __SW_YUV_OVERLAY_FILTER_H__
#define __SW_YUV_OVERLAY_FILTER_H__
#include "SWBaseFilter.h"
#include "SWCarLeft.h"
#include "FreetypeUtils.h"

//#define YUV_TEST

class CSWYUVOverlayFilter : public CSWBaseFilter
{
	CLASSINFO(CSWYUVOverlayFilter, CSWBaseFilter)
	typedef struct tagTEXT
	{
		CHAR  szText[3];
		INT   iSize;
		INT   iWidth;
		INT   iHeight;		
 		
		tagTEXT()
		{
			swpa_memset(this, 0, sizeof(this));
		}
	}TEXT;
public:
	CSWYUVOverlayFilter();
	virtual ~CSWYUVOverlayFilter();
    HRESULT Initialize(BOOL fEnable, BOOL fEnableTime, LPCSTR szOverlay,
        INT iFontSize, INT iX, INT iY, INT iYColor, INT iUColor, INT iVColor, LPCSTR szDeviceID, INT iSpeedLimit, DWORD* dwArrayRGB);
protected:
	virtual HRESULT Receive(CSWObject* obj);
	virtual HRESULT Run();
	virtual HRESULT CalcOSDInfoHeight(
	        LPCSTR szText,
	        INT iImgWidth,
	        INT *piOSDHeight,
	        INT *pnLineNum = NULL,
	        INT nLimitLine = 2);
	virtual HRESULT DoOverlay(CSWImage *pImage, LPCSTR szText, INT nLineNum = -1);
	CSWString RuleString(DWORD dwOverlayTime, CSWCarLeft *pCarLeft = NULL, CSWImage* pImage = NULL);
	TEXT* GetTextBuffer(LPCSTR szText);
	
	TEXT* GetPTZTextBuffer(LPCSTR szText);
	HRESULT DomeOverlay(CSWImage *pImage);
	virtual HRESULT DoOverlayPTZ(CSWImage *pImage, LPCSTR szText, INT iXCoord, INT iYCoord);
	virtual HRESULT OnOSDEnablePTZOverlay(WPARAM wParam, LPARAM lParam);
	virtual HRESULT OnOSDSetDomeInfoOverlay(WPARAM wParam, LPARAM lParam);
	virtual HRESULT OnOSDSetDevID(WPARAM wParam, LPARAM lParam);
	HRESULT UpdatePTZ();
	static PVOID UpdatePTZProxy(PVOID pvArg);

protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWYUVOverlayFilter, CSWBaseFilter)
    SW_DISP_METHOD(Initialize, 12)
	SW_END_DISP_MAP();
protected:
	CTTF2Bitmap* m_pTTFBitmap;
	CSWMemory* m_pMemory;
	INT       m_iOffsetMemory;
	BOOL      m_fEnable;
	BOOL      m_fEnableTime;
	CSWString m_strOverlay;
	INT       m_iFontSize;
	INT       m_iNowFontSize;
	DWORD	  m_dwChangeFontSizeLast;	//上一次更改字体大小时间
	INT       m_iX;
	INT       m_iY;
	INT       m_iYColor;
	INT       m_iUColor;
	INT       m_iVColor;
    INT       m_iRColor;
    INT       m_iGColor;
    INT       m_iBColor;
	CSWString m_strDeviceID;	//用户自定义设备编号

	BOOL      m_fOverlayPTZ;
	BOOL      m_fOverlayDomeInfo;
	CHAR      m_szDomeInfo[256];
	CTTF2Bitmap* m_pTTFBitmapPTZ;
	INT       m_iFontSizePTZ;
	CSWMemory* m_pMemoryPTZ;
	INT       m_iOffsetMemoryPTZ;
	INT       m_iDomeYColor;
	INT       m_iDomeUColor;
	INT       m_iDomeVColor;
	CSWThread m_cPTZUpdateThread;
	DWORD     m_dwPanCoord;
	DWORD     m_dwTiltCoord;
	DWORD     m_dwZoomValue;
	BOOL      m_fPTZUpdated;
    INT m_iSpeedLimit;
	
private:
	BOOL      m_fRuleString;
};
#endif
