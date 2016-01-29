///////////////////////////////////////////////////////////
//  CSWDspLinkCtrl.h
//  Implementation of the Class CSWDspLinkCtrl
//  Created on:      
//  Original author: 
///////////////////////////////////////////////////////////

#if !defined(_SWDSPLINKCTRL_H_)
#define _SWDSPLINKCTRL_H_
#include "SWBaseLinkCtrl.h"

/**
 * @brief 
 */
class CSWDspLinkCtrl : public CSWBaseLinkCtrl
{
CLASSINFO(CSWDspLinkCtrl,CSWBaseLinkCtrl)
public:
	/**
	 * @brief 构造函数
	 */
	CSWDspLinkCtrl();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWDspLinkCtrl();
	
	HRESULT Initialize(VOID);
	VOID Clear();
	/**
	 * @brief 视频流识别初始化接口
	 * 
	 * @param [in] cVideoRecogerParam : 视频流识别参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT InitVideoRecoger(INIT_VIDEO_RECOGER_PARAM& cVideoRecogerParam);

	/**
	 * @brief 图片版识别初始化接口
	 *
	 * @param [in] cPhotoRecogerParam : 图片版识别参数。
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT InitPhotoRecoger(INIT_PHOTO_RECOGER_PARAM& cPhotoRecogerParam);
	
	HRESULT RegisterCallBack(VOID (*OnDSPAlarm)(PVOID pvParam, INT iInfo), PVOID pvParam);
protected:
	HRESULT DoProcess(CSWImage* pImage, const INT& iTimeOut);
private:
	CSWFile* m_pDspLink;		
};

#endif 

