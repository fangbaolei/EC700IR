#ifndef __SW_LPR_Hvc_PARAMETER_H__
#define __SW_LPR_Hvc_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPRHvcParameter : public CSWLPRParameter
{
	CLASSINFO(CSWLPRHvcParameter, CSWLPRParameter)
public:
	CSWLPRHvcParameter();
	virtual ~CSWLPRHvcParameter();
protected:
	virtual HRESULT Initialize(CSWString strFilePath);
	/**
	 *@brief 初始化system节点
	 *@return 成功返回S_OK,其他值表示失败
	 */
	virtual HRESULT InitSystem(VOID);

	/**
	 *@brief
	 */
	virtual HRESULT InitTracker(VOID);

	/**
	 *@brief
	 */
	virtual HRESULT InitHvDsp(VOID);

	/**
	 *
	 */
	virtual HRESULT InitOuterCtrl(VOID);

	/**
	 *
	 */
	virtual HRESULT InitIPTCtrl(VOID);

	/**
	 *
	 */
	virtual HRESULT InitCamApp(VOID);
	/**
	 *
	 */
	virtual HRESULT Init368(VOID);
};
REGISTER_CLASS(CSWLPRHvcParameter)
#endif
