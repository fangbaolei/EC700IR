/**
   Author:	wujf
   Date:	2015/03/25
*/
#ifndef __SWPSMUXER_H__
#define __SWPSMUXER_H__

#include "SWMuxer.h"
#include "SWPESMuxer.h"

/**
   初始化PS复用器的参数
*/
typedef struct _INIT_PSMUXER_PARAM_T
{
	unsigned long long	lSCRBase;
	INT			iFrameRate;
	INT			iBitRate;
}INIT_PSMUXER_PARAM_T;

class CSWPSMuxer : public CSWMuxer
{
public:
	CSWPSMuxer();
	virtual ~CSWPSMuxer();
	virtual INT InitMuxer(VOID *pParam);
	virtual INT Mux(BLOCK_T **pBlock);
/**
   @brief 初始化CRC32表
*/
	INT InitCRC32Table();

/**
   @brief 获取CRC32值
   @param [in] pBuffer 输入缓存
   @param [in] iBuffer 缓存大小
 */
	unsigned int GetCRC32(unsigned char *pBuffer, unsigned int iBuffer);

/**
   @brief PS 头封装
   @param [in][out] pBlock 数据链
*/
	INT PackHeader(BLOCK_T **pBlock);

/**
   @brief 系统头封装
   @param [in][out] pBlock 数据链
*/
	INT SystemHeader(BLOCK_T **pBlock);

/**
   @brief AVC 组建描述字段
   @param [in] pAVC_HRD_DescBuf AVC描述符结构体
 */
	INT AvcTimeHrdDescriptor(AVC_timing_HRD_descriptor_t *pAVC_HRD_DescBuf);

/**
   @brief 程序流图封装
   @param [in][out] pBlock 数据链
*/
	INT PSMHeader(BLOCK_T **pBlock);

/**
  @brief 更新SCRBase
*/
	VOID UpdateSCRBase();

/**
   @brief 设置帧率
   @param [in] iFrameRate 帧率
 */
	VOID SetFrameRate(INT iFrameRate);

/**
   @brief 设置码率
   @param [in] iBitRate 码率
 */
	VOID SetBitRate(INT iBitRate);

private:
	unsigned long long m_lSCRBase;
	unsigned int m_rgCRC32Table[256];
	INT m_iFrameRate;
	INT m_iBitRate;
	CSWPESMuxer *m_pPesMuxer;
};

#endif 
