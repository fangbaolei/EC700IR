
/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_tcp2Aux.h
 *
 *   @path  $(CSLPATH)\tcp2\inc
 *
 *   @desc  Auxiliary API header file for TCP CSL
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  26-Mar-2005  sd File Created.
 *  21-Jul-2005  sd Updated for the requirement changes
 *  15-Sep-2005  sd Changed TCP to TCP2 in all the names
 *  30-Jan-2006  sd Updated the descriptions for TCP2_normalCeil and TCP2_ceil
 *                  and chnages for the bit field names TCPEND register
 * =============================================================================
 */

#ifndef _CSL_TCP2AUX_H_
#define _CSL_TCP2AUX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <tistdtypes.h>
#include <cslr_tcp2.h>
#include <csl_tcp2.h>

/** ============================================================================
 *   @n@b TCP2_normalCeil 
 *
 *   @b Description
 *   @n Returns the value rounded to the nearest integer greater than or 
 *      equal to (val1/val2) 
 *
 *   @b Arguments
     @verbatim
            val1        Value to be augmented.

            val2        Value by which val1 must be divisible.

     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            Uint32 framelen = 51200;
            Uint32  numSubFrame;
            ...
            // to calculate the number of sub frames for SP mode 
            numSubFrame = TCP2_normalCeil (framelen, TCP2_SUB_FRAME_SIZE_MAX);
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_normalCeil (
    Uint32 val1, 
    Uint32 val2
) 
{
	Uint32 x;

	/* x = ceil(val1/val2) */
	/* val1 is increased (if necessary) to be a multiple of val2 */
	x = (((val1) % (val2)) != 0) ? (((val1) / (val2)) + 1) :
										((val1) / (val2));

	return(x);
}

/** ============================================================================
 *   @n@b TCP2_ceil 
 *
 *   @b Description
 *   @n Returns the value rounded to the nearest integer greater than or 
 *      equal to (val/(2^pwr2)). 
 *
 *   @b Arguments
     @verbatim
            val         Value to be augmented.

            pwr2        The power of two by which val must be divisible.

     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        Uint32              val1 = 512;
        Uint32              val2 = 4;
        Uint32              val3;

        val3 = TCP2_ceil(val1, val2);
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_ceil (
    Uint32 val, 
    Uint32 pwr2
) 
{
  Uint32 x;
  
  /* x = ceil(val/ (2^pwr2)) */
  /* val is increased (if necessary) to be a multiple of 2^pwr2 */
  x = (((val) - (((val) >> (pwr2)) << (pwr2))) == 0) ?                           \
      ((val) >> (pwr2)): (((val) >> (pwr2)) + 1);
  

  return(x);
}

/** ============================================================================
 *   @n@b TCP2_setExtScaling 
 *
 *   @b Description
 *   @n This function formats individual bytes into a 32-bit word, which is 
 *		used to set the extrinsic configuration registers.
 *
 *   @b Arguments
     @verbatim
            extrVal1        extrinsic scaling value 1

            extrVal2        extrinsic scaling value 2

            extrVal3        extrinsic scaling value 3

            extrVal4        extrinsic scaling value 4

     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        TCP2_Params configParams;
        TCP2_IcConfig	configIc

        configIc->ic12 = TCP2_setExtScaling (configParams->extrScaling [0],
                    configParams->extrScaling [1],
                    configParams->extrScaling [2],
                    configParams->extrScaling [3]);
     @endverbatim
 * =============================================================================
 */
 
CSL_IDEF_INLINE Uint32 TCP2_setExtScaling (
    Uint8   extrVal1,
    Uint8   extrVal2,	
    Uint8   extrVal3,	
    Uint8   extrVal4
)
{
	Uint32 icConfigVal;

	icConfigVal = (extrVal4 << 18) | (extrVal3 << 12) | (extrVal2 << 6) |
									(extrVal1);
	return icConfigVal;
}

/** ============================================================================
 *   @n@b TCP2_makeTailArgs 
 *
 *   @b Description
 *   @n This function formats individual bytes into a 32-bit word, which is 
 *		used to set the tail bits configuration registers.
 *
 *   @b Arguments
     @verbatim
            byte17_12   Byte to be placed in bits 17-12 of the 32-bit value

            byte11_6    Byte to be placed in bits 11-6 of the 32-bit value

            byte5_0     Byte to be placed in bits 5-0 of the 32-bit value

     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        TCP2_Params configParams;
        TCP2_IcConfig	configIc

        configIc.ic6 = TCP2_makeTailArgs (xabData[10],
                            xabData[8], xabData[6]);
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_makeTailArgs (
    Uint8 byte17_12,
    Uint8 byte11_6,  
    Uint8 byte5_0
) 
{

  Uint32 x = 0;

  x = (byte17_12 << 12) | (byte11_6 << 6) | byte5_0;
  
  return(x);
}
 
/** ============================================================================
 *   @n@b TCP2_getAccessErr 
 *
 *   @b Description
 *   @n This function returns the ACC bit value of the TCPERR register 
 *      indicating whether an invalid access has been made to the TCP during 
 *      operation.
 *	 @n	  0 - no error
 *	 @n   1 - TCP rams (syst, parities, hard decisions, extrinsics, aprioris) 
 *			access is not allowed in state 1. This causes an error interrupt 
 *			to occur
 *
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getAccessErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getAccessErr (
     void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_ACC);
}
 
/** ============================================================================
 *   @n@b TCP2_getErr
 *
 *   @b Description
 *   @n This function returns the ERR bit value of the TCPERR register 
 *      indicating whether an error has occurred during TCP operation.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_ERR);
}

/** ============================================================================
 *   @n@b TCP2_getTcpErrors
 *
 *   @b Description
 *   @n This function returns the TCPERR register value.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getTcpErrors ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getTcpErrors (
	void
) 
{
	return (tcp2Regs->TCPERR);
}
 
/** ============================================================================
 *   @n@b TCP2_getFrameLenErr 
 *
 *   @b Description
 *   @n This function returns a boolean value indicating whether an invalid
 *		frame length has been programmed in the TCP during operation.
 *	 @n	0 - no error. 
 *	 @n	1 - (SA mode) frame length < 40 or frame length > 20730. 
 * 		  - (SP mode) frame length < 256 or frame length > 20480 and f%256!=0 
 *			for the first or middle subframes. 
 *		  - (SP mode) if f<128 or f>20480 for the last subframe.

 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getFrameLenErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getFrameLenErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_F);
}
 
/** ============================================================================
 *   @n@b TCP2_getProlLenErr 
 *
 *   @b Description
 *   @n This function returns the P bit value indicating whether an invalid 
 *		prolog length has been programmed into the TCP.
 *		0 - no error
 *		1 - Prolog length < 4 or > 48
 *
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getProlLenErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getProlLenErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_P);
}
 
/** ============================================================================
 *   @n@b TCP2_getSubFrameErr 
 *
 *   @b Description
 *   @n This function returns a boolean value indicating whether the sub-frame 
 *		length programmed into the TCP is invalid.
 *		0 - no error
 * 		1 - sub-frame length > 20480 (SP mode)
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getSubFrameErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getSubFrameErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_SF);
}
 
/** ============================================================================
 *   @n@b TCP2_getRelLenErr 
 *
 *   @b Description
 *   @n This function returns the R bit value indicating whether an invalid 
 *		reliability length has been programmed into the TCP. The reliability 
 * 		length must be 40 < RL < 128 for SA Mode, or 
 *		SW_R must = 128  during First/Middle subframes of SP mode, and 
 *		SW_R must be > 64 in the Last subframe.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getRelLenErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getRelLenErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_R);
}
 
/** ============================================================================
 *   @n@b TCP2_getSnrErr 
 *
 *   @b Description
 *   @n This function returns the SNR bit value indicating whether the SNR 
 *		threshold exceeded 100 (1) or not (0).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getSnrErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getSnrErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_SNR);
}
 
/** ============================================================================
 *   @n@b TCP2_getInterleaveErr 
 *
 *   @b Description
 *   @n This function returns the INTER value bit indicating whether the TCP 
 *		was incorrectly programmed to receive an interleaver table. An 
 *		interleaver table can only be sent when operating in standalone mode. 
 *		This bit(1) indicates if an interleaver table was sent when in shared 
 *		processing mode.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getInterleaveErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getInterleaveErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_INT);
}
 
/** ============================================================================
 *   @n@b TCP2_getOutParmErr 
 *
 *   @b Description
 *   @n This function returns the OP bit value (1) indicating whether the TCP was 
 *		programmed to transfer output parameters in shared processing mode. 
 *		The output parameters are only valid when operating in standalone mode.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getOutParmErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getOutParmErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_OP);
}
 
/** ============================================================================
 *   @n@b TCP2_getMaxMinErr 
 *
 *   @b Description
 *   @n This function returns the MAXMINITER bit value indicating whether the 
 *		TCP was programmed with the minimum iterations value greater than the 
 *		maximum iterations.
 *		0 = no error, 1 = min_iter > max_iter
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getMaxMinErr ()){
		...
		} 
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getMaxMinErr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPERR, TCP2_TCPERR_MAXMINITER);
}
 
/** ============================================================================
 *   @n@b TCP2_getNumIt 
 *
 *   @b Description
 *   @n This function returns the number of decoded iterations of the TCP in 
 *      standalone processing mode. This function reads the output parameters 
 *      register. Alternatively, the EDMA can be used to transfer the output 
 *      parameters following the hard decisions (recommended).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        Uint32  numIter;
        
		numIter = TCP2_getNumIt ();
        
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getNumIt (
	void
) 
{
	return CSL_FEXT (tcp2CfgRegs->TCPOUT0, TCP2_TCPOUT0_FINAL_ITER);
}

/** ============================================================================
 *   @n@b TCP2_getSnrM1 
 *
 *   @b Description
 *   @n This function returns the 1st moment of SNR calculation. This function 
 *      reads the output parameters register. Alternatively, the EDMA can be 
 *      used to transfer the output parameters following the hard decisions 
 *      (recommended).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim

		Uint32 snrM1;

		snrM1 = TCP2_getSnrM1 ();
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getSnrM1 (
	void
) 
{
	return CSL_FEXT (tcp2CfgRegs->TCPOUT0, TCP2_TCPOUT0_SNR_M1);
}

/** ============================================================================
 *   @n@b TCP2_getSnrM2 
 *
 *   @b Description
 *   @n This function returns the 2nd moment of SNR calculation. This function 
 *      reads the output parameters register. Alternatively, the EDMA can be 
 *      used to transfer the output parameters following the hard decisions 
 *      (recommended).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim

		Uint32 snrM2;

		snrM2 = TCP2_getSnrM2 ();
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getSnrM2 (
	void
) 
{
	return CSL_FEXT (tcp2CfgRegs->TCPOUT1, TCP2_TCPOUT1_SNR_M2);
}

/** ============================================================================
 *   @n@b TCP2_getMap
 *
 *   @b Description
 *   @n This function returns the active MAP of the TCP.
 *		0 - MAP 0 is active
 *		1 - MAP 1 is active
 *      This function reads the output parameters register. Alternatively, 
 *      the EDMA can be used to transfer the output parameters following the 
 *      hard decisions (recommended).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 activeMap;

		activeMap = TCP2_getMap ();
        
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getMap (
	void
) 
{
	return CSL_FEXT (tcp2CfgRegs->TCPOUT1, TCP2_TCPOUT1_ACTIVE_MAP);
}

/** ============================================================================
 *   @n@b TCP2_getMap0Err
 *
 *   @b Description
 *   @n This function returns the number of re-encode errors for MAP 0.
 *      This function reads the output parameters register. Alternatively, 
 *      the EDMA can be used to transfer the output parameters following the 
 *      hard decisions (recommended).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 map0Err;

		map0Err = TCP2_getMap0Err ();
        
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getMap0Err (
	void
) 
{
	return CSL_FEXT (tcp2CfgRegs->TCPOUT2, TCP2_TCPOUT2_CNT_RE_MAP0);
}

/** ============================================================================
 *   @n@b TCP2_getMap1Err
 *
 *   @b Description
 *   @n This function returns the number of re-encode errors for MAP 1.
 *      This function reads the output parameters register. Alternatively, 
 *      the EDMA can be used to transfer the output parameters following the 
 *      hard decisions (recommended).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 map1Err;

		map1Err = TCP2_getMap1Err ();
        
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getMap1Err (
	void
) 
{
	return CSL_FEXT (tcp2CfgRegs->TCPOUT2, TCP2_TCPOUT2_CNT_RE_MAP1);
}

/** ============================================================================
 *   @n@b TCP2_statRun 
 *
 *   @b Description
 *   @n This function returns a boolean status indicating whether the TCP MAP 
 *		decoder is in state 0 or state 1-8 (running or not).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		while (!TCP2_statRun());
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statRun (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_DEC_BUSY);
}
 
/** ============================================================================
 *   @n@b TCP2_statError 
 *
 *   @b Description
 *   @n This function returns the ERR bit value indicating whether any TCP 
 *		error has occurred.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statError ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statError (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_ERR);
}
 
/** ============================================================================
 *   @n@b TCP2_statWaitIc 
 *
 *   @b Description
 *   @n This function returns the WIC bit status indicating whether the TCP 
 *		is waiting to receive new IC values.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitIc ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitIc (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_WIC);
}
 
/** ============================================================================
 *   @n@b TCP2_statWaitInter 
 *
 *   @b Description
 *   @n This function returns the WINT status indicating whether the TCP is 
 *		waiting to receive interleaver table data.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitInter ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitInter (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_WINT);
}
 
/** ============================================================================
 *   @n@b TCP2_statWaitSysPar 
 *
 *   @b Description
 *   @n This function returns the WSP bit status indicating whether the TCP 
 *		is waiting to receive systematic and parity data.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitSysPar ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitSysPar (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_WSP);
}
 
/** ============================================================================
 *   @n@b TCP2_statWaitApriori 
 *
 *   @b Description
 *   @n This function returns the WAP bit status indicating whether the TCP is 
 *		waiting to receive apriori data.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitApriori ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitApriori (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_WAP);
}
 
/** ============================================================================
 *   @n@b TCP2_statWaitExt 
 *
 *   @b Description
 *   @n This function returns the REXT bit status indicating whether the TCP 
 *		is waiting for extrinsic data to be read.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitExt ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitExt (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_REXT);
}
 
/** ============================================================================
 *   @n@b TCP2_statWaitHardDec 
 *
 *   @b Description
 *   @n This function returns the RHD bit status indicating whether the TCP 
 *		is waiting for the hard decisions data to be read.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitHardDec ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitHardDec (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_RHD);
}

/** ============================================================================
 *   @n@b TCP2_statWaitOutParm 
 *
 *   @b Description
 *   @n This function returns the ROP bit status indicating whether the TCP 
 *		is waiting for the output parameters to be read.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statWaitOutParm ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statWaitOutParm (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_ROP);
}

/** ============================================================================
 *   @n@b TCP2_statEmuHalt 
 *
 *   @b Description
 *   @n This function returns the emuhalt bit status indicating whether the 
 *		TCP is halted due to emulation.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statEmuHalt ()){
		...
		}
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statEmuHalt (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_EMUHALT);
}

/** ============================================================================
 *   @n@b TCP2_statActMap 
 *
 *   @b Description
 *   @n This function returns the active_map bit status indicating whether 
 *		the TCP MAP 0 is active (0) or the TCP MAP 1 is active (1).
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 activeMap;
		.....
		activeMap = TCP2_statActMap ();
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statActMap (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_ACTIVE_MAP);
}

/** ============================================================================
 *   @n@b TCP2_statActState 
 *
 *   @b Description
 *   @n This function returns the active_state bit status indicating the 
 *		active TCP MAP decoder state.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 activeState;
		.....
		activeState = TCP2_statActState ();
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statActState (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_ACTIVE_STATE);
}

/** ============================================================================
 *   @n@b TCP2_statActIter
 *
 *   @b Description
 *   @n This function returns the active_iter bit status indicating the 
 *		active TCP iteration.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 activeIter;
		.....
		activeIter = TCP2_statActIter ();
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statActIter (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_ACTIVE_ITER);
}

/** ============================================================================
 *   @n@b TCP2_statSnr
 *
 *   @b Description
 *   @n This function returns the snr_exceed bits, indicating whether the 
 *		TCP MAP 0 or MAP 1 passed the SNR criteria in a particular iteration.
 *		0 - All fail
 *		1 - MAP 1 failed
 *		2 - MAP 0 failed
 *		3 - All passed
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 snrExceed;
		.....
		snrExceed = TCP2_statSnr ();
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statSnr (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_SNR_EXCEED);
}

/** ============================================================================
 *   @n@b TCP2_statCrc
 *
 *   @b Description
 *   @n This function returns the crc_pass bit boolean status indicating whether 
 *		the TCP passed CRC check.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_statCrc ()){
		...
		} 
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statCrc (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_CRC_PASS);
}

/** ============================================================================
 *   @n@b TCP2_statTcpState 
 *
 *   @b Description
 *   @n This function returns the state of the TCP state machine for 
 *		the standalone mode or shared processing mode.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
			Uint32 tcpState;
			....
			tcpState = TCP2_statTcpState ();

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_statTcpState (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPSTAT, TCP2_TCPSTAT_TCP_STATE);
}

/** ============================================================================
 *   @n@b TCP2_getExecStatus
 *
 *   @b Description
 *   @n This function returns the TCPSTAT register value.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 tcpStatus;

		tcpStatus = TCP2_getExecStatus ();
        
     @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getExecStatus (
	void
) 
{
	return (tcp2Regs->TCPSTAT);
}
 
/** ============================================================================
 *   @n@b TCP2_getExtEndian 
 *
 *   @b Description
 *   @n This function returns the value programmed into the TCP_END register 
 *		for the extrinsics data indicating whether the data is in its native 
 *		8-bit format ('1') or consists of values packed in little endian format 
 *		into 32-bit words ('0'). This should always be '0' for little endian 
 *		operation.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getExtEndian ()){
		...
		} 

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getExtEndian (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_EXTR);
}

/** ============================================================================
 *   @n@b TCP2_getInterEndian 
 *
 *   @b Description
 *   @n Returns the value programmed into the TCP_END register for the 
 *		interleaver table data indicating whether the data is in its native 
 *		8-bit format ('1') or consists of values packed in little endian format 
 *		into 32-bit words ('0'). This should always be '0' for little endian 
 *		operation.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getInterEndian ()){
		...
		} 

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getInterEndian (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_INTR);
}

/** ============================================================================
 *   @n@b TCP2_getSlpzvss 
 *
 *   @b Description
 *   @n This function gets the configuration of the internal control of 
 *		the slpzvss.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getSlpzvss ()){
		...
		} 

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getSlpzvss (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPEND, TCP2_TCPEND_SLPZVSS_EN);
}

/** ============================================================================
 *   @n@b TCP2_getSlpzvdd 
 *
 *   @b Description
 *   @n This function gets the configuration of the internal control of 
 *		the slpzvdd.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b>  Uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		if (TCP2_getSlpzvdd ()){
		...
		} 

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE Uint32 TCP2_getSlpzvdd (
	void
) 
{
	return CSL_FEXT (tcp2Regs->TCPEND, TCP2_TCPEND_SLPZVDD_EN);
}

/** ============================================================================
 *   @n@b TCP2_setExtEndian 
 *
 *   @b Description
 *   @n This function programs TCP to view the format of the extrinsics data 
 *		as either native 8-bit format ('1') or values packed into 32-bit words 
 * 		in little endian format ('0'). This should always be '0' for little 
 *		endian operation.
 *
 *   @b Arguments
     @verbatim
           endianMode      Endian setting for extrinsics data
     @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCPEND register bit for the extrinsics data is configured in the 
 *		mode passed. 
 *
 *   @b Modifies
 *   @n TCPEND register
 *
 *   @b Example
 *   @verbatim
		TCP2_setExtEndian (1);  

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_setExtEndian (
    Uint32    endianMode	
) 
{
	CSL_FINS (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_EXTR, endianMode);

	return;
}

/** ============================================================================
 *   @n@b TCP2_setInterEndian
 *
 *   @b Description
 *   @n This function programs TCP to view the format of the interleaver data 
 *		as either native 8-bit format ('1') or values packed into 32-bit words 
 * 		in little endian format ('0'). This should always be '0' for little 
 *		endian operation.
 *
 *   @b Arguments
     @verbatim
            endianMode       Endian setting for interleaver data
     @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCPEND register bit for the interleaver data is configured in the 
 *		mode passed. 
 *
 *   @b Modifies
 *   @n TCPEND register
 *
 *   @b Example
 *   @verbatim
		TCP2_setInterEndian (1);
         

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_setInterEndian (
    Uint32    endianMode	
) 
{
	CSL_FINS (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_INTR, endianMode);

	return;
}

/** ============================================================================
 *   @n@b TCP2_setNativeEndian 
 *
 *   @b Description
 *   @n This function programs the TCP to view the format of all data as 
 *		native 8/16 bit format. This should only be used when running in 
 *		big endian mode.
 *
 *   @b Arguments
 *	 @n	None	
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCPEND register configured to native mode for all data. 
 *
 *   @b Modifies
 *   @n TCPEND register
 *
 *   @b Example
 *   @verbatim
		TCP2_setNativeEndian ();

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_setNativeEndian (
	void	
) 
{
	CSL_FINST (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_INTR, NATIVE16);
	CSL_FINST (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_EXTR, NATIVE8);

	return;
}

/** ============================================================================
 *   @n@b TCP2_setPacked32Endian
 *
 *   @b Description
 *   @n This function programs the TCP to view the format of all data as 
 *		packed data in 32-bit words. This should always be used when running 
 * 		in little endian mode and should be used in big endian mode only if 
 *		the CPU is formatting the data.
 *
 *   @b Arguments
 *	 @n	None	
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCPEND register configured to  packed 32 mode for all data. 
 *
 *   @b Modifies
 *   @n TCPEND register
 *
 *   @b Example
 *   @verbatim
		TCP2_setPacked32Endian ();

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_setPacked32Endian (
	void	
) 
{
	CSL_FINST (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_INTR, 32BITS);
	CSL_FINST (tcp2Regs->TCPEND, TCP2_TCPEND_ENDIAN_EXTR, 32BITS);

	return;
}

/** ============================================================================
 *   @n@b TCP2_start 
 *
 *   @b Description
 *   @n This function starts the TCP by writing a 1 to the EXEINST field 
 *		of the TCPEXE register. See also TCP2_debug(), TCP2_debugStep() and 
 *		TCP2_debugComplete().
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCP state machine starts executing. 
 *
 *   @b Modifies
 *   @n TCPEXE register
 *
 *   @b Example
 *   @verbatim
        TCP2_start ();
 
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_start (
	void
) 
{
	CSL_FINST (tcp2Regs->TCPEXE, TCP2_TCPEXE_EXECUTION_INSTR, START);

	return;
}

/** ============================================================================
 *   @n@b TCP2_debug 
 *
 *   @b Description
 *   @n This function puts the TCP into debug mode by writing '4h' to the 
 *		EXEINST field of the TCPEXE register. Normal initialization is 
 *		performed and TCP waits in MAP state 0 for Debug Step or Debug Complete 
 *		to be performed. See also TCP2_start(), TCP2_debugStep() and 
 *		TCP2_debugComplete()
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	EXEINST feild of the TCPEXE register is configured with the value passed. 
 *
 *   @b Modifies
 *   @n TCPEXE register
 *
 *   @b Example
 *   @verbatim
        TCP2_debug ();

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_debug (
	void
) 
{
	CSL_FINST (tcp2Regs->TCPEXE, TCP2_TCPEXE_EXECUTION_INSTR, DEBUG0);

	return;
}

/** ============================================================================
 *   @n@b TCP2_debugStep 
 *
 *   @b Description
 *   @n This function executes one MAP decode and waits in state 6 when the 
 *		TCP is in Debug mode, by writing '5h' to the EXEINST field of the 
 *		TCPEXE register. See also TCP2_start(), TCP2_debug() and 
 *		TCP2_debugComplete()
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	EXEINST feild of the TCPEXE register is configured with the value passed. 
 *
 *   @b Modifies
 *   @n TCPEXE register
 *
 *   @b Example
 *   @verbatim
        TCP2_debugStep ();
        
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_debugStep (
	void
) 
{
	CSL_FINST (tcp2Regs->TCPEXE, TCP2_TCPEXE_EXECUTION_INSTR, DEBUG6);

	return;
}

/** ============================================================================
 *   @n@b TCP2_debugComplete 
 *
 *   @b Description
 *   @n This function executes the remaing MAP decodes when the TCP is in 
 *		Debug mode, by writing '6h' to the EXEINST field of the TCPEXE register. 
 *		See also TCP2_start(), TCP2_debug() and TCP2_debugComplete()
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	EXEINST feild of the TCPEXE register is configured with the value passed. 
 *
 *   @b Modifies
 *   @n TCPEXE register
 *
 *   @b Example
 *   @verbatim
        TCP2_debugComplete ();

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_debugComplete (
	void
) 
{
	CSL_FINST (tcp2Regs->TCPEXE, TCP2_TCPEXE_EXECUTION_INSTR, DEBUG);

	return;
}

/** ============================================================================
 *   @n@b TCP2_reset 
 *
 *   @b Description
 *   @n This function performs a soft reset of all TCP registers except for 
 *		TCPEXE and TCPEND registers.
 *
 *   @b Arguments
 *	 @n	None
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	performs soft reset of TCP. 
 *
 *   @b Modifies
 *   @n TCPEXE register
 *
 *   @b Example
 *   @verbatim
         TCP2_reset ();

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_reset (
	void
) 
{
	CSL_FINST (tcp2Regs->TCPEXE, TCP2_TCPEXE_EXECUTION_INSTR, SOFTRESET);

	return;
}

/** ============================================================================
 *   @n@b TCP2_setSlpzvdd 
 *
 *   @b Description
 *   @n This function enables/disables the internal control of the slpzvdd.
 *
 *   @b Arguments
     @verbatim
          slpzvddCtrl      enable/disable configuration of the slpzvdd 
	 @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCPEND register configured with the value passed. 
 *
 *   @b Modifies
 *   @n TCPEND register
 *
 *   @b Example
 *   @verbatim
        TCP2_setSlpzvdd (1);

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_setSlpzvdd (
	Uint32	slpzvddCtrl	
) 
{
	CSL_FINS (tcp2Regs->TCPEND, TCP2_TCPEND_SLPZVDD_EN, slpzvddCtrl);

	return;
}

/** ============================================================================
 *   @n@b TCP2_setSlpzvss 
 *
 *   @b Description
 *   @n This function enables/disables the internal control of the slpzvss.
 *
 *   @b Arguments
     @verbatim
           slpzvssCtrl        enable/disable configuration of the slpzvss 
	 @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCPEND register configured with the value passed. 
 *
 *   @b Modifies
 *   @n TCPEND register
 *
 *   @b Example
 *   @verbatim
        TCP2_setSlpzvss (1);

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_setSlpzvss (
    Uint32    slpzvssCtrl	
) 
{
	CSL_FINS (tcp2Regs->TCPEND, TCP2_TCPEND_SLPZVSS_EN, slpzvssCtrl);

	return;
}

/** ============================================================================
 *   @n@b TCP2_getIcConfig
 *
 *   @b Description
 *   @n This function reads the input configuration values currently programmed 
 *		into the TCP.
 *
 *   @b Arguments
     @verbatim
           config        TCP configuration structure to hold the read values 
	 @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	config structure contains the TCP input configuration values. 
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
        TCP2_getIcConfig (&config);
 
	 @endverbatim
 * =============================================================================
 */

CSL_IDEF_INLINE void TCP2_getIcConfig (
	TCP2_ConfigIc *config
) 
{

	Uint32 gie;

	gie =  _disable_interrupts ();

	config->ic0  =  tcp2CfgRegs->TCPIC0;
	config->ic1  =  tcp2CfgRegs->TCPIC1;
	config->ic2  =  tcp2CfgRegs->TCPIC2;
	config->ic3  =  tcp2CfgRegs->TCPIC3;
	config->ic4  =  tcp2CfgRegs->TCPIC4;
	config->ic5  =  tcp2CfgRegs->TCPIC5;
	config->ic6  =  tcp2CfgRegs->TCPIC6;
	config->ic7  =  tcp2CfgRegs->TCPIC7;
	config->ic8  =  tcp2CfgRegs->TCPIC8;
	config->ic9  =  tcp2CfgRegs->TCPIC9;
	config->ic10 =  tcp2CfgRegs->TCPIC10;
	config->ic11 =  tcp2CfgRegs->TCPIC11;
	config->ic12  =  tcp2CfgRegs->TCPIC12;
	config->ic13  =  tcp2CfgRegs->TCPIC13;
	config->ic14  =  tcp2CfgRegs->TCPIC14;
	config->ic15  =  tcp2CfgRegs->TCPIC15;

	_restore_interrupts (gie); 
}

/** ============================================================================
 *   @n@b TCP2_icConfig
 *
 *   @b Description
 *   @n Programs the TCP with the input configuration values passed in the 
 *		TCP2_ConfigIc structure. This is not the recommended means by which to 
 *		program the TCP, as it is more efficient to transfer the IC values using
 *		the EDMA.
 *
 *   @b Arguments
     @verbatim
           config        TCP configuration structure containing the values to be
           				 programmed 
	 @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCP input configuration registers are programmed with the values passed. 
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		TCP2_ConfigIc configIc;
		configIc.ic0 = 0x00283300;
		configIc.ic1 = 0x00270000;
		configIc.ic2 = 0x00080118;
		configIc.ic3 = 0x00000011;
		configIc.ic4 = 0x00000100;
		configIc.ic5 = 0x00000000;
		configIc.ic6 = 0x00032c2f;
		configIc.ic7 = 0x00027831;
		configIc.ic8 = 0x00000000;
		configIc.ic9 = 0x00018430;
		configIc.ic10 = 0x0003bfcd;
		configIc.ic11 = 0x00000000;
		configIc.ic12 = 0x00820820;
		configIc.ic13 = 0x00820820;
		configIc.ic14 = 0x00820820;
		configIc.ic15 = 0x00820820;

        TCP2_icConfig (&config);

	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_icConfig (
	TCP2_ConfigIc *config
) 
{
	Uint32 gie;

	gie =  _disable_interrupts ();

 	tcp2CfgRegs->TCPIC0 = config->ic0;
 	tcp2CfgRegs->TCPIC1 = config->ic1;
 	tcp2CfgRegs->TCPIC2 = config->ic2;
 	tcp2CfgRegs->TCPIC3 = config->ic3;
 	tcp2CfgRegs->TCPIC4 = config->ic4;
 	tcp2CfgRegs->TCPIC5 = config->ic5;
 	tcp2CfgRegs->TCPIC6 = config->ic6;
 	tcp2CfgRegs->TCPIC7 = config->ic7;
 	tcp2CfgRegs->TCPIC8 = config->ic8;
 	tcp2CfgRegs->TCPIC9 = config->ic9;
 	tcp2CfgRegs->TCPIC10 = config->ic10;
 	tcp2CfgRegs->TCPIC11 = config->ic11;
 	tcp2CfgRegs->TCPIC12 = config->ic12;
 	tcp2CfgRegs->TCPIC13 = config->ic13;
 	tcp2CfgRegs->TCPIC14 = config->ic14; 	
 	tcp2CfgRegs->TCPIC15 = config->ic15;
	
	_restore_interrupts (gie); 
}

/** ============================================================================
 *   @n@b TCP2_icConfigArgs
 *
 *   @b Description
 *   @n Programs the TCP with the input configuration values passed. This is 
 *		not the recommended means by which to program the TCP, as it is more 
 *		efficient to transfer the IC values using the EDMA.
 *
 *   @b Arguments
     @verbatim
           ic0        TCP input configuration register 0 value
           ic1        TCP input configuration register 1 value
           ic2        TCP input configuration register 2 value
           ic3        TCP input configuration register 3 value
           ic4        TCP input configuration register 4 value
           ic5        TCP input configuration register 5 value
           ic6        TCP input configuration register 6 value
           ic7        TCP input configuration register 7 value
           ic8        TCP input configuration register 8 value
           ic9        TCP input configuration register 9 value
           ic10        TCP input configuration register 10 value
           ic11        TCP input configuration register 11 value
           ic12        TCP input configuration register 12 value
           ic13        TCP input configuration register 13 value
           ic14        TCP input configuration register 14 value
           ic15        TCP input configuration register 15 value
     @endverbatim
 *
 *   <b> Return Value </b> 
 *	 @n	 None	
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *	 @n	TCP input configuration registers are programmed with the values passed. 
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
		Uint32 ic0, ic1, ic2, ic3, ic4, ic5, ic6, ic7, ic8, ic9 , ic10,
				ic11, ic12, ic13, ic14, ic15;

		ic0 = 0x00283300;
		ic1 = 0x00270000;
		ic2 = 0x00080118;
		ic3 = 0x00000011;
		ic4 = 0x00000100;
		ic5 = 0x00000000;
		ic6 = 0x00032c2f;
		ic7 = 0x00027831;
		ic8 = 0x00000000;
		ic9 = 0x00018430;
		ic10 = 0x0003bfcd;
		ic11 = 0x00000000;
		ic12 = 0x00820820;
		ic13 = 0x00820820;
		ic14 = 0x00820820;
		ic15 = 0x00820820;
		
        TCP2_icConfigArgs (ic0, ic1, ic2, ic3, ic4, ic5, ic6, ic7,
        		ic8, ic9, ic10, ic11, ic12, ic13, ic14, ic15);
                
	 @endverbatim
 * =============================================================================
 */
CSL_IDEF_INLINE void TCP2_icConfigArgs (
	Uint32 ic0,
	Uint32 ic1,
	Uint32 ic2,
	Uint32 ic3,
	Uint32 ic4,
	Uint32 ic5,
	Uint32 ic6,
	Uint32 ic7,
	Uint32 ic8,
	Uint32 ic9,
	Uint32 ic10,
	Uint32 ic11,
	Uint32 ic12,
	Uint32 ic13,
	Uint32 ic14,
	Uint32 ic15

) 
{
	Uint32 gie;

	gie =  _disable_interrupts ();

 	tcp2CfgRegs->TCPIC0 = ic0;
 	tcp2CfgRegs->TCPIC1 = ic1;
 	tcp2CfgRegs->TCPIC2 = ic2;
 	tcp2CfgRegs->TCPIC3 = ic3;
 	tcp2CfgRegs->TCPIC4 = ic4;
 	tcp2CfgRegs->TCPIC5 = ic5;
 	tcp2CfgRegs->TCPIC6 = ic6;
 	tcp2CfgRegs->TCPIC7 = ic7;
 	tcp2CfgRegs->TCPIC8 = ic8;
 	tcp2CfgRegs->TCPIC9 = ic9;
 	tcp2CfgRegs->TCPIC10 = ic10;
 	tcp2CfgRegs->TCPIC11 = ic11;
 	tcp2CfgRegs->TCPIC12 = ic12;
 	tcp2CfgRegs->TCPIC13 = ic13;
 	tcp2CfgRegs->TCPIC14 = ic14; 	
 	tcp2CfgRegs->TCPIC15 = ic15;
	
	_restore_interrupts (gie); 
}


#ifdef __cplusplus
}
#endif

#endif /* _CSL_TCP2AUX_H_ */

























