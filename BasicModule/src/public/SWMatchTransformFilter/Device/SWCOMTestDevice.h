/*
 * SWCOMTestDevice.h
 *
 *  Created on: 2013年12月4日
 *      Author: qinjj
 */

#ifndef __SW_COMTESTDEVICE_H__
#define __SW_COMTESTDEVICE_H__
#include "SWBaseDevice.h"
/*
 *@brief 串口测试模块
**/
class CSWCOMTestDevice : public CSWBaseDevice
{
	CLASSINFO(CSWCOMTestDevice, CSWBaseDevice)
public:
	CSWCOMTestDevice(DEVICEPARAM *pParam);
	virtual ~CSWCOMTestDevice();
	virtual HRESULT Run(VOID);
};

#endif /* __SW_COMTESTDEVICE_H__ */
