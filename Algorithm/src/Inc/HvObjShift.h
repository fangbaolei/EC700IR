#ifndef __HV_OBJ_SHIFT_H__
#define __HV_OBJ_SHIFT_H__

#include "HvBaseType.h"
#include "HvHistogram.h"
#include "HvImageNew.h"

/*********************************** HvTermCriteria *************************************/
#define HV_TERMCRIT_ITER    1
#define HV_TERMCRIT_EPS     2

/****************************************************************************************\
*                             Common macros and inline functions                         *
\****************************************************************************************/
#define HV_PI   3.1415926535897932384626433832795
#define HV_SWAP(a,b,t) ((t) = (a), (a) = (b), (b) = (t))


/* spatial and central moments */
typedef struct HvMoments
{
	DWORD32 m00, m10, m01;
	QWORD64 m20, m11, m02, m30, m21, m12, m03; /* spatial moments */
//	double  m00, m10, m01, m20, m11, m02, m30, m21, m12, m03; /* spatial moments */
	double  mu20, mu11, mu02, mu30, mu21, mu12, mu03; /* central moments */
	double  inv_sqrt_m00; /* m00 != 0 ? 1/sqrt(m00) : 0 */
}
HvMoments;


/**************************** Connected Component  **************************************/
typedef struct HvConnectedComp
{
	double area;    /* area of the connected component  */
	HvScalar value; /* average color of the connected component */
	HV_RECT rect;    /* ROI of the component  */
}
HvConnectedComp;


typedef struct HvTermCriteria
{
	int    type;  /* may be combination of
				  HV_TERMCRIT_ITER
				  HV_TERMCRIT_EPS */
	int    max_iter;
	double epsilon;
}
HvTermCriteria;

inline  HvTermCriteria  hvTermCriteria( int type, int max_iter, double epsilon )
{
	HvTermCriteria t;

	t.type = type;
	t.max_iter = max_iter;
	t.epsilon = (float)epsilon;

	return t;
}

////////////////////////////Ïà¹Øº¯Êý//////////////////////////////////
void hvMoments( HvMat* array, HvMoments* moments, int binary );
HvTermCriteria hvCheckTermCriteria( HvTermCriteria criteria, double default_eps,
								   int default_max_iters );
HvMat* hvGetSubRect( const HvMat* arr, HvMat* submat, HV_RECT rect );
int hvMeanShift( HvMat* imgProb, HV_RECT windowIn,
				HvTermCriteria criteria, HvConnectedComp* comp );
int hvCamShift( HvMat* imgProb, HV_RECT windowIn,
			    HvTermCriteria criteria, HvConnectedComp* comp);

int hvCamShiftImg( HV_COMPONENT_IMAGE* imgSrc, HV_RECT windowIn,
				  HvTermCriteria criteria,
				  HvConnectedComp* comp);



#endif		// #ifndef __HV_OBJ_SHIFT_H__
