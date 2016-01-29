/**
   Author:	wujf
   Date:	2015/03/26
*/

#include "SWPSMuxer.h"

CSWPSMuxer::CSWPSMuxer()
{
	m_pPesMuxer = new CSWPESMuxer();
}

CSWPSMuxer::~CSWPSMuxer()
{
	SAFE_DELETE(m_pPesMuxer);
}

VOID CSWPSMuxer::SetFrameRate(INT iFrameRate)
{
	m_iFrameRate = iFrameRate;
}

VOID CSWPSMuxer::SetBitRate(INT iBitRate)
{
	m_iBitRate = iBitRate;
}

INT CSWPSMuxer::InitCRC32Table()
{
	unsigned int i, j, k;

	for( i = 0; i < 256; i++ )
	{
		k = 0;
		for( j = (i << 24) | 0x800000; j != 0x80000000; j <<= 1 )
			k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);

		m_rgCRC32Table[i] = k;
	}
	return S_OK;
}

unsigned int CSWPSMuxer::GetCRC32(unsigned char *pBuffer, unsigned int iBuffer)
{
	/* CRC32 */
	unsigned int i_crc = 0xffffffff;
	unsigned int i = 0;
	for( i = 0; i < iBuffer; i++ )
        i_crc = (i_crc << 8) ^ m_rgCRC32Table[((i_crc >> 24) ^ pBuffer[i]) & 0xff];

	return i_crc;
}

INT CSWPSMuxer::InitMuxer(VOID *pParam)
{
	if(pParam == NULL)
	{
		return E_FAIL;
	}
	INIT_PSMUXER_PARAM_T *pPsMuxerParam = (INIT_PSMUXER_PARAM_T *)pParam;

	m_lSCRBase 		= pPsMuxerParam->lSCRBase;
	m_iFrameRate	= pPsMuxerParam->iFrameRate;
	m_iBitRate 		= pPsMuxerParam->iBitRate;

	InitCRC32Table();

	return S_OK;
}

//PS包头封装
INT CSWPSMuxer::PackHeader(BLOCK_T **pBlock)
{
	pack_header_t PackHeader;
	pack_header_t *pPackHeaderBuf = new pack_header_t;
	if(pPackHeaderBuf == NULL)
	{
		return E_OUTOFMEMORY;
	}
	BLOCK_T *pCurBlock = new BLOCK_T;
	if(pCurBlock == NULL)
	{
		SAFE_DELETE(pPackHeaderBuf);
		return E_OUTOFMEMORY;
	}

	//program_mux_rate 以50字节为单位
	unsigned int iBitRate = (m_iBitRate + (8 * 50) - 1) / (8 * 50);

	PackHeader.pack_start_code[0]					= 0x00;
	PackHeader.pack_start_code[1]					= 0x00;
	PackHeader.pack_start_code[2]					= 0x01;
	PackHeader.pack_start_code[3]					= 0xBA;
	PackHeader.fix_bit 		   						= 1;		//always 01
	PackHeader.system_clock_reference_base32_30 	= (m_lSCRBase >> 30) & 0x07;
	PackHeader.marker_bit0	   						= 1;
	PackHeader.system_clock_reference_base29_15 	= (m_lSCRBase >> 15) & 0x7FFF;
	PackHeader.marker_bit1		   					= 1;
	PackHeader.system_clock_reference_base14_0 		= (m_lSCRBase) & 0x7FFF;
	PackHeader.marker_bit2		   					= 1;
	PackHeader.system_clock_reference_extension		= 0;	//unuseful
	PackHeader.marker_bit3 	   						= 1;
	PackHeader.program_mux_rate						= iBitRate;
	PackHeader.marker_bit4		   					= 1;
	PackHeader.marker_bit5		   					= 1;
	PackHeader.reserved		   						= 0x1f;
	PackHeader.pack_stuffing_length					= 0;

	BIT_BUFFER_T PackHeaderBitBuffer;
	BitInitWrite(&PackHeaderBitBuffer, sizeof(pack_header_t), (VOID *)pPackHeaderBuf);


	BitWrite(&PackHeaderBitBuffer, 8, PackHeader.pack_start_code[0]);
	BitWrite(&PackHeaderBitBuffer, 8, PackHeader.pack_start_code[1]);
	BitWrite(&PackHeaderBitBuffer, 8, PackHeader.pack_start_code[2]);
	BitWrite(&PackHeaderBitBuffer, 8, PackHeader.pack_start_code[3]);
	BitWrite(&PackHeaderBitBuffer, 2, PackHeader.fix_bit);
	BitWrite(&PackHeaderBitBuffer, 3, PackHeader.system_clock_reference_base32_30);
	BitWrite(&PackHeaderBitBuffer, 1, PackHeader.marker_bit0);
	BitWrite(&PackHeaderBitBuffer, 15, PackHeader.system_clock_reference_base29_15);
	BitWrite(&PackHeaderBitBuffer, 1, PackHeader.marker_bit1);
	BitWrite(&PackHeaderBitBuffer, 15, PackHeader.system_clock_reference_base14_0);
	BitWrite(&PackHeaderBitBuffer, 1, PackHeader.marker_bit2);
	BitWrite(&PackHeaderBitBuffer, 9, PackHeader.system_clock_reference_extension);
	BitWrite(&PackHeaderBitBuffer, 1, PackHeader.marker_bit3);
	BitWrite(&PackHeaderBitBuffer, 22, PackHeader.program_mux_rate);
	BitWrite(&PackHeaderBitBuffer, 1, PackHeader.marker_bit4);
	BitWrite(&PackHeaderBitBuffer, 1, PackHeader.marker_bit5);
	BitWrite(&PackHeaderBitBuffer, 5, PackHeader.reserved);
	BitWrite(&PackHeaderBitBuffer, 3, PackHeader.pack_stuffing_length);

	pCurBlock->pNext	= NULL;
	pCurBlock->pBuffer	= (CHAR *)pPackHeaderBuf;
	pCurBlock->iBuffer	= sizeof(pack_header_t);
	pCurBlock->iType	= BLOCK_TYPE_PACK_HEADER;

	BlockChainAppend(pBlock, pCurBlock); 

	return S_OK;
}

INT CSWPSMuxer::SystemHeader(BLOCK_T **pBlock)
{

	system_header_t SysHeader;
	system_header_t *pSysHeaderBuf = new system_header_t;
	if(pSysHeaderBuf == NULL)
	{
		return E_OUTOFMEMORY;
	}
	BLOCK_T *pCurBlock = new BLOCK_T;
	if(pCurBlock == NULL)
	{
		SAFE_DELETE(pSysHeaderBuf);
		return E_OUTOFMEMORY;
	}

	//program_mux_rate 以50字节为单位
	unsigned int iBitRate = (m_iBitRate + (8 * 50) - 1) / (8 * 50);
	
	SysHeader.system_header_start_code[0] 	= 0x00;
	SysHeader.system_header_start_code[1] 	= 0x00;
	SysHeader.system_header_start_code[2] 	= 0x01;
	SysHeader.system_header_start_code[3] 	= 0xBB;		
	SysHeader.header_length	 				= 0x0C;
	SysHeader.marker_bit0					= 1;
	SysHeader.rate_bound					= iBitRate;
	SysHeader.marker_bit1 					= 1;
	SysHeader.audio_bound					= 1;
	SysHeader.fixed_flag					= 0;
	SysHeader.CSPS_flag						= 0;
	SysHeader.system_audio_lock_flag		= 1;
	SysHeader.system_video_lock_flag		= 1;
	SysHeader.marker_bit2 					= 1;
	SysHeader.video_bound					= 1;
	SysHeader.packet_rate_restriction_flag	= 0;
	SysHeader.reserved_bits					= 0x7f;
	SysHeader.video_stream_id				= VIDEO_STREAM_ID(0);
	SysHeader.video_fit_bit					= 3;
	SysHeader.video_P_STD_buffer_bound_scale= 1;
	SysHeader.video_P_STD_buffer_size_bound	= 0x80; 
	SysHeader.audio_stream_id				= AUDIO_STREAM_ID(0);
	SysHeader.audio_fit_bit					= 3;
	SysHeader.audio_P_STD_buffer_bound_scale= 0;
	SysHeader.audio_P_STD_buffer_size_bound	= 0x08;


	BIT_BUFFER_T SysHeaderBitBuffer;
	BitInitWrite(&SysHeaderBitBuffer, sizeof(system_header_t), (VOID *)pSysHeaderBuf);

	BitWrite(&SysHeaderBitBuffer, 8, SysHeader.system_header_start_code[0]);
	BitWrite(&SysHeaderBitBuffer, 8, SysHeader.system_header_start_code[1]);
	BitWrite(&SysHeaderBitBuffer, 8, SysHeader.system_header_start_code[2]);
	BitWrite(&SysHeaderBitBuffer, 8, SysHeader.system_header_start_code[3]);
	BitWrite(&SysHeaderBitBuffer, 16, SysHeader.header_length);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.marker_bit0);
	BitWrite(&SysHeaderBitBuffer, 22, SysHeader.rate_bound);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.marker_bit1);
	BitWrite(&SysHeaderBitBuffer, 6, SysHeader.audio_bound);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.fixed_flag);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.CSPS_flag);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.system_audio_lock_flag);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.system_video_lock_flag);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.marker_bit2);
	BitWrite(&SysHeaderBitBuffer, 5, SysHeader.video_bound);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.packet_rate_restriction_flag);
	BitWrite(&SysHeaderBitBuffer, 7, SysHeader.reserved_bits);
	BitWrite(&SysHeaderBitBuffer, 8, SysHeader.video_stream_id);
	BitWrite(&SysHeaderBitBuffer, 2, SysHeader.video_fit_bit);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.video_P_STD_buffer_bound_scale);
	BitWrite(&SysHeaderBitBuffer, 13, SysHeader.video_P_STD_buffer_size_bound);
	BitWrite(&SysHeaderBitBuffer, 8, SysHeader.audio_stream_id);
	BitWrite(&SysHeaderBitBuffer, 2, SysHeader.audio_fit_bit);
	BitWrite(&SysHeaderBitBuffer, 1, SysHeader.audio_P_STD_buffer_bound_scale);
	BitWrite(&SysHeaderBitBuffer, 13, SysHeader.audio_P_STD_buffer_size_bound);


	pCurBlock->pNext	= NULL;
	pCurBlock->pBuffer	= (CHAR *)pSysHeaderBuf;
	pCurBlock->iBuffer	= sizeof(system_header_t);
	pCurBlock->iType	= BLOCK_TYPE_SYS_HEADER;

	BlockChainAppend(pBlock, pCurBlock); 
	
	return S_OK;
}

INT CSWPSMuxer::AvcTimeHrdDescriptor(AVC_timing_HRD_descriptor_t *pAVC_HRD_DescBuf)
{
	if(pAVC_HRD_DescBuf == NULL)
		return E_FAIL;
	AVC_timing_HRD_descriptor_t AVC_HRD_Desc;

	AVC_HRD_Desc.descriptor_tag						= 0x2A;		//42
	AVC_HRD_Desc.descriptor_length					= 0x0F;		//15
	AVC_HRD_Desc.hrd_management_valid_flag			= 0x00;
	AVC_HRD_Desc.reserved0							= 0x3F;
	AVC_HRD_Desc.picture_and_timing_info_present	= 0x01;
	AVC_HRD_Desc._90kHz_flag						= 0x01;
	AVC_HRD_Desc.reserved1							= 0x7F;
	AVC_HRD_Desc.N									= 0x01;
	AVC_HRD_Desc.K									= 0x12C;	//300
	AVC_HRD_Desc.num_units_in_tick					= 0x960;	//2400
	AVC_HRD_Desc.fixed_frame_rate_flag				= 0x00;
	AVC_HRD_Desc.temporal_poc_flag					= 0x00;
	AVC_HRD_Desc.picture_to_display_conversion_flag = 0x01;
	AVC_HRD_Desc.reserved							= 0x1F;

	BIT_BUFFER_T AVC_HRDBitBuffer;
	BitInitWrite(&AVC_HRDBitBuffer, sizeof(AVC_timing_HRD_descriptor_t), (VOID *)pAVC_HRD_DescBuf);
	BitWrite(&AVC_HRDBitBuffer, 8, AVC_HRD_Desc.descriptor_tag);
	BitWrite(&AVC_HRDBitBuffer, 8, AVC_HRD_Desc.descriptor_length);
	BitWrite(&AVC_HRDBitBuffer, 1, AVC_HRD_Desc.hrd_management_valid_flag);
	BitWrite(&AVC_HRDBitBuffer, 6, AVC_HRD_Desc.reserved0);
	BitWrite(&AVC_HRDBitBuffer, 1, AVC_HRD_Desc.picture_and_timing_info_present);
	BitWrite(&AVC_HRDBitBuffer, 1, AVC_HRD_Desc._90kHz_flag);
	BitWrite(&AVC_HRDBitBuffer, 7, AVC_HRD_Desc.reserved1);
	BitWrite(&AVC_HRDBitBuffer, 32, AVC_HRD_Desc.N);
	BitWrite(&AVC_HRDBitBuffer, 32, AVC_HRD_Desc.K);
	BitWrite(&AVC_HRDBitBuffer, 32, AVC_HRD_Desc.num_units_in_tick);
	BitWrite(&AVC_HRDBitBuffer, 1, AVC_HRD_Desc.fixed_frame_rate_flag);
	BitWrite(&AVC_HRDBitBuffer, 1, AVC_HRD_Desc.temporal_poc_flag);
	BitWrite(&AVC_HRDBitBuffer, 1, AVC_HRD_Desc.picture_to_display_conversion_flag);
	BitWrite(&AVC_HRDBitBuffer, 5, AVC_HRD_Desc.reserved);


	return S_OK;
}

INT CSWPSMuxer::PSMHeader(BLOCK_T **pBlock)
{
	psm_header_t PsmHeader;
	psm_header_t *pPsmHeaderBuf = new psm_header_t;
	unsigned int CRC32 = 0xFFFFFFFF;
	if(pPsmHeaderBuf == NULL)
	{
		return E_OUTOFMEMORY;
	}
	swpa_memset(pPsmHeaderBuf, 0, sizeof(psm_header_t));
	BLOCK_T *pCurBlock = new BLOCK_T;
	if(pCurBlock == NULL)
	{
		SAFE_DELETE(pPsmHeaderBuf);
		return E_OUTOFMEMORY;
	}

	PsmHeader.packet_start_code_prefix[0] 			= 0x00;
	PsmHeader.packet_start_code_prefix[1] 			= 0x00;
	PsmHeader.packet_start_code_prefix[2] 			= 0x01;
	PsmHeader.map_stream_id							= 0xBC;		/* always 0xBC */
	PsmHeader.program_stream_map_length				= 0x23;		//35
	PsmHeader.current_next_indicator				= 0x01;
	PsmHeader.reserved1								= 0x03;
	PsmHeader.program_stream_map_version			= 0x00;
	PsmHeader.reserved2								= 0x7F;
	PsmHeader.marker_bit							= 0x01;
	PsmHeader.program_stream_info_length			= 0x00;
	PsmHeader.elementary_stream_map_length			= 0x19;		//25
	PsmHeader.audio_stream_type						= 0x90;		/* video H264 265 stream type */
	PsmHeader.audio_elementary_stream_id			= 0xC0;
	PsmHeader.audio_elementary_stream_info_length 	= 0x00;
	PsmHeader.video_stream_type						= 0x1B;		/* video H264 265 stream type */
	PsmHeader.video_elementary_stream_id			= 0xE0;
	PsmHeader.video_elementary_stream_info_length 	= 0x0F;

	// PsmHeader.CRC_32[0]								= 0x45;
	// PsmHeader.CRC_32[1]								= 0xBD;
	// PsmHeader.CRC_32[2]								= 0xDC;
	// PsmHeader.CRC_32[3]								= 0xF4;
	PsmHeader.CRC_32[0]								= 0x00;
	PsmHeader.CRC_32[1]								= 0x00;
	PsmHeader.CRC_32[2]								= 0x00;
	PsmHeader.CRC_32[3]								= 0x00;

	BIT_BUFFER_T PsmHeaderBitBuffer; 
	BitInitWrite(&PsmHeaderBitBuffer, sizeof(psm_header_t), (VOID *)pPsmHeaderBuf);
	
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.packet_start_code_prefix[0]);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.packet_start_code_prefix[1]);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.packet_start_code_prefix[2]);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.map_stream_id);
	BitWrite(&PsmHeaderBitBuffer, 16, PsmHeader.program_stream_map_length);
	BitWrite(&PsmHeaderBitBuffer, 1, PsmHeader.current_next_indicator);
	BitWrite(&PsmHeaderBitBuffer, 2, PsmHeader.reserved1);
	BitWrite(&PsmHeaderBitBuffer, 5, PsmHeader.program_stream_map_version);
	BitWrite(&PsmHeaderBitBuffer, 7, PsmHeader.reserved2);
	BitWrite(&PsmHeaderBitBuffer, 1, PsmHeader.marker_bit);
	BitWrite(&PsmHeaderBitBuffer, 16, PsmHeader.program_stream_info_length);
	BitWrite(&PsmHeaderBitBuffer, 16, PsmHeader.elementary_stream_map_length);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.audio_stream_type);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.audio_elementary_stream_id);
	BitWrite(&PsmHeaderBitBuffer, 16, PsmHeader.audio_elementary_stream_info_length);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.video_stream_type);
	BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.video_elementary_stream_id);
	BitWrite(&PsmHeaderBitBuffer, 16, PsmHeader.video_elementary_stream_info_length);

	AvcTimeHrdDescriptor(&pPsmHeaderBuf->AVC_HRD_desc);
	CRC32 = GetCRC32((unsigned char *)pPsmHeaderBuf, sizeof(psm_header_t));

	BitWrite(&PsmHeaderBitBuffer, 32, CRC32);
	// BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.CRC_32[0]);
	// BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.CRC_32[1]);
	// BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.CRC_32[2]);
	// BitWrite(&PsmHeaderBitBuffer, 8, PsmHeader.CRC_32[3]);
	
	pCurBlock->pNext	= NULL;
	pCurBlock->pBuffer	= (CHAR *)pPsmHeaderBuf;
	pCurBlock->iBuffer	= sizeof(psm_header_t);
	pCurBlock->iType	= BLOCK_TYPE_PSM_HEADER;

	BlockChainAppend(pBlock, pCurBlock); 
	
	return S_OK;
}

VOID CSWPSMuxer::UpdateSCRBase()
{
	m_lSCRBase += (90000 / m_iFrameRate);
}

INT CSWPSMuxer::Mux(BLOCK_T **pBlock)
{
	if(pBlock == NULL || *pBlock == NULL)
	{
		return E_FAIL;
	}
	BLOCK_T *pSrcBlock = *pBlock;
	BLOCK_T *pDstBlock = NULL;

	PackHeader(&pDstBlock);
	if(pSrcBlock->iType == BLOCK_TYPE_H264_H265_IDR_FRAME)
	{
		SystemHeader(&pDstBlock);
		PSMHeader(&pDstBlock);
	}

	m_pPesMuxer->SetPTS(m_lSCRBase);
	m_pPesMuxer->Mux(pBlock);

	BlockChainAppend(&pDstBlock, *pBlock);

	UpdateSCRBase();

	*pBlock = pDstBlock;

	return S_OK;
}
