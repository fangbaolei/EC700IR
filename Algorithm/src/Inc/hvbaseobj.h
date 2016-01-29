#ifndef _HVBASEOBJ_INCLUDED__
#define _HVBASEOBJ_INCLUDED__

#include "swbasetype.h"

//TODO define by hxl,临时的定义和函数，有更好的算法再替换。

//Comment by Shaorg:
//将以下两个宏由之前的小写改为大写。
//原因：max宏与STL中的std::max这个函数会发生冲突，导致例如queue这个STL容器无法使用。

#ifndef MIN
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

namespace HiVideo
{

// forward declarations
class CSize;
class CPoint;
class CRect;

/////////////////////////////////////////////////////////////////////////////
// CSize - Wrapper for Windows HV_SIZE structure.

class CSize : public HV_SIZE
{
public:
// Constructors
	CSize();
	CSize(int initCX, int initCY);
	CSize(HV_SIZE initSize);
	CSize(HV_POINT initPt);
	CSize(DWORD32 dwSize);

// Operations
	BOOL operator==(HV_SIZE size) const;
	BOOL operator!=(HV_SIZE size) const;
	void operator+=(HV_SIZE size);
	void operator-=(HV_SIZE size);
	void SetSize(int CX, int CY);

// Operators returning CSize values
	CSize operator+(HV_SIZE size) const;
	CSize operator-(HV_SIZE size) const;
	CSize operator-() const;

// Operators returning CPoint values
	CPoint operator+(HV_POINT point) const;
	CPoint operator-(HV_POINT point) const;

// Operators returning CRect values
	CRect operator+(const HV_RECT* lpRect) const;
	CRect operator-(const HV_RECT* lpRect) const;
};


/////////////////////////////////////////////////////////////////////////////
// CPoint - Wrapper for Windows HV_POINT structure.

class CPoint : public HV_POINT
{
public:
// Constructors
	CPoint();
	CPoint(int initX, int initY);
	CPoint(HV_POINT initPt);
	CPoint(HV_SIZE initSize);
	CPoint(DWORD32 dwPoint);

// Operations
	void Offset(int xOffset, int yOffset);
	void Offset(HV_POINT point);
	void Offset(HV_SIZE size);
	BOOL operator==(HV_POINT point) const;
	BOOL operator!=(HV_POINT point) const;
	void operator+=(HV_SIZE size);
	void operator-=(HV_SIZE size);
	void operator+=(HV_POINT point);
	void operator-=(HV_POINT point);
	void SetPoint(int X, int Y);

// Operators returning CPoint values
	CPoint operator+(HV_SIZE size) const;
	CPoint operator-(HV_SIZE size) const;
	CPoint operator-() const;
	CPoint operator+(HV_POINT point) const;

// Operators returning CSize values
	CSize operator-(HV_POINT point) const;

// Operators returning CRect values
	CRect operator+(const HV_RECT* lpRect) const;
	CRect operator-(const HV_RECT* lpRect) const;
};


/////////////////////////////////////////////////////////////////////////////
// CRect - Wrapper for Windows HV_RECT structure.

class CRect : public HV_RECT
{
public:
// Constructors
	CRect();
	CRect(int l, int t, int r, int b);
	CRect(const HV_RECT& srcRect);
	CRect(LPCHV_RECT lpSrcRect);
	CRect(HV_POINT point, HV_SIZE size);
	CRect(HV_POINT topLeft, HV_POINT bottomRight);

// Attributes (in addition to HV_RECT members)
	int Width() const;
	int Height() const;
	int Area() const;
	CSize Size() const;
	CPoint& TopLeft();
	CPoint& BottomRight();
	const CPoint& TopLeft() const;
	const CPoint& BottomRight() const;
	CPoint CenterPoint() const;

	// convert between CRect and LPRECT/LPCHV_RECT (no need for &)
	operator LPHV_RECT();
	operator LPCHV_RECT() const;

	BOOL IsRectEmpty() const;
	BOOL IsRectNull() const;
	BOOL PtInRect(HV_POINT point) const;
	BOOL Contains(LPCHV_RECT) const;
	BOOL Contains(LPCHV_POINT) const;
	BOOL Contains(int x, int y) const;

// Operations
	void SetRect(int x1, int y1, int x2, int y2);
	void SetRect(HV_POINT topLeft, HV_POINT bottomRight);
	void SetRectEmpty();
	void CopyRect(LPCHV_RECT lpSrcRect);
	BOOL EqualRect(LPCHV_RECT lpRect) const;

	void InflateRect(int x, int y);
	void InflateRect(HV_SIZE size);
	void InflateRect(LPCHV_RECT lpRect);
	void InflateRect(int l, int t, int r, int b);
	void DeflateRect(int x, int y);
	void DeflateRect(HV_SIZE size);
	void DeflateRect(LPCHV_RECT lpRect);
	void DeflateRect(int l, int t, int r, int b);

	void OffsetRect(int x, int y);
	void OffsetRect(HV_SIZE size);
	void OffsetRect(HV_POINT point);
	void NormalizeRect();

	// absolute position of rectangle
	void MoveToY(int y);
	void MoveToX(int x);
	void MoveToXY(int x, int y);
	void MoveToXY(HV_POINT point);

	// operations that fill '*this' with result
	BOOL IntersectRect(LPCHV_RECT lpRect1, LPCHV_RECT lpRect2);
	BOOL IntersectsWith(const CRect& rect) const;
	BOOL UnionRect(LPCHV_RECT lpRect1, LPCHV_RECT lpRect2);
	BOOL SubtractRect(LPCHV_RECT lpRectSrc1, LPCHV_RECT lpRectSrc2);
	BOOL IsOverlap(const CRect& rect) const;

// Additional Operations
	void operator=(const HV_RECT& srcRect);
	BOOL operator==(const HV_RECT& rect) const;
	BOOL operator!=(const HV_RECT& rect) const;
	void operator+=(HV_POINT point);
	void operator+=(HV_SIZE size);
	void operator+=(LPCHV_RECT lpRect);
	void operator-=(HV_POINT point);
	void operator-=(HV_SIZE size);
	void operator-=(LPCHV_RECT lpRect);
	void operator&=(const HV_RECT& rect);
	void operator|=(const HV_RECT& rect);

// Operators returning CRect values
	CRect operator+(HV_POINT point) const;
	CRect operator-(HV_POINT point) const;
	CRect operator+(LPCHV_RECT lpRect) const;
	CRect operator+(HV_SIZE size) const;
	CRect operator-(HV_SIZE size) const;
	CRect operator-(LPCHV_RECT lpRect) const;
	CRect operator&(const HV_RECT& rect2) const;
	CRect operator|(const HV_RECT& rect2) const;
	CRect MulDiv(int nMultiplier, int nDivisor) const;

//增加的辅助方法

	int MulDiv(int nNumber, int nNumerator, int nDenominator) const;
};


/////////////////////////////////////////////////////////////////////////////
// CSize, CPoint, CRect Implementation

// CSize
inline CSize::CSize()
	{ /* random filled */ }
inline CSize::CSize(int initCX, int initCY)
	{ cx = initCX; cy = initCY; }
inline CSize::CSize(HV_SIZE initSize)
	{ *(HV_SIZE*)this = initSize; }
inline CSize::CSize(HV_POINT initPt)
	{ *(HV_POINT*)this = initPt; }
//inline CSize::CSize(DWORD32 dwSize)
//	{
//		cx = (short)LOWORD(dwSize);
//		cy = (short)HIWORD(dwSize);
//	}
inline BOOL CSize::operator==(HV_SIZE size) const
	{ return (cx == size.cx && cy == size.cy); }
inline BOOL CSize::operator!=(HV_SIZE size) const
	{ return (cx != size.cx || cy != size.cy); }
inline void CSize::operator+=(HV_SIZE size)
	{ cx += size.cx; cy += size.cy; }
inline void CSize::operator-=(HV_SIZE size)
	{ cx -= size.cx; cy -= size.cy; }
inline void CSize::SetSize(int CX, int CY)
	{ cx = CX; cy = CY; }
inline CSize CSize::operator+(HV_SIZE size) const
	{ return CSize(cx + size.cx, cy + size.cy); }
inline CSize CSize::operator-(HV_SIZE size) const
	{ return CSize(cx - size.cx, cy - size.cy); }
inline CSize CSize::operator-() const
	{ return CSize(-cx, -cy); }
inline CPoint CSize::operator+(HV_POINT point) const
	{ return CPoint(cx + point.x, cy + point.y); }
inline CPoint CSize::operator-(HV_POINT point) const
	{ return CPoint(cx - point.x, cy - point.y); }
inline CRect CSize::operator+(const HV_RECT* lpRect) const
	{ return CRect(lpRect) + *this; }
inline CRect CSize::operator-(const HV_RECT* lpRect) const
	{ return CRect(lpRect) - *this; }

// CPoint
inline CPoint::CPoint()
	{ /* random filled */ }
inline CPoint::CPoint(int initX, int initY)
	{ x = initX; y = initY; }
inline CPoint::CPoint(HV_POINT initPt)
	{ *(HV_POINT*)this = initPt; }
inline CPoint::CPoint(HV_SIZE initSize)
	{ *(HV_SIZE*)this = initSize; }
//inline CPoint::CPoint(DWORD32 dwPoint)
//	{
//		x = (short)LOWORD(dwPoint);
//		y = (short)HIWORD(dwPoint);
//	}
inline void CPoint::Offset(int xOffset, int yOffset)
	{ x += xOffset; y += yOffset; }
inline void CPoint::Offset(HV_POINT point)
	{ x += point.x; y += point.y; }
inline void CPoint::Offset(HV_SIZE size)
	{ x += size.cx; y += size.cy; }
inline BOOL CPoint::operator==(HV_POINT point) const
	{ return (x == point.x && y == point.y); }
inline BOOL CPoint::operator!=(HV_POINT point) const
	{ return (x != point.x || y != point.y); }
inline void CPoint::operator+=(HV_SIZE size)
	{ x += size.cx; y += size.cy; }
inline void CPoint::operator-=(HV_SIZE size)
	{ x -= size.cx; y -= size.cy; }
inline void CPoint::operator+=(HV_POINT point)
	{ x += point.x; y += point.y; }
inline void CPoint::operator-=(HV_POINT point)
	{ x -= point.x; y -= point.y; }
inline void CPoint::SetPoint(int X, int Y)
	{ x = X; y = Y; }
inline CPoint CPoint::operator+(HV_SIZE size) const
	{ return CPoint(x + size.cx, y + size.cy); }
inline CPoint CPoint::operator-(HV_SIZE size) const
	{ return CPoint(x - size.cx, y - size.cy); }
inline CPoint CPoint::operator-() const
	{ return CPoint(-x, -y); }
inline CPoint CPoint::operator+(HV_POINT point) const
	{ return CPoint(x + point.x, y + point.y); }
inline CSize CPoint::operator-(HV_POINT point) const
	{ return CSize(x - point.x, y - point.y); }
inline CRect CPoint::operator+(const HV_RECT* lpRect) const
	{ return CRect(lpRect) + *this; }
inline CRect CPoint::operator-(const HV_RECT* lpRect) const
	{ return CRect(lpRect) - *this; }

// CRect
inline CRect::CRect()
	{ /* random filled */ }
inline CRect::CRect(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bottom = b; }
inline CRect::CRect(const HV_RECT& srcRect)
	{ CopyRect(&srcRect); }
inline CRect::CRect(LPCHV_RECT lpSrcRect)
	{ CopyRect(lpSrcRect); }
inline CRect::CRect(HV_POINT point, HV_SIZE size)
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
inline CRect::CRect(HV_POINT topLeft, HV_POINT bottomRight)
	{ left = topLeft.x; top = topLeft.y;
		right = bottomRight.x; bottom = bottomRight.y; }
inline int CRect::Width() const
	{ return right - left; }
inline int CRect::Height() const
	{ return bottom - top; }
inline int CRect::Area() const
	{ return Width() * Height(); }
inline CSize CRect::Size() const
	{ return CSize(right - left, bottom - top); }
inline CPoint& CRect::TopLeft()
	{ return *((CPoint*)this); }
inline CPoint& CRect::BottomRight()
	{ return *((CPoint*)this + 1); }
inline const CPoint& CRect::TopLeft() const
	{ return *((CPoint*)this); }
inline const CPoint& CRect::BottomRight() const
	{ return *((CPoint*)this + 1); }
inline CPoint CRect::CenterPoint() const
	{ return CPoint((left + right) / 2, (top + bottom) / 2); }
inline CRect::operator LPHV_RECT()
	{ return this; }
inline CRect::operator LPCHV_RECT() const
	{ return this; }

inline BOOL CRect::IsRectEmpty() const
{ return (((right <= left) || (bottom <= top)) ? true : false); }

inline BOOL CRect::IsRectNull() const
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }



inline BOOL CRect::Contains(int x,int y) const
    {
        return x >= left && x < right && y >= top && y < bottom;
    }

inline BOOL CRect::Contains(LPCHV_POINT pt) const
    {
        return Contains(pt->x, pt->y);
    }

inline BOOL CRect::Contains(LPCHV_RECT rect) const
    {
        return (rect->left >= left) && (rect->right <= right) &&
               (rect->top >=top) && (rect->bottom <= bottom);
    }

inline BOOL CRect::PtInRect(HV_POINT point) const
	{ return ((point.x >= left && point.x < right && point.y >= top && point.y <bottom) ? true : false); }
inline void CRect::SetRect(int x1, int y1, int x2, int y2)
	{ this->left = x1;this->right = x2;this->top = y1;this->bottom = y2;}
inline void CRect::SetRect(HV_POINT topLeft, HV_POINT bottomRight)
	{ this->left = topLeft.x;this->top = topLeft.y; this->right = bottomRight.x; this->bottom = bottomRight.y; }
inline void CRect::SetRectEmpty()
	{ this->left = this->right = this->top = this->bottom = 0;}
inline void CRect::CopyRect(LPCHV_RECT lpSrcRect)
	{this->left = lpSrcRect->left;this->right = lpSrcRect->right;this->top = lpSrcRect->top;this->bottom = lpSrcRect->bottom;}
inline BOOL CRect::EqualRect(LPCHV_RECT lpRect) const
	{ return (this->left == lpRect->left && this->right == lpRect->right && this->top == lpRect->top && this->bottom ==lpRect->bottom) ? true : false; }
inline void CRect::InflateRect(int x, int y)
	{ this->left -= x; this->right += x; this->top -= y; this->bottom += y;}
inline void CRect::InflateRect(HV_SIZE size)
	{ this->left -= size.cx; this->right += size.cx; this->top -= size.cy; this->bottom += size.cy; }
inline void CRect::DeflateRect(int x, int y)
	{ this->left += x; this->right -= x; this->top += y; this->bottom -= y; }
inline void CRect::DeflateRect(HV_SIZE size)
	{ this->left += size.cx; this->right -= size.cx; this->top += size.cy; this->bottom -= size.cy; }
inline void CRect::OffsetRect(int x, int y)
	{ this->left += x; this->right += x; this->top += y; this->bottom += y; }
inline void CRect::OffsetRect(HV_POINT point)
	{ this->left += point.x; this->right += point.x; this->top += point.y; this->bottom += point.y; }
inline void CRect::OffsetRect(HV_SIZE size)
	{ this->left += size.cx; this->right += size.cx; this->top += size.cy; this->bottom += size.cy; }
inline BOOL CRect::IntersectRect(LPCHV_RECT lpRect1, LPCHV_RECT lpRect2)		//交集
	{
		this->left = MAX(lpRect1->left, lpRect2->left);
		this->right = MIN(lpRect1->right, lpRect2->right);
		this->top = MAX(lpRect1->top, lpRect2->top);
		this->bottom = MIN(lpRect1->bottom, lpRect2->bottom);
		return !IsRectEmpty();
	}

inline BOOL CRect::IntersectsWith(const CRect& rect) const
    {
        return (left < rect.right &&
                top < rect.bottom &&
                right > rect.left &&
                bottom > rect.top);
    }

inline BOOL CRect::UnionRect(LPCHV_RECT lpRect1, LPCHV_RECT lpRect2)
	{
		this->left = MIN(lpRect1->left, lpRect2->left);
		this->right = MAX(lpRect1->right, lpRect2->right);
		this->top = MIN(lpRect1->top, lpRect2->top);
		this->bottom = MAX(lpRect1->bottom, lpRect2->bottom);
		return !IsRectEmpty();
	}

inline void CRect::operator=(const HV_RECT& srcRect)
	{ CopyRect(&srcRect); }
inline BOOL CRect::operator==(const HV_RECT& rect) const
	{ return EqualRect(&rect); }
inline BOOL CRect::operator!=(const HV_RECT& rect) const
	{ return !EqualRect( &rect); }
inline void CRect::operator+=(HV_POINT point)
	{ OffsetRect( point.x, point.y); }
inline void CRect::operator+=(HV_SIZE size)
	{ OffsetRect(size.cx, size.cy); }
inline void CRect::operator+=(LPCHV_RECT lpRect)
	{ InflateRect(lpRect); }
inline void CRect::operator-=(HV_POINT point)
	{ OffsetRect( -point.x, -point.y); }
inline void CRect::operator-=(HV_SIZE size)
	{ OffsetRect( -size.cx, -size.cy); }
inline void CRect::operator-=(LPCHV_RECT lpRect)
	{ DeflateRect(lpRect); }
inline void CRect::operator&=(const HV_RECT& rect)
	{ IntersectRect(this, &rect); }
inline void CRect::operator|=(const HV_RECT& rect)
	{ UnionRect(this, &rect); }
inline CRect CRect::operator+(HV_POINT pt) const
	{ CRect rect(*this); rect.OffsetRect(pt.x, pt.y); return rect; }
inline CRect CRect::operator-(HV_POINT pt) const
	{ CRect rect(*this); rect.OffsetRect(-pt.x, -pt.y); return rect; }
inline CRect CRect::operator+(HV_SIZE size) const
	{ CRect rect(*this); rect.OffsetRect(size.cx, size.cy); return rect; }
inline CRect CRect::operator-(HV_SIZE size) const
	{ CRect rect(*this); rect.OffsetRect(-size.cx, -size.cy); return rect; }
inline CRect CRect::operator+(LPCHV_RECT lpRect) const
	{ CRect rect(this); rect.InflateRect(lpRect); return rect; }
inline CRect CRect::operator-(LPCHV_RECT lpRect) const
	{ CRect rect(this); rect.DeflateRect(lpRect); return rect; }
inline CRect CRect::operator&(const HV_RECT& rect2) const
	{ CRect rect; rect.IntersectRect(this, &rect2);
		return rect; }
inline CRect CRect::operator|(const HV_RECT& rect2) const
	{ CRect rect; rect.UnionRect(this, &rect2);
		return rect; }
inline BOOL CRect::SubtractRect(LPCHV_RECT lpRectSrc1, LPCHV_RECT lpRectSrc2)
	{ return SubtractRect(lpRectSrc1, lpRectSrc2); }

inline void CRect::NormalizeRect()
{
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}

inline void CRect::MoveToY(int y)
	{ bottom = Height() + y; top = y; }
inline void CRect::MoveToX(int x)
	{ right = Width() + x; left = x; }
inline void CRect::MoveToXY(int x, int y)
	{ MoveToX(x); MoveToY(y); }
inline void CRect::MoveToXY(HV_POINT pt)
	{ MoveToX(pt.x); MoveToY(pt.y); }

inline void CRect::InflateRect(LPCHV_RECT lpRect)
{
	left -= lpRect->left;
	top -= lpRect->top;
	right += lpRect->right;
	bottom += lpRect->bottom;
}

inline void CRect::InflateRect(int l, int t, int r, int b)
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

inline void CRect::DeflateRect(LPCHV_RECT lpRect)
{
	left += lpRect->left;
	top += lpRect->top;
	right -= lpRect->right;
	bottom -= lpRect->bottom;
}

inline void CRect::DeflateRect(int l, int t, int r, int b)
{
	left += l;
	top += t;
	right -= r;
	bottom -= b;
}

inline CRect CRect::MulDiv(int nMultiplier, int nDivisor) const
{
	return CRect(
		MulDiv(left, nMultiplier, nDivisor),
		MulDiv(top, nMultiplier, nDivisor),
		MulDiv(right, nMultiplier, nDivisor),
		MulDiv(bottom, nMultiplier, nDivisor));
}

//TODO 应使用swBaseType中的64位类型而不应直接使用long,DSP上累加器是40位的，但运算时可以使用2个累加器
inline int CRect::MulDiv(int nNumber, int nNumerator, int nDenominator) const {
	long mulVar = (long)nNumber * (long)nNumerator;
	long resVar = mulVar /(long)nDenominator;
	//四舍五入
	int modVar = mulVar % nDenominator;
	if (nDenominator < 2 * modVar) {
		resVar++;
	}
	return (int)resVar;
}

inline BOOL CRect::IsOverlap(const CRect& rect) const
{
	int iInnerWidth = MIN(right, rect.right)
			- MAX(left, rect.left);
	int iInnerHeight = MIN(bottom, rect.bottom)
			- MAX(top, rect.top);

	if (iInnerWidth > 0 && iInnerHeight > 0)
	{
		return ( iInnerWidth * iInnerHeight * 2 >
			MAX(Width() * Height(),
			rect.Width() * rect.Height()) );
	}
	else
	{
		return false;
	}
}

}

#endif // _HVBASEOBJ_INCLUDED__
