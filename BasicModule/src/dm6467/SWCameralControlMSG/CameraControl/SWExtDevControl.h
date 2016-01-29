#ifndef CSWEXTDEVCONTROL_H
#define CSWEXTDEVCONTROL_H
#include "SWCameraControlParameter.h"

class CSWExtDevControl
{
public:
    CSWExtDevControl();
    virtual ~CSWExtDevControl();
    // 初始化外部设备参数
    void InitialExtDevParam( LPCAMERAPARAM_INFO pCameraParam );
    // 切换滤光片
    HRESULT FilterSwitch( DWORD dwFilterType );
	// 设置红灯加红区域
	HRESULT SetRedLightRect(SW_RECT* pRect, INT iCount);
	// 璁剧疆鐢电綉鍚屾妯″紡
	HRESULT SetExternSync(INT iMode, INT iRelayUs);
	// 设置触发抓拍沿
	HRESULT SetCaptureEdge(int nValue);
	// 获取触发抓拍沿
	HRESULT GetCaptureEdge(int *pnValue);

	HRESULT GetBarrierState(int *pnValue);
protected:
private:
	// 硬件分车道闪
	int m_iFlashDifferentLaneExt;
};

#endif // CSWEXTDEVCONTROL_H
