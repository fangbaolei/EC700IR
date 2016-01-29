/**
 * @file   pelco_protocol.h
 * @author 
 * @date   Tue Jun 17 09:20:40 2014
 * 
 * @brief  pelco protocol D implement
 * 
 * 
 */

#ifndef __PELCO_PROTOCOL_H
#define __PELCO_PROTOCOL_H

#include <stdint.h>
#include "common_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/// 成功
#define PELCO_SUCCESS       0
/// 失败
#define PELCO_FAILURE       -1
/// 超时
#define PELCO_TIMEOUT       -6
/// 命令执行失败
#define PELCO_CMDERROR      -3

typedef struct _COM_interface PELCOInterface_t;

typedef struct _COM_packet PELCOPacket_t;

///////////////////////////////////////////////////////////////////////////////
/** 
 * 初始化球机串口
 * 
 * @param iface [in] 串口协议接口参数
 * 
 * @return 成功：0  失败：-1
 *
 * @note 需要对PELCOInterface_t进行赋值
 */
int pelco_open_serial(PELCOInterface_t *iface);
/** 
 * 关闭球机串口
 * 
 * @param iface [in] 串口协议接口参数
 * 
 * @return 成功：0  失败：-1
 */
int pelco_close_serial(PELCOInterface_t *iface);
/** 
 * 读取串口缓冲区剩余的数据
 * 
 * @param iface [in] 串口协议接口参数
 * @param buffer [out] 缓冲区
 * @param buffer_size [out] 缓冲区大小
 * 
 * @return 成功：0  失败：-1
 * @note 本函数可防止下次串口通信时因缓冲区有遗留数据而导致错误的现象发生，
 */
int pelco_unread_bytes(PELCOInterface_t *iface, unsigned char *buffer, int *buffer_size);

///////////////////////////////////////////////////////////////////////////////
/** 
 * 停止球机动作
 * 
 * @param iface [in] 串口协议接口参数
 * 
 * @return 成功：0  失败：-1 超时：-6
 * @note 每种动作，都要调用该函数来停止，否则，球机将一直执行(如果设置了水平旋转，则会一直转)。
 */
int pelco_camera_stop(PELCOInterface_t *iface);

/** 
 * 球机垂直移动
 * 
 * @param iface [in] 串口协议接口参数
 * @param action [in] 垂直移动操作 0 = down  1 = up
 * @param speed [in]  移动速度：1~0x3f
 * 
 * @return 成功：0  失败：-1 超时：-6
 */
int pelco_camera_tilt(PELCOInterface_t *iface, int action, int speed);

/** 
 * 球机水平移动
 * 
 * @param iface [in] 串口协议接口参数
 * @param action [in] 水平旋转操作 0 = left  1 = right
 * @param speed [in]  移动速度：1~0x3f
 * 
 * @return 成功：0  失败：-1 超时：-6
 */
int pelco_camera_pan(PELCOInterface_t *iface, int action, int speed);

// a little advanced function
/** 
 * 球机同时水平、垂直移动
 * 
 * @param iface [in] 串口协议接口参数
 * @param pan_act [in] 水平旋转操作 0 = left  1 = right
 * @param pan_speed [in] 移动速度：1~0x3f
 * @param tilt_act [in] 垂直移动操作 0 = down  1 = up
 * @param tilt_speed [in] 移动速度：1~0x3f
 * 
 * @return 成功：0  失败：-1 超时：-6
 * @note 该函数是pelco_camera_pan和pelco_camera_tilt的混合形式
 */
int pelco_camera_pan_tilt(PELCOInterface_t *iface, int pan_act, int pan_speed, int tilt_act, int tilt_speed);
///////////////////////////////////////////////////////////////////////////////

/** 
 * 预置位相关函数
 * 
 * @param iface [in] 串口协议接口参数
 * @param preset_act [in] 预置位操作 0：设置 1：清除 2：调用
 * @param preset_id [in] 1~255 第0号无效
 * 
 * @return 成功：0  失败：-1 超时：-6
 * @todo 球机有部分预置位是已经被占用了，但目前只有部分预置位的文档，无法知道是否有其它保留的预置位。
 */
int pelco_camera_preset(PELCOInterface_t *iface, int preset_act, int preset_id);

/** 
 * 设置球机PT坐标
 * 
 * @param iface [in] 串口协议接口参数
 * @param pan [in] 0~3599(0~359.9度)
 * @param tilt [in] 0~900(0度~90度)
 * 
 * @return 成功：0  失败：-1 超时：-6
 * @note PT即水平(pan)、垂直(tilt)的坐标。水平范围只到3599，设置3600与设置0等效
 */
int pelco_camera_set_ptcoordinates(PELCOInterface_t *iface, int pan, int tilt);

/** 
 * 读取球机PT坐标
 * 
 * @param iface [in] 串口协议接口参数
 * @param pan [out] 0~3599(0~359.9度)
 * @param tilt [out] 0~900(0度~90度)
 * 
 * @return 成功：0  失败：-1 超时：-6
 */
int pelco_camera_get_ptcoordinates(PELCOInterface_t *iface, int* pan, int* tilt);

int pelco_camera_set_infraredpower(PELCOInterface_t *iface, int total_zoom, int zoom_1, int zoom_2);

int pelco_camera_factorytest_start(PELCOInterface_t *iface);

int pelco_camera_factorytest_stop(PELCOInterface_t *iface);

///////////////////////////////////////////////////////////////////////////////
//以下命令是无法确定的或不支持的
///////////////////////////////////////////////////////////////////////////////
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
//int pelco_camera_set_3dpos(PELCOInterface_t *iface, int zoom_act, int o_x, int o_y, int width, int height);

// action: 1 = auto scan 0 = manualscan
int pelco_camera_scan(PELCOInterface_t *iface, int action);
// action: 1 = on  0 = off
int pelco_camera_switch(PELCOInterface_t *iface, int action);
// action: 1 = open  0 = close
int pelco_camera_iris_switch(PELCOInterface_t *iface, int action);
// action: 0 = near 1 = far
int pelco_camera_focus(PELCOInterface_t *iface, int action);
// action: 0 = zoom tele  1 = zoom wide
int pelco_camera_zoom(PELCOInterface_t *iface, int action);

/* pattern
pattern_act: 0 = start  1 = stop  2 = run
pattern_id: ? todo
*/
int pelco_camera_pattern(PELCOInterface_t *iface, int pattern_act, int pattern_id);

///////////////////////////////////////////////////////

int pelco_camera_loop_test(PELCOInterface_t *iface, char *buffer, int len);

int pelco_camera_calibration(PELCOInterface_t *iface);

int pelco_camera_3d(PELCOInterface_t *iface, int centerx, int centery, int width, int height);

int pelco_camera_set_led_mode(PELCOInterface_t *iface, int mode);

int pelco_camera_set_led_power_mode(PELCOInterface_t *iface, int mode);

int pelco_camera_set_led_power(PELCOInterface_t *iface, int nearvalue, int mediumvalue, int farvalue);

int pelco_camera_sync_zoom(PELCOInterface_t *iface);

int pelco_camera_set_privacyzone(PELCOInterface_t *iface, int index, int enable, int width, int height);

int pelco_camera_set_privacycoord(PELCOInterface_t *iface, int index, int x, int y);
int pelco_camera_get_dome_version(PELCOInterface_t *iface, int *dome_version);

#ifdef __cplusplus
};
#endif

#endif /* __PELCO_PROTOCOL_H */
