/// @file
/// @brief 电警违章
/// @author ganzz
/// @date 11/6/2013 16:28:40
/// 
/// 修改说明:
/// [11/6/2013 16:28:40 ganzz] 最初版本
/// 

#pragma once

#include "EPTrackInfo.h"

bool CheckReverseRun(
    CEPTrackInfo* pEPObj
    );

void CheckRunType(
    CEPTrackInfo* pEPObj,
    CRoadInfo* pRoadInfo,
    svEPApi::EP_PARAM* pEPParam
    );
// 返回是否越线
bool CheckCrossLine(
    CEPTrackInfo* pEPObj,
    CRoadInfo* pRoadInfo,
    svEPApi::EP_PARAM* pEPParam,
    int* pnRoadNum,
    ACTIONDETECT_PARAM_TRACKER* pParam
    );

// 返回是否压线
bool CheckPressRoadLine(
    CEPTrackInfo* pEPObj,
    CRoadInfo* pRoadInfo,
    float fltOverLineSensitivity,
    int nStopLinePos,  // 绝对坐标
    int* pnRoadNum,
    ACTIONDETECT_PARAM_TRACKER* pParam
    );
