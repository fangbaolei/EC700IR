#ifndef _LINUXBMP_H_
#define _LINUXBMP_H_

#include <iostream>
using namespace std;

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;

#define BI_RGB 0L
#define BI_RLE8 1L
#define BI_RLE4 2L
#define BI_BITFIELDS 3L
#define BI_JPEG 4L
#define BI_PNG 5L

typedef struct {
WORD bfType;//2
DWORD bfSize;//4
WORD bfReserved1;//2
WORD bfReserved2;//2
DWORD bfOffBits;//4
}__attribute__((packed))BITMAPFILEHEADER;
typedef struct{
DWORD biSize;//4
LONG biWidth;//4
LONG biHeight;//4
WORD biPlanes;//2
WORD biBitCount;//2
DWORD biCompression;//4
DWORD biSizeImage;//4
LONG biXPelsPerMeter;//4
LONG biYPelsPerMeter;//4
DWORD biClrUsed;//4
DWORD biClrImportant;//4
}__attribute__((packed))BITMAPINFOHEADER;




/*typedef struct
{
unsigned char rgbBlue;
unsigned char rgbGreen;
unsigned char rgbRed;
unsigned char rgbReserved;
}RGBQuad;//it may be useless*/
typedef struct
{
BYTE rgbBlue;
BYTE rgbGreen;
BYTE rgbRed;
BYTE rgbReserved;
}RGB_data, RGBQUAD;//RGB TYPE

typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
}BITMAPINFO, FAR *LPBITMAPINFO, *PBITMAPINFO;

#endif // _LINUXBMP_H_
