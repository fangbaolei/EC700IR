#ifndef IDMJPGE_
#define IDMJPGE_

#include <std.h>
#include <xdas.h>
#include <ialg.h>
#include "iimgenc1.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Commands passed to API.
 *
 *  These commands are passed to the JPEG encoder through the
 *  XDAIS interface to control the encoder or to retrieve
 *  information from the encoder.
 */
typedef IIMGENC1_Cmd  IDMJPGE_TIGEM_Cmd;

typedef enum {

    XDM_SETIMGPREPROCESSOR =256//,
    //XDM_INSERTCOMMENT,
   // XDM_GETVERSION

} IDMJPGEPP_TIGEM_CmdId;


/* JPEG encoder Error status messages */
typedef enum{

    /*Successful decoding*/
	JPEGENC_SUCCESS = 0,
	JPEGENC_OUTPUT_BUFF_SIZE =1,   // Set an error if Output buffer size is less than required
	JPEGENC_THUMB_RESOLUTION=2     //set an error if Thumnail_resolutions is greater than Image_resolutions


}IJPEGENC_ErrorStatus;



#define  IDMJPGE_TIGEM_CMD_GETSTATUS           XDM_GETSTATUS
#define  IDMJPGE_TIGEM_CMD_SETPARAMS           XDM_SETPARAMS
#define  IDMJGPE_TIGEM_CMD_RESET               XDM_RESET
#define  IDMJPGE_TIGEM_CMD_SETDEFAULT          XDM_SETDEFAULT
#define  IDMJPGE_TIGEM_CMD_FLUSH               XDM_FLUSH
#define  IDMJPGE_TIGEM_CMD_GETBUFINFO          XDM_GETBUFINFO
#define  IDMJPGE_TIGEM_CMD_SETIMGPREPROCESSOR  XDM_SETIMGPREPROCESSOR
#define  IDMJPGE_TIGEM_CMD_INSERTCOMMENT       XDM_INSERTCOMMENT
#define  IDMJPGE_TIGEM_CMD_GETVERSION		   XDM_GETVERSION


#ifdef iVLCD
/*
 *  ======== IDMJPGE_TIGEM_CustomHuffmanTables ========
 *  JPEG Encoder Custom Huffman Tables - This structure has two parts
 *  1. The first part defines the custom VLC tables that need to be passed to
 *  the JPEG Encoder. This will be used in the VLC Module.
 *  2. The second part defines the format in which the custom Huffman tables
 *  need to be passed to the JPEG Encoder. This is compliant with the format
 *  specified in RFC 2035 (RTP Payload Format for JPEG-compressed Video).This
 *  is used to create the DHT marker in the JPEG Header.
 *
 *  NOTE 1: An important note about the alignment requirement of this structure.
 *  This structure should be aligned to 128 byte boundary,i.e., the last 7 bits
 *  in the allocated base address should be zeroes.
 *
 *  NOTE 2: The "IDMJPGE_TIGEM_DynamicParams" structure defined below contains
 *  a pointer to this structure of type "IDMJPGE_TIGEM_CustomHuffmanTables" -
 *  "IDMJPGE_TIGEM_CustomHuffmanTables *huffmanTable".
 *  This field is used to pass custom Huffman tables to the JPEG Encoder.
 *  The application just needs to initialize this field appropriately.
 *  However, if the application wishes to use the Standard Huffman Tables, then
 *  there are 2 ways of informing this to the JPEG Encoder -
 *  (a) by using the base class only "IIMGENC1_DynamicParams" and not the
 *  extended class "IDMJPGE_TIGEM_DynamicParams", in which case the size field
 *  in the DynamicParams structure should be set to the size of
 *  "IIMGENC1_DynamicParams".
 *  (b) by using the extended class "IDMJPGE_TIGEM_DynamicParams", but by
 *  setting the field "IDMJPGE_TIGEM_CustomHuffmanTables *huffmanTable" to NULL
 */

  typedef struct IDMJPGE_TIGEM_CustomHuffmanTables
  {
    /* =======Part 1: These are the tables required by the VLC Module=========*/
    /* ============================L U M A (Y)================================*/
    /* DC: The array "lum_dc_vlc" defines VLC table for the luma DC component.
     * The index is the size in bits of the DC coefficient. This can be from 0
     * to 11 (0x0 to 0xB). In each 32 bit word, the upper 16 bits define the
     * length of the codeword and the lower 16 bits define the actual codeword
     */
    XDAS_UInt32 lum_dc_vlc[12];
    /* AC: The array "lum_ac_vlc" defines VLC table for the luma AC component.
     * This is a 2D array where the column index is the run-length of zeros
     * that precede the non-zero quantized DCT coefficient. This can be from 0
     * to 15 (0x0 to 0xF). The row index is the size in bits of the non-zero
     * coefficient that followed the run of zeros. This can be from 1 to 10
     * (0x1 to 0xA). This implies rows 1 to 10 will comprise of 16 X 10 = 160
     * codewords. Row 0 contains 2 special codewords - 0x00 and 0xF0. x00
     * represents an End of Block (EOB), which indicates that there are no more
     * non-zero AC coefficients in this component, and that the encoder will
     * move on to the next component. xF0 represents a Zero Run Length (ZRL)
     * which indicates that there was a run of > 15 zeros. This codeword
     * represents a run of 15 zeros, and will be followed by another codeword
     * that indicates another ZRL or a normal run + size codeword. So, there
     * are in total 162 possible AC code words. The standard huffman AC tables
     * include all 162. Rows 11 to 15 should contains all zeroes.In each 32
     * bit word, the upper 16 bits define the length of the codeword and the
     * lower 16 bits define the actual codeword
     */
    XDAS_UInt32 lum_ac_vlc[16][16];
    /* ============================L U M A (Y)================================*/

    /* ========================C H R O M A (Cb Cr)============================*/
    /* DC: The array "chm_dc_vlc" defines VLC table for the chroma DC component.
     * The index is the size in bits of the DC coefficient. This can be from 0
     * to 11 (0x0 to 0xB). In each 32 bit word, the upper 16 bits define the
     * length of the codeword and the lower 16 bits define the actual codeword
     */
    XDAS_UInt32 chm_dc_vlc[12];
    /* AC: The array "chm_ac_vlc" defines VLC table for the chroma AC component.
     * This is a 2D array where the column index is the run-length of zeros
     * that precede the non-zero quantized DCT coefficient. This can be from 0
     * to 15 (0x0 to 0xF). The row index is the size in bits of the non-zero
     * coefficient that followed the run of zeros. This can be from 1 to 10
     * (0x1 to 0xA). This implies rows 1 to 10 will comprise of 16 X 10 = 160
     * codewords. Row 0 contains 2 special codewords - 0x00 and 0xF0. x00
     * represents an End of Block (EOB), which indicates that there are no more
     * non-zero AC coefficients in this component, and that the encoder will
     * move on to the next component. xF0 represents a Zero Run Length (ZRL)
     * which indicates that there was a run of > 15 zeros. This codeword
     * represents a run of 15 zeros, and will be followed by another codeword
     * that indicates another ZRL or a normal run + size codeword. So, there
     * are in total 162 possible AC code words. The standard huffman AC tables
     * include all 162. Rows 11 to 15 should contains all zeroes.In each 32
     * bit word, the upper 16 bits define the length of the codeword and the
     * lower 16 bits define the actual codeword
     */
    XDAS_UInt32 chm_ac_vlc[16][16];
    /* ========================C H R O M A (Cb Cr)============================*/

    /* =======Part 2: This part is required to create the DHT Marker==========*/
    /* ============================L U M A (Y)================================*/
    /* DC: The array "lum_dc_codelens" defines the number of codewords
     * corresponsing to a coded symbol of given code length. The variable
     * "lum_dc_ncodes" defines the size of "lum_dc_codelens", i.e., 16
     */
    XDAS_UInt8  lum_dc_codelens[16];
    XDAS_UInt16 lum_dc_ncodes;
    /* DC: The array "lum_dc_symbols" defines the list of coded symbols.
     * The variable "lum_dc_nsymbols" defines the number of coded symbols OR
     * actual size of "lum_dc_symbols" - maximum 12
     */
    XDAS_UInt8  lum_dc_symbols[12];
    XDAS_UInt16 lum_dc_nsymbols;
    /* AC: The array "lum_ac_codelens" defines the number of codewords
     * corresponsing to a coded symbol of given code length. The variable
     * "lum_ac_ncodes" defines the size of "lum_ac_codelens", i.e., 16
     */
    XDAS_UInt8  lum_ac_codelens[16];
    XDAS_UInt16 lum_ac_ncodes;
    /* AC: The array "lum_ac_symbols" defines the list of coded symbols.
     * The variable "lum_ac_nsymbols" defines the number of coded symbols OR
     * actual size of "lum_ac_symbols" - maximum 162
     */
    XDAS_UInt8  lum_ac_symbols[162];
    XDAS_UInt16 lum_ac_nsymbols;
    /* ============================L U M A (Y)================================*/

    /* ========================C H R O M A (Cb Cr)============================*/
    /* DC: The array "chm_dc_codelens" defines the number of codewords
     * corresponsing to a coded symbol of given code length. The variable
     * "chm_dc_ncodes" defines the size of "chm_dc_codelens", i.e., 16
     */
    XDAS_UInt8  chm_dc_codelens[16];
    XDAS_UInt16 chm_dc_ncodes;
    /* DC: The array "chm_dc_symbols" defines the list of coded symbols.
     * The variable "chm_dc_nsymbols" defines the number of coded symbols OR
     * actual size of "chm_dc_symbols" - maximum 12
     */
    XDAS_UInt8  chm_dc_symbols[12];
    XDAS_UInt16 chm_dc_nsymbols;
    /* AC: The array "chm_ac_codelens" defines the number of codewords
     * corresponsing to a coded symbol of given code length. The variable
     * "chm_ac_ncodes" defines the size of "chm_ac_codelens", i.e., 16
     */
    XDAS_UInt8  chm_ac_codelens[16];
    XDAS_UInt16 chm_ac_ncodes;
    /* AC: The array "chm_ac_symbols" defines the list of coded symbols.
     * The variable "chm_ac_nsymbols" defines the number of coded symbols OR
     * actual size of "chm_ac_symbols" - maximum 162
     */
    XDAS_UInt8  chm_ac_symbols[162];
    XDAS_UInt16 chm_ac_nsymbols;
    /* ========================C H R O M A (Cb Cr)============================*/
  }
  IDMJPGE_TIGEM_CustomHuffmanTables;
#endif
/*
 *  ======== IDMJPGE_TIGEM_CustomQuantTables ========
 *  JPEG Encoder Custom Quantization Tables - This structure defines the custom
 *  quantization tables for both Luma & Chroma that need to be passed to
 *  the JPEG Encoder. This will be used in the Quantization Module.
 *  The format is as specified in RFC 2035 (RTP Payload Format for
 *  JPEG-compressed Video) and also in Table K.1 & K.2 of JPEG Spec.
 *
 *  NOTE: The "IDMJPGE_TIGEM_DynamicParams" structure defined below contains
 *  a pointer to this structure of type "IDMJPGE_TIGEM_CustomQuantTables" -
 *  "IDMJPGE_TIGEM_CustomQuantTables *quantTable".
 *  This field is used to pass custom Quantization tables to the JPEG Encoder.
 *  The application just needs to initialize this field appropriately.
 *  However, if the application wishes to use the Standard Quantization Tables,
 *  then there are 2 ways of informing this to the JPEG Encoder -
 *  (a) by using the base class only "IIMGENC1_DynamicParams" and not the
 *  extended class "IDMJPGE_TIGEM_DynamicParams", in which case the size field
 *  in the DynamicParams structure should be set to the size of
 *  "IIMGENC1_DynamicParams".
 *  (b) by using the extended class "IDMJPGE_TIGEM_DynamicParams", but by
 *  setting the field "IDMJPGE_TIGEM_CustomQuantTables *quantTable" to NULL
 */
  typedef struct IDMJPGE_TIGEM_CustomQuantTables
  {
/*-------------------------------------------------------------------*/
/*							L U M A (Y)				  				 */
/*-------------------------------------------------------------------*/
    XDAS_UInt16 lum_quant_tab[64];
/*-------------------------------------------------------------------*/
/*					C H R O M A (Cb Cr)				  				 */
/*-------------------------------------------------------------------*/
    XDAS_UInt16 chm_quant_tab[64];

  }
  IDMJPGE_TIGEM_CustomQuantTables;

/*-------------------------------------------------------------------*/
/*						IJPEGDEC_Handle				  				 */
/*-------------------------------------------------------------------*/
typedef struct IDMJPGEPP_TIGEM_Obj *   IDMJPGE_TIGEM_Handle;

typedef struct IDMJPGEPP_TIGEM_Obj
{
  struct IDMJPGE_TIGEM_Fxns  * fxns;
} IDMJPGEPP_TIGEM_Obj;

typedef struct IDMJPGE_TIGEM_Params
{
	IIMGENC1_Params         params;
/*-------------------------------------------------------------------*/
/* Thumbnail_H_size_App = Max Horizontal resolution for APP thumbnail*/
/* Thumbnail_V_size_App = Max Vertical resolution for APP thumbnail  */
/*-------------------------------------------------------------------*/
	XDAS_UInt16    	MaxThumbnail_H_size_App0;
	XDAS_UInt16    	MaxThumbnail_V_size_App0;
	XDAS_UInt16    	MaxThumbnail_H_size_App1;
	XDAS_UInt16    	MaxThumbnail_V_size_App1;
	XDAS_UInt16     MaxThumbnail_H_size_App13;
	XDAS_UInt16    	MaxThumbnail_V_size_App13;
} IDMJPGE_TIGEM_Params;


typedef struct IDMJPGE_TIGEM_DynamicParams
{
	IIMGENC1_DynamicParams  params;

	XDAS_Int32 captureHeight;
	XDAS_Int32 DRI_Interval ;
#ifdef iVLCD
	IDMJPGE_TIGEM_CustomHuffmanTables *huffmanTable;
#endif
	IDMJPGE_TIGEM_CustomQuantTables *quantTable;
/*-------------------------------------------------------------------*/
/*				0: no thumbnail to be added In APP data              */
/*				1: thumbnail needs to be appended in 1'st APP data   */
/*				2: thumbnail needs to be appended in 2'nd APP Data   */
/*-------------------------------------------------------------------*/
	XDAS_UInt8    Thumbnail_Index_App0;
	XDAS_UInt8    Thumbnail_Index_App1;
	XDAS_UInt8    Thumbnail_Index_App13;
/*-------------------------------------------------------------------*/
/*				APPN0_numBufs = number of APP0 buffers				 */
/*				APPN1_numBufs = number of APP1 buffers				 */
/*				APPN13_numBufs = number of APP13 buffers			 */
/*-------------------------------------------------------------------*/
	XDAS_UInt8	   APPN0_numBufs;
	XDAS_UInt8	   APPN1_numBufs;
	XDAS_UInt8	   APPN13_numBufs;
/*-------------------------------------------------------------------*/
/*  APPN0_startBuf = start buffer number in inbufs for APP0 buffers  */
/*  APPN1_startBuf = start buffer number in inbufs for APP1 buffers  */
/*  APPN13_startBuf= start buffer number in inbufs for APP13 buffers */
/*-------------------------------------------------------------------*/
	XDAS_UInt8	   APPN0_startBuf;
	XDAS_UInt8	   APPN1_startBuf;
	XDAS_UInt8	   APPN13_startBuf;
/*-------------------------------------------------------------------*/
/*COMMENT_startBuf = start buffer number in inbufs for Comment buffer*/
/*    COMMENT_insert = Decide whether to insert comment or not        */
/*-------------------------------------------------------------------*/
	XDAS_UInt8	   COMMENT_startBuf;
	XDAS_UInt8	   COMMENT_insert;

/*-------------------------------------------------------------------*/
/*	Thumbnail_H_size_App = Horizontal resolution for APP thumbnail   */
/*	Thumbnail_V_size_App = Vertical resolution for APP  thumbnail    */
/*-------------------------------------------------------------------*/

	XDAS_UInt16    Thumbnail_H_size_App1;
	XDAS_UInt16    Thumbnail_V_size_App1;
	XDAS_UInt16    Thumbnail_H_size_App0;
	XDAS_UInt16    Thumbnail_V_size_App0;
	XDAS_UInt16    Thumbnail_H_size_App13;
	XDAS_UInt16    Thumbnail_V_size_App13;


} IDMJPGE_TIGEM_DynamicParams;


typedef struct IDMJPGE_TIGEM_InArgs
{
	IIMGENC1_InArgs         inArgs;

} IDMJPGE_TIGEM_InArgs;

typedef struct IDMJPGE_TIGEM_Status
{
  IIMGENC1_Status         status;
} IDMJPGE_TIGEM_Status;

typedef struct IDMJPGE_TIGEM_OutArgs
{
  IIMGENC1_OutArgs        outArgs;
} IDMJPGE_TIGEM_OutArgs;




extern IALG_Fxns      DMJPGE_TIGEM_IALG;

typedef struct IDMJPGE_TIGEM_Fxns
{
  IIMGENC1_Fxns        iimgenc1;
} IDMJPGE_TIGEM_Fxns;

extern IDMJPGE_TIGEM_Fxns         DMJPGE_TIGEM_IDMJPGE;

#ifdef __cplusplus
}
#endif

#endif
