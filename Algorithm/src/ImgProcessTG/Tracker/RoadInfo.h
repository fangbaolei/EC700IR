#pragma once

#include "svBase/svBase.h"

namespace swTgApp
{
    // 车道信息（车道线）
    typedef struct _SV_ROAD_LINE_INFO
    {
        //边线的坐标与类型
        sv::SV_POINT ptTop;     // 车道线上标定点百分比
        sv::SV_POINT ptBottom;  // 车道线下标定点百分比
        int iRoadType;      // 该线右边车道的类型，目前无效
    } SV_ROAD_LINE_INFO;

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
        sv::SV_RESULT Init(int iFrameWidth, int iFrameHeight, const SV_ROAD_LINE_INFO* rgRoadLine, int iRoadLineCnt);

        int GetRoadNum(sv::SV_POINT pt);

        int GetRoadWidth(sv::SV_POINT pt, int* pRoadNum);
        int GetRoadWidth(int iRoadNum, int iY);
        int GetRoadLineX(int iLineNum, int iLineY);
        int GetRoadLineY(int iLineNum, int iLineX);

        int GetRoadLineCount();
        int GetRoadCount();
        int GetRoadType(int iRoadNum);

        int GetSceneWidth();       // 取设置的原场景宽
        int GetSceneHeight();      // 取设置的原场景高

        // 设置关注区域，即设置后传入所有参数可不用加偏移，但注意用后要设回来
        void SetROI(sv::SV_RECT rcROI);
        sv::SV_RECT GetROI();
        void ResetROI();
        int GetROIWidth();
        int GetROIHeight();
        int GetRoadLinePointY0(); // 获取车道线0与左边交点坐标的Y值
    private:
        float iswGetLineX(sv::SV_POINT pa, sv::SV_POINT pb, int iLineY);
        float iswGetLineY(sv::SV_POINT pa, sv::SV_POINT pb, int iLineX);
        SV_ROAD_LINE_INFO m_rgRoadLine[MAX_ROAD_INFO];
        int  m_iRoadCount;
        int m_iSceneWidth;
        int m_iSceneHeight;
        sv::SV_RECT m_rcROI;
        int m_nRoadLinePointY0;
    };

    // 直线类
    class CSvLine
    {
    public:
        CSvLine();
        CSvLine(int x0, int y0, int x1, int y1);
        CSvLine(sv::SV_POINT pt0, sv::SV_POINT pt1);
        void Reset(int x0, int y0, int x1, int y1);
        void Reset(sv::SV_POINT pt0, sv::SV_POINT pt1);

        int GetLineX(int y);
        int GetLineY(int x);

        // y = K * x + B 中的系数 或 x = K * y + B
        float m_fltK;
        float m_fltB;

    private:
        sv::SV_BOOL m_fKx;  // 是否是 y = K * x + B 类型，否则是 x = K * y + B 类型
    };

    /// 梯形检测区域，
    /// 主要用于确定左右出结果边界
    struct SV_TRAP_AREA
    {
        //四边形扫描区域四个点的坐标
        int nTopLeftX;
        int nTopLeftY;
        int nTopRightX;
        int nTopRightY;
        int nBottomLeftX;
        int nBottomLeftY;
        int nBottomRightX;
        int nBottomRightY;
        SV_TRAP_AREA()
        {
            nTopLeftX = 27;
            nTopLeftY = 24;
            nTopRightX = 67;
            nTopRightY = 24;
            nBottomLeftX = 7;
            nBottomLeftY = 100;
            nBottomRightX = 90;
            nBottomRightY = 100;
        }
    };

    class CSvTrapArea : private SV_TRAP_AREA
    {
    public:
        enum OUT_TYPE
        {
            OT_NOT_OUT = 0,
            OT_LEFT = 1,
            OT_TOP = 2,
            OT_RIGHT = 4,
            OT_BOTTOM = 8
        };
        CSvTrapArea();
        ~CSvTrapArea();

        /// 传入的坐标为百分比*100
        sv::SV_RESULT Init(
            int nFrameWidth,   // 原始图像宽
            int nFrameHeight,  // 原始图像高
            int nTopLeftX,
            int nTopLeftY,
            int nTopRightX,
            int nTopRightY,
            int nBottomLeftX,
            int nBottomLeftY,
            int nBottomRightX,
            int nBottomRightY
        );

        /// 是否包含在区域内
        sv::SV_BOOL IsContainOfPoint(sv::SV_POINT pt);
        /// 是否在区域外
        sv::SV_BOOL IsOutOfArea(sv::SV_POINT pt, int* pOutTypeBit = NULL);

        /// 取场景宽高
        int GetSceneWidth();
        int GetSceneHeight();
        sv::SV_BOOL IsVaild();

    private:
        float m_fltSlopeLeft;
        float m_fltSlopeTop;
        float m_fltSlopeRight;
        float m_fltSlopeBottom;

        /// 初始化的图像宽高
        int m_nSceneWidth;
        int m_nSceneHeight;
        sv::SV_BOOL m_fInit;
    };

    class CSvLeftTurnArea
    {
    public:
        CSvLeftTurnArea();
        ~CSvLeftTurnArea();

        sv::SV_RESULT Init(sv::SV_POINT rgptLeftTurnWaitArea[2],
                           sv::SV_POINT rgRoadStopLine[2],  // 停止线上两点
                           int nFrameWidth, int nFrameHeigh);

        sv::SV_BOOL IsOutOfArea(sv::SV_POINT ptCheck);

        // 左转区是否有效
        sv::SV_BOOL IsVaild();

    private:
        float m_fltB;
        float m_fltA;
        sv::SV_BOOL m_fInit;
    };

}
