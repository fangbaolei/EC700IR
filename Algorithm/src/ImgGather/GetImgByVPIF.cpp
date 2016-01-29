#include "GetImgByVPIF.h"
#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

//#define _DEBUG_

#ifdef _DEBUG_
#define DPRINTF printf
#else
#define DPRINTF(...)
#endif

#if !(defined(_CAMERA_PIXEL_500W_) && defined(_CAM_APP_))

// 缓冲区数量
#define BUFFERS_COUNT 3

#define V4L2_PIX_FMT_YUV422UVP v4l2_fourcc('Y', '8', 'C', '8')
#define SET_INPUT_COUNT 10

typedef struct VideoBuffer
{
    int index;
    void *start;
    size_t length;
} VideoBuffer;

static struct VideoBuffer captuer_buffers[BUFFERS_COUNT] =
{
    0
};
static int capture_buffers_number = 0;

static char input_name[] = "COMPOSITE"; //"TVP5158";
static v4l2_std_id default_std = V4L2_STD_625_50;

int open_capture(int port)
{
    int fd;
    char devname[30] = {0};

    sprintf(devname, "/dev/video%d", port);
    fd = open(devname, O_RDWR, 0); //以“读写+阻塞”模式打开
    if (fd < 0)
    {
        printf("open camera %s error!\n", devname);
    }

    return fd;
}

int init_capture(int fd)
{
    int ret, i;

    // query capabilities
    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret == -1)
    {
        printf("VIDIOC_QUERYCAP ERROR\n");
        return -1;
    }

    // 列举支持的输入
    v4l2_std_id std;
    int input_index = -1;
    struct v4l2_input input;
    memset(&input, 0, sizeof(input));
    i = 0;
    while (1)
    {
        input.index = i;
        ret = ioctl(fd, VIDIOC_ENUMINPUT, &input);
        if (ret == -1)
        {
            break;
        }
        DPRINTF("input name = %s\n", input.name);
        if (strcmp((const char*)input.name, input_name) == 0)
        {
            input_index = input.index;
            std = input.std;
        }
        i++;
    }
    if (input_index == -1)
    {
        printf("VIDIOC_G_INPUT ERROR\n");
        return -1;
    }

    // 设置输入
    for (i=0; i<SET_INPUT_COUNT; i++)
    {
        if (ioctl(fd, VIDIOC_S_INPUT, &input_index) != -1)
        {
            break;
        }
        usleep(100000); // 100ms
    }
    if (i == SET_INPUT_COUNT)
    {
        printf("VIDIOC_S_INPUT timed out\n");
        return -1;
    }

#ifdef _DEBUG_
    // 检查当前视频设备支持的标准
    struct v4l2_standard standard;
    ret = ioctl(fd, VIDIOC_QUERYSTD, &std);
    if (ret == -1)
    {
        printf("VIDIOC_QUERYSTD ERROR\n");
        return -1;
    }
    memset(&standard, 0, sizeof(standard));
    i = 0;
    while (1)
    {
        standard.index = i;
        ret = ioctl(fd, VIDIOC_ENUMSTD, &standard);
        if (ret < 0)
        {
            break;
        }
        DPRINTF("%s standard is detected\n", standard.name);
        if (standard.id & std)
        {
            DPRINTF("curr standard is %s\n", standard.name);
        }
        i++;
    }
#endif

    if (ioctl(fd, VIDIOC_S_STD, &default_std) == -1)
    {
        printf("VIDIOC_S_STD error\n");
        return -1;
    }

#ifdef _DEBUG_
    // 列举支持的视频格式
    struct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    i = 0;
    while (1)
    {
        fmtdesc.index = i;
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc);
        if (ret < 0)
        {
            break;
        }
        DPRINTF("fmtdesc.desc = %s\n", fmtdesc.description);
        if (fmtdesc.type & V4L2_BUF_TYPE_VIDEO_CAPTURE)
        {
            DPRINTF("V4L2_BUF_TYPE_VIDEO_CAPTURE\n");
        }
        if (fmtdesc.type & V4L2_PIX_FMT_YUV422UVP)
        {
            DPRINTF("V4L2_PIX_FMT_YUV422UVP\n");
        }
        i++;
    }
    if (i == 0)
    {
        printf("VIDIOC_ENUM_FMT ERROR\n");
        return -1;
    }
#endif

    struct v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMAGE_WIDTH;
    fmt.fmt.pix.height = IMAGE_HEIGHT;
    fmt.fmt.pix.bytesperline = BYTES_PER_LINE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422UVP;

#ifdef _CAMERA_PIXEL_500W_
    fmt.fmt.pix.field = V4L2_FIELD_NONE; // !!!!!!!!!!!!!!!!!!!!!!!!!
#else
    fmt.fmt.pix.field = V4L2_FIELD_SEQ_BT;
#endif

    fmt.fmt.pix.sizeimage = fmt.fmt.pix.bytesperline*2*fmt.fmt.pix.height;
    fmt.fmt.pix.priv = 0;

    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (ret == -1)
    {
        printf("VIDIOC_S_FMT ERROR\n");
        return -1;
    }

    // 分配内存
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = BUFFERS_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(fd, VIDIOC_REQBUFS, &req);
    if (ret == -1)
    {
        printf("VIDIOC_REQBUFS ERROR\n");
        return -1;
    }

    if (req.count < BUFFERS_COUNT)
    {
        printf("Could not allocate enough input frame buffers\n");
        return -1;
    }

    capture_buffers_number = req.count;

    // 获取并记录缓存的物理空间
    struct v4l2_buffer buf;
    for (i = 0; i < capture_buffers_number; i++)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        // 读取缓存
        ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
        if (ret == -1)
        {
            printf("VIDIOC_QUERYBUF ERROR\n");
            return -1;
        }

        captuer_buffers[i].index = buf.index;
        captuer_buffers[i].length = buf.length;
        // 转换成相对地址
        captuer_buffers[i].start = mmap(NULL, buf.length,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED, fd, buf.m.offset);

        if (captuer_buffers[i].start == MAP_FAILED)
        {
            printf("VIDIOC_REQBUFS ERROR\n");
            return -1;
        }
    }

    for (i = 0; i < capture_buffers_number; i++)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        // 放入缓存队列
        ret = ioctl(fd, VIDIOC_QBUF, &buf);
        if (ret == -1)
        {
            printf("VIDIOC_QBUF ERROR\n");
            perror("init_capture");
            return -1;
        }
    }

    return 0;
}

int get_capture_buffer(int fd, IMG_DATA* pImgData)
{
    struct v4l2_buffer buf;

    // 读取缓存
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
    {
        printf("VIDIOC_DQBUF ERROR\n");
        perror("get_capture_buffer");
        return -1;
    }

    DPRINTF("index: %d\n", buf.index);
    DPRINTF("offset: 0x%08x\n", buf.m.offset);
    DPRINTF("length: %d\n", buf.length);
    DPRINTF("addr: 0x%08x\n", captuer_buffers[buf.index].start);

    pImgData->addr = captuer_buffers[buf.index].start;
    pImgData->iDataLen = buf.length;
    pImgData->offset = (void*)buf.m.offset;
    return 0;
}

int put_capture_buffer(int fd, IMG_DATA* pImgData)
{
    void *addr = pImgData->addr;
    memset(pImgData, 0, sizeof(pImgData));
    if (addr == NULL) return -1;

    int ret, i;
    int index = -1;

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    for (i = 0; i < capture_buffers_number; i++)
    {
        if (addr == captuer_buffers[i].start)
        {
            index = captuer_buffers[i].index;
            break;
        }
    }
    if (index == -1)
    {
        printf("not found the captuer buffer\n");
    }

    buf.m.offset = (unsigned long)addr;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = index;

    // 放入缓存队列
    ret = ioctl(fd, VIDIOC_QBUF, &buf);
    if (ret == -1)
    {
        printf("VIDIOC_QBUF ERROR. addr = 0x%08x\n", (int)addr);
        perror("put_capture_buffer");
    }

    return ret;
}

int start_capture(int fd)
{
    enum v4l2_buf_type type;

    //开始视频的采集
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd, VIDIOC_STREAMON, &type) < 0)
    {
        printf("VIDIOC_STREAMON error\n");
        return -1;
    }
    return 0;
}

int stop_capture(int fd)
{
    enum v4l2_buf_type type;

    // 关闭
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd, VIDIOC_STREAMOFF, &type) < 0)
    {
        printf("VIDIOC_STREAMOFF error\n");
        return -1;
    }
    return 0;
}

int close_capture(int fd)
{
    int i;
    for (i = 0; i < capture_buffers_number; i++)
    {
        munmap(captuer_buffers[i].start, captuer_buffers[i].length);
    }

    close(fd);
    return 0;
}

#else

#include "DspLinkMemBlocks.h"

////////////////////////////////////////////////////////////
// VPIF获取图像时直接使用共享内存

// 缓冲区数量
#ifdef _HV_CAMERA_PLATFORM_
#define BUFFERS_COUNT 20
#else
#define BUFFERS_COUNT 19
#endif

#define BUFFERS_RESERVE_COUNT 2

#define V4L2_PIX_FMT_YUV422UVP v4l2_fourcc('Y', '8', 'C', '8')

#define SET_INPUT_RETRY_COUNT 10

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
int g_buffer_counter = BUFFERS_COUNT;

typedef struct VideoBuffer
{
    int index;
    void *start;
    size_t length;
} VideoBuffer;

static struct VideoBuffer captuer_buffers[BUFFERS_COUNT] =
{
    0
};
static int capture_buffers_number = 0;

static char input_name[] = "COMPOSITE";
static v4l2_std_id default_std = V4L2_STD_625_50;

int open_capture(int port)
{
    int fd;
    char devname[30] = {0};

    DPRINTF("%s:%s\n", __FILE__, __FUNCTION__);

    // 打开相机
    sprintf(devname, "/dev/video%d", port);
    fd = open(devname, O_RDWR, 0); // 阻塞打开
    if (fd < 0)
    {
        printf("open camera %s error!\n", devname);
    }

    return fd;
}

int init_capture(int fd)
{
    int ret, i;
    v4l2_std_id std;
    //struct v4l2_standard standard;
    //struct video_capability capability;
    struct v4l2_capability cap;
    struct v4l2_input input;
    //struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    //enum v4l2_buf_type type;
    int input_index = -1;

    DPRINTF("%s:%s\n", __FILE__, __FUNCTION__);

    // query capabilities
    memset(&cap, 0, sizeof(cap));
    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret == -1)
    {
        printf("VIDIOC_QUERYCAP ERROR\n");
        return -1;
    }

    // 列举支持的输入
    memset(&input, 0, sizeof(input));
    i = 0;
    while (1)
    {
        input.index = i;
        ret = ioctl(fd, VIDIOC_ENUMINPUT, &input);
        if (ret == -1)
        {
            break;
        }
        DPRINTF("input name = %s\n", input.name);
        if (strcmp((const char*)input.name, input_name) == 0)
        {
            input_index = input.index;
            std = input.std;
        }
        i++;
    }
    if (input_index == -1)
    {
        printf("VIDIOC_G_INPUT ERROR\n");
        return -1;
    }

    // 设置输入
    for (i=0; i<SET_INPUT_RETRY_COUNT; i++)
    {
        if (ioctl(fd, VIDIOC_S_INPUT, &input_index) != -1)
        {
            break;
        }
        usleep(100000); // 100ms
    }
    if (i == SET_INPUT_RETRY_COUNT)
    {
        printf("VIDIOC_S_INPUT timed out\n");
        return -1;
    }

    if (ioctl(fd, VIDIOC_S_STD, &default_std) == -1)
    {
        printf("VIDIOC_S_STD error\n");
        return -1;
    }

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMAGE_WIDTH;
    fmt.fmt.pix.height = IMAGE_HEIGHT;
    fmt.fmt.pix.bytesperline = BYTES_PER_LINE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422UVP;
    fmt.fmt.pix.field = V4L2_FIELD_NONE; // !!!!!!!!!!!!!!!!!!!!!!!!!

    fmt.fmt.pix.sizeimage = fmt.fmt.pix.bytesperline*2*fmt.fmt.pix.height;
    fmt.fmt.pix.priv = 0;

    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (ret == -1)
    {
        printf("VIDIOC_S_FMT ERROR\n");
        return -1;
    }

    // 分配内存
    memset(&req, 0, sizeof(req));
    req.count = BUFFERS_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;
    ret = ioctl(fd, VIDIOC_REQBUFS, &req);
    if (ret == -1)
    {
        printf("VIDIOC_REQBUFS ERROR\n");
        return -1;
    }

    capture_buffers_number = BUFFERS_COUNT;

    // 分配空间
    for (i=0; i<BUFFERS_COUNT; i++)
    {
        captuer_buffers[i].index = i;
        captuer_buffers[i].length = fmt.fmt.pix.sizeimage;

        DSPLinkBuffer cDSPLinkBuffer;
        CreateShareMemOnPool(&cDSPLinkBuffer, fmt.fmt.pix.sizeimage);
        captuer_buffers[i].start = cDSPLinkBuffer.addr;

        printf("[%d] CMEM_alloc(%d) = %08x(phys:%08x)\n",
               i,
               fmt.fmt.pix.sizeimage,
               (int)captuer_buffers[i].start,
               (int)CMEM_getPhys(captuer_buffers[i].start));

        if (captuer_buffers[i].start == NULL)
        {
            printf("VIDIOC_REQBUFS ERROR\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "VIDIOC_REQBUFS ERROR!");
        }
        memset(captuer_buffers[i].start, 0, fmt.fmt.pix.sizeimage);
    }

    for (i=0; i<BUFFERS_COUNT; i++)
    {
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.m.userptr = (unsigned long)captuer_buffers[i].start;
        buf.length = captuer_buffers[i].length;
        buf.index = i;

        // 放入缓存队列
        ret = ioctl(fd, VIDIOC_QBUF, &buf);
        if (ret == -1)
        {
            printf("VIDIOC_QBUF ERROR\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "VIDIOC_QBUF ERROR");
        }
    }

    return 0;
}

int get_capture_buffer(int fd, IMG_DATA* pImgData)
{
    struct v4l2_buffer buf;
    int i;

    pthread_mutex_lock(&g_mutex);

    DPRINTF("[[[%d]]]\n", g_buffer_counter);

    if (g_buffer_counter <= BUFFERS_RESERVE_COUNT)
    {
        pthread_mutex_unlock(&g_mutex);
        return 1;
    }

    // 读取缓存
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;
    if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
    {
        printf("VIDIOC_DQBUF ERROR\n");

        pthread_mutex_unlock(&g_mutex);
        return -1;
    }
    else
    {
        DPRINTF("index: %d\n", buf.index);
        DPRINTF("offset: 0x%08x\n", buf.m.offset);
        DPRINTF("length: %d\n", buf.length);
        DPRINTF("addr:   0x%08x\n", (int)captuer_buffers[buf.index].start);

        g_buffer_counter --;
    }

    bool fBufValid = false;

    for (i=0; i<BUFFERS_COUNT; i++)
    {
        if (buf.m.userptr == (unsigned long)captuer_buffers[i].start)
        {
            fBufValid = true;
            break;
        }
    }

    if ( fBufValid )
    {
        pImgData->addr = (void*)buf.m.userptr;
        pImgData->iDataLen = IMAGE_DATA_SIZE;
        pImgData->offset = (void*)CMEM_getPhys(pImgData->addr);
    }

    pthread_mutex_unlock(&g_mutex);
    return (fBufValid) ? (0) : (-1);
}

int put_capture_buffer(int fd, IMG_DATA* pImgData)
{
    void *addr = pImgData->addr;
    memset(pImgData, 0, sizeof(pImgData));

    struct v4l2_buffer buf;
    int i, index = -1;
    int ret;

    DPRINTF("\n");

    if (addr == NULL)
        return -1;

    pthread_mutex_lock(&g_mutex);

    memset(&buf, 0, sizeof(buf));
    for (i = 0; i < capture_buffers_number; i++)
    {
        if (addr == captuer_buffers[i].start)
        {
            index = captuer_buffers[i].index;
            break;
        }
    }
    if (index == -1)
    {
        printf("not found the captuer buffer\n");
        pthread_mutex_unlock(&g_mutex);
        return -1;
    }

    //????
    //if (index / 2 == 0) {
    //	pthread_mutex_unlock(&g_mutex);
    //	return 0;
    //}

    buf.m.userptr = (unsigned long)captuer_buffers[index].start;
    buf.length = captuer_buffers[index].length;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;
    buf.index = index;

    DPRINTF("[%d] = %08x\n", index, (int)captuer_buffers[index].start);

    // 放入缓存队列
    ret = ioctl(fd, VIDIOC_QBUF, &buf);
    if (ret == -1)
    {
        printf("VIDIOC_QBUF ERROR. addr = 0x%08x\n", (int)addr);
        perror("put_capture_buffer");
    }
    else
    {
        g_buffer_counter ++;
    }
    DPRINTF("[[[%d]]]\n", g_buffer_counter);

    pthread_mutex_unlock(&g_mutex);
    return ret;
}

int start_capture(int fd)
{
    enum v4l2_buf_type type;

    DPRINTF("%s:%s\n", __FILE__, __FUNCTION__);

    //开始视频的采集
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd, VIDIOC_STREAMON, &type) < 0)
    {
        printf("VIDIOC_STREAMON error\n");
        return -1;
    }
    return 0;
}

int stop_capture(int fd)
{
    enum v4l2_buf_type type;

    DPRINTF("%s:%s\n", __FILE__, __FUNCTION__);

    // 关闭
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd, VIDIOC_STREAMOFF, &type) < 0)
    {
        printf("VIDIOC_STREAMOFF error\n");
        return -1;
    }
    return 0;
}

int close_capture(int fd)
{
    int i;

    DPRINTF("%s:%s\n", __FILE__, __FUNCTION__);

    for (i = 0; i < capture_buffers_number; i++)
    {
        if (CMEM_free(captuer_buffers[i].start, NULL) < 0)
        {
            printf("Failed to free buffer at %#x\n", (unsigned int) captuer_buffers[i].start);
        }
        printf("[%d] CMEM_free(%08x)\n", i, (int)captuer_buffers[i].start);
    }

    if (CMEM_exit() < 0)
    {
        fprintf(stderr, "Failed to finalize the CMEM module\n");
    }

    close(fd);
    return 0;
}

#endif // #if !(defined(_CAMERA_PIXEL_500W_) && defined(_CAM_APP_))
