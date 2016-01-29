#ifndef __SW_POS_IMAGE_H__
#define __SW_POS_IMAGE_H__

class CSWPosImage : public CSWObject
{
	CLASSINFO(CSWPosImage, CSWObject)
public:	
	CSWPosImage(CSWImage *pImage, DWORD dwPosCount = 0, BOOL fDebugFlag = FALSE);
	virtual ~CSWPosImage();
	CSWImage* GetImage(void);
	HRESULT SetImage(CSWImage* pImage);
	BOOL      GetDebugFlag(void){return m_fDebugFlag;}
	SW_RECT&  operator[](int id);
	SW_RECT&  GetRect(int id);
	DWORD     GetCount(void);
	VOID      SetCount(INT iCnt);
	
	HRESULT SetPolygon(const INT iID, const INT iVertexCount, SW_POINT* psPoints);
	HRESULT GetPolygon(const INT iID, INT& iVertexCount, SW_POINT* psPoints);
	INT GetPolygonMaxCount();
	
private:
	CSWMemory* m_pMemory;
	CSWImage* m_pImage;
	SW_RECT*  m_pRect;
	DWORD   m_dwPosCount;
	DWORD   m_fDebugFlag;
	SW_POINT m_sPolygon[8][8];
};
#endif
