#include "SWFC.h"
#include "SWLogDevice.h"

CSWList<CSWLogDevice *>CSWLogDevice::m_lstLog;
CSWTelnet CSWLogDevice::m_cTelnet;
CSWTelnet CSWLogDevice::m_cDspTelnet;

CSWLogDevice::CSWLogDevice()
{
    m_lstLog.AddTail(this);
}

CSWLogDevice::~CSWLogDevice()
{
}

VOID CSWLogDevice::Log(LPCSTR szLog)
{    
    //网络日志
    if(FAILED(m_cTelnet.IsValid()))
    {
        m_cTelnet.Create();
    }
    
    //DSP专用网络日志
    if(FAILED(m_cDspTelnet.IsValid()))
    {
        m_cDspTelnet.Create(2324);
    }  
    
    //解析字符串
    INT iLevel = -1;
    int index = 0;
    for(LPCSTR ch = szLog; *ch != '\0'; ch++)
    {
        if(*ch == '|')
        {
            index++;
        }
        else if(index == 1)
        {
            if(*ch >= '0' && *ch <= '9')
            {
                if(iLevel < 0)
                {
                    iLevel = 0;
                }
                iLevel = 10 * iLevel + (*ch - '0');
            }
        }
        else if(index == 2)
        {
            szLog = ch + 1;
            break;
        }
    }

    if (iLevel == SW_LOG_LV_DSP)
    {		
        //DSP日志输出到网络
        m_cDspTelnet.Log(2, szLog);
    }
    else
    {
        //输出到网络
        m_cTelnet.Log(2, szLog);
        
        //输出到对应的设备
        SW_POSITION pos = m_lstLog.GetHeadPosition();
        while(m_lstLog.IsValid(pos) && iLevel >= 0)
        {
            CSWLogDevice* pDev = (CSWLogDevice *)m_lstLog.GetNext(pos);
			// 当dsp日志输出时，临时中止串口输出，
			// 以免占用处理器资源过多导致dsp日志丢失
			// 实际上所有输出到串口的日志都已经通过23端口的telnet输出
			if (DEBUG == pDev->GetLevel())
			{
				static DWORD dwCount = 0;
				if (!m_cDspTelnet.IsLogging() && DEBUG >= iLevel)
				{
					pDev->ShowLog(szLog);
					dwCount = 0;
				}
				else if (dwCount++ % 1000 == 0)
				{
					printf("------  Warning: DSP log Client connected, pause serial log output!  ------\n");
					dwCount = 1;
				}
			}
            else if(pDev->GetLevel() >= iLevel)
            {
                pDev->ShowLog(szLog);
            }
        }
    }
}

