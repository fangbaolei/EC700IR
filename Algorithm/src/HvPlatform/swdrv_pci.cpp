#include "swdrv.h"
#include "swdrv_base.h"
#include <signal.h>
#include <sys/mman.h>

#ifdef _CAM_APP_
#include "hvthread.h"
/* CamApp's main.cpp */
extern HV_SEM_HANDLE g_hSemEDMA;
using namespace HiVideo;
#endif

extern "C"
{

int SwPciOpen()
{
    int fd = ENOSYS;
#ifdef SW_DEVID_PCI_DEF
    fd = open(SWPCI_NAME, O_RDWR | O_NONBLOCK, 0); // 非阻塞打开
#endif
    return fd;
}

int SwPciClose(int fd)
{
    if (fd == -1)
    {
        return EINVAL;
    }
    return close(fd);
}

int SwInitPciRWBuffers(int fd, swpci_buffer* pBuf)
{
    int nBufCount = ioctl(fd, SWPCI_IOCTL_LIST_BUFS, pBuf);
	if (nBufCount <= 0)
	{
	    return nBufCount;
	}

	for (int i = 0; i < nBufCount; ++i)
	{
		// buf的地址映射到应用层中
		pBuf[i].user = mmap(NULL,
			pBuf[i].size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			pBuf[i].phys
			);

		if (pBuf[i].user == MAP_FAILED)
		{
			return -1;
		}
	}
	return nBufCount;
}

int SwPciPutRecvBuf(int fd, swpci_buffer* pBuf)
{
    if (fd == -1)
    {
        return EINVAL;
    }
#ifdef _CAM_APP_
    SemPend(&g_hSemEDMA);
    int iRet = ioctl(fd, SWPCI_IOCTL_PUTR_BUF, pBuf);
    SemPost(&g_hSemEDMA);
#else
    int iRet = ioctl(fd, SWPCI_IOCTL_PUTR_BUF, pBuf);
#endif
    return iRet;
}

int SwPciGetSendBuf(int fd, swpci_buffer* pBuf)
{
    if (fd == -1)
    {
        return EINVAL;
    }
#ifdef _CAM_APP_
    SemPend(&g_hSemEDMA);
    int iRet = ioctl(fd, SWPCI_IOCTL_GETW_BUF, pBuf);
    SemPost(&g_hSemEDMA);
#else
    int iRet = ioctl(fd, SWPCI_IOCTL_GETW_BUF, pBuf);
#endif
    return iRet;
}

int SwPciPutSendBuf(int fd, swpci_buffer* pBuf)
{
    if (fd == -1)
    {
        return EINVAL;
    }
#ifdef _CAM_APP_
    SemPend(&g_hSemEDMA);
    int iRet = ioctl(fd, SWPCI_IOCTL_PUTW_BUF, pBuf);
    SemPost(&g_hSemEDMA);
#else
    int iRet = ioctl(fd, SWPCI_IOCTL_PUTW_BUF, pBuf);
#endif
    return iRet;
}

int SwPciFreeSendBuf(int fd, swpci_buffer* pBuf)
{
    if (fd == -1)
    {
        return EINVAL;
    }
#ifdef _CAM_APP_
    SemPend(&g_hSemEDMA);
    int iRet = ioctl(fd, SWPCI_IOCTL_FREE_BUF, pBuf);
    SemPost(&g_hSemEDMA);
#else
    int iRet = ioctl(fd, SWPCI_IOCTL_FREE_BUF, pBuf);
#endif
    return iRet;
}

}   // end of extern "C"
