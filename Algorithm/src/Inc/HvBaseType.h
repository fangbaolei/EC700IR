#ifndef __HV_BASE_TYPE_H__
#define __HV_BASE_TYPE_H__

#include "hvutils.h"

typedef void HvArr;
typedef signed char schar;


#define HV_EXTERN_C extern "C"


#define HV_DEFAULT(val) = val
#define HV_BORDER_REPLICATE  1
#define  HV_NOP(a)      (a)

#define  HV_TOGGLE_FLT(x) ((x)^((int)(x) < 0 ? 0x7fffffff : 0))

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

/* matrices are continuous by default */
#define  HV_DEFAULT_MAT_ROW_ALIGN  1

#define HV_MAGIC_MASK       0xFFFF0000
#define HV_SEQ_MAGIC_VAL             0x42990000
#define HV_SEQ_ELTYPE_GENERIC        0

#define HV_CN_MAX     64
#define HV_CN_SHIFT   3
#define HV_DEPTH_MAX  (1 << HV_CN_SHIFT)

#define HV_MAT_CN_MASK          ((HV_CN_MAX - 1) << HV_CN_SHIFT)
#define HV_MAT_CN(flags)        ((((flags) & HV_MAT_CN_MASK) >> HV_CN_SHIFT) + 1)
#define HV_MAT_DEPTH_MASK       (HV_DEPTH_MAX - 1)
#define HV_MAT_DEPTH(flags)     ((flags) & HV_MAT_DEPTH_MASK)
#define HV_MAT_TYPE_MASK        (HV_DEPTH_MAX*HV_CN_MAX - 1)
#define HV_MAT_TYPE(flags)      ((flags) & HV_MAT_TYPE_MASK)
#define HV_MAT_CONT_FLAG_SHIFT  14
#define HV_MAT_CONT_FLAG        (1 << HV_MAT_CONT_FLAG_SHIFT)
#define HV_IS_MAT_CONT(flags)   ((flags) & HV_MAT_CONT_FLAG)
#define HV_IS_CONT_MAT          HV_IS_MAT_CONT
#define HV_MAT_TEMP_FLAG_SHIFT  15
#define HV_MAT_TEMP_FLAG        (1 << HV_MAT_TEMP_FLAG_SHIFT)
#define HV_IS_TEMP_MAT(flags)   ((flags) & HV_MAT_TEMP_FLAG)


/* size of each channel item,
0x124489 = 1000 0100 0100 0010 0010 0001 0001 ~ array of sizeof(arr_type_elem) */
#define HV_ELEM_SIZE1(type) \
	((((sizeof(size_t)<<28)|0x8442211) >> HV_MAT_DEPTH(type)*4) & 15)

/* 0x3a50 = 11 10 10 01 01 00 00 ~ array of log2(sizeof(arr_type_elem)) */
#define HV_ELEM_SIZE(type) \
	(HV_MAT_CN(type) << ((((sizeof(size_t)/4+1)*16384|0x3a50) >> HV_MAT_DEPTH(type)*2) & 3))


#define HV_MAT_CONT_FLAG_SHIFT  14
#define HV_MAT_CONT_FLAG        (1 << HV_MAT_CONT_FLAG_SHIFT)
#define HV_IS_MAT_CONT(flags)   ((flags) & HV_MAT_CONT_FLAG)

#define HV_MAKETYPE(depth,cn) ((depth) + (((cn)-1) << HV_CN_SHIFT))


#define HV_8U   0
#define HV_8S   1
#define HV_16U  2
#define HV_16S  3
#define HV_32S  4
#define HV_32F  5
#define HV_64F  6

#define HV_8UC1 HV_MAKETYPE(HV_8U,1)
#define HV_8SC1 HV_MAKETYPE(HV_8S,1)
#define HV_32SC1 HV_MAKETYPE(HV_32S,1)
#define HV_32SC2 HV_MAKETYPE(HV_32S,2)
#define HV_32FC1 HV_MAKETYPE(HV_32F,1)
#define HV_32FC2 HV_MAKETYPE(HV_32F,2)
#define HV_32FC3 HV_MAKETYPE(HV_32F,3)

#define HV_WHOLE_SEQ_END_INDEX 0x3fffffff
#define HV_WHOLE_SEQ  hvSlice(0, HV_WHOLE_SEQ_END_INDEX)

/* IPP-compatible return codes */
typedef enum HvStatus
{
	HV_BADMEMBLOCK_ERR          = -113,
	HV_INPLACE_NOT_SUPPORTED_ERR= -112,
	HV_UNMATCHED_ROI_ERR        = -111,
	HV_NOTFOUND_ERR             = -110,
	HV_BADCONVERGENCE_ERR       = -109,

	HV_BADDEPTH_ERR             = -107,
	HV_BADROI_ERR               = -106,
	HV_BADHEADER_ERR            = -105,
	HV_UNMATCHED_FORMATS_ERR    = -104,
	HV_UNSUPPORTED_COI_ERR      = -103,
	HV_UNSUPPORTED_CHANNELS_ERR = -102,
	HV_UNSUPPORTED_DEPTH_ERR    = -101,
	HV_UNSUPPORTED_FORMAT_ERR   = -100,

	HV_BADARG_ERR      = -49,  //ipp comp
	HV_NOTDEFINED_ERR  = -48,  //ipp comp

	HV_BADCHANNELS_ERR = -47,  //ipp comp
	HV_BADRANGE_ERR    = -44,  //ipp comp
	HV_BADSTEP_ERR     = -29,  //ipp comp

	HV_BADFLAG_ERR     =  -12,
	HV_DIV_BY_ZERO_ERR =  -11, //ipp comp
	HV_BADCOEF_ERR     =  -10,

	HV_BADFACTOR_ERR   =  -7,
	HV_BADPOINT_ERR    =  -6,
	HV_BADSCALE_ERR    =  -4,
	HV_OUTOFMEM_ERR    =  -3,
	HV_NULLPTR_ERR     =  -2,
	HV_BADSIZE_ERR     =  -1,
	HV_NO_ERR          =   0,
	HV_OK              =   HV_NO_ERR
}
HvStatus;

#define INT_MAX       2147483647    /* maximum (signed) int value */
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */

#define FLT_MAX         3.402823466e+38F        /* max value */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MAX         1.7976931348623158e+308 /* max value */


//////////////////////////函数表定义/////////////
typedef void (*HvFunc)();

typedef struct HvFuncTable
{
	HvFunc fn_2d[HV_DEPTH_MAX];
}
HvFuncTable;

typedef struct HvBigFuncTable
{
	HvFunc fn_2d[HV_DEPTH_MAX*HV_CN_MAX];
}
HvBigFuncTable;


//////////////////////////四舍五入函数/////////////
typedef union Hv32suf
{
	int i;
	unsigned u;
	float f;
}
Hv32suf;

typedef union Hv64suf
{
	SQWORD64 i;
	QWORD64 u;
	double f;
}
Hv64suf;

inline int  hvRound( double value )
{
	Hv64suf temp;
	temp.f = value + 6755399441055744.0;
	return (int)temp.u;
}

//////////////////////////边界对齐函数/////////////
inline void* hvAlignPtr( const void* ptr, int align=32 )
{
	return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

inline int hvAlign( int size, int align )
{
	return (size + align - 1) & -align;
}

inline int hvAlignLeft( int size, int align )
{
	return size & -align;
}

//////////////////////////内存函数/////////////
/* the alignment of all the allocated buffers */
#define  HV_MALLOC_ALIGN    32

void *hvAllocNew( size_t size );
void hvFreeNew_(void* ptr);
#define hvFreeNew(ptr) (hvFreeNew_(*(ptr)), *(ptr)=0)

//////////////////////////点、矩形和Size定义/////////////
typedef struct
{
	int width;
	int height;
}
HvSize;

typedef struct HvRect
{
	int x;
	int y;
	int width;
	int height;
}
HvRect;
inline  HvSize  hvSize( int width, int height )
{
	HvSize s;

	s.width = width;
	s.height = height;

	return s;
}


inline  HV_POINT  hvPoint( int x, int y )
{
	HV_POINT p;
	p.x = x;
	p.y = y;
	return p;
}

/////////////////////HvPoint2D32f/////////////////////////
typedef struct HvPoint2D32f
{
	float x;
	float y;
}
HvPoint2D32f;


inline  HvPoint2D32f  hvPoint2D32f( double x, double y )
{
	HvPoint2D32f p;

	p.x = (float)x;
	p.y = (float)y;

	return p;
}
//////////////////////////////////////////////////////////

inline  HV_RECT  hvRect( int x, int y, int width, int height )
{
	HV_RECT r;

	r.left = x;
	r.top = y;
	r.right = x + width;
	r.bottom = y + height;

	return r;
}

inline  HvRect hvRectNew( int x, int y, int width, int height )
{
	HvRect r;

	r.x = x;
	r.y = y;
	r.width = width;
	r.height = height;

	return r;
}

//////////////////////////HvSlice/////////////
typedef struct HvSlice
{
	int  start_index, end_index;
}
HvSlice;

inline  HvSlice  hvSlice( int start, int end )
{
	HvSlice slice;
	slice.start_index = start;
	slice.end_index = end;

	return slice;
}

#define HV_WHOLE_SEQ_END_INDEX 0x3fffffff
#define HV_WHOLE_SEQ  hvSlice(0, HV_WHOLE_SEQ_END_INDEX)

//////////////////////////结构元素/////////////
typedef struct _HvConvKernel
{
	int  nCols;
	int  nRows;
	int  anchorX;
	int  anchorY;
	int *values;
	int  nShiftR;
}
HvConvKernel;



//////////////////////////HvScalar/////////////
typedef struct HvScalar
{
	double val[4];
}
HvScalar;

inline  HvScalar  hvScalar( double val0, double val1 HV_DEFAULT(0),
						   double val2 HV_DEFAULT(0), double val3 HV_DEFAULT(0))
{
	HvScalar scalar;
	scalar.val[0] = val0; scalar.val[1] = val1;
	scalar.val[2] = val2; scalar.val[3] = val3;
	return scalar;
}

inline  HvScalar  hvScalarAll( double val0123 )
{
	HvScalar scalar;
	scalar.val[0] = val0123;
	scalar.val[1] = val0123;
	scalar.val[2] = val0123;
	scalar.val[3] = val0123;
	return scalar;
}

//////////////////////////矩阵定义/////////////
#define HV_MAT_MAGIC_VAL    0x42420000

typedef struct HvMat
{
	int type;
	int step;

	/* for internal use only */
	int* refcount;
	int hdr_refcount;

	union
	{
		BYTE8* ptr;
		short* s;
		int* i;
		float* fl;
		double* db;
	} data;

#ifdef __cplusplus
	union
	{
		int rows;
		int height;
	};

	union
	{
		int cols;
		int width;
	};
#else
	int rows;
	int cols;
#endif

}
HvMat;

//////////////////////////基本类型操作定义/////////////
#define HV_IS_MAT_HDR(mat) \
	((mat) != NULL && \
	(((const HvMat*)(mat))->type & HV_MAGIC_MASK) == HV_MAT_MAGIC_VAL && \
	((const HvMat*)(mat))->cols > 0 && ((const HvMat*)(mat))->rows > 0)

#define HV_IS_MAT(mat) \
	(HV_IS_MAT_HDR(mat) && ((const HvMat*)(mat))->data.ptr != NULL)

#define HV_IS_MASK_ARR(mat) \
	(((mat)->type & (HV_MAT_TYPE_MASK & ~HV_8SC1)) == 0)

#define HV_ARE_CNS_EQ(mat1, mat2) \
	((((mat1)->type ^ (mat2)->type) & HV_MAT_CN_MASK) == 0)

#define HV_ARE_TYPES_EQ(mat1, mat2) \
	((((mat1)->type ^ (mat2)->type) & HV_MAT_TYPE_MASK) == 0)


#define HV_ARE_SIZES_EQ(mat1, mat2) \
	((mat1)->height == (mat2)->height && (mat1)->width == (mat2)->width)

//////////////////////////矩阵相关函数/////////////
#define HV_AUTOSTEP  0x7fffffff
#define HV_STUB_STEP     (1 << 30)

/* maximal size of vector to run matrix operations on it inline (i.e. w/o ipp calls) */
#define  HV_MAX_INLINE_MAT_OP_SIZE  10

#define HV_MEMCPY_AUTO( dst, src, len )                                             \
{                                                                                   \
	size_t _ihv_memcpy_i_, _ihv_memcpy_len_ = (len);                                \
	char* _ihv_memcpy_dst_ = (char*)(dst);                                          \
	const char* _ihv_memcpy_src_ = (const char*)(src);                              \
	if( (_ihv_memcpy_len_ & (sizeof(int)-1)) == 0 )                                 \
	{                                                                               \
		for( _ihv_memcpy_i_ = 0; _ihv_memcpy_i_ < _ihv_memcpy_len_;                 \
			_ihv_memcpy_i_+=sizeof(int) )                                           \
		{                                                                           \
			*(int*)(_ihv_memcpy_dst_+_ihv_memcpy_i_) =                              \
			*(const int*)(_ihv_memcpy_src_+_ihv_memcpy_i_);                         \
		}                                                                           \
	}                                                                               \
	else                                                                            \
	{                                                                               \
		for(_ihv_memcpy_i_ = 0; _ihv_memcpy_i_ < _ihv_memcpy_len_; _ihv_memcpy_i_++)\
			_ihv_memcpy_dst_[_ihv_memcpy_i_] = _ihv_memcpy_src_[_ihv_memcpy_i_];    \
	}                                                                               \
}


HvMat hvMat( int rows, int cols, int type, void* data HV_DEFAULT(NULL));
void  hvDecRefDataMat( HvMat* arr );
void hvReleaseMat( HvMat** array, bool fFastMem = false);
HvMat* hvCreateMat( int height, int width, int type, CFastMemAlloc *pFastStack = NULL);
HvMat* hvInitMatHeader(HvMat* arr, int rows, int cols, int type, void* data, int step);
HvSize hvGetMatSize( const HvMat* mat );
void hvCopyMat( const HvMat* srcMat, HvMat* dstMat);
void hvSetMat( HvMat* arr, HvScalar value);
void hvZeroMat(HvMat* arr);

////////////////////////矩阵处理函数////////////////////////////
/* general-purpose saturation macros */ 
#define  HV_CAST_8U(t)  (BYTE8)(!((t) & ~255) ? (t) : (t) > 0 ? 255 : 0)

/* Types of thresholding */
#define HV_THRESH_BINARY      0  /* value = value > threshold ? max_value : 0       */
#define HV_THRESH_BINARY_INV  1  /* value = value > threshold ? 0 : max_value       */
#define HV_THRESH_TRUNC       2  /* value = value > threshold ? threshold : value   */
#define HV_THRESH_TOZERO      3  /* value = value > threshold ? value : 0           */
#define HV_THRESH_TOZERO_INV  4  /* value = value > threshold ? 0 : value           */
#define HV_THRESH_MASK        7

void hvThresholdMat(const HvMat* srcMat, HvMat* dstMat, int thresh, int maxval, int nType );
HRESULT hvAbsDiffMat_32s(const HvMat* mat1, const HvMat* mat2, HvMat* matOut);
HRESULT hvAbsDiffMat_32sDigit(const HvMat* mat1, const HvMat* mat2, HvMat* matOut, BYTE8 nScale);
HRESULT hvScaleMat_32S(HvMat* srcMat, int nScale);

HRESULT ihvCvtTo_32s_C1R( const BYTE8* src, int srcstep, int* dst, int dststep, HvSize size, int param );
HRESULT ihvCvtTo_8u_C1R( const BYTE8* src, int srcstep, BYTE8* dst,
						int dststep, HvSize size, int param );
HRESULT ihvNot_8u_C1R( const BYTE8* src1, int step1, BYTE8* dst, int step, HvSize size);
HRESULT ihvSetZero_8u_C1R( BYTE8* dst, int dststep, HvSize size );

#endif		// #ifndef __HV_BASE_TYPE_H__
