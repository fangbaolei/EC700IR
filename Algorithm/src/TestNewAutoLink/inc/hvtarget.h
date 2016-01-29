#ifndef _HVTARGET_H
#define _HVTARGET_H

#ifdef __cplusplus
	extern "C" {
#endif	/* #ifdef __cplusplus */


#include "dspsys_def.h"

typedef struct _SYS_WORK_TIMER {
	int iGetType;
	DWORD32 dwMSCountLow;
	DWORD32 dwMSCountHigh;
} SYS_WORK_TIMER;

extern SYS_WORK_TIMER SysWorkTimer;

/* Ã¶¾ÙÃèÊö:ÏµÍ³Ö§³Ö´®¿ÚºÅºê¶¨Òå. */
typedef enum {
	SYS_DEBUG_COM 		= 0,
	SYS_CTRL_COM		= 1,
	MAX_SYS_COM_NUM		= 2
} SYS_COM_PORT;

/* Ã¶¾ÙÃèÊö:´®¿ÚÖÕ¶ËÓÃ»§ÊäÈë¹Ø¼ü×Ö·û±êÖ¾,ÒÔÏÂÎª·µ»ØÀàÐÍ³£Á¿¶¨Òå. */
typedef enum {
	USER_KEY_ESC		= 1,
	USER_KEY_BACKSPACE 	= 2,
	USER_KEY_ENTER		= 4
} USER_KEY_VALUE;

/* Ã¶¾ÙÃèÊö:DSPµÄÆô¶¯·½Ê½. */
typedef enum {
	DSP_BOOT_EMULATION	= 0,
	DSP_BOOT_HPI		= 1,
	DSP_BOOT_RESERVED	= 2,
	DSP_BOOT_EMIFA		= 3
} DSP_BOOT_MODE;

/* Ã¶¾ÙÃèÊö:DSPµÄÄÚ´æÓ³Éä·½Ê½. */
typedef enum {
	DSP_MEMMAP_EXTERNAL = 0,			/* Íâ²¿´æ´¢Æ÷µØÖ·´Ó0¿ªÊ¼. */
	DSP_MEMMAP_INTERNAL	= 1				/* ÄÚ²¿´æ´¢Æ÷µØÖ·´Ó0¿ªÊ¼. */
} DSP_MEMMAP_MODE;

/* Êý¾Ý½á¹¹ËµÃ÷:¸ÃÊý¾Ý½á¹¹¸÷ÓòÓÃÀ´Ö¸Ê¾DSPµÄÓÐ¹ØÆ¬ÉÏÉè±¸ÐÅÏ¢ÒÔ¼°DSP°æ±¾ÐÍºÅÐÅÏ¢. */
typedef struct _DSP_CHIP_INFO {
	BYTE8				bCpuTypeId;		/* DSP²úÆ·ÀàÐÍIDºÅ.											*/					
	BYTE8 				bCpuVerId;		/* DSP°æ±¾IDºÅ.                                             */
	BYTE8				bEndianMode;	/* DSPÕýÔÚÊ¹ÓÃµÄ×Ö½ÚÐò,Îª1±íÊ¾ÎªµÍ×Ö½ÚÐò,Îª0±íÊ¾Îª¸ß×Ö½ÚÐò. */
	BYTE8				bVpConfig;		/* DSPµÄÆ¬ÉÏÍâÉèVP¶Ë¿ÚÊ¹ÓÃÇé¿ö,bit0--VP0,bit1-VP1,bit2--VP2 */
	DSP_BOOT_MODE		DspBootMode;	/* DSPµ±Ç°ËùÊ¹ÓÃµÄÆô¶¯Ä£Ê½.									*/					
	DSP_MEMMAP_MODE		DspMemMapMode;	/* DSPµÄÄÚ´æÓ³ÉäÄ£Ê½(ÔÚÃ»ÓÐÊ¹ÓÃL2 CacheµÄÇé¿öÏÂ),			*/
										/* Îª0±íÊ¾ÎªÍâ²¿µØÖ·´Ó0¿ªÊ¼,Îª1±íÊ¾ÄÚ²¿µØÖ·´Ó0¿ªÊ¼.			*/	
	BOOL				fEmacEnable;	/* DSPµÄÆ¬ÉÏÍâÉèEMACÊÇ·ñÊ¹ÄÜ.                               */
	BOOL				fPciEnable;		/* DSPµÄÆ¬ÉÏÍâÉèPCIÊÇ·ñÊ¹ÄÜ.	                            */
} DSP_CHIP_INFO;

/* Êý¾Ý½á¹¹ËµÃ÷:¸ÃÊý¾Ý½á¹¹ÓÃÀ´Ö¸Ê¾Ä¿±ê°åµÄÏµÍ³¼¶ÐÅÏ¢. */
typedef struct _TARGET_SYSTEM_INFO {
	DWORD32 			dwTargetTypeId;	/* Ä¿±ê°åµÄÓ²¼þÆ½Ì¨ÀàÐÍ°æ±¾ºÅ.								*/
	DSP_CHIP_INFO		DspChipInfo;	/* ÓÐ¹ØDSPÆ¬ÉÏÍâÉèÊ¹ÄÜÅäÖÃÒÔ¼°µ±Ç°Ê¹ÓÃDSPÄ£Ê½ÓÐ¹ØÐÅÏ¢.	    */
} TARGET_SYSTEM_INFO;


extern DWORD32 g_dwLed;

extern int g_iResetFlag;

__inline int GetResetFlag()
{
	return g_iResetFlag;
}

__inline void SetResetFlag()
{
	g_iResetFlag = 1;
}

/* »ù±¾²Ù×÷APIÉùÃ÷¼°½Ó¿ÚËµÃ÷. */
/*------------------------------------------------------------------------------------------------------*/
/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÎªºÁÃëÎªµ¥Î»µÄÑÓ³Ù,ÑÓ³ÙÊ±¼äÎª"dwDelayTime * 1ms ".º¯ÊýÎÞ·µ»ØÖµ. */
extern void Target_DelayMS(
	DWORD32 					dwDelayTime		//ÑÓ³ÙMS´ÎÊý.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´È¡µÃÄ¿±êÏµÍ³Ó²¼þ°æ±¾ºÅºÍÍâÉèµÄÔÚ¹ØÐÅÏ¢.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾È¡ÐÅÏ¢³É¹¦;	
	·µ»ØE_POINTER±íÊ¾pSysInfoÎªINVALID_POINTER; */
extern HRESULT GetTargetSysInfo( 
	TARGET_SYSTEM_INFO 			*pSysInfo		//Ö¸ÏòÈ¡µÃÆ½Ì¨ÓÐ¹ØÐÅÏ¢Êý¾Ý.
);

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´¹Ø±ÕÈ«¾ÖÖÐ¶Ï,º¯Êý·µ»Ø¹Ø±ÕÈ«¾ÖÖÐ¶ÏÖ®Ç°µÄÈ«¾ÖÖÐ¶ÏÊ¹ÄÜ. */
extern DWORD32 CloseGlobalInterrupt( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´»Ö¸´È«¾ÖÖÐ¶Ï,¸ù¾Ý´«ÈëµÄÈ«¾ÖÖÐ¶ÏÊ¹ÄÜÇé¿ö. */
extern void RestoreGlobalInterrupt( DWORD32 dwGie );

/* º¯ÊýËµÃ÷:¹Ø±ÕÈ«¾ÖÖÐ¶ÏÊ¹ÄÜ. */
extern void CloseAllInterrupt( void );

/* º¯ÊýËµÃ÷:Ê¹ÄÜÈ«¾ÖÖÐ¶Ï. */
extern void EnableAllInterrupt( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁ»òÕßÏ¨ÃðÏµÍ³Æ½Ì¨µÄ¹¤×÷Ö¸Ê¾µÆ,Èçµ±Ç°LEDµÆÎªÁÁÔòµ÷ÓÃ¸Ãº¯ÊýÖ®ºóµÆ¼´Ãð,º¯ÊýÎÞ·µ»ØÖµ. */
extern void WorkLedLight( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁ»òÕßÏ¨ÃðÏµÍ³Æ½Ì¨µÄLANµÆ,Èçµ±Ç°LEDµÆÎªÁÁÔòµ÷ÓÃ¸Ãº¯ÊýÖ®ºóµÆ¼´Ãð,º¯ÊýÎÞ·µ»ØÖµ. */
extern void LanLedLight( void );
extern void SetLanLedOn( void );
extern void SetLanLedOff( void );
/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁ»òÕßÏ¨ÃðÏµÍ³Æ½Ì¨µÄHardÖ¸Ê¾µÆ,Èçµ±Ç°LEDµÆÎªÁÁÔòµ÷ÓÃ¸Ãº¯ÊýÖ®ºóµÆ¼´Ãð,º¯ÊýÎÞ·µ»ØÖµ. */
extern void HardLedLight( void );
extern void SetHardLedOn( void );
extern void SetHardLedOff( void );
/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁ»òÕßÏ¨ÃðÏµÍ³Æ½Ì¨µÄAlarm1Ö¸Ê¾µÆ,Èçµ±Ç°LEDµÆÎªÁÁÔòµ÷ÓÃ¸Ãº¯ÊýÖ®ºóµÆ¼´Ãð,º¯ÊýÎÞ·µ»ØÖµ. */
extern void Alarm1LedLight( void );
extern void SetAlarm1LedOn( void );
extern void SetAlarm1LedOff( void );
/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁ»òÕßÏ¨ÃðÏµÍ³Æ½Ì¨µÄAlarm2Ö¸Ê¾µÆ,Èçµ±Ç°LEDµÆÎªÁÁÔòµ÷ÓÃ¸Ãº¯ÊýÖ®ºóµÆ¼´Ãð,º¯ÊýÎÞ·µ»ØÖµ. */
extern void Alarm2LedLight( void );
extern void SetAlarm2LedOn( void );
extern void SetAlarm2LedOff( void );
/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÈÃ·åÃùÆ÷Ïì.º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetBuzzerOn( void );
/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÈÃ·åÃùÆ÷²»Ïì.º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetBuzzerOff( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁÏµÍ³Æ½Ì¨ÉÏ×÷Îª¹¤×÷Ö¸Ê¾µÆ.º¯ÊýÎÞÐÎ²Î,º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetWorkLedOn( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´Ï¨ÃðÏµÍ³Æ½Ì¨ÉÏ×÷Îª¹¤×÷Ö¸Ê¾µÆ.º¯ÊýÎÞÐÎ²Î,º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetWorkLedOff( void );

// ÉÏº£½»¼Æ¶ÀÓÐµÄLEDµÆ²Ù×÷
extern void WorkLedLight_Jiaoji( void );
extern void SetWorkLedOn_Jiaoji( void );
extern void SetWorkLedOff_Jiaoji( void );
extern void SetHardLedOn_Jiaoji( void );
extern void SetHardLedOff_Jiaoji( void );
extern void Lan1LedLight_Jiaoji( void );
extern void SetLan1LedOn_Jiaoji( void );
extern void SetLan1LedOff_Jiaoji( void );
extern void SetLan2LedOn_Jiaoji( void );
extern void SetLan2LedOff_Jiaoji( void );
extern void SetAlm1LedOn_Jiaoji( void );
extern void SetAlm1LedOff_Jiaoji( void );
extern void SetAlm2LedOn_Jiaoji( void );
extern void SetAlm2LedOff_Jiaoji( void );
extern void Link1LedLight_Jiaoji( void );
extern void Link2LedLight_Jiaoji( void );
extern void Link3LedLight_Jiaoji( void );
extern void Link4LedLight_Jiaoji( void );
extern void Link5LedLight_Jiaoji( void );
extern void Link6LedLight_Jiaoji( void );
extern void Link7LedLight_Jiaoji( void );
extern void Link8LedLight_Jiaoji( void );
extern void SetLink1LedOn_Jiaoji( void );
extern void SetLink1LedOff_Jiaoji( void );
extern void SetLink2LedOn_Jiaoji( void );
extern void SetLink2LedOff_Jiaoji( void );
extern void SetLink3LedOn_Jiaoji( void );
extern void SetLink3LedOff_Jiaoji( void );
extern void SetLink4LedOn_Jiaoji( void );
extern void SetLink4LedOff_Jiaoji( void );
extern void SetLink5LedOn_Jiaoji( void );
extern void SetLink5LedOff_Jiaoji( void );
extern void SetLink6LedOn_Jiaoji( void );
extern void SetLink6LedOff_Jiaoji( void );
extern void SetLink7LedOn_Jiaoji( void );
extern void SetLink7LedOff_Jiaoji( void );
extern void SetLink8LedOn_Jiaoji( void );
extern void SetLink8LedOff_Jiaoji( void );
extern void Stat1LedLight_Jiaoji( void );
extern void Stat2LedLight_Jiaoji( void );
extern void Stat3LedLight_Jiaoji( void );
extern void Stat4LedLight_Jiaoji( void );
extern void Stat5LedLight_Jiaoji( void );
extern void Stat6LedLight_Jiaoji( void );
extern void Stat7LedLight_Jiaoji( void );
extern void Stat8LedLight_Jiaoji( void );
extern void SetStat1LedOn_Jiaoji( void );
extern void SetStat1LedOff_Jiaoji( void );
extern void SetStat2LedOn_Jiaoji( void );
extern void SetStat2LedOff_Jiaoji( void );
extern void SetStat3LedOn_Jiaoji( void );
extern void SetStat3LedOff_Jiaoji( void );
extern void SetStat4LedOn_Jiaoji( void );
extern void SetStat4LedOff_Jiaoji( void );
extern void SetStatk5LedOn_Jiaoji( void );
extern void SetStat5LedOff_Jiaoji( void );
extern void SetStat6LedOn_Jiaoji( void );
extern void SetStat6LedOff_Jiaoji( void );
extern void SetStat7LedOn_Jiaoji( void );
extern void SetStat7LedOff_Jiaoji( void );
extern void SetStat8LedOn_Jiaoji( void );
extern void SetStat8LedOff_Jiaoji( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´½«WDT»úÖÆ¹·µÄÎ¹¹·µ±Ç°×´Ì¬ÖÃÎªµÍ.º¯ÊýÎÞÐÎ²Î,º¯ÊýÎÞ·µ»ØÖµ. */
extern void ClearWDT( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´½«WDT»úÖÆ¹·µÄÎ¹¹·µ±Ç°×´Ì¬ÖÃÎª¸ß.º¯ÊýÎÞÐÎ²Î,º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetWDT( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´½«WDT»úÖÆ¹·µÄ×´Ì¬ÓÐ¸öÌø±ä. º¯ÊýÎÞÐÎ²Î,º¯ÊýÎÞ·µ»ØÖµ. */
extern void ResWDT( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´È¡µÃÏµÍ³Æ½Ì¨´¥·¢ÊäÈëµÄµ±Ç°×´Ì¬Öµ.º¯ÊýÎÞÐÎ²Î,º¯Êý·µ»ØÖµµÄ×îµÍÎ»ÓÐÐ§. */
extern BYTE8 GetTriggerIn( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´È¡µÃÏµÍ³Æ½Ì¨´¥·¢ÊäÈëµÄµ±Ç°×´Ì¬Öµ.º¯ÊýÎÞÐÎ²Î,º¯Êý·µ»ØÖµµÄ×îµÍÎ»ÓÐÐ§. */
extern BYTE8 GetTriggerIn_2( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´¸ù¾ÝbSetBitÎ»ÖµÉèÖÃ´¥·¢Êä³öµÄ×´Ì¬.º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetTriggerOut(
	BYTE8 						bSetBit			//ÉèÖÃÖµ,Ö»ÓÐ×îºóÒ»Î»ÓÐÐ§.
);

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´¸ù¾ÝbSetBitÎ»ÖµÉèÖÃ´¥·¢Êä³öµÄ×´Ì¬.º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetTriggerOutEx(
	BYTE8 bSetBit,			//ÉèÖÃÖµ,Ö»ÓÐ×îºóÒ»Î»ÓÐÐ§.
	DWORD32 dwVideoID		//ÊÓÆµID
);

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´³õÊ¼»¯µ±Ç°ÏµÍ³LEDµÆÄMASK.*/
extern void InitLedMask();

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´È¡µÃµ±Ç°ÏµÍ³LEDµÆµÄMASK.º¯ÊýÖ±½Ó·µ»Ø¸ÃMASK. */
extern DWORD32 GetLedMask( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´µãÁÁÆ½Ì¨ÉÏÊ¹ÓÃdwLedMaskËù±êÊ¶µÄGPIOÖ¸Ê¾µÆ(Ê¹ÓÃDSPµÄGPIOÒý½ÅµÄµÆ). */
extern void SetLedOn( 
	DWORD32 					dwLedMask		//LEDµÆµÄGPIO MASK.
);

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´Ï¨ÃðÆ½Ì¨ÉÏÊ¹ÓÃdwLedMaskËù±êÊ¶µÄGPIOÖ¸Ê¾µÆ(Ê¹ÓÃDSPµÄGPIOÒý½ÅµÄµÆ). */
extern void SetLedOff( 
	DWORD32 					dwLedMask		//LEDµÆµÄGPIO MASK.
);

/* º¯ÊýËµÃ÷:º¯ÊýÖ±½Ó·µ»ØÓ²¼þÆ½Ì¨ÌøÏßÊäÈë. */
extern DWORD32 GetJumperInput( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´È¡µÃÓ²¼þÆ½Ì¨ÓÃ»§°ÎÂë¿ª¹ØÊäÈëÖµ.º¯ÊýÎÞÐÎ²Î.º¯ÊýÖ±½Ó·µ»Ø¸ÃÖµ. */
extern DWORD32 GetUserConfigPort( void );
/*------------------------------------------------------------------------------------------------------------------------------------*/




/* ÍâÉè²Ù×÷º¯ÊýAPIÉùÃ÷¼°½Ó¿ÚËµÃ÷. */
/*------------------------------------------------------------------------------------------------------------------------------------*/
/* 
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´È¡µÃIDÐ¾Æ¬µÄIDºÅ,¸ÃID¾ßÌåÒâÒåÓÉ¸ÃAPIÊ¹ÓÃÕß½âÊÍ.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾¶ÁÈ¡³É¹¦.	
	·µ»ØE_POINTER±íÊ¾pbIdValueÎªINVALID_POINTER.	
	·µ»ØE_NOTIMPL±íÊ¾µ±Ç°Ó²¼þÆ½Ì¨²»Ö§³Ö.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ö»·µ»ØIDºÅµÄ²¿·ÖÊý¾Ý»òÕßdwBytesToRead³¬¹ýÊµ¼ÊÎ¨Ò»IDÊý¾Ý³¤¶È.	*/
extern HRESULT GetUniqueId( 
	PBYTE8						pbIdValue,		//Ö¸ÏòÈ¡µÃµÄIDºÅÄÚÈÝ.
	DWORD32						dwBytesToRead,	//ÆÚÍû¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

#ifdef _HV_PRODUCT
/* 
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´´ÓFLASHÖÐ¶ÁÈ¡Êý¾Ý,µ±Ç°Ó²¼þÆ½Ì¨ËùÓÐ¶ÔFLASHµÄ¶Á²Ù×÷±ØÐëÍ¨¹ý¸Ãº¯Êý²ÅÄÜ±£Ö¤´ÓFLASH¶Á
	È¡ÕýÈ·/ÓÐÐ§.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾¶ÁÈ¡³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbMemDstÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,¶ÁÈ¡µÄÊý¾Ý³¬³öFLASHµ±Ç°ÈÝÁ¿µÈ.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾¶ÁÈ¡FLASH³ö´í. */
extern HRESULT FLASH_Read(
	DWORD32 					dwSrcPos,		//´ÓFLASH¶ÁÈ¡Êý¾ÝµÄÆðÊ¼Î»ÖÃ.
	PBYTE8						pbMemDst, 		//Ö¸ÏòÄÚ´æÖÐ·ÅÖÃ¶Áµ½µÄÊý¾Ý.
	BYTE8						bReadType,		//FLASH¶ÁÈ¡Ä£Ê½,Îª0Ê±±íÊ¾ÎªÍ¸Ã÷¶Á,Îª1Îª¼ÓÃÜ¶Á
	DWORD32						dwBytesToRead,	//ÆÚÍû¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
	
);

/* 
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÏòFLASHÖÐÐ´ÈëÊý¾Ý,µ±Ç°Ó²¼þÆ½Ì¨ËùÓÐ¶ÔFLASHµÄÐ´²Ù×÷±ØÐëÍ¨¹ý¸Ãº¯Êý²ÅÄÜ±£Ö¤ÏòFLASHÖÐÐ´Èë
	Êý¾ÝÕýÈ·/ÓÐÐ§(µ«ÊÇËùÐ´µÄFLASHÒ³ÇøÓò±ØÐëÏÈ±»ERASE¹ý).
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ð´Èë²Ù×÷³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbMemSrcÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Ð´ÈëµÄÊý¾Ý³¬³öFLASHµ±Ç°ÈÝÁ¿µÈ.
	·µ»ØE_NOTIMPL±íÊ¾µ±Ç°FLASHÐ´²»Ö§³Ö.	
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ð´²Ù×÷FLASH³ö´í. */
extern HRESULT FLASH_Write( 
	const PBYTE8 				pbMemSrc,		//Ö¸ÏòËùÒª¶ÁÈ¡µÄÄÚ´æÊý¾Ý.
	DWORD32 					dwDstPos,		//FLASHÐ´ÈëÊý¾ÝÄ¿±êÆðÊ¼Î»ÖÃ.
	BYTE8 						bWriteType,		//FLASHÐ´Ä£Ê½,0ÎªÍ¸Ã÷Ð´,1Îª¼ÓÃÜÐ´
	DWORD32						dwBytesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32 					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´²Á³ýFLASH¿éÊý¾Ý,»úÖÆ½«Æä´æ´¢×îÐ¡µ¥ÔªÈ«²¿ÖÃÎª1,FLASHµÄÐ´²Ù×÷Ö»ÄÜ¶Ô×îÐ¡µ¥Ôª½øÐÐ1->0µÄ
	Ð´²Ù×÷,¶ø²»¿É½øÐÐ0->1µÄ²Ù×÷,ËùÒÔÔÚÐ´ÈëÊý¾ÝÖ®Ç°ÐèÒª¶ÔÆä´æ´¢¿éÏÈ½øÐÐ²Á³ý²Ù×÷,²Á³ýµÄ×îÐ¡µ¥Î»Îª¿é.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ð´Èë²Ù×÷³É¹¦.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í.
	·µ»ØE_NOTIMPL±íÊ¾µ±Ç°FLASH²Á³ý²»Ö§³Ö.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ð´²Ù×÷FLASH³ö´í. */
extern HRESULT FLASH_Erase( 
	DWORD32						dwStartPos, 	//ÐèÒª²Á³ýFLASH¿éÊ×Î»ÖÃ.
	DWORD32 					dwLength		//µ±Ç°ÐèÒª²Á³ý³¤¶È,ÒÔ×Ö½ÚÎªµ¥Î».
);

extern HRESULT FLASH_RandomWrite(
	DWORD32 					dwFlashAddr, 
	BYTE8 						*pSrcData, 
	int 						iLen
);
#endif

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÉèÖÃFLASH´æ´¢Ê±ËùÊ¹ÓÃµÄ¼ÓÃÜËã×Ó.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾ÉèÖÃ³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbSrcCodeÎªINVALID_POINTER. */
extern HRESULT SetSecurityCode( 
	const BYTE8					*pbSrcCode,		//Ö¸Ïò×÷Îª¼ÓÃÜËã×ÓµÄÊý¾Ý.
	DWORD32						dwCodeLen		//ÔËËã¼ÓÃÜËã×ÓµÄÊý¾Ý×Ö½Ú³¤¶È.
);


/* ³õÊ¼»¯EEPROM */
extern HRESULT EEPROM_Init();

/* ¶ÁÈ¡EEPROMÃÜÇøÊý¾Ý */

extern HRESULT EEPROM_FreeRead(
	DWORD32 					dwReadPos,			//¶ÁÈ¡EEPROMµÄÊ×Î»ÖÃ.
	PBYTE8						pbMemDst,			//Ö¸Ïò¶Á³öµÄÊý¾Ý.
	DWORD32 					dwBytesToRead,		//ÆÚÍû¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesRead		//Êµ¼Ê¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/* ¶ÁÈ¡EEPROM¼ÓÃÜÇøÊý¾Ý */
extern HRESULT EEPROM_EncryptRead(
	DWORD32						dwReadPos,		//¶ÁÈ¡EEPROMµÄÊ×Î»ÖÃ.
	PBYTE8 						pbMemDst, 		//Ö¸Ïò¶Á³öµÄÊý¾Ý.
	DWORD32						dwBytesToRead,	//ÆÚÍû¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesRead		//Êµ¼Ê¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/* Ð´ÈëEEPROM·Ç¼ÓÃÜÇø */
extern HRESULT EEPROM_FreeWrite(
	const PBYTE8				pbMemSrc, 		//Ö¸ÏòÐ´ÈëµÄÊý¾Ý.
	DWORD32 					dwWritePos, 	//Ð´ÈëEEPROMµÄÊ×µØÖ·.
	DWORD32						dwBytesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/* Ð´ÈëEEPROM¼ÓÃÜÇø */
extern HRESULT EEPROM_EncryptWrite(
	const PBYTE8				pbMemSrc, 		//Ö¸ÏòÐ´ÈëµÄÊý¾Ý.
	DWORD32 					dwWritePos, 	//Ð´ÈëEEPROMµÄÊ×µØÖ·.
	DWORD32						dwBytesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´´ÓEEPROMÖÐ¶ÁÈ¡Ò»¶¨Êý¾ÝµÄÊý¾Ý.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾¶ÁÈ¡EEPROM²Ù×÷³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbMemDstÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾¶ÁÈ¡EEPROM³ö´í. */
extern HRESULT EEPROM_Read(
	DWORD32 					dwReadPos,		//¶ÁÈ¡EEPROMµÄÊ×Î»ÖÃ.
	PBYTE8						pbMemDst,		//Ö¸Ïò¶Á³öµÄÊý¾Ý.
	DWORD32 					dwBytesToRead,	//ÆÚÍû¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÏòEEPROMÖÐÐ´ÈëÒ»¶¨Á¿Êý¾Ý.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ð´ÈëEEPROM²Ù×÷³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbMemSrcÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ð´ÈëEEPROM³ö´í. */
extern HRESULT EEPROM_Write(
	const PBYTE8				pbMemSrc,		//Ö¸ÏòÐ´ÈëµÄÊý¾Ý.
	DWORD32 					dwWritePos, 	//Ð´ÈëEEPROMµÄÊ×µØÖ·.
	DWORD32 					dwBytesToWrite, //ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´¶ÁÈ¡ÊµÊ±Ê±ÖÓÐ¾Æ¬ÖÐµÄÊµÊ±Ê±¼äÖµ(º¬ÈÕÆÚ),¾«È·µ½Ãë,µ«ÊÇµ±Ç°µÄÊµÊ±Ê±ÖÓÐ¾Æ¬ÄêÎó²îÎª100·ÖÖÓ×óÓÒ.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾¶ÁÈ¡RTCÈÕÆÚÊ±¼ä³É¹¦.
	·µ»ØE_POINTER±íÊ¾pRealTimeÎªINVALID_POINTER.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾¶ÁÈ¡RTCÈÕÆÚÊ±¼ä³ö´í. */
extern HRESULT RTC_GetDateTime( 
	REAL_TIME_STRUCT 			*pRealTime		//Ö¸Ïò¶Á³öµÄÊ±¼ä.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÉèÖÃÊµÊ±Ê±ÖÓÐ¾Æ¬Ê±¼ä(°üº¬ÈÕÆÚ),¾«È·µ½Ãë.ÆäÊý¾Ý´®µÄ¸÷×Ö½ÚÒâÒå¼ûÉÏÃèÊö.¸ù¾ÝÓÃ»§ËùÉèÖÃµÄÊý¾Ý
	Ð´ÈëÖÁRTCÏà¹ØÊ±¼äÈÕÆÚ¼Ä´æÆ÷ÖÐ.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾ÉèÖÃRTCÈÕÆÚÊ±¼ä³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbTimeValÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄÊµÊ±Ê±¼äÊý¾Ý·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾ÉèÖÃRTCÈÕÆÚÊ±¼ä³ö´í. */
extern HRESULT RTC_SetDateTime(
	const REAL_TIME_STRUCT 		*pRealTime		//Ö¸ÏòÉèÖÃµÄÊ±¼ä.
);

/* 
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÏòComPort´®¿ÚµÄ·¢ËÍ¶ÓÁÐÖÐÌîÐ´×Ö·û,Èç¹ûµ±Ç°¸Ã´®¿Ú¶Ë¿Ú·¢ËÍ¶ÓÁÐÂúÔò»á×èÈûµÈ´ý¶¨Ê±ÖÐ¶Ï
	(¶¨Ê±ÖÐ¶Ï½«·¢ËÍ¶ÓÁÐÖÐµÄÊý¾Ý·¢ËÍÖÁ´®¿Ú¶Ë¿ÚµÄÓ²¼þFIFOÖÐ),Ö»ÓÐÔÚËùÒªÌî¶¨µÄ·¢ËÍ×Ö·ûÖÐÓöµ½'\0'Ê±²Å»á
	Í£Ö¹Ïò·¢ËÍ¶ÓÁÐÖÐÌîÐ´Êý¾Ý.
	¸Ãº¯ÊýÖ»¿ÉÔÚTSK»òÕßSWIÖÐÊ¹ÓÃ,·ñÔò¿ÉÄÜ»áÏÝÈëËÀµÈ×´Ì¬.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pszStringÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾ÝÊ§°Ü. */
extern HRESULT ComWriteStr(
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	const PCHAR					pszString		//Ö¸ÏòËùÒªÐ´ÈëµÄ×Ö·û´®,¸Ã´®ÒÔ'\0'Îª½áÊø±êÖ¾
);

/* 
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÏòComPort´®¿ÚµÄ·¢ËÍ¶ÓÁÐÖÐÌîÐ´¶¨³¤¶ÈÊý¾Ý,Èç¹ûµ±Ç°¸Ã´®¿Ú¶Ë¿Ú·¢ËÍ¶ÓÁÐÂúÔò»á
	×èÈûµÈ´ý¶¨Ê±ÖÐ¶Ï(¶¨Ê±ÖÐ¶Ï½«·¢ËÍ¶ÓÁÐÖÐµÄÊý¾Ý·¢ËÍÖÁ´®¿Ú¶Ë¿ÚµÄÓ²¼þFIFOÖÐ).
	¸Ãº¯ÊýÖ»¿ÉÔÚTSK»òÕßSWIÖÐÊ¹ÓÃ,·ñÔò¿ÉÄÜ»áÏÝÈëËÀµÈ×´Ì¬.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbDataÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾ÝÊ§°Ü. */
extern HRESULT ComWriteData(
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	const PBYTE8				pbData,			//Ö¸ÏòËùÒªÐ´ÈëµÄÊý¾Ý
	DWORD32						dwBytesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú¸öÊý.
	PDWORD32					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú¸öÊý,ÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´´ÓComPort´®¿ÚµÄ½ÓÊÕ¶ÓÁÐÖÐ¶ÁÈ¡×Ö·û´®,Èç¹ûµ±Ç°¸Ã´®¿Ú¶Ë¿Ú½ÓÊÕ¶ÓÁÐÎª¿ÕÔò»á×èÈûµÈ´ý¶¨Ê±ÖÐ¶Ï
	(¶¨Ê±ÖÐ¶Ï»á¸Ã´®¿Ú¶Ë¿ÚµÄÓ²¼þFIFOÖÐ¶ÁÈ¡Êý¾Ý).¸Ãº¯ÊýÖ»ÓÐÔÚÓöµ½Óë»Ø³µ¼ü²Å»áÕý³£ÖÐ¶Ïµ±Ç°¶ÁÈ¡×Ö·û´®²Ù×÷,
	ÔÚ¶ÁÈ¡×Ö·û´®³¤¶È³¬¹ý´«Èë×î´ó³¤¶ÈÊ±,¸Ãº¯Êý²»»áÏòpszStringÖÐÌîÈëÈÎºÎÊý¾Ý(ºóÃæ½ÓÊÕµÄÈ«²¿¶ªÆú).
	¸Ãº¯ÊýÖ»¿ÉÔÚTSK»òÕßSWIÖÐÊ¹ÓÃ,·ñÔò¿ÉÄÜ»áÏÝÈëËÀµÈ×´Ì¬.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Êý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pszStringÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,ç´®¿Ú¶Ë¿Ú·Ç·?
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡µÄÊý¾Ý³¤¶È³¬¹ýÓÃ»§ËùÉè¶¨µÄ×î´óÊý¾Ý³¤¶È. */
extern HRESULT ComReadStr(
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	PCHAR 						pszString,		//Ö¸Ïò¶Á³öµÄ×Ö·û´®Êý¾Ý.
	DWORD32						dwMaxBytesToRead,//³äÐí¶ÁÈëµÄ×î´ó×Ö·ûÊýÄ¿.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄ×Ö·û³¤¶È,°üÀ¨»Ø³µ¼ü,Èç¹ûÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´´ÓComPort´®¿ÚµÄ½ÓÊÕ¶ÓÁÐÖÐ¶ÁÈ¡×Ö·ûÊý¾Ý,Èç¹ûµ±Ç°¸Ã´®¿Ú¶Ë¿Ú½ÓÊÕ¶ÓÁÐÎª¿ÕÖ±½Ó·µ»Øµ±Ç°Êµ¼Ê¶Áµ½
	µÄÊý¾Ý¸öÊý.¶¨Ê±ÖÐ¶Ï(ÔÚÖÐ¶Ïº¯ÊýÖÐ»á¸Ã´®¿Ú¶Ë¿ÚµÄÓ²¼þFIFOÖÐ¶ÁÈ¡Êý¾Ý).
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Êý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbDataÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡µÄÊý¾Ý¸öÊýÉÙÓÚdwBytesToReadËùÖ¸¶¨µÄ¸öÊý. */
extern HRESULT ComReadData(
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	PBYTE8						pbData,			//Ö¸Ïò¶Á³öµÄÊý¾Ý
	DWORD32						dwBytesToRead,	//ÆÚÍû¶ÁÈëµÄ×Ö·ûÊýÄ¿.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄ×Ö·û³¤¶È,Èç¹ûÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´Çå´®¿ÚµÄÈíÓ²¼þ
*/
extern HRESULT ClearComData(
	SYS_COM_PORT 				ComPort 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´Ïò´®¿ÚÐ´Èë×Ö·û´®,ÊÇÒÔ×Ö·û´®¿ÚÓÐ½áÊø×Ö·û'\0'ÎªÖÐÖ¹±êÖ¾µÄ.
	¸Ãº¯Êý¿ÉÒÔÔÚÈÎºÎµØ·½µ÷ÓÃ,µ«ÊÇ±ØÐëÒªÃ÷È·¸Ã·¢ËÍ³ÌÐòÊÇÒ»Ö±µÈ´ýÓ²¼þ·¢ËÍ½áÊøºó²Å·µ»ØµÄ.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pszStringÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾ÝÊ§°Ü. */
extern HRESULT COM_WriteStr( 
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	const PCHAR					pszString		//Ö¸ÏòËùÒªÐ´ÈëµÄ×Ö·û´®Êý¾Ý.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´Ïò´®¿ÚÐ´Èë¶¨³¤¶ÈÊý¾Ý.
	¸Ãº¯Êý¿ÉÒÔÔÚÈÎºÎµØ·½µ÷ÓÃ,µ«ÊÇ±ØÐëÒªÃ÷È·¸Ã·¢ËÍ³ÌÐòÊÇÒ»Ö±µÈ´ýÓ²¼þ·¢ËÍ½áÊøºó²Å·µ»ØµÄ.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pszStringÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾Ïò´®¿Ú¶Ë¿ÚÐ´ÈëÊý¾ÝÊ§°Ü. */
extern HRESULT COM_WriteData(
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	const PBYTE8				pbData,			//Ö¸ÏòËùÒªÐ´ÈëµÄÊý¾Ý
	DWORD32						dwBytesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,Èç¹ûÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´´Ó´®¿Ú¶ÁÈ¡×Ö·û´®,¸Ãº¯ÊýÖ»ÓÐÔÚÓöµ½»Ø³µ¼üÊ±²Å»áÕý³£·µ»Ø.ÔÚ¶ÁÈ¡×Ö·û´®³¤¶È³¬¹ý´«Èë×î´ó³¤¶ÈÊ±,
	¸Ãº¯Êý²»»áÏòpszStringÖÐÌîÈëÈÎºÎÊý¾Ý(ºóÃæ½ÓÊÕµÄÈ«²¿¶ªÆú).
	¸Ãº¯Êý¿ÉÒÔÔÚÈÎºÎµØ·½µ÷ÓÃ,µ«ÊÇÊÇÂÖÑ¯´®¿Ú¶Ë¿ÚÓ²¼þFIFOÊÇ·ñ½ÓÊÕµ½×Ö·ûµÄ,ËùÒÔÆäËûÊÂÎñ¶¼±»µ±Ç°²Ù×÷ÇÀÕ¼.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Êý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pszStringÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Êý¾ÝÊ§°Ü. */
extern HRESULT COM_ReadStr( 
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	PCHAR 						pszString,		//Ö¸Ïò¶Á³öµÄ×Ö·û´®Êý¾Ý.
	DWORD32						dwMaxBytesToRead,//³äÐí¶ÁÈëµÄ×î´ó×Ö·ûÊýÄ¿.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄ×Ö·û³¤¶È,°üÀ¨»Ø³µ¼ü,Èç¹ûÎªNULLÔò²»Ìî.
);

/*
º¯ÊýËµÃ÷:ÊýÓÃÀ´´Ó´®¿Ú¶ÁÈ¡Êý¾Ý,ÒÔ¶Áµ½ÆÚÍûµÃµ½µÄÊý¾Ý³¤¶ÈÎªÖÐÖ¹Ìõ¼þ.
	¸Ãº¯Êý¿ÉÒÔÔÚÈÎºÎµØ·½µ÷ÓÃ,µ«ÊÇÊÇÂÖÑ¯®¿¶ËÚÓ²¼þFIFOÊÇ·ñ½ÓÊÕµ½×Ö·ûµÄ,ËùÒÔÆäËûÊÂÎñ¶¼±»µ±Ç°²Ù×÷ÇÀÕ¼.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Êý¾Ý³É¹¦.
	·µ»ØE_POINTER±íÊ¾pbData»òÕßpdwLenÎªINVALID_POINTER.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,Èç´®¿Ú¶Ë¿Ú·Ç·¨.
	·µ»ØE_FAIL±íÊ¾Ó²¼þµ×²ã²Ù×÷³ö´í.
	·µ»ØS_FALSE±íÊ¾´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Êý¾ÝÊ§°Ü. */
extern HRESULT COM_ReadData(
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	PBYTE8						pbData,			//Ö¸Ïò¶Á³öµÄÊý¾Ý
	DWORD32						dwBytesToRead,	//ÆÚÍû¶ÁÈëµÄ×Ö·ûÊýÄ¿.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄ×Ö·û³¤¶È,Èç¹ûÎªNULLÔò²»Ìî.
);	

/*
º¯ÊýËµÃ÷:¸Ãº¯Êý´Ó´®¿Ú¶Ë¿Ú¶ÁÈ¡Ò»×Ö·û,º¯ÊýÖ±½Ó·µ»Ø¸Ã×Ö·û.
	¸Ãº¯Êý¿ÉÒÔÔÚÈÎºÎµØ·½µ÷ÓÃ,µ«ÊÇÊÇÂÖÑ¯´®¿Ú¶Ë¿ÚÓ²¼þFIFOÊÇ·ñ½ÓÊÕµ½×Ö·ûµÄ,ËùÒÔÆäËûÊÂÎñ¶¼±»µ±Ç°²Ù×÷ÇÀÕ¼. */
extern CHAR COM_GetChar( 
	SYS_COM_PORT 				ComPort 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÏò´®¿Ú¶Ë¿ÚÊä³öÒ»¸ö×Ö·û,º¯ÊýÎÞ·µ»ØÖµ.
	¸Ãº¯Êý¿ÉÒÔÔÚÈÎºÎµØ·½µ÷ÓÃ,µ«ÊÇ±ØÐëÒªÃ÷È·¸Ã·¢ËÍ³ÌÐòÊÇÒ»Ö±µÈ´ýÓ²¼þ·¢ËÍ½áÊøºó²Å·µ»ØµÄ. */
extern void COM_PutChar( 
	SYS_COM_PORT 				ComPort, 		//±íÊ¾µ±Ç°²Ù×÷µÄ´®¿Ú¶Ë¿ÚºÅ.
	CHAR						chVal			//µ±Ç°Òª·¢ËÍµÄÊý¾Ý.
);
/*------------------------------------------------------------------------------------------------------------------------------------*/



/* 
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´ÅÐ¶Ïµ±Ç°µÄÊÓÆµ¶Ë¿ÚÊÇ·ñ±»Ê¹ÓÃ.
º¯Êý·µ»ØÖµ:
	·µ»ØS_OK±íÊ¾È¡»º³åÇø¸öÊý³É¹¦.
	·µ»ØE_INVALIDARG±íÊ¾´«ÈëµÄ²ÎÊýÓÐ´í,¸Ã¶Ë¿ÚÏµÍ³µ±Ç°²»Ö§³Ö.
	·µ»ØE_POINTER±íÊ¾pfPresentÎªINVALID_POINTER.
	·µ»ØS_FALSE±íÊ¾µ±Ç°µÄÊÓÆµ¶Ë¿ÚÎ´Ê¹ÓÃ. */
extern HRESULT VIDEO_IsPortPresent(
	int 				iPort, 
	PBOOL 				pfPresent
);


/*------------------------------------------------------------------------------------------------------------------------------------*/
// ATA IDE Ó²ÅÌ½Ó¿Ú
extern HRESULT ATA_ReadSector(
	DWORD32					dwDeviceID,	
	DWORD32					dwSectorStartLBA,
	PBYTE8					pbMemDst,
	DWORD32					dwSectorsToRead,
	PDWORD32				pdwReadSectors
);

extern HRESULT ATA_WriteSector(
	DWORD32					dwDeviceID,	
	PBYTE8					pbMemSrc,
	DWORD32					dwSectorStartLBA,
	DWORD32					dwSectorsToWrite,
	PDWORD32				pdwWriteSectors
);

extern HRESULT ATA_HdCheck( void );
extern HRESULT ATA_HdReadId(
	PCHAR				pszString,
	DWORD32				dwBytesToRead,
	PDWORD32			pdwReadBytes
);
extern HRESULT ATA_Reset( void );
/*------------------------------------------------------------------------------------------------------------------------------------*/

// SATA ½Ó¿Ú.
/*------------------------------------------------------------------------------------------------------------------------------------*/
extern HRESULT SATA_ReadSector(
	DWORD32					dwDeviceID,	
	DWORD32					dwSectorStartLBA,
	PBYTE8					pbMemDst,
	DWORD32					dwSectorsToRead,
	PDWORD32				pdwReadSectors
);

extern HRESULT SATA_ReadSectors(
	DWORD32					dwDeviceID,	
	DWORD32					dwSectorStartLBA,
	PBYTE8					pbMemDst,
	DWORD32					dwSectorsToRead,
	PDWORD32				pdwReadSectors
);
extern HRESULT SATA_WriteSector(
	DWORD32					dwDeviceID,	
	PBYTE8					pbMemSrc,
	DWORD32					dwSectorStartLBA,
	DWORD32					dwSectorsToWrite,
	PDWORD32				pdwWriteSectors
);

extern HRESULT SATA_WriteSectors(
	DWORD32			dwDeviceID,	
	PBYTE8			pbMemSrc,
	DWORD32			dwSectorStartLBA,
	DWORD32			dwSectorsToWrite,
	PDWORD32		pdwWriteSectors
);

extern HRESULT SATA_HdCheck( PBYTE8 pVar );
extern HRESULT SATA_HdReadId(
	PCHAR				pszString,
	DWORD32				dwBytesToRead,
	PDWORD32			pdwReadBytes
);
extern HRESULT SATA_Reset( void );
extern int SATA_GetStatus();
extern WORD16 SATA_GetDiskSize();
extern HRESULT SATA_CheckError( PDWORD32 pdwError );
/*------------------------------------------------------------------------------------------------------------------------------------*/

extern HRESULT NAND_ReadFlashId(
	PBYTE8					pbID,
	DWORD32					dwBytesToRead,
	PDWORD32				pdwReadBytes
);


//¶Áº¯Êý
extern HRESULT NANDFLASH_Read(
	DWORD32 					dwSrcPos,		//´ÓFLASH¶ÁÈ¡Êý¾ÝµÄÆðÊ¼Î»ÖÃ.
	PBYTE8						pbMemDst, 		//Ö¸ÏòÄÚ´æÖÐ·ÅÖÃ¶Áµ½µÄÊý¾Ý.
	BYTE8						bReadType,		//FLASH¶ÁÈ¡Ä£Ê½,Îª0Ê±±íÊ¾ÎªÍ¸Ã÷¶Á,Îª1Îª¼ÓÃÜ¶Á
	DWORD32						dwBytesToRead,	//ÆÚÍû¶ÁÈ¡µÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32					pdwBytesRead	//Êµ¼Ê¶Áµ½µÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»?
	
);

//Ò³Ð´º¯Êý¡£Ö´ÐÐ´Ë¯ÊýÖ®Ç°±ØÐë±£Ö¤Ò³ÄÚËùÓÐ×Ö½Ú±ØÐëÈ«Îª0xFF£¬·ñÔò±ØÐëÏÈ²Á³ý
extern HRESULT NANDFLASH_Write(
	const PBYTE8 				pbMemSrc,		//Ö¸ÏòËùÒª¶ÁÈ¡µÄÄÚ´æÊý¾Ý.
	DWORD32 					dwDstPos,		//FLASHÐ´ÈëÊý¾ÝÄ¿±êÆðÊ¼Î»ÖÃ.
	BYTE8 						bWriteType,		//FLASHÐ´Ä£Ê½,0ÎªÍ¸Ã÷Ð´,1Îª¼ÓÃÜÐ´
	DWORD32						dwBytesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32 					pdwBytesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

//Ò³Ð´º¯Êý¡£Ö´ÐÐ´Ëº¯ÊýÖ®Ç°±ØÐë±£Ö¤Ò³ÄÚËùÓÐ×Ö½Ú±ØÐëÈ«Îª0xFF£¬·ñÔò±ØÐëÏÈ²Á³ý
extern HRESULT NANDFLASH_WritePage(
	const PBYTE8 				pbMemSrc,		//Ö¸ÏòËùÒª¶ÁÈ¡µÄÄÚ´æÊý¾Ý.
	DWORD32 					dwDstPage,		//FLASHÐ´ÈëÊý¾ÝÄ¿±êÆðÊ¼Î»ÖÃ.
	BYTE8 						bWriteType,		//FLASHÐ´Ä£Ê½,0ÎªÍ¸Ã÷Ð´,1Îª¼ÓÃÜÐ´
	DWORD32						dwPagesToWrite,	//ÆÚÍûÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È.
	PDWORD32 					pdwPagesWrite	//Êµ¼ÊÐ´ÈëµÄÊý¾Ý×Ö½Ú³¤¶È,ÎªNULLÔò²»Ìî.
);

//¿é²Á³ýº¯Êý
extern HRESULT NANDFLASH_Erase( 
	DWORD32						dwStartPos, 	//ÐèÒª²Á³ýFLASH¿éÊ×Î»ÖÃ.
	DWORD32 					dwLength		//µ±Ç°ÐèÒª²Á³ý³¤¶È,ÒÔ×Ö½ÚÎªµ¥Î».
);

//Blank ¼ì²éº¯Êý
extern HRESULT NANDFLASH_Verify(
	DWORD32						dwStartPos, 	//ÐèÒª¼ì²éµÄFLASH¿éÊ×Î»ÖÃ.
	DWORD32 					dwLength		//µ±Ç°ÐèÒª¼ì²éµÄ³¤¶È,ÒÔ×Ö½ÚÎªµ¥Î».
);

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´½«½«CPUµÄÒ»Ð©BUG×ÜÌåÐÔÐÞ¸Ä,¹Ê¶ø¸Ãº¯ÊýÐèÒªÔÚÖ÷Ïß³Ì³õÊ¼»¯Ê±µ÷ÓÃ.
*/
extern HRESULT InitCPURegisterOk( void );

/*
º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´¼ì²âµ±Ç°ÊÇ·ñÊÇVVDµÄ½Ó¿Ú°å.
*/
extern HRESULT IsVVDInterfaceBoard( void );

/* º¯ÊýËµÃ÷:¸Ãº¯ÊýÓÃÀ´¸ù¾ÝbSetBitÎ»ÖµÉèÖÃDPIO 3¸ö´¥·¢Êä³öµÄ×´Ì¬.º¯ÊýÎÞ·µ»ØÖµ. */
extern void SetTriggerOutDPIO(
	BYTE8 	bPos,				// Î»ÖÃ
	BYTE8 	bSetBit			// ÉèÖÃÖµ,Ö»ÓÐ×îºóÒ»Î»ÓÐÐ§(0»ò1)
);

#ifdef __cplusplus
	}
#endif /* #ifdef __cplusplus */



#endif /* #ifndef _HVTARGET_H */


