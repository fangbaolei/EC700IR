#ifndef __RADAR_BASE_H__
#define __RADAR_BASE_H__
#include "hvutils.h"
#include "hvthreadbase.h"
#include "SerialBase.h"
/**
说明：一体机直接连接雷达的基类，其他雷达类型继承此类即可。
*/
class CRadarBase : public CSerialBase, public CHvThreadBase
{
public:
    CRadarBase(){}
    virtual ~CRadarBase(){}
    virtual bool Initialize(void) = 0;
protected:
    virtual const char* GetName() = 0;
};
#endif
