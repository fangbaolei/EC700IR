#ifndef _OBJMODEL_INCLUDED__
#define _OBJMODEL_INCLUDED__

// 模版匹配类

#include "ObjTemple.h"
#include "hist.h" // 直方图处理； added by zoul, 20110815

using namespace signalway; // 有关直方图的结构和操作放在signalway名字空间内

class CObjMatch
{
public:
	CObjMatch();
	~CObjMatch();

	void Clear();

	HRESULT UpdateTemple(HV_COMPONENT_IMAGE *pImg, HiVideo::CRect rc, bool fFastMem = FALSE);
	HRESULT PrepareIntegral(HV_COMPONENT_IMAGE *pImg);			// 积分图计算

	HRESULT GetBestMatch(HV_COMPONENT_IMAGE *pImg, HiVideo::CRect *rcNect, 
		bool fUseHistMatch, float fltThreld, bool fNewMethod, int iDelayCount, int iMaxDiff, bool fIsDay);		// 模版匹配，返回最佳位

	HRESULT TempleMatch(HV_COMPONENT_IMAGE *pImg, CObjTemple *pTemple, HiVideo::CRect *rcNect, 
		bool fUseHistMatch, float fltThreld, int iDelayCount, int iMaxDiff, bool fIsDay);		// 模版匹配，返回最佳位置

	static int X_SCAN_STEP;
	static int Y_SCAN_STEP;
protected:
	CObjTemple *m_pTmpOrg;			// 原始的模型（暂时只考虑1个scale）

private:
	CFastMemAlloc m_cFastStack;

	// 积分图使用的变量
	WORD16 				m_nItgImageWidth;				//积分图宽度.
	WORD16 				m_nItgImageHeight;				//积分图高度.
	WORD16 				m_nItgStrideWidth;				//积分图存储Stride宽度.

	PDWORD32 			m_pdwItgImage;					//一般积分图数据.
	PDWORD32 			m_pdwItgSqImage;				//平方积分图数据.

	WORD16				m_nItgBufStrideWidth;			//积分图内存尺寸
	WORD16				m_nItgBufMaxHeight;

	PDWORD32			m_pdwItgBuf;					//积分图内存指针
	PDWORD32			m_pdwItgSqBuf;

	bool                m_bHistSaved ;                  // 模板的直方图是否被保存； added by zoul, 20110817
	histogram           m_histMatched ;                 // 直方图结构； added by zoul, 20110815
	// zhaopy
	histogram			m_histBase;
	int					m_iDiffCount;
	static const int    MAX_DIFF = 18;
	static const int	CMP_DIFFCOUNT = 8;
	float				m_rgdbDiff[CMP_DIFFCOUNT];

	histogram			m_histBaseOld;
	//直方图缓存, 固定为16*16
	int	m_rgHistBase[16*16];
	int m_nHistBaseType; //0为二维直方图,1为一维直方图
};

#endif // _OBJMODEL_INCLUDED__
