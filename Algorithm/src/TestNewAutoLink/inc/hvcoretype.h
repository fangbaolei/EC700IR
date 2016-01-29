#ifndef _HV_CORE_TYPE_
#define _HV_CORE_TYPE_

#include "swBaseType.h"
#include "swObjBase.h"
#include "swWinError.h"

#include "Guiddef.h"

//类型定义

//命令ID
//0x0000001~0x0000EFFF : 公开命令ID
//0x0000F000~0x0000FFFF : 内部命令ID
//前16字节保留
typedef enum _CMD_ID
{
	CMD_RUN = 0x00000001,
	CMD_STOP,
	CMD_FORCERESULT,
	CMD_REINIT,
	CMD_MAX_ID,
	CMD_NOTUSED = 0xFFFFFFFF
} CMD_ID;

//信息ID
typedef enum _INFO_ID
{
	INFO_WORKSTATE = 0x00000001,
	INFO_SCENARIO ,
	INFO_CHARSET,
	INFO_VIDEODET_PARAM,
	INFO_PRECALIBRATE_PARAM,
	INFO_TRACKCOUNT,
	INFO_VOTE,
	INFO_DETECTAREA,
	INFO_PLATE_POS,
	INFO_SET_REC_MODE,
	INFO_GET_LIGHT_TYPE,
	INFO_MAX_ID,
	INFO_NOTUSED = 0xFFFFFFFF
} INFO_ID;

//测试模块ID
typedef enum _MODULE_ID
{
	MODULE_ALL = 0x00000001,
	MODULE_DET,
	MODULE_RECTIFY,
	MODULE_SEGMENT,
	MODULE_RECOG,
	MODULE_PROCESS_IMG,
	MODULE_COLOR,
	MODULE_MAX_ID,
	MODULE_NOTUSED = 0xFFFFFFFF
} MODULE_ID;

//模型ID
typedef enum _MODEL_TYPE
{
	MODEL_TYPE_DET = 0x00000001,
	MODEL_TYPE_RECOG,
	MODEL_TYPE_MAX_ID,
	MODEL_TYPE_NOTUSED = 0xFFFFFFFF
} MODEL_TYPE;

//参数辅助类
typedef struct _HVINFO
{
	virtual void* GetDat() = 0;
	virtual INT GetLen() = 0;
	virtual DWORD32 GetFlag() = 0;

	virtual HRESULT SetDat(const void* pDat) = 0;
	virtual HRESULT SetLen(INT iLen) = 0;
	virtual HRESULT SetFlag(DWORD32 dwFlag) = 0;
}
HVINFO;

template<typename T>
class CHvInfo : public HVINFO
{
public:
	virtual void* GetDat() { return (void*)&dat; }
	virtual	INT GetLen() { return iLen; }
	virtual DWORD32 GetFlag() { return dwFlag; }

	virtual HRESULT SetDat(const void* pDat) 
	{
		if ( pDat == NULL) return E_POINTER;
		dat = *(T*)pDat; 
		return S_OK;
	}
	virtual HRESULT SetLen(INT iNewLen) { iLen = iNewLen; return S_OK; }
	virtual HRESULT SetFlag(DWORD32 dwNewFlag) { dwFlag = dwNewFlag;  return S_OK; } 

	//构造函数
	CHvInfo()
		:iLen(sizeof(T))
		,dwFlag(0)
	{
	}

	CHvInfo(const T& NewDat, INT iNewLen = sizeof(T), DWORD32 dwNewFlag = 0)
		:dat(NewDat)
		,iLen(iNewLen)
		,dwFlag(dwNewFlag)
	{
	}

	CHvInfo& operator = (const T& NewDat)
	{
		dat = NewDat;
		return *this;
	}

	//数据
	T dat;
	INT iLen;
	DWORD32 dwFlag;
};

#endif //_HV_CORE_TYPE_
