#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdint.h>

#include "common_protocol.h"

#ifdef __DUMP_CMD
void dump_cmd(char* str, unsigned char* buffer, int len)
{
    int i = 0;
    if (str)
        com_debug(6, "%s ", str);
    for (i = 0; i < len; i++)
        com_debug(6, "%02x ", buffer[i]);
    com_debug(6, "\n");
}
#endif
int32_t com_open_serial(const char *device_name, int baudrate, int data_bits, int parity, int stop_bits)
{
    int fd = 0;
    int len = 0;
    int  i = 0;
    struct termios options;
    int speed_arr[] = {B9600, B115200, B38400, B19200, B4800, B2400, B1200};
    int name_arr[] = {9600, 115200, 38400, 19200, 4800, 2400, 1200};

#ifdef __LL_DEBUG
    return 0;
#endif

    if (device_name == NULL)
    {
        com_print("no serial port here, can not continue.\n");
        return -1;
    }

    fd = open(device_name, O_RDWR | O_NDELAY | O_NOCTTY);

    if (fd < 0)
    {
        com_print("(%s): cannot open serial device %s: %s\n",__FILE__,device_name, strerror(errno));
        return -1;
    }	
    fcntl(fd, F_SETFL,0);
    /* Setting port parameters */
    tcgetattr(fd, &options);

    /* baud rate */
    len = sizeof(speed_arr) / sizeof(int);
    for (i = 0; i < len; i++)
    {
        if (baudrate == name_arr[i])
        {
            break;
        }
        if (i == len)
        {
            i = 0;  // default: 9600
        }
    }

    cfsetispeed(&options,speed_arr[i]);

    /* data bits */
    switch (data_bits)
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

    /* parity bits */
    switch (parity)
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
    switch (stop_bits)
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
    com_print("%s init %s at %d %d %c %d\n", __func__, device_name, name_arr[i], data_bits, parity, stop_bits);

    return fd;
}

//todo:在关闭时调用COM_unread_bytes
int32_t com_close_serial(int fd)
{
    int ret = 0;
#ifdef __LL_DEBUG
    return 0;
#endif

    if (fd == -1)
    {
        return 0;
    }
    ret = close(fd);
    if (ret < 0)
    {
        return -1;
    }
    
    return 0;
}

int32_t com_unread_bytes(int fd, unsigned char *buffer, int *buffer_size)
{
    uint32_t bytes = 0;
    //*buffer_size = 0;
    int ret = 0;
#ifdef __LL_DEBUG
    return 0;
#endif
    ioctl(fd, FIONREAD, &bytes);
    if (bytes>0)
    {
        bytes = (bytes>*buffer_size) ? *buffer_size : bytes;
        ret = read(fd, &buffer, bytes);
        *buffer_size = bytes;
        return -1;
    }
    return 0;
}

int32_t com_write_packet_data(int fd, unsigned char *buffer, int buffer_size)
{
    int err;

#ifdef __LL_DEBUG
    dump_cmd(NULL, buffer, buffer_size);
    return 0;
#endif
#ifdef __DUMP_CMD
    dump_cmd(NULL, buffer, buffer_size);
#endif

    // 在写前先清空接收缓冲区
    tcflush(fd, TCIOFLUSH);

    err = write(fd, buffer, buffer_size);
    if (err < buffer_size)
    {
        com_print("write cmd error: %s\n", strerror(errno));
        return -1;
    }
    else
        return 0;
}

int32_t com_get_packet_terminator(int fd, unsigned char *buffer, int *buffer_size, int terminator)
{
    int pos=0;
    int bytes = *buffer_size;
    int ret = 0;
    int bytes_read = 0;
    fd_set rfds;

    int timeout = 5000; // 500 ms
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

#ifdef __LL_DEBUG
    return 0;
#endif

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    // select等待超时
    int retval = select(fd+1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
    {
        com_debug(5, "select error\n");
        return COM_FAILURE;
    }
    else if (retval > 0)
    {
        // wait for message
        ioctl(fd, FIONREAD, &bytes);
        while (bytes==0)
        {
            usleep(0);
            ioctl(fd, FIONREAD, &bytes);
        }

        // get octets one by one
        ret=read(fd, buffer, 1);
        while (buffer[pos] != terminator)
        {
            pos++;
            ret=read(fd, &buffer[pos], 1);
            usleep(0);
        }
        bytes_read = pos+1;
    }
    else
    {
        //com_debug(7, "%s read select timeout\n", __func__);
        return COM_TIMEOUT;
    }
#ifdef __DUMP_CMD
    dump_cmd("recv ", buffer, bytes_read);
#endif
    *buffer_size = bytes_read;
    return COM_SUCCESS;
}

int32_t com_get_packet(int fd, unsigned char *buffer, int *buffer_size)
{
    fd_set rfds;
    int bytes_read = 0;
    int tmp_len = 0;
    unsigned char *tmp = buffer;
    int left = *buffer_size;
    int size = *buffer_size;

    int timeout = 800; // 800 ms
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

#ifdef __LL_DEBUG
    return 0;
#endif

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    // select等待超时
    int retval = select(fd+1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
    {
        //com_debug(5, "select error\n");
        return COM_FAILURE;
    }
    else if (retval > 0)
    {
        // wait for message
        ioctl(fd, FIONREAD, &tmp_len);
        //com_debug(7, "!!!FIONREAD: %d", tmp_len);
        while (tmp_len==0)
        {
            usleep(0);
            ioctl(fd, FIONREAD, &tmp_len);
        }
#if 0
        int ret = 0;
        ret = read(fd, tmp, 6);
        com_debug(7, "read: %d\n", ret);
        dump_cmd("read ", tmp, ret);
        left = size - 6;
        tmp += ret;
        bytes_read = ret;
#endif
        do{
            // 注：测试时发现，当传递的长度比实际大的时候，read读完实际数据后会一直阻塞
            // 故在这里再加一个select。
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            retval = select(fd+1, &rfds, NULL, NULL, &tv);
            if (retval == 0)
            {
                break;
            }
            tmp_len = read(fd, tmp, left);
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
        *buffer_size = 0;
        return COM_TIMEOUT;
    }

#ifdef __DUMP_CMD
    dump_cmd("recv cmd", buffer, bytes_read);
#endif
    *buffer_size = bytes_read;
    return COM_SUCCESS;
}

/////////////////////////////////////
// misc
int32_t com_usleep(uint32_t useconds)
{
    return (uint32_t)usleep(useconds);
}
