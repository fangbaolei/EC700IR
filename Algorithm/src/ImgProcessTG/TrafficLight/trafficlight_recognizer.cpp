#include ".\trafficlight_recognizer.h"

#include <math.h>
//#include <assert.h>

#include "swimageobj.h"
#include "../Inc/HvImageNew.h"
#include "hvutils.h"
#include "findBlobs.h"
#include "fast_stack.h"
/*
#ifndef _WIN32
extern "C" void IMG_median_3x3_8_cn (const unsigned char* i_data, int n, unsigned char* o_data);
#else
*/
void IMG_median_3x3_8_cn (const unsigned char* i_data, int n, unsigned char* o_data)
{
	unsigned char c0h, c1h, c2h, h_min;  /* "hi",  columns 0..2, and "min" */
	unsigned char c0m, c1m, c2m, m_mid;  /* "mid", columns 0..2, and "mid" */
	unsigned char c0l, c1l, c2l, l_max;  /* "lo",  columns 0..2, and "max" */
	unsigned char m_h, m_l, tmp, out;

	int i;

	/* -------------------------------------------------------------------- */
	/*  Start off with a well-defined initial state.                        */
	/* -------------------------------------------------------------------- */
	c1h = c2h = c1m = c2m = c1l = c2l = 127;

	/* -------------------------------------------------------------------- */
	/*  Iterate over the input row.                                         */
	/* -------------------------------------------------------------------- */
	for (i = 0; i < n; i++)
	{
		/* ---------------------------------------------------------------- */
		/*  Slide our two previous columns of sorted pixels over by 1.      */
		/* ---------------------------------------------------------------- */
		c0h = c1h;    c1h = c2h;
		c0m = c1m;    c1m = c2m;
		c0l = c1l;    c1l = c2l;

		/* ---------------------------------------------------------------- */
		/*  Load in a new column of pixels, and sort into lo, med, high.    */
		/* ---------------------------------------------------------------- */
		c2h = i_data[i      ];
		c2m = i_data[i +   n];
		c2l = i_data[i + 2*n];

		if (c2l > c2h) { tmp = c2l; c2l = c2h; c2h = tmp; }
		if (c2l > c2m) { tmp = c2l; c2l = c2m; c2m = tmp; }
		if (c2m > c2h) { tmp = c2m; c2m = c2h; c2h = tmp; }

		/* ---------------------------------------------------------------- */
		/*  Find the minimum value of the "hi" terms.                       */
		/* ---------------------------------------------------------------- */
		h_min = c2h;
		if (c1h < h_min) { h_min = c1h; }
		if (c0h < h_min) { h_min = c0h; }

		/* ---------------------------------------------------------------- */
		/*  Find the middle value of the "mid" terms.                       */
		/* ---------------------------------------------------------------- */
		m_l   = c0m;
		m_mid = c1m;
		m_h   = c2m;

		if (m_l   > m_h  ) { tmp = m_l; m_l = m_h; m_h = tmp; }
		if (m_l   > m_mid) { m_mid = m_l; }
		if (m_mid > m_h  ) { m_mid = m_h; }

		/* ---------------------------------------------------------------- */
		/*  Find the maximum value of the "lo" terms.                       */
		/* ---------------------------------------------------------------- */
		l_max = c2l;
		if (c1l > l_max) { l_max = c1l; }
		if (c0l > l_max) { l_max = c0l; }

		/* ---------------------------------------------------------------- */
		/*  Find the middle value of "h_min", "m_mid", "l_max" into "out".  */
		/* ---------------------------------------------------------------- */
		out = m_mid;

		if (h_min > l_max) { tmp   = h_min; h_min = l_max; l_max = tmp; }
		if (h_min > out  ) { out   = h_min; }
		if (out   > l_max) { out   = l_max; }

		/* ---------------------------------------------------------------- */
		/*  Store the resulting pixel.                                      */
		/* ---------------------------------------------------------------- */
		o_data[i] = out;
	}
}    
//#endif

namespace 
{

using namespace signalway ;

// createRect
HV_RECT createRect(int x, int y, int width, int height)
{
	HV_RECT rc ;
	
	rc.left   = x ;
	rc.top    = y ;
	rc.right  = x + width - 1 ;
	rc.bottom = y + height - 1 ;

	return rc ;

} // createRect

// createSize
HV_SIZE createSize(int width, int height)
{
	HV_SIZE sz ;
	sz.cx = width ;
	sz.cy = height ;

	return sz ;

} // createSize

// calcSize
HV_SIZE calcSize(const HV_RECT& rc)
{
	HV_SIZE sz ;
	sz.cx = rc.right - rc.left + 1 ;
	sz.cy = rc.bottom - rc.top + 1 ;

	return sz ;

} // calcSize

// calcArea
float calcArea(const HV_RECT& rc)
{
	HV_SIZE rcSize = calcSize(rc) ;

	return (float )rcSize.cx * rcSize.cy ;

} // calcArea

// calcCenter
HV_POINT calcCenter(const HV_RECT& rc)
{
	HV_SIZE sz = calcSize(rc) ;
	
	HV_POINT center ;
	center.x = rc.left + sz.cx / 2 ;
	center.y = rc.top  + sz.cy / 2 ;

	return center ;

} // calcCenter

// calcCenter
HV_POINT calcCenter(const signalway::blob& b)
{
	HV_RECT rc = calcBoundingRect(b) ;
	return calcCenter(rc) ;

} // calcCenter


bool contains(const HV_RECT& rc, const HV_POINT& pt)
{
	if (pt.x >= rc.left && pt.x <= rc.right
		&& pt.y >= rc.top && pt.y <= rc.bottom)
	{
		return true ;
	}

	return false ;
}

// moveCenter
HV_RECT moveCenter(const HV_RECT& rc, HV_POINT newCenter)
{
	HV_SIZE sz = calcSize(rc) ;
	int halfW = (int)((float)sz.cx / 2 + 0.5 );
	int halfH = (int)((float)sz.cy / 2 + 0.5 );
	int x = rc.left - halfW ;
	int y = rc.top - halfH ;

	return createRect(x, y, sz.cx, sz.cy) ;

} // moveCenter

// adjustRect
void adjustRect(HV_RECT& rc, float horzScale, float vertScale, 
				int xOff, int yOff, int width, int height)
{

	HV_RECT adjRc = rc ;
	adjRc.left   = adjRc.left >= 0 ? adjRc.left : 0 ;
	adjRc.top    = adjRc.top  >= 0 ? adjRc.top  : 0 ;
	adjRc.right  = adjRc.right < width ? adjRc.right : (width-1) ;
	adjRc.bottom = adjRc.bottom < height ? adjRc.bottom : (height-1) ;
	adjRc.left   = rc.left / horzScale + xOff ;
	adjRc.right  = rc.right / horzScale + xOff ;
	adjRc.top    = rc.top / vertScale + yOff ;
	adjRc.bottom = rc.bottom / vertScale + yOff ;

	rc = adjRc ;

} // adjustRect

void adjustRectSize(HV_RECT& rc, int width, int height)
{
	int halfW = width / 2 ;
	int halfH = height / 2 ;

	HV_POINT center = calcCenter(rc) ;
	rc.left   = center.x - halfW ;
	rc.top    = center.y - halfH ;
	rc.right  = center.x + halfW ;
	rc.bottom = center.y + halfH ;

}

// convertToGray
void convertToGray(const CPersistentComponentImage& src, 
				   CPersistentComponentImage& dest) 
{
	if (src.nImgType == HV_IMAGE_YUV_422 || src.nImgType == HV_IMAGE_GRAY || src.nImgType == HV_IMAGE_YCbYCr)
	{
		dest.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;

		memcpy(GetHvImageData(&dest, 0), 
			GetHvImageData(&src, 0), 
			sizeof(char)*src.iHeight*src.iStrideWidth[0]) ;
	}
	else
	{
		// noly YUV422 and gray image supported
		//HV_Trace("Error: Unsupported image format.") ;
	}

} // convertToGray

// convertYUV2BGR
void convertYUV2BGR(const CPersistentComponentImage& src, CPersistentComponentImage& b,
					CPersistentComponentImage& g,
					CPersistentComponentImage& r)
{
	if (src.iWidth == 0 || src.iHeight == 0 || src.nImgType != HV_IMAGE_YUV_422)
	{
		return ;
	}

	CPersistentComponentImage rImage ;
	CPersistentComponentImage gImage ;
	CPersistentComponentImage bImage ;

	rImage.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;
	gImage.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;
	bImage.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;

	IMAGE_ConvertYCbCr2BGR(&src, GetHvImageData(&rImage, 0), GetHvImageData(&gImage, 0), GetHvImageData(&bImage , 0), src.iStrideWidth[0]) ;

	b.Assign(bImage) ;
	g.Assign(gImage) ;
	r.Assign(rImage) ;

} // convertYUV2BGR

// adaptiveThreshold
float adaptiveThreshold(const CPersistentComponentImage& src, 
						CPersistentComponentImage& dest)
{	
	
	//assert( src.iWidth > 0 && src.iHeight > 0 ) ;
	if ( src.iWidth <= 0 || src.iHeight <= 0 )
		return 0.0;

	int width  = src.iWidth ;
	int height = src.iHeight ;

	//CPersistentComponentImage gray ;
	//convertToGray(src, gray) ;

	float hist[256] ;  // histogram
	for (int i = 0; i < 256; ++i)
	{
		hist[i] = 0.0f ;
	} // end of for (int i = 0; i < 256; ++i)
	
	// calculate the histogram
	unsigned char* data = GetHvImageData(&src , 0);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			hist[data[x]] ++ ;

		} // end of for (int x = 0; x < width; ++x)

		data += src.iStrideWidth[0] ;
	} // end of for (int y = 0; y < height; ++y)


	// normalize the histogram
	float size = width * height ;
	for (int i = 0; i < 256; ++i)
	{
		hist[i] /= size ;
	} // end of for (int i = 0; i < 256; ++i)

	
	// calculate the mean and variance of histogram
	float mean = 0.0f ; // mean of histogram
	for (int i = 0; i < 256; ++i)
	{
		mean += i * hist[i] ;
	} // end of for (int i = 0; i < 256; ++i)

	float squardMean = mean * mean ;
	float var = 0.0f ;  // variance
	for (int i = 0; i < 256; ++i)
	{
		var += fabs(i*i*hist[i] - squardMean) ;
	}

	// calculate the threshold
	int thres = 20 ;
	float maxVal = 0.0f ;
	float p = 0.0f ;
	float a = 0.0f ;

	for (int i = 0; i < 256; ++i)
	{
		p += hist[i] ;
		a += i * hist[i] ;

		float diff = mean*p-a ;

		float s = diff * diff / p / (1.0f - p) ;

		if (s > maxVal)
		{
			thres = i ;
			maxVal = s ;

		} // end of if (s > maxVal)

	} // end of for (int i = 0; i < 256; ++i)

	if (dest.iWidth != width || dest.iHeight != height || dest.nImgType != HV_IMAGE_GRAY)
	{
		dest.Create(HV_IMAGE_GRAY, width, height) ;
	}
	
	hvThreshold(&src, &dest, thres, 255, HV_THRESH_BINARY) ;

	return (float)thres ;

} // adaptiveThreshold

// integral_image
class integral_image
{
public:
	integral_image()
		: width_(0)
		, height_(0)
		, data_(0)
	{ }

	integral_image(int width, int height)
		: width_(width)
		, height_(height)
		, data_(new int[(width)*(height)])
	{ }

	~integral_image()
	{
		delete [] data_ ;
	}

	void create(int width, int height)
	{
		int* ptr = new int[width*height] ;
		if (!ptr)
		{
			return ;
		}

		delete [] data_ ;
		data_   = ptr ;
		width_  = width ;
		height_ = height ;

	}

	void zero()
	{
		if (data_)
		{
			memset((void*)data_, 0, sizeof(int)*width_*height_) ;
		}
	}

	int sum(int l, int t, int w, int h)
	{
		int s = 0, area = 0 ;
		sum(l, t, w, h, s, area) ;

		return s ;
	}

	void sum(int l, int t, int r, int b, int& s, int& area)
	{
		int rcLeft = MAX(l, 0) ;
		int rcTop  = MAX(t, 0) ;
		int rcRight = MIN(r+1, width_-1) ;
		int rcBottom = MIN(b+1, height_-1) ;

		int offset1 = rcTop * width_ ;
		int offset2 = (rcBottom - rcTop) * width_ ;
		int* pt0 = data_ + offset1 + rcLeft ;
		int* pt1 = data_ + offset1 + rcRight ;
		int* pt2 = pt1 + offset2 ;
		int* pt3 = pt0 + offset2 ;

		s = (*pt2)-(*pt1)-(*pt3)+(*pt0) ;
		area = (rcRight - rcLeft) * (rcBottom - rcTop) ;

	}

	float mean(int l, int t, int w, int h)
	{
		int s = 0, area = 0 ;
		sum(l, t, w, h, s, area) ;
		return area > 0 ? ((float)s / area) : 0 ;
	}

public:
	int     width_ ;
	int     height_ ;
	int*    data_ ;

}; // integral_image


// createIntegral
void createIntegral(const CPersistentComponentImage& src, integral_image& integral)
{
	int imageWidth = src.iWidth ;
	int imageHeight = src.iHeight ;

	if (integral.width_ != (imageWidth + 1) || integral.height_ != (imageHeight + 1))
	{
		integral.create(imageWidth+1, imageHeight+1) ;
	}

	integral.zero() ;

	int* itgLastLine = integral.data_ ;
	int* itgThisLine = itgLastLine + integral.width_ ;
	unsigned char* imageThisLine = GetHvImageData(&src, 0);

	for (int y = 0; y < imageHeight; ++y)
	{
		int prevSum = 0 ;
		for (int x = 0; x < imageWidth; ++x)
		{
			prevSum += imageThisLine[x] ;
			itgThisLine[x+1] = prevSum + itgLastLine[x+1] ;

		} // end of for (int x = 0; x < imageWidth; ++x)

		imageThisLine += src.iStrideWidth[0] ;
		itgLastLine = itgThisLine ;
		itgThisLine += integral.width_ ;

	} // end of for (int y = 0; y < imageHeight; ++y)

} // createIntegral

// adaptiveThreshold: local mean algorithm
void adaptiveThreshold(const CPersistentComponentImage& src, 
					   CPersistentComponentImage& dest, 
					   int kernelSize, float thres)
{
	int width = src.iWidth ;
	int height = src.iHeight ;
	if (width <= 0 || height <= 0/* || src.pbData[0] == dest.pbData[0]*/)
	{
		return ;
	}

	// calculate integral image
	integral_image itgl ;
	createIntegral(src, itgl) ;

	CPersistentComponentImage result ;  
	if (result.iWidth != width || result.iHeight != height)
	{
		result.Create(HV_IMAGE_GRAY, width, height) ;
	}

	int halfSize = kernelSize / 2 ;
	unsigned char* srcrow = GetHvImageData(&src, 0);
	unsigned char* dstrow = GetHvImageData(&result, 0);
	int step = src.iStrideWidth[0] ;

	float t = (float)((100.0 - thres) / 100 );

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			//float m = itgl.mean(x-halfSize, y-halfSize, 
			//	x+halfSize, y+halfSize) ;

			int s = 0, area = 0 ;
			itgl.sum(x-halfSize, y-halfSize, 
				x+halfSize, y+halfSize, s, area) ;

			if (srcrow[x]*area < t*s)
			{
				dstrow[x] = 0 ;
			}
			else
			{
				dstrow[x] = 255 ;
			}

		} // end of for (int x = 0; x < width; ++x)

		dstrow += step ;
		srcrow += step ;

	} // end of for (int y = 0; y < height; ++y)

	dest.Assign(result) ;

} // adaptiveThreshold
// erodeGray
void erodeGray(const CPersistentComponentImage& src, 
			CPersistentComponentImage& dest, HV_SIZE kernelSize) 
{

	// only gray image supported
	//assert(src.nImgType == HV_IMAGE_GRAY) ;
	if (src.nImgType != HV_IMAGE_GRAY)
		return;


	int width  = src.iWidth ;
	int height = src.iHeight ;
	int step   = src.iStrideWidth[0] ;

	CFastMemAlloc fastMem;
	BYTE8* fastMemPtr = (BYTE8*)fastMem.StackAlloc(step*height*sizeof(BYTE8));
	BYTE8* tempPtr    = fastMemPtr ;
	BYTE8* srcPtr     = GetHvImageData(&src, 0);

	int halfX         = kernelSize.cx / 2 ;
	int halfY         = kernelSize.cy / 2 ;

	for (int y = 0 ; y < height; ++y)
	{
		int blockTop    = MAX_INT(y-halfY, 0) ;
		int blockBottom = MIN_INT(y+halfY, height-1) ;
		unsigned char* blockTopPtr = GetHvImageData(&src, 0) + blockTop * step ;
		for (int x = 0 ; x < width; ++x)
		{
			// calculate the local minimum
			int localMin = srcPtr[x] ;

			// calculate block range
			int blockLeft   = MAX_INT(x-halfX, 0) ;
			int blockRight  = MIN_INT(x+halfX, width-1) ;

			unsigned char* blockRowPtr = blockTopPtr ;
			for (int by = blockTop; by <= blockBottom; ++by)
			{
				for (int bx = blockLeft; bx <= blockRight; ++bx)
				{
					int neighborVal = blockRowPtr[bx] ;
					if (neighborVal < localMin)
					{
						localMin = neighborVal ;
					}

				}
				blockRowPtr += step ;
			}

			tempPtr[x] = localMin ;

		} // end of for (int x = 0 ; x < width; ++x)

		tempPtr += step ;
		srcPtr  += step ;

	} // end of for (int y = 0 ; y < height; ++y)

	if (dest.iWidth != width || dest.iHeight != height 
		|| dest.nImgType != HV_IMAGE_GRAY)
	{
		dest.Create(HV_IMAGE_GRAY, width, height) ;
	}

	memcpy(GetHvImageData(&dest, 0), fastMemPtr, height*step*sizeof(BYTE8)) ;

	fastMem.Clear() ;

} // erodeGray

// dilateGray
void dilateGray(const CPersistentComponentImage& src, 
			   CPersistentComponentImage& dest, HV_SIZE kernelSize)
{

	// only gray image supported
	//assert(src.nImgType == HV_IMAGE_GRAY) ;
	if (src.nImgType != HV_IMAGE_GRAY)
		return;

	int width  = src.iWidth ;
	int height = src.iHeight ;
	int step   = src.iStrideWidth[0] ;

	CFastMemAlloc fastMem;
	BYTE8* fastMemPtr = (BYTE8*)fastMem.StackAlloc(step*height*sizeof(BYTE8));
	BYTE8* memPtr     = fastMemPtr ;
	BYTE8* srcPtr     = GetHvImageData(&src, 0);

	int halfX         = kernelSize.cx / 2 ;
	int halfY         = kernelSize.cy / 2 ;

	for (int y = 0 ; y < height; ++y)
	{
		int blockTop    = MAX_INT(y-halfY, 0) ;
		int blockBottom = MIN_INT(y+halfY, height-1) ;
		unsigned char* blockTopPtr = GetHvImageData(&src, 0) + blockTop * step ;
		for (int x = 0 ; x < width; ++x)
		{
			// calculate the local maximum
			int localMax = srcPtr[x] ;

			// calculate block range
			int blockLeft   = MAX_INT(x-halfX, 0) ;
			int blockRight  = MIN_INT(x+halfX, width-1) ;

			unsigned char* blockRowPtr = blockTopPtr ;
			for (int by = blockTop; by <= blockBottom; ++by)
			{
				for (int bx = blockLeft; bx <= blockRight; ++bx)
				{
					int neighborVal = blockRowPtr[bx] ;
					if (neighborVal > localMax)
					{
						localMax = neighborVal ;
					}

				}
				blockRowPtr += step ;
			}

			memPtr[x] = localMax ;

		} // end of for (int x = 0 ; x < width; ++x)

		memPtr += step ;
		srcPtr += step ;

	} // end of for (int y = 0 ; y < height; ++y)

	if (dest.iWidth != width || dest.iHeight != height 
		|| dest.nImgType != HV_IMAGE_GRAY)
	{
		dest.Create(HV_IMAGE_GRAY, width, height) ;
	}
	
	memcpy(GetHvImageData(&dest, 0), fastMemPtr, height*step*sizeof(BYTE8)) ;
	fastMem.Clear() ;

} // dilateGray


// topHatTransform: top-hat transform, use for contrast enhancement
void topHatTransform(const CPersistentComponentImage& src, 
					 CPersistentComponentImage& dest, HV_SIZE kernelSize)
{

	// only gray image supported
	//assert(src.nImgType == HV_IMAGE_GRAY) ;
	if (src.nImgType != HV_IMAGE_GRAY)
		return;

	// tophat(I) = I - open(I)

	// open operation
	CPersistentComponentImage opened ;
	erodeGray(src, opened, kernelSize) ;
	dilateGray(opened, opened, kernelSize) ;

	// top-hat operation
	if (dest.iWidth != src.iWidth || dest.iHeight != src.iHeight 
		|| dest.nImgType != HV_IMAGE_GRAY)
	{
		dest.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;
	}

	hvAbsDiff(&src, &opened, &dest) ;

} // topHatTransform

// adjustContrast
void adjustContrast(CPersistentComponentImage& rImage, 
					CPersistentComponentImage& gImage,
					CPersistentComponentImage& bImage,
					float minVal = 126, float maxVal = 128)
{

	if ( rImage.iWidth    <= 0 
		|| rImage.iHeight <= 0 
		|| gImage.iWidth  <= 0 
		|| gImage.iHeight <= 0 
		|| bImage.iWidth  <= 0 
		|| bImage.iHeight <= 0
		|| rImage.iWidth  != gImage.iWidth 
		|| rImage.iHeight != gImage.iHeight
		|| rImage.iWidth  != bImage.iWidth
		|| rImage.iHeight != bImage.iHeight)
	{
		return ;
	}

	if (maxVal <= minVal)
	{
		return ;
	}

	int range = maxVal - minVal ;
	int lut[256] ;
	for (int k = 0; k < 256; ++k)
	{
		int v = k - minVal ;
		v = (int)(256.0*v/range) ;
		if (v < 0)
		{
			v = 0 ; 
		}
		if (v > 255)
		{
			v = 255 ;
		}
		lut[k] = v ;
	} // end of for (int k = 0; k < 256; ++k)

	unsigned char* rRow = GetHvImageData(&rImage, 0);
	unsigned char* gRow = GetHvImageData(&gImage, 0);
	unsigned char* bRow = GetHvImageData(&bImage, 0);

	int height = rImage.iHeight ;
	int width  = rImage.iWidth ;
	int step   = rImage.iStrideWidth[0] ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			rRow[x] = lut[rRow[x]] ;
			gRow[x] = lut[gRow[x]] ;
			bRow[x] = lut[bRow[x]] ;

		} // end of for (int x = 0; x < width; ++x)

		rRow += step ;
		gRow += step ;
		bRow += step ;

	} // end of for (int y = 0; y < height; ++y)


} // adjustContrast

// adjustContrast: gray version
void adjustContrast(CPersistentComponentImage& gray, float minVal = 126, float maxVal = 128)
{

	if ( gray.iWidth <= 0 || gray.iHeight <= 0 )
	{
		return ;
	}

	if (maxVal <= minVal)
	{
		return ;
	}

	int range = maxVal - minVal ;
	int lut[256] ;
	for (int k = 0; k < 256; ++k)
	{
		int v = k - minVal ;
		v = (int)(256.0*v/range) ;
		if (v < 0)
		{
			v = 0 ; 
		}
		if (v > 255)
		{
			v = 255 ;
		}
		lut[k] = v ;
	} // end of for (int k = 0; k < 256; ++k)

	unsigned char* srow = GetHvImageData(&gray, 0);

	int height = gray.iHeight ;
	int width  = gray.iWidth ;
	int step   = gray.iStrideWidth[0] ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{

			srow[x] = lut[srow[x]] ;

		} // end of for (int x = 0; x < width; ++x)

		srow += step ;

	} // end of for (int y = 0; y < height; ++y)


} // adjustContrast
// bubbleSort
void bubbleSort( BYTE8* pA, size_t iCount)
{
	BYTE8 *p = pA;
	BYTE8 t;
	for(int i=0; i < (int)iCount; i++)
	{
		for (int j=i+1;j < (int)iCount; j++)
		{
			if(p[i] > p[j]){
				t = p[i];
				p[i] = p[j];
				p[j] = t;
			}
		}
	}	
} // bubbleSort

// bubbleSort
template<typename T>
void bubbleSort( T* pA, size_t iCount)
{
	T *p = pA;
	T t;
	for(int i=0; i < (int)iCount; i++)
	{
		for (int j=i+1;j < (int)iCount; j++)
		{
			if(p[i] > p[j]){
				t = p[i];
				p[i] = p[j];
				p[j] = t;
			}
		}
	}	
} // bubbleSort
// medianBlur
void medianBlur(const CPersistentComponentImage& src, 
				CPersistentComponentImage& dest, int kernelSize)
{

	// only gray image supported
	//assert(src.nImgType == HV_IMAGE_GRAY && src.iWidth > 0 && src.iHeight > 0) ;
	if (src.nImgType != HV_IMAGE_GRAY || src.iWidth <= 0 || src.iHeight <= 0)
		return;

	int width  = src.iWidth ;
	int height = src.iHeight ;
	int step   = src.iStrideWidth[0] ;

	if (kernelSize == 3)
	{
		// use TI's image library
		CPersistentComponentImage result ;
		result.Create(HV_IMAGE_GRAY, width, height) ;
		unsigned char* srow = GetHvImageData(&src, 0);
		unsigned char* rrow = GetHvImageData(&result, 0)+step ;
		for (int y = 0; y < height-2; ++y)
		{
			IMG_median_3x3_8_cn(srow, width, rrow) ;
			srow += step ;
			rrow += step ;
		} // end of for (int y = 0; y < height; ++y)
		dest.Assign(result) ;
		return ;
	}
	CFastMemAlloc fastMem, fastMem2;
	BYTE8* tempImage = (BYTE8*)fastMem.StackAlloc(step*height*sizeof(BYTE8));
	BYTE8* memPtr    = tempImage ;
	BYTE8* cache     = (BYTE8*)fastMem2.StackAlloc(kernelSize*kernelSize);
	BYTE8* srcPtr    = GetHvImageData(&src, 0);

	int half = kernelSize / 2 ;

	for (int y = 0 ; y < height; ++y)
	{
		int blockTop    = MAX_INT(y-half, 0) ;
		int blockBottom = MIN_INT(y+half, height-1) ;
		unsigned char* blockTopPtr = GetHvImageData(&src, 0) + blockTop * step ;
		for (int x = 0 ; x < width; ++x)
		{

			// calculate block range
			int blockLeft   = MAX_INT(x-half, 0) ;
			int blockRight  = MIN_INT(x+half, width-1) ;

			unsigned char* blockRowPtr = blockTopPtr ;
			int k = 0 ;
			for (int by = blockTop; by <= blockBottom; ++by)
			{
				for (int bx = blockLeft; bx <= blockRight; ++bx)
				{
					int neighborVal = blockRowPtr[bx] ;
					cache[k++] = neighborVal ; 

				}
				blockRowPtr += step ;
			}

			bubbleSort<BYTE8>(cache, k) ;
			memPtr[x] = cache[k/2] ;

		} // end of for (int x = 0 ; x < width; ++x)

		memPtr += step ;
		srcPtr += step ;

	} // end of for (int y = 0 ; y < height; ++y)


	if (dest.iWidth != src.iWidth || dest.iHeight != src.iHeight 
		|| dest.nImgType != HV_IMAGE_GRAY)
	{
		dest.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;
	}

	memcpy(GetHvImageData(&dest, 0), tempImage, height*step*sizeof(BYTE8)) ;

	fastMem.Clear() ;
	fastMem2.Clear() ;

} // medianBlur

// resizeGrayImage
void resizeGrayImage(const CPersistentComponentImage& src, CPersistentComponentImage& dest, HV_SIZE sz)
{
	if (src.iWidth <= 0 || src.iHeight <= 0 ||
		sz.cx <= 0 || sz.cy <= 0 || src.nImgType != HV_IMAGE_GRAY)
	{
		return ;
	}

	CPersistentComponentImage scaledImage ;
	scaledImage.Create(HV_IMAGE_GRAY, sz.cx, sz.cy) ;
	
	//PlateResize(src, scaledImage) ;
	hvImageResizeGray(&scaledImage, (HV_COMPONENT_IMAGE*)&src) ;

	if (dest.iWidth != sz.cx || dest.iHeight != sz.cy)
	{
		dest.Create(HV_IMAGE_GRAY, sz.cx, sz.cy) ;
	}

	dest.Assign(scaledImage) ;

} // resizeGrayImage

// resizeYUV422
void resizeYUV422(const CPersistentComponentImage& src, CPersistentComponentImage& dest, HV_SIZE sz)
{
	if (src.iWidth <= 0 || src.iHeight <= 0 ||
		sz.cx <= 0 || sz.cy <= 0 || src.nImgType != HV_IMAGE_YUV_422)
	{
		return ;
	}

	CPersistentComponentImage scaledImage ;
	scaledImage.Create(HV_IMAGE_YUV_422, sz.cx, sz.cy) ;

	PlateResize(src, scaledImage) ;

	if (dest.iWidth != sz.cx || dest.iHeight != sz.cy)
	{
		dest.Create(HV_IMAGE_YUV_422, sz.cx, sz.cy) ;
	}

	dest.Assign(scaledImage) ;
} // resizeYUV422

// cropGrayImage
void cropGrayImage(const HV_COMPONENT_IMAGE &imgInput,
				   HV_COMPONENT_IMAGE *pimgOutput,
				   HV_RECT& rcCrop)
{
	if (rcCrop.left < 0 || rcCrop.top < 0 ||
		rcCrop.right < rcCrop.left || rcCrop.bottom < rcCrop.top ||
		rcCrop.right > imgInput.iWidth ||
		rcCrop.bottom > imgInput.iHeight)
	{
		return ;
	}
	rcCrop.left &= ~1;

	int rgiOffset = 0 ;
	switch (imgInput.nImgType)
	{
	case HV_IMAGE_GRAY:
		rgiOffset = imgInput.iStrideWidth[0] * rcCrop.top + rcCrop.left;
		break;
	default:
		return ;
	}

	//同步类型信息
	//指针后面会赋值,故可以直接拷贝
	*pimgOutput = imgInput; 

	SetHvImageData(pimgOutput, 0, GetHvImageData(&imgInput, 0) + rgiOffset);
	pimgOutput->iWidth = rcCrop.right - rcCrop.left;
	pimgOutput->iHeight = rcCrop.bottom - rcCrop.top;

} // cropGrayImage

void copyROI(const HV_COMPONENT_IMAGE& src, 
			 CPersistentComponentImage& dest, HV_RECT rcROI)
{
	if(src.nImgType == HV_IMAGE_BT1120_ROTATE_Y)
	{
		if(FAILED(dest.Create(HV_IMAGE_YUV_422, rcROI.right - rcROI.left, rcROI.bottom - rcROI.top)))
		{
			return;
		}
		dest.CropAssign(src, rcROI); 
	}
	else
	{
		HV_COMPONENT_IMAGE temp ;       // region of interest
		CropImage(src, rcROI, &temp) ; // note that roi share the data with src
		if (temp.nImgType != HV_IMAGE_YUV_422)
		{
		
			dest.Create(HV_IMAGE_YUV_422, temp.iWidth, temp.iHeight) ;
			dest.Convert(temp) ;		
		}
		else
		{
			dest.Assign(temp) ;
		}
	}
}

// bitwise_or
void bitwise_or(const CPersistentComponentImage& src1, 
				const CPersistentComponentImage& src2,
				CPersistentComponentImage& dest)
{
	//assert(src1.nImgType == HV_IMAGE_GRAY && src2.nImgType == HV_IMAGE_GRAY) ;  // only gray image supported
	//assert(src1.iWidth == src2.iWidth && src1.iHeight == src2.iHeight \
	//	&& src1.iStrideWidth[0] == src2.iStrideWidth[0]) ;
	
	if (src1.nImgType != HV_IMAGE_GRAY || src2.nImgType != HV_IMAGE_GRAY) return;
	if (src1.iWidth != src2.iWidth || src1.iHeight != src2.iHeight \
		|| src1.iStrideWidth[0] != src2.iStrideWidth[0]) return;

	int bytes = src1.iStrideWidth[0] * src1.iHeight * sizeof(char) ;

	CFastMemAlloc fastMem;
	char* fastMemPtr = (char*)fastMem.StackAlloc(bytes);
	for (int i = 0; i < bytes; ++i)
	{
		fastMemPtr[i] = GetHvImageData(&src1,0)[i] | GetHvImageData(&src2, 0)[i] ;
	}

	if ( (dest.iWidth != src1.iWidth) || (dest.iHeight != src1.iHeight) 
		|| (dest.iStrideWidth[0] != src1.iStrideWidth[0]) || dest.nImgType != HV_IMAGE_GRAY )
	{
		dest.Create(HV_IMAGE_GRAY, src1.iWidth, src1.iHeight) ;
	}

	memcpy(GetHvImageData(&dest, 0), fastMemPtr, bytes) ;

	fastMem.Clear() ;

} // bitwise_or

// bitwise_xor
void bitwise_xor(const CPersistentComponentImage& src1, 
				const CPersistentComponentImage& src2,
				CPersistentComponentImage& dest)
{
	//assert(src1.nImgType == HV_IMAGE_GRAY && src2.nImgType == HV_IMAGE_GRAY) ;  // only gray image supported
	//assert(src1.iWidth == src2.iWidth && src1.iHeight == src2.iHeight \
	//	&& src1.iStrideWidth[0] == src2.iStrideWidth[0]) ;

	if (src1.nImgType != HV_IMAGE_GRAY || src2.nImgType != HV_IMAGE_GRAY) return;
	if (src1.iWidth != src2.iWidth || src1.iHeight != src2.iHeight \
		|| src1.iStrideWidth[0] != src2.iStrideWidth[0]) return;


	int bytes = src1.iStrideWidth[0] * src1.iHeight * sizeof(char) ;

	CFastMemAlloc fastMem;
	char* fastMemPtr = (char*)fastMem.StackAlloc(bytes);

	for (int i = 0; i < bytes; ++i)
	{
		fastMemPtr[i] = GetHvImageData(&src1, 0)[i] ^  GetHvImageData(&src2, 0)[i] ;
	}

	if ( (dest.iWidth != src1.iWidth) || (dest.iHeight != src1.iHeight) 
		|| (dest.iStrideWidth[0] != src1.iStrideWidth[0]) || dest.nImgType != HV_IMAGE_GRAY )
	{
		dest.Create(HV_IMAGE_GRAY, src1.iWidth, src1.iHeight) ;
	}

	memcpy(GetHvImageData(&dest, 0), fastMemPtr, bytes) ;

	fastMem.Clear() ;

}

// appendTo: append s2 to s1
template<typename T>
void appendTo(sequence<T>& s1, const sequence<T>& s2)
{
	sequence<T> merged(s1) ;
	merged.reserve(s1.size() + s2.size()) ;
	typedef typename sequence<T>::size_type size_t ;
	size_t szS2 = s2.size() ;
	for (size_t i = 0; i < szS2; ++i)
	{
		merged.push_back(s2[i]) ;
	}

	s1.swap(merged) ;

} // appendTo

/************************************************************************/
/* business helper functions                                            */
/************************************************************************/

typedef int (*colorFunc)(int, int, int) ;
// getGreenValue
int inline getGreenValue(int r, int g, int b)
{

	int value = (2*g - r - g) / 2 ;
	return value < 0 ? 0 : value ;


} // getGreenValue

// getRedValue
int inline getRedValue(int r, int g, int b)
{
	int value = (2*r - b - g) / 2 ;
	return value < 0 ? 0 : value ;

} // getRedValue

// drawBlob
void drawBlob(CPersistentComponentImage& image, const blob& b, float clr) 
{

	if (b.size() == 0)
	{
		return ;
	}

	int width  = image.iWidth ;
	int height = image.iHeight ;

	//assert (width > 0 && height > 0 &&  image.nImgType == HV_IMAGE_GRAY) ;
	if (width <= 0 || height <= 0 ||  image.nImgType != HV_IMAGE_GRAY)
		return;

	HV_RECT boundRect = calcBoundingRect(b) ;
	int y = boundRect.top ;

	unsigned char* imagePtr = GetHvImageData(&image, 0) + y * image.iStrideWidth[0] ;

	int pointCount = b.size() ;


	for (int i = 0; i < pointCount; ++ i)
	{
		HV_POINT pt = b[i] ;
		if (pt.y != y)
		{
			imagePtr += image.iStrideWidth[0] ;
			y = pt.y ;
		}

		imagePtr[pt.x] = (unsigned char)clr ; 

	}


} // drawBlob


// checkLightStatus
light_status checkLightStatus(const CPersistentComponentImage& rImage,
							  const CPersistentComponentImage& gImage,
							  const CPersistentComponentImage& bImage,
							  const HV_RECT lightRect, 
							  HV_SIZE lightSize)
{

	if (rImage.nImgType != HV_IMAGE_GRAY || gImage.nImgType != HV_IMAGE_GRAY 
		|| bImage.nImgType != HV_IMAGE_GRAY)
		return UNKNOWN ;

	CPersistentComponentImage mask ;
	mask.Create(HV_IMAGE_GRAY, rImage.iWidth, rImage.iHeight) ;

	HV_RECT rcROI = lightRect ;
	adjustRectSize(rcROI, lightSize.cx, lightSize.cy) ;
	rcROI.left   = rcROI.left >= 0 ? rcROI.left : 0 ;
	rcROI.top    = rcROI.top >= 0 ? rcROI.top : 0 ;
	rcROI.right  = rcROI.right < rImage.iWidth ? rcROI.right : (rImage.iWidth-1) ;
	rcROI.bottom = rcROI.bottom < rImage.iHeight ? rcROI.bottom : (rImage.iHeight-1) ;

	CPersistentComponentImage rBlock, gBlock, bBlock ;
	cropGrayImage(rImage, &rBlock, rcROI) ;
	cropGrayImage(gImage, &gBlock, rcROI) ;
	cropGrayImage(bImage, &bBlock, rcROI) ;

	int sR = hvSum(&rBlock) ;
	int sG = hvSum(&gBlock) ;
	int sB = hvSum(&bBlock) ;

	HV_SIZE sz = calcSize(rcROI) ;
	int elemCount = sz.cx * sz.cy ;

	int mR = (int)((float)sR / elemCount );
	int mG = (int)((float)sG / elemCount );
	int mB = (int)((float)sB / elemCount );

	light_status status = GREEN ;

	if (getRedValue(mR, mG, mB) > getGreenValue(mR, mG, mB))
	{
		status = RED ;
	}

	return status ;

} // checkLightStatus


// calcHistMinMax
void calcHistMinMax(const CPersistentComponentImage& grayImage, 
					int & histMin, int & histMax) 
{
	int width = grayImage.iWidth ;
	int height = grayImage.iHeight ;

	if (width <= 0 || height <= 0
		|| grayImage.nImgType != HV_IMAGE_GRAY)
	{
		return ;
	}

	int hist[256] ;
	memset(hist, 0, sizeof(int)*256) ;
	unsigned char* srow = GetHvImageData(&grayImage, 0);
	int step = grayImage.iStrideWidth[0] ;
	int pixelCount = 0 ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			++hist[srow[x]] ;
			++pixelCount ;
		}

		srow += step ;
	} // end of for (int y = 0; y < height; ++y)

	int autoThreshold = 5000 ;
	int threshold = pixelCount / autoThreshold ;
	int limit = pixelCount / 10 ;
	bool found = false ;
	int i = 0 ;
	for (; i < 255, !found; ++i)
	{
		int c = hist[i] ;
		if (c > limit)
		{
			c = 0 ;
		}
		found = c > threshold ;
	} // end of for (; i < 255, !found; ++i)

	int hmin = i ;
	i = 255 ;
	found = false ;
	for (; i >= 0, !found; --i)
	{
		int c = hist[i] ;
		if (c > limit)
		{
			c = 0 ;
		}
		found = c > threshold ;
	} // end of for (; i >= 0, !found; --i)
	int hmax = i ;

	if (hmax <= hmin)
	{
		hmin = 0 ;
		hmax = 255 ;
	}

	histMin = hmin ;
	histMax = hmax ;

} // calcHistMinMax

// findSpotLight
void findSpotLight(const CPersistentComponentImage& grayImage, 
				   CPersistentComponentImage& mask, 
				   int histMin = 0, int histMax = 255)
{
	// only gray image supported
	//assert(src.iWidth > 0 && src.iHeight > 0) ;
	if (grayImage.iWidth <= 0 || grayImage.iHeight <= 0 || grayImage.nImgType != HV_IMAGE_GRAY)
		return;

	mask.Assign(grayImage) ;
	adjustContrast(mask, histMin, histMax) ;
	adaptiveThreshold(mask, mask) ; // binarization
	//adaptiveThreshold(mask, mask, 15, 15.0) ;

}

// createColorMask
void createColorMask(const CPersistentComponentImage& red, 
					 const CPersistentComponentImage& green,
					 const CPersistentComponentImage& blue,
					 CPersistentComponentImage& mask,
					 int kernelSize,
					 colorFunc valuePicker)
{

	if (red.iWidth <= 0 || red.iHeight <= 0 || red.nImgType != HV_IMAGE_GRAY
		|| green.iWidth <= 0 || green.iHeight <= 0 || green.nImgType != HV_IMAGE_GRAY
		|| blue.iWidth <= 0 || blue.iHeight <= 0 || blue.nImgType != HV_IMAGE_GRAY)
		return;

	int width = red.iWidth ;
	int height = red.iHeight ;

	//int grayStride = red.iStrideWidth[0] ;

	CPersistentComponentImage colorImage ;
	colorImage.Create(HV_IMAGE_GRAY, width, height) ;

	unsigned char* redPtr   = GetHvImageData(&red, 0);
	unsigned char* greenPtr = GetHvImageData(&green, 0);
	unsigned char* bluePtr  = GetHvImageData(&blue, 0);
	unsigned char* colorPtr  = GetHvImageData(&colorImage, 0);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{

			int value = valuePicker(redPtr[x], greenPtr[x], bluePtr[x]) ;

			colorPtr[x] = value >= 25 ? value : 0 ;


		} // end of for (int x = 0; x < width; ++x)

		redPtr   += red.iStrideWidth[0] ;
		greenPtr += green.iStrideWidth[0] ;
		bluePtr  += blue.iStrideWidth[0] ;
		colorPtr  += colorImage.iStrideWidth[0] ;

	} //end of for (int y = 0; y < height; ++y)

	adaptiveThreshold(colorImage, mask) ;

} // createColorMask
// createColorMask
void createRGColorMask(const CPersistentComponentImage& red, 
					 const CPersistentComponentImage& green,
					 const CPersistentComponentImage& blue,
					 CPersistentComponentImage& mask)
{
	if (red.iWidth <= 0 || red.iHeight <= 0 || red.nImgType != HV_IMAGE_GRAY
		|| green.iWidth <= 0 || green.iHeight <= 0 || green.nImgType != HV_IMAGE_GRAY
		|| blue.iWidth <= 0 || blue.iHeight <= 0 || blue.nImgType != HV_IMAGE_GRAY)
		return;

	int width = red.iWidth ;
	int height = red.iHeight ;

	//int grayStride = red.iStrideWidth[0] ;

	CPersistentComponentImage redColorImage, greenColorImage ;
	redColorImage.Create(HV_IMAGE_GRAY, width, height) ;
	greenColorImage.Create(HV_IMAGE_GRAY, width, height) ;

	unsigned char* redPtr   = GetHvImageData(&red, 0);
	unsigned char* greenPtr = GetHvImageData(&green, 0);
	unsigned char* bluePtr  = GetHvImageData(&blue, 0);
	unsigned char* redColorPtr  = GetHvImageData(&redColorImage, 0);
	unsigned char* greenColorPtr = GetHvImageData(&greenColorImage, 0);

	int rVal = 0, gVal = 0 ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{

			rVal = getRedValue(redPtr[x], greenPtr[x], bluePtr[x]) ;
			gVal = getGreenValue(redPtr[x], greenPtr[x], bluePtr[x]) ;

			redColorPtr[x] = rVal >= 25 ? rVal : 0 ;
			greenColorPtr[x] = gVal >= 25 ? gVal : 0 ;

		} // end of for (int x = 0; x < width; ++x)

		redPtr   += red.iStrideWidth[0] ;
		greenPtr += green.iStrideWidth[0] ;
		bluePtr  += blue.iStrideWidth[0] ;
		redColorPtr  += redColorImage.iStrideWidth[0] ;
		greenColorPtr += greenColorImage.iStrideWidth[0] ;

	} //end of for (int y = 0; y < height; ++y)

	adaptiveThreshold(redColorImage, redColorImage) ;
	adaptiveThreshold(greenColorImage, greenColorImage) ;
	bitwise_or(redColorImage, greenColorImage, mask) ;

} // createRGColorMask

#define INTEGRAL_WIDTH 401
#define INTEGRAL_HEIGHT 401
#define INTEGRAL_STRIDE (INTEGRAL_WIDTH << 2)

typedef struct {
	int iImgHeight;				// 图象高度(以象素点为单位)
	int iImgWidth; 				// 图象实际宽度(以象素点为单位)
	DWORD32 dwaEdgeItg[ INTEGRAL_HEIGHT ][ INTEGRAL_WIDTH ];
} SEG_ITG_SET;

#define AREA_FROM_2D_ARRAY( dwaItg, iTop, iLeft, iHeight, iWidth ) \
	( dwaItg[ ( iTop ) + ( iHeight ) ][ ( iLeft ) + ( iWidth ) ] - \
	dwaItg[ ( iTop ) ][ ( iLeft ) + ( iWidth ) ] - ( dwaItg[ ( iTop ) + ( iHeight ) ][ ( iLeft ) ] - \
	dwaItg[ ( iTop ) ][ ( iLeft ) ] ) )

// AreaGrowwithKamel: thanks for juedong Lee's help
int AreaGrowwithKamel(HV_COMPONENT_IMAGE *imgIn, BYTE8 *pbBinImg, 
					  int iThreshLow, int iBihuaWidth, HV_RECT rgRect, int iIsWhiteChar, 
					  SEG_ITG_SET& ItgImgSet)
{
	int iCountT = 0;
	int iStride = imgIn->iStrideWidth[0];
	bool bIsTag0, bIsTag1, bIsTag2, bIsTag3;

	//std::stack<int> x;					// 声明栈
	//std::stack<int> y;

	fast_stack<HV_POINT> points ;

	rgRect.top = MAX_INT(rgRect.top - 2, 0);		// 车牌边框扩边
	rgRect.bottom = MIN_INT(rgRect.bottom + 2, imgIn->iHeight);
	rgRect.left = MAX_INT(rgRect.left - 2, 0);
	rgRect.right = MIN_INT(rgRect.right + 2, imgIn->iWidth);
	for(int i = rgRect.top; i < rgRect.bottom; i++)
	{
		BYTE8 *pbBinLine = pbBinImg + i * iStride;
		for(int j = rgRect.left; j < rgRect.right; j++)
		{
			if(pbBinLine[j] == 255)		// 把框内前景点都压进栈
			{
				points.push(createPoint(j, i)) ;

				pbBinLine[j] = 255;
				iCountT++;
			}
		}
	}
	while(1)					// 判断一个点8邻域是否有符合kamel的点 并且压进栈
	{

		if(points.empty())
			break;

		HV_POINT pt = points.top() ;
		points.pop() ;

		int iAreaCount = iBihuaWidth * iBihuaWidth;
		int iHalf = iBihuaWidth >> 1;
		char rgTagY[8] = { 0, -1*iHalf, -1*iHalf, -1*iHalf, 0, iHalf, iHalf, iHalf};	// 计算模板p的横坐标偏移量
		char rgTagX[8] = {-1*iHalf, -1*iHalf,  0,  iHalf, iHalf, iHalf, 0,-1*iHalf};	// 计算模板p的纵坐标偏移量
		char rgNearY[8] = { 0, -1, -1, -1, 0, 1, 1, 1};				//  八邻域的下标 -纵坐标
		char rgNearX[8] = {-1, -1,  0,  1, 1, 1, 0,-1};
		int iHeightEnd = imgIn->iHeight - iBihuaWidth;
		int iWidthEnd = imgIn->iWidth - iBihuaWidth;
		for(int k = 0; k < 8; k++)
		{
			int jX = rgNearX[k] + pt.x;			// 纵?
			int iY = rgNearY[k] + pt.y;
			bool bRealTag = false;
			BYTE8 *pbImgInPo = GetHvImageData(imgIn, 0) + iY * iStride + jX;
			BYTE8 *pbBinPo = pbBinImg + iY * iStride + jX;
			if(jX >= imgIn->iWidth || jX < 0 || iY >= imgIn->iHeight || iY < 0 ||
				*pbBinPo == 100 || *pbBinPo == 200 || *pbBinPo == 255)		// 已经标记过的点跳过
			{
				continue;
			}
			for(int k = 0; k < 4; k++)	// D0 || D1 || D2 || D3 = true   当前点为字符点
			{
				// Di = Pi && Pi+1 && Pi+5 && Pi+6
				int iTempi = iY + rgTagY[k];
				int iTempj = jX + rgTagX[k];	// 纵坐标
				int iTop = RANGE_INT(iTempi - iHalf, 0, iHeightEnd);
				int iLeft = RANGE_INT(iTempj - iHalf, 0, iWidthEnd);
				DWORD32 iAvgArea0 = AREA_FROM_2D_ARRAY(ItgImgSet.dwaEdgeItg, iTop, iLeft, iBihuaWidth, iBihuaWidth);
				iTempi = iY + rgTagY[k + 1];
				iTempj = jX + rgTagX[k + 1];
				iTop = RANGE_INT(iTempi - iHalf, 0, iHeightEnd);
				iLeft = RANGE_INT(iTempj - iHalf, 0, iWidthEnd);
				DWORD32 iAvgArea1 = AREA_FROM_2D_ARRAY(ItgImgSet.dwaEdgeItg, iTop, iLeft, iBihuaWidth, iBihuaWidth);
				iTempi = iY - rgTagY[k];
				iTempj = jX - rgTagX[k];
				iTop = RANGE_INT(iTempi - iHalf, 0, iHeightEnd);
				iLeft = RANGE_INT(iTempj - iHalf, 0, iWidthEnd);
				DWORD32 iAvgArea2 = AREA_FROM_2D_ARRAY(ItgImgSet.dwaEdgeItg, iTop, iLeft, iBihuaWidth, iBihuaWidth);
				iTempi = iY - rgTagY[k + 1];
				iTempj = jX - rgTagX[k + 1];
				iTop = RANGE_INT(iTempi - iHalf, 0, iHeightEnd);
				iLeft = RANGE_INT(iTempj - iHalf, 0, iWidthEnd);
				DWORD32 iAvgArea3 = AREA_FROM_2D_ARRAY(ItgImgSet.dwaEdgeItg, iTop, iLeft, iBihuaWidth, iBihuaWidth);
				if(iIsWhiteChar)
				{
					bIsTag0 = *pbImgInPo > iThreshLow + iAvgArea0 / iAreaCount;
					bIsTag1 = *pbImgInPo > iThreshLow + iAvgArea1 / iAreaCount;
					bIsTag2 = *pbImgInPo > iThreshLow + iAvgArea2 / iAreaCount;
					bIsTag3 = *pbImgInPo > iThreshLow + iAvgArea3 / iAreaCount;
				}
				else
				{
					bIsTag0 = *pbImgInPo + iThreshLow < iAvgArea0 / iAreaCount;
					bIsTag1 = *pbImgInPo + iThreshLow < iAvgArea1 / iAreaCount;
					bIsTag2 = *pbImgInPo + iThreshLow < iAvgArea0 / iAreaCount;
					bIsTag3 = *pbImgInPo + iThreshLow < iAvgArea3 / iAreaCount;	
				}
				bool bRealTagTemp = bIsTag0 && bIsTag1 && bIsTag2 && bIsTag3;
				bRealTag = bRealTag || bRealTagTemp;
			}

			if(bRealTag)			// 如果符合前景条件 压进栈
			{
				points.push(createPoint(jX, iY)) ;
				*pbBinPo = 255;
				iCountT++;
			}
		}
	}

	return 1;
} // AreaGrowwithKamel

// shapeFiltering: by saturation, width/height ratio and so on
void shapeFiltering(sequence<blob>& blobs, 
					float minSaturation, 
					float minWH, 
					float maxWH, 
					HV_SIZE lightSize,
					HV_RECT centerRgn)
{
	int blobCount = blobs.size() ;
	sequence<blob> results ;
	results.reserve(blobCount) ;
	for (int i = 0; i < blobCount; ++i)
	{
		const blob& b = blobs[i] ;
		HV_RECT rc = calcBoundingRect(b) ;
		HV_SIZE sz = calcSize(rc) ;
		HV_POINT center = calcCenter(rc) ;
		
		if (b.size() < 20) continue;
		float saturation = (float)b.size() / calcArea(rc) ;
		//float whRatio = (float)sz.cx / sz.cy ;
		float whRatio = (float)MAX_INT(sz.cx, sz.cy) / MIN_INT(sz.cx, sz.cy) ;
		//if (saturation >= 0.5f && max(sz.cx, sz.cy) <= 2*min(sz.cx, sz.cy) && shapeChecking(b))
		int xDiff = abs(lightSize.cx - sz.cx) ;
		int yDiff = abs(lightSize.cy - sz.cy) ;
		if (saturation >= minSaturation && whRatio >= minWH && whRatio <= maxWH 
			&& b.size() >= 20 && contains(centerRgn, center)
			&& xDiff <= 8 && yDiff <= 8)
		{
			results.push_back(b) ;
		} // end of if (saturation...)

	} // end of for (int i = 0; i < blobCount; ++i)
	blobs.swap(results) ;

} // shapeFiltering

// shapeFiltering
void shapeFiltering(sequence<blob>& blobs,
					sequence<blob>& adhesions,
					float minSaturation, 
					float minWH, 
					float maxWH,
					HV_SIZE lightSize,
					HV_RECT centerRgn)
{
	int blobCount = blobs.size() ;
	sequence<blob> results ;
	results.reserve(blobCount) ;
	adhesions.clear() ;
	adhesions.reserve(blobCount) ;
	for (int i = 0; i < blobCount; ++i)
	{
		const blob& b = blobs[i] ;
		HV_RECT rc = calcBoundingRect(b) ;
		HV_SIZE sz = calcSize(rc) ;
		HV_POINT center = calcCenter(rc) ;

		float saturation = (float)b.size() / calcArea(rc) ;
		//float whRatio = (float)sz.cx / sz.cy ;
		float whRatio = (float)MAX_INT(sz.cx, sz.cy) / MIN_INT(sz.cx, sz.cy) ;
		//if (saturation >= 0.5f && max(sz.cx, sz.cy) <= 2*min(sz.cx, sz.cy) && shapeChecking(b))
		int xDiff = abs(lightSize.cx - sz.cx) ;
		int yDiff = abs(lightSize.cy - sz.cy) ;
		if (saturation >= minSaturation && whRatio >= minWH && whRatio <= maxWH 
			&& b.size() >= 20 && contains(centerRgn, center)
			&& xDiff <= 8 && yDiff <= 8)
		{
			results.push_back(b) ;
		} // end of if (saturation...)
		else
		{
			if (saturation >= minSaturation && whRatio > 1.7f)
			{
				adhesions.push_back(b) ;
			}
		} // end of if (saturation...)

	} // end of for (int i = 0; i < blobCount; ++i)
	blobs.swap(results) ;

} // shapeFiltering

// areaFiltering: by maximum/minimum blob area
void areaFiltering(sequence<blob>& blobs, float minArea, float maxArea)
{
	int blobCount = blobs.size() ;
	sequence<blob> results ;
	results.reserve(blobCount) ;

	for (int i = 0; i < blobCount; ++i)
	{
		const blob& b = blobs[i] ;

		if (b.size() >= minArea && b.size() <= maxArea)
		{
			results.push_back(b) ;
		} // end of if (b.size() >= minArea && b.size() <= maxArea)

	} // end of for (int i = 0; i < blobCount; ++i)
	blobs.swap(results) ;

} // areaFiltering

// findHighLights3
void findHighLights3(const CPersistentComponentImage& src, sequence<HV_RECT>& highlights,
					 int minArea, int maxArea, direction d )
{
	// find max value
	int maxVal = 0 ;
	int step = src.iStrideWidth[0] ;
	unsigned char* srcPtr = GetHvImageData(&src, 0);

	int width = src.iWidth ;
	int height = src.iHeight ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (srcPtr[x] > maxVal)
			{
				maxVal = srcPtr[x] ;
			}
		} // end of for (int x = 0; x < width; ++x)
		srcPtr += step ;
	} // end for (int y = 0; y < height; ++y)


	CPersistentComponentImage mask ;
	mask.Create(HV_IMAGE_GRAY, src.iWidth, src.iHeight) ;
	memset(GetHvImageData(&mask, 0), 0, sizeof(char)*src.iStrideWidth[0]*src.iHeight) ;
	unsigned char* maskPtr = GetHvImageData(&mask, 0);
	srcPtr = GetHvImageData(&src, 0);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (srcPtr[x] >= maxVal-15)
			{
				maskPtr[x] = 255 ;
			}
		} // end of for (int x = 0; x < width; ++x)
		maskPtr += mask.iStrideWidth[0] ;
		srcPtr += src.iStrideWidth[0] ;
	} // end for (int y = 0; y < height; ++y)

	sequence<blob> blobs ;
	findBlobs(GetHvImageData(&mask, 0), blobs, mask.iWidth, mask.iHeight, mask.iStrideWidth[0]) ;
	highlights.clear() ;
	int blobCount = blobs.size() ;

	for (int i = 0; i < blobCount; ++i)
	{
		HV_RECT rc = calcBoundingRect(blobs[i]) ;
		HV_SIZE sz = calcSize(rc) ;
		float wh = (float)MAX_INT(sz.cx, sz.cy) / MIN_INT(sz.cx, sz.cy) ;
		int area = blobs[i].size() ;
		float saturation = (float)area / calcArea(rc) ;  
		if (area < 20 || MAX_INT(sz.cx, sz.cy) < 10 
			|| saturation < 0.2)
		{
			continue ;
		}
		
		int c = 0 ;
		if (wh >= 0.7 && wh <= 1.35)
		{
			if (area >= minArea && area <= maxArea)
			{
				highlights.push_back(rc) ;
				c = 1 ;
			}
		}
		else if (wh >= 1.7 && wh <= 2.35)
		{
			c = 2 ;
		}
		else if (wh >= 2.7 && wh <= 3.35)
		{
			c = 3 ;
		}

		if (c == 0)
		{
			continue ;
		}

		int l = MAX_INT(sz.cx, sz.cy) / c ;
		for (int k = 0; k < c; ++k)
		{
			HV_RECT newRect ;
			if (d == VERT)
			{
				newRect.left = rc.left ;
				newRect.top = rc.top + k * l ;
				newRect.right = rc.right ;
				newRect.bottom = newRect.top + l ;
			}
			else
			{
				newRect.left = rc.left + k * l ;
				newRect.top = rc.top ;
				newRect.right = newRect.left + l ;
				newRect.bottom = rc.bottom ;
			}
			highlights.push_back(newRect) ;
		}

	}

} // findHighLights3

// Deptr
template<typename T>
struct Deptr
{
	typedef typename T::value_type value_type;    // T is iterator
};


template<typename T>
struct Deptr<T*>
{
	typedef T value_type;    // T* is pointer
};

// sort
template<typename ForwardIter, typename Predicate>
void sort(ForwardIter first, ForwardIter last, Predicate pre)
{

	ForwardIter curIter = first ;
	ForwardIter endIter = last ;

	while (curIter != endIter)
	{
		ForwardIter prevIter = curIter++ ;
		for (; curIter != endIter; ++curIter, ++prevIter)
		{
			if (pre(*curIter, *prevIter))
			{
				typename Deptr<ForwardIter>::value_type tmp = *curIter ;
				*curIter = *prevIter ;
				*prevIter = tmp ;
			}
		}

		endIter = prevIter ;
		curIter = first ;

	}

} // sort

// sortByX
bool sortByX(const HV_POINT& pt1, const HV_POINT& pt2)
{
	if (pt1.x < pt2.x)
	{
		return true ;
	}

	return false ;

} // sortByX

// sortByY
bool sortByY(const HV_POINT& pt1, const HV_POINT& pt2)
{
	if (pt1.y < pt2.y)
	{
		return true ;
	}

	return false ;

} // sortByY

// arrangeAsLine
bool arrangeAsLine(const sequence<blob>& blobs, direction d)
{
	int blobCount = blobs.size() ;
	sequence<HV_POINT> centers ;
	centers.reserve(blobCount) ;
	for (int i = 0; i < blobCount; ++i)
	{
		HV_POINT c = calcCenter(blobs[i]) ;
		centers.push_back(c) ;
	}
	
	if (d == VERT)
	{
		sort(centers.begin(), centers.end(), sortByY) ;
	}
	else
	{
		sort(centers.begin(), centers.end(), sortByX) ;
	}

	if (d == VERT)
	{
		for (int i = 1; i < blobCount; ++i)
		{
			int dx = abs(centers[i].x-centers[i-1].x) ;
			int dy = abs(centers[i].y-centers[i-1].y) ;
			if ( dx > 5 || dy < 9 || dy > 18)
			{
				return false ;
			}
		}
	}
    else
	{
		for (int i = 1; i < blobCount; ++i)
		{
			int dx = abs(centers[i].x-centers[i-1].x) ;
			int dy = abs(centers[i].y-centers[i-1].y) ;
			if ( dx < 9 || dx > 18 || dy > 5)
			{
				return false ;
			}
		}
	}
	return true ;

} // arrangeAsLine

// projectionHorz
bool projectionHorz(const CPersistentComponentImage& img, int& x1, int& x2)
{
	int width  = img.iWidth ;
	int height = img.iHeight ;

	if (width <= 0 || height <= 0 || img.nImgType != HV_IMAGE_GRAY)
	{
		return false ;
	}
	
	sequence<int> proj(width, 0) ;
	unsigned char* srow = GetHvImageData(&img, 0);
	int step = img.iStrideWidth[0] ;
	int maxVal = 0 ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (srow[x] > 0)
			{
				++proj[x] ;
				if (proj[x] > maxVal)
				{
					maxVal = proj[x] ;
				}
			}
		} // end of for (int x = 0; x < width; ++x)
		srow += step ;
	} // end of for (int y = 0; y < height; ++y)

	int thres = maxVal / 3 ;
	for (int i = 0; i < width; ++i)
	{
		if (proj[i] < thres)
		{
			proj[i] = 0 ;
		}
		else
		{
			proj[i] = 255 ;
		}
	} // end of for (int i = 0; i < width; ++i)

	
	int begin = 0, end = 0 ;
	for (int i = 1; i < width; ++i)
	{
		if (proj[i] != proj[i-1])
		{
			if (proj[i-1] < thres)
			{
				int dist = i - begin ;
				if (dist >= 13 && dist <= 22)
				{
					x1 = begin ;
					x2 = i-1 ;
					return true ;
				}
			}
			begin = i ;
		}
	} // end of for (int i = 1; i < width; ++i)

	return false ;

} // projectionHorz

// projectionVert
bool projectionVert(const CPersistentComponentImage& img, int& y1, int& y2)
{
	int width  = img.iWidth ;
	int height = img.iHeight ;

	if (width <= 0 || height <= 0 || img.nImgType != HV_IMAGE_GRAY)
	{
		return false ;
	}

	sequence<int> proj(height, 0) ;
	unsigned char* srow = GetHvImageData(&img, 0);
	int step = img.iStrideWidth[0] ;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (srow[x] > 0)
			{
				++proj[y] ;
			}
		} // end of for (int x = 0; x < width; ++x)
		srow += step ;
	} // end of for (int y = 0; y < height; ++y)

	int maxVal = 0 ;
	for (int i = 0; i < height; ++i)
	{
		if (proj[i] > maxVal)
		{
			maxVal = proj[i] ;
		}
	} // end of for (int i = 0; i < width; ++i)

	int thres = maxVal / 3 ;
	int begin = 0, end = 0 ;
	for (int i = 1; i < height; ++i)
	{
		if (proj[i] != proj[i-1])
		{
			if (proj[i-1] < thres)
			{
				int dist = i - begin ;
				if (dist >= 13 && dist <= 22)
				{
					y1 = begin ;
					y2 = i-1 ;
					return true ;
				}
			}
			begin = i ;
		}
	} // end of for (int i = 1; i < width; ++i)

	return false ;
} // projectionVert

// cutDown
void cutDown(CPersistentComponentImage& img, int band1, int band2, bool vert = true)
{
	int width  = img.iWidth ;
	int height = img.iHeight ;

	if (width <= 0 || height <= 0 || img.nImgType != HV_IMAGE_GRAY
		|| band1 < 0 || band1 > width || band2 < 0 || band2 > width)
	{
		return ;
	}

	int b1 = MIN_INT(band1, band2) ;
	int b2 = MAX_INT(band1, band2) ;

	unsigned char* srow = GetHvImageData(&img, 0); 
	int step = img.iStrideWidth[0] ;

	if (vert)
	{
		bool rightOff = b1+1 < width ;
		bool leftOff = b2-1 >= 0 ;
		for (int y = 0 ; y < height; ++y)
		{
			srow[b1] = srow[b2] = 0 ;
			if (rightOff)
			{
				srow[b1+1] = 0 ;
			}
			if (leftOff)
			{
				srow[b2-1] = 0 ;
			}
			srow += step ;
		}
	}
	else
	{
		bool downOff = b1+1 < height ;
		bool upOff = b2-1 >= 0 ;
		int bytes = sizeof(unsigned char) * width ;

		srow = GetHvImageData(&img, 0) + b1 * step ;
		memset(srow, bytes, 0) ;
		if (downOff)
		{
			unsigned char* next = srow + step ;
			memset(next, bytes, 0) ;
		}

		srow = GetHvImageData(&img, 0) + b2 * step ;
		memset(srow, bytes, 0) ;
		if (upOff)
		{
			unsigned char* prev = srow - step ;
			memset(prev, bytes, 0) ;
		}
	}

} // cutDown

} // namespace


namespace signalway
{

// light_group
light_group::light_group()
: position_(createRect(0,0,1,1))
, direction_(HORZ)
, light_amount_(0)
, light_width_(0)
, light_height_(0)
{ }

light_group::light_group( HV_RECT pos, direction d, int lightCount )
: position_(pos)
, direction_(d)
, light_amount_(lightCount)
, light_width_(0)
, light_height_(0)
{

	HV_SIZE sz = calcSize(position_) ;
	if (direction_ == HORZ)
	{		
		light_width_  = sz.cx / light_amount_ ;
		light_height_ = sz.cy ;
		
	}
	else
	{
		light_width_  = sz.cx ;
		light_height_ = sz.cy / light_amount_ ;
	}

}


// trafficlight
trafficlight::trafficlight()
: position_(createRect(0,0,1,1))
, status_(UNKNOWN)
, confidence_(1.0f)
{ }

trafficlight::trafficlight( HV_RECT pos, light_status status, float confidence )
: position_(pos)
, status_(status)
, confidence_(confidence)
{ }


// trafficlight_recognizer
CTrafficLightRecognizer::CTrafficLightRecognizer( light_group light_group, HV_SIZE imageSize, float offsetScale )
: light_group_(light_group)
, offset_scale_(offsetScale)
{ }
CTrafficLightRecognizer::CTrafficLightRecognizer()
{}

CTrafficLightRecognizer::~CTrafficLightRecognizer()
{ }
void CTrafficLightRecognizer::SetRecognizerParam(light_group light_group, HV_SIZE imageSize, float offsetScale)
{
	light_group_ = light_group;
	offset_scale_ = offsetScale;
}
HV_RECT CTrafficLightRecognizer::calc_roi( HV_SIZE imageSize )
{

	float xOffset = light_group_.light_width_  * offset_scale_ ;
	float yOffset = light_group_.light_height_ * offset_scale_ ;

	int xDet = light_group_.position_.left - xOffset + 0.5 ;
	xDet = xDet > 0 ? xDet : 0 ;

	int yDet = light_group_.position_.top - yOffset + 0.5 ;
	yDet = yDet > 0 ? yDet : 0 ;

	int xDet2 = light_group_.position_.right + xOffset + 0.5 ;
	xDet2 = xDet2 >= imageSize.cx ? (imageSize.cx - 1) : xDet2 ;

	int yDet2 = light_group_.position_.bottom + yOffset + 0.5 ;
	yDet2 = yDet2 >= imageSize.cy ? (imageSize.cy - 1) : yDet2 ;

	return createRect(xDet, yDet, xDet2-xDet+1, yDet2-yDet+1) ;

}


void CTrafficLightRecognizer::FindTrafficLight( const HV_COMPONENT_IMAGE* src, 
										 sequence<trafficlight>& lights, 
										 bool smallGroup , 
										 bool processAdhesion, 
										 bool night,
										 HV_POINT lightGroupCenter )
{
	if (src == NULL)
	{
		return;
	}
	HV_SIZE imageSize = createSize(src->iWidth, src->iHeight) ;
	if (src == NULL || imageSize.cx <= 0 || imageSize.cy <= 0)
	{		
		return;
	}
	// relocate the position of light group
	HV_RECT groupRect = recalc_group_rect(lightGroupCenter, imageSize);

	// calculate roi
	HV_RECT rcROI = calc_roi(imageSize) ;
	HV_SIZE roiSize = calcSize(rcROI) ;

	// copy the roi
	CPersistentComponentImage imgROI ;
	copyROI(*src, imgROI, rcROI) ;

	// convert color space
	CPersistentComponentImage grayImage ;
	convertToGray(imgROI, grayImage) ;
	CPersistentComponentImage rImage, bImage, gImage ;
	convertYUV2BGR(imgROI, bImage, gImage, rImage) ;
	// decide the scale
	float horzScale = 1.0f ;
	float vertScale = 1.0f ;
	decide_scale(roiSize, horzScale, vertScale);

	// resize images
	HV_SIZE newSize = createSize(imgROI.iWidth*horzScale, imgROI.iHeight*vertScale) ;
	newSize.cx = newSize.cx / 4 * 4 ;

	// resizing
	resizeGrayImage(grayImage, grayImage, newSize) ;   // Gray
	resizeGrayImage(rImage, rImage, newSize) ;  // R-channel
	resizeGrayImage(gImage, gImage, newSize) ; // G-channel
	resizeGrayImage(bImage, bImage, newSize) ; // B-channel

	// size of light
	HV_SIZE lightSize = createSize(light_group_.light_width_*horzScale,
		light_group_.light_height_*vertScale) ;
	int kernelSize = smallGroup ? 3 : 5 ;
	// calculate area threshold
	float scale = horzScale * vertScale ;
	float maxArea = (float)light_group_.light_width_ * light_group_.light_height_ * scale * 1.85f ;
	float minArea = (float)light_group_.light_width_ * light_group_.light_height_ * scale * 0.20f ;
	// the spot light's center should be inside this region
	HV_RECT centerRgn = calc_center_region(newSize);

	sequence<blob> tempBlobs ;
	sequence<HV_RECT> candidateLights, tempLights ;
	candidateLights.reserve(10) ;
	CPersistentComponentImage spotLightMask ;
	// gray image processing
	{
		medianBlur(grayImage, grayImage, kernelSize) ;
		int histMin = 0, histMax = 0 ;
		calcHistMinMax(grayImage, histMin, histMax) ;

		// find spot light
		findSpotLight(grayImage, spotLightMask, histMin, histMax) ;
		tempBlobs.reserve(50) ;
		findBlobs(GetHvImageData(&spotLightMask, 0), tempBlobs, spotLightMask.iWidth, 
			spotLightMask.iHeight, spotLightMask.iStrideWidth[0]) ;
		// filtering
		// first round filtering : saturation, width/height, etc.
		shapeFiltering(tempBlobs, 0.45, 0.55, 1.5, lightSize, centerRgn) ;
		// second round filtering for the result of first round filtering: maximum/minimum area of blob
		areaFiltering(tempBlobs, minArea, maxArea) ;

		int blobCount = tempBlobs.size() ;
		tempLights.reserve(blobCount) ;
		for (int i = 0; i < blobCount; ++i)
		{
			tempLights.push_back(calcBoundingRect(tempBlobs[i])) ;

		} // end of for (int i = 0; i < blobCount; ++i)
		appendTo(candidateLights, tempLights) ;
	}

	int candidateCount = candidateLights.size() ;
	sequence<HV_POINT> centers ;
	centers.reserve(candidateCount) ;
	for (int i = 0; i < candidateCount; ++i)
	{
		HV_POINT c = calcCenter(candidateLights[i]) ;
		centers.push_back(c) ;
	} // end of for (int i = 0 ;i < blobCount; ++i)

	// RGB image processing
	{

		if (!processAdhesion)
		{
			medianBlur(rImage, rImage, kernelSize) ;
		}
		medianBlur(gImage, gImage, kernelSize) ;
		medianBlur(bImage, bImage, kernelSize) ;

		tempLights.clear() ;
		CPersistentComponentImage colorMask ;
		if (!processAdhesion)
		{
			createRGColorMask(rImage, gImage, bImage, colorMask) ;
			findBlobs(GetHvImageData(&colorMask, 0), tempBlobs, colorMask.iWidth, 
				colorMask.iHeight, colorMask.iStrideWidth[0]) ;
			// first round filtering
			shapeFiltering(tempBlobs, 0.45, 0.55, 1.5, lightSize, centerRgn) ;
			// second round filtering
			areaFiltering(tempBlobs, minArea, maxArea) ;
			int blobCount = tempBlobs.size() ;
			for (int i = 0; i < blobCount; ++i)
			{
				tempLights.push_back(calcBoundingRect(tempBlobs[i])) ;
			}
		}
		else
		{
			int histMin = 0, histMax = 0 ;
			calcHistMinMax(rImage, histMin, histMax) ;
			adjustContrast(rImage, histMin, histMax) ;
			adaptiveThreshold(rImage, colorMask) ;

			if (!night)
			{
				CPersistentComponentImage tempMask ;
				tempMask.Assign(colorMask) ;
				if (light_group_.direction_ == VERT)
				{

					erodeGray(tempMask, tempMask, createSize(1, 15)) ;

					int x1 = 0, x2 = 0 ;
					if ( projectionHorz(tempMask, x1, x2) )
					{
						cutDown(colorMask, x1, x2) ;
					}
				}
				else
				{
					erodeGray(tempMask, tempMask, createSize(15, 1)) ;

					int x1 = 0, x2 = 0 ;
					if ( projectionVert(tempMask, x1, x2) )
					{
						cutDown(colorMask, x1, x2, false) ;
					}
				}

			}
			else
			{
				sequence<HV_RECT> highLights ;
				findHighLights3(grayImage, highLights, minArea, maxArea, light_group_.direction_) ;
				appendTo(tempLights, highLights) ;
			}
			//erodeGray(spotLightMask, colorMask, createSize(3,3)) ;
			findBlobs(GetHvImageData(&colorMask, 0), tempBlobs, colorMask.iWidth, 
				colorMask.iHeight, colorMask.iStrideWidth[0]) ;

			shapeFiltering(tempBlobs, 0.2f, 0.55, 1.65, lightSize, centerRgn) ;
			areaFiltering(tempBlobs, minArea*0.5, maxArea) ;

			int blobCount = tempBlobs.size() ;
			for (int i = 0; i < blobCount; ++i)
			{
				tempLights.push_back(calcBoundingRect(tempBlobs[i])) ;
			}

		}

		int tempCount = tempLights.size() ;
		for (int i = 0; i < tempCount; ++i)
		{
			HV_POINT c = calcCenter(tempLights[i]) ;
			bool overlap = false ;
			for (int k = 0; k < candidateLights.size(); ++k)
			{
				if (abs(c.x-centers[k].x) <=5 && abs(c.y-centers[k].y) <= 5 )
				{
					overlap = true ;
					break ;
				}
			} // end of for (int k = 0; k < candidateCount; ++k)
			if (!overlap)
			{
				candidateLights.push_back(tempLights[i]) ;
				centers.push_back(c) ;

			} // end of if (!overlap)
		} // end of for (int i = 0; i < tempBlobCount; ++i)
	}

	candidateCount = candidateLights.size() ;
	lights.clear() ;

	for (int i = 0; i < candidateCount; ++i)
	{
		HV_RECT rc = candidateLights[i] ;
		adjustRectSize(rc, lightSize.cx, lightSize.cy) ;

		light_status status = checkLightStatus(rImage, gImage, bImage, rc, lightSize) ;

		adjustRect(rc, horzScale, vertScale, rcROI.left, rcROI.top, 
			grayImage.iWidth, grayImage.iHeight) ;
		trafficlight light(rc, status, 1.0) ;
		lights.push_back(light) ;

	}

	// don't forget to update the position of light group
	light_group_.position_ = groupRect ; 

}

HV_RECT CTrafficLightRecognizer::calc_center_region( HV_SIZE sz )
{
	int xOff = 0 ;
	int yOff = 0 ;
	if (light_group_.direction_ == VERT)
	{
		xOff = (float)sz.cx / 6.0f ;
		yOff = (float)sz.cy / (2*light_group_.light_amount_ + 4 ) ;
	}
	else
	{
		xOff = (float)sz.cx / (2*light_group_.light_amount_ + 4 ) ;
		yOff = (float)sz.cy / 6.0f ;
	}

	HV_RECT centerRgn ;
	centerRgn.left   = xOff ;
	centerRgn.top    = yOff ;
	centerRgn.right  = sz.cx - xOff ;
	centerRgn.bottom = sz.cy - yOff ;

	return centerRgn ;

}

void CTrafficLightRecognizer::decide_scale( HV_SIZE roiSize, float &horzScale, float &vertScale )
{

	horzScale = 1.0f ;
	vertScale = 2.0f ;

	if (light_group_.direction_ == VERT)
	{
		//if (light_group_.light_width_ > 40 || light_group_.light_width_ < 20)
		if (roiSize.cx > 45 || roiSize.cx < 45)
		{
			//horzScale = 40.0f / light_group_.light_width_ ;
			horzScale = 45.0f / roiSize.cx ;
			vertScale = horzScale * 2.0f ;
		}
	}
	else
	{
		//if (light_group_.light_height_ > 40 || light_group_.light_height_ < 20)
		if (roiSize.cy > 45 || roiSize.cy < 45)
		{
			//vertScale = 40.0f / light_group_.light_height_ ;
			vertScale = 45.0f / roiSize.cy ;
			horzScale = vertScale / 2.0f ;
		}
	}

}

HV_RECT CTrafficLightRecognizer::recalc_group_rect( HV_POINT newCenter, HV_SIZE imageSize )
{
	// re-calculate the center of light group, and keep its original size 
	HV_RECT groupRect = light_group_.position_ ;

	if (newCenter.x >= 0 && newCenter.y >= 0)
	{

		HV_RECT newRect = moveCenter(groupRect, newCenter) ;

		if (newRect.left >= 0 && newRect.top >= 0 
			&& newRect.right < imageSize.cx && newRect.bottom < imageSize.cy)
		{
			groupRect = newRect ;
		}

	} // end of if (lightGroupCenter.x >= 0 && lightGroupCenter.y >= 0)

	return groupRect ;
}

HV_POINT createPoint( int x, int y )
{
	HV_POINT pt ;
	pt.x = x ;
	pt.y = y ;

	return pt ;
}

} // signalway


