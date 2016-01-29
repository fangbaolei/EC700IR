#ifndef _DAVINCI_HDEDMA_H_
#define _DAVINCI_HDEDMA_H_

/**************************************************************************\
* Davinci EDMA file
\**************************************************************************/

/**************************************************************************\
*  DaVinci Configuration
*
\**************************************************************************/
#define CSL_EDMA_NUM_DMACH            64
#define CSL_EDMA_NUM_QDMACH            8
#define CSL_EDMA_NUM_PARAMENTRY      512
#define CSL_EDMA_NUM_EVQUE             4
#define CSL_EDMA_CHMAPEXIST            1
#define CSL_EDMA_NUMINTCH			  64
#define CSL_EDMA_NUM_REGIONS	       8
#define CSL_EDMA_MEMPROTECT            0

/** @brief Number of Generic Channel instances */
#define CSL_EDMA_CHA_CNT              72




/** @brief Enumerations for EDMA channels
*
*  There are 72 EDMA channels - 64 EDMA Channels and 8 QDMA Channels.
*  The enum values indicate the number of the channel.
*  This is passed as a parameter in @a CSL_dmaOpen() to indicate the
*  channel instance that the user wants to acquire.  The CSL_DMA_CHA_ANY
*  indicates the user is willing to use any available generic channel.
*
*
*/

/* From Davinci-HD Event Mapping spread sheet */

#define     CSL_EDMA_CHA0		   		      0  /**< Channel 0 */
#define     CSL_EDMA_CHA1		   		      1  /**< Channel 1 */
#define     CSL_EDMA_CHA2		   		      2  /**< Channel 2 */
#define     CSL_EDMA_CHA3		   		      3  /**< Channel 3 */
#define     CSL_EDMA_AXEVTE0                  4  /**< Channel 4 */
#define     CSL_EDMA_AXEVTO0                  5  /**< Channel 5 */
#define     CSL_EDMA_AXEVT0                   6  /**< Channel 6 */
#define     CSL_EDMA_AREVTE0 		   	      7  /**< Channel 7 */
#define     CSL_EDMA_AREVTO0		   	      8  /**< Channel 8 */
#define     CSL_EDMA_AREVT0                   9  /**< Channel 9 */
#define     CSL_EDMA_AXEVT1                   10 /**< Channel 10*/
#define     CSL_EDMA_AREVT1                   11 /**< Channel 11*/
#define     CSL_EDMA_CHA12		   		      12 /**< Channel 12*/
#define     CSL_EDMA_CHA13                    13 /**< Channel 13*/
#define     CSL_EDMA_CHA14                    14 /**< Channel 14*/
#define     CSL_EDMA_CHA15		   		      15 /**< Channel 15*/
#define     CSL_EDMA_SPIXEVT                  16 /**< Channel 16*/
#define     CSL_EDMA_SPIREVT		 	      17 /**< Channel 17*/
#define     CSL_EDMA_URXEVT0		 	      18 /**< Channel 18*/
#define     CSL_EDMA_UTXEVT0		   	      19 /**< Channel 19*/
#define     CSL_EDMA_URXEVT1		   	      20 /**< Channel 20*/
#define     CSL_EDMA_UTXEVT1		   	      21 /**< Channel 21*/
#define     CSL_EDMA_URXEVT2		   	      22 /**< Channel 22*/
#define     CSL_EDMA_UTXEVT2		   	      23 /**< Channel 23*/
#define     CSL_EDMA_CHA24		   		      24 /**< Channel 24*/
#define     CSL_EDMA_CHA25                    25 /**< Channel 25*/
#define     CSL_EDMA_CHA26                    26 /**< Channel 26*/
#define     CSL_EDMA_CHA27                    27 /**< Channel 27*/
#define     CSL_EDMA_ICREVT		   		      28 /**< Channel 28*/
#define     CSL_EDMA_ICXEVT		   		      29 /**< Channel 29*/
#define     CSL_EDMA_CHA30		   		      30 /**< Channel 30*/
#define     CSL_EDMA_CHA31		   		      31 /**< Channel 31*/
#define     CSL_EDMA_GPINT0		   		      32 /**< Channel 32*/
#define     CSL_EDMA_GPINT1		   		      33 /**< Channel 33*/
#define     CSL_EDMA_GPINT2		   		      34 /**< Channel 34*/
#define     CSL_EDMA_GPINT3		   		      35 /**< Channel 35*/
#define     CSL_EDMA_GPINT4		   		      36 /**< Channel 36*/
#define     CSL_EDMA_GPINT5		   		      37 /**< Channel 37*/
#define     CSL_EDMA_GPINT6		   		      38 /**< Channel 38*/
#define     CSL_EDMA_GPINT7		   		      39 /**< Channel 39*/
#define     CSL_EDMA_GPBNKINT0	   		      40 /**< Channel 40*/
#define     CSL_EDMA_GPBNKINT1	   		      41 /**< Channel 41*/
#define     CSL_EDMA_GPBNKINT2	   		      42 /**< Channel 42*/
#define     CSL_EDMA_CP_ECDCMP1               43 /**< Channel 43*/
#define     CSL_EDMA_CP_MC1                   44 /**< Channel 44*/
#define     CSL_EDMA_CP_BS1  		   	      45 /**< Channel 45*/
#define     CSL_EDMA_CP_CALC1                 46 /**< Channel 46*/
#define     CSL_EDMA_CP_LPF1                  47 /**< Channel 47*/
#define     CSL_EDMA_TINT0		   		      48 /**< Channel 48*/
#define     CSL_EDMA_TINT1		   		      49 /**< Channel 49*/
#define     CSL_EDMA_TINT2		   		      50 /**< Channel 50*/
#define     CSL_EDMA_TINT3		   		      51 /**< Channel 51*/
#define     CSL_EDMA_PWM0		   		      52 /**< Channel 52*/
#define     CSL_EDMA_PWM1		   		      53 /**< Channel 53*/
#define     CSL_EDMA_CHA54		   		      54 /**< Channel 54*/
#define     CSL_EDMA_CHA55		   		      55 /**< Channel 55*/
#define     CSL_EDMA_CHA56		   		      56 /**< Channel 56*/
#define     CSL_EDMA_CP_ME0		   		      57 /**< Channel 57*/
#define     CSL_EDMA_CP_IPE0                  58 /**< Channel 58*/
#define     CSL_EDMA_CP_ECDCMP0		   	      59 /**< Channel 59*/
#define     CSL_EDMA_CP_MC0		   		      60 /**< Channel 60*/
#define     CSL_EDMA_CP_BS0		   		      61 /**< Channel 61*/
#define     CSL_EDMA_CP_CALC0		   	      62 /**< Channel 62*/
#define     CSL_EDMA_CP_LPF0                  63 /**< Channel 63*/
#define     CSL_EDMA_QCHA0		   		      64 /**< QDMA Channel 0*/
#define     CSL_EDMA_QCHA1		   		      65 /**< QDMA Channel 1*/
#define     CSL_EDMA_QCHA2    	   		      66 /**< QDMA Channel 2*/
#define     CSL_EDMA_QCHA3		   		      67 /**< QDMA Channel 3*/
#define     CSL_EDMA_QCHA4		   		      68 /**< QDMA Channel 4*/
#define     CSL_EDMA_QCHA5		   		      69 /**< QDMA Channel 5*/
#define     CSL_EDMA_QCHA6		   		      70 /**< QDMA Channel 6*/
#define     CSL_EDMA_QCHA7		   		      71 /**< QDMA Channel 7*/


/** @brief Enumerations for EDMA Event Queues
*
*  4 Event Queues
*
*/
typedef enum {
	CSL_EDMA_EVT_QUE_DEFAULT   = 			   0, /**< Queue 0 is default */
	CSL_EDMA_EVT_QUE0 		   = 			   0, /**< Queue 0 */
	CSL_EDMA_EVT_QUE1 		   = 			   1, /**< Queue 1 */
	CSL_EDMA_EVT_QUE2 		   = 			   2, /**< Queue 2 */
	CSL_EDMA_EVT_QUE3 		   = 			   3  /**< Queue 3 */
} CSL_EdmaEventQueue;
/** @brief Enumerations for EDMA Transfer Controllers
*
*  4 Transfer Controllers. One to one mapping exists
*  between Event Queues and Transfer Controllers.
*
*/
typedef enum {
	CSL_EDMA_TC0 		   = 			   0, /**< TC 0 */
	CSL_EDMA_TC1 		   = 			   1, /**< TC 1 */
	CSL_EDMA_TC2 		   = 			   2, /**< TC 2 */
	CSL_EDMA_TC3 		   = 			   3  /**< TC 3 */
} CSL_EdmaTc;

/** @brief Enumeration for EDMA Regions
*
*  8 Regions.
*
*/

#define 	CSL_EDMA_REGION_GLOBAL  -1   /**< Global Region */
#define 	CSL_EDMA_REGION_0  0         /**< EDMA Region 0 : Shadow Region for ARM */
#define 	CSL_EDMA_REGION_1  1         /**< EDMA Region 1 : Shadow Region for GEM */
#define 	CSL_EDMA_REGION_2  2         /**< EDMA Region 2 : Shadow Region for HD-ImComp0 */
#define 	CSL_EDMA_REGION_3  3         /**< EDMA Region 3 : Shadow Region for HD-ImComp0 */
#define 	CSL_EDMA_REGION_4  4         /**< EDMA Region 4 : Shadow Region for HD-ImComp1 */
#define 	CSL_EDMA_REGION_5  5         /**< EDMA Region 5 : Shadow Region for HD-ImComp1 */
#define 	CSL_EDMA_REGION_6  6         /**< EDMA Region 6: Unused Shadow Region / Might take off from PDK CSL */
#define 	CSL_EDMA_REGION_7  7         /**< EDMA Region 7: Unused Shadow Region / Might take off from PDK CSL */

#endif  /* _DAVINCI_HDEDMA_H_ */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 4         14 Jan 2005 13:32:40    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/
