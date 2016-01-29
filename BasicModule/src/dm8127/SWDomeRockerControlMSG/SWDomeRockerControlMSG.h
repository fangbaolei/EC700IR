/**
 * @file   SWDomeRockerControlMSG.h
 * @author 
 * @date   Fri Aug 01 08:43:45 2014
 * 
 * @brief  木星平台摇杆接入模块
 *         流程：摇杆 ->(rs485)               |-> 变焦、对焦、光圈 -> 机芯
 *               -> 接入数据并解析 -> 转发命令| 
 *                                            |-> 其它命令 -> 球机
 * @note 只支持pelco-d协议
 */

#ifndef SWDOMEROCKERCONTROLMSG_H_
#define SWDOMEROCKERCONTROLMSG_H_

#include "SWFC.h"
#include "SWBaseFilter.h"
#include "SWMessage.h"


class CSWDomeRockerControlMSG: public CSWObject, CSWMessage
{
CLASSINFO(CSWDomeRockerControlMSG, CSWObject)
public:
	CSWDomeRockerControlMSG();
    virtual ~CSWDomeRockerControlMSG();

protected:
    HRESULT Run();
    HRESULT Stop();

    static VOID OnProcessProxy(PVOID pvParam);
    HRESULT OnProcess();

    // 说明：iProtocol是预留参数，目前只支持一种，故实际上没使用到
    HRESULT Initialize(BOOL fEnable, INT iBaudrate, INT iProtocol);
    HRESULT OpenSerial(const char *pDeviceName, int iBaudrate, int iDatabits, int iParity, int iStopbits);
	virtual HRESULT Close(VOID);
	virtual HRESULT Read(PVOID pBuffer, INT* iSize);

    int DispatchCommand(unsigned char* szCommand, int iCommandLen);
    void ProcessPelcoCommand(unsigned char* szCommand, int iCommandLen);
    //////////////
    //自动化映射宏
    SW_BEGIN_DISP_MAP(CSWDomeRockerControlMSG, CSWObject)
        SW_DISP_METHOD(Initialize, 3)
        SW_DISP_METHOD(Run, 0)
        SW_DISP_METHOD(Stop, 0)
    SW_END_DISP_MAP()

private:
    CSWThread m_thPro;
    int m_iNeedSleep;
    int m_iFd;
};
REGISTER_CLASS(CSWDomeRockerControlMSG)

#endif /*  */
