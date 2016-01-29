/**
* @file	GetImgByVPIF.h
* @version	1.0
* @brief 基于V4L2构架，通过VPIF从前端相机获取图像数据
*/

#ifndef _GETIMGBYVPIF_H_
#define _GETIMGBYVPIF_H_

#include "hvutils.h"

#ifdef _CAMERA_PIXEL_500W_
    #define IMAGE_WIDTH         2448
    #define IMAGE_HEIGHT        1024
    #define BYTES_PER_LINE      2448
    #define IMAGE_DATA_SIZE     5013504 // 注：前端出来的500w图像是场模式的
#else
    #define IMAGE_WIDTH         1600
    #define IMAGE_HEIGHT        1200
    #define BYTES_PER_LINE      1600
    #define IMAGE_DATA_SIZE     3842048
#endif

typedef struct _IMG_DATA
{
    int iDataType;
    void* addr;  // 虚拟地址
    int iDataLen;
    void* offset;  // 物理地址

    _IMG_DATA()
    {
        iDataType = 0;
        addr = NULL;
        iDataLen = 0;
        offset = NULL;
    };

} IMG_DATA;

int open_capture(int port);
int init_capture(int fd);
int get_capture_buffer(int fd, IMG_DATA* pImgData);
int put_capture_buffer(int fd, IMG_DATA* pImgData);
int start_capture(int fd);
int stop_capture(int fd);
int close_capture(int fd);

#endif
