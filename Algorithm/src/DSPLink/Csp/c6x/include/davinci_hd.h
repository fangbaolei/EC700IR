/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/
#ifndef _DAVINCI_HD_H
#define _DAVINCI_HD_H

/*****************************************************************************/
 /** \file davinci.h
 * 
 * \brief This file contains the Chip Description for DAVINCI (ARM side)
 * 
 *****************************************************************************/

#include <cslr.h>
#include <tistdtypes.h>

#define CSL_IDEF_INLINE static inline
/*****************************************************************************\
* Include files for all the modules in the device
\*****************************************************************************/

#include "cslr_uart_001.h"
#include "cslr_i2c_001.h"
#include "cslr_pwm_001.h"
#include "cslr_pllc_001.h"
#include "cslr_intc_001.h"
#include "cslr_spi_001.h"
#include "cslr_gpio_002.h"
#include "cslr_aemif_001.h"
#include "cslr_vlynq_001.h"
#include "cslr_ddr_001.h"

#include "cslr_emac.h"
#include "cslr_ectl.h"
#include "cslr_mdio.h"
#include "cslr_sys_001.h"
#include "cslr_tmr_001.h"
#include "cslr_uhpi_001.h"
#include "cslr_usb_001.h"
#include "cslr_psc_001.h"

#include "cslr_ge.h"
#include "cslr_spio.h"
#include "cslr_crgen.h"
#include "cslr_vpif.h"
#include "cslr_mcasp_001.h"
#include "cslr_edmacc.h"
#include "cslr_edmatc.h"
#include "cslr_pciif.h"  /* ADDED from C64LC */
#include "cslr_sec.h"
#include "cslr_idma.h"

/* Kaleido registers */
#include "kaleido_mem_map.h"
#include "cslr_top10.h"
#include "cslr_top11.h"
#include "cslr_ecd.h"
#include "cslr_calc.h"
#include "cslr_bs.h"
#include "cslr_mc.h"
#include "cslr_lpf.h"
#include "cslr_me.h"
#include "cslr_ipe.h"
#include "cslr_clkc.h"
#include "cslr_bfswc.h"
#include "cslr_kldintc.h"
#include "cslr_icec.h"

/*****************************************************************************\
* Peripheral Instance counts
\*****************************************************************************/

#define CSL_UART_PER_CNT          3                                       
#define CSL_I2C_PER_CNT           1                                       
#define CSL_PWM_PER_CNT           2                                       
#define CSL_PLLC_PER_CNT          2                                       
#define CSL_ATA_PER_CNT           1                                       
#define CSL_SPI_PER_CNT           1                                       
#define CSL_GPIO_PER_CNT          1
#define CSL_EMIF_PER_CNT          1                                       
#define CSL_VLYNQ_PER_CNT         1                                       
#define CSL_DDR_PER_CNT           1                                       

#define CSL_SYS_PER_CNT           1 
#define CSL_TMR_PER_CNT           3 
#define CSL_UHPI_PER_CNT          1
#define CSL_USB_PER_CNT           1
#define CSL_PSC_PER_CNT           1

#define CSL_EMAC_PER_CNT   		  1
#define CSL_ECTL_PER_CNT    	  1
#define CSL_MDIO_PER_CNT 		  1

/* Davinci-HD Specific Peripherals */
#define CSL_GE_PER_CNT            1
#define CSL_SPIO_PER_CNT          2
#define CSL_CRGEN_PER_CNT         2
#define CSL_VPIF_PER_CNT          1
#define CSL_MCASP_PER_CNT         2
#define CSL_EDMA_CC_CNT           1
#define CSL_EDMA_TC_CNT           4
#define CSL_PCI_PER_CNT			  1
#define CSL_SEC_PER_CNT   	      1
#define CSL_IDMA_PER_CNT   	      1

/* Kaleido Peripheral Instance Counts */
#define CSLR_TOP_PER_CNT          2
#define CSLR_ECD_PER_CNT          2
#define CSLR_CALC_PER_CNT         2
#define CSLR_BS_PER_CNT           2
#define CSLR_MC_PER_CNT           2
#define CSLR_LPF_PER_CNT          2
#define CSLR_ME_PER_CNT           2
#define CSLR_IPE_PER_CNT          2
#define CSLR_CLKC_PER_CNT         2
#define CSLR_BFSWC_PER_CNT        2
#define CSLR_KLDINTC_PER_CNT      2
#define CSLR_ICEC_PER_CNT         2


/*****************************************************************************\
* Peripheral Overlay Structures
\*****************************************************************************/

typedef volatile CSL_UartRegs            *CSL_UartRegsOvly;                  
typedef volatile CSL_I2cRegs             *CSL_I2cRegsOvly;                 
typedef volatile CSL_PwmRegs             *CSL_PwmRegsOvly;                   
typedef volatile CSL_PllcRegs            *CSL_PllcRegsOvly;
typedef volatile CSL_IntcRegs            *CSL_IntcRegsOvly;
typedef volatile CSL_SpiRegs             *CSL_SpiRegsOvly;                   
typedef volatile CSL_GpioRegs            *CSL_GpioRegsOvly;
typedef volatile CSL_AemifRegs           *CSL_EmifRegsOvly;
typedef volatile CSL_VlynqRegs           *CSL_VlynqRegsOvly;                  
typedef volatile CSL_DdrRegs             *CSL_DdrRegsOvly;

typedef volatile CSL_SysRegs             *CSL_SysRegsOvly;                  
typedef volatile CSL_TmrRegs             *CSL_TmrRegsOvly;                  
typedef volatile CSL_UhpiRegs            *CSL_UhpiRegsOvly;                  
typedef volatile CSL_UsbRegs             *CSL_UsbRegsOvly;                  
typedef volatile CSL_PscRegs             *CSL_PscRegsOvly;

typedef volatile CSL_EmacRegs            *CSL_EmacRegsOvly;
typedef volatile CSL_EctlRegs            *CSL_EctlRegsOvly;
typedef volatile CSL_MdioRegs            *CSL_MdioRegsOvly;

typedef volatile CSL_GeRegs              *CSL_GeRegsOvly;
typedef volatile CSL_SpioRegs            *CSL_SpioRegsOvly;
typedef volatile CSL_CrgenRegs           *CSL_CrgenRegsOvly;
typedef volatile CSL_VpifRegs            *CSL_VpifRegsOvly;
typedef volatile CSL_McaspRegs           *CSL_McaspRegsOvly;
typedef volatile CSL_EdmaccRegs          *CSL_EdmaccRegsOvly;
typedef volatile CSL_EdmaccShadowRegs    *CSL_EdmaccShadowRegsOvly;
typedef volatile CSL_EdmatcRegs          *CSL_EdmatcRegsOvly;
typedef volatile CSL_PciifRegs			 *CSL_PciifRegsOvly;
typedef volatile CSL_SecRegs             *CSL_SecRegsOvly;
typedef volatile CSL_IdmaRegs		     *CSL_IdmaRegsOvly;

/* Kaliedo registers */
typedef volatile CSL_Top10Regs           *CSL_Top10RegsOvly;
typedef volatile CSL_Top11Regs           *CSL_Top11RegsOvly;
typedef volatile CSL_EcdRegs             *CSL_EcdRegsOvly;
typedef volatile CSL_CalcRegs            *CSL_CalcRegsOvly;
typedef volatile CSL_BsRegs              *CSL_BsRegsOvly;
typedef volatile CSL_McRegs              *CSL_McRegsOvly;
typedef volatile CSL_LpfRegs             *CSL_LpfRegsOvly;
typedef volatile CSL_MeRegs              *CSL_MeRegsOvly;
typedef volatile CSL_IpeRegs             *CSL_IpeRegsOvly;
typedef volatile CSL_ClkcRegs            *CSL_ClkcRegsOvly;
typedef volatile CSL_BfswcRegs           *CSL_BfswcRegsOvly;
typedef volatile CSL_KldintcRegs         *CSL_KldintcRegsOvly;
typedef volatile CSL_IcecRegs            *CSL_IcecRegsOvly;

/*****************************************************************************\
* Peripheral Base Address
\*****************************************************************************/

#define CSL_INTC_REGS                    ((CSL_IntcRegsOvly) 0x1800000u)
#define CSL_IDMA_REGS		             ((CSL_IdmaRegsOvly)0x01820000u)
#define CSL_DDR_0_REGS                   ((CSL_DdrRegsOvly)  0x20000000)        
#define CSL_EMIF_0_REGS                  ((CSL_EmifRegsOvly) 0x20008000)        
#define CSL_VLYNQ_0_REGS                 ((CSL_VlynqRegsOvly)0x20010000)        

#define CSL_EDMACC_0_REGS                ((CSL_EdmaccRegsOvly) 0x01C00000) 
#define CSL_EDMATC_0_REGS                ((CSL_EdmatcRegsOvly) 0x01C10000)
#define CSL_EDMATC_1_REGS                ((CSL_EdmatcRegsOvly) 0x01C10400)
#define CSL_EDMATC_2_REGS                ((CSL_EdmatcRegsOvly) 0x01C10800)
#define CSL_EDMATC_3_REGS                ((CSL_EdmatcRegsOvly) 0x01C10C00)

#define CSL_VPIF_0_REGS                  ((CSL_VpifRegsOvly) 0x01C12000)
#define CSL_GE_0_REGS                    ((CSL_GeRegsOvly)   0x01C12800)
#define CSL_SPIO_0_REGS                  ((CSL_SpioRegsOvly) 0x01C13000)
#define CSL_SPIO_1_REGS                  ((CSL_SpioRegsOvly) 0x01C13400)
#define CSL_PCIIF_0_REGS				 ((CSL_PciifRegsOvly)0x01c1A000)

#define CSL_UART_0_REGS                  ((CSL_UartRegsOvly) 0x01C20000)         
#define CSL_UART_1_REGS                  ((CSL_UartRegsOvly) 0x01C20400)         
#define CSL_UART_2_REGS                  ((CSL_UartRegsOvly) 0x01C20800)         
#define CSL_I2C_0_REGS                   ((CSL_I2cRegsOvly)  0x01C21000)        
#define CSL_TMR_0_REGS                   ((CSL_TmrRegsOvly)  0x01C21400)         
#define CSL_TMR_1_REGS                   ((CSL_TmrRegsOvly)  0x01C21800)         
#define CSL_TMR_2_REGS                   ((CSL_TmrRegsOvly)  0x01C21C00)         
#define CSL_PWM_0_REGS                   ((CSL_PwmRegsOvly)  0x01C22000)
#define CSL_PWM_1_REGS                   ((CSL_PwmRegsOvly)  0x01C22400)
#define CSL_CRGEN_0_REGS                 ((CSL_CrgenRegsOvly)0x01C26000)
#define CSL_CRGEN_1_REGS                 ((CSL_CrgenRegsOvly)0x01C26400)
#define CSL_SYS_0_REGS                   ((CSL_SysRegsOvly)  0x01C40000)
#define CSL_SEC_0_REGS                   ((CSL_SecRegsOvly)  0x01C40400)
#define CSL_PLLC_0_REGS                  ((CSL_PllcRegsOvly) 0x01C40800)        
#define CSL_PLLC_1_REGS                  ((CSL_PllcRegsOvly) 0x01C40C00)        
#define CSL_PSC_0_REGS                   ((CSL_PscRegsOvly)  0x01C41000)
#define CSL_USB_0_REGS                   ((CSL_UsbRegsOvly)  0x01C64000)
#define CSL_ATA_0_REGS                   ((CSL_AtaRegsOvly)  0x01C66000)          
#define CSL_SPI_0_REGS                   ((CSL_SpiRegsOvly)  0x01C66800)          
#define CSL_GPIO_REGS                    ((CSL_GpioRegsOvly) 0x01C67000)
#define CSL_UHPI_0_REGS                  ((CSL_UhpiRegsOvly) 0x01C67800)
#define CSL_EMAC_0_REGS                  ((CSL_EmacRegsOvly) 0x01C80000)
#define CSL_ECTL_0_REGS                  ((CSL_EctlRegsOvly) 0x01C81000)
#define CSL_MDIO_0_REGS                  ((CSL_MdioRegsOvly) 0x01C84000)
#define CSL_MCASP0_REGS                  ((CSL_McaspRegsOvly)0x01D01000)
#define CSL_MCASP1_REGS                  ((CSL_McaspRegsOvly)0x01D01800)
#define CSL_MCASPCON_0_REGS              ((CSL_McaspRegsOvly)0x01D01000)
#define CSL_MCASPDAT_0_REGS              ((CSL_McaspRegsOvly)0x01D01400)
#define CSL_MCASPCON_1_REGS              ((CSL_McaspRegsOvly)0x01D01800)
#define CSL_MCASPDAT_1_REGS              ((CSL_McaspRegsOvly)0x01D01C00)

/* Kaliedo Reg Base Addresses */
#define KLD0_CFG_BASE   0x02000000
#define KLD1_CFG_BASE   0x02200000

#define KLD0_TOPREG          ( KLD0_CFG_BASE + KLD_CFG_TOPREG   )
#define KLD0_ECDREG          ( KLD0_CFG_BASE + KLD_CFG_ECDREG   )
#define KLD0_CALCREG         ( KLD0_CFG_BASE + KLD_CFG_CALCREG  )
#define KLD0_BSREG           ( KLD0_CFG_BASE + KLD_CFG_BSREG    )
#define KLD0_MCREG           ( KLD0_CFG_BASE + KLD_CFG_MCREG    )
#define KLD0_LPFREG          ( KLD0_CFG_BASE + KLD_CFG_LPFREG   )
#define KLD0_MEREG           ( KLD0_CFG_BASE + KLD_CFG_MEREG    )
#define KLD0_IPEREG          ( KLD0_CFG_BASE + KLD_CFG_IPEREG   )
#define KLD0_CLKCREG         ( KLD0_CFG_BASE + KLD_CFG_CLKCREG  )
#define KLD0_BFSWCREG        ( KLD0_CFG_BASE + KLD_CFG_BFSWCREG )
#define KLD0_INTCREG         ( KLD0_CFG_BASE + KLD_CFG_INTCREG  )
#define KLD0_ICECREG         ( KLD0_CFG_BASE + KLD_CFG_ICECREG  )

#define KLD1_TOPREG          ( KLD1_CFG_BASE + KLD_CFG_TOPREG   )
#define KLD1_ECDREG          ( KLD1_CFG_BASE + KLD_CFG_ECDREG   )
#define KLD1_CALCREG         ( KLD1_CFG_BASE + KLD_CFG_CALCREG  )
#define KLD1_BSREG           ( KLD1_CFG_BASE + KLD_CFG_BSREG    )
#define KLD1_MCREG           ( KLD1_CFG_BASE + KLD_CFG_MCREG    )
#define KLD1_LPFREG          ( KLD1_CFG_BASE + KLD_CFG_LPFREG   )
#define KLD1_CLKCREG         ( KLD1_CFG_BASE + KLD_CFG_CLKCREG  )
#define KLD1_BFSWCREG        ( KLD1_CFG_BASE + KLD_CFG_BFSWCREG )
#define KLD1_INTCREG         ( KLD1_CFG_BASE + KLD_CFG_INTCREG  )
#define KLD1_ICECREG         ( KLD1_CFG_BASE + KLD_CFG_ICECREG  )

#define CSL_KLD0_TOP10_REGS        ((CSL_Top10RegsOvly)  KLD0_TOPREG)
#define CSL_KLD0_ECD_REGS          ((CSL_EcdRegsOvly)    KLD0_ECDREG)  
#define CSL_KLD0_CALC_REGS         ((CSL_CalcRegsOvly)   KLD0_CALCREG)
#define CSL_KLD0_BS_REGS           ((CSL_BsRegsOvly)     KLD0_BSREG)
#define CSL_KLD0_MC_REGS           ((CSL_McRegsOvly)     KLD0_MCREG)
#define CSL_KLD0_LPF_REGS          ((CSL_LpfRegsOvly)    KLD0_LPFREG)
#define CSL_KLD0_ME_REGS           ((CSL_MeRegsOvly)     KLD0_MEREG)
#define CSL_KLD0_IPE_REGS          ((CSL_IpeRegsOvly)    KLD0_IPEREG)
#define CSL_KLD0_CLKC_REGS         ((CSL_ClkcRegsOvly)   KLD0_CLKCREG)
#define CSL_KLD0_BFSW_REGS         ((CSL_BfswcRegsOvly)  KLD0_BFSWCREG)
#define CSL_KLD0_INTC10_REGS       ((CSL_KldintcRegsOvly)KLD0_INTCREG)
#define CSL_KLD0_ICEC_REGS         ((CSL_IcecRegsOvly)   KLD0_ICECREG)

#define CSL_KLD1_TOP11_REGS        ((CSL_Top10RegsOvly)  KLD1_TOPREG)
#define CSL_KLD1_ECD_REGS          ((CSL_EcdRegsOvly)    KLD1_ECDREG)  
#define CSL_KLD1_CALC_REGS         ((CSL_CalcRegsOvly)   KLD1_CALCREG)
#define CSL_KLD1_BS_REGS           ((CSL_BsRegsOvly)     KLD1_BSREG)
#define CSL_KLD1_MC_REGS           ((CSL_McRegsOvly)     KLD1_MCREG)
#define CSL_KLD1_LPF_REGS          ((CSL_LpfRegsOvly)    KLD1_LPFREG)
#define CSL_KLD1_CLKC_REGS         ((CSL_ClkcRegsOvly)   KLD1_CLKCREG)
#define CSL_KLD1_BFSW_REGS         ((CSL_BfswcRegsOvly)  KLD1_BFSWCREG)
#define CSL_KLD1_INTC11_REGS       ((CSL_KldintcRegsOvly)KLD1_INTCREG)
#define CSL_KLD1_ICEC_REGS         ((CSL_IcecRegsOvly)   KLD1_ICECREG)

/******************************************************************************\
* EMAC Descriptor section (Added from Faraday)
\******************************************************************************/

#define CSL_EMAC_DSC_BASE_ADDR              (0x01C82000u) /* CPGMAC CPPI RAM */
// #define CSL_EMAC_DSC_BASE_ADDR_L2           (0x00900800u)

/* EMAC Descriptor Size and Element Count */
#define CSL_EMAC_DSC_SIZE                   8192
#define CSL_EMAC_DSC_ENTRY_SIZE             16   /* Size of a buffer descriptor, in bytes */
#define CSL_EDMA_DSC_ENTRY_COUNT            (CSL_EMAC_DSC_SIZE/CSL_EMAC_DSC_ENTRY_SIZE)  /* 512 */

/** \brief  UART Module Instances
* 
*/
typedef enum {
  CSL_UART_ANY    = -1, /**< <b>: Any instance of UART module</b> */
  CSL_UART_0          =  0,  /**< <b>: UART Instance 0</b> */
  CSL_UART_1          =  1,  /**< <b>: UART Instance 1</b> */
  CSL_UART_2          =  2  /**< <b>: UART Instance 2</b> */
} CSL_UartNum;

/** \brief I2C Module Instances
* 
*/
typedef enum {
  CSL_I2C_ANY    = -1, /**< <b>: Any instance of I2C module</b> */
  CSL_I2C_0      =  0  /**< <b>: I2C Instance 0</b> */
} CSL_I2cNum;

/** \brief  PWM Module Instances
* 
*/
typedef enum {
  CSL_PWM_ANY    = -1, /**< <b>: Used to refer any instance of PWM
			      module</b> */
  CSL_PWM_0          =  0,  /**< <b>: PWM Instance 0</b> */
  CSL_PWM_1          =  1,  /**< <b>: PWM Instance 1</b> */
  CSL_PWM_2          =  2  /**< <b>: PWM Instance 2</b> */
} CSL_PwmNum;

/** \brief PLLC Module Instances
* 
*/
typedef enum {
  CSL_PLLC_ANY    = -1, /**< <b>: Any instance of PLLC module</b> */
  CSL_PLLC_0      =  0, /**< <b>: PLLC Instance 0</b> */
  CSL_PLLC_1      =  1  /**< <b>: PLLC Instance 1</b> */
} CSL_PllcNum;

/** \brief  ATA Module Instances
* 
*/
typedef enum {
    CSL_ATA_PRIMARY = 0,
    CSL_ATA_0 = 0,
    CSL_ATA_SECONDARY = 1,
    CSL_ATA_1 = 0,    
    CSL_ATA_ANY = -1
} CSL_AtaNum;

/** \brief  SPI Module Instances
* 
*/
typedef enum {
  CSL_SPI_ANY    = -1, /**< <b>: Any instance of SPI module</b> */
  CSL_SPI_0      =  0  /**< <b>: SPI Instance 0</b> */
} CSL_SpiNum;

/** \brief GPIO Module Instances
* 
*/
typedef enum {
  CSL_GPIO_ANY    = -1, /**< <b>: Any instance of GPIO module</b> */
  CSL_GPIO        =  0  /**< <b>: GPIO </b> */
} CSL_GpioNum;

/** \brief EMIF Module Instances
* 
*/
typedef enum {
  CSL_EMIF_ANY    = -1, /**< <b>: Any instance of EMIF module</b> */
  CSL_EMIF_0      =  0  /**< <b>: EMIF Instance 0</b> */
} CSL_EmifNum;

/** \brief  VLYNQ Module Instances
* 
*/
typedef enum {
  CSL_VLYNQ_ANY    = -1, /**< <b>: Any instance of VLYNQ module</b> */
  CSL_VLYNQ_0      =  0  /**< <b>: VLYNQ Instance 0</b> */
} CSL_VlynqNum;

/** \brief DDR Module Instances
* 
*/
typedef enum {
  CSL_DDR_ANY    = -1, /**< <b>: Any instance of DDR module</b> */
  CSL_DDR_0      =  0  /**< <b>: DDR Instance 0</b> */
} CSL_DdrNum;


/** \brief SYS Module Instances
* 
*/
typedef enum {
  CSL_SYS_ANY    = -1, /**< <b>: Any instance of SYS module</b> */
  CSL_SYS_0      =  0  /**< <b>: SYS Instance 0</b> */
} CSL_SysNum;

/** \brief TMR Module Instances
* 
*/
typedef enum {
  CSL_TMR_ANY    = -1, /**< <b>: Any instance of TMR module</b> */
  CSL_TMR_0      =  0, /**< <b>: TMR Instance 0</b> */
  CSL_TMR_1      =  1, /**< <b>: TMR Instance 1</b> */
  CSL_TMR_2      =  2  /**< <b>: TMR Instance 2</b> */
} CSL_TmrNum;

/** \brief UHPI Module Instances
* 
*/
typedef enum {
  CSL_UHPI_ANY    = -1, /**< <b>: Any instance of UHPI module</b> */
  CSL_UHPI_0      =  0  /**< <b>: UHPI Instance 0</b> */
} CSL_UhpiNum;

/** \brief USB Module Instances
* 
*/
typedef enum {
  CSL_USB_ANY    = -1, /**< <b>: Any instance of USB module</b> */
  CSL_USB_0      =  0  /**< <b>: USB Instance 0</b> */
} CSL_UsbNum;

/** \brief PSC Module Instances
* 
*/
typedef enum {
  CSL_PSC_ANY    = -1, /**< <b>: Any instance of PSC module</b> */
  CSL_PSC_0      =  0  /**< <b>: PSC Instance 0</b> */
} CSL_PscNum;

/** \brief EMAC Module Instances
* 
*/
typedef enum {
  CSL_EMAC_ANY    = -1, /**< <b>: Any instance of EMAC module</b> */
  CSL_EMAC_0      =  0  /**< <b>: EMAC Instance 0</b> */
} CSL_EmacNum;

/** \brief ECTL Module Instances
* 
*/
typedef enum {
  CSL_ECTL_ANY    = -1, /**< <b>: Any instance of ECTL module</b> */
  CSL_ECTL_0      =  0  /**< <b>: ECTL Instance 0</b> */
} CSL_EctlNum;

/** \brief MDIO Module Instances
* 
*/
typedef enum {
  CSL_MDIO_ANY    = -1, /**< <b>: Any instance of MDIO module</b> */
  CSL_MDIO_0      =  0  /**< <b>: MDIO Instance 0</b> */
} CSL_MdioNum;

/** \brief GE Module Instances
* 
*/
typedef enum {
  CSL_GE_ANY    = -1, /**< <b>: Any instance of GE module</b> */
  CSL_GE_0      =  0  /**< <b>: GE Instance 0</b> */
} CSL_GeNum;

/** \brief SPIO Module Instances
* 
*/
typedef enum {
  CSL_SPIO_ANY    = -1, /**< <b>: Any instance of SPIO module</b> */
  CSL_SPIO_0      =  0, /**< <b>: SPIO Instance 0</b> */
  CSL_SPIO_1      =  1  /**< <b>: SPIO Instance 1</b> */
} CSL_SpioNum;

/** \brief CRGEN Module Instances
* 
*/
typedef enum {
  CSL_CRGEN_ANY    = -1, /**< <b>: Any instance of CRGEN module</b> */
  CSL_CRGEN_0      =  0, /**< <b>: CRGEN Instance 0</b> */
  CSL_CRGEN_1      =  1  /**< <b>: CRGEN Instance 1</b> */
} CSL_CrgenNum;

/** \brief VPIF Module Instances
* 
*/
typedef enum {
  CSL_VPIF_ANY    = -1, /**< <b>: Any instance of VPIF module</b> */
  CSL_VPIF_0      =  0  /**< <b>: VPIF Instance 0</b> */
} CSL_VpifNum;

/** \brief MCASP Module Instances
* 
*/
typedef enum {
  CSL_MCASP_ANY   = -1, /**< <b>: Any instance of MCASP module</b> */
  CSL_MCASP_0     =  0, /**< <b>: MCASP 0 Instance 0</b> */
  CSL_MCASP_1     =  1  /**< <b>: MCASP 1 Instance 1</b> */
} CSL_McaspNum;

/** \brief PCIIF Module Instances
* 
*/
typedef enum {
  CSL_PCIIF_ANY    = -1, /**< <b>: Any instance of PCIIF module</b> */
  CSL_PCIIF_0      =  0  /**< <b>: PCIIF Instance 0</b> */
} CSL_PciifNum;

extern void * _CSL_uartlookup[];
extern void * _CSL_i2clookup[];
extern void * _CSL_pwmlookup[];
extern void * _CSL_pllclookup[];
extern void * _CSL_atalookup[];
extern void * _CSL_spilookup[];
extern void * _CSL_gpiolookup[];
extern void * _CSL_emiflookup[];
extern void * _CSL_vlynqlookup[];
extern void * _CSL_ddrlookup[];
extern void * _CSL_venclookup[];
extern void * _CSL_vpbelookup[];
extern void * _CSL_vfoclookup[];
extern void * _CSL_gelookup[];
extern void * _CSL_spiolookup[];
extern void * _CSL_crgenlookup[];
extern void * _CSL_vpiflookup[];
extern void * _CSL_mcasplookup[];
extern void * _CSL_edmatclookup[];
extern void * _CSL_edmacclookup[];
extern void * _csl_pciiflookup[];

// Memory map for GEM
// Base address
#define GEMUMAP1_KLD0_BASE   0x11400000
#define GEMUMAP1_KLD1_BASE   0x11600000

#define KLD0_PORT1_RW_BASE   0x40400000
#define KLD0_PORT2_W_BASE    0x40480000
#define KLD0_PORT3_R_BASE    0x40440000

#define KLD1_PORT1_RW_BASE   0x40600000
#define KLD1_PORT2_W_BASE    0x40680000
#define KLD1_PORT3_R_BASE    0x40640000

// Macros to access each buffers inside Kaleido
// ex) KLD0_GEMUMAP1_BUF( STRBUF )

#define KLD0_GEMUMAP1_BUF(BUF) (GEMUMAP1_KLD0_BASE + KLD_DMA_##BUF## )
#define KLD0_PORT1_RW_BUF(BUF) (KLD0_PORT1_RW_BASE + KLD_DMA_##BUF## )
#define KLD0_PORT2_W_BUF(BUF)  (KLD0_PORT2_W_BASE  + KLD_DMA_##BUF## )
#define KLD0_PORT3_R_BUF(BUF)  (KLD0_PORT3_R_BASE  + KLD_DMA_##BUF## )

#define KLD1_GEMUMAP1_BUF(BUF) (GEMUMAP1_KLD1_BASE + KLD_DMA_##BUF## )
#define KLD1_PORT1_RW_BUF(BUF) (KLD1_PORT1_RW_BASE + KLD_DMA_##BUF## )
#define KLD1_PORT2_W_BUF(BUF)  (KLD1_PORT2_W_BASE  + KLD_DMA_##BUF## )
#define KLD1_PORT3_R_BUF(BUF)  (KLD1_PORT3_R_BASE  + KLD_DMA_##BUF## )

#endif /* _DAVINCI_HD_H */

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 10        14 Jan 2005 13:32:22    5888             xkeshavm      */
/*                                                                  */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and DSP*/
/********************************************************************/ 
