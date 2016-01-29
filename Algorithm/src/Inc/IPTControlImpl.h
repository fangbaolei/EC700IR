#ifndef _INCLUDE_IPTCONTROLIMPL_H_
#define _INCLUDE_IPTCONTROLIMPL_H_

#include "hvutils.h"
#include "SerialBase.h"
#include "IPTInterface.h"

// ---------------- CIPTControlImpl ------------
class CIPTControlImpl : public IIPTControl
{
public:
	CIPTControlImpl(IComm*);
	~CIPTControlImpl(void);

public:
	//IIPTControl
	virtual HRESULT SendCommand( const BYTE8 bCmdId, PBYTE8 pbData, DWORD32 dwSize );
	virtual HRESULT RecvData( PBYTE8 pbCmdId, PBYTE8 pbData, PDWORD32 pdwLen, DWORD32 dwRelayMs );

    HRESULT CheckData( const DWORD32 dwRecvLen );
	HRESULT Lock();
	void UnLock();

private:
    IComm* m_pIComm;

	static const int MAX_PACK_LEN	= 16*4*8 + 128;   //最大数据包长度
	static const int FALG_NUMBER	= 0x00; //序号
	static const int FALG_BEGIN		= 0xFF; //开始标志
	static const int FALG_END		= 0xFE; //结束标志
	static const int FALG_EXCHANGE	= 0xFD; //转义标志
	static const int FALG_NONE		= 0xF0; //无事件标志

	BYTE8 m_rgbRecvData[MAX_PACK_LEN];

	BYTE8 m_rgbData[MAX_PACK_LEN];
	DWORD32 m_dwDataLen;
	DWORD32 m_dwCmdId;
};


//==------------------------------   命令 ----------------------------------------------==//

//复位
HRESULT IPT_Reset( IIPTControl* pControl );
//读复位计数器
HRESULT IPT_ResetCount( PDWORD32 pdwCount, IIPTControl* pControl );
//清零复位计数器
HRESULT IPT_ClearResetCount( IIPTControl* pControl );
//设置时间
HRESULT IPT_SetTime( DWORD32 dwTime, IIPTControl* pControl );
//读时间
HRESULT IPT_ReadTime( PDWORD32 pdwTime, IIPTControl* pControl );
//恢复默认设置
HRESULT IPT_ResetDefault( IIPTControl* pControl );
//写透明串口命令
HRESULT IPT_WriteComCmd( BYTE8 bDB, PBYTE8 pbData, DWORD32 dwLen, IIPTControl* pControl );
//读透明串口命令
HRESULT IPT_ReadComCmd(  BYTE8 bDB, PBYTE8 pbData, DWORD32 dwLen, PDWORD32 pcbRead, IIPTControl* pControl );
//配置透明串口参数
HRESULT IPT_SetComParam( BYTE8 bDB, DWORD32 dwBT, BYTE8 bCheck, DWORD32 dwBit, IIPTControl* pControl );
//读透明串口配置参数
HRESULT IPT_ReadComParam(BYTE8 bDB, PDWORD32 pdwBT, PBYTE8 pbCheck, PDWORD32 pdwBit, IIPTControl* pControl );
//配置外部设备参数
HRESULT ITP_SetDeviceParam( BYTE8 bDB, BYTE8 bType, PBYTE8 pbData, DWORD32 dwLen, IIPTControl* pControl );
//读外部设备参数
HRESULT IPT_ReadDeviceParam( BYTE8 bDB, PBYTE8 pbType, PBYTE8 pbData, DWORD32 dwLen, PDWORD32 pcbRead, IIPTControl* pControl );
//读外部设备事件
HRESULT IPT_ReadDeviceEvent( PBYTE8 pbEvent, PBYTE8 pbType, PBYTE8 pbEventDeviceType,DWORD32* pdwTime, PBYTE8 pbData, DWORD32 dwLen, PDWORD32 pcbRead, IIPTControl* pControl );
//读I/O输入口电平
HRESULT IPT_ReadIOInputLevel( BYTE8 bDB, PBYTE8 pbFlag, IIPTControl* pControl );
//从I/O输出口输出一个脉冲
HRESULT IPT_OutputPulse( BYTE8 bDB, BYTE8 bLevel, WORD16 wWidth, IIPTControl* pControl );
//设置IO输出口延时
HRESULT IPT_SetIOOutDelay( BYTE8 bDB, int pdelay, IIPTControl* pControl );
//读IO输出口延时
HRESULT IPT_ReadIOOutDelay( BYTE8 bDB, PBYTE8 pdelay, IIPTControl* pControl );
//设置IO输出口的初始化电平
HRESULT IPT_SetIOOutInitLevel( BYTE8 bDB, BYTE8 initlevel, IIPTControl* pControl );
//读IO输出口的初始化电平
HRESULT IPT_ReadIOOutInitLevel( PBYTE8 initlevel, IIPTControl* pControl );
//进入升级状态
HRESULT IPT_EnterUpdateState( PBYTE8 statedata, IIPTControl* pControl );
//退出升级状态
HRESULT IPT_ExitUpdateState( PBYTE8 exitdata, IIPTControl* pControl );
//上传升级数据
HRESULT IPT_UploadUpdateData( int page, PBYTE8 updatedata, IIPTControl* pControl );
//下载升级数据
HRESULT IPT_DownloadUpdateData( int page,PBYTE8 updatedata, IIPTControl* pControl );
//读转换器版本信息
HRESULT IPT_ReadSoftwareVersion( PBYTE8 info, IIPTControl* pControl );
//读设备工作参数
HRESULT IPT_ReadDeviceWorkParam(PBYTE8 pbDeviceType, PBYTE8 pbworkData, IIPTControl* pControl );
//写设备工作参数
HRESULT IPT_WriteDeviceWorkParam(PBYTE8 pbDeviceType, PBYTE8 pbworkData,int iDataSize ,IIPTControl* pControl );
//新版本读外部设备事件
HRESULT IPT_ReadDeviceEvent_Ex(
			       PBYTE8 pbInData, DWORD32 dwInLen,
			       PBYTE8 pbEvent, PBYTE8 pbType,
			       PBYTE8 pbEventDeviceType, DWORD32* pdwTime,
			       PBYTE8 pbData, DWORD32 dwLen,
			       PDWORD32 pcbRead, IIPTControl* pControl
			       );
//读所有I/O输入口电平
HRESULT IPT_ReadAllIOInputLevel( PBYTE8 pbLevel, IIPTControl* pControl );

#endif
