#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "swImage.h"
#include "swBaseType.h"

#ifdef __cplusplus
extern "C" {
#endif		// #ifdef __cplusplus

// 下面的常量定义了下列函数可能的返回值.
#define INVALD_POINTER				-1
#define BUSY_FIELD					-2
#define INVALID_DEVICE				-3
#define INVALID_FORMAT				-4
#define DEVICE_FAIL					-5

#define LOG_CAT_LIST(MACRO_NAME)		\
	MACRO_NAME(COMM)					\
	MACRO_NAME(DETECT)					\
	MACRO_NAME(SEGMENT)					\
	MACRO_NAME(RECOGNIZE)				\
	MACRO_NAME(TRACK)					\
	MACRO_NAME(OTHER)					\
	MACRO_NAME(ERROR)					\
	MACRO_NAME(TIME)					\
	MACRO_NAME(VOTE)					\
	MACRO_NAME(DISPLAY)					\
	MACRO_NAME(EXP_DATA)				\
	MACRO_NAME(VIDEO_DET)				\
	MACRO_NAME(DETECT_DETAIL)			\

#define DEFINE_DEBUG_DEVICE_ENUM(a)		\
	DEBUG_STR_##a##_FILE,				\

enum DebugDevices
{
	LOG_CAT_LIST(DEFINE_DEBUG_DEVICE_ENUM)
	DEBUG_STR_DEVICE_COUNT
};

extern int g_rgfDebugDeviceEnabled[ DEBUG_STR_DEVICE_COUNT ];
extern LPCSTR g_rgkszDebugDeviceName[ DEBUG_STR_DEVICE_COUNT ];
extern char g_rgkszDebugLogPath[ DEBUG_STR_DEVICE_COUNT ][ 260 ];
extern BOOL g_fSaveLogFile;

__inline LPCSTR GetDebugDeviceName( int iDeviceID )
{
	return g_rgkszDebugDeviceName[iDeviceID];
}

__inline void EnableDebugDevice( int iDeviceID, int fEnable ) 
{
	g_rgfDebugDeviceEnabled[ iDeviceID ] = fEnable;
}

__inline int IsDebugDeviceEnabled( int iDeviceID ) 
{
	return g_rgfDebugDeviceEnabled[ iDeviceID ];
}

__inline LPCSTR GetDebugLogPath(int iDeviceID)
{
	return g_rgkszDebugLogPath[iDeviceID];
}

__inline void SetSaveLogFile(BOOL fSave)
{
	g_fSaveLogFile=fSave;
}

extern int UartWriteData( BYTE8 *pDataSrc, int iDataLenb, int iType );
extern int UartReadData( BYTE8 *pDataDest, int iReadLen, int iType );
extern int UartWriteString( LPSTR pszString, int iType );

// 下面的常量定义了 OutputString 中, 参数 iDeviceID 可能的取值.
#define NULL_DEVICE					255	//a

// 宏定义: int OutputString( char *pString, int iDeviceID )
// 描述: 在不同的设备上输出"结果字符串"
// 参数: pString	指向需要输出的字符串. 如果该参数为 NULL, 函数返回 INVALD_POINTER
//		 iDeviceID	给出需要在什么设备上输出"结果字符串". 其可能的取值由上面的常量定义. 如果其取值不是上面的常量中的
//					任何一个, 则本函数返回 INVALID_DEVICE.
//					iDeviceID == NULL_DEVICE 本宏是一个空宏, 没有调用任何函数, 不导致任何运行开销. 这时 pImageInfo 和
//					pImage 可以取任何值, 不会导致错误.
//					iDeviceID == RESULT_STR_FILE 时 pString 将被写入结果文件中. 结果文件的存放位置由实现决定.
//					iDeviceID == DISPLAY_STR_COUNT 时 pString 将显示在界面的"车辆计数"文本域中.
//					iDeviceID == DISPLAY_STR_RESULT 时 pString 将显示在界面的"识别结果"文本域中.
//					iDeviceID == DEBUG_STR_DISPLAY1 时 pString 将显示在界面的"调试信息"文本域中.
//					iDeviceID == DEBUG_STR_DISPLAY2 时 pString 将显示在界面的"调试信息2"文本域中. 目前没有实现"调试信
//					息2"文本域
//					iDeviceID == DEBUG_STR_COMM_FILE 时 pString 将被写入"通用调试信息"文件中. "通用调试信息"文件的存放
//					位置由实现决定.
//					iDeviceID == DEBUG_STR_DETECT_FILE 时 pString 将被写入"检测调试信息"文件中. "检测调试信息"文件的存放
//					位置由实现决定.
//					iDeviceID == DEBUG_STR_SEGMENT_FILE 时 pString 将被写入"分割调试信息"文件中. "分割调试信息"文件的存放
//					位置由实现决定.
//					iDeviceID == DEBUG_STR_RECOGNIZ_FILE 时 pString 将被写入"识别调试信息"文件中. "识别调试信息"文件的存放
//					位置由实现决定.
//					iDeviceID == DEBUG_STR_OTHER_FILE 时 pString 将被写入"其他调试信息"文件中. "其他调试信息"文件的存放
//					位置由实现决定.
//					之所以将调试信息做如此详细的划分, 是因为不同阶段的任务可能在不同的线程中运行, 详细的划分可以避免调试
//					信息文件的内容因线程推进顺序问题变得杂乱无章
// 返回: 成功时返回 0.
//		 如果 pString 为 NULL, 函数返回 INVALD_POINTER
//		 如果 iDeviceID 的取值非法, 则返回 INVALID_DEVICE.
//		 如果无法在指定设备上输出, 则返回 DEVICE_FAIL.
// 注意: 由于目前的实现是在预处理阶段判断 iDeviceID 是否为空设备, 因此调用本宏时 iDeviceID 必须是一个常量, 否则将导致
//		 编译错误.
int DoOutputString( char *pString, int iDeviceID );

BOOL OpenLogFile(int iDeviceID,char* lpszFileName,char* lpszMode);

BOOL CloseLogFile(int iDeviceID);

BOOL IsLogFileOpened(int iDeviceID);

#define OutputString( pString, iDeviceID )			\
	if ( iDeviceID != NULL_DEVICE ) {				\
			DoOutputString( pString, iDeviceID );	\
	}

// 下面的常量定义了 OutputImage 中, 参数 iDeviceID 可能的取值.
#define RESULT_IMG_FILE				1
#define DISPLAY_IMG_COMM_DEV		2
#define DISPLAY_IMG_DETECT_DEV		3
#define DISPLAY_IMG_SEGMENT_DEV		4
#define DISPLAY_IMG_RECOGNIZ_DEV	5
#define DEBUG_IMG_FILE				NULL_DEVICE

// 宏定义: int OutputImage( char *pImageInfo, HV_IMAGE *pImage, int iDeviceID )
// 描述: 在不同的设备上输出非分量存储格式的"结果图象"
// 参数: pImageInfo	指向需要输出的"结果图象"的有关信息. 当 iDeviceID 取某些特定值时本参数被忽略, 这时其取值可以为 NULL,
//					否则它必须指向特定格式的字符串, 详见 iDeviceID 的有关说明.
//		 pImage		指向需要输出的非分量"结果图象". 如果该参数为 NULL, 本函数返回 INVALD_POINTER.
//		 iDeviceID	给出需要在什么设备上输出"结果图象". 其可能的取值由上面的常量定义. 如果其取值不是上面的常量中的
//					任何一个, 则本宏返回 INVALID_DEVICE.
//					iDeviceID == NULL_DEVICE 本宏是一个空宏, 没有调用任何函数, 不导致任何运行开销. 这时 pImageInfo 和
//					pImage 可以取任何值, 不会导致错误.
//					iDeviceID == RESULT_IMG_FILE 时 pImageInfo 都应指向一个合法的文件名(包括目录名), 本函数将把 pImage
//					指出的图象保存在该文件中.
//					iDeviceID == DEBUG_IMG_FILE 时 pImageInfo 都应指向一个合法的文件名(包括目录名), 本函数将把 pImage
//					指出的图象保存在该文件中.
//					iDeviceID == DISPLAY_* 时 pImageInfo 可以为 NULL, 如果它不为 NULL, 本函数将把该字符串显示在图形下面
//					的文本域中. 如果界面上没有为该图形域设计相应的文本域, 则 pImageInfo 被忽略.
// 返回: 成功时返回 0.
//		 如果 pImageInfo 为 NULL, 函数返回 INVALD_POINTER
//		 如果 pImage 为 NULL, 函数返回 INVALD_POINTER
//		 如果 iDeviceID 的取值非法, 则返回 INVALID_DEVICE.
//		 如果无法在指定设备上输出, 则返回 DEVICE_FAIL.
// 注意: 由于目前的实现是在预处理阶段判断 iDeviceID 是否为空设备, 因此调用本宏时 iDeviceID 必须是一个常量, 否则将导致
//		 编译错误.
int DoOutputImage( char *pImageInfo, HV_COMPONENT_IMAGE *pImage, int iDeviceID );

#define OutputImage( pImageInfo, pImage, iDeviceID )		\
	if ( iDeviceID != NULL_DEVICE ) {						\
			DoOutputImage( pImageInfo, pImage, iDeviceID );	\
	}

// 宏定义: int OutputComponentImage( char *pImageInfo, HV_COMPONENT_IMAGE *pImage, int iDeviceID )
// 描述: 在不同的设备上输出分量形式的"结果图象"
// 参数: pImageInfo	指向需要输出的"结果图象"的有关信息. 当 iDeviceID 取某些特定值时本参数被忽略, 这时其取值可以为 NULL,
//					否则它必须指向特定格式的字符串, 详见 iDeviceID 的有关说明.
//		 pImage		指向需要输出的分量形式"结果图象". 如果该参数为 NULL, 本函数返回 INVALD_POINTER.
//		 iDeviceID	给出需要在什么设备上输出"结果图象". 其可能的取值由上面的常量定义. 如果其取值不是上面的常量中的
//					任何一个, 则本宏返回 INVALID_DEVICE.
//					iDeviceID 的取值及含义与 OutputImage 宏完全一致.
// 返回: 成功时返回 0.
//		 如果 pImageInfo 为 NULL, 函数返回 INVALD_POINTER
//		 如果 pImage 为 NULL, 函数返回 INVALD_POINTER
//		 如果 iDeviceID 的取值非法, 则返回 INVALID_DEVICE.
//		 如果无法在指定设备上输出, 则返回 DEVICE_FAIL.
// 注意: 由于目前的实现是在预处理阶段判断 iDeviceID 是否为空设备, 因此调用本宏时 iDeviceID 必须是一个常量, 否则将导致
//		 编译错误.
int DoOutputComponentImage( char *pImageInfo, HV_COMPONENT_IMAGE *pImage, int iDeviceID );

#define OutputComponentImage( pImageInfo, pImage, iDeviceID )			\
	if ( iDeviceID != NULL_DEVICE ) {									\
			DoOutputComponentImage( pImageInfo, pImage, iDeviceID );	\
	}

// 宏定义: int GetSystemTick( void )
// 描述: 取得系统 Tick
// 参数: 无
// 返回: 成功时返回系统 Tick. 目前无其他返回.
// 注意: 系统 Tick 有系统实现有关, 其绝对时间长度不确定. 目前在 DSP_BIOS 和 WINDOWS 中其取值都是 1 ms.
DWORD32 GetSystemTick( void );
void BreakPoint();
int Random();

#ifdef __cplusplus
}
#endif		// #ifdef __cplusplus


#endif		// #ifndef __INTERFACE_H__
