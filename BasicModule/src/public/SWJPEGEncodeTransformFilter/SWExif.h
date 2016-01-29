#ifndef _EXIF_H
#define _EXIF_H


//--------------------------------------------------------------------------
// Describes tag values

#define TAG_INTEROP_INDEX          0x0001
#define TAG_INTEROP_VERSION        0x0002
#define TAG_IMAGE_WIDTH            0x0100
#define TAG_IMAGE_LENGTH           0x0101
#define TAG_BITS_PER_SAMPLE        0x0102
#define TAG_COMPRESSION            0x0103
#define TAG_PHOTOMETRIC_INTERP     0x0106
#define TAG_FILL_ORDER             0x010A
#define TAG_DOCUMENT_NAME          0x010D
#define TAG_IMAGE_DESCRIPTION      0x010E
#define TAG_MAKE                   0x010F
#define TAG_MODEL                  0x0110
#define TAG_SRIP_OFFSET            0x0111
#define TAG_ORIENTATION            0x0112
#define TAG_SAMPLES_PER_PIXEL      0x0115
#define TAG_ROWS_PER_STRIP         0x0116
#define TAG_STRIP_BYTE_COUNTS      0x0117
#define TAG_X_RESOLUTION           0x011A
#define TAG_Y_RESOLUTION           0x011B
#define TAG_PLANAR_CONFIGURATION   0x011C
#define TAG_RESOLUTION_UNIT        0x0128
#define TAG_TRANSFER_FUNCTION      0x012D
#define TAG_SOFTWARE               0x0131
#define TAG_DATETIME               0x0132
#define TAG_ARTIST                 0x013B
#define TAG_WHITE_POINT            0x013E
#define TAG_PRIMARY_CHROMATICITIES 0x013F
#define TAG_TRANSFER_RANGE         0x0156
#define TAG_JPEG_PROC              0x0200
#define TAG_THUMBNAIL_OFFSET       0x0201
#define TAG_THUMBNAIL_LENGTH       0x0202
#define TAG_Y_CB_CR_COEFFICIENTS   0x0211
#define TAG_Y_CB_CR_SUB_SAMPLING   0x0212
#define TAG_Y_CB_CR_POSITIONING    0x0213
#define TAG_REFERENCE_BLACK_WHITE  0x0214
#define TAG_RELATED_IMAGE_WIDTH    0x1001
#define TAG_RELATED_IMAGE_LENGTH   0x1002
#define TAG_CFA_REPEAT_PATTERN_DIM 0x828D
#define TAG_CFA_PATTERN1           0x828E
#define TAG_BATTERY_LEVEL          0x828F
#define TAG_COPYRIGHT              0x8298
#define TAG_EXPOSURETIME           0x829A
#define TAG_FNUMBER                0x829D
#define TAG_IPTC_NAA               0x83BB
#define TAG_EXIF_OFFSET            0x8769
#define TAG_INTER_COLOR_PROFILE    0x8773
#define TAG_EXPOSURE_PROGRAM       0x8822
#define TAG_SPECTRAL_SENSITIVITY   0x8824
#define TAG_GPSINFO                0x8825
#define TAG_ISO_EQUIVALENT         0x8827
#define TAG_OECF                   0x8828
#define TAG_EXIF_VERSION           0x9000
#define TAG_DATETIME_ORIGINAL      0x9003
#define TAG_DATETIME_DIGITIZED     0x9004
#define TAG_COMPONENTS_CONFIG      0x9101
#define TAG_CPRS_BITS_PER_PIXEL    0x9102
#define TAG_SHUTTERSPEED           0x9201
#define TAG_APERTURE               0x9202
#define TAG_BRIGHTNESS_VALUE       0x9203
#define TAG_EXPOSURE_BIAS          0x9204
#define TAG_MAXAPERTURE            0x9205
#define TAG_SUBJECT_DISTANCE       0x9206
#define TAG_METERING_MODE          0x9207
#define TAG_LIGHT_SOURCE           0x9208
#define TAG_FLASH                  0x9209
#define TAG_FOCALLENGTH            0x920A
#define TAG_SUBJECTAREA            0x9214
#define TAG_MAKER_NOTE             0x927C
#define TAG_USERCOMMENT            0x9286
#define TAG_SUBSEC_TIME            0x9290
#define TAG_SUBSEC_TIME_ORIG       0x9291
#define TAG_SUBSEC_TIME_DIG        0x9292

#define TAG_WINXP_TITLE            0x9c9b // Windows XP - not part of exif standard.
#define TAG_WINXP_COMMENT          0x9c9c // Windows XP - not part of exif standard.
#define TAG_WINXP_AUTHOR           0x9c9d // Windows XP - not part of exif standard.
#define TAG_WINXP_KEYWORDS         0x9c9e // Windows XP - not part of exif standard.
#define TAG_WINXP_SUBJECT          0x9c9f // Windows XP - not part of exif standard.

#define TAG_FLASH_PIX_VERSION      0xA000
#define TAG_COLOR_SPACE            0xA001
#define TAG_PIXEL_X_DIMENSION      0xA002
#define TAG_PIXEL_Y_DIMENSION      0xA003
#define TAG_RELATED_AUDIO_FILE     0xA004
#define TAG_INTEROP_OFFSET         0xA005
#define TAG_FLASH_ENERGY           0xA20B
#define TAG_SPATIAL_FREQ_RESP      0xA20C
#define TAG_FOCAL_PLANE_XRES       0xA20E
#define TAG_FOCAL_PLANE_YRES       0xA20F
#define TAG_FOCAL_PLANE_UNITS      0xA210
#define TAG_SUBJECT_LOCATION       0xA214
#define TAG_EXPOSURE_INDEX         0xA215
#define TAG_SENSING_METHOD         0xA217
#define TAG_FILE_SOURCE            0xA300
#define TAG_SCENE_TYPE             0xA301
#define TAG_CFA_PATTERN            0xA302
#define TAG_CUSTOM_RENDERED        0xA401
#define TAG_EXPOSURE_MODE          0xA402
#define TAG_WHITEBALANCE           0xA403
#define TAG_DIGITALZOOMRATIO       0xA404
#define TAG_FOCALLENGTH_35MM       0xA405
#define TAG_SCENE_CAPTURE_TYPE     0xA406
#define TAG_GAIN_CONTROL           0xA407
#define TAG_CONTRAST               0xA408
#define TAG_SATURATION             0xA409
#define TAG_SHARPNESS              0xA40A
#define TAG_DISTANCE_RANGE         0xA40C
#define TAG_IMAGE_UNIQUE_ID        0xA420

#define FMT_BYTE       1
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_ExifInfo{
    CHAR  szCameraMake[32];     // 设备制作商
    CHAR  szCameraModel[32];    // 设备型号(及编号)
    CHAR  szDateTime[20];       // 时间信息 格式："YYYY:MM:DD HH:MM:SS"+0x00, 一共 20个字节
    INT iExposureTime;          // 曝光时间　单位：us
    INT iGain;                  // 增益
    INT iContrast;              //对比度
    INT iSaturation;            //饱和度
    INT iSharpness;             //亮度
    INT iAutoWB;                //自动白平衡
    INT iFilter;                // 滤光片   TODO:是否是字符串形式？
}HvExifInfo;

#define EXIF_BUFFER_MAX_SIZE   512

VOID CreateExif(HvExifInfo* pExifInfo, CHAR* pExifBuffer, INT& iExifLen);

#ifdef __cplusplus
};
#endif
#endif
