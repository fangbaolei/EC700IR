/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
*/

/** @mainpage DDR CSL 3.x
 *
 * @section Introduction
 *
 * @subsection xxx Purpose and Scope
 * The purpose of this document is to identify a set of common CSL APIs for
 * the DDR module across various devices. The CSL developer is expected to
 * refer to this document while designing APIs for these modules. Some of the
 * listed APIs may not be applicable to a given DDR module. While other cases
 * this list of APIs may not be sufficient to cover all the features of a
 * particular DDR Module. The CSL developer should use his discretion designing
 * new APIs or extending the existing ones to cover these.
 *
 * @subsection aaa Terms and Abbreviations
 *   -# CSL:  Chip Support Library
 *   -# API:  Application Programmer Interface
 *
 * @subsection References
 *    -# CSL-001-DES, CSL 3.x Design Specification DocumentVersion 1.02
 *
 */

/** \file  csl_ddr.h
    \brief DDR functional layer API header file
    \internal
    \date   02 August, 2004
    \author Pratheesh Gangadhar (pratheesh@ti.com)
*/

#ifndef _CSL_DDR_H_
#define _CSL_DDR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <csl_error.h>
#include <csl_sysData.h>
#include <csl_types.h>
#include <cslr_ddr.h>

/**
\defgroup CSL_DDR_API DDR
   \internal Based on <b> External Memory Interface Module Specification 3.0.2</b> 
*/
/**
\defgroup CSL_DDR_DATASTRUCT Data Structures
\ingroup CSL_DDR_API
*/
/**
\defgroup CSL_DDR_DEFINE  Defines
\ingroup CSL_DDR_API
*/
/**
\defgroup CSL_DDR_ENUM  Enumerated Data Types
\ingroup CSL_DDR_API
*/

/**
\defgroup CSL_DDR_FUNCTION  Functions
\ingroup CSL_DDR_API
*/

/*****************************************************************************\
          DDR global macro declarations
\*****************************************************************************/ 

/** \defgroup CSL_DDR_INTR_DEFINE Interrupt Masks
*   \ingroup CSL_DDR_DEFINE
*   
*
* @{ */
#define CSL_DDR_INTR_LINETRAP CSL_FMK (DDR_IRR_LT, 1) /**< Line Trap
                             \n indicate illegal memory access type or invalid 
			      cache line size */
#define CSL_DDR_INTR_PARITYERR CSL_FMK (DDR_IRR_PE, 1) /**< Parity Error
                             \n indicate parity error during SDRAM access */
#define CSL_DDR_INTR_ASYNCTIMEOUT CSL_FMK (DDR_IRR_PE, 1) /**< Asynchronous 
 			          Timeout
                             \n indicate that during an extended asynchronous
			     bank access cycle, the padi_mwait_i signal did not
			     go inactive within the number of cycles defined by
			     the max_ext_wait field in Async Wait Cycle Config 
			     register */
/**
@} */

/** \defgroup CSL_DDR_SDRAMSTAT_DEFINE SDRAM Status
*   \ingroup CSL_DDR_DEFINE
*   
*
* @{ */
#define CSL_DDR_SDRAMSTAT_BIGENDIAN CSL_FMK (DDR_SDRSTAT_BE, 1)
	                            /**< Big Endian */
#define CSL_DDR_SDRAMSTAT_DUALCLK CSL_FMK (DDR_SDRSTAT_DCLKMD, 1)
                                   /**< Dual Clock Mode */
#define CSL_DDR_SDRAMSTAT_FASTINIT CSL_FMK (DDR_SDRSTAT_FASTINIT, 1)
                                   /**< Fast init mode*/	
#define CSL_DDR_SDRAMSTAT_DDRPHYRDY CSL_FMK (DDR_SDRSTAT_PHYRDY, 1)
                                   /**< DDR PHY Ready */

/**
@} */


/** \defgroup CSL_DDR_BANCONFIG_DEFINE Bank Config Control Masks
*   \ingroup CSL_DDR_DEFINE
*   
*
* @{ */
#define CSL_DDR_BANKCONFIG_BOOTUNLOCK CSL_FMK (DDR_SDBCR_BOOTUNLOCK, 1)
	            /**< Boot Unlock
		       \n Set to 1 to change the values of the
		       fields that are affected by the boot_unlock bit */
#define CSL_DDR_BANKCONFIG_DDR2TERM_75OHM CSL_FMKT (DDR_SDBCR_DDR2TERM, TERM75)
	            /**< DDR2 Termination Register Value = 75ohm
                       This bit is writeable only when boot_unlock bit is 
		       unlocked */
#define CSL_DDR_BANKCONFIG_DDR2TERM_150OHM \
	                                 CSL_FMKT (DDR_SDBCR_DDR2TERM, TERM150)
	            /**< DDR2 Termination Register Value = 150ohm
                       This bit is writeable only when boot_unlock bit is
		       unlocked */
#define CSL_DDR_BANKCONFIG_DDR2_ENABLE CSL_FMK (DDR_SDBCR_DDR2_EN, 1)
                    /**< DDR2 Enable
                       \n Reflects the value on the ddr2_def_enable port that
		       defines whether the DDR2 mode has been enabled. This bit
		       is writeable only when the boot_unlock bit is unlocked.
		       This bit is only valid when sdram_enable and ddr_enable
		       are set to 1 */
#define CSL_DDR_BANKCONFIG_DISABLE_DLL CSL_FMK (DDR_SDBCR_DISDDRDLL, 1)
		    /**< Disable DLL select for DDR SDRAM
                       \n Set to 1 to disable DLL inside DDR SDRAM. This bit is
		       writeable only when the boot_unlock bit is unlocked */

#define CSL_DDR_BANKCONFIG_DDRDRIVE_NORMAL CSL_FMKT (DDR_SDBCR_DDR_DR, NORM)
		    /**< DDR SDRAM Drive Strength = Normal
		       \n A write will cause EMIF to start SDRAM initialization
		       sequence. This bit is writeable only when the
		       boot_unlock bit is unlocked */
#define CSL_DDR_BANKCONFIG_DDRDRIVE_WEAK CSL_FMKT (DDR_SDBCR_DDR_DR, WEAK)
		    /**< DDR SDRAM Drive Strength = Weak
		       \n A write will cause EMIF to start SDRAM initialization
		       sequence. This bit is writeable only when the
		       boot_unlock bit is unlocked */
#define CSL_DDR_BANKCONFIG_DDR_ENABLE CSL_FMK (DDR_SDBCR_DDR_EN, 1)
	            /**< Double Rate
		      \n Reflects the value on the ddr_def_enable port (active
		      high) that defines whether the EMIF is set to operate at
		      double or single data rate. This bit is writeable only
		      when the boot_unlock bit is unlocked. This bit is only
		      valid when sdram_enable is set to 1 */
#define CSL_DDR_BANKCONFIG_SDRAM_ENABLE CSL_FMK (DDR_SDBCR_SDR_EN, 1)
	            /**< SDRAM Enable
		       Reflects the value on the sdram_def_enable port (active
                       high) that defines whether the SDRAM mode in the EMIF is
		       enabled. This bit is writeable only when the boot_unlock
		       bit is unlocked. When set to 0, EMIF will disable SDRAM
                      initialization and refreshes, but will carry out SDRAM
		      write/read transactions. This bit must not be set to 0
		      when EMIF is in Self Refresh state */						 
						 

/**
@} */

/**************************************************************************\
*         DDR global typedef declarations                                  * 
\**************************************************************************/

/**\defgroup CSL_DDR_MEMSEL_ENUM DDR Memory Select 
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for Memory select
* 
*/
typedef enum {
  CSL_DDR_ASYNC_MEM   = 0, /**< <b>: Asynchronous Memory Select </b> */
  CSL_DDR_SYNC_SBSRAM = 1 /**< <b>: Synchronous Burst SRAM </b> */
} CSL_DdrMemSelect;
/**
@} */

/**\defgroup CSL_DDR_TIMUNLOCKCTL_ENUM Timing Unlock Control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for enable/disable write to timing control registers
* 
*/
typedef enum {
  CSL_DDR_TIMUNLOCKCTL_DISABLE  =  0,
                           /**< <b>: Write to timing registers disabled </b> */
  CSL_DDR_TIMUNLOCKCTL_ENABLE   =  1
	                   /**< <b>: Write to timing registers disabled </b> */
} CSL_DdrTimingUnlockCtrl;


/**
@} */

/**\defgroup CSL_DDR_NMCTL_ENUM Narrow mode control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for Narrow mode control
*   \n A write to this field will cause the EMIF to start the SDRAM
*   initialization sequence
* 
*/
typedef enum {
  CSL_DDR_NARROWMODE_DISABLE = 0,
                            /**< <b>: All 32 bits are used, system bus width to
			      memory bus width is 1:1 for SDR SDRAM and 2:1 for
			      DDR SDRAM </b> */
  CSL_DDR_NARROWMODE_ENABLE = 1
	                    /**< <b>: Only 16 bits are used, system buswidth to
                     		memory buswidth is 2:1 for SDR and 4:1 for DDR 
				</b> */
} CSL_DdrNarrowModeCtrl;
/**
@} */



/**\defgroup CSL_DDR_CASLAT_ENUM CAS Latency
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for CAS Latency
* 
*/
typedef enum {
  CSL_DDR_CASLAT2 = 2, /**< <b>: CAS Latency equal to 2 </b> */
  CSL_DDR_CASLAT3 = 3, /**< <b>: CAS Latency equal to 3 </b> */
  CSL_DDR_CASLAT4 = 4, /**< <b>: CAS Latency equal to 4 </b> */
  CSL_DDR_CASLAT5 = 5  /**< <b>: CAS Latency equal to 5 </b> */
} CSL_DdrCasLatency;
/**
@} */

/**\defgroup CSL_DDR_INTBANK_ENUM Internal SDRAM Bank Setup
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for defining number of banks inside connected SDRAM devices
* 
*/
typedef enum {
  CSL_DDR_INTERN_BANK1 = 0, /**< <b>: 1 bank SDRAM devices </b> */
  CSL_DDR_INTERN_BANK2 = 1, /**< <b>: 2 bank SDRAM devices </b> */
  CSL_DDR_INTERN_BANK4 = 2, /**< <b>: 4 bank SDRAM devices </b> */
  CSL_DDR_INTERN_BANK8 = 3  /**< <b>: 8 bank SDRAM devices </b> */
} CSL_DdrInternSdramBank;
/**
@} */

/**\defgroup CSL_DDR_EXTBANK_ENUM External SDRAM Bank Setup
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for defining chip select lines to use for SDRAM bank accesses
* 
*/
typedef enum {
  CSL_DDR_EXTBANK_USE_CS0     = 0, /**< <b>: Use CS0 for all SDRAM accesses
				      </b> */
  CSL_DDR_EXTBANK_USE_CS0_CS1 = 1  /**< <b>: Use CS0 and CS1 for all SDRAM 
				      accesses </b> */
} CSL_DdrExternSdramBank;
/**
@} */

/**\defgroup CSL_DDR_PAGESIZE_ENUM Internal Page Size
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for defining chip select lines to use for SDRAM bank accesses
* 
*/
typedef enum {
  CSL_DDR_SDRAM_PAGE256  =  0, /**< <b>: 256 word pages requiring 8 column
				           address bits </b> */
  CSL_DDR_SDRAM_PAGE512  =  1,  /**< <b>: 512 word pages requiring 9 column
				           address bits </b> */
  CSL_DDR_SDRAM_PAGE1024 =  2, /**< <b>: 1024 word pages requiring 10 column
				           address bits </b> */
  CSL_DDR_SDRAM_PAGE2048 =  3  /**< <b>: 2048 word pages requiring 11 column
				           address bits </b> */
} CSL_DdrSdramPageSize;
/**
@} */


/**\defgroup CSL_DDR_STRBCTL_ENUM Strobe Mode Select
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for defining strobe mode select
* 
*/
typedef enum {
  CSL_DDR_STROBE_DISABLE  =  0, /**< <b>: Strobe timing disable </b> */
  CSL_DDR_STROBE_ENABLE   =  1  /**< <b>: Chip selects need to have read or
                                          write strobe timing </b> */
} CSL_DdrStrobeCtrl;


/**
@} */

/**\defgroup CSL_DDR_WESTRBCTL_ENUM WE Strobe Mode Select
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for selecting WE strobe mode
* 
*/
typedef enum {
  CSL_DDR_WESTROBE_DISABLE  =  0, /**< <b>: Strobe timing disable </b> */
  CSL_DDR_WESTROBE_ENABLE   =  1  /**< <b>: pado_mdqm_o_n[7/3:0] output pins 
                             will act, as active low byte write enables when
			     accessing chip select N </b> */
} CSL_DdrWeStrobeCtrl;


/**
@} */

/**\defgroup CSL_DDR_EWCTL_ENUM Extended Wait Mode
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for selecting extended wait mode
* 
*/
typedef enum {
  CSL_DDR_EXTWAIT_DISABLE  =  0, /**< <b>: Extended wait disable </b> */
  CSL_DDR_EXTWAIT_ENABLE   =  1  /**< <b>: Extended asynchronous cycles are
 	                    required based on padi_mwait_i </b> */
} CSL_DdrExtWaitCtrl;

/**
@} */

/**\defgroup CSL_DDR_BUSWIDTH_ENUM Databus Width
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums define the width of data bus
* 
*/
typedef enum {
  CSL_DDR_BUSWIDTH_8BIT  =  0, /**< <b>: 8 bit databus </b> */
  CSL_DDR_BUSWIDTH_16BIT =  1, /**< <b>: 16 bit databus </b> */
  CSL_DDR_BUSWIDTH_32BIT =  2, /**< <b>: 32 bit databus </b> */
  CSL_DDR_BUSWIDTH_64BIT =  3  /**< <b>: 64 bit databus </b> */
} CSL_DdrBuswidth;

/**
@} */

/**\defgroup CSL_DDR_VBUSM_WIDTH_ENUM VBUSM Width
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums define the width of VBUSM
* 
*/
typedef enum {
  CSL_DDR_VBUSM_WIDTH_32BIT =  0, /**< <b>: 32 bit databus </b> */
  CSL_DDR_VBUSM_WIDTH_64BIT =  1  /**< <b>: 64 bit databus </b> */
} CSL_DdrVbusmWidth;

/**
@} */

/**\defgroup CSL_DDR_CSEXT_ENUM SBSRAM bank chip select extend
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums define the behaviour of the pad_cs_o[n] pin associated with
 *          the SBSRAM bank, as follows
 *
* 
*/
typedef enum {
  CSL_DDR_CSEXT_DISABLE  =  0, /**< <b>: pad_cs_o[n] goes inactive after final
				        command has been issued </b> */
  CSL_DDR_CSEXT_ENABLE   =  1  /**< <b>:  pad_cs_o[n] goes active low when 
 			   pado_moe_o_n goes active low and will stay active 
			   low until pado_moe_o_n is inactive.The pado_moe_o_n
			   timing is controlled by the read latncy value for
			   the SBSRAM bank. Used for synchronous FIFO reads 
			   </b> */
} CSL_DdrCsExtend;

/**
@} */


/**\defgroup CSL_DDR_RDENCTL_ENUM SBSRAM bank read enable mode
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums define the behaviour of the pad_cs_o[n] pin associated with
 *          the SBSRAM bank, as follows
 *
* 
*/
typedef enum {
  CSL_DDR_ADS_MODE      =  0, /**< <b>: ADS mode </b> */
  CSL_DDR_READEN_MODE   =  1  /**< <b>: Read Enable mode </b> */
} CSL_DdrRdEnCtrl;

/**
@} */

/**\defgroup CSL_DDR_LATENCY_ENUM SBSRAM Latency
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums define the SBSRAM bank's read/write latency in clock cycles 
* 
*/
typedef enum {
  CSL_DDR_LATENCY_CYCLE0      =  0, /**< <b>: 0 cycle latency </b> */
  CSL_DDR_LATENCY_CYCLE1      =  1, /**< <b>: 1 cycle latency </b> */
  CSL_DDR_LATENCY_CYCLE2      =  2, /**< <b>: 2 cycle latency </b> */
  CSL_DDR_LATENCY_CYCLE3      =  3  /**< <b>: 3 cycle latency </b> */
} CSL_DdrLatency;

/**
@} */

/**\defgroup CSL_DDR_SELFREFRCTL_ENUM Self Refresh Control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for enabling/disabling self refresh 
* 
*/
typedef enum {
  CSL_DDR_SELF_REFRESH_DISABLE  =  0, /**< <b>: Self refresh disable </b> */
  CSL_DDR_SELF_REFRESH_ENABLE   =  1  /**< <b>: Cause connected SDRAM devices to
				be place into Self Refresh power-down mode and
				the EMIF to enter the Self Refresh state. In 
				this state the EMIF will service all 
				asynchronous bank accesses immediately but any 
				SDRAM access will take at least 16 cycles due 
				to the time required for the SDRAM devices to
				out of Self Refresh mode. If an SDRAM access 
				immediately follows the setting of the sr bit,
				the access will take 31 cycles </b> */
} CSL_DdrSelfRefrCtrl;

/**
@} */


/**\defgroup CSL_DDR_PARITYCTL_ENUM Parity Control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief  Enums for enabling/disabling error detection
* 
*/
typedef enum {
  CSL_DDR_PARITY_DISABLE  =  0, /**< <b>: Parity disable </b> */
  CSL_DDR_PARITY_ENABLE  =  1  /**< <b>: Parity enable </b> */
} CSL_DdrParityCtrl;

/**
@} */

/**\defgroup CSL_DDR_WAITPOLCTL_ENUM Wait Polarity Control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief Defines the polarity of the padpadi_mwait_i port
*
* 
*/
typedef enum {
  CSL_DDR_WAITPOL_LOW   =  0, /**< <b>: Wait if padpadi_mwait_i port is low
				  </b> */
  CSL_DDR_WAITPOL_HIGH  =  1  /**< <b>: Wait if padpadi_mwait_i port is high
				  </b> */
} CSL_DdrWaitPolCtrl;

/**
@} */


/**\defgroup CSL_DDR_MIDFILTCTL_ENUM Master Id Filter Control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief Enables/disables VBUS master Id input for performance monitor region
*          select 
*
* 
*/
typedef enum {
  CSL_DDR_MIDFILT_DISABLE   =  0, /**< <b>: VBUS master id is not used </b> */
  CSL_DDR_MIDFILT_ENABLE  =  1  /**< <b>: VBUS master id is used </b> */
} CSL_DdrMidFiltCtrl;

/**
@} */

/**\defgroup CSL_DDR_CSFILTCTL_ENUM Chip Select Filter Control
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief Enables/disables chip select input for performance monitor region
*            select
*
* 
*/
typedef enum {
  CSL_DDR_CSFILT_DISABLE   =  0, /**< <b>: Chip select is not used </b> */
  CSL_DDR_CSFILT_ENABLE  =  1  /**< <b>: Chip select is used </b> */
} CSL_DdrCsFiltCtrl;

/**
@} */

/**\defgroup CSL_DDR_FILTCFG_ENUM Filter Configurations
*  \ingroup CSL_DDR_ENUM
*
*  
@{*/

/** \brief Defines performance monitoring filters
*
* 
*/
typedef enum {
  CSL_DDR_TOT_SDRAM_ACCESS   =  0, /**< <b>: Total SDRAM accesses </b> */
  CSL_DDR_TOT_SDRAM_ACTIVATE =  1, /**< <b>: Total SDRAM activates </b> */
  CSL_DDR_TOT_READ           =  2, /**< <b>: Total reads </b> */
  CSL_DDR_TOT_WRITE          =  3, /**< <b>: Total writes </b> */
  CSL_DDR_VBUSM_CMDFIFO_FULL  =  4, /**< <b>: Number of cycles VBUSM Command 
                                             FIFO is full </b> */ 
  CSL_DDR_VBUSM_WRFIFO_FULL  =   5, /**< <b>: Number of cycles VBUSM Write Data
                                             FIFO is full </b> */
  CSL_DDR_VBUSM_RDFIFO_FULL  =   6, /**< <b>: Number of cycles VBUSM Read Data
                                             FIFO is full </b> */
  CSL_DDR_VBUSM_WRSTATFIFO_FULL = 7, /**< <b>: Number of cycles VBUSM Wirte
                                             Status FIFO is full </b> */
  CSL_DDR_TOT_PARITY_ERROR = 8, /**< <b>: Total parity errors </b> */ 
  CSL_DDR_TOT_PRIO_ELEVATION = 9, /**< <b>: Total priority elevations </b> */
  CSL_DDR_TOT_PRECH_ELEVATION = 10,/**< <b>: Total precharge elevations </b> */
  CSL_DDR_CMD_PENDING = 11/**< <b>: Number of cycles command pending  </b> */
                                                                                                                               
} CSL_DdrFiltConfig;

/**
@} */







/**\defgroup CSL_DDR_CONTROLCMD_ENUM  Control Commands 
*  \ingroup CSL_DDR_CONTROL_API
@{*/
/** \brief Enumeration for control commands passed to \a CSL_ddrHwControl()
*
* This is the set of commands that are passed to the \a CSL_ddrHwControl()
* with an optional argument type-casted to \a void* .
* The arguments to be passed with each enumeration (if any) are specified
* next to the enumeration
*/
typedef enum {
  CSL_DDR_CMD_INTR_ENABLE,/**< \brief Enable interrupts specified by input
			              bitmask
                               \param CSL_BitMask32
                               \return CSL_SOK 
			       \sa CSL_DDR_INTR_DEFINE */
  CSL_DDR_CMD_INTR_DISABLE,/**< \brief Disable interrupts specified by input
			               bitmask
                                \param CSL_BitMask32
                                \return CSL_SOK 
				\sa CSL_DDR_INTR_DEFINE */
  CSL_DDR_CMD_CLEAR_INTRSTATUS,/**< \brief Clear interrupt status as specified
				           by input bitmask 
                                    \param CSL_BitMask32
                                    \return CSL_SOK
				    \sa CSL_DDR_INTR_DEFINE */
  CSL_DDR_CMD_DDRPHY_CONTROL,/**< \brief  Used to control the DDR PHY, The bit 
			          field definitions are DDR PHY specific
                                 \param Uint32
                                 \return CSL_SOK */
  CSL_DDR_CMD_VTPIO_CONTROL/**< \brief This register is used to control the IOs
			         such as the VTP calibration of the IOs. The
				 bit field definitions are IO specific and
				 should be described in the IO specification
                                 \param Uint32
                                 \return CSL_SOK */
	  
                                   
} CSL_DdrHwControlCmd;
/**
@} */


/**\defgroup CSL_DDR_QUERYCMD_ENUM Query Commands 
*  \ingroup CSL_DDR_QUERY_API
* @{ */

/** \brief Enumeration for queries passed to \a CSL_DdrGetHwStatus()
*
* This is used to get the status of different operations.The arguments
* to be passed with each enumeration if any are specified next to 
* the enumeration */
typedef enum {
  CSL_DDR_QUERY_PID,/**< \brief Queries Module ID and revision of DDR module
                         \param (Uint32*) 
                         \return CSL_SOK */
  CSL_DDR_QUERY_DDRPHY_PID,/**< \brief Queries Module ID and revision of 
			         DDR PHY being used, the bit field definitions
				 are DDR PHY specific
                                \param (Uint32*) 
                                \return CSL_SOK */
  CSL_DDR_QUERY_SDRAM_STATUS,/**< \brief Queries the SDRAM status 
                                  \param (CSL_BitMask32*) 
                                  \return CSL_SOK 
				  \sa CSL_DDR_SDRAMSTAT_DEFINE */
  CSL_DDR_QUERY_DDRPHY_STATUS,/**< \brief Queries the status of DDR PHY, the
				   bit field definitions are DDR PHY specific
                                   \param (Uint32*) 
                                   \return CSL_SOK */
  CSL_DDR_QUERY_INTR_STATUS,/**< \brief Queries interrupt status
                                 \param (CSL_BitMask32*) 
                                 \return CSL_SOK 
				 \sa CSL_DDR_INTR_DEFINE */
  CSL_DDR_QUERY_INTMSKD_STATUS,/**< \brief Queries interrupt status after
                                           applying mask 
                                    \param (CSL_BitMask32*) 
                                    \return CSL_SOK 
				    \sa CSL_DDR_INTR_DEFINE */
  CSL_DDR_QUERY_PERFCOUNT1,/**< \brief Queries performance counter1 
                                \param (Uint32*) 
                                \return CSL_SOK */
  CSL_DDR_QUERY_PERFCOUNT2,/**< \brief Queries performance counter2
                               \param (Uint32*) 
                               \return CSL_SOK */
  CSL_DDR_QUERY_VTPIO_STATUS/**< \brief Queries the status of IOs such as the
                                 VTP calibration status of the IOs. The bit
				 field definitions are IO specific and should
				 be described in the IO specification
                                 \param (Uint32*) 
                                 \return CSL_SOK */
} CSL_DdrHwStatusQuery;

/**
@} */



/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for performance monitoring
*
*   Used to configure performance counters and regions
*/

typedef struct CSL_DdrPerfCountConfig {
  CSL_DdrMidFiltCtrl count1MidEn;
  Uint16             count1MasterId; /**< Master ID for Performance Counter1
                                          (0-63) */
  CSL_DdrMidFiltCtrl count2MidEn;
  Uint16             count2MasterId;/**< Master ID for Performance Counter2
                                          (0-63) */

  CSL_DdrCsFiltCtrl  count1CsEn;
  Uint16             count1ChipSel;/**< Chip select for Performance Counter2
                                         (0-7) */

  CSL_DdrCsFiltCtrl  count2CsEn;
  Uint16             count2ChipSel;/**< Chip select for Performance Counter2
                                          (0-7) */

  CSL_DdrFiltConfig  count1Filter;
  CSL_DdrFiltConfig  count2Filter;
  
} CSL_DdrPerfCountConfig;
/**
@} */

/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for SDRAM bank configuration 
*
*   Used to configure SDRAM bank, CAS latency, buswidth etc
*/

typedef struct CSL_DdrSdramBankConfig {
  CSL_BitMask32           bootUnlockConfig;/**< Specify the fields that can be
					      updated by setting boot unlock
					      field 
					      \sa CSL_DDR_BANCONFIG_DEFINE */
  CSL_DdrTimingUnlockCtrl timingControl;
  CSL_DdrNarrowModeCtrl   narrowModeCtrl;
  CSL_DdrCasLatency       casLatency;
  CSL_DdrInternSdramBank  internBank;
  CSL_DdrExternSdramBank  externBank;
  CSL_DdrSdramPageSize    pageSize;
} CSL_DdrSdramBankConfig;

/**
@} */

/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for VBUSM burst priority 
*
*   Used to configure VBUSM burst priority
*/

typedef struct CSL_DdrVbusmBurstPriority {
  Uint16 priOldCount; /**< Priority Raise Old Counter:(0-255) Number of memory 
			   bursts after which the EMIF raises the priority
			   of old commands in the VBUSM command FIFO */
} CSL_DdrVbusmBurstPriority;
/**
@} */


/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for VBUSM data bus configuration
*
*   Used to configure VBUSM data bus configuration
*/

typedef struct CSL_DdrVbusmConfig {
  CSL_DdrVbusmWidth   busWidth;
  Uint16              statFifoDepth; /**< Write Status FIFO depth (0-255) */
  Uint16              writeFifoDepth; /**< Write FIFO depth (0-255) */
  Uint16              cmdFifoDepth; /**< Command FIFO depth (0-255) */
  Uint16              regReadFifoDepth; /**< Async/ register read FIFO depth
                                           (0-255) */
  Uint16              sdramFifoDepth; /**< SDRAM read FIFO depth (0-255) */
  Uint16              readCmdFifoDepth; /**<Read command FIFO depth (0-255) */
} CSL_DdrVbusmConfig;
/**
@} */



/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for Asynchronous memory access
*
*   Used to select access types as well as configure the access parameters
*/

typedef struct CSL_DdrAsyncMemConfig {
  CSL_DdrStrobeCtrl    strobeSel;
  CSL_DdrWeStrobeCtrl  weStrbSel;
  CSL_DdrExtWaitCtrl   extWait;
  Uint16               wrSetup;/**< Write Strobe Setup cycles (1-16)
                     \n Number of EM_CLK cycles from pado_ma_o, pado_mba_o, 
		        pad_d_o, pado_mdqm_o_n, and pado_mcs_o_n being set to
                        pado_mwe_o_n asserted, minus one cycle */
  Uint16               wrStrobe;/**< Write Strobe Duration cycles (1-64)
                     \n Number of EM_CLK cycles for which pado_mwe_o_n is held
		        active, minus one cycle */ 
  Uint16               wrHold;/**< Write Strobe Hold cycles (1-8)
                     \n Number of EM_CLK cycles for which pado_ma_o,
		        pado_mba_o, pad_d_o, pado_mdqm_o_n, and pado_mcs_o_n
			are held after pado_mwe_o_n has been deasserted, minus
                        one cycle */
  Uint16               rdSetup;/**< Read Strobe Setup cycles (1-16)
                     \n Number of EM_CLK cycles from pado_ma_o, pado_mba_o, 
		        pad_d_o, pado_mdqm_o_n, and pado_mcs_o_n being set to
                        pado_moe_o_n asserted, minus one cycle */
  Uint16               rdStrobe;/**< Read Strobe Duration cycles (1-64)
                     \n Number of EM_CLK cycles for which pado_moe_o_n is held
		        active, minus one cycle */ 
  Uint16               rdHold;/**< Read Strobe Hold cycles (1-8)
                     \n Number of EM_CLK cycles for which pado_ma_o,
		        pado_mba_o, pad_d_o, pado_mdqm_o_n, and pado_mcs_o_n
			are held after pado_moe_o_n has been deasserted, minus
                        one cycle */ 
  CSL_DdrBuswidth      asize;/**< Asynchronous Bank Size */
  
} CSL_DdrAsyncMemConfig;
/**
@} */


/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for Synchronous burst SRAM access
*
*   Used to select access types as well as configure the access parameters
*/

typedef struct CSL_DdrSyncBurstSramConfig {
  CSL_DdrCsExtend csExt;
  CSL_DdrRdEnCtrl rdEnMode;
  CSL_DdrLatency  wrLat;
  CSL_DdrLatency  rdLat;
  CSL_DdrBuswidth sbsize;
} CSL_DdrSyncBurstSramConfig;
/**
@} */


/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for SDRAM refresh control
*
*   Used to define the refresh rate 
*/

typedef struct CSL_DdrSdramRefreshControl {
  CSL_DdrSelfRefrCtrl  selfRefresh;
  CSL_DdrParityCtrl    parityEn;
  Uint16               refreshRate;/**< Define the rate at which connected 
 	 	       SDRAM devices are refreshed (0-65535) \n 
		       SDRAM refresh rate = EMIF rate(mclk rate)/refreshRate 
                       \n Writing a value < 0x0010 to this field will cause 
                        it to be loaded with the refresh_def_val port value */

} CSL_DdrSdramRefreshControl;
/**
@} */

/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for SDRAM timing register
*
*   Used to define the timing of SDRAM operations
*/

typedef struct CSL_DdrSdramTiming {
  Uint16  rfc;/**< Minimum number of EM_CLK cycles from Refresh or Load Mode
                 to Refresh or Activate, minus one (1-32) */
  Uint16  rp;/**< Minimum number of EM_CLK cycles from Precharge to
                Activate or Refresh, minus one (1-8) */
  Uint16  rcd;/**< Minimum number of EM_CLK cycles from Activate to Read or
	             Write, minus one (1-8) */
  Uint16  wr;/**< Minimum number of EM_CLK cycles from last Write transfer to
	            Precharge, minus one (1-8) */
  Uint16  ras;/**< Minimum number of EM_CLK cycles from Activate to Precharge,
                 minus one (1-16) */
  Uint16  rc;/**< Minimum number of EM_CLK cycles from Activate to Activate,
	            minus one (1-16) */
  Uint16  rrd;/**< Minimum number of EM_CLK cycles from Activate to Activate
                 for a different bank, minus one (1-8) */
  Uint16  wtr;/**< Minimum number of EM_CLK cycles from last DDR Write
                 Transfer to DDR Read, minus one (1-4) */
  Uint16  odt;/**< Minimum number of EM_CLK cycles from ODT enable to write
                 data driven, minus one (1-4) */
  Uint16  sxnr;/**< Minimum number of EM_CLK cycles from Self-Refresh exit to
                  any command other than a Read command, minus one (1-32) */
  Uint16  sxrd;/**< Minimum number of EM_CLK cycles from Self-Refresh exit to
                  a Read command, minus one (1-256) */
  Uint16  rtp;/**< Minimum number of EM_CLK cycles from the last Read command
                 to a Pre-charge command, minus one (1-8) */
  Uint16  cke;/**< Minimum number of EM_CLK cycles between pado_mcke_o 
                 changes, minus one (1-32) */
                  
} CSL_DdrSdramTiming;
/**
@} */


/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Configuration parameters for Async wait cycle configuration
*
*   Used to configure parameters like wait polarity, turn around cycles etc
*/

typedef struct CSL_DdrAsyncWaitConfig {
  CSL_DdrWaitPolCtrl waitPolarity;
  Uint16             turnAround;/**< Turn Around cycles (1-8)
				 \n Number of EM_CLK cycles between the end of
				 one asynchronous bank access and the start of
				 another asynchronous bank access, minus one
				 cycle
				 \n This delay is not incurred between a read 
				 followed by a read, or a write followed by a
				 write to the same bank */

  Uint16             maxExtWait;/**< Maximum External Wait cycles (0-255)
 			         \n Defines the number of 16 EMIF cycle periods
                                the EMIF will wait for an extended asynchronous
				cycle before the cycle is terminated */
} CSL_DdrAsyncWaitConfig;
/**
@} */


/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/**
   \brief Input parameters for setting up DDR
*
*  Used to put DDR into known useful state
*
*  \sa CSL_DdrAsyncMemConfig, CSL_DdrSyncBurstSramConfig 
*/
typedef struct CSL_DdrHwSetup {
  CSL_DdrSdramBankConfig     bankConfig;
  CSL_DdrSdramRefreshControl refreshCtrl;
  CSL_DdrSdramTiming         sdramTiming;
  CSL_DdrAsyncWaitConfig     asyncWait; 
  CSL_DdrVbusmConfig         vbusmConfig; 
  CSL_DdrVbusmBurstPriority  burstPriority;
  CSL_DdrPerfCountConfig     perfConfig;
  CSL_DdrMemSelect           memSel2;
  void                       *configCS2;/**< Pointer to CSL_DdrAsyncMemConfig
                             or CSL_DdrSyncBurstSramConfig based on the memSel2 */
  CSL_DdrMemSelect           memSel3;
  void                       *configCS3;/**< Pointer to CSL_DdrAsyncMemConfig
                             or CSL_DdrSyncBurstSramConfig based on the memSel3 */  					  
  CSL_DdrMemSelect           memSel4;
  void                       *configCS4;/**< Pointer to CSL_DdrAsyncMemConfig
                             or CSL_DdrSyncBurstSramConfig based on the memSel4 */  					  
  CSL_DdrMemSelect           memSel5;
  void                       *configCS5;/**< Pointer to CSL_DdrAsyncMemConfig
                             or CSL_DdrSyncBurstSramConfig based on the memSel5 */  					  
  void                       *extendSetup;
} CSL_DdrHwSetup;
/**
@} */

/** 
\addtogroup CSL_DDR_DATASTRUCT 
@{
*/

/** \brief This object contains the reference to the instance of DDR
* opened using the @a CSL_ddrOpen()
*
* The pointer to this is passed to all DDR CSL APIs
* This structure has the fields required to configure DDR for any test
* case/application. It should be initialized as per requirements of a
* test case/application and passed on to the setup function
*/
typedef struct CSL_DdrObj {
  CSL_OpenMode openMode;/**< This is the mode in which the CSL instance
                             is opened */
  CSL_Uid      uid;/**< This is a unique identifier to the instance of DDR 
                        being referred to by this object */
  CSL_Xio      xio;/**< This is the variable that holds the current state
                        of a resource being shared by current instance of
                        DDR with other peripherals */
  CSL_DdrRegsOvly regs;/**< This is a pointer to the registers of the  
                             instance of DDR referred to by this object */
  CSL_DdrNum  ddrNum;/**< This is the instance of DDR being referred to
                           by this object */
} CSL_DdrObj;

/** \brief this is a pointer to @a CSL_DdrObj and is passed as the first
* parameter to all DDR CSL APIs */
typedef CSL_DdrObj* CSL_DdrHandle; 
/**
@} */




/*****************************************************************************\
          CSL3.x mandatory function prototype definitions
\*****************************************************************************/
/**
\defgroup CSL_DDR_INIT_API DDR Init API
\ingroup CSL_DDR_FUNCTION
    \brief Initializes the DDR
@{*/
CSL_Status CSL_ddrInit ( void );
/**
@} */

/**
\defgroup CSL_DDR_OPEN_API DDR Open API
\ingroup CSL_DDR_FUNCTION
    \brief Opens if possible the instance of DDR requested
    \n  Reserves the specified ddr for use. The device can be 
    re-opened anytime after it has been normally closed if so
    required. The handle returned by this call is input as an
    essential argument for rest of the APIs described for this
    module.

    <b> Usage Constraints: </b>
    The CSL system as well as DDR must be succesfully initialized
    via @a CSL_sysInit() and @a CSL_ddrInit() before calling this
    function. Memory for the @a CSL_DdrObj must be allocated outside
    this call. This object must be retained while usage of this peripheral.
  
    \b Example:
    \verbatim


    CSL_DdrObj     ddrObj;
    CSL_DdrHwSetup ddrSetup;
    CSL_Status       status;
    ...
    hDdr = CSL_ddrOpen(&ddrobj,
                         CSL_DDR_0,
                         CSL_EXCLUSIVE,
                         &ddrSetup,
                         &status);
    \endverbatim
  
@{*/
CSL_DdrHandle CSL_ddrOpen ( CSL_DdrObj *hDdrObj,
                            CSL_DdrNum ddrNum,
                            CSL_OpenMode openMode,
                            CSL_DdrHwSetup *hwSetup,
                            CSL_Status *status );
/**
@} */

/**
\defgroup CSL_DDR_CLOSE_API DDR Close API
\ingroup CSL_DDR_FUNCTION
    \brief Closes the instance of DDR requested

    
@{*/
CSL_Status  CSL_ddrClose ( CSL_DdrHandle hDdr );
/**
@} */


/**
\defgroup CSL_DDR_SETUP_API DDR Setup API
\ingroup CSL_DDR_FUNCTION
    \brief Programs the DDR with the setup configuration as specified in
           the arguments
   		  
	   
@{*/
CSL_Status CSL_ddrHwSetup ( CSL_DdrHandle hDdr,
                            CSL_DdrHwSetup *setup );
/**
@} */

/**
\defgroup CSL_DDR_GETSETUP_API DDR GetSetup API
\ingroup CSL_DDR_FUNCTION
    \brief Reads the DDR setup configuration   		  
	   
@{*/
CSL_Status CSL_ddrGetHwSetup ( CSL_DdrHandle hDdr,
                            CSL_DdrHwSetup *setup );
/**
@} */


/**
\defgroup CSL_DDR_CONTROL_API DDR Control API
\ingroup CSL_DDR_FUNCTION
    \brief Controls the different operations that can be performed by DDR 
 		  
     		  
    
@{*/
CSL_Status CSL_ddrHwControl ( CSL_DdrHandle hDdr,
                              CSL_DdrHwControlCmd cmd,
                              void *arg );
/**
@} */

/**
\defgroup CSL_DDR_QUERY_API DDR Query API
\ingroup CSL_DDR_FUNCTION
    \brief Returns the status of the requested operation on DDR
@{*/

CSL_Status CSL_ddrGetHwStatus ( CSL_DdrHandle hDdr,
                                CSL_DdrHwStatusQuery query,
                                void *response );
/**
@} */
#ifdef __cplusplus
}
#endif
#endif

/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:08:57 9         2272             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
