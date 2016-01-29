#include "CryptOpt.h"
#include <string.h>
#include "SWLog.h"

#include "drv_device.h"

#include "SWUtils.h"

#define PRINT	SW_TRACE_NORMAL

#define FUN_ARG_NULL_CHECK(arg)	{if (NULL == arg){PRINT("Err: arg %s is null\n",#arg);return -1;}}

int CCryptOpt::CryptInit(void)
{
	return drv_sec_crypt_init(DEVID_IIC_AT88SC);
}

int CCryptOpt::InitCryptConfig(unsigned char* nc, unsigned char* key)
{
	FUN_ARG_NULL_CHECK(nc);
	FUN_ARG_NULL_CHECK(key);

	return drv_sec_init_config(DEVID_IIC_AT88SC,nc,key);
}


int CCryptOpt::CryptRead(unsigned int unAddr,unsigned char *pBuf,unsigned int *Len)
{
	FUN_ARG_NULL_CHECK(pBuf);
	FUN_ARG_NULL_CHECK(Len);

	return drv_sec_crypt_read(DEVID_IIC_AT88SC,unAddr,pBuf,Len);
}
//int CryptWrite()

int CCryptOpt::FuseWrite(void)
{
	return drv_sec_fuse_write(DEVID_IIC_AT88SC);
}

int CCryptOpt::FuseGetStatus(unsigned char *pvalue)
{
	FUN_ARG_NULL_CHECK(pvalue);

	return drv_sec_fuse_read(DEVID_IIC_AT88SC,pvalue);
}

int CCryptOpt::DecryptUserData(unsigned char *data, unsigned int len)
{
	unsigned int rgdwKey[4] = {0};
	unsigned int rgdwNC[4] = {0};
	if ( 0 == GetNcKey(&rgdwNC, &rgdwKey) )
	{
		xxtea_decrypt((unsigned char*)rgdwKey, 16, (unsigned int*)rgdwNC);
		xxtea_decrypt(data, len, (unsigned int*)rgdwKey);  // 正式版
		//xxtea_decrypt(data, len, key_HvEncryptFile);  // 内部调试版
		return 0;
	}
	return -1;
}

int CCryptOpt::GetNcKey(unsigned int (*rgdwNC)[4], unsigned int (*prgdwKey)[4])
{
	unsigned char buffer[32] = {0};
	unsigned int len = 0;
	int ret = 0;

	// nc
	len = 0x07;
	ret = drv_sec_std_read(DEVID_IIC_AT88SC, 0x19, buffer, &len);
	if (ret < 0)
	{
		PRINT("Err: read nc error!\n");
		return -1;
	}

	memcpy(rgdwNC, buffer, 16);

	// init crypt
	ret = drv_sec_crypt_init(DEVID_IIC_AT88SC);

	// key
	len = 0x10;
	ret = drv_sec_crypt_read(DEVID_IIC_AT88SC, 0x40, buffer, &len); // key
	if (ret < 0)
	{
		PRINT("Err: crypt read key error!\n");
		return -1;
	}

	memcpy(prgdwKey, buffer, 16);

	PRINT("----nc: \n");
	//DUMP((unsigned char*)rgdwNC, 16);
	PRINT("----key: \n");
	//DUMP((unsigned char*)prgdwKey, 16);

	return 0;
}

#define MX (((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[p&3^e]^z))

//解密
//返回原始数据长度
//解密失败时长度信息无效,最大为 (输入长度/4*4 - 4)
int CCryptOpt::xxtea_decrypt(
	unsigned char* pbIn ,       //输入
	unsigned int nLenght ,   //输入长度
	unsigned int* pKey       //128位密钥
	)
{
	unsigned int* v;
	unsigned int* k;
	int n;
	unsigned int z,y,delta,sum,e;
	int p,q;
	unsigned int nBlockNum;
	unsigned int nOriLenght;

	if ( !pbIn || !pKey || nLenght < 8) return -1;
	v=(unsigned int*)pbIn;
	k=pKey;
	nBlockNum = nLenght>>2;

	n = nBlockNum - 1;

	z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e = 0;
	q = 6 + 52 / (n + 1);
	sum = (unsigned int)(q * delta);
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

	nOriLenght = *(unsigned int*)(pbIn + ((nBlockNum - 1)<<2)); //原始长度

	return (nOriLenght <= ((nBlockNum - 1)<<2) )?nOriLenght:((nBlockNum - 1)<<2);
}

int CCryptOpt::SimpleDecryptData(unsigned char *data, unsigned int len,unsigned long *pos)
{
#define M_NUM 0x6D679BC7
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3, m4) (m1*(M_NUM&m2)*m3*m4)
#define ENCODE(m1, m2, m3, m4) (m1 ^ MAKE_XOR_CODE(m2,m3,m4,M_NUM))

	//unsigned long pos = 0;
	crypt(data,len,pos);

	return 0;
}

int CCryptOpt::crypt(unsigned char *addr, unsigned int len, unsigned long* pos)
{
	unsigned int k0 = 0xA2D1EABA;
	unsigned int k1 = 0;
	unsigned char k2 = 0;
	k1 = CSWUtils::CalcCrc32(0, (BYTE*)&k0, 4);
	k2 = MAKE_KEY(k1, (k1>>8), (k1>>16));
	while ( len-- )
	{
		*addr++ = ENCODE(*addr, k2, (unsigned char)*pos, (k1>>24));
		(*pos)++;
	}
	return 1;
}
