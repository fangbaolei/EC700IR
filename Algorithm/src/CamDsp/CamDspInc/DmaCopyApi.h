#ifndef _DMACOPYAPI_H_
#define _DMACOPYAPI_H_

typedef void* DMA_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

// 初始化DMA
void DmaInit();

// 打开一个DMA句柄
DMA_HANDLE DmaOpen();

// 进行DMA的1D模式拷贝
int DmaCopy1D(DMA_HANDLE hDma, unsigned char* pbDst, unsigned char* pbSrc, int iSize);

// 进行DMA的2D模式拷贝
int DmaCopy2D(
	DMA_HANDLE hDma, 
	unsigned char* pbDst, 
	int iDstStride, 
	unsigned char* pbSrc, 
	int iSrcStride, 
	int iElementSize, 
	int iElementsNum
);

// 等待拷贝结束
int DmaWaitFinish(DMA_HANDLE hDma);

// 关闭一个DMA句柄
int DmaClose(DMA_HANDLE hDma);

// 卸载DMA
void DmaExit();

// 兼容以前标清和高清的DMA封装模式
int HV_dmasetup_dm6467();
int HV_dmawait_dm6467(int iHandle);
int HV_dmacpy1D_dm6467(unsigned char* pbDst, unsigned char* pbSrc, int iSize);
int HV_dmacpy2D_dm6467(unsigned char* pbDst, int iDstStride, unsigned char* pbSrc, int iSrcStride, int iWidth, int iHeight);

// Comment by Shaorg：DMA使用注意事项
// 1、内存限制：要拷贝的内存不能在栈上；一次拷贝最大长度为64KB；必须128字节对齐。
// 2、Cache一致性问题：自行使用csl_cache.h头文件中的相关函数保证Cache的一致性。
// 3、不要在H264Enc_Open函数执行之前初始化并打开DMA通道，否则打开的DMA通道将有部分可能在使用时出现异常。

#ifdef __cplusplus
}
#endif

#ifndef SAFE_CLOSE_DMA
#define SAFE_CLOSE_DMA(h)						\
	if (h)										\
	{											\
		DmaClose(h);							\
		h = NULL;								\
	}
#endif

#endif
