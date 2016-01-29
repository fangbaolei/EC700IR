#include "SWFC.h"
#include "SWJVDCoilDevice.h"
#include "SWMessage.h"

CSWJVDCoilDevice::CSWJVDCoilDevice(DEVICEPARAM *pParam):CSWBaseDevice(pParam)
{
}

CSWJVDCoilDevice::~CSWJVDCoilDevice()
{
}

HRESULT CSWJVDCoilDevice::Run(VOID)
{
	union DWORD_BYTE_UNION
	{
		struct
		{
			BYTE a;
			BYTE b;
			BYTE c;
			BYTE d;
		}B;
		DWORD W;
	};
	DWORD dwTotalChannel = 0;
	DWORD_BYTE_UNION unData;
	INT iCoilTime[6] = {0, 0, 0, 0, 0, 0};
	INT iRealTime[6] = {0, 0, 0, 0, 0, 0};
	INT iFailTimes = 0;
	while(S_OK == IsValid())
	{
		if(S_OK == Read(&unData, 4))
		{
			iFailTimes = 0;
			SW_TRACE_DEBUG("coil data:0x%08x=0x%02x,0x%02x,0x%02x,0x%02x", unData.W, unData.B.a, unData.B.b, unData.B.c, unData.B.d);
			//所有的时间保留不能超过2秒
			DWORD dwNow = CSWDateTime::GetSystemTick();
			for(int i = 0; i < 4; i++)
			{
				if(iRealTime[i] > 0 && dwNow - iRealTime[i] > 2000)
				{
					iCoilTime[i] = 0;
					iRealTime[i] = 0;
				}
			}
			//发送线圈的状态
			CSWMessage::SendMessage(MSG_APP_COIL_STATUS, (WPARAM)(unData.B.d & 0x0F), m_pDevParam->cCoilParam.bTrigger);
			//心跳包
			if(unData.B.a == 0xe2 || unData.B.a == 0xe4 || unData.B.a == 0xe6)
			{	//记录当前的通道总数
				dwTotalChannel = (unData.B.a & 0x0F);
				continue;
			}
			//正常数据
			if((unData.B.a & 0x01) && !m_pDevParam->cCoilParam.iCoilEgdeType || !(unData.B.a & 0x01) && m_pDevParam->cCoilParam.iCoilEgdeType)
			{
				//线圈号
				int index = ((unData.B.a & 0xF0) >> 4) - 1;
				//异常数据				
				if(index >= 6 || (unData.B.d & (1 << index)))
				{
					continue;
				}
				//判断是否要触发抓拍
				if(m_pDevParam->cCoilParam.bTrigger & (1 << index))
				{
					CSWMessage::SendMessage(MSG_APP_COIL_TRIGGER, index);
				}
				//时间
				iCoilTime[index] = ((DWORD(unData.B.b)) << 8)|unData.B.c;
				iRealTime[index] = CSWDateTime::GetSystemTick();

				for(int i = 0; i < 3; i++)
				{
					if(iRealTime[2*i] > 0 && iRealTime[2*i + 1] > 0 && swpa_abs(iRealTime[2*i + 1] - iRealTime[2*i]) > 65536)
					{				
						iRealTime[2*i] = 0;
						iRealTime[2*i + 1] = 0;
						iCoilTime[2*i] = 0;
						iCoilTime[2*i + 1] = 0;
					}
				}
				for(int i = 0; i < 3; i++)
				{
					if(iCoilTime[2*i] > 0 && iCoilTime[2*i + 1] > 0)
					{
						INT iSpeed = CalcSpeed(iCoilTime[2*i], iCoilTime[2*i + 1], m_pDevParam->cCoilParam.iTowCoilInstance);
						if(iSpeed > 0)
						{
							DWORD rgParam[2] = {iSpeed, m_pDevParam->iRoadNo + i};
							m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), (DWORD *)rgParam);
						}						
						iRealTime[2*i] = 0;
						iRealTime[2*i + 1] = 0;
						iCoilTime[2*i] = 0;
						iCoilTime[2*i + 1] = 0;
					}
				}
			}
		}
		//发送线圈的状态
		else if(iFailTimes++ > 10)
		{			
			iFailTimes = 0;
			CSWMessage::SendMessage(MSG_APP_COIL_STATUS, 0x0F, m_pDevParam->cCoilParam.bTrigger);
		}
	}
}

DWORD CSWJVDCoilDevice::CalcSpeed(INT iTime1, INT iTime2, DWORD dwInstance)
{
	SW_TRACE_DEBUG("time1:%d,time2:%d,instance:%d", iTime1, iTime2, dwInstance);
#define UINT16_MAX 	65536
	INT iInterval = (UINT16_MAX + iTime2 - iTime1)%UINT16_MAX;
	INT iSpeed = INT(360.0f*dwInstance / iInterval + 0.5);
	SW_TRACE_DEBUG("coil speed:%d", iSpeed);
	return iSpeed;
}

