/* ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in
 *   the license agreement under which this software has been supplied.
 *  ===========================================================================
 */

/** ===========================================================================
 *   @file  csl_vcp2.h
 *
 *   @path  $(CSLPATH)\inc
 *
 *   @desc  API header for VCP2
 *
 */

/** @mainpage VCP2 CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the VCP2 module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given VCP2 module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular VCP2 Module. The CSL developer should use his discretion
 * designing new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# VCP2 FUNCTIONAL SPEC 2.00.W.12
 *
 */


/* ============================================================================
 *  Revision History
 *  ================
 *  24-March-2005   SPrasad     File Created.
 *  27-May-2005     SPrasad     Updated with new requirements' specification.
 *  03-Aug-2005     Chandra     Minor changes to beautify the file.
 *  08-dec-2005      sd         changed the maxSm and minSm to be signed 
 *                              integers
 *  07-mar-2006      ds         Rename of Out order tokens according to register
 *                              header file. 
 *                              - CSL_VCP2_VCPIC3_OUT_ORDER_0_31 to
 *                                CSL_VCP2_VCPIC3_OUT_ORDER_LSB and 
 *                              - CSL_VCP2_VCPIC3_OUT_ORDER_31_0 to 
 *                                CSL_VCP2_VCPIC3_OUT_ORDER_MSB
 *                               
 * ============================================================================
 */

#ifndef _CSL_VCP2_H_
#define _CSL_VCP2_H_

#include <csl.h>
#include <cslr_vcp2.h>
#include <soc.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 *  VCP Macros
 *****************************************************************************/
/** Code rate = 2 */
#define VCP2_RATE_1_2                2
/** Code rate = 3 */
#define VCP2_RATE_1_3                3
/** Code rate = 4 */
#define VCP2_RATE_1_4                4

/** Speed critical */
#define VCP2_SPEED_CRITICAL          0
/** Speed most critical */
#define VCP2_SPEED_MOST_CRITICAL     1
/** Performance critical */
#define VCP2_PERF_CRITICAL           2
/** Performance most critical */
#define VCP2_PERF_MOST_CRITICAL      3
/** Default value */
#define VCP2_PERF_DEFAULT            VCP2_SPEED_CRITICAL

/** Out order of VCP output for decoded data : 0 to 31 */
#define VCP2_OUTORDER_0_31           CSL_VCP2_VCPIC3_OUT_ORDER_LSB
/** Out order of VCP output for decoded data : 31 to 0 */
#define VCP2_OUTORDER_31_0           CSL_VCP2_VCPIC3_OUT_ORDER_MSB

/** Output decision type : Hard decisions */
#define VCP2_DECISION_HARD           CSL_VCP2_VCPIC5_SDHD_HARD
/** Output decision type : Soft decisions */
#define VCP2_DECISION_SOFT           CSL_VCP2_VCPIC5_SDHD_SOFT

/** Output parameters read flag : VCP read event is not generated */
#define VCP2_OUTF_NO                 CSL_VCP2_VCPIC5_OUTF_NO
/** Output parameters read flag : VCP read event is generated */
#define VCP2_OUTF_YES                CSL_VCP2_VCPIC5_OUTF_YES

/** No trace back allowed */
#define VCP2_TRACEBACK_NONE          CSL_VCP2_VCPIC5_TB_NO
/** Traceback mode : Tailed */
#define VCP2_TRACEBACK_TAILED        CSL_VCP2_VCPIC5_TB_TAIL
/** Traceback mode : Convergent */
#define VCP2_TRACEBACK_CONVERGENT    CSL_VCP2_VCPIC5_TB_CONV
/** Traceback mode : Mixed */
#define VCP2_TRACEBACK_MIXED         CSL_VCP2_VCPIC5_TB_MIX

/**
 * VCP unpause type : VCP restarts and processes one sliding window before 
 * pausing again
 */
#define VCP2_UNPAUSE_ONESW           CSL_VCP2_VCPEXE_COMMAND_RESTART_PAUSE
/** VCP unpause type : VCP restarts */
#define VCP2_UNPAUSE_NORMAL          CSL_VCP2_VCPEXE_COMMAND_RESTART

/** Soft decisions memory format : 32-bit word packed */
#define VCP2_END_PACKED32            CSL_VCP2_VCPEND_SD_32BIT
/** Soft decisions memory format : Native (8 bits) */
#define VCP2_END_NATIVE              CSL_VCP2_VCPEND_SD_NATIVE

/** EMU mode : VCP halts at the end of completion of the current window of
 *  state metric processing or at the end of a frame */
#define VCP2_EMUHALT_DEFAULT         CSL_VCP2_VCPEMU_SOFT_HALT_DEFAULT
/**
 * EMU mode : VCP halts at the end of completion of the processing of the 
 * frame
 */
#define VCP2_EMUHALT_FRAMEEND        CSL_VCP2_VCPEMU_SOFT_HALT_FRAMEEND

/***************************************************************************
 *  Following are Polynomials used in GSM/Edge/GPRS
 **************************************************************************/

/**
 *  GSM/Edge/GPRS generator polynomial 0
 */
#define VCP2_GEN_POLY_0    0x30

/**
 *  GSM/Edge/GPRS generator polynomial 1
 */
#define VCP2_GEN_POLY_1    0xB0

/**
 *  GSM/Edge/GPRS generator polynomial 2
 */
#define VCP2_GEN_POLY_2    0x50

/**
 *  GSM/Edge/GPRS generator polynomial 3
 */
#define VCP2_GEN_POLY_3    0xF0

/**
 *  GSM/Edge/GPRS generator polynomial 4
 */
#define VCP2_GEN_POLY_4    0x6C

/**
 *  GSM/Edge/GPRS generator polynomial 5
 */
#define VCP2_GEN_POLY_5    0x94

/**
 *  GSM/Edge/GPRS generator polynomial 6
 */
#define VCP2_GEN_POLY_6    0xF4

/**
 *  GSM/Edge/GPRS generator polynomial 7
 */
#define VCP2_GEN_POLY_7    0xE4

/**
 *  NULL generator polynomial for GSM/Edge/GPRS
 */
#define VCP2_GEN_POLY_GNULL 0x00




/******************************************************************************
 *  VCP global typedef declarations 
 *****************************************************************************/

/** 
 *  Handle to access VCP2 registers accessible through config bus. 
 */
#define hVcp2     ((CSL_Vcp2ConfigRegs*)CSL_VCP2_0_REGS)

/** 
 *  Handle to access VCP2 registers accessible through EDMA bus. 
 */
#define hVcp2Vbus ((CSL_Vcp2EdmaRegs *)CSL_VCP2_EDMA_REGS)


/** 
 *  VCP code rate type 
 */
typedef Uint32 VCP2_Rate;

/** 
 *  VCP input configuration structure that holds all of the configuration
 *  values that are to be transferred to the VCP via the EDMA. 
 */
typedef struct
{
    /** Value of VCP input configuration register 0 */
    Uint32 ic0;
    /** Value of VCP input configuration register 1 */
    Uint32 ic1;
    /** Value of VCP input configuration register 2 */
    Uint32 ic2;
    /** Value of VCP input configuration register 3 */
    Uint32 ic3;
    /** Value of VCP input configuration register 4 */
    Uint32 ic4;
    /** Value of VCP input configuration register 5 */
    Uint32 ic5;
} VCP2_ConfigIc;

/**
 *  VCP channel parameters structure that holds all of the information 
 *  concerning the user channel. These values are used to generate the 
 *  appropriate input configuration values for the VCP and to program 
 *  the EDMA. 
 */
typedef struct
{
    /** Code rate */
    VCP2_Rate rate;
    /** Constraint length */
    Uint8 constLen;
    /** Polynomial 0 */
    Uint8 poly0;
    /** Polynomial 1 */
    Uint8 poly1;
    /** Polynomial 2 */
    Uint8 poly2;
    /** Polynomial 3 */
    Uint8 poly3;
    /** Yamamoto threshold value*/
    Uint16 yamTh;
    /** Frame length i.e. number of symbols in a frame*/
    Uint16 frameLen;
    /** Reliability length */
    Uint16 relLen;
    /** Convergence distance */
    Uint16 convDist;
    /** Traceback state index */
    Uint16 traceBackIndex;
    /** Traceback state index enable/disable */
    Bool traceBackEn;
    /** Hard decision output ordering */
    Uint16 outOrder;
    /** Maximum initial state metric */
    Int16 maxSm;
    /** Minimum initial state metric */
    Int16 minSm;
    /** State index set to the maximum initial state metric*/
    Uint8 stateNum;
    /** Branch metrics buffer length in input FIFO */
    Uint8 bmBuffLen;
    /** Decisions buffer length in output FIFO */
    Uint8 decBuffLen;
    /** Traceback mode */
    Uint8 traceBack;
    /** Output parameters read flag */
    Uint8 readFlag;
    /** Decision selection: hard or soft */
    Uint8 decision;
    /** Number of branch metric frames*/
    Uint16 numBmFrames;
    /** Number of decision frames */
    Uint16 numDecFrames;
} VCP2_Params;

/**
 *  VCP base parameter structure that is used to configure the VCP parameters
 *  structure with the given values using VCP2_genParams() function.
 */
typedef struct
{
    /** Code rate */
    VCP2_Rate rate;
    /** Constraint length */
    Uint8 constLen;
    /** Frame length */
    Uint16 frameLen;
    /** Yamamoto threshold value */
    Uint16 yamTh;
    /** Maximum initial state metric value */
    Uint8 stateNum;
    /** Traceback convergement mode */
    Bool tbConvrgMode;
    /** Output decision type */
    Uint8 decision;
    /** Output parameters read flag */
    Uint8 readFlag;
    /** Enable/Disable tail biting */
    Bool tailBitEnable;
    /** Tailbiting traceback index mode */
    Uint16 traceBackIndex;
    /** Hard decision output ordering */
    Uint8 outOrder;
    /** Performance and speed */
    Uint8 perf;
} VCP2_BaseParams;

/** VCP Error structure */
typedef struct
{
    /** Traceback mode error */
    Bool tbnaErr;
    /** Frame length error */
    Bool ftlErr;
    /** Reliability + convergence distance error */
    Bool fctlErr;
    /** Max-Min error */
    Bool maxminErr;
    /** SYMX error */
    Bool symxErr;
    /** SYMR error */
    Bool symrErr;
} VCP2_Errors;

/** VCP generator polynomials structure */
typedef struct
{
    /** Generator polynomial 0 */
    Uint8 poly0;
    /** Generator polynomial 1 */
    Uint8 poly1;
    /** Generator polynomial 2 */
    Uint8 poly2;
    /** Generator polynomial 3 */
    Uint8 poly3;
} VCP2_Poly;

/******************************************************************************
 * VCP2 global function declarations
 *****************************************************************************/

/*
 * ============================================================================
 *  @func VCP2_genParams
 *
 *  @desc
 *      This function calculates the VCP parameters based on the input 
 *      VCP2_BaseParams object values and set the values to the output
 *      VCP2_Params parameters structure.
 *
 *  @arg    pConfigBase   
 *          Pointer to VCP base parameters structure.
 *
 *  @arg    pConfigParams
 *          Pointer to output VCP channel parameters structure.
 *
 *  @ret    None
 *
 *  @eg
 *          VCP2_Params      vcpParam;
 *          VCP2_BaseParams  vcpBaseParam;
 *          ...
 *          vcpBaseParam.rate           =   VCP2_RATE_1_4;
 *          vcpBaseParam.constLen       =   5;
 *          vcpBaseParam.frameLen       =   2042;
 *          vcpBaseParam.yamTh          =   50;
 *          vcpBaseParam.stateNum       =   2048;
 *          vcpBaseParam.tbConvrgMode   =   FALSE;
 *          vcpBaseParam.decision       =   VCP2_DECISION_HARD;
 *          vcpBaseParam.readFlag       =   VCP2_OUTF_YES;
 *          vcpBaseParam.tailBitEnable  =   FALSE;
 *          vcpBaseParam.traceBackIndex =   0x0;
 *          vcpBaseParam.outOrder       =   VCP2_OUTORDER_0_31;
 *          vcpBaseParam.perf           =   VCP2_SPEED_CRITICAL;
 *          ... 
 *          VCP2_genParams (&vcpBaseParam, &vcpParam);
 *
 * ============================================================================
 */
extern void VCP2_genParams (
    VCP2_BaseParams * pConfigBase,
    VCP2_Params     * pConfigParams
);

/*
 * ============================================================================
 *  @func VCP2_genIc
 *
 *  @desc
 *      This function generates the required input configuration registers' 
 *      values needed to program the VCP based on the parameters provided by
 *      VCP2_Params object values.
 *
 *  @arg    pConfigParams   
 *          Pointer to channel parameters structure.
 *
 *  @arg    pConfigIc
 *          Pointer to input configuration structure.
 *
 *  @ret    None
 *
 *  @eg
 *          VCP2_Params      vcpParam;
 *          VCP2_ConfigIc    vcpConfig;
 *          VCP2_BaseParams  vcpBaseParam;
 *          ...
 *          vcpBaseParam.rate           =   VCP2_RATE_1_4;
 *          vcpBaseParam.constLen       =   5;
 *          vcpBaseParam.frameLen       =   2042;
 *          vcpBaseParam.yamTh          =   50;
 *          vcpBaseParam.stateNum       =   2048;
 *          vcpBaseParam.tbConvrgMode   =   FALSE;
 *          vcpBaseParam.decision       =   VCP2_DECISION_HARD;
 *          vcpBaseParam.readFlag       =   VCP2_OUTF_YES;
 *          vcpBaseParam.tailBitEnable  =   FALSE;
 *          vcpBaseParam.traceBackIndex =   0x0;
 *          vcpBaseParam.outOrder       =   VCP2_OUTORDER_0_31;
 *          vcpBaseParam.perf           =   VCP2_SPEED_CRITICAL;
 *          ... 
 *          VCP2_genParams (&vcpBaseParam, &vcpParam);
 *
 *          VCP2_genIc (&vcpParam, &vcpConfig);
 *
 * ============================================================================
 */
extern void VCP2_genIc (
    VCP2_Params     * pConfigParams,
    VCP2_ConfigIc   * pConfigIc
);

#ifdef __cplusplus
}
#endif

#endif
