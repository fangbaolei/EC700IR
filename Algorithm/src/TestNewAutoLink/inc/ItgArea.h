#ifndef _ITG_AREA_H_
#define _ITG_AREA_H_

#include "hvbaseobj.h"
#include "HvUtils.h"

class CItgArea
{
private:
	BOOL m_fCalcItg;

	HiVideo::CRect m_rcRect;

	UINT m_nItgStrideWidth;
	UINT m_nItgHeight;

	BOOL m_fFastMem;

	PDWORD32 m_pItgImage, m_pItgSqImage, m_pItgBuf, m_pItgSqBuf;

	CItgArea* m_pNextItgArea;
public:
	CItgArea();

	~CItgArea();

	void Clear();

	HRESULT FreeItgBuf();

	HRESULT AllocItgBuf(UINT nItgBufSize, BOOL fFastMem);

	HRESULT SetRect(const HiVideo::CRect rcSrc, BOOL fCalcItg, BOOL fFastMem = FALSE)
	{
		return SetRect(rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom, fCalcItg, fFastMem);
	}

	HRESULT SetRect(int nLeft, int nTop, int nRight, int nBottom, BOOL fCalcItg, BOOL fFastMem = FALSE);

	void UpdateItgImage(RESTRICT_PBYTE8 pSrc, UINT nSrcWidth, UINT nSrcHeight, UINT nSrcStrideWidth);

	//防止不必要的指针赋值
	CItgArea& operator=(const CItgArea& src) 
	{
		return *this;
	}

	PDWORD32 GetItgImage()
	{
		return m_pItgImage;
	}

	PDWORD32 GetItgSqImage()
	{
		return m_pItgSqImage;
	}

	const HiVideo::CRect& GetRect()
	{
		return m_rcRect;
	}

	DWORD32 GetItgStrideWidth()
	{
		return m_nItgStrideWidth;
	}

	CItgArea* GetNextArea()
	{
		return m_pNextItgArea;
	}

	void SetNextArea(CItgArea* pNextArea)
	{
		m_pNextItgArea = pNextArea;
	}

	void EnableCalcItg(BOOL fEnable)
	{
		m_fCalcItg = fEnable;
	}

	BOOL IsCalcItg()
	{
		return m_fCalcItg;
	}
};

#endif
