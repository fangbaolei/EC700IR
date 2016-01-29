#ifndef __HV_IMAGE_H__
#define __HV_IMAGE_H__
/*
功能：
1、CHVImage图像处理类，目前主要处理YUV422和RGB24,其他图像格式尚未验证
为了处理上的统一，将所有的子类实现在cpp文件上，目的是为了不让用户直接访问子类
统一通过CHVImage::Create来创建一种图像类型，在图像转换上，通过Draw函数进行转换
2、CRegion区域计算类,图像处理中经常会用到通过二值图计算连通性，为了方便计算图像的联通性，特编写此类
3、CHVText在图像上写文字，有时为了在图像上附加文字信息，通过该类可以在图像上写文字信息
编著：黄国超
日期：2010-08-10

不能直接用CHVImage 来定义一张图片，必须用CHVImage::Create函数来创建某一类型的图片，然后通过Draw函数进行图像格式转换
例子: 打开jpg文件解压成YUV422格式，然后将其转换为YV12格式

//创建YUV422图片
CHVImage * yuv422 = CHVImage::Create(0,0,CHVImage::YUV422);
//加载图片
if(yuv422->Load("test.jpg"))
{
//创建YV12图片
CHVImage * yv12 = CHVImage::Create(yuv422->GetWidth(),yuv422->GetHeight(),CHVImage::YV12);
//通过yuv422表面绘制到yv12表面实现格式转换
yuv422->Draw(yv12);
//在yv12上显示文字
CHVText text = "这是yv12图片";
text.SetFont("新宋体",100).SetColor(YUYV(82,90,82,240))).Draw(10,10,yv12);
//显示yv12图像
yv12->Draw(GetActiveWindow());
}

//也可以做成图像合并，如下代码
CHVImage * img1 = CHVImage::Create(100,100,CHVImage::YUV422);
CHVImage * img2 = CHVImage::Create(100,100,CHVImage::YUV422);
CHVImage * img3 = CHVImage::Create(100,200,CHVImage::YUV422);
//将图像img1和img2合并到img3中
img1->Draw(img3,0,0,img1->GetWidth(),img1->GetHeight(),0,0);
img2->Draw(img3,0,0,img2->GetWidth(),img2->GetHeight(),img1->GetWidth(),0);

修改日志：

2010-08-02  创建CHVImage类定义各个接口函数
2010-08-05  添加CHVRegion类，统一管理CHVImage的连通区域
2010-08-06  创建CHVText类，统一管理在图像上写文字信息
2010-08-10  修改CHVImage类，在内存上实现YUV422到jpg的压缩解压
修改CHVImage类，添加Bayer8ToYUV422的实现，不再需要先转换到RGB然后再由RGB到YUV422，
该函数转换一张2448*2050的转换耗时80毫秒，基本上达到12fps的要求。
2010-08-11  为了跨平台，实现win32特有的函数和结构体，修改Bayer8TOYUV422函数，使用查表法提高转换效率转换一张2448*2050图耗时60毫秒
2010-08-11  CHVRegion增加Clear函数，清除连通区域,修改[]函数，如果取值超过连通区域的数量，则扩大内存，返回一个空的矩形
2010-08-20  为了支持更多的YUV格式，增加了YUY2,YVYU,UYVY,YUV444子类，和增加了ijl51.lib包用来在windows下能够快速解压JPEG图片
2010-08-20  为了子类继承的方便，仅保留Resize函数为纯虚函数，其他的均为虚函数，减少子类的代码量。未继承的函数表示没有该功能
2010-08-21  增加了各种YUV格式的转换;增加显示图像到窗口上的函数Draw(HWND)，需要调用CDirectDraw进行绘制
2010-08-23  修改YUV宏定义，原来是YUYV的宏修改为YUV宏，因为一个像素点对应一个YUV值，对于某个YUV类型的宏点，子类函数自己去转换
2010-08-24  根据HSL取得常用颜色的取值范围，方便以后判断
2010-08-25  增加过滤掉某些颜色或者仅保留某些颜色的函数Filter,增加图像调整函数Adjust,增加图像放大缩小函数Resample
2010-08-26  增加YV12和Y411的图像转换函数,修改YUV422加载JPEG图像方法，通过先加载到YUV444然后再转换
2010-08-27  引入智能指针auto_ptr,可以将一个指针类型变量当成一个普通的变量，超过作用域会自动释放内存
2010-09-01  为了能够处理非宽的4倍的标准图像，需要引入扫描行的概念，每次处理一条扫描行,目前YUV411处理尚存在问题
2010-11-11  增加支持IReferenceComponentImage接口,增加解压图像的同时进行旋转的功能
2010-11-12  增加类与类之间继承的关系，通过dcast转换指针，确保类指针转换正确
2010-11-23  增加处理二值图的功能，为了更方便的处理识别器的二值图
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//宏定义
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define dcast(cls, ptr)    (ptr && (ptr)->InternalIsDescendant(cls::Class()) ? (cls *)ptr : NULL)
#define CLASSINFO(cls, par) \
  public: \
  static const char * Class(){ return #cls; } \
  virtual bool InternalIsDescendant(const char * clsName) const {return strcmp(clsName, cls::Class()) == 0 || (strcmp(cls::Class(),par::Class()) && par::InternalIsDescendant(clsName));}
#ifdef min
#undef min
#endif
inline int min(int a, int b){ return ((a)<(b)?(a):(b));}
#ifdef max
#undef max
#endif
inline int max(int a, int b){return ((a)>(b)?(a):(b));}
#ifdef mid
#undef mid
#endif
inline int mid(int a, int min, int max){ if(a < min) a = min; if(a > max) a = max; return a;}
//RGB颜色空间
#ifndef RGB
#define RGB(r,g,b)        ((DWORD)mid(r,0,255))|(((DWORD)mid(g,0,255))<<8)|(((DWORD)mid(b,0,255))<<16) 
#endif
#ifndef R
#define R(rgb)            ((BYTE) (rgb))
#endif
#ifndef G
#define G(rgb)            ((BYTE) (((WORD) (rgb)) >> 8))
#endif
#ifndef B
#define B(rgb)            ((BYTE) (((DWORD)(rgb)) >> 16))
#endif
//YUV颜色空间
#ifndef YUV
#define YUV(y,u,v)        RGB(y,u,v)
#endif
#ifndef Y
#define Y(yuv)            R(yuv)
#endif
#ifndef U
#define U(yuv)            G(yuv)
#endif
#ifndef V
#define V(yuv)            B(yuv)
#endif
//HSL颜色空间
#ifndef HSL
#define HSL(h,s,l)        RGB(mid(h,0,240),mid(s,0,240),mid(l,0,240))
#endif
#ifndef H
#define H(hsl)            R(hsl)
#endif
#ifndef S
#define S(hsl)            G(hsl)
#endif
#ifndef L
#define L(hsl)            B(hsl)
#endif
//转换函数
#define RGB2YUV(rgb)      CHVImage::RGBtoYUV(rgb)
#define YUV2RGB(yuv)      CHVImage::YUVtoRGB(yuv)
#define HSL2RGB(hsl)      CHVImage::HSLtoRGB(hsl)
#define RGB2HSL(rgb)      CHVImage::RGBtoHSL(rgb)
#define YUV2HSL(yuv)      RGB2HSL(YUV2RGB(yuv))
#define HSL2YUV(hsl)      RGB2YUV(HSL2RGB(hsl))
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//公共头文件
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include "swImageObj.h"
#include "gba-jpeg-decode.h"
#endif
#define                                 RGB_YUV_TABLE  //通过查表法转换RGB和YUV
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Windows环境
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <memory>
using namespace std;
#define XJPEG           //jpeg开源库
#define INTEL           //intel的jpeg库
#define DDRAW           //使用ddraw绘制图片
#ifdef  DDRAW
class CDirectDraw;
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//非Windows环境
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
#include <std.h>
#include <string.h>
#include <mem.h>

#define BI_RGB   0

//智能指针
template<class T>class auto_ptr
{
public:
	auto_ptr(T * ptr = NULL){m_Ptr = ptr;}
	virtual ~auto_ptr(){if(m_Ptr)delete m_Ptr;}
	T & operator *() {return *m_Ptr;}
	T * operator ->(){return m_Ptr;}
	operator T*()    {return m_Ptr;}
private:
	T * m_Ptr;
};
//类型定义
typedef char           CHAR;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef long           LONG;
//用到的结构体
typedef struct tagRGBQUAD
{ 
	BYTE    rgbBlue; 
	BYTE    rgbGreen; 
	BYTE    rgbRed; 
	BYTE    rgbReserved; 
} RGBQUAD; 

typedef struct tagBITMAPFILEHEADER
{
	WORD    bfType; 
	DWORD   bfSize; 
	WORD    bfReserved1; 
	WORD    bfReserved2; 
	DWORD   bfOffBits; 
} BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER
{
	DWORD  biSize; 
	LONG   biWidth; 
	LONG   biHeight; 
	WORD   biPlanes; 
	WORD   biBitCount; 
	DWORD  biCompression; 
	DWORD  biSizeImage; 
	LONG   biXPelsPerMeter; 
	LONG   biYPelsPerMeter; 
	DWORD  biClrUsed; 
	DWORD  biClrImportant; 
} BITMAPINFOHEADER;

typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;
}RECT;

typedef struct tagPOINT
{
	int x;
	int y; 
} POINT;
typedef struct tagLOGFONT
{
	LONG lfHeight; 
	LONG lfWidth; 
	LONG lfEscapement; 
	LONG lfOrientation; 
	LONG lfWeight; 
	BYTE lfItalic; 
	BYTE lfUnderline; 
	BYTE lfStrikeOut; 
	BYTE lfCharSet; 
	BYTE lfOutPrecision; 
	BYTE lfClipPrecision; 
	BYTE lfQuality; 
	BYTE lfPitchAndFamily; 
	CHAR lfFaceName[32];
} LOGFONT;

//矩形函数
bool CopyRect(RECT * lprcDst, const RECT *lprcSrc);
bool EqualRect(const RECT *lprc1, const RECT *lprc2);
bool InflateRect(RECT * lprc, int dx, int dy);
bool IntersectRect(RECT * lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2);
bool IsRectEmpty(const RECT *lprc);
bool OffsetRect(RECT * lprc, int dx, int dy);
bool PtInRect(const RECT *lprc, POINT pt);
bool SetRect(RECT * lprc, int xLeft, int yTop, int xRight, int yBottom);
bool SetRectEmpty(RECT * lprc);
bool SubtractRect(RECT * lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2);
bool UnionRect(RECT * lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2);
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//类定义
////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHVImage;
/*
图像局域类，根据指定的颜色寻找连通局域
*/
class CHVRegion
{
public:
	CHVRegion(CHVImage * image);
	virtual  ~CHVRegion(void);
	//根据颜色color创建图像的连通区域
	CHVRegion & Create(DWORD color, RECT * rc = NULL);
	//清除矩形区域
	CHVRegion & Clear(void);
	//合并相邻或者交叉的连通区域,interval为2个连通区域相邻的像素，小于该像素则合并
	CHVRegion & Union(int interval=3);
	//对所有的联通区域进行排序，目前有水平的从左到右和垂直从上到下2种排序
	CHVRegion & Sort(bool m_bHorizon = true);
	//对连通区域用color绘制矩形边框
	CHVRegion & Draw(DWORD color);
	//删掉某个连通区域
	int       Delete(int index);
	//获得连通区域的矩形数组
	operator RECT *(){return rgnRect;}
	//获得第index个连通区域的矩形
	RECT    & operator[](int index);
	//获得连通区域的个数
	int       GetCount(void){return rgnCount;}
protected:
	//判断连通区域是否相邻
	bool     IsNeighbor(RECT rc1,RECT rc2,int interval);
private:
	CHVImage * image;
	int * buffer;
	int **rgn;
	RECT * rgnRect;
	int   rgnCount;
	int   rgnSize;
};
/*
图像处理类，该类是一个接口类，不能直接定义，需要用CHVImage::Create来创建对应的图像格式，目前支持有Bayer8、RGB1、RGB4、RGB24和各种YUV格式，HSL格式
*/
class CHVImage
#ifdef _SWIMAGEOBJ_INCLUDED__
	:public IReferenceComponentImage
#endif
{
public:
	CHVImage();
	virtual ~CHVImage();
#ifdef _SWIMAGEOBJ_INCLUDED__
	CLASSINFO(CHVImage,IReferenceComponentImage)
#else
	CLASSINFO(CHVImage,CHVImage)
#endif
		//图像类型,图像的格式转换可以用draw函数从一种格式转换成另外一种格式，也可以直接条用静态函数进行转换
		enum HVImageType
	{
		NONE,     //无类型，初始值
		BIN,      //车牌二值图,也属于RGB1格式的数据
		RGB1,
		RGB4,
		RGB8,
		RGB15,
		RGB16,
		RGB24,
		RGB32,    //RGB图像格式
		YUV444,   //原始的YUV图像格式
		YUV422,   //YUV422格式，目前公司用的是该格式，y u v 以平面格式存放
		YUY2,     //标准的yuv422格式，yuv以打包的格式存放
		YVYU,     //同上，只是yuv排序不一样而已，比较少用
		UYVY,     //同上，只是yuv排序不一样而已，比较少用
		YV12,     //标准的yuv420格式，yuv以平面格式存放
		Y411,     //yuv411格式，目前公司也用该格式，y u v 以平面格式存放
		HSL       //hsl格式
	};
	//创建某一类型的图像
	static CHVImage * Create(int inWidth,int inHeight,HVImageType inType=NONE);
	//设定用户自定义参数
	CHVImage & SetData(int data){m_iUserData = data; return *this;}
	//取得用户自定义参数
	int      GetData(void)   {return m_iUserData;}
	//图像类型
	HVImageType GetType(void){return m_Type;}
	//图像宽度
	int      GetWidth(void)  {return m_iWidth;}
	//图像高度
	int      GetHeight(void) {return m_iHeight;}
	//一条扫描线的字节数
	int      GetStride(void) {return m_iStride;}
	//图像数据
	BYTE   * GetBits(void)   {return m_bImage;}
	//图像大小
	int      GetSize(void)   {return m_iStride * m_iHeight;}
	//调整图像大小，调整之后原来的图像数据丢失，而不是伸缩，图像伸缩请调用Resample
	virtual  CHVImage & Resize(int width, int height){return *this;}
	//取得图像某一点的像素值
	virtual  DWORD GetPixel(int x,int y);
	virtual  CHVImage & GetPixel(int x, int y, RGBQUAD * color);
	//设置图像某一点的像素值
	virtual  CHVImage & PutPixel(int x,int y,DWORD color);
	virtual  CHVImage & PutPixel(int x, int y, RGBQUAD color);
	//绘制直线
	virtual  CHVImage & Line(int x0,int y0, int x1,int y1,DWORD color);
	virtual  CHVImage & Line(POINT pt1,POINT pt2,DWORD color);
	//绘制矩形
	virtual  CHVImage & Rectangle(int x0,int y0, int x1,int y1,DWORD color);
	virtual  CHVImage & Rectangle(POINT topLeft,POINT rightBottom, DWORD color);
	virtual  CHVImage & Rectangle(RECT rc, DWORD color);
	//取得该图像的连通区域实例，取得之后需要条用CHVRegion::Create来创建连通区域
	virtual  CHVRegion & GetRgn(void){return m_Rgn;}
#if defined(WIN32) && defined(DDRAW)
	//返回DirectDraw实例
	CDirectDraw & GetDDraw(void){return *ddraw;}
	//在窗口上画图，对于YUV422，YV12，RGB的图像格式可以直接显示，其他的格式需要转换
	virtual  CHVImage & Draw(HWND hWnd);
	//更新图像显示
	virtual  CHVImage & Invalidate(void);
#endif
	//图像拷贝函数，可以实现各种图形格式的转换，图像的截取等等
	virtual  CHVImage & Draw(CHVImage * dstImage, RECT * dstRC = NULL, RECT * srcRC = NULL, int angle = 0, bool m_bFlip = false, bool m_bMirror = false);
	virtual  CHVImage & Draw(CHVImage * dstImage, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight, int angle = 0, bool m_bFlip = false, bool m_bMirror = false);
	//图像翻转，目前仅有翻转90度和180度
	virtual  CHVImage & Rotate(int angle=90);
	//图像倒转
	virtual  CHVImage & Flip(void);
	//图像镜像
	virtual  CHVImage & Mirror(void);
	//图像调整,色调[-240,240]、饱和度[-240,240]、亮度[-240,240]
	virtual  CHVImage & Adjust(int addHue, int addSaturation, int addLight, RECT * rc = NULL);
	//统计色调[0,240]和亮度[0,240]在某一范围的值,返回指定区域的平均色调和亮度
	virtual  CHVImage & GetAvgHueAndBright(int minHue, int maxHue, int * retAvgHue, int minLight = 0, int maxLight = 240, int * retAvgLight = NULL, RECT * rc = NULL);
	//转换灰度图片
	virtual  CHVImage & ToGray(RECT * rc = NULL);
	//转换二值图
	virtual  CHVImage & ToBin(RECT * rc = NULL);
	//根据给定的色度范围[色度为一个圆柱形,大于240的则从零开始算,比如startHue=200,endHue=10]和亮度范围进行二值图,在该范围内的值变为白色，否则为黑色,m_bInvert=true则黑白相反
	virtual  CHVImage & ToBin(int startHue, int endHue, int minLight, int maxLight, bool m_bInvert = false, RECT * rc = NULL);
	//图像放大缩小
	virtual CHVImage  & Resample(int newWidth, int newHeight, CHVImage **image = NULL);
	//从文件加载图像，YUV对应jpg图像,m_bIJL=true表示用ijl来压缩解压,false则表示用libjpeg进行压缩解压,仅对jpeg文件有效
	virtual  bool Load(const char * fname, int angle=0, bool m_bHalfHeight = false, bool m_bIJL = true);
	//从内存加载图像,加载是是否要旋转图片,angle=0,90,180,270
	virtual  bool Load(const BYTE * bImage, int size, int angle=0, bool m_bHalfHeight = false, bool m_bIJL = true);
	//加载Bayer8数据
	virtual  bool LoadFromBayer8(BYTE * bayer8, int width, int height, int angle = 0, bool m_bHalfHeight = false);
	//保存图像到文件
	virtual  bool Save(const char * fname, bool m_bIJL = true);
	//保存图像到内存
	virtual  bool Save(BYTE * bImage, int & size, bool m_bIJL = true);
public:
	//拷贝一条扫描线
	virtual  bool CopyALine(int srcX,int srcY, CHVImage *dstImage,int dstX,int dstY, int width);
	//写入一条扫描线，m_bVert垂直还是水平，默认是水平, m_bNormal从左到右[从上到下]还是从右到左[从下到上]
	virtual void WriteAScanLine(int x, int y, BYTE * line, int size, bool m_bVert = false, bool m_bNormal = true);
	//读取以后扫描线,scaleX 是放大因子*16384[为了方便计算，乘以2的14次方]
	virtual void ReadAScanLine(int x, int y, int scaleX, BYTE *line, int size, HVImageType type = NONE);
#ifdef _SWIMAGEOBJ_INCLUDED__
public:
	virtual  bool Load(IReferenceComponentImage * jpgImage, bool m_bHalfHeight = false, CItgArea* pItgArea = NULL,bool m_bTurn = false, bool m_bIJL = true);
	virtual  CHVImage & SetHalfHeight(bool m_bHalf = true);
	virtual  bool IsHalfHeight(void);
	virtual  IReferenceComponentImage  * Save(void);

	STDMETHOD(GetImage)(HV_COMPONENT_IMAGE* pImage)              {return m_nRefImage ? m_nRefImage->GetImage(pImage):E_FAIL;}
	STDMETHOD(Assign)(const HV_COMPONENT_IMAGE& imgInit)         {Resize(imgInit.iWidth, imgInit.iHeight); return m_nRefImage ? m_nRefImage->Assign(imgInit) :E_FAIL;}
	STDMETHOD(Convert)(const HV_COMPONENT_IMAGE& imgInit)        {return m_nRefImage ? m_nRefImage->Convert(imgInit):E_FAIL;}
	STDMETHOD(Serialize)(bool fOut)                              {return m_nRefImage ? m_nRefImage->Serialize(fOut) :E_FAIL;}
	virtual void AddRef()                                        {m_cRef++;}
	virtual void Release()                                       {m_cRef--; if(!m_cRef)delete this;}
	virtual void ClearYUV()                                      {if(m_nRefImage)m_nRefImage->ClearYUV();}
	virtual DWORD32 GetFrameNo()                                 {return m_nRefImage ? m_nRefImage->GetFrameNo()  :0;}
	virtual char *GetFrameName()                                 {return m_nRefImage ? m_nRefImage->GetFrameName():NULL;}
	virtual DWORD32 GetRefTime()                                 {return m_nRefImage ? m_nRefImage->GetRefTime()  :0;}
	virtual DWORD32 GetFlag()                                    {return m_nRefImage ? m_nRefImage->GetFlag()     :0;}
	virtual IReferenceComponentImage *GetLastRefImage()          {return m_nRefImage ? m_nRefImage->GetLastRefImage():NULL;}
	virtual IReferenceComponentImage *GetNextRefImage()          {return m_nRefImage ? m_nRefImage->GetNextRefImage():NULL;}
	virtual void SetFrameNo(DWORD32 dwFrameNo)                   {if(m_nRefImage)m_nRefImage->SetFrameNo(dwFrameNo);}
	virtual void SetFrameName(const char *lpszFrameName)         {if(m_nRefImage)m_nRefImage->SetFrameName(lpszFrameName);}
	virtual void SetRefTime(DWORD32 dwRefTime)                   {if(m_nRefImage)m_nRefImage->SetRefTime(dwRefTime);}
	virtual void SetFlag(DWORD32 dwFlag)                         {if(m_nRefImage)m_nRefImage->SetFlag(dwFlag);}
	virtual void SetLastRefImage(IReferenceComponentImage *pLast){if(m_nRefImage)m_nRefImage->SetLastRefImage(pLast);}
	virtual void SetNextRefImage(IReferenceComponentImage *pNext){if(m_nRefImage)m_nRefImage->SetNextRefImage(pNext);}
	virtual void SetImageContent(HV_COMPONENT_IMAGE& img)        {if(m_nRefImage)m_nRefImage->SetImageContent(img);}
	virtual void SetImageSize(const HV_COMPONENT_IMAGE& img)     {if(m_nRefImage)m_nRefImage->SetImageSize(img);}
protected:
	IReferenceComponentImage * m_nRefImage;
	int m_cRef;
#endif
public:
	//RGB和RGBQUAD转换
	static inline DWORD RGBQUADtoRGB(RGBQUAD c);
	static inline RGBQUAD RGBtoRGBQUAD(DWORD rgb);
	//hue--色调;saturation--饱和度;value--亮度
	static inline void  HSLtoRGB(BYTE h, BYTE s, BYTE l, BYTE *r, BYTE *g, BYTE *b);
	static inline DWORD HSLtoRGB(DWORD hsl);
	static inline void  RGBtoHSL(BYTE r, BYTE g, BYTE b, BYTE *h, BYTE *s, BYTE *l);
	static inline DWORD RGBtoHSL(DWORD rgb);
	static inline void  YUVtoRGB(BYTE y, BYTE u, BYTE v, BYTE *r, BYTE *g, BYTE *b);
	static inline DWORD YUVtoRGB(DWORD yuv);
	static inline void  RGBtoYUV(BYTE r, BYTE g, BYTE b, BYTE *y, BYTE *u = NULL, BYTE *v = NULL);
	static inline DWORD RGBtoYUV(DWORD rgb);
protected:
	HVImageType m_Type;
	int      m_iWidth;
	int      m_iHeight;
	int      m_iStride;
	BYTE   * m_bImage;
	int      m_iUserData;
	//图像区域
	CHVRegion  m_Rgn;
	//图像处理专用
	CHVImage * rotateImage;
	CHVImage * mirrorImage;
#if defined(WIN32) && defined(DDRAW)
	CDirectDraw * ddraw;
#endif
#ifdef RGB_YUV_TABLE
	static bool m_bInitTable;
	static int  V_r[256],U_g[256],V_g[256],U_b[256];
	static int  R_y[256],G_y[256],B_y[256],By_u[256],Ry_v[256];
#endif
};

/*
文本处理类，通过该类可以在图像上显示文字
*/
class CHVText : public CHVImage
{
public:
	CLASSINFO(CHVText,CHVImage)
		CHVText();
	//文字
	CHVText(const char * text);
	//时间
	CHVText(time_t t);
	virtual ~CHVText();
	//重载操作符，赋值址蛘呤奔湫畔?
	CHVText & operator=(const char * text);
	CHVText & operator=(time_t t);
	CHVText & SetText(const char * text);
	CHVText & SetTime(time_t t);
	//设置文字的颜色
	CHVText & SetTextColor(DWORD color=RGB(255,0,0));
	//设置背景色
	CHVText & SetBkColor(DWORD color=RGB(0,0,0));
	//设置背景透明度，如下50表明透明度为50%
	CHVText & SetTransparent(int iTransparent = 50);
	//设置字体的类型和大小
	CHVText & SetFont(const char * fontName="新宋体", int fontSize=0, int fontWeight=400/*FW_NORMAL*/, bool m_bItalic=FALSE, bool m_bUnderline=FALSE);
	//在图片上显示文字信息
	CHVText & Draw(int x, int y, CHVImage * image);
protected:
	//重载基类的虚函数
	virtual  CHVImage & Resize(int width, int height);
	virtual  DWORD GetPixel(int x,int y);
	virtual  CHVImage & PutPixel(int x,int y,DWORD color);
private:
	DWORD   textColor;
	DWORD   bkColor;
	int     m_iTransparent;
	LOGFONT logFont;
};
#endif

