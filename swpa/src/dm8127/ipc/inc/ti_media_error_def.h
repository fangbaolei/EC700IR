/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef __TI_MEDIA_ERROR_DEF_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __TI_MEDIA_ERROR_DEF_H__


/* =============================================================================
 * Defines
 * =============================================================================
 */
#define ERROR_NONE                     ((Int32)0)
#define ERROR_FAIL                     ((Int32)-1)

#define VCAP_ERROR_NONE                ERROR_NONE
#define VCAP_VIP_BAD_ID                MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_INFO,    ERROR_WRONG_DEVID );
#define VCAP_CHN_BAD_ID                MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_INFO,    ERROR_WRONG_CHNID );
#define VCAP_SYS_BUSY                  MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_INFO,    ERROR_SYS_BUSY);
#define VCAP_VIP_WRONG_SETUP           MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_CRASH,   ERROR_VIP_WRONG_SETUP);
#define VCAP_CHN_WRONG_SETUP           MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_WARNING, ERROR_CHN_WRONG_SETUP);
#define VCAP_VIP_NOT_ENABLED           MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_HANG,    ERROR_VIP_NOT_ENABLED);
#define VCAP_CHN_NOT_ENABLED           MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_HANG,    ERROR_CHN_NOT_ENABLED);
#define VCAP_CHN_MEM_EMPTY             MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_HANG,    ERROR_CHN_MEM_EMPTY);
#define VCAP_VIDEO_STANDARD_MISMATCHED MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_WARNING, ERROR_VIDDEO_STANDARD_MISMATCH);
#define VCAP_LOCK_PREVIEW_ERROR        MAP_ERROR_MESSAGE(TI_VCAP, ERROR_CATEGORY_CRASH,   ERROR_CHN_LOCK_PREIVEW);

/* =============================================================================
 * Enums
 * =============================================================================
 */
typedef enum
{
    ERROR_VIP_WRONG_SETUP = 64,
    ERROR_CHN_WRONG_SETUP,
    ERROR_VIP_NOT_ENABLED,
    ERROR_CHN_NOT_ENABLED,   //VIP or capChn is not enabled.
    ERROR_CHN_MEM_EMPTY,
    ERROR_VIDDEO_STANDARD_MISMATCH,
    ERROR_CHN_LOCK_PREIVEW

}VCAP_ERROR_MESSAGE;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


