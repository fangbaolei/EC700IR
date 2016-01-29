#include "Pretreatmentimpl.h"
#include "HvUtils.h"
#include "math.h"

#define MAX_PROC_WIDTH 800
#define MAX_PROC_HEIGHT 300

CPretreatmentImpl::CPretreatmentImpl(void)
	:m_nRotate(0)
	,m_nTilt(0)
	,m_iWidth(0)
	,m_iHeight(0)
	,m_iStride(0)
	,m_pHvParam(NULL)
{
	HV_memset(&m_imgCalibrated,0,sizeof(HV_COMPONENT_IMAGE));
	m_imgCalibrated.nImgType = HV_IMAGE_YUV_422;
}

CPretreatmentImpl::~CPretreatmentImpl(void)
{
	Clear();
}

void CPretreatmentImpl::Clear()
{
	if (m_imgCalibrated.pbData[0])
	{
		HV_FreeMem(m_imgCalibrated.pbData[0], m_iHeight* m_iStride);
		m_imgCalibrated.pbData[0] = NULL;
	}
	if (m_imgCalibrated.pbData[1])
	{
		HV_FreeMem(m_imgCalibrated.pbData[1], m_iHeight*(m_iStride+1)>>1);
		m_imgCalibrated.pbData[1] = NULL;
	}
	if (m_imgCalibrated.pbData[2])
	{
		HV_FreeMem(m_imgCalibrated.pbData[2], m_iHeight*(m_iStride+1)>>1);
		m_imgCalibrated.pbData[2] = NULL;
	}

	HV_memset(&m_imgCalibrated,0,sizeof(HV_COMPONENT_IMAGE));

	m_iWidth = 0;
	m_iHeight = 0;
	m_iStride = 0;
}

HRESULT CPretreatmentImpl::OnInit()
{
	Clear();

	m_nRotate = 0;
	m_nTilt = 0;

	return S_OK;
}

HRESULT CPretreatmentImpl::Initialize()
{
	OnInit();

// 高清版本无需支持预矫正
#ifndef _HIGH_DEVICE
	if (m_pHvParam)
	{
		m_pHvParam->GetInt(
			"Tracker\\PreTreatment", "Rotate",
			&m_nRotate, m_nRotate,
			-45,45,
			"水平旋转角度","",3
			);

		m_pHvParam->GetInt(
			"Tracker\\PreTreatment", "Tilt", 
			&m_nTilt, m_nTilt,
			-45,45,
			"垂直倾斜角度","",3
			);
	}
#endif
	return S_OK;
}

HRESULT CPretreatmentImpl::SetHvParam(HvCore::IHvParam2* pHvParam)
{
	m_pHvParam = pHvParam;
	return S_OK;
}
                           
HRESULT CPretreatmentImpl::PreCalibrate(HV_COMPONENT_IMAGE ImgOri,HV_COMPONENT_IMAGE* pImgCalibrated)
{
	if ( (ImgOri.iStrideWidth[0] < ImgOri.iWidth) || (ImgOri.iHeight == 0) || (ImgOri.iWidth == 0) )
	{
		return E_INVALIDARG;
	}

	if (ImgOri.iWidth != m_iWidth || ImgOri.iHeight != m_iHeight)  //图像大小变化重设变换矩阵并分配内存
	{
		FRAME_PROPERTY cFrameProperty;
		cFrameProperty.iWidth = ImgOri.iWidth;
		cFrameProperty.iHeight = ImgOri.iHeight;
		cFrameProperty.iStride = ImgOri.iStrideWidth[0];

		RTN_HR_IF_FAILED(SetFrameProperty(cFrameProperty));

		ReCalc();
	}

	//m_imgCalibrated.dwTime=ImgOri.dwTime;
	//m_imgCalibrated.dwFlag=ImgOri.dwFlag;
	//m_imgCalibrated.pvImageArg = ImgOri.pvImageArg;

	int iHeight=ImgOri.iHeight;
	int iWidth=ImgOri.iWidth;

	WORD16 iStrideWidth=ImgOri.iStrideWidth[0];
	WORD16 iStrideWidth2=(iStrideWidth+1)>>1;

	RESTRICT_PBYTE8 pbTempY=m_imgCalibrated.pbData[0];
	RESTRICT_PBYTE8 pbTempCb=m_imgCalibrated.pbData[1];
	RESTRICT_PBYTE8 pbTempCr=m_imgCalibrated.pbData[2];

	RESTRICT_PBYTE8 pbLineStart_Y(pbTempY);
	RESTRICT_PBYTE8 pbLineStart_Cr(pbTempCr);
	RESTRICT_PBYTE8 pbLineStart_Cb(pbTempCb);
	RESTRICT_PBYTE8 pbTempLineStart_Y;
	RESTRICT_PBYTE8 pbTempLineStart_Cr;
	RESTRICT_PBYTE8 pbTempLineStart_Cb;

	int nM21=m_Mat.dx , nM22=m_Mat.dy;
	int x_org(0),y_org=(0);

	WORD16 x1(0),y1(0),x1_c(0);
	BYTE8 xm,ym,xm1,ym1,xm_c,xm1_c;

	int iOffset(0),iOffset2(0);

	BYTE8 nPy1,nPy2,nPy3,nPy4;
	int nP1234_y(0);

	BYTE8 nPr1,nPr2,nPr3,nPr4;
	int nP1234_r(0);

	BYTE8 nPb1,nPb2,nPb3,nPb4;
	int nP1234_b(0);

	WORD16 w1,w2,w3,w4,w1_c,w2_c,w3_c,w4_c;

	int nMaxWidth=(iWidth-1)<<4;
	int nMaxHeight=(iHeight-1)<<4;

	int x,y,x_c;

	int x_start,x_end,x_start1,x_end1,x_start2,x_end2;
	
	CFastMemAlloc FastStack;
	RESTRICT_PBYTE8 pLineBuf_Y = (BYTE8*)FastStack.StackAlloc(iStrideWidth);
	RESTRICT_PBYTE8 pLineBuf_Cr = (BYTE8*)FastStack.StackAlloc(iStrideWidth2);
	RESTRICT_PBYTE8 pLineBuf_Cb = (BYTE8*)FastStack.StackAlloc(iStrideWidth2);
	
	if ( !pLineBuf_Y || !pLineBuf_Cr || !pLineBuf_Cb )
	{	
		return E_FAIL;
	} 

	unsigned int iCpyId_Y(0), iCpyId_Cr(0), iCpyId_Cb(0);

	for(y=0 ; y < iHeight ; y++, nM21+=m_Mat.m21 , nM22+=m_Mat.m22, pbLineStart_Y += iStrideWidth, pbLineStart_Cr += iStrideWidth2,pbLineStart_Cb += iStrideWidth2)
	{
		HV_invalidate( pbLineStart_Y, iStrideWidth );
		HV_invalidate( pbLineStart_Cr, iStrideWidth2 );
		HV_invalidate( pbLineStart_Cb, iStrideWidth2 );
		if (m_Mat.m11 == 0) 
		{
			if (nM21< 0 || nM21 >= nMaxWidth) continue;
			x_start1 = 0;
			x_end1 = iWidth;
		}
		else
		{
			x_start1 = -nM21/m_Mat.m11;
			x_end1 = (nMaxWidth - nM21)/m_Mat.m11;

			if (m_Mat.m11 < 0) 
			{
				x_start1 += x_end1;
				x_end1 = x_start1 - x_end1;
				x_start1 = x_start1 - x_end1;
			}
		}

		if (m_Mat.m12 == 0) 
		{
			if (nM22< 0 || nM22 >= nMaxHeight) continue;
			x_start2 = 0;
			x_end2 = iWidth;
		}
		else
		{
			x_start2 = -nM22/m_Mat.m12;
			x_end2 = (nMaxHeight - nM22)/m_Mat.m12;
			if (m_Mat.m12 < 0) 
			{
				x_start2 += x_end2;
				x_end2 = x_start2 - x_end2;
				x_start2 = x_start2 - x_end2;
			}
		}

		x_start = MIN_INT(iWidth,MAX_INT(0,(MAX_INT(x_start1,x_start2)+1)));
		x_end = MAX_INT(0,MIN_INT(iWidth,(MIN_INT(x_end1,x_end2)-1)));
		
		for ( x = x_start, x_c = x_start>>1 , x_org = nM21 + (x_start-1)*m_Mat.m11, y_org = nM22 +(x_start-1)*m_Mat.m12; x < x_end;)      
		{
			x_org += m_Mat.m11;  //求原坐标像素值  x_org= x*m11 + y*m21 + dx
			y_org += m_Mat.m12;  //					y_org= x*m12 + y*m22 + dy

			x1=x_org>>4;
			y1=y_org>>4;

			xm=x_org & 0xF;
			ym=y_org & 0xF;
			xm1=0x10-xm;
			ym1=0x10-ym;

			x1_c=x_org>>5;

			xm_c=(x_org>>1) & 0xF;
			xm1_c=0x10-xm_c;

			iOffset = y1 * iStrideWidth;
			iOffset2 = iOffset>>1;

			pbTempLineStart_Y = ImgOri.pbData[0] + iOffset;
			pbTempLineStart_Cr = ImgOri.pbData[2] + iOffset2;
			pbTempLineStart_Cb = ImgOri.pbData[1] + iOffset2;
			
			nPy1 = *(pbTempLineStart_Y + x1);
			nPy2 = *(pbTempLineStart_Y + 1 + x1);
			nPy3 = *(pbTempLineStart_Y + x1 + iStrideWidth);
			nPy4 = *(pbTempLineStart_Y + 1 + x1 + iStrideWidth);
			
			nPr1 = *(pbTempLineStart_Cr + x1_c);
			nPr2 = *(pbTempLineStart_Cr + 1 + x1_c);
			nPr3 = *(pbTempLineStart_Cr + x1_c + iStrideWidth2);
			nPr4 = *(pbTempLineStart_Cr + 1 + x1_c + iStrideWidth2);
			
			nPb1 = *(pbTempLineStart_Cb + x1_c);
			nPb2 = *(pbTempLineStart_Cb + 1 + x1_c);
			nPb3 = *(pbTempLineStart_Cb + x1_c + iStrideWidth2);
			nPb4 = *(pbTempLineStart_Cb + 1 + x1_c + iStrideWidth2);

			w1=xm1*ym1;
			w2=xm*ym1;
			w3=xm1*ym;
			w4=xm*ym;
			w1_c=xm1_c*ym1;
			w2_c=xm_c*ym1;
			w3_c=xm1_c*ym;
			w4_c=xm_c*ym;
			
			nP1234_y = nPy1*w1;
			nP1234_y += nPy2*w2;
			nP1234_y += nPy3*w3;
			nP1234_y += nPy4*w4;
			
			nP1234_r = nPr1*w1_c;
			nP1234_r += nPr2*w2_c;
			nP1234_r += nPr3*w3_c;
			nP1234_r += nPr4*w4_c;
			
			nP1234_b = nPb1*w1_c;
			nP1234_b += nPb2*w2_c;
			nP1234_b += nPb3*w3_c;
			nP1234_b += nPb4*w4_c;
			
			*(pLineBuf_Y + x++) =nP1234_y>>8; 
			*(pLineBuf_Cr + x_c) =nP1234_r>>8;
			*(pLineBuf_Cb + x_c++) =nP1234_b>>8;

			x_org += m_Mat.m11;
			y_org += m_Mat.m12;

			x1=x_org>>4;
			y1=y_org>>4;

			xm=x_org & 0xF;
			ym=y_org & 0xF;

			xm1=0x10-xm;
			ym1=0x10-ym;

			iOffset=y1*iStrideWidth;

			pbTempLineStart_Y = ImgOri.pbData[0] + iOffset;
			
			nPy1 = *(pbTempLineStart_Y + x1);
			nPy2 = *(pbTempLineStart_Y + 1 + x1);
			nPy3 = *(pbTempLineStart_Y + x1 + iStrideWidth);
			nPy4 = *(pbTempLineStart_Y + 1 + x1 + iStrideWidth);

			w1=xm1*ym1;
			w2=xm*ym1;
			w3=xm1*ym;
			w4=xm*ym;
			
			nP1234_y = nPy1*w1;
			nP1234_y += nPy2*w2;
			nP1234_y += nPy3*w3;
			nP1234_y += nPy4*w4;

			*(pLineBuf_Y + x++) = nP1234_y>>8;
		}
		iCpyId_Y = HV_dmacpy(pbLineStart_Y,pLineBuf_Y,iStrideWidth );
		HV_dmawait( iCpyId_Y );
		
		iCpyId_Cr = HV_dmacpy(pbLineStart_Cr,pLineBuf_Cr,iStrideWidth2 );
		HV_dmawait( iCpyId_Cr );

		iCpyId_Cb = HV_dmacpy(pbLineStart_Cb,pLineBuf_Cb,iStrideWidth2 );
		HV_dmawait( iCpyId_Cb );
	}

	*pImgCalibrated=m_imgCalibrated;
	
	return S_OK;
}

HRESULT CPretreatmentImpl::SetParam(int nHRotate, int nVTilt)
{
	m_nRotate = nHRotate;
	m_nTilt = nVTilt;

	ReCalc();

	if (m_pHvParam)
	{
		m_pHvParam->SetInt("Tracker\\PreTreatment", "Rotate", m_nRotate);
		m_pHvParam->SetInt("Tracker\\PreTreatment", "Tilt", m_nTilt);
		m_pHvParam->Save(0);
	}
	
	return S_OK;
}

HRESULT CPretreatmentImpl::ReCalc()
{
	if ( m_iWidth == 0 || m_iHeight == 0)
	{
		return S_FALSE;
	}

	float dx=(float)m_iWidth/2;
	float dy=(float)m_iHeight/2;
	float cosH=cos((float)(m_nRotate*3.14159265359/180));
	float sinH=sin((float)(m_nRotate*3.14159265359/180));
	float tanV=-tan((float)(m_nTilt*3.14159265359/180));

	m_Mat.m11=(int)(cosH * 16);
	m_Mat.m12=(int)(-sinH * 16);
	m_Mat.m21=(int)((sinH-cosH*tanV) * 16);
	m_Mat.m22=(int)((cosH+sinH*tanV) * 16);
	m_Mat.dx=(int)((dx-dy*sinH+cosH*tanV*dy-dx*cosH) * 16);
	m_Mat.dy=(int)((dy-dy*cosH-sinH*tanV*dy+dx*sinH) * 16);

	return S_OK;
}

HRESULT CPretreatmentImpl::SetFrameProperty(const FRAME_PROPERTY& cFrameProperty)
{
	Clear();

	m_iWidth = cFrameProperty.iWidth;
	m_iHeight = cFrameProperty.iHeight;
	m_iStride = cFrameProperty.iStride;

	m_imgCalibrated.pbData[0]=(PBYTE8)HV_AllocMem(m_iHeight*m_iStride);
	m_imgCalibrated.pbData[1]=(PBYTE8)HV_AllocMem(m_iHeight*(m_iStride+1)>>1);
	m_imgCalibrated.pbData[2]=(PBYTE8)HV_AllocMem(m_iHeight*(m_iStride+1)>>1);

	if( !m_imgCalibrated.pbData[0] || !m_imgCalibrated.pbData[1] || !m_imgCalibrated.pbData[2])
	{
		Clear();
		return E_OUTOFMEMORY;
	}

	m_imgCalibrated.iWidth = m_iWidth;
	m_imgCalibrated.iHeight = m_iHeight;
	m_imgCalibrated.iStrideWidth[0] = m_iStride;

	return S_OK;
}

HRESULT CreatePretreatmentInstance( IPretreatment **ppPretreatment  ) 
{
	CPretreatmentImpl *pImpl;	

	if ( ppPretreatment == NULL ) return( E_POINTER );
	pImpl = new CPretreatmentImpl();
	if ( pImpl == NULL ) return( E_NOTIMPL );
	*ppPretreatment = pImpl;
	return( S_OK );
}
