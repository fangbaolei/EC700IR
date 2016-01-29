/**
   Author:	wujf
   Date:	2015/03/25
*/
#ifndef __SWPESMUXER_H__
#define __SWPESMUXER_H__

#include "SWMuxer.h"

//nal type unit
#define NALU_TYPE_MASK		0x1F
#define NALU_TYPE_SLICE		1
#define NALU_TYPE_DPA		2
#define NALU_TYPE_DPB		3
#define NALU_TYPE_DPC		4
#define NALU_TYPE_IDR		5
#define NALU_TYPE_SEI		6
#define NALU_TYPE_SPS		7
#define NALU_TYPE_PPS		8
#define NALU_TYPE_AUD		9	// access unit delimiter
#define NALU_TYPE_EOSEQ		10	// end of seq
#define NALU_TYPE_EOSTREAM	11	// end of stream
#define NALU_TYPE_FILL		12
#define NALU_TYPE_EXTENSION	13
#define NALU_TYPE_RESERVE_14	14
// ......
#define NALU_TYPE_RESERVE_18	18

#define PES_PAYLOAD_SIZE_MAX 65500

class CSWPESMuxer : public CSWMuxer
{
public:
	CSWPESMuxer();
	virtual ~CSWPESMuxer();

	virtual INT Mux(BLOCK_T **pBlock);
public:
/**
   @brief 设置PTS
   @param [in] lPTS PTS值
 */
	VOID SetPTS(unsigned long long lPTS);

/**
   @brief 分离I帧，使PPS SPS SEI IDR数据都有一个PES头
   @param [in][out] pBlock 视频数据
*/
	INT SplitIDR(BLOCK_T **pBlock);

/**
   @brief 取得一个NAL单元
   @param [in] pBuffer 视频数据缓存
   @param [in] uBuffer 视频数据大小
   @param [out] pNalBuffer Nal单元数据缓存
   @param [out] pNalSize Nal单元数据大小
*/
	INT GetNALU(CHAR *pBuffer, unsigned int uBuffer, 
				unsigned int *uNaluStartPos, unsigned int *uNaluEndPos, unsigned int *uNaluType);

private:
	unsigned long long m_lPTS;
};

#endif 
