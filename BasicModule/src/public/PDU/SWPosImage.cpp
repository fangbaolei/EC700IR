#include "SWFC.h"
#include "SWPosImage.h"

CSWPosImage::CSWPosImage(CSWImage *pImage, DWORD dwPosCount, BOOL fDebugFlag)
{
	m_dwPosCount = dwPosCount;
	m_fDebugFlag = fDebugFlag;
	m_pMemory = CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY)->Alloc(sizeof(SW_RECT)*20);
	m_pRect = (SW_RECT *)m_pMemory->GetBuffer();
	swpa_memset(m_pRect, 0, sizeof(sizeof(SW_RECT)*20));
	m_pImage = pImage;
	if(NULL != m_pImage)
	{
		m_pImage->AddRef();
	}

	swpa_memset(m_sPolygon, -1, sizeof(m_sPolygon));
}

CSWPosImage::~CSWPosImage()
{
	if(m_pMemory)
	{
		CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY)->Free(m_pMemory);
		m_pMemory = NULL;
		m_pRect = NULL;
	}
	SAFE_RELEASE(m_pImage);
}

CSWImage* CSWPosImage::GetImage(void)
{
	return m_pImage;
}


HRESULT CSWPosImage::SetImage(CSWImage* pImage)
{
	if (NULL != pImage)
	{
		SAFE_RELEASE(m_pImage);
		m_pImage = pImage;
		SAFE_ADDREF(m_pImage);

		return S_OK;
	}

	return E_INVALIDARG;
}


SW_RECT& CSWPosImage::operator[](int id)
{
	SW_RECT rc = {0, 0, 0, 0};
	return NULL != m_pRect && id < m_dwPosCount ? m_pRect[id] :rc;
}

SW_RECT& CSWPosImage::GetRect(int id)
{
	return (*this)[id];
}

DWORD CSWPosImage::GetCount(void)
{
	return m_dwPosCount;
}

VOID CSWPosImage::SetCount(INT iCnt)
{
	m_dwPosCount = iCnt;
}

HRESULT CSWPosImage::SetPolygon(INT iID, INT iVertexCount, SW_POINT* psPoints)
{
	if (iID <0 || iID >= 8 || iVertexCount > 8 || (iVertexCount > 0 && NULL == psPoints))
	{
		return E_INVALIDARG;
	}

	swpa_memset(&m_sPolygon[iID], -1, sizeof(m_sPolygon[iID]));

	swpa_memcpy(&m_sPolygon[iID], psPoints, iVertexCount * sizeof(SW_POINT));
	
	return S_OK;
}

HRESULT CSWPosImage::GetPolygon(INT iID, INT& iVertexCount, SW_POINT* psPoints)
{
	if (iID <0 || iID >= 8 || NULL == psPoints)
	{
		return E_INVALIDARG;
	}

	iVertexCount = 0;
	while (m_sPolygon[iID][iVertexCount].x != -1 && m_sPolygon[iID][iVertexCount].y != -1)
	{
		iVertexCount ++;
	}

	if (iVertexCount > 0)
	{
		swpa_memcpy(psPoints, &m_sPolygon[iID], iVertexCount*sizeof(SW_POINT));
	}
	return S_OK;
}


INT CSWPosImage::GetPolygonMaxCount()
{
	return sizeof(m_sPolygon) / sizeof(m_sPolygon[0]);
}


