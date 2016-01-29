
#ifndef _SW_RECORD_H_
#define _SW_RECORD_H_

#include "SWCarLeft.h"

#include "SWImage.h"

#include "tinyxml.h"


typedef struct __RECORD_FILE_INFO
{
//#define MAX_FRAME_NAME_SIZE 256
	SWPA_TIME	sTimeStamp; 				 //
    DWORD dwCarID;
    DWORD dwImgType;
    DWORD dwImgWidth;
    DWORD dwImgHeight;
    //DWORD dwTimeLow;
    //DWORD dwTimeHigh;
    DWORD dwEddyType;
    SW_RECT rcPlate;
    //HV_RECT rcRedLightPos[20];
    //int nRedLightCount;
    SW_RECT rcFacePos[20];
    INT nFaceCount;

    __RECORD_FILE_INFO()
    {
        swpa_memset(this, 0, sizeof(*this));
    };

}_RECORD_FILE_INFO;



class CSWRecord : public CSWObject
{
	CLASSINFO(CSWRecord, CSWObject)
		
public :
	CSWRecord();
	
	virtual ~CSWRecord();
	
	const CHAR* GetXmlString();

	HRESULT Initialize();

	DWORD GetXmlSize();
	
	HRESULT SetXml(CSWCarLeft* pCarLeft, const DWORD dwCarID);

	HRESULT SetXml(const TiXmlDocument* pXmlDoc);
	
	HRESULT SetXml(const CHAR* pszXmlDoc);
	
	CSWImage * GetImage(const DWORD dwID);
	
	HRESULT SetImage(const DWORD dwID, CSWImage *pImage);
	
	_RECORD_FILE_INFO * GetImageInfo(const DWORD dwID);
	
	HRESULT SetImageInfo(const DWORD dwID, const _RECORD_FILE_INFO * psInfo);
	
	DWORD GetImageTypeCount() {return m_dwImageTypeCount;};

	HRESULT SetRefTime(const DWORD dwRefTime);
	
	DWORD GetRefTime();
	
	HRESULT SetCarArriveTime(const DWORD dwRefTime);

	DWORD GetCarArriveTime();

	HRESULT SetTransmittingType(const CHAR* szType);

	HRESULT SetCarID(const DWORD dwCarID);
	
	HRESULT SetPTType(const INT iPTType);
	
	INT GetPTType();
	
	BOOL IsNormal();

	HRESULT SetUnSurePeccancy(const INT iUnSurePeccancy);

	INT GetUnSurePeccancy();	

	HRESULT SetTrafficInfoFlag(const BOOL fFlag)
	{
		if (m_fInited)
		{
			m_fTrafficInfo = fFlag;
		}

		return S_OK;
	}

	BOOL GetTrafficInfoFlag()
	{
		return (m_fInited ? m_fTrafficInfo : FALSE);
	}

	
	BOOL IsValid()
	{
		return (m_fInited);
	}

	HRESULT SetVideo(const DWORD& dwID, CSWImage* pImage);

	HRESULT GetVideo(const DWORD& dwID, CSWImage** pImage);

	DWORD GetVideoCount();

	HRESULT LockBuffer();

	HRESULT UnlockBuffer();

	/* 创建或获取用于发送组包的缓存，调用之前须用LockBuffer()加锁，用完后用UnlockBuffer()释放 */
	HRESULT GetBuffer(const DWORD dwSize, PVOID* ppvBuffer);

private:

	HRESULT Deinitialize();

private:
	BOOL			m_fInited;
	CHAR*			m_pXmlDocString;
	DWORD			m_dwXmlDocSize;
	CSWImage**		m_pImage;
	_RECORD_FILE_INFO **			m_pImageInfo;
	DWORD			m_dwImageTypeCount;

	DWORD			m_dwRefTime;
	DWORD			m_dwCarArriveTime;

	INT				m_iPTType;
	INT				m_iUnSurePeccancy;
	BOOL			m_fTrafficInfo;

	CSWList<CSWImage*>	m_lstVideo;
	CSWSemaphore		m_cVideoLock;
	CSWImage*			m_pVideo[30];
	DWORD				m_dwVideoCount;

	PVOID			m_pvBuffer;
	CSWSemaphore	m_cBufferLock;
};


#endif //_SW_RECORD_H_



