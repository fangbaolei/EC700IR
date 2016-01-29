#ifndef _HV_EDMA3_H_
#define _HV_EDMA3_H_

HRESULT HV_dmasetup2D();
int HV_dmacpy2D(PBYTE8 pbDst, int iDstStride, PBYTE8 pbSrc, int iSrcStride, int iWidth, int iHeight);
HRESULT HV_dmawait2D(int iHandle);

#endif
