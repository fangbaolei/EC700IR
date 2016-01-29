// 该文件编码格式必须为WINDOWS-936

//#pragma once

#ifndef _CAR_FACE_CTRL_H_
#define _CAR_FACE_CTRL_H_

#include "HvDeviceUtils.h"
#include "CarFaceRecogApi.h"
#include "hvthreadex.h"

#ifdef WIN32
#include "HvDevice.h"
#include "swwinerror.h"
#include <atltime.h>
#include <atlstr.h>
#else
#include "HvDeviceEx.h"
#include "swerror.h"
#endif

#define MAX_CF_DAT_SIZE (20 * 1024 * 1024)
#define MAX_CF_INF_SIZE (500 * 1024)
#define MAX_CF_INF_COUNT (20 * 1024)

// 车脸ID、信息对应表。
typedef struct tag_CarFaceInfo
{
	char szInfo[128];
	char szCarType[32];	// 车辆类型：大中小
	DWORD dwId;
}
CAR_FACE_INFO;

enum CFM_TYPE
{
	CFM_UNKNOWN = -1,
	CFM_BIG = 0,
	CFM_NORM = 1
};

typedef struct tag_MODEL_DAT_HEAD
{
	int nSize;
	int nType;
	int fExt;
	int nDatLen;
	char szComment[256];

	tag_MODEL_DAT_HEAD()
	{
		nSize = sizeof(tag_MODEL_DAT_HEAD);
		nType = -1;
		fExt = 0;
		nDatLen = 0;
		szComment[0] = 0;
	}
}MODEL_DAT_HEAD;

class CCarFaceCtrl
{
public:
	CCarFaceCtrl(void);
	~CCarFaceCtrl(void);

	HRESULT InitCtrl();
	HRESULT RecogCarFace(sv::SV_IMAGE* pImgSrc, sv::SV_BOOL fIsNight,sv::SV_BOOL fNoPlate,sv::SV_RECT* pPlatePos,sv::SV_BOOL fYellowPlate, sv::SV_RECT* pCarFacePos, int nIDCount,sv::SV_UINT32* rgRegID);
	HRESULT MatchCarFace(BYTE *pbFeature, int nFeatureLen, char *pszCarFace, char *pszCarTpye);
	HRESULT FindCarFaceString(DWORD dwCarFaceID, char *pszCarFaceName, char *pszCarTpye);


private:
	HRESULT LoadCarFaceRecogModel();
	HRESULT LoadCarFaceInfo();
	

private:
	BOOL m_fIsInit;
	BYTE *m_pbModule;
	char *m_pszInfo;
	DWORD m_dwModuleSize;
	DWORD m_dwInfoSize;
	CarFaceRecogApi::ICarFaceRecogCtrl* m_pCarFaceCtrl;
	CAR_FACE_INFO *m_pCarFaceInfo;
	int m_nCarFaceInfoCount;
	HiVideo::ISemaphore *m_pLock;
};

#endif
