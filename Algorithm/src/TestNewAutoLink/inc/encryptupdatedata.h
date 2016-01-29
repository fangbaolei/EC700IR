#ifndef _ENCRYPTUPDATEDATA_INCLUDE_
#define _ENCRYPTUPDATEDATA_INCLUDE_

#include "HsKey.h"
#include "HvSecurity.h"
#include "HvStream.h"
#include "FastCrc32.h"

//加密文件结构
typedef struct tagHeadFileType{
	DWORD32 dwFileSerialNumber;
	DWORD32 dwFileVersion;
	DWORD32 dwFileLength;
	char    szFileInfo[ 32 ];
	DWORD32 dwOffset;
}HeadFileType;

//文件头信息
typedef struct tagUpdateDataHead{
	DWORD32 dwCount;
	HeadFileType rgDataInfo[32];
	
	tagUpdateDataHead()
	{
		dwCount = 0;
	}
}DATA_HEAD_INFO;

const char* ENCRYPT_FILE_NAME = "updatedata.stg";

//const char* HVDSP_NORMAL_DIR_NAME    = "HvDspOut_Normal";
//const char* HVDSP_TOLLGATE_DIR_NAME  = "HvDspOut_Tollgate";
//const char* HVDSP_HIGHWAY_DIR_NAME   = "HvDspOut_Highway";
//
//const char* FLASH_NORMAL_DIR_NAME    = "HvFlash_Normal";
//const char* FLASH_TOLLGATE_DIR_NAME  = "HvFlash_Tollgate";
//const char* FLASH_HIGHWAY_DIR_NAME   = "HvFlash_Highway";
#ifdef _HIGHWAY_VERSION
const char* HVDSP_DIR_NAME = "HvDspOut_Highway";
const char* FLASH_DIR_NAME = "HvFlash_Highway";
#elif _TOLLGATE_VERSION
const char* HVDSP_DIR_NAME = "HvDspOut_Tollgate";
const char* FLASH_DIR_NAME = "HvFlash_Tollgate";
#else
const char* HVDSP_DIR_NAME = "HvDspOut_Normal";
const char* FLASH_DIR_NAME = "HvFlash_Normal";

const char* IPT_DIR_NAME = "IPT_Normal";
#endif

const char* DETECTOR_DIR_NAME = "Detector";
const char* RECOG_DIR_NAME    = "Recog";

const char* DATA_HEAD_NAME = "data.h";
const char* DATA_FILE_NAME = "data.dat";

const char* HPRDSP_DIR_NAME  = "HPR_DSP";
const char* HPRFALSH_DIR_NAME = "HPR_FLASH";

#endif