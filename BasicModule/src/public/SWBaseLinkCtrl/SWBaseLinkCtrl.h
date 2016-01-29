///////////////////////////////////////////////////////////
//  CSWBaseLinkCtrl.h
//  Implementation of the Class CSWBaseLinkCtrl
//  Created on:      
//  Original author: 
///////////////////////////////////////////////////////////

#if !defined(_SWBASELINKCTRL_H_)
#define _SWBASELINKCTRL_H_

#include "SWObject.h"
#include "SWFile.h"
#include "SWMutex.h"
#include "DspLinkCmd.h"
#include "trackerdef.h"

#define MAX_BUFFER_SIZE  (1024 * 1024)

typedef struct tag_ProcessOneFrameInfo
{
	INT iVideoID;
	INT iDiffTick;
	BOOL fIsCheckLightType;
	DWORD dwIOLevel;
	INT iCarArriveTriggerType;
}
SW_FRAME_OTHER_INFO;

/**
 * @brief 
 */
class CSWBaseLinkCtrl : public CSWObject
{
CLASSINFO(CSWBaseLinkCtrl,CSWObject)
public:
	static CSWBaseLinkCtrl* GetInstance();

public:
	virtual BOOL IsInitialize(VOID){return m_fInitialized;}
	virtual HRESULT Initialize(VOID);
	virtual VOID Clear();
	/**
	 * @brief JPEG压缩接口
	 * 
	 * @param [in] pImageSrc : 待压缩的图片
	 * @param [out] ppImageJpeg : 压缩后的JPEG图片,由接口申请，使用者释放。
	 * @param [in] dwCompressRate : 压缩率
	 * @param [in] dwJpegDataType : JPEG图片格式（0：表示Jpeg数据段分三段；1：表示Jpeg数据段为一段）
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT EncodeJpeg(CSWImage* pImageSrc
		, CSWImage** ppImageJpeg
		, const DWORD& dwCompressRate = 50
		, const DWORD& dwJpegDataType = 0
		, const PBYTE bPhys = NULL
		, const CHAR* szTime = NULL
		, const INT iX = 0
		, const INT iY = 0
		, const BYTE yColor = 0
		, const BYTE uColor = 128
		, const BYTE vColor = 128
		, const SW_RECT* pRect = NULL
        , const BOOL fCrop = FALSE
	);
	/**
	 * @brief 视频流识别初始化接口
	 * 
	 * @param [in] cVideoRecogerParam : 视频流识别参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT InitVideoRecoger(INIT_VIDEO_RECOGER_PARAM& cVideoRecogerParam);

	/**
	 * @brief 视频流识别参数动态修改。
	 *
	 * @param [in] cVideoRecogerParam : 视频流识别参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT ModifyVideoRecogerParam(TRACKER_CFG_PARAM& cVideoRecogerParam);

	/**
	 * @brief 更新算法私有参数。
	 *
	 * @param [in] pbData : 算法参数
	 * @param [in] iDataSize : 参数大小
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT UpdateAlgParam(unsigned char* pbData, int iDataSize);

	/**
	 * @brief 更新车辆检测模型。
	 *
	 * @param [in] uiModelId : 模型ID
	 * @param [in] pbData : 模型数据
	 * @param [in] iDataSize : 模型数据大小
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT UpdateModel(unsigned int uiModelId, unsigned char* pbData, int iDataSize);

	/**
	 * @brief 视频流识别帧接口
	 * 
	 * @param [in] pImage : 当前处理帧图片。
	 * @param [out] pFrameRespond : 一帧处理结束后的返回信息。
	 * @param [in] pFrameRecognizeParam: 单帧识别的参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT ProcessOneFrame(CSWImage* pImage, PROCESS_EVENT_STRUCT* pProcessEvent, FRAME_RECOGNIZE_PARAM* pFrameRecognizeParam = NULL);

	 /**
	  * @brief 视频流车辆检测接口
	  *
	  * @param [in] pImage : 当前处理帧图片。
	  * @param [in] pDetParam : 检测参数。
	  * @param [out] pFrameRespond : 一帧处理结束后的返回信息
	  * @return
	  * - S_OK : 成功
	  * - E_FAIL : 失败
	  */
	 virtual HRESULT ProessDetOneFrame(CSWImage* pImage, CAR_DETECT_PARAM* pDetParam, PROCESS_DETECT_INFO* pFrameRespond);


	/**
	 * @brief 图片版识别初始化接口
	 *
	 * @param [in] cPhotoRecogerParam : 图片版识别参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT InitPhotoRecoger(INIT_PHOTO_RECOGER_PARAM& cPhotoRecogerParam);
	/**
     * @brief 图片版识别帧接口
     *
     * @param [in] pImage : 当前处理帧图片。
     * @param [in] pFrameParam : 识别参数。
     * @param [out] pProcessEvent : 一帧处理结束后的返回信息。
     * @return
     * - S_OK : 成功
     * - E_FAIL : 失败
     */
  virtual HRESULT ProcessPhoto(CSWImage* pImage, PR_PARAM *pFrameParam, PROCESS_EVENT_STRUCT* pProcessEvent);

	virtual DWORD AddRef()
	{
		return 0;
	}
	virtual HRESULT Release()
	{
		return S_OK;
	}
	
	virtual HRESULT RegisterCallBack(VOID (*OnDSPAlarm)(PVOID pvParam, INT iInfo), PVOID pvParam);

protected:
	/**
	 * @brief 构造函数
	 */
	CSWBaseLinkCtrl();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWBaseLinkCtrl();
	
	/**
	 * @brief 处理函数
	 * 
	 * @param [in] cCmdId : 命令ID
	 * @param [in] pvParam : 命令参数
	 * @param [in] dwParamSize : 命令参数长度
	 * @param [in] pvData : 命令数据
	 * @param [in] dwParamSize : 命令数据长度
	 * @param [out] pvRespond : 命令返回数据
	 * @param [int] dwRespondSize : 命令返回数据长度
	 * @param [in] dwTimeOut : 超时时间，单位ms
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT ProcessCmd(
		CSWImage* pImage,
		const INT& iCmdId, 
		PVOID pvParam, const DWORD& dwParamSize, 
		PVOID pvData, const DWORD& dwDataSize,
		PVOID pvRespond, const DWORD& dwRespondSize, 
		const INT& iTimeOut
		);
	
	virtual HRESULT DoProcess(CSWImage* pImage, const INT& iTimeOut);
	virtual HRESULT ConvertImage(CSWImage *pImage, HV_COMPONENT_IMAGE& hvImage);
protected:
	CSWMemory* m_pMemoryData;
	CSWMemory* m_pMemoryRespond;
	CSWMemory* m_pMemoryPoint;
	CSWMemoryFactory* m_pMemoryFactory;
	CSWSemaphore m_cSem;
	CSWSemaphore m_cSemRecog;
	BOOL m_fInitialized;

	BOOL m_fJpegIsWait;	

	CSWImage* m_rgpSmallImage[MAX_EVENT_COUNT];
	CSWImage* m_rgpBinImage[MAX_EVENT_COUNT];
};

#endif 

