#ifndef __CSR_I_RADAR_H__
#define __CSR_I_RADAR_H__
#include "RadarBase.h"
/**
说明:川速雷达协议，该雷达需要用其上位机程序配置速度的输出方式为单字节的方式
     有速度输出速度值，无则输出0或者不输出。该类只负责从串口接收到速度值
*/
class CCSRIRadar : public CRadarBase
{
public:
	CCSRIRadar();
	virtual ~CCSRIRadar();
	virtual bool Initialize(void);
protected:
	virtual const char* GetName()
	{
		return "CCSRIRadar";
	}
	virtual HRESULT Run(void* pvParam);
};
#endif
