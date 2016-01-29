#include "rsa.h"
#include "prime.h"

#include "stdlib.h"
#include "string.h"
#include "time.h"

static vlong from_str( const char * s )
{
	vlong x = 0;
	while (*s)
	{
		x = x * 256 + (unsigned char)*s;
		s += 1;
	}
	return x;
}

void private_key::create(const char * r1, const char * r2 )
{
	// Choose primes
	prime_factory pf;

	vlong s1 = from_str(r1);
	vlong s2 = from_str(r2);

	p = pf.find_prime( s1 );
	q = pf.find_prime( s2 );
	if ( p > q )  // *NOT* SAME AS PKCS#1 STANDARD
	{
		vlong tmp = p;
		p = q;
		q = tmp;
	}

	// Calculate public key
	m = p*q;
	e = 65537;
	while ( gcd(p-1,e) != 1 || gcd(q-1,e) != 1 )
		e += 2;
}

vlong public_key::encrypt( const vlong& plain )
{
	return modexp( plain, e, m );
}

vlong private_key::decrypt( const vlong& cipher )
{
	// Calculate values for performing decryption
	// These could be cached, but the calculation is quite fast
	vlong d = modinv( e, (p-1)*(q-1) );
	vlong u = modinv( p, q );
	vlong dp = d % (p-1);
	vlong dq = d % (q-1);

	// Apply chinese remainder theorem
	vlong a = modexp( cipher % p, dp, p );
	vlong b = modexp( cipher % q, dq, q );
	if ( b < a ) b += q;
	return a + p * ( ((b-a)*u) % q );
}

//创建公私钥
void rsa_create_key(RSA_KEY& pub, RSA_PRIVATE_KEY& pri)
{
	unsigned char prand[2][128],tc;
	UINT i,j, nCount = 64;

	srand((unsigned)time(NULL));

	for(i=0; i<2; i++)
	{
		for(j=0; j<nCount; j++)
		{
			tc = (char)(0x41+rand()%0xAF);
			prand[i][j] = tc;
		}
		prand[i][j]=0;
	}

	// Choose primes
	prime_factory pf;

	vlong s1 = from_str((const char*)prand[0]);
	vlong s2 = from_str((const char*)prand[1]);

	vlong tp = pf.find_prime( s1 );
	vlong tq = pf.find_prime( s2 );

	if ( tq > tp ) //make sure p > q
	{
		vlong tmp = tp;
		tp = tq;
		tq = tmp;
	}

	// Calculate public key
	pub.modulus = tp*tq;

	pub.key = 65537;
	while ( gcd(tp-1,pub.key) != 1 || gcd(tq-1,pub.key) != 1 ) pub.key += 2;

	// Calculate private key
	pri.p=tp;
	pri.q=tq;
	pri.dp=modinv(pub.key,tp-1);
	pri.dq=modinv(pub.key,tq-1);
	pri.qinv=modinv(tq,tp);
}

//创建公私钥
void rsa_create_key(vlong& e, vlong& d , vlong& m )
{
	unsigned char prand[2][128],tc;
	UINT i,j, nCount = 64;

	srand((unsigned)time(NULL));

	for(i=0; i<2; i++)
	{
		for(j=0; j<nCount; j++)
		{
			tc = (char)(0x41+rand()%0xAF);
			prand[i][j] = tc;
		}
		prand[i][j]=0;
	}

	// Choose primes
	prime_factory pf;

	vlong s1 = from_str((const char*)prand[0]);
	vlong s2 = from_str((const char*)prand[1]);

	vlong p = pf.find_prime( s1 );
	vlong q = pf.find_prime( s2 );

	// Calculate public key
	m = p*q;

	e = 65537;
	while ( gcd(p-1,e) != 1 || gcd(q-1,e) != 1 ) e += 2;

	// Calculate private key
	d = modinv( e, (p-1)*(q-1) );
}

//加密
vlong rsa_encrypt( const vlong& plain , const vlong& e , const vlong& m )
{
	if (e == 0 || m == 0) return 0;
	return modexp( plain, e, m );
}

//解密
vlong rsa_decrypt(
				  const vlong& cipher ,
				  const vlong& d,
				  const vlong& m
				  )
{
	if (d == 0 || m == 0) return 0;
	return modexp( cipher , d ,m);
}

//解密,用5元组形式的私钥
vlong rsa_decrypt(
				  const vlong& cipher,
				  const vlong& p,
				  const vlong& q,
				  const vlong& dp,
				  const vlong& dq,
				  const vlong& qinv
				  )
{
	if (p == 0 || q == 0 || dp == 0 || dq == 0 || qinv == 0) return 0;

	vlong m1 = modexp( cipher % p, dp, p );
	vlong m2 = modexp( cipher % q, dq, q );

	if ( m1 < m2 ) m1 += p;

	return m2 + q * ( ((m1-m2)*qinv) % p );
}

//加密
vlong rsa_encrypt( const vlong& plain , RSA_KEY& pubkey )
{
	return rsa_encrypt(plain,pubkey.key,pubkey.modulus);
}

//解密
vlong rsa_decrypt(const vlong& cipher, RSA_KEY& prikey)
{
	return rsa_decrypt(cipher,prikey.key,prikey.modulus);
}

//解密
vlong rsa_decrypt(const vlong& cipher, RSA_PRIVATE_KEY& prikey)
{
	return rsa_decrypt(cipher,prikey.p,prikey.q,prikey.dp,prikey.dq,prikey.qinv);
}

//按PKCS#1标准加密
void rsa_encrypt_pkcs(unsigned char* plain, unsigned int plain_len,vlong& cipher, RSA_KEY& pubkey)
{
	unsigned char pbEncBuf[128]={0};
	vlong tmp_plain;

	PKCS_Encode(plain,plain_len,pbEncBuf,128); //编码
	PKCS_OS2IP(pbEncBuf,128,tmp_plain);	//转为integer
	cipher = rsa_encrypt(tmp_plain,pubkey.key,pubkey.modulus); //加密
	PKCS_I2OSP(cipher,pbEncBuf,128); //转为octet string
	cipher.load((UINT*)pbEncBuf,128/4); //转为vlong保存
}

//按PKCS#1标准解密
unsigned int rsa_decrypt_pkcs(unsigned char* chiperbuf, unsigned int chiper_len, unsigned char* plainbuf, unsigned int plain_len, RSA_KEY& prikey)
{
	vlong t1 = 0,t2 = 0;
	PKCS_OS2IP(chiperbuf,chiper_len,t1);  //转为integer
	t2 = rsa_decrypt(t1, prikey.key, prikey.modulus); //解密
	PKCS_I2OSP(t2, chiperbuf, chiper_len);	//转为octet string
	PKCS_Decode(chiperbuf, chiper_len, plainbuf, plain_len); //解码
	return plain_len;
}

//按PKCS#1标准解密
unsigned int rsa_decrypt_pkcs(unsigned char* chiperbuf, unsigned int chiper_len, unsigned char* plainbuf, unsigned int plain_len, RSA_PRIVATE_KEY& prikey)
{
	vlong t1 = 0,t2 = 0;

	PKCS_OS2IP(chiperbuf,chiper_len,t1);  //转为integer

	t2 = rsa_decrypt(t1, prikey.p, prikey.q, prikey.dp, prikey.dq, prikey.qinv); //解密

	PKCS_I2OSP(t2, chiperbuf, chiper_len);	//转为octet string

	PKCS_Decode(chiperbuf, chiper_len, plainbuf, plain_len); //解码

	return plain_len;
}

//convert an octet string to a very-long-integer
int PKCS_OS2IP(BYTE8* pbBuf,UINT nSize ,vlong& v)
{
	if (pbBuf == NULL || nSize < 1) return -1;

	UINT nBufSize = (nSize + 3) / 4 * 4;

	BYTE8* pbTmpBuf = new BYTE8[nBufSize];
	memset(pbTmpBuf,0,nBufSize);

	for (int i = nSize - 1, j = 0; i >= 0 && j < (int)nBufSize; i--, j++)
	{
		pbTmpBuf[j] = pbBuf[i];
	}

	v.load((UINT*)pbTmpBuf, nBufSize / 4);

	delete[] pbTmpBuf;

	return 0;
}

//convert a very-long-integer to an octet string
int PKCS_I2OSP(vlong& v, BYTE8* pbBuf, UINT nSize)
{
	BYTE8* buf;
	UINT nLen;
	v.getBuffer(&buf,&nLen);

	for (int i = nSize - 1, j = 0 ; i >= 0 && j < (int)nLen ; i-- , j++)
	{
		pbBuf[i] = buf[j];
	}

	return 0;
}

//按PKCS#1标准加密
int PKCS_Encode(BYTE8* pOriMsg,UINT nOriLen,BYTE8* pEncMsg,UINT nEncLen)
{
	if (pOriMsg == NULL || pEncMsg == NULL) return -1;
	if (nEncLen - nOriLen < 11) return -1;

	srand((unsigned int)time(NULL));
	for (UINT i = 0 ; i < nEncLen ; i++)
	{
		pEncMsg[i] = 1 + rand()%0xff ;
	}

	memset(&pEncMsg[0],0x00,1);
	memset(&pEncMsg[1],0x02,1);
	memset(&pEncMsg[nEncLen-nOriLen-1],0x00,1);
	memcpy(&pEncMsg[nEncLen-nOriLen],pOriMsg,nOriLen);

	return 0;
}

//按PKCS#1标准解密
int PKCS_Decode(BYTE8* pEncMsg, UINT nEncLen, BYTE8* pOriMsg, UINT& nOriSize)
{
	if (pOriMsg == NULL || pEncMsg == NULL) return -1;

	if ( nEncLen < 11 ) return -1;

	UINT nPSLen = (UINT)strlen((const char*)(pEncMsg + 2));
	if ( nPSLen < 8 || nPSLen > nEncLen - 3 )
	{
		return -1;
	}

	nOriSize = (nOriSize > nEncLen - nPSLen -3)?(nEncLen - nPSLen - 3):nOriSize;

	memcpy(pOriMsg, pEncMsg + nPSLen + 3, nOriSize);

	return 0;
}







