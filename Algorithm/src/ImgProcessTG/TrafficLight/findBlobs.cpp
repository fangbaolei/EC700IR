#include "findBlobs.h"

#include <assert.h>

#include "ccl2.h"
#include "ccl3.h"

namespace
{
#define INVALID  -1
#define UNMARKED  0
#define MARKED    1

using namespace signalway ;

typedef unsigned char uchar ;

// matrix
template<typename T>
struct matrix
{
public:
	matrix() ;
	matrix(uchar* data, int rows, int cols, int step) ;

	T* ptr(int row) const ;
	const T& at(int col, int row) const ;
	T& at(int col, int row) ;
	T* begin() ;
	T* end() ;
	void fill(T val) ;

	uchar*    data_ ;
	int       rows_ ;
	int       cols_ ;
	int       step_ ;

}; // matrix

template<typename T>
	matrix<T>::matrix()
	: data_(0)
	, rows_(0)
	, cols_(0)
	, step_(0)
{ }

template<typename T>
	matrix<T>::matrix(uchar* data, int rows, int cols, int step)
	: data_(data)
	, rows_(rows)
	, cols_(cols)
	, step_(step)
{ }

template<typename T>
	T* matrix<T>::ptr(int row) const
{
	assert(row <= rows_ - 1) ;
	T* p = (T*)(data_ + row * step_) ;
	return p ;
}

template<typename T>
	const T& matrix<T>::at(int col, int row) const
{
	assert(col <= cols_ - 1) ;
	T* p = ptr(row) ;
	p += col ;
	return *p ;
}

template<typename T>
	T& matrix<T>::at(int col, int row)
{
	assert(col <= cols_ - 1) ;
	T* p = ptr(row) ;
	p += col ;
	return *p ;
}

template<typename T>
	T* matrix<T>::begin() 
{
	return ptr(0) ;
}

template<typename T>
	T* matrix<T>::end()
{
	std::size_t elemCount = step_ / sizeof(T) * rows_ ;
	return ptr(0) + elemCount ;
}

/*template<typename T>
	void matrix<T>::fill(T val)
{
	std::size_t elemCount = step_ / sizeof(T) * rows_ ;
	T* begin = ptr(0) ;
	std::fill_n(begin, elemCount, val) ;
}*/

//// helper function
// createPoint
HV_POINT createPoint(int x, int y)
{
	HV_POINT pt ;
	pt.x = x ;
	pt.y = y ;

	return pt ;

} // createPoint

bool equal(const HV_POINT& pt1, const HV_POINT& pt2)
{
	return (pt1.x == pt2.x) && (pt1.y == pt2.y) ;
}

// neighbor_direction
enum neighbor_direction
{
	right = 0    ,
	bottom_right ,
	bottom       ,
	bottom_left  ,
	left         ,
	top_left     ,
	top          ,
	top_right
}; // neighbor_direction

// neighbor_offset
static HV_POINT neighbor_offset[8] = {
	createPoint(1, 0),
	createPoint(1, 1),
	createPoint(0, 1),
	createPoint(-1,1),
	createPoint(-1,0),
	createPoint(-1,-1),
	createPoint(0,-1),
	createPoint(1,-1)
} ; // neighbor_offset

// neighborValue
//bool neighborValue(const unsigned char* image, int width, int height, int widthStep,
//	int x, int y, neighbor_direction neighbor, int val)
template<typename IT>
	bool neighborValue(const matrix<IT>& image, int x, int y, 
	neighbor_direction neighbor, IT& val)
{
	int width =  image.cols_ ;
	int height = image.rows_ ;

	assert(x >= 0 && y >= 0 && x < width && y < height) ;

	int nx = x + neighbor_offset[neighbor].x ;
	int ny = y + neighbor_offset[neighbor].y ;

	if (nx < 0 || nx >= width || ny < 0 || ny >= height)
	{
		return false ;
	}

	val = image.at(nx, ny) ;

	return true ;

} // neighborValue

// external_contour_tester
struct external_contour_tester
{
	bool operator()(const matrix<uchar>& image, const matrix<int>& labelImage,
		int x, int y)
	{
		int label   = labelImage.at(x, y) ;
		uchar value = image.at(x, y) ;

		if ( label == INVALID )
		{
			// current point has not been labeled
			uchar aboveVal = 0 ;
			bool aboveExists = neighborValue<uchar>(image, x, y, top, aboveVal) ;
			if (aboveExists)
			{
				if (value != aboveVal)
				{
					// we encounter a external contour point first time
					return true ;
				}
			} // end of if (aboveExists)
			else
			{
				// in this case, we encounter a external contour point first time too
				return true ;
			}

		} // end of if ( label == INVALID )

		return false ;

	}

}; // external_contour_tester

// internal_contour_tester
struct internal_contour_tester 
{

	bool operator()(const matrix<uchar>& image, const matrix<int>& mark, 
		int x, int y)
	{

		uchar value = image.at(x, y) ;
		uchar belowVal = 0 ;
		bool belowExists = neighborValue<uchar>(image, x, y, bottom, belowVal) ;
		if (belowExists)
		{
			int belowMark = UNMARKED ;
			neighborValue<int>(mark, x, y, bottom, belowMark) ;

			if ( value != belowVal && belowMark == UNMARKED )
			{
				// the point(x,y) must be on a newly encountered internal contour
				return true ;
			}
		} // end of if (belowExists)

		return false ;
	} // internal_contour_test

}; // internal_contour_tester

// contour_tracer
class contour_tracer
{
public:
	void operator()(const matrix<uchar>& image, matrix<int>& labelImage, matrix<int>& mark, HV_POINT startPoint,
		uchar contourVal, int labelVal, bool internal, sequence<HV_POINT>& cc)
	{
		// label starting contour point

		labelImage.at(startPoint.x, startPoint.y) = labelVal ;

		// record the contour point
		cc.push_back(startPoint) ;

		// second contour point
		neighbor_direction neighborPos = top_right ;
		if (internal)
		{
			neighborPos = bottom_left ;
		}

		HV_POINT secondPoint = trace(image, mark, startPoint, contourVal, neighborPos) ;

		if ( equal(startPoint, secondPoint) )
		{
			// it's a isolated point
			return ;
		}

		HV_POINT nextPoint = secondPoint ;
		HV_POINT curPoint  = secondPoint ; 


		while ( !equal(curPoint, startPoint) || !equal(nextPoint, secondPoint) )
		{
			curPoint = nextPoint ;

			// initial position of current point
			neighborPos = (neighbor_direction)((neighborPos + 6) % 8) ;

			// search for next contour point
			nextPoint = trace(image, mark, curPoint, contourVal, neighborPos) ;

			// label the contour point
			labelImage.at(curPoint.x, curPoint.y) = labelVal ;

			// record the contour point
			cc.push_back(curPoint) ;

		}

	} // contour_trace


private:
	HV_POINT trace(const matrix<uchar>& image, matrix<int>& mark, HV_POINT curPt, 
		uchar curVal, neighbor_direction& pos)
	{

		int rows = image.rows_ ;
		int cols = image.cols_ ;

		for (int d = 0; d < 8; ++d)
		{
			neighbor_direction testedPos = (neighbor_direction)((pos + d) % 8) ;
			int x = curPt.x + neighbor_offset[testedPos].x ;
			int y = curPt.y + neighbor_offset[testedPos].y ;

			if (x < 0 || x >= cols || y < 0 || y >= rows)
			{
				continue ;
			} // end of if (x < 0 || x >= cols || y < 0 || y >= rows)

			if ( curVal == image.at(x, y) )
			{
				pos = testedPos ;
				return createPoint(x, y) ;
			} // end of if ( similar(curVal, image.at<IT>(y, x)) )
			else
			{
				// the unlike pixel around the contour point will be marked
				mark.at(x, y) = MARKED ;
			}

		} // end of for (int d = 0; d < 8; ++d)

		return curPt ;
	} // trace

}; // contour_tracer

} // namespace

namespace signalway
{

// findBlobs
void findBlobs( const unsigned char* image, sequence<blob>& blobs, 
	int width, int height, int widthStep, float bgClr )
{

	int elemCount = width * height ;
	int* labelImage  = new int[elemCount] ;
	int* markerImage = new int[elemCount] ;

	for (int i = 0; i < elemCount; ++i)
	{
		labelImage[i]  = INVALID ;
		markerImage[i] = UNMARKED ;
	} // end of for (int i = 0; i < elemCount; ++i)

	// wrap the raw arrays
	matrix<uchar> src((uchar*)image, height, width, widthStep) ;
	matrix<int> label((uchar*)labelImage, height, width, width*sizeof(int)) ;
	matrix<int> marker((uchar*)markerImage, height, width, width*sizeof(int)) ;

	external_contour_tester externalTest ;
	internal_contour_tester internalTest ;
	contour_tracer          tracer ;

	// labeling...

	int labelValue = 0 ;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			bool isContour = false ;
			uchar curVal = src.at(x, y) ;

			if (curVal == bgClr)
			{
				continue ;
			}

			// check whether (x, y) is a start point of external contour
			if ( externalTest(src, label, x, y) )
			{
				// start external contour tracing...
				sequence<HV_POINT> externalContour ;
				tracer(src, label, marker, createPoint(x, y), 
					curVal, labelValue, false, externalContour) ;

				isContour = true ;
				++labelValue ;
			} // end of if ( externalTest(src, label, x, y) )

			// check whether (x, y) is a start point of internal contour
			if ( internalTest(src, marker, x, y) )
			{
				// start internal contour tracing
				int contourLabel = INVALID ;
				if ( label.at(x, y) == INVALID )
				{
					// get the left neighbor's label
					if (x > 0)
					{
						contourLabel = label.at(x-1, y) ;
					}
				} // end of if ( label.at(x, y) == INVALID )
				else
				{
					contourLabel =  label.at(x, y) ;
				}

				if (contourLabel != INVALID)
				{
					sequence<HV_POINT> internalContour ;
					tracer(src, label, marker, createPoint(x, y), 
						curVal, contourLabel, true, internalContour) ;

				} // end of if (contourLabel != INVALID)

				isContour = true ;

			} // end of if ( internalTest(src, marker, x, y) )

			if ( !isContour && curVal != bgClr &&label.at(x, y) == INVALID )
			{
				// it's not a contour point, so its left neighbor has been labeled
				label.at(x, y) = label.at(x-1, y) ;

			} // end of if ( !isContour && label.at(x, y) == INVALID )

		} // end of for (int x = 0; x < width; ++x)
	} // end of for (int y = 0; y < height; ++y)


	// extract blobs...

	int blobCount = labelValue ;
	blobs.clear() ;
	blobs.resize(blobCount) ;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int labelVal = label.at(x, y) ;
			if (labelVal != INVALID)
			{
				blobs[labelVal].push_back(createPoint(x, y)) ;
			}
		}
	} // end of for (int y = 0; y < height; ++y)

	delete [] labelImage ;
	delete [] markerImage ;

} // findBlobs




// calcBoundingRect
HV_RECT calcBoundingRect( const blob& b )
{
	int pointCount = b.size() ;

	int maxx = 0 ;
	int maxy = 0 ;
	int minx = 1000 ;
	int miny = 1000 ;

	for (int i = 0; i < pointCount; ++i)
	{
		HV_POINT pt = b[i] ;

		if (pt.x > maxx)
		{
			maxx = pt.x ;
		}
		
		if (pt.y > maxy)
		{
			maxy = pt.y ;
		}

		if (pt.x < minx)
		{
			minx = pt.x ;
		}

		if (pt.y < miny)
		{
			miny = pt.y ;
		}

	} // end of for (int i = 0; i < pointCount; ++i)

	HV_RECT boundRect ;
	boundRect.left   = minx ;
	boundRect.top    = miny ;
	boundRect.right  = maxx ;
	boundRect.bottom = maxy ;

	return boundRect ;

} // calcBoundingRect

template<typename T>
struct my_excluder
{
	my_excluder(const T& val) 
		: excluded_value_(val)
	{}

	bool operator()(const T& val)
	{
		return val == excluded_value_ ;
	}

	T    excluded_value_ ;
};

void findBlobs2( const unsigned char* image, sequence<blob>& blobs, int width, int height, int widthStep, float bgClr)
{

	ccl blobExtracor ;
	blobExtracor(image, width, height, widthStep, blobs, default_comparer<unsigned char>(), 
		my_excluder<unsigned char>(bgClr), false) ;
}

void findBlobs3( const unsigned char* image, sequence<blob>& blobs, int width, int height, int widthStep, float bgClr /*= 0*/ )
{
	static ccl3 cc ;
	cc(image, width, height, widthStep, blobs, default_comparer<unsigned char>(), 
		my_excluder<unsigned char>(bgClr)) ;
}

} // signalway