/** @file csl_emifGetHwStatus.c
 *
 *    @brief    File for functional layer of CSL API @a CSL_emifGetHwStatus()
 *
 *  Description
 *    - The @a CSL_emifGetHwStatus() function definition & it's associated
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

#pragma CODE_SECTION (CSL_emifGetHwStatus, ".text:csl_section:emif")

/** @brief Gets the status of the different operations.
 */
CSL_Status  CSL_emifGetHwStatus(
    /** Pointer to the object that holds reference to the
     *  instance of EMIF requested after the call 
	 */
    CSL_EmifHandle                         hEmif,
    /** The query to this API which indicates the status 
     *  to be returned 
	 */
    CSL_EmifHwStatusQuery                  query,
    /** Placeholder to return the status. @a void* casted */
    void                                    *response
	){

	CSL_Status status = CSL_SOK;

	switch(query) {

	case CSL_EMIF_QUERY_REV_ID_STATUS:
		((CSL_EmifRevStatus *)response)->moduleID = CSL_FEXT(hEmif->regs->ERCSR, EMIF_ERCSR_MID);
		((CSL_EmifRevStatus *)response)->majorRev = CSL_FEXT(hEmif->regs->ERCSR, EMIF_ERCSR_MAJREV);
	  	((CSL_EmifRevStatus *)response)->minorRev = CSL_FEXT(hEmif->regs->ERCSR, EMIF_ERCSR_MINREV);
		break;

  	case CSL_EMIF_QUERY_ENDIANNESS:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->ERCSR, EMIF_ERCSR_BE);
		break;

	case CSL_EMIF_QUERY_RATE:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->ERCSR, EMIF_ERCSR_FR);
		break;

	case CSL_EMIF_QUERY_DDR_STATUS:
		((CSL_EmifDdrStatus *)response)->phyDllRdy = CSL_FEXT(hEmif->regs->DDRSR, EMIF_DDRSR_PHYDLLRDY);
        ((CSL_EmifDdrStatus *)response)->ddr = CSL_FEXT(hEmif->regs->DDRSR, EMIF_DDRSR_DDR);
		break;

	case CSL_EMIF_QUERY_DDR_PHY_STATUS:
	    *(Uint32 *)response = CSL_FEXT(hEmif->regs->DDRPHYSR, EMIF_DDRPHYSR_DDRPHYSTAT);
		break;

	case CSL_EMIF_QUERY_SDRAM_ACCESS:
		*(Uint32 *)response = CSL_FEXT(hEmif->regs->TOTAR, EMIF_TOTAR_TA);
		break;

	case CSL_EMIF_QUERY_SDRAM_ACTIVATE:
		*(Uint32 *)response = CSL_FEXT(hEmif->regs->TOTACTR, EMIF_TOTACTR_TACT);
		break;

	case CSL_EMIF_QUERY_DDR_ID_REV:
		*(Uint32 *)response = CSL_FEXT(hEmif->regs->DDRPHYID_REV, EMIF_DDRPHYID_REV_DDRPHYID_REV);
		break;

       case CSL_EMIF_QUERY_WR_INT_STATUS:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->EIRR, EMIF_EIRR_WR);
		break;	

	case CSL_EMIF_QUERY_LT_INT_STATUS:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->EIRR, EMIF_EIRR_LT);
		break;	

	case CSL_EMIF_QUERY_AT_INT_STATUS:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->EIRR, EMIF_EIRR_AT);
		break;	

	case CSL_EMIF_QUERY_IO_STATUS:
    		*(Uint16 *)response = CSL_FEXT(hEmif->regs->IOSTATR, EMIF_IOSTATR_IOSTAT);
    		break;	

	case CSL_EMIF_QUERY_NAND_FLASH_STATUS:
    		*(Uint16 *)response = CSL_FEXT(hEmif->regs->NANDFSR, EMIF_NANDFSR_WAITST);
    		break;

	case CSL_EMIF_QUERY_CS5_NAND_FLASH_ECC:
		((CSL_EmifNandFlashECC *)response)->p2048o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P2048O);
		((CSL_EmifNandFlashECC *)response)->p1024o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P1024O);
		((CSL_EmifNandFlashECC *)response)->p512o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P512O);
		((CSL_EmifNandFlashECC *)response)->p256o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P256O);
		((CSL_EmifNandFlashECC *)response)->p128o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P128O);
		((CSL_EmifNandFlashECC *)response)->p64o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P64O);
              ((CSL_EmifNandFlashECC *)response)->p32o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P32O);
		((CSL_EmifNandFlashECC *)response)->p16o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P16O);
		((CSL_EmifNandFlashECC *)response)->p8o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P8O);
		((CSL_EmifNandFlashECC *)response)->p4o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P4O);
		((CSL_EmifNandFlashECC *)response)->p2o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P2O);
		((CSL_EmifNandFlashECC *)response)->p1o = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P1O);
              ((CSL_EmifNandFlashECC *)response)->p2048e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P2048E);
		((CSL_EmifNandFlashECC *)response)->p1024e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P1024E);
		((CSL_EmifNandFlashECC *)response)->p512e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P512E);
		((CSL_EmifNandFlashECC *)response)->p256e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P256E);
		((CSL_EmifNandFlashECC *)response)->p128e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P128E);
		((CSL_EmifNandFlashECC *)response)->p64e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P64E);
              ((CSL_EmifNandFlashECC *)response)->p32e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P32E);
		((CSL_EmifNandFlashECC *)response)->p16e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P16E);
		((CSL_EmifNandFlashECC *)response)->p8e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P8E);
		((CSL_EmifNandFlashECC *)response)->p4e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P4E);
		((CSL_EmifNandFlashECC *)response)->p2e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P2E);
		((CSL_EmifNandFlashECC *)response)->p1e = CSL_FEXT(hEmif->regs->NANDF1ECC, EMIF_NANDF1ECC_P1E);
		break;

    	case CSL_EMIF_QUERY_CS4_NAND_FLASH_ECC:
		((CSL_EmifNandFlashECC *)response)->p2048o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P2048O);
		((CSL_EmifNandFlashECC *)response)->p1024o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P1024O);
		((CSL_EmifNandFlashECC *)response)->p512o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P512O);
		((CSL_EmifNandFlashECC *)response)->p256o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P256O);
		((CSL_EmifNandFlashECC *)response)->p128o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P128O);
		((CSL_EmifNandFlashECC *)response)->p64o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P64O);
              ((CSL_EmifNandFlashECC *)response)->p32o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P32O);
		((CSL_EmifNandFlashECC *)response)->p16o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P16O);
		((CSL_EmifNandFlashECC *)response)->p8o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P8O);
		((CSL_EmifNandFlashECC *)response)->p4o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P4O);
		((CSL_EmifNandFlashECC *)response)->p2o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P2O);
		((CSL_EmifNandFlashECC *)response)->p1o = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P1O);
              ((CSL_EmifNandFlashECC *)response)->p2048e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P2048E);
		((CSL_EmifNandFlashECC *)response)->p1024e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P1024E);
		((CSL_EmifNandFlashECC *)response)->p512e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P512E);
		((CSL_EmifNandFlashECC *)response)->p256e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P256E);
		((CSL_EmifNandFlashECC *)response)->p128e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P128E);
		((CSL_EmifNandFlashECC *)response)->p64e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P64E);
              ((CSL_EmifNandFlashECC *)response)->p32e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P32E);
		((CSL_EmifNandFlashECC *)response)->p16e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P16E);
		((CSL_EmifNandFlashECC *)response)->p8e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P8E);
		((CSL_EmifNandFlashECC *)response)->p4e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P4E);
		((CSL_EmifNandFlashECC *)response)->p2e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P2E);
		((CSL_EmifNandFlashECC *)response)->p1e = CSL_FEXT(hEmif->regs->NANDF2ECC, EMIF_NANDF2ECC_P1E);
		break;

	case CSL_EMIF_QUERY_CS3_NAND_FLASH_ECC:
		((CSL_EmifNandFlashECC *)response)->p2048o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P2048O);
		((CSL_EmifNandFlashECC *)response)->p1024o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P1024O);
		((CSL_EmifNandFlashECC *)response)->p512o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P512O);
		((CSL_EmifNandFlashECC *)response)->p256o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P256O);
		((CSL_EmifNandFlashECC *)response)->p128o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P128O);
		((CSL_EmifNandFlashECC *)response)->p64o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P64O);
              ((CSL_EmifNandFlashECC *)response)->p32o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P32O);
		((CSL_EmifNandFlashECC *)response)->p16o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P16O);
		((CSL_EmifNandFlashECC *)response)->p8o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P8O);
		((CSL_EmifNandFlashECC *)response)->p4o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P4O);
		((CSL_EmifNandFlashECC *)response)->p2o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P2O);
		((CSL_EmifNandFlashECC *)response)->p1o = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P1O);
              ((CSL_EmifNandFlashECC *)response)->p2048e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P2048E);
		((CSL_EmifNandFlashECC *)response)->p1024e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P1024E);
		((CSL_EmifNandFlashECC *)response)->p512e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P512E);
		((CSL_EmifNandFlashECC *)response)->p256e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P256E);
		((CSL_EmifNandFlashECC *)response)->p128e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P128E);
		((CSL_EmifNandFlashECC *)response)->p64e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P64E);
              ((CSL_EmifNandFlashECC *)response)->p32e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P32E);
		((CSL_EmifNandFlashECC *)response)->p16e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P16E);
		((CSL_EmifNandFlashECC *)response)->p8e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P8E);
		((CSL_EmifNandFlashECC *)response)->p4e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P4E);
		((CSL_EmifNandFlashECC *)response)->p2e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P2E);
		((CSL_EmifNandFlashECC *)response)->p1e = CSL_FEXT(hEmif->regs->NANDF3ECC, EMIF_NANDF3ECC_P1E);
		break;

    	case CSL_EMIF_QUERY_CS2_NAND_FLASH_ECC:
		((CSL_EmifNandFlashECC *)response)->p2048o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P2048O);
		((CSL_EmifNandFlashECC *)response)->p1024o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P1024O);
		((CSL_EmifNandFlashECC *)response)->p512o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P512O);
		((CSL_EmifNandFlashECC *)response)->p256o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P256O);
		((CSL_EmifNandFlashECC *)response)->p128o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P128O);
		((CSL_EmifNandFlashECC *)response)->p64o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P64O);
              ((CSL_EmifNandFlashECC *)response)->p32o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P32O);
		((CSL_EmifNandFlashECC *)response)->p16o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P16O);
		((CSL_EmifNandFlashECC *)response)->p8o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P8O);
		((CSL_EmifNandFlashECC *)response)->p4o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P4O);
		((CSL_EmifNandFlashECC *)response)->p2o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P2O);
		((CSL_EmifNandFlashECC *)response)->p1o = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P1O);
              ((CSL_EmifNandFlashECC *)response)->p2048e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P2048E);
		((CSL_EmifNandFlashECC *)response)->p1024e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P1024E);
		((CSL_EmifNandFlashECC *)response)->p512e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P512E);
		((CSL_EmifNandFlashECC *)response)->p256e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P256E);
		((CSL_EmifNandFlashECC *)response)->p128e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P128E);
		((CSL_EmifNandFlashECC *)response)->p64e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P64E);
              ((CSL_EmifNandFlashECC *)response)->p32e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P32E);
		((CSL_EmifNandFlashECC *)response)->p16e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P16E);
		((CSL_EmifNandFlashECC *)response)->p8e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P8E);
		((CSL_EmifNandFlashECC *)response)->p4e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P4E);
		((CSL_EmifNandFlashECC *)response)->p2e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P2E);
		((CSL_EmifNandFlashECC *)response)->p1e = CSL_FEXT(hEmif->regs->NANDF4ECC, EMIF_NANDF4ECC_P1E);
		break;

	case CSL_EMIF_QUERY_IODFT_TLEC:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->IODFTECR, EMIF_IODFTECR_TLEC);
		break;	

	case CSL_EMIF_QUERY_IODFT_MISR: 
		((CSL_EmifIodftMisr*)response)->misrLsb = CSL_FEXT(hEmif->regs->IODFTMRLR, EMIF_IODFTMRLR_TLMR);
		((CSL_EmifIodftMisr*)response)->misrMid = CSL_FEXT(hEmif->regs->IODFTMRMR, EMIF_IODFTMRMR_TLMR);
              ((CSL_EmifIodftMisr*)response)->misrMsb = CSL_FEXT(hEmif->regs->IODFTMRMSBR, EMIF_IODFTMRMSBR_TLMR);
		break;

	case CSL_EMIF_QUERY_RELEASE_NUM:
		*(Uint16 *)response = CSL_FEXT(hEmif->regs->MODRNR, EMIF_MODRNR_RLNUM);
		break;	

	default:
    	       status = CSL_ESYS_INVQUERY ;
		break;	   

	}
	return status;
}
/* Rev.No.   Date/Time               ECN No.          Modifier      */
/* -------   ---------               -------          --------      */

/* 3          Aug 10:00:33 9         2271             xkeshavm      */
/*                                                                  */
/* To upload API CSL 0.51 Release                                   */
/********************************************************************/ 
