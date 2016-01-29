/**  
* @file  swpa_camera.h   
* @brief  相机相关
* @author luoqz@signalway.com.cn
* @date  2013-2-21
* @version  V0.1
* @note 由于内部实现的精度与接口不一样，因此一些接口获取到的值与设置时传入的值可能有偏差，或是获取到的值与实际生效值可能有偏差
* @see 《水星平台FPGA寄存器说明文档》
*
* @log 2014-07 lijj&lujy
*       新加木星平台的球机控制、机芯控制的API接口。
*       1、球机使用串口ttyS4通信，机芯使用ttyS3通信，在使用本文件API时，
*          必须调用swpa_camera_init和swpa_camera_deinit进行初始化和退出操作。
*          由于串口不是共享设备，如果在其它模块和进程中使用，不要调用这两个函数！！
*       2、使用方式：
          swpa_camera_init();   // 先初始化
          // 调用其它API
          swpa_camera_init_lens();  // 初始化镜头
          swpa_camera_ball_set_pt_coordinates(100, 100); // 将球机定位到(100,100)坐标(PT坐标)
          swpa_camera_basicparam_set_zoom_value(1000);   // 设置变倍数值(Z)
          // 
          swpa_camera_deinit(); // 退出
*/

/**
 @todo
垂直转动范围，球机技术支持说可以达到-10~90，但目前得到的球机不支持，
只达到0~90，目前的程序按实际值来注释。

红外功率使用变倍来实现，后面厂家可能开放预置位设置的方式，不再使用变倍。
*/
 
////返回值定义 TODO: 自测时用，正式发布时直接引用swpa.h即可
/////成功
//#define SWPAR_OK 				( 0)
/////失败
//#define SWPAR_FAIL 				(-1)
/////非法参数
//#define SWPAR_INVALIDARG		(-2)
/////没有实现
//#define SWPAR_NOTIMPL			(-3)
/////内存不足
//#define SWPAR_OUTOFMEMORY 		(-4)
/////没有初始化
//#define SWPAR_NOTINITED			(-5)

#ifndef _SWPA_CAMERA_H_
#define _SWPA_CAMERA_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum PROTOCOL_TYPE_e
{
    PROTOCOL_VISCA = 0,
    PROTOCOL_PELCO
}PROTOCOL_TYPE;

/*
* 相机参数模式
*/
// AE模式
typedef enum AE_MODE_e
{
  AE_FULL_AUTO = 0x0,   // note：这里的值不要随意改动！
  AE_MANUAL = 0x3,
  AE_SHUTTER_PRIORITY = 0xa,
  AE_IRIS_PRIORITY = 0xb
}AE_MODE;

// AF模式
typedef enum AF_MODE_e
{
  AF_AUTO = 0x2,    // note：这里的值不要随意改动！
  AF_MANUAL = 0x3
}AF_MODE;

// 对焦模式
typedef enum FOCUS_MODE_e
{
    FOCUS_NEAR = 0,  // 近焦
    FOCUS_FAR,       // 远焦
    FOCUS_ONE_PUSH,  // 触发对焦
    FOCUS_INFINITY, // 无穷远对焦
    FOCUS_STOP
}FOCUS_MODE;

// 白平衡模式
typedef enum AWB_MODE_e
{
  AWB_AUTO = 0,
  AWB_INDOOR,
  AWB_OUTDOR,
  AWB_ONE_PUSH_WB,
  AWB_ATW,
  AWB_MANUAL,
  AWB_OUTDOOR_AUTO,
  AWB_SODIUM_LAMP_AUTO,
  AWB_SODIUM_LAMP
}AWB_MODE;

// WDR模式
typedef enum WDR_MODE_e
{
	WDR_ON = 0x2,   // note：这里的值不要随意改动！
	WDR_OFF = 0x3
}WDR_MODE;

// 变焦模式
typedef enum ZOOM_MODE_e
{
	ZOOM_TELE = 0,
	ZOOM_WIDE,
    ZOOM_STOP,
}ZOOM_MODE;

// 光圈模式
typedef enum IRIS_MODE_e
{
	IRIS_RST = 0,   // 复位
	IRIS_UP,        // 放大
    IRIS_DOWN,      // 缩小
}IRIS_MODE;

// 机芯输出模式
typedef enum MONITOR_MODE_e
{
	MODE_1080I_60 = 0x01, // 输出帧率为30fps，此为机芯默认值(木星不适用！)
    MODE_1080P_30 = 0x06, // 木星使用
    MODE_1080P_25 = 0x08, // 木星使用
    MODE_720P_60 = 0x09,
    MODE_720P_30 = 0x0e,
    MODE_720P_25 = 0x11,
}MONITOR_MODE;

// 水平转动模式
typedef enum PAN_TYPE_t
{
    PAN_LEFT = 0,
    PAN_RIGHT,
}PAN_TYPE;

// 垂直转动模式
typedef enum TILT_TYPE_t
{
    TILT_DOWN = 0,
    TILT_UP,
}TILT_TYPE;

// 预置位操作
typedef enum PRESET_TYPE_t
{
    PRESET_SET = 0,
    PRESET_CLEAR,
    PRESET_CALL,
}PRESET_TYPE;

// 扫描边界
typedef enum SCAN_BOUND_t
{
    SCAN_BOUND_LEFT = 0,
    SCAN_BOUND_RIGHT,
}SCAN_BOUND;

/**
***********************
* 相机初始化 
* 注：初始化相关接口，只有木星平台才有
***********************
*/


/**
* @brief 初始化相机控制\n
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 球机使用串口ttyS4通信，机芯使用ttyS3通信，该函数对串口进行初始化。
*       ！！！重要！！！：如果有其它设备要使用ttyS3、ttyS4来传输数据，请不要调用本函数！！！
*/
int swpa_camera_init(void);

/**
* @brief 去初始化相机控制\n
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 球机使用串口ttyS4通信，机芯使用ttyS3通信，该函数对串口进行关闭操作。
*       ！！！重要！！！：配合swpa_camara_init函数使用
*/
int swpa_camera_deinit(void);

/////////////////////////////////////////////////////////////////////////////
/**
* @brief 设置相机上/断电\n
*
* @param [in] power 0-断电(standby模式) 1-上电
* @retval 0 : 执行成功
* @retval -1 : 执行失败
*/
int swpa_camera_set_power(int power);

/**
* @brief 获取相机上/断电状态\n
*
* @param [out] power 0-断电(standby模式) 1-上电 -1：出错了
* @retval 0 : 执行成功
* @retval -1 : 执行失败
*/
int swpa_camera_get_power(int *power);

/**
* @brief 初始化镜头\n
*
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 进行远近对焦，然后对焦到上一次的位置
*/
int swpa_camera_init_lens(void);

/**
* @brief 复位相机\n
*
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 复位时间约12秒(6500型号)
*/
int swpa_camera_reset(void);

/////////////////////////////////////////////////////////////////////////////
/**
* @brief 获取机芯版本号\n
*
* @param [out] vendor 厂商 (0x20表示是sony)
* @param [out] model 模块 (0x045f表示FCB-EH6300，0x0446表示FCB-EH6500，0x2203表示PE2203)
* @param [out] rom_version 固件版本号
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 输出的均是数值，非字符串
*/
int swpa_camera_get_version(unsigned int *vendor, unsigned int *model, unsigned int *rom_version);

/**
* @brief 设置机芯输出分辨率、帧率\n
*
* @param [in] mode 模式，参考MONITOR_MODE
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 设置好，要调用swpa_camera_reset来复位才能生效，当前是MODE_1080P_30、MODE_1080P_25才生效
*/
int swpa_camera_set_monitoring_mode(MONITOR_MODE mode);

/**
* @brief 获取机芯输出分辨率、帧率\n
*
* @param [out] mode 模式，参考MONITOR_MODE
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 获取当前机芯的分辨率、帧率模式
*/
int swpa_camera_get_monitoring_mode(MONITOR_MODE *mode);

/**
* @brief 设置机芯同步方式\n
*
* @param [in] mode 模式，0-sav/eav off - sav/eav on
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 测试6500型号机芯，发现不用复位相机也可以正常
*       木星必须使用模式0，即sav/eav off
*/
int swpa_camera_set_sync_mode(int mode);

/**
* @brief 获取机芯同步方式\n
*
* @param [out] mode 模式，0-sav/eav off - sav/eav on
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 使用本函数获取同步方式，如果不为0，则要调用swpa_camera_set_sync_mode设置为0
*/
int swpa_camera_get_sync_mode(int* mode);

///////////////////////////////////////////////////////////////////////////////
/**
***********************
* 相机相关的转换
* swpa_camera_conver
***********************
*/

/**
* @brief 快门转换\n
*
* 
* @param [in] shutter 快门值，实际写在FPGA里面的值。
* @retval 返回实际的快门值，单位微秒。
* @note 木星不使用
* @see .
*/
int swpa_camera_conver_shutter(
	int shutter
);

/**
* @brief 增益转换\n
*
* 
* @param [in] gain 增益值，实际写在FPGA里面的值。
* @retval 返回实际的增益值，单位0.1db。
* @note 木星不使用
* @see .
*/
int swpa_camera_conver_gain(
	int gain
);

/**
* @brief RGB增益转换\n
* @param [in] rgb_gain rgb增益值，实际写在FPGA里面的值。
* @retval 返回实际的RGB增益值.
* @note 木星不使用
* @see .
*/
int swpa_camera_conver_rgb_gain(
	int rgb_gain
	);


/**
***********************
* 相机基本控制
* swpa_camera_basicparam_
***********************
*/

/**
* @brief 设置快门\n
*
* 
* @param [in] shutter 快门值，单位微秒，200w/280w范围0-30000，500w范围0-54000，木星球机范围0x0 - 0x15
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 由于内部实现的精度比此接口低，因此获取到的值与设置时传入的值可能有偏差
*       木星：shutter实际上是快门值索引，该值越小，越亮
* @see .
*/
int swpa_camera_basicparam_set_shutter(int shutter);

/**
* @brief 获取快门\n
*
* 
* @param [out] shutter 快门值，单位微秒，200w/280w范围0-30000，500w范围0-54000
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 由于内部实现的精度比此接口低，因此获取到的值与设置时传入的值可能有偏差
* @see .
*/
int swpa_camera_basicparam_get_shutter(int *shutter);

/**
* @brief 设置增益\n
*
* 
* @param [in] gain 增益值，单位0.1db，范围0-360。木星球机范围:0x0 - 0xf
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see swpa_camera_init_set_gain_correction
*/
int swpa_camera_basicparam_set_gain(int gain);

/**
* @brief 获取增益\n
*
* 
* @param [out] gain 增益值，单位0.1db，范围0-360
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_get_gain(int *gain);

/**
* @brief 设置增益限制值\n
* @param [in] gain 增益值，木星球机范围:0x04 - 0xf
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 0x4 = +6dB 0x5 = +8dB ... 0xf = +28dB
*
*/
int swpa_camera_basicparam_set_gain_limit(int gain);

/**
* @brief 获取增益限制值\n
* @param [out] gain 增益值，木星球机范围:0x04 - 0xf
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 0x4 = +6dB 0x5 = +8dB ... 0xf = +28dB
*
*/
int swpa_camera_basicparam_get_gain_limit(int *gain);

/**
* @brief 设置RGB增益\n
*
* 
* @param [in] gain_r R增益，单位0.07db，范围0-255，木星球机范围:0x00 - 0xff
* @param [in] gain_g G增益，单位0.07db，范围0-255，木星球机不能设置G增益!!
* @param [in] gain_b B增益，单位0.07db，范围0-255，木星球机范围:0x00 - 0xff
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_set_rgb_gain(
	int gain_r,
	int gain_g,
	int gain_b
); 

/**
* @brief 获取RGB增益\n
*
* 
* @param [in] gain_r R增益，单位0.07db，范围0-255
* @param [in] gain_g G增益，单位0.07db，范围0-255
* @param [in] gain_b B增益，单位0.07db，范围0-255
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_get_rgb_gain(
	int * gain_r,
	int * gain_g,
	int * gain_b
); 

/**
* @brief 设置黑电平\n
*
* 
* @param [in] value1 通道1的值，范围0-1023
* @param [in] value2 通道2的值，范围0-1023
* @param [in] value3 通道3的值，范围0-1023
* @param [in] value4 通道4的值，范围0-1023
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 当前设备具备的通道数可通过swpa_camera_init_get_ad_count查询
*       木星不使用
* @see .
*/
int swpa_camera_basicparam_set_clamp_level(
	int value1,
	int value2,
	int value3,
	int value4
); 

/**
* @brief 获取黑电平\n
*
* 
* @param [out] value1 通道1的值，范围0-1023
* @param [out] value2 通道2的值，范围0-1023
* @param [out] value3 通道3的值，范围0-1023
* @param [out] value4 通道4的值，范围0-1023
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 当前设备具备的通道数可通过swpa_camera_init_get_ad_count查询
*       木星不使用
* @see .
*/
int swpa_camera_basicparam_get_clamp_level(
	int * value1,
	int * value2,
	int * value3,
	int * value4
); 

/**
* @brief 设置抓拍的快门\n
*
* 
* @param [in] shutter 快门值，单位微秒，200w/280w范围0-30000，500w范围0-54000，-1表示与非抓拍取相同值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_set_capture_shutter(
	int shutter
);

/**
* @brief 获取抓拍的快门\n
*
* 
* @param [out] shutter 快门值，单位微秒，200w/280w范围0-30000，500w范围0-54000，-1表示与非抓拍取相同值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_get_capture_shutter(
	int * shutter
);

/**
* @brief 设置抓拍的增益\n
*
* 
* @param [in] gain 增益值，单位0.1db，范围0-360，-1表示与非抓拍取相同值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see swpa_camera_init_set_capture_gain_correction
*/
int swpa_camera_basicparam_set_capture_gain(
	int gain
);

/**
* @brief 获取抓拍的增益\n
*
* 
* @param [out] gain 增益值，单位0.1db，范围0-360，-1表示与非抓拍取相同值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_get_capture_gain(
	int * gain
);

/**
* @brief 设置抓拍的RGB增益\n
*
* 
* @param [in] gain_r R增益，单位0.07db，范围0-255，-1表示与非抓拍取相同值
* @param [in] gain_g G增益，单位0.07db，范围0-255，-1表示与非抓拍取相同值
* @param [in] gain_b B增益，单位0.07db，范围0-255，-1表示与非抓拍取相同值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @retval -2 : 参数非法
* @note RGB三个参数只要有一个为-1，则设置为与非抓拍取相同值
* @see .
*/
int swpa_camera_basicparam_set_capture_rgb_gain(
	int gain_r,
	int gain_g,
	int gain_b
);

/**
* @brief 获取抓拍的RGB增益\n
*
* 
* @param [in] gain_r R增益，单位0.07db，范围0-255，-1表示与非抓拍取相同值
* @param [in] gain_g G增益，单位0.07db，范围0-255，-1表示与非抓拍取相同值
* @param [in] gain_b B增益，单位0.07db，范围0-255，-1表示与非抓拍取相同值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_basicparam_get_capture_rgb_gain(
	int * gain_r,
	int * gain_g,
	int * gain_b
);

/**
* @brief 设置抓拍黑电平\n
*
* 
* @param [in] value1 通道1的值，范围0-1023
* @param [in] value2 通道2的值，范围0-1023
* @param [in] value3 通道3的值，范围0-1023
* @param [in] value4 通道4的值，范围0-1023
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 当前设备具备的通道数可通过swpa_camera_init_get_ad_count查询
* @see .
*/
int swpa_camera_basicparam_set_capture_clamp_level(
	int value1,
	int value2,
	int value3,
	int value4
); 

/**
* @brief 获取抓拍黑电平\n
*
* 
* @param [out] value1 通道1的值，范围0-1023
* @param [out] value2 通道2的值，范围0-1023
* @param [out] value3 通道3的值，范围0-1023
* @param [out] value4 通道4的值，范围0-1023
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 当前设备具备的通道数可通过swpa_camera_init_get_ad_count查询
* @see .
*/
int swpa_camera_basicparam_get_capture_clamp_level(
	int * value1,
	int * value2,
	int * value3,
	int * value4
); 

/**
* @brief 设置曝光模式\n
* @param [in] mode :AE 模式，参见AE_MODE定义
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_basicparam_set_AE(AE_MODE mode);

/**
* @brief 获取曝光模式\n
* @param [out] mode :AE 模式，参见AE_MODE定义
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_basicparam_get_AE(AE_MODE *mode);

/**
* @brief 设置对焦模式\n
* @param [in] mode : AF 模式，参见AF_MODE定义
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_basicparam_set_AF(AF_MODE mode);

/**
* @brief 获取对焦模式\n
* @param [out] mode :AF 模式，参见AF_MODE定义
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_basicparam_get_AF(AF_MODE *mode);

/**
* @brief 设置对焦模式\n
* @param [in] mode : 对焦模式
* @retval 
* 0 - 成功
* -1 - 失败
* @note 本函数设置远近对焦，请在手动对焦模式下设置
*       注意，本函数将将几个功能集合在一起，无对应的获取模式命令
*       木星使用
*/
int swpa_camera_basicparam_set_focus(FOCUS_MODE mode);

/**
* @brief 设置对焦模式\n
* @param [int] mode : 对焦模式
* @param [int] speed: 对焦速度 范围：0x0~0x07
* @retval 
* 0 - 成功
* -1 - 失败
* @note 本函数只设置远近对焦，请在手动对焦模式下设置
        本函数与swpa_camera_basicparam_set_focus相似，但多了速度控制
*       木星使用
*/
int swpa_camera_basicparam_set_focus_speed(FOCUS_MODE mode, int speed);

/**
* @brief 设置对焦数值\n
* @param [int] value : 对焦数值 范围：0x1000 - 0xd000
* @retval 
* 0 - 成功
* -1 - 失败
* @note 本函数设置远近对焦，请在手动对焦模式下设置
        0x1000表示近端，0xd000表示远端，手册上值范围到0xc000，实际测试为0xd000
*       木星使用
*/
int swpa_camera_basicparam_set_focus_value(int value);

/**
* @brief 获取对焦数值\n
* @param [out] value : 对焦数值 范围：0x1000 - 0xd000
* @retval 
* 0 - 成功
* -1 - 失败
* @note 0x1000表示近端，0xd000表示远端，手册上值范围到0xc000，实际测试为0xd000
*/
int swpa_camera_basicparam_get_focus_value(int *value);


/**
* @brief 设置相机红外模式\n
* @param [in] mode ： 红外模式: 0 - Off; 1 - On
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
*/
int swpa_camera_basicparam_set_infrared_mode(int value);


/**
* @brief 设置自动白平衡\n
* @param [in] mode ： 白平衡模式
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
*/
int swpa_camera_basicparam_set_AWB(AWB_MODE mode);

/**
* @brief 获取白平衡模式\n
* @param [out] mode :AWB 模式
* @retval 
* 0 - 成功
* -1 - 失败
* @note
*/
int swpa_camera_basicparam_get_AWB(AWB_MODE *mode);

/**
* @brief 设置光圈模式\n
* @param [in] mode
* @retval 
* 0 - 成功
* -1 - 失败
* @note *  ！！！在AE模式为手动或光圈优先情况下才能调节光圈！！
*          后同
*/
int swpa_camera_basicparam_set_iris_mode(IRIS_MODE mode);

/**
* @brief 设置光圈\n
* @param [in] value
* @retval 
* 0 - 成功
* -1 - 失败
* @note 光圈值范围0x0~0x11，值越大，光圈越大。0表示关闭光圈
*/
int swpa_camera_basicparam_set_iris(int iris);

/**
* @brief 获取光圈\n
* @param [out] value
* @retval 
* 0 - 成功
* -1 - 失败
* @note
*/
int swpa_camera_basicparam_get_iris(int *iris);

/**
* @brief 设置变倍\n
* @param [in] mode
* @retval 
* 0 - 成功
* -1 - 失败
* @note 本函数分别有放大、缩小、停止三个功能，
*       注意，本函数将将几个功能集合在一起，无对应的获取模式命令
*       木星使用
*/
int swpa_camera_basicparam_set_zoom_mode(ZOOM_MODE mode);

/**
* @brief 设置变倍类型及其速度\n
* @param [in] mode 见ZOOM_MODE枚举
* @param [in] speed 速度，0~7，值越高速度越快
* @retval 
* 0 - 成功
* -1 - 失败
* @note 功能同swpa_camera_basicparam_set_zoom_mode，但多了速度的控制，
        本函数无STOP说法，调用STOP无效果
*       木星使用
*/
int swpa_camera_basicparam_set_zoom_speed(ZOOM_MODE mode, int speed);

/**
* @brief 设置变倍数值\n
* @param [in] value 0~0x4000
* @retval 
* 0 - 成功
* -1 - 失败
* @note 本函数直接调用变倍的数值，注意，木星上不使用数字变倍，最大值为0x4000
*       木星使用
*/
int swpa_camera_basicparam_set_zoom_value(int value);

/**
* @brief 获取变倍\n
* @param [out] value 0~0x4000
* @retval 
* 0 - 成功
* -1 - 失败
*/
int swpa_camera_basicparam_get_zoom_value(int *value);

/**
* @brief 使能/禁止数字变焦\n
* @param [in] enable 1：使能 0：禁止
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
*/
int swpa_camera_basicparam_dzoom_enable(int enable);

/**
* @brief 冻结图像\n
* @param [in] mode 0-不冻结图像 1-冻结图像
* @note 木星使用
* @retval 
* 0 - 成功
* -1 - 失败
*/
int swpa_camera_basicparam_set_freeze(int mode);

/**
* @brief 获取冻结图像\n
* @param [in] mode 0-不冻结图像 1-冻结图像
* @note 木星使用
* @retval 
* 0 - 成功
* -1 - 失败
*/
int swpa_camera_basicparam_get_freeze(int *mode);

////////////
/**
* @brief 设置隐私遮蔽区域大小\n
* @param [in] id 区域id号，范围0~23
* @param [in] half_width  隐私区域宽的一半数值
* @param [in] half_height 隐私区域高的一半数值
* @param [in] is_new      是否属于新建区域 范围：1：新建一个区域 0：已有区域
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
*       ！！！重要：所有和隐私遮蔽有关的设置命令，均不能保存在机芯中，
*             机芯断电后会无效，要再次进行设置 ！！！
*
*       is_new参数说明：如果为0，表示在原有区域ID基础上更改区域大小(坐标不改变)，
*       如果为1，则会回到当前图像中心点并建立区域
*       注意：swpa_camera_basicparam_set_ptz是会设置ID的
*
*/
int swpa_camera_basicparam_set_mask(int id, int half_width, int half_height, int is_new);

/**
* @brief 设置隐私遮蔽坐标、区域大小\n
* @param [in] id 区域id号，范围0~23
* @param [in] center_x    隐私区域的中心点X坐标
* @param [in] center_y    隐私区域的中心点Y坐标
* @param [in] half_width  隐私区域宽的一半数值
* @param [in] half_height 隐私区域高的一半数值
* 
* @retval 
* 0 - 成功
-1 - 失败
* @note 木星使用
*       ！！！重要：本函数设置的区域不随球机转动而改变！！！
示意图：

+---------------------------------------------------
|                 |<----half_width*2 --->|
|              -- +----------------------+
|              |  |                      |
|    half_height*2|         .(o_x, o_y)  |
|              |  |                      |
|             -- -+----------------------+
|
+---------------------------------------------------
o_x=center_x
o_y=center_y
*/
int swpa_camera_basicparam_set_nolock_mask(int id, int center_x, int center_y, int half_width, int half_height);

/**
* @brief 显示指定ID号、颜色的隐私遮蔽区域\n
* @param [in] id 区域id号，范围0~23
* @param [in] color 遮蔽区域的颜色索引值，见下面的说明
* @retval 
* 0 - 成功
* -1 - 失败
* @note 不调用本函数将不显示隐私遮蔽区域
*       画面中所有的隐私区域将显示同一种颜色(即不同的区域[不能]用不同的颜色)
*
颜色索引值：
Black   0x00
Gray1   0x01
Gray2   0x02
Gray3   0x03
Gray4   0x04
Gray5   0x05
Gray6   0x06
White   0x07
Red     0x08
Green   0x09
Blue    0x0A
Cyan    0x0B
Yellow  0x0C
Magenta 0x0D
*/
int swpa_camera_basicparam_set_display(int id, int color);

/**
* @brief 获取隐私遮蔽区域的ID\n
* @param [out] display 区域id号，一共24bit，区域ID对应display中的一个比特
* @retval 
* 0 - 成功
* -1 - 失败
* @note display的第0位为1，表示ID为0的区域生效
*/
int swpa_camera_basicparam_get_display(int *display);

// not used
int swpa_camera_basicparam_get_monitor(int *monitor);

/**
* @brief 删除隐私遮蔽区域\n
* @param [in] id 区域id号，范围-1~23 ！！！重要！！！id为-1时表示删除所有的区域 ！！！
* @retval 
* 0 - 成功
* -1 - 失败
* @note 要删除某一隐私区域，传递指定对应的区域ID号
*
*/
int swpa_camera_basicparam_clear_display(int id);

/**
* @brief 设置机芯的pt\n
* @param [in] pan 水平坐标 范围-2048~2048
* @param [in] tilt 垂直坐标 范围-1024~1024
* @retval 
* 0 - 成功
* -1 - 失败
* @note 机芯设置pt坐标的精度约为0.088，坐标值请参考协议文档
* @todo 实际上的机芯的pt和球机的pt是不同的，这里还要做一个转换
*/
int swpa_camera_basicparam_set_pt(int pan, int tilt);

/**
* @brief 获取机芯的pt\n
* @param [out] pan 水平坐标 范围-2048~2048
* @param [out] tilt 垂直坐标 范围-1024~1024
* @retval 
* 0 - 成功
* -1 - 失败
* @note 经测试，获取到的pt是不准确的。不建议使用此函数
* @todo 实际上的机芯的pt和球机的pt是不同的，这里还要做一个转换
*/
int swpa_camera_basicparam_get_pt(int *pan, int *tilt);

/**
* @brief 设置机芯隐私遮蔽区域的ptz值\n
* @param [in] id   区域id号，范围0~23
* @param [in] pan  水平坐标 范围0~3599(0~359.9度)
* @param [in] tilt 垂直坐标 范围0~900(0~90度)
* @param [in] zoom 变焦值，范围0~31424
* @retval 
* 0 - 成功
* -1 - 失败
* @note zoom值应该与swpa_camera_basicparam_set_zoom_value中相同，
*      如果不调用本函数设置zoom，则获取到的zoom是默认值(0)
* @todo 实际上的机芯的pt和球机的pt是不同的，这里还要做一个转换
*/
int swpa_camera_basicparam_set_ptz(int id, int pan, int tilt, int zoom);

/**
* @brief 获取机芯隐私遮蔽区域的ptz值\n
* @param [in] id 区域id号，范围0~23
* @param [out] pan
* @param [out] tilt
* @param [out] zoom
* @retval 
* 0 - 成功
* -1 - 失败
* @note  经测试，获取到的pt是不准确的。不建议使用此函数(传入使用补码，但获取到的不是补码形式)
*
*/
int swpa_camera_basicparam_get_ptz(int id, int *pan, int *tilt, int *zoom);


/**
* @brief 设置机芯的high resolution模式\n
* @param [in] mode 模式开关: 0-Off; 1-On
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_set_hrmode(int mode);


/**
* @brief 获取机芯的high resolution模式\n
* @param [out] mode 模式开关: 0-Off; 1-On
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_get_hrmode(int* mode);

/**
* @brief 设置机芯的曝光补偿模式\n
* @param [in] mode 模式开关: 0-Off; 1-On
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_set_expcomp_mode(int mode);

/**
* @brief 获取机芯的曝光补偿模式\n
* @param [out] mode 模式开关: 0-Off; 1-On
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_get_expcomp_mode(int* mode);

/**
* @brief 设置机芯的曝光补偿值\n
* @param [in] value 取值范围: 0x0 ~ 0xe
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_set_expcomp_value(int value);


/**
* @brief 获取机芯的曝光补偿值\n
* @param [out] value 取值范围: 0x0 ~ 0xe
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_get_expcomp_value(int* value);



/**
* @brief 设置机芯防抖功能\n
* @param [in] value 取值范围: 0x0 ~ 0x1
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星专用
*/
int swpa_camera_basicparam_set_stabilizer(int value);


// not used
int swpa_camera_basicparam_set_grid(int mode);
//////////////////////////////////////////////////

/**
***********************
* 相机输入输出控制
* swpa_camera_io_
***********************
*/

/**
* @brief 软触发抓拍\n
*
* 
* @param [in] flash_set 触发的闪光灯组合（目前支持3路闪光灯，从低位开始，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯）
* @param [in] info 抓拍信息（对应的抓拍图会带有此信息，可用于抓拍图的标识），硬触发的帧号为累加方式
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 如果flash_set中用到的闪光灯与当前启用的闪光灯组不一致，则软触发时临时启用flash_set设置，完成后再恢复原来的使能位设置
* @see .
*/
int swpa_camera_io_soft_capture(
	int flash_set,
	int info
);

// 相对时间清零，相对时间类似于软触发的info信息，在每次硬触发时自动累加
int swpa_camera_io_clear_time();

/**
* @brief 设置硬触发抓拍参数\n
*
* 
* @param [in] capture_edge_mode1 第一路硬触发抓拍沿的工作模式：0-外部下降沿触发；1-外部上升沿触发；2-上升沿下降沿都触发；3-不触发
* @param [in] capture_edge_mode2 第二路硬触发抓拍沿的工作模式：0-外部下降沿触发；1-外部上升沿触发；2-上升沿下降沿都触发；3-不触发
* @param [in] flash_set_1 第一路硬触发的闪光灯组合（目前支持3路闪光灯，从低位开始，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯）
* @param [in] flash_set_2 第二路硬触发的闪光灯组合（目前支持3路闪光灯，从低位开始，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_capture_param(
	int capture_edge_mode1,
	int capture_edge_mode2,
	int flash_set_1,
	int flash_set_2
	);

/**
* @brief 获取硬触发抓拍参数\n
*
* 
* @param [out] capture_edge_mode1 第一路硬触发抓拍沿的工作模式：0-外部下降沿触发；1-外部上升沿触发；2-上升沿下降沿都触发；3-不触发
* @param [out] capture_edge_mode2 第二路硬触发抓拍沿的工作模式：0-外部下降沿触发；1-外部上升沿触发；2-上升沿下降沿都触发；3-不触发
* @param [out] lane_differentiation 是否分车道：0-不分车道，两路硬触发都会使所有闪光灯工作；1-分车道，第一路硬触发只会使第一路闪光灯工作，第二路硬触发只会使第二路闪光灯工作
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_capture_param(
	int * capture_edge_mode1,
	int * capture_edge_mode2,
	int * lane_differentiation
);

/**
* @brief 设置io触发抓拍参数\n
*
* 
* @param [in] lane_differentiation 是否分车道：0-不分车道，每次io触发都会使所有闪光灯工作；1-分车道，每次io触发时指定哪几路闪光灯工作
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note io触发抓拍，是指由CPU管脚直接发送抓拍命令，有别于硬触发
* @see .
*/
int swpa_camera_io_set_iocapture_param(
	int lane_differentiation
);

/**
* @brief 获取io触发抓拍参数\n
*
* 
* @param [out] lane_differentiation 是否分车道：0-不分车道，每次io触发都会使所有闪光灯工作；1-分车道，每次io触发时指定哪几路闪光灯工作
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note io触发抓拍，是指由CPU管脚直接发送抓拍命令，有别于硬触发
* @see .
*/
int swpa_camera_io_get_iocapture_param(
	int * lane_differentiation
);

/**
* @brief 设置帧率（外同步）模式\n
*
* 
* @param [in] mode 外同步模式：1-开启内部电网同步；2-开启外部电网同步（目前定义从第二路抓拍信号接口接收同步信号）；3-200w/280w时25帧模式，500w时12.5帧模式；4-200w/280w时30帧模式，500w时15帧模式
* @param [in] external_sync_delay 外同步信号延迟：范围0-200，单位为0.1ms
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note external_sync_delay只在mode为1或2时生效。
* @see .
*/
int swpa_camera_io_set_framerate_mode(
	int mode,
	int external_sync_delay
);

/**
* @brief 获取帧率（外同步）模式\n
*
* 
* @param [out] mode 外同步模式：1-开启内部电网同步；2-开启外部电网同步（目前定义从第二路抓拍信号接口接收同步信号）；3-200w/280w时25帧模式，500w时12.5帧模式；4-200w/280w时30帧模式，500w时15帧模式
* @param [out] external_sync_delay 外同步信号延迟：范围0-200，单位为0.1ms
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note external_sync_delay只在mode为1或2时生效。
* @see .
*/
int swpa_camera_io_get_framerate_mode(
	int * mode,
	int * external_sync_delay
);

/**
* @brief 设置频闪灯信号开关\n
*
* 
* @param [in] enable 频闪信号开关：0-关闭频闪同步信号输出；1-开启频闪同步信号输出
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_strobe_signal(
	int enable
);

/**
* @brief 获取频闪灯信号开关\n
*
* 
* @param [out] enable 频闪信号开关：0-关闭频闪同步信号输出；1-开启频闪同步信号输出
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_strobe_signal(
	int * enable
);

/**
* @brief 设置闪光灯信号开关\n
*
* 
* @param [in] enable_flash_set 启用的闪光灯组合（目前支持3路闪光灯，从低位开始，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯），默认值为7即三路都使能
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_flash_signal(
	int enable_flash_set
);

/**
* @brief 获取闪光灯信号开关\n
*
* 
* @param [out] enable_flash_set 启用的闪光灯组合（目前支持3路闪光灯，从低位开始，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_flash_signal(
	int * enable_flash_set
);

/**
* @brief 设置频闪补光灯参数\n
*
* 
* @param [in] multiplication 倍频：1-倍频（>=50hz）；0-不倍频（红外灯）
* @param [in] polarity 极性：1-正极性；0-负极性
* @param [in] pulse_width 脉宽：范围1-200，精度0.1ms，即允许的值范围为0.1ms-20ms
* @param [in] output_type 输出类型：0-上拉（电平）；1-OC（开关）
* @param [in] enable_when_capture 抓拍帧频闪灯使能：1-使能；0-不使能
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_strobe_param(
	int multiplication, 
	int polarity, 
	int pulse_width,
	int output_type,
	int enable_when_capture
);

/**
* @brief 获取频闪补光灯参数\n
*
* 
* @param [out] multiplication 倍频：1-倍频（>=50hz）；0-不倍频（红外灯）
* @param [out] polarity 极性：1-正极性；0-负极性
* @param [out] pulse_width 脉宽：范围1-200，精度0.1ms，即允许的值范围为0.1ms-20ms
* @param [out] output_type 输出类型：0-上拉（电平）；1-OC（开关）
* @param [out] enable_when_capture 抓拍帧频闪灯使能：1-使能；0-不使能
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_strobe_param(
	int * multiplication, 
	int * polarity, 
	int * pulse_width,
	int * output_type,
	int * enable_when_capture
);

/**
* @brief 设置闪光灯参数\n
*
* 
* @param [in] id 设置的闪光灯序号（1到3）
* @param [in] coupling 抓拍耦合到频闪（在抓拍时，频闪灯的亮的时间至少要等于闪光灯的脉宽），默认值为1：1-耦合；0-不耦合
* @param [in] polarity 极性，默认值为1：1-正极性；0-负极性
* @param [in] pulse_width 脉宽，默认值为1：范围1-100，精度0.1ms，即允许的值范围为0.1ms-10ms
* @param [in] output_type 输出类型，默认值为1：0-上拉（电平）；1-OC（开关）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_flash_param(
	int id,
	int coupling,
	int polarity,
	int pulse_width,
	int output_type
);

/**
* @brief 获取闪光灯参数\n
*
* 
* @param [in] id 获取的闪光灯序号（1到3）
* @param [out] coupling 抓拍耦合到频闪（在抓拍时，频闪灯的亮的时间至少要等于闪光灯的脉宽），默认值为1：1-耦合；0-不耦合
* @param [out] polarity 极性，默认值为1：1-正极性；0-负极性
* @param [out] pulse_width 脉宽，默认值为1：范围1-100，精度0.1ms，即允许的值范围为0.1ms-10ms
* @param [out] output_type 输出类型，默认值为1：0-上拉（电平）；1-OC（开关）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_flash_param(
	int id,
	int * coupling,
	int * polarity,
	int * pulse_width,
	int * output_type
);

/**
* @brief 设置地灯参数\n
*
* 
* @param [in] enable 地灯开关：0-关闭地灯；1-开启地灯
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_lamp(
	int enable
);

/**
* @brief 获取地灯参数\n
*
* 
* @param [out] enable 地灯开关：0-关闭地灯；1-开启地灯
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_lamp(
	int * enable
);

/**
* @brief 控制自动DC光圈（电控光圈）\n
*
* 
* @param [in] mode 光圈工作模式，0-停止，1-打开，2-关闭
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 当DC光圈处于全关或者是半关状态时，DC光圈在停止状态时，并不会一直停止，而只是做短暂的停留，光圈会自动的张开。所以需要不停的在停止和关闭状态切换，才能基本保持停止。 
* @see .
*/
int swpa_camera_io_control_dc_iris(
	int mode
);

/**
* @brief 控制滤光片\n
*
* 
* @param [in] mode 滤光片工作模式：0-停止；1-正常滤光片；2-红外（或偏振）滤光片
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 切换滤光片后，请在2s后，设置为停止状态。即在以1或2为参数调用此接口后，等待2s再以0为参数调用此接口。
*       滤光片的当前状态，无法从寄存器或是图片输出信息获取，需要由上层相机控制模块自行维护。
* @see .
*/
int swpa_camera_io_control_filter(
	int mode
);

/**
* @brief 设置报警区域\n
*
* 
* @param [in] x1 报警区域的左上角x坐标，范围0-2047
* @param [in] y1 报警区域的左上角y坐标，范围0-2047
* @param [in] x2 报警区域的右下角x坐标，范围0-2047
* @param [in] y2 报警区域的右下角y坐标，范围0-2047
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_set_alarm_zone(
	int x1, int y1,
	int x2, int y2
);

/**
* @brief 获取报警区域\n
*
* 
* @param [in] x1 报警区域的左上角x坐标，范围0-2047
* @param [in] y1 报警区域的左上角y坐标，范围0-2047
* @param [in] x2 报警区域的右下角x坐标，范围0-2047
* @param [in] y2 报警区域的右下角y坐标，范围0-2047
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_alarm_zone(
	int * x1, int * y1,
	int * x2, int * y2
);


/**
* @brief 获取栏杆机状态
*
* 
* @param [out] pstate 栏杆机状态值，范围0-1
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_get_barrier_state(int* pstate);


/**
* @brief 使用TG2作为外部信号输入功能，不用做触发
*
* 
* @param [in] enable 作外部信号输入使用标志，范围0-1
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_io_backupio_enable(int enable);

/**
***********************
* 相机图像处理
* swpa_camera_imgproc_
***********************
*/



/**
* @brief 设置Gamma值\n
*
* 
* @param [in] data Gamma值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_set_gamma(
	int data[8][2]
);

/**
* @brief 获取Gamma值\n
*
* 
* @param [out] data Gamma值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 应用暂时没有使用需求，且取值过程较复杂，不实现
* @see .
*/
int swpa_camera_imgproc_get_gamma(
	int data[8][2]
);

/**
* @brief 设置Gamma 通道选择模式\n
*
* 
* @param [in] mode 通道选择值：0：6467和368都无GAMMA，1：6467有368无，2：6467无368有，3：6467有368有
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_set_gamma_mode(
	int mode
);

/**
* @brief 读取Gamma 通道选择模式\n
*
* 
* @param [in] mode 通道选择值：0：6467和368都无GAMMA，1：6467有368无，2：6467无368有，3：6467有368有
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_get_gamma_mode(
	int* mode
	);

/**
* @brief 设置红灯加红参数\n
*
* 
* @param [in] h_threshold （与黄灯加红有关）HSL空间，色相阈值，范围0-184320，精度为1/512度，即表达范围是0-360度
* @param [in] l_threshold （与黄灯加红有关）HSL空间，亮度阈值，范围0-16383
* @param [in] s_threshold （与黄灯加红有关）HSL空间，饱和度阈值，范围0-16383
* @param [in] color_factor （与黄灯加红有关）色相乘法因子，范围0-511
* @param [in] lum_l_threshold （与白灯加红有关）亮度阈值，范围0-16383
* @param [in] lum_l_factor （与白灯加红有关）亮度乘法因子，范围0-511
* @param [in] lum_h_factor （与白灯加红有关）色相乘法因子，范围0-511
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_set_trafficlight_enhance_param(
	int h_threshold, 
	int l_threshold, 
	int s_threshold, 
	int color_factor,
	int lum_l_threshold,
	int lum_l_factor,
	int lum_h_factor
);

/**
* @brief 获取红灯加红参数\n
*
* 
* @param [out] h_threshold （与黄灯加红有关）HSL空间，色相阈值，范围0-184320，精度为1/512度，即表达范围是0-360度
* @param [out] l_threshold （与黄灯加红有关）HSL空间，亮度阈值，范围0-16383
* @param [out] s_threshold （与黄灯加红有关）HSL空间，饱和度阈值，范围0-16383
* @param [out] color_factor （与黄灯加红有关）色相乘法因子，范围0-511
* @param [out] lum_l_threshold （与白灯加红有关）亮度阈值，范围0-16383
* @param [out] lum_l_factor （与白灯加红有关）亮度乘法因子，范围0-511
* @param [out] lum_h_factor （与白灯加红有关）色相乘法因子，范围0-511
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_get_trafficlight_enhance_param(
	int * h_threshold, 
	int * l_threshold, 
	int * s_threshold, 
	int * color_factor,
	int * lum_l_threshold,
	int * lum_l_factor,
	int * lum_h_factor
);

/**
* @brief 设置红灯加红区域\n
*
* 
* @param [in] id 红灯加红区域ID值，合法值为1至8
* @param [in] x1 加红区域的左上角x坐标
* @param [in] y1 加红区域的左上角y坐标
* @param [in] x2 加红区域的右下角x坐标
* @param [in] y2 加红区域的右下角y坐标
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_set_trafficlight_enhance_zone(
	int id,
	int x1, int y1,
	int x2, int y2
);

/**
* @brief 获取红灯加红区域\n
*
* 
* @param [in] id 红灯加红区域ID值，合法值为1至8
* @param [out] x1 加红区域的左上角x坐标
* @param [out] y1 加红区域的左上角y坐标
* @param [out] x2 加红区域的右下角x坐标
* @param [out] y2 加红区域的右下角y坐标
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 寄存器暂时不支持取值，不实现此接口
* @see .
*/
int swpa_camera_imgproc_get_trafficlight_enhance_zone(
	int id,
	int * x1, int * y1,
	int * x2, int * y2
);

/**
* @brief 设置色彩参数\n
*
* 
* @param [in] mode 工作模式 1-关闭色彩调整功能；2-作用于抓拍帧；3-作用于所有帧
* @param [in] contrast_value 对比度值，范围：[-100~100]，默认值：0(此值针对应用层，计算得到的因子是FPGA寄存器的默认值)
* @param [in] saturation_value 饱和度值，范围：[-100~100]，默认值：0(此值针对应用层，计算得到的r/g/b分量是FPGA寄存器的默认值)
* @param [in] color_temperature_value 色温值
* @param [in] hue_value 色度值（色彩、色相）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_set_color_param(
	int mode,
	int contrast_value,
	int saturation_value,
	int color_temperature_value,
	int hue_value
);

/**
* @brief 获取色彩参数\n
*
* 
* @param [out] mode 工作模式 1-关闭色彩调整功能；2-作用于抓拍帧；3-作用于所有帧
* @param [out] contrast_value 对比度值，范围：[-100~100]，默认值：0(此值针对应用层，计算得到的因子是FPGA寄存器的默认值)
* @param [out] saturation_value 饱和度值，范围：[-100~100]，默认值：0(此值针对应用层，计算得到的r/g/b分量是FPGA寄存器的默认值)
* @param [out] color_temperature_value 色温值
* @param [out] hue_value 色度值（色彩、色相）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 应用暂时没有使用需求，且暂时没有实现将寄存器值拆分的算法，因此不实现此接口
* @see .
*/
int swpa_camera_imgproc_get_color_param(
	int * mode,
	int * contrast_value,
	int * saturation_value,
	int * color_temperature_value,
	int * hue_value
);

/**
* @brief 设置锐化相关参数\n
*
* 
* @param [in] mode 工作模式 1-关闭锐化功能；2-作用于抓拍帧；3-作用于所有帧  木星不使用
* @param [in] threshold 阈值 范围0-127 木星范围0~15
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 
* @see .
*/
int swpa_camera_imgproc_set_sharpen_param(
	int mode,
	int threshold
);

/**
* @brief 获取锐化相关参数\n
*
* 
* @param [out] mode 工作模式 1-关闭锐化功能；2-作用于抓拍帧；3-作用于所有帧  木星不使用
* @param [out] threshold 阈值 范围0-127  木星范围0~15
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 
* @see .
*/
int swpa_camera_imgproc_get_sharpen_param(
	int * mode,
	int * threshold
);

/**
* @brief 设置除雾相关参数\n
*
* 
* @param [in] mode 工作模式 1-关闭除雾功能；2-作用于抓拍帧；3-作用于所有帧
* @param [in] factor 除雾系数 范围：[1~10]。 值越高，雾除得越干净，但图像会变暗
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 除雾系数可由用户设置，默认及推荐值为8，推荐范围：6~8
* @todo 木星暂不支持图像的除雾
* @see .
*/
int swpa_camera_imgproc_set_defog_param(
	int mode,
	int factor
);

/**
* @brief 获取除雾相关参数\n
*
* 
* @param [out] mode 工作模式 1-关闭除雾功能；2-作用于抓拍帧；3-作用于所有帧
* @param [out] factor 除雾系数 范围：[1~10]。 值越高，雾除得越干净，但图像会变暗
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_get_defog_param(
	int * mode,
	int * factor
);

/**
* @brief 设置彩色/黑白输出模式\n
*
* 
* @param [in] mode 工作模式 0-彩色；1-黑白。默认为0
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_set_color_black(
	int mode
);

/**
* @brief 获取彩色/黑白输出模式\n
*
* 
* @param [out] mode 工作模式 0-彩色；1-黑白。默认为0
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_imgproc_get_color_black(
	int * mode
);

/**
* @brief 设置饱和度\n
* @param [in] value 0x0 -0x0e
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_set_saturation(
    int saturation
);

/**
* @brief 获取饱和度\n
* @param [out] value 0x0 -0x0e
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_get_saturation(
    int *saturation
);

/**
* @brief 设置gamma\n
* @param [in] value 机芯:   0:	Standard,	1	to	4
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_set_gamma_type(
    int gamma
);

/**
* @brief 获取gamma\n
* @param [out] value
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_get_gamma_type(
    int *gamma
);

/**
* @brief 设置WDR\n
* @param [in] mode ：wdr 模式 参见WDR_MODE
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_set_WDR(
    WDR_MODE mode
);

/**
* @brief 获取WDR\n
* @param [out] mode ：wdr 模式
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_get_WDR(
    WDR_MODE *mode
);

/**
* @brief 设置降噪\n
* @param [in] value   机芯:   0:	OFF,	level	1	to	5
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_set_NR(
    int NR
);

/**
* @brief 获取降噪\n
* @param [out] value
* @retval 
* 0 - 成功
* -1 - 失败
* @note 木星使用
* @see .
*/
int swpa_camera_imgproc_get_NR(
    int *NR
);

/**
***********************
* 相机初始参数设置（如采样点、前端像素增益等）,等补充
* swpa_camera_init_
***********************
*/

/**
* @brief 设置RAW RGB增益\n
*
* 
* @param [in] gain_r R增益，范围0-131071（17位无符号整型），单位为1/2^14倍，相当于可以表达的倍数为[0.0-8.0>
* @param [in] gain_gr Gr增益，范围0-131071（17位无符号整型），单位为1/2^14倍，相当于可以表达的倍数为[0.0-8.0>
* @param [in] gain_gb Gb增，范围0-131071（17位无符号整型），单位为1/2^14倍，相当于可以表达的倍数为[0.0-8.0>
* @param [in] gain_b B增益，范围0-131071（17位无符号整型），单位为1/2^14倍，相当于可以表达的倍数为[0.0-8.0>
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 如果不调用这个接口，fpga的各分量默认值为0x4000，即换算为倍数是1.0倍
* @see .
*/
int swpa_camera_init_set_raw_rgb_gain(
	int gain_r,
	int gain_gr,
	int gain_gb,
	int gain_b
); 

/**
* @brief 设置增益的各通道修正值\n
*
* 
* @param [in] gain_correction1 增益的第1通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @param [in] gain_correction2 增益的第2通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @param [in] gain_correction3 增益的第3通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @param [in] gain_correction4 增益的第4通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 参数可以为负值，在设置增益时自动将修正值叠加到各通道上。
*       设计此接口只在初始化时调用一次，因此内部不保证线程安全性
*       不同分辨率的设备具备不同的通道数，如500W设备有两个通道，则只有gain_correction1和gain_correction2会起作用
* @see swpa_camera_basicparam_set_gain
*/
int swpa_camera_init_set_gain_correction(
	int gain_correction1,
	int gain_correction2,
	int gain_correction3,
	int gain_correction4
);

/**
* @brief 设置抓拍增益的各通道修正值\n
*
* 
* @param [in] gain_correction1 增益的第1通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @param [in] gain_correction2 增益的第2通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @param [in] gain_correction3 增益的第3通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @param [in] gain_correction4 增益的第4通道修正值，范围-1023~1023，精度0.0359db，即增益范围为-36db~36db
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 参数可以为负值，在设置增益时自动将修正值叠加到各通道上。
*       设计此接口只在初始化时调用一次，因此内部不保证线程安全性
*       不同分辨率的设备具备不同的通道数，如500W设备有两个通道，则只有gain_correction1和gain_correction2会起作用
* @see swpa_camera_basicparam_set_capture_gain
*/
int swpa_camera_init_set_capture_gain_correction(
	int gain_correction1,
	int gain_correction2,
	int gain_correction3,
	int gain_correction4
);

/**
* @brief 设置坏点去除参数\n
*
* 
* @param [in] threshold 阈值，范围0-16383，默认值8000。该值越小，坏点去除效果越明显，但是太小会滤掉某些不是坏点的点，取最大值时，没有坏点去除功能。
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_init_set_dead_pixel_removal(
	int threshold
); 

/**
* @brief 获取设备相关的特性\n
*
* 
* @param [out] max_shutter 快门最大值，单位1us，快门最小值为0
* @param [out] max_gain 增益最大值，单位0.1db，增益最小值为0
* @param [out] ad_channels AD通道数
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
int swpa_camera_init_get_device_dependent_info(
	int * max_shutter,
	int * max_gain,
	int * ad_channels
); 

///////////////////////////////////////////////////////////////////////////////
/**
***********************
* 下面是木星平台球机相关接口实现
*/

/** 
 * 停止球机动作
 *
 * @return 成功：0  失败：-1 超时：-6
 * @note 每种动作，都要调用该函数来停止，否则，球机将一直执行(如果设置了水平旋转，则会一直转)。
 */
int swpa_camera_ball_stop(void);

/** 
 * 球机垂直移动
 * @param action [in] 垂直移动操作 0 = down  1 = up
 * @param speed [in]  移动速度：1~0x3f
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 */
int swpa_camera_ball_tilt(TILT_TYPE action, int speed);

/** 
 * 球机水平移动
 * @param action [in] 水平旋转操作 0 = left  1 = right
 * @param speed [in]  移动速度：1~0x3f
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 */
int swpa_camera_ball_pan(PAN_TYPE action, int speed);

/** 
 * 球机同时水平、垂直移动
 * @param pan_act [in] 水平旋转操作 0 = left  1 = right
 * @param pan_speed [in] 移动速度：1~0x3f
 * @param tilt_act [in] 垂直移动操作 0 = down  1 = up
 * @param tilt_speed [in] 移动速度：1~0x3f
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 该函数是swpa_camera_ball_pan和swpa_camera_ball_tilt的混合形式
 */
int swpa_camera_ball_pan_tilt(PAN_TYPE pan_act, int pan_speed, TILT_TYPE tilt_act, int tilt_speed);
///////////////////////////////////////////////////////////////////////////////

/** 
 * 预置位相关函数
 * @param preset_act [in] 预置位操作 0：设置 1：清除 2：调用
 * @param preset_id [in] 1~255 第0号无效
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @todo 球机有部分预置位是已经被占用了，但目前只有部分预置位的文档，无法知道是否有其它保留的预置位。
 */
int swpa_camera_ball_preset(PRESET_TYPE preset_act, int preset_id);

/** 
 * 设置球机PT坐标
 * @param pan [in] 0~3599(0~359.9度)
 * @param tilt [in] -100~900(-10度~90度)
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note PT即水平(pan)、垂直(tilt)的坐标。水平范围只到3599，设置3600与设置0等效
 * @todo 需求方要求垂直角度在-10~90，在这个函数中进行参数判断。
 */
int swpa_camera_ball_set_pt_coordinates(int pan, int tilt);

/** 
 * 读取球机PT坐标
 * @param pan [out] 0~3599(0~359.9度)
 * @param tilt [out] 0~900(0度~90度)
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 说明！！球机精度为0.1，即数值1，设置与获取之间会有1的误差。
 */
int swpa_camera_ball_get_pt_coordinates(int* pan, int* tilt);

/** 
 * 设置红外模式
 *
 * @param mode [in] 模式：0-关闭 1-开启 2-自动控制
 * @return 成功：0  失败：-1 超时：-6
 */
 /* //Marked in Version B 
int swpa_camera_ball_set_infrared_led(int mode);*/

/** 
 * 设置红外功率
 * @param zoom_total [in] 机芯总的变倍数(即最大能放大多少倍)
 * @param zoom_1 [in]     当前变倍率的整数部分
 * @param zoom_2 [in]     当前变倍率的小数部分
 * 
 * @return 成功：0  失败：-1 超时：-6
 * @note 如设置当前的变倍率为3.75，则zoom_1为3,zoom_2为75
 *       如发送当前变倍为1倍，则近红外灯亮，发送当前变倍为20倍，则远红外灯亮 。
 *       ！！！在调用swpa_camera_ball_set_infrared_led打开红外灯情况下才能设置！！！
 */
/* //Marked in Version B 
int swpa_camera_ball_set_infrared_power(int zoom_total, int zoom_1, int zoom_2);*/

/** 
 * 设置红外灯开启灵敏度
 * @param sensitivity [in] 灵敏度值 范围：1-10
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 数值越小，红外灯开启需要的环境光照度越低，出厂默认为3
 */
int swpa_camera_ball_set_infrared_sensitive(int sensitivity);

/** 
 * 设置红外开启阈值窗口
 * @param threshold [in] 阈值 范围：1-5
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 如果红外灯开关频繁，可以把切换窗口值适当调大，出厂默认为2
 */
int swpa_camera_ball_set_infrared_threshold(int threshold);


/** 
 * 启动雨刷
 *
 * @return 成功：0  失败：-1 超时：-6
 * @note 启动雨刷，会自动来回移动3次，然后停止
 */
int swpa_camera_ball_set_wiper(void);

///////////////////////////////////////////////////////////////////////////////

/** 
 * 远程复位
 *
 * @return 成功：0  失败：-1 超时：-6
 * @note 复位后，球机会自动进行自检操作(如转动)
 */
int swpa_camera_ball_remote_reset(void);

/** 
 * 恢复出厂设置
 *
 * @return 成功：0  失败：-1 超时：-6
 */
int swpa_camera_ball_restore_factory_settings(void);

/** 
 * 开始老化测试
 *
 * @return 成功：0  失败：-1 超时：-6
 */
int swpa_camera_ball_factorytest_start(void);

/** 
 * 关闭老化测试
 *
 * @return 成功：0  失败：-1 超时：-6
 */
int swpa_camera_ball_factorytest_stop(void);

/** 
 * 设置球机除雾除霜功能
 *
 * @param mode [in] 模式：0-关闭 1-开启
 * @return 成功：0  失败：-1 超时：-6
 * @note 本函数实际上开启球机上的风扇(及加热功能)使雾气散去，开启和关闭之间间隔不能太频繁
 */
int swpa_camera_ball_set_defog_fan(int mode);


/** 
 * 球机零点快速(3秒内完成)校准
 *
 * @return 成功：0  失败：-1 
 * @note: 木星专用
 */
int swpa_camera_ball_calibration(void);

/** 
 * 球机3D定位
 *
 * @param centerx [in] 定位中心点x坐标 范围：0 ~ 1920
 * @param centery [in] 定位中心点y坐标 范围：0 ~ 1080
 * @param width [in] 定位区域宽度 范围：0 ~ 1920
 * @param height [in] 定位区域高度 范围：0 ~ 1080
 * @return 成功：0  失败：-1 
 * @note: 木星专用
 */
int swpa_camera_ball_3d(int centerx, int centery, int width, int height);

/** 
 * 设置球机补光灯工作模式
 *
 * @param mode [in] 工作模式枚举，0x0:LED_AUTO; 0x1:LED_TIMING; 0x2:LED_ON; 0x3:LED_OFF;0x4:LED_CMR_CTRL
 * @param powermode [in] 匹配模式枚举，
 *                  0x0: LED_POWER_STD ：红外灯标准模式，功率根据当前变倍以最大功率匹配;
 *                  0x1: LED_POWER_MATCH：红外灯匹配模式，摄像机1倍时，红外灯控制功率为80%;
 *                  0x2: LED_POWER_USR : 自定义模式，此模式下，红外灯各组功率可以任意设置。
 * @return 成功：0  失败：-1 
 * @note: 木星专用
 */
int swpa_camera_ball_set_led_mode(int mode, int powermode);



/** 
 * 设置球机补光灯功率
 *
 * @param nearpower [in] 近光灯功率值，范围：0 ~ 10
 * @param mediumpower [in] 中光灯功率值 范围：0 ~ 10
 * @param farpower [in] 远光灯功率值 范围：0 ~ 10
 * @return 成功：0  失败：-1 
 * @note: 木星专用
 */
int swpa_camera_ball_set_led_power(int nearpower, int mediumpower, int farpower);

/** 
 * 触发球机与机芯进行zoom值同步
 *
 * @return 成功：0  失败：-1 
 * @note: 木星专用
 */
int swpa_camera_ball_sync_zoom();

/** 
 * 设置球机隐私区域(代替机芯本身的隐私遮盖功能)
 *
 * @param index [in] 隐私区域编号，范围：0 ~ 8
 * @param enable [in] 使能标志 范围：0 (不使能，隐藏)， 1(使能，显示)
 * @param width [in] 遮盖区域宽度 范围：0 ~ 80
 * @param height [in] 遮盖区域高度 范围：0 ~ 45
 * @return 成功：0  失败：-1 
 * @note: 木星专用
 */
int swpa_camera_ball_set_privacyzone(int index, int enable, int width, int height);

/** 
 * 设置球机隐私区域坐标(球机B版本不支持随意调整该坐标，需强制成画面中心)
 *
 * @param index [in] 隐私区域编号，范围：0 ~ 8
 * @param x [in] 区块中心横坐标，范围：0 ~ 100
 * @param y [in] 区块中心纵坐标，范围：0 ~ 100
 * @return 成功：0  失败：-1 
 * @note: 木星专用，需先调用该函数，再调用swpa_camera_ball_set_privacyzone()才能正常显示遮盖区域
 */
int swpa_camera_ball_set_privacycoord(int index, int x, int y);



/*
*  ----调试用----
*/


 /** 
 * 设置机芯或球机参数 未调试通过
 *
 * @param 
 * type : visca 或者pelco
 * write_buf : 命令buffer
 * write_buf_size : 命令长度
 * @return 成功：0  失败：-1 失败
 */
int spwa_camera_send_cmd(PROTOCOL_TYPE type,unsigned char *write_buf,int write_buf_size);

/** 
* 读取机芯或球机参数 未调试通过
*
* @param 
* type : visca 或者pelco
* write_buf : 命令buffer
* write_buf_size : 命令长度
* awk_buf : 返回buf
* awk_buf_size : 返回数据长度
* @return 成功：0  失败：-1 失败
*/
int spwa_camera_recive_cmd(PROTOCOL_TYPE type,unsigned char *write_buf,int write_buf_size,unsigned char *awk_buf,int *awk_buf_size);

/** 
 * 向机芯或球机发送数据
 * @param type [in] 协议类型，参考PROTOCOL_TYPE定义(仅支持该枚举中的值)
 * @param buffer [in] 数据
 * @param buffer_size [in] 数据长度
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 本函数直接使用系统的write函数进行数据传输
 */
int spwa_camera_com_send_data(PROTOCOL_TYPE type,unsigned char *buffer, int buffer_size);

/** 
 * 读取机芯或球机的数据
 * @param type [in] 协议类型，参考PROTOCOL_TYPE定义(仅支持该枚举中的值)
 * @param buffer [in] 数据
 * @param buffer_size [in] 数据长度
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 直接使用系统的read函数接收数据
 */
int spwa_camera_com_rcv_data(PROTOCOL_TYPE type, unsigned char *buffer, int buffer_size);

/** 
 * 读取机芯或球机的数据
 * @param type [in] 协议类型，参考PROTOCOL_TYPE定义(仅支持该枚举中的值)
 * @param buffer [out] 数据缓冲区
 * @param buffer_size [in/out] 数据长度，传入时，表示希望接收的数据，传出为实际接收的数据
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 本函数添加超时接收机制
 */
int spwa_camera_com_rcv_packet_data(PROTOCOL_TYPE type, unsigned char *buffer, int* buffer_size);

/** 
 * 读取机芯或球机的数据
 * @param type [in] 协议类型，参考PROTOCOL_TYPE定义(仅支持该枚举中的值)
 * @param buffer [out] 数据缓冲区
 * @param buffer_size [in/out] 数据长度，传入时，表示希望接收的数据，传出为实际接收的数据
 * @param terminator [in] 结束符
 * 
 * @return 成功：0  失败：-1 参数非法：-2 超时：-6
 * @note 本函数与spwa_camera_com_rcv_packet_data不同之处在于，是以接收到terminator结束符为标志的
 *       适合visca以0xff结束的数据方式
 */
int spwa_camera_com_rcv_packet_terminator(PROTOCOL_TYPE type, unsigned char *buffer, int *buffer_size, int terminator);

/**
   @brief 图像上下倒转
   @param value [IN] ON:0x02 OFF:0x03
 */
int swpa_camera_basicparam_set_picture_flip(int value);

/**
   @brief 图像左右倒转
   @param value [IN] ON:0x02 OFF:0x03
 */
int swpa_camera_basicparam_set_lr_reverse(int value);

/**
   @brief 获取球机固件版本大小
   @param pDomeVersion [OUT] bit:0-7 低版本字段 8-15高版本字段
 */
int swpa_camera_ball_get_dome_version(int *pDomeVersion);

/**
   @brief 获取机芯版本信息
   @param pCamVersion [OUT] byte0-1:VenderID byte2-3:ModelID byte4-5:ROM version
 */
int swpa_camera_ball_get_camera_version(char *pCamVersion);

#ifdef __cplusplus
}
#endif

#endif // _SWPA_CAMERA_H_

