#include "RoadInfo.h"
#include <math.h>
#include "stdio.h"

using namespace sv;

namespace swTgApp
{
    //////////////////////////////////////////////////////////////////////////
    // CRoadInfo impl
    //

    CRoadInfo::CRoadInfo()
    {

    }

    CRoadInfo::~CRoadInfo()
    {

    }

    SV_RESULT CRoadInfo::Init(int iFrameWidth, int iFrameHeight, const SV_ROAD_LINE_INFO* rgRoadLine, int iRoadLineCnt)
    {
        m_iRoadCount = iRoadLineCnt - 1;

        if((m_iRoadCount <= 0)
                || (m_iRoadCount >= MAX_ROAD_INFO))
        {
            return RS_E_INVALIDARG;
        }

        m_iSceneWidth = iFrameWidth;
        m_iSceneHeight = iFrameHeight;

        //         for(int i = 0; i < m_iRoadCount + 1; i++)
        //         {
        //             utTrace("pt_%d.m_nX:[%d] y:[%d], pt_%d.m_nX:[%d] y:[%d]\n",
        //                     i * 2, rgRoadLine[i].ptTop.m_nX, rgRoadLine[i].ptTop.m_nY,
        //                     i * 2 + 1, rgRoadLine[i].ptBottom.m_nX, rgRoadLine[i].ptBottom.m_nY);
        //         }

        //int iCount = 0;
        for(int i = 0; i <= m_iRoadCount; i++)
        {
            m_rgRoadLine[i].ptTop.m_nX = rgRoadLine[i].ptTop.m_nX * iFrameWidth / 100;
            m_rgRoadLine[i].ptTop.m_nY = rgRoadLine[i].ptTop.m_nY * iFrameHeight / 100;
            m_rgRoadLine[i].ptBottom.m_nX = rgRoadLine[i].ptBottom.m_nX * iFrameWidth / 100;
            m_rgRoadLine[i].ptBottom.m_nY = rgRoadLine[i].ptBottom.m_nY * iFrameHeight / 100;
            m_rgRoadLine[i].iRoadType = rgRoadLine[i].iRoadType;
        }

        //        utTrace("\nRoadNum %d, w:%d h%d", m_iRoadCount, iFrameWidth, iFrameHeight);

        //计算直线方程
        for(int i = 0; i <= m_iRoadCount; i++)
        {
            int dx = (m_rgRoadLine[i].ptBottom.m_nX - m_rgRoadLine[i].ptTop.m_nX);
            int dy = (m_rgRoadLine[i].ptBottom.m_nY - m_rgRoadLine[i].ptTop.m_nY);

            if(dx == 0)
            {
                //k 不存在
                //x 不变, y拓展
                m_rgRoadLine[i].ptTop.m_nY = 0;
                m_rgRoadLine[i].ptBottom.m_nY = iFrameHeight - 1;
            }
            else
            {
                float K = (float)dy / dx;
                float B = m_rgRoadLine[i].ptTop.m_nY - K * m_rgRoadLine[i].ptTop.m_nX;

                float UY = 0;
                float UX = (UY - B) / K;

                float DY = (float)(iFrameHeight - 1);
                float DX = (DY - B) / K;

                if(UX < 0)
                {
                    UX = 0;
                    UY = B;
                }

                if(DX < 0)
                {
                    DX = 0;
                    DY = B;
                }

                if(UX > iFrameWidth)
                {
                    UX = (float)(iFrameWidth - 1);
                    UY = UX * K + B;
                }

                if(DX > iFrameWidth)
                {
                    DX = (float)(iFrameWidth - 1);
                    DY = DX * K + B;
                }

                //更新
                m_rgRoadLine[i].ptTop.m_nX = (int)(UX + 0.5f);
                m_rgRoadLine[i].ptTop.m_nY = (int)(UY + 0.5f);

                m_rgRoadLine[i].ptBottom.m_nX = (int)(DX + 0.5f);
                m_rgRoadLine[i].ptBottom.m_nY = (int)(DY + 0.5f);

            }
        }

        ResetROI();
        m_nRoadLinePointY0 = -1;

        if(m_iRoadCount > 0)
        {
            m_nRoadLinePointY0 = ((int)GetRoadLineY(0, 0) * 100) / m_iSceneHeight;
        }

        m_nRoadLinePointY0 = m_nRoadLinePointY0 < 0 ? 100 : m_nRoadLinePointY0;
        return RS_S_OK;
    }

    // pa车道线起始点，pb车道线终点，给出线上的Y求X
    float CRoadInfo::iswGetLineX(SV_POINT pa, sv::SV_POINT pb, int iLineY)
    {
        int tx = pa.m_nX;
        int ty = pa.m_nY;
        int bx = pb.m_nX;
        int by = pb.m_nY;

        int iDX1 = bx - tx;
        int iDY1 = by - ty;

        if(iDX1 == 0)
        {
            iDX1 = 1;
        }

        if(iDY1 == 0)
        {
            iDY1 = 1;
        }

        int iDY = iLineY - ty;
        return ((float)iDX1 / iDY1) * iDY + tx + 0.5f;
    }

    // pa车道线起始点，pb车道线终点，给出线上的X求Y
    float CRoadInfo::iswGetLineY(SV_POINT pa, sv::SV_POINT pb, int iLineX)
    {
        int tx = pa.m_nX;
        int ty = pa.m_nY;
        int bx = pb.m_nX;
        int by = pb.m_nY;

        int iDX1 = bx - tx;
        int iDY1 = by - ty;

        if(iDX1 == 0)
        {
            iDX1 = 1;
        }

        if(iDY1 == 0)
        {
            iDY1 = 1;
        }

        int iDX = iLineX - tx;
        return ((float)iDX / iDX1) * iDY1 + ty + 0.5f;
    }

    //////////////////////////////////////////////////////////////////////////
    int CRoadInfo::GetRoadNum(SV_POINT pt)
    {
        pt.m_nX += m_rcROI.m_nLeft;
        pt.m_nY += m_rcROI.m_nTop;

        int iLineNum = -1;

        // 遍历所求点水平线上与车道线相交的点
        for(int i = 0; i <= m_iRoadCount; i++)
        {
            // 求所求点的Y在每条线上的X
            float fltX = iswGetLineX(m_rgRoadLine[i].ptTop, m_rgRoadLine[i].ptBottom, pt.m_nY);

            if((int)fltX >= pt.m_nX)
            {
                iLineNum = i - 1;
                break;
            }
        }

        return iLineNum;
    }

    int CRoadInfo::GetRoadWidth(SV_POINT pt, int* pRoadNum)
    {
        pt.m_nX += m_rcROI.m_nLeft;
        pt.m_nY += m_rcROI.m_nTop;

        int iLineNum = -1;
        float fltX = -1.0f;
        float preFltX = -1.0f;
        int iCount = 0;

        // 遍历所求点水平线上与车道线相交的点
        // 记下所求点X相临两个点的X值
        for(int i = 0; i <= m_iRoadCount; i++, iCount++)
        {
            // 求所求点的Y在每条线上的X
            fltX = iswGetLineX(m_rgRoadLine[i].ptTop, m_rgRoadLine[i].ptBottom, pt.m_nY);

            if((int)fltX >= pt.m_nX)
            {
                iLineNum = i - 1;
                break;
            }
            else
            {
                preFltX = fltX;
            }
        }

        // 用相临两个点X值相减即车道宽
        float fltRoadWidth = fabs(fltX - preFltX);

        if(pRoadNum != NULL)
        {
            *pRoadNum = iLineNum;
        }

        // 车道宽度用平均宽度计算
        float fltLeftX = iswGetLineX(m_rgRoadLine[0].ptTop, m_rgRoadLine[0].ptBottom, pt.m_nY);
        float fltRightX = iswGetLineX(m_rgRoadLine[m_iRoadCount].ptTop, m_rgRoadLine[m_iRoadCount].ptBottom, pt.m_nY);
        fltRoadWidth = fabs(fltRightX - fltLeftX) / m_iRoadCount;

        //         // 在所求车道外，也给出相临车道宽度
        //         if(iLineNum == -1)
        //         {
        //             if(iCount > m_iRoadCount)
        //             {
        //                 //右边道路外，应计算左边
        //                 preFltX = iswGetLineX(m_rgRoadLine[m_iRoadCount - 1].ptTop, m_rgRoadLine[m_iRoadCount - 1].ptBottom, pt.m_nY);
        //                 fltRoadWidth = fabs(fltX - preFltX);
        //             }
        //             else if(iCount == 0)
        //             {
        //                 //fltx为第一条线
        //                 float fltLineX = iswGetLineX(m_rgRoadLine[1].ptBottom, m_rgRoadLine[1].ptTop, pt.m_nY);
        //                 fltRoadWidth = fabs(fltX - fltLineX);
        //             }
        //         }

        if(fltRoadWidth < 1)
        {
            return 1;
        }

        return (int)(fltRoadWidth + 0.5f);
    }

    int CRoadInfo::GetRoadWidth(int iRoadNum, int iY)
    {
        //int iWidth = GetRoadLineX(iRoadNum + 1, iY) - GetRoadLineX(iRoadNum, iY);

        // 车道宽度用平均宽度计算
        int nLeftX = GetRoadLineX(0, iY);
        int nRightX = GetRoadLineX(m_iRoadCount, iY);
        int iWidth = (nRightX - nLeftX) / m_iRoadCount;

        if(iWidth < 0)
        {
            iWidth = 1;
        }

        return iWidth;
    }

    int CRoadInfo::GetRoadLineCount()
    {
        return m_iRoadCount + 1;
    }

    int CRoadInfo::GetRoadCount()
    {
        return m_iRoadCount;
    }

    int CRoadInfo::GetRoadType(int iRoadNum)
    {
        return ((iRoadNum >= 0) && (iRoadNum < GetRoadCount()))
               ?
               m_rgRoadLine[iRoadNum].iRoadType
               :
               -1;
    }

    // 给出车道线上线上的Y求X
    int CRoadInfo::GetRoadLineX(int iLineNum, int iLineY)
    {
        iLineY += m_rcROI.m_nTop;
        return (iLineNum > m_iRoadCount)
               ?
               -1
               :
               (int)iswGetLineX(m_rgRoadLine[iLineNum].ptTop,
                                m_rgRoadLine[iLineNum].ptBottom,
                                iLineY);
    }

    int CRoadInfo::GetRoadLineY(int iLineNum, int iLineX)
    {
        iLineX += m_rcROI.m_nLeft;
        return (iLineNum > m_iRoadCount)
               ?
               -1
               :
               (int)iswGetLineY(m_rgRoadLine[iLineNum].ptTop,
                                m_rgRoadLine[iLineNum].ptBottom,
                                iLineX);
    }

    int CRoadInfo::GetSceneWidth()
    {
        return m_iSceneWidth;
    }

    int CRoadInfo::GetSceneHeight()
    {
        return m_iSceneHeight;
    }

    // 设置关注区域，即可不用加偏移，但注意用后要设回来
    void CRoadInfo::SetROI(SV_RECT rcROI)
    {
        m_rcROI = rcROI;
    }

    sv::SV_RECT CRoadInfo::GetROI()
    {
        return m_rcROI;
    }

    void CRoadInfo::ResetROI()
    {
        m_rcROI.m_nLeft = 0;
        m_rcROI.m_nTop = 0;
        m_rcROI.m_nRight = m_iSceneWidth - 1;
        m_rcROI.m_nBottom = m_iSceneHeight - 1;
    }

    int CRoadInfo::GetROIWidth()
    {
        return m_rcROI.m_nRight - m_rcROI.m_nLeft + 1;
    }
    int CRoadInfo::GetROIHeight()
    {
        return m_rcROI.m_nBottom - m_rcROI.m_nTop + 1;
    }
    int CRoadInfo::GetRoadLinePointY0()
    {
        return m_nRoadLinePointY0;
    }


    // CSvLine类
    CSvLine::CSvLine()
    {
    }

    CSvLine::CSvLine(int x0, int y0, int x1, int y1)
    {
        Reset(x0, y0, x1, y1);
    }

    CSvLine::CSvLine(sv::SV_POINT pt0, sv::SV_POINT pt1)
    {
        Reset(pt0.m_nX, pt0.m_nY, pt1.m_nX, pt1.m_nY);
    }

    void CSvLine::Reset(int x0, int y0, int x1, int y1)
    {
        int dy = y0 - y1;
        int dx = x0 - x1;

        if(SV_ABS(dx) > SV_ABS(dy))
        {
            m_fKx = TRUE;
            // y = K * x + B 型
            m_fltK = (float)dy / dx;
            m_fltB = y0 - m_fltK * x0;
        }
        else if(dy != 0)
        {
            m_fKx = FALSE;
            // x = K * y + B 型
            m_fltK = (float)dx / dy;
            m_fltB = x0 - m_fltK * y0;
        }
        else
        {
            m_fKx = TRUE;
            m_fltK = 1.f;
            m_fltB = 1.f;
        }
    }

    void CSvLine::Reset(sv::SV_POINT pt0, sv::SV_POINT pt1)
    {
        Reset(pt0.m_nX, pt0.m_nY, pt1.m_nX, pt1.m_nY);
    }

    int CSvLine::GetLineX(int y)
    {
        return (m_fKx)
               ? (int)((y - m_fltB) / m_fltK + 0.5f)
               : (int)(m_fltK * y + m_fltB + 0.5f);
    }

    int CSvLine::GetLineY(int x)
    {
        return (m_fKx)
               ? (int)(m_fltK * x + m_fltB + 0.5f)
               : (int)((x - m_fltB) / m_fltK + 0.5f);
    }

    // CSvTrapArea 类
    CSvTrapArea::CSvTrapArea()
        :   m_fInit(FALSE)
    {
    }

    CSvTrapArea::~CSvTrapArea()
    {
    }

    sv::SV_RESULT CSvTrapArea::Init(
        int nFrameWidth,
        int nFrameHeight,
        int _nTopLeftX,
        int _nTopLeftY,
        int _nTopRightX,
        int _nTopRightY,
        int _nBottomLeftX,
        int _nBottomLeftY,
        int _nBottomRightX,
        int _nBottomRightY)
    {
        // 动态修改需求，不能用静态。
        nTopLeftX = (int)(1.0f * _nTopLeftX * nFrameWidth / 100 + 0.5f);
        nTopLeftY = (int)(1.0f * _nTopLeftY * nFrameHeight / 100 + 0.5f);
        nBottomLeftX = (int)(1.0f * _nBottomLeftX * nFrameWidth / 100 + 0.5f);
        nBottomLeftY = (int)(1.0f * _nBottomLeftY * nFrameHeight / 100 + 0.5f);
        nTopRightX = (int)(1.0f * _nTopRightX * nFrameWidth / 100 + 0.5f);
        nTopRightY = (int)(1.0f * _nTopRightY * nFrameHeight / 100 + 0.5f);
        nBottomRightX = (int)(1.0f * _nBottomRightX * nFrameWidth / 100 + 0.5f);
        nBottomRightY = (int)(1.0f * _nBottomRightY * nFrameHeight / 100 + 0.5f);

        //左边线
        float fltDX1 = (float)nTopLeftX - nBottomLeftX;
        float fltDY1 = (float)nTopLeftY - nBottomLeftY;

        //         if(fltDX1 == 0.0f)
        //         {
        //             fltDX1 = 1.0f;
        //         }

        if(fltDY1 == 0.0f)
        {
            return RS_S_FALSE;
            //fltDY1 = 1.0f;
        }

        m_fltSlopeLeft = fltDX1 / fltDY1;

        //右边线
        float fltRDX1 = (float)nTopRightX - nBottomRightX;
        float fltRDY1 = (float)nTopRightY - nBottomRightY;

        //         if(fltRDX1 == 0.0f)
        //         {
        //             fltRDX1 = 1.0f;
        //         }

        if(fltRDY1 == 0.0f)
        {
            return RS_S_FALSE;
            //fltRDY1 = 1.0f;
        }

        m_fltSlopeRight = fltRDX1 / fltRDY1;

        //上边线
        float fltTDX1 = (float)nTopRightX - nTopLeftX;
        float fltTDY1 = (float)nTopRightY - nTopLeftY;

        if(fltTDX1 == 0.0f)
        {
            return RS_S_FALSE;
            //fltTDX1 = 1.0f;
        }

        //         if(fltTDY1 == 0.0f)
        //         {
        //             fltTDY1 = 1.0f;
        //         }

        m_fltSlopeTop = fltTDY1 / fltTDX1;

        //下边线
        float fltBDX1 = (float)nBottomRightX - nBottomLeftX;
        float fltBDY1 = (float)nBottomRightY - nBottomLeftY;

        if(fltBDX1 == 0.0f)
        {
            return RS_S_FALSE;
            //fltBDX1 = 1.0f;
        }

        //         if(fltBDY1 == 0.0f)
        //         {
        //             fltBDY1 = 1.0f;
        //         }

        m_fltSlopeBottom = fltBDY1 / fltBDX1;

        m_fInit = TRUE;

        return RS_S_OK;
    }

    sv::SV_BOOL CSvTrapArea::IsContainOfPoint(sv::SV_POINT pt)
    {
        SV_BOOL bret = FALSE;
        int iLX = -1;
        int iRX = -1;
        int iTY = -1;
        int iBY = -1;

        float fltLDY = (float)pt.m_nY - nBottomLeftY;
        iLX = (int)(m_fltSlopeLeft * fltLDY + nBottomLeftX + 0.5f);

        float fltRDY = (float)pt.m_nY - nBottomRightY;
        iRX = (int)(m_fltSlopeRight * fltRDY + nBottomRightX + 0.5f);

        float fltTDX = (float)pt.m_nX - nTopLeftX;
        iTY = (int)(m_fltSlopeTop * fltTDX + nTopLeftY + 0.5f);

        float fltBDX = (float)pt.m_nX - nBottomLeftX;
        iBY = (int)(m_fltSlopeBottom * fltBDX + nBottomLeftY + 0.5f);

        //printf("%d %d %d %d\n", iLX, iRX, iTY, iBY);

        //判断包含关系
        if(pt.m_nX >= iLX && pt.m_nX <= iRX && pt.m_nY >= iTY && pt.m_nY <= iBY)
        {
            bret = TRUE;
        }

        return bret;
    }

    SV_BOOL CSvTrapArea::IsOutOfArea(SV_POINT pt, int* pOutTypeBit /*= NULL*/)
    {
        SV_BOOL bret = FALSE;
        int iLX = -1;
        int iRX = -1;
        int iTY = -1;
        int iBY = -1;

        float fltLDY = (float)pt.m_nY - nBottomLeftY;
        iLX = (int)(m_fltSlopeLeft * fltLDY + nBottomLeftX + 0.5f);

        float fltRDY = (float)pt.m_nY - nBottomRightY;
        iRX = (int)(m_fltSlopeRight * fltRDY + nBottomRightX + 0.5f);

        float fltTDX = (float)pt.m_nX - nTopLeftX;
        iTY = (int)(m_fltSlopeTop * fltTDX + nTopLeftY + 0.5f);

        float fltBDX = (float)pt.m_nX - nBottomLeftX;
        iBY = (int)(m_fltSlopeBottom * fltBDX + nBottomLeftY + 0.5f);

        //printf("%d %d %d %d\n", iLX, iRX, iTY, iBY);

        if(pOutTypeBit != NULL)
        {
            *pOutTypeBit = OT_NOT_OUT;
        }

        //判断包含关系
        if(pt.m_nX < iLX)
        {
            if(pOutTypeBit != NULL)
            {
                *pOutTypeBit |= OT_LEFT;
            }

            bret = TRUE;
        }
        else if(pt.m_nX > iRX)
        {
            if(pOutTypeBit != NULL)
            {
                *pOutTypeBit |= OT_RIGHT;
            }

            bret = TRUE;
        }

        if(pt.m_nY < iTY)
        {
            if(pOutTypeBit != NULL)
            {
                *pOutTypeBit |= OT_TOP;
            }

            bret = TRUE;
        }
        else if(pt.m_nY > iBY)
        {
            if(pOutTypeBit != NULL)
            {
                *pOutTypeBit |= OT_BOTTOM;
            }

            bret = TRUE;
        }

        return bret;
    }

    int CSvTrapArea::GetSceneWidth()
    {
        return m_nSceneWidth;
    }

    int CSvTrapArea::GetSceneHeight()
    {
        return m_nSceneHeight;
    }

    sv::SV_BOOL CSvTrapArea::IsVaild()
    {
        return m_fInit;
    }

    CSvLeftTurnArea::CSvLeftTurnArea()
        : m_fInit(FALSE)
    {

    }

    CSvLeftTurnArea::~CSvLeftTurnArea()
    {

    }

    SV_RESULT CSvLeftTurnArea::Init(sv::SV_POINT rgptLeftTurnWaitArea[2],
                                    sv::SV_POINT rgRoadStopLine[2],
                                    int nFrameWidth, int nFrameHeigh)
    {
        m_fInit = FALSE;

        if(rgptLeftTurnWaitArea[0].m_nY == 0 ||
                rgptLeftTurnWaitArea[0].m_nX == rgptLeftTurnWaitArea[1].m_nX)
        {
            return RS_S_OK;
        }

        m_fltA = (float)(rgptLeftTurnWaitArea[1].m_nY - rgptLeftTurnWaitArea[0].m_nY)
                 / (rgptLeftTurnWaitArea[1].m_nX - rgptLeftTurnWaitArea[0].m_nX);
        m_fltB = rgptLeftTurnWaitArea[1].m_nY - m_fltA * rgptLeftTurnWaitArea[1].m_nX;

        m_fInit = TRUE;
        return RS_S_OK;
    }

    sv::SV_BOOL CSvLeftTurnArea::IsOutOfArea(sv::SV_POINT ptCheck)
    {
        if(!m_fInit)
        {
            return FALSE;
        }

        float xLine = (ptCheck.m_nY - m_fltB) / m_fltA;
        return (ptCheck.m_nX < xLine) ? TRUE : FALSE;
    }

    sv::SV_BOOL CSvLeftTurnArea::IsVaild()
    {
        return m_fInit;
    }
} // svTgApi

