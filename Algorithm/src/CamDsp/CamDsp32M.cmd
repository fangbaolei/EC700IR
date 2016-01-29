/****************************************************************************/

/*  lnk.cmd   v1.01                                                         */

/*  Copyright (c) 1996-1997  Texas Instruments Incorporated                 */

/****************************************************************************/

/* -cr */

-l ./DSPBIOS/CamDsp32Mcfg.cmd

-l ../DSPLink/Csp/Csl/lib/dspcsl_davinci.lib
-l ../DSPLink/Csp/Csl/lib/csl_sysservicesdavinci.lib

-l ./lib/JpegEnc/dmjpge_tigem.l64P

-l ./lib/H264Enc/edma3_rm_bios.lib
-l ./lib/H264Enc/hdvicp_ti_api_c64Plus.lib
-l ./lib/H264Enc/rman.a64P
-l ./lib/H264Enc/nullres.a64P
-l ./lib/H264Enc/edma3.a64P
-l ./lib/H264Enc/edma3Chan.a64P
-l ./lib/H264Enc/hdvicp.a64P
-l ./lib/H264Enc/h264fhdvenc_tii.l64P

-l ./lib/DmaCopy/dman3.a64P
-l ./lib/DmaCopy/acpy3.a64P

-c

MEMORY
{
	INT_MEM     : o = 0x11818000 l = 0x00010000 /* 64K */
	DSPLINKRAM  : o = 0x1182f000 l = 0x00001000 /*  4K */
	USERRAM     : o = 0x11828000 l = 0x00005000 /* 20K */

	/* ----------- H.264 ----------- */

	KLD1_UMAP1:  o = 0x11600000  l = 0x00004000 /* 16K */

	KLD1_RW_0:   o = 0x40604000  l = 0x00002000 /* 8K */
	KLD1_RW_1:   o = 0x40606000  l = 0x00002000 /* 8K */
	KLD1_RW_2:   o = 0x40608000  l = 0x00002000 /* 8K */
	KLD1_RW_3:   o = 0x4060A000  l = 0x00002000 /* 8K */

	KLD1_RW_4:   o = 0x4060C000  l = 0x00002000 /* 8K */
	KLD1_RW_5:   o = 0x40613800  l = 0x00003800 /* 14K */ 

	KLD1_RW_6:   o = 0x4061D000  l = 0x00001800 /* 6KB */
}

SECTIONS
{
	.userOnChipRAM > USERRAM

	.sysReset > RESET_VECTOR

	/* ----------- H.264 ----------- */

    .data:H264EncStateSect /*:{ _H264EncStateTable = .; . += 0x4000; }*/ > KLD1_UMAP1

    .data:H264LpfOutSect0  /*:{ _H264LpfOutTable0Y = .; . += 0x2000; }*/ > KLD1_RW_0
    .data:H264LpfOutSect1  /*:{ _H264LpfOutTable0UV= .; . += 0x2000; }*/ > KLD1_RW_1
    .data:H264LpfOutSect2  /*:{ _H264LpfOutTable1Y = .; . += 0x2000; }*/ > KLD1_RW_2
    .data:H264LpfOutSect3  /*:{ _H264LpfOutTable1UV= .; . += 0x2000; }*/ > KLD1_RW_3

    .data:H264PredTopRowSect  /* :{ _H264PredTopRowTable   = .; . += 0x2000; }*/ > KLD1_RW_4
    .data:H264IpePredTopRowSect/*:{ _H264IpePredTopRowTable= .; . += 0x3800; }*/ > KLD1_RW_5 

	.data:H264ChromaConvMBDataSect > KLD1_RW_6

	.intDataMem     >       INT_MEM
	.CoProcData     >       PRAM

	GROUP : {
            /* MB Loop Functions */
     		.text:_H264EncodePFlow1080iStage1 
            .text:_H264FHDVENC_TII_encIsr
            .text:_H264EncodePFlow1080i
            .text:_dma_reint_l2coproc
    		.text:_CalculateMBSkipRun
		    .text:_H264FHDVENC_TI_DMA_AIUPDATE
			.text:_execflag_SLOT8_init
            /*.text:_DMA_setup */
            .text:_decimation_MBs_pair_sa
			.text:_execflagS1_SLOT6_init_S1
            .text:_hdvicp_TI_IpeGetOutputH264Enc_1080i
            .text:_hdvicp_TI_MeGetOutputH264Enc1080i
            .text:_slot4_p1
			.text:_H264VENC_TI_PerceptualRateControl
            .text:_slot4_p3
            .text:_MC_dmasetup
            .text:_slot4_p2
            .text:_hdvicp_TI_CalcDynamicH264Enc_1080
            .text:_slot0_p
            .text:_slot1_p0
            .text:_slot1_p1
            /*.text:_Determine16x16MVPredictorBCD*/    
			.text:_slotME_p3
			.text:_slotME_p0
            .text:_slot2_p
    
            .text:_H264FHDVENC_TI_Ipe_Initqp_1080
            .text:_slot5_p

        .text:_SetRemPrevFlags_opt

        .text:_SetConditionalSkipFlag_opt
            .text:.1080_enc:_Determine16x16MVPredictor
            .text:_hdvicp_TI_EcdDynamicH264Enc_1080
            .text:_slot6_p
            .text:_slot7_p
            .text:_slot9_p
            .text:_slot_RowlevelRC
            .text:_H264FHDVENC_TI_startOfMBProc
            .text:_propogate_count
            
            .text:csl_section:intc:__CSL_intcDispatcher
			.text:_gem_int_handler
        .csl_vect
        .text:_HDVICP_wait
            		.text:_SliceDataDMAOut
    		.text:_SliceReint
        .text:_H264FHDVENC_TI_DMA_ECopy_1D1D
        .text:_H264FHDVENC_TI_DMA_Wait
	.text:_ProvideLeftNeighborToCalc
	.text:_ProvideLeftNeighbor_AfterCalcExec
	 } > PRAM, align = 0x10000


    GROUP : {
        .text:.1080_enc:_H264EncodeIFlow
        .text:_H264FHDVENC_TI_DMA_AIUPDATE_I
        /*.text:_H264EncodePFlow1080i_kernel*/
        /* Other Functions */
        .text:_H264FHDVENC_TII_encStart
        .text:_H264FHDVENC_TI_DMA_INIT
        .text:_H264FHDVENC_TI_GenerateSliceNAL
        .text:.1080_enc:_initCoprocessors
        .text:_H264FHDVENC_TI_M_DMA_ECopy_1D1D_Linked
        .text:algInit:_H264FHDVENC_TII_initObj
        .text:_H264FHDVENC_TI_GeneratePicParamNAL
        .text:algControl:_H264FHDVENC_TII_control
        .text:.1080_enc:_H264EncInit
        .text:_H264FHDVENC_TI_CopyToCompBuffer
        .text:_luma_paddec_loop
        .text:_luma_pad_loop
        .text:_chroma_pad_loop
        .text:.1080_enc:_H264Enc_padVOP
        .text:_H264Enc_padDecVOP
        .text:_H264FHDVENC_TI_DMA_ECopy_2D1D_Linked
        .text:_H264FHDVENC_TI_DMA_ECopy_2D1D_Linked_src_dst_update
        .text:_H264FHDVENC_TII_encEnd
        .text:_H264FHDVENC_TI_GenerateSeqParamNAL
        ..text:.1080_enc:_InitIntc
        .text:_H264FHDVENC_TI_DMA_Open
        .text:_H264FHDVENC_DMAOpen
        .text:.1080_enc:_PutRemEcdData
        .text:.1080_enc:_IPC_HostInit
        .text:algAlloc:_H264FHDVENC_TII_alloc
        .text:_H264FHDVENC_TI_getHRDBufferSize
        /*.text:_sliceBuffFill*/
        .text:_sliceBuffFill_1080
        /*.text:_H264FHDVENC_TI_DMA_Wait*/
        .text:.1080_enc:_IPC_GemInit_Frame
        .text:_hdvicp_TI_IpeInitH264Enc_1080
        .text:_H264FHDVENC_TI_RateCtrlPLR1
        .text:_H264FHDVENC_TI_UpdateStructs
        .text:_hdvicp_TI_EcdInitH264Enc_1080
        .text:_H264FHDVENC_TI_RateCtrlH263TMN5
        .text:_H264FHDVENC_TI_startOfPicture
        .text:_H264FHDVENC_TI_updateForI
        .text:_H264FHDVENC_TI_updateForP
        /*.text:_SetConditionalSkipFlag*/
        /*.text:_H264EncHeaderPack_ECD*/
        .text:_H264EncHeaderPack_ECD_1080
        .text:_H264FHDVENC_TI_decideFrameSkipping
        .text:_H264FHDVENC_TI_endOfPicture
        .text:_H264FHDVENC_TI_getPicAveQ
        .text:_H264FHDVENC_TI_startOfPictureProc
        .text:_H264FHDVENC_TI_updatePrevBitCountForP
        .text:.1080_enc:_H264EncHeaderPack_ECD_DMA
        .text:_H264FHDVENC_TI_RcUpdateInit
        .text:_H264FHDVENC_TI_initHRDBuffer
        .text:_hdvicp_TI_BsInitH264Enc_1080
        .text:_hdvicp_TI_CalcInitH264Enc_1080
        .text:.1080_enc:_COPY1D1D_LINKED
        .text:_H264FHDVENC_TI_DMA_Close
        .text:_H264FHDVENC_TI_RateCtrlPLR3
        .text:_H264FHDVENC_TI_getvbvLevel
        .text:_H264FHDVENC_TI_modifyQByVBV
        .text:_H264FHDVENC_TI_M_DMA_ECopy_1D1D
        .text:_H264FHDVENC_TI_M_DMA_ECopy_2D2D
        .text:.1080_enc:_memcpy_dm6467
        .text:algDeactivate:_H264FHDVENC_TII_deactivate
        .text:process:_H264FHDVENC_TII_encode
        .text:_H264FHDVENC_TI_increaseRCVbv
        .text:_HDVICP_TI_BS_H264_E_RegFill_1080
        .text:_HDVICP_TI_BS_H264_E_SliceDataFill_1080
        /*.text:_commonRegFill_headerpack*/
        .text:_commonRegFill_headerpack_1080
        .text:algFree:_H264FHDVENC_TII_free
        .text:_H264FHDVENC_DMAClose
        .text:_H264FHDVENC_TI_decreaseRCVbv
        /*.text:_h264RegFill*/
        .text:_h264RegFill_1080
        .text:.1080_enc:_DMA_WAIT
        .text:_H264FHDVENC_TII_numAlloc
        .text:.1080_enc:_SetImcopAddress
        .text:_hdvicp_TI_LpfInitH264Enc_1080
        .text:_hdvicp_TI_McInitH264Enc_1080
        .text:_hdvicp_TI_MeInitH264Enc_1080
        .text:_H264FHDVENC_TI_DMA_ECopy_1D1D_Linked
        .text:_H264FHDVENC_TI_DMA_ECopy_2D1D_Linked
        /*.text:_H264FHDVENC_TI_DMA_ECopy_2D2D_Linked*/
        .text:algActivate:_H264FHDVENC_TII_activate
		.text:_chroma_conv
       } > PRAM, align = 0x10000
}
