#ifndef _GENERATEGC_H_
#define _GENERATEGC_H_

#if SECURITY_IC_INIT || ADV_SECURITY_INIT || NML_SECURITY_INIT || HIGH_SECURITY_INIT

#define MX (z>>5^y<<2)+(y>>3^z<<4)^(sum^y)+(k[p&3^e]^z)

//固定输出长度加密
//不附加长度信息,  输出长度 = 输入长度 = 加密后数据长度, 应为4的整数倍
static inline int encrypt_block(
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

inline void GenerateGc(PBYTE8 pbNc, PBYTE8 pbGc)
{
	DWORD32 rgdwKey[4] = {XXTEA_KEY0, XXTEA_KEY1, XXTEA_KEY2, XXTEA_KEY3};
	encrypt_block(pbNc, 8, pbGc, rgdwKey);
}
#endif

#endif

