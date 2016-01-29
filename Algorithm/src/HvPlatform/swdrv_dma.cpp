#include "swdrv.h"

extern "C"
{

    int SwDmaCopy1D(
        int fd,
        BYTE8* pbDstPhys,
        BYTE8* pbSrcPhys,
        DWORD32 dwSize
    )
    {
        DMACOPY_STRUCT dma;

        dma.src = (u32)pbSrcPhys;
        dma.dst = (u32)pbDstPhys;
        dma.a = dwSize;
        dma.b = 1;
        dma.c = 1;
        dma.sbidx = 1;
        dma.scidx = 1;
        dma.dbidx = 1;
        dma.dcidx = 1;

        return ioctl(fd, SWDEV_IOCTL_DMA_COPY, &dma);
    }

    int SwDmaCopy2D(
        int fd,
        BYTE8* pbDstPhys, int iDstStride,
        BYTE8* pbSrcPhys, int iSrcStride,
        int iWidth, int iHeight
    )
    {
        DMACOPY_STRUCT dma;

        dma.src = (u32)pbSrcPhys;
        dma.dst = (u32)pbDstPhys;
        dma.a = iWidth;
        dma.b = iHeight;
        dma.c = 1;
        dma.sbidx = iSrcStride;
        dma.scidx = 1;
        dma.dbidx = iDstStride;
        dma.dcidx = 1;

        return ioctl(fd, SWDEV_IOCTL_DMA_COPY, &dma);
    }

    int SwDmaConvImage()
    {
        return ENOSYS;
    }

    int SwDmaCopyJpeg()
    {
        return ENOSYS;
    }

    int SwDmaCopyH264()
    {
        return ENOSYS;
    }

    int SwDmaCopyYUV()
    {
        return ENOSYS;
    }

    int SwDmaRotationYUV(
        int fd,
        BYTE8* pbSrcImgY,
        BYTE8* pbSrcImgUV,
        BYTE8* pbDstImgY,
        BYTE8* pbDstImgUV,
        int iWidth,
        int iHeight)
    {
        ROTATION_YUV_STRUCT dma;
        dma.y_src = (u32)pbSrcImgY;
        dma.y_dst = (u32)pbDstImgY;
        dma.uv_src = (u32)pbSrcImgUV;
        dma.uv_dst = (u32)pbDstImgUV;
        dma.width = iWidth;
        dma.height = iHeight;
        return ioctl(fd, SWDEV_IOCTL_DMA_ROTATION_YUV, &dma);
    }

    int SwDmaCopyUV(
        int fd,
        BYTE8* pbSrcImgUV,
        BYTE8* pbDstImgU,
        BYTE8* pbDstImgV,
        int iWidth,
        int iHeight)
    {
        COPY_UV_STRUCT dma;
        dma.uv_src = (u32)pbSrcImgUV;
        dma.u_dst = (u32)pbDstImgU;
        dma.v_dst = (u32)pbDstImgV;
        dma.width = iWidth;
        dma.height = iHeight;
        return ioctl(fd, SWDEV_IOCTL_DMA_COPY_UV, &dma);
    }

    int SwDmaPCIWrite()
    {
        return ENOSYS;
    }

    int SwDmaPCIRead()
    {
        return ENOSYS;
    }

}   // end of extern "C"
