#include "gba-jpeg-decode.h"
#include "../CamDsp/CamDspInc/DmaCopyApi.h"

/* Setup the IWRAM-loading definitions if this has been enabled.  There are
* three sections of this; the register definitions, the IWRAM end
* determination, and the DMA copying functions.  Register definitions are the
* same as anywhere else, only with a JPEG_IWRAM prefix.  The IWRAM end
* determination uses a generated variable that DevKit Advance's linker script
* creates.  Because of this, other linker scripts might not work with this
* code.  Finally, the DMA copying uses DMA 3.
*
* Functions that are to be copied to IWRAM must obey certain restrictions.
* They cannot refer to external constant data.  They must be declared static.
* They must have a JPEG_FUNCTION_END(NAME) macro after them; see how it is
* used in the code ahead for an example.  Finally, you should avoid external
* references altogether because of how it limits your flexibility.  Instead,
* pass necessary variable and function pointers in the arguments.
*/


/* Converts left-to-right coefficient indices into zig-zagged indices. */
#pragma DATA_SECTION (".global_tbl");
const unsigned char JPEG_ToZigZag [JPEG_DCTSIZE2] =
{
	0, 1, 8, 16, 9, 2, 3, 10,
		17, 24, 32, 25, 18, 11, 4, 5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13, 6, 7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63,
};

#define JEPG_MAX_MCU_ROW 400
#define JPEG_MAX_MCU_COL 600

extern "C" void IMG_idct_8x8_12q4(short *idct_data, unsigned num_idcts);

using namespace HiVideo;

//========================================
//段解析
//========================================

/* Return whether this code is a JPEG file.  Unfortunately it will incorrectly
* match variants such as JPEG 2000 and JPEG-LS.  A better function would
* skip known markers until it reaches an unknown marker or a handled
* SOFn.
*/

int JPEG_Match (const unsigned char *data, int length)
{
	if (length == 0) return 0;
	if (data [0] != 0xFF) return 0;
	if (length == 1) return 1;
	if (data [1] != 0xD8) return 0;
	if (length == 2) return 1;
	return 1;
}

/* Read an JPEG_Marker_SOFn marker into frame.  This expects to start
* processing immediately after the marker.
*/
int JPEG_FrameHeader_Read (JPEG_FrameHeader *frame, const unsigned char **dataBase, JPEG_Marker marker)
{
	const unsigned char *data = *dataBase;
	unsigned short length = (data [0] << 8) | data [1];
	int index;

	(void) length;
	JPEG_Assert (length >= 8);
	data += 2; /* Skip the length. */
	frame->marker = marker;
	frame->encoding = (marker >= 0xFFC0 && marker <= 0xFFC7) ? 0 : 1;
	frame->differential = !(marker >= 0xFFC0 && marker <= 0xFFC3 && marker >= 0xFFC8 && marker <= 0xFFCB);

	frame->precision = *data ++;
	frame->height = (data [0] << 8) | data [1]; data += 2;
	frame->width = (data [0] << 8) | data [1]; data += 2;
	frame->componentCount = *data ++;

	JPEG_Assert (frame->precision == 8);
	JPEG_Assert (frame->componentCount <= JPEG_MAXIMUM_COMPONENTS);
	JPEG_Assert (length == 8 + 3 * frame->componentCount);

	/* Read the frame components. */
	for (index = 0; index < frame->componentCount; index ++)
	{
		JPEG_FrameHeader_Component *c = &frame->componentList [index];
		unsigned char pair;

		c->selector = *data ++;
		pair = *data ++;
		c->horzFactor = pair >> 4;
		c->vertFactor = pair & 15;
		c->quantTable = *data ++;

		JPEG_Assert (c->horzFactor == 1 || c->horzFactor == 2 || c->horzFactor == 4);
		JPEG_Assert (c->vertFactor == 1 || c->vertFactor == 2 || c->vertFactor == 4);
		JPEG_Assert (c->quantTable <= 3);
	}

	BOOL fFixCompID = FALSE;
	for(int i = 0; i < frame->componentCount; i++)
	{
		if(frame->componentList[i].selector == 0)
		{
			fFixCompID = TRUE;
			break;
		}
	}

	if(fFixCompID)
	{
		for(int i = 0; i < frame->componentCount; i++)
		{
			frame->componentList[i].selector++;
		}
	}

	*dataBase = data;
	return 1;
}

/* Read a JPEG_Marker_SOS marker into scan.  This expects to start processing
* immediately after the marker.
*/
int JPEG_ScanHeader_Read (JPEG_ScanHeader *scan, const unsigned char **dataBase)
{
	const unsigned char *data = *dataBase;
	unsigned short length = (data [0] << 8) | data [1];
	JPEG_ScanHeader_Component *c, *cEnd;
	unsigned char pair;

	(void) length;
	JPEG_Assert (length >= 6);
	data += 2; /* Skip the length. */
	scan->componentCount = *data ++;

	JPEG_Assert (scan->componentCount <= JPEG_MAXIMUM_COMPONENTS);
	JPEG_Assert (length == 6 + 2 * scan->componentCount);

	/* Read the scan components. */
	for (c = scan->componentList, cEnd = c + scan->componentCount; c < cEnd; c ++)
	{
		c->selector = *data ++;
		pair = *data ++;
		c->dcTable = pair >> 4;
		c->acTable = pair & 15;

		JPEG_Assert (c->dcTable < 4);
		JPEG_Assert (c->acTable < 4);
	}

	/* Read the spectral and approximation footers, which are used for
	* progressive.
	*/

	scan->spectralStart = *data ++;
	scan->spectralEnd = *data ++;
	JPEG_Assert (scan->spectralStart <= 63);
	JPEG_Assert (scan->spectralEnd <= 63);
	pair = *data ++;
	scan->successiveApproximationBitPositionHigh = pair >> 4;
	scan->successiveApproximationBitPositionLow = pair & 15;
	JPEG_Assert (scan->successiveApproximationBitPositionHigh <= 13);
	JPEG_Assert (scan->successiveApproximationBitPositionLow <= 15);

	BOOL fFixCompID = FALSE;
	for (c = scan->componentList, cEnd = c + scan->componentCount; c < cEnd; c ++)
	{
		if(c->selector == 0)
		{
			fFixCompID = TRUE;
			break;
		}
	}

	if(fFixCompID)
	{
		for (c = scan->componentList, cEnd = c + scan->componentCount; c < cEnd; c ++)
		{
			c->selector++;
		}
	}

	*dataBase = data;
	return 1;
}

/* Read all headers from the very start of the JFIF stream to right after the
* SOS marker.
*/

int JPEG_Decoder_ReadHeaders (JPEG_Decoder *decoder, const unsigned char **dataBase)
{
	const unsigned char *data = *dataBase;
	JPEG_Marker marker;
	int c;

	if(((data [0] << 8) | data [1]) != JPEG_Marker_SOI) return 0; //by liaoy: 判断是否是JPEG格式

	/* Initialize state and assure that this is a JFIF file. */
	decoder->restartInterval = 0;
	JPEG_Assert (((data [0] << 8) | data [1]) == JPEG_Marker_SOI);
	data += 2;

	/* Start reading every marker as it comes in. */
	while (1)
	{
		marker = (JPEG_Marker) ((data [0] << 8) | data [1]);
		data += 2;

		switch (marker)
		{
			/* This block is just skipped over. */
		case JPEG_Marker_APP0:
		case JPEG_Marker_APP1:
		case JPEG_Marker_APP2:
		case JPEG_Marker_APP3:
		case JPEG_Marker_APP4:
		case JPEG_Marker_APP5:
		case JPEG_Marker_APP6:
		case JPEG_Marker_APP7:
		case JPEG_Marker_APP8:
		case JPEG_Marker_APP9:
		case JPEG_Marker_APP10:
		case JPEG_Marker_APP11:
		case JPEG_Marker_APP12:
		case JPEG_Marker_APP13:
		case JPEG_Marker_APP14:
		case JPEG_Marker_APP15:
		case JPEG_Marker_COM:
			data += (data [0] << 8) | data [1];
			break;

		case JPEG_Marker_DHT: /* Define Huffman table.  We just skip it for later decompression. */
			{
				unsigned short length = (data [0] << 8) | data [1];
				const unsigned char *end = data + length;

				JPEG_Assert (length >= 2);
				data += 2;
				while (data < end)
				{
					unsigned char pair, type, slot;

					pair = *data ++;
					type = pair >> 4;
					slot = pair & 15;

					JPEG_Assert (type == 0 || type == 1);
					JPEG_Assert (slot <= 15);

					if (type == 0)
						decoder->dcTables [slot] = data;
					else
						decoder->acTables [slot] = data;

					if (!JPEG_HuffmanTable_Skip (&data))
						return 0;
				}

				JPEG_Assert (data == end);
				break;
			}

		case JPEG_Marker_DQT: /* Define quantization table. */
			{
				unsigned short length = (data [0] << 8) | data [1];
				const unsigned char *end = data + length;

				JPEG_FIXED_TYPE *s;

				JPEG_Assert (length >= 2);
				data += 2;

				while (data < end)
				{
					int pair, slot, precision;

					pair = *data ++;
					precision = pair >> 4;
					slot = pair & 15;

					JPEG_Assert (precision == 0); /* Only allow 8-bit. */
					JPEG_Assert (slot < 4); /* Ensure the slot is in-range. */
					JPEG_Assert (data + 64 <= end); /* Ensure it's the right size. */

					s = decoder->quantTables [slot];

					//by liaoy: 为适应IDCT固定左移5位
					for (c = 0; c < JPEG_DCTSIZE2; c ++)
						s [c] = JPEG_ITOFIX (*data ++);
				}

				JPEG_Assert (data == end); /* Ensure we've finished it. */
				break;
			}

		case JPEG_Marker_DRI: /* Define restart interval. */
			JPEG_Assert (((data [0] << 8) | data [1]) == 4); /* Check the length. */
			decoder->restartInterval = (data [2] << 8) | data [3];
			data += 4;
			break;

		case JPEG_Marker_SOF0: /* Start of Frame: Baseline Sequential Huffman. */
			if (!JPEG_FrameHeader_Read (&(decoder->frame), &data, marker))
				return 0;
			break;

		case JPEG_Marker_SOS: /* Start of scan, immediately followed by the image. */
			if (!JPEG_ScanHeader_Read (&(decoder->scan), &data))
				return 0;
			*dataBase = data;
			return 1;

		default: /* No known marker of this type. */
			JPEG_Assert (0);
			//			break;
		}
	}
}

//================================================
//HUFFMAN解压
//================================================

/** Start reading bits. */
#define JPEG_BITS_START() \
	unsigned int bits_left = 0; \
	unsigned int bits_data = 0

/** Rewind any bytes that have not been read from and reset the state. */
#define JPEG_BITS_REWIND() \
	if (bits_left > 32)	\
	{	\
		return 0;	\
	}	\
	unsigned int count = bits_left >> 3; \
	while (count --) \
	{ \
		data --; \
		if (data [-1] == 0xFF) \
		data --; \
	} \
	bits_left = 0; \
	bits_data = 0;

/** Fill the buffer. */ //读入位缓存,保证至少有25位可取
#define JPEG_BITS_CHECK() \
	while (bits_left < 25) \
	{ \
		bits_data = (bits_data << 8) | (*data ++); \
		if (data [-1] == 0xFF) data ++; \
		bits_left += 8; \
	}

/** Return and consume a number of bits. */ //从左向右取COUNT位,同时减少位计数
#define JPEG_BITS_GET(COUNT) \
	((bits_data >> (bits_left -= (COUNT))) & ((1 << (COUNT)) - 1));

/** Return a number of bits without consuming them. */ //从左向右取COUNT位,不改变位计数
#define JPEG_BITS_PEEK(COUNT) \
	((bits_data >> (bits_left - (COUNT))) & ((1 << (COUNT)) - 1))

/** Drop a number of bits from the stream. */ //丢弃COUNT位
#define JPEG_BITS_DROP(COUNT) \
	(bits_left -= (COUNT))

/** Read a single unsigned char from the current bit-stream by using the provided table. */
#define JPEG_HuffmanTable_Decode(TABLE, OUT) \
	result = JPEG_BITS_PEEK (8); \
	if ((bitcount = (TABLE)->look_nbits [result]) != 0) \
	{ \
		JPEG_BITS_DROP (bitcount); \
		result = (TABLE)->look_sym [result]; \
	} \
	else \
	{ \
		i = 7; \
		JPEG_BITS_DROP (8); \
		do \
		{  \
			result = (result << 1) | JPEG_BITS_GET (1); \
			i++; \
			if (i == 16) return 0;	\
		} \
		while (result > (TABLE)->maxcode [i]); \
		result = (TABLE)->valptr [i] [result]; \
		JPEG_BITS_CHECK (); \
	} \
	(OUT) = result;

/* Compute a signed value.  COUNT is the number of bits to read, and OUT is
where to store the result. */
#define JPEG_Value(COUNT, OUT) \
	value = JPEG_BITS_GET (COUNT); \
	if (value < (unsigned int) (1 << ((unsigned int) (COUNT - 1)))) \
	value += (-1 << COUNT) + 1; \
	(OUT) = value;

/* Decode the coefficients from the input stream and do dequantization at the
* same time.  dcLast is the previous block's DC value and is updated.  zz is
* the output coefficients and will be all ready for an IDCT.  quant is the
* quantization table to use, dcTable and acTable are the Huffman tables for
* the DC and AC coefficients respectively, dataBase, bitsLeftBase, and
* bitsDataBase are for input stream state, and toZigZag is a pointer to
* JPEG_ToZigZag or to its IWRAM copy.
*/
//by liaoy: 输出需满足TI库IDCT要求: [-2048, 2047], 12Q4
static int JPEG_DecodeCoefficients_16 (
										JPEG_FIXED_TYPE* dcLast,
										JPEG_FIXED_TYPE* zz,
										JPEG_FIXED_TYPE* quant,
										JPEG_HuffmanTable *dcTable,
										JPEG_HuffmanTable *acTable,
										const unsigned char **dataBase,
										unsigned int *bitsLeftBase,
										unsigned int *bitsDataBase,
										const unsigned char *toZigZag,
										int nFlag
									 )
{
	unsigned bits_left = *bitsLeftBase, bits_data = *bitsDataBase; /* Input stream state. */
	const unsigned char *data = *dataBase; /* Input stream state. */
	int r, s, diff; /* Various temporary data variables. */
	int index = 1; /* The current zig-zagged index. */
	int bitcount, result, i;
	unsigned int value;

	if(!nFlag) //快速解码,不输出
	{
		/* Read the DC coefficient. */
		JPEG_BITS_CHECK ();
		JPEG_HuffmanTable_Decode (dcTable, s);
		JPEG_Value (s, diff);

		/* Store the DC coefficient. */
		*dcLast += diff;

		while (1)
		{
			/* Read a bits/run-length value. */
			JPEG_BITS_CHECK ();
			JPEG_HuffmanTable_Decode(acTable, s);

			r = s >> 4;
			s &= 15;

			/* If there is a value at this cell +r, then read it. */
			if (s)
			{
				index += r;
				JPEG_BITS_DROP (s);

				if (index == JPEG_DCTSIZE2 - 1)
					break;
				index ++;
			}
			/* Otherwise we skip 16 cells or finish up. */
			else
			{
				if (r != 15)
					break;
				index += 16;
			}
		}

		/* Restore state for the caller. */
		*bitsDataBase = bits_data;
		*bitsLeftBase = bits_left;
		*dataBase = data;

		return 1;
	}

	/* Clear all coefficients to zero. */
	{
		JPEG_FIXED_TYPE *ez = zz + JPEG_DCTSIZE2;
		do *-- ez = 0;
		while (ez > zz);
	}

	/* Read the DC coefficient. */
	JPEG_BITS_CHECK ();
	JPEG_HuffmanTable_Decode (dcTable, s);
	JPEG_Value (s, diff);

	/* Store the DC coefficient. */
	*dcLast += diff;
	int z = *dcLast * quant [0]; //处理溢出
	z = MIN_INT(z, 32752);
	z = MAX_INT(z, -32768);
	zz[toZigZag [0]] = z;

	while (1)
	{
		/* Read a bits/run-length value. */
		JPEG_BITS_CHECK ();
		JPEG_HuffmanTable_Decode(acTable, s);

		r = s >> 4;
		s &= 15;

		/* If there is a value at this cell +r, then read it. */
		if (s)
		{
			index += r;
			JPEG_Value (s, r);
			zz [toZigZag [index]] = r * quant [index]; //by liaoy: 交流分量不会溢出

			if (index == JPEG_DCTSIZE2 - 1)
				break;
			index ++;
		}
		/* Otherwise we skip 16 cells or finish up. */
		else
		{
			if (r != 15)
				break;
			index += 16;
		}
	}

	/* Restore state for the caller. */
	*bitsDataBase = bits_data;
	*bitsLeftBase = bits_left;
	*dataBase = data;
	return 1;
}

//================================================
//生成HUFFMAN查找表
//================================================

int JPEG_HuffmanTable_Read_16(JPEG_HuffmanTable *huffmanTable, const unsigned char **dataBase)
{
	const unsigned char *data = *dataBase;
	const unsigned char *bits;
	int huffcode [256] = {0};
	unsigned char huffsize [256] = {0};
	int total = 0;
	int c;

	bits = data;
	for (c = 0; c < 16; c ++)
		total += *data ++;
	huffmanTable->huffval = data;
	data += total;

	/*void GenerateSizeTable ()*/
	{
		int k = 0, i = 1, j = 1;

		do
		{
			while (j ++ <= bits [i - 1])
				huffsize [k ++] = i;
			i ++;
			j = 1;
		}
		while (i <= 16);

		huffsize [k] = 0;
	}

	/*void GenerateCodeTable ()*/
	{
		int k = 0, code = 0, si = huffsize [0];

		while (1)
		{
			do huffcode [k ++] = code ++;
			while (huffsize [k] == si);

			if (huffsize [k] == 0)
				break;

			do code <<= 1, si ++;
			while (huffsize [k] != si);
		}
	}

	/*void DecoderTables ()*/
	{
		int i = 0, j = 0;

		while (1)
		{
			if (i >= 16)
				break;
			if (bits [i] == 0)
				huffmanTable->maxcode [i] = -1;
			else
			{
				huffmanTable->valptr [i] = &huffmanTable->huffval [j - huffcode [j]];
				j += bits [i];
				huffmanTable->maxcode [i] = huffcode [j - 1];
			}
			i ++;
		}
	}


	/*void GenerateLookahead ()*/
	int l, i, p, ctr, lookbits;
	{
		memset(huffmanTable->look_nbits, 0, 256);

		p = 0;
		for (l = 1; l <= 8; l ++)
		{
			for (i = 1; i <= bits [l - 1]; i ++, p ++)
			{
				lookbits = huffcode [p] << (8 - l);
				ctr = 1 << (8 - l);
				memset(huffmanTable->look_nbits + lookbits, l, ctr);
				memset(huffmanTable->look_sym + lookbits, huffmanTable->huffval [p], ctr);
			}
		}
	}

	*dataBase = data;
	return 1;
}

/* Skip past a Huffman table section.  This expects to be called after reading
* the DHT marker and the type/slot pair.
*/
int JPEG_HuffmanTable_Skip (const unsigned char **dataBase)
{
	const unsigned char *data = *dataBase;
	int c, total = 16;

	for (c = 0; c < 16; c ++)
		total += *data ++;
	*dataBase += total;
	return 1;
}

//====================================================================
//by liaoy: 按行解压, 直邮涑鯤V_COMP_IMAGE同时进行帧场转换
//====================================================================

//拷贝到输出
void CopyLine(
			  RESTRICT_PBYTE8 pYBuf,
			  RESTRICT_PBYTE8 pCbBuf,
			  RESTRICT_PBYTE8 pCrBuf,
			  int nYBufWidth,
			  int nCbBufWidth,
			  int nCrBufWidth,
			  int nBufHeight,
			  int &iYDmaHandle,
			  int &iCbDmaHandle,
			  int &iCrDmaHandle,
			  HV_COMPONENT_IMAGE* pImgOut,
			  int& nLineCount,
			  CItgArea* pItgArea,
			  BYTE8 nFlag
			  )
{
	int nCopyCount = MIN_INT(pImgOut->iHeight - nLineCount, nBufHeight);

	CRect rcBufRow(0, nLineCount, MIN_INT(nYBufWidth, pImgOut->iWidth), nLineCount + nCopyCount - 1);

	BOOL fCopyFullLine = nFlag > 1;

	if(fCopyFullLine)
	{
#ifdef CHIP_6467
		iYDmaHandle = HV_dmacpy2D_dm6467(GetHvImageData(pImgOut, 0) + nLineCount * pImgOut->iStrideWidth[0], pImgOut->iStrideWidth[0],
			pYBuf, nYBufWidth, pImgOut->iWidth, nCopyCount);
		iCbDmaHandle = HV_dmacpy2D_dm6467(GetHvImageData(pImgOut, 1) + nLineCount * pImgOut->iStrideWidth[1], pImgOut->iStrideWidth[1],
			pCbBuf, nCbBufWidth, pImgOut->iWidth >> 1, nCopyCount);
		iCrDmaHandle = HV_dmacpy2D_dm6467(GetHvImageData(pImgOut, 2) + nLineCount * pImgOut->iStrideWidth[2], pImgOut->iStrideWidth[2],
			pCrBuf, nCrBufWidth, pImgOut->iWidth >> 1, nCopyCount);
#else

		pY = pYBuf;
		pCb = pCbBuf;
		pCr = pCrBuf;

		pYLine = GetHvImageData(pImgOut, 0)+ nLineCount * pImgOut->iStrideWidth[0];
		pCbLine = GetHvImageData(pImgOut, 1) + nLineCount * nStride2;
		pCrLine = GetHvImageData(pImgOut, 2) + nLineCount * nStride2;

		for(int i = 0;
			i < nCopyCount;
			i++,
			pY += nYBufWidth,
			pCb += nCbBufWidth,
			pCr += nCrBufWidth,
			pYLine += pImgOut->iStrideWidth[0],
			pCbLine += nStride2,
			pCrLine += nStride2
			)
		{
			iYDmaHandle = HV_dmacpy1D_dm6467(pYLine, pY, pImgOut->iWidth);
			iCbDmaHandle = HV_dmacpy1D_dm6467(pCbLine, pCb, (pImgOut->iWidth) / 2);
			iCrDmaHandle = HV_dmacpy1D_dm6467(pCrLine, pCr, (pImgOut->iWidth) / 2); 
			HV_dmawait_dm6467(iYDmaHandle);
			HV_dmawait_dm6467(iCbDmaHandle);
			HV_dmawait_dm6467(iCrDmaHandle);
		}
#endif
	}

	nLineCount += nCopyCount;
}

/*#ifndef CHIP_6467
	int HV_dmacpy2D_dm6467(PBYTE8 pbDst, int iDstStride, PBYTE8 pbSrc, int iSrcStride, int iWidth, int iHeight)
	{
		RESTRICT_PBYTE8 pDestLine = pbDst;
		RESTRICT_PBYTE8 pSrcLine = pbSrc;
		for(; iHeight; iHeight--, pDestLine += iDstStride, pSrcLine += iSrcStride)
		{
			for(int i = 0; i < iWidth; i++)
			{
				pDestLine[i] = pSrcLine[i];
			}
		}

		return 0;
	}
#endif*/

typedef struct _CompInfo
{
	JPEG_FIXED_TYPE* pLastDC;
	JPEG_FIXED_TYPE* *ppBufPtr;

	JPEG_HuffmanTable* pDC_Tbl;
	JPEG_HuffmanTable* pAC_Tbl;

	JPEG_FIXED_TYPE* pQuant_Tbl;
}
CompInfo;

/* Takes information discovered in JPEG_Decoder_ReadHeaders and loads the
* image.  This is a public function; see gba-jpeg.h for more information on it.
*/
//按行进行HUFFMAN及IDCT
int JPEG_Decoder_ReadImage_YUV_EX(
								  JPEG_Decoder *decoder,
								  const unsigned char **dataBase,
								  HV_COMPONENT_IMAGE* pImgOut,
								  CItgArea* pItgArea,
								  BOOL fTurnLeft //逆时针旋转90度
								  )
{
	//参数有效性判断
	if( !decoder || !dataBase || !pImgOut ) return 0;

	JPEG_FrameHeader *frame = &decoder->frame; /* Pointer to the image's frame. */
	JPEG_ScanHeader *scan = &decoder->scan; /* Pointer to the image's scan. */
	int YHorzFactor = 0, YVertFactor = 0; /* Scaling factors for the Y component. */
	int CbHorzFactor = 1, CbVertFactor = 1; /* Scaling factors for the Cb component.  The default is important because it is used for greyscale images. */
	int CrHorzFactor = 1, CrVertFactor = 1; /* Scaling factors for the Cr component.  The default is important because it is used for greyscale images. */
	int horzMax = 0, vertMax = 0; /* The maximum horizontal and vertical scaling factors for the components. */
	const unsigned char *data = *dataBase; /* The input data pointer; this must be right at the start of scan data. */

	if(frame->width == 0 || frame->height == 0) return 0;

	JPEG_FrameHeader_Component *frameComponents [JPEG_MAXIMUM_COMPONENTS]; /* Pointers translating scan header components to frame header components. */
	JPEG_FrameHeader_Component *item, *itemEnd = frame->componentList + frame->componentCount; /* The frame header's components for loops. */

	JPEG_HuffmanTable acTableList [2]; /* The decompressed AC Huffman tables.  JPEG Baseline allows only two AC Huffman tables in a scan. */
	int acTableUse [2] = { -1, -1 }; /* The indices of the decompressed AC Huffman tables, or -1 if this table hasn't been used. */
	JPEG_HuffmanTable dcTableList [2]; /* The decompressed DC Huffman tables.  JPEG Baseline allows only two DC Huffman tables in a scan. */
	int dcTableUse [2] = { -1, -1 }; /* The indices of the decompressed DC Huffman tables, or -1 if this table hasn't been used. */
	int restartInterval = decoder->restartInterval; /* Number of blocks until the next restart. */

	CFastMemAlloc cStack;

	/* Start decoding bits. */
	JPEG_BITS_START ();

	/* The sum of all factors in the scan; this cannot be greater than 10 in JPEG Baseline. */
	int factorSum = 0;

	/* Find the maximum factors and the factors for each component. */
	//计算各分量采样系数,初始化HUFFMAN表
	for (item = frame->componentList; item < itemEnd; item ++)
	{
		/* Find the opposing scan header component. */
		for (int c = 0; ; c ++)
		{
			JPEG_ScanHeader_Component *sc;

			JPEG_Assert (c < scan->componentCount);
			sc = &scan->componentList [c];
			if (sc->selector != item->selector)
				continue;

			/* Decompress the DC table if necessary. */
			if (sc->dcTable != dcTableUse [0] && sc->dcTable != dcTableUse [1])
			{
				const unsigned char *tablePointer = decoder->dcTables [sc->dcTable];

				if (dcTableUse [0] == -1)
					dcTableUse [0] = sc->dcTable, JPEG_HuffmanTable_Read_16 (&dcTableList [0], &tablePointer);
				else if (dcTableUse [1] == -1)
					dcTableUse [1] = sc->dcTable, JPEG_HuffmanTable_Read_16 (&dcTableList [1], &tablePointer);
				else
					JPEG_Assert (0);
			}

			/* Decompress the AC table if necessary. */
			if (sc->acTable != acTableUse [0] && sc->acTable != acTableUse [1])
			{
				const unsigned char *tablePointer = decoder->acTables [sc->acTable];

				if (acTableUse [0] == -1)
					acTableUse [0] = sc->acTable, JPEG_HuffmanTable_Read_16 (&acTableList [0], &tablePointer);
				else if (acTableUse [1] == -1)
					acTableUse [1] = sc->acTable, JPEG_HuffmanTable_Read_16 (&acTableList [1], &tablePointer);
				else
					JPEG_Assert (0);
			}

			frameComponents [c] = item;
			break;
		}

		/* Add the sum for a later assertion test. */
		factorSum += item->horzFactor * item->vertFactor;

		/* Adjust the maximum horizontal and vertical scaling factors as necessary. */
		if (item->horzFactor > horzMax)
			horzMax = item->horzFactor;
		if (item->vertFactor > vertMax)
			vertMax = item->vertFactor;

		/* Update the relevant component scaling factors if necessary. */
		if (item->selector == 1)
		{
			YHorzFactor = item->horzFactor;
			YVertFactor = item->vertFactor;
		}
		else if (item->selector == 2)
		{
			CbHorzFactor = item->horzFactor;
			CbVertFactor = item->vertFactor;
		}
		else if (item->selector == 3)
		{
			CrHorzFactor = item->horzFactor;
			CrVertFactor = item->vertFactor;
		}
	}
	
	/* Ensure that we have enough memory for these factors. */
	JPEG_Assert (factorSum < JPEG_MAXIMUM_SCAN_COMPONENT_FACTORS);

	//计算MCU参数
	int nMcuX = horzMax * JPEG_DCTSIZE;
	int nMcuY = vertMax * JPEG_DCTSIZE;
	int nMcuPerRow = (frame->width + nMcuX - 1)/nMcuX;
	int nMcuRowCount = (frame->height + nMcuY -1)/nMcuY;

	int nYBlockPerMcu = YHorzFactor * YVertFactor;
	int nCbBlockPerMcu = CbHorzFactor * CbVertFactor;
	int nCrBlockPerMcu = CrHorzFactor * CrVertFactor;

	int nYBlockPerRow = nMcuPerRow * nYBlockPerMcu;
	int nCbBlockPerRow = nMcuPerRow * nCbBlockPerMcu;
	int nCrBlockPerRow = nMcuPerRow * nCrBlockPerMcu;

	int nBlockPerMcu = nYBlockPerMcu + nCbBlockPerMcu + nCrBlockPerMcu;
	int nBlockPerRow = nYBlockPerRow + nCbBlockPerRow + nCrBlockPerRow;

	//确定要输出的MCU行
	BYTE8 rgOutputRow[JEPG_MAX_MCU_ROW] = {0};
	int nMaxBottom = 0;
	nMaxBottom = nMcuRowCount - 1;
	HV_memset(rgOutputRow, 2, nMcuRowCount);

	//分配MCU行空间
	JPEG_FIXED_TYPE* pMcuRowBuf = (JPEG_FIXED_TYPE*)cStack.StackAlloc(nBlockPerRow * JPEG_DCTSIZE2 * sizeof(JPEG_FIXED_TYPE), TRUE);

	//分配各分量缓存位置
	JPEG_FIXED_TYPE* pYBase = pMcuRowBuf;
	JPEG_FIXED_TYPE* pCbBase = pYBase + nYBlockPerRow * JPEG_DCTSIZE2;
	JPEG_FIXED_TYPE* pCrBase = pCbBase + nCbBlockPerRow * JPEG_DCTSIZE2;

	//MCU块定义
	JPEG_FIXED_TYPE rgLastDC[JPEG_MAXIMUM_COMPONENTS] = {0}; //记录每个分量的直流系数
	JPEG_FIXED_TYPE* rgBufPtr[JPEG_MAXIMUM_COMPONENTS] = { pYBase, pCbBase, pCrBase }; //记录每个分量当前输出位置

	CompInfo rgCompInfo[JPEG_MAXIMUM_SCAN_COMPONENT_FACTORS] = {0};
	int nCompCount = 0;
	for(int i = 0; i < decoder->frame.componentCount; i++)
	{
		JPEG_ScanHeader_Component* sc = &scan->componentList[i];
		JPEG_FrameHeader_Component* fc = frameComponents [i];	//FRAMEHEADER

		for(int j = 0; j < fc->horzFactor * fc->vertFactor; j++)
		{
			rgCompInfo[nCompCount].pDC_Tbl = &dcTableList [sc->dcTable == dcTableUse [1] ? 1 : 0];
			rgCompInfo[nCompCount].pAC_Tbl = &acTableList [sc->acTable == acTableUse [1] ? 1 : 0];

			rgCompInfo[nCompCount].pLastDC = &rgLastDC[fc->selector - 1];
			rgCompInfo[nCompCount].ppBufPtr = &rgBufPtr[fc->selector - 1];

			rgCompInfo[nCompCount].pQuant_Tbl = decoder->quantTables [fc->quantTable];

			nCompCount++;
		}
	}

	//计算缓存行大小
	int nBufHeight = nMcuY>>1; //行数
	int nYBufWidth = nYBlockPerRow * JPEG_DCTSIZE; //宽度
	int nCbBufWidth, nCrBufWidth;
	nCrBufWidth = nCbBufWidth = (nCbBlockPerRow +  1)/2 * horzMax * JPEG_DCTSIZE;
	int nYBufSize = nBufHeight * nYBufWidth;
	int nCbBufSize = nBufHeight * nCbBufWidth;
	int nCrBufSize = nBufHeight * nCrBufWidth;

	//分配缓存行内存
	PBYTE8 pYBuf = (BYTE8*)cStack.StackAlloc(nYBufSize);
	PBYTE8 pCbBuf = (BYTE8*)cStack.StackAlloc(nCbBufSize);
	PBYTE8 pCrBuf = (BYTE8*)cStack.StackAlloc(nCrBufSize);

	int iYDmaHandle = -1, iCbDmaHandle = -1, iCrDmaHandle = -1;
	int nCol(0), nRow(0), c(0), nLineCount(0);

	for( nRow = 0; nRow <= nMaxBottom; nRow++, rgBufPtr[0] = pYBase,  rgBufPtr[1] = pCbBase, rgBufPtr[2] = pCrBase)
	{
		//确定要输出的MCU块
		BYTE8 rgOutputCol[JPEG_MAX_MCU_COL] = {0};
		int nMinLeft = nMcuPerRow - 1;
		int nMaxRight = 0;

		if( rgOutputRow[nRow] > 1 ) //全输出
		{
			nMinLeft = 0;
			nMaxRight = nMcuPerRow - 1;

			HV_memset(rgOutputCol, 1, nMcuPerRow);
		}

		//解压一个MCU行
		for( nCol = 0; nCol < nMcuPerRow; nCol++ )
		{
			//解压一个MCU块
			for( c = 0; c < nBlockPerMcu; (*rgCompInfo[c].ppBufPtr) += JPEG_DCTSIZE2, c++)
			{
				if (!JPEG_DecodeCoefficients_16 (
						rgCompInfo[c].pLastDC,
						*rgCompInfo[c].ppBufPtr,
						rgCompInfo[c].pQuant_Tbl,
						rgCompInfo[c].pDC_Tbl,
						rgCompInfo[c].pAC_Tbl,
						&data,
						&bits_left,
						&bits_data,
						JPEG_ToZigZag,
						rgOutputCol[nCol]
						))
				{
					return 0;
				}
			}

			//处理RST
			if (decoder->restartInterval && --restartInterval == 0)
			{
				restartInterval = decoder->restartInterval;
				JPEG_BITS_REWIND ();
				if ( ((data [0] << 8) | data [1]) == JPEG_Marker_EOI )
				{
					if( rgOutputRow[nRow] > 1)	//全输出
					{
						IMG_idct_8x8_12q4(
							pMcuRowBuf,
							nBlockPerRow
							);
#ifdef CHIP_6467
						if (iYDmaHandle >= 0)
						{
							HV_dmawait_dm6467(iYDmaHandle);
							HV_dmawait_dm6467(iCbDmaHandle);
							HV_dmawait_dm6467(iCrDmaHandle);
							iYDmaHandle = -1;
						}
#endif
						ConvertMCU(
							pYBase,
							pCbBase,
							pCrBase,
							nYBlockPerRow,
							nCbBlockPerRow,
							nCrBlockPerRow,
							pYBuf,
							pCbBuf,
							pCrBuf,
							nYBufWidth,
							nCbBufWidth,
							nCrBufWidth,
							nBufHeight,
							horzMax,
							vertMax
							);
					}
					else if( rgOutputRow[nRow] )
					{
						JPEG_FIXED_TYPE* pYBlockBuf = pYBase + nMinLeft * nYBlockPerMcu * JPEG_DCTSIZE2;
						JPEG_FIXED_TYPE* pCbBlockBuf = pCbBase + nMinLeft * nCbBlockPerMcu * JPEG_DCTSIZE2;
						JPEG_FIXED_TYPE* pCrBlockBuf = pCrBase + nMinLeft * nCrBlockPerMcu * JPEG_DCTSIZE2;
						int nYBlockCount = (nMaxRight - nMinLeft + 1) * nYBlockPerMcu;
						int nCbBlockCount = (nMaxRight - nMinLeft + 1) * nCbBlockPerMcu;
						int nCrBlockCount = (nMaxRight - nMinLeft + 1) * nCrBlockPerMcu;

						IMG_idct_8x8_12q4(
							pYBlockBuf,
							nYBlockCount
							);

						IMG_idct_8x8_12q4(
							pCbBlockBuf,
							nCbBlockCount
							);

						IMG_idct_8x8_12q4(
							pCrBlockBuf,
							nCrBlockCount
							);
#ifdef CHIP_6467
						if (iYDmaHandle >= 0)
						{
							HV_dmawait_dm6467(iYDmaHandle);
							HV_dmawait_dm6467(iCbDmaHandle);
							HV_dmawait_dm6467(iCrDmaHandle);
							iYDmaHandle = -1;
						}
#endif
						ConvertMCU(
							pYBlockBuf,
							pCbBlockBuf,
							pCrBlockBuf,
							nYBlockCount,
							nCbBlockCount,
							nCrBlockCount,
							pYBuf + nMinLeft * nMcuX,
							pCbBuf + nMinLeft * nMcuX/2,
							pCrBuf + nMinLeft * nMcuX/2,
							nYBufWidth,
							nCbBufWidth,
							nCrBufWidth,
							nBufHeight,
							horzMax,
							vertMax
							);
					}

					if(rgOutputRow[nRow])
					{
						CopyLine(
							pYBuf,
							pCbBuf,
							pCrBuf,
							nYBufWidth,
							nCbBufWidth,
							nCrBufWidth,
							nBufHeight,
							iYDmaHandle,
							iCbDmaHandle,
							iCrDmaHandle,
							pImgOut,
							nLineCount,
							pItgArea,
							rgOutputRow[nRow]
							);
					}

					return 1;
				}
				//fixed by liaoy: 存在不规范的JPEG文件, 跳过校验
				//	JPEG_Assert (data [0] == 0xFF && (data [1] >= 0xD0 && data [1] <= 0xD7));
				for (int i = 0; i < JPEG_MAXIMUM_COMPONENTS; i ++)
					rgLastDC[i] = 0;
				data += 2;
			}
		}

		if( rgOutputRow[nRow] == 0) //不输出
		{
			nLineCount += nBufHeight;
			continue;
		}
		else if( rgOutputRow[nRow] > 1)	//全输出
		{
			IMG_idct_8x8_12q4(
				pMcuRowBuf,
				nBlockPerRow
				);
#ifdef CHIP_6467
			if (iYDmaHandle >= 0)
			{
				HV_dmawait_dm6467(iYDmaHandle);
				HV_dmawait_dm6467(iCbDmaHandle);
				HV_dmawait_dm6467(iCrDmaHandle);
				iYDmaHandle = -1;
			}
#endif
			ConvertMCU(
				pYBase,
				pCbBase,
				pCrBase,
				nYBlockPerRow,
				nCbBlockPerRow,
				nCrBlockPerRow,
				pYBuf + nMinLeft * nMcuX,
				pCbBuf + nMinLeft * nMcuX/2,
				pCrBuf + nMinLeft * nMcuX/2,
				nYBufWidth,
				nCbBufWidth,
				nCrBufWidth,
				nBufHeight,
				horzMax,
				vertMax
				);
		}
		else
		{
			JPEG_FIXED_TYPE* pYBlockBuf = pYBase + nMinLeft * nYBlockPerMcu * JPEG_DCTSIZE2;
			JPEG_FIXED_TYPE* pCbBlockBuf = pCbBase + nMinLeft * nCbBlockPerMcu * JPEG_DCTSIZE2;
			JPEG_FIXED_TYPE* pCrBlockBuf = pCrBase + nMinLeft * nCrBlockPerMcu * JPEG_DCTSIZE2;
			int nYBlockCount = (nMaxRight - nMinLeft + 1) * nYBlockPerMcu;
			int nCbBlockCount = (nMaxRight - nMinLeft + 1) * nCbBlockPerMcu;
			int nCrBlockCount = (nMaxRight - nMinLeft + 1) * nCrBlockPerMcu;

			IMG_idct_8x8_12q4(
				pYBlockBuf,
				nYBlockCount
				);

			IMG_idct_8x8_12q4(
				pCbBlockBuf,
				nCbBlockCount
				);

			IMG_idct_8x8_12q4(
				pCrBlockBuf,
				nCrBlockCount
				);
#ifdef CHIP_6467
			if (iYDmaHandle >= 0)
			{
				HV_dmawait_dm6467(iYDmaHandle);
				HV_dmawait_dm6467(iCbDmaHandle);
				HV_dmawait_dm6467(iCrDmaHandle);
				iYDmaHandle = -1;
			}
#endif
			ConvertMCU(
				pYBlockBuf,
				pCbBlockBuf,
				pCrBlockBuf,
				nYBlockCount,
				nCbBlockCount,
				nCrBlockCount,
				pYBuf + nMinLeft * nMcuX,
				pCbBuf + nMinLeft * nMcuX / 2,
				pCrBuf + nMinLeft * nMcuX / 2,
				nYBufWidth,
				nCbBufWidth,
				nCrBufWidth,
				nBufHeight,
				horzMax,
				vertMax
				);
		}

		CopyLine(
			pYBuf,
			pCbBuf,
			pCrBuf,
			nYBufWidth,
			nCbBufWidth,
			nCrBufWidth,
			nBufHeight,
			iYDmaHandle,
			iCbDmaHandle,
			iCrDmaHandle,
			pImgOut,
			nLineCount,
			pItgArea,
			rgOutputRow[nRow]
			);
	}

	return 1;
}

