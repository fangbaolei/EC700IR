/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */

/** ============================================================================
 *   @file  csl_tcp2.h
 *
 *   @path  $(CSLPATH)\tcp2\inc
 *
 *   @desc  API header file for TCP2 CSL
 *
 */

/* =============================================================================
 *  Revision History
 *  ===============
 *  15-Mar-2005  sd File Created.
 *  21-Jul-2005  sd Updated for the requirement changes
 *  15-Sep-2005  sd Changed TCP to TCP2 in all the names
 *  30-Jan-2006  sd Minor changes in the descriptions
 * =============================================================================
 */

#ifndef _CSL_TCP2_H_
#define _CSL_TCP2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <tistdtypes.h>
#include <cslr_tcp2.h>
#include <soc.h>

/****************************************\
* TCP global typedef declarations
\****************************************/  
/** Address of the TCP2 registers */     
#define tcp2Regs ((CSL_Tcp2Regs*)CSL_TCP2_0_REGS) 

/** Address of the configuration registers */
#define tcp2CfgRegs ((CSL_Tcp2CfgRegs*)CSL_TCP2_CFG_REGS)

/****************************************\
* TCP global defines
\****************************************/
/** define for TCP2 code rate 3/4 */ 
#define TCP2_RATE_3_4           CSL_TCP2_TCPIC0_RATE_3_4
/** define for TCP2 code rate 1/2 */
#define TCP2_RATE_1_2           CSL_TCP2_TCPIC0_RATE_1_2
/** define for TCP2 code rate 1/3 */
#define TCP2_RATE_1_3           CSL_TCP2_TCPIC0_RATE_1_3
/** define for TCP2 code rate 1/4 */
#define TCP2_RATE_1_4           CSL_TCP2_TCPIC0_RATE_1_4
/** define for TCP2 code rate 1/5 */
#define TCP2_RATE_1_5           CSL_TCP2_TCPIC0_RATE_1_5

/** Number of sliding windows per block is <= 128 */
#define TCP2_SW_LEQ128          CSL_TCP2_TCPIC0_NUMSW_LEQ_128
/** Number of sliding windows per block is > 128 */
#define TCP2_SW_G128            CSL_TCP2_TCPIC0_NUMSW_G_128

/** TCP stand alone mode  */
#define TCP2_MODE_SA        CSL_TCP2_TCPIC0_OPMOD_SA
/** TCP shared processing, first sub frame */
#define TCP2_FIRST_SF       CSL_TCP2_TCPIC0_OPMOD_SP_FF
/** TCP shared processing, middle sub frame */
#define TCP2_MIDDLE_SF  CSL_TCP2_TCPIC0_OPMOD_SP_MF
/** TCP shared processing, last sub frame */
#define TCP2_LAST_SF        CSL_TCP2_TCPIC0_OPMOD_SP_LF
/** TCP shared processing mode  */
#define TCP2_MODE_SP        1

/** Decoder standard 3GPP */
#define TCP2_STANDARD_3GPP      0
/** Decoder standard IS2000 */
#define TCP2_STANDARD_IS2000    1

/** TCP maximum reliability length */
#define TCP2_RLEN_MAX         128

/** TCP maximum sub frame size */
#define TCP2_SUB_FRAME_SIZE_MAX    20480

/** TCP maximum standalone mode frame size */
#define TCP2_FLEN_MAX    20730

/** TCP shared processing non interleaved MAP*/
#define TCP2_MAP_MAP1           0

/** TCP shared processing interleaved MAP*/
#define TCP2_MAP_MAP2           1 

/* typedefs */

/** This data type is used to define the TCP standards */
typedef Uint8  TCP2_Standard;

/** This data type is used to define the TCP mode 
  * (stand alone or shared processing)
  */
typedef Uint8  TCP2_Mode;

/** This data type is used to define the TCP map (1,2) */
typedef Uint8   TCP2_Map;

/** This data type is used to define the number of sliding 
  * windows per block 
  */
typedef Uint8  TCP2_NumSW;

/** This data type is used to define the TCP code rates 
  * (1/2, 1/3, 1/4, 1/5, 3/4)
  */
typedef Uint8  TCP2_Rate;

/** This data type is used to represent the TCP data */
typedef Uint8  TCP2_UserData;

/** This data type is used to represent the TCP tail data */
typedef Int8  TCP2_TailData;

/** This data type is used to represent the TCP extrinsic data */
typedef Uint8  TCP2_ExtrinsicData;

/** This data type is used to represent the TCP input data*/
typedef Uint32  TCP2_InputData;

/** Enum for the input sign values */
typedef enum {
    /** Multiply the channel input by +1 */
    TCP2_INPUT_SIGN_POSITIVE = CSL_TCP2_TCPIC3_INPUTSIGN_POSITIVE,
    /** Multiply the channel input by -1 */
    TCP2_INPUT_SIGN_NEGATIVE = CSL_TCP2_TCPIC3_INPUTSIGN_NEGATIVE
}TCP2_InputSign;

/** Enum for the output order values */
typedef enum {
    /** Order of the bits in the output data is  0-31 */
    TCP2_OUT_ORDER_0_31 = CSL_TCP2_TCPIC3_OUTORDER_0_31,
    /** Order of the bits in the output data is 31-0 */
    TCP2_OUT_ORDER_31_0 = CSL_TCP2_TCPIC3_OUTORDER_31_0
}TCP2_OutputOrder;

/** The TCP input configuration structure holds all the 
  * configuration values that are to be transferred to the 
  * TCP via the EDMA 
  */
typedef struct {
   Uint32 ic0; /**< TCP input configuration word 0 value */
   Uint32 ic1; /**< TCP input configuration word 1 value */
   Uint32 ic2; /**< TCP input configuration word 2 value */
   Uint32 ic3; /**< TCP input configuration word 3 value */
   Uint32 ic4; /**< TCP input configuration word 4 value */
   Uint32 ic5; /**< TCP input configuration word 5 value */
   Uint32 ic6; /**< TCP input configuration word 6 value */
   Uint32 ic7; /**< TCP input configuration word 7 value */
   Uint32 ic8; /**< TCP input configuration word 8 value */
   Uint32 ic9; /**< TCP input configuration word 9 value */
   Uint32 ic10; /**< TCP input configuration word 10 value */
   Uint32 ic11; /**< TCP input configuration word 11 value */
   Uint32 ic12; /**< TCP input configuration word 12 value */
   Uint32 ic13; /**< TCP input configuration word 13 value */
   Uint32 ic14; /**< TCP input configuration word 14 value */
   Uint32 ic15; /**< TCP input configuration word 15 value */ 
} TCP2_ConfigIc;

/** The TCP parameters structure holds all the information concerning 
  * the user channel. These values are used to generate the appropriate 
  * input configuration values for the TCP.
  */ 
typedef struct {
    TCP2_Standard standard; /**< TCP standard */ 
    /* IC0 parameters */
    TCP2_Mode     mode;  /**< TCP mode */
    TCP2_Map      map;   /**< TCP shared processing MAP */
    TCP2_Rate     rate;  /**< TCP code rate */
    TCP2_NumSW   numSlideWin; /**< Number of sliding window per sub block */
    Uint32       intFlag;     /**< Interleaver write flag     */
    Uint32       outParmFlag; /**< Output parameters read flag */
    Uint32       frameLen;    /**< Frame length               */

    /* IC1 parameters */
    Uint32       relLen;      /**< Reliability length */

    /* IC2 parameters */
    Uint32       prologSize;  /**< Prolog length       */   
    Uint32       numSubBlock; /**< Number of sub blocks */
    Uint32       maxIter;     /**< Maximum number of iterations            */
    Uint32       snr;         /**< SNR threshold used for stopping test */  

    /* IC3 parameters */
    Bool        maxStarEn;    /**< Enable/disable the max star        */
    Bool        prologRedEn;  /**< Enable/disable the prolog reduction */
    Uint8       minIter;     /**< Minimum number of iterations to be executed */    
    TCP2_InputSign  inputSign;/**< The sign of the input data (+/-)          */
    TCP2_OutputOrder    outputOrder; /**< The bit ordering of the output data */

    /* IC4 parameters */
    Uint8       numCrcPass;   /**< Number of passed CRC iterations required 
                                * before decoder termination              
                                */
    Uint8       crcLen;       /**< CRC polynomial length */ 

    /* IC5 parameters */
    Uint32      crcPoly;      /**< CRC polynomial       */

    /* IC12 - IC15 parameters */
    Uint8       extrScaling [16];  /**< Extrinsic scaling factors */

} TCP2_Params;

/** The TCP base parameters structure is used to set up the TCP programmable 
  * parameters. You create the object and pass it to the TCP2_genParams() 
  * function which returns the TCP2_Params structure.
  */
typedef struct {
    TCP2_Standard standard;   /**< TCP decoder standards */ 
    TCP2_Rate   rate;       /**< TCP code rate */
    TCP2_Map    map;        /**< TCP shared processing MAP */ 
    Uint32      intFlag;     /**< Interleaver write flag     */
    Uint32      outParmFlag; /**< Output parameters read flag */
    Uint32      frameLen;    /**< Frame length               */
    Uint32      prologSize;  /**< Prolog length       */   
    Uint32      maxIter;     /**< Maximum number of iterations            */
    Uint32      snr;         /**< SNR threshold used for stopping test */  
    Bool        maxStarEn;    /**< Enable/disable the max star        */
    Bool        prologRedEn;  /**< Enable/disable the prolog reduction */
    Uint8       minIter;      /**< Minimum number of iterations to be executed*/   
    TCP2_InputSign  inputSign;/**< The sign of the input data (+/-)          */
    TCP2_OutputOrder    outputOrder; /**< The bit ordering of the output data */
    Uint8       numCrcPass;   /**< Number of passed CRC iterations required 
                                * before decoder termination              
                                */
    Uint8       crcLen;       /**< CRC polynomial length */ 
    Uint32      crcPoly;      /**< CRC polynomial       */
    Uint8       extrScaling [16];  /**< Extrinsic scaling factors */
} TCP2_BaseParams;


/* function declarations */

/* ============================================================================
 *   @n@b TCP2_setParams 
 *
 *   @b Description
 *   @n This function sets up the TCP2 input configuration parameters in the
 *      TCP2_ConfigIc structure. The configuration values are passed in the 
 *      configParms input argument.
 *
 *   @b Arguments
     @verbatim
            configParms     Pointer to the structure holding the TCP 
                            configuration parameters.
                            
            configIc        Pointer to the TCP2_ConfigIc structure to be filled.

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n The configIc argument passed.   
 *
 *   @b Example
 *   @verbatim 
        extern TCP2_Params *configParms;
        TCP2_ConfigIc *configIc;
        ...
        TCP2_setParams(configParms, configIc);
     @endverbatim
 * ===========================================================================
 */
void TCP2_setParams (
    TCP2_Params     *restrict configParms,
    TCP2_ConfigIc   *restrict configIc
);

/* ============================================================================
 *   @n@b TCP2_tailConfig 
 *
 *   @b Description
 *   @n This function generates the input control values IC6-IC11 based on the 
 *      processing to be performed by the TCP. These values consist of the tail 
 *      data following the systematics and parities data. This function 
 *      calls specific tail generation functions depending on the
 *      standard followed.
 *
 *   @b Arguments
     @verbatim
            standard        3G standard to be decoded.

            mode            TCP processing mode (SA or SP)

            map             TCP shared processing MAP 

            rate            Code rate of the TCP

            xabData         Pointer to the tail data

            configIc        Pointer to the TCP2_ConfigIc structure to be filled.

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n The configIc argument passed.   
 *
 *   @b Example
 *   @verbatim
        extern TCP2_Params *configParms;
        TCP2_ConfigIc *configIc;
        TCP2_UserData *xabData = &userData[fraemLen];       
        
        ...
        TCP2_tailConfig (configParms->mode, configParms->map,
                    configParms->standard, configParms->rate,
                    userData, configIc);
     @endverbatim
 * ===========================================================================
 */
void TCP2_tailConfig (
    TCP2_Standard   standard,
    TCP2_Mode       mode,
    TCP2_Map        map,
    TCP2_Rate       rate,
    TCP2_TailData   *restrict tailData,
    TCP2_ConfigIc   *restrict configIc
);

/* ============================================================================
 *   @n@b TCP2_genIc 
 *
 *   @b Description
 *   @n This function sets up the TCP2 input configuration parameters in the
 *      TCP2_ConfigIc structure. The configuration values are passed in the 
 *      configParms input argument.
 *
 *   @b Arguments
     @verbatim
            configParms     Pointer to the structure holding the TCP 
                            configuration parameters.
    
            tailData        Tail data

            configIc        Pointer to the TCP2_ConfigIc structure to be filled.

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n The configIc argument passed.   
 *
 *   @b Example
 *   @verbatim
        extern TCP2_UserData *userData;
        
        TCP2_BaseParams configBase;
        TCP2_Params     configParams;
        TCP2_TailData   *xabData;
        Uint32          frameLen = 40;

        xabData = &userData [frameLen];
        
        // assign the configuration parameters 
        configBase.frameLen     = frameLen;
        configBase.inputSign    = TCP2_INPUT_SIGN_POSITIVE;
        configBase.intFlag      = 1;
        configBase.maxIter      = 8;
        configBase.maxStarEn    = TRUE;
        configBase.standard     = TCP2_STANDARD_3GPP;
        configBase.crcLen       = 0;
        configBase.crcPoly      = 0;
        configBase.minIter      = 1;
        configBase.numCrcPass   = 1;
        configBase.outParmFlag  = 0;
        configBase.outputOrder  = TCP2_OUT_ORDER_0_31;
        configBase.prologRedEn  = FALSE;
        configBase.prologSize   = 24;
        configBase.rate         = TCP2_RATE_1_3;
        configBase.snr          = 0;

        for (cnt = 0; cnt < 16; cnt++)
            configBase.extrScaling [cnt] = 32;

        // setup the TCP configuration registers parmeters 
        TCP2_genParams (&configBase, &configParams);

        // generate the configuration register values 
        TCP2_genIc (&configParams, xabData, &configIc); 

     @endverbatim
 * ===========================================================================
 */
void TCP2_genIc (
    TCP2_Params      *restrict configParms,
    TCP2_TailData    *restrict tailData,
    TCP2_ConfigIc    *restrict configIc
);

/* ============================================================================
 *   @n@b TCP2_genParams  
 *
 *   @b Description
 *   @n This function copies the basic parameters, to the configParams 
 *      parameters structure.
 *
 *   @b Arguments
     @verbatim
            configBase      Pointer to the TCP2_BaseParams structure 
            
            configParams    Pointer to the TCP configuration parameters 
                            structure.

     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *   @n                     The number sub frames for shared processing mode                       
 *
 *   <b> Pre Condition </b>
 *   @n  configBase is populated with all the configurtaion parameters
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n The configParams argument passed.   
 *
 *   @b Example
 *   @verbatim
        extern TCP2_UserData *userData;
        
        TCP2_BaseParams configBase;
        TCP2_Params     configParams;
        TCP2_TailData   *xabData;
        Uint32          frameLen = 40;

        xabData = &userData [frameLen];
        
        // assign the configuration parameters 
        configBase.frameLen     = frameLen;
        configBase.inputSign    = TCP2_INPUT_SIGN_POSITIVE;
        configBase.intFlag      = 1;
        configBase.maxIter      = 8;
        configBase.maxStarEn    = TRUE;
        configBase.standard     = TCP2_STANDARD_3GPP;
        configBase.crcLen       = 0;
        configBase.crcPoly      = 0;
        configBase.minIter      = 1;
        configBase.numCrcPass   = 1;
        configBase.outParmFlag  = 0;
        configBase.outputOrder  = TCP2_OUT_ORDER_0_31;
        configBase.prologRedEn  = FALSE;
        configBase.prologSize   = 24;
        configBase.rate         = TCP2_RATE_1_3;
        configBase.snr          = 0;

        for (cnt = 0; cnt < 16; cnt++)
            configBase.extrScaling [cnt] = 32;

        // setup the TCP configuration registers parmeters 
        TCP2_genParams (&configBase, &configParams);

     @endverbatim
 * ===========================================================================
 */
Uint32 TCP2_genParams (
    TCP2_BaseParams  *restrict configBase,
    TCP2_Params      *restrict configParams
);


/* ============================================================================
 *   @n@b TCP2_calcSubBlocksSA 
 *
 *   @b Description
 *   @n This function calculates the number of sub blocks for the TCP 
 *      standalone processing. The reliability length is also calculated and the 
 *      configParms structure is populated.
 *
 *   @b Arguments
     @verbatim
            configParms     Pointer to the structure holding the TCP 
                            configuration parameters.

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n The configParms argument passed.   
 *
 *   @b Example
 *   @verbatim
        extern TCP2_Params *configParms;
        ...
        TCP2_calcSubBlocksSA (configParms);
     @endverbatim
 * ===========================================================================
 */
void TCP2_calcSubBlocksSA (
    TCP2_Params *configParms 
);
/* ============================================================================
 *   @n@b TCP2_calcSubBlocksSP 
 *
 *   @b Description
 *   @n This function calculates the number of sub blocks for the TCP 
 *      shared processing. The reliability length is also calculated and the 
 *      configParms structure is populated.
 *
 *   @b Arguments
     @verbatim
            configParms     Pointer to the structure holding the TCP 
                            configuration parameters.

     @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *   @n                     Number of sub frames the frame is divided into
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None 
 *
 *   @b Modifies
 *   @n The configParms argument passed.   
 *
 *   @b Example
 *   @verbatim
        extern TCP2_Params *configParms;
        ...
        TCP2_calcSubBlocksSP (configParms);
     @endverbatim
 * ===========================================================================
 */
Uint32 TCP2_calcSubBlocksSP (
    TCP2_Params *configParms 
);

/* ============================================================================
 *   @n@b TCP2_tailConfig3GPP 
 *
 *   @b Description
 *   @n This function generates the input control values IC6-IC11 for 3GPP
 *      channels. These values consist of the tail data following the 
 *      systematics and parities data. This function is called from the generic 
 *      TCP2_tailConfig function.
 *
 *   @b Arguments
     @verbatim
            mode        TCP processing mode (SA or SP)

            map         TCP shared processing MAP 

            rate        TCP data code rate

            tailData    Pointer to the tail data

            configIc    Pointer to the IC values structure

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The configIc structure is assigned the tail configuration values 
 *       based on the tailData. 
 *
 *   @b Modifies
 *   @n The configIc argument passed.   
 *
 *   @b Example
 *   @verbatim

     @endverbatim
 * ===========================================================================
 */
void TCP2_tailConfig3GPP (
        TCP2_Mode       mode,
        TCP2_Map        map,
        TCP2_Rate       rate,
        TCP2_TailData   *restrict tailData,
        TCP2_ConfigIc   *restrict configIc
); 

/* ============================================================================
 *   @n@b TCP2_tailConfigIs2000 
 *
 *   @b Description
 *   @n This function generates the input control values IC6-IC11 for IS2000
 *      channels. These values consist of the tail data following the 
 *      systematics and parities data. This function is called from the generic 
 *      TCP2_tailConfig function.
 *
 *   @b Arguments
     @verbatim
            mode        TCP processing mode (SA or SP)

            map         TCP shared processing MAP 

            rate        TCP data code rate

            tailData    Pointer to the tail data

            configIc    Pointer to the IC values structure

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The configIc structure is assigned the tail configuration values 
 *       based on the taildata. 
 *
 *   @b Modifies
 *   @n The configIc argument passed.   
 *
 *   @b Example
 *   @verbatim

     @endverbatim
 * ===========================================================================
 */
void TCP2_tailConfigIs2000 (
        TCP2_Mode       mode,
        TCP2_Map        map,
        TCP2_Rate       rate,
        TCP2_TailData   *restrict tailData,
        TCP2_ConfigIc   *restrict configIc
); 

/* ============================================================================
 *   @n@b TCP2_deinterleaveExt 
 *
 *   @b Description
 *   @n This function de-interleaves the MAP2 extrinsics data to generate 
 *      apriori data for the MAP1 decode. This function is for use in 
 *      performing shared processing.
 *
 *   @b Arguments
     @verbatim
            aprioriMap1         Apriori data for MAP1 decode

            extrinsicsMap2      Extrinsics data 

            interleaverTable    Interleaver data table

            numExt              Number of Extrinsics
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The aprioriMap1 will contain the deinterleaved data. 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
            <...MAP 2 decode...>

            TCP2_deinterleaveExt(aprioriMap1, extrinsicsMap2,

                      interleaverTable, numExt);

            <...MAP 1 decode...>
     @endverbatim
 * ===========================================================================
 */
void TCP2_deinterleaveExt (
    TCP2_ExtrinsicData          *aprioriMap1,
    const TCP2_ExtrinsicData    *extrinsicsMap2,
    const Uint16                *interleaverTable,
    Uint32                      numExt
);


/* ============================================================================
 *   @n@b TCP2_interleaveExt 
 *
 *   @b Description
 *   @n This function interleaves the MAP1 extrinsics data to generate 
 *      apriori data for the MAP2 decode. This function is for used in 
 *      performing shared processing.
 *
 *   @b Arguments
     @verbatim
            aprioriMap2          Apriori data for MAP2 decode

            extrinsicsMap1       Extrinsics data 

            interleaverTable     Interleaver data table

            numExt               Number of Extrinsics
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The aprioriMap2 will contain the interleaved data. 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
            <...MAP 1 decode...>

            TCP2_interleaveExt(aprioriMap2, extrinsicsMap1,

                         interleaverTable, numExt);

            <...MAP 2 decode...>
     @endverbatim
 * ===========================================================================
 */
void TCP2_interleaveExt (
    TCP2_ExtrinsicData          *aprioriMap2,
    const TCP2_ExtrinsicData    *extrinsicsMap1,
    const Uint16                *interleaverTable,
    Uint32                      numExt
);

/* ============================================================================
 *   @n@b TCP2_depunctInputs
 *
 *   @b Description
 *   @n This function scales and sorts input data into a code rate 1/5 format.
 *
 *   @b Arguments
     @verbatim
            frameLen            Input data length in bytes
            
            inputData           Input data
            
            rate                Input data code rate

            scalingFact         Scaling factor

            depunctData         Depunctured data
            
      @endverbatim
 *
 *   <b> Return Value </b>  Uint32
 *   @n  Length of output data in 32 bit words  
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The depunctData will contain the data depunctured to rate 1/5. 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim

            TCP2_depunctInputs (length, inputData, rate

                      scalingFact, depunctData);

     @endverbatim
 * ===========================================================================
 */
void TCP2_depunctInputs (
    Uint32              frameLen,
    TCP2_UserData*      inputData,
    TCP2_Rate           rate,
    Uint32              scalingFact,
    TCP2_InputData*     depunctData       
);

/* ============================================================================
 *   @n@b TCP2_calculateHd
 *
 *   @b Description
 *   @n This function calculates the hard decisions following multiple MAP 
 *      decodings in shared processing mode.
 *
 *   @b Arguments
     @verbatim
            extrinsicsMap1      Extrinsics data following MAP1 decode

            apriori             Apriori data following MAP2 decode

            channel_data        Input channel data

            hardDecisions       Hard decisions

            numExt              Number of extrinsics

     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  The hardDecisions will contain the calculated hard decisions. 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
            void TCP2_calculateHd(extrinsicsMap1, apriori,
                      channel_data, hardDecisions, numExt);
     @endverbatim
 * ===========================================================================
 */
void TCP2_calculateHd (
    const TCP2_ExtrinsicData    *extrinsicsMap1,
    const TCP2_ExtrinsicData    *apriori,
    const TCP2_UserData         *channelData,
    Uint32                      *hardDecisions,
    Uint16                      numExt
);


/* ============================================================================
 *   @n@b TCP2_demuxInput 
 *
 *   @b Description
 *   @n This function splits the input data into two working sets. One set 
 *      contains the non-interleaved input data and is used with the MAP 1 
 *      decoding. The other contains the interleaved input data and is used 
 *      with the MAP2 decoding. This function is used in shared processing mode.
 *
 *   @b Arguments
     @verbatim
            rate                TCP data code rate

            frameLen            Frame length

            input               Input channel data

            interleaver         Interleaver data table

            nonInterleaved      Non Interleaved data for SP MAP0

            interleaved         Interleaved data for SP MAP1
     @endverbatim
 *
 *   <b> Return Value </b>  
 *   @n  None 
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n The nonInterleaved will contain the non-interleaved 
 *      data and the interleaved will contain the interleaved data. 
 *
 *   @b Modifies
 *   @n None   
 *
 *   @b Example
 *   @verbatim
             TCP2_demuxInput (frameLen, input,
                       interleaver, interleaved, nonInterleaved);
     @endverbatim
 * ===========================================================================
 */
void TCP2_demuxInput (
    Uint32                  rate,
    Uint32                  frameLen,
    const TCP2_UserData     *input,
    const Uint16            *interleaver,
    TCP2_ExtrinsicData      *nonInterleaved,
    TCP2_ExtrinsicData      *interleaved
);

#ifdef __cplusplus
}
#endif

#endif  /* _CSL_TCP2_H_ */
