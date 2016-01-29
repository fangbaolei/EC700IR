/*
 * SWJZRadar.h
 *
 *  Created on: 2013年12月4日
 *      Author: qinjj
 */

#ifndef __SW_JZRADAR_H__
#define __SW_JZRADAR_H__
#include "SWBaseDevice.h"
/*
 *@brief 四川九洲雷达测速
**/
class CSWJZRadar : public CSWBaseDevice
{
	CLASSINFO(CSWJZRadar, CSWBaseDevice)
public:
	CSWJZRadar(DEVICEPARAM *pParam);
	virtual ~CSWJZRadar();
	virtual HRESULT Run(VOID);
//private:
	//HRESULT JZRadarInit(VOID);
};

#endif /* __SW_JZRADAR_H__ */
