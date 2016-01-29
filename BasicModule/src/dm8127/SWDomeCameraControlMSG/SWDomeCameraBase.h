#ifndef _SWDOMECAMERABASE_H_
#define _SWDOMECAMERABASE_H_

#include "SWBaseType.h"
#include "SWObject.h"
#include "SWDomeCameraSetting.h"
#include "SWDomeCameraStruct.h"


#define CHECK_ID(dwID, MAX)\
if ((dwID) >= (MAX))\
{\
	SW_TRACE_DEBUG("Err: invalid arg dwID (=%d)\n", dwID);\
	return E_INVALIDARG;\
}\

#define CAMERA_DEVICE_MAX_NUM  8



// 焦距的对应表结构
typedef struct tag_ZoomRatio
{
	INT iRatio;
	INT iValue;
} ZOOM_RATIO;



typedef struct DOME_SRC_STATUS
{
    DOUBLE dbMaxHViewAng;          
    DOUBLE dbMinHViewAng;          
    DOUBLE dbMaxVViewAng;          
    DOUBLE dbMinVViewAng;          
    DOUBLE dbMaxFocalDis;          
    DOUBLE dbMinFocalDis;          
    DOUBLE dbCurHViewAng;          
    DOUBLE dbCurVViewAng;          
    DOUBLE dbCurHTurnAng;          
    DOUBLE dbCurVTurnAng;          
    DOUBLE dbImageCenterX;         
    DOUBLE dbImageCenterY;         
    DOUBLE dbInputLTopX;           
    DOUBLE dbInputLTopY;           
    DOUBLE dbInputRLowerX;         
    DOUBLE dbInputRLowerY;         
	DOUBLE dbInputPointX;			
	DOUBLE dbInputPointY;			

}DOME_SRC_STATUS;



typedef struct _tagDOME_DST_STATUS
{
    DOUBLE dbPan;
    DOUBLE dbTilt;
    DOUBLE dbZoom;
    DOUBLE dbZoomPosition;

}DOME_DST_STATUS;

typedef enum _enumCAMERA_DEVICE
{
	SONY_FCBCH6500 = 0,
	POWERVIEW_PV8430
}CAMERA_DEVICE_ENUM;

typedef struct _tagCAMERA_DEVICE
{
	INT iCameraDevice;				//机芯设备枚举值
	SHORT nVendorID;				//厂家ID
	SHORT nModelID;					//设备ID
	CHAR chName[32];
}CAMERA_DEVICE;



class CSWDomeCameraBase : public CSWObject
{
		CLASSINFO(CSWDomeCameraBase, CSWObject)
public:
    CSWDomeCameraBase(){m_fInit = FALSE; m_pcSettings = NULL;};
    virtual ~CSWDomeCameraBase(){m_fInit = FALSE;};
	virtual BOOL IsInitialized(){return m_fInit;}
	
    HRESULT Initialize(CSWDomeCameraSetting * pcSetting)
	{
		if (!m_fInit && NULL != pcSetting)
		{	
			m_pcSettings = pcSetting;
			m_fInit = TRUE;
		}

		return S_OK;
	}

	
	static INT ConvertPCoordinate(const INT& iDomePanCoord) {return iDomePanCoord > 0 ? (iDomePanCoord * 4096 / 3600) : -1;};
	static INT ConvertTCoordinate(const INT& iDomeTiltCoord) {return iDomeTiltCoord > 0 ? (4096 - iDomeTiltCoord* 4096/3600) : -1;};
	
protected:
	CSWDomeCameraSetting& GetSetting()
	{	
        if (NULL != m_pcSettings)
    	{
    		return *m_pcSettings;
    	}
	}

	HRESULT GetPosParam(POS_PARAM_STRUCT& sPos)
	{
		INT iPanCoord = 0, iTiltCoord = 0, iZoom = 0, iFocus = 0, iIris = 0;
		AF_MODE iFocusMode = AF_AUTO;

		INT iRet = SWPAR_OK;
		
		//swpa_thread_sleep_ms(50);

		INT iTryCount = 10;
	    do 
	    {	
		    iRet = SWPAR_OK;
			
	    	iRet += swpa_camera_ball_get_pt_coordinates(&iPanCoord, &iTiltCoord);
			swpa_thread_sleep_ms(50);
			iRet += swpa_camera_basicparam_get_zoom_value(&iZoom);
			swpa_thread_sleep_ms(50);
			iRet += swpa_camera_basicparam_get_focus_value(&iFocus);
			swpa_thread_sleep_ms(50);
			iRet += swpa_camera_basicparam_get_iris(&iIris);
			swpa_thread_sleep_ms(50);
			iRet += swpa_camera_basicparam_get_AF(&iFocusMode);

			if (iRet != SWPAR_OK)
		    {
				SW_TRACE_DEBUG("Err: failed to get dome camera params(%d, %d, %d, %d, %d)\n",
					iPanCoord, iTiltCoord, iZoom, iFocus, iFocusMode);
				CSWApplication::Sleep(200);
			}
			else
			{
				break;
			}
	    } while (iTryCount > 0);
		
		if (SWPAR_OK != iRet)
		{
			SW_TRACE_DEBUG("Err: failed to get position param.\n");
			return E_FAIL;
		}
		else
		{
			sPos.iFocus = iFocus;
			sPos.iFocusMode = iFocusMode;
			sPos.iIris = iIris;
			sPos.iPanCoordinate = iPanCoord;
			sPos.iTiltCoordinate = iTiltCoord;
			sPos.iTiltSpeed = 0; //todo: no need to get Tilt speed
			sPos.iPanSpeed = 0; //todo: no need to get Pan speed
			sPos.iZoom = iZoom;

			return S_OK;
		}
	};

	HRESULT SetPosParam(const POS_PARAM_STRUCT& sPos)
	{
		INT iPanCoord, iTiltCoord, iZoomValue, iFocusMode, iFocusValue;
		iPanCoord = sPos.iPanCoordinate;
		iTiltCoord = sPos.iTiltCoordinate;
		iZoomValue = sPos.iZoom;
		iFocusMode = sPos.iFocusMode;
		iFocusValue = sPos.iFocus;


	    if (SWPAR_OK != swpa_camera_basicparam_set_AF((AF_MODE)iFocusMode))
	    {
	    	swpa_thread_sleep_ms(500);
	        if (SWPAR_OK != swpa_camera_basicparam_set_AF((AF_MODE)iFocusMode))
		    {
			    SW_TRACE_DEBUG("Err: failed to set AF to %d\n", iFocusMode);
		        return E_FAIL;
	        }
	    }   

		//动力视讯相机要先ZOOM后FOCUS
		if (SWPAR_OK != swpa_camera_basicparam_set_zoom_value(iZoomValue))
	    {
	    	if (SWPAR_OK != swpa_camera_basicparam_set_zoom_value(iZoomValue))
	    	{
		        SW_TRACE_DEBUG("Err: failed to set zoom to %d\n", iZoomValue);
		        return E_FAIL;
	    	}
	    }

	    if (AF_MANUAL == iFocusMode)
	    {
	    	swpa_thread_sleep_ms(500);
	        if (SWPAR_OK != swpa_camera_basicparam_set_focus_value(iFocusValue))
	        {
	            SW_TRACE_DEBUG("Err: failed to set focus value %d\n", iFocusValue);
	            return E_FAIL;
	        }
	    }

		

	    if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iPanCoord, iTiltCoord))
	    {
	    	swpa_thread_sleep_ms(500);
	    	if (SWPAR_OK != swpa_camera_ball_set_pt_coordinates(iPanCoord, iTiltCoord))
	    	{
	    		SW_TRACE_DEBUG("Err: failed to set pt to (%d, %d)\n", iPanCoord, iTiltCoord);
		        return E_FAIL;
	    	}
	    }
   
	    BOOL fOK = FALSE;
	    INT iTryCount = 10;
	    do 
	    {
	        CSWApplication::Sleep(500);

	        INT iPan = -1, iTilt = -1, iZoom = -1, iAF = -1, iFocus = -1;
			swpa_camera_basicparam_get_zoom_value(&iZoom);
	        swpa_camera_ball_get_pt_coordinates(&iPan, &iTilt);	        
			//SW_TRACE_DEBUG("---tobemarked: preset goto zoom %d, now %d\n", iZoomValue, iZoom);
			swpa_thread_sleep_ms(50);
			swpa_camera_ball_sync_zoom();
			swpa_thread_sleep_ms(50);
	        swpa_camera_basicparam_get_AF((AF_MODE*)&iAF);
	        swpa_camera_basicparam_get_focus_value(&iFocus);

	        BOOL fPanOK = FALSE, fTiltOK = FALSE, fZoomOK = FALSE, fAFOK = FALSE, fFocusOK = FALSE;
	        fPanOK = (swpa_abs(iPanCoord - iPan) < 5);
	        if (!fPanOK && iPanCoord > 3595)
	        {
	            fPanOK = (iPan > 0 && iPan < 5 && swpa_abs(3600 + iPan - iPanCoord));
	        }
	        else if (!fPanOK && iPanCoord < 5)
	        {
	            fPanOK = (iPan > 3595 && swpa_abs(3600 + iPanCoord - iPan));
	        }

	        fTiltOK = (swpa_abs(iTiltCoord - iTilt) < 5);
	        fZoomOK = (swpa_abs(iZoomValue - iZoom) < 5);
	        fAFOK = iFocusMode == iAF;
	        fFocusOK = iFocusMode == (AF_MANUAL) ? (swpa_abs(iFocusValue - iFocus) < 5) : TRUE;
	        
	        fOK = fPanOK && fTiltOK && fZoomOK && fAFOK && fFocusOK;
	        iTryCount --;

	        //SW_TRACE_DEBUG("---tobemarked: (%d, %d, %d, %d, %d)\n", iPan, iTilt, iZoom, iAF, iFocus);
	        //SW_TRACE_DEBUG("---tobemarked: (%d, %d, %d, %d, %d)\n", fPanOK, fTiltOK, fZoomOK, fAFOK, fFocusOK);
	    } while ( !fOK && iTryCount != 0);

	    if (0 == iTryCount && !fOK)
	    {
	        return E_FAIL;
	    }

		swpa_camera_ball_sync_zoom();

	    return S_OK;
	};

public:
	const static CAMERA_DEVICE m_cDeviceTable[CAMERA_DEVICE_MAX_NUM];
	
protected:
    BOOL m_fInit;
	CSWDomeCameraSetting* m_pcSettings;
	
    CSWThread m_cThread;
};



class CSWCamera : public CSWObject
{
	CLASSINFO(CSWCamera, CSWObject)
public:
//	CSWCamera();
	CSWCamera();
	virtual ~CSWCamera();
	INT Initialize(SHORT nVendorID, SHORT nModelID, SHORT nROMVersion);
	VOID SonyInitialize();
	VOID PowerViewInitialize();
	BOOL ComputeDemoCameraPTZ( DOME_SRC_STATUS *pSrcParam, DOME_DST_STATUS *pDstParam);
	INT ZoomRatioToValue(FLOAT fltZoomRatio);
	FLOAT ZoomValueToRatio(INT iDomeZoomValue);
	FLOAT ZoomToHViewAngle(FLOAT fltZoom);
	FLOAT ZoomToVViewAngle(FLOAT fltZoom);
	FLOAT HViewAngleToZoom(FLOAT fltHViewAnale);
	FLOAT GetMaxFocalDis();
	FLOAT GetMinFocalDis();
	SHORT GetVendorID();
	SHORT GetModelID();
	SHORT GetROMVersion();
	VOID GetCamName(CHAR *pCamName);
	INT GetCamID();
	

public:
	ZOOM_RATIO m_cZoomTable[30];
	FLOAT m_fltAngleOfVViewTable[30];
	FLOAT m_fltAngleOfHViewTable[30];
	INT m_iMaxZoom;
	
private:
	FLOAT m_fltMaxFocalDis;
	FLOAT m_fltMinFocalDis;
	SHORT m_nVendorID;
	SHORT m_nModelID;
	SHORT m_nROMVersion;
	CHAR m_chCamName[32];
	INT m_iCamID;
};


#endif // _SWDOMECAMERABASE_H_

