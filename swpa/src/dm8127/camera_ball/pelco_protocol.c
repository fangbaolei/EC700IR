#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdint.h>

#include "pelco_protocol.h"

// for motion commands
// cmd1
#define FOCUSNEAR       0x01
#define IRISOPEN        0x02
#define IRISCLOSE       0x04
#define CAMERAONOFF     0x08
#define AUTOMANUALSCAN  0x10
#define SENSEBIT        0x80

// cmd2
#define PANRIGHT    0x02
#define PANLEFT     0x04
#define TILTUP      0x08
#define TILTDOWN    0x10
#define ZOOMTELE    0x20
#define ZOOMWIDE    0x40
#define FOCUSFAR    0x80

// Data1
#define PAN_SPEED_MIN 0x01
#define PAN_SPEED_MAX 0x3F

// Data2
#define TILT_SPEED_MIN 0x01
#define TILT_SPEED_MAX 0x3F

// preset
#define PRESET_SET 0x03
#define PRESET_CLEAR 0x05
#define PRESET_CALL 0x07

extern int g_camera_lock;
static int _pelco_lock = 0;


////////////////////////////////////////////////////////////////////

int pelco_open_serial(PELCOInterface_t *iface)
{
    if (iface == NULL)
    {
        com_print("(%s)no PELCOInterface_t here, do not know how to continue.\n", __FILE__);
        return PELCO_FAILURE;
    }

	if (g_camera_lock == 0)
	{
		_CREATE_LOCK(_pelco_lock);
		g_camera_lock = _pelco_lock;
	}
	else
	{
		_pelco_lock = g_camera_lock;
	}
	
	_LOCK(_pelco_lock);
	
    iface->port_fd = com_open_serial(iface->device, 115200, 8, 'N', 1);
    if (iface->port_fd < 0)
    {
        com_print("no %s can init here, do not know how to continue.\n", iface->device);
		_UNLOCK(_pelco_lock);
		_DELETE_LOCK(_pelco_lock);
        return PELCO_FAILURE;  
    }

    iface->bytes = 0;
	
	_UNLOCK(_pelco_lock);

    //com_debug(7, "%s open fd: %d\n", __func__, iface->port_fd);
    return PELCO_SUCCESS;
}

int pelco_close_serial(PELCOInterface_t *iface)
{
    int ret = 0;
	
	_LOCK(_pelco_lock);

    if (iface->device==NULL || iface->port_fd <= 0)
    {
        com_print("(%s) no need to close.\n", __FILE__);
		_UNLOCK(_pelco_lock);
        return PELCO_SUCCESS;  
    }
    ret = com_close_serial(iface->port_fd);
    if (ret < 0)
    {
        com_print("(%s) close %s failed.\n", __FILE__, iface->device);
		_UNLOCK(_pelco_lock);
        return PELCO_FAILURE;  
    }
    iface->port_fd = 0;
	
	_UNLOCK(_pelco_lock);
	_DELETE_LOCK(_pelco_lock);
    
    return PELCO_SUCCESS;
}

int pelco_unread_bytes(PELCOInterface_t *iface, unsigned char *buffer, int *buffer_size)
{
    //_LOCK(_pelco_lock);
	int ret = com_unread_bytes(iface->port_fd, buffer, buffer_size);
	//_UNLOCK(_pelco_lock);
	return ret;
}

// 内部使用
int _pelco_write_packet_data(PELCOInterface_t *iface, PELCOPacket_t *packet)
{
	swpa_thread_sleep_ms(50); //球机命令间至少间隔50ms
	
	//_LOCK(_pelco_lock);
	int ret = com_write_packet_data(iface->port_fd, packet->bytes, packet->length);
	//_UNLOCK(_pelco_lock);
	return ret;
}

int _pelco_get_packet(PELCOInterface_t *iface)
{
    //com_debug(7, "bytes: %d\n", iface->bytes);
    //iface->bytes = 1024;  // todo
    //if (iface->bytes == 0)
    //    iface->bytes = 4;
    //LOCK(_pelco_lock);
	int ret = com_get_packet(iface->port_fd, iface->ibuf, (int*)&iface->bytes);
    //UNLOCK(_pelco_lock);
	return ret;
}

void _pelco_append_byte(PELCOPacket_t *packet, unsigned char byte)
{
    packet->bytes[packet->length]=byte;
    (packet->length)++;
}

void _pelco_init_packet(PELCOPacket_t *packet)
{
    // set it to null
    memset(packet->bytes, '\0', sizeof(packet->bytes));
    // we start writing at byte 2, the first byte and the second will be filled by the
    // packet sending function(_pelco_send_packet). This function will also append a cksm.
    packet->length = 2;
}

// All commands are 7 bytes long in D Protocol
int _pelco_send_packet(PELCOInterface_t *iface, PELCOPacket_t *packet)
{
    int cksm = 0;
    int i = 0;

	//_LOCK(_pelco_lock);
    // check data:
    if (iface->address>0xff)
    {
        com_print("(%s): Invalid header parameters, addr: %d\n",__FILE__, iface->address);
		//_UNLOCK(_pelco_lock);
        return PELCO_FAILURE;
    }

    // build header:
    packet->bytes[0] = 0xff;
    packet->bytes[1] = iface->address;

    // see spec, some cmd has 7 bytes, some has 9 bytes
    for (i = 0; i < packet->length; i++)
    {
        cksm += packet->bytes[1+i];
    }
    cksm  = cksm % 0x100;

    /*
    cksm = (packet->bytes[1] + packet->bytes[2] + packet->bytes[3] + 
            packet->bytes[4] + packet->bytes[5]) % 0x100;
    */
    // append footer(cksm)
    _pelco_append_byte(packet,cksm);

    //return _pelco_write_packet_data(iface,packet);
    
    int ret = _pelco_write_packet_data(iface,packet);
    //com_debug(7, "%s ret: %d\n", __func__, ret);
    //_UNLOCK(_pelco_lock);
    return ret;
}

///////////////////////////
/* protocol D: 3/4 type response
len    
0   For some commands no response is generated
4   This is called the "General Response" and almost all commands generate it.
7   This is called the "Extended Response" and is normally sent as a response when asking for data
18  This is generated by the "Query" command and its variants

*/
int _pelco_get_reply(PELCOInterface_t *iface)
{
    int i = 0;
    int cksm = 0;
    int rcv_cksm = 0;
#ifdef __LL_DEBUG
    return 0;
#endif
    int ret = 0;
    ret = _pelco_get_packet(iface);
    if (ret != PELCO_SUCCESS)
        return ret;

    for (i = 0; i < iface->bytes-2; i++)
    {
        cksm += iface->ibuf[1+i];
    }
    cksm  = cksm % 0x100;

    //com_debug(7,"chsm: 0x%x\n", cksm);

    iface->type=iface->ibuf[1];
    rcv_cksm = iface->ibuf[iface->bytes-1];

    if (iface->type != iface->address)
    {
        return PELCO_FAILURE;
    }
    
    if (cksm != rcv_cksm)
    {
        com_print("%s check sum failed. expect: 0x%x, rcv: 0x%x\n", __func__, cksm, rcv_cksm);
        return PELCO_FAILURE;
    }

    return PELCO_SUCCESS;
}

int _pelco_send_packet_with_reply(PELCOInterface_t *iface, PELCOPacket_t *packet)
{
    int ret = 0;
    ret = _pelco_send_packet(iface,packet);
    if (ret != PELCO_SUCCESS)
        return ret;

    ret = _pelco_get_reply(iface);
    if (ret != PELCO_SUCCESS)
        return ret;

    return PELCO_SUCCESS;
}

int _pelco_send_packet_no_reply(PELCOInterface_t *iface, PELCOPacket_t *packet)
{
    //unsigned char buffer[128] = {0};
    //int len = 64;
    //int ret = 0;
    if (_pelco_send_packet(iface,packet)!=PELCO_SUCCESS)
        return PELCO_FAILURE;
#if 0
    pelco_unread_bytes(iface, buffer, &len);
    printf("unread len: %d\n", len);
    int i = 0;
    for (i = 0; i <len; i++)
    {
        printf("%x ", buffer[i]);
    }
    printf("\n");

    //ret = _pelco_get_reply(iface);
    if (ret != PELCO_SUCCESS && ret != PELCO_TIMEOUT)
        return ret;
#endif
    return PELCO_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// basic command: motion commands: pan, tilt, iris, zoom and focus

// action: 1 = auto scan 0 = manualscan
int pelco_camera_scan(PELCOInterface_t *iface, int action)
{
    PELCOPacket_t packet;
    int tmp = AUTOMANUALSCAN;

	_LOCK(_pelco_lock);

    if (action == 1)
        tmp = AUTOMANUALSCAN | SENSEBIT;   // see spec
    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// action: 1 = on  0 = off
int pelco_camera_switch(PELCOInterface_t *iface, int action)
{
    PELCOPacket_t packet;
    int tmp = CAMERAONOFF;
	
	_LOCK(_pelco_lock);

    if (action == 1)
        tmp = CAMERAONOFF | SENSEBIT;   // see spec
    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

int pelco_camera_stop(PELCOInterface_t *iface)
{
    PELCOPacket_t packet;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// action: 1 = open  0 = close
int pelco_camera_iris_switch(PELCOInterface_t *iface, int action)
{
    PELCOPacket_t packet;
    int tmp = IRISCLOSE;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    if (action == 1)
        tmp = IRISOPEN;
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// action: 0 = near 1 = far
int pelco_camera_focus(PELCOInterface_t *iface, int action)
{
    PELCOPacket_t packet;
    int tmp = FOCUSNEAR;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    if (action == 1)
    {
        tmp = FOCUSFAR;
        _pelco_append_byte(&packet,0x00);
        _pelco_append_byte(&packet,tmp);
    }
    else
    {
        _pelco_append_byte(&packet,tmp);
        _pelco_append_byte(&packet,0x00);

    }
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// action: 0 = zoom tele  1 = zoom wide
int pelco_camera_zoom(PELCOInterface_t *iface, int action)
{
    PELCOPacket_t packet;
    int tmp = ZOOMTELE;

    if (action == 1)
        tmp = ZOOMWIDE;
	
	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// action 0 = down  1 = up
// speed: 1~0x3f
int pelco_camera_tilt(PELCOInterface_t *iface, int action, int speed)
{
    PELCOPacket_t packet;
    int tmp = TILTDOWN;

    //if (speed < TILT_SPEED_MIN) speed = TILT_SPEED_MIN;
    //if (speed > TILT_SPEED_MAX) speed = TILT_SPEED_MAX;
    
    if (action == 1)
        tmp = TILTUP;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,speed);  // tilt in data2

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// action 0 = left  1=right
// speed: 1~0x3f
int pelco_camera_pan(PELCOInterface_t *iface, int action, int speed)
{
    PELCOPacket_t packet;
    int tmp = PANLEFT;
    
    //if (speed < PAN_SPEED_MIN) speed = PAN_SPEED_MIN;
    //if (speed > PAN_SPEED_MAX) speed = PAN_SPEED_MAX;
    
    if (action == 1)
        tmp = PANRIGHT;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,speed);  // pan in data1
    _pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// a little advanced function
// pan_act 0 = left  1=right speed: 1~0x3f
// tilt_act 0 = down  1 = up speed: 1~0x3f
int pelco_camera_pan_tilt(PELCOInterface_t *iface, int pan_act, int pan_speed, int tilt_act, int tilt_speed)
{
    PELCOPacket_t packet;
    int pan_act_tmp = PANLEFT;
    int tilt_act_tmp = TILTDOWN;
    int tmp = 0;

    //if (pan_speed < TILT_SPEED_MIN) pan_speed = PAN_SPEED_MIN;
    //if (pan_speed > TILT_SPEED_MAX) pan_speed = PAN_SPEED_MAX;

    //if (tilt_speed < PAN_SPEED_MIN) tilt_speed = TILT_SPEED_MIN;
    //if (tilt_speed > PAN_SPEED_MAX) tilt_speed = TILT_SPEED_MAX;

    if (pan_act == 1)
        pan_act_tmp = PANRIGHT;
    if (tilt_act == 1)
        tilt_act_tmp = TILTUP;

    tmp = pan_act_tmp | tilt_act_tmp;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,pan_speed);  // pan in data1
    _pelco_append_byte(&packet,tilt_speed); // tilt in data2

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// advanced command : non-motion commands
/* preset
preset_act: 0 = set preset 1 = clear preset 2 = call preset
preset_id 1~255 preset_id 1~255 Preset 0 is invalid
*/
int pelco_camera_preset(PELCOInterface_t *iface, int preset_act, int preset_id)
{
    PELCOPacket_t packet;
    int tmp = PRESET_SET; // default
    
    if (preset_act == 0)
        tmp = PRESET_SET;
    else if (preset_act == 1)
        tmp = PRESET_CLEAR;
    else if (preset_act == 2)
        tmp = PRESET_CALL;
    if (preset_id < 1) preset_id = 1;
    if (preset_id > 255) preset_id = 255;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,preset_id);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

/* pattern
pattern_act: 0 = start  1 = stop  2 = run
pattern_id: ? todo
*/
int pelco_camera_pattern(PELCOInterface_t *iface, int pattern_act, int pattern_id)
{
    PELCOPacket_t packet;
    int tmp = 0x1f; // default
    
    if (pattern_act == 0)
        tmp = 0x1f;
    else if (pattern_act == 1)
        tmp = 0x21;
    else if (pattern_act == 2)
        tmp = 0x23;
    if (pattern_id < 1) pattern_id = 1;
    if (pattern_id > 255) pattern_id = 255;
	
	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,pattern_id);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

/* auxiliary todo: don't know how to do this
auxiliary_act: 0 = set  1 = clear
auxiliary_id: 1~8
*/
int pelco_camera_auxiliary(PELCOInterface_t *iface, int auxiliary_act, int auxiliary_id)
{
    PELCOPacket_t packet;
    int tmp = 0x09; // default
    
    if (auxiliary_act == 0)
        tmp = 0x09;
    else if (auxiliary_act == 1)
        tmp = 0x0b;
    if (auxiliary_id < 1) auxiliary_id = 1;
    if (auxiliary_id > 8) auxiliary_id = 8;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,auxiliary_id);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// Flip (rotate 180)
int pelco_camera_flip(PELCOInterface_t *iface)
{
    PELCOPacket_t packet;
	
	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x07);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x21);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

// Go To Zero Pan
int pelco_camera_zeropan(PELCOInterface_t *iface)
{
    PELCOPacket_t packet;

	_LOCK(_pelco_lock);
	
    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x07);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x22);

    int ret = _pelco_send_packet_no_reply(iface, &packet);

	_UNLOCK(_pelco_lock);
	
	return ret;
}

///////////////////// other cmd

int pelco_camera_set_ptcoordinates(PELCOInterface_t *iface, int pan, int tilt)
{
    PELCOPacket_t packet;
    int tmp_pan_h = 0;
    int tmp_pan_l = 0;
    int tmp_tilt_h = 0;
    int tmp_tilt_l = 0;

    tmp_pan_l = pan&0xff;
    tmp_pan_h = (pan>>8)&0xff;
    
    tmp_tilt_l = tilt&0xff;
    tmp_tilt_h = (tilt>>8)&0xff;

    //com_debug(7, "pan 0x%x tilt 0x%x\n", pan, tilt);
    //com_debug(7, "pan 0x%x 0x%x tilt: 0x%x 0x%x\n", tmp_pan_h, tmp_pan_l, tmp_tilt_h, tmp_tilt_l);
	
	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x53);
    _pelco_append_byte(&packet,tmp_pan_h);
    _pelco_append_byte(&packet,tmp_pan_l);
    _pelco_append_byte(&packet,tmp_tilt_h);
    _pelco_append_byte(&packet,tmp_tilt_l);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

int pelco_camera_get_ptcoordinates(PELCOInterface_t *iface, int* pan, int* tilt)
{
    PELCOPacket_t packet;
    int ret = 0;
    int tmp_pan = 0;
    int tmp_tilt = 0;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x51);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
	
    iface->bytes = 9;
	//    _LOCK(_pelco_lock);
    ret = _pelco_send_packet_with_reply(iface, &packet);
	//    _UNLOCK(_pelco_lock);
    if (ret < 0) 
	{
		_UNLOCK(_pelco_lock);
		return ret;
	}

    tmp_pan = (iface->ibuf[4]<<8) + iface->ibuf[5];
    tmp_tilt = (iface->ibuf[6]<<8) + iface->ibuf[7];
    //com_debug(7, "1111pan: 0x%x(%d) tilt: 0x%x(%d)\n", tmp_pan, tmp_pan, tmp_tilt, tmp_tilt);
    // note：测试发现*有些*值小1，但*有些*值又与实际的相同，误差在1左右。下面的判断也不完全符合。
    if (tmp_pan > 0 && tmp_pan < 3599 && tmp_pan != 900 && tmp_pan != 1800 && tmp_pan != 2700)
        tmp_pan+=1;
    if (tmp_tilt > 0 && tmp_tilt < 900)
        tmp_tilt+=1;
    //com_debug(7, "pan: 0x%x(%d) tilt: 0x%x(%d)\n", tmp_pan, tmp_pan, tmp_tilt, tmp_tilt);
    *pan = tmp_pan;
    *tilt = tmp_tilt;

	_UNLOCK(_pelco_lock);
	
    return PELCO_SUCCESS;
}

/*
红外功率
power_n [in] 近光功率 
power_m [in] 中距离红外光功率
power_f [in] 远光功率
范围值：0~100
not OK
*/
int pelco_camera_set_infraredpower_bad(PELCOInterface_t *iface, int power_n, int power_m, int power_f)
{
    PELCOPacket_t packet;

    if (power_n < 0) power_n = 0;
    if (power_n > 100) power_n = 100;
    if (power_m < 0) power_m = 0;
    if (power_m > 100) power_m = 100;
    if (power_f < 0) power_f = 0;
    if (power_f > 100) power_f = 100;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x3b);
    _pelco_append_byte(&packet,0x08);
    _pelco_append_byte(&packet,power_n);
    _pelco_append_byte(&packet,power_m);
    _pelco_append_byte(&packet,power_f);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

int pelco_camera_set_infraredpower(PELCOInterface_t *iface, int total_zoom, int zoom_1, int zoom_2)
{
    PELCOPacket_t packet;

	_LOCK(_pelco_lock);
	
    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,total_zoom);
    _pelco_append_byte(&packet,0x6d);
    _pelco_append_byte(&packet,zoom_1);
    _pelco_append_byte(&packet,zoom_2);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
	
	return ret;
}

int pelco_camera_factorytest_start(PELCOInterface_t *iface)
{
    int ret = 0;
    PELCOPacket_t packet;
	
	_LOCK(_pelco_lock);
    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x07);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0xcb);

    ret = _pelco_send_packet_no_reply(iface, &packet);
	swpa_thread_sleep_ms(300);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x07);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0xfa);
    
    ret |= _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);
    
    return ret;
}

int pelco_camera_factorytest_stop(PELCOInterface_t *iface)
{
    PELCOPacket_t packet;
	
	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x07);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0xcc);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	_UNLOCK(_pelco_lock);

	return ret;
}

#if 0
// 不使用
/*
读取光敏照度
light_illumination [out] 0~255
*/
int pelco_camera_get_light_illumination(PELCOInterface_t *iface, int* light_illumination)
{
    PELCOPacket_t packet;
    int ret = 0;
    int tmp = 0;

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x3b);
    _pelco_append_byte(&packet,0x10);
    _pelco_append_byte(&packet,0x00);

    iface->bytes = 9;
    ret = _pelco_send_packet_with_reply(iface, &packet);
    
    if (ret < 0) return ret;
    
    tmp = iface->ibuf[5];
    *light_illumination = tmp;

    com_debug(7, "got light: 0x%x(%d)\n", tmp, tmp);

    return PELCO_SUCCESS;
}
#endif
#if 0
//报警功能没有使用
/*
todo
报警输出设置
alarm_act: [in] 0 = off 1 = on
*/
int pelco_camera_set_alarm(PELCOInterface_t *iface, int alarm_act)
{
    PELCOPacket_t packet;
    int tmp = 0; // default

    if (alarm_act == 1)
    {
        tmp = 0x01;
    }
    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x09);
    _pelco_append_byte(&packet,tmp);
    _pelco_append_byte(&packet,0x00);

    return _pelco_send_packet_no_reply(iface, &packet);
}

/*
报警输入查询
alarmin: [out] bit0 = AIN1, bit1 = AIN2, etc
*/
int pelco_camera_get_alarm(PELCOInterface_t *iface, int* alarmin)
{
    PELCOPacket_t packet;
    int ret = 0;

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x19);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);

    iface->bytes = 7;
    ret = _pelco_send_packet_with_reply(iface, &packet);
    
    if (ret < 0) return ret;
    
    *alarmin = iface->ibuf[4];

    com_debug(7, "got alarm in: 0x%x\n", *alarmin);

    return PELCO_SUCCESS;
}
#endif

/*
no support!!!
3D定位
zoom_act 0 = tele(放大) 1 = wide(缩小)
o_x,o_y: 定位范围的中心点坐标
width, height: 范围宽高
说明：当width和height任意一个数据为0时，摄像机将不进行变倍动作
示意图：
(0, 0)                                (100, 0)
+-----------------------------------------
|       |<----width --->|
|    -- +-------------------+
|    |  |                   |
| height|        .(o_x, o_y)|
|    |  |                   |
|   -- -+-------------------+
|
+---------------------------------------
(100, 0)
*/
int pelco_camera_set_3dpos(PELCOInterface_t *iface, int zoom_act, int o_x, int o_y, int width, int height)
{
    PELCOPacket_t packet;
    int tmp_zoom = 0x55;

    if (zoom_act == 1)
    {
        tmp_zoom = 0x57;
    }
	
	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,tmp_zoom);
    _pelco_append_byte(&packet,o_x);
    _pelco_append_byte(&packet,o_y);
    _pelco_append_byte(&packet,width);
    _pelco_append_byte(&packet,height);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}

//////////////////////////////////////////////////

int pelco_camera_loop_test(PELCOInterface_t *iface, char *buffer, int len)
{
    char tmp_buffer[1024] = {0};
    int tmp_len = 0;
	
	_LOCK(_pelco_lock);
    com_write_packet_data(iface->port_fd, (unsigned char*)buffer, len);
    
    tmp_len = 1024;
    com_get_packet(iface->port_fd, (unsigned char*)tmp_buffer, &tmp_len);
    _UNLOCK(_pelco_lock);
    printf("recv len %d : %s\n", tmp_len, tmp_buffer);
    return 0;
}

int pelco_camera_calibration(PELCOInterface_t *iface)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x6F);
    _pelco_append_byte(&packet,0x0);
    _pelco_append_byte(&packet,0x0);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}


int pelco_camera_3d(PELCOInterface_t *iface, int centerx, int centery, int width, int height)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x55); //todo
    _pelco_append_byte(&packet,centerx);
    _pelco_append_byte(&packet,centery);
	_pelco_append_byte(&packet,width);
    _pelco_append_byte(&packet,height);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}



int pelco_camera_set_led_mode(PELCOInterface_t *iface, int mode)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x3B);
    _pelco_append_byte(&packet,0x01);
    _pelco_append_byte(&packet,mode & 0xFF);
	_pelco_append_byte(&packet,0x0);
	_pelco_append_byte(&packet,0x0);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}


int pelco_camera_set_led_power_mode(PELCOInterface_t *iface, int mode)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x3B);
    _pelco_append_byte(&packet,0x03);
    _pelco_append_byte(&packet,mode & 0xFF);
	_pelco_append_byte(&packet,0x0);
	_pelco_append_byte(&packet,0x0);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}



int pelco_camera_set_led_power(PELCOInterface_t *iface, int nearvalue, int mediumvalue, int farvalue)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x3B);
    _pelco_append_byte(&packet,0x04);
    _pelco_append_byte(&packet,nearvalue & 0xFF);
	_pelco_append_byte(&packet,mediumvalue & 0xFF);
	_pelco_append_byte(&packet,farvalue & 0xFF);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}


int pelco_camera_sync_zoom(PELCOInterface_t *iface)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x55);
    _pelco_append_byte(&packet,0x00);
	_pelco_append_byte(&packet,0x00);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}


int pelco_camera_set_privacyzone(PELCOInterface_t *iface, int index, int enable, int width, int height)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x58);
    _pelco_append_byte(&packet,enable);
	_pelco_append_byte(&packet,index);
	_pelco_append_byte(&packet,width);
	_pelco_append_byte(&packet,height);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}



int pelco_camera_set_privacycoord(PELCOInterface_t *iface, int index, int x, int y)
{
	PELCOPacket_t packet;
	
    _LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x59);
	_pelco_append_byte(&packet,0x00);
	_pelco_append_byte(&packet,index);
	_pelco_append_byte(&packet,x);
	_pelco_append_byte(&packet,y);

    int ret = _pelco_send_packet_no_reply(iface, &packet);
	
    _UNLOCK(_pelco_lock);

	return ret;
}


int pelco_camera_get_dome_version(PELCOInterface_t *iface, int *dome_version)
{
    PELCOPacket_t packet;
    int ret = 0;
	int tmp_version = 0;

	_LOCK(_pelco_lock);

    _pelco_init_packet(&packet);
    _pelco_append_byte(&packet,0x40);
    _pelco_append_byte(&packet,0x5A);
    _pelco_append_byte(&packet,0x00);
    _pelco_append_byte(&packet,0x00);
	
    iface->bytes = 7;
	//    _LOCK(_pelco_lock);
    ret = _pelco_send_packet_with_reply(iface, &packet);
	//    _UNLOCK(_pelco_lock);
    if (ret < 0) 
	{
		_UNLOCK(_pelco_lock);
		return ret;
	}

	tmp_version = (iface->ibuf[4] << 8) + iface->ibuf[5];
	*dome_version = tmp_version;

	_UNLOCK(_pelco_lock);
	
    return PELCO_SUCCESS;
}



