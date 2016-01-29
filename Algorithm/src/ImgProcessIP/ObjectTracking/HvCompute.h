//////////////////////////////////////////////////
//HvCompute.h
//author:  liujie
//company: signalway
//date:    2010-08-29
//////////////////////////////////////////////////

#ifndef _HVCOMPUTE_H
#define _HVCOMPUTE_H

#include <math.h>
#include "HvBaseType.h"
#include "swImageObj.h"
#include "swBaseType.h"
#include "swImage.h"
#include "hvbaseobj.h"
#include "hv_math.h"

//#include "..\tracker\TrackInfoHigh.h"
#include "HvContours.h"
#include "ObjectTracking.h"

#define MAX_FIGHT_GROUP	0.88f			// 两灯合并灯组的最大比率 //0.68
#define MIN_X_CROSS_TO_MERGE	0.64f		// 两灯组合并的最小X方向交叉百分比       0.75f

#ifdef VERSION_IR
#define MAX_Y_DISTANCE_TO_MERGE	0.20f		// 两灯组合并的最大的Y方向上的距离百分比 //0.50
#else // #ifdef VERSION_IR
#define MAX_Y_DISTANCE_TO_MERGE	0.30f		// 两灯组合并的最大的Y方向上的距离百分比 //0.50
#endif // #ifdef VERSION_IR

#define MAX_PER_TRACK_DX			0.20f		// 保持检测跟踪的最大X 方向偏移百分比
#define MAX_PER_TRACK_DY			0.50f		// 保持检测跟踪的最大Y 方向偏移百分比

#define MIN_PER_WIDTH_CREATE		0.32f		// 车灯宽度建立跟踪的最小阈值
#define MIN_PER_SHORT_LIGHT_LEN     0.20f       // 删除的最小短灯比例

#define MAX_Y_DISTANCE_TO_MERGE_DUSK 0.35f


#define RECT_TOP(r) ((r).y)
#define RECT_BOTTOM(r) ((r).y + (r).height)
#define RECT_LEFT(r) ((r).x)
#define RECT_RIGHT(r) ((r).x + (r).width)
#define CPCIMG CPersistentComponentImage 
#define MAX_EP_QUEUE_ELEM 50
#define MAX_EP_QUEUE_NUM 100
typedef struct SplictInfo
{
    int lineWidth;
}SPLICTINFO;
//保存某连通分量中所有预测的阴影线X起始坐标和结束坐标
typedef struct DensityLen
{
	int iBegin;
	int iEnd;
}DR,*pDR;

typedef enum{
    COODINATE_RECT_SHIFT = 0,           //矩形平移
    COODINATE_RECT_SHRINK,               //矩形缩放
    COODINATE_RECT_SHRINK_SHIFT,         //矩形缩放后平移
    COODINATE_RECT_BROADEN,             //矩形扩大
    COODINATE_RECT_BROADEN_SHIFT        //矩形扩大后平移
}COODINATETRANSKIND;

//阴影线长度计算系数
const double rgShadowCoeff[5] = 
{
	1.0,             //0车道(未设置车道或无法读取车道情况),缺省系数
		1.0 + 2/3.0,     //单车道,原系数增加三分之二
		1.0 + 1/3.0,     //双车道,原系数增加三分之一
		1.0,             //三车道,缺省系数为1
		1.0 - 1/3.0      //四车道,原系数递减三分之一
};    

typedef struct {
    int nIdx;			// 序号
    HvRect rc;			// 外接矩形
} CAR_LIGHT_POS_INFO;	// 车灯位置信息

typedef struct {
    int nIdxL;			// 左边灯序号
    int nIdxR;			// 右边灯序号
} CAR_LIGHT_GROUP_INFO;	// 灯组信息
extern HV_RECT g_rgContourRect[];     //保存轮廓的数组

int EraseLine(IReferenceComponentImage *pImgAlloc1, IReferenceComponentImage *pImgAlloc2,
			  HV_COMPONENT_IMAGE *pCanvas, HV_RECT roi, int iThreld1, int iThreld2, int iZoom, int iRoadNum);
void EraseRegion(HV_COMPONENT_IMAGE *pImgData, HV_RECT roi);
void EraseRegion(HV_COMPONENT_IMAGE *pImgData, HvRect roi);
int ThreldCompute(HvSize imgSize, int iAlpha, int iBeta, int iMidY);
void ImageBitwiseExclusive(HV_COMPONENT_IMAGE* pPre, HV_COMPONENT_IMAGE* pNext,
						   HV_COMPONENT_IMAGE* pDst, HV_RECT roi);
void DensityCompute(HV_COMPONENT_IMAGE* pCanvas, HV_RECT roi, DR pHD[], DR pLD[], int iZoom, int iRoadNum);
void ImageCopyRoi(HV_COMPONENT_IMAGE *pSrc, HV_COMPONENT_IMAGE *pDst, HV_RECT roi);
int ShadowLenGuess(int iRoadNum, int iSrcWidth, int iZoom);
void ThreldSetting(int *Alpha1, int *Alpha2, int *Beta1, int *Beta2, int iDiffType, int iRoadNum);
void RefineCanvasRoi(HV_COMPONENT_IMAGE *pImg, HV_COMPONENT_IMAGE *pImgSobel, HV_RECT roi);
//void SaveImageGrayOT(const char *pDir, HV_COMPONENT_IMAGE *imgData, int fileCount);
void Medfilter(HV_COMPONENT_IMAGE *pSrc, HV_COMPONENT_IMAGE *pDst,
			   size_t iWinSize, int iImageType, HV_COMPONENT_IMAGE *pbMask);
HRESULT CutBottomArea(HV_COMPONENT_IMAGE* pImg, HV_RECT& roi);
HRESULT FilterLightByConnectNum(HV_COMPONENT_IMAGE* pImg, HV_COMPONENT_IMAGE* pDst, 
                                HV_RECT& roi, int iNum);
HRESULT LightBlobRemove(HV_COMPONENT_IMAGE* pSrc, 
                        HV_COMPONENT_IMAGE* pDst, 
                        int iDownScaleX, 
                        int iDownScaleY, 
                        HiVideo::CPoint& offset,
                        HvSize& orgSize);
			   
HRESULT HvImageYCbCrMultiDownSampleToYUV422(const HV_COMPONENT_IMAGE *pSrc,
                                            const HV_COMPONENT_IMAGE *pDst, int iDownX, int iDownY);
HRESULT CoodinateTrans(int iDownX, int iDownY, 
                       HiVideo::CPoint offset, 
                       HvRect& srcRect, HvRect& dstRect, 
                       HvSize srcSize, HvSize dstSize, 
                       COODINATETRANSKIND coodKind);

HRESULT CoodinateTrans(int iDownX, int iDownY, 
                       HiVideo::CPoint offset, 
                       HV_RECT& srcRect, HV_RECT& dstRect, 
                       HvSize srcSize, HvSize dstSize, 
                       COODINATETRANSKIND coodKind);
//区域生长
void RegionGrow(HV_COMPONENT_IMAGE* pFG, HV_COMPONENT_IMAGE* pGray, HV_RECT& r);
BOOL RainyFlashCheck(HV_COMPONENT_IMAGE* pImg, HV_RECT& roi, int iDownX, int iDownY);
BOOL RainyFlashCheck2(HV_COMPONENT_IMAGE* pImg, HV_RECT& roi, int iDownX, int iDownY);
int GetAvgRoi(HV_COMPONENT_IMAGE* pImg, HV_RECT& roi, HV_COMPONENT_IMAGE* pMask = NULL);
//找灯
float GetRoadWidthByY(float fltY);
float GetRoadWidthByY(float fltX, float fltY, int scale);
HvRect hvMaxRect( const HvRect* rect1, const HvRect* rect2 );
int CompareContourNight(const void* a, const void* b, void* );
bool IsLightGroup(HvRect rc1, HvRect rc2, int scale, float *pDis = NULL);
HRESULT AddToLightGroup(CAR_LIGHT_GROUP_INFO *pLightGroupInfo, 
                        CAR_LIGHT_POS_INFO *pInfo, HvSeq* lightGroup);
bool IsMergeLight(HvRect rc1, HvRect rc2, int scale);
BOOL RainyObjCheckInit(HV_COMPONENT_IMAGE* pImg, HV_RECT& roi);
HRESULT SetImgData(HV_COMPONENT_IMAGE* pSrc, int iNum);
HRESULT SetImgData2(HV_COMPONENT_IMAGE* pSrc, HV_COMPONENT_IMAGE* pDst);
HRESULT SetImgData(HV_COMPONENT_IMAGE* pSrc, HV_COMPONENT_IMAGE* pDst, int iNum);
HRESULT MaskImageTest(RESTRICT_PBYTE8 pbIn,
                      RESTRICT_PBYTE8 pbOut,
                      int iCols,
                      int iRows);
HRESULT SplitCarByProjection(HV_COMPONENT_IMAGE* pFG,
                             HvRect roi,
                             HvSeq* pNewCars,
                             SPLICTINFO& sinfo);
                            

BOOL EPRectModify(HV_COMPONENT_IMAGE* pBin, HvRect& roi, HvRect* rSplit, int iSplitNum);
HRESULT MaskOutLine(HV_COMPONENT_IMAGE* pBin,
                    int iDownScaleX, 
                    int iDownScaleY, 
                    HiVideo::CPoint& offset,
                    HV_RECT& videoDet);

//根据大顺光两车道情景修改
inline bool IsSmallComponent(bool fIsNight, int iShadowDet, int iProcessType, int iLeft, int iRight, int iImageWidth, int iGrey, int iRoadNum)
{
	if (iLeft < 5 || (iImageWidth - iRight) < 5 )
	{
		return false;
	}
	if((!fIsNight) && (!iProcessType) 
		&& (iRoadNum >= 2) 
		&& (iRoadNum <= 3) 
		&& (iShadowDet) && (iGrey > 130))
	{
		return true;
	}
	return false;
}

//计算阈值,按照线性规律,阈值T分布在(iAlpha,iBeta)之间
inline int ThreldCompute(HvSize imgSize, int iAlpha, int iBeta, int iMidY)
{
	return hvRound(iAlpha + ((double)(iBeta - iAlpha) / imgSize.height) * iMidY);
}

//获取密度计算阴影长度线预测
inline int ShadowLenGuess(int iRoadNum, int iSrcWidth)
{
	double dblBase = 15.0/720; //基数值
	//限制在4车道内
	if (iRoadNum > 4)
		iRoadNum = 4;
	
	return hvRound(dblBase * rgShadowCoeff[iRoadNum] * iSrcWidth);
}

//根据均值调整二值化阈值
inline int GrayMeanCutoff(int iGreyMean, int iThreld, int iTDefualt)
{
	return (iGreyMean < iThreld ? (hvRound(2 * sqrt((double)(iGreyMean)))) : (iTDefualt));
}
#endif



