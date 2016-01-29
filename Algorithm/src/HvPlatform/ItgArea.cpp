#include "ItgArea.h"


CItgArea::CItgArea()
{
	m_rcRect.SetRect(0,0,0,0);

	m_nItgStrideWidth = m_nItgHeight = 0;

	m_pItgBuf = m_pItgSqBuf = m_pItgImage = m_pItgSqImage = NULL;

	m_pNextItgArea = NULL;

	m_fCalcItg = FALSE;
}

CItgArea::~CItgArea()
{
	Clear();
}

void CItgArea::Clear()
{
	FreeItgBuf();

	m_rcRect.SetRect(0,0,0,0);

	m_nItgStrideWidth = m_nItgHeight = 0;

	m_pItgBuf = m_pItgSqBuf = NULL;

	m_pNextItgArea = NULL;

	m_fCalcItg = FALSE;
}

HRESULT CItgArea::FreeItgBuf()
{
	UINT nBufSize = m_nItgStrideWidth * m_nItgHeight;
	if(m_fFastMem)
	{
		if(m_pItgImage) HV_FreeFastMem(m_pItgImage, nBufSize * sizeof(DWORD32));
		if(m_pItgSqImage) HV_FreeFastMem(m_pItgSqImage, nBufSize * sizeof(DWORD32));
	}
	else
	{
		if(m_pItgImage) HV_FreeMem(m_pItgImage, nBufSize * sizeof(DWORD32));
		if(m_pItgSqImage) HV_FreeMem(m_pItgSqImage, nBufSize * sizeof(DWORD32));
	}

	m_pItgImage = m_pItgSqImage = NULL;

	return S_OK;
}

HRESULT CItgArea::AllocItgBuf(UINT nItgBufSize, BOOL fFastMem)
{
	if(nItgBufSize < m_nItgStrideWidth * m_nItgHeight) return S_FALSE;

	FreeItgBuf();

	if(fFastMem)
	{
		m_pItgImage = (PDWORD32)HV_AllocFastMem(nItgBufSize * sizeof(DWORD32));
		m_pItgSqImage = (PDWORD32)HV_AllocFastMem(nItgBufSize * sizeof(DWORD32));
	}
	else
	{
		m_pItgImage = (PDWORD32)HV_AllocMem(nItgBufSize * sizeof(DWORD32));
		m_pItgSqImage = (PDWORD32)HV_AllocMem(nItgBufSize * sizeof(DWORD32));
	}

	if( !m_pItgImage || !m_pItgSqImage)
	{
		Clear();
		return E_OUTOFMEMORY;
	}

	return S_OK;
}

HRESULT CItgArea::SetRect(int nLeft, int nTop, int nRight, int nBottom, BOOL fCalcItg, BOOL fFastMem)
{
	if(!fCalcItg)
	{
		m_rcRect.SetRect(nLeft, nTop, nRight, nBottom);
		m_fCalcItg = fCalcItg;
		return S_OK;
	}

	m_fCalcItg = fCalcItg;

	int nItgStrideWidth = (((nRight - nLeft + 127) >> 7) << 7) + 4;
	int nItgHeight = nBottom - nTop + 4;
	int nItgBufSize = nItgStrideWidth * nItgHeight;

	RTN_HR_IF_FAILED(AllocItgBuf(nItgBufSize, fFastMem));

    m_fFastMem = fFastMem;
	m_nItgStrideWidth = nItgStrideWidth;
	m_nItgHeight = nItgHeight;

	HV_memset(m_pItgImage, 0, m_nItgStrideWidth * sizeof(DWORD32));
	HV_memset(m_pItgSqImage, 0, m_nItgStrideWidth * sizeof(DWORD32));

	m_pItgBuf = m_pItgImage + m_nItgStrideWidth;
	m_pItgSqBuf = m_pItgSqImage + m_nItgStrideWidth;

	m_rcRect.SetRect(nLeft, nTop, nRight, nBottom);

	return S_OK;
}

void CItgArea::UpdateItgImage(RESTRICT_PBYTE8 pSrc, UINT nSrcWidth, UINT nSrcHeight, UINT nSrcStrideWidth)
{
	RESTRICT_PDWORD32 pItgUp = m_pItgBuf - m_nItgStrideWidth;
	RESTRICT_PDWORD32 pItgSqUp = m_pItgSqBuf - m_nItgStrideWidth;

	DWORD32 dwItgSum(0), dwItgSqSum(0);

	for(UINT i = 0; i < nSrcHeight; i++)
	{
		m_pItgBuf[0] = 0;
		m_pItgSqBuf[0] = 0;
#if (RUN_PLATFORM == PLATFORM_DSP_BIOS)
#pragma MUST_ITERATE (20);
#endif
		for(UINT j = 1; j <= nSrcWidth; j++)
		{
			dwItgSum += pSrc[j-1];
			dwItgSqSum += pSrc[j-1]*pSrc[j-1];

			m_pItgBuf[j] = dwItgSum + pItgUp[j];
			m_pItgSqBuf[j] = dwItgSqSum + pItgSqUp[j];
		}
		pItgUp = m_pItgBuf;
		pItgSqUp = m_pItgSqBuf;

		m_pItgBuf += m_nItgStrideWidth;
		m_pItgSqBuf += m_nItgStrideWidth;

		pSrc += nSrcStrideWidth;

		dwItgSum = 0;
		dwItgSqSum = 0;
	}
}
