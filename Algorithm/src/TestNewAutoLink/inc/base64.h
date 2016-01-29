#ifndef _BASE64_H
#define _BASE64_H

//计算编码需要的输出缓存长度,此长度不包括编码结果字符串的结尾'\0'
inline unsigned int base64_code_size(const unsigned int data_size)
{
	return (data_size+2)/3*4;
}

typedef enum _B64ReultType{  
	b64Result_OK=0, 
	b64Result_CODE_SIZE_ERROR, 
	b64Result_DATA_SIZE_SMALLNESS,
	b64Result_CODE_ERROR
}
B64ReultType;

void base64_encode(
		const void* pdata,	//原始数据
		const unsigned int data_size,	//原始数据长度
		void* out_pcode	//输出BASE64编码
		);

B64ReultType base64_decode(
				const void* pcode,	//BASE64编码
				const unsigned int code_size,	//编码长度
				void* out_pdata,	//输出
				const unsigned int data_size,	//输出缓存长度
				unsigned int* out_pwrited_data_size	//实际输出长度
				);

//////////////////////////////////////////////////////////////////////////
//辅助类

class CBase64Encoder
{
public:
	CBase64Encoder(const void* pData, unsigned int nDataLen);
	~CBase64Encoder();
public:
	const char* ToStr();
	unsigned int GetLen();
protected:
	char* m_pEncBuf;
	unsigned int m_nBufLen;
};

class CBase64Decoder
{
public:
	//pCode: 以'\0'结尾的Base64编码字符串
	CBase64Decoder(const char* pCode);
	~CBase64Decoder();
public:
	const void* ToBin();
	unsigned int GetLen();
protected:
	unsigned char* m_pDecBuf;
	unsigned int m_nBufLen;
	unsigned int m_nDecodeLen;
};

#endif
