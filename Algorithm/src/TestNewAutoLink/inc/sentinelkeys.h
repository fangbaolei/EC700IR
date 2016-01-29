/*******************************************************************/
/*                                                                 */
/*         Copyright (C) 2007 SafeNet, Inc.                        */
/*                   All Rights Reserved                           */
/*                                                                 */
/*     This Module contains Proprietary Information of             */
/*    SafeNet, Inc, and should be treated as Confidential.         */
/*******************************************************************/
/******************************************************************************
* FILENAME  - SentinelKeys.h
*   
* USAGE     - Function prototypes for library function.
*
* REVISION HISTORY - 
******************************************************************************/ 
#ifndef _SENTINELKEYS_HEAD_H
#define _SENTINELKEYS_HEAD_H

#include "SentinelKeystypes.h"

/*SFNTGetLicense flags*/
#define SP_TCP_PROTOCOL                     0x00000001
#define SP_IPX_PROTOCOL                     0x00000002
#define SP_NETBEUI_PROTOCOL                 0x00000004
#define SP_STANDALONE_MODE                  0x00000020
#define SP_SERVER_MODE                      0x00000040
#define SP_SHARE_ON                         0x00000080
#define SP_GET_NEXT_LICENSE                 0x00000400
#define SP_ENABLE_TERMINAL_CLIENT           0x00000800

/*Query feature flags */
#define SP_SIMPLE_QUERY						1
#define SP_CHECK_DEMO                       0

/*Form factor type*/
#define SP_USB								0x0001
#define SP_PARALLEL							0x0010

/*Device Capabilities*/
#define SP_CAPS_AES_128						0x00000001
#define SP_CAPS_ECC_K163					0x00000002
#define SP_CAPS_ECC_KEYEXCH					0x00000004
#define SP_CAPS_ECC_SIGN					0x00000008
#define SP_CAPS_TIME_SUPP					0x00000010
#define SP_CAPS_TIME_RTC					0x00000020

/*Feature Attributes*/
#define SP_ATTR_WRITE_ONCE					0x0200	/* For all feature */
#define SP_ATTR_ACTIVE						0x0020	/* For Key feature only */
#define SP_ATTR_AUTODEC						0x0010	/* For Key feature only */
#define SP_ATTR_SIGN						0x0004	/* For ECC feature only */
#define SP_ATTR_DECRYPT						0x0002  /* For AES feature only */
#define SP_ATTR_ENCRYPT						0x0001  /* For AES feature only */
#define SP_ATTR_SECMSG_READ					0x0080  /* For data feature only */

/*SFNTEnumServer flags*/
#define SP_RET_ON_FIRST_AVAILABLE           1 /* first found Sentinel Key Server that has a license to offer */
#define SP_GET_ALL_SERVERS                  4 /* all the Sentinel Key Servers in the subnet */

#define SP_MAX_NUM_SERVERS					10 /* The maximum number of servers that can be enumerated */

/*Feature Type*/
#define DATA_FEATURE_TYPE_BOOLEAN				1
#define DATA_FEATURE_TYPE_BYTE					2
#define DATA_FEATURE_TYPE_WORD					3
#define DATA_FEATURE_TYPE_DWORD					4
#define DATA_FEATURE_TYPE_RAW					5
#define DATA_FEATURE_TYPE_STRING				6
#define FEATURE_TYPE_COUNTER					7
#define FEATURE_TYPE_AES					8
#define FEATURE_TYPE_ECC					9

/*Length definition*/
#define	SP_PUBILC_KEY_LEN					42
#define	SP_SOFTWARE_KEY_LEN					112
#define	SP_MIN_ENCRYPT_DATA_LEN					16
#define	SP_MAX_QUERY_LEN					112
#define	SP_MAX_RAW_LEN						256
#define	SP_MAX_STRING_LEN					256
#define SP_MAX_SIGN_BUFFER_LEN					0xFFFFFFFF

/*Heartbeat Interval Scope*/
#define SP_MAX_HEARTBEAT					2592000
#define SP_MIN_HEARTBEAT					60
#define SP_INFINITE_HEARTBEAT					0xFFFFFFFF

#pragma pack(8)

typedef struct SP_DateTime
{
    SP_DWORD        year;
    SP_BYTE         month;
    SP_BYTE         dayOfMonth;
    SP_BYTE         hour;
    SP_BYTE         minute;
    SP_BYTE         second;
} SP_DATE_TIME, *SPP_DATE_TIME;

typedef struct SP_TimeCtrl
{
    SP_DWORD        duration;
    SP_DATE_TIME    stopTime;
} SP_TIME_CONTROL, *SPP_TIME_CONTROL;

typedef struct SP_Feature_Info
{
    SP_DWORD        featureType;
    SP_DWORD        featureSize;
    SP_DWORD        featureAttributes;
    SP_BOOLEAN      bEnableCounter;
    SP_BOOLEAN      bEnableStopTime;
    SP_BOOLEAN      bEnableDurationTime;
    SP_TIME_CONTROL timeControl;
    SP_DWORD        leftExecutionNumber;
} SP_FEATURE_INFO, *SPP_FEATURE_INFO;

typedef struct SP_License_Info
{
    SP_DWORD        licenseID;
    SP_DWORD        userLimit;
    SP_DWORD        featureNums;
    SP_DWORD        licenseSize;
} SP_LICENSE_INFO, *SPP_LICENSE_INFO;

typedef struct SP_Device_Info
{
    SP_DWORD        formFactorType;
    SP_DWORD        productCode;
    SP_DWORD        hardlimit;
    SP_DWORD        capabilities;
    SP_DWORD        devID;
    SP_DWORD        devSN;
    SP_DATE_TIME    timeValue;
    SP_DWORD        memorySize;
    SP_DWORD        freeSize;
    SP_DWORD        drvVersion;
} SP_DEVICE_INFO, *SPP_DEVICE_INFO;

typedef struct SP_Server_Info
{
    SP_CHAR         serverName[64];
    SP_WORD         protocols;
    SP_WORD         majorVersion;
    SP_WORD         minorVersion;
} SP_SERVER_INFO, *SPP_SERVER_INFO;

typedef struct SP_Enum_Server_Info
{
   SP_CHAR          serverAddress[64];
   SP_WORD          numLicAvail;
}  SP_ENUM_SERVER_INFO, *SPP_ENUM_SERVER_INFO;

#pragma pack()

/*Core obj public interfaces*/
SP_EXPORT SP_STATUS SP_API SFNTGetLicense(
    SP_IN  SP_DWORD         devID,
    SP_IN  SPP_BYTE         softwareKey,
    SP_IN  SP_DWORD         licID,
    SP_IN  SP_DWORD         flags,
    SP_IO  SPP_HANDLE       licHandle);

SP_EXPORT SP_STATUS SP_API SFNTQueryFeature(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SP_DWORD         flags,
    SP_IN  SPP_BYTE         query,
    SP_IN  SP_DWORD         queryLength,
    SP_OUT SPP_BYTE         response,
    SP_IN  SP_DWORD         responseLength );

SP_EXPORT SP_STATUS SP_API SFNTReadString(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_OUT SPP_CHAR         string,
    SP_IN  SP_DWORD         stringLength );

SP_EXPORT SP_STATUS SP_API SFNTWriteString(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SPP_CHAR         string,
    SP_IN  SP_DWORD         writePassword );

SP_EXPORT SP_STATUS SP_API SFNTReadInteger(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_OUT SPP_DWORD        value );

SP_EXPORT SP_STATUS SP_API SFNTWriteInteger(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SP_DWORD         value,
    SP_IN  SP_DWORD         writePassword );

SP_EXPORT SP_STATUS SP_API SFNTReadRawData(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_OUT SPP_VOID         buffer,
    SP_IN  SP_DWORD         offset,
    SP_IN  SP_DWORD         length );

SP_EXPORT SP_STATUS SP_API SFNTWriteRawData(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SPP_VOID         buffer,
    SP_IN  SP_DWORD         offset,
    SP_IN  SP_DWORD         length,
    SP_IN  SP_DWORD         writePassword );

SP_EXPORT SP_STATUS SP_API SFNTCounterDecrement(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SP_DWORD         decrementValue );

SP_EXPORT SP_STATUS SP_API SFNTEncrypt(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SPP_BYTE         plainBuffer,
    SP_OUT SPP_BYTE         cipherBuffer );

SP_EXPORT SP_STATUS SP_API SFNTDecrypt(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SPP_BYTE         cipherBuffer,
    SP_OUT SPP_BYTE         plainBuffer );


SP_EXPORT SP_STATUS SP_API SFNTVerify(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SPP_BYTE         publicKey,
    SP_IN  SPP_BYTE         signBuffer,
    SP_IN  SP_DWORD         length,
    SP_IN  SPP_BYTE         signResult );

SP_EXPORT SP_STATUS SP_API SFNTSign(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_IN  SPP_BYTE         signBuffer,
    SP_IN  SP_DWORD         length,
    SP_OUT SPP_BYTE         signResult );

SP_EXPORT SP_STATUS SP_API SFNTSetHeartbeat(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         heartBeatValue );

SP_EXPORT SP_STATUS SP_API SFNTGetLicenseInfo(
    SP_IN  SP_HANDLE        licHandle,
    SP_OUT SPP_LICENSE_INFO licenseInfo );

SP_EXPORT SP_STATUS SP_API SFNTGetFeatureInfo(
    SP_IN  SP_HANDLE        licHandle,
    SP_IN  SP_DWORD         featureID,
    SP_OUT SPP_FEATURE_INFO featureInfo );

SP_EXPORT SP_STATUS SP_API SFNTGetDeviceInfo(
    SP_IN  SP_HANDLE        licHandle,
    SP_OUT SPP_DEVICE_INFO  deviceInfo );

SP_EXPORT SP_STATUS SP_API SFNTGetServerInfo(
    SP_IN  SP_HANDLE        licHandle,
    SP_OUT SPP_SERVER_INFO  serverInfo );

SP_EXPORT SP_STATUS SP_API SFNTReleaseLicense(
    SP_IN  SP_HANDLE        licHandle );


SP_EXPORT SP_STATUS SP_API SFNTSetContactServer(
    SP_IN  SPP_CHAR         serverAddr );

SP_EXPORT SP_STATUS SP_API SFNTEnumServer(
    SP_IN  SP_DWORD         developerID,
	SP_IN  SP_DWORD			licID,
    SP_IN  SP_DWORD         enumFlag,
    SP_OUT SPP_ENUM_SERVER_INFO srvInfo,
    SP_IO  SPP_DWORD        numSrvInfo );

#endif
