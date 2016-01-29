#ifndef JPEG_DECODE_H__
#define JPEG_DECODE_H__

#include "Jpeg_Decode_Def.h"

enum IMAGE_TYPE
{
	IMAGE_JPEG = 0,
	IMAGE_YUV
};

struct JPEG_HuffmanTable
{
	const unsigned char* huffval;
	int maxcode[16];
	const unsigned char* valptr[16];
	unsigned char look_nbits[256];
	unsigned char look_sym[256];
};

typedef struct _COMPONENT_IMAGE
{
	unsigned char* pImageData;
	unsigned char* pBuffer1;
	unsigned char* pBuffer2;
	unsigned char* pBuffer3;
	int iWidth;
	int iHeight;
	IMAGE_TYPE eImageType;
}COMPONENT_IMAGE;

struct JPEG_FrameHeader_Component
{
	unsigned char selector;
	unsigned char horzFactor;
	unsigned char vertFactor;
	unsigned char quantTable;
};

struct JPEG_FrameHeader
{
	JPEG_Marker marker;
	int encoding;
	char differential;
	unsigned char precision;
	unsigned short height;
	unsigned short width;
	JPEG_FrameHeader_Component componentList[JPEG_MAXIMUM_COMPONENTS];
	int componentCount;
};

struct JPEG_ScanHeader_Component
{
	unsigned char selector;
	unsigned char dcTable;
	unsigned char acTable;
};

struct JPEG_ScanHeader
{
	JPEG_ScanHeader_Component componentList[JPEG_MAXIMUM_COMPONENTS];
	int componentCount;
	unsigned char spectralStart;
	unsigned char spectralEnd;
	unsigned char successiveApproximationBitPositionHigh; 
	unsigned char successiveApproximationBitPositionLow; 
};

struct JPEG_Decoder
{
	const unsigned char* acTables[4];
	const unsigned char* dcTables[4];
	JPEG_FIXED_TYPE quantTables[4][JPEG_DCTSIZE2];
	unsigned int restartInterval;
	JPEG_FrameHeader frame;
	JPEG_ScanHeader scan;
};

typedef struct _CompInfo
{
	JPEG_FIXED_TYPE* pLastDC;
	JPEG_FIXED_TYPE** ppBufPtr;
	JPEG_HuffmanTable* pDC_Tbl;
	JPEG_HuffmanTable* pAC_Tbl;
	JPEG_FIXED_TYPE* pQuant_Tbl;
}CompInfo;

int Jpeg_Decoder_ReadHeaders(JPEG_Decoder* decoder, const unsigned char** dataBase);

int Jpeg_Decoder_ReadImage_ConvertToYuv(JPEG_Decoder* decoder, 
										const unsigned char** dataBase,
										COMPONENT_IMAGE* pImgOut);

#endif
