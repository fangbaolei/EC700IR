#include "StringOverlay.h"
#include "ascii_12x24.h"

#define YUV_NUM_128	2097152 /* 128 << 14 */
#define Rgb2Yuv(pbR, pbG, pbB, pbY, pbU, pbV) \
	pbY = (  1868 * pbB + 9617 * pbG + 4899 * pbR ) >> 14; \
	pbU = (  8192 * pbB - 5428 * pbG - 2758 * pbR + YUV_NUM_128 ) >> 14; \
	pbV = ( -1332 * pbB - 6860 * pbG + 8192 * pbR + YUV_NUM_128 ) >> 14; \

// 字体亮度与背景色形成反差
#define ChangeYValue(iBackgroundY) ( (iBackgroundY + 128 >= 255) ? (iBackgroundY + 128 - 255) : (iBackgroundY + 128 ) )

// { --- 字符叠加功能使用到的全局变量 [BEGIN] ---
static int g_iStringOverlayX = 0;
static int g_iStringOverlayY = 0;

static int g_iStringOverlayW = 0;
static int g_iStringOverlayH = 0;

static PBYTE8 g_rgbStringOverlayDateTimeDotMatrix = NULL;
static PBYTE8 g_rgbStringOverlayDateTimeDotMatrixBuffer[16*1024]; // 16KB
static PBYTE8 g_rgbStringOverlayMessageDotMatrix  = NULL;

static BYTE8 g_bStringOverlayYData                = NULL;
static BYTE8 g_bStringOverlayUData                = NULL;
static BYTE8 g_bStringOverlayVData                = NULL;

static BOOL g_fStringOverlayInit                  = FALSE;

static int g_iIsFixedLight                        = 0;	// 字体亮度固定
static BOOL g_fIsSideInstall                      = 0;	// 侧装
static BOOL g_fDoubleStream                       = 0;	// 双码流
// --- 字符叠加功能使用到的全局变量 [END] --- }

// 初始化字符叠加必要参数
HRESULT InitStringOverlay(
	const STRING_OVERLAY_PARAM* pStringOverlayParam,
	const STRING_OVERLAY_DATA*  pStringOverlayData,
	STRING_OVERLAY_RESPOND* pStringOverlayRespond
)
{
	SetOverlayMessageContent(
		pStringOverlayParam->x,
		pStringOverlayParam->y,
		pStringOverlayParam->w,
		pStringOverlayParam->h,
		pStringOverlayParam->iIsFixedLight,
		pStringOverlayParam->iFontColor,
		pStringOverlayParam->fIsSideInstall,
		(PBYTE8)pStringOverlayData->rgDotMatrix.phys
	);
	pStringOverlayRespond->dwExtErrCode = S_OK;
	return S_OK;	
}

// 创建叠加日期时间内容
static void CreateOverlayDateTimeContent(
	const PBYTE8 szDateTimeStrings,
	int* piW, int* piH
)
{
	int i,j;
	int len = strlen((const char *)szDateTimeStrings);

	int w = 0;
	int h = 0;

	// 跟据szDateTimeStrings中的内容，求出其w和h
	w = len * 12;
	h = g_iStringOverlayH;

	*piW = w;
	*piH = h;

	/*
	// 1
	if ( NULL == g_rgbStringOverlayDateTimeDotMatrix )
	{
		g_rgbStringOverlayDateTimeDotMatrix = (PBYTE8)malloc(w*h*sizeof(BYTE8));
	}
	*/

	/*
	// 2
	if ( NULL != g_rgbStringOverlayDateTimeDotMatrix )
	{
		free( g_rgbStringOverlayDateTimeDotMatrix );
	}
	g_rgbStringOverlayDateTimeDotMatrix = (PBYTE8)malloc(w*h*sizeof(BYTE8));
	*/

	///*
	// 3
	// 注：此处需满足 w*h*sizeof(BYTE8) < sizeof(g_rgbStringOverlayDateTimeDotMatrixBuffer) 这个条件
	g_rgbStringOverlayDateTimeDotMatrix = (PBYTE8)g_rgbStringOverlayDateTimeDotMatrixBuffer;
	//*/

	// 跟据szDateTimeStrings中的内容，求出其点阵，并填充到g_rgbStringOverlayDateTimeDotMatrix

	for ( i = 0; i < len; ++i )
	{
		char* p = (char*)ascii_fontdata24 + ((szDateTimeStrings[i] - 0x20) * HALFWIDTH_SIZE);

		for ( j = 0; j < 12; ++j )
		{
			int iDstLine = 0;

			char c0 = p[0];
			char c1 = p[1];
			char c2 = p[2];

			#define BIT0 0x01
			#define BIT1 0x02
			#define BIT2 0x04
			#define BIT3 0x08
			#define BIT4 0x10
			#define BIT5 0x20
			#define BIT6 0x40
			#define BIT7 0x80

			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT0); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT1); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT2); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT3); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT4); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT5); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT6); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c0&BIT7); ++iDstLine;

			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT0); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT1); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT2); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT3); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT4); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT5); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT6); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c1&BIT7); ++iDstLine;

			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT0); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT1); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT2); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT3); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT4); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT5); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT6); ++iDstLine;
			g_rgbStringOverlayDateTimeDotMatrix[w*iDstLine + j + 12*i] = (c2&BIT7); ++iDstLine;

			p += 3;
		}
	}
}

// 字符叠加，对应YUV422P色彩格式
static void StringOverlayYUV422P(
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbU, int iUStride,
	PBYTE8 pbV, int iVStride,
	int x, int y,
	int w, int h,
	const PBYTE8 rgbDotMatrix,
	BYTE8 bYData,
	BYTE8 bUData,
	BYTE8 bVData,
	int iImageWidth, int iImageHeight
)
{
	int i=0,j=0,k=0;
	int iBackgroundY = 0;
	int iDeltaY = bYData;

	PBYTE8 pbDstY = pbY + y*iYStride + x;
	PBYTE8 pbDstU = pbU + y*iUStride + (x>>1);
	PBYTE8 pbDstV = pbV + y*iVStride + (x>>1);

	for ( i = 0; i < h && (i+y) < iImageHeight; ++i )
	{
		for ( j = 0, k = 0; j < w && (j+x) < iImageWidth; j+=2, ++k )
		{
			if ( rgbDotMatrix[i*w + j] != 0 )
			{
				if (g_iIsFixedLight == 0)
				{
					iBackgroundY = pbDstY[i*iYStride + j];
					iDeltaY = ChangeYValue(iBackgroundY);
				}
				pbDstY[i*iYStride + j] = iDeltaY;
				pbDstU[i*iUStride + k] = bUData;
			}
			if ( rgbDotMatrix[i*w + j + 1] != 0 )
			{
				if (g_iIsFixedLight == 0)
				{
					iBackgroundY = pbDstY[i*iYStride + j + 1];
					iDeltaY = ChangeYValue(iBackgroundY);
				}
				pbDstY[i*iYStride + j + 1] =  iDeltaY;
				pbDstV[i*iVStride + k] = bVData;
			}
		}
	}
}

// 字符叠加，对应YUV422SP色彩格式
static void StringOverlayYUV422SP(
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbC, int iCStride,
	int x, int y,
	int w, int h,
	const PBYTE8 rgbDotMatrix,
	BYTE8 bYData,
	BYTE8 bUData,
	BYTE8 bVData,
	int iImageWidth, int iImageHeight
)
{
	int i=0,j=0;
	int iBackgroundY = 0;
	int iDeltaY = bYData;

	PBYTE8 pbDstY = pbY + y*iYStride + x;
	PBYTE8 pbDstC = pbC + y*iCStride + x;

	for ( i = 0; i < h && (i+y) < iImageHeight; ++i )
	{
		for ( j = 0; j < w && (j+x) < iImageWidth; j+=2 )
		{			
			if ( rgbDotMatrix[i*w + j] != 0 )
			{
				if (g_fIsSideInstall || !g_fDoubleStream)
				{
					if (g_iIsFixedLight == 0)
					{
						iBackgroundY = pbDstY[i*iYStride + j];
						iDeltaY = ChangeYValue(iBackgroundY);
					}
					pbDstY[i*iYStride + j] =  iDeltaY;
				}
				pbDstC[i*iCStride + j] = bUData;
			}
			if ( rgbDotMatrix[i*w + j + 1] != 0 )
			{
				if (g_fIsSideInstall || !g_fDoubleStream)
				{
					if (g_iIsFixedLight == 0)
					{
						iBackgroundY = pbDstY[i*iYStride + j + 1];
						iDeltaY = ChangeYValue(iBackgroundY);
					}
					pbDstY[i*iYStride + j + 1] =  iDeltaY;
				}
				pbDstC[i*iCStride + j + 1] = bVData;
			}
		}
	}
}

// 字符叠加，对应YUV420P色彩格式
static void StringOverlayYUV420P(
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbU, int iUStride,
	PBYTE8 pbV, int iVStride,
	int x, int y,
	int w, int h,
	const PBYTE8 rgbDotMatrix,
	BYTE8 bYData,
	BYTE8 bUData,
	BYTE8 bVData,
	int iImageWidth, int iImageHeight
)
{
	int i=0,j=0,m=0,n=0;
	int iBackgroundY = 0;
	int iDeltaY = bYData;

	PBYTE8 pbDstY = pbY + y*iYStride + x;
	PBYTE8 pbDstU = pbU + (y>>1)*iUStride + (x>>1);
	PBYTE8 pbDstV = pbV + (y>>1)*iVStride + (x>>1);

	for ( i = 0, m = 0; i < h && (i+y) < iImageHeight; i+=2, ++m )
	{
		for ( j = 0, n = 0; j < w && (j+x) < iImageWidth; j+=2, ++n )
		{
			if ( rgbDotMatrix[i*w + j] != 0 )
			{
				if (g_iIsFixedLight == 0)
				{
					iBackgroundY = pbDstY[i*iYStride + j];
					iDeltaY = ChangeYValue(iBackgroundY);
				}
				pbDstY[i*iYStride + j] =  iDeltaY;
				pbDstU[m*iUStride + n] = bUData;
			}
			if ( rgbDotMatrix[i*w + j + 1] != 0 )
			{
				if (g_iIsFixedLight == 0)
				{
					iBackgroundY = pbDstY[i*iYStride + j + 1];
					iDeltaY = ChangeYValue(iBackgroundY);
				}
				pbDstY[i*iYStride + j + 1] =  iDeltaY;
			}

			if ( rgbDotMatrix[(i+1)*w + j] != 0 )
			{
				if (g_iIsFixedLight == 0)
				{
					iBackgroundY = pbDstY[(i+1)*iYStride + j];
					iDeltaY = ChangeYValue(iBackgroundY);
				}
				pbDstY[(i+1)*iYStride + j] =  iDeltaY;
				pbDstV[m*iVStride + n] = bVData;
			}
			if ( rgbDotMatrix[(i+1)*w + j + 1] != 0 )
			{
				if (g_iIsFixedLight == 0)
				{
					iBackgroundY = pbDstY[(i+1)*iYStride + j + 1];
					iDeltaY = ChangeYValue(iBackgroundY);
				}
				pbDstY[(i+1)*iYStride + j + 1] =  iDeltaY;
			}
		}
	}
}

// 函数功能：设置待叠加信息字符串的相关内容
// x，y：在图片中开始叠加信息字符串的起始位置
// w，h：待叠加信息字符串的点阵的总体宽高（以像素点为单位）
// 字体亮度固定
// iFontColor：待叠加信息字符串的字体颜色
// rgbDotMatrix：待叠加信息字符串的点阵数据
void SetOverlayMessageContent(
	int x, int y,
	int w, int h,
	int iIsFixedLight,
	int iFontColor,
	BOOL  fIsSideInstall,
	const PBYTE8 rgbDotMatrix
)
{
	BYTE8 bFontR;
	BYTE8 bFontG;
	BYTE8 bFontB;
	//if ( FALSE == g_fStringOverlayInit )
	{
		g_iStringOverlayX = ((x + 7)/8) * 8;
		g_iStringOverlayY = ((y + 7)/8) * 8;
		g_iStringOverlayW = w;
		g_iStringOverlayH = h;
		g_iIsFixedLight   = iIsFixedLight;
		g_fIsSideInstall  = fIsSideInstall;

		if ( NULL != g_rgbStringOverlayMessageDotMatrix )
		{
			free( g_rgbStringOverlayMessageDotMatrix );
		}
		g_rgbStringOverlayMessageDotMatrix = (PBYTE8)malloc(w*h*sizeof(BYTE8));

		memcpy(g_rgbStringOverlayMessageDotMatrix, rgbDotMatrix, w*h*sizeof(BYTE8));

		bFontR = (iFontColor & 0x00FF0000) >> 16;
		bFontG = (iFontColor & 0x0000FF00) >> 8;
		bFontB = iFontColor  & 0x000000FF;
		Rgb2Yuv(bFontR,bFontG,bFontB, g_bStringOverlayYData,g_bStringOverlayUData,g_bStringOverlayVData);

		g_fStringOverlayInit = TRUE;
	}
}

// 叠加字符串到200w的Jpeg图片中
void DoStringOverlayYUV422P(
	const PBYTE8 szDateTimeStrings,
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbU, int iUStride,
	PBYTE8 pbV, int iVStride,
	int iImageWidth, int iImageHeight
)
{
	int w = 0;
	int h = 0;

	if ( strlen((const char *)szDateTimeStrings) <= 0 || FALSE == g_fStringOverlayInit )
	{
		return;
	}

	CreateOverlayDateTimeContent(
		szDateTimeStrings,
		&w, &h
	);

	// 叠加日期时间到图片中
	StringOverlayYUV422P(
		pbY, iYStride,
		pbU, iUStride,
		pbV, iVStride,
		g_iStringOverlayX, g_iStringOverlayY,
		w, h,
		g_rgbStringOverlayDateTimeDotMatrix,
		g_bStringOverlayYData,
		g_bStringOverlayUData,
		g_bStringOverlayVData,
		iImageWidth, iImageHeight
	);

	// 叠加信息字符串到图片中
	StringOverlayYUV422P(
		pbY, iYStride,
		pbU, iUStride,
		pbV, iVStride,
		g_iStringOverlayX + w, g_iStringOverlayY,
		g_iStringOverlayW, g_iStringOverlayH,
		g_rgbStringOverlayMessageDotMatrix,
		g_bStringOverlayYData,
		g_bStringOverlayUData,
		g_bStringOverlayVData,
		iImageWidth, iImageHeight
	);
}

// 叠加字符串到H.264码流中
void DoStringOverlayYUV422SP(
	const PBYTE8 szDateTimeStrings,
	BOOL fDoubleStream,
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbC, int iCStride,
	int iImageWidth, int iImageHeight
)
{
	int w = 0;
	int h = 0;

	g_fDoubleStream = fDoubleStream;

    // 注：不进行坐标缩放
	/*
	float fScaleX = 1600.0f/1600;
	float fScaleY = 1088.0f/1200;

	int x = g_iStringOverlayX*fScaleX;
	int y = g_iStringOverlayY*fScaleY;

	x = ((x + 7)/8)*8;
	y = ((y + 7)/8)*8;
	*/

    int x = g_iStringOverlayX;
    int y = g_iStringOverlayY;

	if (g_fIsSideInstall)
	{
		y = y / 2;
	}

	if ( strlen((const char *)szDateTimeStrings) <= 0 || FALSE == g_fStringOverlayInit )
	{
		return;
	}

	CreateOverlayDateTimeContent(
		szDateTimeStrings,
		&w, &h
	);

	// 叠加日期时间到图片中
	StringOverlayYUV422SP(
		pbY, iYStride,
		pbC, iCStride,
		x, y, 
		w, h,
		g_rgbStringOverlayDateTimeDotMatrix,
		g_bStringOverlayYData,
		g_bStringOverlayUData,
		g_bStringOverlayVData,
		iImageWidth, iImageHeight
	);

	// 叠加信息字符串到图片中
	StringOverlayYUV422SP(
		pbY, iYStride,
		pbC, iCStride,
		x + w, y, //JPEG的图像高度是H264的两倍，所以要除以2
		g_iStringOverlayW, g_iStringOverlayH,
		g_rgbStringOverlayMessageDotMatrix,
		g_bStringOverlayYData,
		g_bStringOverlayUData,
		g_bStringOverlayVData,
		iImageWidth, iImageHeight
	);
}

// 叠加字符串到500w的Jpeg图片中
void DoStringOverlayYUV420P(
	const PBYTE8 szDateTimeStrings,
	PBYTE8 pbY, int iYStride,
	PBYTE8 pbU, int iUStride,
	PBYTE8 pbV, int iVStride,
	int iImageWidth, int iImageHeight
)
{
	int w = 0;
	int h = 0;

	if ( strlen((const char *)szDateTimeStrings) <= 0 || FALSE == g_fStringOverlayInit )
	{
		return;
	}

	CreateOverlayDateTimeContent(
		szDateTimeStrings,
		&w, &h
	);

	// 叠加日期时间到图片中
	StringOverlayYUV420P(
		pbY, iYStride,
		pbU, iUStride,
		pbV, iVStride,
		g_iStringOverlayX, g_iStringOverlayY,
		w, h,
		g_rgbStringOverlayDateTimeDotMatrix,
		g_bStringOverlayYData,
		g_bStringOverlayUData,
		g_bStringOverlayVData,
		iImageWidth, iImageHeight
	);

	// 叠加信息字符串到图片中
	StringOverlayYUV420P(
		pbY, iYStride,
		pbU, iUStride,
		pbV, iVStride,
		g_iStringOverlayX + w, g_iStringOverlayY,
		g_iStringOverlayW, g_iStringOverlayH,
		g_rgbStringOverlayMessageDotMatrix,
		g_bStringOverlayYData,
		g_bStringOverlayUData,
		g_bStringOverlayVData,
		iImageWidth, iImageHeight
	);
}

