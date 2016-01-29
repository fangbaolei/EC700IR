/**
* @file  swpa_camera.c
* @brief  相机相关
* @author lujy@signalway.com.cn
* @date  2014-6-30
* @version  V0.1
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "swpa.h"
#include "swpa_camera.h"
#include "visca_protocol.h"
#include "pelco_protocol.h"

#ifdef SWPA_CAMERA
#define SWPA_CAMERA_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_CAMERA_CHECK(arg)      {if (!(arg)){SWPA_CAMERA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_CAMERA_PRINT(fmt, ...)
#define SWPA_CAMERA_CHECK(arg)
#endif

// 连接机芯的串口设备名称
#define CAMERA_DEVICE "/dev/ttyO3"
// 连接球机的串口设备名称
#define BALL_DEVICE "/dev/ttyO4"

// 机芯接口结构体
static VISCAInterface_t g_visca_iface = {0};

// 球机接口结构体
static PELCOInterface_t g_pelco_iface = {0};

static int g_init = 0;

static int g_tty_camera = 0;
static int g_tty_ball = 0;

int g_camera_lock = 0;

int swpa_camera_init()
{
    int ret = 0;
    int camera_num = 0;

    g_pelco_iface.device = BALL_DEVICE;     // 连接球机的串口设备名称在此
    g_pelco_iface.address = 1;              // 球机地址在此(由拨码盘决定)

    // 已经初始化过了
    if (g_init == 1)
    {
        com_debug(7, "%s already init everything, will no init again.\n", __func__);
        return 0;
    }

    //printf("%d %d\n", g_pelco_iface.port_fd, g_visca_iface.port_fd);
    // 初始化
    if (g_tty_ball == 1)
    {
        //com_debug(7, "will close pelco port...\n");
        pelco_close_serial(&g_pelco_iface);
    }

    ret = pelco_open_serial(&g_pelco_iface);
    if (ret < 0)
    {
        g_tty_ball = 0;
        com_print("%s init ball machine failed.\n", __func__);
        return -1;
    }
    g_tty_ball = 1;

#if SEPARATE
	g_visca_iface.device = CAMERA_DEVICE;   // 连接机芯的串口设备名称在此
	if (g_tty_camera == 1)
    {
        visca_close_serial(&g_visca_iface);
    }

	ret = visca_open_serial(&g_visca_iface);
    if (ret < 0)
    {
        g_tty_camera = 0;
        com_print("%s init camera module failed.\n", __func__);
        return -1;
    }

	ret = visca_set_address(&g_visca_iface, &camera_num);
    if (ret < 0 || camera_num <= 0 || camera_num > 7)
    {
        com_print("%s get camera module address failed, addr: %d.\n", __func__, camera_num);
        return -1;
    }
#else
	ret = visca_open_serial(&g_visca_iface);
    if (ret < 0)
    {
        g_tty_camera = 0;
        com_print("%s init camera module failed.\n", __func__);
        return -1;
    }
	g_visca_iface.device = BALL_DEVICE;   // 连接机芯的串口设备名称在此
	g_visca_iface.port_fd = g_pelco_iface.port_fd;
	camera_num = 1;
#endif

    SWPA_CAMERA_PRINT("camera_num: %x\n", camera_num);

    g_visca_iface.address = camera_num; // 机芯地址在此

	g_tty_camera = 1;

    g_init = 1;
    return ret;
}

int swpa_camera_deinit()
{
    int ret = -1;

    ret = pelco_close_serial(&g_pelco_iface);
#if SEPARATE	
	ret |= visca_close_serial(&g_visca_iface);
#endif

    g_init = 0;
    g_tty_camera = 0;
    g_tty_ball = 0;

    return ret;
}

int swpa_camera_set_power(int power)
{
    if (power < 0 || power > 1) return -2;

    if(visca_set_power(&g_visca_iface, (int )power))
    {
        SWPA_CAMERA_PRINT("set power error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_get_power(int *power)
{
    if (power == NULL) return -2;

    if(visca_get_power(&g_visca_iface, power))
    {
        SWPA_CAMERA_PRINT("get power error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_init_lens(void)
{
    if(visca_lens_init(&g_visca_iface))
    {
        SWPA_CAMERA_PRINT("init lens error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_reset(void)
{
    if(visca_camera_reset(&g_visca_iface))
    {
        SWPA_CAMERA_PRINT("reset camera error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_get_version(unsigned int *vendor, unsigned int *model, unsigned int *rom_version)
{
    VISCACamera_t camera = {0};

    if(visca_get_camera_info(&g_visca_iface, &camera))
    {
        SWPA_CAMERA_PRINT("get camera info error !\n");
        return -1;
    }

    *vendor = camera.vendor;
    *model = camera.model;
    *rom_version = camera.rom_version;

    return 0;
}

int swpa_camera_set_monitoring_mode(MONITOR_MODE mode)
{
    return visca_set_camera_register(&g_visca_iface, 0x72, (int)mode);
}

int swpa_camera_get_monitoring_mode(MONITOR_MODE *mode)
{
    return visca_get_camera_register(&g_visca_iface, 0x72, (int*)mode);
}

int swpa_camera_set_sync_mode(int mode)
{
    if (mode < 0 || mode > 1) return -2;
    return visca_set_camera_register(&g_visca_iface, 0x56, mode);
}

int swpa_camera_get_sync_mode(int *mode)
{
    if (mode == NULL) return -2;

    return visca_get_camera_register(&g_visca_iface, 0x56, mode);
}
/*
========== 3A 相关 ============
*/
int swpa_camera_basicparam_set_AE(
    AE_MODE mode
)
{
    if(visca_set_ae_mode(&g_visca_iface, (int )mode))
    {
        SWPA_CAMERA_PRINT("set AE error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_get_AE(
    AE_MODE *mode
)
{
    if(!mode)
        return -1;
    if(visca_get_ae_mode(&g_visca_iface, (int *)mode))
    {
        SWPA_CAMERA_PRINT("get AE error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_set_AF(
    AF_MODE mode
)
{
    if(visca_set_focus_mode(&g_visca_iface, (int )mode))
    {
        SWPA_CAMERA_PRINT("set AF error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_get_AF(
    AF_MODE *mode
)
{
    if(!mode)
        return -1;
    if(visca_get_focus_mode(&g_visca_iface, (int *)mode))
    {
        SWPA_CAMERA_PRINT("get AF error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_set_focus(FOCUS_MODE mode)
{
    if (mode == FOCUS_NEAR)
    {
        if(visca_set_focus_near(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set focus near error !\n");
            return -1;
        }
    }
    else if (mode == FOCUS_FAR)
    {
        if(visca_set_focus_far(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set focus far error !\n");
            return -1;
        }
    }
    else if (mode == FOCUS_ONE_PUSH)
    {
        if(visca_set_focus_one_push(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set focus stop error !\n");
            return -1;
        }
    }
    else if (mode == FOCUS_INFINITY)
    {
        if(visca_set_focus_infinity(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set focus infinity error !\n");
            return -1;
        }
    }
    else if (mode == FOCUS_STOP)
    {
        if(visca_set_focus_stop(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set focus stop error !\n");
            return -1;
        }
    }

    return 0;
}

int swpa_camera_basicparam_set_focus_speed(FOCUS_MODE mode, int speed)
{
    if (speed < 0 || speed > 7) return -2;
    if (mode == FOCUS_NEAR)
    {
        if(visca_set_focus_near_speed(&g_visca_iface, speed))
        {
            SWPA_CAMERA_PRINT("set focus near speed error !\n");
            return -1;
        }
    }
    else if (mode == FOCUS_FAR)
    {
        if(visca_set_focus_far_speed(&g_visca_iface, speed))
        {
            SWPA_CAMERA_PRINT("set focus far speed error !\n");
            return -1;
        }
    }

    return 0;
}

int swpa_camera_basicparam_set_focus_value(int value)
{
    if(visca_set_focus_value(&g_visca_iface, value))
    {
        SWPA_CAMERA_PRINT("set focus  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_get_focus_value(int *value)
{
    if(!value)
        return -1;
    if(visca_get_focus_value(&g_visca_iface, value))
    {
        SWPA_CAMERA_PRINT("get focus error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_set_infrared_mode(int value)
{
    if (value != 0 && value != 1)
        return SWPAR_INVALIDARG;

    if (visca_set_infraredmode(&g_visca_iface, value))
    {
        SWPA_CAMERA_PRINT("set infrared mode to %d failed !\n", value);
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

/*
* === 自动控制相关 ===
*/

int swpa_camera_basicparam_set_AWB(
    AWB_MODE mode
)
{
    if(visca_set_whitebal_mode(&g_visca_iface, (int )mode))
    {
        SWPA_CAMERA_PRINT("set AWB error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_get_AWB(
    AWB_MODE *mode
)
{
    if(!mode)
        return -1;
    if(visca_get_whitebal_mode(&g_visca_iface, (int *)mode))
    {
        SWPA_CAMERA_PRINT("get AWB error !\n");
        return -1;
    }
    return 0;
}

/*
* === 手动控制相关 ===
*/

/*
*shutter :0x0 - 0x15
*/
int swpa_camera_basicparam_set_shutter(
	int shutter
)
{
    if(visca_set_shutter_value(&g_visca_iface, shutter))
    {
        SWPA_CAMERA_PRINT("set shutter  error !\n");
        return -1;
    }
    return 0;
}


int swpa_camera_basicparam_get_shutter(
	int * shutter
)
{
    if(!shutter)
        return -1;
    if(visca_get_shutter_value(&g_visca_iface, shutter))
    {
        SWPA_CAMERA_PRINT("get shutter error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_set_iris_mode(IRIS_MODE mode)
{
    if ((mode != IRIS_RST) && (mode != IRIS_UP) && (mode != IRIS_DOWN)) return -2;
    if (mode == IRIS_RST)
    {
        return visca_set_iris_reset(&g_visca_iface);
    }
    else if (mode == IRIS_UP)
    {
        return visca_set_iris_up(&g_visca_iface);
    }
    else if (mode == IRIS_DOWN)
    {
        return visca_set_iris_down(&g_visca_iface);
    }

    return 0;
}

/*
*shutter :0x0 - 0x11
*/
int swpa_camera_basicparam_set_iris(
    int iris
)
{
    if(visca_set_iris_value(&g_visca_iface, iris))
    {
        SWPA_CAMERA_PRINT("set iris  error !\n");
        return -1;
    }
    return 0;
}


int swpa_camera_basicparam_get_iris(
    int *iris
)
{
    if(!iris)
        return -1;
    if(visca_get_iris_value(&g_visca_iface, iris))
    {
        SWPA_CAMERA_PRINT("get iris error !\n");
        return -1;
    }
    return 0;
}
/*
*shutter :0x0 - 0xf
*/

int swpa_camera_basicparam_set_gain(
    int gain
)
{
    if(visca_set_gain_value(&g_visca_iface, gain))
    {
        SWPA_CAMERA_PRINT("set gain  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_get_gain(
    int *gain
)
{
    if(!gain)
        return -1;
    if(visca_get_gain_value(&g_visca_iface, gain))
    {
        SWPA_CAMERA_PRINT("get gain error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_basicparam_set_gain_limit(int gain)
{
    if (gain < 0x04 || gain > 0x0f)
    {
        return -2;
    }

    return visca_set_gain_limit_value(&g_visca_iface, gain);
}

int swpa_camera_basicparam_get_gain_limit(int *gain)
{
    if (gain == NULL)
    {
        return -2;
    }

    return visca_get_gain_limit_value(&g_visca_iface, gain);
}

int swpa_camera_basicparam_set_lr_reverse(int value)
{
    if (value != 0x02 && value != 0x03)
    {
        return -2;
    }

    return visca_set_lr_reverse_value(&g_visca_iface, value);
}

int swpa_camera_basicparam_set_picture_flip(int value)
{
    if (value != 0x02 && value != 0x03)
    {
        return -2;
    }

    return visca_set_picture_flip_value(&g_visca_iface, value);
}
/*
*0x00 - 0xff
*/
int swpa_camera_basicparam_set_rgb_gain(
    int RGain,
    int GGain,
    int BGain
)
{
    if(visca_set_rgain_value(&g_visca_iface, RGain))
    {
        SWPA_CAMERA_PRINT("set RGain  error !\n");
        return -1;
    }
    if(visca_set_bgain_value(&g_visca_iface, BGain))
    {
        SWPA_CAMERA_PRINT("set RGain  error !\n");
        return -1;
    }
    return 0;

}

int swpa_camera_basicparam_get_rgb_gain(
    int *RGain,
    int *GGain,
    int *BGain
)
{
    if(!RGain||!BGain)
        return -1;
    if(visca_get_rgain_value(&g_visca_iface, RGain))
    {
        SWPA_CAMERA_PRINT("get RGain error !\n");
        return -1;
    }
    if(visca_get_bgain_value(&g_visca_iface, BGain))
    {
        SWPA_CAMERA_PRINT("get BGain error !\n");
        return -1;
    }
    return 0;

}

int swpa_camera_basicparam_set_zoom_mode(ZOOM_MODE mode)
{
    if (mode == ZOOM_TELE)
    {
        if (visca_set_zoom_tele(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set zoom tele error !\n");
            return -1;
        }
    }
    else if (mode == ZOOM_WIDE)
    {
        if (visca_set_zoom_wide(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set zoom wide error !\n");
            return -1;
        }
    }
    else if (mode == ZOOM_STOP)
    {
        if (visca_set_zoom_stop(&g_visca_iface))
        {
            SWPA_CAMERA_PRINT("set zoom stop error !\n");
            return -1;
        }
    }
    return 0;
}

int swpa_camera_basicparam_set_zoom_speed(ZOOM_MODE mode, int speed)
{
    if (speed < 0 || speed > 7) return -1;

    if (mode == ZOOM_TELE)
    {
        if (visca_set_zoom_tele_speed(&g_visca_iface, speed))
        {
            SWPA_CAMERA_PRINT("set zoom tele speed error !\n");
            return -1;
        }
    }
    else if (mode == ZOOM_WIDE)
    {
        if (visca_set_zoom_wide_speed(&g_visca_iface, speed))
        {
            SWPA_CAMERA_PRINT("set zoom wide speed error !\n");
            return -1;
        }
    }

    return 0;
}

int swpa_camera_basicparam_set_zoom_value(int value)
{
    if (value < 0 || value > 0x4000)
    {
        return -2;
    }
    if (visca_set_zoom_value(&g_visca_iface, value))
    {
        SWPA_CAMERA_PRINT("set zoom value error !\n");
        return -1;
    }

    return 0;
}

int swpa_camera_basicparam_get_zoom_value(int *value)
{
    if(!value)
        return -1;
    if (visca_get_zoom_value(&g_visca_iface, value))
    {
        SWPA_CAMERA_PRINT("get zoom value error !\n");
        return -1;
    }

    return 0;
}

int swpa_camera_basicparam_dzoom_enable(int enable)
{
    if(enable < 0 || enable > 1)  return -2;
    if (enable)
    {
        visca_set_dzoom_on(&g_visca_iface);
    }
    else
    {
        visca_set_dzoom_off(&g_visca_iface);
    }
    return 0;
}

int swpa_camera_basicparam_set_freeze(int mode)
{
    if(mode < 0 || mode > 1)  return -2;

    if (visca_set_freeze(&g_visca_iface, mode))
    {
        SWPA_CAMERA_PRINT("set freeze value error !\n");
        return -1;
    }

    return 0;
}

int swpa_camera_basicparam_get_freeze(int *mode)
{
    if(mode == NULL)  return -2;

    if (visca_get_freeze(&g_visca_iface, mode))
    {
        SWPA_CAMERA_PRINT("get freeze value error !\n");
        return -1;
    }

    return 0;
}

int swpa_camera_basicparam_set_mask(int id, int half_width, int half_height, int is_new)
{
    return visca_privacy_set_mask(&g_visca_iface, id, half_width, half_height, is_new);
}

int swpa_camera_basicparam_set_pt(int pan, int tilt)
{
    return visca_privacy_set_pt(&g_visca_iface, pan, tilt);
}

int swpa_camera_basicparam_get_pt(int *pan, int *tilt)
{
    return visca_get_privacy_pt(&g_visca_iface, pan, tilt);
}

int swpa_camera_basicparam_set_ptz(int id, int pan, int tilt, int zoom)
{
    return visca_privacy_set_ptz(&g_visca_iface, id, pan, tilt, zoom);
}

int swpa_camera_basicparam_get_ptz(int id, int *pan, int *tilt, int *zoom)
{
    return visca_get_privacy_ptz(&g_visca_iface, id, pan, tilt, zoom);
}

int swpa_camera_basicparam_set_nolock_mask(int id, int center_x, int center_y, int half_width, int half_height)
{
    return visca_privacy_set_noninterlock_mask(&g_visca_iface, id, center_x, center_y, half_width, half_height);
}

int swpa_camera_basicparam_set_display(int id, int color)
{
    return visca_privacy_set_display(&g_visca_iface, id, color);
}

int swpa_camera_basicparam_get_display(int *display)
{
    return visca_get_privacy_display(&g_visca_iface, display);
}

int swpa_camera_basicparam_get_monitor(int *monitor)
{
    return visca_get_privacy_monitor(&g_visca_iface, monitor);
}

int swpa_camera_basicparam_clear_display(int id)
{
    return visca_privacy_clear_display(&g_visca_iface, id);
}

int swpa_camera_basicparam_set_grid(int act)
{
    return visca_privacy_set_grid(&g_visca_iface, act);
}


int swpa_camera_basicparam_set_hrmode(int mode)
{
    return visca_set_hrmode(&g_visca_iface, mode);
}

int swpa_camera_basicparam_get_hrmode(int* mode)
{
	if (NULL == mode)
	{
		return SWPAR_INVALIDARG;
	}
	
    return visca_get_hrmode(&g_visca_iface, mode);
}

int swpa_camera_basicparam_set_expcomp_mode(int mode)
{
    return visca_set_exposure_compensation_mode(&g_visca_iface, mode);
}

int swpa_camera_basicparam_get_expcomp_mode(int* mode)
{
	if (NULL == mode)
	{
		return SWPAR_INVALIDARG;
	}
	
    return visca_get_exposure_compensation_mode(&g_visca_iface, mode);
}


int swpa_camera_basicparam_set_expcomp_value(int value)
{
    return visca_set_exposure_compensation_value(&g_visca_iface, value);
}

int swpa_camera_basicparam_get_expcomp_value(int* value)
{
	if (NULL == value)
	{
		return SWPAR_INVALIDARG;
	}
	
    return visca_get_exposure_compensation_value(&g_visca_iface, value);
}


int swpa_camera_basicparam_set_stabilizer(int value)
{
    return visca_set_stabilizer(&g_visca_iface, value);
}



// not need
int swpa_camera_io_backupio_enable(int enable)
{
	return -1;
}

/*
* === 图像处理相关 ===
*/

/*
* 0x0 -0x0e
*/
int swpa_camera_imgproc_set_saturation(
    int saturation
)
{
    if(visca_set_color_gain(&g_visca_iface, saturation))
    {
        SWPA_CAMERA_PRINT("set saturation  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_get_saturation(
    int *saturation
)
{
    if(!saturation)
        return -1;
    if(visca_get_color_gain(&g_visca_iface, saturation))
    {
        SWPA_CAMERA_PRINT("get saturation  error !\n");
        return -1;
    }
    return 0;
}

/*
*0:	Standard,	1	to	4
*/
int swpa_camera_imgproc_set_gamma_type(
    int gamma
)
{
    if(visca_set_gamma(&g_visca_iface, gamma))
    {
        SWPA_CAMERA_PRINT("set gamma  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_get_gamma_type(
    int *gamma
)
{
    if(!gamma)
        return -1;
    if(visca_get_gamma(&g_visca_iface, gamma))
    {
        SWPA_CAMERA_PRINT("get gamma  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_set_WDR(
    WDR_MODE mode
)
{
    if(visca_set_wdr(&g_visca_iface, mode))
    {
        SWPA_CAMERA_PRINT("set wdr  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_get_WDR(
    WDR_MODE *mode
)
{
    if(!mode)
        return -1;
    if(visca_get_wdr(&g_visca_iface, (int *)mode))
    {
        SWPA_CAMERA_PRINT("get gamma  error !\n");
        return -1;
    }
    return 0;
}

/*
*0:	OFF,	level	1	to	5
*/
int swpa_camera_imgproc_set_NR(
    int NR
)
{
    if(visca_set_nr(&g_visca_iface, NR))
    {
        SWPA_CAMERA_PRINT("set NR  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_get_NR(
    int *NR
)
{
    if(!NR)
        return -1;
    if(visca_get_nr(&g_visca_iface, NR))
    {
        SWPA_CAMERA_PRINT("get nr  error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_set_sharpen_param(
	int mode,
	int threshold
)
{
    if (threshold < 0 || threshold > 15) return -2;
    if(visca_set_aperture_value(&g_visca_iface, threshold))
    {
        SWPA_CAMERA_PRINT("set sharpen error !\n");
        return -1;
    }
    return 0;
}

int swpa_camera_imgproc_get_sharpen_param(
	int * mode,
	int * threshold
)
{
    if(!threshold)
        return -1;
    if(visca_get_aperture_value(&g_visca_iface, threshold))
    {
        SWPA_CAMERA_PRINT("get sharpen error !\n");
        return -1;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// 下面是球机相关接口实现
int swpa_camera_ball_stop(void)
{
    return pelco_camera_stop(&g_pelco_iface);
}

int swpa_camera_ball_tilt(TILT_TYPE action, int speed)
{
    int tmp = 0;
    if (speed < 1 || speed > 0x3f)  return -2;
    if ((action != TILT_DOWN) && (action != TILT_UP)) return -2;

    if (action == TILT_DOWN)
    {
        tmp = 0;
    }
    else if (action == TILT_UP)
    {
        tmp = 1;
    }

    return pelco_camera_tilt(&g_pelco_iface, tmp, speed);
}

int swpa_camera_ball_pan(PAN_TYPE action, int speed)
{
    int tmp = 0;

    if (speed < 1 || speed > 0x3f)  return -2;
    if ((action != PAN_LEFT) && (action != PAN_RIGHT)) return -2;

    if (action == PAN_LEFT)
    {
        tmp = 0;
    }
    else if (action == PAN_RIGHT)
    {
        tmp = 1;
    }
    return pelco_camera_pan(&g_pelco_iface, tmp, speed);
}

int swpa_camera_ball_pan_tilt(PAN_TYPE pan_act, int pan_speed, TILT_TYPE tilt_act, int tilt_speed)
{
    if (pan_speed < 1 || pan_speed > 0x3f || tilt_speed < 1 || tilt_speed > 0x3f )  return -2;
    if ((pan_act != PAN_LEFT) && (pan_act != PAN_RIGHT)) return -2;
    if ((tilt_act != TILT_DOWN) && (tilt_act!= TILT_UP)) return -2;

    return pelco_camera_pan_tilt(&g_pelco_iface, pan_act, pan_speed, tilt_act, tilt_speed);
}

int swpa_camera_ball_preset(PRESET_TYPE preset_act, int preset_id)
{
    if (preset_act!= PRESET_SET && preset_act!= PRESET_CLEAR && preset_act!= PRESET_CALL) return -2;
    if (preset_id < 0 || preset_id > 255)   return -2;
    return pelco_camera_preset(&g_pelco_iface, preset_act, preset_id);
}

int swpa_camera_ball_set_pt_coordinates(int pan, int tilt)
{
    if (pan < 0 || pan > 3599 || tilt < -100 || tilt > 950) return -2;
    return pelco_camera_set_ptcoordinates(&g_pelco_iface, pan, tilt);
}

int swpa_camera_ball_get_pt_coordinates(int* pan, int* tilt)
{
    if (pan == NULL && tilt == NULL)    return -2;

	int ipan = 0, itilt = 0;
    if (0 != pelco_camera_get_ptcoordinates(&g_pelco_iface, &ipan, &itilt))
	{
		return -1;
	}
	
	if (pan != NULL) *pan = ipan;
	if (tilt != NULL) *tilt = itilt;

	return 0;
}


/* //Marked in Version B 
int swpa_camera_ball_set_infrared_power(int zoom_total, int zoom_1, int zoom_2)
{
    return pelco_camera_set_infraredpower(&g_pelco_iface, zoom_total, zoom_1, zoom_2);
}*/

int swpa_camera_ball_set_infrared_sensitive(int sensitivity)
{
    int ret = 0;
    if (sensitivity < 1 || sensitivity > 10) return -2;

    ret = pelco_camera_preset(&g_pelco_iface, PRESET_CALL, 50);
    ret |= pelco_camera_preset(&g_pelco_iface, PRESET_CALL, sensitivity);

    return ret;
}

int swpa_camera_ball_set_infrared_threshold(int threshold)
{
    int ret = 0;
    if (threshold < 1 || threshold > 5) return -2;

    ret = pelco_camera_preset(&g_pelco_iface, PRESET_CALL, 51);
    ret |= pelco_camera_preset(&g_pelco_iface, PRESET_CALL, threshold);

    return ret;
}

int swpa_camera_ball_set_wiper()
{
    return pelco_camera_preset(&g_pelco_iface, PRESET_CALL, 71);
}

int swpa_camera_ball_remote_reset(void)
{
    return pelco_camera_preset(&g_pelco_iface, PRESET_CALL, 94);
}

int swpa_camera_ball_restore_factory_settings(void)
{
    return pelco_camera_preset(&g_pelco_iface, PRESET_CALL, 82);
}

int swpa_camera_ball_factorytest_start(void)
{
    return pelco_camera_factorytest_start(&g_pelco_iface);
}

int swpa_camera_ball_factorytest_stop(void)
{
    return pelco_camera_factorytest_stop(&g_pelco_iface);
}

/* //Marked in Version B 
int swpa_camera_ball_set_infrared_led(int mode)
{
    int tmp = 53;
    if (mode == 1)
    {
        tmp = 52;
    }
    else if (mode == 2)
    {
        tmp = 54;
    }

    return pelco_camera_preset(&g_pelco_iface, PRESET_CALL, tmp);
}*/

int swpa_camera_ball_set_defog_fan(int mode)
{
    int tmp = 110;
    if (mode == 1)
    {
        tmp = 109;
    }

    return pelco_camera_preset(&g_pelco_iface, PRESET_CALL, tmp);
}


int swpa_camera_ball_calibration(void)
{
	return pelco_camera_calibration(&g_pelco_iface);
}

int swpa_camera_ball_3d(int centerx, int centery, int width, int height)
{
    if (centerx < 0 || centerx > 100)    return -2;
	if (centery < 0 || centery > 100)    return -2;
	if (width < 0 || width > 100)    return -2;
	if (height < 0 || height > 100)    return -2;

    if (0 != pelco_camera_3d(&g_pelco_iface, centerx, centery, width, height))
	{
		return -1;
	}
	
	return 0;
}


int swpa_camera_ball_set_led_mode(int mode, int powermode)
{
    if (mode < 0 || mode > 4)    return -2;
	if (powermode < 0 || powermode > 2)    return -2;
/*
	// 这段代码出现的原因是球机B版本早期有设置LED模式bug，现已修正，因此注释掉。
	if (0x2 == mode || 0x0 == mode)
	{
		if (0x2 == powermode)
		{
			pelco_camera_set_led_mode(&g_pelco_iface, 0x3); //off
			swpa_thread_sleep_ms(100);
			pelco_camera_set_led_power_mode(&g_pelco_iface, 0x2);//usr mode
			swpa_thread_sleep_ms(100);
			pelco_camera_set_led_power(&g_pelco_iface, 0, 0, 0);
			swpa_thread_sleep_ms(100);
			pelco_camera_set_led_power(&g_pelco_iface, 0, 0, 0);
			swpa_thread_sleep_ms(100);
		}
		else if (0x0 == powermode)
		{
			pelco_camera_set_led_mode(&g_pelco_iface, 0x3); //off
			swpa_thread_sleep_ms(100);
			pelco_camera_set_led_power_mode(&g_pelco_iface, 0x0);//std mode
			swpa_thread_sleep_ms(100);
		}
	}
*/
	pelco_camera_set_led_power_mode(&g_pelco_iface, powermode);
	swpa_thread_sleep_ms(50);
	//pelco_camera_set_led_power_mode(&g_pelco_iface, powermode);
	//swpa_thread_sleep_ms(50);
    if (0 != pelco_camera_set_led_mode(&g_pelco_iface, mode))
	{
		return -1;
	}
	
	return 0;
}





int swpa_camera_ball_set_led_power(int nearpower, int mediumpower, int farpower)
{
    if (nearpower < 0 || nearpower > 10)    return -2;
	if (mediumpower < 0 || mediumpower > 10)    return -2;
	if (farpower < 0 || farpower > 10)    return -2;

    if (0 != pelco_camera_set_led_power(&g_pelco_iface, nearpower, mediumpower, farpower))
	{
		return -1;
	}	
	
	return 0;
}




int swpa_camera_ball_sync_zoom()
{
    if (0 != pelco_camera_sync_zoom(&g_pelco_iface))
	{
		return -1;
	}	
	
	return 0;
}


int swpa_camera_ball_set_privacyzone(int index, int enable, int width, int height)
{
	if (index < 0 || index > 8) return SWPAR_INVALIDARG;
	if (width < 0 || width > 80) return SWPAR_INVALIDARG;
	if (height < 0 || height > 45) return SWPAR_INVALIDARG;
	
    if (0 != pelco_camera_set_privacyzone(&g_pelco_iface, index, enable, width, height))
	{
		return -1;
	}	
	
	return 0;
}


int swpa_camera_ball_set_privacycoord(int index, int x, int y)
{
	if (index < 0 || index > 8) return SWPAR_INVALIDARG;
	if (x < 0 || x > 100) return SWPAR_INVALIDARG;
	if (y < 0 || y > 100) return SWPAR_INVALIDARG;
	
    if (0 != pelco_camera_set_privacycoord(&g_pelco_iface, index, x, y))
	{
		return -1;
	}	
	
	return 0;
}

int swpa_camera_ball_get_dome_version(int *pDomeVersion)
{
	if(pDomeVersion == NULL)
		return -1;
	if(0 != pelco_camera_get_dome_version(&g_pelco_iface, pDomeVersion))
	{
		return -1;
	}

	return 0;
}

int swpa_camera_ball_get_camera_version(char *pCamVersion)
{
	if(pCamVersion == NULL)
		return -1;
	if(0 != visca_get_camera_version(&g_visca_iface, pCamVersion))
	{
		return -1;
	}

	return 0;
}



int spwa_camera_send_cmd(PROTOCOL_TYPE type, unsigned char *write_buf, int write_buf_size)
{
    unsigned char recv[10] = {0};
    int recv_size = 0;
    if(!write_buf )
        return -1;
    if(PROTOCOL_VISCA == type)
    {
        if(com_write_packet_data(g_visca_iface.port_fd, write_buf, write_buf_size))
            return -1;
        if(com_get_packet_terminator(g_visca_iface.port_fd,recv,&recv_size,0xff))
            return -1;
        if(5 != (recv[1]&0xf0)>>4)
        {
            if(com_get_packet_terminator(g_visca_iface.port_fd,recv,&recv_size,0xff))
                return -1;
        }
    }
    if(PROTOCOL_PELCO == type)
    {
        if(com_write_packet_data(g_pelco_iface.port_fd,write_buf,write_buf_size))
            return -1;
    }
    return 0;
}

int spwa_camera_recive_cmd(PROTOCOL_TYPE type,unsigned char *write_buf,int write_buf_size,unsigned char *awk_buf,int *awk_buf_size)
{
    if(!write_buf || !awk_buf || !awk_buf_size)
        return -1;
    if(PROTOCOL_VISCA == type)
    {

        if(com_write_packet_data(g_visca_iface.port_fd,write_buf,write_buf_size))
            return -1;
        if(com_get_packet_terminator(g_visca_iface.port_fd,awk_buf,awk_buf_size,0xff))
            return -1;
    }
    if(PROTOCOL_PELCO == type)
    {
        // TODO:
    }
    return 0;
}

int spwa_camera_com_send_data(PROTOCOL_TYPE type,unsigned char *buffer, int buffer_size)
{
    int fd = 0;
    if(PROTOCOL_VISCA == type)
    {
        fd = g_visca_iface.port_fd;
    }
    else if (PROTOCOL_PELCO == type)
    {
        fd = g_pelco_iface.port_fd;
    }
    else
    {
        return -2;
    }
    
    // 在发送前先清空接收缓冲区
    tcflush(fd, TCIFLUSH);

    return write(fd, buffer, buffer_size);
}

int spwa_camera_com_rcv_data(PROTOCOL_TYPE type, unsigned char *buffer, int buffer_size)
{
    int fd = 0;
    if(PROTOCOL_VISCA == type)
    {
        fd = g_visca_iface.port_fd;
    }
    else if (PROTOCOL_PELCO == type)
    {
        fd = g_pelco_iface.port_fd;
    }
    else
    {
        return -2;
    }
    return read(fd, buffer, buffer_size);
}

int spwa_camera_com_rcv_packet_data(PROTOCOL_TYPE type, unsigned char *buffer, int* buffer_size)
{
    int fd = 0;
    if(PROTOCOL_VISCA == type)
    {
        fd = g_visca_iface.port_fd;
    }
    else if (PROTOCOL_PELCO == type)
    {
        fd = g_pelco_iface.port_fd;
    }
    else
    {
        return -2;
    }
    fd_set rfds;
    int bytes_read = 0;
    int tmp_len = 0;
    unsigned char *tmp = buffer;
    int left = *buffer_size;
    int size = *buffer_size;

    int timeout = 500; // 500 ms
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

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

        do{
            // 注：测试时发现，当传递的长度比实际大的时候，read读完实际数据后会一直阻塞
            // 故在这里再加一个select。
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            retval = select(fd+1, &rfds, NULL, NULL, &tv);
            if (retval == 0)
            {
                //printf("!!!!!!!timeout.\n");
                break;
            }

            //printf("before read left:%d\n", left);
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

    *buffer_size = bytes_read;
    return COM_SUCCESS;
}

int32_t spwa_camera_com_rcv_packet_terminator(PROTOCOL_TYPE type, unsigned char *buffer, int *buffer_size, int terminator)
{
    int fd = 0;
    if(PROTOCOL_VISCA == type)
    {
        fd = g_visca_iface.port_fd;
    }
    else if (PROTOCOL_PELCO == type)
    {
        fd = g_pelco_iface.port_fd;
    }
    else
    {
        return -2;
    }

    int pos=0;
    int bytes = *buffer_size;
    int ret = 0;
    int bytes_read = 0;
    fd_set rfds;

    int timeout = 500; // 500 ms
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;


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

    *buffer_size = bytes_read;
    return COM_SUCCESS;
}





