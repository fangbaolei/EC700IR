#include "crypt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MX (((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[(p&3)^e]^z))

//扩展密钥为4字节整倍数
UINT* xxtea_initkey(
				  void* pKeyIn,
				  UINT nKeyLenght,
				  UINT* pKeyOut
				  )
{
	UINT nMaxLenght=4*sizeof(UINT);
	if (nKeyLenght < 1) return NULL;
	if (nKeyLenght > nMaxLenght ) nKeyLenght = nMaxLenght;

	memset(pKeyOut,0,nMaxLenght);
	memcpy(pKeyOut,pKeyIn,nKeyLenght);

	return pKeyOut;
}

//加密
//最后附加原始数据长度
//返回加密数据总长度(包括原始长度信息)
//输出缓存最小长度 =  (输入长度+3)/4*4+4
int xxtea_encrypt(
				  BYTE8* pbIn ,          //输入
				  UINT nLenght ,     //输入长度
				  BYTE8* pbOut ,       //输出
				  UINT* pKey				//128位密钥
				  )
{
	UINT* v;
	UINT* k;
	INT n;
	UINT z,y,delta,sum,e;
	int p,q;
	UINT nBlockNum;

	if ( !pbIn || !pbOut || !pKey || nLenght < 1) return -1;

	v=(UINT*)pbOut;
	k=pKey;
	nBlockNum = (nLenght + 3)>>2;

	memcpy(pbOut,pbIn,nLenght);
	*(int*)(pbOut + (nBlockNum<<2)) = nLenght;	//最后一块是原始数据长度

	n = nBlockNum; //输出数组（包括长度信息）最后的INDEX

	z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
	q = 6 + 52 / (n + 1);
	while (q-- > 0)
	{
		sum += delta;
		e = sum >> 2 & 3;
		for (p = 0; p < n; p++)
		{
			y = v[p + 1];
			z = (v[p] += MX);
		}
		y = v[0];
		z = (v[n] += MX);
	}

	return (nBlockNum + 1)<<2;  //返回加密后的总长度
}

//解密
//返回原始数据长度
//解密失败时长度信息无效,最大为 (输入长度/4*4 - 4)
int xxtea_decrypt(
				  BYTE8* pbIn ,       //输入
				  UINT nLenght ,   //输入长度
				  UINT* pKey       //128位密钥
				  )
{
	UINT* v;
	UINT* k;
	UINT n;
	UINT z,y,delta,sum,e;
	int p,q;
	UINT nBlockNum;
	int nOriLenght;

	if ( !pbIn || !pKey || nLenght < 8) return -1;
	v=(UINT*)pbIn;
	k=pKey;
	nBlockNum = nLenght>>2;

	n = nBlockNum - 1;

	z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
	q = 6 + 52 / (n + 1);
	sum = (UINT)(q * delta);
	while (sum != 0)
	{
		e = sum >> 2 & 3;
		for (p = n; p > 0; p--)
		{
			z = v[p - 1];
			y = (v[p] -= MX);
		}
		z = v[n];
		y = (v[0] -= MX);
		sum -= delta;
	}

	nOriLenght = *(int*)(pbIn + ((nBlockNum - 1)<<2)); //原始长度

	return (nOriLenght <= ((nBlockNum - 1)<<2) )?nOriLenght:((nBlockNum - 1)<<2);
}

//固定输出长度加密
//不附加长度信息,  输出长度 = 输入长度 = 加密后数据长度, 应为4的整数倍
int encrypt_block(
				  BYTE8* pbIn,
				  UINT nLenght,
				  BYTE8* pbOut,
				  UINT* pKey
				  )
{
	UINT* v;
	UINT* k;
	INT n;
	UINT z,y,delta,sum,e;
	int p,q;
	UINT nBlockNum;

	if ( !pbIn || !pbOut || !pKey || nLenght < 8 ) return -1;
	v=(UINT*)pbOut;
	k=pKey;

	nBlockNum=nLenght>>2;

	memcpy(pbOut,pbIn,nLenght);

	n = nBlockNum - 1;

	z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
	q = 6 + 52 / (n + 1);
	while (q-- > 0)
	{
		sum += delta;
		e = sum >> 2 & 3;
		for (p = 0; p < n; p++)
		{
			y = v[p + 1];
			z = (v[p] += MX);
		}
		y = v[0];
		z = (v[n] += MX);
	}

	return nLenght;
}

//固定输出长度解密
//输出长度 = 输入长度
int decrypt_block(
				  BYTE8* pbIn,
				  UINT nLenght,
				  UINT* pKey
				  )
{
	xxtea_decrypt(pbIn , nLenght , pKey);
	return nLenght;
}

//将pbKey中第nIndex个字节由pbUID扩展为256字节,存放在pbBuf
void HS_EncodeKey(int nIndex, unsigned char* pbKey, unsigned char* pbBuf, unsigned char* pbUID)
{
	int i;
	unsigned char pbTmpUID[6]={0};
	unsigned char ShuffleTab[6]={1,5,3,0,4,2};
	unsigned int nP;

	for ( i = 0; i < 256; i++)
	{
		pbBuf[i]=rand()%256;
	}

	memcpy(pbTmpUID,pbUID,6);

	pbTmpUID[ShuffleTab[nIndex%6]] = (nIndex == 0)?0x67:pbKey[nIndex -1];

	nP = ((pbTmpUID[0]^pbTmpUID[4]) + (pbTmpUID[1]^pbTmpUID[3]) + (pbTmpUID[2]^pbTmpUID[5]))/3;

	pbBuf[211 + nP%45] = pbBuf[nP%181]^pbBuf[nP%103]^pbBuf[nP%211]^pbBuf[nP%71]^pbBuf[nP%23]^pbBuf[nP%157]^pbBuf[nP%131]^pbTmpUID[nP%6]^pbKey[nIndex];
}

//将256字节pbBuf按pbUID还原为pbKey第nIndex字节
void HS_DecodeKey(int nIndex, unsigned char* pbKey, unsigned char* pbBuf, unsigned char* pbUID)
{
	unsigned char pbTmpUID[6]={0};
	unsigned char ShuffleTab[6]={1,5,3,0,4,2};
	unsigned int nP;

	memcpy(pbTmpUID, pbUID, 6);

	pbTmpUID[ShuffleTab[nIndex%6]] = (nIndex == 0)?0x67:pbKey[nIndex -1];

	nP = ((pbTmpUID[0]^pbTmpUID[4]) + (pbTmpUID[1]^pbTmpUID[3]) + (pbTmpUID[2]^pbTmpUID[5]))/3;

	pbKey[nIndex] = pbBuf[nP%181]^pbBuf[nP%103]^pbBuf[nP%211]^pbBuf[nP%71]^pbBuf[nP%23]^pbBuf[nP%157]^pbBuf[nP%131]^pbTmpUID[nP%6]^pbBuf[211 + nP%45];
}

//将pbBuf用rgCode进行异或加密
void HS_XorEnc(unsigned char* rgCode, unsigned int dwCodeLen, unsigned char* pbBuf, unsigned int dwBufLen)
{
	unsigned int i;
	for ( i = 0; i < dwBufLen; i++)
	{
			pbBuf[i] ^= rgCode[i%dwCodeLen];
	}
}

