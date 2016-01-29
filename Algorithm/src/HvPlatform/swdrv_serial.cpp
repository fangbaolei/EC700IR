#include "swdrv.h"
#include <signal.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

#ifdef	DEBUG_SWDRV_SERIAL
#define PRINTF printf
#else
#define PRINTF(...)
#endif

extern "C"
{

////////////////////////////////////////////////////////////
// linux串口参数设置相关
////////////////////////////////////////////////////////////

static int baud_rate[] =
{
    B1200,   B1800,   B2400,   B4800,
    B9600,   B19200,  B38400,  B57600,
    B115200, B230400, B460800, B500000,
    B576000, B921600,
    -1
};

static int baud[] =
{
    1200,   1800,   2400,   4800,
    9600,   19200,  38400,  57600,
    115200, 230400, 460800, 500000,
    576000, 921600,
    -1
};

// 设置波特率
static int set_serial_baud_rate(struct termios *opts, int br)
{
    int i = 0;

    while (baud[i] != -1)
    {
        if (baud[i] == br)
        {
            break;
        }
        i++;
    }

    if (baud[i] == -1)
    {
        PRINTF("baud rate is invalid!\n");
        return EINVAL;
    }

    if (-1 == cfsetispeed(opts, baud_rate[i]))
    {
        PRINTF("in baud rate set failed!\n");
        return EINVAL;
    }

    if (-1 == cfsetospeed(opts, baud_rate[i]))
    {
        PRINTF("out baud rate set failed!\n");
        return EINVAL;
    }

    return 0;
}

// 设置数据位
static int set_serial_data_size(struct termios *opts, int ds)
{
    int dsize = 0;

    switch (ds)
    {
    case 5:
        dsize = CS5;
        break;
    case 6:
        dsize = CS6;
        break;
    case 7:
        dsize = CS7;
        break;
    case 8:
        dsize = CS8;
        break;
    default:
        return EINVAL;
    }

    opts->c_cflag &= ~CSIZE;
    opts->c_cflag |= dsize;

    return 0;
}

// 设置奇偶校验
static void set_serial_parity(struct termios *opts, int p)
{
    switch (p)
    {
    case 0: /* none */
        opts->c_cflag &= ~PARENB;
        opts->c_iflag &= ~INPCK;
        break;

    case 1: /* odd */
        opts->c_cflag |= PARENB;
        opts->c_cflag |= PARODD;

        /* check parity and skip it */
        opts->c_iflag |= (INPCK | ISTRIP);
        break;

    case 2: /* even */
        opts->c_cflag |= PARENB;
        opts->c_cflag &= ~PARODD;

        /* check parity and skip it */
        opts->c_iflag |= (INPCK | ISTRIP);
        break;

    default:
        PRINTF("Invalid Parity number");
        break;
    }
}

// 设置停止位
static void set_serial_stop_bit(struct termios *opts, int s)
{
    if (s != 1)
        opts->c_cflag |= CSTOPB;
    else
        opts->c_cflag &= ~CSTOPB;
}

// 设置串口属性
// br: 波特率
// ds: 数据位
// p:  奇偶校验位
// sb: 停止位
static int set_serial_attr(int fd, int br, int ds, int p, int sb)
{
    struct termios opts;

    if (tcgetattr(fd, &opts) < 0)
    {
        PRINTF("Get terminal attribute failed");
        return EINVAL;
    }

    // choose raw input
    opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // choose raw output
    opts.c_oflag &= ~OPOST;

    // Enable the receiver and set local mode...
    opts.c_cflag |= (CLOCAL | CREAD);

    // set baudrate
    if (set_serial_baud_rate(&opts, br) != 0)
    {
        return EINVAL;
    }

    // set data bits
    if (set_serial_data_size(&opts, ds) != 0)
    {
        return EINVAL;
    }

    set_serial_parity(&opts, p);

    set_serial_stop_bit(&opts, sb);

    //设置流控
    //RTS/CTS (硬件) 流控制
    opts.c_cflag &= ~CRTSCTS; //无流控
    //输入的 XON/XOFF 流控制
    opts.c_iflag &= ~IXOFF;//不启用
    //输出的 XON/XOFF 流控制
    opts.c_iflag &= ~IXON ;//不启用

    // 防止0x0D变0x0A
    opts.c_iflag &= ~(ICRNL|IGNCR);

    // change attribute now
    if (tcsetattr(fd, TCSANOW, &opts) < 0)
    {
        PRINTF("Set terminal attribute failed");
        return EINVAL;
    }
    return 0;
}
////////////////////////////////////////////////////////////

int SwSerialOpen(const char* szDevName)
{
    return open(szDevName, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
}

int SwSerialClose(int fd)
{
    return close(fd);
}

int SwSerialSetAttr(int fd,
                    int iBaudrate,
                    int iDatasize,
                    int iParity,
                    int iStopBit)
{
    return set_serial_attr(fd, iBaudrate, iDatasize, iParity, iStopBit);
}

int SwSerialSend(int fd, BYTE8* pBuf, DWORD32 dwLen)
{
    return write(fd, pBuf, dwLen);
}

int SwSerialRecv(int fd, BYTE8* pBuf, DWORD32 dwLen)
{
    return read(fd, pBuf, dwLen);
}

int SwSerialFlush(int fd, int iMode)
{
    int iRet = EINVAL;
    switch (iMode)
    {
    case SW_MODE_SERIAL_IOFLUSH :
        iRet = tcflush(fd, TCIOFLUSH);
        break;

    case SW_MODE_SERIAL_IFLUSH :
        iRet = tcflush(fd, TCIFLUSH);
        break;

    case SW_MODE_SERIAL_OFLUSH :
        iRet = tcflush(fd, TCOFLUSH);
        break;

    default :
        break;
    }
    return iRet;
}

}   // end of extern "C"

