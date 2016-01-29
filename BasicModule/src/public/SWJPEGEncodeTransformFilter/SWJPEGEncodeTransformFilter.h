#ifndef __SW_JPEG_ENCODE_TRANSFORM_FILTER_H__
#define __SW_JPEG_ENCODE_TRANSFORM_FILTER_H__
#include "SWBaseFilter.h"
#include "SWFilterStruct.h"
#include "SWMessage.h"


class CSWJPEGEncodeTransformFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWJPEGEncodeTransformFilter, CSWBaseFilter)
public:
	CSWJPEGEncodeTransformFilter();
	virtual ~CSWJPEGEncodeTransformFilter();
	HRESULT Initialize(INT iJPEGType
		, INT iCompressType
		, INT iValue
		, INT iMinQuantity
		, INT iMaxQuantity
		, INT iQuantityCapture
		, PVOID pvInfo = NULL
	);
protected:
	virtual HRESULT Receive(CSWObject* obj);	
	HRESULT OnJpegEncode(WPARAM wParam, LPARAM lParam);
	HRESULT OnJpegSetCompressRate(WPARAM wParam, LPARAM lParam);
	HRESULT OnJpegGetCompressRate(WPARAM wParam, LPARAM lParam);
	HRESULT OnJpegSetCompressRateCapture(WPARAM wParam, LPARAM lParam);
	HRESULT OnJpegGetCompressRateCapture(WPARAM wParam, LPARAM lParam);

	HRESULT OnSetAutoJPEGCompressEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetAutoJPEGCompressEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetAutoJPEGCompressParam(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetAutoJPEGCompressParam(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetJPEGCompressRect(WPARAM wParam, LPARAM lParam);
protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWJPEGEncodeTransformFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 7)
	SW_END_DISP_MAP()
	//消息映射宏
	SW_BEGIN_MESSAGE_MAP(CSWJPEGEncodeTransformFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_JPEGENCODE, OnJpegEncode)
		SW_MESSAGE_HANDLER(MSG_JPEGENCODE_COMPRESSRATE, OnJpegSetCompressRate)
		SW_MESSAGE_HANDLER(MSG_JPEGENCODE_GET_COMPRESSRATE, OnJpegGetCompressRate)
		SW_MESSAGE_HANDLER(MSG_JPEGENCODE_COMPRESSRATE_CAPTURE, OnJpegSetCompressRateCapture)
		SW_MESSAGE_HANDLER(MSG_JPEGENCODE_GET_COMPRESSRATE_CAPTURE, OnJpegGetCompressRateCapture)

		SW_MESSAGE_HANDLER(MSG_SET_AUTO_JPEG_COMPRESS_ENABLE, OnSetAutoJPEGCompressEnable)
    	SW_MESSAGE_HANDLER(MSG_GET_AUTO_JPEG_COMPRESS_ENABLE, OnGetAutoJPEGCompressEnable)
    	SW_MESSAGE_HANDLER(MSG_SET_AUTO_JPEG_COMPRESS_PARAM,  OnSetAutoJPEGCompressParam)
    	SW_MESSAGE_HANDLER(MSG_GET_AUTO_JPEG_COMPRESS_PARAM,  OnGetAutoJPEGCompressParam)
    	SW_MESSAGE_HANDLER(MSG_SET_JPEG_COMPRESS_RECT,  OnSetJPEGCompressRect)
	SW_END_MESSAGE_MAP()
private:
	const INT ENCODE_FAIL_MAX_COUNT;
	INT m_iJPEGType;
	INT m_iCompressType;
	INT m_iMinQuantity;
	INT m_iMaxQuantity;
	INT m_iQuantity;
	INT m_iQuantityCapture;
	INT m_iSize;
	INT m_iMinSize;
	INT m_iMaxSize;
	INT m_iEncFailCounter;	  			//jpeg压缩失败计数器
	
	CTTF2Bitmap m_cTTFBitmap;
	JPEG_OVERLAY_INFO* m_pJPEGOverlayInfo;
	INT m_iYColor;
	INT m_iUColor;
	INT m_iVColor;
	CSWMemory *m_pMemory;
	INT m_iPlateOverlayOffset;
	CSWMutex m_cJpegMutex;

	SW_RECT m_cRect;
};
REGISTER_CLASS(CSWJPEGEncodeTransformFilter)
#endif

