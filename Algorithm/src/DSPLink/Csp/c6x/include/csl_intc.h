/*****************************************************\
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/

/** @mainpage Interrupt Controller
*
* @section Introduction
*
* @subsection xxx Purpose and Scope
* The purpose of this document is to detail the  CSL APIs for the
* INTC Module.
*
* @subsection aaa Terms and Abbreviations
*   -# CSL:  Chip Support Library
*   -# API:  Application Programmer Interface
*   -# INTC: Interrupt Controller
*
* @subsection References
*    -# CSL 3.x Technical Requirements Specifications Version 0.5, dated
*       May 14th, 2003
*    -# Inerrupt Controller Specification
*
* @subsection Assumptions
*     The abbreviations INTC, Intc and intc have been used throughout this
*     document to refer to Interrupt Controller
*/

/** @file csl_intc.h
 *
 *    @brief    Header file for functional layer CSL of INTC
 *
 *  Description
 *    - The different enumerations, structure definitions
 *      and function declarations
 */
#ifndef _CSL_INTC_H_
#define _CSL_INTC_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <csl.h>
#include <davinci_hd.h>
#include <davinci_hd64plus.h>

/**
@defgroup CSL_INTC_API INTC
*/
/**
@defgroup CSL_INTC_DATASTRUCT  INTC Data Structures
@ingroup CSL_INTC_API
*/
/**
@defgroup CSL_INTC_SYMBOL  INTC Symbols Defined
@ingroup CSL_INTC_API
*/
/**
@defgroup CSL_INTC_ENUM  INTC Enumerated Data Types
@ingroup CSL_INTC_API
*/

/**
@defgroup CSL_INTC_FUNCTION  INTC Functions
@ingroup CSL_INTC_API
*/
/**
@defgroup CSL_INTC_FUNCTION_INTERNAL  INTC Internal Functions
@ingroup CSL_INTC_FUNCTION
*/

/** @addtogroup CSL_INTC_SYMBOL
* @{ */

#define CSL_INTC_EVENTID_CNT        128			/**< Number of Events in the System */

#define CSL_INTC_EVTHANDLER_NONE        ((CSL_IntcEventHandler) 0) /**<Indicates there is no associated event-handler */

#define CSL_INTC_BADHANDLE        (0) /**< Invalid handle */

#define CSL_INTC_MAPPED_NONE 	    (-1)
/**
@}
*/
/** @addtogroup CSL_INTC_ENUM
* @{ */
/**
 * @brief   Interrupt Vector IDs
 */
typedef enum {
    CSL_INTC_VECTID_NMI                 =   1, /**< Should be used only along with CSL_intcHookIsr() */
    CSL_INTC_VECTID_4                   =   4, /**< CPU Vector 4 */
    CSL_INTC_VECTID_5                   =   5, /**< CPU Vector 5 */
    CSL_INTC_VECTID_6                   =   6, /**< CPU Vector 6 */
    CSL_INTC_VECTID_7                   =   7, /**< CPU Vector 7 */
    CSL_INTC_VECTID_8                   =   8, /**< CPU Vector 8 */
    CSL_INTC_VECTID_9                   =   9, /**< CPU Vector 9 */
    CSL_INTC_VECTID_10                  =   10, /**< CPU Vector 10 */
    CSL_INTC_VECTID_11                  =   11,	/**< CPU Vector 11 */
    CSL_INTC_VECTID_12                  =   12, /**< CPU Vector 12 */
    CSL_INTC_VECTID_13                  =   13, /**< CPU Vector 13 */
	CSL_INTC_VECTID_14                  =   14, /**< CPU Vector 14 */
	CSL_INTC_VECTID_15                  =   15, /**< CPU Vector 15 */
	CSL_INTC_VECTID_COMBINE             =   16, /**< Should be used at the time of opening an Event handle
	                                               to specify that the event needs to go to the combiner */
	CSL_INTC_VECTID_EXCEP               =   17 /**< Should be used at the time of opening an Event handle
	                                               to specify that the event needs to go to the combiner */
} CSL_IntcVectId;


/**
 * @brief   Interrupt Event IDs
 */
typedef enum {
    CSL_INTC_EVENTID_0                  =   0,
    CSL_INTC_EVENTID_1           	    =   1,
    CSL_INTC_EVENTID_2                  =   2,
    CSL_INTC_EVENTID_3                  =   3,
    CSL_INTC_EVENTID_4                  =   4,
    CSL_INTC_EVENTID_5                  =   5,
    CSL_INTC_EVENTID_6                  =   6,
    CSL_INTC_EVENTID_7                  =   7,
    CSL_INTC_EVENTID_8                  =   8,
    CSL_INTC_EVENTID_9                  =   9,
    CSL_INTC_EVENTID_10                 =   10,
    CSL_INTC_EVENTID_11                 =   11,
	CSL_INTC_EVENTID_12                 =   12,
    CSL_INTC_EVENTID_13           	    =   13,
    CSL_INTC_EVENTID_14                 =   14,
    CSL_INTC_EVENTID_15                 =   15,
    CSL_INTC_EVENTID_16                 =   16,
    CSL_INTC_EVENTID_17                 =   17,
    CSL_INTC_EVENTID_18                 =   18,
    CSL_INTC_EVENTID_19                 =   19,
    CSL_INTC_EVENTID_20                 =   20,
    CSL_INTC_EVENTID_21                 =   21,
    CSL_INTC_EVENTID_22                 =   22,
    CSL_INTC_EVENTID_23                 =   23,
    CSL_INTC_EVENTID_24                 =   24,
    CSL_INTC_EVENTID_25           	    =   25,
    CSL_INTC_EVENTID_26                 =   26,
    CSL_INTC_EVENTID_27                 =   27,
    CSL_INTC_EVENTID_28                 =   28,
    CSL_INTC_EVENTID_29                 =   29,
    CSL_INTC_EVENTID_30                 =   30,
    CSL_INTC_EVENTID_31                 =   31,
    CSL_INTC_EVENTID_32                 =   32,
    CSL_INTC_EVENTID_33                 =   33,
    CSL_INTC_EVENTID_34                 =   34,
    CSL_INTC_EVENTID_35                 =   35,
    CSL_INTC_EVENTID_36                 =   36,
    CSL_INTC_EVENTID_37                 =   37,
    CSL_INTC_EVENTID_38                 =   38,
    CSL_INTC_EVENTID_39                 =   39,
    CSL_INTC_EVENTID_40                 =   40,
    CSL_INTC_EVENTID_41                 =   41,
    CSL_INTC_EVENTID_42                 =   42,
    CSL_INTC_EVENTID_43                 =   43,
    CSL_INTC_EVENTID_44                 =   44,
    CSL_INTC_EVENTID_45                 =   45,
    CSL_INTC_EVENTID_46                 =   46,
    CSL_INTC_EVENTID_47                 =   47,
    CSL_INTC_EVENTID_48                 =   48,
    CSL_INTC_EVENTID_49                 =   49,
    CSL_INTC_EVENTID_50                 =   50,
    CSL_INTC_EVENTID_51                 =   51,
    CSL_INTC_EVENTID_52                 =   52,
    CSL_INTC_EVENTID_53                 =   53,
    CSL_INTC_EVENTID_54                 =   54,
    CSL_INTC_EVENTID_55                 =   55,
    CSL_INTC_EVENTID_56                 =   56,
    CSL_INTC_EVENTID_57                 =   57,
    CSL_INTC_EVENTID_58                 =   58,
    CSL_INTC_EVENTID_59                 =   59,
    CSL_INTC_EVENTID_60                 =   60,
    CSL_INTC_EVENTID_61                 =   61,
    CSL_INTC_EVENTID_62                 =   62,
    CSL_INTC_EVENTID_63                 =   63,
    CSL_INTC_EVENTID_64                 =   64,
    CSL_INTC_EVENTID_65                 =   65,
    CSL_INTC_EVENTID_66                 =   66,
    CSL_INTC_EVENTID_67                 =   67,
    CSL_INTC_EVENTID_68                 =   68,
    CSL_INTC_EVENTID_69                 =   69,
    CSL_INTC_EVENTID_70                 =   70,
    CSL_INTC_EVENTID_71                 =   71,
    CSL_INTC_EVENTID_72                 =   72,
    CSL_INTC_EVENTID_73                 =   73,
    CSL_INTC_EVENTID_74                 =   74,
    CSL_INTC_EVENTID_75                 =   75,
    CSL_INTC_EVENTID_76                 =   76,
    CSL_INTC_EVENTID_77                 =   77,
    CSL_INTC_EVENTID_78                 =   78,
    CSL_INTC_EVENTID_79                 =   79,
    CSL_INTC_EVENTID_80                 =   80,
    CSL_INTC_EVENTID_81                 =   81,
    CSL_INTC_EVENTID_82                 =   82,
    CSL_INTC_EVENTID_83                 =   83,
    CSL_INTC_EVENTID_84                 =   84,
    CSL_INTC_EVENTID_85                 =   85,
    CSL_INTC_EVENTID_86                 =   86,
    CSL_INTC_EVENTID_87                 =   87,
    CSL_INTC_EVENTID_88                 =   88,
    CSL_INTC_EVENTID_89                 =   89,
    CSL_INTC_EVENTID_90                 =   90,
    CSL_INTC_EVENTID_91                 =   91,
    CSL_INTC_EVENTID_92                 =   92,
    CSL_INTC_EVENTID_93                 =   93,
    CSL_INTC_EVENTID_94                 =   94,
    CSL_INTC_EVENTID_95                 =   95,
    CSL_INTC_EVENTID_96                 =   96,
    CSL_INTC_EVENTID_97                 =   97,
    CSL_INTC_EVENTID_98                 =   98,
    CSL_INTC_EVENTID_99                 =   99,
    CSL_INTC_EVENTID_100                =   100,
    CSL_INTC_EVENTID_101                =   101,
    CSL_INTC_EVENTID_102                =   102,
    CSL_INTC_EVENTID_103                =   103,
    CSL_INTC_EVENTID_104                =   104,
    CSL_INTC_EVENTID_105                =   105,
    CSL_INTC_EVENTID_106                =   106,
    CSL_INTC_EVENTID_107                =   107,
    CSL_INTC_EVENTID_108                =   108,
    CSL_INTC_EVENTID_109                =   109,
    CSL_INTC_EVENTID_110                =   110,
    CSL_INTC_EVENTID_111                =   111,
    CSL_INTC_EVENTID_112                =   112,
    CSL_INTC_EVENTID_113                =   113,
    CSL_INTC_EVENTID_114                =   114,
    CSL_INTC_EVENTID_115                =   115,
    CSL_INTC_EVENTID_116                =   116,
    CSL_INTC_EVENTID_117                =   117,
    CSL_INTC_EVENTID_118                =   118,
    CSL_INTC_EVENTID_119                =   119,
    CSL_INTC_EVENTID_120                =   120,
    CSL_INTC_EVENTID_121                =   121,
    CSL_INTC_EVENTID_122                =   122,
    CSL_INTC_EVENTID_123                =   123,
    CSL_INTC_EVENTID_124                =   124,
    CSL_INTC_EVENTID_125                =   125,
    CSL_INTC_EVENTID_126                =   126,
    CSL_INTC_EVENTID_127                =   127
} CSL_IntcEventId;

/* Event Tag Names */
#define CSL_INTC_EVENTID_EVT0               CSL_INTC_EVENTID_0    /* GEM (INTC)      */
#define CSL_INTC_EVENTID_EVT1				CSL_INTC_EVENTID_1	  /* GEM (INTC)      */
#define CSL_INTC_EVENTID_EVT2				CSL_INTC_EVENTID_2	  /* GEM (INTC)      */
#define CSL_INTC_EVENTID_EVT3				CSL_INTC_EVENTID_3	  /* GEM (INTC)      */
#define CSL_INTC_EVENTID_TINT0				CSL_INTC_EVENTID_4	  /* Timer 0         */
#define CSL_INTC_EVENTID_TINT1				CSL_INTC_EVENTID_5	  /* Timer 0         */
#define CSL_INTC_EVENTID_TINT2				CSL_INTC_EVENTID_6	  /* Timer 1         */
#define CSL_INTC_EVENTID_TINT3				CSL_INTC_EVENTID_7	  /* Timer 1         */
#define CSL_INTC_EVENTID_WDINT				CSL_INTC_EVENTID_8	  /* Timer 2         */
#define CSL_INTC_EVENTID_EMU_DTDMA			CSL_INTC_EVENTID_9	  /* GEM (ECM)       */
#define CSL_INTC_EVENTID_RSVD10				CSL_INTC_EVENTID_10	  /* Reserved        */
#define CSL_INTC_EVENTID_EMU_RTDXRX			CSL_INTC_EVENTID_11	  /* GEM (RTDX)      */
#define CSL_INTC_EVENTID_EMU_RTDXTX			CSL_INTC_EVENTID_12	  /* GEM (RTDX)      */
#define CSL_INTC_EVENTID_IDMAINT0			CSL_INTC_EVENTID_13	  /* GEM (EMC)       */
#define CSL_INTC_EVENTID_IDMAINT1			CSL_INTC_EVENTID_14	  /* GEM (EMC)       */
#define CSL_INTC_EVENTID_RSVD15				CSL_INTC_EVENTID_15	  /* Reserved        */
#define CSL_INTC_EVENTID_ARM2DSP0			CSL_INTC_EVENTID_16	  /* System Module   */
#define CSL_INTC_EVENTID_ARM2DSP1			CSL_INTC_EVENTID_17	  /* System Module   */
#define CSL_INTC_EVENTID_ARM2DSP2			CSL_INTC_EVENTID_18	  /* System Module   */
#define CSL_INTC_EVENTID_ARM2DSP3			CSL_INTC_EVENTID_19	  /* System Module   */
#define CSL_INTC_EVENTID_CP_ME0				CSL_INTC_EVENTID_20	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_IPE0			CSL_INTC_EVENTID_21	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_UNDEF0			CSL_INTC_EVENTID_22	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_ECDERR0			CSL_INTC_EVENTID_23	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_ECDCMP0			CSL_INTC_EVENTID_24	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_MC0				CSL_INTC_EVENTID_25	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_BS0				CSL_INTC_EVENTID_26	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_CALC0			CSL_INTC_EVENTID_27	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_LPF0			CSL_INTC_EVENTID_28	  /* ImCOP0          */
#define CSL_INTC_EVENTID_CP_ARM0			CSL_INTC_EVENTID_29	  /* ImCOP0          */
#define CSL_INTC_EVENTID_RSVD30				CSL_INTC_EVENTID_30	  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD31				CSL_INTC_EVENTID_31	  /* Reserved        */
#define CSL_INTC_EVENTID_CP_UNDEF1			CSL_INTC_EVENTID_32	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_ECDERR1			CSL_INTC_EVENTID_33	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_ECDCMP1			CSL_INTC_EVENTID_34	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_MC1				CSL_INTC_EVENTID_35	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_BS1				CSL_INTC_EVENTID_36	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_CALC1			CSL_INTC_EVENTID_37	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_LPF1			CSL_INTC_EVENTID_38	  /* ImCOP1          */
#define CSL_INTC_EVENTID_CP_ARM1			CSL_INTC_EVENTID_39	  /* ImCOP1          */
#define CSL_INTC_EVENTID_VP_VERTINT0		CSL_INTC_EVENTID_40	  /* VPIF            */
#define CSL_INTC_EVENTID_VP_VERTINT1		CSL_INTC_EVENTID_41	  /* VPIF            */
#define CSL_INTC_EVENTID_VP_VERTINT2		CSL_INTC_EVENTID_42	  /* VPIF            */
#define CSL_INTC_EVENTID_VP_VERTINT3		CSL_INTC_EVENTID_43	  /* VPIF            */
#define CSL_INTC_EVENTID_VP_ERRINT			CSL_INTC_EVENTID_44	  /* VPIF            */
#define CSL_INTC_EVENTID_RSVD45				CSL_INTC_EVENTID_45	  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD46				CSL_INTC_EVENTID_46	  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD47				CSL_INTC_EVENTID_47	  /* Reserved        */
#define CSL_INTC_EVENTID_CRGENINT0			CSL_INTC_EVENTID_48	  /* CRGEN 0         */
#define CSL_INTC_EVENTID_CRGENINT1			CSL_INTC_EVENTID_49	  /* CRGEN 1         */
#define CSL_INTC_EVENTID_STRMINT0			CSL_INTC_EVENTID_50	  /* Stream I/O 0    */
#define CSL_INTC_EVENTID_STRMINT1			CSL_INTC_EVENTID_51	  /* Stream I/O 1    */
#define CSL_INTC_EVENTID_GRENGINT			CSL_INTC_EVENTID_52	  /* Graphics Engine */
#define CSL_INTC_EVENTID_PBISTINT			CSL_INTC_EVENTID_53	  /* PBIST Module    */
#define CSL_INTC_EVENTID_AXINT0				CSL_INTC_EVENTID_54	  /* McASP 0         */
#define CSL_INTC_EVENTID_ARINT0				CSL_INTC_EVENTID_55	  /* McASP 0         */
#define CSL_INTC_EVENTID_AXINT1				CSL_INTC_EVENTID_56	  /* McASP 1         */
#define CSL_INTC_EVENTID_ARINT1				CSL_INTC_EVENTID_57	  /* McASP 1         */
#define CSL_INTC_EVENTID_MAC_RXTH			CSL_INTC_EVENTID_58	  /* CPGMAC          */
#define CSL_INTC_EVENTID_MAC_RX				CSL_INTC_EVENTID_59	  /* CPGMAC          */
#define CSL_INTC_EVENTID_MAC_TX				CSL_INTC_EVENTID_60	  /* CPGMAC          */
#define CSL_INTC_EVENTID_MAC_MISC			CSL_INTC_EVENTID_61	  /* CPGMAC          */
#define CSL_INTC_EVENTID_USBINT				CSL_INTC_EVENTID_62	  /* USB             */
#define CSL_INTC_EVENTID_USBDMAINT			CSL_INTC_EVENTID_63	  /* USB             */
#define CSL_INTC_EVENTID_GPIO0				CSL_INTC_EVENTID_64	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO1				CSL_INTC_EVENTID_65	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO2				CSL_INTC_EVENTID_66	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO3				CSL_INTC_EVENTID_67	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO4				CSL_INTC_EVENTID_68	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO5				CSL_INTC_EVENTID_69	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO6				CSL_INTC_EVENTID_70	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIO7				CSL_INTC_EVENTID_71	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIOBNK0			CSL_INTC_EVENTID_72	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIOBNK1			CSL_INTC_EVENTID_73	  /* GPIO            */
#define CSL_INTC_EVENTID_GPIOBNK2			CSL_INTC_EVENTID_74	  /* GPIO            */
#define CSL_INTC_EVENTID_IICINT				CSL_INTC_EVENTID_75	  /* I2C             */
#define CSL_INTC_EVENTID_SPINT0				CSL_INTC_EVENTID_76	  /* SPI             */
#define CSL_INTC_EVENTID_SPINT1				CSL_INTC_EVENTID_77	  /* SPI             */
#define CSL_INTC_EVENTID_PWMINT0			CSL_INTC_EVENTID_78	  /* PWM 0           */
#define CSL_INTC_EVENTID_PWMINT1			CSL_INTC_EVENTID_79	  /* PWM 1           */
#define CSL_INTC_EVENTID_UARTINT0			CSL_INTC_EVENTID_80	  /* UART 0          */
#define CSL_INTC_EVENTID_UARTINT1			CSL_INTC_EVENTID_81	  /* UART 1          */
#define CSL_INTC_EVENTID_UARTINT2			CSL_INTC_EVENTID_82	  /* UART 2          */
#define CSL_INTC_EVENTID_VLQINT				CSL_INTC_EVENTID_83	  /* VLYNQ           */
#define CSL_INTC_EVENTID_CCINT1				CSL_INTC_EVENTID_84	  /* TPCC Region 1   */
#define CSL_INTC_EVENTID_CCERRINT			CSL_INTC_EVENTID_85	  /* TPCC Error      */
#define CSL_INTC_EVENTID_TCERRINT0			CSL_INTC_EVENTID_86	  /* TPTC0 Error     */
#define CSL_INTC_EVENTID_TCERRINT1			CSL_INTC_EVENTID_87	  /* TPTC1 Error     */
#define CSL_INTC_EVENTID_TCERRINT2			CSL_INTC_EVENTID_88	  /* TPTC2 Error     */
#define CSL_INTC_EVENTID_TCERRINT3			CSL_INTC_EVENTID_89	  /* TPTC3 Error     */
#define CSL_INTC_EVENTID_IDEINT				CSL_INTC_EVENTID_90	  /* ATA             */
#define CSL_INTC_EVENTID_HPIINT				CSL_INTC_EVENTID_91	  /* UHPI            */
#define CSL_INTC_EVENTID_PCIINT				CSL_INTC_EVENTID_92	  /* PCI             */
#define CSL_INTC_EVENTID_DDRINT				CSL_INTC_EVENTID_93	  /* DDR EMIF        */
#define CSL_INTC_EVENTID_AEMIFINT			CSL_INTC_EVENTID_94	  /* Async EMIF      */
#define CSL_INTC_EVENTID_PSCINT				CSL_INTC_EVENTID_95	  /* PSC             */
#define CSL_INTC_EVENTID_INTERR				CSL_INTC_EVENTID_96	  /* GEM (INTC)      */
#define CSL_INTC_EVENTID_EMC_IDMAERR		CSL_INTC_EVENTID_97	  /* GEM (EMC)       */
#define CSL_INTC_EVENTID_GEM_PBISTINT		CSL_INTC_EVENTID_98	  /* GEM (PBIST)     */
#define CSL_INTC_EVENTID_RSVD99				CSL_INTC_EVENTID_99	  /* Reserved        */
#define CSL_INTC_EVENTID_EFIINTA			CSL_INTC_EVENTID_100  /* GEM (EFI A)     */
#define CSL_INTC_EVENTID_EFIINTB			CSL_INTC_EVENTID_101  /* GEM (EFI B)     */
#define CSL_INTC_EVENTID_RSVD102			CSL_INTC_EVENTID_102  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD103			CSL_INTC_EVENTID_103  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD104			CSL_INTC_EVENTID_104  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD105			CSL_INTC_EVENTID_105  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD106			CSL_INTC_EVENTID_106  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD107			CSL_INTC_EVENTID_107  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD108			CSL_INTC_EVENTID_108  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD109			CSL_INTC_EVENTID_109  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD110			CSL_INTC_EVENTID_110  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD111			CSL_INTC_EVENTID_111  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD112			CSL_INTC_EVENTID_112  /* Reserved        */
#define CSL_INTC_EVENTID_PMC_ED				CSL_INTC_EVENTID_113  /* GEM (PMC)       */
#define CSL_INTC_EVENTID_RSVD114			CSL_INTC_EVENTID_114  /* Reserved        */
#define CSL_INTC_EVENTID_RSVD115			CSL_INTC_EVENTID_115  /* Reserved        */
#define CSL_INTC_EVENTID_UMC_ED1			CSL_INTC_EVENTID_116  /* GEM (UMC)       */
#define CSL_INTC_EVENTID_UMC_ED2			CSL_INTC_EVENTID_117  /* GEM (UMC)       */
#define CSL_INTC_EVENTID_PDC_INT			CSL_INTC_EVENTID_118  /* GEM (PDC)       */
#define CSL_INTC_EVENTID_SYS_CMPA			CSL_INTC_EVENTID_119  /* SYS             */
#define CSL_INTC_EVENTID_PMC_CMPA			CSL_INTC_EVENTID_120  /* GEM (PMC)       */
#define CSL_INTC_EVENTID_PMC_DMPA			CSL_INTC_EVENTID_121  /* GEM (PMC)       */
#define CSL_INTC_EVENTID_DMC_CMPA			CSL_INTC_EVENTID_122  /* GEM (DMC)       */
#define CSL_INTC_EVENTID_DMC_DMPA			CSL_INTC_EVENTID_123  /* GEM (DMC)       */
#define CSL_INTC_EVENTID_UMC_CMPA			CSL_INTC_EVENTID_124  /* GEM (UMC)       */
#define CSL_INTC_EVENTID_UMC_DMPA			CSL_INTC_EVENTID_125  /* GEM (UMC)       */
#define CSL_INTC_EVENTID_EMC_CMPA			CSL_INTC_EVENTID_126  /* GEM (EMC)       */
#define CSL_INTC_EVENTID_EMC_BUSERR			CSL_INTC_EVENTID_127  /* GEM (EMC)       */

/**
 * @brief   Enumeration of the control commands
 *
 * These are the control commands that could be used with
 * CSL_intcHwControl(..). Some of the commands expect an
 * argument as documented along-side the description of
 * the command.
 */
typedef enum {
    CSL_INTC_CMD_EVTENABLE,
        /**<
         * @brief   Enables the event
         * @param   CSL_IntcEnableState
         */
    CSL_INTC_CMD_EVTDISABLE,
        /**<
         * @brief   Disables the event
         * @param   CSL_IntcEnableState
         */
    CSL_INTC_CMD_EVTSET,
    	/**<
         * @brief   Sets the event manually
         * @param   None
         */
    CSL_INTC_CMD_EVTCLEAR,
        /**<
         * @brief   Clears the event (if pending)
         * @param   None
         */
    CSL_INTC_CMD_EVTDROPENABLE,
        /**<
         * @brief   Enables the Drop Event detection feature for this event
         * @param   None
         */
    CSL_INTC_CMD_EVTDROPDISABLE,
        /**<
         * @brief   Disables the Drop Event detection feature for this event
         * @param   None
         */
    CSL_INTC_CMD_EVTINVOKEFUNCTION
        /**<
         * @brief   To be used ONLY to invoke the associated Function handle with Event
         * when the user is writing an exception handling routine.
         * @param   None
         */
} CSL_IntcHwControlCmd;


/**
 * @brief   Enumeration of the queries
 *
 * These are the queries that could be used with CSL_intcGetHwStatus(..).
 * The queries return a value through the object pointed to by the pointer
 * that it takes as an argument. The argument supported by the query is
 * documented along-side the description of the query.
 */
typedef enum {

	CSL_INTC_QUERY_PENDSTATUS
		/**<
         * @brief   The Pend Status of the Event is queried
         * @param   Bool
         */

}CSL_IntcHwStatusQuery;


/**
 * @brief   Enumeration of the exception mask registers
 *
 * These are the symbols used along with the value to be programmed
 * into the Exception mask register.
 */
typedef enum {
    CSL_INTC_EXCEP_0TO31,
        /**<
         * @brief   Symbol for EXPMASK[0]
         * @param   BitMask for EXPMASK0
         */
    CSL_INTC_EXCEP_32TO63,
        /**<
         * @brief   Symbol for EXPMASK[1]
         * @param   BitMask for EXPMASK1
         */
    CSL_INTC_EXCEP_64TO95,
    	/**<
         * @brief   Symbol for EXPMASK[2]
         * @param   BitMask for EXPMASK2
         */
    CSL_INTC_EXCEP_96TO127
        /**<
         * @brief   Symbol for EXPMASK[3]
         * @param   BitMask for EXPMASK3
         */
} CSL_IntcExcepEn;

/**
 * @brief   Enumeration of the exception
 *
 * These are the symbols used along with the Excpetion Clear
 * API
 */
typedef enum {
	CSL_INTC_EXCEPTION_NMI = 31,
		/**<
         * @brief   Symbol for NMI
         * @param   None
         */
	CSL_INTC_EXCEPTION_EXT = 30,
		/**<
         * @brief   Symbol for External Exception
         * @param   None
         */
	CSL_INTC_EXCEPTION_INT = 1,
		/**<
         * @brief   Symbol for Internal Exception
         * @param   None
         */
	CSL_INTC_EXCEPTION_SW = 0
		/**<
         * @brief   Symbol for Software Exception
         * @param   None
         */
}CSL_IntcExcep;
/**
@}
*/
/** @addtogroup CSL_INTC_DATASTRUCT
@{ */


/**
 * @brief   Event Handler pointer
 *
 * Event handlers ought to conform to this type
 */
typedef void (* CSL_IntcEventHandler)(void *);


/**
 * @brief   Event Handler Record
 *
 * Used to set-up the event-handler using CSL_intcPlugEventHandler(..)
 */
typedef struct CSL_IntcEventHandlerRecord {
    CSL_IntcEventHandler    handler;    /**< pointer to the event handler */
    void *                  arg;        /**< the argument to be passed to the
                                          handler when it is invoked */
} CSL_IntcEventHandlerRecord;

/**
 * @brief   INTC Module Context
 */
typedef struct {
    CSL_IntcEventHandlerRecord* eventhandlerRecord;
    CSL_BitMask32   eventAllocMask[(CSL_INTC_EVENTID_CNT + 31) / 32];
    Uint16          numEvtEntries;
    Int8            offsetResv[128];
} CSL_IntcContext;


/**
 * @brief   Event enable state
 */
typedef Uint32 CSL_IntcEventEnableState;


/**
 * @brief   Global Interrupt enable state
 */
typedef Uint32 CSL_IntcGlobalEnableState;
/**
 * @brief   The interrupt handle object
 *
 * This object is used refenced by the handle to identify the event.
 */
typedef struct CSL_IntcObj {
    CSL_IntcEventId eventId;    /**< The event-id */
    CSL_IntcVectId   vectId;    /**< The vector-id */
} CSL_IntcObj;

/**
 * @brief   The drop status structure
 *
 * This object is used along with the CSL_intcQueryDropStatus()
 * API.
 */
typedef struct CSL_IntcDropStatus {
	Bool drop;					/**< whether dropped/not */
	CSL_IntcEventId eventId;	/**< The event-id */
	CSL_IntcVectId  vectId;	    /**< The vect-id */
}CSL_IntcDropStatus;

/**
 * @brief   INTC module parameters for open
 *
 * This is equivalent to the Vector Id for the event number.
 */
typedef CSL_IntcVectId CSL_IntcParam;

/**
@}
*/

/**
 * @brief   The interrupt handle
 *
 * This is returned by the CSL_intcOpen(..) API. The handle is used
 * to identify the event of interest in all INTC calls.
 */

typedef struct CSL_IntcObj *    CSL_IntcHandle;

/** @addtogroup CSL_INTC_FUNCTION
@{ */
/*
 * ======================================================
 *   @func   CSL_intcInit
 * ======================================================
 */
/** @brief
 *	   Must be called before the use of the
 *  top level APIs.
 */
 /* ======================================================
 *   @arg  intcEventHandlerRecord
 *		Pointer to the user allocated Intc context
 *   @ret  CSL_Status - CSL_SOK
 *
 * ======================================================
 */

CSL_Status
    CSL_intcInit (
        CSL_IntcContext *   pContext
);

/*
 * ======================================================
 *   @func   CSL_intcOpen
 * ======================================================
 */
/** @brief
 *	   Intc Open API.
 */
 /* ======================================================
 *   @arg  intcObj
 *		Intc Object
 *   @arg  eventId
 *		Event ID associated with the Intc Object
 *   @arg  CSL_IntcParam
 *	    Vector Id attributes.
 *   @arg  status
 *	    place holder for status
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */

CSL_IntcHandle
    CSL_intcOpen (
        CSL_IntcObj *       intcObj,    /**< pointer to the CSL-object allocated by the user */
        CSL_IntcEventId     eventId,    /**< the event-id of the interrupt */
        CSL_IntcParam *    params,     /**< module specific parameter */
        CSL_Status *        status      /**< (optional) pointer to a variable that would receive the status */
);
/*
 * ======================================================
 *   @func   CSL_intcClose
 * ======================================================
 */
/** @brief
 *	   Intc Close API. Invalidates the Intc Handle and frees the
 *     resource.
 */
 /* ======================================================
 *   @arg  hIntc
 *		Intc Handle
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */

CSL_Status
    CSL_intcClose (
        CSL_IntcHandle      hIntc   /**< Handle to an event; see CSL_intcOpen(..) */
);
/*
 * ======================================================
 *   @func   CSL_intcPlugEventHandler
 * ======================================================
 */
/** @brief
 *	   Must be called to register the event handler and the
 *  associated argument.
 */
 /* ======================================================
 *   @arg  hIntc
 *		Intc Handle
 *   @arg  eventHandlerRecord
 *		record to be registered for the specified handle.
 *   @ret  CSL_Status - CSL_SOK
 *
 * ======================================================
 */
CSL_Status
    CSL_intcPlugEventHandler (
        CSL_IntcHandle                  hIntc,
        /**< Handle identifying the interrupt-event; see CSL_intcOpen(..) */
		CSL_IntcEventHandlerRecord *    eventHandlerRecord
        /**< Provides the details of the event-handler */
);
/*
 * ======================================================
 *   @func   CSL_intcHookIsr
 * ======================================================
 */
/** @brief
 *	   Associates an ISR with an event (CPU vector id) directly
 */
 /* ======================================================
 *   @arg  vectId
 *		Vector Id for which the Interrupt service routine
 *   is plugged in
 *   @arg  isrAddr
 *		Address of the Interrupt Service routine.
 *   @ret  CSL_Status - CSL_SOK
 *
 * ======================================================
 */
CSL_Status
    CSL_intcHookIsr (
        CSL_IntcVectId  vectId,     /**< Vector identifier   */
        void *          isrAddr     /**< Pointer to the ISR */
);
/*
 * ======================================================
 *   @func   CSL_intcHwControl
 * ======================================================
 */
/** @brief
 *	   Intc Control API.
 */
 /* ======================================================
 *   @arg  hIntc
 *		Intc Handle
 *   @arg  command
 *		Intc command
 *   @arg  commandArg
 *		Intc command arguments
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */

CSL_Status
    CSL_intcHwControl (
        CSL_IntcHandle          hIntc,      /**< Handle to an event; see CSL_intcOpen(..) */
        CSL_IntcHwControlCmd    command,    /**< Command identifier */
        void *                  commandArg  /**< Optional command parameter */
);

/*
 * ======================================================
 *   @func   CSL_intcGetHwStatus
 * ======================================================
 */
/** @brief
 *	   Intc query API.
 */
 /* ======================================================
 *   @arg  hIntc
 *		Intc Handle
 *   @arg  query
 *		Intc query
 *   @arg  response
 *		Place holder for the response
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
    CSL_intcGetHwStatus (
        CSL_IntcHandle          hIntc,      /**< Handle to an event; see CSL_intcOpen(..) */
        CSL_IntcHwStatusQuery   query,      /**< Query identifier */
        void *                  response    /**< Pointer to an object that would contain the retrieved information */
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalEnable
 * ======================================================
 */
/** @brief
 *	   Global Interrupt Enable API
 */
 /* ======================================================
 *   @arg  prevState
 *		place holder for the previous state if desired by the user.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
    CSL_intcGlobalEnable (
        CSL_IntcGlobalEnableState * prevState   /**< (Optional) Pointer to object that would store current state */
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalDisable
 * ======================================================
 */
/** @brief
 *	   Global Interrupt Disable API
 */
 /* ======================================================
 *   @arg  prevState
 *		place holder for the previous state if desired by the user.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
    CSL_intcGlobalDisable (
        CSL_IntcGlobalEnableState * prevState   /**< (Optional) Pointer to object that would store current state */
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalRestore
 * ======================================================
 */
/** @brief
 *	   Global Interrupt Restore API
 */
 /* ======================================================
 *   @arg  prevState
 *		State to be programmed
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
    CSL_intcGlobalRestore (
        CSL_IntcGlobalEnableState   prevState   /**< Object containing information about previous state */
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalNmiEnable
 * ======================================================
 */
/** @brief
 *	   Global NMI enable
 */
 /* ======================================================
 *   @arg  None
 *
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcGlobalNmiEnable(
		void
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalExcepEnable
 * ======================================================
 */
/** @brief
 *	   Global Exception enable
 */
 /* ======================================================
 *   @arg  None
 *
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcGlobalExcepEnable(
		void
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalExtExcepEnable
 * ======================================================
 */
/** @brief
 *	   Global External Exception enable
 */
 /* ======================================================
 *   @arg  None
 *
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcGlobalExtExcepEnable(
		void
);
/*
 * ======================================================
 *   @func   CSL_intcGlobalExcepClear
 * ======================================================
 */
/** @brief
 *	   Global Exception Clear
 */
 /* ======================================================
 *   @arg  exc
 *		Type of exception :- NMI/SW/EXT/INT to be cleared
 *
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcGlobalExcepClear(
		CSL_IntcExcep exc
);
/*
 * ======================================================
 *   @func   CSL_intcExcepAllEnable
 * ======================================================
 */
/** @brief
 *	   Exception enable
 */
 /* ======================================================
 *   @arg  excepMask
 *		Register to be programmed EXPMASK0/1/2/3.
 *   @arg  excVal
 *		Bitmask to be programmed in the above register.
 *   @arg  prevState
 *		place holder for the previous state.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcExcepAllEnable(
		CSL_IntcExcepEn excepMask,
		CSL_IntcEventEnableState  excVal,
		CSL_IntcEventEnableState  *prevState
);
/*
 * ======================================================
 *   @func   CSL_intcExcepAllDisable
 * ======================================================
 */
/** @brief
 *	   Exception disable
 */
 /* ======================================================
 *   @arg  excepMask
 *		Register to be programmed EXPMASK0/1/2/3.
 *   @arg  excVal
 *		Bitmask to be programmed in the above register.
 *   @arg  prevState
 *		place holder for the previous state.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcExcepAllDisable(
		CSL_IntcExcepEn excepMask,
		CSL_BitMask32   excVal,
		CSL_IntcEventEnableState *prevState
);
/*
 * ======================================================
 *   @func   CSL_intcExcepAllRestore
 * ======================================================
 */
/** @brief
 *	   Exception restore
 */
 /* ======================================================
 *   @arg  excepMask
 *		Register to be programmed EXPMASK0/1/2/3.
 *   @arg  excVal
 *		Bitmask to be programmed in the above register.
 *   @arg  prevState
 *		place holder for the previous state.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcExcepAllRestore(
		CSL_IntcExcepEn excepMask,
		CSL_IntcEventEnableState prevState
);

/*
 * ======================================================
 *   @func   CSL_intcExcepAllClear
 * ======================================================
 */
/** @brief
 *	   Exception clear
 */
 /* ======================================================
 *   @arg  excepMask
 *		Register to be programmed EVTCLR/1/2/3.
 *   @arg  excVal
 *		Bitmask to be programmed in the above register.
 *   @arg  prevState
 *		place holder for the previous state.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcExcepAllClear(
		CSL_IntcExcepEn excepMask,
		CSL_BitMask32   excVal
);
/*
 * ======================================================
 *   @func   CSL_intcExcepAllStatus
 * ======================================================
 */
/** @brief
 *	   Exception status query
 */
 /* ======================================================
 *   @arg  excepMask
 *		Register to be programmed EVTCLR0/1/2/3.
 *   @arg  status
 *		Place holder for the status.
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status
	CSL_intcExcepAllStatus(
		CSL_IntcExcepEn excepMask,
		CSL_IntcEventEnableState *status
);
/*
 * ======================================================
 *   @func   CSL_intcQueryDropStatus
 * ======================================================
 */
/** @brief
 *	   Exception drop status query
 */
 /* ======================================================
 *   @arg  dropStat
 *		Place holder for the drop status
 *   @ret  CSL_Status - CSL_SOK/CSL_ESYS_BADHANDLE
 *
 * ======================================================
 */
CSL_Status CSL_intcQueryDropStatus(
		CSL_IntcDropStatus   *dropStat
);
/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 1          Aug 11:11:56 9         2274             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/
