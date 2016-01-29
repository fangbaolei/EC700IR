/*  ============================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */

#ifndef _SOC_H_
#define _SOC_H_

/* =============================================================================
 *  Revision History
 *  ===============
 *  14-Mar-2005 brn Moved the Event Ids from csl_intc.h to soc64plus.h
 *  20-Jun-2005 sd  Changed the interrupt ID defines
 *  22-Aug-2005 sd  added the defines for MDIO 
 *  14-Dec-2005 sd  updated for the PLLC_2 based address and removed the EVENT ID
 *                  for VLYNQ and EDC
 * =============================================================================
 */

#include <cslr.h>
/**************************************************************************\
* 64 soc file
\**************************************************************************/

/*****************************************************************************\
* Static inline definition
\*****************************************************************************/
#ifndef CSL_IDEF_INLINE
#define CSL_IDEF_INLINE static inline
#endif

/**************************************************************************\
* Peripheral Instance count
\**************************************************************************/
/** @brief Number of MCBSP instances */
#define CSL_MCBSP_CNT                   2

/** @brief Number of TIMER 64 instances */
#define CSL_TMR_CNT                     2

/** @brief Number of DDR2 instances */
#define CSL_DDR2_CNT                    1

/** @brief Number of EMIF64 instances */
#define CSL_EMIF64_CNT                  1

/** @brief Number of EDMA3 CC instances */
#define CSL_EDMA3_CC_CNT                1

/** @brief Number of EDMA3 CC instances */
#define CSL_EDMA3_TC_CNT                4

/** @brief Number of EMAC instances */
#define CSL_EMAC_CNT                    1

/** @brief Number of ECTL instances */
#define CSL_ECTL_CNT                   1

/** @brief Number of HPI instances */
#define CSL_HPI_CNT                    1

/** @brief Number of UTOPIA instances */
#define CSL_UTOPIA2_CNT                  1

/** @brief Number of I2C instances */
#define CSL_I2C_CNT                     1

/** @brief Number of GPIO instances */
#define CSL_GPIO_CNT                    1

/** @brief Number of MDIO instances */
#define CSL_MDIO_CNT                    1

/** @brief Number of SRIO instances */
#define CSL_SRIO_CNT                    1

/**************************************************************************\
* Peripheral Instance definitions.
\**************************************************************************/
/** @brief Peripheral Instances of MCBSP instances */
#define CSL_MCBSP_0                     (0)
#define CSL_MCBSP_1                     (1)

/** @brief Peripheral Instances of Timer 64 instances */
#define CSL_TMR_0                       (0)
#define CSL_TMR_1                       (1)


/** @brief Peripheral Instance of EDMA instances */
#define CSL_EDMA3                       (0)

/** @brief Peripheral Instance for DDR2 */
#define CSL_DDR2                        (0) 

/** @brief Peripheral Instance for EMIFA */
#define  CSL_EMIFA                      (0) 

/** @brief Peripheral Instance for EMAC */
#define  CSL_EMAC                       (0) 

/** @brief Peripheral Instance for ECTL */
#define  CSL_ECTL                      (0) 

/** @brief Peripheral Instance for HPI */
#define CSL_HPI                        (0) 

/** @brief Peripheral Instance for UTOPIA */
#define CSL_UTOPIA2                      (0)

/** @brief Peripheral Instance for I2C */
#define CSL_I2C                         (0) 

/** @brief Peripheral Instance for GPIO */
#define CSL_GPIO				        (0)

/** @brief Peripheral Instances for MDIO */
#define CSL_MDIO           	            (0) 

/** @brief Peripheral Instances for PWRDWN */
#define CSL_PWRDWN                      (0)

/** @brief Instance number of L2 memory protection */
#define CSL_MEMPROT_L2                  (0) 

/** @brief Instance number of L1P memory protection */
#define CSL_MEMPROT_L1P                 (1) 

/** @brief Instance number of L1D memory protection */
#define CSL_MEMPROT_L1D                 (2) 

/** @brief Instance number of memory protection config */
#define CSL_MEMPROT_CONFIG              (3) 

/** @brief Instance number of Bandwidth Management */
#define CSL_BWMNGMT                     (0) 

/** @brief Instance number of PLL controller 1 */
#define CSL_PLLC_1                     (0) 

/** @brief Instance number of PLL controller 1 */
#define CSL_PLLC_2                     (1) 

/** @brief Instance number of RAPID IO */
#define CSL_SRIO                     (0) 

/**************************************************************************\
* Peripheral Base Address
\**************************************************************************/
/** @brief Base address of MCBSP memory mapped registers */
#define CSL_MCBSP_0_REGS                (0x028C0000u)
#define CSL_MCBSP_1_REGS                (0x02900000u)

/** @brief Base address of MCBSP EDMA memory mapped registers */
#define CSL_MCBSP_0_TX_EDMA_REGS        (0x30000010u)
#define CSL_MCBSP_0_RX_EDMA_REGS        (0x30000000u)
#define CSL_MCBSP_1_TX_EDMA_REGS        (0x34000010u)
#define CSL_MCBSP_1_RX_EDMA_REGS        (0x34000000u)

/** @brief Base address of timer64 memory mapped registers */
#define CSL_TMR_0_REGS                  (0x02940000u)
#define CSL_TMR_1_REGS                  (0x02980000u)

/** #brief DDR2 Module memory mapped address  */
#define CSL_DDR2_0_REGS                 (0x78000000)

/** #brief EMIF64 Module memory mapped address  */
#define CSL_EMIFA_0_REGS                (0x70000000)

/** #brief I2C Module memory mapped address  */
#define CSL_I2C_0_REGS                  (0x02B04000u)

/** #brief Cache Module memory mapped address  */
#define CSL_CACHE_0_REGS                (0x01840000u)

/** #brief IDMA Module memory mapped address  */
#define CSL_IDMA_0_REGS                 (0x01820000u)

/** @brief Base address of INTC memory mapped registers */
#define CSL_INTC_0_REGS                 (0x01800000u)

/** @brief Base address of Channel controller  memory mapped registers */
#define CSL_EDMA3CC_0_REGS              (0x02A00000u)

/** @brief Base address of Transfer controller  memory mapped registers */
#define CSL_EDMA3TC_0_REGS               (0x02A20000u)
#define CSL_EDMA3TC_1_REGS               (0x02A28000u)
#define CSL_EDMA3TC_2_REGS               (0x02A30000u)
#define CSL_EDMA3TC_3_REGS               (0x02A38000u)

/** @brief Base address of TCP2 memory mapped registers */
#define CSL_TCP2_0_REGS                  (0x02BA0000u)

/** @brief Base address of TCP2 configuration registers */
#define CSL_TCP2_CFG_REGS                (0x50000000u) 

/** @brief Base address of TCP2 memories */
#define CSL_TCP2_X0_MEM                 (0x50010000u) 
#define CSL_TCP2_W0_MEM                 (0x50030000u) 
#define CSL_TCP2_W1_MEM                 (0x50040000u) 
#define CSL_TCP2_I0_MEM                 (0x50050000u) 
#define CSL_TCP2_O0_MEM                 (0x50060000u) 
#define CSL_TCP2_S0_MEM                 (0x50070000u) 
#define CSL_TCP2_T0_MEM                 (0x50080000u) 
#define CSL_TCP2_C0_MEM                 (0x50090000u) 
#define CSL_TCP2_A0_MEM                 (0x500A0000u) 
#define CSL_TCP2_B0_MEM                 (0x500B0000u) 

/** @brief Base address of VCP2 memory mapped registers */
#define CSL_VCP2_0_REGS                 (0x02B80000u)

/** @brief Base address of VCP2 regsiters accessed via EDMA */
#define CSL_VCP2_EDMA_REGS              (0x58000000u) 

/** @brief Base address of EMAC memory mapped registers */
#define CSL_EMAC_0_REGS                 (0x02c80000u)

/** @brief Base address of EMAC control memory mapped registers */
#define CSL_ECTL_0_REGS                (0x02C81000u)

/** @brief HPI Module memory mapped address  */
#define CSL_HPI_0_REGS                 (0x02880000u)

/** @brief UTOPIA Module memory mapped address  */
#define CSL_UTOPIA2_0_REGS               (0x02B40000u)

/** @brief UTOPIA RX data Module memory mapped address  */
#define CSL_UTOPIA2_RX_EDMA_REGS          (0x3C000000u)

/** @brief UTOPIA TX data memory mapped address  */
#define CSL_UTOPIA2_TX_EDMA_REGS          (0x3C000400u)

/** @brief GPIO Module memory mapped address  */
#define CSL_GPIO_0_REGS                 (0x02B00000u)

/** @brief MDIO Module memory mapped address    */
#define CSL_MDIO_0_REGS                 (0x02C81800u)

/** @brief device configuration registers memory mapped address    */
#define CSL_DEV_REGS                    (0x02A80000u)     

/** @brief Base address of PDC registers */
#define CSL_PWRDWN_PDC_REGS 	        (0x01810000)

/** @brief Base address of L2 power Down registers */
#define CSL_PWRDWN_L2_REGS	            (0x0184c000)

/** @brief Base address of UMC Memory protection registers */
#define CSL_MEMPROT_L2_REGS             (0x184A000u)
 
/** @brief Base address of PMC memory Protection registers */
#define CSL_MEMPROT_L1P_REGS            (0x184A400u)

/** @brief Base address of DMC memory protection registers */
#define CSL_MEMPROT_L1D_REGS            (0x184AC00u)

/** @brief Base address of CONFIG memory protection registers */
#define CSL_MEMPROT_CONFIG_REGS         (0x1820300u)

/** @brief Bandwidth Management module address */
#define CSL_BWMNGMT_0_REGS	            (0x01820200u)

/** @brief PLL controller instance 1 module address */
#define CSL_PLLC_1_REGS	                (0x029A0000u)

/** @brief PLL controller instance 2 module address */
#define CSL_PLLC_2_REGS	                (0x029C0000u)

/** @brief SRIO module base address */
#define CSL_SRIO_0_REGS                 (0x02D00000u)


/*****************************************************************************\
* Interrupt Event IDs
\*****************************************************************************/

/**
 * @brief   Interrupt Event IDs
 */
/* Output of event combiner 0, for events 1 to 31  */
#define    CSL_INTC_EVENTID_EVT0            (0)              
/* Output of event combiner 0, for events 32 to 63 */
#define    CSL_INTC_EVENTID_EVT1            (1)              
/* Output of event combiner 0, for events 64 to 95 */
#define    CSL_INTC_EVENTID_EVT2            (2)              
/* Output of event combiner 0, for events 96 to 127 */
#define    CSL_INTC_EVENTID_EVT3            (3)              

/* Event ID 4-8 are reserved */

/** EMU interrupt for: 
  * 1. Host scan access 
  * 2. DTDMA transfer complete
  * 3. AET interrupt
  */    
#define    CSL_INTC_EVENTID_EMU_DTDMA       (9)         

/* Event ID 10 is reserved */

/* EMU real time data exchange receive complete    */
#define    CSL_INTC_EVENTID_EMU_RTDXRX      (11)        
/* EMU RTDX transmit complete   */
#define    CSL_INTC_EVENTID_EMU_RTDXTX      (12)        
/* IDMA Channel 0 Interrupt */
#define    CSL_INTC_EVENTID_IDMA0           (13)        
/* IDMA Channel 1 Interrupt */
#define    CSL_INTC_EVENTID_IDMA1           (14)        
/* HPI/PCI Host interrupt   */
#define    CSL_INTC_EVENTID_HINT            (15)        
/* I2C interrupt            */
#define    CSL_INTC_EVENTID_I2CINT          (16)        
/* Ethernet MAC interrupt   */
#define    CSL_INTC_EVENTID_MACINT          (17)        
/* EMIFA Error Interrupt    */
#define    CSL_INTC_EVENTID_AEASYNCERR      (18)   
     
/* Event ID 19 is reserved  */

/* RapidIO interrupt 0      */
#define    CSL_INTC_EVENTID_RIOINT0         (20)        
/* RapidIO interrupt 1      */
#define    CSL_INTC_EVENTID_RIOINT1         (21)        
/* RapidIO interrupt 4            */
#define    CSL_INTC_EVENTID_RIOINT4         (22)        

/* Event ID 23 is reserved */

/* EDMA3 channel global completion interrupt */
#define    CSL_INTC_EVENTID_EDMA3CC_GINT         (24)        

/* Event ID 25-29 is reserved */    

/* L2 Wakeup interrupt 0      */
#define    CSL_INTC_EVENTID_L2PDWAKE0       (30)        
/* L2 Wakeup interrupt 1      */
#define    CSL_INTC_EVENTID_L2PDWAKE1       (31)        
/* VCP2 error interrupt     */
#define    CSL_INTC_EVENTID_VCP2_INT        (32)        
/* TCP2 error interrupt     */
#define    CSL_INTC_EVENTID_TCP2_INT        (33)        

/* Event ID 34-35 is reserved */ 

/* Utopia interrupt         */
#define    CSL_INTC_EVENTID_UINT            (36)        

/* Event ID 37-39 is reserved */ 

/* McBSP0 receive interrupt */
#define    CSL_INTC_EVENTID_RINT0           (40)        
/* McBSP0 transmit interrupt */
#define    CSL_INTC_EVENTID_XINT0           (41)        
/* McBSP1 receive interrupt */
#define    CSL_INTC_EVENTID_RINT1           (42)        
/* McBSP1 transmit interrupt */
#define    CSL_INTC_EVENTID_XINT1           (43)        

/* Event ID 44-50 is reserved */ 

/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT0          (51)         
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT1          (52)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT2          (53)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT3          (54)        
/* GPIO Interrupt */
#define    CSL_INTC_EVENTID_GPINT4          (55)        
/* GPIO Interrupt */
#define    CSL_INTC_EVENTID_GPINT5          (56)        
/* GPIO Interrupt */
#define    CSL_INTC_EVENTID_GPINT6          (57)        
/* GPIO Interrupt */
#define    CSL_INTC_EVENTID_GPINT7          (58)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT8          (59)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT9          (60)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT10         (61)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT11         (62)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT12         (63)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT13         (64)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT14         (65)        
/* GPIO Interrupt           */ 
#define    CSL_INTC_EVENTID_GPINT15         (66)        
/* Timer 0 lower counter interrupt     */
#define    CSL_INTC_EVENTID_TINTLO0         (67)        
/* Timer 0 higher counter interrupt    */
#define    CSL_INTC_EVENTID_TINTHI0         (68)        
/* Timer 1 lower counter interrupt     */
#define    CSL_INTC_EVENTID_TINTLO1         (69)        
/* Timer 1 higher counter interrupt    */
#define    CSL_INTC_EVENTID_TINTHI1         (70)        
/* CC Completion Interrupt - Mask0 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT0       (71)        
/* CC Completion Interrupt - Mask1 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT1       (72)        
/* CC Completion Interrupt - Mask2 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT2       (73)        
/* CC Completion Interrupt - Mask3 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT3       (74)        
/* CC Completion Interrupt - Mask4 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT4       (75)        
/* CC Completion Interrupt - Mask5 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT5       (76)        
/* CC Completion Interrupt - Mask6 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT6       (77)        
/* CC Completion Interrupt - Mask7 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT7       (78)        
/* CC Error Interrupt    */
#define    CSL_INTC_EVENTID_EDMA3CC_ERRINT     (79)              
/* CC Memory Protection Interrupt */
#define    CSL_INTC_EVENTID_EDMA3CC_MPINT      (80)        
/* TC0 Error Interrupt */
#define    CSL_INTC_EVENTID_EDMA3TC0_ERRINT    (81)        
/* TC1 Error Interrupt */
#define    CSL_INTC_EVENTID_EDMA3TC1_ERRINT    (82)        
/* TC2 Error Interrupt */
#define    CSL_INTC_EVENTID_EDMA3TC2_ERRINT    (83)        
/* TC3 Error Interrupt */
#define    CSL_INTC_EVENTID_EDMA3TC3_ERRINT    (84)        
 
/* Event ID 85-95 is reserved */ 

/* Dropped CPU interrupt event */
#define    CSL_INTC_EVENTID_INTERR          (96)        
/* EMC Invalid IDMA parameters     */
#define    CSL_INTC_EVENTID_EMC_IDMAERR     (97)           

/* Event ID 98 - 99 is reserved */ 

/* EFI Interrupt from side A    */
#define    CSL_INTC_EVENTID_EFIINTA         (100)       
/* EFI Interrupt from side B    */
#define    CSL_INTC_EVENTID_EFIINTB         (101)       

/* Event ID 102-117 is reserved */ 

/* Power Down sleep interrupt */
#define    CSL_INTC_EVENTID_PDC_INT        (118)       

/* Event ID 119 is reserved */ 

/* L1P CPU memory protection fault */
#define    CSL_INTC_EVENTID_L1P_CMPA        (120)       
/* L1P DMA memory protection fault */
#define    CSL_INTC_EVENTID_L1P_DMPA        (121)        
/* L1D CPU memory protection fault */
#define    CSL_INTC_EVENTID_L1D_CMPA        (122)       
/* L1D DMA memory protection fault */
#define    CSL_INTC_EVENTID_L1D_DMPA        (123)       
/* L2 CPU memory protection fault */
#define    CSL_INTC_EVENTID_L2_CMPA         (124)       
/* L2 DMA memory protection fault */
#define    CSL_INTC_EVENTID_L2_DMPA         (125)       
/* IDMA CPU memory protection fault */
#define    CSL_INTC_EVENTID_IDMA_CMPA       (126)       
/* IDMA Bus error interrupt  */
#define    CSL_INTC_EVENTID_IDMA_BUSERR     (127)       




/**** EDMA RELATED DEFINES  *********/


/**************************************************************************\
*  Parameterizable Configuration:- These are fed directly from the RTL
*  parameters for the given SOC
\**************************************************************************/

#define CSL_EDMA3_NUM_DMACH            64
#define CSL_EDMA3_NUM_QDMACH            4
#define CSL_EDMA3_NUM_PARAMSETS       256
#define CSL_EDMA3_NUM_EVQUE             4
#define CSL_EDMA3_CHMAPEXIST            1
#define CSL_EDMA3_NUM_REGIONS           8
#define CSL_EDMA3_MEMPROTECT            1

/**************************************************************************\
* Channel Instance count
\**************************************************************************/
#define CSL_EDMA3_CHA_CNT              68

/* EDMA channel synchronization events */
  
/* HPI/PCI-to-DSP event          */
#define CSL_EDMA3_CHA_DSP_EVT   0
/* Timer 0 lower counter event   */
#define CSL_EDMA3_CHA_TEVTLO0   1
/* Timer 0 higher counter event  */
#define CSL_EDMA3_CHA_TEVTHI0   2
/* EDMA3 channel 3 */
#define CSL_EDMA3_CHA_3         3
/* EDMA3 channel 4 */
#define CSL_EDMA3_CHA_4         4
/* EDMA3 channel 5 */
#define CSL_EDMA3_CHA_5         5
/* EDMA3 channel 6 */
#define CSL_EDMA3_CHA_6         6
/* EDMA3 channel 7 */
#define CSL_EDMA3_CHA_7         7
/* EDMA3 channel 8 */
#define CSL_EDMA3_CHA_8         8
/* EDMA3 channel 9 */
#define CSL_EDMA3_CHA_9         9
/* EDMA3 channel 10 */
#define CSL_EDMA3_CHA_10       10
/* EDMA3 channel 11 */
#define CSL_EDMA3_CHA_11       11
/* McBSP0 transmit event */
#define CSL_EDMA3_CHA_XEVT0    12
/* McBSP0 receive event  */
#define CSL_EDMA3_CHA_REVT0    13
/* McBSP1 transmit event */
#define CSL_EDMA3_CHA_XEVT1    14
/* McBSP1 receive event  */
#define CSL_EDMA3_CHA_REVT1    15
/* Timer 1 lower counter event  */
#define CSL_EDMA3_CHA_TEVTLO1   16
/* Timer 1 higher counter event */
#define CSL_EDMA3_CHA_TEVTHI1   17
/* EDMA channel 18*/
#define CSL_EDMA3_CHA_18       18
/* EDMA3 channel 19*/
#define CSL_EDMA3_CHA_19       19
/* Rapid IO Interrupt 1 */
#define CSL_EDMA3_CHA_RIOINT1  20
/* EDMA3 channel 21*/
#define CSL_EDMA3_CHA_21       21
/* EDMA3 channel 22*/
#define CSL_EDMA3_CHA_22       22
/* EDMA3 channel 23*/
#define CSL_EDMA3_CHA_23       23
/* EDMA3 channel 24*/
#define CSL_EDMA3_CHA_24       24
/* EDMA3 channel 25*/
#define CSL_EDMA3_CHA_25       25
/* EDMA3 channel 26*/
#define CSL_EDMA3_CHA_26       26
/* EDMA3 channel 27*/
#define CSL_EDMA3_CHA_27       27
/* VCP2 receive event  */
#define CSL_EDMA3_CHA_VCP2REVT  28
/* VCP2 transmit event */
#define CSL_EDMA3_CHA_VCP2XEVT  29
/* TCP2 receive event  */
#define CSL_EDMA3_CHA_TCP2REVT  30
/* TCP2 transmit event */
#define CSL_EDMA3_CHA_TCP2XEVT  31
/* UTOPIA receive event */
#define CSL_EDMA3_CHA_UREVT    32
/* EDMA3 channel 33 */
#define CSL_EDMA3_CHA_33       33
/* EDMA3 channel 34 */
#define CSL_EDMA3_CHA_34       34
/* EDMA3 channel 35 */
#define CSL_EDMA3_CHA_35       35
/* EDMA3 channel 36 */
#define CSL_EDMA3_CHA_36       36
/* EDMA3 channel 37 */
#define CSL_EDMA3_CHA_37       37
/* EDMA3 channel 38 */
#define CSL_EDMA3_CHA_38       38
/* EDMA3 channel 39 */
#define CSL_EDMA3_CHA_39       39
/* UTOPIA transmit event */
#define CSL_EDMA3_CHA_UXEVT    40
/* EDMA3 channel 41*/
#define CSL_EDMA3_CHA_41       41
/* EDMA3 channel 42*/
#define CSL_EDMA3_CHA_42       42
/* EDMA3 channel 43*/
#define CSL_EDMA3_CHA_43       43
/* I2C receive event */
#define CSL_EDMA3_CHA_ICREVT   44
/* I2C transmit event */
#define CSL_EDMA3_CHA_ICXEVT   45
/** EDMA3 channel 46*/
#define CSL_EDMA3_CHA_46       46
/** EDMA3 channel 47*/
#define CSL_EDMA3_CHA_47       47
/* GPIO event 0 */
#define CSL_EDMA3_CHA_GPINT0   48
/* GPIO event 1 */
#define CSL_EDMA3_CHA_GPINT1   49
/* GPIO event 2 */
#define CSL_EDMA3_CHA_GPINT2   50
/* GPIO event 3 */
#define CSL_EDMA3_CHA_GPINT3   51
/* GPIO event 4 */
#define CSL_EDMA3_CHA_GPINT4   52
/* GPIO event 5 */
#define CSL_EDMA3_CHA_GPINT5   53
/* GPIO event 6 */
#define CSL_EDMA3_CHA_GPINT6   54
/* GPIO event 7 */
#define CSL_EDMA3_CHA_GPINT7   55
/* GPIO event 8 */
#define CSL_EDMA3_CHA_GPINT8   56
/* GPIO event 9 */
#define CSL_EDMA3_CHA_GPINT9   57
/* GPIO event 10 */
#define CSL_EDMA3_CHA_GPINT10  58
/* GPIO event 11 */
#define CSL_EDMA3_CHA_GPINT11  59
/* GPIO event 12 */
#define CSL_EDMA3_CHA_GPINT12  60
/* GPIO event 13 */
#define CSL_EDMA3_CHA_GPINT13  61
/* GPIO event 14 */
#define CSL_EDMA3_CHA_GPINT14  62
/* GPIO event 15 */
#define CSL_EDMA3_CHA_GPINT15  63

/* QDMA channels */
#define    CSL_EDMA3_QCHA_0                     64   /* QDMA Channel 0*/
#define    CSL_EDMA3_QCHA_1                     65   /* QDMA Channel 1*/
#define    CSL_EDMA3_QCHA_2                     66   /* QDMA Channel 2*/
#define    CSL_EDMA3_QCHA_3                     67   /* QDMA Channel 3*/


/* Enumerations for EDMA Event Queues */
typedef enum {
	CSL_EDMA3_QUE_0 		   = 			   0, /* Queue 0 */  
	CSL_EDMA3_QUE_1 		   = 			   1, /* Queue 1 */  
	CSL_EDMA3_QUE_2 		   = 			   2, /* Queue 2 */  
	CSL_EDMA3_QUE_3 		   = 			   3  /* Queue 3 */  
} CSL_Edma3Que;

/* Enumerations for EDMA Transfer Controllers
 *
 * There are 4 Transfer Controllers. Typically a one to one mapping exists
 * between Event Queues and Transfer Controllers.
 *
 */
typedef enum {
	CSL_EDMA3_TC_0 		   = 			   0, /* TC 0 */  
	CSL_EDMA3_TC_1 		   = 			   1, /* TC 1 */  
	CSL_EDMA3_TC_2 		   = 			   2, /* TC 2 */  
	CSL_EDMA3_TC_3 		   = 			   3  /* TC 3 */  
} CSL_Edma3Tc;


#define CSL_EDMA3_REGION_GLOBAL  -1
#define CSL_EDMA3_REGION_0  0
#define CSL_EDMA3_REGION_1  1
#define CSL_EDMA3_REGION_2  2
#define CSL_EDMA3_REGION_3  3
#define CSL_EDMA3_REGION_4  4
#define CSL_EDMA3_REGION_5  5
#define CSL_EDMA3_REGION_6  6
#define CSL_EDMA3_REGION_7  7



/*********** DAT RELATED DEFINES ******************/

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
*  There are 4 QDMA channels -
*
*/

#define    CSL_DAT_QCHA_0		   		      0 /* QDMA Channel 0*/
#define    CSL_DAT_QCHA_1		   		      1 /* QDMA Channel 1*/
#define    CSL_DAT_QCHA_2    	   		      2 /* QDMA Channel 2*/
#define    CSL_DAT_QCHA_3		   		      3 /* QDMA Channel 3*/

/** @brief Enumerations for EDMA Event Queues
*
*  There are 8 Event Queues. Q0 is the highest priority and Q3 is the least priority
*
*/
typedef enum {
	CSL_DAT_PRI_DEFAULT   = 			   0, /* Queue 0 is default */  
	CSL_DAT_PRI_0 		   = 			   0, /* Queue 0 */  
	CSL_DAT_PRI_1 		   = 			   1, /* Queue 1 */  
	CSL_DAT_PRI_2 		   = 			   2, /* Queue 2 */  
	CSL_DAT_PRI_3 		   = 			   3  /* Queue 3 */  
} CSL_DatPriority;

/** @brief Enumeration for EDMA Regions 
*
*  
*/

#define	CSL_DAT_REGION_GLOBAL  -1   /* Global Region */
#define	CSL_DAT_REGION_0  0         /* EDMA Region 0 */
#define	CSL_DAT_REGION_1  1         /* EDMA Region 1 */
#define	CSL_DAT_REGION_2  2         /* EDMA Region 2 */
#define	CSL_DAT_REGION_3  3         /* EDMA Region 3 */
#define	CSL_DAT_REGION_4  4         /* EDMA Region 4 */
#define	CSL_DAT_REGION_5  5         /* EDMA Region 5 */
#define	CSL_DAT_REGION_6  6         /* EDMA Region 6 */
#define	CSL_DAT_REGION_7  7         /* EDMA Region 7 */

#endif  /* _SOC_H_ */

