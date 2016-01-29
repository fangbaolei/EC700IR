#ifndef _CSLSYS_SOCC64PLUS_H_
#define _CSLSYS_SOCC64PLUS_H_

#include <cslr.h>
/**************************************************************************\
* 64 soc file
\**************************************************************************/

/**************************************************************************\
*  Parameterizable Configuration:- These are fed directly from the RTL 
*  parameters for the given SOC
\**************************************************************************/
/**************************************************************************\
* Channel Instance count
\**************************************************************************/
/** @brief Number of Generic Channel instances */


/** @brief Enumerations for EDMA channels
*
*  There are 8 QDMA channels -
*
*/

#define    CSL_DAT_QCHA0		   		      0 /**< QDMA Channel 0*/
#define    CSL_DAT_QCHA1		   		      1 /**< QDMA Channel 1*/
#define    CSL_DAT_QCHA2    	   		      2 /**< QDMA Channel 2*/
#define    CSL_DAT_QCHA3		   		      3 /**< QDMA Channel 3*/
#define    CSL_DAT_QCHA4		   		      4 /**< QDMA Channel 4*/
#define    CSL_DAT_QCHA5		   		      5 /**< QDMA Channel 5*/
#define    CSL_DAT_QCHA6		   		      6 /**< QDMA Channel 6*/
#define    CSL_DAT_QCHA7		   		      7 /**< QDMA Channel 7*/

/** @brief Enumerations for EDMA Event Queues
*
*  There are 8 Event Queues. Q0 is the highest priority and Q7 is the least priority
*
*/
typedef enum {
	CSL_DAT_PRI_DEFAULT   = 			   0, /**< Queue 0 is default */  
	CSL_DAT_PRI_0 		   = 			   0, /**< Queue 0 */  
	CSL_DAT_PRI_1 		   = 			   1, /**< Queue 1 */  
	CSL_DAT_PRI_2 		   = 			   2, /**< Queue 2 */  
	CSL_DAT_PRI_3 		   = 			   3, /**< Queue 3 */  
	CSL_DAT_PRI_4 		   = 			   4, /**< Queue 4 */  
	CSL_DAT_PRI_5 		   = 			   5, /**< Queue 5 */  
	CSL_DAT_PRI_6 		   = 			   6, /**< Queue 6 */  
	CSL_DAT_PRI_7 		   = 			   7  /**< Queue 7 */  
} CSL_DatPriority;

/** @brief Enumeration for EDMA Regions 
*
*  
*/

#define	CSL_DAT_REGION_GLOBAL  -1   /**< Global Region */
#define	CSL_DAT_REGION_0  0         /**< EDMA Region 0 */
#define	CSL_DAT_REGION_1  1         /**< EDMA Region 1 */
#define	CSL_DAT_REGION_2  2         /**< EDMA Region 2 */
#define	CSL_DAT_REGION_3  3         /**< EDMA Region 3 */

/** @brief Base address of Channel controller  memory mapped registers */
#ifdef HIMALAYA
#define CSL_EDMACC_0_REGS                   (0x02A00000u)
#else
#define CSL_EDMACC_0_REGS                   (0x01c00000u)
#endif
/** #brief Cache Module base address  */
#define CSL_CACHE_REGS		                (0x01840000u)
#endif  /* _CSLSYS_SOCC64PLUS_H_ */

