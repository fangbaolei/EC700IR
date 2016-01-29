#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdint.h>

#include "swpa_mem.h"
#include "common_protocol.h"
#include "visca_protocol.h"


extern int g_camera_lock;

static int _visca_lock = 0;


int _visca_write_packet_data(VISCAInterface_t *iface, VISCAPacket_t *packet);
int _visca_get_packet(VISCAInterface_t *iface);
int _visca_send_packet(VISCAInterface_t *iface, VISCAPacket_t *packet);
int _visca_get_reply(VISCAInterface_t *iface);
int _visca_send_packet_with_reply(VISCAInterface_t *iface, VISCAPacket_t *packet);

/// 对外
int visca_open_serial(VISCAInterface_t *iface)
{
    if (iface == NULL)
    {
        com_print("(%s)no VISCAInterface_t here, do not know how to continue.\n", __FILE__);
        return VISCA_FAILURE;
    }

#if SEPARATE
	_CREATE_LOCK(_visca_lock);

	_LOCK(_visca_lock);
    iface->port_fd = com_open_serial(iface->device, 9600, 8, 'N', 1);
    if (iface->port_fd < 0)
    {
        com_print("no %s can init here, do not know how to continue.\n", iface->device);
		_UNLOCK(_visca_lock);
		_DELETE_LOCK(_visca_lock);
        return VISCA_FAILURE;  
    }

	_UNLOCK(_visca_lock);
#else
	if (g_camera_lock == 0)
	{
		_CREATE_LOCK(_visca_lock);
		g_camera_lock = _visca_lock;
	}
	else
	{
		_visca_lock = g_camera_lock;
	}
#endif
    return VISCA_SUCCESS;
}

int visca_close_serial(VISCAInterface_t *iface)
{
#if SEPARATE

    int ret = 0;
 
	_LOCK(_visca_lock);
    if (iface->device==NULL || iface->port_fd <= 0)
    {
        com_print("(%s) no need to close.\n", __FILE__);
		_UNLOCK(_visca_lock);
        return VISCA_SUCCESS;  
    }
	
    ret = com_close_serial(iface->port_fd);
    if (ret < 0)
    {
        com_print("(%s) close %s failed.\n", __FILE__, iface->device);
		_UNLOCK(_visca_lock);
        return VISCA_FAILURE;  
    }
    iface->port_fd = 0;
	
	_UNLOCK(_visca_lock);

	_DELETE_LOCK(_visca_lock);
#endif

    return VISCA_SUCCESS;
}

int visca_unread_bytes(VISCAInterface_t *iface, unsigned char *buffer, int *buffer_size)
{
    return com_unread_bytes(iface->port_fd, buffer, buffer_size);
}

// 内部使用
int _visca_write_packet_data(VISCAInterface_t *iface, VISCAPacket_t *packet)
{
    return com_write_packet_data(iface->port_fd, packet->bytes, packet->length);
}

int _visca_get_packet(VISCAInterface_t *iface)
{
    iface->bytes = 0;   // or: 1024 ??
    return com_get_packet_terminator(iface->port_fd, iface->ibuf, (int*)&iface->bytes, VISCA_TERMINATOR);
}

///////////////////////////////////////////////////////////////////////////////

void _visca_append_byte(VISCAPacket_t *packet, unsigned char byte)
{
    packet->bytes[packet->length]=byte;
    (packet->length)++;
}

void _visca_init_packet(VISCAPacket_t *packet)
{
    // set it to null
    memset(packet->bytes, '\0', sizeof(packet->bytes));
    // we start writing at byte 1, the first byte will be filled by the
    // packet sending function(_visca_send_packet). This function will also append a terminator.
    packet->length=1;
}

int _visca_send_packet(VISCAInterface_t *iface, VISCAPacket_t *packet)
{
	//机芯处理两个命令之间至少需间隔40ms
	swpa_thread_sleep_ms(50);
	
    // check data:
    if ((iface->address>7) || (iface->broadcast>1))
    {
        com_print("(%s): Invalid header parameters\n",__FILE__);
        com_print("addr: %d broadcast: %d(0x%x)\n",iface->address,
                    iface->broadcast,iface->broadcast);
        return VISCA_FAILURE;
    }

    // build header:
    packet->bytes[0] = 0x80;
    //packet->bytes[0]|=(iface->address << 4);
    if (iface->broadcast > 0)
    {
        packet->bytes[0] |= (iface->broadcast << 3);
        packet->bytes[0] &= 0xF8;
    }
    else
    {
        packet->bytes[0] |= iface->address;
    }

    //com_debug(7, "iface addr: 0x%x, buf[0]: 0x%x\n", iface->address,  packet->bytes[0]);
    // append footer(0xff)
    _visca_append_byte(packet,VISCA_TERMINATOR);

    return _visca_write_packet_data(iface, packet);
}

///////////////////////////
int _visca_get_reply(VISCAInterface_t *iface)
{
    int tmp = 0;
    // first message: -------------------
    if (_visca_get_packet(iface)!= VISCA_SUCCESS)
        return VISCA_FAILURE;

    iface->type=iface->ibuf[1]&0xF0;
    tmp = iface->ibuf[2];
    // skip ack messages
    //球机B版本固件中无法正常返回机芯SetZoom的VISCA_SUCCESS状态而导致这里超时，因此注释掉不做该值检查
    /*while (iface->type==VISCA_RESPONSE_ACK)
    {
        if (_visca_get_packet(iface)!=VISCA_SUCCESS)
        {
            com_print("%s ack expect another packet, but failed.!\n", __func__);
            return VISCA_FAILURE;
        }
        iface->type = iface->ibuf[1]&0xF0;
        tmp = iface->ibuf[2];
    }*/

    switch (iface->type)
    {
    case VISCA_RESPONSE_CLEAR:
        return VISCA_SUCCESS;
        break;
    case VISCA_RESPONSE_ADDRESS:
        return VISCA_SUCCESS;
        break;
    case VISCA_RESPONSE_COMPLETED:
        return VISCA_SUCCESS;
        break;
    case VISCA_RESPONSE_ERROR:
        {
            // 不同的错误打印出来
            if (tmp == 0x02)
            {
                com_print("%s: Syntax Error.\n", __func__);
                return VISCA_CMDNOACT;
            }
            else if (tmp == 0x41)
            {
                com_print("%s: Command Not Executable.\n", __func__);
                return VISCA_CMDNOACT;
            }
            else if (tmp == 0x03)
            {
                com_print("%s: Command Buffer Full.\n", __func__);
                return VISCA_CMDNOACT;
            }
            else if (tmp == 0x04)
            {
                com_print("%s: Command Canceled.\n", __func__);
                return VISCA_CMDNOACT;
            }
            else if (tmp == 0x05)
            {
                com_print("%s: No Socket.\n", __func__);
                return VISCA_CMDNOACT;
            }
            else
            {
                com_print("%s: Unknown Error Type.\n", __func__);
                return VISCA_CMDNOACT;
            }
            
            return VISCA_CMDERROR;
        }
        break;
    }

    return VISCA_FAILURE;
}

int _visca_send_packet_with_reply(VISCAInterface_t *iface, VISCAPacket_t *packet)
{
    int ret = 0;

    memset(iface->ibuf, '\0', COM_INPUT_BUFFER_SIZE);
    ret = _visca_send_packet(iface, packet);
    //com_debug(7, "_visca_send_packet ret: %d\n", ret);
    if (ret != VISCA_SUCCESS)
        return VISCA_FAILURE;

    ret = _visca_get_reply(iface);
    //com_debug(7, "_visca_get_reply ret: %d\n", ret);
    if (ret != VISCA_SUCCESS)
        return VISCA_FAILURE;

    return VISCA_SUCCESS;
}

//////////////////////////////////////////////////////////
// 基本类
int visca_set_address(VISCAInterface_t *iface, int *camera_num)
{
    VISCAPacket_t packet;
    int backup;
    VISCACamera_t camera; /* dummy camera struct */

	_LOCK(_visca_lock);

    camera.address=0;
    backup=iface->broadcast;


    _visca_init_packet(&packet);
    _visca_append_byte(&packet,0x30);
    _visca_append_byte(&packet,0x01);

    iface->broadcast=1;

    if (_visca_send_packet(iface, &packet)!=VISCA_SUCCESS)
    {
        iface->broadcast=backup;
        goto err;
    }
    else
        iface->broadcast=backup;

    if (_visca_get_reply(iface)!=VISCA_SUCCESS)
    {
        goto err;
    }
    /* We parse the message from the camera here  */
    /* We expect to receive 4*camera_num bytes,
    every packet should be 88 30 0x FF, x being
    the camera id+1. The number of cams will thus be
    ibuf[bytes-2]-1  */
    if ((iface->bytes & 0x3)!=0) /* check multiple of 4 */
        goto err;
    else
    {
        *camera_num=iface->ibuf[iface->bytes-2]-1;
        if ((*camera_num==0)||(*camera_num>7))
            goto err;
        else
    	{
    		_UNLOCK(_visca_lock);
            return VISCA_SUCCESS;
    	}
    }
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
err:
    *camera_num = -1;
	_UNLOCK(_visca_lock);
    return VISCA_FAILURE;
}

/* note：
此函数即文档上说的IF_Clear，如果是广播(0x88开头命令)，则返回值与发送的一样
如果是普通的命令(0x8x开头命令)，则返回completion，不返回ack
*/
int visca_clear(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet,0x01);
    _visca_append_byte(&packet,0x00);
    _visca_append_byte(&packet,0x01);

    if (_visca_send_packet(iface, &packet)!=VISCA_SUCCESS)
    {
    	_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }
    if (_visca_get_reply(iface)!=VISCA_SUCCESS)
    {
    	_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }

	_UNLOCK(_visca_lock);
	
    return VISCA_SUCCESS;
}

int visca_get_camera_info(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
	
    packet.bytes[0]=0x80 | iface->address;
    packet.bytes[1]=0x09;
    packet.bytes[2]=0x00;
    packet.bytes[3]=0x02;
    packet.bytes[4]=VISCA_TERMINATOR;
    packet.length=5;

    if (_visca_write_packet_data(iface, &packet)!=VISCA_SUCCESS)
    {
        com_print("write packet failed!\n");
		_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }
    if (_visca_get_reply(iface)!=VISCA_SUCCESS)
    {
        com_print("get reply failed!\n");
		_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }

    if (iface->bytes!= 10) /* we expect 10 bytes as answer, just as spec said */
    {
        com_print("%s get bytes != 10!\n", __func__);
		_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }
    else
    {
        camera->vendor=(iface->ibuf[2]<<8) + iface->ibuf[3];
        camera->model=(iface->ibuf[4]<<8) + iface->ibuf[5];
        camera->rom_version=(iface->ibuf[6]<<8) + iface->ibuf[7];
        camera->socket_num=iface->ibuf[8];

		_UNLOCK(_visca_lock);
        return VISCA_SUCCESS;
    }
}

///////////////////////////////////////////////////////////////////////////////
// 命令类
//---------- CAM_Power
// power: 1==on 0 == off(standby)
int visca_set_power(VISCAInterface_t *iface, int power)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x00);
    //2==on 3 == off(standby)
    if (power == 1)
    {
        _visca_append_byte(&packet, 0x02);
    }
    else if (power == 0)
    {
        _visca_append_byte(&packet, 0x03);
    }
    else
    {
    	_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_Initialize
int visca_lens_init(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x19);
    _visca_append_byte(&packet, 0x01);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_camera_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x19);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//---------- CAM_Zoom
int visca_set_zoom_stop(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x07);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_zoom_tele(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x07);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_zoom_wide(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x07);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
// speed: 0(low)~7(high)
int visca_set_zoom_tele_speed(VISCAInterface_t *iface, int speed)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x07);
    _visca_append_byte(&packet, 0x20 | (speed & 0x7));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
// speed: 0(low)~7(high)
int visca_set_zoom_wide_speed(VISCAInterface_t *iface, int speed)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x07);
    _visca_append_byte(&packet, 0x30 | (speed & 0x7));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_zoom_value(VISCAInterface_t *iface, int zoom)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x47);
    _visca_append_byte(&packet, (zoom & 0xF000) >> 12);
    _visca_append_byte(&packet, (zoom & 0x0F00) >>  8);
    _visca_append_byte(&packet, (zoom & 0x00F0) >>  4);
    _visca_append_byte(&packet, (zoom & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_dzoom_off(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x06);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_dzoom_on(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x06);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_ZoomFocus
int visca_set_zoom_and_focus_value(VISCAInterface_t *iface, int zoom, int focus)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x47);
    _visca_append_byte(&packet, (zoom & 0xF000) >> 12);
    _visca_append_byte(&packet, (zoom & 0x0F00) >>  8);
    _visca_append_byte(&packet, (zoom & 0x00F0) >>  4);
    _visca_append_byte(&packet, (zoom & 0x000F));
    _visca_append_byte(&packet, (focus & 0xF000) >> 12);
    _visca_append_byte(&packet, (focus & 0x0F00) >>  8);
    _visca_append_byte(&packet, (focus & 0x00F0) >>  4);
    _visca_append_byte(&packet, (focus & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//---------- CAM_Focus
int visca_set_focus_stop(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x08);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_focus_far(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x08);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_focus_near(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x08);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
// speed: 0(low)~7(high)
int visca_set_focus_far_speed(VISCAInterface_t *iface, int speed)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x08);
    _visca_append_byte(&packet, 0x20 | (speed & 0x7));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

// speed: 0(low)~7(high)
int visca_set_focus_near_speed(VISCAInterface_t *iface, int speed)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x08);
    _visca_append_byte(&packet, 0x30 | (speed & 0x7));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_focus_value(VISCAInterface_t *iface, int focus)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x48);
    _visca_append_byte(&packet, (focus & 0xF000) >> 12);
    _visca_append_byte(&packet, (focus & 0x0F00) >>  8);
    _visca_append_byte(&packet, (focus & 0x00F0) >>  4);
    _visca_append_byte(&packet, (focus & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

// todo power 2==auto 3 = manual 0x10 = auto/manual(?????????)
int visca_set_focus_mode(VISCAInterface_t *iface, int power)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x38);
    _visca_append_byte(&packet, power);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_focus_one_push(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x18);
    _visca_append_byte(&packet, 0x01);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_focus_infinity(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x18);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_focus_near_limit(VISCAInterface_t *iface, int limit)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x28);
    _visca_append_byte(&packet, (limit & 0xF000) >> 12);
    _visca_append_byte(&packet, (limit & 0x0F00) >>  8);
    _visca_append_byte(&packet, (limit & 0x00F0) >>  4);
    _visca_append_byte(&packet, (limit & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}


//----------CAM_Bright
// *ONLY* used in "Bright Mode (Manual control)"
int visca_set_bright_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0d);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_bright_up(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0d);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_bright_down(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0d);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
// value: bright position 0~0xff
int visca_set_bright_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4d);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_SlowShutter
/*
power
0:auto slow shutter off(manual)
1:auto slow shutter on
*/
int visca_set_slowshutter(VISCAInterface_t *iface, int power)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x5a);
    if (power == 0)
        _visca_append_byte(&packet, 0x02);
    else if (power == 1)
        _visca_append_byte(&packet, 0x03);
	
    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_Shutter
// *ONLY* used in "Shutter Priority Automatic Exposure mode" or "Manual Control mode"
int visca_set_shutter_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0a);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_shutter_up(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0a);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_shutter_down(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0a);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

// value: shutter position 0~0x15
int visca_set_shutter_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4a);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_Gain
int visca_set_gain_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0C);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
int visca_set_gain_up(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0C);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_gain_down(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0c);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
// value: gain position 0x0~0xf
int visca_set_gain_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4c);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

// value gain position 0x04~0x0f
int visca_set_gain_limit_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x2c);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//value ON:0x02 OFF:0x03
int visca_set_picture_flip_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x66);
    _visca_append_byte(&packet, (value));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//value ON:0x02 OFF:0x03
int visca_set_lr_reverse_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x61);
    _visca_append_byte(&packet, value);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_RGain
int visca_set_rgain_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x03);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_rgain_up(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x03);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_rgain_down(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x03);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_rgain_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x43);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_BGain
int visca_set_bgain_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x04);
    _visca_append_byte(&packet, 0x00);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_bgain_up(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x04);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_bgain_down(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x04);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_bgain_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x44);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_WB
/* mode
0-Normal Auto
1-Indoor mode
2-Outdoor mode
3-One Push WB mode
4-Auto Tracing White Balance
5-Manual Control mode
6-Outdoor auto
7-Auto including sodium lamp source
8-Sodium lamp source fixed mode
*/
int visca_set_whitebal_mode(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x35);
    _visca_append_byte(&packet, mode);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_whitebal_one_push(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x10);
    _visca_append_byte(&packet, 0x05);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_AE
/*
***
mode
0-Automatic Exposure mode
0x03-Manual Control mode
0x0a-Shutter Priority Automatic Exposure mode
0x0b-Iris Priority Automatic Exposure mode
0x0d-Bright Mode (Manual control)

todo:用个宏会不会好点？
*/
int visca_set_ae_mode(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x39);
    _visca_append_byte(&packet, mode);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_AFMode
/*
0-Normal AF
1-Interval AF
2-Zoom Trigger AF
*/
int visca_set_af_mode(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x57);
    _visca_append_byte(&packet, mode);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

/*
*ONLY* in Interval AF mode!!!
active - 0~0xff
interval - 0~0xff
*/
int visca_set_af_intervaltime(VISCAInterface_t *iface, int active, int interval)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x27);
    _visca_append_byte(&packet, (active & 0x00F0) >> 4);
    _visca_append_byte(&packet, (active & 0x000F));
    _visca_append_byte(&packet, (interval & 0x00F0) >>  4);
    _visca_append_byte(&packet, (interval & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//---------- AF Sensitivity
/*
0-Normal
1-Low
*/
int visca_set_focus_autosense_mode(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x58);
    _visca_append_byte(&packet, mode+0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_Iris
// *ONLY* used in "Iris Priority Automatic Exposure mode"
int visca_set_iris_reset(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0b);
    _visca_append_byte(&packet, 0x00);
  
    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_iris_up(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0b);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_iris_down(VISCAInterface_t *iface)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x0b);
    _visca_append_byte(&packet, 0x03);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

// value: iris postion 0x01~0x0c
int visca_set_iris_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4b);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

/////////////////////////////////////////////////////////
//----------CAM_IRCorrection

//----------CAM_ICR
int visca_set_infraredmode(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;
    int tmp = 0x03; // default off
    if (mode == 1)
        tmp = 0x02;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x01);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_AutoICR
//----------CAM_AutoICRAlarmReply

//----------CAM_Defog
// mode 0 - off leve: 1~4
int visca_set_defog(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x54);
    _visca_append_byte(&packet, mode);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_FlickerCancel

//----------CAM_ExpComp
int visca_set_exposure_compensation_mode(VISCAInterface_t *iface, int mode)
{
	VISCAPacket_t packet;

	int tmp = 0x3;
	if (1 == mode )
	{
		tmp = 0x2;
 	}
	 
	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x3e);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_ExpComp
int visca_set_exposure_compensation_value(VISCAInterface_t *iface, int value)
{
     VISCAPacket_t packet;
	 
	 _LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4e);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}


//----------CAM_BlackLight
//----------CAM_SpotAE
//----------CAM_AE_Response
//----------CAM_WD
int visca_set_wdr(VISCAInterface_t *iface, int wdr)
{
     VISCAPacket_t packet;
	 
	 _LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x3d);
    _visca_append_byte(&packet, wdr&0xF);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_Aperture
int visca_set_aperture(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;
    int tmp = 0x00; // default reset
    if (mode == 1)  // up
        tmp = 0x02;
    else if (mode == 2) // down
        tmp = 0x03;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x02);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_set_aperture_value(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x42);
    _visca_append_byte(&packet, (value & 0xF000) >> 12);
    _visca_append_byte(&packet, (value & 0x0F00) >>  8);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_HR
//----------CAM_NR
int visca_set_nr(VISCAInterface_t *iface, int nr)
{
     VISCAPacket_t packet;
	 
	 _LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x53);
    _visca_append_byte(&packet, nr&0xF);

    int ret = _visca_send_packet_with_reply(iface, &packet);  
	_UNLOCK(_visca_lock);

	return ret; 
}
//----------CAM_Gamma
int visca_set_gamma(VISCAInterface_t *iface, int gamma)
{
     VISCAPacket_t packet;
	 
	 _LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x5b);
    _visca_append_byte(&packet, gamma&0xF);

    int ret = _visca_send_packet_with_reply(iface, &packet);   
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_HighSensitivity
//----------CAM_LR_Reverse
//----------CAM_Freeze
int visca_set_freeze(VISCAInterface_t *iface, int power)
{
    VISCAPacket_t packet;
    int tmp = 0x03; // default: off
    if (power == 1)
        tmp = 0x02;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x62);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_PictureEffect
//----------CAM_PictureFlip
int visca_set_pictureflip(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;
    int tmp = 0x03; // default off
    if (mode == 1)
        tmp = 0x02;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x66);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_HR
int visca_set_hrmode(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;
    int tmp = 0x03; // default off
    if (mode == 1)
        tmp = 0x02;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x52);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}


//----------CAM_Stabilizer
int visca_set_stabilizer(VISCAInterface_t *iface, int mode)
{
    VISCAPacket_t packet;
    int tmp = 0x03; // default off
    if (mode)
        tmp = 0x02;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x34);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}



//----------CAM_Memory
// mode 0-reset 1-rest 2-recall
// channel 1~6
int visca_memory(VISCAInterface_t *iface, int mode, int channel)
{
    VISCAPacket_t packet;
    int tmp = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x3f);
    _visca_append_byte(&packet, mode);
    _visca_append_byte(&packet, (tmp-1)&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_CUSTOM
//----------CAM_MemSave
//----------CAM_PrivacyZone
int visca_privacy_set_mask(VISCAInterface_t *iface, int id, int width, int height, int type)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x76);
    _visca_append_byte(&packet, id);
    _visca_append_byte(&packet, type);
    _visca_append_byte(&packet, (width&0xf0)>>4);
    _visca_append_byte(&packet, width&0x0f);
    _visca_append_byte(&packet, (height&0xf0)>>4);
    _visca_append_byte(&packet, height&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_privacy_set_display(VISCAInterface_t *iface, int id, int color)
{
    int ret = 0;
    VISCAPacket_t packet;
    unsigned char tmp1 = 0;
    unsigned char tmp2 = 0;
    unsigned char tmp3 = 0;
    unsigned char tmp4 = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x77);

    ret = _visca_send_packet_with_reply(iface, &packet);
    if (ret < 0)
    {
    	_UNLOCK(_visca_lock);
        return ret;
    }

    tmp1 = iface->ibuf[2];
    tmp2 = iface->ibuf[3];
    tmp3 = iface->ibuf[4];
    tmp4 = iface->ibuf[5];

    if (id >= 0x00 && id <= 0x05)
    {
        tmp4 |= 1 << id;
    }
    if (id >= 0x06 && id <= 0x0b)
    {
        tmp3 |= 1 << (id+2 - 8);
    }
    if (id >= 0x0c && id <= 0x11)
    {
        tmp2 |= 1 << (id+4 - 16);
    }
    if (id >= 0x12 && id <= 0x17)
    {
        tmp1 |= 1 << (id+6 - 24);
    }

    // set color
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x78);
    _visca_append_byte(&packet, tmp1);
    _visca_append_byte(&packet, tmp2);
    _visca_append_byte(&packet, tmp3);
    _visca_append_byte(&packet, tmp4);
    _visca_append_byte(&packet, 0x00);  // color code(not the mask zone, default to 0)
    _visca_append_byte(&packet, color);  // display color code, see spec
    ret = _visca_send_packet_with_reply(iface, &packet);

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x77);
    _visca_append_byte(&packet, tmp1);
    _visca_append_byte(&packet, tmp2);
    _visca_append_byte(&packet, tmp3);
    _visca_append_byte(&packet, tmp4);
    ret |= _visca_send_packet_with_reply(iface, &packet);
    
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_privacy_clear_display(VISCAInterface_t *iface, int id)
{
    int ret = 0;
    VISCAPacket_t packet;
    unsigned char tmp1 = 0;
    unsigned char tmp2 = 0;
    unsigned char tmp3 = 0;
    unsigned char tmp4 = 0;

	_LOCK(_visca_lock);
    if (id >= 0 && id <= 23)
    {
        _visca_init_packet(&packet);
        _visca_append_byte(&packet, VISCA_INQUIRY);
        _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
        _visca_append_byte(&packet, 0x77);

        ret = _visca_send_packet_with_reply(iface, &packet);
        if (ret < 0)
        {
        	_UNLOCK(_visca_lock);
            return ret;
        }

        tmp1 = iface->ibuf[2];
        tmp2 = iface->ibuf[3];
        tmp3 = iface->ibuf[4];
        tmp4 = iface->ibuf[5];

        if (id >= 0x00 && id <= 0x05)
        {
            tmp4 &= ~(1 << id);
        }
        if (id >= 0x06 && id <= 0x0b)
        {
            tmp3 &= ~(1 << (id+2 - 8));
        }
        if (id >= 0x0c && id <= 0x11)
        {
            tmp2 &= ~(1 << (id+4 - 16));
        }
        if (id >= 0x12 && id <= 0x17)
        {
            tmp1 &= ~(1 << (id+6 - 24));
        }
    }

    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x77);
    _visca_append_byte(&packet, tmp1);
    _visca_append_byte(&packet, tmp2);
    _visca_append_byte(&packet, tmp3);
    _visca_append_byte(&packet, tmp4);
    ret |= _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_privacy_set_pt(VISCAInterface_t *iface, int pan, int tilt)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x79);
    _visca_append_byte(&packet, (pan&0xf00)>>8);
    _visca_append_byte(&packet, (pan&0xf0)>>4);
    _visca_append_byte(&packet, pan&0x0f);
    _visca_append_byte(&packet, (tilt&0xf00)>>8);
    _visca_append_byte(&packet, (tilt&0xf0)>>4);
    _visca_append_byte(&packet, tilt&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_privacy_set_ptz(VISCAInterface_t *iface, int id, int pan, int tilt, int zoom)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x7b);
    _visca_append_byte(&packet, id);
    _visca_append_byte(&packet, (pan&0xf00)>>8);
    _visca_append_byte(&packet, (pan&0xf0)>>4);
    _visca_append_byte(&packet, pan&0x0f);
    _visca_append_byte(&packet, (tilt&0xf00)>>8);
    _visca_append_byte(&packet, (tilt&0xf0)>>4);
    _visca_append_byte(&packet, tilt&0x0f);
    _visca_append_byte(&packet, (zoom&0xf000)>>12);
    _visca_append_byte(&packet, (zoom&0xf00)>>8);
    _visca_append_byte(&packet, (zoom&0xf0)>>4);
    _visca_append_byte(&packet, zoom&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_privacy_set_noninterlock_mask(VISCAInterface_t *iface, int id, int x, int y, int width, int height)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x6f);
    _visca_append_byte(&packet, id);
    _visca_append_byte(&packet, (x&0xf0)>>4);
    _visca_append_byte(&packet, x&0x0f);
    _visca_append_byte(&packet, (y&0xf0)>>4);
    _visca_append_byte(&packet, y&0x0f);
    _visca_append_byte(&packet, (width&0xf0)>>4);
    _visca_append_byte(&packet, width&0x0f);
    _visca_append_byte(&packet, (height&0xf0)>>4);
    _visca_append_byte(&packet, height&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

int visca_privacy_set_grid(VISCAInterface_t *iface, int type)
{
    int tmp = 3;

    VISCAPacket_t packet;
    if (type == 1)  tmp = 2;

	_LOCK(_visca_lock);	
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x7c);
    _visca_append_byte(&packet, tmp);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_IDWrite
int visca_set_camera_id(VISCAInterface_t *iface, int id)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, 0x01);
    _visca_append_byte(&packet, 0x04);
    _visca_append_byte(&packet, 0x22);
    _visca_append_byte(&packet, (id & 0xF000) >> 12);
    _visca_append_byte(&packet, (id & 0x0F00) >>  8);
    _visca_append_byte(&packet, (id & 0x00F0) >>  4);
    _visca_append_byte(&packet, (id & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_Alarm
//----------CAM_RegisterValue
int visca_set_camera_register(VISCAInterface_t *iface, int id, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x24);
    _visca_append_byte(&packet, id);
    _visca_append_byte(&packet, (value & 0x00F0) >>  4);
    _visca_append_byte(&packet, (value & 0x000F));

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_ChromaSuppress
//----------CAM_ColorGain
// value 0~0x0e
int visca_set_color_gain(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x49);
    _visca_append_byte(&packet, 0);
    _visca_append_byte(&packet, 0);
    _visca_append_byte(&packet, 0);
    _visca_append_byte(&packet, value&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_ColorHue
// value 0~0x0e
int visca_set_colorhue(VISCAInterface_t *iface, int value)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_COMMAND);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4f);
    _visca_append_byte(&packet, 0);
    _visca_append_byte(&packet, 0);
    _visca_append_byte(&packet, 0);
    _visca_append_byte(&packet, value&0x0f);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	_UNLOCK(_visca_lock);

	return ret;
}
//----------CAM_Scene Mode
//----------CAM_Display
//----------CAM_MultiLineTitle


///////////////////////////////////////////////////////////////////////////////
// 查询类
//----------CAM_PowerInq
/*
*power: return value 0== power off 1==power on
*/
int visca_get_power(VISCAInterface_t *iface, int *power)
{
    VISCAPacket_t packet;
    int err = 0;
    int tmp = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x00);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
    {
    	_UNLOCK(_visca_lock);
        return err;
    }
    tmp=iface->ibuf[2];
    if (tmp == 0x02)
        *power = 1;
    else if (tmp == 0x03)
        *power = 0;
    else
        *power = -1;
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_ZoomPosInq
int visca_get_zoom_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x47);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return err;
	}

    if (iface->bytes != 7) 
	{
		_UNLOCK(_visca_lock);
		return VISCA_FAILURE;
	}

    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	
	_UNLOCK(_visca_lock);

    return VISCA_SUCCESS;

}
//----------CAM_FocusModeInq
int visca_get_focus_mode(VISCAInterface_t *iface, int *power)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x38);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return err;
	}

    *power=iface->ibuf[2];

	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_FocusPosInq
int visca_get_focus_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x48);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return err;
	}
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_AFSensitivityInq
int visca_get_focus_auto_sense(VISCAInterface_t *iface, int *mode)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x58 );
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}
    *mode=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_FocusNearLimitInq

//----------CAM_AFModeInq
int visca_get_af_mode(VISCAInterface_t *iface, int *mode)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x57);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}
    *mode=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_AFTimeSettingInq
//----------CAM_IRCorrectionInq
//----------CAM_WBModeInq
// mode: see visca_set_whitebal_mode
int visca_get_whitebal_mode(VISCAInterface_t *iface, int *mode)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x35);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}
    *mode=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_RGainInq
int visca_get_rgain_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x43);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_BGainInq
int visca_get_bgain_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x44);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_AEModeInq
// mode: see visca_set_ae_mode
int visca_get_ae_mode(VISCAInterface_t *iface, int *mode)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x39);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *mode=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_ShutterPosInq
int visca_get_shutter_value(VISCAInterface_t *iface, int *shutter)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4a);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *shutter=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_IrisPosInq
int visca_get_iris_value(VISCAInterface_t *iface, int *iris)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4b);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    if (iface->bytes != 7)
    {
        //printf("try again...\n");
        _visca_init_packet(&packet);
        _visca_append_byte(&packet, VISCA_INQUIRY);
        _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
        _visca_append_byte(&packet, 0x4b);
        err=_visca_send_packet_with_reply(iface, &packet);
		if (err!=VISCA_SUCCESS)
		{
			_UNLOCK(_visca_lock);
			return err;
		}
    }

    *iris=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_GainPosInq
int visca_get_gain_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4c);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_GainLimitInq
int visca_get_gain_limit_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x2c);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *value=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_BrightPosInq
//----------CAM_ExpCompModeInq
int visca_get_exposure_compensation_mode(VISCAInterface_t *iface, int* mode)
{
	VISCAPacket_t packet;
	 
	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x3e);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	if (ret!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return ret;
	}

	if (NULL != mode)
	{
		*mode=iface->ibuf[2] == 0x2 ? 1 : 0;
	}

	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_ExpCompPosInq
int visca_get_exposure_compensation_value(VISCAInterface_t *iface, int* value)
{
     VISCAPacket_t packet;
	 
	 _LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x4e);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	if (ret!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return ret;
	}
	
	if (NULL != value)
	{
		*value = (iface->ibuf[2]<<12 | iface->ibuf[3]<<8 | iface->ibuf[4]<<4 | iface->ibuf[5]<<0);
	}
	_UNLOCK(_visca_lock);

	return ret;
}

//----------CAM_BackLightModeInq
//----------CAM_SpotAEModeInq
//----------CAM_SpotAEPosInq
//----------CAM_AE_ResponseInq
//----------CAM_WDModeInq
int visca_get_wdr(VISCAInterface_t *iface, int *wdr)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x3d);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *wdr=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;

}
//----------CAM_WDParameterInq
//----------CAM_ApertureInq
int visca_get_aperture_value(VISCAInterface_t *iface, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x42);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_HRModeInq
//----------CAM_NRModeInq
int visca_get_nr(VISCAInterface_t *iface, int *nr)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x53);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *nr=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_GammaInq
int visca_get_gamma(VISCAInterface_t *iface, int *gamma)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x5b);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *gamma=iface->ibuf[2];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_HighSensitivityInq
//----------CAM_LR_ReverseModeInq
//----------CAM_FreezeModeInq
int visca_get_freeze(VISCAInterface_t *iface, int *power)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x62);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    if (iface->ibuf[2] == 2)
    {
        *power = 1;
    }
    else if (iface->ibuf[2] == 3)
    {
        *power = 0;
    }
    else
    {
        *power = -1;
		_UNLOCK(_visca_lock);
        return VISCA_FAILURE;
    }

	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}

//----------CAM_PictureEffectModeInq
//----------CAM_PictureFlipModeInq
//----------CAM_ICRModeInq
//----------CAM_AutoICRModeInq
//----------CAM_AutoICRThresholdInq
//----------CAM_AutoICRAlarm ReplyInq
//----------CAM_MemoryInq
//----------CAM_MemSaveInq

//----------CAM_PrivacyDisplayInq
int visca_get_privacy_display(VISCAInterface_t *iface, int *value)
{
    int err = 0;
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x77);

    err = _visca_send_packet_with_reply(iface, &packet);
    if (err < 0)
    {
    	_UNLOCK(_visca_lock);
        return err;
    }

    // see spec
    *value = (iface->ibuf[2]<<18)+(iface->ibuf[3]<<12)+(iface->ibuf[4]<<6)+iface->ibuf[5];
	_UNLOCK(_visca_lock);

    return VISCA_SUCCESS;
}

//----------CAM_PrivacyPanTiltInq
int visca_get_privacy_pt(VISCAInterface_t *iface, int *pan, int *tilt)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x79);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *pan=(iface->ibuf[2]<<8)+(iface->ibuf[3]<<4)+iface->ibuf[4];
    *tilt=(iface->ibuf[5]<<8)+(iface->ibuf[6]<<4)+iface->ibuf[7];
	_UNLOCK(_visca_lock);

    return VISCA_SUCCESS;
}

//----------CAM_PrivacyPTZInq
int visca_get_privacy_ptz(VISCAInterface_t *iface, int id, int *pan, int *tilt, int *zoom)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x7b);
    _visca_append_byte(&packet, id);
    err=_visca_send_packet_with_reply(iface, &packet);
	if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
		return err;
	}

    *pan=(iface->ibuf[2]<<8)+(iface->ibuf[3]<<4)+iface->ibuf[4];
    *tilt=(iface->ibuf[5]<<8)+(iface->ibuf[6]<<4)+iface->ibuf[7];
    *zoom=(iface->ibuf[8]<<12)+(iface->ibuf[9]<<8)+(iface->ibuf[10]<<4)+iface->ibuf[11];
	_UNLOCK(_visca_lock);
	
    return VISCA_SUCCESS;
}

//----------CAM_PrivacyMonitorInq
int visca_get_privacy_monitor(VISCAInterface_t *iface, int *value)
{
    int err = 0;
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x6f);

    err = _visca_send_packet_with_reply(iface, &packet);
    if (err < 0)
    {
    	_UNLOCK(_visca_lock);
        return err;
    }

    // see spec
    *value = (iface->ibuf[2]<<18)+(iface->ibuf[3]<<12)+(iface->ibuf[4]<<6)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
	
    return VISCA_SUCCESS;
}
//----------CAM_SpotAEModeInq

//----------CAM_KeyLockInq
//----------CAM_IDInq
//----------CAM_VersionInq
//----------CAM_AlarmInq
//----------CAM_AlarmModeInq
//----------CAM_AlarmDayNightLevelInq
//----------CAM_AlarmDetectLevelInq
//----------CAM_ReplyIntervalTimeInq
//----------CAM_ChromaSuppressInq
//----------CAM_ColorGainInq
int visca_get_color_gain(VISCAInterface_t *iface, int *gain)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x49);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return err;
	}

    *gain=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
	_UNLOCK(_visca_lock);
    return VISCA_SUCCESS;
}
//----------CAM_ColorHueInq
//----------CAM_DefogInq
int visca_get_defog(VISCAInterface_t *iface, int *mode)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x54);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return err;
	}

    *mode=iface->ibuf[2];
	_UNLOCK(_visca_lock);

    return VISCA_SUCCESS;
}
//----------CAM_FlickerCancelInq
//----------CAM_DisplayModeInq

int visca_get_camera_register(VISCAInterface_t *iface, int id, int *value)
{
    VISCAPacket_t packet;
    int err = 0;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x24);
    _visca_append_byte(&packet, id);
    err=_visca_send_packet_with_reply(iface, &packet);
    if (err!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return err;
	}

    *value=(iface->ibuf[2]<<4)+iface->ibuf[3];
	_UNLOCK(_visca_lock);

    return VISCA_SUCCESS; 
}

//----------CAM_HRModeInq
int visca_get_hrmode(VISCAInterface_t *iface, int* mode)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _visca_append_byte(&packet, 0x52);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	if (ret!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return ret;
	}

	if (NULL != mode)
	{
		*mode = iface->ibuf[2] == 0x2 ? 1 : 0;
	}
	_UNLOCK(_visca_lock);

	return ret; 
}

int visca_get_camera_version(VISCAInterface_t *iface, char *cam_version)
{
    VISCAPacket_t packet;

	_LOCK(_visca_lock);
    _visca_init_packet(&packet);
    _visca_append_byte(&packet, VISCA_INQUIRY);
    _visca_append_byte(&packet, VISCA_CATEGORY_INTERFACE);
    _visca_append_byte(&packet, 0x02);

    int ret = _visca_send_packet_with_reply(iface, &packet);
	if (ret!=VISCA_SUCCESS)
	{
		_UNLOCK(_visca_lock);
        return ret;
	}

	if (NULL != cam_version)
	{
		//*mode = iface->ibuf[2] == 0x2 ? 1 : 0;
		swpa_memcpy(cam_version, &iface->ibuf[2], 6);
	}
	_UNLOCK(_visca_lock);

	return ret; 
}
