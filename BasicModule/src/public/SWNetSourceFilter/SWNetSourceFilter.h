/**
* @file SWNetSourceFilter.h 
* @brief 网络采集Filter
* @copyright Signalway All Rights Reserved
* @author zhaopy
* @date 2013-04-02
* @version 1.0
*/

#ifndef _SWNET_SOURCEFILTER_H_
#define _SWNET_SOURCEFILTER_H_

#include "SWBaseFilter.h"
#include "SWFilterStruct.h"

class CSWNetSourceFilter : public CSWBaseFilter
{
	CLASSINFO(CSWNetSourceFilter, CSWBaseFilter)
public:
	CSWNetSourceFilter();
	virtual ~CSWNetSourceFilter();


	virtual HRESULT Run();

public:
	/**
   *@brief 初始化
   */
	HRESULT Initialize(PVOID pvParam);
	/**
   *@brief 设置帧率
   */
	HRESULT SetFps(INT iFps);

	static VOID OnProcessProxy(PVOID pvParam);
protected:	
	/**
   *@brief 采集主线程
   */
	HRESULT OnProcess();
		
	VOID Clear();

protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWNetSourceFilter, CSWBaseFilter)
		SW_DISP_METHOD(SetFps, 1)
		SW_DISP_METHOD(Initialize, 1)
	SW_END_DISP_MAP()
private:
	static const DWORD MAX_IMAGE_COUNT = 3;		// VPIF API最少要初始要多少个内存块

	int m_iFps;	// 帧率： -1 ：不改变帧率。

	int m_iWidth; // 图片宽
	int m_iHeight;// 图片高
	CSWMemoryFactory* m_pMemoryFactory; 

	CSWThread* m_pThread; // 采集线程
	CSWList<CSWImage*> m_lstImage; // 图片队列
	BOOL m_fInitialized;

	CAMERA_INFO m_cCameraInfo;
	CSWTCPSocket m_cTcpSocket;
};
REGISTER_CLASS(CSWNetSourceFilter)

#endif

