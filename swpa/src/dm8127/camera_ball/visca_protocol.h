/**
 * @file   visca_protocol.h
 * @author 
 * @date   Tue Jun 17 09:20:09 2014
 * 
 * @brief  
 * 
 * @note pls see libvisca project.
 */

#ifndef __VISCA_PROTOCOL_H
#define __VISCA_PROTOCOL_H

#include <stdint.h>
#include "common_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define SEPARATE //NOTE: enable this for Jupiter A-Version

/// 成功
#define VISCA_SUCCESS       0
/// 失败
#define VISCA_FAILURE       -1
/// 超时
#define VISCA_TIMEOUT       -6
/// 命令执行失败
#define VISCA_CMDERROR      -7
/// 命令未实际执行
#define VISCA_CMDNOACT      -8

typedef struct _COM_interface VISCAInterface_t;

typedef struct _COM_packet VISCAPacket_t;

/* CAMERA STRUCTURE */
typedef struct _VISCA_camera
{
    // VISCA data:
    int address;

    // camera info:
    uint32_t vendor;
    uint32_t model;
    uint32_t rom_version;
    uint32_t socket_num;
} VISCACamera_t;

///////////////////////////////////////////////////////////////////////////////
// macro
// command 
#define VISCA_COMMAND                    0x01
#define VISCA_INQUIRY                    0x09
#define VISCA_TERMINATOR                 0xFF

#define VISCA_CATEGORY_INTERFACE         0x00
#define VISCA_CATEGORY_CAMERA1           0x04
#define VISCA_CATEGORY_PAN_TILTER        0x06
#define VISCA_CATEGORY_CAMERA2           0x07

/* response types */
#define VISCA_RESPONSE_CLEAR             0x40
#define VISCA_RESPONSE_ADDRESS           0x30
#define VISCA_RESPONSE_ACK               0x40
#define VISCA_RESPONSE_COMPLETED         0x50
#define VISCA_RESPONSE_ERROR             0x60

///////////////////////////////////////////////////////////////////////////////

/// 对外
int visca_open_serial(VISCAInterface_t *iface);

int visca_close_serial(VISCAInterface_t *iface);

int visca_unread_bytes(VISCAInterface_t *iface, unsigned char *buffer, int *buffer_size);

///////////////////

int visca_set_address(VISCAInterface_t *iface, int *camera_num);

int visca_clear(VISCAInterface_t *iface);

int visca_get_camera_info(VISCAInterface_t *iface, VISCACamera_t *camera);

//---------- CAM_Power
// power: 1==on 0 == off(standby)
// ok! 从off切换到on大约7秒
int visca_set_power(VISCAInterface_t *iface, int power);

//----------CAM_Initialize
// ok! 进行远近对焦，然后对焦到上一次的位置
int visca_lens_init(VISCAInterface_t *iface);
// ok! 复位时间约10秒
int visca_camera_reset(VISCAInterface_t *iface);

//---------- CAM_Zoom
int visca_set_zoom_stop(VISCAInterface_t *iface);
int visca_set_zoom_tele(VISCAInterface_t *iface);
int visca_set_zoom_wide(VISCAInterface_t *iface);
// speed: 0(low)~7(high)
int visca_set_zoom_tele_speed(VISCAInterface_t *iface, int speed);
// speed: 0(low)~7(high)
int visca_set_zoom_wide_speed(VISCAInterface_t *iface, int speed);
// zoom: 0(wide)~31424(tele)
int visca_set_zoom_value(VISCAInterface_t *iface, int zoom);

//---------- CAM_DZoom
int visca_set_dzoom_off(VISCAInterface_t *iface);
int visca_set_dzoom_on(VISCAInterface_t *iface);

//----------CAM_ZoomFocus
int visca_set_zoom_and_focus_value(VISCAInterface_t *iface, int zoom, int focus);

//---------- CAM_Focus
int visca_set_focus_stop(VISCAInterface_t *iface);
int visca_set_focus_far(VISCAInterface_t *iface);
int visca_set_focus_near(VISCAInterface_t *iface);
// speed: 0(low)~7(high)
int visca_set_focus_far_speed(VISCAInterface_t *iface, int speed);
// speed: 0(low)~7(high)
int visca_set_focus_near_speed(VISCAInterface_t *iface, int speed);
int visca_set_focus_value(VISCAInterface_t *iface, int focus);
// todo power 2==auto 3 manual 0x10 = auto/manual(?????????)
int visca_set_focus_mode(VISCAInterface_t *iface, int power);
int visca_set_focus_one_push(VISCAInterface_t *iface);
int visca_set_focus_infinity(VISCAInterface_t *iface);
int visca_set_focus_near_limit(VISCAInterface_t *iface, int limit);

//----------CAM_Bright
// *ONLY* used in "Bright Mode (Manual control)"
int visca_set_bright_reset(VISCAInterface_t *iface);
int visca_set_bright_up(VISCAInterface_t *iface);
int visca_set_bright_down(VISCAInterface_t *iface);
// value: bright position 0~0xff
int visca_set_bright_value(VISCAInterface_t *iface, int value);

//----------CAM_SlowShutter
/*
power
0:auto slow shutter off(manual)
1:auto slow shutter on
*/
int visca_set_slowshutter(VISCAInterface_t *iface, int power);

//----------CAM_Shutter
// *ONLY* used in "Shutter Priority Automatic Exposure mode" or "Manual Control mode"
int visca_set_shutter_reset(VISCAInterface_t *iface);
int visca_set_shutter_up(VISCAInterface_t *iface);
int visca_set_shutter_down(VISCAInterface_t *iface);
// value: shutter position 0~0x15
int visca_set_shutter_value(VISCAInterface_t *iface, int value);

//----------CAM_Gain
int visca_set_gain_reset(VISCAInterface_t *iface);
int visca_set_gain_up(VISCAInterface_t *iface);
int visca_set_gain_down(VISCAInterface_t *iface);
// value: gain position 0x0~0xf
int visca_set_gain_value(VISCAInterface_t *iface, int value);

// value gain position 0x04~0x0f
int visca_set_gain_limit_value(VISCAInterface_t *iface, int value);

//----------CAM_RGain
int visca_set_rgain_reset(VISCAInterface_t *iface);
int visca_set_rgain_up(VISCAInterface_t *iface);
int visca_set_rgain_down(VISCAInterface_t *iface);
// value: 0x0~0xff
int visca_set_rgain_value(VISCAInterface_t *iface, int value);

//----------CAM_BGain
int visca_set_bgain_reset(VISCAInterface_t *iface);
int visca_set_bgain_up(VISCAInterface_t *iface);
int visca_set_bgain_down(VISCAInterface_t *iface);
// value: 0x0~0xff
int visca_set_bgain_value(VISCAInterface_t *iface, int value);

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
int visca_set_whitebal_mode(VISCAInterface_t *iface, int mode);
int visca_set_whitebal_one_push(VISCAInterface_t *iface);

//----------CAM_AE
/*
***
mode
0-Automatic Exposure mode
0x03-Manual Control mode
0x0a-Shutter Priority Automatic Exposure mode
0x0b-Iris Priority Automatic Exposure mode
0x0d-Bright Mode (Manual control)
*/
int visca_set_ae_mode(VISCAInterface_t *iface, int mode);

//----------CAM_AFMode
/*
0-Normal AF
1-Interval AF
2-Zoom Trigger AF
*/
// not used!!!
int visca_set_af_mode(VISCAInterface_t *iface, int mode);
/*
*ONLY* in Interval AF mode!!!
active - 0~0xff
interval - 0~0xff
*/
// not used!!!
int visca_set_af_intervaltime(VISCAInterface_t *iface, int active, int interval);

//---------- AF Sensitivity
/*
0-Normal
1-Low
*/
int visca_set_focus_autosense_mode(VISCAInterface_t *iface, int mode);

//----------CAM_Iris
// *ONLY* used in "Iris Priority Automatic Exposure mode"
int visca_set_iris_reset(VISCAInterface_t *iface);
int visca_set_iris_up(VISCAInterface_t *iface);
int visca_set_iris_down(VISCAInterface_t *iface);
// value: iris postion 0x00~0x11
int visca_set_iris_value(VISCAInterface_t *iface, int value);

//----------CAM_IRCorrection

//----------CAM_ICR
// mode 0 = off 1 = on
int visca_set_infraredmode(VISCAInterface_t *iface, int mode);
//----------CAM_AutoICR
//----------CAM_AutoICRAlarmReply

//----------CAM_Defog
// mode 0 - off leve: 1~4
int visca_set_defog(VISCAInterface_t *iface, int mode);
//----------CAM_FlickerCancel

//----------CAM_ExpComp
//mode 0 - Off; 1 - On
int visca_set_exposure_compensation_mode(VISCAInterface_t *iface, int mode);
//value: 0x0 ~ 0xE
int visca_set_exposure_compensation_value(VISCAInterface_t *iface, int value);

//----------CAM_BlackLight
//----------CAM_SpotAE
//----------CAM_AE_Response
//----------CAM_WD
int visca_set_wdr(VISCAInterface_t *iface, int wdr);
//----------CAM_Aperture
// 实际上是锐化功能
// mode 0 = reset 1 = up 2 = down
int visca_set_aperture(VISCAInterface_t *iface, int mode);
// value: 0~0x0f
int visca_set_aperture_value(VISCAInterface_t *iface, int value);

//----------CAM_HR
//----------CAM_NR
int visca_set_nr(VISCAInterface_t *iface, int nr);
//----------CAM_Gamma
int visca_set_gamma(VISCAInterface_t *iface, int gamma);
//----------CAM_HighSensitivity
//----------CAM_LR_Reverse
//----------CAM_Freeze
// power 1 = freeze 0 = off
int visca_set_freeze(VISCAInterface_t *iface, int power);

//----------CAM_PictureEffect
//----------CAM_PictureFlip
// mode 1 = flip 0 = no
int visca_set_pictureflip(VISCAInterface_t *iface, int mode);

//----------CAM_HR
// mode 1 = On; 0 = Off
int visca_set_hrmode(VISCAInterface_t *iface, int mode);

//----------CAM_Stabilizer
// mode 1 = On; 0 = Off
int visca_set_stabilizer(VISCAInterface_t *iface, int mode);

//----------CAM_Memory
// mode 0-reset 1-set 2-recall
// channel 1~6
// no ok
int visca_memory(VISCAInterface_t *iface, int mode, int channel);
//----------CAM_CUSTOM
//----------CAM_MemSave
//----------CAM_PrivacyZone
int visca_privacy_set_mask(VISCAInterface_t *iface, int id, int width, int height, int type);

int visca_privacy_set_display(VISCAInterface_t *iface, int id, int color);

int visca_privacy_clear_display(VISCAInterface_t *iface, int id);

int visca_privacy_set_pt(VISCAInterface_t *iface, int pan, int tilt);

int visca_privacy_set_ptz(VISCAInterface_t *iface, int id, int pan, int tilt, int zoom);

int visca_privacy_set_noninterlock_mask(VISCAInterface_t *iface, int id, int center_x, int center_y, int half_width, int half_height);

int visca_privacy_set_grid(VISCAInterface_t *iface, int type);

//----------CAM_IDWrite
//----------CAM_Alarm
//----------CAM_RegisterValue
int visca_set_camera_register(VISCAInterface_t *iface, int id, int value);

//----------CAM_ChromaSuppress
//----------CAM_ColorGain
// gain 0~0x0e
int visca_set_color_gain(VISCAInterface_t *iface, int gain);

//----------CAM_ColorHue
// value 0~0x0e
int visca_set_colorhue(VISCAInterface_t *iface, int value);
//----------CAM_Scene Mode
//----------CAM_Display
//----------CAM_MultiLineTitle

///////////////////////////////////////////////////////////////////////////////
// 查询类
//----------CAM_PowerInq
/*
*power: return value 0== power off 1==power on
*/
int visca_get_power(VISCAInterface_t *iface, int *power);
//----------CAM_ZoomPosInq
int visca_get_zoom_value(VISCAInterface_t *iface, int *value);
//----------CAM_FocusModeInq
// *power: 2-auto 3-manual
int visca_get_focus_mode(VISCAInterface_t *iface, int *power);
//----------CAM_FocusPosInq
// out: 4096(far)~53248(near)
int visca_get_focus_value(VISCAInterface_t *iface, int *value);
//----------CAM_AFSensitivityInq
int visca_get_focus_auto_sense(VISCAInterface_t *iface, int *mode);
//----------CAM_FocusNearLimitInq

//----------CAM_AFModeInq
// not used!!!
int visca_get_af_mode(VISCAInterface_t *iface, int *mode);
//----------CAM_AFTimeSettingInq
//----------CAM_IRCorrectionInq
//----------CAM_WBModeInq
// mode: see visca_set_whitebal_mode
int visca_get_whitebal_mode(VISCAInterface_t *iface, int *mode);
//----------CAM_RGainInq
int visca_get_rgain_value(VISCAInterface_t *iface, int *value);
//----------CAM_BGainInq
int visca_get_bgain_value(VISCAInterface_t *iface, int *value);
//----------CAM_AEModeInq
// mode: see visca_set_ae_mode
int visca_get_ae_mode(VISCAInterface_t *iface, int *mode);
//----------CAM_ShutterPosInq
int visca_get_shutter_value(VISCAInterface_t *iface, int *shutter);
//----------CAM_IrisPosInq
int visca_get_iris_value(VISCAInterface_t *iface, int *iris);
//----------CAM_GainPosInq
int visca_get_gain_value(VISCAInterface_t *iface, int *value);
//----------CAM_GainLimitInq
int visca_get_gain_limit_value(VISCAInterface_t *iface, int *value);

//----------CAM_BrightPosInq
//----------CAM_ExpCompModeInq
int visca_get_exposure_compensation_mode(VISCAInterface_t *iface, int* mode);
//----------CAM_ExpCompPosInq
int visca_get_exposure_compensation_value(VISCAInterface_t *iface, int* value);

//----------CAM_BackLightModeInq
//----------CAM_SpotAEModeInq
//----------CAM_SpotAEPosInq
//----------CAM_AE_ResponseInq
//----------CAM_WDModeInq
int visca_get_wdr(VISCAInterface_t *iface, int *wdr);
//----------CAM_WDParameterInq
//----------CAM_ApertureInq
int visca_get_aperture_value(VISCAInterface_t *iface, int *value);
//----------CAM_HRModeInq
//----------CAM_NRModeInq
int visca_get_nr(VISCAInterface_t *iface, int *nr);
//----------CAM_GammaInq
int visca_get_gamma(VISCAInterface_t *iface, int *gamma);
//----------CAM_HighSensitivityInq
//----------CAM_LR_ReverseModeInq
//----------CAM_FreezeModeInq
int visca_get_freeze(VISCAInterface_t *iface, int *power);
//----------CAM_PictureEffectModeInq
//----------CAM_PictureFlipModeInq
//----------CAM_ICRModeInq
int visca_get_infraredmode(VISCAInterface_t *iface, int mode);

//----------CAM_AutoICRModeInq
//----------CAM_AutoICRThresholdInq
//----------CAM_AutoICRAlarm ReplyInq
//----------CAM_MemoryInq
//----------CAM_MemSaveInq

//----------CAM_PrivacyDisplayInq
int visca_get_privacy_display(VISCAInterface_t *iface, int *value);
//----------CAM_PrivacyPanTiltInq
int visca_get_privacy_pt(VISCAInterface_t *iface, int *pan, int *tilt);
//----------CAM_PrivacyPTZInq
int visca_get_privacy_ptz(VISCAInterface_t *iface, int id, int *pan, int *tilt, int *zoom);
//----------CAM_PrivacyMonitorInq
int visca_get_privacy_monitor(VISCAInterface_t *iface, int *value);

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
int visca_get_color_gain(VISCAInterface_t *iface, int *gain);
//----------CAM_ColorHueInq
//----------CAM_DefogInq
// mode 0 - off other 1~4
int visca_get_defog(VISCAInterface_t *iface, int *mode);
//----------CAM_FlickerCancelInq
//----------CAM_DisplayModeInq
//----------CAM_RegisterValueInq
int visca_get_camera_register(VISCAInterface_t *iface, int id, int *value);


//----------CAM_HRModeInq
int visca_get_hrmode(VISCAInterface_t *iface, int* mode);

//----------CAM_LR_Reverse
int visca_set_lr_reverse_value(VISCAInterface_t *iface, int value);
//----------CAM_PictureFlip
int visca_set_picture_flip_value(VISCAInterface_t *iface, int value);
//----------CAM_VersionInq
int visca_get_camera_version(VISCAInterface_t *iface, char *value);
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
};
#endif

#endif /* __VISCA_PROTOCOL_H */
