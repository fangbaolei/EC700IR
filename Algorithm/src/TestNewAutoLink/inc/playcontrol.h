#pragma once
#include "hvvartype.h"
#include "tracker.h"

typedef HRESULT (*CFrameSinkCallback)	(
	int iVideoID,
	IReferenceComponentImage *pRefImage,
	LPVOID pParam
	);

typedef struct _PLAYCONTROL_PARAM 
{
	BOOL fEnableTrigger;
	BOOL fEnableCircle;
	BOOL fEnableSave;
	BOOL fUseStorageCollect;
	char szSavePath[256];
	BOOL fTermOfHour;
	BOOL fEnableAviCompress;
	int nAviMinutes;
	int iAviFrameRate;


	_PLAYCONTROL_PARAM()
		:fEnableTrigger(FALSE)
		,fEnableCircle(FALSE)
		,fEnableSave(FALSE)
		,fUseStorageCollect(TRUE)
		,fTermOfHour(FALSE)
		,iAviFrameRate(3)
	{
		szSavePath[0] = 0;
	}
} PLAYCONTROL_PARAM;

//录像编辑接口
class ISequenceEditor
{
public:
	virtual int DeleteSpaceFrame(const CHvString &strBegin, const CHvString &strEnd) = 0;
	virtual BOOL GetFrameScope(const CHvString &strCurrentFrame, CHvString &strBeginFrame, CHvString &strEndFrame) = 0;
};

//视频播放控制
class IPlayControl
{
public:
	enum { State_Ready, State_Running, State_Stopped, State_Paused };
	virtual ~IPlayControl() {};
	STDMETHOD(Play)() = 0;
	STDMETHOD(Stop)() = 0;
	STDMETHOD(Pause)() = 0;

	STDMETHOD(Next)() = 0;
	STDMETHOD(Prev)() = 0;
	STDMETHOD(Seek)(long long rt) = 0;

	STDMETHOD(SetCallback)(
		CFrameSinkCallback pfnCallback, 
		LPVOID pParam=NULL
	) = 0;
	STDMETHOD(GetStatus)(int &iStatus) = 0;
	STDMETHOD(SetParam)(const PLAYCONTROL_PARAM &param	) = 0;
	STDMETHOD(GetParam)(PLAYCONTROL_PARAM &param) = 0;
	STDMETHOD(GetSequenceEditor)(ISequenceEditor **ppSequenceEditor) = 0;
	STDMETHOD(ShowSetup)() = 0;
	STDMETHOD(SetCamParameter)(LIGHT_TYPE nLightType) = 0;
	STDMETHOD(GetCameraStatus)(int &iStatus) = 0;
	virtual bool ThreadIsOk() = 0;
};
