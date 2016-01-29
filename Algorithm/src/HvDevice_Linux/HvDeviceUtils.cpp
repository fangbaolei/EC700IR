#include "HvDeviceUtils.h"
#include "LinuxBmp.h"
#include "inifile.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
#include <setjmp.h>


#define XMD_H    //xurui
#include "ximage.h"

#include "jpeglib.h"
#include "jerror.h"

#include<string>
#undef max
#include<vector>


#ifdef _WIN32
#define 	 hvstricmp stricmp
#else
#define     hvstricmp strcasecmp
#endif







static int HvMakeInfoReq(char* szXmlBuf, int nInfoCount,
char** szInfoName, char** szInfoText );

struct SXmlAttr
{
   std::string strName;
   std::string  strValue;
   SXmlAttr()
   {
       strName = "";
       strValue = "";
   }
};

struct SXmlCmd
{
    std::string strCmdName;
    std::vector<SXmlAttr> listAttr;

    SXmlCmd()
    {
        strCmdName="";
        listAttr.clear();
    }
};

typedef std::vector<SXmlCmd>  XmlCmdList;


typedef enum {
	XML_CMD_GETTER, //
	XML_CMD_SETTER  //
} XML_CMD_CLASS;

//XML
struct SXmlCmdMercury
{
	std::string            strCmdName;
	std::vector<SXmlAttr>  listAttr;
	XML_CMD_CLASS          emCmdClass;
	XML_CMD_TYPE           emCmdType;
    int                    iArrayColumn; // INTARRAY1
    int                    iArrayRow;    // INTARRAY2

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

//XML
typedef std::vector<SXmlCmdMercury>    XmlCmdListMercury;

// XML_CMD_APPENDINFO_MAP_ITEM
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

// ÓÃÓÚÌî³ä XML_CMD_APPENDINFO_MAP_ITEM
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
    int iArrayColumn;         // INTARRAY1D
    int iArrayRow;            // INTARRAY2D
//    int iRetLen; // ¿ÉÒÔ¸ù¾ÝemCmdClassºÍemCmdTypeËã³ö
    int iParamNum;           //ÈôemCmdClass ÎªXML_CMD_SETTER
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
// Ë®ÐÇÐ­ÒéÐèÒªÎªÃ¿¸öÃüÁî¸½¼ÓType¡¢Value¡¢ClassÐÅÏ¢
XML_CMD_APPENDINFO_MAP_ITEM g_XmlCmdAppendInfoMap[] =
{
    //Ë®ÐÇDeviceÐÞ¸Ä ½«SetTime ÀàÐÍ»»ÎªXML_CMD_TYPE_CUSTOM_ENTRY
//	{ "SetTime", XML_CMD_SETTER, XML_CMD_TYPE_INTARRAY1D_ENTRY(7), 2, "Date", "Time", EMPTY_PARAMNAME30 }, // °ÑÄê¡¢ÔÂ¡¢ÈÕ¡¢Ê±¡¢·Ö¡¢Ãë¡¢ºÁÃë·Ö³É7¸öÕûÐÎ×é³ÉÊý×é´«Êä
    { "SetTime", XML_CMD_SETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 1, "SetTime", EMPTY_PARAMNAME31 }, // °ÑÄê¡¢ÔÂ¡¢ÈÕ¡¢Ê±¡¢·Ö¡¢Ãë¡¢ºÁÃë·Ö³É7¸öÕûÐÎ×é³É×Ö·û´®´«Êä
    { "GetWorkModeIndex", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1, "WorkModeName", EMPTY_PARAMNAME31 },
    { "SetIP", XML_CMD_SETTER, XML_CMD_TYPE_CUSTOM_ENTRY(), 1, EMPTY_PARAMNAME31 }, // ÏñÔ­À´Ò»ÑùÊ¹ÓÃ¶à¸öÊôÐÔÀ´±í´ïIP¡¢NETMASK¡¢GATEWAY
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
    { "GetCaptureEdge", DEFAULT_GETTER }, // Ô­À´µÄÉè±¸ÊÇÖ±½Ó·µ»ØE_FAILµÄ
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
	{ "SetCtrlCpl" , XML_CMD_SETTER ,XML_CMD_TYPE_BOOL_ENTRY() ,1 , "Enable" ,  EMPTY_PARAMNAME31},   // ÉèÖÃÆ«Õñ¾µ×´Ì¬
	{ "SetDCAperture", XML_CMD_SETTER , XML_CMD_TYPE_INT_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 }, //ÉèÖÃDC¹âÈ¦×´Ì¬
    { "GetBlackBoxMessage",XML_CMD_GETTER , XML_CMD_TYPE_STRING_ENTRY() , 1 , "RetMsg" , EMPTY_PARAMNAME31 },  //»ñÈ¡ºÚÏ»×ÓÏûÏ¢

    //Ô­Info
//    Ë®ÐÇDeviceÐÞ¸Ä ½«DateTime ÀàÐÍ»»ÎªXML_CMD_TYPE_CUSTOM_ENTRY
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
	{ "SendTriggerOut", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
	{ "SetImgType" ,XML_CMD_SETTER, XML_CMD_TYPE_INT_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },
	{ "SetNameList", XML_CMD_SETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "Value" , EMPTY_PARAMNAME31 },
	{ "GetNameList", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY() , 1 , "RetMsg" , EMPTY_PARAMNAME31 },




	// ÒÔÏÂÊÇ½ðÐÇÐèÇó¹Ø¼ü×Ö
	//Éè±¸¹ÜÀí
	/*GetDevType*/ //ÉÏÃæÒÑ¾­ÓÐÁË{ "GetDevType", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 2, "DevType", "BuildNo",EMPTY_PARAMNAME30 },
	/*GetDevRunMode*/ { "OptResetMode", DEFAULT_GETTER },
	/*GetDevBasicInfo*/ { "GetDevBasicInfo", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 14 ,"Mode","SN","DevType","WorkMode","DevVersion","IP","Mac","Mask","Gateway","BackupVersion","FPGAVersion","KernelVersion","UbootVersion","UBLVersion", EMPTY_PARAMNAME18 },
	/*GetDevState*/ { "GetDevState", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 15, "CpuUsage","MemUsage","CpuTemperature","RecordLinkIP","ImageLinkIP","VideoLinkIP","Date","Time","HddOpStatus","ResetCount","TimeZone","NTPEnable","NTPServerIP","NTPServerUpdateInterval","TraceRank",EMPTY_PARAMNAME17 },
	/*GetResetCount* / /*OptResetCountÌæ»»*///{ "GetResetCount", DEFAULT_GETTER },
	/*GetHddCheckReport *///ÉÏÃæÒÑ¾­ÓÐÁË { "GetHddCheckReport", XML_CMD_GETTER ,  XML_CMD_TYPE_STRING_ENTRY (), 1, "HddCheckReport", EMPTY_PARAMNAME31  },
	/*SetIP*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetIP", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY(), 1, EMPTY_PARAMNAME31 }, // ÏñÔ­À´Ò»ÑùÊ¹ÓÃ¶à¸öÊôÐÔÀ´±í´ïIP¡¢NETMASK¡¢GATEWAY
	/*SetNTPServerIP*/ { "SetNTPServerIP", XML_CMD_SETTER,XML_CMD_TYPE_STRING_ENTRY (), 1, "ServerIP", EMPTY_PARAMNAME31},
	/*SetNTPServerUpdateInterval*/ { "SetNTPServerUpdateInterval", XML_CMD_SETTER,XML_CMD_TYPE_INT_ENTRY (), 1, "UpdateInterval", EMPTY_PARAMNAME31},
	/*SetTimeZone*/ { "SetTimeZone", DEFAULT_SETTER },
	/*SetNTPEnable*/ { "SetNTPEnable", DEFAULT_SETTER },
	/*SetTime*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetTime", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY (), 1, "SetTime", EMPTY_PARAMNAME31 }, // °ÑÄê¡¢ÔÂ¡¢ÈÕ¡¢Ê±¡¢·Ö¡¢Ãë¡¢ºÁÃë·Ö³É7¸öÕûÐÎ×é³É×Ö·û´®´«Êä
	/*ResetDevice*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "ResetDevice", XML_CMD_SETTER ,XML_CMD_TYPE_INT_ENTRY (), 1, "ResetMode", EMPTY_PARAMNAME31 },
	/*RestoreDefaultParam*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "RestoreDefaultParam", DEFAULT_SETTER },
	/*RestoreFactoryParam*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "RestoreFactoryParam", XML_CMD_SETTER , XML_CMD_TYPE_NULL_ENTRY (), 0, EMPTY_PARAMNAME32 },
	/*SetOCGate*/ /*SendTriggerOutÌæ»»*///{ "SetOCGate", DEFAULT_SETTER },
	/*EnableCOMCheck*//*SetComTestEnable SetComTestDisableÌæ»»*/ //{ "EnableCOMCheck", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "COMNum", "Enable",EMPTY_PARAMNAME30 },
	/*SetComTestEnable */{ "SetComTestEnable", DEFAULT_SETTER },
	/*SetComTestDisable */{ "SetComTestDisable", DEFAULT_SETTER },
	/*SetTraceRank */{ "SetTraceRank", DEFAULT_SETTER },
	/*GetTraceRank */{ "GetTraceRank", DEFAULT_GETTER },
	{"HDDFdisk" , DEFAULT_SETTER },
	{"HDDStatus", XML_CMD_GETTER, XML_CMD_TYPE_STRING_ENTRY(), 1 ,"RetMsg", EMPTY_PARAMNAME31},

	//Ïà»ú
	/*GetContrast*/ { "GetContrast", DEFAULT_GETTER  },
	/*GetSaturation*/ { "GetSaturation", DEFAULT_GETTER  },
	/*GetSharpness*/ { "GetSharpness", DEFAULT_GETTER  },
	/*GetDCAperture*/ { "GetDCAperture", DEFAULT_GETTER  },
	/*GetWDREnable*/{ "GetWDREnable", DEFAULT_GETTER  },
	/*GetWDRLevel*/{ "GetWDRLevel", DEFAULT_GETTER  },
	/*GetAWBEnable*///ÉÏÃæÒÑ¾­ÓÐÁË
	/*GetAGCEnable*///ÉÏÃæÒÑ¾­ÓÐÁË
	/*GetDeNoiseLevel*/{ "GetDeNoiseLevel", DEFAULT_GETTER  },


	/*GetSNFEnable*/ { "GetSNFEnable", DEFAULT_GETTER  },
	/*GetEnableGrayImage*/ { "GetEnableGrayImage", DEFAULT_GETTER  },
	/*GetSyncPower*/ { "GetSyncPower", XML_CMD_GETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (2), 2, "Mode", "DelayMS",EMPTY_PARAMNAME30 },
	/*GetDeNoiseMode*/ { "GetDeNoiseMode", DEFAULT_GETTER  },
	/*GetCameraBasicInfo*//*´óÃüÁî*/ { "GetCameraBasicInfo", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () ,32, "Contrast","Saturation","SharpnessEnable","Sharpness","DCEnable","WDREnable ","WDRLevel","AWBEnable","AGCEnable","ACSyncMode","ACSyncDelay","DeNoiseMode","DeNoiseLevel","AGCLightBaseLine","AGCShutterMin","AGCShutterMax","AGCGainMin","AGCGainMax","GrayImageEnable","ImageEnhancementEnable","GammaEnable","GammaValue","EnRedLightThreshold","DeNoiseTNFEnable", "DeNoiseSNFEnable", "EdgeEnhance","ManualShutter", "ManualGain", "ManualGainR", "ManualGainG", "ManualGainB", "FilterMode" },
	/*GetCameraState*//*´óÃüÁî*/ { "GetCameraState", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 8, "AGCEnable","Shutter","Gain","AWBEnable","GainR","GainG","GainB", "LEDSwitchState", EMPTY_PARAMNAME24 },
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
	/*ZoomDCIRIS*/ /*ÉèÖÃDC¹âÈ¦·Å´ó*/{ "ZoomDCIRIS", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32 },
	/*ShrinkDCIRISÊÇËõÐ¡±êÊ¶*/{ "ShrinkDCIRIS", XML_CMD_SETTER, XML_CMD_TYPE_NULL_ENTRY(), 0, EMPTY_PARAMNAME32  },
	/*SetDCIRIS*//*Ê¹ÓÃSetDCApertureÌæ»»*/ //{ "SetDCIRIS", DEFAULT_SETTER },
	/*SetFilterMode*/ /*SetCtrlCplÌæ»»*///{ "SetFilterMode", DEFAULT_SETTER },
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
	/*SetRgbGain*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetRgbGain", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (3), 3, "GainR", "GainG", "GainB",EMPTY_PARAMNAME29 },
	/*SetAGCEnable*/ //ÉÏÃæÒÑ¾­ÓÐÁË { "SetAGCEnable", DEFAULT_SETTER },
	/*SetAgcLightBaseLine*///ÉÏÃæÒÑ¾­ÓÐÁË  { "SetAgcLightBaseLine", DEFAULT_SETTER },
	/*SetAGCParam*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetAGCParam", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (4), 4, "ShutterMin", "ShutterMax", "GainMin", "GainMax", EMPTY_PARAMNAME28 },
	/*SetAGCZone*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetAGCZone", XML_CMD_SETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "AGCZone00", "AGCZone01", "AGCZone02", "AGCZone03", "AGCZone04", "AGCZone05", "AGCZone06", "AGCZone07", "AGCZone08", "AGCZone09", "AGCZone10", "AGCZone11", "AGCZone12", "AGCZone13", "AGCZone14", "AGCZone15",EMPTY_PARAMNAME16 },
	/*GetAGCZone*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "GetAGCZone", XML_CMD_GETTER ,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "AGCZone00", "AGCZone01", "AGCZone02", "AGCZone03", "AGCZone04", "AGCZone05", "AGCZone06", "AGCZone07", "AGCZone08", "AGCZone09", "AGCZone10", "AGCZone11", "AGCZone12", "AGCZone13", "AGCZone14", "AGCZone15",EMPTY_PARAMNAME16 },
	/*SetShutter*/ //ÉÏÃæÒÑ¾­ÓÐÁË { "SetShutter", XML_CMD_SETTER ,XML_CMD_TYPE_INT_ENTRY (), 1, "Shutter", EMPTY_PARAMNAME31 },
	/*SetGain*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetGain", XML_CMD_SETTER ,XML_CMD_TYPE_INT_ENTRY(), 1, "Gain", EMPTY_PARAMNAME31 },
	/*SetEnableGrayImage*/ { "SetEnableGrayImage", DEFAULT_SETTER  },
	/*´ýYYYÐ­ÉÌHVAPI_SetACSync Ó¦Í¬Ê±¿ÉÉèÖÃÄ£Ê½ºÍÑÓÊ±£¬*//*SetSyncPower*/ { "SetSyncPower", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1,  EMPTY_PARAMNAME31 },
	/*GetLUT*//*8µã×ø±ê*//*GetGammaDataÌæ»»?*/ //{ "GetLUT", XML_CMD_GETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "Point0_X", "Point0_Y", "Point1_X", "Point1_Y", "Point2_X", "Point2_Y", "Point3_X", "Point3_Y", "Point4_X", "Point4_Y", "Point5_X", "Point5_Y", "Point6_X", "Point6_Y", "Point7_X", "Point7_Y", EMPTY_PARAMNAME16 },
	/*SetLUT*//*8µã×ø±ê*//*SetGammaDataÌæ»»?*/ //{ "SetLUT", XML_CMD_SETTER,XML_CMD_TYPE_INTARRAY1D_ENTRY (16), 16, "Point0_X", "Point0_Y", "Point1_X", "Point1_Y", "Point2_X", "Point2_Y", "Point3_X", "Point3_Y", "Point4_X", "Point4_Y", "Point5_X", "Point5_Y", "Point6_X", "Point6_Y", "Point7_X", "Point7_Y", EMPTY_PARAMNAME16 },
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
	/*SetLEDSwitch*/{ "SetLEDSwitch", DEFAULT_SETTER },
	/*GetLEDSwitch*/{ "GetLEDSwitch", DEFAULT_GETTER },

	//ÊÓÆµ
	/*GetVideoState*/ { "GetVideoState", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 9, "CVBSDisplayMode","H264BitRate","JpegCompressRate","AutoJpegCompressEnable","AutoJpegCompressEnable", "JpegFileSize", "JpegCompressMaxRate", "JpegCompressMinRate", "DebugJpegStatus", EMPTY_PARAMNAME23 },
	/*SetH264BitRate*/ { "SetH264BitRate", DEFAULT_SETTER },
	/*SetJpegCompressRate*/  //ÉÏÃæÒÑ¾­ÓÐÁË{ "SetJpegCompressRate", DEFAULT_SETTER },
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
	/*SetOSDh264Font ×ÖÌå´óÐ¡*/ { "SetOSDh264Font", DEFAULT_SETTER  },
	/*SetOSDjpegFont ×ÖÌå´óÐ¡*/{ "SetOSDjpegFont", DEFAULT_SETTER  },
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

	//ÓÃ»§¹ÜÀí
	/*DoLogin*/ { "DoLogin", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1,  EMPTY_PARAMNAME31 },
	/*GetUsers*/ { "GetUsers", XML_CMD_GETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	/*AddUser*/ { "AddUser", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	/*DelUser*/ { "DelUser", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },
	/*ModUser*/ { "ModUser", XML_CMD_SETTER ,XML_CMD_TYPE_CUSTOM_ENTRY () , 1, EMPTY_PARAMNAME31 },

};
static int g_XmlCmdAppendInfoMapItemCount = sizeof(g_XmlCmdAppendInfoMap)/sizeof(g_XmlCmdAppendInfoMap[0]);







ThreadMutex g_mutexLog;

//Get True Path(No include filename)
char* GetProcessTruePath(char* path, int size )
{
    char *tempath = ( char* )malloc(size);
    chdir("./");
    getcwd(path, size);
    chdir(tempath);
    delete tempath;
    return path;
}

const int BUFFER_SIZE = 3;
bool CopyFile(char *lpExistingFileName, char *lpNewFileName, bool bFailExists)
{
    if (lpExistingFileName == NULL)
    return false;
    if (lpNewFileName == NULL)
     return false;

    FILE * from_fd =  NULL, *to_fd =  NULL;
    int bytes_read = 0, bytes_write = 0;

    char buffer[BUFFER_SIZE];
    char *ptr = NULL;

    from_fd = fopen(lpExistingFileName, "r");
    if (from_fd== NULL)
        return false;

    to_fd = fopen(lpNewFileName, "w");
    if ( to_fd == NULL)
        return false;

    while (bytes_read = fread(buffer, 1, BUFFER_SIZE, from_fd))
    {
        if ((bytes_read == -1) && errno != EINTR)
            break;
        else if (bytes_read > 0)
        {
            ptr = buffer;
            while (bytes_write =fwrite(ptr, 1, bytes_read, to_fd))
            {
                if ((bytes_write == -1) && (errno != EINTR))
                    break;
                else if (bytes_write == bytes_read)
                   break;
                else if (bytes_write > 0)
                {
                    ptr += bytes_write;
                    bytes_read -= bytes_write;
                }
            }
            if (bytes_write == -1)
                break;
        }

        bytes_read = 0;
    }
    fclose(from_fd);
    fclose(to_fd);
    return true;
}

static unsigned long GetTickCount()
{
   // tms tm;
    //return times;
    unsigned long currentTime;
    struct timeval current;
    gettimeofday(&current, NULL);
    currentTime = current.tv_sec * 1000 + current.tv_usec/1000;

    return currentTime;
}

void WriteLog(const char *pszLog)
{
   const int NOT_FIND = -99;
	 g_mutexLog.lock();

	//ÃÃºÂ³ÃLOGÃÃÅÃ¾ÃÃ»
	const int MAXPATH = 260;
	char szFileName[MAXPATH] = {0};
	char szIniName[MAXPATH] = {0};
	char szBakName[MAXPATH] = {0};
    GetProcessTruePath(szFileName, MAXPATH);
	strncpy(szIniName, szFileName, MAXPATH);
	strncpy(szBakName, szFileName, MAXPATH);
	strncat(szFileName, "/HvDevice.log", MAXPATH - 1);
	strncat(szIniName, "/HvDevice.ini", MAXPATH - 1);
	strncat(szBakName, "/HvDevice.bak", MAXPATH - 1);

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

		//ÃÂ¡ÂµÃÂµÂ±ÃÂ°ÂµÃÅ¸Â«ÃÂ·ÂºÃÃÃ«ÂµÃÃÂ±ÅÃ¤
		static time_t starttime = time(NULL);
		static unsigned long starttick = GetTickCount();
		unsigned long dwNowTick = GetTickCount() - starttick;
		time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
		struct tm *pTM = localtime(&nowtime);
		unsigned long dwMS = dwNowTick % 1000;

		int iYDay = GetPrivateProfileInt("LogInf", "YDay", NOT_FIND, szIniName);
		if (NOT_FIND == iYDay)
		{
			iYDay = pTM->tm_yday;
			char szYDay[20];
			sprintf(szYDay, "%d", iYDay);
			WritePrivateProfileString("LogInf", "YDay", szYDay, szIniName);
		}

		//ÃÂ·Â¶Å¡ÃÃÂ·Ã±ÃÃÃÅ½ÃÃÅÃ¾
		FILE *pfile = NULL;
		if ((pTM->tm_yday - iYDay < iHoldDays) && (pTM->tm_yday - iYDay >= 0))
		{
			pfile = fopen(szFileName, "a+");
		}
		else
		{
			//CopyFile(szFileName, szBakName, FALSE);
			pfile = fopen(szFileName, "w");
			char szYDay[20];
			sprintf(szYDay, "%d", pTM->tm_yday);
			WritePrivateProfileString("LogInf", "YDay", szYDay, szIniName);
		}

		//ÃÅ½ÃÃ«LOGÃÃÃÂ¢
		if (pfile != NULL)
		{
			fprintf(pfile, "%04d-%02d-%02d %02d:%02d:%02d:%03d %s\n",
				pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
				pTM->tm_hour, pTM->tm_min, pTM->tm_sec, dwMS,
				pszLog);
			fclose(pfile);
		}
	}

    // g_IsemWriteLog->Post(ISemaphore::NOWAIT);
    g_mutexLog.unlock();
}

HRESULT HvGetNonGreedyMatchBetweenBeginLabelAndEndLabel(const CHAR* szBeginLabel,const  CHAR* szEndLabel,const  CHAR* pszRetMsg, INT nRetMsgLen, CHAR* szJieQuReslut, INT nReslutLen )
{
	if ( NULL == szBeginLabel || NULL == szEndLabel || NULL == pszRetMsg || NULL == szJieQuReslut)
	{
	}
	char* pBegin = (char*)strstr( pszRetMsg , szBeginLabel );
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


// ËÑË÷g_XmlCmdAppendInfoMap±í£¬·µ»ØÃüÁîË÷Òý£¬Èç¹ûÕÒ²»µ½ÃüÁîÔò·µ»Ø-1
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
    return true;
}
bool UnInitWSA()
{
    return true;
}

int RecvAll( int socket, char *pBuffer, int iRecvLen )
	{
		char *pTemp = pBuffer;
		int iRst = iRecvLen;

		while ( iRecvLen > 0 )
		{
			int iTemp = recv( socket, pTemp, iRecvLen, 0 );

			if ( iTemp < 0 )
			{
			    //int irr = errno;
			   // strerror(errno);
			    if ( errno ==  EINTR /*|| errno == EAGAIN || errno == EWOULDBLOCK*/)
			    {
			        continue ;
			    }
				return iTemp;
			}
			if (  iTemp == 0 )
				break;
			pTemp += iTemp;
			iRecvLen -= iTemp;
		}
		return iRst - iRecvLen;
	}

int RecvAll(int socket, char *pBuffer, int iRecvLen , int& iRealRecvLen) {
	char *pTemp = pBuffer;
	int iRst = iRecvLen;

	int iBlockLen = 10240;

	while (iRecvLen > 0)
	{
		int iTemp = recv(socket, pTemp, iRecvLen>iBlockLen?iBlockLen:iRecvLen, 0);
		//int iTemp = recv( socket, pTemp, iRecvLen, 0 );

        if ( iTemp < 0 )
        {
            //int irr = errno;
           // strerror(errno);
            if ( errno ==  EINTR /*|| errno == EAGAIN || errno == EWOULDBLOCK*/)
            {
                continue ;
            }
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






const int MAX_HV_COUNT = 1000;
const unsigned short GETIP_COMMAND_PORT = 6666;
const DWORD32 GETIP_COMMAND = 0x00000101;
const DWORD32 SETIP_COMMAND = 0x00000102;
static int g_iDeviceCount=0;
static unsigned char g_aDeviceAddr[MAX_HV_COUNT][18+32+32];


int ForceCloseSocket(int &iNetSocket)
{
    if ( iNetSocket == 0)
        return 0;
    shutdown(iNetSocket, SHUT_RDWR);
    int iRet = close(iNetSocket);
    iNetSocket = 0;
    return iRet;
}

//
HRESULT SearchHVDeviceCount(DWORD32 *pdwCount)
{
    if ( pdwCount == NULL)
        return E_FAIL;

    //char szHostName[1024] = {0};
	//gethostname(szHostName, sizeof(szHostName));    //获得本地主机名
    //struct hostent* pHostInfo = gethostbyname(szHostName);//信息结构体


    int updSocket = 0;
    updSocket = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( updSocket < 0 )
        return E_FAIL;

    int flags = fcntl(updSocket, F_GETFL, 0);
    fcntl(updSocket, F_SETFL, flags&~O_NONBLOCK);

  /*  bool bBroadcast = true;
    if ( 0  !=   setsockopt(updSocket, SOL_SOCKET, SO_BROADCAST, (const void*)&bBroadcast, sizeof(bool))  )
        return E_FAIL;
*/
    int bBroadcast = 1;
    if ( 0  !=   setsockopt(updSocket, SOL_SOCKET, SO_BROADCAST, (const void*)&bBroadcast, sizeof(bBroadcast))  )
        return E_FAIL;

    struct timeval timeout ;
    timeout.tv_sec =2;
    timeout.tv_usec =  0;

     //if ( setsockopt(updSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&timeout, sizeof(struct timeval))  != 0 )
       //     return -1;
    if ( setsockopt(updSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval))  != 0 )
            return -1;

    struct sockaddr_in addrRemote;
    memset(&addrRemote, 0, sizeof(addrRemote));
  //  addrRemote.sin_addr.S_un.S_addr = 0xffffffff;
    addrRemote.sin_addr.s_addr = htonl(0xffffffff);
    addrRemote.sin_family = AF_INET;
    short sPort = GETIP_COMMAND_PORT;
    addrRemote.sin_port = htons(sPort);

    int iLen = sizeof(GETIP_COMMAND);

    if ( iLen  != sendto(updSocket, (char*)&GETIP_COMMAND, iLen, 0, (struct sockaddr *)&addrRemote, sizeof(addrRemote)) )
    {
       // printf("%s",strerror(errno));
        return E_FAIL;
    }


   // int iCount = 0;
    g_iDeviceCount = 0;
    while ( true )
    {
        /*
        struct timeval tv;
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(updSocket, &rset);
        int iTemp = select(updSocket+1, &rset, NULL, NULL, &tv);
        if (iTemp == -1)
            break;
        if ( iTemp== 0 )
            break;
        */
        struct sockaddr_in addrRemote;
         int iAddlen = sizeof(addrRemote);
        char chBuffer[4096] = {0};
		int iRet = recvfrom(updSocket, (char*)chBuffer, sizeof(chBuffer), 0, (struct sockaddr *)&addrRemote, (socklen_t *)&iAddlen);

		 if ( iRet == -1 )
        {
              //printf("strerror: %d, %s\n", errno, strerror(errno));
             break;
          }
        if ( iRet == 0 )
            break ;
        if (iRet == E_HANDLE || iRet == E_POINTER)
            break ;

        TiXmlDocument cXmlDoc;
        if ( cXmlDoc.Parse(chBuffer ) )
        {
            const TiXmlElement *pRootElement = cXmlDoc.RootElement();
            if ( NULL == pRootElement )
                continue;

            const TiXmlElement *pCmdArgElement = pRootElement->FirstChildElement("CmdName");
            if ( pCmdArgElement == NULL )
                continue ;

            const char* szIP = pCmdArgElement->Attribute("IP");
            const char* szMask = pCmdArgElement->Attribute("Mask");
            const char* szGateway=pCmdArgElement->Attribute("Gateway");
            const char* szMac=pCmdArgElement->Attribute("MAC");
            const char* szSN=pCmdArgElement->Attribute("SN");
            const char* szMode=pCmdArgElement->Attribute("Mode");
            if ( szIP==NULL || szMask==NULL || szGateway==NULL || szMac==NULL ||
                szSN==NULL || szMode==NULL )
                continue;

            DWORD dwIP[4]={0}, dwMask[4]={0},dwGateway[4]={0},dwMac[4]={0};
            sscanf(szIP, "%d.%d.%d.%d", &dwIP[3],&dwIP[2],&dwIP[1],&dwIP[0]);
            sscanf(szMask, "%d.%d.%d.%d", &dwMask[3],&dwMask[2],&dwMask[1],&dwMask[0]);
            sscanf(szGateway, "%d.%d.%d.%d", &dwGateway[3],&dwGateway[2],&dwGateway[1],&dwGateway[0]);
            sscanf(szMac, "%x:%x:%x:%x:%x:%x", &dwMac[0],&dwMac[1],&dwMac[2],&dwMac[3],&dwMac[4],&dwMac[5] );

            int i= 0;
            for(i=0; i<g_iDeviceCount; i++)
            {
                if ( (dwMac[0] & 0xFF) == g_aDeviceAddr[i][0] &&
                    (dwMac[1] & 0xFF) == g_aDeviceAddr[i][1] &&
                    (dwMac[2] & 0xFF) == g_aDeviceAddr[i][2] &&
                    (dwMac[3] & 0xFF) == g_aDeviceAddr[i][3] &&
                    (dwMac[4] & 0xFF) == g_aDeviceAddr[i][4] &&
                    (dwMac[5] & 0xFF) == g_aDeviceAddr[i][5] )
                break;
            }

            if ( i == g_iDeviceCount  )
            {
                if ( g_iDeviceCount >= MAX_HV_COUNT )
                    break;

                g_aDeviceAddr[i][0] = dwMac[0] & 0xFF;
                g_aDeviceAddr[i][1] = dwMac[1] & 0xFF;
                g_aDeviceAddr[i][2] = dwMac[2] & 0xFF;
                g_aDeviceAddr[i][3] = dwMac[3] & 0xFF;
                g_aDeviceAddr[i][4] = dwMac[4] & 0xFF;
                g_aDeviceAddr[i][5] = dwMac[5] & 0xFF;

                g_aDeviceAddr[i][6] = dwIP[0] & 0xFF;
                g_aDeviceAddr[i][7] = dwIP[1] & 0xFF;
                g_aDeviceAddr[i][8] = dwIP[2] & 0xFF;
                g_aDeviceAddr[i][9] = dwIP[3] & 0xFF;

                g_aDeviceAddr[i][10] = dwMask[0] & 0xFF;
                g_aDeviceAddr[i][11] = dwMask[1] & 0xFF;
                g_aDeviceAddr[i][12] = dwMask[2] & 0xFF;
                g_aDeviceAddr[i][13] = dwMask[3] & 0xFF;

                g_aDeviceAddr[i][14] = dwGateway[0] & 0xFF;
                g_aDeviceAddr[i][15] = dwGateway[1] & 0xFF;
                g_aDeviceAddr[i][16] = dwGateway[2] & 0xFF;
                g_aDeviceAddr[i][17] = dwGateway[3] & 0xFF;

                strncpy((char*)&g_aDeviceAddr[i][18], szSN, 31);
                strncpy((char*)&g_aDeviceAddr[i][40], szMode, 31);
                g_iDeviceCount++;

            }
        }
        else
        {
            if ( iRet == 22 || iRet==22+32 || iRet == 22+32+32 )
            {
                DWORD32 dwCommand;
                memcpy(&dwCommand, chBuffer, sizeof(dwCommand));
                //   printf("%d\n" , dwCommand);
                if ( dwCommand == GETIP_COMMAND )
                {
                    if ( (chBuffer[9] & 0xFF) == 0xB6 || (chBuffer[9]&0xFF)==0xB7
                        || (chBuffer[9]&0xFF) == 0xB8 || (chBuffer[9]&0xFF) == 0xB9 )
                    {
                        int i=0;
                        for (i=0; i<g_iDeviceCount; i++)
                        {
                            if ( memcmp(&chBuffer[4], g_aDeviceAddr[i], 6) == 0)
                            {
                                break ;
                            }
                        }//end for

                        if (i == g_iDeviceCount)
                        {
                            memset(g_aDeviceAddr[g_iDeviceCount], 0, 18+32+32);
                            memcpy(g_aDeviceAddr[g_iDeviceCount], &chBuffer[4], iRet - sizeof(dwCommand));

                            g_iDeviceCount++;
                            if ( g_iDeviceCount == MAX_HV_COUNT )
                                break ;
                        }
                    }

                }//if
            }// if doc
        }


    }//while

   // printf("DeviceCount:%d\n", iCount);
    // close socketint nSendLen = send( m_hSocket, (char*)pv, cb, MSG_NOSIGNAL);
    ForceCloseSocket(updSocket);
    usleep(50000);
    *pdwCount  = g_iDeviceCount;
    return S_OK;
}

HRESULT GetHVDeviceAddr(int iIndex,
                                                    DWORD64 *pdw64MacAddr,
                                                    DWORD32 *pdwIP,
                                                    DWORD32 *pdwMask,
                                                    DWORD32 *pdwGateway)
 {
     if ( iIndex >= g_iDeviceCount || iIndex < 0 )
        return E_FAIL;
    if (pdw64MacAddr != NULL)
        memcpy(pdw64MacAddr, g_aDeviceAddr[iIndex], 6);
    if (pdwIP != NULL)
        memcpy(pdwIP, &g_aDeviceAddr[iIndex][6], 4);
    if (pdwMask != NULL)
        memcpy(pdwMask, &g_aDeviceAddr[iIndex][6+4], 4);
    if (pdwGateway)
        memcpy(pdwGateway, &g_aDeviceAddr[iIndex][6+4+4], 4);
    return S_OK;


}

HRESULT SetIPFromMac(DWORD64  dw64MacAddr, DWORD dwIP,DWORD dwMask, DWORD dwGateway)
{
    int iSock = socket(PF_INET, SOCK_DGRAM, 0);
    if ( iSock  == -1 )
        return E_FAIL;

     int bBroadcast = 1;
    if ( 0  !=   setsockopt(iSock , SOL_SOCKET, SO_BROADCAST, (const void*)&bBroadcast, sizeof(bBroadcast))  )
        return E_FAIL;

    struct sockaddr_in addrRemote;
    memset(&addrRemote, 0, sizeof(addrRemote));
    addrRemote.sin_addr.s_addr = htonl(0xffffffff);
    addrRemote.sin_family = AF_INET;
    addrRemote.sin_port = htons(GETIP_COMMAND_PORT);

    char chTemp[22] = {0};
    memcpy(&chTemp[0], &SETIP_COMMAND, 4);
    memcpy(&chTemp[4], &dw64MacAddr, 6);
    memcpy(&chTemp[10], &dwIP, 4);
    memcpy(&chTemp[14], &dwMask, 4);
    memcpy(&chTemp[18], &dwGateway, 4);

    int iLen = sizeof(chTemp);
    int iRtn = sendto(iSock, (const void *)chTemp, iLen, 0, (struct sockaddr *)&addrRemote, sizeof(addrRemote));
    if (iRtn != iLen)
        return E_FAIL;



  //    struct sockaddr_in addrRemote;
   int iRst = E_FAIL;
    int iaddrLen = sizeof(addrRemote);
      char acBuf[20] = {0};
      int iRecvLen = recvfrom(iSock, acBuf, sizeof(acBuf),0,(sockaddr*)&addrRemote, (socklen_t*)&iaddrLen);
     if ( iRecvLen == 4 || iRecvLen == 8 )
        iRst = 0;

    ForceCloseSocket(iSock);
    usleep(50000);

    //arp
    system("sudo arp -n|awk");
  // if ( iTemp == 0 )
  if ( iRst != 0 )
        return S_FALSE;
    return iRst;
}

HRESULT HvGetDeviceExteInfo(int iIndex, LPSTR lpExtInfo, int iBufLen)
{
      if ( iIndex >= g_iDeviceCount || iIndex < 0 )
        return E_FAIL;

    if ( lpExtInfo == NULL || iBufLen<=0  )
        return E_FAIL;
   // int iSerStrinLen = (int)strlen( (char*)g_aDeviceAddr[iIndex] );
   int iSerStrinLen = 32;
    if ( iSerStrinLen<=0 || iBufLen <= iSerStrinLen)
        return E_FAIL;
    memcpy((char*)lpExtInfo, (char*)&g_aDeviceAddr[iIndex][18], iSerStrinLen);
    return S_OK;
}


bool ConnectCamera( char *szIp, WORD wPort, int &hSocket, int iTryTimes, int iReceiveTimeOutMs)
{
    if ( hSocket !=  -1)
    {
        ForceCloseSocket(hSocket);
        hSocket = -1;
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if ( hSocket == -1)
        return false;

    struct timeval tv;
    tv.tv_sec =  iReceiveTimeOutMs/1000;
    tv.tv_usec= iReceiveTimeOutMs%1000;
    //if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval))  != 0 )
     //       return -1;

    if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval))  != 0 )
            return -1;

   // int val = 1;
    //setsockopt(hSocket, SOL_SOCKET,SO_REUSEADDR, (void*)&val, sizeof(val));

    int val = 1;
    setsockopt(hSocket, SOL_SOCKET,SO_REUSEADDR, (char*)&val, sizeof(val));

	int flags = fcntl(hSocket, F_GETFL, 0);
    fcntl(hSocket, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(wPort);
    addr.sin_addr.s_addr = inet_addr(szIp);

    do
    {
       if (  connect(hSocket, (struct sockaddr*)&addr, sizeof(addr) ) != 0 )
       {
           if ( errno != EINPROGRESS )
           {
              // iTryTimes = -1;
              // break;
              continue;
           }

           struct timeval timeout ;
			fd_set r;
			FD_ZERO( &r );
			FD_SET(hSocket, &r);
			timeout.tv_sec =  4;
			timeout.tv_usec =0;
			int ret = select(hSocket+ 1,NULL, &r,  NULL, &timeout);
			if ( ret > 0 )
             {
                 if ( FD_ISSET(hSocket, &r) )
                 {
                     int ierror = 0;
                     socklen_t  len  = sizeof(int);
                     getsockopt(hSocket, SOL_SOCKET,  SO_ERROR,  &ierror , &len);
                     if ( ierror == 0 )
                        break;
                 }

             }
       }
       else
       {
           break;
       }
    }while (iTryTimes-- > 0 );

    if ( iTryTimes < 0 )
    {
        char szLog[256] = { 0 };
        sprintf(szLog, "[HvDevice] %s Cmd Link Fail ", szIp);
        ForceCloseSocket(hSocket);
        return false;
    }

    flags = fcntl(hSocket, F_GETFL, 0);
    fcntl(hSocket, F_SETFL, flags & ~O_NONBLOCK);

    char szLog[256] = { 0 };
    sprintf(szLog, "[HvDevice] %s Cmd Link Success ", szIp);
    WriteLog(szLog);
    return true;
}

bool IsNewProtocol(char *szIP)
{
    if ( szIP == NULL )
        return false;
    const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hsocketcmd = -1;
    CAMERA_CMD_HEADER  cmdHearder;
    CAMERA_CMD_RESPOND cmdRespond;

    if ( !ConnectCamera(szIP, wPort, hsocketcmd, 0 ) )
     {
            return false;
     }

    bool fIsOK = false;
    cmdHearder.dwID = CAMERA_PROTOCAL_COMMAND;
    cmdHearder.dwInfoSize = 0;

    int iHeaderLen = sizeof(cmdHearder);
    if ( iHeaderLen  !=  send( hsocketcmd, (char*)&cmdHearder, iHeaderLen, MSG_NOSIGNAL) )
    {
        ForceCloseSocket(hsocketcmd);
        return fIsOK;
    }
    int iRespondLen = sizeof(cmdRespond);
   if ( iRespondLen !=  RecvAll(hsocketcmd, (char *)&cmdRespond, iRespondLen ) )
   {
       ForceCloseSocket(hsocketcmd);
        return fIsOK;
   }

   fIsOK =  ( cmdRespond.dwID == CAMERA_PROTOCAL_COMMAND );
   ForceCloseSocket(hsocketcmd);
   return fIsOK;
}

HRESULT HvGetXmlProtocolVersion(char* szIP, DWORD *pdwVersionType)
{
    if ( szIP == NULL || pdwVersionType == NULL )
        return E_FAIL;
    int hSocketCmd = -1;
    CAMERA_CMD_HEADER cCmdHeader;
    CAMERA_CMD_RESPOND cCmdRespond;
    if (!ConnectCamera(szIP, CAMERA_CMD_LINK_PORT, hSocketCmd))
    {
        return E_FAIL;
    }
    cCmdHeader.dwID  = CAMERA_PROTOCAL_COMMAND;
    cCmdHeader.dwInfoSize = 0;
    if ( send (hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader),  MSG_NOSIGNAL) != sizeof(cCmdHeader) )
    {
        ForceCloseSocket(hSocketCmd);
        hSocketCmd = INVALID_SOCKET;
        return E_FAIL;
    }

    if ( recv( hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond),  MSG_NOSIGNAL ) != sizeof(cCmdRespond) )
    {
        ForceCloseSocket(hSocketCmd);
        hSocketCmd = INVALID_SOCKET;
        return E_FAIL;
    }
    ForceCloseSocket(hSocketCmd);
    if ( cCmdRespond.dwID != CAMERA_PROTOCAL_COMMAND)
    {
        return E_FAIL;
    }
    *pdwVersionType = 1;
    return S_OK;
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
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader),  MSG_NOSIGNAL) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE(pbTmpXmlBuf);
			return false;
		}
		if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize,  MSG_NOSIGNAL) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE(pbTmpXmlBuf);
			return false;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond),  MSG_NOSIGNAL) )
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
						SAFE_DELETE(pbTmpXmlBuf);
						return true;
					}
					else
					{
						nBufLen = cCmdRespond.dwInfoSize;
						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE(pbTmpXmlBuf);
						return false;
					}
				}
				else
				{
					nBufLen = 0;
					ForceCloseSocket(hSocketCmd);
					SAFE_DELETE(pbTmpXmlBuf);
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
	SAFE_DELETE(pbTmpXmlBuf);
	return false;
}



const int MAX_WITHE_BLACK_LENGTH= (1024<<10) * 2 + 1024;

bool ExecXmlExtCmdMercury(char* szIP, char* szXmlCmd, char* szRetBuf, int& iBufLen, int sktSend)
{

	if ( strlen(szXmlCmd) > MAX_WITHE_BLACK_LENGTH)
	{
		return false;
	}


	char sLog[256];
	const int iMaxLen = (1024 << 10);
	char* pszTmpXmlBuf = new char[iMaxLen];
	if(pszTmpXmlBuf == NULL)
		return false;

	memset(pszTmpXmlBuf, 0, iMaxLen);
	bool fIsTempSocket = true;
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	int hSocketCmd = INVALID_SOCKET;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if(sktSend == INVALID_SOCKET)
	{
		if ( strstr(szXmlCmd, "AutoTestCamera") )
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
				SAFE_DELETE_ARG(pszTmpXmlBuf);
				return false;
			}

			//设置超时
			struct timeval tv;
            tv.tv_sec =  20;
            tv.tv_usec= 20;
            //if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval))  != 0 )
             //       return -1;

            if ( setsockopt(hSocketCmd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval))  != 0 )
                    return -1;


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


	bool fIsOk = false;
	cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	cCmdHeader.dwInfoSize = (int)strlen(szXmlCmd)+1;
	if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader),  MSG_NOSIGNAL) )
	{
		fIsOk = false;
		if (  fIsTempSocket  )
            ForceCloseSocket(hSocketCmd);
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return fIsOk;
	}
	if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlCmd, cCmdHeader.dwInfoSize,  MSG_NOSIGNAL) )
	{

		fIsOk = false;

		if (  fIsTempSocket  )
            ForceCloseSocket(hSocketCmd);
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return fIsOk;
	}
	if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond),  MSG_NOSIGNAL) )
	{

		if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID	&& 0 == cCmdRespond.dwResult && 0 < cCmdRespond.dwInfoSize )
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

					if (  fIsTempSocket  )
                        ForceCloseSocket(hSocketCmd);
                    SAFE_DELETE_ARG(pszTmpXmlBuf);
                    return fIsOk;
				}
			}
			else
			{
				iBufLen = 0;
				fIsOk = false;

				if (  fIsTempSocket  )
                    ForceCloseSocket(hSocketCmd);
                SAFE_DELETE_ARG(pszTmpXmlBuf);
                return fIsOk;
			}
		}
		fIsOk = true;
	}


	TiXmlDocument cXmlDocReceived;
    TiXmlDocument cResultXmlDoc;
    TiXmlPrinter cPrinter;

	// ×éÖ¯ÉÏÎ»»úXMLÍ·
    TiXmlDeclaration* pResultDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    TiXmlElement* pResultRootElement = new TiXmlElement("HvCmdRespond");
    if ( NULL == pResultDecl || NULL == pResultRootElement)
    {
        fIsOk = false;
		//sprintf(sLog, "ExecXmlExtCmdMercury out of memory for pResultDecl or pResultRootElement");
		//WrightLogEx(szIP, sLog);

		if (  fIsTempSocket  )
            ForceCloseSocket(hSocketCmd);
		SAFE_DELETE_ARG(pszTmpXmlBuf);
		return fIsOk;

    }
    pResultRootElement->SetAttribute("ver", "2.0"); // //TODO: Èç¹û¸ÄÎª3.0£¬»á¶ÔÉÏÎ»»úÔì³ÉÊ²Ã´Ó°Ïì£¿  ÓÐÓ°Ïì 3.0Ö±½Ó²»½âÎö

	// ½âÎöÉè±¸½á¹ûXML²¢×é×°ÉÏÎ»»úXML
	if ( cXmlDocReceived.Parse(pszTmpXmlBuf) )
	{
		TiXmlElement* pCurElement = cXmlDocReceived.FirstChildElement("HvCmdRespond");
		if ( pCurElement == NULL )
		{
			//sprintf(sLog, "ExecXmlExtCmdMercury can't find \"HvCmdRespond\" element");
			//WrightLogEx(szIP, sLog);
	        fIsOk = false;

	        if (  fIsTempSocket  )
                ForceCloseSocket(hSocketCmd);
            SAFE_DELETE_ARG(pszTmpXmlBuf);
            return fIsOk;
		}

		const char* szVerAttrib = pCurElement->Attribute("Ver");
		if ( (szVerAttrib==NULL) || (0 != strcmp(szVerAttrib, "3.0")) )
		{
			//sprintf(sLog, "ExecXmlExtCmdMercury can't find \"Ver\" attribute with value \"3.0\"");
			//WrightLogEx(szIP, sLog);
	        fIsOk = false;

	        if (  fIsTempSocket  )
                ForceCloseSocket(hSocketCmd);
            SAFE_DELETE_ARG(pszTmpXmlBuf);
            return fIsOk;
		}

		pCurElement = pCurElement->FirstChildElement(); // //TODO: ÈçºÎÅÐ¶Ï"CmdName"
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
				//sprintf(sLog, "ExecXmlExtCmdMercury out of memory for pElementRet");
				//WrightLogEx(szIP, sLog);

				if (  fIsTempSocket  )
                    ForceCloseSocket(hSocketCmd);
                SAFE_DELETE_ARG(pszTmpXmlBuf);
                return fIsOk;
		    }

			if(pszCmdName = pCurElement->GetText())
			{
				//sprintf(sLog, "ExecXmlExtCmdMercury received response from command %s", pszCmdName);
				//WrightLogEx(szIP, sLog);
				pszRetCode = pCurElement->Attribute("RetCode");
				pszRetMsg = pCurElement->Attribute("RetMsg");
				if(!pszRetMsg)
				{
					pszRetMsg = "";
				}
			//	if(strcmp(pszCmdName, "GetConnectedIP") == 0)
				//{
				//	printf("aaaaaa");
				//}

				//Ë®ÐÇ
				//if(pszRetMsg && pszRetCode)
				if(pszRetMsg || pszRetCode)
				{
					// ×é×°ÉÏÎ»»úXML
					int iCmdIndex = SearchXmlCmdMapMercury(pszCmdName);
					if (-1 == iCmdIndex)
					{ // Ã»ÕÒµ½¶ÔÓ¦µÄÃüÁî
						fCmdTransformed = false;
						//sprintf(sLog, "ExecXmlExtCmdMercury can't find command [%s]", pszCmdName);
						//WrightLogEx(szIP, sLog);
					}
					else
					{
						TiXmlText *pXmlCmdText = new TiXmlText(pszCmdName);
						if (NULL == pXmlCmdText)
						{
					        fIsOk = false;
							//sprintf(sLog, "ExecXmlExtCmdMercury out of memory for pXmlCmdText");
							//WrightLogEx(szIP, sLog);

							if (  fIsTempSocket  )
                                ForceCloseSocket(hSocketCmd);
                            SAFE_DELETE_ARG(pszTmpXmlBuf);
                            return fIsOk;

					    }
					    pElementRet->LinkEndChild(pXmlCmdText);


						strcpy(szRetCode, pszRetCode);
						if (0 == strcmp("0", pszRetCode))
						{
							switch ( g_XmlCmdAppendInfoMap[iCmdIndex].emCmdClass )
							{
								case XML_CMD_GETTER: // Îª½á¹ûÊýÖµÌíÉÏÃû³Æ
									switch ( g_XmlCmdAppendInfoMap[iCmdIndex].emCmdType )
									{
										case XML_CMD_TYPE_INTARRAY1D: // //TODO: 2DÊý×éµÄ´¦Àí
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
														{ // ×îºóÒ»¸ö×Ö¶Î
															pElementRet->SetAttribute(
																*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),
																pszRetMsgBuf);
															fCmdTransformed = true;
														}
														else
														{ // ¼È²»ÊÇ×îºóÒ»¸ö×Ö¶Î£¬ÓÖ²»°üº¬¡°£¬¡±
															strcpy(szRetCode, "-1");
															fCmdTransformed = false;
															break;
														}
													}
												}
												SAFE_DELETE(pszRetMsgBufBackup);
											}
											break;
										case XML_CMD_TYPE_INT:
										case XML_CMD_TYPE_DOUBLE:
										case XML_CMD_TYPE_FLOAT:
										case XML_CMD_TYPE_BOOL:
										case XML_CMD_TYPE_DWORD:
										case XML_CMD_TYPE_STRING:

											pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, pszRetMsg);
											fCmdTransformed = true;
											break;
										case XML_CMD_TYPE_CUSTOM:

											if (strcmp(pszCmdName, "ReadFPGA") == 0)
											{
												pElementRet->SetAttribute(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1, pszRetMsg);
												fCmdTransformed = true;
											}
											if(strcmp(pszCmdName,"DateTime") == 0)
											{
												char szDate[16] = {0};
												char szTime[16] = {0};
												char* pDateBegin = (char*)strstr( pszRetMsg , "Date[" );
												if ( NULL != pDateBegin )
												{
													pDateBegin+=5;
												}
												char* pDateEnd = (char*)strstr( pszRetMsg , "]" );
												char* pTimeBegin = (char*)strstr( pszRetMsg , "Time[" );
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
													sprintf(szUserNameKeyName, "UserName%2d", i);
													sprintf(szUserAuthorityKeyName, "UserAuthority%2d", i);
													sprintf(szUserNameBeginLabel, "%s=[",szUserNameKeyName);
													sprintf(szUserAuthorityBeginLabel, "%s=[",szUserAuthorityKeyName);

													//¶ÁÈ¡ÓÃ»§Ãû
													char* pUserNameBegin = (char*)strstr( pszRetMsg , szUserNameBeginLabel );
													if (pUserNameBegin!=NULL)
													{
														pUserNameBegin+=strlen(szUserNameBeginLabel);
													}
													char* pUserNameEnd=strstr(pUserNameBegin, "]");
													if ( NULL != pUserNameEnd )
													{
														memcpy( szUserNameValue ,pUserNameBegin , pUserNameEnd-pUserNameBegin );
													}
													pElementRet->SetAttribute(szUserNameKeyName, szUserNameValue);


													//¶ÁÈ¡ÓÃ»§µÈ¼¶
													char* pUserAuthorityBegin=(char*)strstr(pszRetMsg, szUserAuthorityBeginLabel);
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
															*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),//ÕâÀïÓÐ¸öµØÖ·Æ«ÒÆ·ÃÎÊ£¬ÈÃÊôÐÔÎ»ÖÃÏëºóÒÆ¶¯
															szResult);
													}
													else
													{
														pElementRet->SetAttribute(
															*(&(g_XmlCmdAppendInfoMap[iCmdIndex].szParamName1) + i),//ÕâÀïÓÐ¸öµØÖ·Æ«ÒÆ·ÃÎÊ£¬ÈÃÊôÐÔÎ»ÖÃÏëºóÒÆ¶¯
															"");
													}
												}
											}

											fCmdTransformed = true;
											break;
									}
									break;
								case XML_CMD_SETTER: // Setter ²»ÐèÒª×éÖ¯ÓÐvalueµÄXML·µ»Ø
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
				//else
				//{
				//	sprintf(sLog, "ExecXmlExtCmdMercury command [%s] has no RetMsg or RetCode", pszCmdName);
				//	WrightLogEx(szIP, sLog);
				//}

			}

			if (fCmdTransformed)
			{
				pResultRootElement->LinkEndChild(pElementRet);
			}
			else
			{
				SAFE_DELETE(pElementRet);
			}

			pCurElement = pCurElement->NextSiblingElement();
		}
	}

	// ½«½á¹ûXML×ª»»³É×Ö·û´®Êä³ö
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


	if(fIsTempSocket)
	{
		// //TODO: Ë®ÐÇµÄSOCKETÔÚÊý¾Ý½»»¥Íê³Éºó¡¢¹Ø±ÕSOCKETÖ®Ç°£¬ÓÐ¿ÉÄÜÐèÒªÔÙ½øÐÐÒ»´ÎREAD/SEND²Ù×÷²ÅÄÜ±£Ö¤Êý¾ÝÕæÕý·¢ËÍÍê³É
		ForceCloseSocket(hSocketCmd);
	}

	SAFE_DELETE_ARG(pszTmpXmlBuf);
	return fIsOk;
}






static int HvMakeXmlCmdReq(char *szXmlBuf, char *szCmdName, int nArgCount, char ** szArgID, char **szArgValue)
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

static int HvMakeXmlCmdReq2(const XmlCmdList &listXmlCmd, char *szXmlBuf)
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

		//ÃÃ¼ÃÃ®ÃÃ»Â³Ã
		TiXmlText *pXmlCmdText = new TiXmlText(iter->strCmdName.c_str());
		pXmlElementCmd->LinkEndChild(pXmlCmdText);

		//Â²ÃÃÃ½(ÃÃ´ÃÃ)
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


static int HvMakeXmlCmdReq3(const XmlCmdListMercury& listXmlCmd, char* szXmlBuf)
{
	TiXmlDocument doc;
	TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
	doc.LinkEndChild(pDecl);

	TiXmlElement* pXmlRootElement = new TiXmlElement("HvCmd");
	pXmlRootElement->SetAttribute("ver", "3.0");

	doc.LinkEndChild(pXmlRootElement);

	std::vector<SXmlCmdMercury>::const_iterator iter = listXmlCmd.begin();
	for(; iter!=listXmlCmd.end(); ++iter)
	{
		TiXmlElement *pXmlElementCmd = new TiXmlElement("CmdName");
		pXmlRootElement->LinkEndChild(pXmlElementCmd);

		//ÃüÁîÃû³Æ
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
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
				}
				else
				{
					if (strcmp("ReadFPGA", iter->strCmdName.c_str()) == 0)//TODO(liyh) ÁÙÊ±·½°¸£¬ÐèÔÚ
					{
						sprintf(szValue, iter->listAttr[0].strValue.c_str());
					}

				}
				break;
			case XML_CMD_TYPE_INT:
				sprintf(szType, "INT");
				if (iter->emCmdClass == XML_CMD_SETTER)
				{
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
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
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
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
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
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
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
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
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
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
					sprintf(szValue, iter->listAttr[0].strValue.c_str());
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
						szValue[strlen(szValue) -1] = '\0'; // È¥µô×îºóÒ»¸ö¶ººÅ
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
					szValue[strlen(szValue)] = '\0'; // È¥µô×îºóÒ»¸ö¶ººÅ
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

	TiXmlPrinter printer;
	doc.Accept(&printer);

	int iLen = (int)printer.Size();
	memcpy(szXmlBuf, printer.CStr(), iLen);
	szXmlBuf[iLen] = '\0';

	return iLen;
}


/*
HRESULT HvMakeXmlCmdByString2(const char* inXmlOrStrBuf,  int nInlen, char *szOutXmlBuf, int &nOutlen)
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
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2Â±Ã­ÃÅ¸Â±ÃÃÃ¨ÃÂªÃÃÃÂ»Å¾Ã¶ÃÃÂ·Ã»
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2Â±Ã­ÃÅ¸Â±ÃÃÃ¨ÃÂªÃÃÃÂ»Å¾Ã¶ÃÃÂ·Ã»
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
	}

	nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
	SAFE_DELETE_ARG(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}
*/

HRESULT HvMakeXmlCmdByString(bool fNewProtocol, const char* inXmlOrStrBuf,  int nInlen, char *szOutXmlBuf, int &nOutlen)
{
   	if(fNewProtocol)
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
	// ÃÂªÅ½Å½ÅÅ¡XmlÃÅÂ±Å¾Â²ÃÃÃ½Â¿ÃÅÃ¤
	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	int nCount = 0;
	char* pCmdName = pszInBufCopy;
	// ÅÃÂ¶Å¡ÃÃ¼ÃÃ®ÂµÃÂ²ÃÃÃ½Å¾Ã¶ÃÃ½Â²Â»Â³Â¬Â¹Ã½64Å¾Ã¶Â¡Â£
	char* pValueID[64];
	char* pValueText[64];
	char* pTemp;
	// ÃÃÂ¶ÃÅÂ°ÅÃ¢ÃÃ¶XmlÃÃ ÃÃÃÃÂ·Ã»Å½Â®
	while( pszInBufCopy = strchr(pszInBufCopy, ',') )
	{
		*pszInBufCopy = '\0';
		pValueID[nCount] = pszInBufCopy + 1;
		if ( ((pTemp = strchr(pszInBufCopy+2, '[')) == NULL)     // +2Â±Ã­ÃÅ¸Â±ÃÃÃ¨ÃÂªÃÃÃÂ»Å¾Ã¶ÃÃÂ·Ã»
			|| ((pszInBufCopy = strchr(pTemp+2, ']')) == NULL) ) // +2Â±Ã­ÃÅ¸Â±ÃÃÃ¨ÃÂªÃÃÃÂ»Å¾Ã¶ÃÃÂ·Ã»
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
		if (!fNewProtocol)
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


HRESULT HvSendXmlCmd(char *szIP, LPCSTR szCmd, LPSTR szRetBuf, INT iBufLen, INT *piRetLen, int sktSend)
{
    if ( NULL == szIP || NULL == szCmd || NULL == szRetBuf || iBufLen <=0)
        return E_FAIL;
    const int iMaxLen = (1024 << 4);
    char *pszXmlBuf = new char[iMaxLen];
    int iXmlBufLen = iMaxLen;
    if ( pszXmlBuf == NULL )
        return E_FAIL;

    memset(pszXmlBuf, 0, iMaxLen);
    TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // æ£æ¥szCmdæ¯å¦æ¯XML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(IsNewProtocol(szIP), szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE(pszXmlBuf);
			return E_FAIL;
		}
		szCmd = (char*)pszXmlBuf;
	}
    bool fRet = ExecXmlExtCmdEx(szIP, (char*)szCmd, (char *)szRetBuf, iBufLen,sktSend);
    if ( piRetLen )
        *piRetLen = iBufLen;
    SAFE_DELETE_ARG(pszXmlBuf);
    return (true == fRet) ? S_OK : E_FAIL;
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
  if (0 == strcmp(pAttrib->Value(), "1.0"))   // é»è®¤è®¤ä¸ºç¬¬ä¸ä¸ªå±æ§å°±æ¯ver
  { // æ¥æ¾è¿åå¼
   pCurElement = pCurElement->FirstChildElement("RetCode");
   assert(pCurElement != NULL);
   pszTextGet = pCurElement->GetText();
  }
  else if (0 == strcmp(pAttrib->Value(), "2.0"))   // é»è®¤è®¤ä¸ºç¬¬ä¸ä¸ªå±æ§å°±æ¯ver
  {
   for (;pCurElement; pCurElement = pCurElement->NextSiblingElement())
   {
    if (pCurElement->Attribute("RetCode") && pCurElement->GetText()
    && !strcasecmp(pCurElement->GetText(), szCommand))
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

HRESULT HvParseXmlCmdRespRetcode2(char*szXmlBuf, char* szCmdName,
char* szCmdValueName, char* szCmdValueText)
{
    if ( szXmlBuf == NULL ||szCmdName == NULL || szCmdValueName == NULL ||  szCmdValueText== NULL )
        return E_FAIL;

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szXmlBuf) == NULL  )
        return E_FAIL;

    const char* pszTextGet = NULL;
    TiXmlElement *pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
    if ( pCurElement == NULL )
        return E_FAIL;
    TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
    if ( pAttrib == NULL )
        return E_FAIL;
    if ( strcmp(pAttrib->Value(), "2.0") != 0 )
        return E_FAIL;

    HRESULT hr = E_FAIL;
    pCurElement = pCurElement->FirstChildElement();
    while ( pCurElement )
    {
        if ( strcmp( pCurElement->GetText(), szCmdName) == 0 )
        {
            pszTextGet = pCurElement->Attribute(szCmdValueName);
            if ( pszTextGet )
            {
                memcpy(szCmdValueText, pszTextGet, strlen(pszTextGet)+1);
                hr = S_OK;
                break;
            }
        }
    }
    return hr;
}

HRESULT HvParseXmlInfoRespValue(char*szXmlBuf, char* szInfoName,
char* nInfoValueName, char* szInfoValueText)
{
    HRESULT rst = E_FAIL;
    if ( szXmlBuf == NULL )
        return rst;

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szXmlBuf) == NULL )
        return rst;

    const char *szTextGet = NULL;
    TiXmlElement *pCurElement = cXmlDoc.FirstChildElement("HvInfoRespond");
    if ( pCurElement == NULL )
        return rst;
    TiXmlAttribute *pAttrib = pCurElement->FirstAttribute();
    if ( pAttrib == NULL || 0 != strcmp(pAttrib->Value(), "1.0") )
        return rst;

    for (pCurElement = pCurElement->FirstChildElement("Info"); pCurElement != NULL;
    pCurElement = pCurElement->NextSiblingElement("Info"))
    {
        pAttrib = pCurElement->FirstAttribute();
        szTextGet = pAttrib->Value();
        if ( strncasecmp(szInfoName, szTextGet, strlen(szTextGet)+1) == 0 )
        {
            TiXmlElement *pCurValueElement = NULL;
            for (pCurValueElement= pCurElement->FirstChildElement("Value"); pCurValueElement != NULL;
                    pCurValueElement = pCurValueElement->NextSiblingElement("Value"))
            {
                  pAttrib = pCurValueElement->FirstAttribute();
                 szTextGet = pAttrib->Value();
                 if ( strncasecmp(nInfoValueName, szTextGet, strlen(szTextGet)+1) == 0 )
                 {
                     szTextGet = pCurValueElement->GetText();
                     if ( szTextGet == NULL )
                        return rst;
                    memcpy(szInfoValueText, szTextGet, strlen(szTextGet)+1);
                    rst = S_OK;
                    break;
                 }
            }//FOR
        }//if
    }//for

    return rst;
}

static int HvMakeXmlInfoReq(char* szXmlBuf, int nInfoCount,
char** szInfoName, char** szInfoText )
{
    TiXmlDocument doc;
    TiXmlDeclaration *pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    doc.LinkEndChild(pDecl);

    TiXmlElement *pXmlRootElement = new TiXmlElement("HvInfo");
    pXmlRootElement->SetAttribute("ver", "1.0");
    doc.LinkEndChild(pXmlRootElement);
    for  ( int i=0; i<nInfoCount; i++ )
    {
        TiXmlElement *pXmlElementInfo = new TiXmlElement("Info");
        pXmlRootElement->LinkEndChild(pXmlElementInfo);

        pXmlElementInfo->SetAttribute("name", szInfoName[i]);
        if ( (szInfoText == NULL ) || (szInfoText[i] == NULL ))
            continue ;
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

HRESULT HvMakeXmlInfoByString(bool fNewProtocol, const char*inXmlOrStrBuf, int nInlen,
char* szOutXmlBuf, int &nOutlen)
{
    bool bGet = false;
    char* pszInBufCopy = new char[nInlen+1];
    char* pszInBufCopyOld = pszInBufCopy;
    if ( pszInBufCopy == NULL )
        return E_FAIL;
    memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
    pszInBufCopy[nInlen] = '\0';
    int nCount = 1;
    char* pInfoName[64] = {pszInBufCopy};
    while( pszInBufCopy = strchr(pszInBufCopy, ','))
    {
        *pszInBufCopy = '\0';
        pszInBufCopy++;
        if ( *pszInBufCopy == '\0')
            break;
        pInfoName[nCount] = pszInBufCopy;
        nCount++;
    }//while

    if ( !fNewProtocol )
    {
        nOutlen = HvMakeXmlInfoReq(szOutXmlBuf, nCount, pInfoName, NULL );
    }
    else
    {
        XmlCmdList listXmlCmd;
        for (int i=0; i<nCount; ++i )
        {
            if ( pInfoName[i] )
            {
                SXmlCmd sXmlCmd;
                sXmlCmd.strCmdName = pInfoName[i];
                listXmlCmd.push_back(sXmlCmd);
            }
        }//for
        nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
    }
    delete[] pszInBufCopyOld;
    pszInBufCopyOld = NULL;
    return S_OK;
}

bool ExecXmlExtCmdEx(char *szIP, char *szXmlCmd, char *szRetBuf, int &iBufLen, int sktSend)
{
    if ( szIP == NULL || szXmlCmd == NULL )
        return false;

    if ( strlen(szXmlCmd) > (1024 << 4) )
        return false;

    bool fIsTempSocket = true;
    const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hSocketCmd = INVALID_SOCKET;
    CAMERA_CMD_HEADER cmdHearder;
    CAMERA_CMD_RESPOND cmdRespond;
    if ( sktSend == INVALID_SOCKET )
    {
        if ( !ConnectCamera(szIP, wPort, hSocketCmd) )
            return false;
        sktSend = hSocketCmd;
    }
    else
    {
        hSocketCmd = sktSend;
        fIsTempSocket = false;
    }

    struct timeval tv;
    tv.tv_sec = 12;
    tv.tv_usec=0;
    setsockopt(hSocketCmd, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval));
    bool fIsOK = false;
    cmdHearder.dwID = CAMERA_XML_EXT_CMD;
    cmdHearder.dwInfoSize = (int )strlen(szXmlCmd) + 1;
    int iHeaderLen = sizeof(cmdHearder);
    if ( iHeaderLen  !=  send( hSocketCmd, (const char*)&cmdHearder, iHeaderLen, MSG_NOSIGNAL) )
    {
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    int iValue = send( hSocketCmd, (const char*)szXmlCmd, cmdHearder.dwInfoSize, MSG_NOSIGNAL);
    if ( cmdHearder.dwInfoSize  !=   iValue )
    {
        int error = errno;
        perror("Send");
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    int iRespondLen = sizeof(cmdRespond);
    if (  iRespondLen != RecvAll(hSocketCmd, (char*)&cmdRespond, iRespondLen) )
    {
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }
    if ( CAMERA_XML_EXT_CMD  != cmdRespond.dwID  ||  0 != cmdRespond.dwResult  ||  cmdRespond.dwInfoSize < 0 )
    {
         if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        return false;
    }

    const int iMaxLen = (1024 << 10 );
    char *pszTmpXmlBuf = new char[iMaxLen];
     if ( pszTmpXmlBuf == NULL )
        return false;
    memset(pszTmpXmlBuf, 0, iMaxLen);

    int iRetLen = RecvAll(hSocketCmd, pszTmpXmlBuf, cmdRespond.dwInfoSize);
    if ( cmdRespond.dwInfoSize != iRetLen )
    {
        iBufLen = 0;
        if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(pszTmpXmlBuf);
        return false;
    }

    if ( iBufLen >  (int)cmdRespond.dwInfoSize )
    {
        memcpy(szRetBuf, pszTmpXmlBuf, cmdRespond.dwInfoSize );
        szRetBuf[cmdRespond.dwInfoSize] = 0;
        iBufLen = cmdRespond.dwInfoSize;
        fIsOK = true;
    }
    else
    {
        int iTmpLen =  iBufLen-1;
        memcpy(szRetBuf, pszTmpXmlBuf, iTmpLen );
        szRetBuf[iTmpLen] = 0;
        fIsOK = true;
    }

      if ( fIsTempSocket)
            ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(pszTmpXmlBuf);

    return  fIsOK;
}


void HvSafeCloseThread(pthread_t  &pthreadHandle)
{
    if ( pthreadHandle !=  0 )
    {
        pthread_join(pthreadHandle, NULL);
        pthreadHandle = 0;
    }
}

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
    if ( pcRecordImage == NULL || pbRecordData == NULL || dwRecordDataLen < 0 )
        return E_FAIL;

    IMAGE_INFO cImageInfo;
    DWORD dwImgInfoLen;
    DWORD dwImgDataLen;
    PBYTE pbTempData = pbRecordData;
    PBYTE pbTempInfo = NULL;
    DWORD dwRemainLen = dwRecordDataLen;
    memset(pcRecordImage, 0, sizeof(RECORD_IMAGE_GROUP_EX));
    while ( dwRemainLen > 0 )
    {
        memcpy(&dwImgInfoLen, pbTempData, 4);
        pbTempData+=4;
        if  ( dwImgInfoLen > dwRemainLen )
            return E_FAIL;

        memcpy(&cImageInfo, pbTempData, dwImgInfoLen);
        pbTempInfo = pbTempData;
        pbTempData += dwImgInfoLen;

        memcpy(&dwImgDataLen, pbTempData, 4 );
        pbTempData+=4;
        if ( dwImgDataLen > dwRemainLen )
            return E_FAIL;

        switch (cImageInfo.dwImgType)
        {
            case RECORD_IMAGE_BEST_SNAPSHOT:
                {
                    pcRecordImage->cBestSnapshot.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cBestSnapshot.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cBestSnapshot.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs<<=32;
                    pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cBestSnapshot.pbImgData = pbTempData;
                    pcRecordImage->cBestSnapshot.pbImgInfo = pbTempInfo;
                    pcRecordImage->cBestSnapshot.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cBestSnapshot.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cBestSnapshot.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cBestSnapshot.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));

                }
            break;
            case RECORD_IMAGE_LAST_SNAPSHOT:
             {
                    pcRecordImage->cLastSnapshot.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cLastSnapshot.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cLastSnapshot.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cLastSnapshot.pbImgData = pbTempData;
                    pcRecordImage->cLastSnapshot.pbImgInfo = pbTempInfo;
                    pcRecordImage->cLastSnapshot.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cLastSnapshot.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cLastSnapshot.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));
                }
            break;

            case RECORD_IMAGE_BEGIN_CAPTURE:
             {
                    pcRecordImage->cBeginCapture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cBeginCapture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cBeginCapture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cBeginCapture.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cBeginCapture.pbImgData = pbTempData;
                    pcRecordImage->cBeginCapture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cBeginCapture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cBeginCapture.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cBeginCapture.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cBeginCapture.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));
                }
            break;
            case RECORD_IMAGE_BEST_CAPTURE:
             {
                    pcRecordImage->cBestCapture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cBestCapture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cBestCapture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cBestCapture.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cBestCapture.pbImgData = pbTempData;
                    pcRecordImage->cBestCapture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cBestCapture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cBestCapture.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cBestCapture.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cBestCapture.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));

                }
            break;
            case RECORD_IMAGE_LAST_CAPTURE:
             {
                    pcRecordImage->cLastCapture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cLastCapture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cLastCapture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;

                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.top = cImageInfo.rcPlate.top;
                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.left = cImageInfo.rcPlate.left;
                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.bottom = cImageInfo.rcPlate.bottom;
                    pcRecordImage->cLastCapture.cImgInfo.rcPlate.right = cImageInfo.rcPlate.right;

                    pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cLastCapture.pbImgData = pbTempData;
                    pcRecordImage->cLastCapture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cLastCapture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cLastCapture.dwImgInfoLen = dwImgInfoLen;

                    pcRecordImage->cLastCapture.cImgInfo.nFaceCount = cImageInfo.nFaceCount;
                    memcpy( pcRecordImage->cLastCapture.cImgInfo.rcFacePos, cImageInfo.rcFacePos, sizeof(cImageInfo.rcFacePos));

                }
            break;
            case RECORD_IMAGE_SMALL_IMAGE:
             {
                    pcRecordImage->cPlatePicture.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cPlatePicture.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cPlatePicture.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;
                      pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs <<=32;
                    pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cPlatePicture.pbImgData = pbTempData;
                    pcRecordImage->cPlatePicture.pbImgInfo = pbTempInfo;
                    pcRecordImage->cPlatePicture.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cPlatePicture.dwImgInfoLen = dwImgInfoLen;
                }
            break;
            case RECORD_IMAGE_BIN_IMAGE:
             {
                    pcRecordImage->cPlateBinary.cImgInfo.dwCarID = cImageInfo.dwCarID;
                    pcRecordImage->cPlateBinary.cImgInfo.dwHeight = cImageInfo.dwImgHeight;
                    pcRecordImage->cPlateBinary.cImgInfo.dwWidth = cImageInfo.dwImgWidth;
                    pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs = cImageInfo.dwTimeHigh;
                    pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs <<= 32;
                    pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs |=cImageInfo.dwTimeLow;
                    pcRecordImage->cPlateBinary.pbImgData = pbTempData;
                    pcRecordImage->cPlateBinary.pbImgInfo = pbTempInfo;
                    pcRecordImage->cPlateBinary.dwImgDataLen = dwImgDataLen;
                    pcRecordImage->cPlateBinary.dwImgInfoLen = dwImgInfoLen;
                }
            break;
            default :
            break;
        }
        pbTempData+=dwImgDataLen;
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
	if(!szAppendInfo /*|| !pbRecordData || dwRecordDataLen <= 0*/)
	{
		return E_FAIL;
	}

	// ŽÓszAppendInfoÖÐœâ³öResultExtInfo¡¢Resultœáµã
	TiXmlElement* pExtInfoEle = NULL;
	TiXmlElement* pResultEle = NULL;
	TiXmlDocument cXmlDoc;
	if (cXmlDoc.Parse(szAppendInfo))
	{
		TiXmlElement* pEleRoot = cXmlDoc.RootElement();
		if (NULL == pEleRoot)
		{
			//WrightLogEx("test IP", "HvGetRecordImage_Mercury pEleRoot == NULL");
			return E_FAIL;
		}

		pExtInfoEle = pEleRoot->FirstChildElement("ResultExtInfo");
		if (NULL == pExtInfoEle)
		{
			//WrightLogEx("test IP", "HvGetRecordImage_Mercury pExtInfoEle == NULL");
			return E_FAIL;
		}

		TiXmlElement* pResultSetEle = pEleRoot->FirstChildElement("ResultSet");
		if (NULL == pResultSetEle)
		{
			//WrightLogEx("test IP", "HvGetRecordImage_Mercury pResultSetEle == NULL");
			return E_FAIL;
		}

		pResultEle = pResultSetEle->FirstChildElement("Result");
		if (NULL == pResultEle)
		{
			//WrightLogEx("test IP", "HvGetRecordImage_Mercury pResultEle == NULL");
			return E_FAIL;
		}
	}
	else
	{
		//WrightLogEx("test IP", "HvGetRecordImage_Mercury szAppendInfo can't be parsed");
		return E_FAIL;
	}

	// œâÎöpExtInfoEleÖÐž÷ÍŒÆ¬µÄÐÅÏ¢
	TiXmlElement * pBestSnapShotEle = pExtInfoEle->FirstChildElement("Image0");
	TiXmlElement * pLastSnapShotEle = pExtInfoEle->FirstChildElement("Image1");
	TiXmlElement * pBeginCaptureEle = pExtInfoEle->FirstChildElement("Image2");
	TiXmlElement * pBestCaptureEle  = pExtInfoEle->FirstChildElement("Image3");
	TiXmlElement * pLastCaptureEle  = pExtInfoEle->FirstChildElement("Image4");
	TiXmlElement * pPlateEle         = pExtInfoEle->FirstChildElement("Image5");
	TiXmlElement * pPlateBinEle      = pExtInfoEle->FirstChildElement("Image6");
	TiXmlElement * pFaceRectElement		 = NULL;

	// œâÎöCarID
	TiXmlElement* pCarIDEle = pResultEle->FirstChildElement("CarID");
	if (NULL == pCarIDEle)
	{
		//WrightLogEx("test IP", "HvGetRecordImage_Mercury pCarIDEle == NULL");
		return E_FAIL;
	}
	const char * szCarID = pCarIDEle->Attribute("value");
	if (NULL == szCarID)
	{
		//WrightLogEx("test IP", "HvGetRecordImage_Mercury szCarID == NULL");
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
		//WrightLogEx("test IP", "HvGetRecordImage_Mercury pPlateNameEle == NULL");
		return E_FAIL;
	}
	const char * szPlateName = pPlateNameEle->GetText();
	if (NULL == szPlateName)
	{
		//WrightLogEx("test IP", "HvGetRecordImage_Mercury szPlateName == NULL");
		return E_FAIL;
	}
	char szLog[1024];
	sprintf(szLog, "Received record[%d] %s with images[%s] in XML", dwCarID, szPlateName, szImages);
	//WrightLogEx("Test IP", szLog);

	//FILE* fp = NULL;
	//int i = 0;
	memset(szImages, 0, sizeof(szImages));
	while(dwRemainLen > 0)
	{

		memcpy(&cImgInfo, pbTempData, sizeof(cImgInfo));
		pbTempData += sizeof(cImgInfo);
		if(cImgInfo.dwImgLength > dwRemainLen)
		{
			sprintf(szLog, "HvGetRecordImage_Mercury cImgInfo.dwImgLength[%d] > dwRemainLen[%d]", cImgInfo.dwImgLength, dwRemainLen);
			//WrightLogEx("test IP", szLog);
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
			//WrightLogEx("test IP", szImages);
			if (NULL == pBestSnapShotEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pBestSnapShotEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cBestSnapshot.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cBestSnapshot.cImgInfo.dwHeight = GetValueFromAttribute(pBestSnapShotEle->Attribute("Height"));
			pcRecordImage->cBestSnapshot.cImgInfo.dwWidth = GetValueFromAttribute(pBestSnapShotEle->Attribute("Width"));
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.top = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosTop"));
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.left = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosLeft"));
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosBottom"));
			pcRecordImage->cBestSnapshot.cImgInfo.rcPlate.right = GetValueFromAttribute(pBestSnapShotEle->Attribute("PlatePosRight"));
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs = GetValueFromAttribute(pBestSnapShotEle->Attribute("TimeHigh"));
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cBestSnapshot.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pBestSnapShotEle->Attribute("TimeLow"));
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
			//WrightLogEx("test IP", szImages);
			if (NULL == pLastSnapShotEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pLastSnapShotEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cLastSnapshot.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cLastSnapshot.cImgInfo.dwHeight = GetValueFromAttribute(pLastSnapShotEle->Attribute("Height"));
			pcRecordImage->cLastSnapshot.cImgInfo.dwWidth = GetValueFromAttribute(pLastSnapShotEle->Attribute("Width"));
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.top = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosTop"));
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.left = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosLeft"));
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosBottom"));
			pcRecordImage->cLastSnapshot.cImgInfo.rcPlate.right = GetValueFromAttribute(pLastSnapShotEle->Attribute("PlatePosRight"));
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs = GetValueFromAttribute(pLastSnapShotEle->Attribute("TimeHigh"));
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cLastSnapshot.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pLastSnapShotEle->Attribute("TimeLow"));
			pcRecordImage->cLastSnapshot.pbImgData = pbTempData;
			//pcRecordImage->cLastSnapshot.pbImgInfo = pbTempInfo;
			pcRecordImage->cLastSnapshot.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cLastSnapshot.dwImgInfoLen = dwImgInfoLen;
			pcRecordImage->cLastSnapshot.cImgInfo.nFaceCount = min( GetValueFromAttribute(pLastSnapShotEle->Attribute("FaceCount")), sizeof(pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos) / sizeof(RECT) ); // Ö»ÓÐÕâÕÅÍŒÓÐÈËÁ³×ø±ê
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
						//WrightLogEx("test IP", szLog);
						break;
					}

					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].bottom = GetValueFromAttribute(pFaceEle->Attribute("Bottom"));
					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].left = GetValueFromAttribute(pFaceEle->Attribute("Left"));
					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].right = GetValueFromAttribute(pFaceEle->Attribute("Right"));
					pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos[i].top = GetValueFromAttribute(pFaceEle->Attribute("Top"));
				}
			}
			//else
			//{
			//	WrightLogEx("test IP", " Not Find  FaceInfo\n");
			//}

			//if (cImgInfo.nFaceCount != 0)
			//{
			//	int iFlag = 0;
			//}
			//memcpy(pcRecordImage->cLastSnapshot.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_BEGIN_CAPTURE:
			sprintf(szImages, "%s %d-%d", szImages, 2, cImgInfo.dwImgLength);
			//WrightLogEx("test IP", szImages);
			if (NULL == pBeginCaptureEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pBeginCaptureEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cBeginCapture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cBeginCapture.cImgInfo.dwHeight = GetValueFromAttribute(pBeginCaptureEle->Attribute("Height"));
			pcRecordImage->cBeginCapture.cImgInfo.dwWidth = GetValueFromAttribute(pBeginCaptureEle->Attribute("Width"));
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.top = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosTop"));
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.left = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosLeft"));
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosBottom"));
			pcRecordImage->cBeginCapture.cImgInfo.rcPlate.right = GetValueFromAttribute(pBeginCaptureEle->Attribute("PlatePosRight"));
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs = GetValueFromAttribute(pBeginCaptureEle->Attribute("TimeHigh"));
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cBeginCapture.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pBeginCaptureEle->Attribute("TimeLow"));
			pcRecordImage->cBeginCapture.pbImgData = pbTempData;
			//pcRecordImage->cBeginCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cBeginCapture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cBeginCapture.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cBeginCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//memcpy(pcRecordImage->cBeginCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_BEST_CAPTURE:
			sprintf(szImages, "%s %d-%d", szImages, 3, cImgInfo.dwImgLength);
            //WrightLogEx("test IP", szImages);
			if (NULL == pBestCaptureEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pBestCaptureEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cBestCapture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cBestCapture.cImgInfo.dwHeight = GetValueFromAttribute(pBestCaptureEle->Attribute("Height"));
			pcRecordImage->cBestCapture.cImgInfo.dwWidth = GetValueFromAttribute(pBestCaptureEle->Attribute("Width"));
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.top = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosTop"));
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.left = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosLeft"));
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosBottom"));
			pcRecordImage->cBestCapture.cImgInfo.rcPlate.right = GetValueFromAttribute(pBestCaptureEle->Attribute("PlatePosRight"));
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs = GetValueFromAttribute(pBestCaptureEle->Attribute("TimeHigh"));
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cBestCapture.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pBestCaptureEle->Attribute("TimeLow"));
			pcRecordImage->cBestCapture.pbImgData = pbTempData;
			//pcRecordImage->cBestCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cBestCapture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cBestCapture.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cBestCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//memcpy(pcRecordImage->cBestCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_LAST_CAPTURE:
			sprintf(szImages, "%s %d-%d", szImages, 4, cImgInfo.dwImgLength);
			//WrightLogEx("test IP", szImages);
			if (NULL == pLastCaptureEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pLastCaptureEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cLastCapture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cLastCapture.cImgInfo.dwHeight = GetValueFromAttribute(pLastCaptureEle->Attribute("Height"));
			pcRecordImage->cLastCapture.cImgInfo.dwWidth = GetValueFromAttribute(pLastCaptureEle->Attribute("Width"));
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.top = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosTop"));
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.left = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosLeft"));
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.bottom = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosBottom"));
			pcRecordImage->cLastCapture.cImgInfo.rcPlate.right = GetValueFromAttribute(pLastCaptureEle->Attribute("PlatePosRight"));
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs = GetValueFromAttribute(pLastCaptureEle->Attribute("TimeHigh"));
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cLastCapture.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pLastCaptureEle->Attribute("TimeLow"));
			pcRecordImage->cLastCapture.pbImgData = pbTempData;
			//pcRecordImage->cLastCapture.pbImgInfo = pbTempInfo;
			pcRecordImage->cLastCapture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cLastCapture.dwImgInfoLen = dwImgInfoLen;
			//pcRecordImage->cLastCapture.cImgInfo.nFaceCount = cImgInfo.nFaceCount;
			//memcpy(pcRecordImage->cLastCapture.cImgInfo.rcFacePos, cImgInfo.rcFacePos, sizeof(cImgInfo.rcFacePos));
			break;
		case RECORD_IMAGE_SMALL_IMAGE:
			sprintf(szImages, "%s %d-%d", szImages, 5, cImgInfo.dwImgLength);
			//WrightLogEx("test IP", szImages);
			if (NULL == pPlateEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pPlateEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cPlatePicture.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cPlatePicture.cImgInfo.dwHeight = GetValueFromAttribute(pPlateEle->Attribute("Height"));
			pcRecordImage->cPlatePicture.cImgInfo.dwWidth = GetValueFromAttribute(pPlateEle->Attribute("Width"));
			sprintf(szLog, "HvGetRecordImage_Mercury Ð¡ÍŒ¿í[%d] žß[%d]", pcRecordImage->cPlatePicture.cImgInfo.dwWidth, pcRecordImage->cPlatePicture.cImgInfo.dwHeight);
//			WrightLogEx("test IP", szLog);
			//// °ÑYUVÊýŸÝÐŽµœÎÄŒþÖÐ
			//char szSmallFileName[128];
			//sprintf(szSmallFileName, "D:\\Result\\YUV-%4d-w%d-h%d.yuv", i, pcRecordImage->cPlatePicture.cImgInfo.dwWidth, pcRecordImage->cPlatePicture.cImgInfo.dwHeight);
			//fp = fopen(szSmallFileName, "a+");
			//if(fp)
			//{
			//	fwrite(pbTempData, cImgInfo.dwImgLength, 1, fp);
			//	fclose(fp);
			//}
			//i++;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs = GetValueFromAttribute(pPlateEle->Attribute("TimeHigh"));
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cPlatePicture.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pPlateEle->Attribute("TimeLow"));
			pcRecordImage->cPlatePicture.pbImgData = pbTempData;
			//pcRecordImage->cPlatePicture.pbImgInfo = pbTempInfo;
			pcRecordImage->cPlatePicture.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cPlatePicture.dwImgInfoLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_BIN_IMAGE:
			sprintf(szImages, "%s %d-%d", szImages, 6, cImgInfo.dwImgLength);
			//WrightLogEx("test IP", szImages);
			if (NULL == pPlateBinEle)
			{
				//WrightLogEx("test IP", "HvGetRecordImage_Mercury pPlateBinEle == NULL");
				return E_FAIL;
			}
			pcRecordImage->cPlateBinary.cImgInfo.dwCarID = dwCarID;
			pcRecordImage->cPlateBinary.cImgInfo.dwHeight = GetValueFromAttribute(pPlateBinEle->Attribute("Height"));
			pcRecordImage->cPlateBinary.cImgInfo.dwWidth = GetValueFromAttribute(pPlateBinEle->Attribute("Width"));
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs = GetValueFromAttribute(pPlateBinEle->Attribute("TimeHigh"));
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs <<= 32;
			pcRecordImage->cPlateBinary.cImgInfo.dw64TimeMs |= GetValueFromAttribute(pPlateBinEle->Attribute("TimeLow"));
			pcRecordImage->cPlateBinary.pbImgData = pbTempData;
			pcRecordImage->cPlateBinary.pbImgData = pbTempData;
			//pcRecordImage->cPlateBinary.pbImgInfo = pbTempInfo;
			pcRecordImage->cPlateBinary.dwImgDataLen = cImgInfo.dwImgLength;
			//pcRecordImage->cPlateBinary.dwImgInfoLen = dwImgInfoLen;
			break;
		default:
			break;
		}
		pbTempData += cImgInfo.dwImgLength;
		dwRemainLen -= (sizeof(cImgInfo) + cImgInfo.dwImgLength);
		sprintf(szImages, "%sR%d", szImages, dwRemainLen);
	}

	//sprintf(szLog, "Received record[%d] %s with images[%s] in pbData(length %d)", dwCarID, szPlateName, szImages, dwRecordDataLen);
	//WrightLogEx("Test IP", szLog);

	return S_OK;
}




////////////////////////////

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

// æè¿°:	è½¬æ¢YUVæ°æ®å°RGB
// åæ°:	pbDest			è¾åºRGBæ°æ®çç¼å²åºæé;
//			pbSrc			è¾å¥YUVæ°æ®çç¼å²åºæé;
//			iSrcWidth		å¾åå®½åº¦;
//			iSrcHeight		å¾åé«åº¦;
//			iBGRStride		RGBæ°æ®æ¯è¡çæ­¥é¿;
// è¿åå¼:  è¿åS_OK, è¡¨ç¤ºæä½æå,
//          è¿åE_POINTER, åæ°ä¸­åå«æéæ³çæé;
//          è¿åE_FAIL, è¡¨ç¤ºæªç¥çéè¯¯å¯¼è´æä½å¤±è´¥;
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

	*(pbDest++) = 'B';										//BMPæä»¶æ å¿.
	*(pbDest++) = 'M';
	pbDest += SetInt32ToByteArray(pbDest, *piDestLen);		//BMPæä»¶å¤§å°.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//reserved
	pbDest += SetInt32ToByteArray(pbDest, dwOffSet);		//æä»¶å¤´åç§»é.
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
		//å­èä¸­ååbitä½äºæ¢
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
		//int iTemp = WSAGetLastError();
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
					SAFE_DELETE(pszTmpXmlBuf);
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
	if ( !cXmlDoc.Parse(szCmd) )  // ÅÃ¬Â²Ã©szCmdÃÃÂ·Ã±ÃÃXML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(true , szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
		szCmd = (char*)pszXmlBuf;
	}
	//printf("%s----\n", szCmd);
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
    /*
    struct timeval tv;
    tv.tv_sec = 8;
    tv.tv_usec=0;
    if ( setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, sizeof(struct timeval))  != 0 )
            return -1;*/

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

	struct sockaddr_in addr;
	size_t addrlen = sizeof(addr);

	if ( -1 == iTimeout )
	{
		hNewSocket = accept(hSocket, (struct sockaddr*)&addr, &addrlen);
		return (INVALID_SOCKET!=hNewSocket) ? S_OK : E_FAIL;
	}

	HRESULT hr = E_FAIL;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(hSocket, &r);

	struct timeval timeout;
	timeout.tv_sec = iTimeout / 1000;
	timeout.tv_usec = (iTimeout % 1000) * 1000;

	int ret = select(hSocket+1, &r, NULL, NULL, &timeout);
	if (ret == 1)
	{
		hNewSocket = accept(hSocket, (struct sockaddr*)&addr, &addrlen);
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

//çº¢ç¯å çº¢
HRESULT HvEnhanceTrafficLight(PBYTE pbSrcImg, DWORD dwiSrcImgDataLen, int iRedLightCount,
							  PBYTE pbRedLightPos, PBYTE pbDestImgBuf, DWORD& dwDestImgBufLen,
							  INT iBrightness, INT iHueThreshold, INT iCompressRate)
{
    /*
	if(pbSrcImg == NULL || dwiSrcImgDataLen <= 0) return E_FAIL;
	if(pbDestImgBuf == NULL || dwDestImgBufLen <= 0) return E_FAIL;
	if(pbRedLightPos == NULL && sizeof(pbRedLightPos)/sizeof(RECT) < iRedLightCount)return E_FAIL;

	RECT* pcRect = new RECT[iRedLightCount*sizeof(RECT)];
	if(pcRect == NULL)return E_FAIL;
	memcpy(pcRect, pbRedLightPos, iRedLightCount*sizeof(RECT));
	if(iBrightness < -255) iBrightness = -255;
	if(iBrightness > 255) iBrightness = 255;

	CxImage imgSrc(pbSrcImg, dwiSrcImgDataLen, CXIMAGE_FORMAT_UNKNOWN);
	if(!imgSrc.IsValid())
	{
	    fprintf(stderr, "%s\n", imgSrc.GetLastError());
	    return E_FAIL;
	}
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

        if(!imgSrc.Crop(cTempRect.left, cTempRect.top, cTempRect.right, cTempRect.bottom, &imgCrop))
//		if(!imgSrc.Crop(cTempRect, &imgCrop))
		{
			fprintf(stderr, "Crop:%s\n", imgSrc.GetLastError());
			continue;
		}
		if(!imgCrop.SplitHSL(&imgH, &imgS, &imgV))
		{
			fprintf(stderr, "SplitHSL:%s\n", imgSrc.GetLastError());
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
//					imgV.SetPixelColor(x, y, v);
					COLORREF crV=RGB(v.rgbRed,v.rgbGreen,v.rgbBlue);
                    imgV.SetPixelColor(x, y, crV);

					s.rgbRed = iAvgS;
					s.rgbGreen = s.rgbRed;
					s.rgbBlue = s.rgbRed;
//					imgS.SetPixelColor(x, y, s);
					COLORREF crS=RGB(s.rgbRed,s.rgbGreen,s.rgbBlue);
					imgS.SetPixelColor(x, y, crS);
				}
				h.rgbBlue = 0;
				h.rgbGreen = 0;
				h.rgbRed = 0;
//				imgH.SetPixelColor(x, y, h);
				COLORREF crH=RGB(h.rgbRed,h.rgbGreen,h.rgbBlue);
				imgH.SetPixelColor(x, y, crH);
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
	if(!imgSrc.Encode(pbuffer, lsize, CXIMAGE_FORMAT_JPG))
	{
	    fprintf(stderr, "Encode:%s\n", imgSrc.GetLastError());
	    return E_FAIL;
	}
	if(lsize > (int)dwDestImgBufLen)
	{
		imgSrc.FreeMemory(pbuffer);
		dwDestImgBufLen = lsize;
		return E_FAIL;
	}
	memcpy(pbDestImgBuf, pbuffer, lsize);
	dwDestImgBufLen = lsize;
	imgSrc.FreeMemory(pbuffer);
	*/

	return S_OK;

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
	// Îª´´½¨Xml×¼±¸²ÎÊý¿Õ¼ä
	char* pszInBufCopy = new char[nInlen+1];
	if(pszInBufCopy == NULL)
	{
		return E_FAIL;
	}
	memcpy(pszInBufCopy, inXmlOrStrBuf, nInlen);
	pszInBufCopy[nInlen] = '\0';

	int nCount = 0;
	char* pCmdName = pszInBufCopy;
	// ¼Ù¶¨ÃüÁîµÄ²ÎÊý¸öÊý²»³¬¹ý64¸ö¡£
	char* pValueID[64];
	char* pValueText[64];
	char* pTemp;
	// ÅÐ¶Ï¼°½âÎöXmlÀàÐÍ×Ö·û´®
	while( pszInBufCopy = strchr(pszInBufCopy, ',') )
	{
		*pszInBufCopy = '\0';
		pValueID[nCount] = pszInBufCopy + 1;
		if ( ((pTemp = strchr(pszInBufCopy+2, '[')) == NULL)     // +2±íÊ¾±ØÐèÒªÓÐÒ»¸ö×Ö·û
			|| ((pszInBufCopy = strchr(pTemp+2, ']')) == NULL) ) // +2±íÊ¾±ØÐèÒªÓÐÒ»¸ö×Ö·û
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

	SAFE_DELETE(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
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
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2±íÊ¾±ØÐèÒªÓÐÒ»¸ö×Ö·û
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2±íÊ¾±ØÐèÒªÓÐÒ»¸ö×Ö·û
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
		SAFE_DELETE(pszCmdName);
	}

	nOutlen = HvMakeXmlCmdReq2(listXmlCmd, szOutXmlBuf);
	SAFE_DELETE(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}


HRESULT HvMakeXmlCmdByString3(const char* inXmlOrStrBuf, int nInlen,
					  char* szOutXmlBuf, int& nOutlen)
{
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(inXmlOrStrBuf))
	{ // ²»Ö§³ÖÖ±½Ó´«ÈëXML£¬ÒòÎªË®ÐÇµÄXML¸ñÊ½Óë1.0¡¢2.0Ð­ÒéµÄXML²»Ò»Ñù
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
		char* pTmpCmdEnd = strstr(pTmpCmd, ";"); // ÃüÁî·Ö¸ô·û
		if(pTmpCmdEnd)
		{
			*pTmpCmdEnd = '\0';
		}
		int nCount = 0;
		char* pszCmdName = pTmpCmd;
		char* pValueID[64];
		char* pValueText[64];
		char* pTemp;
		while(pTmpCmd = strchr(pTmpCmd, ',')) // ²ÎÊý·Ö¸ô·û
		{
			*pTmpCmd = '\0';
			pValueID[nCount] = pTmpCmd + 1;
			if ( ((pTemp = strchr(pTmpCmd+2, '[')) == NULL)     // +2±íÊ¾±ØÐèÒªÓÐÒ»¸ö×Ö·û
				|| ((pTmpCmd = strchr(pTemp+2, ']')) == NULL) ) // +2±íÊ¾±ØÐèÒªÓÐÒ»¸ö×Ö·û
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
		// ÅÐ¶Ï´«Èë²ÎÊýµÄÓÐÐ§ÐÔ
		if (strstr("SetTime", pszCmdName))
		{ // TODO:SetTimeÐèÒªÌØÊâ´¦Àí
			if(nCount == 2)
			{
				if(strlen(pValueID[0]) + strlen(pValueID[1]) + strlen(pValueText[0]) + strlen(pValueText[1]) < 256)
				{
					//CString strTmp;
					//strTmp.Format("%s[%s],%s[%s]",pValueID[0], pValueText[0], pValueID[1], pValueText[1]);

					char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s[%s],%s[%s]",pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf,  szTemp, 256);
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
					//CString strTmp;
				//	strTmp.Format("%s[%s],%s[%s],%s[%s],%s[%s]",
					//	pValueID[0], pValueText[0], pValueID[1], pValueText[1], pValueID[2], pValueText[2], pValueID[3], pValueText[3]);

                    char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s[%s],%s[%s],%s[%s],%s[%s]", pValueID[0], pValueText[0], pValueID[1], pValueText[1], pValueID[2], pValueText[2], pValueID[3], pValueText[3]);
					memcpy(szBuf, szTemp, 255);
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
			iFixParamCount = -1; //È¥³ýÊ¹ÄÜ½Úµã
			if(strcmp(pValueText[3], "0") == 0) //²»Ê¹ÄÜ Ôò½«RGB¸³ÖµÎª-1
			{
				pValueText[0] = "-1";
				pValueText[1] = "-1";
				pValueText[2] = "-1";
			}
		}
		if(strstr("SetCaptureGain", pszCmdName))
		{
			iFixParamCount = -1; //È¥³ýÊ¹ÄÜ½Úµã
			if(strcmp(pValueText[1], "0") == 0) //²»Ê¹ÄÜ Ôò½«RGB¸³ÖµÎª-1
			{
				pValueText[0] = "-1";
			}
		}
		if(strstr("SetCaptureShutter", pszCmdName))
		{
			iFixParamCount = -1; //È¥³ýÊ¹ÄÜ½Úµã
			if(strcmp(pValueText[1], "0") == 0) //²»Ê¹ÄÜ Ôò½«RGB¸³ÖµÎª-1
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
					//CString strTmp;
					//strTmp.Format("%s=[%s],%s=[%s]",
					//	pValueID[0], pValueText[0], pValueID[1], pValueText[1]);

                    char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s=[%s],%s=[%s]", pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf,  szTemp,255+iAddLenOfForNOTOSDText);
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
					//CString strTmp;
					//strTmp.Format("%s=[%s],%s=[%s]",
					//	pValueID[0], pValueText[0], pValueID[1], pValueText[1]);

					char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s=[%s],%s=[%s]", 	pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf, szTemp,255);
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
					//CString strTmp;
					//strTmp.Format("%s=[%s],%s=[%s]",
					//	pValueID[0], pValueText[0], pValueID[1], pValueText[1]);

					char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s=[%s],%s=[%s]", 	pValueID[0], pValueText[0], pValueID[1], pValueText[1]);
					memcpy(szBuf,  szTemp,255);
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
					//CString strTmp;
					//strTmp.Format("%s=[%s],%s=[%s],%s=[%s],%s=[%s],%s=[%s]",
					//	pValueID[0], pValueText[0],
					//	pValueID[1], pValueText[1],
					//	pValueID[2], pValueText[2],
					//	pValueID[3], pValueText[3],
					//	pValueID[4], pValueText[4]);

                    char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s=[%s],%s=[%s],%s=[%s],%s=[%s],%s=[%s]",
					pValueID[0], pValueText[0],  pValueID[1], pValueText[1],  pValueID[2], pValueText[2], pValueID[3], pValueText[3], pValueID[4], pValueText[4]);

					memcpy(szBuf, szTemp,255);
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
					//CString strTmp;
					//strTmp.Format("%s=[%s],%s=[%s],%s=[%s]",
					//	pValueID[0], pValueText[0], pValueID[1], pValueText[1], pValueID[2], pValueText[2]);

					char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s=[%s],%s=[%s],%s=[%s]", 	pValueID[0], pValueText[0], pValueID[1], pValueText[1], pValueID[2], pValueText[2]);
					memcpy(szBuf, szTemp, 255);
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
					//CString strTmp;
					//strTmp.Format("%s=[%s],%s=[%s],%s=[%s],%s=[%s],%s=[%s]",
					//	pValueID[0], pValueText[0],
					//	pValueID[1], pValueText[1],
					//	pValueID[2], pValueText[2],
					//	pValueID[3], pValueText[3],
					//	pValueID[4], pValueText[4]);

					char szTemp[1024] = { 0 };
					sprintf(szTemp, "%s=[%s],%s=[%s],%s=[%s],%s=[%s],%s=[%s]",
					pValueID[0], pValueText[0],
					pValueID[1], pValueText[1],
					pValueID[2], pValueText[2],
					pValueID[3], pValueText[3],
					pValueID[4], pValueText[4]);

					memcpy(szBuf, szTemp,255);
				}
			}
			pValueID[0] = "ModUser";
			pValueText[0] = szBuf;
			nCount = 1;

		}


		int iCmdIndex = SearchXmlCmdMapMercury(pszCmdName);
		if (-1 == iCmdIndex)
		{ // Ã»ÕÒµ½¶ÔÓ¦µÄÃüÁî
			//bGet = false;
			sprintf(sLog, "HvMakeXmlCmdByString3 can't find command [%s]", pszCmdName);
		//	WrightLogEx("==testIP==", sLog);
			//SAFE_DELETE(pTmpCmdBackup);
			//goto cleanup;
		}
		else
		{
			if (XML_CMD_SETTER == g_XmlCmdAppendInfoMap[iCmdIndex].emCmdClass)
			{ // GETTER²»ÐèÒª×éÖ¯ÊäÈë²ÎÊý
				if (0 == strcmp(pszCmdName, "ResetDevice") && nCount < g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum)
				{ // ResetDeviceÓÐÄ¬ÈÏ²ÎÊýÖµ£¬ÐèÒªÌØÊâ´¦Àí
					SXmlAttr  sXmlAttr;
					sXmlAttr.strName = "ResetMode";
					sXmlAttr.strValue = "-1";
					sXmlCmd.listAttr.push_back(sXmlAttr);
				}
				else
				{
					if (nCount < g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum)
					{ // ²ÎÊýÊýÁ¿²»¶Ô
						bGet = false;
						//sprintf(sLog, "HvMakeXmlCmdByString3 Command [%s] doesn't have enough parameters", pszCmdName);
						//WrightLogEx("testIP", sLog);
						SAFE_DELETE(pTmpCmdBackup);
						goto cleanup;
					}
					else
					{
						for (int i=0; i<g_XmlCmdAppendInfoMap[iCmdIndex].iParamNum + iFixParamCount; ++i) // ¶àÓàµÄ²ÎÊýºöÂÔµô
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
				if ( 0 == strcmp("ReadFPGA"  ,g_XmlCmdAppendInfoMap[iCmdIndex].szCmdName))
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

		SAFE_DELETE(pTmpCmdBackup);
	}

	//char szLog[1024];
	//sprintf(szLog, "HvMakeXmlCmdByString3 got commands[%s] to execute", szCmdForLog);
	//WrightLogEx("Test IP", szLog);

	nOutlen = HvMakeXmlCmdReq3(listXmlCmd, szOutXmlBuf);

cleanup:
	SAFE_DELETE(pszInBufCopy);
	return (bGet) ? S_OK : E_FAIL;
}








PROTOCOL_VERSION GetProtocolVersion(char* szIP)
{
	PROTOCOL_VERSION version = PROTOCOL_VERSION_1;

	int hsocketcmd = -1;
    CAMERA_CMD_HEADER cmdHearder;
    CAMERA_CMD_RESPOND cmdRespond;
    if (!ConnectCamera(szIP, CAMERA_CMD_LINK_PORT, hsocketcmd))
    {
        return version;
    }

    cmdHearder.dwID  = CAMERA_PROTOCAL_COMMAND;
    cmdHearder.dwInfoSize = 0;

	int iHeaderLen = sizeof(cmdHearder);
    if ( iHeaderLen  !=  send( hsocketcmd, (char*)&cmdHearder, iHeaderLen, MSG_NOSIGNAL) )
    {
        ForceCloseSocket(hsocketcmd);
        return version;
    }
    int iRespondLen = sizeof(cmdRespond);
   	if ( iRespondLen !=  RecvAll(hsocketcmd, (char *)&cmdRespond, iRespondLen ) )
   	{
       ForceCloseSocket(hsocketcmd);
       return version;
   	}

   	if ( cmdRespond.dwID == CAMERA_PROTOCAL_COMMAND )
   	{
   		if (cmdRespond.dwResult == PROTOCOL_MERCURY)
			version = PROTOCOL_VERSION_MERCURY;
		else if (cmdRespond.dwResult == PROTOCOL_EARTH)
			version = PROTOCOL_VERSION_EARTH;
		else
			version = PROTOCOL_VERSION_2;
   	}
	ForceCloseSocket(hsocketcmd);
   	return version;

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
		//OutputDebugString( " !myDocument.Parse(szXmlBuff) \n");
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

			char *pWhiteList = (char*)strstr(chElement, "WhiteNameList");
			if ( pWhiteList != NULL )
			{
				sscanf(pWhiteList,"WhiteNameList[%[^]]", szWhiteNameList);

			}

			char *pBlackList = (char*)strstr(chElement, "BlackNameList");
			if (pBlackList != NULL )
			{
				sscanf(pBlackList,"BlackNameList[%[^]]", szBlackNameList);
			}
			break;
		}
	}

	return true;
}


HRESULT	HvXmlParse( CHAR* szXmlCmdName , CHAR* szXml , INT nXmlLen, CXmlParseInfo* prgXmlParseInfo , INT nXmlParseInfoCount )
{
	if ( NULL == prgXmlParseInfo || nXmlParseInfoCount <= 0 )
	{
		return E_FAIL;
	}
	HRESULT hr = E_FAIL;
	TiXmlDocument cXmlDoc;
	//WrightLogEx("xml-2" , szXml );
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
										//WrightLogEx( "XML_CMD_TYPE_BIN" , prgXmlParseInfo[i].szKeyName );
										break;
									case XML_CMD_TYPE_NULL:
									default:
										//WrightLogEx( "XML_CMD_TYPE_NULL" , prgXmlParseInfo[i].szKeyName );
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
	//WrightLogEx("xml-2" , szXml );
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
		//WrightLogEx( pIP , "ExecuteCmd ConnectCamera Fail\n");
		return E_FAIL;
	}
	//发送请求命令头
	cCmdHeader.dwID = eCmdId;
	cCmdHeader.dwInfoSize = nCmdByte;
	if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
	{
		//WrightLogEx( pIP , "ExecuteCmd send header Fail\n");
		ForceCloseSocket(hSocketCmd);
		return E_FAIL;
	}

	//发送请求数据包
	if ( nCmdByte > 0 && nCmdByte != send( hSocketCmd , pCmdData , nCmdByte , 0 ) )
	{
		//WrightLogEx( pIP , "ExecuteCmd send data Fail\n");
		ForceCloseSocket(hSocketCmd);
		return E_FAIL;
	}


	//接收数据包头
	if ( sizeof( cCmdRespond ) != recv( hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0)
			|| cCmdRespond.dwID != eCmdId
			|| cCmdRespond.dwResult != 0 )
	{
		//WrightLogEx( pIP , "ExecuteCmd recv header Fail\n");
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
			//WrightLogEx( pIP , "ExecuteCmd recv buf Fail\n");
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}
	}
	*pnRetByte = cCmdRespond.dwInfoSize;
	//接收数据
	ForceCloseSocket(hSocketCmd);
	return S_OK;
}


struct my_error_mgr
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr* my_error_ptr;


HRESULT JPEG2RGB(BYTE* psrcData, int iSrcSize, BYTE**pDestBuff, int *piDestSize)
{
    //CxImage jpegImage(psrcData, iSrcSize, CXIMAGE_FORMAT_JPG);

    //image->GetBits();

    if (psrcData == NULL || iSrcSize == NULL )
        return S_FALSE;


    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;


    JSAMPARRAY buffer;
    int row_stride;

    struct jpeg_error_mgr *ppub = &jerr.pub;
    cinfo.err = jpeg_std_error(ppub);
    //jerr.pub.error_exit = my_error_mgr;
    //jerr.pub.error_exit = my_error_mgr;

    if (setjmp(jerr.setjmp_buffer))
    {
        return S_FALSE;
    }

   jpeg_create_decompress(&cinfo);
   jpeg_mem_src(&cinfo, psrcData, iSrcSize);


   jpeg_read_header(&cinfo, TRUE);

    printf("image width is %d\n", cinfo.image_width);
 /* nominal image width (from SOF marker) */
    printf("image height is %d\n", cinfo.image_height);
 /* nominal image width (from SOF marker) */
     printf("numcom is %d\n", cinfo.num_components);
/* nominal image width (from SOF marker) */

    cinfo.out_color_space = JCS_RGB;



     jpeg_start_decompress(&cinfo);
     printf("image width is %d\n", cinfo.output_width);
 /* nominal image width (from SOF marker) */
    printf("image height is %d\n", cinfo.output_height);
 /* nominal image width (from SOF marker) */
    printf("numcom is %d\n", cinfo.output_components);
/* nominal image width (from SOF marker) */


    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    int iSize = cinfo.output_width * cinfo.output_components*cinfo.output_height + 1024;
    BYTE *pTempBuffer = new BYTE[iSize];
    BYTE *pSrc = pTempBuffer;
    memset(pTempBuffer, 0, iSize);

    int itatalLen = 0;
     while (cinfo.output_scanline < cinfo.output_height)
     {
         jpeg_read_scanlines(&cinfo, buffer, 1);

         //put_scanline_someplace(buffer[0], row_stride);
         //fwrite(buffer[0], 1, row_stride, outfile);


        memcpy(pTempBuffer, buffer[0], row_stride);

        for(int i=0; i<cinfo.output_width; i++)
        {
            int ibase = i*cinfo.output_components;
            BYTE r = pTempBuffer[ibase];
           // BYTE g = pTempBuffer[ibase+1];
            BYTE b = pTempBuffer[ibase+2];
            pTempBuffer[ibase] = b;
            pTempBuffer[ibase+2] = r;
            //bm.SetPixel(i,line,Color(255,(BYTE)buffer[0][i*3],(BYTE)buffer[0][i*3+1],(BYTE)buffer[0][i*3+2]));
        }

        row_stride = cinfo.output_width * cinfo.output_components;

        pTempBuffer+=row_stride;


        //int line = cinfo.output_height-1;
        //BYTE *pTemp = pTempBuffer+line*row_stride;
       // memcpy(pTemp, buffer[0], row_stride);
        itatalLen += row_stride;
     }

     jpeg_finish_decompress(&cinfo);
     jpeg_destroy_decompress(&cinfo);

     *pDestBuff = pSrc;
      *piDestSize = itatalLen;



    return S_OK;
}


/*********************
/*********************
//输入
// rgb_buffer: RGB24缓冲区指针
// nWidth    : 图片宽度
// nHeight   : 图片高度
// fp2       : 指向加bmp头后的数据
**************************/

typedef struct
{    long imageSize;
    long blank;
    long startPosition;
}BmpHead;

typedef struct

{
    long    Length;
    long    width;
    long    height;
    WORD    colorPlane;
    WORD    bitColor;
    long    zipFormat;
    long    realSize;
    long    xPels;
    long    yPels;
    long    colorUse;
    long    colorImportant;

}InfoHead;

typedef struct
{         BYTE   rgbBlue;
         BYTE   rgbGreen;
         BYTE   rgbRed;
         BYTE   rgbReserved;

}RGBMixPlate;



int RGB2BMP1(char *rgb_buffer,int nWidth,int nHeight,char*fp2)
{
     BmpHead m_BMPHeader;
     char bfType[2]={'B','M'};
     m_BMPHeader.imageSize=3*nWidth*nHeight+54;
     m_BMPHeader.blank=0;
     m_BMPHeader.startPosition=54;
     char* temp=fp2;
     memcpy(fp2,bfType,2);
     fp2+=sizeof(bfType);

     memcpy(fp2,&m_BMPHeader,sizeof(m_BMPHeader.imageSize));
     fp2+=sizeof(m_BMPHeader.imageSize);


     memcpy(fp2,&m_BMPHeader.blank,sizeof(m_BMPHeader.blank));
     fp2+=sizeof(m_BMPHeader.blank);


     memcpy(fp2,&m_BMPHeader.startPosition,sizeof(m_BMPHeader.startPosition));
     fp2+=sizeof(m_BMPHeader.startPosition);


     InfoHead  m_BMPInfoHeader;
     m_BMPInfoHeader.Length=40;
     m_BMPInfoHeader.width=nWidth;
     m_BMPInfoHeader.height=-nHeight;
     m_BMPInfoHeader.colorPlane=1;
     m_BMPInfoHeader.bitColor=24;
     m_BMPInfoHeader.zipFormat=0;
     m_BMPInfoHeader.realSize=3*nWidth*nHeight;
     m_BMPInfoHeader.xPels=0;
     m_BMPInfoHeader.yPels=0;
     m_BMPInfoHeader.colorUse=0;
     m_BMPInfoHeader.colorImportant=0;

     memcpy(fp2,&m_BMPInfoHeader.Length,sizeof(m_BMPInfoHeader.Length));
      fp2+=sizeof(m_BMPInfoHeader.Length);

     memcpy(fp2,&m_BMPInfoHeader.width,sizeof(m_BMPInfoHeader.width));
      fp2+=sizeof(m_BMPInfoHeader.width);

      memcpy(fp2,&m_BMPInfoHeader.height,sizeof(m_BMPInfoHeader.height));
      fp2+=sizeof(m_BMPInfoHeader.height);

     memcpy(fp2,&m_BMPInfoHeader.colorPlane,sizeof(m_BMPInfoHeader.colorPlane));
     fp2+=sizeof(m_BMPInfoHeader.colorPlane);

     memcpy(fp2,&m_BMPInfoHeader.bitColor,sizeof(m_BMPInfoHeader.bitColor));
     fp2+=sizeof(m_BMPInfoHeader.bitColor);


     memcpy(fp2,&m_BMPInfoHeader.zipFormat,sizeof(m_BMPInfoHeader.zipFormat));
     fp2+=sizeof(m_BMPInfoHeader.zipFormat);

     memcpy(fp2,&m_BMPInfoHeader.realSize,sizeof(m_BMPInfoHeader.realSize));
     fp2+=sizeof(m_BMPInfoHeader.realSize);

     memcpy(fp2,&m_BMPInfoHeader.xPels,sizeof(m_BMPInfoHeader.xPels));
     fp2+=sizeof(m_BMPInfoHeader.xPels);

     memcpy(fp2,&m_BMPInfoHeader.yPels,sizeof(m_BMPInfoHeader.yPels));
     fp2+=sizeof(m_BMPInfoHeader.yPels);

     memcpy(fp2,&m_BMPInfoHeader.colorUse,sizeof(m_BMPInfoHeader.colorUse));
     fp2+=sizeof(m_BMPInfoHeader.colorUse);

     memcpy(fp2,&m_BMPInfoHeader.colorImportant,sizeof(m_BMPInfoHeader.colorImportant));
     fp2+=sizeof(m_BMPInfoHeader.colorImportant);

     memcpy(fp2,rgb_buffer,3*nWidth*nHeight);
     fp2=temp;
     return 1;
}


int RGB2BMPEx(char *rgb_buffer,int nWidth,int nHeight)
{
    static int iCount = 0;
    static int buffSzie = 1024*1024*20;
    iCount++;

    char *fp2 = new char[buffSzie];
    memset(fp2, 0, buffSzie);

     BmpHead m_BMPHeader;
     char bfType[2]={'B','M'};
     m_BMPHeader.imageSize=3*nWidth*nHeight+54;
     m_BMPHeader.blank=0;
     m_BMPHeader.startPosition=54;
     char* temp=fp2;
     memcpy(fp2,bfType,2);
     fp2+=sizeof(bfType);

     memcpy(fp2,&m_BMPHeader,sizeof(m_BMPHeader.imageSize));
     fp2+=sizeof(m_BMPHeader.imageSize);


     memcpy(fp2,&m_BMPHeader.blank,sizeof(m_BMPHeader.blank));
     fp2+=sizeof(m_BMPHeader.blank);


     memcpy(fp2,&m_BMPHeader.startPosition,sizeof(m_BMPHeader.startPosition));
     fp2+=sizeof(m_BMPHeader.startPosition);


     InfoHead  m_BMPInfoHeader;
     m_BMPInfoHeader.Length=40;
     m_BMPInfoHeader.width=nWidth;
     m_BMPInfoHeader.height=-nHeight;
     m_BMPInfoHeader.colorPlane=1;
     m_BMPInfoHeader.bitColor=24;
     m_BMPInfoHeader.zipFormat=0;
     m_BMPInfoHeader.realSize=3*nWidth*nHeight;
     m_BMPInfoHeader.xPels=0;
     m_BMPInfoHeader.yPels=0;
     m_BMPInfoHeader.colorUse=0;
     m_BMPInfoHeader.colorImportant=0;

     memcpy(fp2,&m_BMPInfoHeader.Length,sizeof(m_BMPInfoHeader.Length));
      fp2+=sizeof(m_BMPInfoHeader.Length);

     memcpy(fp2,&m_BMPInfoHeader.width,sizeof(m_BMPInfoHeader.width));
      fp2+=sizeof(m_BMPInfoHeader.width);

      memcpy(fp2,&m_BMPInfoHeader.height,sizeof(m_BMPInfoHeader.height));
      fp2+=sizeof(m_BMPInfoHeader.height);

     memcpy(fp2,&m_BMPInfoHeader.colorPlane,sizeof(m_BMPInfoHeader.colorPlane));
     fp2+=sizeof(m_BMPInfoHeader.colorPlane);

     memcpy(fp2,&m_BMPInfoHeader.bitColor,sizeof(m_BMPInfoHeader.bitColor));
     fp2+=sizeof(m_BMPInfoHeader.bitColor);


     memcpy(fp2,&m_BMPInfoHeader.zipFormat,sizeof(m_BMPInfoHeader.zipFormat));
     fp2+=sizeof(m_BMPInfoHeader.zipFormat);

     memcpy(fp2,&m_BMPInfoHeader.realSize,sizeof(m_BMPInfoHeader.realSize));
     fp2+=sizeof(m_BMPInfoHeader.realSize);

     memcpy(fp2,&m_BMPInfoHeader.xPels,sizeof(m_BMPInfoHeader.xPels));
     fp2+=sizeof(m_BMPInfoHeader.xPels);

     memcpy(fp2,&m_BMPInfoHeader.yPels,sizeof(m_BMPInfoHeader.yPels));
     fp2+=sizeof(m_BMPInfoHeader.yPels);

     memcpy(fp2,&m_BMPInfoHeader.colorUse,sizeof(m_BMPInfoHeader.colorUse));
     fp2+=sizeof(m_BMPInfoHeader.colorUse);

     memcpy(fp2,&m_BMPInfoHeader.colorImportant,sizeof(m_BMPInfoHeader.colorImportant));
     fp2+=sizeof(m_BMPInfoHeader.colorImportant);

     memcpy(fp2,rgb_buffer,3*nWidth*nHeight);
     fp2=temp;

     char fileName[256] = {0};
     sprintf(fileName, "./image_%d.bmp", iCount );

     FILE *myFile =fopen(fileName, "wb");
     if (myFile != NULL)
     {
         fwrite(fp2,3*nWidth*nHeight+54,1,myFile);
         fclose(myFile);
     }

     delete[] fp2;
     fp2 = NULL;

     return 1;
}






HRESULT JPEG2RGBEx(BYTE* psrcData, int iSrcSize, sv::SV_IMAGE* pRGBImage)
{
    if ( psrcData == NULL || iSrcSize == 0 )
        return S_FALSE;

    if ( pRGBImage == NULL )
        return S_FALSE;

    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;


    JSAMPARRAY buffer;
    int row_stride;

    struct jpeg_error_mgr *ppub = &jerr.pub;
    cinfo.err = jpeg_std_error(ppub);
    //jerr.pub.error_exit = my_error_mgr;
    //jerr.pub.error_exit = my_error_mgr;

    if (setjmp(jerr.setjmp_buffer))
    {
        return S_FALSE;
    }

   jpeg_create_decompress(&cinfo);
   jpeg_mem_src(&cinfo, psrcData, iSrcSize);


   jpeg_read_header(&cinfo, TRUE);

   // printf("image width is %d\n", cinfo.image_width);
 /* nominal image width (from SOF marker) */
   // printf("image height is %d\n", cinfo.image_height);
 /* nominal image width (from SOF marker) */
    // printf("numcom is %d\n", cinfo.num_components);
/* nominal image width (from SOF marker) */

    cinfo.out_color_space = JCS_RGB;



     jpeg_start_decompress(&cinfo);
    // printf("image width is %d\n", cinfo.output_width);
 /* nominal image width (from SOF marker) */
  //  printf("image height is %d\n", cinfo.output_height);
 /* nominal image width (from SOF marker) */
   // printf("numcom is %d\n", cinfo.output_components);
/* nominal image width (from SOF marker) */


    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    int iSize = cinfo.output_width * cinfo.output_components*cinfo.output_height + 1024;

    BYTE *pTempBuffer = (BYTE*)pRGBImage->m_pData[0];
    BYTE *pSrc = pTempBuffer;

    int itatalLen = 0;

     pRGBImage->m_nType = sv::SV_IMAGE_RGB;
     pRGBImage->m_nWidth = cinfo.output_width;
     pRGBImage->m_nHeight = cinfo.output_height;

     pRGBImage->m_rgStrideWidth[0] = row_stride;


     while (cinfo.output_scanline < cinfo.output_height)
     {
         jpeg_read_scanlines(&cinfo, buffer, 1);

         //put_scanline_someplace(buffer[0], row_stride);
         //fwrite(buffer[0], 1, row_stride, outfile);
         memcpy(pTempBuffer, buffer[0], row_stride);

        for(int i=0; i<cinfo.output_width; i++)
        {
            int ibase = i*cinfo.output_components;
            BYTE r = pTempBuffer[ibase];
           BYTE g = pTempBuffer[ibase+1];
           BYTE b = pTempBuffer[ibase+2];
            pTempBuffer[ibase] = b;
           pTempBuffer[ibase+2] = r;
            //bm.SetPixel(i,line,Color(255,(BYTE)buffer[0][i*3],(BYTE)buffer[0][i*3+1],(BYTE)buffer[0][i*3+2]));
        }

        //row_stride = cinfo.output_width * cinfo.output_components;

        pTempBuffer+=row_stride;
        itatalLen += row_stride;
     }

     //
     //pRGBImage->m_pData[0] = (sv::SV_UINT8*)pSrc;
     //delete[] pTempBuffer;
     //TempBuffer = NULL;


    /*
     char *fp2 = new char[1024*1024*10];
     memset(fp2, 0, 1014*1024*10);
     RGB2BMP1((char*)pSrc,cinfo.output_width,cinfo.output_height,fp2);*/
    /*
    FILE *myfile = fopen("./rgb.bin", "wb");
    if (myfile)
    {
        fwrite(pSrc, itatalLen ,1,myfile);
        fclose(myfile);
    }
    */


    //RGB2BMPEx((char*)pRGBImage->m_pData[0],cinfo.output_width,cinfo.output_height);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);


    return S_OK;
}


