#ifndef __HV_DATASTRUCT_H__
#define __HV_DATASTRUCT_H__

#include "HvBaseType.h"

/****************************************************************************************\
*                         Raster->Chain Tree (Suzuki algorithms)                         *
\****************************************************************************************/
/*
Structure that is used for sequental retrieving contours from the image.
It supports both hierarchical and plane variants of Suzuki algorithm.
*/
typedef struct _HvContourScanner
{
	char *img0;                 /* image origin */
	char *img;                  /* current image row */
	int img_step;               /* image step */
	HvSize img_size;            /* ROI size */
	HV_POINT offset;             /* ROI offset: coordinates, added to each contour point */
	HV_POINT pt;                 /* current scanner position */
	HV_POINT lnbd;               /* position of the last met contour */
	int nbd;                    /* current mark val */
	int approx_method1;         /* approx method when tracing */
	int approx_method2;         /* final approx method */
	int mode;                   /* contour scanning mode:
								0 - external only
								1 - all the contours w/o any hierarchy
								2 - connected components (i.e. two-level structure -
								external contours and holes) */
}
_HvContourScanner;

/*********************************** Chain/Countour *************************************/


/* 轮廓提取模式(只支持HV_RETR_EXTERNAL) */
#define HV_RETR_EXTERNAL 0
#define HV_RETR_LIST     1
#define HV_RETR_CCOMP    2
#define HV_RETR_TREE     3

/* 轮廓逼近方法(只支持HV_CHAIN_APPROX_SIMPLE) */
#define HV_CHAIN_CODE               0
#define HV_CHAIN_APPROX_NONE        1
#define HV_CHAIN_APPROX_SIMPLE      2
#define HV_CHAIN_APPROX_TC89_L1     3
#define HV_CHAIN_APPROX_TC89_KCOS   4
#define HV_LINK_RUNS                5

typedef struct _HvContourScanner* HvContourScanner;


/////////////////////////////////轮廓函数/////////////////////////////////////
HvContourScanner hvStartFindContours(
								HvMat *_img,
								int mode,
                                int  method,
								HV_POINT offset,
								HvContourScanner scanner
								);
bool hvFindNextContour( HvContourScanner scanner );

int hvFindContours(
				   HvMat* img,
				   int mode HV_DEFAULT(HV_RETR_LIST),
				   int method HV_DEFAULT(HV_CHAIN_APPROX_SIMPLE),
				   HV_POINT offset HV_DEFAULT(hvPoint(0,0))
				   );

extern HV_RECT g_rgContourRect[];	// 保存轮廓的数组

#endif		// #ifndef __HV_DATASTRUCT_H__
