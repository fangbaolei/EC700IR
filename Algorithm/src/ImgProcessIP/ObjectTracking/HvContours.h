#ifndef __HV_CONTOURS_H__
#define __HV_CONTOURS_H__

#include "HvSeq.h"

/****************************************************************************************\
*                         Raster->Chain Tree (Suzuki algorithms)                         *
\****************************************************************************************/

typedef struct _HvContourInfo
{
	int flags;
	struct _HvContourInfo *next;        /* next contour with the same mark value */
	struct _HvContourInfo *parent;      /* information about parent contour */
	HvSeq *contour;             /* corresponding contour (may be 0, if rejected) */
	HvRect rect;                /* bounding rectangle */
	HV_POINT origin;             /* origin point (where the contour was traced from) */
	int is_hole;                /* hole flag */
}
_HvContourInfo;


/*
Structure that is used for sequental retrieving contours from the image.
It supports both hierarchical and plane variants of Suzuki algorithm.
*/
typedef struct _HvContourScannerOrg
{
	HvMemStorage *storage1;     /* contains fetched contours */
	HvMemStorage *storage2;     /* contains approximated contours
								(!=storage1 if approx_method2 != approx_method1) */
	HvMemStorage *cinfo_storage;        /* contains _CvContourInfo nodes */
	HvSet *cinfo_set;           /* set of _CvContourInfo nodes */
	HvMemStoragePos initial_pos;        /* starting storage pos */
	HvMemStoragePos backup_pos; /* beginning of the latest approx. contour */
	HvMemStoragePos backup_pos2;        /* ending of the latest approx. contour */
	schar *img0;                /* image origin */
	schar *img;                 /* current image row */
	int img_step;               /* image step */
	HvSize img_size;            /* ROI size */
	HV_POINT offset;             /* ROI offset: coordinates, added to each contour point */
	HV_POINT pt;                 /* current scanner position */
	HV_POINT lnbd;               /* position of the last met contour */
	int nbd;                    /* current mark val */
	_HvContourInfo *l_cinfo;    /* information about latest approx. contour */
	_HvContourInfo cinfo_temp;  /* temporary var which is used in simple modes */
	_HvContourInfo frame_info;  /* information about frame */
	HvSeq frame;                /* frame itself */
	int approx_method1;         /* approx method when tracing */
	int approx_method2;         /* final approx method */
	int mode;                   /* contour scanning mode:
								0 - external only
								1 - all the contours w/o any hierarchy
								2 - connected components (i.e. two-level structure -
								external contours and holes) */
	int subst_flag;
	int seq_type1;              /* type of fetched contours */
	int header_size1;           /* hdr size of fetched contours */
	int elem_size1;             /* elem size of fetched contours */
	int seq_type2;              /*                                       */
	int header_size2;           /*        the same for approx. contours  */
	int elem_size2;             /*                                       */
	_HvContourInfo *cinfo_table[126];
}
_HvContourScannerOrg;

#define _HV_FIND_CONTOURS_FLAGS_EXTERNAL_ONLY    1
#define _HV_FIND_CONTOURS_FLAGS_HIERARCHIC       2

/**************************** Connected Component  **************************************/

typedef struct HvConnectedCompNew
{
	double area;    /* area of the connected component  */
	HvScalar value; /* average color of the connected component */
	HvRect rect;    /* ROI of the component  */
	HvSeq* contour; /* optional component boundary
					(the contour might have child contours corresponding to the holes)*/
}
HvConnectedCompNew;

/*
Internal structure that is used for sequental retrieving contours from the image.
It supports both hierarchical and plane variants of Suzuki algorithm.
*/
typedef struct _HvContourScannerOrg* HvContourScannerOrg;

/* contour retrieval mode */
#define HV_RETR_EXTERNAL 0
#define HV_RETR_LIST     1
#define HV_RETR_CCOMP    2
#define HV_RETR_TREE     3

/* contour approximation method */
#define HV_CHAIN_CODE               0
#define HV_CHAIN_APPROX_NONE        1
#define HV_CHAIN_APPROX_SIMPLE      2
#define HV_CHAIN_APPROX_TC89_L1     3
#define HV_CHAIN_APPROX_TC89_KCOS   4
#define HV_LINK_RUNS                5

/* Freeman chain reader state */
typedef struct HvChainPtReader
{
	HV_SEQ_READER_FIELDS()
		char      code;
	HV_POINT   pt;
	schar     deltas[8][2];
}
HvChainPtReader;

/* Contour tree header */
typedef struct HvContourTree
{
	HV_SEQUENCE_FIELDS()
		HV_POINT p1;            /* the first point of the binary tree root segment */
	HV_POINT p2;            /* the last point of the binary tree root segment */
}
HvContourTree;

/* Finds a sequence of convexity defects of given contour */
typedef struct HvConvexityDefect
{
	HV_POINT* start; /* point of the contour where the defect begins */
	HV_POINT* end; /* point of the contour where the defect ends */
	HV_POINT* depth_point; /* the farthest from the convex hull point within the defect */
	float depth; /* distance between the farthest point and the convex hull */
}
HvConvexityDefect;

/* curvature: 0 - 1-curvature, 1 - k-cosine curvature. */
HvStatus  ihvApproximateChainTC89( HvChain*      chain,
								  int header_size,
								  HvMemStorage* storage,
								  HvSeq**   contour,
								  int method );




int hvFindContours( HvArr* image, HvMemStorage* storage, HvSeq** first_contour,
						   int header_size HV_DEFAULT(sizeof(HvContour)),
						   int mode HV_DEFAULT(HV_RETR_LIST),
						   int method HV_DEFAULT(HV_CHAIN_APPROX_SIMPLE),
						   HV_POINT offset HV_DEFAULT(hvPoint(0,0)));


#endif
