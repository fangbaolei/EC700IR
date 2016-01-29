// 内部逻辑实现

#include "HvDeviceUtils.h"
#include "HVAPI_HANDLE_CONTEXT.h"
#include <string>
#include <vector>
#include <list>
#include "Console.h"
#include "ximage.h"
#pragma warning( disable : 4996 )
#include <atlstr.h>

using namespace HiVideo;
extern void WrightLogEx(LPCSTR lpszIP, LPCSTR lpszLogInfo);

#ifdef _WIN32
#define 	 hvstricmp stricmp
#else
#define     hvstricmp strcasecmp 
#endif
//XML的一个属性
struct SXmlAttr
{
	std::string            strName;
	std::string            strValue;
	
	SXmlAttr()
	{
		strName = "";
		strValue = "";
	}
};

//XML命令
struct SXmlCmd
{
	std::string            strCmdName;
	std::vector<SXmlAttr>  listAttr;

	SXmlCmd()
	{
		strCmdName = "";
		listAttr.clear();
	}
};


// 水星协议命令类型
typedef enum {
	XML_CMD_GETTER, // 命令输出数据
	XML_CMD_SETTER  // 命令输入数据
} XML_CMD_CLASS;
//XML命令集合
typedef std::vector<SXmlCmd>    XmlCmdList;

//XML命令
struct SXmlCmdMercury
{
	std::string            strCmdName;
	std::vector<SXmlAttr>  listAttr;
	XML_CMD_CLASS          emCmdClass;
	XML_CMD_TYPE           emCmdType;
    int                    iArrayColumn; // INTARRAY1D的长度，INTARRAY2D的列数
    int                    iArrayRow;    // INTARRAY2D的行数

	SXmlCmdMercury()
	{
		strCmdName = "";
		listAttr.clear();
		emCmdClass = XML_CMD_GETTER;
		emCmdType = XML_CMD_TYPE_INT;
		iArrayColumn = 0;
		iArrayRow = 0;
	}
};

//XML命令集合
typedef std::vector<SXmlCmdMercury>    XmlCmdListMercury;
	
// 用于填充 XML_CMD_APPENDINFO_MAP_ITEM 结构体的宏
#define EMPTY_PARAMNAME32 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME31 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME30 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME29 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME28 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME27 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME26 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME25 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME24 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME23 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME22 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME21 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME20 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME19 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME18 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME17 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME16 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME15 "" , "", "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME14 "" , "", "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME13 "" , "", "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME12 "" , "", "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME11 "" , "", "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME10 "" , "", "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME9 "" , "", "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME8 "" , "", "", "", "", "", "", ""
#define EMPTY_PARAMNAME7 "" , "", "", "", "", "", ""
#define EMPTY_PARAMNAME6 "" , "", "", "", "", ""
#define EMPTY_PARAMNAME5 "" , "", "", "", ""
#define EMPTY_PARAMNAME4 "" , "", "", ""
#define EMPTY_PARAMNAME3 "" , "", ""
#define EMPTY_PARAMNAME2 "" , ""
#define EMPTY_PARAMNAME1 "" 

// 用于填充 XML_CMD_APPENDINFO_MAP_ITEM 结构体的宏
#define XML_CMD_TYPE_NULL_ENTRY()               XML_CMD_TYPE_NULL, 0, 0
#define XML_CMD_TYPE_CUSTOM_ENTRY()             XML_CMD_TYPE_CUSTOM, 0, 0
#define XML_CMD_TYPE_INT_ENTRY()                XML_CMD_TYPE_INT, 0, 0
#define XML_CMD_TYPE_DOUBLE_ENTRY()             XML_CMD_TYPE_DOUBLE, 0, 0
#define XML_CMD_TYPE_FLOAT_ENTRY()              XML_CMD_TYPE_FLOAT, 0, 0
#define XML_CMD_TYPE_BOOL_ENTRY()               XML_CMD_TYPE_BOOL, 0, 0
#define XML_CMD_TYPE_DWORD_ENTRY()              XML_CMD_TYPE_DWORD, 0, 0
#define XML_CMD_TYPE_STRING_ENTRY()             XML_CMD_TYPE_STRING, 0, 0
#define XML_CMD_TYPE_BIN_ENTRY()				XML_CMD_TYPE_BIN, 0, 0
#define XML_CMD_TYPE_INTARRAY1D_ENTRY(a)        XML_CMD_TYPE_INTARRAY1D, (a) , 0
#define XML_CMD_TYPE_INTARRAY2D_ENTRY(a, b)     XML_CMD_TYPE_INTARRAY2D, (a) , (b)

typedef struct tag_XmlCmdAppendInfoMapItem
{
    char szCmdName[32];
    XML_CMD_CLASS emCmdClass;
    XML_CMD_TYPE emCmdType;
    int iArrayColumn;         // INTARRAY1D的长度，INTARRAY2D的列数
    int iArrayRow;            // INTARRAY2D的行数
//    int iRetLen; // 可以根据emCmdClass和emCmdType算出
    int iParamNum;           //若emCmdClass 为XML_CMD_SETTER 则为输入参数数量 ，若为XML_CMD_GETTER 则为输出参数数量
    char szParamName1[32];
    char szParamName2[32];
    char szParamName3[32];
    char szParamName4[32];
    char szParamName5[32];
    char szParamName6[32];
    char szParamName7[32];
    char szParamName8[32];
    char szParamName9[32];
    char szParamName10[32];
    char szParamName11[32];
    char szParamName12[32];
    char szParamName13[32];
    char szParamName14[32];
    char szParamName15[32];
    char szParamName16[32];
    char szParamName17[32];
    char szParamName18[32];
    char szParamName19[32];
    char szParamName20[32];
    char szParamName21[32];
    char szParamName22[32];
    char szParamName23[32];
    char szParamName24[32];
    char szParamName25[32];
    char szParamName26[32];
    char szParamName27[32];
    char szParamName28[32];
    char szParamName29[32];
    char szParamName30[32];
    char szParamName31[32];
    char szParamName32[32];
} XML_CMD_APPENDINFO_MAP_ITEM;

#define DEFAULT_SETTER     XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Value", EMPTY_PARAMNAME31
#define DEFAULT_GETTER     XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "value", EMPTY_PARAMNAME31
// 水星协议需要为每个命令附加Type、Value、Class信息
XML_CMD_APPENDINFO_MAP_ITEM g_XmlCmdAppendInfoMap[] =
{
    //水星Device修改 将SetTime 类型换为XML_CMD_TYPE_CUSTOM_ENTRY
//	{ "SetTime", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(7), 2, "Date", "Time", EMPTY_PARAMNAME30 }, // 把年、月、日、时、分、秒、毫秒分成7个整形组成数组传输
    { "SetTime", XML_CMD_SETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 1, "SetTime", EMPTY_PARAMNAME31 }, // 把年、月、日、时、分、秒、毫秒分成7个整形组成字符串传输
    { "GetWorkModeIndex", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "WorkModeName", EMPTY_PARAMNAME31 },
    { "SetIP", XML_CMD_SETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 1, EMPTY_PARAMNAME31 }, // 像原来一样使用多个属性来表达IP、NETMASK、GATEWAY
    { "SetOptWorkMode", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "WorkMode", EMPTY_PARAMNAME31 },
    { "ForceSend", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "SetShutter", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Shutter", EMPTY_PARAMNAME31 },
    { "GetShutter", DEFAULT_GETTER },
    { "SetGain", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Gain", EMPTY_PARAMNAME31 },
    { "GetGain", DEFAULT_GETTER },
    { "SetRgbGain", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(3), 3, "GainR", "GainG", "GainB", EMPTY_PARAMNAME29 },
    { "GetRgbGain", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(3), 3, "GainR", "GainG", "GainB", EMPTY_PARAMNAME29 },
    { "SetCaptureShutter", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "Shutter", "Enable", EMPTY_PARAMNAME30 },
    { "GetCaptureShutter", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "Shutter", "Enable", EMPTY_PARAMNAME30 },
    { "SetCaptureGain", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "Gain", "Enable", EMPTY_PARAMNAME30 },
    { "GetCaptureGain", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "Gain", "Enable", EMPTY_PARAMNAME30 },
    { "SetCaptureRgbGain", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(4), 4, "GainR", "GainG", "GainB", "Enable", EMPTY_PARAMNAME28 },
    { "GetCaptureRgbGain", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(4), 4, "GainR", "GainG", "GainB", "Enable", EMPTY_PARAMNAME28 },
    { "SoftTriggerCapture", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "SetEncodeMode", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "EncodeMode", EMPTY_PARAMNAME31 },
    { "SetENetSyn", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Enable", EMPTY_PARAMNAME31 },
    { "GetENetSyn", DEFAULT_GETTER },
    { "SetAgcLightBaseline", DEFAULT_SETTER },
    { "GetAgcLightBaseline", DEFAULT_GETTER },
    { "SetGammaData", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(16), 16, "Point0_X", "Point0_Y", "Point1_X", "Point1_Y", "Point2_X", "Point2_Y", "Point3_X", "Point3_Y", "Point4_X", "Point4_Y", "Point5_X", "Point5_Y", "Point6_X", "Point6_Y", "Point7_X", "Point7_Y", EMPTY_PARAMNAME16 },
    { "GetGammaData", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(16), 16, "Point0_X", "Point0_Y", "Point1_X", "Point1_Y", "Point2_X", "Point2_Y", "Point3_X", "Point3_Y", "Point4_X", "Point4_Y", "Point5_X", "Point5_Y", "Point6_X", "Point6_Y", "Point7_X", "Point7_Y", EMPTY_PARAMNAME16 },
    { "SetAGCZone", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(16), 16, "AGCZone00", "AGCZone01", "AGCZone02", "AGCZone03", "AGCZone04", "AGCZone05", "AGCZone06", "AGCZone07", "AGCZone08", "AGCZone09", "AGCZone10", "AGCZone11", "AGCZone12", "AGCZone13", "AGCZone14", "AGCZone15", EMPTY_PARAMNAME16 },
    { "GetAGCZone", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(16), 16, "AGCZone00", "AGCZone01", "AGCZone02", "AGCZone03", "AGCZone04", "AGCZone05", "AGCZone06", "AGCZone07", "AGCZone08", "AGCZone09", "AGCZone10", "AGCZone11", "AGCZone12", "AGCZone13", "AGCZone14", "AGCZone15", EMPTY_PARAMNAME16 },
    { "SetCaptureEdge", DEFAULT_SETTER },
    { "GetCaptureEdge", DEFAULT_GETTER }, // 原来的设备是直接返回E_FAIL的
    { "SetAGCEnable", DEFAULT_SETTER },
    { "GetAGCEnable", DEFAULT_GETTER },
    { "SetAWBEnable", DEFAULT_SETTER },
    { "GetAWBEnable", DEFAULT_GETTER },
    { "SetAGCParam", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(4), 4, "ShutterMin", "ShutterMax", "GainMin", "GainMax", EMPTY_PARAMNAME28 },
    { "GetAGCParam", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(4), 4, "ShutterMin", "ShutterMax", "GainMin", "GainMax", EMPTY_PARAMNAME28 },
    { "SetJpegCompressRate", DEFAULT_SETTER },
    { "GetJpegCompressRate", DEFAULT_GETTER },
    { "SetJpegCompressRateCapture", DEFAULT_SETTER },
    { "GetJpegCompressRateCapture", DEFAULT_GETTER },
    { "SetFlashRateSynSignalEnable", DEFAULT_SETTER },
    { "GetFlashRateSynSignalEnable", DEFAULT_GETTER },
    { "SetCaptureSynSignalEnable", DEFAULT_SETTER },
    { "GetCaptureSynSignalEnable", DEFAULT_GETTER },
    { "ResetDevice", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "ResetMode", EMPTY_PARAMNAME31 },
    { "RestoreDefaultParam", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "RestoreFactoryParam", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "SaveADSamplingValue", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "ValueA", "ValueB", EMPTY_PARAMNAME30 },
//    { "SetControllPannelStatus", HvXml_SetControllPannelWorkStatus },
    { "SetPulseWidthRange", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "PulseWidthMin", "PulseWidthMax", EMPTY_PARAMNAME30 },
    { "TestPolarizingPrismMode", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Mode", EMPTY_PARAMNAME31 },
    { "SetFlashSingle", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(5), 5, "Channel", "Polarity", "TriggerType", "PulseWidth", "Coupling", EMPTY_PARAMNAME27 },
//    { "SaveControllPannelConfig", HvXml_SaveControllPannelConfig },
//    { "SetControllPannelAutoRunStatus", HvXml_SetControllPannelAutoRunStatus },
    { "SetControllPannelPulseLevel", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Level", EMPTY_PARAMNAME31 },
//    { "SetControllPannelDefParam", HvXml_SetControllPannelDefParam },
    { "SaveParam", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "SetCharacterEnable", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "EnableCharacter", EMPTY_PARAMNAME31 },
    { "GetCharacterEnable", DEFAULT_GETTER },
    { "GetCharacterInfo", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(7), 7, "x", "y", "DateFormat", "FontColor", "CharacterWidth", "CharacterHeight", "FixedLight", EMPTY_PARAMNAME25 },
    { "SaveCharacterInfo", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "SetCharacterFixedLight", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "value", EMPTY_PARAMNAME31},
	{ "SetCtrlCpl" , XML_CMD_SETTER ,XML_CMD_TYPE_BOOL_ENTRY() ,1 , "Enable" ,  EMPTY_PARAMNAME31},   // 设置偏振镜状态
	{ "SetDCAperture", XML_CMD_SETTER , XML_CMD_TYPE_INT_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 }, //设置DC光圈状态
	{ "TriggerPlateRecog", XML_CMD_SETTER , XML_CMD_TYPE_INT_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 }, 

    { "GetBlackBoxMessage",XML_CMD_GETTER , XML_CMD_TYPE_STRING_ENTRY() , 1 , "RetMsg" , EMPTY_PARAMNAME31 },  //获取黑匣子消息
	{ "GetCusTomInfo",XML_CMD_GETTER , XML_CMD_TYPE_STRING_ENTRY() , 1 , "RetMsg" , EMPTY_PARAMNAME31 },  //获取黑匣子消息
	{ "SetCusTomInfo", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },

	

    //原Info
//    水星Device修改 将DateTime 类型换为XML_CMD_TYPE_CUSTOM_ENTRY
//    { "DateTime", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(7), 2, "Date", "Time", EMPTY_PARAMNAME30 }, 
    { "DateTime", XML_CMD_GETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 1, "DateTime", EMPTY_PARAMNAME31 },
    { "HvName", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "Name", EMPTY_PARAMNAME31 },
//    { "HvID", HvXml_GetHvID },
    { "OptResetCount", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "ResetCount", EMPTY_PARAMNAME31 },
    { "OptWorkMode", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "WorkMode", EMPTY_PARAMNAME31 },
    { "OptWorkModeCount", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "WorkModeCount", EMPTY_PARAMNAME31 },
    { "OptResetMode", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "ResetMode", EMPTY_PARAMNAME31 },
    { "OptProductName", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "ProductName", EMPTY_PARAMNAME31 },
    { "GetVideoCount", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "VideoCount", EMPTY_PARAMNAME31 },
    { "GetVersion", XML_CMD_GETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 2, "SoftVersion", "ModelVersion", EMPTY_PARAMNAME30 },
    { "GetVersionString", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "SoftVersionString", EMPTY_PARAMNAME31 },
    { "GetConnectedIP", XML_CMD_GETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 16, "COUNT", "IP1", "IP2", "IP3", "IP4", "IP5", "IP6", "IP7", "IP8", "IP9", "IP10", "IP11", "IP12", "IP13", "IP14", "IP15" },
    { "GetHddOpStatus", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "HddOpStatus", EMPTY_PARAMNAME31 },
    { "GetHddCheckReport", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "HddCheckReport", EMPTY_PARAMNAME31 },
    { "GetResetReport", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "ResetReport", EMPTY_PARAMNAME31 },
    { "GetDevType", XML_CMD_GETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 2, "DevType", "BuildNo", EMPTY_PARAMNAME30 },
    { "GetCpuTemperature", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "Value", EMPTY_PARAMNAME31 },
    { "GetEncodeMode", XML_CMD_GETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "EncodeMode", EMPTY_PARAMNAME31 },
	{ "GetPlateRecogerIP", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "RetMsg", EMPTY_PARAMNAME31 },//获取185抓拍相机(海外)
//    { "GetControllPannelVersion", HvXml_GetControllPannelVersion },
//    { "GetControllPannelTemperature", HvXml_GetControllPannelTemperature },
//    { "GetControllPannelStatus", HvXml_GetControllPannelStatus },
    { "GetControllPannelPulseInfo", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(4), 4, "PulseLevel", "PulseStep", "PulseWidthMin", "PulseWidthMax", EMPTY_PARAMNAME28 },
//    { "GetControllPannelDeviceStatus", HvXml_GetControllPannelDeviceStatus },
    { "GetControllPannelFlashInfo", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(12), 12, "Flash1PulseWidth", "Flash1Polarity", "Flash1Coupling", "Flash1ResistorMode", "Flash2PulseWidth", "Flash2Polarity", "Flash2Coupling", "Flash2ResistorMode", "Flash3PulseWidth", "Flash3Polarity", "Flash3Coupling", "Flash3ResistorMode", EMPTY_PARAMNAME20 },
//    { "GetControllPannelUpdatingStatus", HvXml_GetControllPannelUpdatingStatus },
//    { "GetControllPannelCRCValue", HvXml_GetControllPannelCRCValue },
//    { "GetControllPannelAutoRunStatus", HvXml_GetControllPannelAutoRunStatus },
//    { "GetControllPannelAutoRunStle", HvXml_GetControllPannelAutoRunStyle },
    { "DoPartition", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "DoCheckDisk", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
    { "GetCheckDisk", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "RetMsg", EMPTY_PARAMNAME31 },
	{ "GetRunStatus", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "RunStatus", EMPTY_PARAMNAME31 },
	{ "GetCameraWorkState", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "RunStatus", EMPTY_PARAMNAME31 },
	{ "SendTriggerOut", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
	{ "SendAlarmOut", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
	{ "SetImgType" ,XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },
	{ "SetNameList", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },
	{ "GetNameList", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "RetMsg" , EMPTY_PARAMNAME31 },



	// 以下是金星需求关键字
	//设备管理
	/*GetDevType*/ //上面已经有了{ "GetDevType", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 2, "DevType", "BuildNo",EMPTY_PARAMNAME30 },
	/*GetDevRunMode*/ { "OptResetMode", DEFAULT_GETTER },
	/*GetDevBasicInfo*/ { "GetDevBasicInfo", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 16 ,"Mode","SN","DevType","WorkMode","DevVersion","IP","Mac","Mask","Gateway","BackupVersion","FPGAVersion","KernelVersion","UbootVersion","UBLVersion", "FirmwareVersion", "NetPackageVersion", EMPTY_PARAMNAME16 },
	/*GetDevState*/ { "GetDevState", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 15, "CpuUsage","MemUsage","CpuTemperature","RecordLinkIP","ImageLinkIP","VideoLinkIP","Date","Time","HddOpStatus","ResetCount","TimeZone","NTPEnable","NTPServerIP","NTPServerUpdateInterval","TraceRank",EMPTY_PARAMNAME17 },
	/*GetResetCount* / /*OptResetCount替换*///{ "GetResetCount", DEFAULT_GETTER },
	/*GetHddCheckReport *///上面已经有了 { "GetHddCheckReport", XML_CMD_GETTER ,  XML_CMD_TYPE_STRING_ENTRY (), 1, "HddCheckReport", EMPTY_PARAMNAME31  },
	/*SetIP*/  //上面已经有了{ "SetIP", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY(), 1, EMPTY_PARAMNAME31 }, // 像原来一样使用多个属性来表达IP、NETMASK、GATEWAY
	/*SetNTPServerIP*/ { "SetNTPServerIP", XML_CMD_SETTER,XML_CMD_TYPE_STRING_ENTRY (), 1, "ServerIP", EMPTY_PARAMNAME31},
	/*SetNTPServerUpdateInterval*/ { "SetNTPServerUpdateInterval", XML_CMD_SETTER,XML_CMD_TYPE_INT_ENTRY (), 1, "UpdateInterval", EMPTY_PARAMNAME31},
	/*SetTimeZone*/ { "SetTimeZone", DEFAULT_SETTER },
	/*SetNTPEnable*/ { "SetNTPEnable", DEFAULT_SETTER },
	/*SetTime*/  //上面已经有了{ "SetTime", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 1, "SetTime", EMPTY_PARAMNAME31 }, // 把年、月、日、时、分、秒、毫秒分成7个整形组成字符串传输
	/*ResetDevice*/  //上面已经有了{ "ResetDevice", XML_CMD_SETTER ,XML_CMD_TYPE_INT_ENTRY (), 1, "ResetMode", EMPTY_PARAMNAME31 }, 
	/*RestoreDefaultParam*/  //上面已经有了{ "RestoreDefaultParam", DEFAULT_SETTER },
	/*RestoreFactoryParam*/  //上面已经有了{ "RestoreFactoryParam", XML_CMD_SETTER , XML_CMD_TYPE_NULL_ENTRY (), 0, EMPTY_PARAMNAME32 },
	/*SetOCGate*/ /*SendTriggerOut替换*///{ "SetOCGate", DEFAULT_SETTER },
	/*EnableCOMCheck*//*SetComTestEnable SetComTestDisable替换*/ //{ "EnableCOMCheck", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "COMNum", "Enable",EMPTY_PARAMNAME30 },
	/*SetComTestEnable */{ "SetComTestEnable", DEFAULT_SETTER },
	/*SetComTestDisable */{ "SetComTestDisable", DEFAULT_SETTER },
	/*SetTraceRank */{ "SetTraceRank", DEFAULT_SETTER },
	/*GetTraceRank */{ "GetTraceRank", DEFAULT_GETTER },
	{"HDDFdisk" , DEFAULT_SETTER },
	{"HDDStatus", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1 ,"RetMsg", EMPTY_PARAMNAME31},
	{ "SetCustomizedDevName", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "DevName", EMPTY_PARAMNAME31 },
	{ "GetCustomizedDevName", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "DevName", EMPTY_PARAMNAME31 },

	// 卡口人脸
	{ "GetPCSFlow", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "RetMsg" , EMPTY_PARAMNAME31 },

	// 移动稽查
	{ "CaptureVideo", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },
	{ "OBCLightSwitch", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },

	//相机
	/*GetContrast*/ { "GetContrast", DEFAULT_GETTER  },
	/*GetSaturation*/ { "GetSaturation", DEFAULT_GETTER  },
	/*GetSharpness*/ { "GetSharpness", DEFAULT_GETTER  },
	/*GetDCAperture*/ { "GetDCAperture", DEFAULT_GETTER  },
	/*GetWDREnable*/{ "GetWDREnable", DEFAULT_GETTER  },
	/*GetWDRLevel*/{ "GetWDRLevel", DEFAULT_GETTER  },
	/*GetAWBEnable*///上面已经有了
	/*GetAGCEnable*///上面已经有了
	/*GetDeNoiseLevel*/{ "GetDeNoiseLevel", DEFAULT_GETTER  },


	/*GetSNFEnable*/ { "GetSNFEnable", DEFAULT_GETTER  },
	/*GetEnableGrayImage*/ { "GetEnableGrayImage", DEFAULT_GETTER  },
	/*GetSyncPower*/ { "GetSyncPower", XML_CMD_GETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "Mode", "DelayMS",EMPTY_PARAMNAME30 },
	/*GetDeNoiseMode*/ { "GetDeNoiseMode", DEFAULT_GETTER  },
	/*GetCameraBasicInfo*//*大命令*/ { "GetCameraBasicInfo", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () ,32, "Contrast","Saturation","SharpnessEnable","Sharpness","DCEnable","WDREnable ","WDRLevel","AWBEnable","AGCEnable","ACSyncMode","ACSyncDelay","DeNoiseMode","DeNoiseLevel","AGCLightBaseLine","AGCShutterMin","AGCShutterMax","AGCGainMin","AGCGainMax","GrayImageEnable","ImageEnhancementEnable","GammaEnable","GammaValue","EnRedLightThreshold","DeNoiseTNFEnable", "DeNoiseSNFEnable", "EdgeEnhance","ManualShutter", "ManualGain", "ManualGainR", "ManualGainG", "ManualGainB", "FilterMode" },
	/*GetCameraState*//*大命令*/ { "GetCameraState", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 7, "AGCEnable","Shutter","Gain","AWBEnable","GainR","GainG","GainB", EMPTY_PARAMNAME25 },
	/*SetBrightness*/ { "SetBrightness", DEFAULT_SETTER },
	/*SetContrast*/ { "SetContrast", DEFAULT_SETTER },
	/*SetSaturation*/ { "SetSaturation", DEFAULT_SETTER },
	/*SetSharpness*/ { "SetSharpness", DEFAULT_SETTER },
	/*SetSharpnessEnable*/{ "SetSharpnessEnable", DEFAULT_SETTER },
	/*GetSharpnessEnable*/{ "GetSharpnessEnable", DEFAULT_GETTER },
	/*SetBLCEnable*/ { "SetBLCEnable", DEFAULT_SETTER },
	/*GetBLCEnable*/ { "GetBLCEnable", DEFAULT_GETTER },
	/*SetBLCLevel*/ { "SetBLCLevel", DEFAULT_SETTER },
	/*GetBLCLevel*/ { "GetBLCLevel", DEFAULT_GETTER },
	/*ZoomDCIRIS*/ /*设置DC光圈放大*/{ "ZoomDCIRIS", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
	/*ShrinkDCIRIS是缩小标识*/{ "ShrinkDCIRIS", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32  },
	/*SetDCIRIS*//*使用SetDCAperture替换*/ //{ "SetDCIRIS", DEFAULT_SETTER },
	/*SetFilterMode*/ /*SetCtrlCpl替换*///{ "SetFilterMode", DEFAULT_SETTER },
	/*SetDREEnable*/ { "SetDREEnable", DEFAULT_SETTER },
	/*GetDREEnable*/ { "GetDREEnable", DEFAULT_GETTER },
	/*SetDRELevel*/ { "SetDRELevel", DEFAULT_SETTER },
	/*GetDRELevel*/ { "GetDRELevel", DEFAULT_GETTER },
	/*SetDREMode*/ { "SetDREMode", DEFAULT_SETTER },
	/*GetDREMode*/ { "GetDREMode", DEFAULT_GETTER },
	/*SetWDREnable*/{ "SetWDREnable", DEFAULT_SETTER },
	/*GetWDREnable*/{ "GetWDREnable", DEFAULT_GETTER },
	/*SetWDRLevel*/ { "SetWDRLevel", DEFAULT_SETTER },
	/*GetWDRLevel*/ { "GetWDRLevel", DEFAULT_GETTER },
	/*SetSensorWDR*/ { "SetSensorWDR", DEFAULT_SETTER },
	/*GetSensorWDR*/ { "GetSensorWDR", DEFAULT_GETTER },
	/*SetAWBMode*/ { "SetAWBMode", DEFAULT_SETTER },
	/*GetAWBMode*/ { "GetAWBMode", DEFAULT_GETTER },
	/*SetRgbGain*/  //上面已经有了{ "SetRgbGain", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "GainR", "GainG", "GainB",EMPTY_PARAMNAME29 },
	/*SetAGCEnable*/ //上面已经有了 { "SetAGCEnable", DEFAULT_SETTER },
	/*SetAgcLightBaseLine*///上面已经有了  { "SetAgcLightBaseLine", DEFAULT_SETTER },
	/*SetAGCParam*/  //上面已经有了{ "SetAGCParam", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (4), 4, "ShutterMin", "ShutterMax", "GainMin", "GainMax", EMPTY_PARAMNAME28 },
	/*SetAGCZone*/  //上面已经有了{ "SetAGCZone", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "AGCZone00", "AGCZone01", "AGCZone02", "AGCZone03", "AGCZone04", "AGCZone05", "AGCZone06", "AGCZone07", "AGCZone08", "AGCZone09", "AGCZone10", "AGCZone11", "AGCZone12", "AGCZone13", "AGCZone14", "AGCZone15",EMPTY_PARAMNAME16 },
	/*GetAGCZone*/  //上面已经有了{ "GetAGCZone", XML_CMD_GETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "AGCZone00", "AGCZone01", "AGCZone02", "AGCZone03", "AGCZone04", "AGCZone05", "AGCZone06", "AGCZone07", "AGCZone08", "AGCZone09", "AGCZone10", "AGCZone11", "AGCZone12", "AGCZone13", "AGCZone14", "AGCZone15",EMPTY_PARAMNAME16 },
	/*SetShutter*/ //上面已经有了 { "SetShutter", XML_CMD_SETTER ,XML_CMD_TYPE_INT_ENTRY (), 1, "Shutter", EMPTY_PARAMNAME31 },
	/*SetGain*/  //上面已经有了{ "SetGain", XML_CMD_SETTER ,XML_CMD_TYPE_INT_ENTRY(), 1, "Gain", EMPTY_PARAMNAME31 },
	/*SetEnableGrayImage*/ { "SetEnableGrayImage", DEFAULT_SETTER  },
	/*待YYY协商HVAPI_SetACSync 应同时可设置模式和延时，*//*SetSyncPower*/ { "SetSyncPower", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1,  EMPTY_PARAMNAME31 },
	/*GetLUT*//*8点坐标*//*GetGammaData替换?*/ //{ "GetLUT", XML_CMD_GETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "Point0_X", "Point0_Y", "Point1_X", "Point1_Y", "Point2_X", "Point2_Y", "Point3_X", "Point3_Y", "Point4_X", "Point4_Y", "Point5_X", "Point5_Y", "Point6_X", "Point6_Y", "Point7_X", "Point7_Y", EMPTY_PARAMNAME16 },
	/*SetLUT*//*8点坐标*//*SetGammaData替换?*/ //{ "SetLUT", XML_CMD_SETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "Point0_X", "Point0_Y", "Point1_X", "Point1_Y", "Point2_X", "Point2_Y", "Point3_X", "Point3_Y", "Point4_X", "Point4_Y", "Point5_X", "Point5_Y", "Point6_X", "Point6_Y", "Point7_X", "Point7_Y", EMPTY_PARAMNAME16 },
	/*SetDeNoiseEnable*/ { "SetDeNoiseEnable", DEFAULT_SETTER },
	/*GetDeNoiseEnable*/ { "GetDeNoiseEnable", DEFAULT_GETTER },
	/*SetDeNoiseMode*/ { "SetDeNoiseMode", DEFAULT_SETTER },
	/*GetDeNoiseMode*/ { "GetDeNoiseMode", DEFAULT_GETTER },
	/*SetDeNoiseLevel*/ { "SetDeNoiseLevel", DEFAULT_SETTER },
	/*GetDeNoiseLevel*/ { "GetDeNoiseLevel", DEFAULT_GETTER },
	/*SetDeNoiseSNFEnable*/ { "SetDeNoiseSNFEnable", DEFAULT_SETTER },
	/*SetDeNoiseTNFEnable*/ { "SetDeNoiseTNFEnable", DEFAULT_GETTER },
	/*AutoTestCamera*/ { "AutoTestCamera", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32  },
	/*SetEnRedLight*/{ "SetEnRedLightEnable", DEFAULT_SETTER },
	/*GetEnRedLight*/{ "GetEnRedLightEnable", DEFAULT_GETTER },
	/*SetRedLightRect*/{ "SetRedLightRect", XML_CMD_SETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (32), 32, "Point00_X", "Point00_Y",  "Point01_X", "Point01_Y",  "Point02_X", "Point02_Y",  "Point03_X", "Point03_Y",  "Point04_X", "Point04_Y",  "Point05_X", "Point05_Y",  "Point06_X", "Point06_Y",  "Point07_X", "Point07_Y",  "Point08_X", "Point08_Y",  "Point09_X", "Point09_Y",  "Point10_X", "Point10_Y",  "Point11_X", "Point11_Y",  "Point12_X", "Point12_Y",  "Point13_X", "Point13_Y",  "Point14_X", "Point14_Y",  "Point15_X", "Point15_Y" },
	/*GetRedLightRect*/{ "GetRedLightRect", XML_CMD_GETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (32), 32, "Point00_X", "Point00_Y",  "Point01_X", "Point01_Y",  "Point02_X", "Point02_Y",  "Point03_X", "Point03_Y",  "Point04_X", "Point04_Y",  "Point05_X", "Point05_Y",  "Point06_X", "Point06_Y",  "Point07_X", "Point07_Y",  "Point08_X", "Point08_Y",  "Point09_X", "Point09_Y",  "Point10_X", "Point10_Y",  "Point11_X", "Point11_Y",  "Point12_X", "Point12_Y",  "Point13_X", "Point13_Y",  "Point14_X", "Point14_Y",  "Point15_X", "Point15_Y"},
	/*SetImageEnhancementEnable*/ { "SetImageEnhancementEnable", DEFAULT_SETTER },
	/*GetImageEnhancementEnable*/ { "GetImageEnhancementEnable", DEFAULT_GETTER },
	/*WriteFPGA*/{"WriteFPGA", XML_CMD_SETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "FPGAAddress", "FPGAValue", EMPTY_PARAMNAME30},
	/*ReadFPGA*/{"ReadFPGA", XML_CMD_GETTER , XML_CMD_TYPE_CUSTOM_ENTRY (), 1, "FPGAValue" , EMPTY_PARAMNAME30 },
	/*SetGAMMAValue*/{ "SetGAMMAValue", DEFAULT_SETTER },
	/*GetGAMMAValue*/{ "GetGAMMAValue", DEFAULT_GETTER },
	/*SetGAMMAEnable*/{ "SetGAMMAEnable", DEFAULT_SETTER },
	/*GetGAMMAEnable*/{ "GetGAMMAEnable", DEFAULT_GETTER },
	/*SetEnRedLightThreshold*/{ "SetEnRedLightThreshold", DEFAULT_SETTER },
	/*SetEdgeEnhance*/{ "SetEdgeEnhance", DEFAULT_SETTER },
	/*SetMJPEGRect*/{ "SetMJPEGRect",XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(4), 4, "Point00_X", "Point00_Y",  "Point01_X", "Point01_Y", EMPTY_PARAMNAME28},
	/*SetColorGradation*/{"SetColorGradation", DEFAULT_SETTER},
	/*SetVedioRequestControl*/{"SetVedioRequestControl", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(3), 3, "Enable",   "IP", "Port", EMPTY_PARAMNAME29},
	/*SetAEScene*/{"SetAEScene", DEFAULT_SETTER},
	/*GetAEScene*/{"GetAEScene", DEFAULT_GETTER},

	//视频
	/*GetVideoState*/ { "GetVideoState", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 9, "CVBSDisplayMode","H264BitRate","JpegCompressRate","AutoJpegCompressEnable","AutoJpegCompressEnable", "JpegFileSize", "JpegCompressMaxRate", "JpegCompressMinRate", "DebugJpegStatus", EMPTY_PARAMNAME23 },
	/*SetH264BitRate*/ { "SetH264BitRate", DEFAULT_SETTER },
	/*SetJpegCompressRate*/  //上面已经有了{ "SetJpegCompressRate", DEFAULT_SETTER },
	/*GetDebugJpegStatus*/ { "GetDebugJpegStatus", DEFAULT_GETTER  },
	/*SetDebugJpegStatus*/ { "SetDebugJpegStatus", DEFAULT_SETTER  },
	/*SetCVBSDisplayMode*/ { "SetCVBSDisplayMode", DEFAULT_SETTER  },

	/*SetOSDh264Enable*/  { "SetOSDh264Enable", DEFAULT_SETTER  }, 
	/*SetOSDjpegEnable*/ { "SetOSDjpegEnable", DEFAULT_SETTER  }, 
	/*SetOSDh264PlateEnable*/ { "SetOSDh264PlateEnable", DEFAULT_SETTER  },
	/*SetOSDjpegPlateEnable*/  { "SetOSDjpegPlateEnable", DEFAULT_SETTER  },
	/*SetOSDh264TimeEnable*/ { "SetOSDh264TimeEnable", DEFAULT_SETTER  },
	/*SetOSDjpegTimeEnable*/ { "SetOSDjpegTimeEnable", DEFAULT_SETTER  }, 
	/*SetOSDh264Text*/  {"SetOSDh264Text",XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, EMPTY_PARAMNAME31},
	/*SetOSDjpegText*/   {"SetOSDjpegText",XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, EMPTY_PARAMNAME31}, 
	/*SetOSDh264Font 字体大小*/ { "SetOSDh264Font", DEFAULT_SETTER  }, 
	/*SetOSDjpegFont 字体大小*/{ "SetOSDjpegFont", DEFAULT_SETTER  },  
	/*SetOSDh264FontRGB*/{ "SetOSDh264FontRGB", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "ColoR", "ColorG", "ColorB",EMPTY_PARAMNAME29 },  
	/*SetOSDjpegFontRGB*/{ "SetOSDjpegFontRGB" , XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "ColoR", "ColorG", "ColorB",EMPTY_PARAMNAME29 },	
	/*SetOSDh264Pos*/{ "SetOSDh264Pos", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "PosX", "PosY",EMPTY_PARAMNAME30 },   
	/*SetOSDjpegPos*/ { "SetOSDjpegPos", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "PosX", "PosY",EMPTY_PARAMNAME30 },   
	/*GetH264Caption*/ { "GetH264Caption", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 9, "TimeStampEnable", "OSDEnable", "PosX", "PosY", "FontSize", "ColorR", "ColorG", "ColorB", "Text",  EMPTY_PARAMNAME22 },
	/*GetJPEGCaption*/ { "GetJPEGCaption", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 9, "TimeStampEnable", "OSDEnable", "PosX", "PosY", "FontSize", "ColorR", "ColorG", "ColorB", "Text",  EMPTY_PARAMNAME22 },

	//第二路j264设置
	/*SetOSDh264SecondEnable*/{"SetOSDh264SecondEnable", DEFAULT_SETTER  }, 
	/*SetOSDh264SecondPlateEnable*/{"SetOSDh264SecondPlateEnable", DEFAULT_SETTER  }, 
	/*SetOSDh264SecondTimeEnable*/{"SetOSDh264SecondTimeEnable", DEFAULT_SETTER  }, 
	/*SetOSDh264SecondFont*/{"SetOSDh264SecondFont", DEFAULT_SETTER  }, 

	/*SetOSDh264SecondText*/{"SetOSDh264SecondText", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, EMPTY_PARAMNAME31},

	/*SetOSDh264SecondFontRGB*/{"SetOSDh264SecondFontRGB", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "ColoR", "ColorG", "ColorB",EMPTY_PARAMNAME29 },  
	/*SetOSDh264SecondPos*/{"SetOSDh264SecondPos", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "PosX", "PosY",EMPTY_PARAMNAME30 },   

	/*GetH264SecondCaption*/{"GetH264SecondCaption", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 9, "TimeStampEnable", "OSDEnable", "PosX", "PosY", "FontSize", "ColorR", "ColorG", "ColorB", "Text",  EMPTY_PARAMNAME22 },

	/*SetH264SecondBitRate*/{"SetH264SecondBitRate", DEFAULT_SETTER  }, 
	/*GetH264SecondBitRate*/{"GetH264SecondBitRate", DEFAULT_GETTER  }, 


    /*SetAutoJpegCompressEnable*/{ "SetAutoJpegCompressEnable", DEFAULT_SETTER  }, 
    /*GetAutoJpegCompressEnable*/{ "GetAutoJpegCompressEnable", DEFAULT_GETTER  }, 
	/*SetAutoJpegCompressParam*/{ "SetAutoJpegCompressParam", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "MaxCompressRate", "MinCompressRate", "ImageSize",EMPTY_PARAMNAME29 }, 
	/*GetAutoJpegCompressParam*/{ "GetAutoJpegCompressParam", XML_CMD_GETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "MaxCompressRate", "MinCompressRate", "ImageSize",EMPTY_PARAMNAME29 }, 

	//用户管理
	/*DoLogin*/ { "DoLogin", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, "Authority",  EMPTY_PARAMNAME31 },
	/*GetUsers*/ { "GetUsers", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	/*AddUser*/ { "AddUser", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	/*DelUser*/ { "DelUser", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	/*ModUser*/ { "ModUser", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	
	//IO接口
	/*SetF1IO*/  {"SetF1IO", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "Polarity", "Type", EMPTY_PARAMNAME30},
	/*GetF1IO*/  {"GetF1IO", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2,  "Polarity", "Type", EMPTY_PARAMNAME30},
	/*SetEXPIO*/  {"SetEXPIO", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2,  "Polarity", "Type", EMPTY_PARAMNAME30},
	/*GetEXPIO*/  {"GetEXPIO", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2,  "Polarity", "Type", EMPTY_PARAMNAME30},
	/*SetALMIO*/  {"SetALMIO", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2,  "Polarity", "Type", EMPTY_PARAMNAME30},
	/*GetALMIO*/  {"GetALMIO", XML_CMD_GETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2,  "Polarity", "Type", EMPTY_PARAMNAME30},
	/*SetTGIO*/  {"SetTGIO", DEFAULT_SETTER},
	/*GetTGIO*/  {"GetTGIO", DEFAULT_GETTER},
	/*GetNameList*/	{ "GetNameList", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 2, "WhiteNameList", "BlackNameList" EMPTY_PARAMNAME30 },
	{ "SetCaptureSharpen", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(2), 2, "Sharpen", "Enable", EMPTY_PARAMNAME30 },
	{ "SetExpPluseWidth", XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY(), 1, "PluseWidth", EMPTY_PARAMNAME31 },

	{ "SetJpegOutType", DEFAULT_SETTER },
	{ "GetJpegOutType", DEFAULT_GETTER },
	{ "SetCoilEnable", DEFAULT_SETTER },
	{ "GetCoilEnable", DEFAULT_GETTER },

	{ "SetAWBWorkMode", DEFAULT_SETTER },
    { "GetAWBWorkMode", DEFAULT_GETTER },
	{ "SetAutoControlCammeraAll", DEFAULT_SETTER },
    { "GetAutoControlCammeraAll", DEFAULT_GETTER },
	{ "SetH264Resolution", DEFAULT_SETTER },
    { "GetH264Resolution", DEFAULT_GETTER },

};
static int g_XmlCmdAppendInfoMapItemCount = sizeof(g_XmlCmdAppendInfoMap)/sizeof(g_XmlCmdAppendInfoMap[0]);



HRESULT	HvXmlParse( CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount )
{
	if ( NULL == prgXmlParseInfo || nXmlParseInfoCount <= 0 )
	{
		return E_FAIL;
	}
	HRESULT hr = E_FAIL;
	TiXmlDocument cXmlDoc;
	WrightLogEx("xml-2" , szXml );
	if ( cXmlDoc.Parse(szXml) )
	{
		TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement  )   
		{
			if ( 0 == strcmp( szXmlCmdName , pRootElement->FirstChildElement(HX_CMDNAME)->GetText() ) )
			{
				const TiXmlElement* pCmdElement = pRootElement->FirstChildElement(HX_CMDNAME);
				if ( pCmdElement )
				{
					hr = S_OK;
					for ( INT i = 0 ; i < nXmlParseInfoCount ; ++i  )
					{
						const TiXmlElement* pElement = pCmdElement->FirstChildElement(prgXmlParseInfo[i].szKeyName);
						if (pElement)
						{
							const char* pKeyType = pElement->Attribute(HX_TYPE);
							if ( NULL != pKeyType )
							{
								if ( 0 == strcmp( pKeyType , HX_TYPE_INT  ) )
								{
									prgXmlParseInfo[i].eKeyType = XML_CMD_TYPE_INT;
								}
								else if( 0 == strcmp( pKeyType, HX_TYPE_STRING ))
								{
									prgXmlParseInfo[i].eKeyType = XML_CMD_TYPE_STRING;
								}
								else if( 0 == strcmp( pKeyType , HX_TYPE_BIN ) )
								{
									prgXmlParseInfo[i].eKeyType = XML_CMD_TYPE_BIN;
								}
								else if (0 == strcmp( pKeyType , HX_TYPE_FLOAT ))
								{
									prgXmlParseInfo[i].eKeyType = XML_CMD_TYPE_FLOAT;
								}
								else
								{
									prgXmlParseInfo[i].eKeyType = XML_CMD_TYPE_NULL;
								}
								const char* pKeyValue = pElement->Attribute(HX_VALUE);
								if ( NULL != pKeyValue )
								{
									int nKeyValueLen = 0;
									switch( prgXmlParseInfo[i].eKeyType )
									{
									case XML_CMD_TYPE_INT:
										nKeyValueLen = (int)strlen(pKeyValue);
										if( nKeyValueLen < prgXmlParseInfo[i].nKeyValueLen )
										{

											strcpy( prgXmlParseInfo[i].szKeyValue , pKeyValue );
											prgXmlParseInfo[i].nKeyValueLen = nKeyValueLen;
										}
										break;
									case XML_CMD_TYPE_STRING:
										nKeyValueLen = (int)strlen(pKeyValue);
										if( nKeyValueLen < prgXmlParseInfo[i].nKeyValueLen )
										{
											strcpy( prgXmlParseInfo[i].szKeyValue , pKeyValue );
											prgXmlParseInfo[i].nKeyValueLen = nKeyValueLen;
										}
										break;
									case XML_CMD_TYPE_BIN:
										WrightLogEx( "XML_CMD_TYPE_BIN" , prgXmlParseInfo[i].szKeyName );
										break;
									case XML_CMD_TYPE_NULL:
									default:
										WrightLogEx( "XML_CMD_TYPE_NULL" , prgXmlParseInfo[i].szKeyName );
										break;

									}
								}
							}
						}

					}
				}
				
			}

		}
	}

	return hr;
}


HRESULT	HvXmlParseMercury( const CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount )
{
	if ( NULL == prgXmlParseInfo || nXmlParseInfoCount <= 0 )
	{
		return E_FAIL;
	}
	HRESULT hr = E_FAIL;
	TiXmlDocument cXmlDoc;
	WrightLogEx("xml-2" , szXml );
	if ( cXmlDoc.Parse(szXml) )
	{
		TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement  )   
		{
			if (NULL == pRootElement->FirstChildElement())
			{
				return E_FAIL;
			}

			if ( 0 == hvstricmp( szXmlCmdName , pRootElement->FirstChildElement(HX_CMDNAME)->GetText() ) )
			{
				const TiXmlElement* pCmdElement = pRootElement->FirstChildElement(HX_CMDNAME);
				if ( pCmdElement )
				{
					hr = S_OK;
					for ( INT i = 0 ; i < nXmlParseInfoCount ; ++i  )
					{
						const char* pKeyValue = pCmdElement->Attribute(prgXmlParseInfo[i].szKeyName);

						if ( pKeyValue !=NULL)
						{
							int nKeyValueLen = (int)strlen(pKeyValue);
							if( nKeyValueLen <= prgXmlParseInfo[i].nKeyValueLen )
							{
								strcpy( prgXmlParseInfo[i].szKeyValue , pKeyValue );
								prgXmlParseInfo[i].nKeyValueLen = nKeyValueLen;
							}	
						}
									

					}

				}

			}

		}
	}

	return hr;
}

HRESULT HvGetNonGreedyMatchBetweenBeginLabelAndEndLabel(const CHAR* szBeginLabel,const  CHAR* szEndLabel,const  CHAR* pszRetMsg, INT nRetMsgLen, CHAR* szJieQuReslut, INT nReslutLen )
{
	if ( NULL == szBeginLabel || NULL == szEndLabel || NULL == pszRetMsg || NULL == szJieQuReslut)
	{
	}
	char* pBegin = strstr( pszRetMsg , szBeginLabel );
	if (pBegin!=NULL)
	{
		pBegin+=strlen(szBeginLabel);
	}
	else
	{
		return E_FAIL;
	}
	char* pEnd=strstr(pBegin, szEndLabel);
	if ( NULL != pEnd )
	{
		if (pEnd - pBegin > nReslutLen)
		{
			return E_FAIL;
		}
		memcpy( szJieQuReslut ,pBegin , pEnd-pBegin );
		return S_OK;
	}	
	return E_FAIL;
}



//水星多参数解析如
/*
<CmdName RetCode=0 RetMsg="xx1 "> cmd1   <CmdName>    
<CmdName RetCode=0 RetMsg="xx2 "> cmd2   <CmdName>       
<CmdName RetCode=0 RetMsg="xx3 "> cmd3   <CmdName>   
*/
HRESULT	HvXmlParseMercuryMulti( CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount )
{
	if ( NULL == prgXmlParseInfo || nXmlParseInfoCount <= 0 )
	{
		return E_FAIL;
	}
	HRESULT hr = E_FAIL;
	TiXmlDocument cXmlDoc;
	WrightLogEx("xml-2" , szXml );
	if ( cXmlDoc.Parse(szXml) )
	{
		TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement  )   
		{
			if (NULL == pRootElement->FirstChildElement(HX_CMDNAME)->GetText())
			{
				return E_FAIL;
			}

			if ( 0 == hvstricmp( szXmlCmdName , pRootElement->FirstChildElement(HX_CMDNAME)->GetText() ) )
			{
				const TiXmlElement* pCmdElement = pRootElement->FirstChildElement(HX_CMDNAME);
				if ( pCmdElement )
				{
					hr = S_OK;
					for ( INT i = 0 ; i < nXmlParseInfoCount ; ++i  )
					{
						const char* pKeyValue = pCmdElement->Attribute(prgXmlParseInfo[i].szKeyName);
						int nKeyValueLen = (int)strlen(pKeyValue);
						if( nKeyValueLen < prgXmlParseInfo[i].nKeyValueLen )
						{
							strcpy( prgXmlParseInfo[i].szKeyValue , pKeyValue );
							prgXmlParseInfo[i].nKeyValueLen = nKeyValueLen;
						}										

					}

				}

			}

		}
	}

	return hr;
}

// 搜索g_XmlCmdAppendInfoMap表，返回命令索引，如果找不到命令则返回-1
int SearchXmlCmdMapMercury(const char* pcszCmdName)
{
	int iIndex = 0;
	for ( ; iIndex < g_XmlCmdAppendInfoMapItemCount; iIndex++)
	{
		if (0 == hvstricmp(g_XmlCmdAppendInfoMap[iIndex].szCmdName, pcszCmdName))
		{
			break;
		}
	}
	if (iIndex >= g_XmlCmdAppendInfoMapItemCount)
	{
		iIndex = -1;
	}
	
	return iIndex;
}

bool InitWSA()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int iError;
	wVersionRequested = MAKEWORD(2, 2);
	iError = WSAStartup(wVersionRequested, &wsaData);
	if (iError != 0)
	{
		HV_WriteLog("[HvCamDLL][Warning] WSAStartup Error!");
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) 
	{
		HV_WriteLog("[HvCamDLL][Warning] WSA Version is not 2.2...");
		WSACleanup();
		return false;
	}
	return true;
}

bool UnInitWSA()
{
	bool fRtn = WSACleanup() == 0 ? true : false;
	if (!fRtn)
	{
		int iError = WSAGetLastError();
		char szError[256] = {0};
		sprintf(szError, "[HvCamDLL][Warning] WSACleanup Error Code:%d", iError);
		HV_WriteLog(szError);
	}
	return fRtn;
}

bool ConnectCamera(char* szIp, WORD wPort, SOCKET& hSocket, int iTryTimes, int iReciveTimeOutMS )
{
	if(hSocket != INVALID_SOCKET)
	{
//		CConsole::Write("sock is not invalid, close it.");
		ForceCloseSocket(hSocket);
		hSocket = INVALID_SOCKET;
		Sleep(100);
	}
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
//		CConsole::Write("create socket error.");
		return false;
	}
	//int iReciveTimeOutMS = 20000; // 接收超时设为20s，因为读取参数时可能耗时较长。
	setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iReciveTimeOutMS, sizeof(iReciveTimeOutMS));

	int nReuseAddr = 1;
	setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nReuseAddr, sizeof(int));

	u_long cmd_value = 1;
	ioctlsocket(hSocket, FIONBIO, &cmd_value);
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(wPort);
	addr.sin_addr.S_un.S_addr = inet_addr(szIp);

	do
	{
		if (0 != connect(hSocket, (sockaddr*)&addr, sizeof(addr)))
		{
			struct timeval timeout ;
			fd_set r;
			FD_ZERO(&r);
			FD_SET((hSocket), &r);
			timeout.tv_sec = 0;
			timeout.tv_usec =500000;
			int ret = select(0, 0, &r, 0, &timeout);
			if (ret == 0 || ret == SOCKET_ERROR)
			{
				if(iTryTimes > 0)
				{
//					CConsole::Write("connect error. code=%d, retry...", GetLastError());
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}while(iTryTimes-- > 0);


	if(iTryTimes < 0)
	{
		//cmd_value = 0;
		//ioctlsocket(hSocket, FIONBIO, &cmd_value);

		ForceCloseSocket(hSocket);
		hSocket = INVALID_SOCKET;
//		CConsole::Write("connect %s:%d time out", szIp, wPort);
		return false;
	}
//	CConsole::Write("connect %s:%d success.", szIp, wPort);
	cmd_value = 0;
	ioctlsocket(hSocket, FIONBIO, &cmd_value);
	return true;
}

int ForceCloseSocket(SOCKET &s) 
{
	if ( s == INVALID_SOCKET )
	{
		return 0;
	}

	//struct linger ling = {1, 0};
	//setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));


	shutdown(s, SD_BOTH);
	int iRtn = closesocket(s); 
	s = INVALID_SOCKET; 
	return iRtn;
}

/* HvDevice.cpp */
extern CRITICAL_SECTION g_csWriteLog;

void HV_WriteLog(const char *pszLog)
{
	const int NOT_FIND = -99;
	EnterCriticalSection(&g_csWriteLog);

	//生成LOG文件名
	const int MAXPATH = 260;
	TCHAR szFileName[ MAXPATH ] = {0};
	TCHAR szIniName[MAXPATH] = {0};
	TCHAR szBakName[MAXPATH] = {0};
	GetModuleFileName(NULL, szFileName, MAXPATH);	//取得包括程序名的全路径
	PathRemoveFileSpec(szFileName);				//去掉程序名
	_tcsncpy(szIniName, szFileName, MAXPATH);
	_tcsncpy(szBakName, szFileName, MAXPATH);
	_tcsncat(szFileName, _T("\\HvCamDLL.log"), MAXPATH - 1);
	_tcsncat(szIniName, _T("\\HvCamDLL.ini"), MAXPATH - 1);
	_tcsncat(szBakName, _T("\\HvCamLog.bak"), MAXPATH - 1);

	int iEnable = GetPrivateProfileInt("LogInf", "Enable", NOT_FIND, szIniName);
	if (NOT_FIND == iEnable)
	{
		iEnable = 0;
		char szEnable[10];
		sprintf(szEnable, "%d", iEnable);
		WritePrivateProfileString("LogInf", "Enable", szEnable, szIniName);
	}
	if (iEnable) 
	{
		int iHoldDays = GetPrivateProfileInt("LogInf", "HoldDays", NOT_FIND, szIniName);
		if (iHoldDays <= 0)
		{
			iHoldDays = 1;
			char szHoldDays[10];
			sprintf(szHoldDays, "%d", iHoldDays);
			WritePrivateProfileString("LogInf", "HoldDays", szHoldDays, szIniName);
		}

		//取得当前的精确毫秒的时间
		static time_t starttime = time(NULL);
		static DWORD starttick = GetTickCount(); 
		DWORD dwNowTick = GetTickCount() - starttick;
		time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
		struct tm *pTM = localtime(&nowtime);
		DWORD dwMS = dwNowTick % 1000;

		int iYDay = GetPrivateProfileInt("LogInf", "YDay", NOT_FIND, szIniName);
		if (NOT_FIND == iYDay)
		{
			iYDay = pTM->tm_yday;
			char szYDay[20];
			sprintf(szYDay, "%d", iYDay);
			WritePrivateProfileString("LogInf", "YDay", szYDay, szIniName);
		}

		//确定是否重写文件
		FILE *pfile;
		if ((pTM->tm_yday - iYDay < iHoldDays) && (pTM->tm_yday - iYDay >= 0))
		{
			pfile = _tfopen(szFileName, _T("at"));
		}
		else
		{
			CopyFile(szFileName, szBakName, FALSE);
			pfile = _tfopen(szFileName, _T("wt"));
			char szYDay[20];
			sprintf(szYDay, "%d", pTM->tm_yday);
			WritePrivateProfileString("LogInf", "YDay", szYDay, szIniName);
		}

		//写入LOG信息
		if (pfile != NULL)
		{
			fprintf(pfile, "%04d-%02d-%02d %02d:%02d:%02d:%03d %s\n",
				pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
				pTM->tm_hour, pTM->tm_min, pTM->tm_sec, dwMS,
				pszLog);
			fclose(pfile);
		}
	}
	LeaveCriticalSection(&g_csWriteLog);
}

void HvSafeCloseThread(HANDLE& hThread)
{
	if ( hThread != NULL )
	{
/*
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while (WaitForSingleObject(hThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if (iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(hThread, 0);
		}
		CloseHandle(hThread);
*/
//		CConsole::Write("start HvSafeCloseThread handle=0x%x", (int)hThread);
		MSG msg;
		while(WaitForSingleObject(hThread, 0) != WAIT_OBJECT_0)
		{
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
//		CConsole::Write("end HvSafeCloseThread handle=0x%x", (int)hThread);
		CloseHandle(hThread);
		hThread = NULL;
	}
}

int RecvAll(SOCKET socket, char *pBuffer, int iRecvLen) {
	char *pTemp = pBuffer;
	int iRst = iRecvLen;

	while (iRecvLen > 0) {
		int iTemp = recv(socket, pTemp, iRecvLen, 0);
		if (iTemp == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
				  struct sockaddr_in ss;
				int len;
				len = sizeof(ss);
				getpeername(socket, (sockaddr*)&ss, &len);   //通过socket获取ip地址
				char szIpAddress[16];
				strcpy(szIpAddress, inet_ntoa(ss.sin_addr)); //获取IP地址
				char sz[64];
				sprintf(sz, "RecvAllEOR %d", err);
				WrightLogEx("RecvAllEOR", sz);
			switch (err)
			{
				case WSANOTINITIALISED:
					WrightLogEx(szIpAddress,"A successful WSAStartup call must occur before using this function. ");
					break;
				case WSAENETDOWN:
					WrightLogEx(szIpAddress,"The network subsystem has failed. ");
					break;
				case WSAEACCES:
					WrightLogEx(szIpAddress,"The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address. ");
					break;
				case WSAEINVAL:
					WrightLogEx(szIpAddress,"An unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. ");
					break;
				case WSAEINTR:
					WrightLogEx(szIpAddress,"A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. ");
					break;
				case WSAEINPROGRESS:
					WrightLogEx(szIpAddress,"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ");
					break;
				case WSAEFAULT:
					WrightLogEx(szIpAddress,"The buf or to parameters are not part of the user address space, or the tolen parameter is too small. ");
					break;
				case WSAENETRESET:
					WrightLogEx(szIpAddress,"The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. ");
					break;
				case WSAENOBUFS:
					WrightLogEx(szIpAddress,"No buffer space is available. ");
					break;
				case WSAENOTCONN:
					WrightLogEx(szIpAddress,"The socket is not connected (connection-oriented sockets only). ");
					break;
				case WSAENOTSOCK:
					WrightLogEx(szIpAddress,"The descriptor is not a socket. ");
					break;
				case WSAEOPNOTSUPP:
					WrightLogEx(szIpAddress,"MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. ");
					break;
				case WSAESHUTDOWN:
					WrightLogEx(szIpAddress,"The socket has been shut down; it is not possible to sendto on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. ");
					break;
				case WSAEWOULDBLOCK:
					WrightLogEx(szIpAddress,"The socket is marked as nonblocking and the requested operation would block. ");
					break;
				case WSAEMSGSIZE:
					WrightLogEx(szIpAddress,"The socket is message oriented, and the message is larger than the maximum supported by the underlying transport. ");
					break;
				case WSAEHOSTUNREACH:
					WrightLogEx(szIpAddress,"The remote host cannot be reached from this host at this time. ");
					break;
				case WSAECONNABORTED:
					WrightLogEx(szIpAddress,"The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. ");
					break;
				case WSAECONNRESET:
					WrightLogEx(szIpAddress,"The virtual circuit was reset by the remote side executing a hard or abortive close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable. ");
					break;
				case WSAEADDRNOTAVAIL:
					WrightLogEx(szIpAddress,"The remote address is not a valid address, for example, ADDR_ANY. ");
					break;
				case WSAEAFNOSUPPORT:
					WrightLogEx(szIpAddress,"Addresses in the specified family cannot be used with this socket. ");
					break;
				case WSAEDESTADDRREQ:
					WrightLogEx(szIpAddress,"A destination address is required. ");
					break;
				case WSAENETUNREACH:
					WrightLogEx(szIpAddress,"The network cannot be reached from this host at this time. ");
					break;
				case WSAETIMEDOUT:
					WrightLogEx(szIpAddress,"The connection has been dropped, because of a network failure or because the system on the other end went down without notice. ");
					break;
				default:
					WrightLogEx(szIpAddress,"Unknown socket error. ");
					break;
			}
			err = 0;
			return iTemp;
		}
		if (iTemp == 0)
			break;
		pTemp += iTemp;
		iRecvLen -= iTemp;
	}
	return iRst - iRecvLen;
}

int RecvAll(SOCKET socket, char *pBuffer, int iRecvLen , int& iRealRecvLen) {
	char *pTemp = pBuffer;
	int iRst = iRecvLen;

	int iBlockLen = 10240;


	while (iRecvLen > 0) {
		int iTemp = recv(socket, pTemp, iRecvLen>iBlockLen?iBlockLen:iRecvLen, 0);
		if (iTemp == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			err = 0;
			return iTemp;
		}
		if (iTemp == 0)
			break;
		pTemp += iTemp;
		iRecvLen -= iTemp;
		iRealRecvLen = iRst - iRecvLen;
	}
	return iRst - iRecvLen;
}


static HRESULT XML_New2Old(const char* szXmlBufNew ,char* szXmlBufOld ,int &iBufSize)
{
	if (NULL == szXmlBufNew || NULL == szXmlBufOld)
	{
		return E_FAIL;
	}

	TiXmlDocument cXmlDocNew;
	TiXmlDocument cXmlDocOld;

	if ( cXmlDocNew.Parse(szXmlBufNew) )
	{
		if (strstr(szXmlBufNew, "RetCode"))// Cmd
		{
			TiXmlDeclaration* pResultDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
			TiXmlElement* pResultRootElement = new TiXmlElement("HvCmdRespond");
			if ( NULL == pResultDecl || NULL == pResultRootElement )
			{
				return E_FAIL;
			}
			cXmlDocOld.LinkEndChild(pResultDecl);
			pResultRootElement->SetAttribute("ver", "1.0");

			const TiXmlElement* pRootElement = cXmlDocNew.RootElement();
			const TiXmlElement* pCmdArgElement = pRootElement->FirstChildElement();
			if (pCmdArgElement)
			{
				TiXmlElement* pRetMsgElement = NULL;
				const TiXmlAttribute* pAttr = pCmdArgElement->FirstAttribute();
				while (pAttr)
				{
					if (0 == strcmp(pAttr->Name(), "RetCode"))
					{
						TiXmlElement* pRetCodeElement = new TiXmlElement("RetCode");
						TiXmlText *pXmlCmdText = new TiXmlText(pAttr->Value());
						pRetCodeElement->LinkEndChild(pXmlCmdText);
						pResultRootElement->LinkEndChild(pRetCodeElement);
					}
					else
					{
						if (NULL == pRetMsgElement)
						{
							pRetMsgElement = new TiXmlElement("RetMsg");
						}
						TiXmlElement* pResultElementValue = new TiXmlElement("Value");
						pResultElementValue->SetAttribute("id", pAttr->Name());
						TiXmlText *pXmlCmdText = new TiXmlText(pAttr->Value());
						pResultElementValue->LinkEndChild(pXmlCmdText);
						pRetMsgElement->LinkEndChild(pResultElementValue);
					}
					pAttr = pAttr->Next();
				}
				if (pRetMsgElement)
				{
					pResultRootElement->LinkEndChild(pRetMsgElement);
				}
			}
			cXmlDocOld.LinkEndChild(pResultRootElement);
		}
		else//Info
		{
			TiXmlDeclaration* pResultDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
			TiXmlElement* pResultRootElement = new TiXmlElement("HvInfoRespond");
			if ( NULL == pResultDecl || NULL == pResultRootElement )
			{
				return E_FAIL;
			}
			cXmlDocOld.LinkEndChild(pResultDecl);
			pResultRootElement->SetAttribute("ver", "1.0");

			const TiXmlElement* pRootElement = cXmlDocNew.RootElement();
			const TiXmlElement* pCmdArgElement = pRootElement->FirstChildElement();
			while (pCmdArgElement)
			{
				TiXmlElement* pResultElement = new TiXmlElement("Info");
				pResultElement->SetAttribute("name", pCmdArgElement->GetText());

				const TiXmlAttribute* pAttr = pCmdArgElement->FirstAttribute();
				while (pAttr)
				{
					TiXmlElement* pResultElementValue = new TiXmlElement("Value");
					pResultElementValue->SetAttribute("name", pAttr->Name());
					TiXmlText *pXmlCmdText = new TiXmlText(pAttr->Value());
					pResultElementValue->LinkEndChild(pXmlCmdText);
					pResultElement->LinkEndChild(pResultElementValue);

					pAttr = pAttr->Next();
				}
				pResultRootElement->LinkEndChild(pResultElement);
				pCmdArgElement = pCmdArgElement->NextSiblingElement();
			}

			cXmlDocOld.LinkEndChild(pResultRootElement);
		}

		TiXmlPrinter printer;
		cXmlDocOld.Accept(&printer);
		int iLen = (int)printer.Size();
		if (iLen < iBufSize)
		{
			iBufSize = iLen;
			memcpy(szXmlBufOld, printer.CStr(), iLen);
			szXmlBufOld[iLen] = '\0';
			return S_OK;
		}
	}

	return E_FAIL;
}

bool ExecXmlExtCmd(char* szIP, char* szXmlCmd, char* szRetBuf, int& nBufLen)
{
	if ( strlen(szXmlCmd) > 16*1024 )
	{
		return false;
	}

	const int iBufLen = (1024 << 10);
	char* pbTmpXmlBuf = new char[iBufLen];
	if(pbTmpXmlBuf == NULL)
	{
		return FALSE;
	}
	memset(pbTmpXmlBuf, 0, sizeof(pbTmpXmlBuf));

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if ( ConnectCamera(szIP, wPort, hSocketCmd) )
	{
		cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
		cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE_ARG(pbTmpXmlBuf);
			return false;
		}
		if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize, 0) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE_ARG(pbTmpXmlBuf);
			return false;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
				&& 0 == cCmdRespond.dwResult 
				&& 0 < cCmdRespond.dwInfoSize )
			{
				if ( cCmdRespond.dwInfoSize == RecvAll(hSocketCmd, (char*)pbTmpXmlBuf, cCmdRespond.dwInfoSize) )
				{
					if ( nBufLen > (int)cCmdRespond.dwInfoSize )
					{   
						nBufLen = cCmdRespond.dwInfoSize;
						memcpy(szRetBuf, pbTmpXmlBuf, cCmdRespond.dwInfoSize);
						memset(szRetBuf+cCmdRespond.dwInfoSize, 0, 1);

						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE_ARG(pbTmpXmlBuf);
						return true;
					}
					else
					{
						nBufLen = cCmdRespond.dwInfoSize;
						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE_ARG(pbTmpXmlBuf);
						return false;
					}
				}
				else
				{
					nBufLen = 0;
					ForceCloseSocket(hSocketCmd);
					SAFE_DELETE_ARG(pbTmpXmlBuf);
					return false;
				}
			}
		}
		else
		{
			nBufLen = 0;
			ForceCloseSocket(hSocketCmd);
		}
	}
	SAFE_DELETE_ARG(pbTmpXmlBuf);
	return false;
}

// 生成Xml协议信息请求的Xml数据
static int HvMakeXmlCmdReq(
						   char* szXmlBuf,
						   char* szCmdName,
						   int nArgCount,
						   char** szArgID,
						   char** szArgValue
						   )
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "1.0");

	doc.LinkEndChild(pXmlRootElement);

	TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
	pXmlRootElement->LinkEndChild(pXmlElementCmd);

	TiXmlText *pXmlCmdText = new TiXmlText(szCmdName);
	pXmlElementCmd->LinkEndChild(pXmlCmdText);

	if (nArgCount != 0)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdArg");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		for (int i=0; i<nArgCount; i++)
		{
			TiXmlElement *pXmlElementID = new TiXmlElement("Value");
			pXmlElementCmd->LinkEndChild(pXmlElementID);

			pXmlElementID->SetAttribute("id", szArgID[i]);

			if (szArgValue[i] == NULL)
				continue;

			TiXmlText *pXmlIDText = new TiXmlText(szArgValue[i]);
			pXmlElementID->LinkEndChild(pXmlIDText);
		}
	}


	TiXmlPrinter printer;
	doc.Accept(&printer);

	int len = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), len);
	szXmlBuf[len] = '\0';

	return len;
}

// 生成Xml协议信息请求的Xml数据
static int HvMakeXmlCmdReq2(const XmlCmdList& listXmlCmd, char* szXmlBuf)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "2.0");

	doc.LinkEndChild(pXmlRootElement);

	std::vector<SXmlCmd>::const_iterator iter = listXmlCmd.begin();
	for(; iter!=listXmlCmd.end(); ++iter)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		//命令名称
		TiXmlText *pXmlCmdText = new TiXmlText(iter->strCmdName.c_str());
		pXmlElementCmd->LinkEndChild(pXmlCmdText);

		//参数(属性)
		std::vector<SXmlAttr>::const_iterator iterAttr = iter->listAttr.begin();
		for(; iterAttr!=iter->listAttr.end(); ++iterAttr)
		{
			if (iterAttr->strName.size() > 0 && iterAttr->strValue.size() > 0)
			{
				pXmlElementCmd->SetAttribute(iterAttr->strName.c_str(), iterAttr->strValue.c_str());
			}
		}
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), iLen);
	szXmlBuf[iLen] = '\0';
	
	return iLen;
}

// 生成水星Xml协议信息请求的Xml数据
static int HvMakeXmlCmdReq3(const XmlCmdListMercury& listXmlCmd, char* szXmlBuf)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "3.0");

	doc.LinkEndChild(pXmlRootElement);

	std::vector<SXmlCmdMercury>::const_iterator iter = listXmlCmd.begin();


	bool fXml = true;

	for(; iter!=listXmlCmd.end(); ++iter)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		//命令名称
		TiXmlText *pXmlCmdText = new TiXmlText(iter->strCmdName.c_str());
		pXmlElementCmd->LinkEndChild(pXmlCmdText);

		char szType[128];
		memset(szType, 0, sizeof(szType));
		char szClass[128];
		memset(szClass, 0, sizeof(szClass));

		char szValue[1024];
		memset(szValue, 0, sizeof(szValue));

		switch (iter->emCmdType)
		{
			case XML_CMD_TYPE_NULL:
				sprintf(szType, "NULL");
				sprintf(szValue, "0");
				break;
			case XML_CMD_TYPE_CUSTOM:
				sprintf(szType, "CUSTOM");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					if (iter->listAttr[0].strValue.length() > 1024)
					{
						fXml = false;
						break;
					}

					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					if (
							strcmp("ReadFPGA", iter->strCmdName.c_str()) == 0
						  ||strcmp("DoLogin", iter->strCmdName.c_str()) == 0
						)//TODO(liyh) 临时方案，需在
					{
						strcpy(szValue, iter->listAttr[0].strValue.c_str());
					}
					
				}
				break;
			case XML_CMD_TYPE_INT:
				sprintf(szType, "INT");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					if (iter->listAttr[0].strValue.length() > 1024)
					{
						fXml = false;
						break;
					}

					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_DOUBLE:
				sprintf(szType, "DOUBLE");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					if (iter->listAttr[0].strValue.length() > 1024)
					{
						fXml = false;
						break;
					}

					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_FLOAT:
				sprintf(szType, "FLOAT");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					if (iter->listAttr[0].strValue.length() > 1024)
					{
						fXml = false;
						break;
					}

					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_BOOL:
				sprintf(szType, "BOOL");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_DWORD:
				sprintf(szType, "DWORD");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_STRING:
				sprintf(szType, "STRING");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					if (iter->listAttr[0].strValue.length() > 1024)
					{
						fXml = false;
						break;
					}

					strcpy(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_INTARRAY1D:
				sprintf(szType, "INTARRAY1D,SIZE:%d", iter->iArrayColumn);
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					for (int i = 0; i < iter->iArrayColumn; i++)
					{
						if (iter->listAttr[i].strValue.size() > 0)
						{
							strcat(szValue, iter->listAttr[i].strValue.c_str());
						}
						strcat(szValue, ",");
					}
					if(strlen(szValue) > 0)
					{
						szValue[strlen(szValue) -1] = '\0'; // 去掉最后一个逗号
					}
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
			case XML_CMD_TYPE_INTARRAY2D:
				sprintf(szType, "INTARRAY2D,SIZE:%d*%d", iter->iArrayColumn, iter->iArrayRow);
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					for (int i = 0; i < iter->iArrayRow; i++)
					{
						for (int j = 0; j < iter->iArrayColumn; j++)
						{
							if (iter->listAttr[i * iter->iArrayColumn + j].strValue.size() > 0)
							{
								strcat(szValue, iter->listAttr[i * iter->iArrayColumn + j].strValue.c_str());
							}
							strcat(szValue, ",");
						}
					}
					szValue[strlen(szValue)] = '\0'; // 去掉最后一个逗号
				}
				else
				{
					sprintf(szValue, "0");
				}
				break;
		}

		switch (iter->emCmdClass)
		{
			case XML_CMD_GETTER:
				sprintf(szClass, "GETTER");
				break;
			case XML_CMD_SETTER:
				sprintf(szClass, "SETTER");
				break;
		}
		pXmlElementCmd->SetAttribute("Type", szType);
		pXmlElementCmd->SetAttribute("Class", szClass);
		pXmlElementCmd->SetAttribute("Value", szValue);

	}

	if ( !fXml)
		return 0;


	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), iLen);
	szXmlBuf[iLen] = '\0';
	
	return iLen;
}
HRESULT HvMakeXmlCmdByString2(const char* inXmlOrStrBuf, int nInlen, 
					  char* szOutXmlBuf, int& nOutlen)
{
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(inXmlOrStrBuf))
	{
		memcpy(szOutXmlBuf, inXmlOrStrBuf, nInlen);
		nOutlen = nInlen;
		return S_OK;
	}
	bool bGet = false;

	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	char* pszCmd = pszInBufCopy;
	XmlCmdList listXmlCmd;
	while(pszCmd)
	{
		int iBufLen = (int)strlen(pszCmd);
		char* pTmpCmd = new char[iBufLen + 1];//pszCmd;
		memcpy(pTmpCmd, pszCmd, iBufLen);
		pTmpCmd[iBufLen] = '\0';
		char* pTmpCmdEnd = strstr(pTmpCmd, ";");
		if(pTmpCmdEnd)
		{
			*pTmpCmdEnd = '\0';
		}
		int nCount = 0;
		char* pszCmdName = pTmpCmd;
		char* pValueID[64];
		char* pValueText[64];
		char* pTemp;
		while(pTmpCmd = strchr(pTmpCmd, ','))
		{
			*pTmpCmd = '\0';
			pValueID[nCount] = pTmpCmd + 1;
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2表示必需要有一个字符
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2表示必需要有一个字符
			{
				goto getone;
			}
			pValueText[nCount] = pTemp + 1;
			nCount++;

			*pTemp = '\0';
			*pTmpCmd = '\0';
			pTmpCmd++;
		}

getone:
		bGet = TRUE;
		SXmlCmd  sXmlCmd;
		sXmlCmd.strCmdName = pszCmdName;
		for (int i=0; i<nCount; ++i)
		{
			if (pValueID[i] && pValueText[i])
			{
				SXmlAttr  sXmlAttr;
				sXmlAttr.strName = pValueID[i];
				sXmlAttr.strValue = pValueText[i];
				sXmlCmd.listAttr.push_back(sXmlAttr);
			}
		}
		listXmlCmd.push_back(sXmlCmd);

		pszCmd = strstr(pszCmd, ";");
		if(pszCmd)
		{
			pszCmd += 1;
		}
		SAFE_DELETE_ARG(pszCmdName);
	}

	nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
	SAFE_DELETE_ARG(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}

HRESULT HvMakeXmlCmdByString3(const char* inXmlOrStrBuf, int nInlen, 
					  char* szOutXmlBuf, int& nOutlen)
{
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(inXmlOrStrBuf))
	{ // 不支持直接传入XML，因为水星的XML格式与1.0、2.0协议的XML不一样
		return E_FAIL;
	}

	bool bGet = false;
	char sLog[256];

	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	char szCmdForLog[1024];
	memset(szCmdForLog, 0, sizeof(szCmdForLog));

	char* pszCmd = pszInBufCopy;
	XmlCmdListMercury listXmlCmd;
	while(pszCmd)
	{
		int iBufLen = (int)strlen(pszCmd);
		if (iBufLen <= 0)
		{
			break;
		}
		char* pTmpCmd = new char[iBufLen + 1];//pszCmd;
		char* pTmpCmdBackup = pTmpCmd;
		memcpy(pTmpCmd, pszCmd, iBufLen);
		pTmpCmd[iBufLen] = '\0';
		char* pTmpCmdEnd = strstr(pTmpCmd, ";"); // 命令分隔符
		if(pTmpCmdEnd)
		{
			*pTmpCmdEnd = '\0';
		}
		int nCount = 0;
		char* pszCmdName = pTmpCmd;
		char* pValueID[64];
		char* pValueText[64];
		char* pTemp;
		while(pTmpCmd = strchr(pTmpCmd, ',')) // 参数分隔符
		{
			*pTmpCmd = '\0';
			pValueID[nCount] = pTmpCmd + 1;
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2表示必需要有一个字符
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2表示必需要有一个字符
			{
				goto getone;
			}
			pValueText[nCount] = pTemp + 1;
			nCount++;

			*pTemp = '\0';
			*pTmpCmd = '\0';
			pTmpCmd++;
		}

getone:
		SXmlCmdMercury  sXmlCmd;
		sXmlCmd.strCmdName = pszCmdName;
		char szBuf[256+1];
		memset(szBuf, 0, sizeof(szBuf));
		// 判断传入参数的有效性
		if (strstr("SetTime", pszCmdName))
		{ // TODO:SetTime需要特殊处理
			if(nCount == 2)
			{
				if(strlen(pValueID[0]) + strlen(pValueID[1]) + strlen(pValueText[0]) + strlen(pValueText[1]) < 256)
				{
					CString strTmp;
					strTmp.Format("%s[%s],%s[%s]",pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					//memcpy(szBuf, strTmp.GetBuffer(),256);
					sprintf(szBuf, "%s", strTmp);
					szBuf[256] = 0;
				}
			}
			pValueID[0] = "SetDateTime";
			pValueText[0] = szBuf;
			nCount = 1;
			
		//	goto next;
		}
		if(strstr("SetIP", pszCmdName))
		{
			if(nCount == 4)
			{
				if(strlen(pValueID[0]) + strlen(pValueID[1]) + strlen(pValueID[2]) + strlen(pValueID[3]) +
					strlen(pValueText[0]) + strlen(pValueText[1]) + strlen(pValueText[2]) + strlen(pValueText[3])< 256)
				{
					CString strTmp;
					strTmp.Format("%s[%s],%s[%s],%s[%s],%s[%s]",
						pValueID[0], pValueText[0], pValueID[1], pValueText[1], pValueID[2], pValueText[2], pValueID[3], pValueText[3]);
					memcpy(szBuf, strTmp.GetBuffer(),255);
				}
			}
			pValueID[0] = "SetIP";
			pValueText[0] = szBuf;
			nCount = 1;

		}
		if(strstr("DateTime", pszCmdName))
		{
			printf("DateTime");
		}

		int iFixParamCount = 0;
		if(strstr("SetCaptureRgbGain", pszCmdName) )
		{
			iFixParamCount = -1; //去除使能节点
			if(strcmp(pValueText[3], "0") == 0) //不使能 则将RGB赋值为-1
			{
				pValueText[0] = "-1";
				pValueText[1] = "-1";
				pValueText[2] = "-1";
			}
		}
		if(strstr("SetCaptureGain", pszCmdName))
		{
			iFixParamCount = -1; //去除使能节点
			if(strcmp(pValueText[1], "0") == 0) //不使能 则将RGB赋值为-1
			{
				pValueText[0] = "-1";
			}
		}
		if(strstr("SetCaptureShutter", pszCmdName))
		{
			iFixParamCount = -1; //去除使能节点
			if(strcmp(pValueText[1], "0") == 0) //不使能 则将RGB赋值为-1
			{
				pValueText[0] = "-1";
			}
		}
		if (strstr("SetCaptureSharpen", pszCmdName))
		{
			iFixParamCount = -1; //去除使能节点
			if(strcmp(pValueText[1], "0") == 0) //不使能
			{
				pValueText[0] = "-1";
			}
		}
		if(strstr("SetOSDText", pszCmdName))
		{
			int iAddLenOfForNOTOSDText=128;
			if(nCount == 2)
			{
				if( strlen(pValueID[0]) + strlen(pValueID[1]) +
					strlen(pValueText[0]) + strlen(pValueText[1]) < 256+iAddLenOfForNOTOSDText)
				{
					CString strTmp;
					strTmp.Format("%s=[%s],%s=[%s]",
						pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf, strTmp.GetBuffer(),255+iAddLenOfForNOTOSDText);
				}
			}
			pValueID[0] = "SetOSDText";
			pValueText[0] = szBuf;
			nCount = 1;
		}
		if (strstr("SetSyncPower", pszCmdName))
		{
			if(nCount == 2)
			{
				if( strlen(pValueID[0]) + strlen(pValueID[1]) +
					strlen(pValueText[0]) + strlen(pValueText[1]) < 256)
				{
					CString strTmp;
					strTmp.Format("%s=[%s],%s=[%s]",
						pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf, strTmp.GetBuffer(),255);
				}
			}
			pValueID[0] = "SetSyncPower";
			pValueText[0] = szBuf;
			nCount = 1;
		}
		if(strstr("DoLogin", pszCmdName))
		{
			if(nCount == 2)
			{
				if( strlen(pValueID[0]) + strlen(pValueID[1]) +
					strlen(pValueText[0]) + strlen(pValueText[1]) < 256)
				{
					CString strTmp;
					strTmp.Format("%s=[%s],%s=[%s]",
						pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf, strTmp.GetBuffer(),255);
				}
			}
			pValueID[0] = "DoLogin";
			pValueText[0] = szBuf;
			nCount = 1;

		}
		//AddUser,UserName[%s],Password[%s],AddUserName[%s],AddUserPassword[%s],AddUserAuthority[%d]
		if(strstr("AddUser", pszCmdName))
		{
			if(nCount == 5)
			{
				if( strlen(pValueID[0]) + strlen(pValueID[1]) +strlen(pValueID[2]) + strlen(pValueID[3])+strlen(pValueID[4]) +
					strlen(pValueText[0]) + strlen(pValueText[1]) + strlen(pValueText[2])+ strlen(pValueText[3]) + strlen(pValueText[4])< 256)
				{
					CString strTmp;
					strTmp.Format("%s=[%s],%s=[%s],%s=[%s],%s=[%s],%s=[%s]",
						pValueID[0], pValueText[0], 
						pValueID[1], pValueText[1], 
						pValueID[2], pValueText[2], 
						pValueID[3], pValueText[3], 
						pValueID[4], pValueText[4]);
					memcpy(szBuf, strTmp.GetBuffer(),255);
				}
			}
			pValueID[0] = "AddUser";
			pValueText[0] = szBuf;
			nCount = 1;

		}

		//DelUser,UserName[%s],Password[%s],DelUserName[%s]
		if(strstr("DelUser", pszCmdName))
		{
			if(nCount == 3)
			{
				if( strlen(pValueID[0]) + strlen(pValueID[1]) + strlen(pValueID[2]) +
					strlen(pValueText[0]) + strlen(pValueText[1])+ strlen(pValueText[2]) < 256)
				{
					CString strTmp;
					strTmp.Format("%s=[%s],%s=[%s],%s=[%s]",
						pValueID[0], pValueText[0], pValueID[1], pValueText[1], pValueID[2], pValueText[2]);
					memcpy(szBuf, strTmp.GetBuffer(),255);
				}
			}
			pValueID[0] = "DelUser";
			pValueText[0] = szBuf;
			nCount = 1;

		}
		//ModUser,UserName[%s],Password[%s],ModUserName[%s],ModUserPassword[%s],ModUserAuthority[%d]
		if(strstr("ModUser", pszCmdName))
		{
			if(nCount == 5)
			{
				if( strlen(pValueID[0]) + strlen(pValueID[1]) +strlen(pValueID[2]) + strlen(pValueID[3])+strlen(pValueID[4]) +
					strlen(pValueText[0]) + strlen(pValueText[1]) + strlen(pValueText[2])+ strlen(pValueText[3]) + strlen(pValueText[4])< 256)
				{
					CString strTmp;
					strTmp.Format("%s=[%s],%s=[%s],%s=[%s],%s=[%s],%s=[%s]",
						pValueID[0], pValueText[0], 
						pValueID[1], pValueText[1], 
						pValueID[2], pValueText[2], 
						pValueID[3], pValueText[3], 
						pValueID[4], pValueText[4]);
					memcpy(szBuf, strTmp.GetBuffer(),255);
				}
			}
			pValueID[0] = "ModUser";
			pValueText[0] = szBuf;
			nCount = 1;

		}


		
		if ( strstr("SetCusTomInfo", pszCmdName ) )
		{
			int iValueLen = 0;
			if ( nCount == 0 )
			{
				pValueID[0] = "Value";
				pValueText[0] = "";
				nCount = 1;
			}

			if ( strlen(pValueText[0]) > 1024 )
			{
				SAFE_DELETE_ARG(pTmpCmdBackup);
				goto cleanup;
			}



		}
			

		int iCmdIndex = SearchXmlCmdMapMercury(pszCmdName);
		if (-1 == iCmdIndex)
		{ // 没找到对应的命令
			//bGet = false;
			sprintf(sLog, "HvMakeXmlCmdByString3 can't find command [%s]", pszCmdName);
			WrightLogEx("==testIP==", sLog);
			//SAFE_DELETE_ARG(pTmpCmdBackup);
			//goto cleanup;
		}
		else
		{
			if (XML_CMD_SETTER == g_XmlCmdAppendInfoMap[iCmdIndex].emCmdClass)
			{ // GETTER不需要组织输入参数
				if (0 == strcmp(pszCmdName, "ResetDevice") && nCount < g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum)
				{ // ResetDevice有默认参数值，需要特殊处理
					SXmlAttr  sXmlAttr;
					sXmlAttr.strName = "ResetMode";
					sXmlAttr.strValue = "-1";
					sXmlCmd.listAttr.push_back(sXmlAttr);
				}
				else
				{
					if (nCount < g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum)
					{ // 参数数量不对
						bGet = false;
						sprintf(sLog, "HvMakeXmlCmdByString3 Command [%s] doesn't have enough parameters", pszCmdName);
						WrightLogEx("testIP", sLog);
						SAFE_DELETE_ARG(pTmpCmdBackup);
						goto cleanup;
					}
					else
					{
						for (int i=0; i<g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum + iFixParamCount; ++i) // 多余的参数忽略掉
						{
							if (pValueID[i] && pValueText[i])
							{
								SXmlAttr  sXmlAttr;
								sXmlAttr.strName = pValueID[i];
								sXmlAttr.strValue = pValueText[i];
								sXmlCmd.listAttr.push_back(sXmlAttr);
							}
						}
					}
				}
			}
			else if (XML_CMD_GETTER == g_XmlCmdAppendInfoMap[iCmdIndex].emCmdClass)
			{
				if (
 					0 == strcmp("ReadFPGA"  ,g_XmlCmdAppendInfoMap[iCmdIndex].szCmdName)
					||0 == strcmp("DoLogin"  ,g_XmlCmdAppendInfoMap[iCmdIndex].szCmdName)
					)
				{
					SXmlAttr  sXmlAttr;
					if (pValueID[0] && pValueText[0])
					{
						 
						sXmlAttr.strName = pValueID[0];
						sXmlAttr.strValue = pValueText[0];
						
					}
					else
					{
						
						sXmlAttr.strName = "value";
						sXmlAttr.strValue = "-1";
					}
					sXmlCmd.listAttr.push_back(sXmlAttr);

				}
			}
 

			sXmlCmd.emCmdClass = g_XmlCmdAppendInfoMap[iCmdIndex].emCmdClass;
			sXmlCmd.emCmdType = g_XmlCmdAppendInfoMap[iCmdIndex].emCmdType;
			sXmlCmd.iArrayColumn = g_XmlCmdAppendInfoMap[iCmdIndex].iArrayColumn + iFixParamCount;
			sXmlCmd.iArrayRow = g_XmlCmdAppendInfoMap[iCmdIndex].iArrayRow;
			listXmlCmd.push_back(sXmlCmd);
			sprintf(szCmdForLog, "%s %s", szCmdForLog, sXmlCmd.strCmdName.c_str());
			bGet = true;
		}

next:
		pszCmd = strstr(pszCmd, ";");
		if(pszCmd)
		{
			pszCmd += 1;
		}
		
		SAFE_DELETE_ARG(pTmpCmdBackup);
	}

	char szLog[1024];
	sprintf(szLog, "HvMakeXmlCmdByString3 got commands[%s] to execute", szCmdForLog);
	WrightLogEx("Test IP", szLog);

	nOutlen = HvMakeXmlCmdReq3(listXmlCmd, szOutXmlBuf);

	
cleanup:
	SAFE_DELETE_ARG(pszInBufCopy);
	if ( nOutlen == 0 )
		return E_FAIL;

	return (bGet) ? S_OK : E_FAIL;
}

HRESULT HvMakeXmlCmdByString(PROTOCOL_VERSION emProtocolVersion, const char* inXmlOrStrBuf, int nInlen, 
							 char* szOutXmlBuf, int& nOutlen)
{
	if(PROTOCOL_VERSION_2 == emProtocolVersion || PROTOCOL_VERSION_MERCURY == emProtocolVersion )
	{
		return HvMakeXmlCmdByString2(inXmlOrStrBuf, nInlen, szOutXmlBuf, nOutlen);
	}

	TiXmlDocument cXmlDoc;

	if ( cXmlDoc.Parse(inXmlOrStrBuf) )
	{
		memcpy(szOutXmlBuf, inXmlOrStrBuf, nInlen);
		nOutlen = nInlen;
		return S_OK;
	}

	bool bGet = false;
	// 为创建Xml准备参数空间
	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	int nCount = 0;
	char* pCmdName = pszInBufCopy;
	// 假定命令的参数个数不超过64个。
	char* pValueID[64];
	char* pValueText[64];
	char* pTemp;
	// 判断及解析Xml类型字符串
	while( pszInBufCopy = strchr(pszInBufCopy, ',') )
	{
		*pszInBufCopy = '\0';
		pValueID[nCount] = pszInBufCopy + 1;
		if ( ((pTemp = strchr(pszInBufCopy+2, '[')) == NULL)     // +2表示必需要有一个字符
			|| ((pszInBufCopy = strchr(pTemp+2, ']')) == NULL) ) // +2表示必需要有一个字符
		{
			goto done;
		}
		pValueText[nCount] = pTemp + 1;
		nCount++;

		*pTemp = '\0';
		*pszInBufCopy = '\0';
		pszInBufCopy++;
	}
	bGet = true;
done:
	if (bGet)
	{
		if (PROTOCOL_VERSION_1 == emProtocolVersion)
		{
			nOutlen = HvMakeXmlCmdReq(szOutXmlBuf, pCmdName, nCount, pValueID, pValueText);
		}
		else
		{
			XmlCmdList listXmlCmd;
			SXmlCmd  sXmlCmd;
			sXmlCmd.strCmdName = pCmdName;
			for (int i=0; i<nCount; ++i)
			{
				if (pValueID[i] && pValueText[i])
				{
					SXmlAttr  sXmlAttr;
					sXmlAttr.strName = pValueID[i];
					sXmlAttr.strValue = pValueText[i];
					sXmlCmd.listAttr.push_back(sXmlAttr);
				}
			}
			listXmlCmd.push_back(sXmlCmd);
			nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
		}
	}

	SAFE_DELETE_ARG(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}


HRESULT HvMakeXmlCmdByString1(const char* inXmlOrStrBuf, int nInlen, 
							 char* szOutXmlBuf, int& nOutlen)
{
	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(inXmlOrStrBuf) )
	{
		memcpy(szOutXmlBuf, inXmlOrStrBuf, nInlen);
		nOutlen = nInlen;
		return S_OK;
	}

	bool bGet = false;
	// 为创建Xml准备参数空间
	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	int nCount = 0;
	char* pCmdName = pszInBufCopy;
	// 假定命令的参数个数不超过64个。
	char* pValueID[64];
	char* pValueText[64];
	char* pTemp;
	// 判断及解析Xml类型字符串
	while( pszInBufCopy = strchr(pszInBufCopy, ',') )
	{
		*pszInBufCopy = '\0';
		pValueID[nCount] = pszInBufCopy + 1;
		if ( ((pTemp = strchr(pszInBufCopy+2, '[')) == NULL)     // +2表示必需要有一个字符
			|| ((pszInBufCopy = strchr(pTemp+2, ']')) == NULL) ) // +2表示必需要有一个字符
		{
			goto done;
		}
		pValueText[nCount] = pTemp + 1;
		nCount++;

		*pTemp = '\0';
		*pszInBufCopy = '\0';
		pszInBufCopy++;
	}
	bGet = true;
done:
	if (bGet)
	{
		nOutlen = HvMakeXmlCmdReq(szOutXmlBuf, pCmdName, nCount, pValueID, pValueText);
	}

	SAFE_DELETE_ARG(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}

// 生成Xml协议信息请求的Xml数据
static int HvMakeXmlInfoReq(
							char* szXmlBuf,
							int nInfoCount,
							char** szInfoName,
							char** szInfoText
							)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvInfo");
	pXmlRootElement->SetAttribute("ver", "1.0");
	doc.LinkEndChild(pXmlRootElement);

	for (int i=0; i<nInfoCount; i++)
	{
		TiXmlElement *pXmlElementInfo = new TiXmlElement("Info");
		pXmlRootElement->LinkEndChild(pXmlElementInfo);

		pXmlElementInfo->SetAttribute("name", szInfoName[i]);

		if ((szInfoText == NULL) || (szInfoText[i] == NULL))
			continue;

		TiXmlText *pXmlIDText = new TiXmlText(szInfoText[i]);
		pXmlElementInfo->LinkEndChild(pXmlIDText);
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int len = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), len);
	szXmlBuf[len] = '\0';

	return len;
}

HRESULT HvMakeXmlInfoByString(PROTOCOL_VERSION emProtocolVersion,
							  const char* inXmlOrStrBuf, 
							  int nInlen, 
							  char* szOutXmlBuf, 
							  int& nOutlen
							  )
{
	bool bGet = false;
	// 为创建Xml准备参数空间
	char* pszInBufCopy = new char[nInlen+1];
	char* pszInBufCopyOld = pszInBufCopy;
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	int nCount = 1;
	char* pInfoName[64] = { pszInBufCopy };  // 假定信息的类型不超过64种。

	// 判断及解析Xml类型字符串
	while( pszInBufCopy = strchr(pszInBufCopy, ',') )
	{
		*pszInBufCopy = '\0';
		pszInBufCopy++;
		if (*pszInBufCopy == '\0')
			break;
		pInfoName[nCount] = pszInBufCopy;
		nCount++;
	}
	
	if (PROTOCOL_VERSION_1 == emProtocolVersion)
	{
		nOutlen = HvMakeXmlInfoReq(szOutXmlBuf, nCount, pInfoName, NULL);
	}
	else if( PROTOCOL_VERSION_2 == emProtocolVersion )
	{
		XmlCmdList listXmlCmd;
		for (int i=0; i<nCount; ++i)
		{
			if (pInfoName[i])
			{
				SXmlCmd  sXmlCmd;
				sXmlCmd.strCmdName = pInfoName[i];
				listXmlCmd.push_back(sXmlCmd);
			}
		}	
		nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
	}
	else if(PROTOCOL_VERSION_EARTH == emProtocolVersion)
	{
		HvMakeXmlCmdByString2(inXmlOrStrBuf, nInlen, szOutXmlBuf, nOutlen);
	}
	else// 默认水星
	{
		HvMakeXmlCmdByString3(inXmlOrStrBuf, nInlen, szOutXmlBuf, nOutlen);
	}
	
	delete[] pszInBufCopyOld;
	return S_OK;
}

HRESULT HvParseXmlCmdRespRetcode(char* szXmlBuf, char* szCommand)
{
	HRESULT rst = E_FAIL;

	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szXmlBuf) )
	{
		const char* pszTextGet = NULL;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if ( pCurElement == NULL ) goto done;

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if (pAttrib==NULL) 
		{
			goto done;
		}
		if (0 == strcmp(pAttrib->Value(), "1.0"))   // 默认认为第一个属性就是ver
		{	// 查找返回值
			pCurElement = pCurElement->FirstChildElement("RetCode");
			assert(pCurElement != NULL);
			pszTextGet = pCurElement->GetText();
		}
		else if (0 == strcmp(pAttrib->Value(), "2.0"))   // 默认认为第一个属性就是ver
		{
			for (;pCurElement; pCurElement = pCurElement->NextSiblingElement())
			{
				if (pCurElement->Attribute("RetCode") && pCurElement->GetText() && !stricmp(pCurElement->GetText(), szCommand))
				{
					pszTextGet = pCurElement->Attribute("RetCode");
				}
			}
		}
		if(NULL != pszTextGet && !atoi(pszTextGet))
		{
			rst = S_OK;
		}
	}
done:
	return rst;
}

HRESULT HvParseXmlCmdRespRetcode2(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText)
{
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(szXmlBuf))
	{
		const char* pszTextGet = NULL;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if(pCurElement == NULL)
		{
			goto done;
		}

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if(pAttrib == NULL)
		{
			goto done;
		}

		if(strcmp(pAttrib->Value(), "2.0") != 0)
		{
			goto done;
		}

		pCurElement = pCurElement->FirstChildElement();
		while(pCurElement)
		{
			if(strcmp(pCurElement->GetText(), szCmdName) == 0)
			{
				pszTextGet = pCurElement->Attribute(szCmdValueName);
				if(pszTextGet)
				{
					memcpy(szCmdValueText, pszTextGet, strlen(pszTextGet) + 1);
					return S_OK;
				}
				else
				{
					goto done;
				}

			}
			pCurElement = pCurElement->NextSiblingElement();
		}

	}

done:
	return rst; 
}

HRESULT HvParseXmlCmdRespRetcode2Adv(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText, int* pnCmdValueTextLen)
{
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(szXmlBuf))
	{
		const char* pszTextGet = NULL;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if(pCurElement == NULL)
		{
			goto done;
		}

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if(pAttrib == NULL)
		{
			goto done;
		}

		if(strcmp(pAttrib->Value(), "2.0") != 0)
		{
			goto done;
		}

		pCurElement = pCurElement->FirstChildElement();
		while(pCurElement)
		{
			if(strcmp(pCurElement->GetText(), szCmdName) == 0)
			{
				pszTextGet = pCurElement->Attribute(szCmdValueName);
				if(pszTextGet)
				{
					int nTextLen = (int)strlen(pszTextGet)+1;
					if ( *pnCmdValueTextLen  < nTextLen)
					{
						goto done;
					}
					memcpy(szCmdValueText, pszTextGet, nTextLen);
					*pnCmdValueTextLen = nTextLen;
					return S_OK;
				}
				else
				{
					goto done;
				}

			}
			pCurElement = pCurElement->NextSiblingElement();
		}

	}

done:
	return rst; 
}

HRESULT HvParseXmlCmdRespRetcode3(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText)
{
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(szXmlBuf))
	{
		const char* pszTextGet = NULL;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if(pCurElement == NULL)
		{
			goto done;
		}

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if(pAttrib == NULL)
		{
			goto done;
		}

		if(strcmp(pAttrib->Value(), "2.0") != 0)
		{
			goto done;
		}

		pCurElement = pCurElement->FirstChildElement();
		while(pCurElement)
		{
			if(strcmp(pCurElement->GetText(), szCmdName) == 0)
			{
				pszTextGet = pCurElement->Attribute(szCmdValueName);
				if(pszTextGet)
				{
					memcpy(szCmdValueText, pszTextGet, strlen(pszTextGet) + 1);
					return S_OK;
				}
				else
				{
					goto done;
				}

			}
			pCurElement = pCurElement->NextSiblingElement();
		}

	}

done:
	return rst; 
}


HRESULT ParseConnectIP(const CHAR* szIn, const CHAR* szPre, CHAR* szOut, INT *pnszOut)
{

	const CHAR* pszTempConnetIP=szIn;
	CHAR szIPList[512]={0};
	CHAR* sub;
	INT nCount = 0;
	CHAR* left=0;
	CHAR* right=0;
	while(
		pszTempConnetIP != '\0'
		&& (left=strchr(pszTempConnetIP,'(')) != NULL 
		&& (right=strchr(pszTempConnetIP, ')')) != NULL
		)
	{
		CHAR szTemp[128];
		INT iLen=right-left;
		memcpy(szTemp, left+1, right-left-1);
		szTemp[right-left-1]='\0';
		if( sub = strchr(szTemp, ',') )
		{
			*sub=':';
		}
		strcat(szTemp, ",\n");

		strcat(szIPList, szTemp);
		nCount++;
		pszTempConnetIP=right+1;

	}
	
	CHAR szFormatInfo[1024];
	sprintf(szFormatInfo, "%s有%d个连接\n%s", szPre, nCount,szIPList );
	if( strlen(szFormatInfo) > *pnszOut)
	{
		return E_FAIL;
	}
	strcpy(szOut, szFormatInfo);
	*pnszOut = strlen(szOut);
	return S_OK;

}

HRESULT HvParseXmlCmdRespMsg(char* szXmlBuf, char* szMsgID, char* szMsgOut)
{
	HRESULT rst = E_FAIL;

	TiXmlDocument cXmlDoc;

	if ( cXmlDoc.Parse(szXmlBuf) )
	{
		const char* szTextGet;

		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if ( pCurElement == NULL ) goto done;

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if ( (pAttrib==NULL) 
			|| (0 != strcmp(pAttrib->Value(), "1.0")) )   // 默认认为第一个属性就是ver
			goto done;

		// 查找返回信息
		pCurElement = pCurElement->FirstChildElement("RetMsg");
		// 如果没有返回信息，返回
		if ( pCurElement == NULL ) goto done;

		// 查找Value字段
		for ( pCurElement = pCurElement->FirstChildElement("Value");
			pCurElement != NULL; 
			pCurElement = pCurElement->NextSiblingElement("Value") )
		{
			// 默认认为第一个属性就是id
			pAttrib = pCurElement->FirstAttribute();
			szTextGet = pAttrib->Value();

			if(_strnicmp(szMsgID, szTextGet, strlen(szTextGet)+1) == 0)
			{
				szTextGet = pCurElement->GetText();
				if ( szTextGet )
				{
					memcpy(szMsgOut, szTextGet, strlen(szTextGet)+1);
				}
				else
				{
					strcpy(szMsgOut, "NULL");
				}

				rst = S_OK;
				break;
			}
		}
	}

done:
	return rst;
}

HRESULT HvParseXmlInfoRespValue(
								char* szXmlBuf,
								char* szInfoName, 
								char* nInfoValueName,
								char* szInfoValueText
								)
{
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;

	if ( cXmlDoc.Parse(szXmlBuf) )
	{
		const char* szTextGet;

		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvInfoRespond");
		if ( pCurElement == NULL )   goto done;

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if ( (pAttrib==NULL) 
			|| (0 != strcmp(pAttrib->Value(), "1.0")) )   // 默认认为第一个属性就是ver
			goto done;

		// 查找Info字段
		for ( pCurElement = pCurElement->FirstChildElement("Info");
			pCurElement != NULL; 
			pCurElement = pCurElement->NextSiblingElement("Info") )
		{

			// 默认认为第一个属性就是name
			pAttrib = pCurElement->FirstAttribute();
			szTextGet = pAttrib->Value();

			if(_strnicmp(szInfoName, szTextGet, strlen(szTextGet)+1) == 0)
			{
				// 查找Value字段  
				for ( TiXmlElement* pCurValueElement = pCurElement->FirstChildElement("Value");
					pCurValueElement != NULL; 
					pCurValueElement = pCurValueElement->NextSiblingElement("Value") )
				{
					// 默认认为第一个属性就是name
					pAttrib = pCurValueElement->FirstAttribute();
					szTextGet = pAttrib->Value();
					if(_strnicmp(nInfoValueName, szTextGet, strlen(szTextGet)+1) == 0)
					{
						szTextGet = pCurValueElement->GetText();
						if(szTextGet == NULL)
						{
							goto done;
						}
						memcpy(szInfoValueText, szTextGet, strlen(szTextGet)+1);
						rst = S_OK;
						break;
					}
				}
			}
		}
	}

done:
	return rst;
}

// ---------------------------------------------------------------------

static inline int SetInt32ToByteArray(BYTE *pbDest, INT32 i32Src)
{
	memcpy(pbDest, &i32Src, sizeof(i32Src));
	return sizeof(i32Src);
}

static inline int SetInt16ToByteArray(BYTE *pbDest, INT16 i16Src)
{
	memcpy(pbDest, &i16Src, sizeof(i16Src));
	return sizeof(i16Src);
}

// 描述:	转换YUV数据到RGB
// 参数:	pbDest			输出RGB数据的缓冲区指针;
//			pbSrc			输入YUV数据的缓冲区指针;
//			iSrcWidth		图像宽度;
//			iSrcHeight		图像高度;
//			iBGRStride		RGB数据每行的步长;
// 返回值:  返回S_OK, 表示操作成功,
//          返回E_POINTER, 参数中包含有非法的指针;
//          返回E_FAIL, 表示未知的错误导致操作失败;
static HRESULT Yuv2Rgb(
					   BYTE *pbDest,
					   BYTE *pbSrc,
					   int iSrcWidth,
					   int iSrcHeight,
					   int iBGRStride
					   )
{
	bool fBottomUp = true;
	if (iSrcHeight < 0)
	{
		iSrcHeight = -iSrcHeight;
		fBottomUp = false;
	}
	int x, y;
	unsigned char *pY = reinterpret_cast< unsigned char* >(pbSrc);
	unsigned char *pCb = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight;
	unsigned char *pCr = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight + (iSrcWidth >> 1) * iSrcHeight;
	for (y = 0; y < iSrcHeight; y++)
		for (x = 0; x < iSrcWidth; x++)
		{
			int iY = *(pY + y * iSrcWidth + x);
			int iCb = *(pCb + y * (iSrcWidth >> 1) + (x >> 1));
			int iCr = *(pCr + y * (iSrcWidth >> 1) + (x >> 1));
			int iR = static_cast< int >(1.402 * (iCr - 128) + iY);
			int iG = static_cast< int >(-0.34414 * (iCb - 128) - 0.71414 * (iCr - 128) + iY);
			int iB = static_cast< int >(1.772 * (iCb - 128) + iY);
			if (iR > 255)
				iR = 255;
			if (iR < 0)
				iR = 0;
			if (iG > 255)
				iG = 255;
			if (iG < 0)
				iG = 0;
			if (iB > 255)
				iB = 255;
			if (iB < 0)
				iB = 0;
			if (fBottomUp)
			{
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 ] = iB;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 2 ] = iR;
			}
			else
			{
				pbDest[ y * iBGRStride + x * 3 ] = iB;
				pbDest[ y * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ y * iBGRStride + x * 3 + 2 ] = iR;
			}
		}
		return S_OK;
}

HRESULT Yuv2BMP(
				BYTE *pbDest,
				int iDestBufLen,
				int *piDestLen,
				BYTE *pbSrc,
				int iSrcWidth,
				int iSrcHeight
				)
{
	DWORD32 dwOffSet = 54;
	DWORD32 dwRBGStride = ((iSrcWidth * 24 + 31) >> 5) << 2;
	DWORD32 dwRgbSize = dwRBGStride * iSrcHeight;
	*piDestLen = dwOffSet + dwRgbSize;
	if (*piDestLen > iDestBufLen)
	{
		*piDestLen = 0;
		return E_FAIL;
	}

	*(pbDest++) = 'B';										//BMP文件标志.
	*(pbDest++) = 'M';
	pbDest += SetInt32ToByteArray(pbDest, *piDestLen);		//BMP文件大小.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//reserved
	pbDest += SetInt32ToByteArray(pbDest, dwOffSet);		//文件头偏移量.
	pbDest += SetInt32ToByteArray(pbDest, 0x28);			//biSize.
	pbDest += SetInt32ToByteArray(pbDest, iSrcWidth);		//biWidth.
	pbDest += SetInt32ToByteArray(pbDest, iSrcHeight);		//biHeight.
	pbDest += SetInt16ToByteArray(pbDest, 1);				//biPlanes.
	pbDest += SetInt16ToByteArray(pbDest, 24);				//biBitCount.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biCompression.
	pbDest += SetInt32ToByteArray(pbDest, dwRgbSize);		//biSizeImage.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biXPerMeter.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biYPerMeter.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biClrUsed.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biClrImportant.
	Yuv2Rgb(pbDest, pbSrc, iSrcWidth, iSrcHeight, dwRBGStride);
	return S_OK;
}

// ---------------------------------------------------------------------

const int BIN_BIT_COUNT = 8;
const int BIN_WIDTH = 112;
const int BIN_HEIGHT = 20;
const int BIN_STRIDE = BIN_WIDTH / BIN_BIT_COUNT;
const int BIN_BYTE_COUNT = BIN_HEIGHT * BIN_STRIDE;

static void RgbBin2ByteBin(PBYTE m_rgbBinImage, PBYTE pbByteBinImage)
{
	for (int y = 0; y < BIN_HEIGHT; y++)
	{
		BYTE *pSrc = &m_rgbBinImage[(BIN_HEIGHT - 1) * BIN_STRIDE];
		BYTE *pDest = pbByteBinImage;
		for (int y = 0; y < BIN_HEIGHT; y++)
		{
			memcpy(pDest, pSrc, BIN_STRIDE);
			pSrc -= BIN_STRIDE;
			pDest += BIN_STRIDE;
		}
		//字节中前后bit位互换
		for (int i = 0; i < BIN_BYTE_COUNT; i++)
		{
			BYTE bTemp = pbByteBinImage[i];
			BYTE bTemp2 = 0;
			bTemp2 |= (bTemp & 0x80) >> 7;
			bTemp2 |= (bTemp & 0x40) >> 5;
			bTemp2 |= (bTemp & 0x20) >> 3;
			bTemp2 |= (bTemp & 0x10) >> 1;
			bTemp2 |= (bTemp & 0x08) << 1;
			bTemp2 |= (bTemp & 0x04) << 3;
			bTemp2 |= (bTemp & 0x02) << 5;
			bTemp2 |= (bTemp & 0x01) << 7;
			pbByteBinImage[i] = bTemp2;
		}
	}
}

static BOOL BinImage2BitmapData(int iCX, int iCY, BYTE* pbByteBinImage, char* pbBitmapData, int* piBitmapDataLen)
{
	BOOL bSuccess=TRUE;
	int iByteBinImageLen = BIN_BYTE_COUNT;
	if ( iCX != BIN_WIDTH || iCY != BIN_HEIGHT || pbByteBinImage == NULL || piBitmapDataLen == NULL )
	{
		bSuccess = FALSE;
	}
	else
	{
		int iStride = ((BIN_STRIDE + 3) >> 2) << 2;
		int iBmpDataSize = iStride * BIN_HEIGHT;

		BITMAPFILEHEADER bmfHdr;
		bmfHdr.bfType = 0x4d42; 
		bmfHdr.bfSize = iBmpDataSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
		bmfHdr.bfReserved1 = 0; 
		bmfHdr.bfReserved2 = 0; 
		bmfHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD); 

		BITMAPINFO *pInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
		pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
		pInfo->bmiHeader.biWidth = BIN_WIDTH;
		pInfo->bmiHeader.biHeight = BIN_HEIGHT;
		pInfo->bmiHeader.biPlanes = 1; 
		pInfo->bmiHeader.biBitCount = 1; 
		pInfo->bmiHeader.biCompression = BI_RGB; 
		pInfo->bmiHeader.biSizeImage = 0;
		pInfo->bmiHeader.biXPelsPerMeter = 0; 
		pInfo->bmiHeader.biYPelsPerMeter = 0; 
		pInfo->bmiHeader.biClrUsed = 0; 
		pInfo->bmiHeader.biClrImportant = 0; 

		pInfo->bmiColors[0].rgbBlue = 0;
		pInfo->bmiColors[0].rgbGreen = 0;
		pInfo->bmiColors[0].rgbRed = 0;
		pInfo->bmiColors[0].rgbReserved = 0;
		pInfo->bmiColors[1].rgbBlue = 255;
		pInfo->bmiColors[1].rgbGreen = 255;
		pInfo->bmiColors[1].rgbRed = 255;
		pInfo->bmiColors[1].rgbReserved = 0;

		BYTE *pBmpData = (BYTE*)malloc(iBmpDataSize);
		memset(pBmpData, 0, iBmpDataSize);
		BYTE *pSrc = pbByteBinImage;
		BYTE *pDest = pBmpData;
		for (int i = 0; i < BIN_HEIGHT; i++)
		{
			memcpy(pDest, pSrc, BIN_STRIDE);
			pDest += iStride;
			pSrc += BIN_STRIDE;
		}

		memcpy(pbBitmapData, &bmfHdr, sizeof(BITMAPFILEHEADER));
		pbBitmapData += sizeof(BITMAPFILEHEADER);
		*piBitmapDataLen += sizeof(BITMAPFILEHEADER);

		memcpy(pbBitmapData, pInfo, sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
		pbBitmapData += sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
		*piBitmapDataLen += sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);

		memcpy(pbBitmapData, pBmpData, iBmpDataSize);
		pbBitmapData += iBmpDataSize;
		*piBitmapDataLen += iBmpDataSize;

		free(pBmpData);
		free(pInfo);
	}

	return bSuccess;
}

void Bin2BMP(PBYTE pbBinData, PBYTE pbBmpData, INT& nBmpLen)
{
	PBYTE pbByteBinImage[BIN_BYTE_COUNT] = {0};
	RgbBin2ByteBin(pbBinData, (PBYTE)pbByteBinImage);

	int iBitmapDataLen = 0;
	BinImage2BitmapData(BIN_WIDTH, BIN_HEIGHT, (PBYTE)pbByteBinImage, (char*)pbBmpData, &iBitmapDataLen);
	nBmpLen = iBitmapDataLen;
}

// ---------------------------------------------------------------------

const int MAX_HV_COUNT = 1000;
const u_short GETIP_COMMAND_PORT = 6666;
const DWORD32 GETIP_COMMAND = 0x00000101;
const DWORD32 SETIP_COMMAND = 0x00000102;
static int iDeviceCount;
static unsigned char aDeviceAddr[ MAX_HV_COUNT ][ 18 + 32 + 32 ];

// 描述:	搜索局域网内所有的视频处理设备，返回设备数目
// 参数:	pdwMaxCount  此参数值为搜索到的视频处理系统的数目;
// 返回值: 返回S_OK, 表示操作成功;
//         返回E_FAIL, 表示未知的错误导致操作失败;
HRESULT SearchHVDeviceCount(DWORD32 *pdwCount)
{
	char szHostName[1024] = {0};
	gethostname(szHostName, sizeof(szHostName));    //获得本地主机名
	PHOSTENT pHostInfo = gethostbyname(szHostName);//信息结构体

	iDeviceCount = 0;

	while(*(pHostInfo->h_addr_list) != NULL)
	{
		char* szIP = inet_ntoa(*(struct in_addr *) *(pHostInfo->h_addr_list));

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET)
		{
			pHostInfo->h_addr_list ++;
			continue;
		}
		const BOOL on = 1;
		if (0 != setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on)))
		{
			ForceCloseSocket(sock);
			pHostInfo->h_addr_list ++;
			continue;
		}

		SOCKADDR_IN addrClient = {0};
		addrClient.sin_family = AF_INET;
		addrClient.sin_addr.s_addr = inet_addr(szIP);
		addrClient.sin_port = 0;
		if (0 != bind(sock, (sockaddr*)&addrClient, sizeof(addrClient)))
		{
			ForceCloseSocket(sock);
			pHostInfo->h_addr_list ++;
			continue;
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(GETIP_COMMAND_PORT);
		addr.sin_addr.S_un.S_addr = 0xffffffff;
		if (sendto(sock, (char*)&GETIP_COMMAND, sizeof(GETIP_COMMAND), 0, (sockaddr*)&addr, sizeof(addr)) != sizeof(GETIP_COMMAND))
		{
			ForceCloseSocket(sock);
			pHostInfo->h_addr_list ++;
			continue;
		}

		while (true)
		{
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			fd_set rset;
			FD_ZERO(&rset);
			FD_SET(sock, &rset);
			int iTemp = select(static_cast< int >(sock) + 1, &rset, NULL, NULL, &tv);
			if (iTemp == SOCKET_ERROR) {
				break;
			}
			if (iTemp == 0) {
				break;
			}
			int iLen = sizeof(addr);
			char acBuf[ 4096 ];
			int iRecvLen = recvfrom(sock, acBuf, sizeof(acBuf), 0, (sockaddr*)&addr, &iLen);

			TiXmlDocument cXmlDoc;
			if(cXmlDoc.Parse(acBuf))
			{ 
				// 新的搜索协议，（目前包括：水星、地球）
				const TiXmlElement* pRootElement = cXmlDoc.RootElement();
				if (NULL == pRootElement)
				{
					continue;
				}
				const TiXmlElement* pCmdArgElement = pRootElement->FirstChildElement("CmdName");
				if (NULL == pCmdArgElement)
				{
					continue;
				}
				const char* szIP = pCmdArgElement->Attribute("IP");
				const char* szMask = pCmdArgElement->Attribute("Mask");
				const char* szGateway = pCmdArgElement->Attribute("Gateway");
				const char* szMac = pCmdArgElement->Attribute("MAC");
				const char* szSN = pCmdArgElement->Attribute("SN");
				const char* szMode = pCmdArgElement->Attribute("Mode");
				if (NULL == szIP || 
					NULL == szMask || 
					NULL == szGateway || 
					NULL == szMac || 
					NULL == szSN || 
					NULL == szMode)
				{
					continue;
				}

				char szLog[128];
				sprintf(szLog, "SearchHVDeviceCount found a Mercury device with IP [%s]", szIP);
				WrightLogEx("TestIP", szLog);

				DWORD dwIP[4];
				DWORD dwMask[4];
				DWORD dwGateway[4];
				DWORD dwMac[6];
				sscanf(szIP, "%d.%d.%d.%d", &dwIP[3], &dwIP[2], &dwIP[1], &dwIP[0]);                                   // 逆序
				sscanf(szMask, "%d.%d.%d.%d", &dwMask[3], &dwMask[2], &dwMask[1], &dwMask[0]);                         // 逆序
				sscanf(szGateway, "%d.%d.%d.%d", &dwGateway[3], &dwGateway[2], &dwGateway[1], &dwGateway[0]);          // 逆序
				sscanf(szMac, "%x:%x:%x:%x:%x:%x", &dwMac[0], &dwMac[1], &dwMac[2], &dwMac[3], &dwMac[4], &dwMac[5]);  // 不需逆序

				// 搜索原有设备列表，如果是新设备则加入设备列表
				int i = 0;
				for (i = 0; i < iDeviceCount; i++)
				{
					if ((dwMac[0] & 0xFF) == aDeviceAddr[i][0] && 
						(dwMac[1] & 0xFF) == aDeviceAddr[i][1] && 
						(dwMac[2] & 0xFF) == aDeviceAddr[i][2] &&
						(dwMac[3] & 0xFF) == aDeviceAddr[i][3] &&
						(dwMac[4] & 0xFF) == aDeviceAddr[i][4] &&
						(dwMac[5] & 0xFF) == aDeviceAddr[i][5])
					{
						break;
					}
				}
				if (i == iDeviceCount)
				{
					if (iDeviceCount >= MAX_HV_COUNT)
						break;

					memset(aDeviceAddr[ iDeviceCount ], 0, 18 + 32 + 32);//todo add Len

					aDeviceAddr[iDeviceCount][0] = dwMac[0] & 0xFF;
					aDeviceAddr[iDeviceCount][1] = dwMac[1] & 0xFF;
					aDeviceAddr[iDeviceCount][2] = dwMac[2] & 0xFF;
					aDeviceAddr[iDeviceCount][3] = dwMac[3] & 0xFF;
					aDeviceAddr[iDeviceCount][4] = dwMac[4] & 0xFF;
					aDeviceAddr[iDeviceCount][5] = dwMac[5] & 0xFF;

					aDeviceAddr[iDeviceCount][6] = dwIP[0] & 0xFF;
					aDeviceAddr[iDeviceCount][7] = dwIP[1] & 0xFF;
					aDeviceAddr[iDeviceCount][8] = dwIP[2] & 0xFF;
					aDeviceAddr[iDeviceCount][9] = dwIP[3] & 0xFF;

					aDeviceAddr[iDeviceCount][10] = dwMask[0] & 0xFF;
					aDeviceAddr[iDeviceCount][11] = dwMask[1] & 0xFF;
					aDeviceAddr[iDeviceCount][12] = dwMask[2] & 0xFF;
					aDeviceAddr[iDeviceCount][13] = dwMask[3] & 0xFF;

					aDeviceAddr[iDeviceCount][14] = dwGateway[0] & 0xFF;
					aDeviceAddr[iDeviceCount][15] = dwGateway[1] & 0xFF;
					aDeviceAddr[iDeviceCount][16] = dwGateway[2] & 0xFF;
					aDeviceAddr[iDeviceCount][17] = dwGateway[3] & 0xFF;

					strncpy((char*)&aDeviceAddr[iDeviceCount][18], szSN, 31);
					strncpy((char*)&aDeviceAddr[iDeviceCount][40], szMode, 31);



					iDeviceCount++;

				}
			}
			else
			{ // 1.0和2.0版协议
				if (iRecvLen == 22 || iRecvLen == 22 + 32 || iRecvLen == 22 + 32 + 32) 
				{
					DWORD32 dwCommand;
					memcpy(&dwCommand, acBuf, sizeof(dwCommand));
					if (dwCommand == GETIP_COMMAND)
					{
						if((acBuf[9]&0xFF) == 0xB6 || (acBuf[9]&0xFF) == 0xB7
							|| (acBuf[9]&0xFF) == 0xB8 || (acBuf[9]&0xFF) == 0xB9)
						{
							int i;
							for (i = 0; i < iDeviceCount; i++)
							{
								if (memcmp(&acBuf[ 4 ], aDeviceAddr[ i ], 6) == 0)
								{
									break;
								}
							}
							if (i == iDeviceCount)
							{
								if (iDeviceCount >= MAX_HV_COUNT)
									break;

								memset(aDeviceAddr[ iDeviceCount ], 0, 18 + 32 + 32);//todo add Len
								memcpy(aDeviceAddr[ iDeviceCount ], &acBuf[ 4 ], iRecvLen - sizeof(dwCommand));


								iDeviceCount++;

							}
						}
					}
				}
			}
		}

		ForceCloseSocket(sock);
		pHostInfo->h_addr_list ++;
		Sleep(100);
	}

	*pdwCount = iDeviceCount;

	return S_OK;
}

// 描述:	根据索引查询设备ip地址
// 参数:	iIndex			指定查询的索引,该索引不大于视频处理系统的最大数目;
//			pdw64MacAddr	返回设备以太网地址
//			pdwIP			返回设备IP地址
//			pdwMask			返回设备IP掩码
//			pdwGateway		返回设备IP网关地址
// 返回值:	返回S_OK, 表示操作成功;
//			返回E_FAIL, 表示未知的错误导致操作失败;
// 注意:	在调用本函数之前先调用SearchHVDeviceCount搜索设备
HRESULT GetHVDeviceAddr(
	int iIndex,
	DWORD64 *pdw64MacAddr,
	DWORD32 *pdwIP,
	DWORD32 *pdwMask,
	DWORD32 *pdwGateway
)
{
	if (iIndex >= iDeviceCount || iIndex < 0)
		return E_FAIL;
	memcpy(pdw64MacAddr, aDeviceAddr[ iIndex ], 6);
	memcpy(pdwIP, &aDeviceAddr[ iIndex ][ 6 ], 4);
	memcpy(pdwMask, &aDeviceAddr[ iIndex ][ 6 + 4 ], 4);
	memcpy(pdwGateway, &aDeviceAddr[ iIndex ][ 6 + 4 + 4 ], 4);

	//序列号
	const char* pSerialNo = (char*)(&aDeviceAddr[ iIndex ][ 6 + 4 + 4 + 4 ]);
	if (pSerialNo != 0)
	{
	}
	return S_OK;
}

HRESULT HvGetDeviceExteInfo(int iIndex, LPSTR lpExtInfo, int iBufLen)
{
	if(lpExtInfo == NULL || iBufLen <= 0)
	{
		return E_FAIL;
	}
	
	int iSerStrinLen = (int)strlen((char*)&aDeviceAddr[iIndex][18]); 
	int iSerStrinLen2 = (int)strlen((char*)&aDeviceAddr[iIndex][18 + 32]);

	if(iSerStrinLen <= 0 || iBufLen <= (iSerStrinLen + iSerStrinLen2))
	{
		return E_FAIL;
	}
	
	if (iSerStrinLen2 > 0)
	{
		memcpy((char*)lpExtInfo, (char*)&aDeviceAddr[iIndex][18], 32);
		memcpy((char*)(lpExtInfo + 32), (char*)&aDeviceAddr[iIndex][18 + 32], iSerStrinLen2);
		lpExtInfo[32 + iSerStrinLen2] = '\0';
	}
	else
	{
		memcpy((char*)lpExtInfo, (char*)&aDeviceAddr[iIndex][18], iSerStrinLen);
		lpExtInfo[iSerStrinLen + iSerStrinLen2] = '\0';
	}
	
	return S_OK;
}

HRESULT SetIPFromMac(DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway)
{
	char szHostName[1024] = {0};
	gethostname(szHostName, sizeof(szHostName));    //获得本地主机名
	PHOSTENT pHostInfo = gethostbyname(szHostName);//信息结构体

	int iRst = E_FAIL;
	while(*(pHostInfo->h_addr_list) != NULL)
	{
		char* szIP = inet_ntoa(*(struct in_addr *) *pHostInfo->h_addr_list);

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET)
		{
			pHostInfo->h_addr_list++;
			continue;
		}

		const BOOL on = 1;
		if (0 != setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on)))
		{
			ForceCloseSocket(sock);
			pHostInfo->h_addr_list++;
			continue;
		}

		SOCKADDR_IN	addrClient		= {0};
		addrClient.sin_family		= AF_INET;
		addrClient.sin_addr.s_addr = inet_addr(szIP);
		addrClient.sin_port = 0;				/// 0 表示由系统自动分配端口号
		if (0 != bind (sock, (sockaddr*)&addrClient, sizeof(addrClient)))
		{
			ForceCloseSocket(sock);
			pHostInfo->h_addr_list++;
			continue;
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(GETIP_COMMAND_PORT);
		addr.sin_addr.S_un.S_addr = 0xffffffff;
		char acSendBuf[ 22 ];
		memcpy(&acSendBuf[ 0 ], &SETIP_COMMAND, 4);
		memcpy(&acSendBuf[ 4 ], &dw64MacAddr, 6);
		memcpy(&acSendBuf[ 10 ], &dwIP, 4);
		memcpy(&acSendBuf[ 14 ], &dwMask, 4);
		memcpy(&acSendBuf[ 18 ], &dwGateway, 4);
		if (sendto(sock, acSendBuf, sizeof(acSendBuf), 0, (sockaddr*)&addr, sizeof(addr)) != sizeof(acSendBuf))
		{
			ForceCloseSocket(sock);
			pHostInfo->h_addr_list++;
			continue;
		}

		struct timeval tv;
		tv.tv_sec = 8;
		tv.tv_usec = 0;
		fd_set rset;
		FD_ZERO(&rset);
		FD_SET(sock, &rset);
		iRst = E_FAIL;
		int iTemp = select(static_cast< int >(sock) + 1, &rset, NULL, NULL, &tv);
		if (iTemp != SOCKET_ERROR && iTemp != 0)
		{
			int iLen = sizeof(addr);
			char acBuf[ 20 ];
			int iRecvLen = recvfrom(sock, acBuf, sizeof(acBuf), 0, (sockaddr*)&addr, &iLen);
			if (iRecvLen == 8 || iRecvLen == 4 )
			{ 
				iRst = 0;
			}
		}
		ForceCloseSocket(sock);
		pHostInfo->h_addr_list++;
		Sleep(100);

		if (iRst == 0)
		{
			break;
		}

	}
	

	//删除旧的ARP缓存
	Sleep(2000);
	ShellExecute(NULL, "", "arp", "-d *", NULL, SW_HIDE);

	return iRst==0?S_OK:E_FAIL;
}

// ---------------------------------------------------------------------

BOOL MyGetIpString(DWORD dwIP, LPSTR lpszIP)
{
	if( lpszIP == NULL ) return FALSE;

	PBYTE pByte=(PBYTE)&dwIP;
	if( -1 == sprintf(lpszIP, "%d.%d.%d.%d", pByte[3],pByte[2],pByte[1],pByte[0]) )
	{
		return FALSE;
	}
	return TRUE;
}

BOOL MyGetMacString(DWORD64 dwMac, LPSTR lpszMac, DWORD dwDevType)
{
	if( lpszMac == NULL ) return FALSE;

	PBYTE pByte=(PBYTE)&dwMac;

	BOOL	fIsNeedType = FALSE;
	if(dwDevType == DEV_TYPE_ALL || dwDevType == DEV_TYPE_UNKNOWN)
	{
		if(pByte[5] == 0xB6 || pByte[5] == 0xB7
			|| pByte[5] == 0xB8 || pByte[5] == 0xB9)
		{
			fIsNeedType = TRUE;
			goto done;
		}
		return FALSE;
	}
	BOOL fIsGetHVCAM_200W = FALSE;
	BOOL fIsGetHVCAM_500W = FALSE;
	BOOL fIsGetHVSINGLE = FALSE;
	BOOL fIsGetHVCAM_SINGLE = FALSE;
	if(dwDevType & 0x1) fIsGetHVCAM_200W = TRUE;
	if((dwDevType>>1) & 0x1) fIsGetHVCAM_500W = TRUE;
	if((dwDevType>>2) & 0x1) fIsGetHVSINGLE = TRUE;
	if((dwDevType>>3) & 0x1) fIsGetHVCAM_SINGLE = TRUE;

	if ( fIsGetHVCAM_200W && pByte[5] == 0xB6 )
	{
		fIsNeedType = TRUE;
	}
	if(fIsGetHVCAM_500W && pByte [5] == 0xB7)
	{
		fIsNeedType = TRUE;
	}
	if(fIsGetHVSINGLE && pByte [5] == 0xB8)
	{
		fIsNeedType = TRUE;
	}
	if(fIsGetHVCAM_SINGLE && pByte[5] == 0xB9)
	{
		fIsNeedType = TRUE;
	}

done:
	if(fIsNeedType)
	{
		if( -1 == sprintf(lpszMac, "%02X-%02X-%02X-%02X-%02X-%02X", pByte[0],pByte[1],pByte[2],pByte[3],pByte[4],pByte[5]) )
		{
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

HRESULT HvSendXmlCmd(char* szIP, LPCSTR szCmd, LPSTR szRetBuf,
					 INT iBufLen, INT* piRetLen, SOCKET sktSend)
{
	if ( NULL == szIP || NULL == szCmd || NULL == szRetBuf || iBufLen <= 0 )
	{
		return E_FAIL;
	}
	const int iMaxLen = (1024 << 4);
	char* pszXmlBuf = new char[iMaxLen];  // 16KB
	int iXmlBufLen = iMaxLen;
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	memset(pszXmlBuf, 0, iMaxLen);
	TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // 检查szCmd是否是XML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(GetProtocolVersion(szIP), szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
		szCmd = (char*)pszXmlBuf;
	}
	bool fRet = ExecXmlExtCmdEx(szIP, (char*)szCmd, (char*)szRetBuf, iBufLen, sktSend);
	if ( piRetLen )
	{
		*piRetLen = iBufLen;
	}
	SAFE_DELETE_ARG(pszXmlBuf);
	return (true == fRet) ? S_OK : E_FAIL;
}

bool ExecXmlExtCmdEx(char* szIP, char* szXmlCmd, char* szRetBuf,
					 int& iBufLen, SOCKET sktSend)
{
	if ( strlen(szXmlCmd) > (1024 << 4) )
	{
		return false;
	}
	const int iMaxLen = (1024 << 10);
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
	{
		return false;
	}
	memset(pszTmpXmlBuf, 0, iMaxLen);
	bool fIsTempSocket = true;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;
	if(sktSend == INVALID_SOCKET)
	{
		if(!ConnectCamera(szIP, wPort, hSocketCmd))
		{
			SAFE_DELETE_ARG(pszTmpXmlBuf);
			return false;
		}
		sktSend = hSocketCmd;
	}
	else
	{
		hSocketCmd = sktSend;
		fIsTempSocket = false;
	}
	bool fIsOk = false;
	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
	if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
	{
		if(fIsTempSocket)
		{
			ForceCloseSocket(hSocketCmd);
		}
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return false;
	}
	if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize, 0) )
	{
		if(fIsTempSocket)
		{
			ForceCloseSocket(hSocketCmd);
		}
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return false;
	}
	if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
	{
		if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult 
			&& 0 < cCmdRespond.dwInfoSize )
		{
			int iRecvLen = RecvAll(hSocketCmd, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
			if ( cCmdRespond.dwInfoSize ==  iRecvLen)
			{
				if ( iBufLen > (int)cCmdRespond.dwInfoSize )
				{
					memcpy(szRetBuf, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
					memset(szRetBuf+cCmdRespond.dwInfoSize, 0, 1);
					iBufLen = cCmdRespond.dwInfoSize;
					if(fIsTempSocket)
					{
						ForceCloseSocket(hSocketCmd);
					}
					SAFE_DELETE_ARG(pszTmpXmlBuf);
					return true;
				}
				else
				{
					iBufLen = cCmdRespond.dwInfoSize;
					if(fIsTempSocket)
					{
						ForceCloseSocket(hSocketCmd);
					}
					SAFE_DELETE_ARG(pszTmpXmlBuf);
					return false;
				}
			}
			else
			{
				iBufLen = 0;
				if(fIsTempSocket)
				{
					ForceCloseSocket(hSocketCmd);
				}
				SAFE_DELETE_ARG(pszTmpXmlBuf);
				return false;
			}
		}
		fIsOk = true;
	}
	if(fIsTempSocket)
	{
		ForceCloseSocket(hSocketCmd);
	}
	SAFE_DELETE_ARG(pszTmpXmlBuf);
	return fIsOk;
}

const int MAX_WITHE_BLACK_LENGTH= (1024<<10) * 2 + 1024;

bool ExecXmlExtCmdMercury(char* szIP, char* szXmlCmd, char* szRetBuf,
					 int& iBufLen, SOCKET sktSend)
{
	/*
	if ( strlen(szXmlCmd) > (1024 << 4) )
	{
		return false;
	}*/

	if ( strlen(szXmlCmd) > MAX_WITHE_BLACK_LENGTH)
	{
		return false;
	}
	char sLog[256];
	//const int iMaxLen = (1024 << 10);
	const int iMaxLen = MAX_WITHE_BLACK_LENGTH;
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
	{
		return false;
	}
	memset(pszTmpXmlBuf, 0, iMaxLen);
	bool fIsTempSocket = true;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;
	if(sktSend == INVALID_SOCKET)
	{
		if (
			strstr(szXmlCmd, "AutoTestCamera")
			)
		{
			if(!ConnectCamera(szIP, wPort, hSocketCmd, 8, 30000))
			{
				SAFE_DELETE_ARG(pszTmpXmlBuf);
				return false;
			}
		}
		else if (strstr(szXmlCmd, "SetNameList") || strstr(szXmlCmd, "GetNameList"))
		{
			if(!ConnectCamera(szIP, wPort, hSocketCmd, 1, 2000))
			{
				SAFE_DELETE(pszTmpXmlBuf);
				return false;
			}

			//设置超时
			int iReciveTimeOutMS = 50000;
			setsockopt(hSocketCmd, SOL_SOCKET, SO_RCVTIMEO, (char*)&iReciveTimeOutMS, sizeof(iReciveTimeOutMS));

		}
		else
		{
			if(!ConnectCamera(szIP, wPort, hSocketCmd, 1, 2000))
			{
				SAFE_DELETE_ARG(pszTmpXmlBuf);
				return false;
			}
		}

		sktSend = hSocketCmd;
	}
	else
	{
		hSocketCmd = sktSend;
		fIsTempSocket = false;
	}

	TiXmlDocument cXmlDocReceived;
    TiXmlDocument cResultXmlDoc;
    TiXmlPrinter cPrinter;

	bool fIsOk = false;
	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
    // 发送通知信息
	if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
	{
		fIsOk = false;
		goto cleanup;
	}
    // 发送命令
	if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize, 0) )
	{
		fIsOk = false;
		goto cleanup;
	}
    // 接收
	if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
	{
		if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult 
			&& 0 < cCmdRespond.dwInfoSize )
		{
			int iRecvLen = RecvAll(hSocketCmd, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
			if ( cCmdRespond.dwInfoSize ==  iRecvLen)
			{
				if ( iBufLen > (int)cCmdRespond.dwInfoSize )
				{
					fIsOk = true;
				}
				else
				{
					iBufLen = cCmdRespond.dwInfoSize;
					fIsOk = false;
					goto cleanup;
				}
			}
			else
			{
				iBufLen = 0;
				fIsOk = false;
				goto cleanup;
			}
		}
		fIsOk = true;
	}
	
	WrightLogEx( szIP, pszTmpXmlBuf  );

	// 组织上位机XML头
	//TiXmlDeclaration* pResultDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	TiXmlElement* pResultRootElement = new TiXmlElement("HvCmdRespond");
	if ( /*NULL == pResultDecl ||*/ NULL == pResultRootElement)
	{
        fIsOk = false;
		sprintf(sLog, "ExecXmlExtCmdMercury out of memory for pResultDecl or pResultRootElement");
		WrightLogEx(szIP, sLog);
		goto cleanup;
    }
    pResultRootElement->SetAttribute("ver", "2.0"); // TODO: 如果改为3.0，会对上位机造成什么影响？  有影响 3.0直接不解析
    
	// 解析设备结果XML并组装上位机XML
	if ( cXmlDocReceived.Parse(pszTmpXmlBuf) )
	{
		TiXmlElement* pCurElement = cXmlDocReceived.FirstChildElement("HvCmdRespond");
		if ( pCurElement == NULL )
		{
			sprintf(sLog, "ExecXmlExtCmdMercury can't find \"HvCmdRespond\" element");
			WrightLogEx(szIP, sLog);
	        fIsOk = false;
			goto cleanup;
		}

		const char* szVerAttrib = pCurElement->Attribute("Ver");
		if ( (szVerAttrib==NULL) 
			|| (0 != strcmp(szVerAttrib, "3.0")) )
		{
			sprintf(sLog, "ExecXmlExtCmdMercury can't find \"Ver\" attribute with value \"3.0\"");
			WrightLogEx(szIP, sLog);
	        fIsOk = false;
			goto cleanup;
		}

		pCurElement = pCurElement->FirstChildElement(); // TODO: 如何判断"CmdName"
		const char * pszCmdName = NULL;
		const char * pszRetMsg = NULL;
		const char * pszRetCode = NULL;
		char szRetCode[16];
		while(pCurElement)
		{
			bool fCmdTransformed = false;
			
		    TiXmlElement* pElementRet = new TiXmlElement("CmdName");
		    if ( NULL == pElementRet )
		    {
		        fIsOk = false;
				sprintf(sLog, "ExecXmlExtCmdMercury out of memory for pElementRet");
				WrightLogEx(szIP, sLog);
				goto cleanup;
		    }
		
			if(pszCmdName = pCurElement->GetText())
			{
				sprintf(sLog, "ExecXmlExtCmdMercury received response from command %s", pszCmdName);
				WrightLogEx(szIP, sLog);
				pszRetCode = pCurElement->Attribute("RetCode");
				pszRetMsg = pCurElement->Attribute("RetMsg");
				if(!pszRetMsg)
				{
					pszRetMsg = "";
				}
				if(strcmp(pszCmdName, "GetConnectedIP") == 0)
				{
					printf("aaaaaa");
				}

				//水星
				//if(pszRetMsg && pszRetCode)
				if(pszRetMsg || pszRetCode)
				{
					// 组装上位机XML
					int iCmdIndex = SearchXmlCmdMapMercury(pszCmdName);
					if (-1 == iCmdIndex)
					{ // 没找到对应的命令
						fCmdTransformed = false;
						sprintf(sLog, "ExecXmlExtCmdMercury can't find command [%s]", pszCmdName);
						WrightLogEx(szIP, sLog);
					}
					else
					{
						TiXmlText *pXmlCmdText = new TiXmlText(pszCmdName);
						if (NULL == pXmlCmdText)
						{
					        fIsOk = false;
							sprintf(sLog, "ExecXmlExtCmdMercury out of memory for pXmlCmdText");
							WrightLogEx(szIP, sLog);
							goto cleanup;
					    }
					    pElementRet->LinkEndChild(pXmlCmdText);
						////水星 临时调试 GetVersion命令
						//if(strcmp(pszCmdName, "GetVersion") == 0)
						//{
						//	pszRetCode = "0";
						//}
						strcpy(szRetCode, pszRetCode);
						if (0 == strcmp("0", pszRetCode))
						{
							switch ( g_XmlCmdAppendInfoMap[iCmdIndex].emCmdClass )
							{
								case XML_CMD_GETTER: // 为结果数值添上名称
									switch ( g_XmlCmdAppendInfoMap[iCmdIndex].emCmdType )
									{
										case XML_CMD_TYPE_INTARRAY1D: // TODO: 2D数组的处理
											{
												char * pszRetMsgBuf = new char[strlen(pszRetMsg) + 1];
												char * pszRetMsgBufBackup = pszRetMsgBuf;
												strcpy(pszRetMsgBuf, pszRetMsg);
												for (int i = 0; i < g_XmlCmdAppendInfoMap[iCmdIndex].iArrayColumn; i++)
												{
													char *pszBufIndex = strstr(pszRetMsgBuf, ",");
													if (NULL != pszBufIndex)
													{
														*pszBufIndex = '\0';
														pElementRet->SetAttribute(
															*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),
															pszRetMsgBuf);
														pszRetMsgBuf = pszBufIndex + 1;
														if (i == g_XmlCmdAppendInfoMap[iCmdIndex].iArrayColumn - 1)
														{
															fCmdTransformed=true;
														}
													}
													else
													{
														if (i == g_XmlCmdAppendInfoMap[iCmdIndex].iArrayColumn - 1)
														{ // 最后一个字段
															pElementRet->SetAttribute(
																*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),
																pszRetMsgBuf);
															fCmdTransformed = true;
														}
														else
														{ // 既不是最后一个字段，又不包含“，”
															strcpy(szRetCode, "-1");
															fCmdTransformed = false;
															break;
														}
													}
												}
												SAFE_DELETE_ARG(pszRetMsgBufBackup);
											}
											break;
										case XML_CMD_TYPE_INT:
										case XML_CMD_TYPE_DOUBLE:
										case XML_CMD_TYPE_FLOAT:
										case XML_CMD_TYPE_BOOL:
										case XML_CMD_TYPE_DWORD:
										case XML_CMD_TYPE_STRING:
											////水星临时测试 GetDevType
											//if(strcmp(pszCmdName,"GetDevType") == 0)
											//{
											//	char *pcDevInfo = "DevType=[6467_280W],BuildNo=[2.2.1.168]";
											//	char szDevType[64] = {0};
											//	char szBuildNo[64] = {0};
											//	char *pcParam1 = g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1;
											//	char *pcParam2 = g_XmlCmdAppendInfoMap[iCmdIndex].szParamName2;
											//	if(strstr(pcDevInfo, pcParam1))
											//	{
											//		pcDevInfo = strstr(pcDevInfo,"[")+1;
											//		memcpy(szDevType,  pcDevInfo, (strstr(pcDevInfo, "]") -  pcDevInfo));
											//		pElementRet->SetAttribute(pcParam1, szDevType);
											//		pcDevInfo = strstr(pcDevInfo, ",");
											//	}
											//	if(strstr(pcDevInfo, pcParam2))
											//	{
											//		pcDevInfo = strstr(pcDevInfo,"[")+1;
											//		memcpy(szBuildNo, pcDevInfo, (strstr(pcDevInfo, "]") -  pcDevInfo));
											//		pElementRet->SetAttribute(pcParam2, szBuildNo);
											//	}
											//	fCmdTransformed = true;
											//break;
											//}
											pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, pszRetMsg);
											fCmdTransformed = true;
											break;
										case XML_CMD_TYPE_CUSTOM:
											//pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, pszRetMsg);
											//pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, "Date[1978.10.01],Time[23:08:01 239]");
											if (  strcmp(pszCmdName, "ReadFPGA") == 0
												||strcmp(pszCmdName, "DoLogin") == 0
												)
											{
												pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, pszRetMsg);
												fCmdTransformed = true;
											}
											if(strcmp(pszCmdName,"DateTime") == 0)
											{
												char szDate[16] = {0};
												char szTime[16] = {0};
												char* pDateBegin = strstr( pszRetMsg , "Date[" );
												if ( NULL != pDateBegin )
												{
													pDateBegin+=5;
												}
												char* pDateEnd = strstr( pszRetMsg , "]" );
												char* pTimeBegin = strstr( pszRetMsg , "Time[" );
												if ( NULL != pTimeBegin )
												{
													pTimeBegin+=5;
												}
												
												char* pTimeEnd = strstr( pTimeBegin , "]");
								
												if ( NULL != pDateBegin )
												{
													memcpy( szDate ,pDateBegin , pDateEnd-pDateBegin );
												}
												if ( NULL != pTimeBegin )
												{
													memcpy( szTime ,pTimeBegin , pTimeEnd-pTimeBegin);
												}
												if(!szDate[0] && !szTime[0])
												{
													memcpy(szDate, "0000.00.00",strlen("0000.00.00"));
													memcpy(szTime, "00:00:00 000", strlen("00:00:00 000"));
												}
												pElementRet->SetAttribute("Date", szDate);
												pElementRet->SetAttribute("Time", szTime);
											}
											if(strcmp(pszCmdName,"GetConnectedIP") == 0)
											{
											//	pszRetMsg = "COUNT[4],[127.1.1.1,9902],[168.12.4.4,444],[124.12.1.87,95555],[168.12.1.87,9909]";
												int iCount = 0;
												char szCount[3] = {0};
												char szIP[512] = {0};
												sscanf(pszRetMsg,"COUNT[%d],%s", &iCount, szIP);
												sprintf(szCount, "%d", iCount);
												pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, szCount);
												char* pcTmpIP = szIP; 
												for(int i=0; i < iCount; i++)
												{
													char szIPNo[6] = {0};
													char szIPTmp[64] = {0};
													sprintf(szIPNo, "IP%d", i+1);
													pcTmpIP++;
													memcpy(szIPTmp, pcTmpIP, (strstr(pcTmpIP, "]") -  pcTmpIP) );
													pElementRet->SetAttribute(szIPNo, szIPTmp);
													pcTmpIP += (2 + strlen(szIPTmp));
												}
												
											}
											if(strcmp(pszCmdName,"GetUsers") == 0)
											{
												//	pszRetMsg = "UserCount=[2],UserName00=[admin],UserAuthority00=[1],UserName01=[usr1], UserAuthority01=[1]";
												int iCount = 0;
												char szCount[3] = {0};
												char szUsersList[512] = {0};
												char szUserNameBeginLabel[128];
												char szUserAuthorityBeginLabel[128];
												char szEndLabel[128];
												sscanf(pszRetMsg,"UserCount=[%d],%s", &iCount, szUsersList);
												sprintf(szCount, "%d", iCount);
												pElementRet->SetAttribute("UserCount", szCount);
												char* pcTmpUserlist = szUsersList; 


												char szUserNameKeyName[256];
												char szUserAuthorityKeyName[256];
												for(int i=0; i < iCount; i++)
												{
													char szUserNameValue[256]={0};
													char szUserAuthorityValue[256]={0};
													sprintf(szUserNameKeyName, "UserName%02d", i);
													sprintf(szUserAuthorityKeyName, "UserAuthority%02d", i);
													sprintf(szUserNameBeginLabel, "%s=[",szUserNameKeyName);
													sprintf(szUserAuthorityBeginLabel, "%s=[",szUserAuthorityKeyName);
													
													//读取用户名
													char* pUserNameBegin = strstr( pszRetMsg , szUserNameBeginLabel );
													if (pUserNameBegin!=NULL)
													{
														pUserNameBegin+=strlen(szUserNameBeginLabel);
													}
													else
													{
														fIsOk = false;
														goto cleanup;
													}
													char* pUserNameEnd=strstr(pUserNameBegin, "]");
													if ( NULL != pUserNameEnd )
													{
														memcpy( szUserNameValue ,pUserNameBegin , pUserNameEnd-pUserNameBegin );
													}	
													else
													{
														fIsOk = false;
														goto cleanup;
													}
													pElementRet->SetAttribute(szUserNameKeyName, szUserNameValue);


													//读取用户等级
													char* pUserAuthorityBegin=strstr(pszRetMsg, szUserAuthorityBeginLabel);
													if (pUserAuthorityBegin != NULL)
													{
														pUserAuthorityBegin+=strlen(szUserAuthorityBeginLabel);
													}
													char* pUserAuthorityEnd=strstr(pUserAuthorityBegin, "]");
													if (NULL != pUserAuthorityEnd)
													{
														memcpy(szUserAuthorityValue, pUserAuthorityBegin, pUserAuthorityEnd-pUserAuthorityBegin);
													}
													pElementRet->SetAttribute(szUserAuthorityKeyName, szUserAuthorityValue);
 
												}

											}
											if(strcmp(pszCmdName,"GetVersion") == 0)
											{
												char *pcVersion = "SoftVersion=[水星1.0],ModelVersion=[金星2.0]";
												pcVersion = (char *)pszRetMsg;
												char szSoftVersion[64] = {0};
												char szModelVersion[64] = {0};
												char *pcParam1 = g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1;
												char *pcParam2 = g_XmlCmdAppendInfoMap[iCmdIndex].szParamName2;
												if(strstr(pcVersion, pcParam1))
												{
													pcVersion = strstr(pcVersion,"[")+1;
													memcpy(szSoftVersion,  pcVersion, (strstr(pcVersion, "]") -  pcVersion));
													pElementRet->SetAttribute(pcParam1, szSoftVersion);
													pcVersion = strstr(pcVersion, ",");
												}
												if(strstr(pcVersion, pcParam2))
												{
													pcVersion = strstr(pcVersion,"[")+1;
													memcpy(szModelVersion, pcVersion, (strstr(pcVersion, "]") -  pcVersion));
													pElementRet->SetAttribute(pcParam2, szModelVersion);
												}
											}
											if(strcmp(pszCmdName,"GetDevType") == 0)
											{
												char *pcDevInfo = "DevType=[6467_280W],BuildNo=[2.2.1.168]";
												pcDevInfo = (char *)pszRetMsg;
												char szDevType[64] = {0};
												char szBuildNo[64] = {0};
												char *pcParam1 = g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1;
												char *pcParam2 = g_XmlCmdAppendInfoMap[iCmdIndex].szParamName2;
												if(strstr(pcDevInfo, pcParam1))
												{
													pcDevInfo = strstr(pcDevInfo,"[")+1;
													memcpy(szDevType,  pcDevInfo, (strstr(pcDevInfo, "]") -  pcDevInfo));
													pElementRet->SetAttribute(pcParam1, szDevType);
													pcDevInfo = strstr(pcDevInfo, ",");
												}
												if(strstr(pcDevInfo, pcParam2))
												{
													pcDevInfo = strstr(pcDevInfo,"[")+1;
													memcpy(szBuildNo, pcDevInfo, (strstr(pcDevInfo, "]") -  pcDevInfo));
													pElementRet->SetAttribute(pcParam2, szBuildNo);
												}
											}
											if (strcmp(pszCmdName,"GetDevBasicInfo") == 0  
												|| strcmp(pszCmdName,"GetDevState") == 0 
												|| strcmp(pszCmdName,"GetCameraBasicInfo") == 0
												|| strcmp(pszCmdName,"GetCameraState") == 0
												|| strcmp(pszCmdName,"GetVideoState") == 0
												|| strcmp(pszCmdName,"GetH264Caption") == 0
												|| strcmp(pszCmdName,"GetJPEGCaption") == 0
												|| strcmp(pszCmdName,"GetH264SecondCaption") == 0
												|| strcmp(pszCmdName, "DoLogin") == 0
												)
											{
												for (int i = 0; i < g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum; i++)
												{
													char szBeginLabel[128];
													char szEndLabel[128];	
													char szResult[128]={0};
													sprintf(szBeginLabel,"%s=[", *(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i) );
													strcpy(szEndLabel, "]");
													if( S_OK == HvGetNonGreedyMatchBetweenBeginLabelAndEndLabel(szBeginLabel, szEndLabel, pszRetMsg, strlen(pszRetMsg), szResult, 128))
													{
														pElementRet->SetAttribute(
															*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),//这里有个地址偏移访问，让属性位置想后移动
															szResult);
													}
													else
													{
														pElementRet->SetAttribute(
															*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),//这里有个地址偏移访问，让属性位置想后移动
															"");
													}
												}
											}

											fCmdTransformed = true;
											break;
									}
									break;
								case XML_CMD_SETTER: // Setter 不需要组织有value的XML返回
									fCmdTransformed = true;
									break;
							}
						}
						else
						{ // RetCode != "0
							fCmdTransformed = true;
						}
					    pElementRet->SetAttribute("RetCode", szRetCode);
					}
				}
				else
				{
					sprintf(sLog, "ExecXmlExtCmdMercury command [%s] has no RetMsg or RetCode", pszCmdName);
					WrightLogEx(szIP, sLog);
				}

			}
			
			if (fCmdTransformed)
			{
				pResultRootElement->LinkEndChild(pElementRet);
			}
			else
			{
				SAFE_DELETE_ARG(pElementRet);
			}
			
			pCurElement = pCurElement->NextSiblingElement();
		}
	}
	
	// 将结果XML转换成字符串输出
	cResultXmlDoc.LinkEndChild(pResultRootElement);
    cResultXmlDoc.Accept(&cPrinter);
    if ( cPrinter.Size() > 0 && cPrinter.Size() < iBufLen )
    {
        iBufLen = cPrinter.Size();
        strcpy(szRetBuf, cPrinter.CStr());
        szRetBuf[cPrinter.Size()] = '\0';
        fIsOk = true;
    }
    else
    {
        iBufLen = cPrinter.Size();
        fIsOk = false;
    }

cleanup:
	if(fIsTempSocket)
	{
		// TODO: 水星的SOCKET在数据交互完成后、关闭SOCKET之前，有可能需要再进行一次READ/SEND操作才能保证数据真正发送完成
		ForceCloseSocket(hSocketCmd);
	}
	SAFE_DELETE_ARG(pszTmpXmlBuf);
	WrightLogEx( szIP, szRetBuf  );
	return fIsOk;
}

PROTOCOL_VERSION GetProtocolVersion(char* szIP)
{
	PROTOCOL_VERSION result = PROTOCOL_VERSION_1;
	SOCKET hSocketCmd = INVALID_SOCKET;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;
	if(!ConnectCamera(szIP, wPort, hSocketCmd))
	{
		return PROTOCOL_UNKNOWN;
	}
	cCmdHeader.dwID = CAMERA_PROTOCOL_COMMAND;
	cCmdHeader.dwInfoSize = 0;
	if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
	{
		ForceCloseSocket(hSocketCmd);
		return PROTOCOL_UNKNOWN;
	}
	if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
	{
		if (cCmdRespond.dwID == CAMERA_PROTOCOL_COMMAND)
		{
			if (cCmdRespond.dwResult == PROTOCOL_MERCURY)
			{
				result = PROTOCOL_VERSION_MERCURY;
			}
			else if ( cCmdRespond.dwResult == PROTOCOL_EARTH )
			{
				result = PROTOCOL_VERSION_EARTH;
				WrightLogEx("" , "PROTOCOL_VERSION_EARTH" );
			}
			else
			{
				result = PROTOCOL_VERSION_2;
			}
		}
		else
		{
			result = PROTOCOL_UNKNOWN;
		}
	}
	ForceCloseSocket(hSocketCmd);
	return result;
}

HRESULT ExecuteCmd( CHAR* pIP, CAMERA_COMMAND_TYPE eCmdId 
				    , CHAR* pCmdData , INT nCmdByte
					, CHAR* pRetData , INT* pnRetByte  )
{

	SOCKET hSocketCmd = INVALID_SOCKET;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;


	if(!ConnectCamera( pIP, wPort, hSocketCmd))
	{
		WrightLogEx( pIP , "ExecuteCmd ConnectCamera Fail\n");
		return E_FAIL;
	}
	//发送请求命令头
	cCmdHeader.dwID = eCmdId;
	cCmdHeader.dwInfoSize = nCmdByte;
	if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
	{
		WrightLogEx( pIP , "ExecuteCmd send header Fail\n");
		ForceCloseSocket(hSocketCmd);
		return E_FAIL;
	}
	
	//发送请求数据包
	if ( nCmdByte > 0 && nCmdByte != send( hSocketCmd , pCmdData , nCmdByte , 0 ) )
	{
		WrightLogEx( pIP , "ExecuteCmd send data Fail\n");
		ForceCloseSocket(hSocketCmd);
		return E_FAIL;
	}
	

	//接收数据包头
	if ( sizeof( cCmdRespond ) != recv( hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0)
			|| cCmdRespond.dwID != eCmdId 
			|| cCmdRespond.dwResult != 0 )
	{
		WrightLogEx( pIP , "ExecuteCmd recv header Fail\n");
		ForceCloseSocket(hSocketCmd);
		if (cCmdRespond.dwResult == 1)
		{
			return S_FALSE;
		}
		else
		{
			return E_FAIL;
		}
	}
	//char szDebug[256];
	//sprintf( szDebug , " Size:%d , Ret:%d , RetByte:%d , BufByte:%d , ")

	
	if ( cCmdRespond.dwInfoSize > 0 )
	{	
		if(
			NULL == pRetData
			|| NULL == pnRetByte  
			|| *pnRetByte < cCmdRespond.dwInfoSize
			|| cCmdRespond.dwInfoSize != RecvAll( hSocketCmd, (char*)pRetData, cCmdRespond.dwInfoSize, *pnRetByte )
		   )
		
		{
			WrightLogEx( pIP , "ExecuteCmd recv buf Fail\n");
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}
	}
	*pnRetByte = cCmdRespond.dwInfoSize;
	//接收数据
	ForceCloseSocket(hSocketCmd);
	return S_OK;
}

//HRESULT HvGetXmlProtocolVersion(char* szIP, DWORD* pdwVersionType)
//{
//	SOCKET hSocketCmd;
//	CAMERA_CMD_HEADER cCmdHeader;
//	CAMERA_CMD_RESPOND cCmdRespond;
//	if(!ConnectCamera(szIP, CAMERA_CMD_LINK_PORT, hSocketCmd))
//	{
//		return E_FAIL;
//	}
//
//	cCmdHeader.dwID = CAMERA_PROTOCOL_COMMAND;
//	cCmdHeader.dwInfoSize = 0;
//	if(send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) != sizeof(cCmdHeader))
//	{
//		ForceCloseSocket(hSocketCmd);
//		return E_FAIL;
//	}
//	if(recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) != sizeof(cCmdRespond))
//	{
//		ForceCloseSocket(hSocketCmd);
//		return E_FAIL;
//	}
//
//	ForceCloseSocket(hSocketCmd);
//	if(cCmdRespond.dwID != CAMERA_PROTOCOL_COMMAND)
//	{
//		return E_FAIL;
//	}
//
//	*pdwVersionType = 1;
//	return S_OK;
//}

typedef struct _IMAGE_INFO
{
	DWORD32 dwCarID;
	DWORD32 dwImgType;
	DWORD32 dwImgWidth;
	DWORD32 dwImgHeight;
	DWORD32 dwTimeLow;
	DWORD32 dwTimeHigh;
	DWORD32 dwEddyType;
	RECT rcPlate;
	RECT rcRedLightPos[20];
	INT iRedLightCount;
	RECT rcFacePos[20];
	int nFaceCount;

	_IMAGE_INFO()
	{
		memset(this, 0, sizeof(*this));
	}
}IMAGE_INFO;

typedef struct _IMAGE_INFO_MERCURY
{
	DWORD32 dwImgType;
	DWORD32 dwImgLength;

	_IMAGE_INFO_MERCURY()
	{
		memset(this, 0, sizeof(*this));
	}
}IMAGE_INFO_MERCURY;

HRESULT HvGetRecordImage(PBYTE pbRecordData, DWORD dwRecordDataLen, RECORD_IMAGE_GROUP_EX* pcRecordImage)
{
	IMAGE_INFO cImgInfo;
	DWORD dwImgInfoLen;
	DWORD dwImgDataLen;
	PBYTE pbTempData = pbRecordData;
	PBYTE pbTempInfo = NULL;
	DWORD dwRemainLen = dwRecordDataLen;
	memset(pcRecordImage, 0, sizeof(RECORD_IMAGE_GROUP_EX));
	if(!pcRecordImage || !pbRecordData || dwRecordDataLen <= 0)
	{
		return E_FAIL;
	}
	while(dwRemainLen > 0)
	{
		memcpy(&dwImgInfoLen, pbTempData, 4);
		pbTempData += 4;
		if(dwImgInfoLen > dwRemainLen  || dwImgInfoLen <= 0)
		{
			CString strError;
			strError.Format("dwImgInfoLen = %d  dwRemainLen = %d",
				dwImgInfoLen, dwRemainLen);
			WrightLogEx("HvGetRecordImage", strError.GetBuffer());
			return E_FAIL;
		}

		int iBuffLen;
		iBuffLen = (sizeof(IMAGE_INFO) < dwImgInfoLen) ? sizeof(IMAGE_INFO) : dwImgInfoLen;
		memcpy(&cImgInfo, pbTempData, iBuffLen);
		pbTempInfo = pbTempData;
		pbTempData += dwImgInfoLen;

		memcpy(&dwImgDataLen, pbTempData, 4);
		pbTempData += 4;
		if(dwImgDataLen > dwRemainLen)
		{
			return E_FAIL;
		}
		//cImgInfo.rcFacePos[0].bottom = 10;cImgInfo.rcFacePos[0].top = 10;cImgInfo.rcFacePos[0].right = 10;cImgInfo.rcFacePos[0].left = 10;
		switch(cImgInfo.dwImgType)
		{
		case RECORD_IMAGE_BEST_SNAPSHOT:
			pcRecordImage->cBestSnapshot.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cBestSnapshot.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cBestSnapshot.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.top = cImgInfo.rcPlate.top; 
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.left = cImgInfo.rcPlate.left; 
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.bottom = cImgInfo.rcPlate.bottom; 
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.right = cImgInfo.rcPlate.right; 
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cBestSnapshot.pbImgData = pbTempData;
			pcRecordImage->cBestSnapshot.pbImgInfo = pbTempInfo;
			pcRecordImage->cBestSnapshot.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cBestSnapshot.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cBestSnapshot.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			if (cImgInfo.nFaceCount != 0)
			{
				int iFlag = 0;
			}
			memcpy(pcRecordImage->cBestSnapshot.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_LAST_SNAPSHOT:
			pcRecordImage->cLastSnapshot.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cLastSnapshot.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cLastSnapshot.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.top = cImgInfo.rcPlate.top ; 
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.left = cImgInfo.rcPlate.left ; 
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.bottom = cImgInfo.rcPlate.bottom ; 
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.right = cImgInfo.rcPlate.right ;
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cLastSnapshot.pbImgData = pbTempData;
			pcRecordImage->cLastSnapshot.pbImgInfo = pbTempInfo;
			pcRecordImage->cLastSnapshot.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cLastSnapshot.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cLastSnapshot.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			if (cImgInfo.nFaceCount != 0)
			{
				int iFlag = 0;
			}
			memcpy(pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_BEGIN_CAPTURE:
			pcRecordImage->cBeginCapture.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cBeginCapture.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cBeginCapture.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.top = cImgInfo.rcPlate.top; 
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.left = cImgInfo.rcPlate.left; 
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.bottom = cImgInfo.rcPlate.bottom; 
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.right = cImgInfo.rcPlate.right; 
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cBeginCapture.pbImgData = pbTempData;
			pcRecordImage->cBeginCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cBeginCapture.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cBeginCapture.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cBeginCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			memcpy(pcRecordImage->cBeginCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_BEST_CAPTURE:
			pcRecordImage->cBestCapture.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cBestCapture.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cBestCapture.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.top = cImgInfo.rcPlate.top; 
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.left = cImgInfo.rcPlate.left; 
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.bottom = cImgInfo.rcPlate.bottom; 
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.right = cImgInfo.rcPlate.right; 
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cBestCapture.pbImgData = pbTempData;
			pcRecordImage->cBestCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cBestCapture.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cBestCapture.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cBestCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			memcpy(pcRecordImage->cBestCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_LAST_CAPTURE:
			pcRecordImage->cLastCapture.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cLastCapture.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cLastCapture.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.top = cImgInfo.rcPlate.top; 
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.left = cImgInfo.rcPlate.left; 
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.bottom = cImgInfo.rcPlate.bottom; 
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.right = cImgInfo.rcPlate.right; 
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cLastCapture.pbImgData = pbTempData;
			pcRecordImage->cLastCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cLastCapture.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cLastCapture.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cLastCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			memcpy(pcRecordImage->cLastCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_SMALL_IMAGE:
			pcRecordImage->cPlatePicture.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cPlatePicture.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cPlatePicture.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cPlatePicture.pbImgData = pbTempData;
			pcRecordImage->cPlatePicture.pbImgInfo = pbTempInfo;
			pcRecordImage->cPlatePicture.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cPlatePicture.dwImgInfoLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_BIN_IMAGE:
			pcRecordImage->cPlateBinary.cImgInfo.dwCarID = cImgInfo.dwCarID;
			pcRecordImage->cPlateBinary.cImgInfo.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImage->cPlateBinary.cImgInfo.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMS = cImgInfo.dwTimeHigh;
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMS |= cImgInfo.dwTimeLow;
			pcRecordImage->cPlateBinary.pbImgData = pbTempData;
			pcRecordImage->cPlateBinary.pbImgInfo = pbTempInfo;
			pcRecordImage->cPlateBinary.dwImgDataLen = dwImgDataLen;
			pcRecordImage->cPlateBinary.dwImgInfoLen = dwImgInfoLen;
			break;
		default:
			break;
		}
		pbTempData += dwImgDataLen;
		dwRemainLen -= (8+dwImgInfoLen+dwImgDataLen);
	}
	return S_OK;
}

inline DWORD GetValueFromAttribute(const char* szString)
{
	DWORD dwValue = 0;
	if (szString)
	{
		//return atoi(szString);
		if (1 == sscanf(szString, "%u", &dwValue))
		{
			return dwValue;
		}
	}
	return 0;
}

HRESULT HvGetRecordImage_Mercury(const char* szAppendInfo, PBYTE pbRecordData, DWORD dwRecordDataLen, RECORD_IMAGE_GROUP_EX* pcRecordImage)
{
	IMAGE_INFO_MERCURY cImgInfo;
	//DWORD dwImgInfoLen;
	//DWORD dwImgDataLen;
	PBYTE pbTempData = pbRecordData;
	//PBYTE pbTempInfo = NULL;
	DWORD dwRemainLen = dwRecordDataLen;
	if (pcRecordImage)
	{
		memset(pcRecordImage, 0, sizeof(RECORD_IMAGE_GROUP_EX));
	}
	else
	{
		return E_FAIL;
	}
	if(!szAppendInfo || !pbRecordData || dwRecordDataLen <= 0)
	{
		return E_FAIL;
	}

	// 从szAppendInfo中解出ResultExtInfo、Result结点
	TiXmlElement* pExtInfoEle = NULL;
	TiXmlElement* pResultEle = NULL;
	TiXmlDocument cXmlDoc;
	if (cXmlDoc.Parse(szAppendInfo))
	{
		TiXmlElement* pEleRoot = cXmlDoc.RootElement();
		if (NULL == pEleRoot)
		{
			WrightLogEx("test IP", "HvGetRecordImage_Mercury pEleRoot == NULL");
			return E_FAIL; 
		}

		pExtInfoEle = pEleRoot->FirstChildElement("ResultExtInfo");
		if (NULL == pExtInfoEle)
		{
			WrightLogEx("test IP", "HvGetRecordImage_Mercury pExtInfoEle == NULL");
			return E_FAIL;
		}

		TiXmlElement* pResultSetEle = pEleRoot->FirstChildElement("ResultSet");
		if (NULL == pResultSetEle)
		{
			WrightLogEx("test IP", "HvGetRecordImage_Mercury pResultSetEle == NULL");
			return E_FAIL;
		}

		pResultEle = pResultSetEle->FirstChildElement("Result");
		if (NULL == pResultEle)
		{
			WrightLogEx("test IP", "HvGetRecordImage_Mercury pResultEle == NULL");
			return E_FAIL;
		}
	}
	else
	{
		WrightLogEx("test IP", "HvGetRecordImage_Mercury szAppendInfo can't be parsed");
		return E_FAIL;
	}

	// 解析pExtInfoEle中各图片的信息
	TiXmlElement * pBestSnapShotEle = pExtInfoEle->FirstChildElement("Image0");
	TiXmlElement * pLastSnapShotEle = pExtInfoEle->FirstChildElement("Image1");
	TiXmlElement * pBeginCaptureEle = pExtInfoEle->FirstChildElement("Image2");
	TiXmlElement * pBestCaptureEle  = pExtInfoEle->FirstChildElement("Image3");
	TiXmlElement * pLastCaptureEle  = pExtInfoEle->FirstChildElement("Image4");
	TiXmlElement * pPlateEle         = pExtInfoEle->FirstChildElement("Image5");
	TiXmlElement * pPlateBinEle      = pExtInfoEle->FirstChildElement("Image6");
	TiXmlElement * pFaceRectElement		 = NULL;
	TiXmlElement * pIllegalVideoElement	= pExtInfoEle->FirstChildElement("Video");

	// 解析CarID
	TiXmlElement* pCarIDEle = pResultEle->FirstChildElement("CarID");
	if (NULL == pCarIDEle)
	{
		WrightLogEx("test IP", "HvGetRecordImage_Mercury pCarIDEle == NULL");
		return E_FAIL;
	}
	const char * szCarID = pCarIDEle->Attribute("value");
	if (NULL == szCarID)
	{
		WrightLogEx("test IP", "HvGetRecordImage_Mercury szCarID == NULL");
		return E_FAIL;
	}
	DWORD dwCarID = atoi(szCarID);

	char szImages[1024];
	memset(szImages, 0, sizeof(szImages));
	if (pBestSnapShotEle) sprintf(szImages, "%s %d", szImages, 0);
	if (pLastSnapShotEle) sprintf(szImages, "%s %d", szImages, 1);
	if (pBeginCaptureEle) sprintf(szImages, "%s %d", szImages, 2);
	if (pBestCaptureEle) sprintf(szImages, "%s %d", szImages, 3);
	if (pLastCaptureEle) sprintf(szImages, "%s %d", szImages, 4);
	if (pPlateEle) sprintf(szImages, "%s %d", szImages, 5);
	if (pPlateBinEle) sprintf(szImages, "%s %d", szImages, 6);
	
	
	TiXmlElement* pPlateNameEle = pResultEle->FirstChildElement("PlateName");
	if (NULL == pPlateNameEle)
	{
		WrightLogEx("test IP", "HvGetRecordImage_Mercury pPlateNameEle == NULL");
		return E_FAIL;
	}
	const char * szPlateName = pPlateNameEle->GetText();
	if (NULL == szPlateName)
	{
		WrightLogEx("test IP", "HvGetRecordImage_Mercury szPlateName == NULL");
		return E_FAIL;
	}
	char szLog[1024];
	sprintf(szLog, "Received record[%d] %s with images[%s] in XML", dwCarID, szPlateName, szImages);
	WrightLogEx("Test IP", szLog);

	memset(szImages, 0, sizeof(szImages));
	while(dwRemainLen > 0)
	{

		memcpy(&cImgInfo, pbTempData, sizeof(cImgInfo));
		pbTempData += sizeof(cImgInfo);
		if(cImgInfo.dwImgLength > dwRemainLen)
		{
			sprintf(szLog, "HvGetRecordImage_Mercury cImgInfo.dwImgLength[%d] > dwRemainLen[%d]", cImgInfo.dwImgLength, dwRemainLen);
			WrightLogEx("test IP", szLog);
			return E_FAIL;
		}

		//int iBuffLen;
		//iBuffLen = (sizeof(IMAGE_INFO) < dwImgInfoLen) ? sizeof(IMAGE_INFO) : dwImgInfoLen;
		//memcpy(&cImgInfo, pbTempData, iBuffLen);
		//pbTempInfo = pbTempData;
		//pbTempData += dwImgInfoLen;

		//memcpy(&dwImgDataLen, pbTempData, 4);
		//pbTempData += 4;
		//if(dwImgDataLen > dwRemainLen)
		//{
		//	return E_FAIL;
		//}
		//cImgInfo.rcFacePos[0].bottom = 10;cImgInfo.rcFacePos[0].top = 10;cImgInfo.rcFacePos[0].right = 10;cImgInfo.rcFacePos[0].left = 10;
		switch(cImgInfo.dwImgType)
		{
		case RECORD_IMAGE_BEST_SNAPSHOT:
			sprintf(szImages, "%s %d-%d", szImages, 0, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pBestSnapShotEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pBestSnapShotEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cBestSnapshot.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cBestSnapshot.cImgInfo.dwHeight = GetValueFromAttribute(pBestSnapShotEle->Attribute("Height"));
			pcRecordImage->cBestSnapshot.cImgInfo.dwWidth = GetValueFromAttribute(pBestSnapShotEle->Attribute("Width"));
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.top = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosTop")); 
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.left = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosLeft"));
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosBottom")); 
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.right = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosRight"));  
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMS = GetValueFromAttribute(pBestSnapShotEle->Attribute("TimeHigh"));  
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pBestSnapShotEle->Attribute("TimeLow")); 
			pcRecordImage->cBestSnapshot.pbImgData = pbTempData;
			//pcRecordImage->cBestSnapshot.pbImgInfo = pbTempInfo;
			pcRecordImage->cBestSnapshot.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cBestSnapshot.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cBestSnapshot.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//if (cImgInfo.nFaceCount != 0)
			//{
			//	int iFlag = 0;
			//}
			//memcpy(pcRecordImage->cBestSnapshot.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_LAST_SNAPSHOT:
			sprintf(szImages, "%s %d-%d", szImages, 1, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pLastSnapShotEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pLastSnapShotEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cLastSnapshot.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cLastSnapshot.cImgInfo.dwHeight = GetValueFromAttribute(pLastSnapShotEle->Attribute("Height"));
			pcRecordImage->cLastSnapshot.cImgInfo.dwWidth = GetValueFromAttribute(pLastSnapShotEle->Attribute("Width"));
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.top = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosTop")); 
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.left = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosLeft"));
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosBottom")); 
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.right = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosRight"));  
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMS = GetValueFromAttribute(pLastSnapShotEle->Attribute("TimeHigh"));  
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pLastSnapShotEle->Attribute("TimeLow")); 
			pcRecordImage->cLastSnapshot.pbImgData = pbTempData;
			//pcRecordImage->cLastSnapshot.pbImgInfo = pbTempInfo;
			pcRecordImage->cLastSnapshot.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cLastSnapshot.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cLastSnapshot.cImgInfo.nFaceCount = min( GetValueFromAttribute(pLastSnapShotEle->Attribute("FaceCount")), sizeof(pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos) / sizeof(RECT) ); // 只有这张图有人脸坐标
			pFaceRectElement = pLastSnapShotEle->FirstChildElement("FaceInfo");

			if( NULL != pFaceRectElement )
			{
				int iFaceCount = GetValueFromAttribute(pFaceRectElement->Attribute("Count"));
				for (int i = 0; i < iFaceCount; i++)
				{
					char szFaceEleName[16];
					sprintf(szFaceEleName, "Face%d", i);
					TiXmlElement* pFaceEle = pFaceRectElement->FirstChildElement(szFaceEleName);
					if (NULL == pFaceEle)
					{
						char szLog[256];
						sprintf(szLog, "HvGetRecordImage_Mercury pFaceEle[%d] == NULL", i);
						WrightLogEx("test IP", szLog);
						break;
					}

					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].bottom = GetValueFromAttribute(pFaceEle->Attribute("Bottom"));
					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].left = GetValueFromAttribute(pFaceEle->Attribute("Left"));
					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].right = GetValueFromAttribute(pFaceEle->Attribute("Right"));
					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].top = GetValueFromAttribute(pFaceEle->Attribute("Top"));
				}
			}
			else
			{
				WrightLogEx("test IP", " Not Find  FaceInfo\n");
			}

			//if (cImgInfo.nFaceCount != 0)
			//{
			//	int iFlag = 0;
			//}
			//memcpy(pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_BEGIN_CAPTURE:
			sprintf(szImages, "%s %d-%d", szImages, 2, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pBeginCaptureEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pBeginCaptureEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cBeginCapture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cBeginCapture.cImgInfo.dwHeight = GetValueFromAttribute(pBeginCaptureEle->Attribute("Height"));
			pcRecordImage->cBeginCapture.cImgInfo.dwWidth = GetValueFromAttribute(pBeginCaptureEle->Attribute("Width"));
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.top = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosTop")); 
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.left = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosLeft"));
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosBottom")); 
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.right = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosRight"));  
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMS = GetValueFromAttribute(pBeginCaptureEle->Attribute("TimeHigh"));  
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pBeginCaptureEle->Attribute("TimeLow")); 
			pcRecordImage->cBeginCapture.pbImgData = pbTempData;
			//pcRecordImage->cBeginCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cBeginCapture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cBeginCapture.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cBeginCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//memcpy(pcRecordImage->cBeginCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_BEST_CAPTURE:
			sprintf(szImages, "%s %d-%d", szImages, 3, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pBestCaptureEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pBestCaptureEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cBestCapture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cBestCapture.cImgInfo.dwHeight = GetValueFromAttribute(pBestCaptureEle->Attribute("Height"));
			pcRecordImage->cBestCapture.cImgInfo.dwWidth = GetValueFromAttribute(pBestCaptureEle->Attribute("Width"));
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.top = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosTop")); 
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.left = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosLeft"));
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosBottom")); 
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.right = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosRight"));  
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMS = GetValueFromAttribute(pBestCaptureEle->Attribute("TimeHigh"));  
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pBestCaptureEle->Attribute("TimeLow")); 
			pcRecordImage->cBestCapture.pbImgData = pbTempData;
			//pcRecordImage->cBestCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cBestCapture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cBestCapture.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cBestCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//memcpy(pcRecordImage->cBestCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_LAST_CAPTURE:
			sprintf(szImages, "%s %d-%d", szImages, 4, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pLastCaptureEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pLastCaptureEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cLastCapture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cLastCapture.cImgInfo.dwHeight = GetValueFromAttribute(pLastCaptureEle->Attribute("Height"));
			pcRecordImage->cLastCapture.cImgInfo.dwWidth = GetValueFromAttribute(pLastCaptureEle->Attribute("Width"));
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.top = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosTop")); 
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.left = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosLeft"));
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosBottom")); 
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.right = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosRight"));  
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMS = GetValueFromAttribute(pLastCaptureEle->Attribute("TimeHigh"));  
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pLastCaptureEle->Attribute("TimeLow")); 
			pcRecordImage->cLastCapture.pbImgData = pbTempData;
			//pcRecordImage->cLastCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cLastCapture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cLastCapture.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cLastCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//memcpy(pcRecordImage->cLastCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_SMALL_IMAGE:
			sprintf(szImages, "%s %d-%d", szImages, 5, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pPlateEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pPlateEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cPlatePicture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cPlatePicture.cImgInfo.dwHeight = GetValueFromAttribute(pPlateEle->Attribute("Height"));
			pcRecordImage->cPlatePicture.cImgInfo.dwWidth = GetValueFromAttribute(pPlateEle->Attribute("Width"));
			sprintf(szLog, "HvGetRecordImage_Mercury 小图宽[%d] 高[%d]", pcRecordImage->cPlatePicture.cImgInfo.dwWidth, pcRecordImage->cPlatePicture.cImgInfo.dwHeight);
			WrightLogEx("test IP", szLog);
			//// 把YUV数据写到文件中
			//char szSmallFileName[128];
			//sprintf(szSmallFileName, "D:\\Result\\YUV-%4d-w%d-h%d.yuv", i, pcRecordImage->cPlatePicture.cImgInfo.dwWidth, pcRecordImage->cPlatePicture.cImgInfo.dwHeight);
			//fp = fopen(szSmallFileName, "a+");
			//if(fp)
			//{
			//	fwrite(pbTempData, cImgInfo.dwImgLength, 1, fp);
			//	fclose(fp);
			//}
			//i++;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMS = GetValueFromAttribute(pPlateEle->Attribute("TimeHigh"));  
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pPlateEle->Attribute("TimeLow")); 
			pcRecordImage->cPlatePicture.pbImgData = pbTempData;
			//pcRecordImage->cPlatePicture.pbImgInfo = pbTempInfo;
			pcRecordImage->cPlatePicture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cPlatePicture.dwImgInfoLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_BIN_IMAGE:
			sprintf(szImages, "%s %d-%d", szImages, 6, cImgInfo.dwImgLength);
			WrightLogEx("test IP", szImages);
			if (NULL == pPlateBinEle)
			{
				WrightLogEx("test IP", "HvGetRecordImage_Mercury pPlateBinEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cPlateBinary.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cPlateBinary.cImgInfo.dwHeight = GetValueFromAttribute(pPlateBinEle->Attribute("Height"));
			pcRecordImage->cPlateBinary.cImgInfo.dwWidth = GetValueFromAttribute(pPlateBinEle->Attribute("Width"));
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMS = GetValueFromAttribute(pPlateBinEle->Attribute("TimeHigh"));  
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMS <<= 32;
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMS |= GetValueFromAttribute(pPlateBinEle->Attribute("TimeLow")); 
			pcRecordImage->cPlateBinary.pbImgData = pbTempData;
			pcRecordImage->cPlateBinary.pbImgData = pbTempData;
			//pcRecordImage->cPlateBinary.pbImgInfo = pbTempInfo;
			pcRecordImage->cPlateBinary.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cPlateBinary.dwImgInfoLen = dwImgInfoLen;
			break;
		case PACKET_RECORD_VIDEO_ILLEGAL:
			if (pIllegalVideoElement != NULL)
			{
				pcRecordImage->cIllegalVideo.dwCarID = dwCarID;
				pcRecordImage->cIllegalVideo.dw64TimeMS = GetValueFromAttribute(pIllegalVideoElement->Attribute("TimeHigh"));
				pcRecordImage->cIllegalVideo.dw64TimeMS <<= 32;
				pcRecordImage->cIllegalVideo.dw64TimeMS |= GetValueFromAttribute(pIllegalVideoElement->Attribute("TimeLow"));
				pcRecordImage->cIllegalVideo.dwHeight = GetValueFromAttribute(pIllegalVideoElement->Attribute("Height"));
				pcRecordImage->cIllegalVideo.dwWidth = GetValueFromAttribute(pIllegalVideoElement->Attribute("Width"));
				pcRecordImage->cIllegalVideo.dwVideoDataLen = cImgInfo.dwImgLength;
				pcRecordImage->cIllegalVideo.dwVideoInfoLen = 0;
				pcRecordImage->cIllegalVideo.pbVideoData = pbTempData;
				pcRecordImage->cIllegalVideo.pbVideoInfo = NULL;
				
			}
			break;
		default:
			break;
		}
		pbTempData += cImgInfo.dwImgLength;
		dwRemainLen -= (sizeof(cImgInfo) + cImgInfo.dwImgLength);
		sprintf(szImages, "%sR%d", szImages, dwRemainLen);
	}

	sprintf(szLog, "Received record[%d] %s with images[%s] in pbData(length %d)", dwCarID, szPlateName, szImages, dwRecordDataLen);
	WrightLogEx("Test IP", szLog);

	return S_OK;
}
HRESULT HvEnhanceTrafficLight(PBYTE pbSrcImg, DWORD dwiSrcImgDataLen, int iRedLightCount,
							  PBYTE pbRedLightPos, PBYTE pbDestImgBuf, DWORD& dwDestImgBufLen,
							  INT iBrightness, INT iHueThreshold, INT iCompressRate)
{
	if(pbSrcImg == NULL || dwiSrcImgDataLen <= 0) return E_FAIL;
	if(pbDestImgBuf == NULL || dwDestImgBufLen <= 0) return E_FAIL;
	if(pbRedLightPos == NULL && sizeof(pbRedLightPos)/sizeof(RECT) < iRedLightCount)return E_FAIL;

	RECT* pcRect = new RECT[iRedLightCount*sizeof(RECT)];
	if(pcRect == NULL)return E_FAIL;
	memcpy(pcRect, pbRedLightPos, iRedLightCount*sizeof(RECT));
	if(iBrightness < -255) iBrightness = -255;
	if(iBrightness > 255) iBrightness = 255;

	CxImage imgSrc(pbSrcImg, dwiSrcImgDataLen, CXIMAGE_FORMAT_UNKNOWN);
	if(!imgSrc.IsValid()) return E_FAIL;
	int iWidth = imgSrc.GetWidth();
	int iHeight = imgSrc.GetHeight();

	if(iRedLightCount < 1)
	{
		pcRect[0].left = 0;
		pcRect[0].top = 0;
		pcRect[0].right = iWidth;
		pcRect[0].bottom = iHeight;
		iRedLightCount = 1;
	}

	for(int i=0; i<iRedLightCount; i++)
	{
		RECT cTempRect;
		LONG lTemp;
		cTempRect.left = pcRect[i].left;
		cTempRect.top = pcRect[i].top;
		cTempRect.right = pcRect[i].right;
		cTempRect.bottom = pcRect[i].bottom;

		if(cTempRect.left > cTempRect.right)
		{
			lTemp = cTempRect.left;
			cTempRect.left = cTempRect.right;
			cTempRect.right = lTemp;
		}

		if(cTempRect.top > cTempRect.bottom)
		{
			lTemp = cTempRect.top;
			cTempRect.top = cTempRect.bottom;
			cTempRect.bottom = lTemp;
		}

		int iRectWidht, iRectHeight;
		iRectWidht = cTempRect.right - cTempRect.left;
		iRectHeight = cTempRect.bottom - cTempRect.top;
		if(iRectWidht <= 0 || iRectHeight <= 0)
		{
			continue;
		}
		CxImage imgCrop, imgH, imgS, imgV;
		if(!imgSrc.Crop(cTempRect, &imgCrop))
		{
			continue;
		}
		if(!imgCrop.SplitHSL(&imgH, &imgS, &imgV))
		{
			continue;
		}

		RGBQUAD h, s, v;
		int iSumH = 0, iSumV = 0, iSumS = 0;
		int iPixelCount(0);

		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				h = imgH.GetPixelColor(x, y);
				s = imgS.GetPixelColor(x, y);
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue > 40 && v.rgbBlue < 230)
				{
					iSumH += h.rgbBlue;
					iSumS += s.rgbBlue;
					iSumV += v.rgbBlue;
					iPixelCount++;
				}
			}
		}
		if(iPixelCount == 0)
		{
			continue;
		}
		int iAvgH = iSumH / iPixelCount;
		if(iAvgH > iHueThreshold)
		{
			continue;
		}
		int iAvgV = iSumV / iPixelCount;
		int iAvgS = iSumS / iPixelCount;

		iSumV = 0;
		iPixelCount = 0;
		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue >= iAvgV)
				{
					iSumV += v.rgbBlue;
					iPixelCount++;
				}
			}
		}
		int iHeightAvgV = iAvgV;
		if(iPixelCount != 0) iHeightAvgV = iSumV / iPixelCount;

		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				h = imgH.GetPixelColor(x, y);
				s = imgS.GetPixelColor(x, y);
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue <= 20 || s.rgbBlue < 40 || (h.rgbBlue > 60 && h.rgbBlue < 230)) continue;
				if(v.rgbBlue >= iHeightAvgV)
				{
					v.rgbRed = (BYTE)(v.rgbBlue - (v.rgbBlue-iHeightAvgV)*0.4);
					v.rgbGreen = v.rgbRed;
					v.rgbBlue = v.rgbRed;
					imgV.SetPixelColor(x, y, v);

					s.rgbRed = iAvgS;
					s.rgbGreen = s.rgbRed;
					s.rgbBlue = s.rgbRed;
					imgS.SetPixelColor(x, y, s);
				}
				h.rgbBlue = 0;
				h.rgbGreen = 0;
				h.rgbRed = 0;
				imgH.SetPixelColor(x, y, h);
			}
		}
		imgCrop.Combine(&imgH, &imgS, &imgV, NULL, 1);
		imgSrc.Mix(imgCrop, CxImage::OpDstCopy, -cTempRect.left, cTempRect.bottom-iHeight);
	}
	delete[] pcRect;
	if(iBrightness)
	{
		imgSrc.Light(iBrightness);
	}
	long lsize = 0;
	BYTE* pbuffer = NULL;
	imgSrc.SetJpegQuality(iCompressRate);
	if(!imgSrc.Encode(pbuffer, lsize, CXIMAGE_FORMAT_JPG)) return E_FAIL;

	if(lsize > (int)dwDestImgBufLen)
	{
		imgSrc.FreeMemory(pbuffer);
		dwDestImgBufLen = lsize;
		return E_FAIL;
	}
	memcpy(pbDestImgBuf, pbuffer, lsize);
	dwDestImgBufLen = lsize;
	imgSrc.FreeMemory(pbuffer);
	return S_OK;
}

int BuildHvCmdXml(
    char* pXmlBuf,
    char* pCmdName,
    int iArgCount,
    const char rgszName[][IVN_MAX_SIZE],
    const char rgszValue[][IVN_MAX_SIZE]
)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "2.0");

	doc.LinkEndChild(pXmlRootElement);

	TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
	pXmlRootElement->LinkEndChild(pXmlElementCmd);

	TiXmlText *pXmlCmdText = new TiXmlText(pCmdName);
	pXmlElementCmd->LinkEndChild(pXmlCmdText);

	if (iArgCount != 0)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdArg");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		for (int i=0; i<iArgCount; i++)
		{
			TiXmlElement *pXmlElementID = new TiXmlElement("Value");
			pXmlElementCmd->LinkEndChild(pXmlElementID);

			pXmlElementID->SetAttribute("id", rgszName[i]);

			if (0 == strcmp(rgszValue[i] ,""))
				continue;

			TiXmlText *pXmlIDText = new TiXmlText(rgszValue[i]);
			pXmlElementID->LinkEndChild(pXmlIDText);
		}
	}


	TiXmlPrinter printer;
	doc.Accept(&printer);

	int len = (int)printer.Size();
	memcpy(pXmlBuf, printer.CStr(), len);
	pXmlBuf[len] = '\0';

	return len;
}


HRESULT GetParamStringFromXml(
							  const TiXmlElement* pCmdArgElement,
							  const char *pszParamName,
							  char *pszParamValue,
							  int nParamValueSize
							  )
{
	if ( pCmdArgElement == NULL
		|| pszParamName == NULL
		|| pszParamValue == NULL
		|| nParamValueSize <= 0 )
	{
		return E_POINTER;
	}

	bool fFind = false;
	const TiXmlElement* pValueElement = pCmdArgElement->FirstChildElement("Value");
	while ( pValueElement )
	{
		if ( 0 == strcmp(pszParamName, pValueElement->Attribute("id")) )
		{
			if ( strlen(pValueElement->GetText()) < (size_t)nParamValueSize )
			{
				fFind = true;
				strcpy(pszParamValue, pValueElement->GetText());
				break;
			}
		}
		pValueElement = pValueElement->NextSiblingElement();
	}

	return fFind ? S_OK : E_FAIL;
}

HRESULT GetParamIntFromXml(
						   const TiXmlElement* pCmdArgElement,
						   const char *pszParamName,
						   int *pnParamValue
						   )
{
	if (pCmdArgElement == NULL || pszParamName == NULL || pnParamValue == NULL)
	{
		return E_POINTER;
	}

	bool fFind = false;
	const TiXmlElement* pValueElement = pCmdArgElement->FirstChildElement("Value");
	while ( pValueElement )
	{
		if ( 0 == strcmp(pszParamName, pValueElement->Attribute("id")) )
		{
			fFind = true;
			(*pnParamValue) = atoi(pValueElement->GetText());
			break;
		}
		pValueElement = pValueElement->NextSiblingElement();
	}

	return fFind ? S_OK : E_FAIL;
}

bool ExecXmlExtCmd(char* szXmlCmd, char* szRetBuf,
					 int& iBufLen, SOCKET sktSend)
{
	if ( strlen(szXmlCmd) > (1024 << 4) )
	{
		return false;
	}
	const int iMaxLen = (1024 << 10);
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
	{
		return false;
	}
	memset(pszTmpXmlBuf, 0, iMaxLen);
	bool fIsTempSocket = true;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;
	if(sktSend == INVALID_SOCKET)
	{
		return false;
	}
	else
	{
		hSocketCmd = sktSend;
		fIsTempSocket = false;
	}
	bool fIsOk = false;
	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
	int isend0 = send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) ;
	if ( sizeof(cCmdHeader) != isend0)
	{
		if(fIsTempSocket)
		{
			ForceCloseSocket(hSocketCmd);
		}
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return false;
	}

	int isend = send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize, 0);
	if ( cCmdHeader.dwInfoSize !=  isend)
	{
		int iTemp = WSAGetLastError();
		if(fIsTempSocket)
		{
			ForceCloseSocket(hSocketCmd);
		}
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return false;
	}
	if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
	{
		if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult 
			&& 0 < cCmdRespond.dwInfoSize )
		{
			int iRecvLen = RecvAll(hSocketCmd, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
			if ( cCmdRespond.dwInfoSize ==  iRecvLen)
			{
				if ( iBufLen > (int)cCmdRespond.dwInfoSize )
				{
					memcpy(szRetBuf, pszTmpXmlBuf, cCmdRespond.dwInfoSize);
					memset(szRetBuf+cCmdRespond.dwInfoSize, 0, 1);
					iBufLen = cCmdRespond.dwInfoSize;
					if(fIsTempSocket)
					{
						ForceCloseSocket(hSocketCmd);
					}
					SAFE_DELETE_ARG(pszTmpXmlBuf);
					return true;
				}
				else
				{
					iBufLen = cCmdRespond.dwInfoSize;
					if(fIsTempSocket)
					{
						ForceCloseSocket(hSocketCmd);
					}
					SAFE_DELETE_ARG(pszTmpXmlBuf);
					return false;
				}
			}
			else
			{
				if(fIsTempSocket)
				{
					ForceCloseSocket(hSocketCmd);
				}
				SAFE_DELETE_ARG(pszTmpXmlBuf);
				return false;
			}
		}
		fIsOk = true;
	}

	if(fIsTempSocket)
	{
		ForceCloseSocket(hSocketCmd);
	}
	SAFE_DELETE_ARG(pszTmpXmlBuf);
	return fIsOk;
}

HRESULT HvSendXmlCmd(LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT* piRetLen, DWORD dwXmlVersion, SOCKET sktSend)
{
	if (NULL == szCmd || NULL == szRetBuf || iBufLen <= 0 )
	{
		return E_FAIL;
	}
	const int iMaxLen = (1024 << 4);
	char* pszXmlBuf = new char[iMaxLen];  // 16KB
	int iXmlBufLen = iMaxLen;
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	memset(pszXmlBuf, 0, iMaxLen);
	TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // 检查szCmd是否是XML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(PROTOCOL_VERSION_2 , szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
		szCmd = (char*)pszXmlBuf;
	}
	bool fRet = ExecXmlExtCmd((char*)szCmd, (char*)szRetBuf, iBufLen, sktSend);
	if ( piRetLen )
	{
		*piRetLen = iBufLen;
	}
	SAFE_DELETE_ARG(pszXmlBuf);
	return (true == fRet) ? S_OK : E_FAIL;
}





HRESULT HvSetRecvTimeOut( const SOCKET& hSocket, int iMS )
{
	struct timeval uTime;
	uTime.tv_sec = iMS / 1000;
	uTime.tv_usec = (iMS % 1000) * 1000;
	int iRet = setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&uTime, sizeof(timeval));
	return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvSetSendTimeOut( const SOCKET& hSocket, int iMS )
{
	struct timeval uTime;
	uTime.tv_sec = iMS / 1000;
	uTime.tv_usec = (iMS % 1000) * 1000;
	int iRet = setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&uTime, sizeof(timeval));
	return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvListen( const SOCKET& hSocket, int nPort, int backlog )
{
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(nPort);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	int iOpt = 1;
	int iOptLen = sizeof(iOpt);
	setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);

	int iRet = -1;
	iRet = bind(hSocket, (sockaddr*)&sa, sizeof(struct sockaddr));
	if ( 0 != iRet ) return E_FAIL;

	iRet = listen(hSocket, backlog);
	return (0 == iRet) ? S_OK : E_FAIL;
}

HRESULT HvAccept( const SOCKET& hSocket, SOCKET& hNewSocket, int iTimeout/*=-1*/ )
{
	if (INVALID_SOCKET == hSocket)	return E_INVALIDARG;

	struct sockaddr addr;
	int addrlen = sizeof(struct sockaddr);

	if ( -1 == iTimeout )
	{
		hNewSocket = accept(hSocket, &addr, &addrlen);
		return (INVALID_SOCKET!=hNewSocket) ? S_OK : E_FAIL;
	}

	HRESULT hr = E_FAIL;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(hSocket, &r);

	struct timeval timeout;
	timeout.tv_sec = iTimeout / 1000;
	timeout.tv_usec = (iTimeout % 1000) * 1000;

	//int ret = select((int)(hSocket+1), &r, NULL, NULL, &timeout);
	int ret = select(0, &r, NULL, NULL, &timeout);
	if (ret == 1)
	{
		hNewSocket = accept(hSocket, &addr, &addrlen);
		hr = ( INVALID_SOCKET != hNewSocket ) ? S_OK : E_FAIL;
	}
	else if (ret == 0)
	{
		hr = S_FALSE;
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

SOCKET HvCreateSocket( int af/*=AF_INET*/, int type/*=SOCK_STREAM*/, int protocol/*=0*/ )
{
	return socket(af, type, protocol);
}

char* stradd( char* str, char c )
{
	int n = strlen( str) ;
	str[n] = c;
	str[n+1] = 0;
	return str;
}

HRESULT ZBase64Encode( unsigned char* pSrcData, int nSrcByte ,  char* pDestData ,int* pnDestByte )
{
	if( NULL == pSrcData || nSrcByte <= 0
		|| NULL == pDestData || *pnDestByte <= 0 )
	{
		return E_FAIL;
	}

	//初始化
	pDestData[0]=0;
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	unsigned char Tmp[4]={0};
	int LineLength=0;
	int EncodeByte = 0;
	HRESULT hr = S_OK;
	for(int i=0;i<(int)(nSrcByte / 3);i++)
	{
		if( EncodeByte + 4 >= *pnDestByte )
		{
			hr = E_FAIL;
			break;
		}
		Tmp[1] = *pSrcData++;
		Tmp[2] = *pSrcData++;
		Tmp[3] = *pSrcData++;
		stradd( pDestData , EncodeTable[Tmp[1] >> 2]);
		stradd( pDestData , EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F]);
		stradd( pDestData , EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F]);
		stradd( pDestData , EncodeTable[Tmp[3] & 0x3F]);
		EncodeByte+=4;

		if(LineLength+=4,LineLength==76)
		{
			if( EncodeByte + 2 >= *pnDestByte )
			{
				hr = E_FAIL;
				break;
			}
			strcat( pDestData , "\r\n");
			LineLength=0;
			EncodeByte+=2;
		}
	}
	if( S_OK != hr || EncodeByte + 4 >= *pnDestByte )
	{
		return hr;
	}

	//对剩余数据进行编码
	int Mod=nSrcByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *pSrcData++;
		stradd( pDestData , EncodeTable[(Tmp[1] & 0xFC) >> 2]);
		stradd( pDestData , EncodeTable[((Tmp[1] & 0x03) << 4)]);
		strcat( pDestData , "==");
	}
	else if(Mod==2)
	{
		Tmp[1] = *pSrcData++;
		Tmp[2] = *pSrcData++;
		stradd( pDestData , EncodeTable[(Tmp[1] & 0xFC) >> 2]);
		stradd( pDestData , EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)]);
		stradd( pDestData , EncodeTable[((Tmp[2] & 0x0F) << 2)]);
		strcat( pDestData , "=");
	}
	EncodeByte+=4;
	return hr;
}

HRESULT ZBase64Decode(  unsigned char* pSrcData, int nSrcByte ,  char* pDestData ,int* pnDestByte
					  /*const char* Data,int DataByte, int& OutByte*/)
{
	if( NULL == pSrcData || nSrcByte <= 0
		|| NULL == pDestData || *pnDestByte <= 0 )
	{
		return E_FAIL;
	}

	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			62, // '+'
			0, 0, 0,
			63, // '/'
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
			0, 0, 0, 0, 0, 0, 0,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
			0, 0, 0, 0, 0, 0,
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	pDestData[0] = 0;
	int nValue;
	int i= 0;
	HRESULT hr = S_OK;
	int DecodeByte =0;
	while (i < nSrcByte)
	{
		if (*pSrcData != '\r' && *pSrcData!='\n')
		{
			if( DecodeByte+1> *pnDestByte ){hr = E_FAIL;break;}

			nValue = DecodeTable[*pSrcData++] << 18;
			nValue += DecodeTable[*pSrcData++] << 12;
			stradd( pDestData , (nValue & 0x00FF0000) >> 16);
			DecodeByte++;
			if (*pSrcData != '=')
			{
				if( DecodeByte+1> *pnDestByte ){hr = E_FAIL;break;}

				nValue += DecodeTable[*pSrcData++] << 6;
				stradd( pDestData , (nValue & 0x0000FF00) >> 8);
				DecodeByte++;
				if (*pSrcData != '=')
				{
					if( DecodeByte+1> *pnDestByte ){hr = E_FAIL;break;}

					nValue += DecodeTable[*pSrcData++];
					stradd( pDestData , nValue & 0x000000FF);
					DecodeByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			pSrcData++;
			i++;
		}
	}
	*pnDestByte = DecodeByte;
	return hr;
}

typedef struct _BroadcastInfo
{
	char szInfo[4096];
	int nInfoLen;
}BroadcastInfo;

void DeleteBroadcastInfoBuff(std::list<BroadcastInfo *> listBroadcast)
{
	 std::list<BroadcastInfo *>::iterator iter;
	 for(iter = listBroadcast. begin(); iter != listBroadcast. end(); iter ++)
	 {
		  BroadcastInfo *pBroadcastInfo = * iter;
		  if (NULL != pBroadcastInfo)
		  {
			  delete[] pBroadcastInfo;
			  pBroadcastInfo = NULL;
		  }

	 }
}

HRESULT SearchDeviceEx ( CDevBasicInfo* rgDevInfo , INT * pnDevCount )
{
	if ( NULL == rgDevInfo || NULL == pnDevCount )
	{
		return E_POINTER ;
	}

	memset( rgDevInfo , 0 , sizeof( CDevBasicInfo)*(*pnDevCount ) );
	INT nDeviceCount = 0;
	HRESULT hr = S_OK;

	char szHostName[1024] = {0};
	gethostname(szHostName, sizeof(szHostName));    //获得本地主机名
	PHOSTENT pHostInfo = gethostbyname(szHostName);//信息结构体
	std::list<BroadcastInfo *> listBroadcast;

	while(*(pHostInfo->h_addr_list) != NULL)
	{
		char* szIP = inet_ntoa(*(struct in_addr *) *(pHostInfo->h_addr_list));			// 获取单个网卡IP

		SOCKET sock = socket( AF_INET, SOCK_DGRAM , 0);
		if (sock == INVALID_SOCKET)
		{
			pHostInfo->h_addr_list ++;
			continue;
		}

		const BOOL on = 1;
		if (0 != setsockopt (sock, SOL_SOCKET, SO_BROADCAST , (const char*)&on , sizeof( on)))
		{
			goto NextLoop;
		}


		SOCKADDR_IN addrClient = {0};
		addrClient.sin_family = AF_INET;
		addrClient.sin_addr.s_addr = inet_addr(szIP);
		addrClient.sin_port = 0;
		if (0 != bind(sock, (sockaddr*)&addrClient, sizeof(addrClient)))
		{
			goto NextLoop;
		}

		struct sockaddr_in addr;
		memset(&addr , 0, sizeof( addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons( GETIP_COMMAND_PORT);
		addr.sin_addr.S_un.S_addr = 0xffffffff;

		struct timeval tv;
		for (int j =1; j<3; j++)
		{
			if (sendto (sock, ( char*)&GETIP_COMMAND , sizeof( GETIP_COMMAND), 0, (sockaddr *)&addr, sizeof(addr )) != sizeof( GETIP_COMMAND))
			{
				continue;
			}       

			int iCountOfUDP =0;
			while (true )
			{
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				fd_set rset ;
				FD_ZERO(&rset );
				FD_SET(sock , &rset);
				int iTemp = select( static_cast< int >(sock) + 1, & rset, NULL , NULL, & tv);
				if (iTemp == SOCKET_ERROR)
				{
					break;
				}
				if (iTemp == 0)
				{
					break;
				}
				int iLen = sizeof( addr);
				BroadcastInfo* pBroad = new BroadcastInfo;
				if (NULL == pBroad)
				{
					goto NextLoop;
				}

				pBroad->nInfoLen = recvfrom( sock, pBroad ->szInfo, 4096, 0, ( sockaddr*)&addr , &iLen);//int iRecvLen = recvfrom(sock, acBuf, sizeof(acBuf), 0, (sockaddr*)&addr, &iLen);
				listBroadcast.push_back(pBroad);
				iCountOfUDP++;
			}
			Sleep(1000);
			if (iCountOfUDP == 0)
			{
				break;
			}
		}

		std::list<BroadcastInfo *>::iterator iter;
		for(iter = listBroadcast. begin(); iter != listBroadcast. end(); iter ++)
		{
			BroadcastInfo *pBroadcastInfo = * iter;
			TiXmlDocument cXmlDoc ;
			if( cXmlDoc .Parse( pBroadcastInfo->szInfo ) )
			{
				// 新的搜索协议，（目前包括：水星、地球）
				const TiXmlElement * pRootElement = cXmlDoc.RootElement ();
				if (NULL == pRootElement)
				{
					continue;
				}
				const TiXmlElement * pCmdArgElement = pRootElement->FirstChildElement ("CmdName");
				if (NULL == pCmdArgElement)
				{
					continue;
				}
				const char * szIP = pCmdArgElement->Attribute ("IP");
				const char * szMask = pCmdArgElement->Attribute ("Mask");
				const char * szGateway = pCmdArgElement->Attribute ("Gateway");
				const char * szMac = pCmdArgElement->Attribute ("MAC");
				const char * szSN = pCmdArgElement->Attribute ("SN");
				const char * szMode = pCmdArgElement->Attribute ("Mode");
				const char * szDevType = pCmdArgElement->Attribute ("DevType");
				const char * szModelVersion = pCmdArgElement->Attribute ("ModelVersion");
				const char * szWorkMode = pCmdArgElement->Attribute ("WorkMode");
				const char * szDevVersion = pCmdArgElement->Attribute ("DevVersion");
				const char * szVersion = pCmdArgElement->Attribute ("Version");
				const char * szRemark = pCmdArgElement->Attribute ("Remark");

				if (NULL == szIP ||
					NULL == szMask ||
					NULL == szGateway ||
					NULL == szMac ||
					NULL == szSN ||
					NULL == szMode )
				{
					continue;
				}

				char szLog [128];
				sprintf(szLog , "SearchHVDeviceCount found a Mercury device with IP [%s]", szIP);
				WrightLogEx("TestIP", szLog );

				// 搜索原有设备列表，如果是新设备则加入设备列表
				int i = 0;
				for ( i = 0; i < nDeviceCount; i ++ )
				{
					if ( 0 == strcmp ( szMac , rgDevInfo[i ].szMac ) )
					{
						break;
					}
				}
				if ( i == nDeviceCount )
				{
					if ( i >= *pnDevCount ) //下标大于等于数组空间
					{
						hr = S_FALSE ;
						break;
					}

					strcpy( rgDevInfo [i]. szIP , szIP );
					strcpy( rgDevInfo [i]. szMac , szMac );
					strcpy( rgDevInfo [i]. szGateway , szGateway );
					strcpy( rgDevInfo [i]. szMask , szMask );
					strcpy( rgDevInfo [i]. szMode , szMode );
					strcpy( rgDevInfo [i]. szSN , szSN );

					if ( NULL != szModelVersion )
					{
						strcpy( rgDevInfo [i]. szModelVersion , szModelVersion   );
					}
					if ( NULL != szWorkMode )
					{
						strcpy( rgDevInfo [i]. szWorkMode , szWorkMode );
					}
					if ( NULL != szDevType )
					{
						strcpy( rgDevInfo [i]. szDevType , szDevType );
					}
					if ( NULL != szDevVersion)
					{
						strcpy( rgDevInfo [i]. szDevVersion , szDevVersion );
					}

					if ( NULL != szRemark)
					{
						strcpy( rgDevInfo [i]. szRemark , szRemark );
					}

					nDeviceCount++;

				}
			}
			else
			{
				// 1.0和2.0版协议
				if (pBroadcastInfo ->nInfoLen == 22 || pBroadcastInfo->nInfoLen == 22 + 32 || pBroadcastInfo->nInfoLen == 22 + 32 + 32)
				{
					DWORD32 dwCommand ;
					memcpy(&dwCommand , pBroadcastInfo-> szInfo, sizeof (dwCommand));
					if (dwCommand == GETIP_COMMAND)
					{
						if((pBroadcastInfo ->szInfo[9]&0xFF) == 0xB6 || (pBroadcastInfo->szInfo [9]&0xFF) == 0xB7
							|| (pBroadcastInfo->szInfo [9]&0xFF) == 0xB8 || (pBroadcastInfo->szInfo [9]&0xFF) == 0xB9)
						{
							DWORD dwMac [6];
							int i ;
							for (i = 0; i < nDeviceCount; i ++)
							{
								//若MAC地址一致的话
								sscanf(rgDevInfo [i]. szMac, "%x:%x:%x:%x:%x:%x", &dwMac [0], &dwMac[1], & dwMac[2], &dwMac [3], &dwMac[4], & dwMac[5]);
								if (memcmp( &(pBroadcastInfo->szInfo[4]), dwMac , 6) == 0)
								{
									break;
								}
							}
							if ( i == nDeviceCount )
							{
								if ( i >= *pnDevCount ) //下标大于等于数组空间
								{
									hr = S_FALSE ;
									break;
								}

								char* pMac = &pBroadcastInfo-> szInfo[4];
								sprintf(rgDevInfo [i]. szMac, "%x:%x:%x:%x:%x:%x", pMac [0]&0xFF, pMac[1]&0xFF, pMac[2]&0xFF, pMac [3]&0xFF, pMac[4]&0xFF, pMac[5]&0xFF);  // 不需逆序
								char* pIP = &pBroadcastInfo-> szInfo[10];
								sprintf(rgDevInfo [i]. szIP, "%d.%d.%d.%d", pIP [3] &0xFF, pIP[2]&0xFF, pIP[1]&0xFF, pIP [0]&0xFF);                                   // 逆序
								char* pMask = &pBroadcastInfo-> szInfo[14];
								sprintf(rgDevInfo [i]. szMask, "%d.%d.%d.%d", pMask [3]&0xFF, pMask[2]&0xFF, pMask[1]&0xFF, pMask [0]&0xFF);                         // 逆序
								char* pGateway = &pBroadcastInfo->szInfo [18];
								sprintf(rgDevInfo [i]. szGateway, "%d.%d.%d.%d", pGateway [3]&0xFF, pGateway[2]&0xFF, pGateway[1]&0xFF, pGateway [0]&0xFF);          // 逆序
								nDeviceCount++;
							}
						}
					}
				}
			}
		}

NextLoop:
		ForceCloseSocket(sock);
		DeleteBroadcastInfoBuff(listBroadcast);
		listBroadcast.clear();
		pHostInfo->h_addr_list ++;
		Sleep(100);
	}

	*pnDevCount = nDeviceCount ;
	printf("Count=%d ", nDeviceCount );
	return hr ;
}


HRESULT HvGetFirstParamNameFromXmlCmdAppendInfoMap(const CHAR* szCmdName, CHAR * szParamName, INT nParamNameLen)
{
	int iCmdIndex = SearchXmlCmdMapMercury(szCmdName);
	if (-1 == iCmdIndex)
	{ // 没找到对应的命令
		CHAR sLog[128];
		sprintf(sLog, "HvGetFirstParamNameFromXmlCmdAppendInfoMap can't find command [%s]", szCmdName);
		WrightLogEx("", sLog);
		return E_FAIL;
	}
	int iLen=strlen(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1);   //取得的参数
	if (iLen < nParamNameLen)
	{
		strcpy(szParamName, g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
	
}


HRESULT HvGetParamNameFromXmlCmdAppendInfoMap(const CHAR* szCmdName,  CXmlParseInfo* pXmlParseInfo, INT nXmlParseInfoNum)
{
	if (NULL == szCmdName || NULL == pXmlParseInfo)
	{
		return E_FAIL;
	}

	int iCmdIndex = SearchXmlCmdMapMercury(szCmdName);
	if (-1 == iCmdIndex)
	{ // 没找到对应的命令
		CHAR sLog[128];
		sprintf(sLog, "HvGetFirstParamNameFromXmlCmdAppendInfoMap can't find command [%s]", szCmdName);
		WrightLogEx("", sLog);
		return E_FAIL;
	}
	
	if (g_XmlCmdAppendInfoMap[iCmdIndex].iArrayColumn < nXmlParseInfoNum -1)
	{
		return E_FAIL;
	}

	for ( int i=0; i<nXmlParseInfoNum-1; i++)
	{
		int iLen=strlen(   *(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i )  );   //取得的参数
		if ( iLen <  128  )  //TODO(liyh) keyName的长度，当前为128
		{
			strcpy(pXmlParseInfo[i+1].szKeyName, *(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i )  );
		}
		else
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

// 封装黑白名单XML
int HvGetXmlForSetNameList( const CHAR* szValue, INT iValueLen, CHAR *szXmlBuff, INT iBuffLen)
{
	if ( szValue == NULL || szXmlBuff == NULL )
		return 0;

	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "3.0");

	doc.LinkEndChild(pXmlRootElement);

	TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
	pXmlRootElement->LinkEndChild(pXmlElementCmd);

	//命令名称
	TiXmlText *pXmlCmdText = new TiXmlText("SetNameList");
	pXmlElementCmd->LinkEndChild(pXmlCmdText);

	char szType[128] = {"STRING"};
	char szClass[128] = {"SETTER"};

	pXmlElementCmd->SetAttribute("Type", szType);
	pXmlElementCmd->SetAttribute("Class", szClass);
	pXmlElementCmd->SetAttribute("Value", szValue);

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuff, printer.CStr(), iLen);
	szXmlBuff[iLen] = '\0';
	return iLen;
}


// 解析黑白名单XML
bool HvGetNamListFromXML(const CHAR* szXmlBuff, CHAR* szWhiteNameList, CHAR* szBlackNameList)
{
	if (szXmlBuff == NULL || NULL == szWhiteNameList || NULL == szBlackNameList)
		return false;

	// 创建解析文档
	TiXmlDocument myDocument;
	if ( !myDocument.Parse(szXmlBuff))
	{
		OutputDebugString( " !myDocument.Parse(szXmlBuff) \n");
		return false;
	}

	// 文档节点
	TiXmlElement* pRoot = myDocument.RootElement();
	if (pRoot ==  NULL)
		return false;

	// 解析结果
	TiXmlElement* pEle = NULL;
	const char *chElement = NULL;

	for (pEle = pRoot->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
	{
		if ( strcmp(pEle->Value(), "CmdName") == 0)
		{
			chElement = pEle->Attribute("RetMsg");
			
			if ( strcmp(chElement, "") == 0 )
			{
				szWhiteNameList[0] = '\0';
				szBlackNameList[0] = '\0';
				break;
			}

			
			//取到指定字符为止的字符串。如在下例中，取遇到空格为止字符串。
			//sscanf("123456abcdedf","%[^]",str);

			char *pWhiteList = strstr(chElement, "WhiteNameList");
			if ( pWhiteList != NULL )
			{
				sscanf(pWhiteList,"WhiteNameList[%[^]]", szWhiteNameList);
		
			}

			char *pBlackList = strstr(chElement, "BlackNameList");
			if (pBlackList != NULL )
			{
				sscanf(pBlackList,"BlackNameList[%[^]]", szBlackNameList);
			}

			/*
			// 查找字符串
			size_t iNameListLen = 0;
			iNameListLen = strlen(chElement);

			char* pBlackNameList = NULL;
			pBlackNameList = strstr(chElement, ",BlackNameList") + 1;
			size_t iBlackNameListLen = strlen(pBlackNameList);

			char* pchWhiteNameListTemp = NULL;
			pchWhiteNameListTemp = new char[128*1024];
			if (NULL == pchWhiteNameListTemp)
				return 0;

			memset(pchWhiteNameListTemp, 0, 128*1024);
			strncpy(pchWhiteNameListTemp, chElement, iNameListLen - iBlackNameListLen - 2);
			sscanf(pchWhiteNameListTemp, "WhiteNameList[%s", szWhiteNameList);

			char* pchBlackNameListTemp = NULL;
			pchBlackNameListTemp = new char[128*1024];
			if (NULL == pchBlackNameListTemp)
			{
				delete[] pchWhiteNameListTemp;
				pchWhiteNameListTemp = NULL;
				return 0;
			}
			memset(pchBlackNameListTemp, 0, 128*1024);
			strncpy(pchBlackNameListTemp, pBlackNameList, iBlackNameListLen - 1);
			sscanf(pchBlackNameListTemp, "BlackNameList[%s", szBlackNameList);

			if (pchWhiteNameListTemp != NULL)
			{
				delete[] pchWhiteNameListTemp;
				pchWhiteNameListTemp = NULL;
			}
			if (pchWhiteNameListTemp != NULL)
			{
				delete[] pchWhiteNameListTemp;
				pchWhiteNameListTemp = NULL;
			}*/

			break;
		}
	}

	return true;
}

// 封装黑白名单XML
int HvGetXmlForTriggerPlate( const CHAR* szValue, INT iValueLen, CHAR *szXmlBuff, INT iBuffLen)
{
	if ( szValue == NULL || szXmlBuff == NULL )
		return 0;

	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "3.0");

	doc.LinkEndChild(pXmlRootElement);

	TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
	pXmlRootElement->LinkEndChild(pXmlElementCmd);

	//命令名称
	TiXmlText *pXmlCmdText = new TiXmlText("TriggerPlateRecog");
	pXmlElementCmd->LinkEndChild(pXmlCmdText);

	char szType[128] = {"CUSTOM"};
	char szClass[128] = {"SETTER"};

	pXmlElementCmd->SetAttribute("Type", szType);
	pXmlElementCmd->SetAttribute("Class", szClass);
	pXmlElementCmd->SetAttribute("Value", szValue);

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuff, printer.CStr(), iLen);
	szXmlBuff[iLen] = '\0';
	return iLen;
}

// 封装STRING类型的XML，协议3.0
int HvGetXmlOfStringType(const CHAR* szCMD, const CHAR* szClass, const CHAR* szValue, CHAR *szXmlBuff)
{
    if ( szValue == NULL || szXmlBuff == NULL )
        return 0;

    TiXmlDocument doc;
    TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    doc.LinkEndChild(pDecl);

    TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
    pXmlRootElement->SetAttribute("ver", "3.0");

    doc.LinkEndChild(pXmlRootElement);

    TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
    pXmlRootElement->LinkEndChild(pXmlElementCmd);

    // 命令名称
    TiXmlText *pXmlCmdText = new TiXmlText(szCMD);
    pXmlElementCmd->LinkEndChild(pXmlCmdText);

    char szType[128] = {"STRING"};

    pXmlElementCmd->SetAttribute("Type", szType);
    pXmlElementCmd->SetAttribute("Class", szClass);
    pXmlElementCmd->SetAttribute("Value", szValue);

    TiXmlPrinter printer;
    doc.Accept(&printer);

    int iLen = (int)printer.Size();
    memcpy(szXmlBuff, printer.CStr(), iLen);
    szXmlBuff[iLen] = '\0';
    return iLen;
}

// 解析XML获取返回字符串信息
bool HvGetRetMsgFromXml(const CHAR* szXmlBuff, CHAR* szRetMsg)
{
    if (szXmlBuff == NULL || NULL == szRetMsg)
        return false;

    // 创建解析文档
    TiXmlDocument myDocument;
    if ( !myDocument.Parse(szXmlBuff))
    {
        OutputDebugString( " !myDocument.Parse(szXmlBuff) \n");
        return false;
    }

    // 文档节点
    TiXmlElement* pRoot = myDocument.RootElement();
    if (pRoot ==  NULL)
        return false;

    // 解析结果
    TiXmlElement* pEle = NULL;
    const char *chElement = NULL;

    for (pEle = pRoot->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
    {
        if ( strcmp(pEle->Value(), "CmdName") == 0)
        {
            chElement = pEle->Attribute("RetMsg");

            if ( strcmp(chElement, "") == 0 )
            {
                szRetMsg[0] = '\0';
                break;
            }
            else
            {
                strcpy(szRetMsg, chElement);
            }
        }
    }

    return true;
}
