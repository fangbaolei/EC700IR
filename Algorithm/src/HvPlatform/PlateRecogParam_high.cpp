#include "platerecogparam.h"

PlateRecogParam g_rgPlateRecogParam[PRM_MaxMode]=
{
	{
		// 收费站
		PRM_TOLLGATE,
		PR_SPEED_NORMAL,
		MD_TOP2BOTTOM,			// g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{0, 30, 95, 90},		// g_rectDetectionArea
		{0, 0, 100, 90},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		0.5f,					// g_fltTrackInflateY
		3,						// g_nContFrames_EstablishTrack;
		15,						// g_nMissFrames_EndTrack;
		10,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.15f,					// g_fltThreshold_StartTrack
		0.12f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		6,						// g_nDetMinScaleNum
		14,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		6,						// g_nMinStopPlateDetNum
		false,					// g_fSubscanPredict
		0,						// g_nSubscanPredictMinScale
		0,						// g_nSubscanPredictMaxScale

		0.075f,					// g_kfltPlateInflateRate
		0.2f,					// g_kfltPlateInflateRateV
		0.2f,					// g_kfltHighInflateArea
		0.15f,					// g_kfltPlateInflateRate2
		0.3f,					// g_kfltPlateInflateRateV2

		false,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		    //电子警察开关
		0,						//m_fUseTemplet;				// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		0,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 高速路卡口 - 车头
		PRM_HIGHWAY_HEAD,
		PR_SPEED_FAST,
		MD_TOP2BOTTOM,			// g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{0, 15, 100, 55},		// g_rectDetectionArea
		{0, 10, 100, 90},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		5,						// g_nDetMinScaleNum, 85~170
		9,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-1,						// g_nSubscanPredictMinScale
		2,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 高速路卡口 - 车尾
		PRM_HIGHWAY_TAIL,
		PR_SPEED_FAST,
		MD_BOTTOM2TOP,			//g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{0, 40, 100, 90},		// g_rectDetectionArea
		{0, 20, 100, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		6,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		7,						// g_nDetMinScaleNum
		13,						// g_nDetMaxScaleNum
		16,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-2,						// g_nSubscanPredictMinScale
		1,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 移动稽查 - 左侧
		PRM_PATROL_LEFT,
		PR_SPEED_FAST,
		MD_LEFT2RIGHT,			// g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{0, 0, 50, 100},		// g_rectDetectionArea	//遮挡比较多
		{0, 0, 80, 100},		// g_rectTrackArea

		0.75f,					// g_fltTrackInflateX
		0.5f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		5,						// g_nDetMinScaleNum
		12,						// g_nDetMaxScaleNum
		60,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-3,						// g_nSubscanPredictMinScale
		1,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.15f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		////for plate segmentation
		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		0,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 移动稽查 - 右侧
		PRM_PATROL_RIGHT,
		PR_SPEED_FAST,
		MD_RIGHT2LEFT,			// g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{50, 0, 100, 100},		// g_rectDetectionArea	//遮挡比较多
		{20, 0, 100, 100},		// g_rectTrackArea

		0.75f,					// g_fltTrackInflateX
		0.5f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		5,						// g_nDetMinScaleNum
		12,						// g_nDetMaxScaleNum
		60,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-3,						// g_nSubscanPredictMinScale
		1,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.15f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		0,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		//随意拍
		PRM_PORTABLE,
		PR_SPEED_NORMAL,		// g_PlateRecogSpeed
		MD_NOTSURE,				// g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{0, 0, 100, 100},		// g_rectDetectionArea
		{0, 0, 100, 100},		// g_rectTrackArea

		1.0f,					// g_fltTrackInflateX
		1.0f,					// g_fltTrackInflateY
		3,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.12f,					// g_fltThreshold_StartTrack
		0.10f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		8,						// g_nDetMinScaleNum
		18,						// g_nDetMaxScaleNum
		60,						// g_nDetMinStdVar
		4,						// g_nMinStopPlateDetNum
		false,					// g_fSubscanPredict
		0,						// g_nSubscanPredictMinScale
		0,						// g_nSubscanPredictMaxScale

		0.075f,					// g_kfltPlateInflateRate
		0.15f,					// g_kfltPlateInflateRateV
		0.2f,					// g_kfltHighInflateArea
		0.15f,					// g_kfltPlateInflateRate2
		0.3f,					// g_kfltPlateInflateRateV2

		false,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		1000,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		200,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		18,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		0,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 电子警察
		PRM_ELECTRONIC_POLICE,
		PR_SPEED_FAST,
		MD_BOTTOM2TOP,			//g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
		{2, 60, 98, 100},		// g_rectDetectionArea
		{2, 40, 98, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		6,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		6,						// g_nDetMinScaleNum
		10,						// g_nDetMaxScaleNum
		16,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-2,						// g_nSubscanPredictMinScale
		1,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		1,						//m_fUsedTrafficLight;		//电子警察开关
		1,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 交通事件检测 - 车头
		PRM_TRAFFICE_EVENT_HEAD,
		PR_SPEED_FAST,
		MD_TOP2BOTTOM,			// g_MovingDirection

		0,						// g_cnDetStep, 高清不使用
 		{0, 0, 100, 50},		// g_rectDetectionArea
		{0, 0, 100, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		5,						// g_nDetMinScaleNum, 85~170
		9,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-1,						// g_nSubscanPredictMinScale
		2,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		1,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 交通事件检测 - 车尾
		PRM_TRAFFICE_EVENT_TAIL,
		PR_SPEED_FAST,
		MD_BOTTOM2TOP,			//g_MovingDirection

		0,							// g_cnDetStep, 高清不使用
		{0, 40, 100, 80},		// g_rectDetectionArea
		{0, 20, 100, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		6,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		7,						// g_nDetMinScaleNum
		13,						// g_nDetMaxScaleNum
		16,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-2,						// g_nSubscanPredictMinScale
		1,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		1,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		//抓拍识别
		PRM_HVC,
		PR_SPEED_NORMAL,
		MD_TOP2BOTTOM,			// g_MovingDirection

		0,							// g_cnDetStep, 高清不使用
		{0, 0, 100, 100},		// g_rectDetectionArea
		{0, 0, 100, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		0.5f,					// g_fltTrackInflateY
		3,						// g_nContFrames_EstablishTrack;
		15,						// g_nMissFrames_EndTrack;
		10,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.15f,					// g_fltThreshold_StartTrack
		0.12f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		6,						// g_nDetMinScaleNum
		14,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		6,						// g_nMinStopPlateDetNum
		false,					// g_fSubscanPredict
		0,						// g_nSubscanPredictMinScale
		0,						// g_nSubscanPredictMaxScale

		0.075f,					// g_kfltPlateInflateRate
		0.2f,					// g_kfltPlateInflateRateV
		0.2f,					// g_kfltHighInflateArea
		0.15f,					// g_kfltPlateInflateRate2
		0.3f,					// g_kfltPlateInflateRateV2

		false,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		400,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		0,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 停车场双路
		PRM_PARK,
		PR_SPEED_NORMAL,
		MD_NOTSURE,			// g_MovingDirection

		12,						// g_cnDetStep
		{20, 20, 80, 80},		// g_rectDetectionArea
		{0, 0, 100, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		0.5f,					// g_fltTrackInflateY
		3,						// g_nContFrames_EstablishTrack;
		15,						// g_nMissFrames_EndTrack;
		10,						// g_nMissFrames_EndTrackQuick;
		5,						// g_nObservedFrames_ConsiderAsTrack;
		0.15f,					// g_fltThreshold_StartTrack
		0.12f,					// g_fltThreshold_BeObservation
		0.4f,					// g_fltMinConfidenceForVote

		////for plate detection
		6,						// g_nDetMinScaleNum
		12,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		6,						// g_nMinStopPlateDetNum
		false,					// g_fSubscanPredict
		0,						// g_nSubscanPredictMinScale
		0,						// g_nSubscanPredictMaxScale

		0.075f,					// g_kfltPlateInflateRate
		0.2f,					// g_kfltPlateInflateRateV
		0.2f,					// g_kfltHighInflateArea
		0.15f,					// g_kfltPlateInflateRate2
		0.3f,					// g_kfltPlateInflateRateV2

		false,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		true,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;				// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;			//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;		//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 停车场单路
		PRM_PARK_SINGLE,
		PR_SPEED_NORMAL,
		MD_NOTSURE,			// g_MovingDirection

		12,						// g_cnDetStep
		{20, 20, 80, 80},		// g_rectDetectionArea
		{0, 0, 100, 100},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		0.5f,					// g_fltTrackInflateY
		3,						// g_nContFrames_EstablishTrack;
		15,						// g_nMissFrames_EndTrack;
		10,						// g_nMissFrames_EndTrackQuick;
		5,						// g_nObservedFrames_ConsiderAsTrack;
		0.15f,					// g_fltThreshold_StartTrack
		0.12f,					// g_fltThreshold_BeObservation
		0.4f,					// g_fltMinConfidenceForVote

		////for plate detection
		6,						// g_nDetMinScaleNum
		12,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		6,						// g_nMinStopPlateDetNum
		false,					// g_fSubscanPredict
		0,						// g_nSubscanPredictMinScale
		0,						// g_nSubscanPredictMaxScale

		0.075f,					// g_kfltPlateInflateRate
		0.2f,					// g_kfltPlateInflateRateV
		0.2f,					// g_kfltHighInflateArea
		0.15f,					// g_kfltPlateInflateRate2
		0.3f,					// g_kfltPlateInflateRateV2

		false,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		true,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;			// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;		//车辆逆行检测开关
		0,						//m_iDetectOverYellowLineEnable;//车辆压黄线检测开关
		0,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 卡口人脸抓拍
		PRM_CAP_FACE,
		PR_SPEED_FAST,
		MD_TOP2BOTTOM,			// g_MovingDirection

		0,							// g_cnDetStep, 高清不使用
		{0, 15, 100, 55},		// g_rectDetectionArea
		{0, 10, 100, 90},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		5,						// g_nDetMinScaleNum, 85~170
		9,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-1,						// g_nSubscanPredictMinScale
		2,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;				// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;			//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;		//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
	{
		// 断面控制器
		PRM_COVER_CTRL,
		PR_SPEED_FAST,
		MD_TOP2BOTTOM,			// g_MovingDirection

		0,							// g_cnDetStep, 高清不使用
		{0, 15, 100, 55},		// g_rectDetectionArea
		{0, 10, 100, 90},		// g_rectTrackArea

		0.5f,					// g_fltTrackInflateX
		1.6f,					// g_fltTrackInflateY
		2,						// g_nContFrames_EstablishTrack;
		3,						// g_nMissFrames_EndTrack;
		0,						// g_nMissFrames_EndTrackQuick;
		0,						// g_nObservedFrames_ConsiderAsTrack, 高清不使用;
		0.08f,					// g_fltThreshold_StartTrack
		0.08f,					// g_fltThreshold_BeObservation
		0.0f,					// g_fltMinConfidenceForVote, 高清不使用；

		////for plate detection
		5,						// g_nDetMinScaleNum, 85~170
		9,						// g_nDetMaxScaleNum
		30,						// g_nDetMinStdVar
		0,						// g_nMinStopPlateDetNum
		true,					// g_fSubscanPredict
		-1,						// g_nSubscanPredictMinScale
		2,						// g_nSubscanPredictMaxScale

		0.05f,					// g_kfltPlateInflateRate
		0.10f,					// g_kfltPlateInflateRateV
		0.0f,					// g_kfltHighInflateArea
		0.0f,					// g_kfltPlateInflateRate2
		0.0f,					// g_kfltPlateInflateRateV2

		true,					// g_fSegPredict

		60,						// g_nMinPlateWidth, 进行识别的最小车牌宽度（以分割后的结果为准）
		200,					// g_nMaxPlateWidth, 进行识别的最大车牌宽度（以分割后的结果为准）
		false,					// g_fBestResultOnly, 如果图像中有多个识别结果，只输出最好的

		400,					// g_nMinDownSampleWidthInSeg, 分割时DownSample的最小宽度
		52,						// g_nMinDownSampleHeightInSeg, 分割时DownSample的最小高度

		0,						//m_fUsedTrafficLight;		//电子警察开关
		0,						//m_fUseTemplet;				// 模版匹配开关

		0,						//m_iDetectCarStopEnable;		//车辆停止检测开关
		1,						//m_iDetectReverseEnable;			//车辆逆行检测开关
		1,						//m_iDetectOverYellowLineEnable;		//车辆压黄线检测开关
		1,						//m_iDetectCrossLineEnable;		//车辆跨线检测开关
	},
};
