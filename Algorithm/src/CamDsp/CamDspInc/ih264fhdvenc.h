/*
//============================================================================
//
//    FILE NAME : IH264FHDVENC.h
//
//    ALGORITHM : h264enc
//
//    VENDOR    : TII
//
//    TARGET DSP: C64x
//
//    PURPOSE   : IH264FHDVENC Interface Header
//
//============================================================================
*/

#ifndef _IH264FHDVENC_
#define _IH264FHDVENC_


#include <xdas.h>
#include <ialg.h>
#include <ividenc1.h>

/*
// ===========================================================================
// IH264FHDVENC_Handle
//
// This handle is used to reference all h264enc instance objects
*/
typedef struct IH264FHDVENC_Obj *IH264FHDVENC_Handle;

/*
// ===========================================================================
// IH264FHDVENC_Obj
//
// This structure must be the first field of all h264enc instance objects
*/
typedef struct IH264FHDVENC_Obj
{
    struct IH264FHDVENC_Fxns *fxns;
} IH264FHDVENC_Obj;


/*
// ===========================================================================
// IH264FHDVENC_Status
//
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct IH264FHDVENC_Status {
  
  /*!
   * Mandatory fields of the status structure - Base class
   */
  IVIDENC1_Status  videncStatus; 


  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
} IH264FHDVENC_Status;

/*
// ===========================================================================
// IH264FHDVENC_Cmd
//
// The Cmd enumeration defines the control commands for the MPEG4 video encoder
// control method.
*/
typedef IVIDENC1_Cmd IH264FHDVENC_Cmd;

//!< Level Identifier for H.264 Encoder
typedef enum
{
  IH264FHDVENC_LEVEL_10 = 10,  //!< Level 1.0
  IH264FHDVENC_LEVEL_1b =  9,  //!< Level 1.b
  IH264FHDVENC_LEVEL_11 = 11,  //!< Level 1.1
  IH264FHDVENC_LEVEL_12 = 12,  //!< Level 1.2
  IH264FHDVENC_LEVEL_13 = 13,  //!< Level 1.3
  IH264FHDVENC_LEVEL_20 = 20,  //!< Level 2.0
  IH264FHDVENC_LEVEL_21 = 21,  //!< Level 2.1
  IH264FHDVENC_LEVEL_22 = 22,  //!< Level 2.2
  IH264FHDVENC_LEVEL_30 = 30,  //!< Level 3.0
  IH264FHDVENC_LEVEL_31 = 31,  //!< Level 3.1
  IH264FHDVENC_LEVEL_32 = 32,  //!< Level 3.2
  IH264FHDVENC_LEVEL_40 = 40,  //!< Level 4.0
  IH264FHDVENC_LEVEL_41 = 41,  //!< Level 4.1
  IH264FHDVENC_LEVEL_42 = 42,  //!< Level 4.2
  IH264FHDVENC_LEVEL_50 = 50,  //!< Level 5.0
  IH264FHDVENC_LEVEL_51 = 51  //!< Level 5.1
} IH264FHDVENC_Level ;
/*
// ===========================================================================
// control method commands
*/
#define IH264FHDVENC_GETSTATUS      XDM_GETSTATUS
#define IH264FHDVENC_SETPARAMS      XDM_SETPARAMS
#define IH264FHDVENC_RESET          XDM_RESET
#define IH264FHDVENC_FLUSH          XDM_FLUSH
#define IH264FHDVENC_SETDEFAULT     XDM_SETDEFAULT
#define IH264FHDVENC_GETBUFINFO     XDM_GETBUFINFO

/* Example of extension of command ID, starting with 256 on ward  */
#define IH264FHDVENC_PREPROCESS   256/* internal preprocssing for noise revomal */
/* Error code */
#define SET_BUFFEROVERFLOWERROR 0x0
#define SET_ISBUFFEROVERFLOWERROR(x) (((x) >> SET_BUFFEROVERFLOWERROR) & 0x1) 

#define SET_SLICESIZEOVERFLOW 0x1
#define SET_ISSLICESIZEOVERFLOW(x) (((x) >> SET_SLICESIZEOVERFLOW) & 0x1)  

/* Extend enums in xdm.h for 422SP */
#define H264FHDVENC_TI_422SP (XDM_CUSTOMENUMBASE + 0)

/** ===========================================================================
*@typedef IH264FHDVENC_STATUS
*
*@brief  Enumeration of the different error codes that can be returned by the 
*        H.264 Encoder implementation
*
*@note   None
*
*@n ===========================================================================
*/
typedef enum
{
  H264FHDVENC_PASS,         /*!< 0x00 => Pass                                   */
  H264FHDVENC_FAIL,         /*!< 0x01 => General Failure                        */
  H264FHDVENC_SIZE_OVRFLW   /*!< 0x02 => Input frame size more than maximum     */

}IH264FHDVENC_STATUS; 
 
//!< 4x4 and 8x8 ipe luma mode for H.264 Encoder 
typedef enum 
{
  IH264FHDVENC_IPE_DEFAULT = 0,
  IH264FHDVENC_IPE_VERT  = 1,
  IH264FHDVENC_IPE_HORIZ = 2,
  IH264FHDVENC_IPE_DDL   = 8,
  IH264FHDVENC_IPE_DDR   = 16,
  IH264FHDVENC_IPE_VERRIGHT  = 32,
  IH264FHDVENC_IPE_HORDOWN = 64,
  IH264FHDVENC_IPE_VERTLEFT = 128,
  IH264FHDVENC_IPE_HORUP    = 256
} IH264FHDVENC_IPE_MODES; 
#define IH264FHDVENC_IPE_ALL (IH264FHDVENC_IPE_VERT | IH264FHDVENC_IPE_HORIZ | IH264FHDVENC_IPE_DDL |\
                              IH264FHDVENC_IPE_DDR | IH264FHDVENC_IPE_VERRIGHT | IH264FHDVENC_IPE_HORDOWN |\
                              IH264FHDVENC_IPE_VERTLEFT | IH264FHDVENC_IPE_HORUP)
//!< 16x16 ipe luma mode for H.264 Encoder 
typedef enum 
{ 
  IH264FHDVENC_16x16_DEFAULT = 0,
  IH264FHDVENC_16x16_VERT   = 1,
  IH264FHDVENC_16x16_HORIZ  = 2,
  IH264FHDVENC_16x16_PLANE  = 8 
} IH264FHDVENC_16x16_MODES;
#define IH264FHDVENC_16x16_ALL (IH264FHDVENC_16x16_VERT | IH264FHDVENC_16x16_HORIZ | IH264FHDVENC_16x16_PLANE)

//!< 4x4 mode for H.264 Encoder
typedef enum
{
  IH264FHDVENC_4x4_NONE    = 0,
  IH264FHDVENC_4x4_I_FRAME = 1,
  IH264FHDVENC_4x4_P_FRAME = 2,
  IH264FHDVENC_4x4_IP_FRAME = 3
} IH264FHDVENC_4x4_ENABLE;

//!< 8x8 mode for H.264 Encoder
typedef enum
{
  IH264FHDVENC_8x8_NONE    = 0,
  IH264FHDVENC_8x8_I_FRAME = 1,
  IH264FHDVENC_8x8_P_FRAME = 2,
  IH264FHDVENC_8x8_IP_FRAME = 3
} IH264FHDVENC_8x8_ENABLE;

typedef enum
{
  IH264FHDVENC_POC_TYPE_0 = 0,
  IH264FHDVENC_POC_TYPE_2 = 2
}IH264FHDVENC_POC_TYPE;

/*ME alg 1080i and 720p selector*/
 typedef enum
{ 
 IH264FHDVENC_ME720p,
 IH264FHDVENC_ME1080i
}IH264FHDVENC_ME_SELECT;

/*ME1080iMode */
typedef enum
{ 
  IH264FHDVENC_HIGH_VIDEO_QUALITY,
  IH264FHDVENC_PREDICTABLE_DMA
} IH264FHDVENC_ME1080I_MODE;

typedef enum
{
  IH264FHDVENC_ROWS_SLICES,
  IH264FHDVENC_BYTES_SLICES
}IH264FHDVENC_SLICE_TYPE;

typedef enum
{
  IH264FHDVENC_ONE_FIELD_DEFAULT,   /*!< lowest quality, highest performance            */
  IH264FHDVENC_TWO_FIELDS_DEFAULT,  /*!< Prediction from the same parity                */
  IH264FHDVENC_TWO_FIELDS_MRCRF,    /*!< Most Recently Coded Reference Field            */
  IH264FHDVENC_TWO_FIELDS_ADAPTIVE  /*!< highest quality, lowest performance            */
} IH264FHDVENC_REF_MODES;

typedef enum
{
  IH264FHDVENC_CCV_LINEDROP,
  IH264FHDVENC_CCV_AVERAGE
} IH264FHDVENC_CHROMA_CONV;

#define IH264FHDVENC_MAXCPBCNT 1
/*
// ===========================================================================
// IH264FHDVENC_HrdParm
//
// HRD (hypothetical reference decoder) parameters
*/
typedef struct
{
  XDAS_UInt32  cpb_cnt_minus1;
  XDAS_UInt8   bit_rate_scale;
  XDAS_UInt8   cpb_size_scale;
  XDAS_UInt8   cbr_flag[IH264FHDVENC_MAXCPBCNT];
  XDAS_UInt8   initial_cpb_removal_delay_length_minus1;
  XDAS_UInt8   cpb_removal_delay_length_minus1;
  XDAS_UInt8   dpb_output_delay_length_minus1;
  XDAS_UInt8   time_offset_length;
} IH264FHDVENC_HrdParm;

/*
// ===========================================================================
// IH264FHDVENC_VUIDataStructure
//
// This structure defines VUI parameters
*/
typedef struct
{
  XDAS_UInt8   aspect_ratio_info_present_flag;
  XDAS_UInt8   overscan_info_present_flag;
  XDAS_UInt8   chroma_loc_info_present_flag;
  XDAS_UInt8   timing_info_present_flag;
  XDAS_UInt8   nal_hrd_parameters_present_flag;
  IH264FHDVENC_HrdParm   nal_hrd_parameters;
  XDAS_UInt8   vcl_hrd_parameters_present_flag;
  XDAS_UInt8   bitstream_restriction_flag;
} IH264FHDVENC_VUIDataStructure;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*!
  @enum   IH264ENC_SliceCodingPreset
  @brief  These enumerations control the type of slice coding 
*/

typedef enum
{
  IH264_SLICECODING_DEFAULT     = 0 , //!< Default slice coding params
  IH264_SLICECODING_USERDEFINED = 1 , //!< User defined slicecoding params
  IH264_SLICECODING_EXISTING    = 2 , //!< Keep the slice coding params as existing
                                      //!< This is useful because during control call
                                      //!< if user don't want to chnage the sliceCodingParams
  IH264_SLICECODING_MAX

} IH264FHDVENC_SliceCodingPreset;

/*!
  @enum   IH264ENC_SliceMode
  @brief  These enumerations control the type of slice coding 
*/

typedef enum
{
  IH264_SLICEMODE_NONE    = 0 , //!< No multiple slices
  IH264_SLICEMODE_MBUNIT  = 1 , //!< Slices are controlled based upon number of Macroblocks
  IH264_SLICEMODE_BYTES   = 2 , //!< Slices are controlled based upon number of bytes
  IH264_SLICEMODE_OFFSET  = 3 , //!< Slices are controlled based upon user defined offset in 
                                //!< unit of Rows
  IH264_SLICEMODE_DEFAULT = IH264_SLICEMODE_NONE,  //!< Default slice coding mode Single Slice
  IH264_SLICEMODE_MAX

} IH264FHDVENC_SliceMode;

/*!
  @enum   IH264ENC_StreamFormat
  @brief  These enumerations control the type stream format
*/
typedef enum
{
  IH264_BYTE_STREAM = 0,                        //!< bit-stream contains the start code identifier
  IH264_NALU_STREAM = 1,                        //!< bit-stream doesn't contain the start code identifier
  IH264_STREAM_FORMAT_DEFAULT = IH264_BYTE_STREAM , //!< Default slice coding mode is byte-stream
  IH264_STREAM_FORMAT_MAX
}IH264FHDVENC_StreamFormat;


#define MAX_NUM_SLICE_START_OFFSET  (4) //!< Maximum Number of slice start points
 
/*!
  @struct IH264ENC_SliceCodingParams
  @brief  This structure contains all the parameters which controls Slice 
          encoding

  @param  sliceCodingPreset  
          This Preset controls the USER_DEFINED vs DEFAULT mode. if User is 
          not aware about following fields, it should be set as 
          IH264_SLICECODING_DEFAULT

  @param  sliceMode  : regarded @ IH264ENC_DynamicParams::sliceCodingParams
          This defines the control mechanism to split a picture in slices. 
          It can be either MB based or bytes based

  @param  sliceUnitSize  : regarded @ IH264ENC_DynamicParams::sliceCodingParams  
          The meaning of this parameter depends upon sliceMode. 
          sliceMode == IH264_SLICEMODE_MBUNIT then this 
          parameter informs the number of Macroblocks in one slice
          sliceMode == IH264_SLICEMODE_BYTES then this 
          parameter informs the number of bytes in one slice
          sliceMode == IH264_SLICEMODE_OFFSET then this 
          parameter informs the number of offset information provided by user.
          Actual offset are provided with sliceRowStartNumber

  @param  sliceStartOffset[MAX_NUM_SLICE_START_OFFSET]  : regarded @ IH264ENC_DynamicParams::sliceCodingParams  
          ith entry of this array contains the starting row number of ith slice in picture

  @param  streamFormat  : ignored @ IH264ENC_DynamicParams::sliceCodingParams  
          Controls the type of stream : byte stream format or NALU format
          refer IH264ENC_StreamFormat for possible values
*/
typedef struct IH264FHDVENC_SliceCodingParams {
  IH264FHDVENC_SliceCodingPreset sliceCodingPreset     ; 
  IH264FHDVENC_SliceMode         sliceMode             ; 
  XDAS_Int32                 sliceUnitSize         ;
  XDAS_Int32                 sliceStartOffset[MAX_NUM_SLICE_START_OFFSET] ;
  IH264FHDVENC_StreamFormat      streamFormat          ;

} IH264FHDVENC_SliceCodingParams;

/*
// ===========================================================================
// IH264FHDVENC_Params
//
// This structure defines the creation parameters for all h264enc objects
*/
typedef struct IH264FHDVENC_Params
{
  
  /*!
   * Mandatory fields of the Params structure - Base class
   */
  IVIDENC1_Params videncParams; 

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
  XDAS_Int32  profileIdc;     //!< profile idc
  XDAS_Int32  levelIdc;       //!< level idc
  XDAS_Int32  EntropyCodingMode; /*!< Entryopy Coding Mode */
} IH264FHDVENC_Params;

/*
// ===========================================================================
// IH264FHDVENC_PARAMS
//
// Default parameter values for h264enc instance objects
*/
extern IH264FHDVENC_Params IH264FHDVENC_PARAMS;

/*
 *  ======== IVIDENC_InArgs ========
 *  This structure defines the run time arguments for all VIDENC objects.
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */
typedef struct IH264FHDVENC_InArgs
{
  /*!
   * Mandatory fields of the InArgs structure - Base class
   */
  IVIDENC1_InArgs   videncInArgs;

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */

}IH264FHDVENC_InArgs;

/*
// ===========================================================================
// IH264FHDVENC_DynamicParams
//
// This structure defines the run time parameters for all h264enc objects
*/
typedef struct IH264FHDVENC_DynamicParams
{
  /*!
   * Mandatory fields of the DynamicParams structure - Base class
   */
  IVIDENC1_DynamicParams videncDynamicParams;
  
  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
  IH264FHDVENC_SliceCodingParams   sliceCodingParams; 
  XDAS_Int32 OutBufSize;       /*!< Size of the Output Buffer          */
  XDAS_UInt8  QPISlice;         /*!< Quant. param for I Slices (0-51)   */
  XDAS_UInt8  QPSlice;          /*!< Quant. Param for non - I Slices    */
  XDAS_UInt8  RateCtrlQpMax;    /*!< Maximum QP to be used  Range[0,51] */
  XDAS_UInt8  RateCtrlQpMin;    /*!< Minimum QP to be used  Range[0,51] */
  XDAS_UInt8  NumRowsInSlice;   /*!< Number of rows in a Slice          */ 
  XDAS_UInt8  LfDisableIdc;     /*!< Loop Filter enable/disable control */
  XDAS_Int8   LFAlphaC0Offset;  /* Alpha & C0 offset div. 2,            */
  XDAS_Int8   LFBetaOffset;     /* Beta offset div. 2,                  */
  XDAS_Int8   ChromaQPOffset;   /*!< Chroma QP offset (-12..12)         */ 
  XDAS_Int8   SecChromaQPOffset;/*!<Secondary chroma Qp offset          */
  XDAS_UInt8  PicAFFFlag;       /*!< PicAFF for interlace content       */
  XDAS_UInt8  PicOrderCountType;/*!< POC type                           */
  XDAS_UInt16 AdaptiveMBs;      /*!< Number of the adaptive I MBs  */
  XDAS_UInt8  SEIParametersFlag;/*!< XDAS_TRUE will give the SEI parameters*/
  XDAS_UInt8  VUIParametersFlag;/*!< XDAS_TRUE will give the VUI parameters*/
  XDM_SingleBufDesc  VUIData;   /*!< one can pass a pointer of the vui 
                                  *   structure to the encoder */
  XDAS_Void  (*NALUnitCallback)(
               XDAS_UInt8 *addr, 
               XDAS_UInt32 len); /*!< callback*/
  XDAS_UInt8  SkipStartCodesInCallback; /*!< XDAS_TRUE will remove start codes
                                              from NAL in callback */
  XDAS_UInt8  Intra4x4EnableFlag; /*!< One choose Intra 4x4 mode in 
                                       I and P frames separately, 
                                        check the IH264FHDVENC_4x4Mode */
  XDAS_UInt8  BlockingCallFlag;  /*!< Enable blocking calls support*/
  XDAS_UInt8  MESelect;          /*!< ME algorithm control*/
  XDAS_UInt8  ME1080iMode;       /*!< Predictable DMA/ High Video Quality,
                                      IH264FHDVENC_ME1080iMode*/
  XDAS_UInt8  MVDataFlag;         /**< Flag to indicate that the algorithm should
                                 *  generate MV and SAD Data */
  XDAS_UInt8  Transform8x8DisableFlag; /*!< XDAS_TRUE will disable 8x8 transform
                                       *   for the HIGH_PROFILE, only Inter MBs
                                       *   Default value: 0 */
  XDAS_UInt8  Intra8x8EnableFlag; /*!< One choose Intra 8x8 mode in 
                                       I and P frames separately, 
                                        check the IH264FHDVENC_8x8Mode */
  XDAS_UInt8  InterlaceReferenceMode; /*!< Reference fields number,
                                           refer to IH264FHDVENC_REF_MODES*/
  XDAS_UInt8  ChromaConversionMode; /*!< Chroma conversion mode,
                                     refer to IH264FHDVENC_CHROMA_CONV */ 
  XDAS_Int32 maxDelay;       /*!< Set the max Delay value in milli seconds        */
  XDAS_Int32 MaxSlicesSupported_IFrame; /*!< Maximum number of slices supported for I Frame */
  XDAS_Int32 MaxSlicesSupported_PFrame; /*!< Maximum number of slices supported for P Frame */

} IH264FHDVENC_DynamicParams;

/*
 *  ======== IH264FHDVENC_OutArgs ========
 * This structure defines a data related with MBs
 */
typedef struct
{  
  XDAS_UInt32  uiSAD_inter; /*!< Array to store intra/inter MB SADs output  */
  XDAS_Int16  MVx,MVy;  /*!< Array to hold the Luma MVs  */
}IH264FHDVENC_H264MBData;

typedef enum
{
  METADATA_NALU_INFO, 	/*!< Meta data Containing the information about slice size.*/ 
  METADATA_MB_INFO 	    /*! <Meta data Containing the information about Each Macro block.*/ 
}IH264ENC_MetaDataId;

typedef struct
{
  IH264ENC_MetaDataId metaDataID;
  void * 	          metaDataBuffer;
  XDAS_Int32 	      numPayLoadUnits;
  XDAS_Int32 	      payLoadUnitSize;
} IH264ENC_MetaDataParams;


/*
 *  ======== IH264FHDVENC_OutArgs ========
 *  This structure defines the run time output arguments for IH264FHDVENC
 *  instance objects.
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */
typedef struct IH264FHDVENC_OutArgs
{
  /*!
   * Mandatory fields of the OutArgs structure - Base class
   */
  IVIDENC1_OutArgs  videncOutArgs;

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
  IH264ENC_MetaDataParams metaData[XDM_MAX_IO_BUFFERS-1];
  //Int              *mv_table_ptr;
  XDAS_UInt32 MVDataSize;    /*!< Size of the buffer containing the MV data */


} IH264FHDVENC_OutArgs;

/*
// ===========================================================================
// IH264FHDVENC_Fxns
//
// This structure defines all of the operations on h264enc objects
*/
typedef struct IH264FHDVENC_Fxns
{
  /*!
   * Mandatory functions of the function table - Base class
   */
    IVIDENC1_Fxns  ividenc;    
   
  /*!
   * Additional functions specific to H.264 Encoder - Extensions to base class
   */

} IH264FHDVENC_Fxns;

#endif  /* _IH264FHDVENC_ */

