/** @file csl_emifHwControl.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_emifHwControl()
 *
 *  Description
 *    - The @a CSL_emifHwControl() function definition & it's associated
 *      functions
 *
 *  Modification 1
 *    - Modified on: 23/6/2004
 *    - Reason: created the sources
 *
 *  @date 23rd June, 2004
 *  @author Santosh Narayanan.
 */
#include <csl_emif.h>

#pragma CODE_SECTION (CSL_emifHwControl, ".text:csl_section:emif");

/** @brief Takes a command with an optional argument & implements it.
 */
CSL_Status  CSL_emifHwControl(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call
	 */
    CSL_EmifHandle                         hEmif,
    /** The command to this API indicates the action to be taken
	 */
    CSL_EmifHwControlCmd                   cmd,
    /** An optional argument @a void* casted
	 */
    void                                     *arg
	){

	CSL_Status status = CSL_SOK;

	switch(cmd){

	case CSL_EMIF_CMD_SET_DDR_REF_THRESH:
		CSL_FINS(hEmif->regs->SDRCR,EMIF_SDRCR_DDRRT,(*(Uint16*)arg) );
		break;

	case CSL_EMIF_CMD_REF_RATE:
		CSL_FINS(hEmif->regs->SDRCR,EMIF_SDRCR_RR,(*(Uint16*)arg) );
		break;

	case CSL_EMIF_CMD_WR_ENABLE:
		CSL_FINS(hEmif->regs->EIMSR, EMIF_EIMSR_WRMSET, (*(Uint16*)arg));
		break;

	case CSL_EMIF_CMD_WR_DISABLE:
		CSL_FINS(hEmif->regs->EIMCR, EMIF_EIMCR_WRMCLR,(*(Uint16*)arg));
		break;

	case CSL_EMIF_CMD_LT_ENABLE:
		CSL_FINS(hEmif->regs->EIMSR, EMIF_EIMSR_LTMSET,TRUE);
		break;

	case CSL_EMIF_CMD_LT_DISABLE:
		CSL_FINS(hEmif->regs->EIMCR, EMIF_EIMCR_LTMCLR,TRUE);
		break;

       case CSL_EMIF_CMD_AT_ENABLE:
		CSL_FINS(hEmif->regs->EIMSR, EMIF_EIMSR_ATMSET,TRUE);
		break;

	case CSL_EMIF_CMD_AT_DISABLE:
		CSL_FINS(hEmif->regs->EIMCR, EMIF_EIMCR_ATMCLR,TRUE);
		break;

	case CSL_EMIF_CMD_IO_CONTROL:
		CSL_FINS(hEmif->regs->IOCTRLR, EMIF_IOCTRLR_IOCTRL, (*(Uint16*)arg));
		break;

       case CSL_EMIF_CMD_CS5_ECC_START:
	   	CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS5ECC, TRUE);
		break;

       case CSL_EMIF_CMD_CS4_ECC_START:
	   	CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS4ECC, TRUE);
		break;

       case CSL_EMIF_CMD_CS3_ECC_START:
	   	CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS3ECC, TRUE);
		break;

	case CSL_EMIF_CMD_CS2_ECC_START:
	   	CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS2ECC, TRUE);
		break;
  
	case CSL_EMIF_CMD_CS5_NAND_ENABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS5NAND, TRUE);
		break;

	case CSL_EMIF_CMD_CS5_NAND_DISABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS5NAND, FALSE);
		break;

       case CSL_EMIF_CMD_CS4_NAND_ENABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS4NAND, TRUE);
		break;

	case CSL_EMIF_CMD_CS4_NAND_DISABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS4NAND, FALSE);
		break;

       case CSL_EMIF_CMD_CS3_NAND_ENABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS3NAND, TRUE);
		break;

	case CSL_EMIF_CMD_CS3_NAND_DISABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS3NAND, FALSE);
		break;

       case CSL_EMIF_CMD_CS2_NAND_ENABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS2NAND, TRUE);
		break;

	case CSL_EMIF_CMD_CS2_NAND_DISABLE:
		CSL_FINS(hEmif->regs->NANDFCR, EMIF_NANDFCR_CS2NAND, FALSE);
		break;

	case CSL_EMIF_CMD_IODFT_SET_TLEC:
		CSL_FINS(hEmif->regs->IODFTECR, EMIF_IODFTECR_TLEC, (*(Uint16*)arg));
		break;

       case CSL_EMIF_CMD_IODFT_MISR_ON:
              CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_MC, (*(Uint16*)0x3));	   	
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_MT, TRUE);
		break;

	case CSL_EMIF_CMD_IODFT_MISR_OFF:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_MT, FALSE);
		break;

	case CSL_EMIF_CMD_IODFT_OPGLD:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_OPGLD, TRUE);
		break;

	case CSL_EMIF_CMD_IODFT_MMS_OP_REG:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_MMS, FALSE);
		break;

	case CSL_EMIF_CMD_IODFT_MMS_INP_CAP:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_MMS, TRUE);
		break;

	case CSL_EMIF_CMD_IODFT_ESEL_TEST:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_ESEL, FALSE);
		break;

       case CSL_EMIF_CMD_IODFT_ESEL_NORMAL:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_ESEL, TRUE);
		break;

       case CSL_EMIF_CMD_IODFT_TOEN_ENABLE:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_TOEN, FALSE);
		break;

       case CSL_EMIF_CMD_IODFT_TOEN_DISABLE:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_TOEN, TRUE);
		break;

	case CSL_EMIF_CMD_IODFT_SET_MISR_STATE:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_MC, (*(Uint16*)arg));
		break;

       case CSL_EMIF_CMD_IODFT_SET_PATTERN_CODE:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_PC, (*(Uint16*)arg));
		break;

	case CSL_EMIF_CMD_IODFT_FUNC_MODE:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_TM, TRUE);
		break;

      	case CSL_EMIF_CMD_IODFT_MODE:
		CSL_FINS(hEmif->regs->IODFTGCR, EMIF_IODFTGCR_TM, FALSE);
		break;

	default:
		status = CSL_ESYS_INVCMD ;
		break;
	};
  return status;

}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:33 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
