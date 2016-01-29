#ifndef __HV_IMAGE_NEW_H__
#define __HV_IMAGE_NEW_H__

#include "HvBaseType.h"
#include "HvDataStruct.h"
#include "HVHistogram.h"
#include "swImageObj.h"
#include "swWinError.h"

#define  HV_SHAPE_RECT      0

#define HV_WHOLE   0
#define HV_START   1
#define HV_END     2
#define HV_MIDDLE  4
#define HV_ISOLATED_ROI 8

typedef void (*HvRowFilterFunc)( const BYTE8 * src, BYTE8 * dst, const int ksize, const int width );
typedef void (*HvColumnFilterFunc)( BYTE8 ** src, BYTE8 * dst, int dst_step, int count,
			const int ksize, const int width );

class HvBaseImageFilter
{
public:
	HvBaseImageFilter();
	/* calls init() */
	HvBaseImageFilter(
		int _max_width,
		int _src_type,
		int _dst_type,
		bool _is_separable,
		HvSize _ksize,
		HV_POINT _anchor=hvPoint(-1,-1),
		int _border_mode=HV_BORDER_REPLICATE,
		HvScalar _border_value=hvScalarAll(0)
		);
	virtual ~HvBaseImageFilter();

	/* initializes the class for processing an image of maximal width _max_width,
	input image has data type _src_type, the output will have _dst_type.
	_is_separable != 0 if the filter is separable
	(specific behaviour is defined in a derived class), 0 otherwise.
	_ksize and _anchor specify the kernel size and the anchor point. _anchor=(-1,-1) means
	that the anchor is at the center.
	to get interpolate pixel values outside the image _border_mode=IPL_BORDER_*** is used,
	_border_value specify the pixel value in case of IPL_BORDER_CONSTANT border mode.
	before initialization clear() is called if necessary.
	*/
	virtual void init( int _max_width, int _src_type, int _dst_type,
		bool _is_separable,
		HvSize _ksize,
		HV_POINT _anchor=hvPoint(-1,-1),
		int _border_mode=HV_BORDER_REPLICATE,
		HvScalar _border_value = hvScalarAll(0) );
	/* releases all the internal buffers.
	for the further use of the object, init() needs to be called. */
	virtual void clear();
	/* processes input image or a part of it.
	input is represented either as matrix (HvMat* src)
	or a list of row pointers (BYTE8** src2).
	in the later case width, _src_y1 and _src_y2 are used to specify the size.
	_dst is the output image/matrix.
	_src_roi specifies the roi inside the input image to process,
	(0,0,-1,-1) denotes the whole image.
	_dst_origin is the upper-left corner of the filtered roi within the output image.
	_phase is either HV_START, or HV_END, or HV_MIDDLE, or HV_START|HV_END, or HV_WHOLE,
	which is the same as HV_START|HV_END.
	HV_START means that the input is the first (top) stripe of the processed image [roi],
	HV_END - the input is the last (bottom) stripe of the processed image [roi],
	HV_MIDDLE - the input is neither first nor last stripe.
	HV_WHOLE - the input is the whole processed image [roi].
	*/
	virtual int process( const HvMat* _src, HvMat* _dst,
		HV_RECT _src_roi=hvRect(0,0,-1,-1),
		HV_POINT _dst_origin=hvPoint(0,0), int _flags=0 );
	/* retrieve various parameters of the filtering object */
	int get_src_type() const { return src_type; }
	int get_dst_type() const { return dst_type; }
	int get_work_type() const { return work_type; }
	HvSize get_kernel_size() const { return ksize; }
	HV_POINT get_anchor() const { return anchor; }
	int get_width() const { return prev_x_range.end_index - prev_x_range.start_index; }
	HvRowFilterFunc get_x_filter_func() const { return x_func; }
	HvColumnFilterFunc get_y_filter_func() const { return y_func; }

protected:
	/* initializes work_type, buf_size and max_rows */ 
	virtual void get_work_params();
	/* it is called (not always) from process when _phase=HV_START or HV_WHOLE.
	the method initializes ring buffer (buf_end, buf_head, buf_tail, buf_count, rows),
	prev_width, prev_x_range, const_row, border_tab, border_tab_sz* */
	virtual void start_process( HvSlice x_range, int width );
	/* forms pointers to "virtual rows" above or below the processed roi using the specified
	border mode */
	virtual void make_y_border( int row_count, int top_rows, int bottom_rows );

	virtual int fill_cyclic_buffer( const BYTE8* src, int src_step,
		int y, int y1, int y2 );

	enum { ALIGN=32 };

	int max_width;
	/* currently, work_type must be the same as src_type in case of non-separable filters */
	int min_depth, src_type, dst_type, work_type;

	/* pointers to convolution functions, initialized by init method.
	for non-separable filters only y_conv should be set */
	HvRowFilterFunc x_func;
	HvColumnFilterFunc y_func;

	BYTE8* buffer;
	BYTE8** rows;
	int top_rows, bottom_rows, max_rows;
	BYTE8 *buf_start, *buf_end, *buf_head, *buf_tail;
	int buf_size, buf_step, buf_count, buf_max_count;

	bool is_separable;
	HvSize ksize;
	HV_POINT anchor;
	int max_ky, border_mode;
	HvScalar border_value;
	BYTE8* const_row;
	int* border_tab;
	int border_tab_sz1, border_tab_sz;

	HvSlice prev_x_range;
	int prev_width;

	CFastMemAlloc m_cFastMem;
};


/* basic morphological operations: erosion & dilation */
class HvMorphology : public HvBaseImageFilter
{
public:
	HvMorphology();
	HvMorphology( int _operation, int _max_width, int _src_dst_type,
		int _element_shape, HvMat* _element,
		HvSize _ksize=hvSize(0,0), HV_POINT _anchor=hvPoint(-1,-1),
		int _border_mode=HV_BORDER_REPLICATE,
		HvScalar _border_value=hvScalarAll(0) );
	virtual ~HvMorphology();
	virtual void init( int _operation, int _max_width, int _src_dst_type,
		int _element_shape, HvMat* _element,
		HvSize _ksize=hvSize(0,0), HV_POINT _anchor=hvPoint(-1,-1),
		int _border_mode=HV_BORDER_REPLICATE,
		HvScalar _border_value=hvScalarAll(0) );

	/* dummy method to avoid compiler warnings */
	virtual void init( int _max_width, int _src_type, int _dst_type,
		bool _is_separable, HvSize _ksize,
		HV_POINT _anchor=hvPoint(-1,-1),
		int _border_mode=HV_BORDER_REPLICATE,
		HvScalar _border_value=hvScalarAll(0) );

	virtual void clear();
	const HvMat* get_element() const { return element; }
	int get_element_shape() const { return el_shape; }
	int get_operation() const { return operation; }
	BYTE8* get_element_sparse_buf() { return el_sparse; }
	int get_element_sparse_count() const { return el_sparse_count; }

	enum { RECT=0, CROSS=1, ELLIPSE=2, CUSTOM=100, BINARY = 0, GRAYSCALE=256 };
	enum { ERODE=0, DILATE=1 };

	static void init_binary_element( HvMat* _element, int _element_shape,
		HV_POINT _anchor=hvPoint(-1,-1) );
protected:

	void start_process( HvSlice x_range, int width );
	int fill_cyclic_buffer( const BYTE8* src, int src_step,
		int y0, int y1, int y2 );
	BYTE8* el_sparse;
	int el_sparse_count;

	HvMat *element;
	int el_shape;
	int operation;
};


/////////////////////////结构元素//////////////////
HvConvKernel *hvCreateStructuringElementEx(
	int cols, int  rows, int  anchor_x, int  anchor_y,
	int shape, int* values HV_DEFAULT(NULL) );

void hvReleaseStructuringElement( HvConvKernel** element );


/////////////////////////MultiDownSample函数/////////////////
HRESULT HvImageMultiDownSample(const HV_COMPONENT_IMAGE *pSrc, HV_COMPONENT_IMAGE *pDst, int iMultiDownSample, int *nAvg);
HRESULT HvImageMultiDownSample(const HV_COMPONENT_IMAGE *pSrc, HV_COMPONENT_IMAGE *pDst, int iDownX, int iDownY, int *nAvg);
HRESULT HvImageYCbCrMultiDownSampleToHSV(const HV_COMPONENT_IMAGE *pSrc, const HV_COMPONENT_IMAGE *pDst, int iMultiDownSample);
HRESULT HvImageYCbCrMultiDownSampleToHSV(const HV_COMPONENT_IMAGE *pSrc,
                                         const HV_COMPONENT_IMAGE *pDst, int iDownX, int iDownY);
/////////////////////////通道提取函数/////////////////
HRESULT HvSplitHue(
				const HV_COMPONENT_IMAGE* pSrc,
				HV_COMPONENT_IMAGE *pDst
				);
/////////////////////////MASK设定函数/////////////////
HRESULT hvInRangeHSV(
					 const HV_COMPONENT_IMAGE* pSrc,
					 HvScalar lower,
					 HvScalar upper,
					 HV_COMPONENT_IMAGE *pMask
					 );

////////////////////////图象运算函数////////////////////////////
int hvSum( const HV_COMPONENT_IMAGE* pImg);
int hvAvg( const HV_COMPONENT_IMAGE* pImg);
HRESULT hvAdd(
			  const HV_COMPONENT_IMAGE* pImgAdd1,
			  const HV_COMPONENT_IMAGE* pImgAdd2,
			  HV_COMPONENT_IMAGE* pImgOut
			  );
HRESULT hvAbsDiff(
			  const HV_COMPONENT_IMAGE* pImgAdd1,
			  const HV_COMPONENT_IMAGE* pImgAdd2,
			  HV_COMPONENT_IMAGE* pImgOut
			  );			//add by: qinyj  for: 帧差法
HRESULT hvAnd(const HV_COMPONENT_IMAGE* pImgAnd1,
			  const HV_COMPONENT_IMAGE* pImgAnd2,
			  HV_COMPONENT_IMAGE* pImgOut
			  );
void hvNot( const HV_COMPONENT_IMAGE* srcImg, HV_COMPONENT_IMAGE* dstImg );
void hvThreshold(const HV_COMPONENT_IMAGE* srcMat, HV_COMPONENT_IMAGE* dstMat, int thresh, int maxval, int nType );

////////////////////////图象转换函数////////////////////////////
HvMat* hvGetMatFromImg(const HV_COMPONENT_IMAGE* img, HvMat* mat);
void hvConvertImg2Mat( const HV_COMPONENT_IMAGE* srcImg, HvMat *dstMat);
void hvConvertImg8ToMat32sDigit( const HV_COMPONENT_IMAGE* srcImg, HvMat *dstMat, BYTE8 nScale);
void hvConvertMat2Img(const HvMat *srcMat, HV_COMPONENT_IMAGE* dstImg);
void hvConvertMat32ToImg8(const HvMat *srcMat, HV_COMPONENT_IMAGE* dstImg);

////////////////////////背景函数////////////////////////////
void hvRunningAvg( const HvMat* arrY, HvMat* arrU,
				  double alpha, const HV_COMPONENT_IMAGE* maskArr );
void hvRunningAvgMaskDigit( const HvMat* arrY, HvMat* arrU,
			 DWORD32 dwAlpha1, DWORD32 dwAlpha2, 
			 const HV_COMPONENT_IMAGE* maskArr);

////////////////////////膨胀腐蚀////////////////////////////
void hvErode(
			 const HV_COMPONENT_IMAGE* srcImg,
			 HV_COMPONENT_IMAGE* dstImg,
			 HvConvKernel* element HV_DEFAULT(NULL),
			 int iterations HV_DEFAULT(1)
			 );
void hvDilate(
			  const HV_COMPONENT_IMAGE* srcImg,
			  HV_COMPONENT_IMAGE* dstImg,
			  HvConvKernel* element HV_DEFAULT(NULL),
			  int iterations HV_DEFAULT(1)
			  );

void hvErodeNew(
			 const HV_COMPONENT_IMAGE* srcImg,
			 HV_COMPONENT_IMAGE* dstImg,
			 HvConvKernel* element HV_DEFAULT(NULL),
			 int iterations HV_DEFAULT(1)
			 );
void hvDilateNew(
			  const HV_COMPONENT_IMAGE* srcImg,
			  HV_COMPONENT_IMAGE* dstImg,
			  HvConvKernel* element HV_DEFAULT(NULL),
			  int iterations HV_DEFAULT(1)
			  );

int hvFindContoursImg(
				   HV_COMPONENT_IMAGE* srcImg,
				   int mode HV_DEFAULT(HV_RETR_LIST),
				   int method HV_DEFAULT(HV_CHAIN_APPROX_SIMPLE),
				   HV_POINT offset HV_DEFAULT(hvPoint(0,0))
				   );

// 直方图计算
void hvCalcHistImgInRect(
							HV_COMPONENT_IMAGE* srcImg,
							HvHistogram* hist,
							int do_not_clear,
							const HV_COMPONENT_IMAGE* maskImg,
							HV_RECT rect
						 );

// 反向投影计算
void hvCalcBackProjectImg(
							 HV_COMPONENT_IMAGE* srcImg,
							 HV_COMPONENT_IMAGE* dstImg,
							 const HvHistogram* hist
						  );

// Downsample.sobel并转化成32位量化矩阵						  
int CalcBackgroundImg2MatDigit(
	const HV_COMPONENT_IMAGE* srcImg,
	HvMat *dstMat,
	DWORD32 iMultiDownSample,
	BYTE8 nScale
	);

HRESULT hvAbsDiffMat_32sDigitToImg8(
	const HvMat* mat1,
	const HvMat* mat2,
	HV_COMPONENT_IMAGE* dstImg,
	BYTE8 nScale
	);

#endif		// #ifndef __HV_IMAGE_NEW_H__
