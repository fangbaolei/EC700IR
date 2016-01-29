// 该文件编码格式必须是WIN936
#ifndef SWCENTAURUSSOURCEFILTER_H
#define SWCENTAURUSSOURCEFILTER_H

#include "SWBaseFilter.h"
#include "SWMessage.h"


class CSWCentaurusSourceFilter : public CSWBaseFilter, public CSWMessage
{
    CLASSINFO(CSWCentaurusSourceFilter, CSWBaseFilter)
public:
    CSWCentaurusSourceFilter();
    virtual ~CSWCentaurusSourceFilter();

	static VOID OnProcessH264Proxy(PVOID pvParam);		//处理H264回调数据
	static VOID	OnProcessJPEGProxy(PVOID pvParam);		//

protected:
    virtual HRESULT Run();
    virtual HRESULT Stop();
    static int OnResult(void *pContext, int type, void *struct_ptr);

	/**
   	*@brief 处理主线程
   	*/
	HRESULT OnProcessH264();
	HRESULT OnProcessJPEG();
	
protected:

	HRESULT Initialize( INT iCVBSExport, INT iCVBSCropStartX, INT iCVBSCropStartY);

	/**
	*@brief 获取JPEG采集帧率
	*
	*/
	HRESULT OnGetJpegRawFps(WPARAM wParam,LPARAM lParam);

	/**
	*@brief 获取H264采集帧率
	*
	*/
	HRESULT OnGetH264RawFps(WPARAM wParam,LPARAM lParam);

	/**
	*@brief 获取H264第二通道采集帧率
	*
	*/
	HRESULT OnGetH264SecondRawFps(WPARAM wParam,LPARAM lParam);

	/**
	*@brief 设置并更新图片扩展信息
	*
	*/
	HRESULT UpdateExtensionInfo(CSWImage* pImage, IMAGE *pImageInfo);

	
	/**
	*@brief 图片扩展信息
	*
	*/
	static PVOID GetExtensionInfo(PVOID pvArg);
	
	 //消息映射宏定义
    SW_BEGIN_MESSAGE_MAP(CSWCentaurusSourceFilter, CSWMessage)
    	SW_MESSAGE_HANDLER(MSG_SOURCE_GET_JPEG_RAW_FPS, OnGetJpegRawFps)
    	SW_MESSAGE_HANDLER(MSG_SOURCE_GET_H264_RAW_FPS, OnGetH264RawFps)
    	SW_MESSAGE_HANDLER(MSG_SOURCE_GET_H264_SECOND_RAW_FPS, OnGetH264SecondRawFps)
    SW_END_MESSAGE_MAP()

    //自动化映射宏
	SW_BEGIN_DISP_MAP(CSWCentaurusSourceFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 3)
	SW_END_DISP_MAP()
	
public:
	static const INT MAX_IMAGE_COUNT = 1;

private:
	static FLOAT	m_fltJpegRawFps;
	static FLOAT	m_fltH264RawFps;
	static FLOAT 	m_fltH264SecondRawFps;
	static INT m_iJpegFrameCount;
	static INT m_iH264FrameCount;
	static INT m_iJpegDropCount;
	static INT m_iH264DropCount;
	static INT m_iH264SecondFrameCount;
	static INT m_iCVBSExport;
	static INT m_iCVBSCropStartX;
	static INT m_iCVBSCropStartY;

	BOOL m_fInitialized;

	CSWList<CSWImage*> m_lstH264Image;		// H264图片对列
	CSWList<CSWImage*> m_lstJPEGImage;		// JPEG

	CSWMutex m_cMutexH264Image;             // H264访问临界区
	CSWMutex m_cMutexJPEGImage; 			// JPEG访问临界区

	CSWThread* m_pThreadH264;				// H264处理线程
	CSWSemaphore m_cSemImageH264;		

	CSWThread* m_pThreadJPEG;				// JPEG处理线程
	CSWSemaphore m_cSemImageJPEG;		

	INT m_iShutter;
	INT m_iAGCGain;
	INT m_iRGBGain[3];
	CSWThread m_cThreadExtInfo;	

public:
	static INT m_iAvgY;
	static INT m_iRawAvgY;
};
REGISTER_CLASS(CSWCentaurusSourceFilter)


#endif // SWCENTAURUSSOURCEFILTER_H
