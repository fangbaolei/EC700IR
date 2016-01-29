/**
* @file  swpa_camera.c
* @brief  相机相关
* @author luoqz@signalway.com.cn
* @date  2013-2-21
* @version  V0.1
* @note
*/

#include "swpa_private.h"
#include "swpa_camera.h"
#include "drv_device.h"

#ifdef SWPA_CAMERA
#define SWPA_CAMERA_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_CAMERA_CHECK(arg)      {if (!(arg)){SWPA_CAMERA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_CAMERA_PRINT(fmt, ...)
#define SWPA_CAMERA_CHECK(arg)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
* 平台相关定义
*/
#define SWPA_MAX_GAIN 0x3ff    // 寄存器的限制
#define SWPA_MAX_GAIN_API 360  // 接口的限制

#define _CAMERA_PIXEL_200W_ // TODO: 正式发布时需删掉

#ifdef _CAMERA_PIXEL_500W_
	#define SWPA_MAX_SHUTTER 54000                           // 快门最大值（单位us）
	#define SWPA_SHUTTER_UNIT 32                             // 快门单位（单位us）
	#define SWPA_AD_CHANNELS 2                               // AD通道数 TODO: 跟通道有关的参数（增益、黑电平）需要以此值进行判断
#elif defined (_CAMERA_PIXEL_200W_)
	#define SWPA_MAX_SHUTTER 30000                           // 快门最大值（单位us）
	#define SWPA_SHUTTER_UNIT 27.685                         // 快门单位（单位us）
	#define SWPA_AD_CHANNELS 2                               // AD通道数
#elif defined (_CAMERA_PIXEL_280W_)
	#define SWPA_MAX_SHUTTER 30000                           // 快门最大值（单位us）
	#define SWPA_SHUTTER_UNIT 27.685                         // 快门单位（单位us）
	#define SWPA_AD_CHANNELS 2                               // AD通道数
#endif // 平台相关定义

/**
* 结构体
*/
typedef union SWPA_REG_SYN_AC_EXT_T // 电网同步寄存器
{
	struct {
		unsigned int sync_signal_delay : 8;
		unsigned int internal_sync_enable : 1;
		unsigned int external_sync_enable : 1;
	} bits;
	unsigned int word;
} SWPA_REG_SYN_AC_EXT;

typedef union SWPA_REG_CAPTURE_EN_FLAG_T // 抓拍参数使能位寄存器
{
	struct {
		unsigned int shutter_enable : 1;
		unsigned int gain_enable : 1;
		unsigned int clamp_level_enable : 1;
		unsigned int rgb_gain_enable : 1;
	} bits;
	unsigned int word;
} SWPA_REG_CAPTURE_EN_FLAG;

typedef union SWPA_REG_CAPTURE_EDGE_EXT_T // 抓拍沿寄存器
{
	struct {
		unsigned int capture_edge1 : 2;
		unsigned int capture_edge2 : 2;
	} bits;
	unsigned int word;
} SWPA_REG_CAPTURE_EDGE_EXT;

typedef union SWPA_REG_LED_SET_T // 频闪参数寄存器
{
	struct {
		unsigned int pulse_width : 8;
		unsigned int polarity : 1;
		unsigned int output_type : 1;
		unsigned int multiplication : 1;
		unsigned int enable_when_capture : 1;
		unsigned int enable : 1;
	} bits;
	unsigned int word;
} SWPA_REG_LED_SET;

typedef union SWPA_REG_FLASH_SET_T // 闪光灯参数寄存器
{
	struct {
		unsigned int pulse_width : 8;
		unsigned int polarity : 1;
		unsigned int output_type : 1;
		unsigned int coupling : 1;
		unsigned int enable : 1;
	} bits;
	unsigned int word;
} SWPA_REG_FLASH_SET;

typedef enum SWPA_REGISTER_TYPE_T {
	REGISTER_FPGA,       // fpga
	REGISTER_AD1,        // 第1片AD
	REGISTER_AD2,        // 第2片AD
	REGISTER_AD3,        // 第3片AD
	REGISTER_AD4         // 第4片AD
} SWPA_REGISTER_TYPE;

/**
* 内部函数
*/
#ifndef MIN
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

// 构造寄存器的SPI协议地址
void construct_spi_addr(
	int * spi_addr,
	SWPA_REGISTER_TYPE register_type,
	int register_addr,
	int read                             // 0表示写，1表示读
)
{
	int cmd = 0;
	switch (register_type)
	{
		case REGISTER_FPGA:
			cmd = 1;
			break;
		case REGISTER_AD1:
			cmd = 0;
			break;
		case REGISTER_AD2:
			cmd = 2;
			break;
		case REGISTER_AD3:
			cmd = 4;
			break;
		case REGISTER_AD4:
			cmd = 6;
			break;
	}
	*spi_addr = (cmd << 12) | register_addr | (read << 15);
}

// 向fpga_struct中插入一个寄存器数据
int fill_fpga_struct(
    FPGA_STRUCT * fpga_struct,
	SWPA_REGISTER_TYPE register_type,  // [IN]寄存器类型
	unsigned int register_addr,        // [IN]寄存器地址
    unsigned int data                  // [IN]数据
)
{
	int spi_addr = 0;
	SWPA_CAMERA_CHECK(0 != fpga_struct);
	SWPA_CAMERA_CHECK(fpga_struct->count < MAX_FPGA_REG);
	SWPA_CAMERA_CHECK(0 < register_addr && register_addr < 0x1000);

	construct_spi_addr(&spi_addr, register_type, register_addr, 0);

    fpga_struct->regs[fpga_struct->count].addr  = spi_addr;
    fpga_struct->regs[fpga_struct->count].data  = data;
    fpga_struct->count++;


    return SWPAR_OK;
}

// 读取寄存器值
int read_register(
	DRV_DEVICE_TYPE dev_id,
	SWPA_REGISTER_TYPE register_type,  // [IN]寄存器类型
	unsigned int register_addr,        // [IN]寄存器地址
    unsigned int * data                // [OUT]数据
)
{
	int result = 0;
	int spi_addr = 0;
	SWPA_CAMERA_CHECK(0 != data);
	SWPA_CAMERA_CHECK(0 < register_addr && register_addr < 0x1000);

	construct_spi_addr(&spi_addr, register_type, register_addr, 1);

	result = drv_fpga_read_single(dev_id, spi_addr, data);
	if (0 != result)
	{
		SWPA_CAMERA_PRINT("result=%d\n", result);
		return SWPAR_FAIL;
	}
	return SWPAR_OK;
}

#define FILL_FPGA_STRUCT(reg_type,addr,data)                   \
	result = fill_fpga_struct(&datas, reg_type, addr, data);   \
	if (SWPAR_OK != result)                                    \
	{                                                          \
		SWPA_CAMERA_PRINT("result=%d\n", result);                \
		return SWPAR_FAIL;                                     \
	}

#define DRV_FPGA_WRITE()                                       \
	result = drv_fpga_write(DEVID_SPI_FPGA, &datas);            \
	if (0 != result)                                           \
	{                                                          \
		SWPA_CAMERA_PRINT("result=%d\n", result);              \
		return SWPAR_FAIL;                                     \
	}

#define READ_REGISTER(reg_type,addr,variable)                           \
	result = read_register(DEVID_SPI_FPGA, reg_type, addr, variable);   \
	if (SWPAR_OK != result)                                             \
	{                                                                   \
		SWPA_CAMERA_PRINT("result=%d\n",result);                        \
		return SWPAR_FAIL;                                              \
	}

// 颜色矩阵合并算法
int ColorMartrixIntegrateAlg(
	int nContrast,
	int nSaturation,
	int nHue,
	int nColorTemperature,
	int nOutput[3][4]
);

/**
***********************
* 相机相关的转换
* swpa_camera_conver
***********************
**/

int swpa_camera_conver_shutter(
	int shutter
	)
{
	return (int)((float)shutter * SWPA_SHUTTER_UNIT);
}

int swpa_camera_conver_gain(
	int gain
	)
{
	return (int)((float)(gain) / (float)SWPA_MAX_GAIN * (float)SWPA_MAX_GAIN_API);
}

int swpa_camera_conver_rgb_gain(
	int rgb_gain
	)
{
	return rgb_gain / 2;
}


/**
* 内部变量
*/
static int swpa_gain_correction1 = 0;                             // 第1通道增益修正值
static int swpa_gain_correction2 = 0;                             // 第2通道增益修正值
static int swpa_gain_correction3 = 0;                             // 第3通道增益修正值
static int swpa_gain_correction4 = 0;                             // 第4通道增益修正值
static int swpa_capture_gain_correction1 = 0;                     // 抓拍第1通道增益修正值
static int swpa_capture_gain_correction2 = 0;                     // 抓拍第2通道增益修正值
static int swpa_capture_gain_correction3 = 0;                     // 抓拍第3通道增益修正值
static int swpa_capture_gain_correction4 = 0;                     // 抓拍第4通道增益修正值


/**
***********************
* 相机基本控制
* swpa_camera_basicparam_
***********************
*/

int swpa_camera_basicparam_set_shutter(
	int shutter
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("shutter=%d\n", shutter);
	SWPA_CAMERA_CHECK(shutter >= 0 && shutter <= SWPA_MAX_SHUTTER);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x909, (float)shutter / SWPA_SHUTTER_UNIT);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_shutter(
	int * shutter
)
{
	int result = 0;
	int reg_shutter = 0;

	SWPA_CAMERA_CHECK(shutter != 0);

	READ_REGISTER(REGISTER_FPGA, 0x909, &reg_shutter);

	*shutter = (float)reg_shutter * SWPA_SHUTTER_UNIT;

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_gain(
	int gain
)
{
	int result = 0;
	int register_gain = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("gain=%d\n", gain);
	SWPA_CAMERA_CHECK(gain >= 0 && gain <= SWPA_MAX_GAIN_API);

	register_gain = (int)( (float)gain / (float)SWPA_MAX_GAIN_API * (float)SWPA_MAX_GAIN ); // fpga寄存器的单位换算

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT( REGISTER_AD1, 0x5, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_gain_correction1) ) );

	if (SWPA_AD_CHANNELS > 1)
	{
		FILL_FPGA_STRUCT( REGISTER_AD2, 0x5, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_gain_correction2) ) );
		if (SWPA_AD_CHANNELS > 2)
		{
			FILL_FPGA_STRUCT( REGISTER_AD3, 0x5, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_gain_correction3) ) );
			FILL_FPGA_STRUCT( REGISTER_AD4, 0x5, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_gain_correction4) ) );
		}
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_gain(
	int * gain
)
{
	int result = 0;
	int reg_gain1 = 0; // 取一个通道的值减掉修正值即可

	SWPA_CAMERA_CHECK(gain != 0);

	READ_REGISTER(REGISTER_AD1, 0x5, &reg_gain1);

	*gain = (float)(reg_gain1 - swpa_gain_correction1) / (float)SWPA_MAX_GAIN * (float)SWPA_MAX_GAIN_API;

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_rgb_gain(
	int gain_r,
	int gain_g,
	int gain_b
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("gain_r=%d\n", gain_r);
	SWPA_CAMERA_PRINT("gain_g=%d\n", gain_g);
	SWPA_CAMERA_PRINT("gain_b=%d\n", gain_b);
	SWPA_CAMERA_CHECK(gain_r >= 0 && gain_r <= 255);
	SWPA_CAMERA_CHECK(gain_g >= 0 && gain_g <= 255);
	SWPA_CAMERA_CHECK(gain_b >= 0 && gain_b <= 255);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x90b, gain_r * 2); // 单位转换
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x90c, gain_g * 2);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x90d, gain_b * 2);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_rgb_gain(
	int * gain_r,
	int * gain_g,
	int * gain_b
)
{
	int result = 0;
	int reg_gain_r = 0;
	int reg_gain_g = 0;
	int reg_gain_b = 0;

	SWPA_CAMERA_CHECK(gain_r != 0);
	SWPA_CAMERA_CHECK(gain_g != 0);
	SWPA_CAMERA_CHECK(gain_b != 0);

	READ_REGISTER(REGISTER_FPGA, 0x90b, &reg_gain_r);
	READ_REGISTER(REGISTER_FPGA, 0x90c, &reg_gain_g);
	READ_REGISTER(REGISTER_FPGA, 0x90d, &reg_gain_b);

	*gain_r = reg_gain_r / 2;
	*gain_g = reg_gain_g / 2;
	*gain_b = reg_gain_b / 2;

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_clamp_level(
	int value1,
	int value2,
	int value3,
	int value4
)
{
	int result = 0;
	FPGA_STRUCT datas;

	swpa_memset(&datas, 0, sizeof(datas));

	SWPA_CAMERA_PRINT("value1=%d\n", value1);
	SWPA_CAMERA_CHECK(value1 >= 0 && value1 <= 1023);
	FILL_FPGA_STRUCT(REGISTER_AD1, 0x6, value1);
	if (SWPA_AD_CHANNELS > 1)
	{
		SWPA_CAMERA_PRINT("value2=%d\n", value2);
		SWPA_CAMERA_CHECK(value2 >= 0 && value2 <= 1023);
		FILL_FPGA_STRUCT(REGISTER_AD2, 0x6, value2);
		if (SWPA_AD_CHANNELS > 2)
		{
			SWPA_CAMERA_PRINT("value3=%d\n", value3);
			SWPA_CAMERA_PRINT("value4=%d\n", value4);
			SWPA_CAMERA_CHECK(value3 >= 0 && value3 <= 1023);
			SWPA_CAMERA_CHECK(value4 >= 0 && value4 <= 1023);
			FILL_FPGA_STRUCT(REGISTER_AD3, 0x6, value3);
			FILL_FPGA_STRUCT(REGISTER_AD4, 0x6, value4);
		}
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_clamp_level(
	int * value1,
	int * value2,
	int * value3,
	int * value4
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(value1 != 0);
	READ_REGISTER(REGISTER_AD1, 0x6, value1);
	if (SWPA_AD_CHANNELS > 1)
	{
		SWPA_CAMERA_CHECK(value2 != 0);
		READ_REGISTER(REGISTER_AD2, 0x6, value2);
		if (SWPA_AD_CHANNELS > 2)
		{
			SWPA_CAMERA_CHECK(value3 != 0);
			SWPA_CAMERA_CHECK(value4 != 0);
			READ_REGISTER(REGISTER_AD3, 0x6, value3);
			READ_REGISTER(REGISTER_AD4, 0x6, value4);
		}
	}

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_capture_shutter(
	int shutter
)
{
	int result = 0;
	FPGA_STRUCT datas;
	SWPA_REG_CAPTURE_EN_FLAG reg_old_capture_enable;
	reg_old_capture_enable.word = 0;

	SWPA_CAMERA_PRINT("shutter=%d\n", shutter);
	SWPA_CAMERA_CHECK(shutter >= -1 && shutter <= SWPA_MAX_SHUTTER);

	swpa_memset(&datas, 0, sizeof(datas));

	// 读取当前抓拍使能位的参数
	READ_REGISTER(REGISTER_FPGA, 0x914, &reg_old_capture_enable.word);

	if (shutter == -1)
	{
		if (reg_old_capture_enable.bits.shutter_enable == 0) // 原本不使能，不需调整
		{
			return SWPAR_OK;
		}
		else
		{ // 原本使能，则需要改成不使能
			reg_old_capture_enable.bits.shutter_enable = 0;
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x914, reg_old_capture_enable.word);
		}
	}
	else
	{
		if (reg_old_capture_enable.bits.shutter_enable == 0) // 原本不使能，则需要改成使能
		{
			reg_old_capture_enable.bits.shutter_enable = 1;
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x914, reg_old_capture_enable.word);
		}
		// 填入快门值
		FILL_FPGA_STRUCT(REGISTER_FPGA, 0x915, (float)shutter / SWPA_SHUTTER_UNIT);
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_capture_shutter(
	int * shutter
)
{
	int result = 0;
	int reg_capture_shutter = 0;
	SWPA_REG_CAPTURE_EN_FLAG reg_capture_enable;
	reg_capture_enable.word = 0;

	SWPA_CAMERA_CHECK(shutter != 0);

	READ_REGISTER(REGISTER_FPGA, 0x915, &reg_capture_shutter);
	READ_REGISTER(REGISTER_FPGA, 0x914, &reg_capture_enable.word);

	if (reg_capture_enable.bits.shutter_enable == 0)
	{
		*shutter = -1;
	}
	else
	{
		*shutter = (float)reg_capture_shutter * SWPA_SHUTTER_UNIT;
	}

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_capture_gain(
	int gain
)
{
	int result = 0;
	FPGA_STRUCT datas;
	int register_gain = 0;
	SWPA_REG_CAPTURE_EN_FLAG reg_old_capture_enable;
	reg_old_capture_enable.word = 0;

	SWPA_CAMERA_PRINT("gain=%d\n", gain);
	SWPA_CAMERA_CHECK(gain >= -1 && gain <= SWPA_MAX_GAIN_API);

	swpa_memset(&datas, 0, sizeof(datas));

	// 读取当前抓拍使能位的参数
	READ_REGISTER(REGISTER_FPGA, 0x914, &reg_old_capture_enable.word);
	
	if (gain == -1)
	{
		if (reg_old_capture_enable.bits.gain_enable == 0) // 原本不使能，不需调整
		{
			return SWPAR_OK;
		}
		else
		{ // 原本使能，则需要改成不使能
			reg_old_capture_enable.bits.gain_enable = 0;
			// todo. 设置黑电平独立 
			reg_old_capture_enable.bits.clamp_level_enable = 0;
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x914, reg_old_capture_enable.word);
		}
	}
	else
	{
		if (reg_old_capture_enable.bits.gain_enable == 0) // 原本不使能，则需要改成使能
		{
			reg_old_capture_enable.bits.gain_enable = 1;
			// todo. 设置黑电平独立 
			reg_old_capture_enable.bits.clamp_level_enable = 1;
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x914, reg_old_capture_enable.word);
		}

		// 填入增益值
		register_gain = (int)( (float)gain / (float)SWPA_MAX_GAIN_API * (float)SWPA_MAX_GAIN ); // fpga寄存器的单位换算

		FILL_FPGA_STRUCT( REGISTER_FPGA, 0x919, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_capture_gain_correction1) ) );
		if (SWPA_AD_CHANNELS > 1)
		{
			FILL_FPGA_STRUCT( REGISTER_FPGA, 0x91a, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_capture_gain_correction2) ) );
			if (SWPA_AD_CHANNELS > 2)
			{
				FILL_FPGA_STRUCT( REGISTER_FPGA, 0x91b, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_capture_gain_correction3) ) );
				FILL_FPGA_STRUCT( REGISTER_FPGA, 0x91c, MIN( SWPA_MAX_GAIN, MAX(0, register_gain + swpa_capture_gain_correction4) ) );
			}
		}
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_capture_gain(
	int * gain
)
{
	int result = 0;
	int reg_capture_gain1 = 0; // 取一个通道的值减掉修正值即可
	SWPA_REG_CAPTURE_EN_FLAG reg_capture_enable;
	reg_capture_enable.word = 0;

	SWPA_CAMERA_CHECK(gain != 0);

	READ_REGISTER(REGISTER_FPGA, 0x919, &reg_capture_gain1);
	READ_REGISTER(REGISTER_FPGA, 0x914, &reg_capture_enable.word);

	if (reg_capture_enable.bits.gain_enable == 0)
	{
		*gain = -1;
	}
	else
	{
		*gain = (float)(reg_capture_gain1 - swpa_capture_gain_correction1) / (float)SWPA_MAX_GAIN * (float)SWPA_MAX_GAIN_API;
	}

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_capture_rgb_gain(
	int gain_r,
	int gain_g,
	int gain_b
)
{
	int result = 0;
	FPGA_STRUCT datas;
	int same_with_noncapture = 0;
	SWPA_REG_CAPTURE_EN_FLAG reg_old_capture_enable;
	reg_old_capture_enable.word = 0;

	SWPA_CAMERA_PRINT("gain_r=%d\n", gain_r);
	SWPA_CAMERA_PRINT("gain_g=%d\n", gain_g);
	SWPA_CAMERA_PRINT("gain_b=%d\n", gain_b);
	SWPA_CAMERA_CHECK(gain_r >= -1 && gain_r <= 255);
	SWPA_CAMERA_CHECK(gain_g >= -1 && gain_g <= 255);
	SWPA_CAMERA_CHECK(gain_b >= -1 && gain_b <= 255);

	if ( -1 == gain_r || -1 == gain_g || -1 == gain_b )
	{
		same_with_noncapture = 1;
	}

	swpa_memset(&datas, 0, sizeof(datas));

	// 读取当前抓拍使能位的参数
	READ_REGISTER(REGISTER_FPGA, 0x914, &reg_old_capture_enable.word);

	if (1 == same_with_noncapture)
	{
		if (reg_old_capture_enable.bits.rgb_gain_enable == 0) // 原本不使能，不需调整
		{
			return SWPAR_OK;
		}
		else
		{ // 原本使能，则需要改成不使能
			reg_old_capture_enable.bits.rgb_gain_enable = 0;
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x914, reg_old_capture_enable.word);
		}
	}
	else
	{
		if (reg_old_capture_enable.bits.rgb_gain_enable == 0) // 原本不使能，则需要改成使能
		{
			reg_old_capture_enable.bits.rgb_gain_enable = 1;
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x914, reg_old_capture_enable.word);
		}

		FILL_FPGA_STRUCT(REGISTER_FPGA, 0x916, gain_r * 2); // 单位转换
		FILL_FPGA_STRUCT(REGISTER_FPGA, 0x917, gain_g * 2);
		FILL_FPGA_STRUCT(REGISTER_FPGA, 0x918, gain_b * 2);
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_capture_rgb_gain(
	int * gain_r,
	int * gain_g,
	int * gain_b
)
{
	int result = 0;
	int reg_capture_gain_r = 0;
	int reg_capture_gain_g = 0;
	int reg_capture_gain_b = 0;
	SWPA_REG_CAPTURE_EN_FLAG reg_capture_enable;
	reg_capture_enable.word = 0;

	SWPA_CAMERA_CHECK(gain_r != 0);
	SWPA_CAMERA_CHECK(gain_g != 0);
	SWPA_CAMERA_CHECK(gain_b != 0);

	READ_REGISTER(REGISTER_FPGA, 0x916, &reg_capture_gain_r);
	READ_REGISTER(REGISTER_FPGA, 0x917, &reg_capture_gain_g);
	READ_REGISTER(REGISTER_FPGA, 0x918, &reg_capture_gain_b);
	READ_REGISTER(REGISTER_FPGA, 0x914, &reg_capture_enable.word);

	if (reg_capture_enable.bits.rgb_gain_enable == 0)
	{
		*gain_r = -1;
		*gain_g = -1;
		*gain_b = -1;
	}
	else
	{
		*gain_r = reg_capture_gain_r / 2;
		*gain_g = reg_capture_gain_g / 2;
		*gain_b = reg_capture_gain_b / 2;
	}

	return SWPAR_OK;
}

int swpa_camera_basicparam_set_capture_clamp_level(
	int value1,
	int value2,
	int value3,
	int value4
)
{
	int result = 0;
	FPGA_STRUCT datas;

	swpa_memset(&datas, 0, sizeof(datas));

	SWPA_CAMERA_PRINT("value1=%d\n", value1);
	SWPA_CAMERA_CHECK(value1 >= 0 && value1 <= 1023);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x91d, value1);
	if (SWPA_AD_CHANNELS > 1)
	{
		SWPA_CAMERA_PRINT("value2=%d\n", value2);
		SWPA_CAMERA_CHECK(value2 >= 0 && value2 <= 1023);
		FILL_FPGA_STRUCT(REGISTER_FPGA, 0x91e, value2);
		if (SWPA_AD_CHANNELS > 2)
		{
			SWPA_CAMERA_PRINT("value3=%d\n", value3);
			SWPA_CAMERA_PRINT("value4=%d\n",value4);
			SWPA_CAMERA_CHECK(value3 >= 0 && value3 <= 1023);
			SWPA_CAMERA_CHECK(value4 >= 0 && value4 <= 1023);
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x91f, value3);
			FILL_FPGA_STRUCT(REGISTER_FPGA, 0x920, value4);
		}
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_basicparam_get_capture_clamp_level(
	int * value1,
	int * value2,
	int * value3,
	int * value4
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(value1 != 0);
	READ_REGISTER(REGISTER_FPGA, 0x91d, value1);
	if (SWPA_AD_CHANNELS > 1)
	{
		SWPA_CAMERA_CHECK(value2 != 0);
		READ_REGISTER(REGISTER_FPGA, 0x91e, value2);
		if (SWPA_AD_CHANNELS > 2)
		{
			SWPA_CAMERA_CHECK(value3 != 0);
			SWPA_CAMERA_CHECK(value4 != 0);
			READ_REGISTER(REGISTER_FPGA, 0x91f, value3);
			READ_REGISTER(REGISTER_FPGA, 0x920, value4);
		}
	}

	return SWPAR_OK;
}

/**
***********************
* 相机输入输出控制
* swpa_camera_io_
***********************
*/


int swpa_camera_io_soft_capture(
	int flash_set,
	int info
	)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("flash_set=%d\n", flash_set);
	SWPA_CAMERA_PRINT("info=%d\n", info);
	SWPA_CAMERA_CHECK(flash_set >= 0 && flash_set <= 7);

	swpa_memset(&datas, 0, sizeof(datas));
	// 设置闪光灯使能信息
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x92f, flash_set);

	// 软触发
	// 传入info
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x913, info);
	// 先置1
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x912, 1);
	// 再置0
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x912, 0);


	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_clear_time()
{
	int result = 0;
	FPGA_STRUCT datas;

	swpa_memset(&datas, 0, sizeof(datas));

	// 先置1
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x92d, 1);
	// 再置0
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x92d, 0);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_set_capture_param(
	int capture_edge_mode1,
	int capture_edge_mode2,
	int flash_set_1,
	int flash_set_2
	)
{
	int result = 0;
	FPGA_STRUCT datas;
	SWPA_REG_CAPTURE_EDGE_EXT reg_capture_edge_mode;
	reg_capture_edge_mode.word = 0;

	SWPA_CAMERA_PRINT("capture_edge_mode1=%d\n", capture_edge_mode1);
	SWPA_CAMERA_PRINT("capture_edge_mode2=%d\n", capture_edge_mode2);
	SWPA_CAMERA_PRINT("lane_differentiation=%d\n", lane_differentiation);
	SWPA_CAMERA_CHECK(capture_edge_mode1 >= 0 && capture_edge_mode1 <= 3);
	SWPA_CAMERA_CHECK(capture_edge_mode2 >= 0 && capture_edge_mode2 <= 3);
	SWPA_CAMERA_CHECK(lane_differentiation >= 0 && lane_differentiation <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	reg_capture_edge_mode.bits.capture_edge1 = capture_edge_mode1;
	reg_capture_edge_mode.bits.capture_edge2 = capture_edge_mode2;

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x921, reg_capture_edge_mode.word);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x930, flash_set_1);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x931, flash_set_2);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_capture_param(
	int * capture_edge_mode1,
	int * capture_edge_mode2,
	int * lane_differentiation
)
{
	int result = 0;
	SWPA_REG_CAPTURE_EDGE_EXT reg_capture_edge_mode;
	reg_capture_edge_mode.word = 0;

	SWPA_CAMERA_CHECK(capture_edge_mode1 != 0);
	SWPA_CAMERA_CHECK(capture_edge_mode2 != 0);
	SWPA_CAMERA_CHECK(lane_differentiation != 0);

	READ_REGISTER(REGISTER_FPGA, 0x921, &reg_capture_edge_mode.word);
	READ_REGISTER(REGISTER_FPGA, 0x922, lane_differentiation);

	*capture_edge_mode1 = reg_capture_edge_mode.bits.capture_edge1;
	*capture_edge_mode2 = reg_capture_edge_mode.bits.capture_edge2;

	return SWPAR_OK;
}

int swpa_camera_io_set_iocapture_param(
	int lane_differentiation
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("lane_differentiation=%d\n", lane_differentiation);
	SWPA_CAMERA_CHECK(lane_differentiation >= 0 && lane_differentiation <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x92e, lane_differentiation);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_iocapture_param(
	int * lane_differentiation
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(lane_differentiation != 0);

	READ_REGISTER(REGISTER_FPGA, 0x92e, lane_differentiation);

	return SWPAR_OK;
}

int swpa_camera_io_set_framerate_mode(
	int mode,
	int external_sync_delay
)
{
	int result = 0;
	FPGA_STRUCT datas;
	int work_mode = 0;
	SWPA_REG_SYN_AC_EXT reg_syn_ac_ext;
	reg_syn_ac_ext.word = 0;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_PRINT("external_sync_delay=%d\n", external_sync_delay);
	SWPA_CAMERA_CHECK(mode >= 1 && mode <= 4);
	SWPA_CAMERA_CHECK(external_sync_delay >= 0 && external_sync_delay <= 200);

	swpa_memset(&datas, 0, sizeof(datas));

	switch (mode)
	{
		case 1:
			reg_syn_ac_ext.bits.external_sync_enable = 0;
			reg_syn_ac_ext.bits.internal_sync_enable = 1;
			work_mode = 0;
			break;
		case 2:
			reg_syn_ac_ext.bits.external_sync_enable = 1;
			reg_syn_ac_ext.bits.internal_sync_enable = 0;
			work_mode = 0;
			break;
		case 3:
			reg_syn_ac_ext.bits.external_sync_enable = 0;
			reg_syn_ac_ext.bits.internal_sync_enable = 0;
			work_mode = 0;
			break;
		case 4:
			reg_syn_ac_ext.bits.external_sync_enable = 0;
			reg_syn_ac_ext.bits.internal_sync_enable = 0;
			work_mode = 1;
			break;
	}
	reg_syn_ac_ext.bits.sync_signal_delay = external_sync_delay;

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x908, reg_syn_ac_ext.word);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x907, work_mode);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_framerate_mode(
	int * mode,
	int * external_sync_delay
)
{
	int result = 0;
	int capture_edge_mode = 0;
	int work_mode = 0;
	SWPA_REG_SYN_AC_EXT reg_syn_ac_ext;
	reg_syn_ac_ext.word = 0;

	SWPA_CAMERA_CHECK(mode != 0);
	SWPA_CAMERA_CHECK(external_sync_delay != 0);

	READ_REGISTER(REGISTER_FPGA, 0x908, &reg_syn_ac_ext.word);
	READ_REGISTER(REGISTER_FPGA, 0x907, &work_mode);

	*external_sync_delay = reg_syn_ac_ext.bits.sync_signal_delay;
	if (reg_syn_ac_ext.bits.external_sync_enable == 1)
	{
		*mode = 2;
	}
	else if (reg_syn_ac_ext.bits.internal_sync_enable == 1)
	{
		*mode = 1;
	}
	else if (work_mode == 0)
	{
		*mode = 3;
	}
	else
	{
		*mode = 4;
	}

	return SWPAR_OK;
}

int swpa_camera_io_set_strobe_signal(
	int enable
)
{
	int result = 0;
	FPGA_STRUCT datas;
	SWPA_REG_LED_SET reg_old_strobe_setting;
	reg_old_strobe_setting.word = 0;

	SWPA_CAMERA_PRINT("enable=%d\n", enable);
	SWPA_CAMERA_CHECK(enable >= 0 && enable <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	// 读取当前频闪灯的参数
	READ_REGISTER(REGISTER_FPGA, 0x923, &reg_old_strobe_setting.word);

	// 填入使能信息
	reg_old_strobe_setting.bits.enable = enable;

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x923, reg_old_strobe_setting.word);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_strobe_signal(
	int * enable
)
{
	int result = 0;
	SWPA_REG_LED_SET reg_strobe_setting;
	reg_strobe_setting.word = 0;

	SWPA_CAMERA_CHECK(enable != 0);

	READ_REGISTER(REGISTER_FPGA, 0x923, &reg_strobe_setting.word);

	*enable = reg_strobe_setting.bits.enable;

	return SWPAR_OK;
}

int swpa_camera_io_set_flash_signal(
	int enable_flash_set
)
{
	int result = 0;
	FPGA_STRUCT datas;
	SWPA_REG_FLASH_SET reg_old_flash_setting1;
	SWPA_REG_FLASH_SET reg_old_flash_setting2;
	SWPA_REG_FLASH_SET reg_old_flash_setting3;
	reg_old_flash_setting1.word = 0;
	reg_old_flash_setting2.word = 0;
	reg_old_flash_setting3.word = 0;

	SWPA_CAMERA_PRINT("enable_flash_set=%d\n", enable_flash_set);
	SWPA_CAMERA_CHECK(enable_flash_set >= 0 && enable_flash_set <= 7);

	// 读取当前各闪光灯的参数
	READ_REGISTER(REGISTER_FPGA, 0x926, &reg_old_flash_setting1.word);
	READ_REGISTER(REGISTER_FPGA, 0x927, &reg_old_flash_setting2.word);
	READ_REGISTER(REGISTER_FPGA, 0x928, &reg_old_flash_setting3.word);

	// 调整各闪光灯的使能位
	swpa_memset(&datas, 0, sizeof(datas));

	reg_old_flash_setting1.bits.enable = (enable_flash_set & 0x1) ? 1 : 0;
	reg_old_flash_setting2.bits.enable = (enable_flash_set & 0x2) ? 1 : 0;
	reg_old_flash_setting3.bits.enable = (enable_flash_set & 0x4) ? 1 : 0;

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x926, reg_old_flash_setting1.word);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x927, reg_old_flash_setting2.word);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x928, reg_old_flash_setting3.word);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_flash_signal(
	int * enable_flash_set
)
{
	int result = 0;
	SWPA_REG_FLASH_SET reg_flash_setting1;
	SWPA_REG_FLASH_SET reg_flash_setting2;
	SWPA_REG_FLASH_SET reg_flash_setting3;
	reg_flash_setting1.word = 0;
	reg_flash_setting2.word = 0;
	reg_flash_setting3.word = 0;

	SWPA_CAMERA_CHECK(enable_flash_set != 0);

	READ_REGISTER(REGISTER_FPGA, 0x926, &reg_flash_setting1.word);
	READ_REGISTER(REGISTER_FPGA, 0x927, &reg_flash_setting2.word);
	READ_REGISTER(REGISTER_FPGA, 0x928, &reg_flash_setting3.word);

	*enable_flash_set = 0;
	if (reg_flash_setting1.bits.enable == 1)
	{
		*enable_flash_set |= 0x1;
	}
	if (reg_flash_setting2.bits.enable == 1)
	{
		*enable_flash_set |= 0x2;
	}
	if (reg_flash_setting3.bits.enable == 1)
	{
		*enable_flash_set |= 0x4;
	}

	return SWPAR_OK;
}

int swpa_camera_io_set_strobe_param(
	int multiplication,
	int polarity,
	int pulse_width,
	int output_type,
	int enable_when_capture
)
{
	int result = 0;
	FPGA_STRUCT datas;
	SWPA_REG_LED_SET reg_old_strobe_setting;
	reg_old_strobe_setting.word = 0;

	SWPA_CAMERA_PRINT("multiplication=%d\n", multiplication);
	SWPA_CAMERA_PRINT("polarity=%d\n", polarity);
	SWPA_CAMERA_PRINT("pulse_width=%d\n", pulse_width);
	SWPA_CAMERA_PRINT("output_type=%d\n", output_type);
	SWPA_CAMERA_PRINT("enable_when_capture=%d\n", enable_when_capture);
	SWPA_CAMERA_CHECK(multiplication >= 0 && multiplication <= 1);
	SWPA_CAMERA_CHECK(polarity >= 0 && polarity <= 1);
	SWPA_CAMERA_CHECK(pulse_width >= 1 && pulse_width <= 200);
	SWPA_CAMERA_CHECK(output_type >= 0 && output_type <= 1);
	SWPA_CAMERA_CHECK(enable_when_capture >= 0 && enable_when_capture <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	// 读取当前频闪灯的参数
	READ_REGISTER(REGISTER_FPGA, 0x923, &reg_old_strobe_setting.word);

	// 填入频闪灯的参数
	reg_old_strobe_setting.bits.multiplication = multiplication;
	reg_old_strobe_setting.bits.polarity = polarity;
	reg_old_strobe_setting.bits.pulse_width = pulse_width;
	reg_old_strobe_setting.bits.output_type = output_type;
	reg_old_strobe_setting.bits.enable_when_capture = enable_when_capture;


	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x923, reg_old_strobe_setting.word);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_strobe_param(
	int * multiplication,
	int * polarity,
	int * pulse_width,
	int * output_type,
	int * enable_when_capture
)
{
	int result = 0;
	SWPA_REG_LED_SET reg_strobe_setting;
	reg_strobe_setting.word = 0;

	SWPA_CAMERA_CHECK(multiplication != 0);
	SWPA_CAMERA_CHECK(polarity != 0);
	SWPA_CAMERA_CHECK(pulse_width != 0);
	SWPA_CAMERA_CHECK(output_type != 0);
	SWPA_CAMERA_CHECK(enable_when_capture != 0);

	READ_REGISTER(REGISTER_FPGA, 0x923, &reg_strobe_setting.word);

	*multiplication = reg_strobe_setting.bits.multiplication;
	*polarity = reg_strobe_setting.bits.polarity;
	*pulse_width = reg_strobe_setting.bits.pulse_width;
	*output_type = reg_strobe_setting.bits.output_type;
	*enable_when_capture = reg_strobe_setting.bits.enable_when_capture;

	return SWPAR_OK;
}

int swpa_camera_io_set_flash_param(
	int id,
	int coupling,
	int polarity,
	int pulse_width,
	int output_type
)
{
	int result = 0;
	FPGA_STRUCT datas;
	SWPA_REG_FLASH_SET reg_old_flash_setting;
	reg_old_flash_setting.word = 0;

	SWPA_CAMERA_PRINT("id=%d\n", id);
	SWPA_CAMERA_PRINT("coupling=%d\n", coupling);
	SWPA_CAMERA_PRINT("polarity=%d\n", polarity);
	SWPA_CAMERA_PRINT("pulse_width=%d\n", pulse_width);
	SWPA_CAMERA_PRINT("output_type=%d\n", output_type);
	SWPA_CAMERA_CHECK(id >= 1 && id <= 3);
	SWPA_CAMERA_CHECK(coupling >= 0 && coupling <= 1);
	SWPA_CAMERA_CHECK(polarity >= 0 && polarity <= 1);
	SWPA_CAMERA_CHECK(pulse_width >= 1 && pulse_width <= 200);
	SWPA_CAMERA_CHECK(output_type >= 0 && output_type <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	// 读取当前频闪灯的参数
	READ_REGISTER(REGISTER_FPGA, 0x925 + id, &reg_old_flash_setting.word);

	// 填入频闪灯的参数
	reg_old_flash_setting.bits.coupling = coupling;
	reg_old_flash_setting.bits.polarity = polarity;
	reg_old_flash_setting.bits.pulse_width = pulse_width;
	reg_old_flash_setting.bits.output_type = output_type;

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x925 + id, reg_old_flash_setting.word);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_flash_param(
	int id,
	int * coupling,
	int * polarity,
	int * pulse_width,
	int * output_type
)
{
	int result = 0;
	SWPA_REG_FLASH_SET reg_flash_setting;
	reg_flash_setting.word = 0;

	SWPA_CAMERA_PRINT("id=%d\n", id);
	SWPA_CAMERA_CHECK(id >= 1 && id <= 3);
	SWPA_CAMERA_CHECK(coupling != 0);
	SWPA_CAMERA_CHECK(polarity != 0);
	SWPA_CAMERA_CHECK(pulse_width != 0);
	SWPA_CAMERA_CHECK(output_type != 0);

	READ_REGISTER(REGISTER_FPGA, 0x925 + id, &reg_flash_setting.word);

	*coupling = reg_flash_setting.bits.coupling;
	*polarity = reg_flash_setting.bits.polarity;
	*pulse_width = reg_flash_setting.bits.pulse_width;
	*output_type = reg_flash_setting.bits.output_type;

	return SWPAR_OK;
}

int swpa_camera_io_set_lamp(
	int enable
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("enable=%d\n", enable);
	SWPA_CAMERA_CHECK(enable >= 0 && enable <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x92b, enable);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_lamp(
	int * enable
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(enable != 0);

	READ_REGISTER(REGISTER_FPGA, 0x92b, enable);

	return SWPAR_OK;
}

int swpa_camera_io_control_dc_iris(
	int mode
)
{
	int result = 0;
	FPGA_STRUCT datas;
	int reg_mode = mode;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_CHECK(mode >= 0 && mode <= 2);

	switch (mode)
	{
		case 0: // 停止
			reg_mode = 2;
			break;
		case 1: // 打开
			reg_mode = 0;
			break;
		case 2: // 关闭
			reg_mode = 3;
			break;
	}

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x929, reg_mode);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_control_filter(
	int mode
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_CHECK(mode >= 0 && mode <= 2);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x92a, mode);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_set_alarm_zone(
	int x1, int y1,
	int x2, int y2
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("x1=%d\n", x1);
	SWPA_CAMERA_PRINT("y1=%d\n", y1);
	SWPA_CAMERA_PRINT("x2=%d\n", x2);
	SWPA_CAMERA_PRINT("y2=%d\n", y2);
	SWPA_CAMERA_CHECK(x1 >= 0 && x1 <= 2047);
	SWPA_CAMERA_CHECK(y1 >= 0 && y1 <= 2047);
	SWPA_CAMERA_CHECK(x2 >= 0 && x2 <= 2047);
	SWPA_CAMERA_CHECK(y2 >= 0 && y2 <= 2047);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa1c, x1);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa1e, y1);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa1d, x2);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa1f, y2);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_io_get_alarm_zone(
	int * x1, int * y1,
	int * x2, int * y2
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(x1 != 0);
	SWPA_CAMERA_CHECK(y1 != 0);
	SWPA_CAMERA_CHECK(x2 != 0);
	SWPA_CAMERA_CHECK(y2 != 0);

	READ_REGISTER(REGISTER_FPGA, 0xa1c, x1);
	READ_REGISTER(REGISTER_FPGA, 0xa1e, y1);
	READ_REGISTER(REGISTER_FPGA, 0xa1d, x2);
	READ_REGISTER(REGISTER_FPGA, 0xa1f, y2);

	return SWPAR_OK;
}


int swpa_camera_io_get_barrier_state(int* pstate)
{
	int result = 0;
		
	SWPA_CAMERA_CHECK(pstate != 0);

	READ_REGISTER(REGISTER_FPGA, 0xb00, pstate);

	return SWPAR_OK;
}

int swpa_camera_io_backupio_enable(int enable)
{
	int result = 0;
	
	FPGA_STRUCT datas;

	unsigned int value = 0;

	if (0 != enable)
	{

		READ_REGISTER(REGISTER_FPGA, 0x921, &value);

		value |= 0xC;

		swpa_memset(&datas, 0, sizeof(datas));
		FILL_FPGA_STRUCT(REGISTER_FPGA, 0x921, value);//第二路不用作触发
		DRV_FPGA_WRITE();
	}

	return SWPAR_OK;
}


/**
***********************
* 相机图像处理
* swpa_camera_imgproc_
***********************
*/
int swpa_camera_imgproc_set_gamma(
	int data[8][2]
)
{
	int result = 0;
	FPGA_STRUCT datas;
    static int gamma_y[4096] = {0};
    int x = 0, y = 0;
    int x1 = 0, y1 = 0;
    int x2 = 0, y2 = 0;
    int tmp = 0;
	int i = 0, j = 0;

//	SWPA_CAMERA_CHECK(enable >= 0 && enable <= 1);
	swpa_memset(&datas, 0, sizeof(datas));

    /// 8个点
    for (i = 0; i < 8; i++)
    {
        /// 第一个点
        if (i == 0)     /// 补回原点(0, 0)
        {
            x1 = 0;
            y1 = 0;
        }
        else
        {
            x1 = data[i - 1][0] * 4095 / 255;
            y1 = data[i - 1][1] * 4095 / 255;
        }
        /// 第二个点
        x2 = data[i][0] * 4095 / 255;
        y2 = data[i][1] * 4095 / 255;

        /// 计算两点间(包含这两点)所有的Y值
        for (x = x1; x <= x2; x++)
        {
            y = (y2 - y1) * (x - x1) / (x2 - x1) + y1;
            gamma_y[x] = y;
        }
    }
    /// 注：第1个值与最后1个值不用写到FPGA中
    for (j = 1; j < 4095; j += 2)
    {
        tmp = (gamma_y[j+1]<<12) | gamma_y[j];

		FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa02, tmp);

		DRV_FPGA_WRITE();

        datas.count = 0;
    }

    // 写完gamma值后，须向0xa03写0表示结束
    FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa03, 0);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_gamma(
	int data[8][2]
)
{
	return SWPAR_NOTIMPL;
}

int swpa_camera_imgproc_set_gamma_mode(
	int mode
	)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_CHECK(mode >= 0 && mode <= 3);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xA22, mode);
	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_gamma_mode(
	int* mode
	)
{
	int result = 0;
	SWPA_CAMERA_CHECK(mode != 0);

	READ_REGISTER(REGISTER_FPGA, 0xA22, mode);

	return SWPAR_OK;
}

int swpa_camera_imgproc_set_trafficlight_enhance_param(
	int h_threshold,
	int l_threshold,
	int s_threshold,
	int color_factor,
	int lum_l_threshold,
	int lum_l_factor,
	int lum_h_factor
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("h_threshold=%d\n", h_threshold);
	SWPA_CAMERA_PRINT("l_threshold=%d\n", l_threshold);
	SWPA_CAMERA_PRINT("s_threshold=%d\n", s_threshold);
	SWPA_CAMERA_PRINT("color_factor=%d\n", color_factor);
	SWPA_CAMERA_PRINT("lum_l_threshold=%d\n", lum_l_threshold);
	SWPA_CAMERA_PRINT("lum_l_factor=%d\n", lum_l_factor);
	SWPA_CAMERA_PRINT("lum_h_factor=%d", lum_h_factor);
	SWPA_CAMERA_CHECK(h_threshold >= 0 && h_threshold <= 184320);
	SWPA_CAMERA_CHECK(l_threshold >= 0 && l_threshold <= 16383);
	SWPA_CAMERA_CHECK(s_threshold >= 0 && s_threshold <= 16383);
	SWPA_CAMERA_CHECK(color_factor >= 0 && color_factor <= 511);
	SWPA_CAMERA_CHECK(lum_l_threshold >= 0 && lum_l_threshold <= 16383);
	SWPA_CAMERA_CHECK(lum_l_factor >= 0 && lum_l_factor <= 511);
	SWPA_CAMERA_CHECK(lum_h_factor >= 0 && lum_h_factor <= 511);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa08, h_threshold);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa09, l_threshold);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa0a, s_threshold);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa0b, color_factor);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa0c, lum_l_threshold);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa0d, lum_l_factor);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa0e, lum_h_factor);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_trafficlight_enhance_param(
	int * h_threshold,
	int * l_threshold,
	int * s_threshold,
	int * color_factor,
	int * lum_l_threshold,
	int * lum_l_factor,
	int * lum_h_factor
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(h_threshold != 0);
	SWPA_CAMERA_CHECK(l_threshold != 0);
	SWPA_CAMERA_CHECK(s_threshold != 0);
	SWPA_CAMERA_CHECK(color_factor != 0);
	SWPA_CAMERA_CHECK(lum_l_threshold != 0);
	SWPA_CAMERA_CHECK(lum_l_factor != 0);
	SWPA_CAMERA_CHECK(lum_h_factor != 0);

	READ_REGISTER(REGISTER_FPGA, 0xa08, h_threshold);
	READ_REGISTER(REGISTER_FPGA, 0xa09, l_threshold);
	READ_REGISTER(REGISTER_FPGA, 0xa0a, s_threshold);
	READ_REGISTER(REGISTER_FPGA, 0xa0b, color_factor);
	READ_REGISTER(REGISTER_FPGA, 0xa0c, lum_l_threshold);
	READ_REGISTER(REGISTER_FPGA, 0xa0d, lum_l_factor);
	READ_REGISTER(REGISTER_FPGA, 0xa0e, lum_h_factor);

	return SWPAR_OK;
}

int swpa_camera_imgproc_set_trafficlight_enhance_zone(
	int id,
	int x1, int y1,
	int x2, int y2
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("id=%d\n", id);
	SWPA_CAMERA_PRINT("x1=%d\n", x1);
	SWPA_CAMERA_PRINT("y1=%d\n", y1);
	SWPA_CAMERA_PRINT("x2=%d\n", x2);
	SWPA_CAMERA_PRINT("y2=%d\n", y2);
	SWPA_CAMERA_CHECK(id >= 1 && id <= 8);
	SWPA_CAMERA_CHECK(x1 >= 0 && x1 <= 4095);
	SWPA_CAMERA_CHECK(y1 >= 0 && y1 <= 4095);
	SWPA_CAMERA_CHECK(x2 >= 0 && x2 <= 4095);
	SWPA_CAMERA_CHECK(y2 >= 0 && y2 <= 4095);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa10, x1);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa12, y1);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa11, x2);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa13, y2);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa0f, id); // 确保在传完坐标寄存器后，才传ID
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x988, 0); // 最后要设置0x988寄存器，任意值。

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_trafficlight_enhance_zone(
	int id,
	int * x1, int * y1,
	int * x2, int * y2
)
{
	return SWPAR_NOTIMPL;
}

int swpa_camera_imgproc_set_color_param(
	int mode,
	int contrast_value,
	int saturation_value,
	int color_temperature_value,
	int hue_value
)
{
	int result = 0;
	FPGA_STRUCT datas;
	int color_matrix[3][4];
	int i = 0, j = 0;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_PRINT("contrast_value=%d\n", contrast_value);
	SWPA_CAMERA_PRINT("saturation_value=%d\n", saturation_value);
	SWPA_CAMERA_PRINT("color_temperature_value=%d\n", color_temperature_value);
	SWPA_CAMERA_PRINT("hue_value=%d\n", hue_value);
	SWPA_CAMERA_CHECK(mode >= 1 && mode <= 3);
	SWPA_CAMERA_CHECK(contrast_value >= -100 && contrast_value <= 100); // TODO: 确定取值范围
	SWPA_CAMERA_CHECK(saturation_value >= -100 && saturation_value <= 100);
	SWPA_CAMERA_CHECK(color_temperature_value >= -100 && color_temperature_value <= 100);
	SWPA_CAMERA_CHECK(hue_value >= -100 && hue_value <= 100);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa1a, mode);

	if (mode != 1)
	{
		ColorMartrixIntegrateAlg(contrast_value,
			saturation_value,
			hue_value,
			color_temperature_value,
			color_matrix);

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 4; j++)
			{
				FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa19, color_matrix[i][j]);
			}
		}
	}

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_color_param(
	int * mode,
	int * contrast_value,
	int * saturation_value,
	int * color_temperature_value,
	int * hue_value
)
{
	return SWPAR_NOTIMPL;
}

int swpa_camera_imgproc_set_sharpen_param(
	int mode,
	int threshold
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_PRINT("threshold=%d\n", threshold);
	SWPA_CAMERA_CHECK(mode >= 1 && mode <= 3);
	SWPA_CAMERA_CHECK(threshold >= 0 && threshold <= 127);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa17, mode);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa18, threshold);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_sharpen_param(
	int * mode,
	int * threshold
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(mode != 0);
	SWPA_CAMERA_CHECK(threshold != 0);

	READ_REGISTER(REGISTER_FPGA, 0xa17, mode);
	READ_REGISTER(REGISTER_FPGA, 0xa18, threshold);

	return SWPAR_OK;
}

int swpa_camera_imgproc_set_defog_param(
	int mode,
	int factor
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_PRINT("factor=%d\n", factor);
	SWPA_CAMERA_CHECK(mode >= 1 && mode <= 3);
	SWPA_CAMERA_CHECK(factor >= 1 && factor <= 10);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa06, mode);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa07, (float)factor / 10.0 * 1023);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_defog_param(
	int * mode,
	int * factor
)
{
	int result = 0;
	int reg_factor = 0;

	SWPA_CAMERA_CHECK(mode != 0);
	SWPA_CAMERA_CHECK(factor != 0);

	READ_REGISTER(REGISTER_FPGA, 0xa06, mode);
	READ_REGISTER(REGISTER_FPGA, 0xa07, &reg_factor);

	*factor = MAX( 1, (float)reg_factor / (float)1023 * 10.0 );

	return SWPAR_OK;
}

int swpa_camera_imgproc_set_color_black(
	int mode
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("mode=%d\n", mode);
	SWPA_CAMERA_CHECK(mode >= 0 && mode <= 1);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa05, mode);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_imgproc_get_color_black(
	int * mode
)
{
	int result = 0;

	SWPA_CAMERA_CHECK(mode != 0);

	READ_REGISTER(REGISTER_FPGA, 0xa05, mode);

	return SWPAR_OK;
}

/**
***********************
* 相机初始参数设置（如采样点、前端像素增益等）,等补充
* swpa_camera_init_
***********************
*/

int swpa_camera_init_set_raw_rgb_gain(
	int gain_r,
	int gain_gr,
	int gain_gb,
	int gain_b
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("gain_r=%d\n", gain_r);
	SWPA_CAMERA_PRINT("gain_gr=%d\n", gain_gr);
	SWPA_CAMERA_PRINT("gain_gb=%d\n", gain_gb);
	SWPA_CAMERA_PRINT("gain_b=%d\n", gain_b);
	SWPA_CAMERA_CHECK(gain_r >= 0 && gain_r <= 131071);
	SWPA_CAMERA_CHECK(gain_gr >= 0 && gain_gr <= 131071);
	SWPA_CAMERA_CHECK(gain_gb >= 0 && gain_gb <= 131071);
	SWPA_CAMERA_CHECK(gain_b >= 0 && gain_b <= 131071);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x90e, gain_r);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x90f, gain_gr);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x910, gain_gb);
	FILL_FPGA_STRUCT(REGISTER_FPGA, 0x911, gain_b);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_init_set_gain_correction(
	int gain_correction1,
	int gain_correction2,
	int gain_correction3,
	int gain_correction4
)
{
	SWPA_CAMERA_PRINT("gain_correction1", gain_correction1);
	SWPA_CAMERA_PRINT("gain_correction2", gain_correction2);
	SWPA_CAMERA_PRINT("gain_correction3", gain_correction3);
	SWPA_CAMERA_PRINT("gain_correction4", gain_correction4);
	SWPA_CAMERA_CHECK(gain_correction1 >= -1023 && gain_correction1 <= 1023);
	SWPA_CAMERA_CHECK(gain_correction2 >= -1023 && gain_correction2 <= 1023);
	SWPA_CAMERA_CHECK(gain_correction3 >= -1023 && gain_correction3 <= 1023);
	SWPA_CAMERA_CHECK(gain_correction4 >= -1023 && gain_correction4 <= 1023);

	swpa_gain_correction1 = gain_correction1;
	swpa_gain_correction2 = gain_correction2;
	swpa_gain_correction3 = gain_correction3;
	swpa_gain_correction4 = gain_correction4;

	return SWPAR_OK;
}

int swpa_camera_init_set_capture_gain_correction(
	int gain_correction1,
	int gain_correction2,
	int gain_correction3,
	int gain_correction4
)
{
	SWPA_CAMERA_PRINT("gain_correction1", gain_correction1);
	SWPA_CAMERA_PRINT("gain_correction2", gain_correction2);
	SWPA_CAMERA_PRINT("gain_correction3", gain_correction3);
	SWPA_CAMERA_PRINT("gain_correction4", gain_correction4);
	SWPA_CAMERA_CHECK(gain_correction1 >= -1023 && gain_correction1 <= 1023);
	SWPA_CAMERA_CHECK(gain_correction2 >= -1023 && gain_correction2 <= 1023);
	SWPA_CAMERA_CHECK(gain_correction3 >= -1023 && gain_correction3 <= 1023);
	SWPA_CAMERA_CHECK(gain_correction4 >= -1023 && gain_correction4 <= 1023);

	swpa_capture_gain_correction1 = gain_correction1;
	swpa_capture_gain_correction2 = gain_correction2;
	swpa_capture_gain_correction3 = gain_correction3;
	swpa_capture_gain_correction4 = gain_correction4;

	return SWPAR_OK;
}

int swpa_camera_init_set_dead_pixel_removal(
	int threshold
)
{
	int result = 0;
	FPGA_STRUCT datas;

	SWPA_CAMERA_PRINT("threshold=%d\n", threshold);
	SWPA_CAMERA_CHECK(threshold >= 0 && threshold <= 16383);

	swpa_memset(&datas, 0, sizeof(datas));

	FILL_FPGA_STRUCT(REGISTER_FPGA, 0xa20, threshold);

	DRV_FPGA_WRITE();

	return SWPAR_OK;
}

int swpa_camera_init_get_device_dependent_info(
	int * max_shutter,
	int * max_gain,
	int * ad_channels
)
{
	SWPA_CAMERA_CHECK(max_shutter != 0);
	SWPA_CAMERA_CHECK(max_gain != 0);
	SWPA_CAMERA_CHECK(ad_channels != 0);

	*max_shutter = SWPA_MAX_SHUTTER;
	*max_gain = SWPA_MAX_GAIN_API;
	*ad_channels = SWPA_AD_CHANNELS;

	return SWPAR_OK;
}


#ifdef __cplusplus
}
#endif


