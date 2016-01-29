#ifndef __SW_LOG_DEVICE_H__
#define __SW_LOG_DEVICE_H__

#include "SWTelnet.h"

#ifndef REGISTER_LOG_DEVICE
#define REGISTER_LOG_DEVICE(cls) cls cls##Instance; void cls##Load(){cls *pTmp = &cls##Instance; pTmp->Print();}
#define LOAD_LOG_DEVICE(cls) extern void cls##Load();cls##Load();
#endif

class CSWLogDevice : public CSWObject
{
	CLASSINFO(CSWLogDevice, CSWObject)
	enum{ERROR, NORMAL, OPERATOR, DEBUG};
public:
	CSWLogDevice();
	virtual ~CSWLogDevice();
	virtual void Print(void)
	{
		if(!m_lstLog.Find(this))
		{
			m_lstLog.AddTail(this);			
		}
		printf("log device name:%s, level:%d\n", Name(), GetLevel());
	}
	static HRESULT Initialize(const INT nPort = 23)
	{
		LOAD_LOG_DEVICE(CSWDebugLogDevice)
		LOAD_LOG_DEVICE(CSWEEPROMLogDevice)
		LOAD_LOG_DEVICE(CSWNormalLogDevice)
		LOAD_LOG_DEVICE(CSWOperatorLogDevice)
				
		return m_cTelnet.Create(nPort);
	}
	static VOID Clear(VOID)
	{
		m_cTelnet.Clear();
	}
	static VOID Log(LPCSTR szLog);
protected:
	/**
	 *@brief 获得日志设备的等级
	 */
	virtual DWORD GetLevel(void) = 0;
	/**
	 *@brief 写日志
	 */
	virtual VOID ShowLog(LPCSTR szLog) = 0;
private:
	static CSWList<CSWLogDevice *>m_lstLog;
	static CSWTelnet m_cTelnet;
	static CSWTelnet m_cDspTelnet;
};
#endif

