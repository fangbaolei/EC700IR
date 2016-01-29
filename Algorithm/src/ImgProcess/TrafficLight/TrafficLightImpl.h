#pragma once
#include ".\trafficlightinfo.h"
#include "tracker.h"

#define MAX_ERROR_COUNT 500
#define MAX_LIGHT_TYPE_COUNT 32
#define STATUS_BUF_LEN 100
#define MAX_IO_LIGHT_COUNT 8

class CTrafficLightImpl
{
public:
	CTrafficLightImpl(void);
	~CTrafficLightImpl(void);
public:
	//设置红绿灯参数
	HRESULT SetTrafficLightParam(TRAFFIC_LIGHT_PARAM tlpTrafficLightParam);
	//设置接口对外层回调设置
	HRESULT SetTrafficLightCallback(ITracker *pCallback);
	//pSceneImage原始图片,iCurrentStatus当前识别出的红绿灯的状态,iLastStatus最后稳定识别的状态,fNight当前是晚上还是白天
	HRESULT RecognizeTrafficLight(HV_COMPONENT_IMAGE* pSceneImage, int* iCurrentStatus, int* iLastStatus, bool fNight);
	//取得对应索引的场景,iPos场景索引值
	bool GetLightScene(int iPos, TRAFFICLIGHT_SCENE* pts);
	//获取红灯开始时间
	bool GetRedLightStartTime(LIGHT_REALTIME* plrRedLightTime);
	//获取绿灯时间
	bool GetGreenLightStartTime(LIGHT_TICK* pGreenTick);
	//获取指定灯组的红绿灯信息类,iIndex红绿灯灯组索引值
	CTrafficLightInfo* GetTrafficLightInfo(int iIndex);
private:
	int InitTrafficLightParam();
	HRESULT PrintLightStatus(const LIGHT_TEAM_STATUS& ltsInfo , char* pLightStatus,int iLightStatusLen);
	//通过IO取当前红绿灯状态
	DWORD32 GetIOStatus(int iTeam, BYTE8 bLevel);
	//传入各灯组的状态，返回场景状态编号
	int TransitionScene(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount, const int& iLastOkLightStatus, bool fFlag);
	//如果是灯组状态与场景是邦定的
	int GetSceneNumber(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount);
	bool GetSceneInfo(int iPos, SCENE_INFO* pts);
	void SetSceneInfo(int iPos, const SCENE_INFO& ts);
	void SetTrafficLightType(int iTeam, int iPos, _TRAFFICLIGHT_POSITION tpPos, _TRAFFICLIGHT_LIGHT_STATUS tlsStatush);
	bool GetTrafficLightType(int iTeam, int iPos, TRAFFICLIGHT_TYPE* ltLightType);	
	void SetSceneStatus(int nPos, int nStatus);
	int GetSceneStatus(int nPos);
	
	HRESULT UpdateRedLightPos();
private:
	ITracker *m_pCallback;
	TRAFFIC_LIGHT_PARAM m_tlpTrafficLightParam;
	
	CTrafficLightInfo m_rgLightInfo[MAX_TRAFFICLIGHT_COUNT];
	
	//用于回调输出
	HV_RECT m_rgLightRect[MAX_TRAFFICLIGHT_COUNT];	//灯组位置
	DWORD32 m_rgLightStatus[MAX_TRAFFICLIGHT_COUNT]; //灯组状态
	TRAFFICLIGHT_SCENE m_rgLightScene[MAX_SCENE_COUNT];
	SCENE_INFO m_rgsiInfo[MAX_SCENE_COUNT];
	int m_iLightTypeCount;
	TRAFFICLIGHT_TYPE m_rgLightType[MAX_LIGHT_TYPE_COUNT];
	
	//外接IO红绿灯信息
	TRAFFICLIGHT_TYPE m_rgIOLight[MAX_IO_LIGHT_COUNT];
	//当前场景
	int m_iLastLightStatus;
	//实时红绿灯状态
	int m_iCurLightStatus;
	//上一个有效的红绿灯状态
	int m_iLastOkLightStatus;
	//快速转换场景
	bool m_fCheckSpeed;
	//连续无效场景数
	int m_iErrorSceneCount;
	//每个方向红绿灯开始时间(左转,直行,右转,掉头）
	LIGHT_REALTIME m_redrealtime;
	//绿灯开始相对时间
	LIGHT_TICK m_greentick;
	//红灯延迟时间
	LIGHT_TICK m_ltRedRelay;
	int m_iRedLightCount;
	HiVideo::CRect m_rgRedLightRect[MAX_TRAFFICLIGHT_COUNT * 2];	//红灯灯组位置
	int m_rgiSceneStatus[STATUS_BUF_LEN];
};
