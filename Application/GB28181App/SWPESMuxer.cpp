/**
   Author:	wujf
   date:	2015/03/27
*/
#include "SWPESMuxer.h"

//#define SPLIT_IDR_FLAG
//#undef SPLIT_IDR_FLAG

CSWPESMuxer::CSWPESMuxer()
{}

CSWPESMuxer::~CSWPESMuxer()
{}

VOID CSWPESMuxer::SetPTS(unsigned long long lPTS)
{
	m_lPTS = lPTS;
}

/**
   拆分IDR帧，传进来的内存段被删除，重新构建新的数据链
*/
INT CSWPESMuxer::SplitIDR(BLOCK_T **pBlock)
{
	if(*pBlock == NULL)
		return E_FAIL;
	BLOCK_T *pOriginalBlock = *pBlock;
	BLOCK_T *pSplitBlock = NULL;
	CHAR *pBuffer = (*pBlock)->pBuffer;
	unsigned int uBuffer = (*pBlock)->iBuffer;
	unsigned int uNaluStartPos = 0, uNaluEndPos = 0, uNaluType = 0;
	unsigned int uBufferOffset = 0;
	INT ret  = 0;

	//将每个NAL单元分离成一个独立数据快
	while(uBufferOffset < uBuffer)
	{
		ret = GetNALU(pBuffer + uBufferOffset, uBuffer - uBufferOffset,
					  &uNaluStartPos, &uNaluEndPos, &uNaluType);
		if(ret < 0)
			break;
		BLOCK_T *pNewBlock = new BLOCK_T;
		CHAR *pNewBuffer = new CHAR[uNaluEndPos - uNaluStartPos];
		memcpy(pNewBuffer, pBuffer + uBufferOffset + uNaluStartPos, uNaluEndPos - uNaluStartPos);
		pNewBlock->pNext = NULL;
		pNewBlock->pBuffer = pNewBuffer;
		pNewBlock->iBuffer = uNaluEndPos - uNaluStartPos;
		BlockChainAppend(&pSplitBlock, pNewBlock);
		uBufferOffset += uNaluEndPos;
	}

	*pBlock = pSplitBlock;
	//删除原数据快
	BlockChainFree(pOriginalBlock);
}

/**
   当数据包超过PES的负载长度字段，数据长度字段就会设为0，这样做Elecard Stream Analyzer分析软件
   就会识别不到封装的格式，因此，当数据大于负载长度时就拆分为多个PES数据包。
*/
INT CSWPESMuxer::GetNALU(CHAR *pBuffer, unsigned int uBuffer, 
						 unsigned int *uNaluStartPos, unsigned int *uNaluEndPos, unsigned int *uNaluType)
{
	if(pBuffer == NULL || uNaluStartPos == NULL || uNaluEndPos == NULL)
		return E_FAIL;
	const unsigned int cntStartCode = 0x00000001;
	const unsigned int cntStartCodeLen = 4;
	unsigned int uTmpNalSize = 0, uNalOffset = 0;
	INT uStartCodePos = 0, uEndCodePos = -1;
	unsigned int uTmpNaluType = 0;
	BOOL fFoundStartCode = FALSE;
	
	for(INT i = 0; (i + 3) < uBuffer; i++)
	{
		unsigned int uEndMarker = 	(pBuffer[i] << 24) | 
									(pBuffer[i+1] << 16) | 
									(pBuffer[i+2] << 8) | 
									(pBuffer[i+3]);
		if(uEndMarker == cntStartCode)
		{
			if(!fFoundStartCode)	//首次发现
			{
				fFoundStartCode = TRUE;
				uStartCodePos = i;
				uTmpNaluType = pBuffer[i + 4] & NALU_TYPE_MASK;
				i += 3;
				continue;
			}
			uEndCodePos = i;
			break;
		}

		if((i - uStartCodePos) > PES_PAYLOAD_SIZE_MAX)
		{
			uEndCodePos = i;
			break;
		}
	}
	if(-1 == uEndCodePos)
	{
		uEndCodePos = uBuffer;
	}

	*uNaluStartPos = uStartCodePos;
	*uNaluEndPos = uEndCodePos;
	*uNaluType = uTmpNaluType;

	return S_OK;
}

INT CSWPESMuxer::Mux(BLOCK_T **pBlock)
{
	if(*pBlock == NULL)
		return E_FAIL;

	BLOCK_T *pDstBlock = NULL;
	pes_header_t PesHeader;

	BOOL fSetPTSFlag = FALSE;

#ifdef SPLIT_IDR_FLAG			//是否拆分IDR，打开则把SPS、PPS、SEI、IDR数据拆分为多个PES包
	SplitIDR(pBlock);
#endif
	BLOCK_T *pVideoBlock = *pBlock;
	BLOCK_T *pTmpBlock = NULL;
	
	while(pVideoBlock != NULL)
	{
		pes_header_t *pPesHeaderBuf = new pes_header_t;
		if(pPesHeaderBuf == NULL)
		{
			return E_OUTOFMEMORY;
		}
		BLOCK_T *pCurBlock = new BLOCK_T;
		if(pCurBlock == NULL)
		{
			SAFE_DELETE(pPesHeaderBuf);
			return E_OUTOFMEMORY;
		}
		unsigned int iPES_header_data_length = 0, iPES_packet_length = 0;
		if(!fSetPTSFlag)
		{
			iPES_header_data_length = 5;
		}
		//当负载长度超过16bit，则设为0值，表示负载未限定长度
		iPES_packet_length = 3 + iPES_header_data_length + pVideoBlock->iBuffer;
		iPES_packet_length = iPES_packet_length > PES_PAYLOAD_SIZE_MAX ? 0 : iPES_packet_length;

		PesHeader.pes_start_code_prefix[0] 		 = 0x00;
		PesHeader.pes_start_code_prefix[1] 		 = 0x00;
		PesHeader.pes_start_code_prefix[2] 		 = 0x01;
		PesHeader.stream_id						 = VIDEO_STREAM_ID(0);
		PesHeader.PES_packet_length				 = iPES_packet_length;	//此字节之后的PES长度
		PesHeader.fix_bit 			  			 = 0x02;		//10
		PesHeader.PES_scrambling_control 		 = 0x00;
		PesHeader.PES_priority		   			 = 0x01;
		PesHeader.data_alignment_indicator		 = 0x00;
		PesHeader.copyright			   			 = 0x00;
		PesHeader.original_or_copy				 = 0x00;
		if(!fSetPTSFlag)
			PesHeader.PTS_DTS_flags				 = 0x02;
		else
			PesHeader.PTS_DTS_flags				 = 0x00;
		PesHeader.ESCR_flag						 = 0x00;
		PesHeader.ES_rate_flag					 = 0x00;
		PesHeader.DSM_trick_mode_flag			 = 0x00;
		PesHeader.additional_copy_info_flag		 = 0x00;
		PesHeader.PES_CRC_flag					 = 0x00;
		PesHeader.PES_extension_flag			 = 0x00;
		PesHeader.PES_header_data_length		 = iPES_header_data_length;		//PT信息长度

		if(!fSetPTSFlag)
		{
			PesHeader.PTS_flags					 = 0x02;			//0010
			PesHeader.PTS32_30					 = (m_lPTS >> 30) & 0x07;
			PesHeader.marker_bit0				 = 1;
			PesHeader.PTS29_15					 = (m_lPTS >> 15) & 0x7FFF;
			PesHeader.marker_bit1				 = 1;
			PesHeader.PTS14_0					 = (m_lPTS) & 0x7FFF;
			PesHeader.marker_bit2				 = 1;
		}

		fSetPTSFlag = TRUE;

		BIT_BUFFER_T PesHeaderBitBuffer; 
		BitInitWrite(&PesHeaderBitBuffer, sizeof(pes_header_t), (VOID *)pPesHeaderBuf);
		BitWrite(&PesHeaderBitBuffer, 8, PesHeader.pes_start_code_prefix[0]);
		BitWrite(&PesHeaderBitBuffer, 8, PesHeader.pes_start_code_prefix[1]);
		BitWrite(&PesHeaderBitBuffer, 8, PesHeader.pes_start_code_prefix[2]);
		BitWrite(&PesHeaderBitBuffer, 8, PesHeader.stream_id);
		BitWrite(&PesHeaderBitBuffer, 16, PesHeader.PES_packet_length);
		BitWrite(&PesHeaderBitBuffer, 2, PesHeader.fix_bit);
		BitWrite(&PesHeaderBitBuffer, 2, PesHeader.PES_scrambling_control);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.PES_priority);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.data_alignment_indicator);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.copyright);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.original_or_copy);
		BitWrite(&PesHeaderBitBuffer, 2, PesHeader.PTS_DTS_flags);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.ESCR_flag);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.ES_rate_flag);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.DSM_trick_mode_flag);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.additional_copy_info_flag);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.PES_CRC_flag);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.PES_extension_flag);
		BitWrite(&PesHeaderBitBuffer, 8, PesHeader.PES_header_data_length);
		BitWrite(&PesHeaderBitBuffer, 4, PesHeader.PTS_flags);
		BitWrite(&PesHeaderBitBuffer, 3, PesHeader.PTS32_30);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.marker_bit0);
		BitWrite(&PesHeaderBitBuffer, 15, PesHeader.PTS29_15);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.marker_bit1);
		BitWrite(&PesHeaderBitBuffer, 15, PesHeader.PTS14_0);
		BitWrite(&PesHeaderBitBuffer, 1, PesHeader.marker_bit2);


		pCurBlock->pNext	= NULL;
		pCurBlock->pBuffer	= (CHAR *)pPesHeaderBuf;
		pCurBlock->iBuffer	= sizeof(pes_header_t) + iPES_header_data_length - 5;
		pCurBlock->iType	= BLOCK_TYPE_PES_HEADER;

		BlockChainAppend(&pDstBlock, pCurBlock);
		BlockChainAppend(&pDstBlock, pVideoBlock);

		pTmpBlock = pVideoBlock;
		pVideoBlock = pVideoBlock->pNext;
		pTmpBlock->pNext = NULL;
	}

	*pBlock = pDstBlock;

	return S_OK;
}
