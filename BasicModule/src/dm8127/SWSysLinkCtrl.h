///////////////////////////////////////////////////////////
//  CSWBaseLinkCtrl.h
//  Implementation of the Class CSWBaseLinkCtrl
//  Created on:      
//  Original author: 
///////////////////////////////////////////////////////////

#if !defined(_SWSYSLINKCTRL_H_)
#define _SWSYSLINKCTRL_H_
#include "SWBaseLinkCtrl.h"
//#define PR_IN_ARM
#ifdef PR_IN_ARM
#include "EPAppDetCtrl.h"
#endif


/**
 * @brief 
 */
class CSWSysLinkCtrl : public CSWBaseLinkCtrl
{
CLASSINFO(CSWSysLinkCtrl,CSWBaseLinkCtrl)

public:
	/**
	 * @brief 构造函数
	 */
	CSWSysLinkCtrl();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWSysLinkCtrl();
	
	virtual HRESULT Initialize(VOID);
	VOID Clear();

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
	 * @brief 图片版识别初始化接口
	 *
	 * @param [in] cPhotoRecogerParam : 图片版识别参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT InitPhotoRecoger(INIT_PHOTO_RECOGER_PARAM& cPhotoRecogerParam);

	virtual HRESULT RegisterCallBack(VOID (*OnDSPAlarm)(PVOID pvParam, INT iInfo), PVOID pvParam);
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
protected:
	virtual HRESULT DoProcess(CSWImage* pImage, const INT& iTimeOut);
	static int OnResult(void *pContext, int type, void *struct_ptr);
private:
	INT m_hlstJpeg;
	INT m_hlstDspRes;
#ifdef PR_IN_ARM
	CEPAppDetCtrl m_cEPApp;
	INIT_VIDEO_RECOGER_PARAM m_cVideoRecogerParam;
	BOOL m_fInitEPApp;
#endif	
};

#endif 

