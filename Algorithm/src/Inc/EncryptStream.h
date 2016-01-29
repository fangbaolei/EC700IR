#ifndef _IRESULTSEND_H_
#define _IRESULTSEND_H_

#include "HvStream.h"
#include "rsa.h"

#define HV_ENC_RSA_NORMAL			0x00000000	//自定义方式
#define HV_ENC_RSA_PKCS				0x00000001	//PKCS#1标准
#define HV_ENC_BLOCK_TYPE0			0x00000000	//正常块类型(不添加附加块)
#define HV_ENC_BLOCK_TYPE1			0x00000010	//块类型1
#define HV_ENC_BLOCK_TYPE2			0x00000020	//块类型2
#define HV_ENC_BLOCK_TYPE3			0x00000030	//块类型3
#define HV_ENC_BUF_SIZE0			0x00000000	//用户指定缓存大小
#define HV_ENC_BUF_SIZE1			0x00000100	//缓存类型1
#define HV_ENC_BUF_SIZE2			0x00000200	//缓存类型2
#define HV_ENC_BUF_SIZE3			0x00000300	//缓存类型3
#define HV_ENC_INTERVAL_0			0x00000000	//加密间隔0(无间隔)
#define HV_ENC_INTERVAL_1			0x00001000	//加密间隔1
#define HV_ENC_INTERVAL_2			0x00002000	//加密间隔2
#define HV_ENC_INTERVAL_3			0x00003000	//加密间隔3

namespace HiVideo
{
	struct EncryptStreamInfo
	{
		BOOL fWrite;
		BOOL fValid;
		BOOL fInitialized;

		UINT nStreamStart;
		UINT nCurPos;
		INT nCurBlockNum;
		INT nInterval;

		UINT nEncryptBufSize;
		UINT nAvailBufSize;

		UINT nBlockSize;
		UINT nBlkOffset;

		UINT nStreamSize;
		UINT nCrc32;
	};

	struct EncryptStreamHead
	{
		UINT  Tag;
		UINT  Flags;
		UINT  BufSize;
		UINT  RsaDatLen;
		UINT  Reserved1;
		UINT  Reserved2;
	};

	struct EncryptStreamTail
	{
		UINT Size;
		UINT Crc;
		UINT Reserved1;
		UINT Reserved2;
		UINT Valid;
	};

	typedef struct _EncryptStreamParam
	{
		CHvStream* pIOStream;
		BOOL fWrite;
		const DWORD32* pKey;

		//--注：以下两个参数只能设置一个
		const RSA_KEY* pRsaKey;
		const RSA_PRIVATE_KEY* pRsaPriKey;  //使用RSA_PRIVATE_KEY的只读流
		//--

		DWORD32 nEncryptBufSize;
		DWORD32 dwFlags;

		_EncryptStreamParam()
		{
			pIOStream       = NULL;
			fWrite          = FALSE;
			pKey            = NULL;
			pRsaKey         = NULL;
			pRsaPriKey      = NULL;
			nEncryptBufSize = 2048;
			dwFlags         = HV_ENC_BUF_SIZE1|HV_ENC_BLOCK_TYPE2|HV_ENC_RSA_PKCS;
		};

	} ENCRYPTSTREAM_PARAM;  // 加密流的相关初始化参数

	//加密流
	class CEncryptStream : public CHvStream
	{
	public:
		CEncryptStream();
		virtual ~CEncryptStream();

	public:
		HRESULT Initialize(
			ENCRYPTSTREAM_PARAM* pParam
			);

		HRESULT Close(
			void
			);

		HRESULT Read(
			PVOID pv,
			UINT cb,
			PUINT pcbRead
			);

		HRESULT Write(
			LPCVOID pv,
			UINT cb,
			PUINT pcbWritten
			);

		HRESULT Seek(
			INT dlibMove,
			STREAM_SEEK ssOrigin,
			PUINT pnNewPosition
			);

		//不实现
		HRESULT SetSize(
			UINT nNewSize
			);

		//不实现
		HRESULT GetBuf(
			BYTE8** ppBuf,
			DWORD32* pSize
			);

		HRESULT CopyTo(
			CEncryptStream *pstm,
			UINT cb,
			PUINT pcbRead,
			PUINT pcbWritten
			);

		HRESULT GetVerifyInfo(
			DWORD32* pCrc,
			DWORD32* pSize
			);

	protected:
		IStream* m_pIOStream;

		BYTE8* m_pbBuf;

		UINT m_rgKey[4];

		CFastCrc32 m_FastCrc;

		EncryptStreamInfo m_Info;

		HVRESULT CheckHead(EncryptStreamHead& head);
		HVRESULT CheckTail(EncryptStreamTail& tail);

		HVRESULT UpdateHead(const EncryptStreamHead& head,vlong& keydat);
		HVRESULT RandMem(BYTE8* pbBuf,UINT nSize);

		HVRESULT Flush(void);
	};
}

#endif
