#include "gba-jpeg-decode.h"

#define JPEG_ConvertMCU_Limit(v) (v + 256)>>1

//by liaoy: 根据采样系数转换MUC块到缓存行
void ConvertMCU(	
				 JPEG_FIXED_TYPE* pYBlock,  //块缓存
				 JPEG_FIXED_TYPE* pCbBlock, 
				 JPEG_FIXED_TYPE* pCrBlock, 
				 int nYBlockCount,	//各分量的块数
				 int nCbBlockCount, 
				 int nCrBlockCount, 
				 PBYTE8 pYBuf,		//输出行缓存
				 PBYTE8 pCbBuf,
				 PBYTE8 pCrBuf,
				 int nYBufWidth,
				 int nCbBufWidth,
				 int nCrBufWidth,
				 int nBufHeight,
				 int nHorzFactor,		//采样系数
				 int nVertFactor
				 )
{
	RESTRICT_PBYTE8 pY0, pY1, pY2, pY3, pY4, pY5, pY6, pY7;  
	RESTRICT_PBYTE8 pCb0, pCb1, pCb2, pCb3, pCb4, pCb5, pCb6, pCb7; 
	RESTRICT_PBYTE8 pCr0, pCr1, pCr2, pCr3, pCr4, pCr5, pCr6, pCr7;

	int nMcuCount, nStep, nBlockStep, i;

	if( nHorzFactor == 2 && nVertFactor == 1) //H2V1
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;

		nMcuCount = nYBlockCount>>1;
		nStep = JPEG_DCTSIZE<<1;
		nBlockStep = JPEG_DCTSIZE2<<1;
		
		for( i = nMcuCount; i; i--, 
			pY0 += nStep, 
			pY1 += nStep,
			pY2 += nStep,
			pY3 += nStep,
			pYBlock +=  nBlockStep
			)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[1]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[2]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[3]); 
			pY0[4] = JPEG_ConvertMCU_Limit(pYBlock[4]);  
			pY0[5] = JPEG_ConvertMCU_Limit(pYBlock[5]);  
			pY0[6] = JPEG_ConvertMCU_Limit(pYBlock[6]); 
			pY0[7] = JPEG_ConvertMCU_Limit(pYBlock[7]);
			pY0[8] = JPEG_ConvertMCU_Limit(pYBlock[64]); 
			pY0[9] = JPEG_ConvertMCU_Limit(pYBlock[65]);  
			pY0[10] = JPEG_ConvertMCU_Limit(pYBlock[66]); 
			pY0[11] = JPEG_ConvertMCU_Limit(pYBlock[67]); 
			pY0[12] = JPEG_ConvertMCU_Limit(pYBlock[68]);  
			pY0[13] = JPEG_ConvertMCU_Limit(pYBlock[69]);  
			pY0[14] = JPEG_ConvertMCU_Limit(pYBlock[70]); 
			pY0[15] = JPEG_ConvertMCU_Limit(pYBlock[71]);

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[17]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[18]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[19]); 
			pY1[4] = JPEG_ConvertMCU_Limit(pYBlock[20]);  
			pY1[5] = JPEG_ConvertMCU_Limit(pYBlock[21]);  
			pY1[6] = JPEG_ConvertMCU_Limit(pYBlock[22]); 
			pY1[7] = JPEG_ConvertMCU_Limit(pYBlock[23]); 
			pY1[8] = JPEG_ConvertMCU_Limit(pYBlock[80]);  
			pY1[9] = JPEG_ConvertMCU_Limit(pYBlock[81]);  
			pY1[10] = JPEG_ConvertMCU_Limit(pYBlock[82]); 
			pY1[11] = JPEG_ConvertMCU_Limit(pYBlock[83]); 
			pY1[12] = JPEG_ConvertMCU_Limit(pYBlock[84]);  
			pY1[13] = JPEG_ConvertMCU_Limit(pYBlock[85]);  
			pY1[14] = JPEG_ConvertMCU_Limit(pYBlock[86]); 
			pY1[15] = JPEG_ConvertMCU_Limit(pYBlock[87]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[32]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[33]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[34]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[35]); 
			pY2[4] = JPEG_ConvertMCU_Limit(pYBlock[36]);  
			pY2[5] = JPEG_ConvertMCU_Limit(pYBlock[37]);  
			pY2[6] = JPEG_ConvertMCU_Limit(pYBlock[38]); 
			pY2[7] = JPEG_ConvertMCU_Limit(pYBlock[39]); 
			pY2[8] = JPEG_ConvertMCU_Limit(pYBlock[96]);  
			pY2[9] = JPEG_ConvertMCU_Limit(pYBlock[97]);  
			pY2[10] = JPEG_ConvertMCU_Limit(pYBlock[98]); 
			pY2[11] = JPEG_ConvertMCU_Limit(pYBlock[99]); 
			pY2[12] = JPEG_ConvertMCU_Limit(pYBlock[100]);  
			pY2[13] = JPEG_ConvertMCU_Limit(pYBlock[101]);  
			pY2[14] = JPEG_ConvertMCU_Limit(pYBlock[102]); 
			pY2[15] = JPEG_ConvertMCU_Limit(pYBlock[103]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[49]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[50]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[51]); 
			pY3[4] = JPEG_ConvertMCU_Limit(pYBlock[52]);  
			pY3[5] = JPEG_ConvertMCU_Limit(pYBlock[53]);  
			pY3[6] = JPEG_ConvertMCU_Limit(pYBlock[54]); 
			pY3[7] = JPEG_ConvertMCU_Limit(pYBlock[55]); 
			pY3[8] = JPEG_ConvertMCU_Limit(pYBlock[112]);  
			pY3[9] = JPEG_ConvertMCU_Limit(pYBlock[113]);  
			pY3[10] = JPEG_ConvertMCU_Limit(pYBlock[114]); 
			pY3[11] = JPEG_ConvertMCU_Limit(pYBlock[115]); 
			pY3[12] = JPEG_ConvertMCU_Limit(pYBlock[116]);  
			pY3[13] = JPEG_ConvertMCU_Limit(pYBlock[117]);  
			pY3[14] = JPEG_ConvertMCU_Limit(pYBlock[118]); 
			pY3[15] = JPEG_ConvertMCU_Limit(pYBlock[119]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += JPEG_DCTSIZE, 
			pCb1 += JPEG_DCTSIZE,
			pCb2 += JPEG_DCTSIZE,
			pCb3 += JPEG_DCTSIZE, 
			pCr0 += JPEG_DCTSIZE, 
			pCr1 += JPEG_DCTSIZE, 
			pCr2 += JPEG_DCTSIZE, 
			pCr3 += JPEG_DCTSIZE, 
			pCbBlock += JPEG_DCTSIZE2,
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[1]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[3]);
			pCb0[4] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[5] = JPEG_ConvertMCU_Limit(pCbBlock[5]);
			pCb0[6] = JPEG_ConvertMCU_Limit(pCbBlock[6]);
			pCb0[7] = JPEG_ConvertMCU_Limit(pCbBlock[7]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[17]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[19]);
			pCb1[4] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb1[5] = JPEG_ConvertMCU_Limit(pCbBlock[21]);
			pCb1[6] = JPEG_ConvertMCU_Limit(pCbBlock[22]);
			pCb1[7] = JPEG_ConvertMCU_Limit(pCbBlock[23]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[33]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[35]);
			pCb2[4] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb2[5] = JPEG_ConvertMCU_Limit(pCbBlock[37]);
			pCb2[6] = JPEG_ConvertMCU_Limit(pCbBlock[38]);
			pCb2[7] = JPEG_ConvertMCU_Limit(pCbBlock[39]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[49]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[50]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[51]); 
			pCb3[4] = JPEG_ConvertMCU_Limit(pCbBlock[52]);
			pCb3[5] = JPEG_ConvertMCU_Limit(pCbBlock[53]); 
			pCb3[6] = JPEG_ConvertMCU_Limit(pCbBlock[54]); 
			pCb3[7] = JPEG_ConvertMCU_Limit(pCbBlock[55]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]); 
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[1]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[2]); 
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[3]);
			pCr0[4] = JPEG_ConvertMCU_Limit(pCrBlock[4]); 
			pCr0[5] = JPEG_ConvertMCU_Limit(pCrBlock[5]);
			pCr0[6] = JPEG_ConvertMCU_Limit(pCrBlock[6]); 
			pCr0[7] = JPEG_ConvertMCU_Limit(pCrBlock[7]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]); 
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[17]);  
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[18]); 
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[19]); 
			pCr1[4] = JPEG_ConvertMCU_Limit(pCrBlock[20]); 
			pCr1[5] = JPEG_ConvertMCU_Limit(pCrBlock[21]);  
			pCr1[6] = JPEG_ConvertMCU_Limit(pCrBlock[22]); 
			pCr1[7] = JPEG_ConvertMCU_Limit(pCrBlock[23]); 

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]); 
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[33]); 
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[34]);  
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[35]);  
			pCr2[4] = JPEG_ConvertMCU_Limit(pCrBlock[36]); 
			pCr2[5] = JPEG_ConvertMCU_Limit(pCrBlock[37]); 
			pCr2[6] = JPEG_ConvertMCU_Limit(pCrBlock[38]);  
			pCr2[7] = JPEG_ConvertMCU_Limit(pCrBlock[39]);  

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]); 
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[49]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[50]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[51]);
			pCr3[4] = JPEG_ConvertMCU_Limit(pCrBlock[52]); 
			pCr3[5] = JPEG_ConvertMCU_Limit(pCrBlock[53]); 
			pCr3[6] = JPEG_ConvertMCU_Limit(pCrBlock[54]); 
			pCr3[7] = JPEG_ConvertMCU_Limit(pCrBlock[55]);
		}
	}
	else if( nHorzFactor == 1 && nVertFactor == 2) //H1V2
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;
		pY4 = pYBuf + nYBufWidth * 4;
		pY5 = pYBuf + nYBufWidth * 5;
		pY6 = pYBuf + nYBufWidth * 6;
		pY7 = pYBuf + nYBufWidth * 7;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;
		pCb4 = pCbBuf + nCbBufWidth * 4;
		pCb5 = pCbBuf + nCbBufWidth * 5;
		pCb6 = pCbBuf + nCbBufWidth * 6;
		pCb7 = pCbBuf + nCbBufWidth * 7;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;
		pCr4 = pCrBuf + nCrBufWidth * 4;
		pCr5 = pCrBuf + nCrBufWidth * 5;
		pCr6 = pCrBuf + nCrBufWidth * 6;
		pCr7 = pCrBuf + nCrBufWidth * 7;

		nMcuCount = nYBlockCount>>1;
		nStep = JPEG_DCTSIZE>>1;
	    nBlockStep = JPEG_DCTSIZE2<<1;

		for( i = nMcuCount; i; i--, 
			pY0 += JPEG_DCTSIZE, 
			pY1 += JPEG_DCTSIZE,
			pY2 += JPEG_DCTSIZE,
			pY3 += JPEG_DCTSIZE,
			pY4 += JPEG_DCTSIZE, 
			pY5 += JPEG_DCTSIZE,
			pY6 += JPEG_DCTSIZE,
			pY7 += JPEG_DCTSIZE,
			pYBlock += nBlockStep
			)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[1]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[2]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[3]); 
			pY0[4] = JPEG_ConvertMCU_Limit(pYBlock[4]);  
			pY0[5] = JPEG_ConvertMCU_Limit(pYBlock[5]);  
			pY0[6] = JPEG_ConvertMCU_Limit(pYBlock[6]); 
			pY0[7] = JPEG_ConvertMCU_Limit(pYBlock[7]);

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[17]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[18]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[19]); 
			pY1[4] = JPEG_ConvertMCU_Limit(pYBlock[20]);  
			pY1[5] = JPEG_ConvertMCU_Limit(pYBlock[21]);  
			pY1[6] = JPEG_ConvertMCU_Limit(pYBlock[22]); 
			pY1[7] = JPEG_ConvertMCU_Limit(pYBlock[23]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[32]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[33]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[34]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[35]); 
			pY2[4] = JPEG_ConvertMCU_Limit(pYBlock[36]);  
			pY2[5] = JPEG_ConvertMCU_Limit(pYBlock[37]);  
			pY2[6] = JPEG_ConvertMCU_Limit(pYBlock[38]); 
			pY2[7] = JPEG_ConvertMCU_Limit(pYBlock[39]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[49]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[50]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[51]); 
			pY3[4] = JPEG_ConvertMCU_Limit(pYBlock[52]);  
			pY3[5] = JPEG_ConvertMCU_Limit(pYBlock[53]);  
			pY3[6] = JPEG_ConvertMCU_Limit(pYBlock[54]); 
			pY3[7] = JPEG_ConvertMCU_Limit(pYBlock[55]); 

			pY4[0] = JPEG_ConvertMCU_Limit(pYBlock[64]); 
			pY4[1] = JPEG_ConvertMCU_Limit(pYBlock[65]);  
			pY4[2] = JPEG_ConvertMCU_Limit(pYBlock[66]); 
			pY4[3] = JPEG_ConvertMCU_Limit(pYBlock[67]); 
			pY4[4] = JPEG_ConvertMCU_Limit(pYBlock[68]);  
			pY4[5] = JPEG_ConvertMCU_Limit(pYBlock[69]);  
			pY4[6] = JPEG_ConvertMCU_Limit(pYBlock[70]); 
			pY4[7] = JPEG_ConvertMCU_Limit(pYBlock[71]);

			pY5[0] = JPEG_ConvertMCU_Limit(pYBlock[80]);  
			pY5[1] = JPEG_ConvertMCU_Limit(pYBlock[81]);  
			pY5[2] = JPEG_ConvertMCU_Limit(pYBlock[82]); 
			pY5[3] = JPEG_ConvertMCU_Limit(pYBlock[83]); 
			pY5[4] = JPEG_ConvertMCU_Limit(pYBlock[84]);  
			pY5[5] = JPEG_ConvertMCU_Limit(pYBlock[85]);  
			pY5[6] = JPEG_ConvertMCU_Limit(pYBlock[86]); 
			pY5[7] = JPEG_ConvertMCU_Limit(pYBlock[87]); 

			pY6[0] = JPEG_ConvertMCU_Limit(pYBlock[96]);  
			pY6[1] = JPEG_ConvertMCU_Limit(pYBlock[97]);  
			pY6[2] = JPEG_ConvertMCU_Limit(pYBlock[98]); 
			pY6[3] = JPEG_ConvertMCU_Limit(pYBlock[99]); 
			pY6[4] = JPEG_ConvertMCU_Limit(pYBlock[100]);  
			pY6[5] = JPEG_ConvertMCU_Limit(pYBlock[101]);  
			pY6[6] = JPEG_ConvertMCU_Limit(pYBlock[102]); 
			pY6[7] = JPEG_ConvertMCU_Limit(pYBlock[103]); 

			pY7[0] = JPEG_ConvertMCU_Limit(pYBlock[112]);  
			pY7[1] = JPEG_ConvertMCU_Limit(pYBlock[113]);  
			pY7[2] = JPEG_ConvertMCU_Limit(pYBlock[114]); 
			pY7[3] = JPEG_ConvertMCU_Limit(pYBlock[115]); 
			pY7[4] = JPEG_ConvertMCU_Limit(pYBlock[116]);  
			pY7[5] = JPEG_ConvertMCU_Limit(pYBlock[117]);  
			pY7[6] = JPEG_ConvertMCU_Limit(pYBlock[118]); 
			pY7[7] = JPEG_ConvertMCU_Limit(pYBlock[119]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += nStep, 
			pCb1 += nStep,
			pCb2 += nStep,
			pCb3 += nStep,
			pCb4 += nStep, 
			pCb5 += nStep,
			pCb6 += nStep,
			pCb7 += nStep,
			pCr0 += nStep, 
			pCr1 += nStep, 
			pCr2 += nStep, 
			pCr3 += nStep, 
			pCr4 += nStep, 
			pCr5 += nStep, 
			pCr6 += nStep, 
			pCr7 += nStep, 
			pCbBlock += JPEG_DCTSIZE2, 
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[6]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[8]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[10]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[12]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[14]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[22]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[24]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[26]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[28]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[30]); 

			pCb4[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb4[1] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb4[2] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb4[3] = JPEG_ConvertMCU_Limit(pCbBlock[38]);

			pCb5[0] = JPEG_ConvertMCU_Limit(pCbBlock[40]);
			pCb5[1] = JPEG_ConvertMCU_Limit(pCbBlock[42]);
			pCb5[2] = JPEG_ConvertMCU_Limit(pCbBlock[44]);
			pCb5[3] = JPEG_ConvertMCU_Limit(pCbBlock[46]);

			pCb6[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb6[1] = JPEG_ConvertMCU_Limit(pCbBlock[50]);
			pCb6[2] = JPEG_ConvertMCU_Limit(pCbBlock[52]);
			pCb6[3] = JPEG_ConvertMCU_Limit(pCbBlock[54]);

			pCb7[0] = JPEG_ConvertMCU_Limit(pCbBlock[56]);
			pCb7[1] = JPEG_ConvertMCU_Limit(pCbBlock[58]); 
			pCb7[2] = JPEG_ConvertMCU_Limit(pCbBlock[60]); 
			pCb7[3] = JPEG_ConvertMCU_Limit(pCbBlock[62]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]); 
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[2]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[4]); 
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[6]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[8]); 
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[10]);  
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[12]); 
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[14]); 

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]); 
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[18]); 
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[20]);  
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[22]);  

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[24]); 
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[26]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[28]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[30]);

			pCr4[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]); 
			pCr4[1] = JPEG_ConvertMCU_Limit(pCrBlock[34]);
			pCr4[2] = JPEG_ConvertMCU_Limit(pCrBlock[36]); 
			pCr4[3] = JPEG_ConvertMCU_Limit(pCrBlock[38]);

			pCr5[0] = JPEG_ConvertMCU_Limit(pCrBlock[40]); 
			pCr5[1] = JPEG_ConvertMCU_Limit(pCrBlock[42]);  
			pCr5[2] = JPEG_ConvertMCU_Limit(pCrBlock[44]); 
			pCr5[3] = JPEG_ConvertMCU_Limit(pCrBlock[46]); 

			pCr6[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]); 
			pCr6[1] = JPEG_ConvertMCU_Limit(pCrBlock[50]); 
			pCr6[2] = JPEG_ConvertMCU_Limit(pCrBlock[52]);  
			pCr6[3] = JPEG_ConvertMCU_Limit(pCrBlock[54]);  

			pCr7[0] = JPEG_ConvertMCU_Limit(pCrBlock[56]); 
			pCr7[1] = JPEG_ConvertMCU_Limit(pCrBlock[58]); 
			pCr7[2] = JPEG_ConvertMCU_Limit(pCrBlock[60]); 
			pCr7[3] = JPEG_ConvertMCU_Limit(pCrBlock[62]);
		}
	}
	else if ( nHorzFactor == 2 && nVertFactor == 2) //H2V2
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;
		pY4 = pYBuf + nYBufWidth * 4;
		pY5 = pYBuf + nYBufWidth * 5;
		pY6 = pYBuf + nYBufWidth * 6;
		pY7 = pYBuf + nYBufWidth * 7;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;
		pCb4 = pCbBuf + nCbBufWidth * 4;
		pCb5 = pCbBuf + nCbBufWidth * 5;
		pCb6 = pCbBuf + nCbBufWidth * 6;
		pCb7 = pCbBuf + nCbBufWidth * 7;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;
		pCr4 = pCrBuf + nCrBufWidth * 4;
		pCr5 = pCrBuf + nCrBufWidth * 5;
		pCr6 = pCrBuf + nCrBufWidth * 6;
		pCr7 = pCrBuf + nCrBufWidth * 7;

		nMcuCount = nYBlockCount>>2;
		nStep = JPEG_DCTSIZE<<1;
		nBlockStep = JPEG_DCTSIZE2<<2;

		for( i = nMcuCount; i; i--, 
			pY0 += nStep, 
			pY1 += nStep,
			pY2 += nStep,
			pY3 += nStep,
			pY4 += nStep,
			pY5 += nStep,
			pY6 += nStep,
			pY7 += nStep,
			pYBlock += nBlockStep
		)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[1]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[2]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[3]); 
			pY0[4] = JPEG_ConvertMCU_Limit(pYBlock[4]);  
			pY0[5] = JPEG_ConvertMCU_Limit(pYBlock[5]);  
			pY0[6] = JPEG_ConvertMCU_Limit(pYBlock[6]); 
			pY0[7] = JPEG_ConvertMCU_Limit(pYBlock[7]);
			pY0[8] = JPEG_ConvertMCU_Limit(pYBlock[64]); 
			pY0[9] = JPEG_ConvertMCU_Limit(pYBlock[65]);  
			pY0[10] = JPEG_ConvertMCU_Limit(pYBlock[66]); 
			pY0[11] = JPEG_ConvertMCU_Limit(pYBlock[67]); 
			pY0[12] = JPEG_ConvertMCU_Limit(pYBlock[68]);  
			pY0[13] = JPEG_ConvertMCU_Limit(pYBlock[69]);  
			pY0[14] = JPEG_ConvertMCU_Limit(pYBlock[70]); 
			pY0[15] = JPEG_ConvertMCU_Limit(pYBlock[71]);

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[17]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[18]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[19]); 
			pY1[4] = JPEG_ConvertMCU_Limit(pYBlock[20]);  
			pY1[5] = JPEG_ConvertMCU_Limit(pYBlock[21]);  
			pY1[6] = JPEG_ConvertMCU_Limit(pYBlock[22]); 
			pY1[7] = JPEG_ConvertMCU_Limit(pYBlock[23]); 
			pY1[8] = JPEG_ConvertMCU_Limit(pYBlock[80]);  
			pY1[9] = JPEG_ConvertMCU_Limit(pYBlock[81]);  
			pY1[10] = JPEG_ConvertMCU_Limit(pYBlock[82]); 
			pY1[11] = JPEG_ConvertMCU_Limit(pYBlock[83]); 
			pY1[12] = JPEG_ConvertMCU_Limit(pYBlock[84]);  
			pY1[13] = JPEG_ConvertMCU_Limit(pYBlock[85]);  
			pY1[14] = JPEG_ConvertMCU_Limit(pYBlock[86]); 
			pY1[15] = JPEG_ConvertMCU_Limit(pYBlock[87]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[32]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[33]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[34]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[35]); 
			pY2[4] = JPEG_ConvertMCU_Limit(pYBlock[36]);  
			pY2[5] = JPEG_ConvertMCU_Limit(pYBlock[37]);  
			pY2[6] = JPEG_ConvertMCU_Limit(pYBlock[38]); 
			pY2[7] = JPEG_ConvertMCU_Limit(pYBlock[39]); 
			pY2[8] = JPEG_ConvertMCU_Limit(pYBlock[96]);  
			pY2[9] = JPEG_ConvertMCU_Limit(pYBlock[97]);  
			pY2[10] = JPEG_ConvertMCU_Limit(pYBlock[98]); 
			pY2[11] = JPEG_ConvertMCU_Limit(pYBlock[99]); 
			pY2[12] = JPEG_ConvertMCU_Limit(pYBlock[100]);  
			pY2[13] = JPEG_ConvertMCU_Limit(pYBlock[101]);  
			pY2[14] = JPEG_ConvertMCU_Limit(pYBlock[102]); 
			pY2[15] = JPEG_ConvertMCU_Limit(pYBlock[103]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[49]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[50]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[51]); 
			pY3[4] = JPEG_ConvertMCU_Limit(pYBlock[52]);  
			pY3[5] = JPEG_ConvertMCU_Limit(pYBlock[53]);  
			pY3[6] = JPEG_ConvertMCU_Limit(pYBlock[54]); 
			pY3[7] = JPEG_ConvertMCU_Limit(pYBlock[55]); 
			pY3[8] = JPEG_ConvertMCU_Limit(pYBlock[112]);  
			pY3[9] = JPEG_ConvertMCU_Limit(pYBlock[113]);  
			pY3[10] = JPEG_ConvertMCU_Limit(pYBlock[114]); 
			pY3[11] = JPEG_ConvertMCU_Limit(pYBlock[115]); 
			pY3[12] = JPEG_ConvertMCU_Limit(pYBlock[116]);  
			pY3[13] = JPEG_ConvertMCU_Limit(pYBlock[117]);  
			pY3[14] = JPEG_ConvertMCU_Limit(pYBlock[118]); 
			pY3[15] = JPEG_ConvertMCU_Limit(pYBlock[119]); 

			pY4[0] = JPEG_ConvertMCU_Limit(pYBlock[128]); 
			pY4[1] = JPEG_ConvertMCU_Limit(pYBlock[129]);  
			pY4[2] = JPEG_ConvertMCU_Limit(pYBlock[130]); 
			pY4[3] = JPEG_ConvertMCU_Limit(pYBlock[131]); 
			pY4[4] = JPEG_ConvertMCU_Limit(pYBlock[132]);  
			pY4[5] = JPEG_ConvertMCU_Limit(pYBlock[133]);  
			pY4[6] = JPEG_ConvertMCU_Limit(pYBlock[134]); 
			pY4[7] = JPEG_ConvertMCU_Limit(pYBlock[135]);
			pY4[8] = JPEG_ConvertMCU_Limit(pYBlock[192]); 
			pY4[9] = JPEG_ConvertMCU_Limit(pYBlock[193]);  
			pY4[10] = JPEG_ConvertMCU_Limit(pYBlock[194]); 
			pY4[11] = JPEG_ConvertMCU_Limit(pYBlock[195]); 
			pY4[12] = JPEG_ConvertMCU_Limit(pYBlock[196]);  
			pY4[13] = JPEG_ConvertMCU_Limit(pYBlock[197]);  
			pY4[14] = JPEG_ConvertMCU_Limit(pYBlock[198]); 
			pY4[15] = JPEG_ConvertMCU_Limit(pYBlock[199]);

			pY5[0] = JPEG_ConvertMCU_Limit(pYBlock[144]);  
			pY5[1] = JPEG_ConvertMCU_Limit(pYBlock[145]);  
			pY5[2] = JPEG_ConvertMCU_Limit(pYBlock[146]); 
			pY5[3] = JPEG_ConvertMCU_Limit(pYBlock[147]); 
			pY5[4] = JPEG_ConvertMCU_Limit(pYBlock[148]);  
			pY5[5] = JPEG_ConvertMCU_Limit(pYBlock[149]);  
			pY5[6] = JPEG_ConvertMCU_Limit(pYBlock[150]); 
			pY5[7] = JPEG_ConvertMCU_Limit(pYBlock[151]); 
			pY5[8] = JPEG_ConvertMCU_Limit(pYBlock[208]);  
			pY5[9] = JPEG_ConvertMCU_Limit(pYBlock[209]);  
			pY5[10] = JPEG_ConvertMCU_Limit(pYBlock[210]); 
			pY5[11] = JPEG_ConvertMCU_Limit(pYBlock[211]); 
			pY5[12] = JPEG_ConvertMCU_Limit(pYBlock[212]);  
			pY5[13] = JPEG_ConvertMCU_Limit(pYBlock[213]);  
			pY5[14] = JPEG_ConvertMCU_Limit(pYBlock[214]); 
			pY5[15] = JPEG_ConvertMCU_Limit(pYBlock[215]); 

			pY6[0] = JPEG_ConvertMCU_Limit(pYBlock[160]);  
			pY6[1] = JPEG_ConvertMCU_Limit(pYBlock[161]);  
			pY6[2] = JPEG_ConvertMCU_Limit(pYBlock[162]); 
			pY6[3] = JPEG_ConvertMCU_Limit(pYBlock[163]); 
			pY6[4] = JPEG_ConvertMCU_Limit(pYBlock[164]);  
			pY6[5] = JPEG_ConvertMCU_Limit(pYBlock[165]);  
			pY6[6] = JPEG_ConvertMCU_Limit(pYBlock[166]); 
			pY6[7] = JPEG_ConvertMCU_Limit(pYBlock[167]); 
			pY6[8] = JPEG_ConvertMCU_Limit(pYBlock[224]);  
			pY6[9] = JPEG_ConvertMCU_Limit(pYBlock[225]);  
			pY6[10] = JPEG_ConvertMCU_Limit(pYBlock[226]); 
			pY6[11] = JPEG_ConvertMCU_Limit(pYBlock[227]); 
			pY6[12] = JPEG_ConvertMCU_Limit(pYBlock[228]);  
			pY6[13] = JPEG_ConvertMCU_Limit(pYBlock[229]);  
			pY6[14] = JPEG_ConvertMCU_Limit(pYBlock[230]); 
			pY6[15] = JPEG_ConvertMCU_Limit(pYBlock[231]); 

			pY7[0] = JPEG_ConvertMCU_Limit(pYBlock[176]);  
			pY7[1] = JPEG_ConvertMCU_Limit(pYBlock[177]);  
			pY7[2] = JPEG_ConvertMCU_Limit(pYBlock[178]); 
			pY7[3] = JPEG_ConvertMCU_Limit(pYBlock[179]); 
			pY7[4] = JPEG_ConvertMCU_Limit(pYBlock[180]);  
			pY7[5] = JPEG_ConvertMCU_Limit(pYBlock[181]);  
			pY7[6] = JPEG_ConvertMCU_Limit(pYBlock[182]); 
			pY7[7] = JPEG_ConvertMCU_Limit(pYBlock[183]); 
			pY7[8] = JPEG_ConvertMCU_Limit(pYBlock[240]);  
			pY7[9] = JPEG_ConvertMCU_Limit(pYBlock[241]);  
			pY7[10] = JPEG_ConvertMCU_Limit(pYBlock[242]); 
			pY7[11] = JPEG_ConvertMCU_Limit(pYBlock[243]); 
			pY7[12] = JPEG_ConvertMCU_Limit(pYBlock[244]);  
			pY7[13] = JPEG_ConvertMCU_Limit(pYBlock[245]);  
			pY7[14] = JPEG_ConvertMCU_Limit(pYBlock[246]); 
			pY7[15] = JPEG_ConvertMCU_Limit(pYBlock[247]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += JPEG_DCTSIZE, 
			pCb1 += JPEG_DCTSIZE,
			pCb2 += JPEG_DCTSIZE,
			pCb3 += JPEG_DCTSIZE,
			pCb4 += JPEG_DCTSIZE, 
			pCb5 += JPEG_DCTSIZE,
			pCb6 += JPEG_DCTSIZE,
			pCb7 += JPEG_DCTSIZE,
			pCr0 += JPEG_DCTSIZE, 
			pCr1 += JPEG_DCTSIZE, 
			pCr2 += JPEG_DCTSIZE, 
			pCr3 += JPEG_DCTSIZE, 
			pCr4 += JPEG_DCTSIZE, 
			pCr5 += JPEG_DCTSIZE, 
			pCr6 += JPEG_DCTSIZE, 
			pCr7 += JPEG_DCTSIZE, 
			pCbBlock += JPEG_DCTSIZE2, 
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[1]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[3]);
			pCb0[4] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[5] = JPEG_ConvertMCU_Limit(pCbBlock[5]);
			pCb0[6] = JPEG_ConvertMCU_Limit(pCbBlock[6]);
			pCb0[7] = JPEG_ConvertMCU_Limit(pCbBlock[7]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[8]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[9]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[10]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[11]);
			pCb1[4] = JPEG_ConvertMCU_Limit(pCbBlock[12]);
			pCb1[5] = JPEG_ConvertMCU_Limit(pCbBlock[13]);
			pCb1[6] = JPEG_ConvertMCU_Limit(pCbBlock[14]);
			pCb1[7] = JPEG_ConvertMCU_Limit(pCbBlock[15]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[17]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[19]);
			pCb2[4] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb2[5] = JPEG_ConvertMCU_Limit(pCbBlock[21]);
			pCb2[6] = JPEG_ConvertMCU_Limit(pCbBlock[22]);
			pCb2[7] = JPEG_ConvertMCU_Limit(pCbBlock[23]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[24]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[25]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[26]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[27]); 
			pCb3[4] = JPEG_ConvertMCU_Limit(pCbBlock[28]);
			pCb3[5] = JPEG_ConvertMCU_Limit(pCbBlock[29]); 
			pCb3[6] = JPEG_ConvertMCU_Limit(pCbBlock[30]); 
			pCb3[7] = JPEG_ConvertMCU_Limit(pCbBlock[31]); 

			pCb4[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb4[1] = JPEG_ConvertMCU_Limit(pCbBlock[33]);
			pCb4[2] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb4[3] = JPEG_ConvertMCU_Limit(pCbBlock[35]);
			pCb4[4] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb4[5] = JPEG_ConvertMCU_Limit(pCbBlock[37]);
			pCb4[6] = JPEG_ConvertMCU_Limit(pCbBlock[38]);
			pCb4[7] = JPEG_ConvertMCU_Limit(pCbBlock[39]);

			pCb5[0] = JPEG_ConvertMCU_Limit(pCbBlock[40]);
			pCb5[1] = JPEG_ConvertMCU_Limit(pCbBlock[41]);
			pCb5[2] = JPEG_ConvertMCU_Limit(pCbBlock[42]);
			pCb5[3] = JPEG_ConvertMCU_Limit(pCbBlock[43]);
			pCb5[4] = JPEG_ConvertMCU_Limit(pCbBlock[44]);
			pCb5[5] = JPEG_ConvertMCU_Limit(pCbBlock[45]);
			pCb5[6] = JPEG_ConvertMCU_Limit(pCbBlock[46]);
			pCb5[7] = JPEG_ConvertMCU_Limit(pCbBlock[47]);

			pCb6[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb6[1] = JPEG_ConvertMCU_Limit(pCbBlock[49]);
			pCb6[2] = JPEG_ConvertMCU_Limit(pCbBlock[50]);
			pCb6[3] = JPEG_ConvertMCU_Limit(pCbBlock[51]);
			pCb6[4] = JPEG_ConvertMCU_Limit(pCbBlock[52]);
			pCb6[5] = JPEG_ConvertMCU_Limit(pCbBlock[53]);
			pCb6[6] = JPEG_ConvertMCU_Limit(pCbBlock[54]);
			pCb6[7] = JPEG_ConvertMCU_Limit(pCbBlock[55]);

			pCb7[0] = JPEG_ConvertMCU_Limit(pCbBlock[56]);
			pCb7[1] = JPEG_ConvertMCU_Limit(pCbBlock[57]); 
			pCb7[2] = JPEG_ConvertMCU_Limit(pCbBlock[58]); 
			pCb7[3] = JPEG_ConvertMCU_Limit(pCbBlock[59]); 
			pCb7[4] = JPEG_ConvertMCU_Limit(pCbBlock[60]);
			pCb7[5] = JPEG_ConvertMCU_Limit(pCbBlock[61]); 
			pCb7[6] = JPEG_ConvertMCU_Limit(pCbBlock[62]); 
			pCb7[7] = JPEG_ConvertMCU_Limit(pCbBlock[63]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]);
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[1]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[2]);
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[3]);
			pCr0[4] = JPEG_ConvertMCU_Limit(pCrBlock[4]);
			pCr0[5] = JPEG_ConvertMCU_Limit(pCrBlock[5]);
			pCr0[6] = JPEG_ConvertMCU_Limit(pCrBlock[6]);
			pCr0[7] = JPEG_ConvertMCU_Limit(pCrBlock[7]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[8]);
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[9]);
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[10]);
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[11]);
			pCr1[4] = JPEG_ConvertMCU_Limit(pCrBlock[12]);
			pCr1[5] = JPEG_ConvertMCU_Limit(pCrBlock[13]);
			pCr1[6] = JPEG_ConvertMCU_Limit(pCrBlock[14]);
			pCr1[7] = JPEG_ConvertMCU_Limit(pCrBlock[15]);

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]);
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[17]);
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[18]);
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[19]);
			pCr2[4] = JPEG_ConvertMCU_Limit(pCrBlock[20]);
			pCr2[5] = JPEG_ConvertMCU_Limit(pCrBlock[21]);
			pCr2[6] = JPEG_ConvertMCU_Limit(pCrBlock[22]);
			pCr2[7] = JPEG_ConvertMCU_Limit(pCrBlock[23]);

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[24]);
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[25]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[26]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[27]); 
			pCr3[4] = JPEG_ConvertMCU_Limit(pCrBlock[28]);
			pCr3[5] = JPEG_ConvertMCU_Limit(pCrBlock[29]); 
			pCr3[6] = JPEG_ConvertMCU_Limit(pCrBlock[30]); 
			pCr3[7] = JPEG_ConvertMCU_Limit(pCrBlock[31]); 

			pCr4[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]);
			pCr4[1] = JPEG_ConvertMCU_Limit(pCrBlock[33]);
			pCr4[2] = JPEG_ConvertMCU_Limit(pCrBlock[34]);
			pCr4[3] = JPEG_ConvertMCU_Limit(pCrBlock[35]);
			pCr4[4] = JPEG_ConvertMCU_Limit(pCrBlock[36]);
			pCr4[5] = JPEG_ConvertMCU_Limit(pCrBlock[37]);
			pCr4[6] = JPEG_ConvertMCU_Limit(pCrBlock[38]);
			pCr4[7] = JPEG_ConvertMCU_Limit(pCrBlock[39]);

			pCr5[0] = JPEG_ConvertMCU_Limit(pCrBlock[40]);
			pCr5[1] = JPEG_ConvertMCU_Limit(pCrBlock[41]);
			pCr5[2] = JPEG_ConvertMCU_Limit(pCrBlock[42]);
			pCr5[3] = JPEG_ConvertMCU_Limit(pCrBlock[43]);
			pCr5[4] = JPEG_ConvertMCU_Limit(pCrBlock[44]);
			pCr5[5] = JPEG_ConvertMCU_Limit(pCrBlock[45]);
			pCr5[6] = JPEG_ConvertMCU_Limit(pCrBlock[46]);
			pCr5[7] = JPEG_ConvertMCU_Limit(pCrBlock[47]);

			pCr6[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]);
			pCr6[1] = JPEG_ConvertMCU_Limit(pCrBlock[49]);
			pCr6[2] = JPEG_ConvertMCU_Limit(pCrBlock[50]);
			pCr6[3] = JPEG_ConvertMCU_Limit(pCrBlock[51]);
			pCr6[4] = JPEG_ConvertMCU_Limit(pCrBlock[52]);
			pCr6[5] = JPEG_ConvertMCU_Limit(pCrBlock[53]);
			pCr6[6] = JPEG_ConvertMCU_Limit(pCrBlock[54]);
			pCr6[7] = JPEG_ConvertMCU_Limit(pCrBlock[55]);

			pCr7[0] = JPEG_ConvertMCU_Limit(pCrBlock[56]);
			pCr7[1] = JPEG_ConvertMCU_Limit(pCrBlock[57]); 
			pCr7[2] = JPEG_ConvertMCU_Limit(pCrBlock[58]); 
			pCr7[3] = JPEG_ConvertMCU_Limit(pCrBlock[59]); 
			pCr7[4] = JPEG_ConvertMCU_Limit(pCrBlock[60]);
			pCr7[5] = JPEG_ConvertMCU_Limit(pCrBlock[61]); 
			pCr7[6] = JPEG_ConvertMCU_Limit(pCrBlock[62]); 
			pCr7[7] = JPEG_ConvertMCU_Limit(pCrBlock[63]); 
		}
	}
	else if( nHorzFactor == 1 && nVertFactor == 1 ) //H1V1
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;

		nStep = JPEG_DCTSIZE>>1;

		for( i = nYBlockCount; i; i--, 
				pY0 += JPEG_DCTSIZE, 
				pY1 += JPEG_DCTSIZE,
				pY2 += JPEG_DCTSIZE,
				pY3 += JPEG_DCTSIZE,
				pYBlock += JPEG_DCTSIZE2 
				)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[1]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[2]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[3]); 
			pY0[4] = JPEG_ConvertMCU_Limit(pYBlock[4]);  
			pY0[5] = JPEG_ConvertMCU_Limit(pYBlock[5]);  
			pY0[6] = JPEG_ConvertMCU_Limit(pYBlock[6]); 
			pY0[7] = JPEG_ConvertMCU_Limit(pYBlock[7]); 

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[17]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[18]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[19]); 
			pY1[4] = JPEG_ConvertMCU_Limit(pYBlock[20]);  
			pY1[5] = JPEG_ConvertMCU_Limit(pYBlock[21]);  
			pY1[6] = JPEG_ConvertMCU_Limit(pYBlock[22]); 
			pY1[7] = JPEG_ConvertMCU_Limit(pYBlock[23]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[32]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[33]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[34]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[35]); 
			pY2[4] = JPEG_ConvertMCU_Limit(pYBlock[36]);  
			pY2[5] = JPEG_ConvertMCU_Limit(pYBlock[37]);  
			pY2[6] = JPEG_ConvertMCU_Limit(pYBlock[38]); 
			pY2[7] = JPEG_ConvertMCU_Limit(pYBlock[39]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[49]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[50]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[51]); 
			pY3[4] = JPEG_ConvertMCU_Limit(pYBlock[52]);  
			pY3[5] = JPEG_ConvertMCU_Limit(pYBlock[53]);  
			pY3[6] = JPEG_ConvertMCU_Limit(pYBlock[54]); 
			pY3[7] = JPEG_ConvertMCU_Limit(pYBlock[55]); 
		}

		for( i = nCbBlockCount; i; i--, 
				pCb0 += nStep, 
				pCb1 += nStep,
				pCb2 += nStep,
				pCb3 += nStep,
				pCr0 += nStep, 
				pCr1 += nStep, 
				pCr2 += nStep, 
				pCr3 += nStep, 
				pCbBlock += JPEG_DCTSIZE2, 
				pCrBlock += JPEG_DCTSIZE2
				)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[6]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[22]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[38]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[50]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[52]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[54]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]); 
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[2]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[4]); 
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[6]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]); 
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[18]);  
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[20]); 
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[22]); 

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]); 
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[34]); 
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[36]);  
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[38]);  

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]); 
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[50]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[52]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[54]);
		}
	}
}

////////////////////////////////////////////////////////////////
//逆时针旋转用
void ConvertMCU_LT(	
				JPEG_FIXED_TYPE* pYBlock,  //块缓存
				JPEG_FIXED_TYPE* pCbBlock, 
				JPEG_FIXED_TYPE* pCrBlock, 
				int nYBlockCount,	//各分量的块数
				int nCbBlockCount, 
				int nCrBlockCount, 
				PBYTE8 pYBuf,		//输出行缓存
				PBYTE8 pCbBuf,
				PBYTE8 pCrBuf,
				int nYBufWidth,
				int nCbBufWidth,
				int nCrBufWidth,
				int nBufHeight,
				int nHorzFactor,		//采样系数
				int nVertFactor
				)
{
	RESTRICT_PBYTE8 pY0, pY1, pY2, pY3, pY4, pY5, pY6, pY7,pY8,pY9,pY10,pY11,pY12,pY13,pY14,pY15;  
	RESTRICT_PBYTE8 pCb0, pCb1, pCb2, pCb3, pCb4, pCb5, pCb6, pCb7; 
	RESTRICT_PBYTE8 pCr0, pCr1, pCr2, pCr3, pCr4, pCr5, pCr6, pCr7;

	int nMcuCount, nStep, nBlockStep, i;

	if( nHorzFactor == 2 && nVertFactor == 1) //H2V1
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;
		pY4 = pYBuf + nYBufWidth * 4;
		pY5 = pYBuf + nYBufWidth * 5;
		pY6 = pYBuf + nYBufWidth * 6;
		pY7 = pYBuf + nYBufWidth * 7;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;

		nMcuCount = nYBlockCount>>1;
		nBlockStep = JPEG_DCTSIZE2<<1;

		for( i = nMcuCount; i; i--, 
			pY0 += JPEG_DCTSIZE, 
			pY1 += JPEG_DCTSIZE,
			pY2 += JPEG_DCTSIZE,
			pY3 += JPEG_DCTSIZE,
			pY4 += JPEG_DCTSIZE,
			pY5 += JPEG_DCTSIZE,
			pY6 += JPEG_DCTSIZE,
			pY7 += JPEG_DCTSIZE,
			pYBlock += nBlockStep
			)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[2]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[4]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[6]); 
			pY0[4] = JPEG_ConvertMCU_Limit(pYBlock[64]);  
			pY0[5] = JPEG_ConvertMCU_Limit(pYBlock[66]);  
			pY0[6] = JPEG_ConvertMCU_Limit(pYBlock[68]); 
			pY0[7] = JPEG_ConvertMCU_Limit(pYBlock[70]);

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[8]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[10]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[12]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[14]); 
			pY1[4] = JPEG_ConvertMCU_Limit(pYBlock[72]);  
			pY1[5] = JPEG_ConvertMCU_Limit(pYBlock[74]);  
			pY1[6] = JPEG_ConvertMCU_Limit(pYBlock[76]); 
			pY1[7] = JPEG_ConvertMCU_Limit(pYBlock[78]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[18]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[20]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[22]); 
			pY2[4] = JPEG_ConvertMCU_Limit(pYBlock[80]);  
			pY2[5] = JPEG_ConvertMCU_Limit(pYBlock[82]);  
			pY2[6] = JPEG_ConvertMCU_Limit(pYBlock[84]); 
			pY2[7] = JPEG_ConvertMCU_Limit(pYBlock[86]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[24]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[26]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[28]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[30]); 
			pY3[4] = JPEG_ConvertMCU_Limit(pYBlock[88]);  
			pY3[5] = JPEG_ConvertMCU_Limit(pYBlock[90]);  
			pY3[6] = JPEG_ConvertMCU_Limit(pYBlock[92]); 
			pY3[7] = JPEG_ConvertMCU_Limit(pYBlock[94]); 

			pY4[0] = JPEG_ConvertMCU_Limit(pYBlock[32]);  
			pY4[1] = JPEG_ConvertMCU_Limit(pYBlock[34]);  
			pY4[2] = JPEG_ConvertMCU_Limit(pYBlock[36]); 
			pY4[3] = JPEG_ConvertMCU_Limit(pYBlock[38]); 
			pY4[4] = JPEG_ConvertMCU_Limit(pYBlock[96]);  
			pY4[5] = JPEG_ConvertMCU_Limit(pYBlock[98]);  
			pY4[6] = JPEG_ConvertMCU_Limit(pYBlock[100]); 
			pY4[7] = JPEG_ConvertMCU_Limit(pYBlock[102]); 

			pY5[0] = JPEG_ConvertMCU_Limit(pYBlock[40]);  
			pY5[1] = JPEG_ConvertMCU_Limit(pYBlock[42]);  
			pY5[2] = JPEG_ConvertMCU_Limit(pYBlock[44]); 
			pY5[3] = JPEG_ConvertMCU_Limit(pYBlock[46]); 
			pY5[4] = JPEG_ConvertMCU_Limit(pYBlock[104]);  
			pY5[5] = JPEG_ConvertMCU_Limit(pYBlock[106]);  
			pY5[6] = JPEG_ConvertMCU_Limit(pYBlock[108]); 
			pY5[7] = JPEG_ConvertMCU_Limit(pYBlock[110]); 

			pY6[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY6[1] = JPEG_ConvertMCU_Limit(pYBlock[50]);  
			pY6[2] = JPEG_ConvertMCU_Limit(pYBlock[52]); 
			pY6[3] = JPEG_ConvertMCU_Limit(pYBlock[54]); 
			pY6[4] = JPEG_ConvertMCU_Limit(pYBlock[112]);  
			pY6[5] = JPEG_ConvertMCU_Limit(pYBlock[114]);  
			pY6[6] = JPEG_ConvertMCU_Limit(pYBlock[116]); 
			pY6[7] = JPEG_ConvertMCU_Limit(pYBlock[118]); 

			pY7[0] = JPEG_ConvertMCU_Limit(pYBlock[56]);  
			pY7[1] = JPEG_ConvertMCU_Limit(pYBlock[58]);  
			pY7[2] = JPEG_ConvertMCU_Limit(pYBlock[60]); 
			pY7[3] = JPEG_ConvertMCU_Limit(pYBlock[62]); 
			pY7[4] = JPEG_ConvertMCU_Limit(pYBlock[120]);  
			pY7[5] = JPEG_ConvertMCU_Limit(pYBlock[122]);  
			pY7[6] = JPEG_ConvertMCU_Limit(pYBlock[124]); 
			pY7[7] = JPEG_ConvertMCU_Limit(pYBlock[126]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += JPEG_DCTSIZE, 
			pCb1 += JPEG_DCTSIZE,
			pCb2 += JPEG_DCTSIZE,
			pCb3 += JPEG_DCTSIZE, 
			pCr0 += JPEG_DCTSIZE,
			pCr1 += JPEG_DCTSIZE, 
			pCr2 += JPEG_DCTSIZE, 
			pCr3 += JPEG_DCTSIZE, 
			pCbBlock += JPEG_DCTSIZE2,
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[1]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[3]);
			pCb0[4] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[5] = JPEG_ConvertMCU_Limit(pCbBlock[5]);
			pCb0[6] = JPEG_ConvertMCU_Limit(pCbBlock[6]);
			pCb0[7] = JPEG_ConvertMCU_Limit(pCbBlock[7]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[17]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[19]);
			pCb1[4] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb1[5] = JPEG_ConvertMCU_Limit(pCbBlock[21]);
			pCb1[6] = JPEG_ConvertMCU_Limit(pCbBlock[22]);
			pCb1[7] = JPEG_ConvertMCU_Limit(pCbBlock[23]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[24]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[25]); 
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[26]); 
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[27]); 
			pCb2[4] = JPEG_ConvertMCU_Limit(pCbBlock[28]);
			pCb2[5] = JPEG_ConvertMCU_Limit(pCbBlock[29]); 
			pCb2[6] = JPEG_ConvertMCU_Limit(pCbBlock[30]); 
			pCb2[7] = JPEG_ConvertMCU_Limit(pCbBlock[31]); 

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[40]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[41]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[42]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[43]); 
			pCb3[4] = JPEG_ConvertMCU_Limit(pCbBlock[44]);
			pCb3[5] = JPEG_ConvertMCU_Limit(pCbBlock[45]); 
			pCb3[6] = JPEG_ConvertMCU_Limit(pCbBlock[46]); 
			pCb3[7] = JPEG_ConvertMCU_Limit(pCbBlock[47]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]); 
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[1]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[2]); 
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[3]);
			pCr0[4] = JPEG_ConvertMCU_Limit(pCrBlock[4]); 
			pCr0[5] = JPEG_ConvertMCU_Limit(pCrBlock[5]);  
			pCr0[6] = JPEG_ConvertMCU_Limit(pCrBlock[6]); 
			pCr0[7] = JPEG_ConvertMCU_Limit(pCrBlock[7]); 

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]); 
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[17]); 
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[18]);  
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[19]);  
			pCr1[4] = JPEG_ConvertMCU_Limit(pCrBlock[20]); 
			pCr1[5] = JPEG_ConvertMCU_Limit(pCrBlock[21]); 
			pCr1[6] = JPEG_ConvertMCU_Limit(pCrBlock[22]); 
			pCr1[7] = JPEG_ConvertMCU_Limit(pCrBlock[23]);

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[40]); 
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[41]); 
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[42]); 
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[43]);
			pCr2[4] = JPEG_ConvertMCU_Limit(pCrBlock[44]); 
			pCr2[5] = JPEG_ConvertMCU_Limit(pCrBlock[45]); 
			pCr2[6] = JPEG_ConvertMCU_Limit(pCrBlock[46]); 
			pCr2[7] = JPEG_ConvertMCU_Limit(pCrBlock[47]);

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[56]); 
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[57]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[58]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[59]);
			pCr3[4] = JPEG_ConvertMCU_Limit(pCrBlock[60]); 
			pCr3[5] = JPEG_ConvertMCU_Limit(pCrBlock[61]); 
			pCr3[6] = JPEG_ConvertMCU_Limit(pCrBlock[62]); 
			pCr3[7] = JPEG_ConvertMCU_Limit(pCrBlock[63]);
		}
	}
	else if( nHorzFactor == 1 && nVertFactor == 2) //H1V2
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;
		pY4 = pYBuf + nYBufWidth * 4;
		pY5 = pYBuf + nYBufWidth * 5;
		pY6 = pYBuf + nYBufWidth * 6;
		pY7 = pYBuf + nYBufWidth * 7;
		pY8 = pYBuf + nYBufWidth * 8;
		pY9 = pYBuf + nYBufWidth * 9;
		pY10 = pYBuf + nYBufWidth * 10;
		pY11 = pYBuf + nYBufWidth * 11;
		pY12 = pYBuf + nYBufWidth * 12;
		pY13 = pYBuf + nYBufWidth * 13;
		pY14 = pYBuf + nYBufWidth * 14;
		pY15 = pYBuf + nYBufWidth * 15;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;
		pCb4 = pCbBuf + nCbBufWidth * 4;
		pCb5 = pCbBuf + nCbBufWidth * 5;
		pCb6 = pCbBuf + nCbBufWidth * 6;
		pCb7 = pCbBuf + nCbBufWidth * 7;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;
		pCr4 = pCrBuf + nCrBufWidth * 4;
		pCr5 = pCrBuf + nCrBufWidth * 5;
		pCr6 = pCrBuf + nCrBufWidth * 6;
		pCr7 = pCrBuf + nCrBufWidth * 7;

		nMcuCount = nYBlockCount>>1;
		nStep = JPEG_DCTSIZE>>1;
		nBlockStep = JPEG_DCTSIZE2<<1;

		for( i = nMcuCount; i; i--, 
			pY0 += nStep, 
			pY1 += nStep,
			pY2 += nStep,
			pY3 += nStep,
			pY4 += nStep, 
			pY5 += nStep,
			pY6 += nStep,
			pY7 += nStep,
			pY8 += nStep,
			pY9 += nStep,
			pY10 += nStep,
			pY11 += nStep,
			pY12 += nStep,
			pY13 += nStep,
			pY14 += nStep,
			pY15 += nStep,
			pYBlock += nBlockStep
			)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[2]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[4]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[6]); 

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[8]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[10]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[12]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[14]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[18]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[20]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[22]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[24]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[26]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[28]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[30]); 

			pY4[0] = JPEG_ConvertMCU_Limit(pYBlock[32]); 
			pY4[1] = JPEG_ConvertMCU_Limit(pYBlock[34]);  
			pY4[2] = JPEG_ConvertMCU_Limit(pYBlock[36]); 
			pY4[3] = JPEG_ConvertMCU_Limit(pYBlock[38]); 

			pY5[0] = JPEG_ConvertMCU_Limit(pYBlock[40]);  
			pY5[1] = JPEG_ConvertMCU_Limit(pYBlock[42]);  
			pY5[2] = JPEG_ConvertMCU_Limit(pYBlock[44]); 
			pY5[3] = JPEG_ConvertMCU_Limit(pYBlock[46]); 

			pY6[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY6[1] = JPEG_ConvertMCU_Limit(pYBlock[50]);  
			pY6[2] = JPEG_ConvertMCU_Limit(pYBlock[52]); 
			pY6[3] = JPEG_ConvertMCU_Limit(pYBlock[54]); 

			pY7[0] = JPEG_ConvertMCU_Limit(pYBlock[56]);  
			pY7[1] = JPEG_ConvertMCU_Limit(pYBlock[58]);  
			pY7[2] = JPEG_ConvertMCU_Limit(pYBlock[60]); 
			pY7[3] = JPEG_ConvertMCU_Limit(pYBlock[62]); 

			pY8[0] = JPEG_ConvertMCU_Limit(pYBlock[64]); 
			pY8[1] = JPEG_ConvertMCU_Limit(pYBlock[66]);  
			pY8[2] = JPEG_ConvertMCU_Limit(pYBlock[68]); 
			pY8[3] = JPEG_ConvertMCU_Limit(pYBlock[70]); 

			pY9[0] = JPEG_ConvertMCU_Limit(pYBlock[72]);  
			pY9[1] = JPEG_ConvertMCU_Limit(pYBlock[74]);  
			pY9[2] = JPEG_ConvertMCU_Limit(pYBlock[76]); 
			pY9[3] = JPEG_ConvertMCU_Limit(pYBlock[78]); 

			pY10[0] = JPEG_ConvertMCU_Limit(pYBlock[80]);  
			pY10[1] = JPEG_ConvertMCU_Limit(pYBlock[82]);  
			pY10[2] = JPEG_ConvertMCU_Limit(pYBlock[84]); 
			pY10[3] = JPEG_ConvertMCU_Limit(pYBlock[86]); 

			pY11[0] = JPEG_ConvertMCU_Limit(pYBlock[88]);  
			pY11[1] = JPEG_ConvertMCU_Limit(pYBlock[90]);  
			pY11[2] = JPEG_ConvertMCU_Limit(pYBlock[92]); 
			pY11[3] = JPEG_ConvertMCU_Limit(pYBlock[94]); 

			pY12[0] = JPEG_ConvertMCU_Limit(pYBlock[96]); 
			pY12[1] = JPEG_ConvertMCU_Limit(pYBlock[98]);  
			pY12[2] = JPEG_ConvertMCU_Limit(pYBlock[100]); 
			pY12[3] = JPEG_ConvertMCU_Limit(pYBlock[102]); 

			pY13[0] = JPEG_ConvertMCU_Limit(pYBlock[104]);  
			pY13[1] = JPEG_ConvertMCU_Limit(pYBlock[106]);  
			pY13[2] = JPEG_ConvertMCU_Limit(pYBlock[108]); 
			pY13[3] = JPEG_ConvertMCU_Limit(pYBlock[110]); 

			pY14[0] = JPEG_ConvertMCU_Limit(pYBlock[112]);  
			pY14[1] = JPEG_ConvertMCU_Limit(pYBlock[114]);  
			pY14[2] = JPEG_ConvertMCU_Limit(pYBlock[116]); 
			pY14[3] = JPEG_ConvertMCU_Limit(pYBlock[118]); 

			pY15[0] = JPEG_ConvertMCU_Limit(pYBlock[120]);  
			pY15[1] = JPEG_ConvertMCU_Limit(pYBlock[122]);  
			pY15[2] = JPEG_ConvertMCU_Limit(pYBlock[124]); 
			pY15[3] = JPEG_ConvertMCU_Limit(pYBlock[126]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += nStep, 
			pCb1 += nStep,
			pCb2 += nStep,
			pCb3 += nStep,
			pCb4 += nStep, 
			pCb5 += nStep,
			pCb6 += nStep,
			pCb7 += nStep,
			pCr0 += nStep, 
			pCr1 += nStep, 
			pCr2 += nStep, 
			pCr3 += nStep, 
			pCr4 += nStep, 
			pCr5 += nStep, 
			pCr6 += nStep, 
			pCr7 += nStep, 
			pCbBlock += JPEG_DCTSIZE2, 
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[6]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[8]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[10]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[12]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[14]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[22]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[24]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[26]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[28]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[30]); 

			pCb4[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb4[1] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb4[2] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb4[3] = JPEG_ConvertMCU_Limit(pCbBlock[38]);

			pCb5[0] = JPEG_ConvertMCU_Limit(pCbBlock[40]);
			pCb5[1] = JPEG_ConvertMCU_Limit(pCbBlock[42]);
			pCb5[2] = JPEG_ConvertMCU_Limit(pCbBlock[44]);
			pCb5[3] = JPEG_ConvertMCU_Limit(pCbBlock[46]);

			pCb6[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb6[1] = JPEG_ConvertMCU_Limit(pCbBlock[50]);
			pCb6[2] = JPEG_ConvertMCU_Limit(pCbBlock[52]);
			pCb6[3] = JPEG_ConvertMCU_Limit(pCbBlock[54]);

			pCb7[0] = JPEG_ConvertMCU_Limit(pCbBlock[56]);
			pCb7[1] = JPEG_ConvertMCU_Limit(pCbBlock[58]); 
			pCb7[2] = JPEG_ConvertMCU_Limit(pCbBlock[60]); 
			pCb7[3] = JPEG_ConvertMCU_Limit(pCbBlock[62]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]); 
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[2]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[4]); 
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[6]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[8]); 
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[10]);  
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[12]); 
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[14]); 

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]); 
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[18]); 
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[20]);  
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[22]);  

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[24]); 
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[26]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[28]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[30]);

			pCr4[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]); 
			pCr4[1] = JPEG_ConvertMCU_Limit(pCrBlock[34]);
			pCr4[2] = JPEG_ConvertMCU_Limit(pCrBlock[36]); 
			pCr4[3] = JPEG_ConvertMCU_Limit(pCrBlock[38]);

			pCr5[0] = JPEG_ConvertMCU_Limit(pCrBlock[40]); 
			pCr5[1] = JPEG_ConvertMCU_Limit(pCrBlock[42]);  
			pCr5[2] = JPEG_ConvertMCU_Limit(pCrBlock[44]); 
			pCr5[3] = JPEG_ConvertMCU_Limit(pCrBlock[46]); 

			pCr6[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]); 
			pCr6[1] = JPEG_ConvertMCU_Limit(pCrBlock[50]); 
			pCr6[2] = JPEG_ConvertMCU_Limit(pCrBlock[52]);  
			pCr6[3] = JPEG_ConvertMCU_Limit(pCrBlock[54]);  

			pCr7[0] = JPEG_ConvertMCU_Limit(pCrBlock[56]); 
			pCr7[1] = JPEG_ConvertMCU_Limit(pCrBlock[58]); 
			pCr7[2] = JPEG_ConvertMCU_Limit(pCrBlock[60]); 
			pCr7[3] = JPEG_ConvertMCU_Limit(pCrBlock[62]);
		}
	}
	else if ( nHorzFactor == 2 && nVertFactor == 2) //H2V2
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;
		pY4 = pYBuf + nYBufWidth * 4;
		pY5 = pYBuf + nYBufWidth * 5;
		pY6 = pYBuf + nYBufWidth * 6;
		pY7 = pYBuf + nYBufWidth * 7;
		pY8 = pYBuf + nYBufWidth * 8;
		pY9 = pYBuf + nYBufWidth * 9;
		pY10 = pYBuf + nYBufWidth * 10;
		pY11 = pYBuf + nYBufWidth * 11;
		pY12 = pYBuf + nYBufWidth * 12;
		pY13 = pYBuf + nYBufWidth * 13;
		pY14 = pYBuf + nYBufWidth * 14;
		pY15 = pYBuf + nYBufWidth * 15;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;
		pCb4 = pCbBuf + nCbBufWidth * 4;
		pCb5 = pCbBuf + nCbBufWidth * 5;
		pCb6 = pCbBuf + nCbBufWidth * 6;
		pCb7 = pCbBuf + nCbBufWidth * 7;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;
		pCr4 = pCrBuf + nCrBufWidth * 4;
		pCr5 = pCrBuf + nCrBufWidth * 5;
		pCr6 = pCrBuf + nCrBufWidth * 6;
		pCr7 = pCrBuf + nCrBufWidth * 7;

		nMcuCount = nYBlockCount>>2;
		nBlockStep = JPEG_DCTSIZE2<<2;

		for( i = nMcuCount; i; i--, 
			pY0 += JPEG_DCTSIZE, 
			pY1 += JPEG_DCTSIZE,
			pY2 += JPEG_DCTSIZE,
			pY3 += JPEG_DCTSIZE,
			pY4 += JPEG_DCTSIZE,
			pY5 += JPEG_DCTSIZE,
			pY6 += JPEG_DCTSIZE,
			pY7 += JPEG_DCTSIZE,
			pY8 += JPEG_DCTSIZE, 
			pY9 += JPEG_DCTSIZE,
			pY10 += JPEG_DCTSIZE,
			pY11 += JPEG_DCTSIZE,
			pY12 += JPEG_DCTSIZE,
			pY13 += JPEG_DCTSIZE,
			pY14 += JPEG_DCTSIZE,
			pY15 += JPEG_DCTSIZE,
			pYBlock += nBlockStep
			)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[2]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[4]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[6]); 
			pY0[4] = JPEG_ConvertMCU_Limit(pYBlock[64]);  
			pY0[5] = JPEG_ConvertMCU_Limit(pYBlock[66]);  
			pY0[6] = JPEG_ConvertMCU_Limit(pYBlock[68]); 
			pY0[7] = JPEG_ConvertMCU_Limit(pYBlock[70]);

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[8]); 
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[10]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[12]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[14]); 
			pY1[4] = JPEG_ConvertMCU_Limit(pYBlock[72]);  
			pY1[5] = JPEG_ConvertMCU_Limit(pYBlock[74]);  
			pY1[6] = JPEG_ConvertMCU_Limit(pYBlock[76]); 
			pY1[7] = JPEG_ConvertMCU_Limit(pYBlock[78]);

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[18]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[20]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[22]); 
			pY2[4] = JPEG_ConvertMCU_Limit(pYBlock[80]);  
			pY2[5] = JPEG_ConvertMCU_Limit(pYBlock[82]);  
			pY2[6] = JPEG_ConvertMCU_Limit(pYBlock[84]); 
			pY2[7] = JPEG_ConvertMCU_Limit(pYBlock[86]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[24]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[26]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[28]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[30]); 
			pY3[4] = JPEG_ConvertMCU_Limit(pYBlock[88]);  
			pY3[5] = JPEG_ConvertMCU_Limit(pYBlock[90]);  
			pY3[6] = JPEG_ConvertMCU_Limit(pYBlock[92]); 
			pY3[7] = JPEG_ConvertMCU_Limit(pYBlock[94]); 

			pY4[0] = JPEG_ConvertMCU_Limit(pYBlock[32]);  
			pY4[1] = JPEG_ConvertMCU_Limit(pYBlock[34]);  
			pY4[2] = JPEG_ConvertMCU_Limit(pYBlock[36]); 
			pY4[3] = JPEG_ConvertMCU_Limit(pYBlock[38]); 
			pY4[4] = JPEG_ConvertMCU_Limit(pYBlock[96]);  
			pY4[5] = JPEG_ConvertMCU_Limit(pYBlock[98]);  
			pY4[6] = JPEG_ConvertMCU_Limit(pYBlock[100]); 
			pY4[7] = JPEG_ConvertMCU_Limit(pYBlock[102]); 

			pY5[0] = JPEG_ConvertMCU_Limit(pYBlock[40]);  
			pY5[1] = JPEG_ConvertMCU_Limit(pYBlock[42]);  
			pY5[2] = JPEG_ConvertMCU_Limit(pYBlock[44]); 
			pY5[3] = JPEG_ConvertMCU_Limit(pYBlock[46]); 
			pY5[4] = JPEG_ConvertMCU_Limit(pYBlock[104]);  
			pY5[5] = JPEG_ConvertMCU_Limit(pYBlock[106]);  
			pY5[6] = JPEG_ConvertMCU_Limit(pYBlock[108]); 
			pY5[7] = JPEG_ConvertMCU_Limit(pYBlock[110]); 

			pY6[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY6[1] = JPEG_ConvertMCU_Limit(pYBlock[50]);  
			pY6[2] = JPEG_ConvertMCU_Limit(pYBlock[52]); 
			pY6[3] = JPEG_ConvertMCU_Limit(pYBlock[54]); 
			pY6[4] = JPEG_ConvertMCU_Limit(pYBlock[112]);  
			pY6[5] = JPEG_ConvertMCU_Limit(pYBlock[114]);  
			pY6[6] = JPEG_ConvertMCU_Limit(pYBlock[116]); 
			pY6[7] = JPEG_ConvertMCU_Limit(pYBlock[118]);

			pY7[0] = JPEG_ConvertMCU_Limit(pYBlock[56]);  
			pY7[1] = JPEG_ConvertMCU_Limit(pYBlock[58]);  
			pY7[2] = JPEG_ConvertMCU_Limit(pYBlock[60]); 
			pY7[3] = JPEG_ConvertMCU_Limit(pYBlock[62]); 
			pY7[4] = JPEG_ConvertMCU_Limit(pYBlock[120]);  
			pY7[5] = JPEG_ConvertMCU_Limit(pYBlock[122]);  
			pY7[6] = JPEG_ConvertMCU_Limit(pYBlock[124]); 
			pY7[7] = JPEG_ConvertMCU_Limit(pYBlock[126]); 

			pY8[0] = JPEG_ConvertMCU_Limit(pYBlock[128]); 
			pY8[1] = JPEG_ConvertMCU_Limit(pYBlock[130]);  
			pY8[2] = JPEG_ConvertMCU_Limit(pYBlock[132]); 
			pY8[3] = JPEG_ConvertMCU_Limit(pYBlock[134]); 
			pY8[4] = JPEG_ConvertMCU_Limit(pYBlock[192]);  
			pY8[5] = JPEG_ConvertMCU_Limit(pYBlock[194]);  
			pY8[6] = JPEG_ConvertMCU_Limit(pYBlock[196]); 
			pY8[7] = JPEG_ConvertMCU_Limit(pYBlock[198]);

			pY9[0] = JPEG_ConvertMCU_Limit(pYBlock[136]); 
			pY9[1] = JPEG_ConvertMCU_Limit(pYBlock[138]);  
			pY9[2] = JPEG_ConvertMCU_Limit(pYBlock[140]); 
			pY9[3] = JPEG_ConvertMCU_Limit(pYBlock[142]); 
			pY9[4] = JPEG_ConvertMCU_Limit(pYBlock[200]);  
			pY9[5] = JPEG_ConvertMCU_Limit(pYBlock[202]);  
			pY9[6] = JPEG_ConvertMCU_Limit(pYBlock[204]); 
			pY9[7] = JPEG_ConvertMCU_Limit(pYBlock[206]);

			pY10[0] = JPEG_ConvertMCU_Limit(pYBlock[144]);  
			pY10[1] = JPEG_ConvertMCU_Limit(pYBlock[146]);  
			pY10[2] = JPEG_ConvertMCU_Limit(pYBlock[148]); 
			pY10[3] = JPEG_ConvertMCU_Limit(pYBlock[150]); 
			pY10[4] = JPEG_ConvertMCU_Limit(pYBlock[208]);  
			pY10[5] = JPEG_ConvertMCU_Limit(pYBlock[210]);  
			pY10[6] = JPEG_ConvertMCU_Limit(pYBlock[212]); 
			pY10[7] = JPEG_ConvertMCU_Limit(pYBlock[214]);

			pY11[0] = JPEG_ConvertMCU_Limit(pYBlock[152]);  
			pY11[1] = JPEG_ConvertMCU_Limit(pYBlock[154]);  
			pY11[2] = JPEG_ConvertMCU_Limit(pYBlock[156]); 
			pY11[3] = JPEG_ConvertMCU_Limit(pYBlock[158]); 
			pY11[4] = JPEG_ConvertMCU_Limit(pYBlock[216]);  
			pY11[5] = JPEG_ConvertMCU_Limit(pYBlock[218]);  
			pY11[6] = JPEG_ConvertMCU_Limit(pYBlock[220]); 
			pY11[7] = JPEG_ConvertMCU_Limit(pYBlock[222]); 

			pY12[0] = JPEG_ConvertMCU_Limit(pYBlock[160]);  
			pY12[1] = JPEG_ConvertMCU_Limit(pYBlock[162]);  
			pY12[2] = JPEG_ConvertMCU_Limit(pYBlock[164]); 
			pY12[3] = JPEG_ConvertMCU_Limit(pYBlock[166]); 
			pY12[4] = JPEG_ConvertMCU_Limit(pYBlock[224]);  
			pY12[5] = JPEG_ConvertMCU_Limit(pYBlock[226]);  
			pY12[6] = JPEG_ConvertMCU_Limit(pYBlock[228]); 
			pY12[7] = JPEG_ConvertMCU_Limit(pYBlock[230]);

			pY13[0] = JPEG_ConvertMCU_Limit(pYBlock[168]);  
			pY13[1] = JPEG_ConvertMCU_Limit(pYBlock[170]);  
			pY13[2] = JPEG_ConvertMCU_Limit(pYBlock[172]); 
			pY13[3] = JPEG_ConvertMCU_Limit(pYBlock[174]); 
			pY13[4] = JPEG_ConvertMCU_Limit(pYBlock[232]);  
			pY13[5] = JPEG_ConvertMCU_Limit(pYBlock[234]);  
			pY13[6] = JPEG_ConvertMCU_Limit(pYBlock[236]); 
			pY13[7] = JPEG_ConvertMCU_Limit(pYBlock[238]); 

			pY14[0] = JPEG_ConvertMCU_Limit(pYBlock[176]);  
			pY14[1] = JPEG_ConvertMCU_Limit(pYBlock[178]);  
			pY14[2] = JPEG_ConvertMCU_Limit(pYBlock[180]); 
			pY14[3] = JPEG_ConvertMCU_Limit(pYBlock[182]); 
			pY14[4] = JPEG_ConvertMCU_Limit(pYBlock[240]);  
			pY14[5] = JPEG_ConvertMCU_Limit(pYBlock[242]);  
			pY14[6] = JPEG_ConvertMCU_Limit(pYBlock[244]); 
			pY14[7] = JPEG_ConvertMCU_Limit(pYBlock[246]);

			pY15[0] = JPEG_ConvertMCU_Limit(pYBlock[184]);  
			pY15[1] = JPEG_ConvertMCU_Limit(pYBlock[186]);  
			pY15[2] = JPEG_ConvertMCU_Limit(pYBlock[188]); 
			pY15[3] = JPEG_ConvertMCU_Limit(pYBlock[190]); 
			pY15[4] = JPEG_ConvertMCU_Limit(pYBlock[248]);  
			pY15[5] = JPEG_ConvertMCU_Limit(pYBlock[250]);  
			pY15[6] = JPEG_ConvertMCU_Limit(pYBlock[252]); 
			pY15[7] = JPEG_ConvertMCU_Limit(pYBlock[254]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += JPEG_DCTSIZE, 
			pCb1 += JPEG_DCTSIZE,
			pCb2 += JPEG_DCTSIZE,
			pCb3 += JPEG_DCTSIZE,
			pCb4 += JPEG_DCTSIZE, 
			pCb5 += JPEG_DCTSIZE,
			pCb6 += JPEG_DCTSIZE,
			pCb7 += JPEG_DCTSIZE,
			pCr0 += JPEG_DCTSIZE, 
			pCr1 += JPEG_DCTSIZE, 
			pCr2 += JPEG_DCTSIZE, 
			pCr3 += JPEG_DCTSIZE, 
			pCr4 += JPEG_DCTSIZE, 
			pCr5 += JPEG_DCTSIZE, 
			pCr6 += JPEG_DCTSIZE, 
			pCr7 += JPEG_DCTSIZE, 
			pCbBlock += JPEG_DCTSIZE2, 
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[1]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[3]);
			pCb0[4] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[5] = JPEG_ConvertMCU_Limit(pCbBlock[5]);
			pCb0[6] = JPEG_ConvertMCU_Limit(pCbBlock[6]);
			pCb0[7] = JPEG_ConvertMCU_Limit(pCbBlock[7]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[8]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[9]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[10]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[11]);
			pCb1[4] = JPEG_ConvertMCU_Limit(pCbBlock[12]);
			pCb1[5] = JPEG_ConvertMCU_Limit(pCbBlock[13]);
			pCb1[6] = JPEG_ConvertMCU_Limit(pCbBlock[14]);
			pCb1[7] = JPEG_ConvertMCU_Limit(pCbBlock[15]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[17]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[19]);
			pCb2[4] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb2[5] = JPEG_ConvertMCU_Limit(pCbBlock[21]);
			pCb2[6] = JPEG_ConvertMCU_Limit(pCbBlock[22]);
			pCb2[7] = JPEG_ConvertMCU_Limit(pCbBlock[23]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[24]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[25]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[26]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[27]); 
			pCb3[4] = JPEG_ConvertMCU_Limit(pCbBlock[28]);
			pCb3[5] = JPEG_ConvertMCU_Limit(pCbBlock[29]); 
			pCb3[6] = JPEG_ConvertMCU_Limit(pCbBlock[30]); 
			pCb3[7] = JPEG_ConvertMCU_Limit(pCbBlock[31]); 

			pCb4[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb4[1] = JPEG_ConvertMCU_Limit(pCbBlock[33]);
			pCb4[2] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb4[3] = JPEG_ConvertMCU_Limit(pCbBlock[35]);
			pCb4[4] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb4[5] = JPEG_ConvertMCU_Limit(pCbBlock[37]);
			pCb4[6] = JPEG_ConvertMCU_Limit(pCbBlock[38]);
			pCb4[7] = JPEG_ConvertMCU_Limit(pCbBlock[39]);

			pCb5[0] = JPEG_ConvertMCU_Limit(pCbBlock[40]);
			pCb5[1] = JPEG_ConvertMCU_Limit(pCbBlock[41]);
			pCb5[2] = JPEG_ConvertMCU_Limit(pCbBlock[42]);
			pCb5[3] = JPEG_ConvertMCU_Limit(pCbBlock[43]);
			pCb5[4] = JPEG_ConvertMCU_Limit(pCbBlock[44]);
			pCb5[5] = JPEG_ConvertMCU_Limit(pCbBlock[45]);
			pCb5[6] = JPEG_ConvertMCU_Limit(pCbBlock[46]);
			pCb5[7] = JPEG_ConvertMCU_Limit(pCbBlock[47]);

			pCb6[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb6[1] = JPEG_ConvertMCU_Limit(pCbBlock[49]);
			pCb6[2] = JPEG_ConvertMCU_Limit(pCbBlock[50]);
			pCb6[3] = JPEG_ConvertMCU_Limit(pCbBlock[51]);
			pCb6[4] = JPEG_ConvertMCU_Limit(pCbBlock[52]);
			pCb6[5] = JPEG_ConvertMCU_Limit(pCbBlock[53]);
			pCb6[6] = JPEG_ConvertMCU_Limit(pCbBlock[54]);
			pCb6[7] = JPEG_ConvertMCU_Limit(pCbBlock[55]);

			pCb7[0] = JPEG_ConvertMCU_Limit(pCbBlock[56]);
			pCb7[1] = JPEG_ConvertMCU_Limit(pCbBlock[57]); 
			pCb7[2] = JPEG_ConvertMCU_Limit(pCbBlock[58]); 
			pCb7[3] = JPEG_ConvertMCU_Limit(pCbBlock[59]); 
			pCb7[4] = JPEG_ConvertMCU_Limit(pCbBlock[60]);
			pCb7[5] = JPEG_ConvertMCU_Limit(pCbBlock[61]); 
			pCb7[6] = JPEG_ConvertMCU_Limit(pCbBlock[62]); 
			pCb7[7] = JPEG_ConvertMCU_Limit(pCbBlock[63]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]);
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[1]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[2]);
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[3]);
			pCr0[4] = JPEG_ConvertMCU_Limit(pCrBlock[4]);
			pCr0[5] = JPEG_ConvertMCU_Limit(pCrBlock[5]);
			pCr0[6] = JPEG_ConvertMCU_Limit(pCrBlock[6]);
			pCr0[7] = JPEG_ConvertMCU_Limit(pCrBlock[7]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[8]);
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[9]);
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[10]);
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[11]);
			pCr1[4] = JPEG_ConvertMCU_Limit(pCrBlock[12]);
			pCr1[5] = JPEG_ConvertMCU_Limit(pCrBlock[13]);
			pCr1[6] = JPEG_ConvertMCU_Limit(pCrBlock[14]);
			pCr1[7] = JPEG_ConvertMCU_Limit(pCrBlock[15]);

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]);
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[17]);
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[18]);
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[19]);
			pCr2[4] = JPEG_ConvertMCU_Limit(pCrBlock[20]);
			pCr2[5] = JPEG_ConvertMCU_Limit(pCrBlock[21]);
			pCr2[6] = JPEG_ConvertMCU_Limit(pCrBlock[22]);
			pCr2[7] = JPEG_ConvertMCU_Limit(pCrBlock[23]);

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[24]);
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[25]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[26]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[27]); 
			pCr3[4] = JPEG_ConvertMCU_Limit(pCrBlock[28]);
			pCr3[5] = JPEG_ConvertMCU_Limit(pCrBlock[29]); 
			pCr3[6] = JPEG_ConvertMCU_Limit(pCrBlock[30]); 
			pCr3[7] = JPEG_ConvertMCU_Limit(pCrBlock[31]); 

			pCr4[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]);
			pCr4[1] = JPEG_ConvertMCU_Limit(pCrBlock[33]);
			pCr4[2] = JPEG_ConvertMCU_Limit(pCrBlock[34]);
			pCr4[3] = JPEG_ConvertMCU_Limit(pCrBlock[35]);
			pCr4[4] = JPEG_ConvertMCU_Limit(pCrBlock[36]);
			pCr4[5] = JPEG_ConvertMCU_Limit(pCrBlock[37]);
			pCr4[6] = JPEG_ConvertMCU_Limit(pCrBlock[38]);
			pCr4[7] = JPEG_ConvertMCU_Limit(pCrBlock[39]);

			pCr5[0] = JPEG_ConvertMCU_Limit(pCrBlock[40]);
			pCr5[1] = JPEG_ConvertMCU_Limit(pCrBlock[41]);
			pCr5[2] = JPEG_ConvertMCU_Limit(pCrBlock[42]);
			pCr5[3] = JPEG_ConvertMCU_Limit(pCrBlock[43]);
			pCr5[4] = JPEG_ConvertMCU_Limit(pCrBlock[44]);
			pCr5[5] = JPEG_ConvertMCU_Limit(pCrBlock[45]);
			pCr5[6] = JPEG_ConvertMCU_Limit(pCrBlock[46]);
			pCr5[7] = JPEG_ConvertMCU_Limit(pCrBlock[47]);

			pCr6[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]);
			pCr6[1] = JPEG_ConvertMCU_Limit(pCrBlock[49]);
			pCr6[2] = JPEG_ConvertMCU_Limit(pCrBlock[50]);
			pCr6[3] = JPEG_ConvertMCU_Limit(pCrBlock[51]);
			pCr6[4] = JPEG_ConvertMCU_Limit(pCrBlock[52]);
			pCr6[5] = JPEG_ConvertMCU_Limit(pCrBlock[53]);
			pCr6[6] = JPEG_ConvertMCU_Limit(pCrBlock[54]);
			pCr6[7] = JPEG_ConvertMCU_Limit(pCrBlock[55]);

			pCr7[0] = JPEG_ConvertMCU_Limit(pCrBlock[56]);
			pCr7[1] = JPEG_ConvertMCU_Limit(pCrBlock[57]); 
			pCr7[2] = JPEG_ConvertMCU_Limit(pCrBlock[58]); 
			pCr7[3] = JPEG_ConvertMCU_Limit(pCrBlock[59]); 
			pCr7[4] = JPEG_ConvertMCU_Limit(pCrBlock[60]);
			pCr7[5] = JPEG_ConvertMCU_Limit(pCrBlock[61]); 
			pCr7[6] = JPEG_ConvertMCU_Limit(pCrBlock[62]); 
			pCr7[7] = JPEG_ConvertMCU_Limit(pCrBlock[63]); 
		}
	}
	else if( nHorzFactor == 1 && nVertFactor == 1 ) //H1V1
	{
		pY0 = pYBuf;
		pY1 = pYBuf + nYBufWidth;
		pY2 = pYBuf + nYBufWidth * 2;
		pY3 = pYBuf + nYBufWidth * 3;
		pY4 = pYBuf + nYBufWidth * 4;
		pY5 = pYBuf + nYBufWidth * 5;
		pY6 = pYBuf + nYBufWidth * 6;
		pY7 = pYBuf + nYBufWidth * 7;

		pCb0 = pCbBuf;
		pCb1 = pCbBuf + nCbBufWidth;
		pCb2 = pCbBuf + nCbBufWidth * 2;
		pCb3 = pCbBuf + nCbBufWidth * 3;

		pCr0 = pCrBuf;
		pCr1 = pCrBuf + nCrBufWidth;
		pCr2 = pCrBuf + nCrBufWidth * 2;
		pCr3 = pCrBuf + nCrBufWidth * 3;

		nStep = JPEG_DCTSIZE>>1;

		for( i = nYBlockCount; i; i--, 
			pY0 += nStep, 
			pY1 += nStep,
			pY2 += nStep,
			pY3 += nStep,
			pY4 += nStep, 
			pY5 += nStep,
			pY6 += nStep,
			pY7 += nStep,
			pYBlock += JPEG_DCTSIZE2 
			)
		{
			pY0[0] = JPEG_ConvertMCU_Limit(pYBlock[0]); 
			pY0[1] = JPEG_ConvertMCU_Limit(pYBlock[2]);  
			pY0[2] = JPEG_ConvertMCU_Limit(pYBlock[4]); 
			pY0[3] = JPEG_ConvertMCU_Limit(pYBlock[6]); 

			pY1[0] = JPEG_ConvertMCU_Limit(pYBlock[8]);  
			pY1[1] = JPEG_ConvertMCU_Limit(pYBlock[10]);  
			pY1[2] = JPEG_ConvertMCU_Limit(pYBlock[12]); 
			pY1[3] = JPEG_ConvertMCU_Limit(pYBlock[14]); 

			pY2[0] = JPEG_ConvertMCU_Limit(pYBlock[16]);  
			pY2[1] = JPEG_ConvertMCU_Limit(pYBlock[18]);  
			pY2[2] = JPEG_ConvertMCU_Limit(pYBlock[20]); 
			pY2[3] = JPEG_ConvertMCU_Limit(pYBlock[22]); 

			pY3[0] = JPEG_ConvertMCU_Limit(pYBlock[24]);  
			pY3[1] = JPEG_ConvertMCU_Limit(pYBlock[26]);  
			pY3[2] = JPEG_ConvertMCU_Limit(pYBlock[28]); 
			pY3[3] = JPEG_ConvertMCU_Limit(pYBlock[30]); 

			pY4[0] = JPEG_ConvertMCU_Limit(pYBlock[32]); 
			pY4[1] = JPEG_ConvertMCU_Limit(pYBlock[34]);  
			pY4[2] = JPEG_ConvertMCU_Limit(pYBlock[36]); 
			pY4[3] = JPEG_ConvertMCU_Limit(pYBlock[38]); 

			pY5[0] = JPEG_ConvertMCU_Limit(pYBlock[40]);  
			pY5[1] = JPEG_ConvertMCU_Limit(pYBlock[42]);  
			pY5[2] = JPEG_ConvertMCU_Limit(pYBlock[44]); 
			pY5[3] = JPEG_ConvertMCU_Limit(pYBlock[46]); 

			pY6[0] = JPEG_ConvertMCU_Limit(pYBlock[48]);  
			pY6[1] = JPEG_ConvertMCU_Limit(pYBlock[50]);  
			pY6[2] = JPEG_ConvertMCU_Limit(pYBlock[52]); 
			pY6[3] = JPEG_ConvertMCU_Limit(pYBlock[54]); 

			pY7[0] = JPEG_ConvertMCU_Limit(pYBlock[56]);  
			pY7[1] = JPEG_ConvertMCU_Limit(pYBlock[58]);  
			pY7[2] = JPEG_ConvertMCU_Limit(pYBlock[60]); 
			pY7[3] = JPEG_ConvertMCU_Limit(pYBlock[62]); 
		}

		for( i = nCbBlockCount; i; i--, 
			pCb0 += nStep, 
			pCb1 += nStep,
			pCb2 += nStep,
			pCb3 += nStep,
			pCr0 += nStep, 
			pCr1 += nStep, 
			pCr2 += nStep, 
			pCr3 += nStep, 
			pCbBlock += JPEG_DCTSIZE2, 
			pCrBlock += JPEG_DCTSIZE2
			)
		{
			pCb0[0] = JPEG_ConvertMCU_Limit(pCbBlock[0]);
			pCb0[1] = JPEG_ConvertMCU_Limit(pCbBlock[2]);
			pCb0[2] = JPEG_ConvertMCU_Limit(pCbBlock[4]);
			pCb0[3] = JPEG_ConvertMCU_Limit(pCbBlock[6]);

			pCb1[0] = JPEG_ConvertMCU_Limit(pCbBlock[16]);
			pCb1[1] = JPEG_ConvertMCU_Limit(pCbBlock[18]);
			pCb1[2] = JPEG_ConvertMCU_Limit(pCbBlock[20]);
			pCb1[3] = JPEG_ConvertMCU_Limit(pCbBlock[22]);

			pCb2[0] = JPEG_ConvertMCU_Limit(pCbBlock[32]);
			pCb2[1] = JPEG_ConvertMCU_Limit(pCbBlock[34]);
			pCb2[2] = JPEG_ConvertMCU_Limit(pCbBlock[36]);
			pCb2[3] = JPEG_ConvertMCU_Limit(pCbBlock[38]);

			pCb3[0] = JPEG_ConvertMCU_Limit(pCbBlock[48]);
			pCb3[1] = JPEG_ConvertMCU_Limit(pCbBlock[50]); 
			pCb3[2] = JPEG_ConvertMCU_Limit(pCbBlock[52]); 
			pCb3[3] = JPEG_ConvertMCU_Limit(pCbBlock[54]); 

			pCr0[0] = JPEG_ConvertMCU_Limit(pCrBlock[0]); 
			pCr0[1] = JPEG_ConvertMCU_Limit(pCrBlock[2]);
			pCr0[2] = JPEG_ConvertMCU_Limit(pCrBlock[4]); 
			pCr0[3] = JPEG_ConvertMCU_Limit(pCrBlock[6]);

			pCr1[0] = JPEG_ConvertMCU_Limit(pCrBlock[16]); 
			pCr1[1] = JPEG_ConvertMCU_Limit(pCrBlock[18]);  
			pCr1[2] = JPEG_ConvertMCU_Limit(pCrBlock[20]); 
			pCr1[3] = JPEG_ConvertMCU_Limit(pCrBlock[22]); 

			pCr2[0] = JPEG_ConvertMCU_Limit(pCrBlock[32]); 
			pCr2[1] = JPEG_ConvertMCU_Limit(pCrBlock[34]); 
			pCr2[2] = JPEG_ConvertMCU_Limit(pCrBlock[36]);  
			pCr2[3] = JPEG_ConvertMCU_Limit(pCrBlock[38]);  

			pCr3[0] = JPEG_ConvertMCU_Limit(pCrBlock[48]); 
			pCr3[1] = JPEG_ConvertMCU_Limit(pCrBlock[50]); 
			pCr3[2] = JPEG_ConvertMCU_Limit(pCrBlock[52]); 
			pCr3[3] = JPEG_ConvertMCU_Limit(pCrBlock[54]);
		}
	}
}
