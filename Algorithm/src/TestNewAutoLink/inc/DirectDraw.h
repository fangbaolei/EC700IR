// DDraw.h: interface for the CDirectDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDRAW_H__32A5477E_B6C0_4D99_ACBE_887BF9247475__INCLUDED_)
#define AFX_DDRAW_H__32A5477E_B6C0_4D99_ACBE_887BF9247475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HVIMAGE
#ifdef HVIMAGE
class CHVImage;
#endif

#include <mmsystem.h>
#include <ddraw.h>

class CDirectDraw
{
public:
	enum MODE
	{
		NONE,
		YV12   = mmioFOURCC('Y','V','1','2'),/*YUV420*/
                YUV422,
		YUY2   = mmioFOURCC('Y','U','Y','2'),/*YUV422,YUYV*/
		YVYU,/*YVYU*/
		UYVY,/*UYVY*/
		RGB1   = 1,
                RGB4   = 4,
		RGB8   = 8,
                RGB15  = 15,
		RGB16  = 16,
		RGB24  = 24,
		RGB32  = 32,
	};
	         CDirectDraw();
	virtual ~CDirectDraw();
	//是否初始化
	BOOL     IsValid(void){return lpDDSPrimary != NULL;}
	//初始化函数
        CDirectDraw & Create(HWND hwnd,int width,int height,MODE mode= YV12);
#ifdef HVIMAGE
        CDirectDraw & Create(HWND hwnd, CHVImage * image);
#endif
	//释放资源
	CDirectDraw & Release(void);
	//设置数据
	CDirectDraw & SetImage(const BYTE * buffer);
	//绘制文字
	HDC           GetDC(void);
	CDirectDraw & ReleaseDC(HDC hdc);
        //获得图像的高度和宽度
        int           GetWidth(void) {return width;}
        int           GetHeight(void){return height;}
        MODE          GetMode(void){return mode;}
	//画图
	CDirectDraw & Draw(void);
protected:
        static void VFWDraw(CDirectDraw & ddraw, const void * buffer);
        static void RGB15TORGB16(CDirectDraw & ddraw, const void * buffer);
        static void RGB15TORGB24(CDirectDraw & ddraw, const void * buffer);
        static void RGB15TORGB32(CDirectDraw & ddraw, const void * buffer);

        static void RGB16TORGB15(CDirectDraw & ddraw, const void * buffer);
        static void RGB16TORGB24(CDirectDraw & ddraw, const void * buffer);
        static void RGB16TORGB32(CDirectDraw & ddraw, const void * buffer);

        static void RGB24TORGB15(CDirectDraw &ddraw, const void * buffer);
        static void RGB24TORGB16(CDirectDraw &ddraw, const void * buffer);
        static void RGB24TORGB32(CDirectDraw &ddraw, const void * buffer);

        static void RGB32TORGB15(CDirectDraw &ddraw, const void * buffer);
        static void RGB32TORGB16(CDirectDraw &ddraw, const void * buffer);
        static void RGB32TORGB24(CDirectDraw &ddraw, const void * buffer);

	static void CopyRGB(CDirectDraw &ddraw,  const void * buffer);
	static void CopyYV12(CDirectDraw &ddraw, const void * buffer);
        static void CopyYUV422(CDirectDraw &ddraw, const void * buffer);
	static void CopyYUY2(CDirectDraw &ddraw, const void * buffer);
	static void CopyYVYU(CDirectDraw &ddraw, const void * buffer);
	static void CopyUYVY(CDirectDraw &ddraw, const void * buffer);
	static LRESULT CALLBACK OnWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
private:
	HWND                    m_hWnd;
#ifndef _MSC_VER
   typedef int CALLBACK (*WNDPROC)();
#endif
	WNDPROC                 oldProc;
	DDSURFACEDESC2          ddsd;
        LPDIRECTDRAW7           lpDD;
	LPDIRECTDRAWSURFACE7    lpDDSPrimary;
	LPDIRECTDRAWSURFACE7    lpDDBuffer;
	LPDIRECTDRAWSURFACE7    lpDDSOffscreen;
	LPDIRECTDRAWCLIPPER     lpClipper;
	MODE                    mode;
	int                     width;
	int                     height;
        int                     rgbBitCount;
	typedef void(*CopyDDraw)(CDirectDraw &ddraw, const void * buffer);
	CopyDDraw               fnCopyDDraw;
};
#endif // !defined(AFX_DDRAW_H__32A5477E_B6C0_4D99_ACBE_887BF9247475__INCLUDED_)
