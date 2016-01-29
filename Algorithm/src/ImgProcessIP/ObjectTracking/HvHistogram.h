#ifndef __HV_HISTOGRAM_H__
#define __HV_HISTOGRAM_H__

#include "HvBaseType.h"
#include "HvDataStruct.h"

#define HV_HIST_ARRAY         0				// 目前只支持数组

#define HV_MAX_DIM            32

#define INT_MAX       2147483647    /* maximum (signed) int value */
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */

/**************************** HvMatND  **************************************/
typedef struct HvMatND
{
	int type;
	int dims;

	int* refcount;
	int hdr_refcount;

	union
	{
		BYTE8* ptr;
		float* fl;
		double* db;
		int* i;
		short* s;
	} data;

	struct
	{
		int size;
		int step;
	}
	dim[HV_MAX_DIM];
}
HvMatND;

/**************************** 直方图  **************************************/
#define HV_HIST_MAGIC_VAL     0x42450000
#define HV_MATND_MAGIC_VAL    0x42430000
#define HV_HIST_DEFAULT_TYPE HV_32F

#define HV_HIST_UNIFORM_FLAG  (1 << 10)
/* indicates whether bin ranges are set already or not */
#define HV_HIST_RANGES_FLAG   (1 << 11)

#define HV_HIST_HAS_RANGES( hist ) \
	(((hist)->type & HV_HIST_RANGES_FLAG) != 0)
#define HV_IS_UNIFORM_HIST( hist ) \
	(((hist)->type & HV_HIST_UNIFORM_FLAG) != 0)

typedef int HvHistType;

typedef struct HvHistogram
{
	int     type;
	HvMatND *bins;
	int thresh[HV_MAX_DIM][2]; /* for uniform histograms */
	int** thresh2; /* for non-uniform histograms */
	HvMatND mat;	/* embedded matrix header for array histograms */
}
HvHistogram;

#define HV_IS_HIST( hist ) \
	((hist) != NULL  && \
	(((HvHistogram*)(hist))->type & HV_MAGIC_MASK) == HV_HIST_MAGIC_VAL && \
	(hist)->bins != NULL)


/**************************** 矩阵枚举器  **************************************/
/**************** matrix iterator: used for n-ary operations on dense arrays *********/
#define HV_MAX_ARR 10
typedef struct HvNArrayIterator
{
	int count; /* number of arrays */
	int dims; /* number of dimensions to iterate */
	HvSize size; /* maximal common linear size: { width = size, height = 1 } */
	BYTE8* ptr[HV_MAX_ARR]; /* pointers to the array slices */
	int stack[HV_MAX_DIM]; /* for internal use */
	HvMatND* hdr[HV_MAX_ARR]; /* pointers to the headers of the
							  matrices that are processed */
}
HvNArrayIterator;

//////////////////////////////////矩阵枚举函数////////////////////////
int hvInitNArrayIterator( int count, HvMatND** arrs,
						 const HvMatND* mask, HvMatND* stubs,
						 HvNArrayIterator* iterator, int flags HV_DEFAULT(0));
int hvNextNArraySlice( HvNArrayIterator* array_iterator );

//////////////////////////////////HvMatND 基本函数////////////////////////
void  hvDecRefDataMatND( HvMatND* arr );
void hvReleaseDataMat( HvMatND* arr );
HvMatND* hvInitMatNDHeader(
					HvMatND* mat,
					int dims,
					const int* sizes,
					int type,
					void* data HV_DEFAULT(NULL)
					);
void hvCreateDataMatND( HvMatND* arr );
int hvGetDimsMatND( const HvMatND* arr, int* sizes );
HvMat* hvGetMatND( const HvMatND* array, HvMat* mat,
				  int* pCOI, int allowND );

//////////////////////////////////////直方图函数////////////////////////////
void hvReleaseHist( HvHistogram **hist );
HvHistogram *hvCreateHist( int dims, int *sizes, HvHistType type, int** ranges, int uniform );
void hvSetHistBinRanges( HvHistogram* hist, int** ranges, int uniform );
void hvCalcHistMat( HvMat** img, HvHistogram* hist, int do_not_clear, const HvMat* mask );
void hvGetMinMaxHistValue( const HvHistogram* hist, int *value_min, int* value_max);
void hvConvertMatND2MatNDScale( const HvMatND* srcarr, HvMatND* dstarr,int nMax);
void hvCalcBackProjectMat( HvMat ** img, HvMat * dst, const HvHistogram* hist );

#endif		// #ifndef __HV_HISTOGRAM_H__
