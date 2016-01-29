#ifndef _INCLUDE_SAFESAVER_H_
#define _INCLUDE_SAFESAVER_H_

#include "resultsend.h"

////////////////////////////////////////////////////////////
//安全保存接口(自维护),确保数据的完整
////////////////////////////////////////////////////////////

#ifndef E_RECORD_NONE 
#define E_RECORD_NONE 0x80008901
#endif
#ifndef  E_RECORD_WRITEING
#define E_RECORD_WRITEING 0x80008902
#endif

const DWORD32 MAX_BIG_IMG_COUNT = 5;

class CSafeSaverData
{
public:
	HVIO_BigImage* rgpBigimg[MAX_BIG_IMG_COUNT];
	HVIO_SmallImage* pSmallImg;
	HVIO_SmallImage* pBinaryImg;
	HVIO_AppendInfo* pAppendInfo;
	HVIO_CarPlate* pPlateNoInfo;

	static const int MAX_PLATENO_BUFF = 1024;
	CSafeSaverData()
		: pSmallImg(NULL)
		, pBinaryImg(NULL)
		, pAppendInfo(NULL)
		, pPlateNoInfo(NULL)
	{
		for(int i = 0; i < MAX_BIG_IMG_COUNT; ++i)
		{
			rgpBigimg[i] = new HVIO_BigImage;
			memset( rgpBigimg[i], 0, sizeof(HVIO_BigImage));
		}
		pSmallImg = new HVIO_SmallImage;
		memset( pSmallImg, 0, sizeof(HVIO_SmallImage));
		pBinaryImg = new HVIO_SmallImage;
		memset( pBinaryImg, 0, sizeof(HVIO_SmallImage));
		pAppendInfo = new HVIO_AppendInfo;
		memset( pAppendInfo, 0, sizeof(HVIO_AppendInfo) );
		pPlateNoInfo = new HVIO_CarPlate;
		memset( pPlateNoInfo, 0, sizeof(HVIO_CarPlate) );
	}

	~CSafeSaverData()
	{
		for(int i = 0; i < MAX_BIG_IMG_COUNT; ++i)
		{
			SAFE_DELETE(rgpBigimg[i]);
		}
		SAFE_DELETE(pSmallImg);
		SAFE_DELETE(pBinaryImg);
		SAFE_DELETE(pPlateNoInfo);
		SAFE_DELETE(pAppendInfo);
	}

	CSafeSaverData &operator =(const CSafeSaverData &data)
	{
		for(int i = 0; i < MAX_BIG_IMG_COUNT; ++i)
		{
			if( data.rgpBigimg[i]->m_dwImageSize > 0 )
			{
				HV_memcpy(rgpBigimg[i], data.rgpBigimg[i], sizeof(HVIO_BigImage));
			}
			else 
			{
				rgpBigimg[i]->m_dwImageSize = 0;
			}
		}
		if( data.pSmallImg->m_dwImageSize > 0 )
		{
			HV_memcpy(pSmallImg, data.pSmallImg, sizeof(HVIO_SmallImage));
		}
		else 
		{
			pSmallImg->m_dwImageSize = 0;
		}
		if( data.pBinaryImg->m_dwImageSize > 0 )
		{
			HV_memcpy(pBinaryImg, data.pBinaryImg, sizeof(HVIO_SmallImage));
		}
		else
		{
			pBinaryImg->m_dwImageSize = 0;
		}

		HV_memcpy(pAppendInfo, data.pAppendInfo, sizeof(HVIO_AppendInfo));
		HV_memcpy(pPlateNoInfo, data.pPlateNoInfo, sizeof(HVIO_CarPlate));

		return *this;
	}
};

class CDetectData
{
public:
	HVIO_String* pString;
	CDetectData()
	{
		pString = new HVIO_String;
		memset(pString, 0, sizeof(HVIO_String));
	}
	~CDetectData()
	{
		SAFE_DELETE(pString);
	}
};

class ISafeSaver
{
public:
	virtual HRESULT Init(LPCSTR lpszIniFile) = 0;
	virtual HRESULT SaveData(CSafeSaverData* pData) = 0;
	virtual HRESULT SaveVideo(DWORD32 dwTimeLow, DWORD32 dwTimeHigh, unsigned char* pbImage, DWORD32 dwSize, int iEddy) = 0;
	virtual HRESULT GetPlateRecord(DWORD32 dwTimeLow, DWORD32 dwTimeHigh, int& index,  int iType, CSafeSaverData* pData) = 0;
	virtual HRESULT GetVideoRecord(DWORD32 dwTimeLow, DWORD32 dwTimeHigh, unsigned char** ppbVideo, int* pcbVideoSize) = 0;
	virtual HRESULT GetDiskStatus() = 0;
	virtual HRESULT GetLastCamStatus(DWORD32* pdwStatus) = 0;
	virtual HRESULT SaveDetectData(CDetectData *pData) = 0;
	virtual HRESULT GetDetectData(DWORD32& dwTimeLow, DWORD32& dwTimeHigh, CDetectData *pData) = 0;
	virtual bool ThreadIsOk() = 0;
};

extern HRESULT GetSafeSaver( ISafeSaver** ppSafeSaver );

#endif

