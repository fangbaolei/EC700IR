#ifndef _ROAD_INFO_H_
#define _ROAD_INFO_H_

#include "hvbaseobj.h"

namespace HiVideo
{
    // 车道信息（车道线）
    struct HV_ROAD_LINE_INFO
    {
        //边线的坐标与类型
        HV_POINT ptTop;     // 车道线上标定点百分比
        HV_POINT ptBottom;  // 车道线下标定点百分比
        int iRoadType;      // 该线右边车道的类型，目前无效
    } ;

    class CRoadInfo
    {
    public:
        enum
        {
            MAX_ROAD_INFO = 5
        };
    public:
        CRoadInfo();
        ~CRoadInfo();

        // rgRoadLine车数线数组，传入的 rgRoadLine 为百分比
        HRESULT Init(int iFrameWidth, int iFrameHeight, const HV_ROAD_LINE_INFO* rgRoadLine, int iRoadLineCnt);

        int GetRoadNum(HV_POINT pt);

        int GetRoadWidth(HV_POINT pt, int* pRoadNum);
        int GetRoadWidth(int iRoadNum, int iY);
        int GetRoadLineX(int iLineNum, int iLineY);

        int GetRoadLineCount();
        int GetRoadCount();
        int GetRoadType(int iRoadNum);

        int GetSceneWidth();       // 取设置的原场景宽
        int GetSceneHeight();      // 取设置的原场景高

        // 设置关注区域，即设置后传入所有参数可不用加偏移，但注意用后要设回来
        void SetROI(HV_RECT rcROI);
        HV_RECT GetROI();
        void ResetROI();
        int GetROIWidth();
        int GetROIHeight();
    private:
        float iswGetLineX(HV_POINT pa, HV_POINT pb, int iLineY);
        HV_ROAD_LINE_INFO m_rgRoadLine[MAX_ROAD_INFO];
        int  m_iRoadCount;
        int m_iSceneWidth;
        int m_iSceneHeight;
        HV_RECT m_rcROI;
    };

}


#endif // _ROAD_INFO_H_
