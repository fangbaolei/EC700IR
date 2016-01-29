/*
本模块只支持pelco D协议摇杆
光圈控制：在手动模式、光圈优先模式下才能有效
对焦控制：在手动对焦模式下才有效
变焦控制：任何时候均可

对焦、变焦接口，有速度的参数，可以调节速度

测试时发现，摇杆同时进行PTZ，只发一个stop命令过来，代码已做处理，会停止变焦和转动

注：
1、使用swfc的读写操作接口，不能满足实际情况(会出现接收不完全，造成数据解析错误)。
2、目前的框架对串口的封装函数，不满足使用。
3、SWBaseDevice不能满足使用。
4、故在此文件自行实现串口操作接口
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdint.h>

#include "SWDomeRockerControlMSG.h"

#include "swpa_device.h"
#include "swpa_camera.h"

// 第7级是内部调试等级
#define DEFAULT_RANK 7

#define com_debug(rank, fmt, ...) \
    do {\
        if (rank <= DEFAULT_RANK) \
        {\
            printf(fmt, ##__VA_ARGS__); \
        }\
    }while(0)

// 木星只有第1个串口可以接摇杆，故这里直接用宏定义
#define TTY_DEV "/dev/ttyO1"

//////////////////////////////////////////////////////
CSWDomeRockerControlMSG::CSWDomeRockerControlMSG() : CSWMessage(0, 1)
{
    m_iFd = -1;
    m_iNeedSleep = 0;
}

CSWDomeRockerControlMSG::~CSWDomeRockerControlMSG()
{
    Close();
}

HRESULT CSWDomeRockerControlMSG::Initialize(BOOL fEnable, INT iBaudrate, INT iProtocol)
{
    HRESULT hr = 0;
    if (fEnable == FALSE)
    {
        SW_TRACE_NORMAL("No start Dome Rocker.\n");
        return S_OK;
    }
    hr = OpenSerial(TTY_DEV, iBaudrate, 8, 'N', 1);
    if (hr != S_OK)
    {
        SW_TRACE_NORMAL("No %s can init here, can not continue.\n", TTY_DEV);
        return E_FAIL;  
    }

    swpa_device_set_rs485(0);   // 设置485为接收状态

    Run();

    return S_OK;
}

HRESULT CSWDomeRockerControlMSG::OpenSerial(const char *pDeviceName, int iBaudrate, int iDatabits, int iParity, int iStopbits)
{
    int fd = 0;
    int len = 0;
    int i = 0;
    int baud_id = 0;
    struct termios options;
    int speed_arr[] = {B9600, B115200, B38400, B19200, B4800, B2400, B1200};
    int name_arr[] = {9600, 115200, 38400, 19200, 4800, 2400, 1200};

    if (pDeviceName == NULL)
    {
        SW_TRACE_NORMAL("no serial port here, can not continue.\n");
        return E_FAIL;
    }

    fd = open(pDeviceName, O_RDWR | O_NDELAY | O_NOCTTY);

    if (fd < 0)
    {
        SW_TRACE_NORMAL("Cannot open serial device %s: %s\n", pDeviceName, strerror(errno));
        return E_FAIL;
    }    
    fcntl(fd, F_SETFL,0);
    /* Setting port parameters */
    tcgetattr(fd, &options);

    /* baud rate */
    len = sizeof(speed_arr) / sizeof(int);
    for (i = 0; i < len; i++)
    {
        if (iBaudrate == name_arr[i])
        {
            break;
        }
        if (i == len)
        {
            i = 0; // default: 9600
        }
    }

    cfsetispeed(&options,speed_arr[i]);

    /* data bits */
    switch (iDatabits)
    {
    case 8:
        options.c_cflag |= CS8;
        break;
    case 7:
        options.c_cflag |= CS7;
        break;
    default:
        options.c_cflag |= CS8;
        break;
    }

    /* iParity bits */
    switch (iParity)
    {
    case 'N':
    case 'n':
        options.c_iflag &= ~INPCK;
        options.c_cflag &= ~PARENB;
        break;
    case 'O':
    case 'o':
        options.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
        options.c_cflag |= (PARODD | PARENB);
        break;
    case 'E':
    case 'e':
        options.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    default:
        options.c_iflag &= ~INPCK;
        options.c_cflag &= ~PARENB;
        break;
    }

    /* stop bits */
    switch (iStopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        options.c_cflag &= ~CSTOPB;
        break;
    }
#if 0
    options.c_cflag &= ~PARENB;     /* No parity  */
    options.c_cflag &= ~CSTOPB;     /*            */
    options.c_cflag &= ~CSIZE;      /* 8bit       */
    options.c_cflag |= CS8;         /*            */
    options.c_cflag &= ~CRTSCTS;    /* No hdw ctl */
#endif
    options.c_cflag &= ~CRTSCTS;    /* No hdw ctl */

    /* local flags */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* raw input */

    /* input flags */
    /*
    options.c_iflag &= ~(INPCK | ISTRIP); // no parity
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // no soft ctl
    */
    /* patch: bpflegin: set to 0 in order to avoid invalid pan/tilt return values */
    options.c_iflag = 0;

    /* output flags */
    options.c_oflag &= ~OPOST; /* raw output */

    tcflush(fd, TCIOFLUSH);
    options.c_cc[VTIME] = 0; /* no time out */
    options.c_cc[VMIN] = 0; /* minimum number of characters to read */

    tcsetattr(fd, TCSANOW, &options);
    SW_TRACE_NORMAL("init %s at %d %d %c %d\n", pDeviceName, name_arr[i], iDatabits, iParity, iStopbits);

    m_iFd = fd;
//    return fd;
    return S_OK;
}

HRESULT CSWDomeRockerControlMSG::Close(VOID)
{
    int ret = 0;

    if (m_iFd == -1)
    {
        return S_OK;
    }
    ret = close(m_iFd);
    if (ret < 0)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWDomeRockerControlMSG::Read(PVOID pBuffer, INT* iSize)
{
    fd_set rfds;
    int bytes_read = 0;
    int tmp_len = 0;
    unsigned char *tmp = (unsigned char*)pBuffer;
    int left = *iSize;
    int size = *iSize;

    int timeout = 500; // 500 ms
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    
    // 在读取前先清空接收缓冲区
    //tcflush(fd, TCIFLUSH);

    FD_ZERO(&rfds);
    FD_SET(m_iFd, &rfds);
    // select等待超时
    int retval = select(m_iFd+1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
    {
        SW_TRACE_NORMAL("select error\n");
        return E_FAIL;
    }
    else if (retval > 0)
    {
        do{
            FD_ZERO(&rfds);
            FD_SET(m_iFd, &rfds);
            retval = select(m_iFd+1, &rfds, NULL, NULL, &tv);
            if (retval == 0)
            {
                //printf("!!!!!!!timeout.\n");
                break;
            }
            //printf("before read left:%d\n", left);
            tmp_len = read(m_iFd, tmp, left);
            //printf("after read: %d\n", tmp_len);
            
            if (tmp_len == -1)
            {
                return -1;
            }
            if (tmp_len == 0)
            {
                break;
            }
            //com_debug(7, "read1: %d\n", tmp_len);
            //dump_cmd("read1 ", tmp, tmp_len);

            tmp += tmp_len;
            bytes_read += tmp_len;
            left = size - bytes_read;
            usleep(0);
        } while (bytes_read < size);
    }
    else
    {
        //com_debug(7, "%s read select timeout\n", __func__);
        *iSize = 0;
        return E_OUTOFTIME;
    }

    *iSize = bytes_read;
    return S_OK;
}

static void dump_cmd(char* str, unsigned char* buffer, int len)
{
    int i = 0;
    if (str)
        com_debug(6, "%s ", str);
    for (i = 0; i < len; i++)
        com_debug(6, "%02x ", buffer[i]);
    com_debug(6, "\n");
}

/*
iris cmd1 02-open 04-close
focus cmd1 01 cmd2 80
zoom cmd2 20 cmd2 40
stop cmd1&cmd2 0
*/
int CSWDomeRockerControlMSG::DispatchCommand(unsigned char* szCommand, int iCommandLen)
{
    unsigned char cmd1 = 0;
    unsigned char cmd2 = 0;

    // 说明：接摇杆测试发现，同时进行PT、Z时，会只发一个stop命令，
    // 这样PT或Z有一个可能不会停止，每种操作都记录，只有进行过的，最后都stop
    static int type_zoom = 0;
    static int type_focus = 0;
    //static int type3 = 0;
    static int type_normal = 0;

    cmd1 = szCommand[2];
    cmd2 = szCommand[3];

    CSWMessage::SendMessage(MSG_TOUCH_DOME_CAMERA, 0, 0);

    // zoom
    if (cmd2 == 0x20 || cmd2 == 0x40)
    {
        CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(TRUE), 0);
        CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(TRUE), 0);

        ZOOM_MODE mode = ZOOM_TELE;
        type_zoom = 1;
        if (cmd2 == 0x40) mode = ZOOM_WIDE;
        //swpa_camera_basicparam_set_zoom_mode(mode);
        swpa_camera_basicparam_set_zoom_speed(mode, 0x05);
    }
    // focus
    else if (cmd1 == 0x01 || cmd2 == 0x80)
    {
        FOCUS_MODE mode = FOCUS_NEAR;
        type_focus = 1;
        if (cmd2 == 0x80) mode = FOCUS_FAR;
        swpa_camera_basicparam_set_focus(mode);
        //swpa_camera_basicparam_set_focus_speed(mode, 0x03);
    }
    // iris
    else if (cmd1 == 0x02 || cmd1 == 0x04)
    {
        IRIS_MODE mode = IRIS_UP;
        //type3 = 1;
        if (cmd1 == 0x04) mode = IRIS_DOWN;
        swpa_camera_basicparam_set_iris_mode(mode);
    }
    // 处理stop命令
    else if (cmd2 == 0 &&  cmd1 == 0)
    {
        if (type_zoom)
        {
            type_zoom = 0;
            if (m_iNeedSleep)
            {
                usleep(80*1000);
                m_iNeedSleep = 0;
            }
            CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(FALSE), 0);
            CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(FALSE), 0);
            swpa_camera_basicparam_set_zoom_mode(ZOOM_STOP);
        }
        if (type_focus)
        {
            if (m_iNeedSleep)
            {
                usleep(80*1000);
                m_iNeedSleep = 0;
            }
            type_focus = 0;
            swpa_camera_basicparam_set_focus(FOCUS_STOP);
        }
        // not stop cmd for *IRIS* !!!
        /*else if (type3 == 1)
        {
        }*/
        if (type_normal)
        {
            if (cmd2 == 0x00)//stop pan, tilt or pantilt
            {
                //disable ptz overlay
                CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(FALSE), 0);
                CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(FALSE), 0);
            }
            
            type_normal = 0;
            spwa_camera_com_send_data(PROTOCOL_PELCO, szCommand, iCommandLen);
        }
    }
    else
    {
        if (cmd2 == 0x00)//start pan, tilt or pantilt
        {
            //enable ptz overlay
            CSWMessage::SendMessage(MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY, WPARAM(TRUE), 0);
            CSWMessage::SendMessage(MSG_OSD_ENABLE_H264_PTZ_OVERLAY, WPARAM(TRUE), 0);
        }
        type_normal = 1;
        spwa_camera_com_send_data(PROTOCOL_PELCO, szCommand, iCommandLen);
    }
    
    return 0;
}

// 逐位判断是为了保持每一字节都进行检查
// pelco D协议解析
void CSWDomeRockerControlMSG::ProcessPelcoCommand(unsigned char* szCommand, int iCommandLen)
{
    unsigned char* tmp = NULL;
    int cksm = 0;
    unsigned char ch = 0;
    int tmp_len = iCommandLen;

    int cmd_len = 0;
    unsigned char* cmd_ptr = NULL;

    tmp = szCommand;
    
    while (tmp_len > 0)
    {
        while (*tmp != 0xff)
        {
            tmp++;
            tmp_len--;
            if (tmp_len <= 0) // 防止后面是其它数据干扰
            goto end;
        }
        // 找到命令头，此时tmp指到0xff
        cmd_ptr = tmp; 
        // 第一个字节
        ch = *(++tmp);
        cksm += ch;
        cmd_len = 2;    // 此时已经得到2个字节的命令
        tmp_len--;

        while (tmp_len > 0)
        {
            ch = *(++tmp);
            cmd_len++;
            tmp_len--;
            // 防止数据合理，但恰好符合检验码的数据出现，pelco至少7个字节
            // 比如 0xff 0x01 0x02 0x03是符合检验码但却不是pelco命令
            if (ch == cksm && cmd_len >= 7)
            {
                //dump_cmd("got cmd:", cmd_ptr, cmd_len);
                DispatchCommand(cmd_ptr, cmd_len);
                cksm = 0;
                break;
            }
            cksm += ch;
            cksm %= 0x100;
        }

end:
        ;
    }
}

HRESULT CSWDomeRockerControlMSG::Run()
{
    m_thPro.Start((START_ROUTINE)&OnProcessProxy, this);
    return S_OK;
}

HRESULT CSWDomeRockerControlMSG::Stop()
{
    m_thPro.Stop();
    return S_OK;
}

VOID CSWDomeRockerControlMSG::OnProcessProxy(PVOID pvParam)
{
    if( pvParam != NULL)
    {
        CSWDomeRockerControlMSG* pThis = (CSWDomeRockerControlMSG*)pvParam;
        pThis->OnProcess();
    }
}

HRESULT CSWDomeRockerControlMSG::OnProcess()
{
    unsigned char rcv_buffer[512] = {0};
    int rcv_len = 512;
    int ret = 0;
    static int cnt = 0;

    while (S_OK == m_thPro.IsValid())
    {
        ret = Read(rcv_buffer, &rcv_len);
        if (ret == 0 && rcv_len > 0)
        {
            // 说明：当接收到2个命令(14字节)时，由于时间间隔太短，图像无法反映出来，故加上延时。
            // 等效于按下按键马上放开
            if (rcv_len == 14)
            {
                m_iNeedSleep = 1;
            }
            //SW_TRACE_NORMAL("rcv len: %d\n", rcv_len);
            //dump_cmd("rcv data: ", rcv_buffer, rcv_len);
            ProcessPelcoCommand(rcv_buffer, rcv_len);
        }
        else
        {
            //SW_TRACE_NORMAL("rcv error.\n");
        }
        rcv_len = 512;
        usleep(100);
    }

    return S_OK;
}
