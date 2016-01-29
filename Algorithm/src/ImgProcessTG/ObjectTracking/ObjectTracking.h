#ifndef _OBJECTTRACKING_H_
#define _OBJECTTRACKING_H_

#include "swImageObj.h"
#include "hvBaseObj.h"

#include "hvinterface.h"
#include "HvCompute.h"
#include "HvImageNew.h"
//#include "Tracker.h"
#include "HvContours.h"
#include "HvObjShift.h"

#define SEQ_SIZE_MAX    30
#define SEQ_NUM         1000

using namespace HiVideo;

typedef struct struct_objdetect_info
{
    int*   piRectCount;
    int*   piRectEPCount;
    CRect* pRects;
    CRect* pEPRects;
    int    iMaxCount;
    int    iDetectKind;
    int*   piRectModelCount;
    CRect* pModelRects;
    int    iDownX;                 //X方向缩放比例
    int    iDownY;                 //Y方向缩放比例  
    CRect* pVideoDetArea;         //参数输入的视频检测区域
    int    iSrcH;                 //原图高
    int    iSrcW;                 //原图宽
    CPoint* pOffSet;
}OBJDETECT_INFO;

// zhaopy 0624 
struct ObjDetInfo
{
	HiVideo::CRect* videoDet;
	int*   piRectLightCount;
	CRect* pLightRects;
};

const int MAX_LIGHT_NIGHT_COUNT = 1;

//移动物体检测接口
class IObjectDetector
{
public:
	// zhaopy 0624
	//处理一帧视频, 取得移动物体数量
	virtual HRESULT Process(CPoint offset, const HV_COMPONENT_IMAGE *pCompImage, CRect *pRects, int iMaxRectCount, int *piRectCount, bool fIsNight, ObjDetInfo& detInfo) = 0;

	    
    virtual HRESULT ProcessEx(CPoint offset, 
                              const HV_COMPONENT_IMAGE *pCompImage, 
                              bool fIsNight, OBJDETECT_INFO& objDetectInfo) = 0;
                                 
	//清除背景模型
	virtual HRESULT ClearBackgroup() = 0;
	//取得背景亮度
	virtual HRESULT GetAvgGrey(int &iAvgGrey) = 0;
	//设置车牌后五字宽度查找表, 用来计算对应高度的车辆宽度
	virtual HRESULT SetPlateWidth(int *piPlateFiveCharWidth, int iCount, int iHeightStep) = 0;
	//释放模块
	virtual HRESULT Release() = 0;

	virtual HRESULT SetHvParam(HvCore::IHvParam2* pHvParam) = 0;
	
    virtual HRESULT GetCurrFrameGray(IReferenceComponentImage** pGray) = 0;

    virtual HRESULT GetCurrFG(IReferenceComponentImage** pGray) = 0;

	// zhaopy 0624
	virtual IReferenceComponentImage* GetCurrFrame() = 0;
	virtual int GetMultiDownSampleScale() = 0;

	virtual HRESULT GetVersion(char* szBuf, int szBufLen) = 0;
};

//创建移动物体检测模块
HRESULT CreateObjectDetector(IObjectDetector **pObj);

//移动物体背景接口
class IObjTrackBackgroup
{
public:
	//预处理背景
	virtual HRESULT Prepare(IReferenceComponentImage *pRefImage) = 0;
	//清除背景
	virtual HRESULT Clear() = 0;
	//释放模块
	virtual HRESULT Release() = 0;
	
	//add by: qinyj for: 修正范围(20090409)
	//修改范围参数
	virtual HRESULT SetLimitedRange(const HV_RECT &rect) = 0;
	
	//add by: qinyj for:修正范围(20090409)
	//读取范围参数
	virtual HRESULT GetLimitedRange(HV_RECT &rect) = 0;
};

//创建移动物体背景模块
HRESULT CreateObjTrackBackgroup(IObjTrackBackgroup **pObj);

//移动物体跟踪接口
class IObjectTracker
{
public:
	//重新刷新模板
	virtual HRESULT PrepareTemplate(IObjTrackBackgroup *pObjTrackBackgroup, CRect rcOrig) = 0;
	//跟踪一帧
	virtual HRESULT ProcessOneFrame(IObjTrackBackgroup *pObjTrackBackgroup, CRect rcOrig, CRect *prcNew) = 0;
	//引用接口
	virtual HRESULT AddRef() = 0;
	virtual HRESULT Release() = 0;
};

//创建物体跟踪模块
HRESULT CreateObjectTracker(IObjectTracker **ppObj);

class CObjectDetector: public IObjectDetector
{
public:
	CObjectDetector()
		:m_iDownSampleScale(1)
		,m_pElementDay(NULL)
		,m_pElementNight(NULL)
		,m_pElementLight(NULL)
		,m_pElementNoon(NULL)
		,m_pElementBig(NULL)
		,m_pImgOldFrame(NULL)
		,m_cFiveCharWidth(0)
		,m_iHeightStep(0)
		,m_pHvParam(NULL)
		,m_pImgFG(NULL)
		,m_pImgNewFrame(NULL)
		,m_pOld(NULL) // zhaopy 0624
	{
	}
	~CObjectDetector()
	{
		ClearBackgroup();
	}
	//处理一帧视频, 取得移动物体数量
	virtual HRESULT Process(CPoint offset, const HV_COMPONENT_IMAGE *pCompImage, CRect *pRects, int iMaxRectCount, int *piRectCount, bool fIsNight, ObjDetInfo& detInfo);
	    //电警条件下视频检测
    virtual HRESULT ProcessEx(CPoint offset, 
                              const HV_COMPONENT_IMAGE *pCompImage, 
                              bool fIsNight, OBJDETECT_INFO& objDetectInfo);
                              
	//清除背景模型
	virtual HRESULT ClearBackgroup();
	//取得背景亮度
	virtual HRESULT GetAvgGrey(int &iAvgGrey);
	//设置车牌后五字宽度查找表, 用来计算对应高度的车辆宽度
	virtual HRESULT SetPlateWidth(int *piPlateFiveCharWidth, int iCount, int iHeightStep);
	virtual HRESULT SetHvParam(HvCore::IHvParam2* pHvParam);
	//释放模块
	virtual HRESULT Release()
	{
		delete this;
		return S_OK;
	}
    virtual HRESULT GetCurrFrameGray(IReferenceComponentImage** pGray);
    virtual HRESULT GetCurrFG(IReferenceComponentImage** pFG);
	
	// zhaopy 0624
	virtual IReferenceComponentImage* GetCurrFrame();
	virtual int GetMultiDownSampleScale();
	HRESULT FindCarLight(HvSeq* pLights, HV_COMPONENT_IMAGE *pBin, HvMemStorage* storage);//夜景找灯

	virtual HRESULT GetVersion(char* szBuf, int szBufLen);

protected:
	int m_nNewWidth;					// DownSample后的图象宽度
	int m_nNewHeight;					// DownSample后的图象高度

	//视频图像DOWNSAMPLE的倍数
	int m_iDownSampleScale;
	int m_iDownX;
    int m_iDownY;
	int m_iAvgGrey;

	HvCore::IHvParam2* m_pHvParam;

	HvConvKernel* m_pElementDay;
	HvConvKernel* m_pElementNight;
	HvConvKernel* m_pElementNoon;
	HvConvKernel* m_pElementBig;
	HvConvKernel* m_pElementLight;
	
	//帧差法保存数据
	IReferenceComponentImage *m_pImgOldFrame;		//add by: qinyj  for: 帧差法
	IReferenceComponentImage *m_pImgNewFrame;
    IReferenceComponentImage *m_pImgFG;

	IReferenceComponentImage *m_pOld;      //开辟空间保存新帧差图  zhaopy 0624

	//车牌后五字宽度对应表
	int m_rgiFiveCharWidth[40];
	int m_cFiveCharWidth;
	int m_iHeightStep;
	int GetMaxCarWidth(int x, int y);
	
    //void MObjPickSimple(HV_COMPONENT_IMAGE* pFG, HvCarSeq* pCars, HvMemStorage* storage);
    void MObjPickSimple(HV_COMPONENT_IMAGE* pFG, HvSeq* pCars, HvMemStorage* storage);
    HRESULT SegmentCarEP2(HV_COMPONENT_IMAGE* pGray,
                         HV_COMPONENT_IMAGE* pFG,
                         HV_COMPONENT_IMAGE* pTempImg,
                         HV_COMPONENT_IMAGE* pImgYUV,
                         HV_COMPONENT_IMAGE* pImgHSV,
                         OBJDETECT_INFO& objDetInfo,
                         HvSeq* detectCars);
                         //HvCarSeq& oldCars);
public:
	static int m_iProcessType;					// 处理类型 0:缺省 1:厦门行人检测项目
	static int m_iNightPlus;						// 晚上加强对非机动车的检测。场景够亮时才可使用。
	static int m_iShadowDetect;                //是否进行阴影检测
	static int m_iShadowEffect;                //阴影检测强弱程度值
	static int m_iDiffType;                    //0: 正常帧间差分; 1: 差分后提sobel边缘图 
	static int m_iMedFilter;                   //中值滤波开关
	static int m_iMedFilterUpLine;             //中值滤波作用区域控制
	static int m_iMedFilterDownLine;           //中值滤波作用区域控制
    static int m_iEPOutPutDetectLine;           //电警条件下检测控制线
    static int m_iEPFGDetectLine;               //前景检测线

	// zhaopy 0624
	static int m_iNightMerge;                  
	static int m_iLightDetectThreld;           //夜景灯组二值化阈值
	static int m_iInverseMaskThreld;           //反向mask阈值
	static int m_iLightGroup;                  //灯组跟踪开关
	static int m_iRectEnlage;                   //夜景矩形合并开关

	// ganzz 0219
	static int m_iDayBackgroudThreshold;	//白天背景检测阈值
	static int m_iNightBackgroudThreshold;	//晚上背景检测阈值
	static int m_iNightLightBinThreshold;   //晚上灯组跟踪检测二值化阈值
	static int m_iRoadNumber;					//车道线数
};

#endif
