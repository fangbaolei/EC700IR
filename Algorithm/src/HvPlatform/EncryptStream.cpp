#include "EncryptStream.h"
#include "crypt.h"

namespace HiVideo
{
	CEncryptStream::CEncryptStream()
	{
		m_Info.fWrite = FALSE;
		m_Info.fValid = FALSE;
		m_Info.fInitialized = FALSE;
		m_Info.nStreamStart = 0;
		m_Info.nCurPos = 0;
		m_Info.nCurBlockNum = -1;
		m_Info.nInterval = 1;
		m_Info.nEncryptBufSize = 0;
		m_Info.nAvailBufSize = 0;
		m_Info.nBlockSize = 0;
		m_Info.nBlkOffset = 0;
		m_Info.nStreamSize = 0;
		m_Info.nCrc32 = 0;
		m_pbBuf = NULL;
	}

	CEncryptStream::~CEncryptStream()
	{
		Close();
	}

	HRESULT CEncryptStream::Initialize(
		ENCRYPTSTREAM_PARAM* pParam
		)
	{
		//从结构体中取出各参数
		CHvStream* pIOStream = pParam->pIOStream;
		BOOL fWrite = pParam->fWrite;
		const UINT* pKey = pParam->pKey;

		const RSA_KEY* pRsaKey = pParam->pRsaKey;
		const RSA_PRIVATE_KEY* pRsaPriKey = pParam->pRsaPriKey;

		UINT nEncryptBufSize = pParam->nEncryptBufSize;
		DWORD32 dwFlags = pParam->dwFlags;

		if ( !pIOStream ) return E_INVALIDARG;
		if (nEncryptBufSize <8 || nEncryptBufSize%4) return E_INVALIDARG;

		EncryptStreamHead head;
		EncryptStreamTail tail;

		if ( pRsaKey )
		{
			srand((unsigned)time(NULL));

			Close();

			m_pIOStream = pIOStream;

			m_Info.fWrite = fWrite;		//模式: 读或写
			m_Info.fValid = FALSE;		//流有效性标志
			m_Info.fInitialized = FALSE;	//初始化标志
			m_Info.nStreamStart = 0;	//流数据起始
			m_Info.nCurPos = 0;	//当前位置
			m_Info.nCurBlockNum = -1;  //块编号为-1,保证第一次读取时刷新缓存
			m_Info.nInterval = 1;		//加密间隔
			m_Info.nEncryptBufSize = nEncryptBufSize;	//默认缓存大小为参数指定
			m_Info.nAvailBufSize = nEncryptBufSize;	//可用缓存大小为缓存大小
			m_Info.nBlockSize = nEncryptBufSize;	//块大小为缓存大小
			m_Info.nBlkOffset = 0;	//块内偏移
			m_Info.nStreamSize = 0; //流数据长度
			m_Info.nCrc32 = 0;	//流数据校验值

			if (pKey != NULL) //块加密密钥
			{
				HV_memcpy(m_rgKey,pKey,sizeof(m_rgKey));
			}
			else if (fWrite)  //如果是写入且未指定KEY,随机生成
			{
				RandMem((BYTE8*)m_rgKey,sizeof(m_rgKey));
			}

			if( m_Info.fWrite )  //写入
			{
				head.Tag = 0x77748783;
				head.Flags = dwFlags;
				head.BufSize=m_Info.nEncryptBufSize;
				head.RsaDatLen=0;
				head.Reserved1=0;
				head.Reserved2=0;

				if (dwFlags & 0x00000f00) head.BufSize = 0; //将通过FLAG指定BUFSIZE

				m_Info.nCurBlockNum = 0; //写入时起始块编号为0

				// 保存用RSA加密的KEY
				vlong cipher = 0;
				if (pRsaKey != NULL )  //指定了RSAKEY
				{
					if ((head.Flags&HV_ENC_RSA_PKCS) == HV_ENC_RSA_PKCS)  //如果是用PKCS标准加密
					{
						rsa_encrypt_pkcs((BYTE8*)m_rgKey,sizeof(m_rgKey),cipher,(RSA_KEY&)*pRsaKey);
					}
					else //正常模式
					{
						vlong plain;
						plain.load(m_rgKey,4);
						cipher = rsa_encrypt(plain,pRsaKey->key,pRsaKey->modulus);
					}
				}
				head.RsaDatLen = cipher.getLen();
				//ASSERT(head.RsaDatLen == 128); //暂只支持1024位RSA
				RTN_HR_IF_FAILED(UpdateHead(head,cipher)); //写入头信息
			}
			else   //读出
			{
				//流有效性判断:
				// 1. 头尾格式正确
				// 2. KEY有效: 参数指定,或者由RSA解密得出
				// 3. 不同时满足以上两点认为是无效流,使用下层IO流读取

				if ( S_OK == CheckHead(head) && S_OK == CheckTail(tail) ) //判断格式有效性
				{
					dwFlags = head.Flags;
					m_Info.nEncryptBufSize = head.BufSize;
					m_Info.nStreamSize = tail.Size;
					m_Info.nCrc32 = tail.Crc;

					if (pKey != NULL) //头尾正确且有KEY,有效流
					{
						m_Info.fValid = TRUE;
					}
					else if ( head.RsaDatLen != 0 && pRsaKey != NULL) //RSA解密相关
					{
						//ASSERT(head.RsaDatLen == 128); //only support RSA_1024
						BYTE8 buf[128] = {0}; //固定分配128字节(1024位)

						//读入加密后的密钥数据
						RTN_HR_IF_FAILED(pIOStream->Read(buf,head.RsaDatLen,NULL));

						if ((head.Flags&HV_ENC_RSA_PKCS) == HV_ENC_RSA_PKCS)
						{
							rsa_decrypt_pkcs(buf,head.RsaDatLen,(BYTE8*)m_rgKey,sizeof(m_rgKey),(RSA_KEY&)*pRsaKey);
						}
						else
						{
							vlong temp = 0, keydat = 0;
							temp.load((UINT*)buf,head.RsaDatLen/sizeof(UINT));
							keydat = rsa_decrypt(temp,pRsaKey->key,pRsaKey->modulus);
							keydat.store(m_rgKey,4);
						}
						m_Info.fValid = TRUE;  //有RSA解密的KEY,有效流
					}
				}
			}

			if ( !m_Info.fWrite ) //如果是读取
			{
				if ( !m_Info.fValid ) //如果格式无效,IO流SEEK到开头
				{
					m_Info.nStreamStart = 0;
					RTN_HR_IF_FAILED(m_pIOStream->Seek(0,STREAM_SEEK_SET,NULL));
					m_Info.fInitialized = TRUE ;
					return S_FALSE; //成功但格式不对
				}
				else
				{
					m_Info.nStreamStart = sizeof(EncryptStreamHead) + head.RsaDatLen; //流数据起始
					RTN_HR_IF_FAILED(m_pIOStream->Seek(m_Info.nStreamStart,STREAM_SEEK_SET,NULL)); //SEEK到数据起始位置
				}
			}

			DWORD32 dwFlagValue = 0; //标志位值

			if ( dwFlagValue = dwFlags & 0x00000F00 ) // BUF_SIZE_TYPE不为0, 通过FLAG指定BUFSIZE
			{
				switch(dwFlagValue)
				{
				case HV_ENC_BUF_SIZE1:
					m_Info.nEncryptBufSize = 0x800; //2k
					break;
				case HV_ENC_BUF_SIZE2:
					m_Info.nEncryptBufSize = 0x1000; //4k
					break;
				case HV_ENC_BUF_SIZE3:
					m_Info.nEncryptBufSize = 0x2000; //8k
					break;
				default:
					break;
				}
			}

			//缓存大小可能由标志位重指定,重设块大小
			m_Info.nBlockSize = m_Info.nEncryptBufSize;

			if ( dwFlagValue = dwFlags & 0x000000F0 ) //如果BLOCK_TYPE不为0,有填充数据
			{
				switch(dwFlagValue)
				{
				case HV_ENC_BLOCK_TYPE1:
					m_Info.nBlockSize = m_Info.nEncryptBufSize + 20; //前15,后5,扩充20
					m_Info.nBlkOffset = 15;
					break;
				case HV_ENC_BLOCK_TYPE2:
					m_Info.nBlockSize = m_Info.nEncryptBufSize + 44; //前19,后25,扩充44
					m_Info.nBlkOffset = 19;
					break;
				case HV_ENC_BLOCK_TYPE3:
					m_Info.nBlockSize = m_Info.nEncryptBufSize + 68; //前41,后27,扩充68
					m_Info.nBlkOffset = 41;
					break;
				default:
					break;
				}
			}

			if ( dwFlagValue = dwFlags & 0x0000F000)//如果INTERVAL_TYPE不为0,间隔加密
			{
				switch(dwFlagValue)
				{
				case HV_ENC_INTERVAL_1:
					m_Info.nInterval = 4;
					break;
				case HV_ENC_INTERVAL_2:
					m_Info.nInterval = 8;
					break;
				case HV_ENC_INTERVAL_3:
					m_Info.nInterval = 16;
					break;
				default:
					break;
				}
			}

			//申请缓存
			m_pbBuf = (BYTE8*)HV_AllocMem(m_Info.nEncryptBufSize);
			if (!m_pbBuf) return E_OUTOFMEMORY;
			memset(m_pbBuf,0,m_Info.nEncryptBufSize);

			m_Info.nAvailBufSize = m_Info.nEncryptBufSize; //当前可用缓存大小
			m_Info.fInitialized = TRUE ;

			return S_OK;
		}
		else
		{
			if (fWrite == TRUE) return E_INVALIDARG;  //只读流

			Close();

			m_pIOStream = pIOStream;

			m_Info.fWrite = FALSE;  //只读
			m_Info.fValid = FALSE;		//有效性标志
			m_Info.fInitialized = FALSE; //初始化标志
			m_Info.nStreamStart = 0;	//流数据起始
			m_Info.nCurPos = 0;	//当前位置
			m_Info.nCurBlockNum = -1;	//当前块编号
			m_Info.nInterval = 1;		//加密间隔
			m_Info.nEncryptBufSize = nEncryptBufSize;	//缓存大小
			m_Info.nAvailBufSize = nEncryptBufSize;	//可用缓存大小
			m_Info.nBlockSize = nEncryptBufSize;	//块大小
			m_Info.nBlkOffset = 0;	//块内偏移
			m_Info.nStreamSize = 0;	//流长度
			m_Info.nCrc32 = 0;	//流校验值

			if ( S_OK == CheckHead(head) && S_OK == CheckTail(tail) )
			{
				dwFlags = head.Flags;
				m_Info.nEncryptBufSize = head.BufSize;
				m_Info.nStreamSize = tail.Size;
				m_Info.nCrc32 = tail.Crc;

				if (pKey != NULL)
				{
					HV_memcpy(m_rgKey,pKey,sizeof(m_rgKey));
					m_Info.fValid = TRUE;
				}
				else if ( head.RsaDatLen != 0 && pRsaPriKey != NULL) //RSA解密相关
				{
					//ASSERT(head.RsaDatLen == 128); //only support RSA_1024
					BYTE8 buf[128] = {0};  //固定分配128字节(1024位)

					RTN_HR_IF_FAILED(pIOStream->Read(buf,head.RsaDatLen,NULL));

					vlong temp,keydat;

					if ((head.Flags&HV_ENC_RSA_PKCS) == HV_ENC_RSA_PKCS)
					{
						rsa_decrypt_pkcs(buf,head.RsaDatLen,(BYTE8*)m_rgKey,sizeof(m_rgKey),(RSA_PRIVATE_KEY&)*pRsaPriKey);
					}
					else
					{
						temp.load((UINT*)buf,head.RsaDatLen/sizeof(UINT));
						keydat = rsa_decrypt(temp,(RSA_PRIVATE_KEY&)(*pRsaPriKey));
						keydat.store(m_rgKey,4);
					}
					m_Info.fValid = TRUE;
				}
			}

			if ( !m_Info.fValid )
			{
				m_Info.nStreamStart = 0;
				RTN_HR_IF_FAILED(m_pIOStream->Seek(0,STREAM_SEEK_SET,NULL));
				m_Info.fInitialized = TRUE ;
				return S_FALSE;
			}

			m_Info.nStreamStart = sizeof(EncryptStreamHead) + head.RsaDatLen;

			RTN_HR_IF_FAILED(m_pIOStream->Seek(m_Info.nStreamStart,STREAM_SEEK_SET,NULL));

			DWORD32 dwFlagValue = 0; //标志位值

			if ( dwFlagValue = dwFlags & 0x00000F00 ) // BUF_SIZE_TYPE不为0, 通过FLAG指定BUFSIZE
			{
				switch(dwFlagValue)
				{
				case HV_ENC_BUF_SIZE1:
					m_Info.nEncryptBufSize = 0x800; //2k
					break;
				case HV_ENC_BUF_SIZE2:
					m_Info.nEncryptBufSize = 0x1000; //4k
					break;
				case HV_ENC_BUF_SIZE3:
					m_Info.nEncryptBufSize = 0x2000; //8k
					break;
				default:
					break;
				}
			}

			//缓存大小可能由标志位重指定,重设块大小
			m_Info.nBlockSize = m_Info.nEncryptBufSize;

			if ( dwFlagValue = dwFlags & 0x000000F0 ) //如果BLOCK_TYPE不为0,有填充数据
			{
				switch(dwFlagValue)
				{
				case HV_ENC_BLOCK_TYPE1:
					m_Info.nBlockSize = m_Info.nEncryptBufSize + 20; //前15,后5,扩充20
					m_Info.nBlkOffset = 15;
					break;
				case HV_ENC_BLOCK_TYPE2:
					m_Info.nBlockSize = m_Info.nEncryptBufSize + 44; //前19,后25,扩充44
					m_Info.nBlkOffset = 19;
					break;
				case HV_ENC_BLOCK_TYPE3:
					m_Info.nBlockSize = m_Info.nEncryptBufSize + 68; //前41,后27,扩充68
					m_Info.nBlkOffset = 41;
					break;
				default:
					break;
				}
			}

			if ( dwFlagValue = dwFlags & 0x0000F000)//如果INTERVAL_TYPE不为0,间隔加密
			{
				switch(dwFlagValue)
				{
				case HV_ENC_INTERVAL_1:
					m_Info.nInterval = 4;
					break;
				case HV_ENC_INTERVAL_2:
					m_Info.nInterval = 8;
					break;
				case HV_ENC_INTERVAL_3:
					m_Info.nInterval = 16;
					break;
				default:
					break;
				}
			}

			//申请缓存
			m_pbBuf = (BYTE8*)HV_AllocMem(m_Info.nEncryptBufSize);
			if (!m_pbBuf) return E_OUTOFMEMORY;
			memset(m_pbBuf,0,m_Info.nEncryptBufSize);

			m_Info.nAvailBufSize = m_Info.nEncryptBufSize;	//当前可用缓存大小
			m_Info.fInitialized = TRUE ;

			return S_OK;
		}
	}

	HRESULT CEncryptStream::Close(
		void
		)
	{
		if (!m_Info.fInitialized) //未初始化直接返回
		{
			return S_OK;
		}

		RTN_HR_IF_FAILED( Flush() );

		m_pIOStream=NULL;

		if (m_pbBuf)
		{
			HV_FreeMem(m_pbBuf, m_Info.nEncryptBufSize);
			m_pbBuf=NULL;
		}

		HV_memset(m_rgKey,0,sizeof(m_rgKey));

		m_Info.fWrite = FALSE;
		m_Info.fValid = FALSE;
		m_Info.fInitialized = FALSE;
		m_Info.nStreamStart = 0;
		m_Info.nCurPos = 0;
		m_Info.nCurBlockNum = -1;
		m_Info.nInterval = 1;
		m_Info.nEncryptBufSize = 0;
		m_Info.nAvailBufSize = 0;
		m_Info.nBlockSize = 0;
		m_Info.nBlkOffset = 0;
		m_Info.nStreamSize = 0;
		m_Info.nCrc32 = 0;

		return S_OK;
	}

	HRESULT CEncryptStream::Read(
		PVOID pv,
		UINT cb,
		PUINT pcbRead
		)
	{
		if (pcbRead) *pcbRead = 0;

		if (!m_Info.fInitialized) return E_FAIL; //未初始化

		if (!m_Info.fValid) return m_pIOStream->Read(pv,cb,pcbRead); //格式不对直接返回下层IO流数据

		if ( !pv ) return E_INVALIDARG;
		if ( m_Info.fWrite ) return E_ACCESSDENIED; //写入时不支持读

		if ( cb == 0 ) return S_OK;
		if ( m_Info.nCurPos >= m_Info.nStreamSize) return S_FALSE; //当前位置大于流长度,不读取

		UINT cb_old = cb; //保存预期读取字节数

		cb=min(cb,m_Info.nStreamSize - m_Info.nCurPos); //实际可读取字节数

		UINT nBlockNum(0),nOffset(0),nBlockAvail(0),nReadLen(0),nMaxReadLen(0);

		do
		{
			nBlockNum = m_Info.nCurPos / m_Info.nEncryptBufSize; //计算数据块编号
			nOffset = m_Info.nCurPos % m_Info.nEncryptBufSize; //计算数据块偏移
			nBlockAvail = m_Info.nEncryptBufSize - nOffset; //块内可读取数据

			//如果缓存块编号不等于数据块编号,从IO流读取新数据,否则直接读缓存
			if (m_Info.nCurBlockNum != (int)nBlockNum)
			{
				//跳过附加数据
				RTN_HR_IF_FAILED(m_pIOStream->Seek(m_Info.nBlkOffset,STREAM_SEEK_CUR,NULL));
				//读取实际加密数据
				RTN_HR_IF_FAILED(m_pIOStream->Read(m_pbBuf,m_Info.nEncryptBufSize,NULL));
				//解密
				if (((nBlockNum + 1)%m_Info.nInterval) == 0)
				{
					decrypt_block(m_pbBuf,m_Info.nEncryptBufSize,m_rgKey);
				}
				//跳过附加数据, 此时IO流指向下次读取位置
				RTN_HR_IF_FAILED( m_pIOStream->Seek(m_Info.nBlockSize - m_Info.nEncryptBufSize - m_Info.nBlkOffset, STREAM_SEEK_CUR, NULL));
				//更新缓存块编号,此时缓存块内容对应当前数据块
				m_Info.nCurBlockNum = nBlockNum;
			}

			//从缓存块读取
			nMaxReadLen=min(cb,nBlockAvail); //缓存最多可读取字节数
			HV_memcpy((BYTE8*)pv + nReadLen ,m_pbBuf + nOffset ,nMaxReadLen); //读取

			nReadLen += nMaxReadLen; //更新读取总长度
			m_Info.nCurPos += nMaxReadLen; //更新当前位置
			cb -= nMaxReadLen;	//更新剩余读取字节
		}
		while(cb > 0); //还要读,继续

		if(pcbRead) *pcbRead = nReadLen; //返回实际读取字节数

		return ( nReadLen == cb_old)?S_OK:S_FALSE; //返回S_FALSE表明已到流结尾
	}

	HRESULT CEncryptStream::Write(
		LPCVOID pv,
		UINT cb,
		PUINT pcbWritten
		)
	{
		if (pcbWritten) *pcbWritten = 0;

		if (!m_Info.fInitialized) return E_FAIL; //未初始化

		if ( !pv ) return E_INVALIDARG;
		if ( !m_Info.fWrite ) return E_ACCESSDENIED; //读取时不能写入

		if (cb == 0) return S_OK;

		BYTE8* pbWriteBuf = (BYTE8*)HV_AllocMem(m_Info.nBlockSize);	//申请写缓存

		if (!pbWriteBuf) return E_OUTOFMEMORY;

		//填充随机数据
		RandMem(pbWriteBuf,m_Info.nBlkOffset);
		RandMem(pbWriteBuf + m_Info.nBlkOffset + m_Info.nEncryptBufSize, m_Info.nBlockSize - m_Info.nBlkOffset - m_Info.nEncryptBufSize);

		UINT nCopiedSize(0);

		//如果要写入数据大于可用缓存数,加密写入IO流,否则直接写入缓存
		while( cb >= m_Info.nAvailBufSize)
		{
			//写满缓存块
			HV_memcpy(m_pbBuf + m_Info.nEncryptBufSize - m_Info.nAvailBufSize , (BYTE8*)pv + nCopiedSize , m_Info.nAvailBufSize);

			//计算流内已写入数据CRC
			m_Info.nCrc32=m_FastCrc.CalcCrc32(m_Info.nCrc32,m_pbBuf + m_Info.nEncryptBufSize - m_Info.nAvailBufSize,m_Info.nAvailBufSize);

			//加密
			if ( ((m_Info.nCurBlockNum + 1) % m_Info.nInterval) == 0)
			{
				encrypt_block(m_pbBuf, m_Info.nEncryptBufSize,pbWriteBuf + m_Info.nBlkOffset, m_rgKey);
			}
			else
			{
				HV_memcpy(pbWriteBuf + m_Info.nBlkOffset, m_pbBuf, m_Info.nEncryptBufSize);
			}

			//写入IO流
			RTN_HR_IF_FAILED(m_pIOStream->Write(pbWriteBuf,m_Info.nBlockSize,NULL));

			cb -= m_Info.nAvailBufSize;  //更新剩余写入长度
			nCopiedSize += m_Info.nAvailBufSize; //更新已写入长度
			m_Info.nCurPos += m_Info.nAvailBufSize;	//更新当前位置
			m_Info.nStreamSize += m_Info.nAvailBufSize;	//更新流数据总长
			m_Info.nAvailBufSize = m_Info.nEncryptBufSize;	//更新可用缓存容量
			m_Info.nCurBlockNum++; //更新当前块编号
		}

		//写入缓存
		if (cb > 0)
		{
			HV_memcpy(m_pbBuf + m_Info.nEncryptBufSize - m_Info.nAvailBufSize , (BYTE8*)pv + nCopiedSize , cb);

			m_Info.nCrc32=m_FastCrc.CalcCrc32(m_Info.nCrc32,m_pbBuf + m_Info.nEncryptBufSize - m_Info.nAvailBufSize,cb);

			nCopiedSize += cb; //更新已写入长度
			m_Info.nAvailBufSize -= cb;	//更新可用缓存容量
			m_Info.nCurPos += cb;	//更新当前位置
			m_Info.nStreamSize += cb;	//更新流数据总长
		}

		HV_FreeMem(pbWriteBuf, m_Info.nBlockSize); //释放写缓存

		if (pcbWritten) *pcbWritten = nCopiedSize; //返回已写入长度

		return S_OK;
	}

	HRESULT CEncryptStream::Seek(
		INT dlibMove,
		STREAM_SEEK dwOrigin,
		PUINT plibNewPosition
		)
	{
		if (!m_Info.fInitialized) return E_FAIL; //未初始化

		if ( m_Info.fWrite ) return E_NOTIMPL; //写入时不能SEEK

		if (!m_Info.fValid) return m_pIOStream->Seek(dlibMove,dwOrigin,plibNewPosition); //格式不对SEEK IO流

		if (plibNewPosition)  *plibNewPosition= m_Info.nCurPos;

		UINT nNewPos = m_Info.nCurPos; //新位置

		switch (dwOrigin) //起始位置
		{
		case STREAM_SEEK_SET:
			nNewPos = 0; //指到头
			break;
		case STREAM_SEEK_END:
			nNewPos = m_Info.nStreamSize; //指到尾
			break;
		case STREAM_SEEK_CUR: //指到当前位置
			break;
		default:
			break;
		}

		nNewPos += dlibMove; //加上偏移量得到新位置

		if (nNewPos > m_Info.nStreamSize) return E_FAIL; //如果越界返回失败,不更新当前位置

		m_Info.nCurPos = nNewPos; //更新当前位置

		//如果缓存块编号不等于数据块编号,IO流SEEK到数据块起始位置
		//下次读时再实际读取并更新缓存块内容
		if(m_Info.nCurBlockNum != (int)(m_Info.nCurPos/m_Info.nEncryptBufSize))
		{
			RTN_HR_IF_FAILED(m_pIOStream->Seek(
				m_Info.nStreamStart + (m_Info.nCurPos/m_Info.nEncryptBufSize) * m_Info.nBlockSize, //IO流数据
				STREAM_SEEK_SET,
				NULL)
				);
		}

		if (plibNewPosition) *plibNewPosition = m_Info.nCurPos;

		return S_OK;
	}

	HRESULT CEncryptStream::CopyTo(
		CEncryptStream *pstm,
		UINT cb,
		PUINT pcbRead,
		PUINT pcbWritten
		)
	{
		if (pcbWritten) *pcbWritten=0;
		if (pcbRead) *pcbRead=0;

		if (!m_Info.fInitialized) return E_FAIL; //未初始化

		if ( !pstm ) return E_INVALIDARG;

		BYTE8 buf[g_kcCopyBuf]={0};	 //读取缓存

		UINT nReadLen(0), nWriteLen(0), nReadCount(0), nWriteCount(0), nMaxReadLen(0);

		do
		{
			nMaxReadLen = min(g_kcCopyBuf,cb);		//最多可读字节数
			RTN_HR_IF_FAILED( Read(buf,nMaxReadLen,&nReadCount) );  //读取
			RTN_HR_IF_FAILED( pstm->Write(buf,nReadCount,&nWriteCount) ); //写入

			nReadLen += nReadCount; //更新读取总长
			nWriteLen += nWriteCount; //更新写入

			//判断是否读到尾及写入是否成功
			if ( nReadCount != nMaxReadLen || nWriteCount != nReadCount) break;

			cb -= nReadCount; //更新剩余读取数
		}
		while(cb>0);

		if (pcbRead) *pcbRead = nReadLen;
		if (pcbWritten) *pcbWritten = nWriteLen;

		return S_OK;
	}

	HRESULT CEncryptStream::GetVerifyInfo(
		DWORD32* pCrc,
		DWORD32* pSize
		)
	{
		if (!pCrc || !pSize) return E_INVALIDARG;

		*pCrc = 0;
		*pSize =0;

		if (!m_Info.fInitialized) return E_FAIL;  //未初始化
		if ( m_Info.fWrite ) return E_NOTIMPL;	//写入时不支持读校验信息

		if (!m_Info.fValid) //如果是无效流,返回下层IO流校验信息
		{
			return m_pIOStream->GetVerifyInfo(pCrc,pSize);
		}

		UINT nOldPos; //保留当前位置
		Seek(0,STREAM_SEEK_CUR,&nOldPos);
		Seek(0,STREAM_SEEK_SET,NULL);

		BYTE8 buf[g_kcCopyBuf]={0};		//读取缓存
		UINT nReadLen=0;	//总共读取长度
		UINT nReadCount=0;	//每次读取长度
		UINT nCrc32=0;	//校验值
		do
		{
			Read(buf, g_kcCopyBuf, &nReadCount);		//每次读取g_kcCopyBuf个字节,实际读取nReadCount个
			nReadLen+=nReadCount;	//总共读取长度
			nCrc32=m_FastCrc.CalcCrc32(nCrc32,buf,nReadCount);	//计算总共读取长度的CRC
		}
		while(nReadCount == g_kcCopyBuf);	//如果已经到尾则读不到g_kcCopyBuf

		*pCrc=nCrc32;
		*pSize=nReadLen;

		Seek(nOldPos,STREAM_SEEK_SET,NULL); //回到原来位置

		return m_Info.fValid?S_OK:S_FALSE;
	}

	//不实现
	HRESULT CEncryptStream::SetSize(
		UINT nNewSize
		)
	{
		return E_NOTIMPL;
	}

	//不实现
	HRESULT CEncryptStream::GetBuf(
		BYTE8** ppBuf,
		DWORD32* pSize
		)
	{
		return E_NOTIMPL;
	}

	HVRESULT CEncryptStream::CheckHead(EncryptStreamHead& head)
	{
		RTN_HR_IF_FAILED(m_pIOStream->Seek(0, STREAM_SEEK_SET, NULL) );
		RTN_HR_IF_FAILED(m_pIOStream->Read(&head,sizeof(EncryptStreamHead),NULL) );

		return head.Tag == 0x77748783 ? S_OK : S_FALSE;
	}

	HVRESULT CEncryptStream::CheckTail(EncryptStreamTail& tail)
	{
		UINT nOldPos = 0;
		int nTailSize=sizeof(EncryptStreamTail);
		RTN_HR_IF_FAILED(m_pIOStream->Seek(0, STREAM_SEEK_CUR, &nOldPos));
		RTN_HR_IF_FAILED(m_pIOStream->Seek(-nTailSize, STREAM_SEEK_END, NULL));
		RTN_HR_IF_FAILED(m_pIOStream->Read(&tail,nTailSize,NULL));

		INT nNewPos = 0;
		nNewPos=nOldPos;

		RTN_HR_IF_FAILED(m_pIOStream->Seek(nNewPos,STREAM_SEEK_SET,NULL));

		return tail.Valid==0x6586?S_OK:S_FALSE;
	}

	HVRESULT CEncryptStream::UpdateHead(const EncryptStreamHead& head,vlong& keydat)
	{
		//write head info
		RTN_HR_IF_FAILED(m_pIOStream->Write(&head, sizeof(EncryptStreamHead), NULL) );

		//write keydat(encrypted with rsa)
		if (head.RsaDatLen !=0)
		{
			BYTE8* pbBuf;
			UINT nSize;

			keydat.getBuffer(&pbBuf,&nSize);

			RTN_HR_IF_FAILED(m_pIOStream->Write(pbBuf,nSize,NULL));
		}

		return S_OK;
	}

	//随机初始化内存块
	HVRESULT CEncryptStream::RandMem(BYTE8* pbBuf,UINT nSize)
	{
		if (!pbBuf) return E_POINTER;
		for (UINT i=0;i<nSize;i++)
		{
			*pbBuf = rand()%256;
			pbBuf++;
		}
		return S_OK;
	}

	HVRESULT CEncryptStream::Flush(void)
	{
        if (!m_Info.fWrite) return S_OK; //如果是读取不FLUSH

		if (m_Info.nAvailBufSize != m_Info.nEncryptBufSize) //缓存非空
		{
			//申请写入缓存
			BYTE8* pbWriteBuf =(BYTE8*) HV_AllocMem(m_Info.nBlockSize);
			if (!pbWriteBuf) return E_OUTOFMEMORY;

			//填充随机数据
			RandMem(pbWriteBuf,m_Info.nBlkOffset);
			RandMem(pbWriteBuf + m_Info.nBlkOffset + m_Info.nEncryptBufSize, m_Info.nBlockSize - m_Info.nBlkOffset - m_Info.nEncryptBufSize);

			//加密
			if ( ((m_Info.nCurBlockNum + 1) % m_Info.nInterval) == 0)
			{
				encrypt_block(m_pbBuf,m_Info.nEncryptBufSize,pbWriteBuf + m_Info.nBlkOffset,m_rgKey);
			}
			else
			{
				HV_memcpy(pbWriteBuf + m_Info.nBlkOffset, m_pbBuf, m_Info.nEncryptBufSize);
			}

			//写入IO流
			RTN_HR_IF_FAILED(m_pIOStream->Write(pbWriteBuf,m_Info.nBlockSize,NULL));

			//释放写入缓存
			HV_FreeMem(pbWriteBuf,m_Info.nBlockSize);
		}

		EncryptStreamTail tail;

		tail.Size=m_Info.nStreamSize;  //总长度
		tail.Crc=m_Info.nCrc32;	//校验值
		tail.Reserved1=0;
		tail.Reserved2=0;
		tail.Valid=0x6586;		//VALID FLAG

		RTN_HR_IF_FAILED( m_pIOStream->Write(&tail,sizeof(EncryptStreamTail),NULL));	//写入尾信息
		return S_OK;
	}
}
