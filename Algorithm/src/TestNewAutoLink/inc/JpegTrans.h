#ifndef _JPEG_TRANS_H_
#define _JPEG_TRANS_H_

#include "HvInterface.h"
#include "jpegdecoder.h"
#include "swImageObj.h"

class CJpegTrans : public jpeg_decoder_stream
{
public:
	HRESULT Begin(HvCore::IHvStream* pInput, BOOL fUseMMX = TRUE);
	HRESULT Trans(HvCore::IHvStream* pOutput, BOOL fFeildTrans);
	HRESULT Trans(HV_COMPONENT_IMAGE* pImg, BOOL fFeildTrans);

	HRESULT End()	//用于手动释放资源
	{
		Close();
		return S_OK;
	}
	
	HRESULT GetImgProperty(DWORD32* pdwWidth, DWORD32* pdwHeight, DWORD32* pdwComponents);

public:
	virtual int read(uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag);

public:
	CJpegTrans();
	~CJpegTrans();

protected:
	HRESULT Close();

	jpeg_decoder* m_pDecoder;	
	HvCore::IHvStream* m_pInput;
};

#endif
