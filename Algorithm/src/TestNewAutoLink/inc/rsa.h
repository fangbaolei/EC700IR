#ifndef __RSA_H__
#define __RSA_H__ 

#include "swBaseType.h"
#include "vlong.h"

#define RSA_BITCNT 1024
#define RSA_BYTE8CNT RSA_BITCNT/8

typedef struct _RSA_KEY
{
	vlong key;
	vlong modulus;
} RSA_KEY;

typedef struct _RSA_PRIVATE_KEY
{
	vlong p;
	vlong q;
	vlong dp;
	vlong dq;
	vlong qinv;
} RSA_PRIVATE_KEY;

class public_key
{
  public:
  vlong m,e;
  vlong encrypt( const vlong& plain ); // Requires 0 <= plain < m
};

class private_key : public public_key
{
  public:
  vlong p,q;
  vlong decrypt( const vlong& cipher );

  void create( const char * r1, const char * r2 );
  // r1 and r2 should be null terminated random strings
  // each of length around 35 characters (for a 500 bit modulus)
};

extern void rsa_create_key(vlong& e, vlong& d, vlong& m );
extern void rsa_create_key(RSA_KEY& pub, RSA_PRIVATE_KEY& pri);

extern vlong rsa_encrypt( const vlong& plain, const vlong& e , const vlong& m );
extern vlong rsa_decrypt( const vlong& cipher, const vlong& d, const vlong& m);
extern vlong rsa_decrypt( const vlong& cipher, const vlong& p, const vlong& q, 
									const vlong& dp, const vlong& dq,const vlong& qinv);

extern vlong rsa_encrypt( const vlong& plain , RSA_KEY& pubkey );
extern vlong rsa_decrypt(const vlong& cipher, RSA_KEY& prikey );
extern vlong rsa_decrypt(const vlong& cipher, RSA_PRIVATE_KEY& prikey);

extern void rsa_encrypt_pkcs(unsigned char* plain, unsigned int plain_len, vlong& cipher, RSA_KEY& pubkey);
extern unsigned int rsa_decrypt_pkcs(unsigned char* chiperbuf, unsigned int chiper_len, unsigned char* plainbuf, unsigned int plain_len, RSA_KEY& prikey);
extern unsigned int  rsa_decrypt_pkcs(unsigned char* chiperbuf, unsigned int chiper_len, unsigned char* plainbuf, unsigned int plain_len, RSA_PRIVATE_KEY& prikey);

extern int PKCS_OS2IP(BYTE8* pbBuf,UINT nSize ,vlong& v);
extern int PKCS_I2OSP(vlong& v,BYTE8* pbBuf,UINT nSize);
extern int PKCS_Encode(BYTE8* pOriMsg,UINT nOriLen,BYTE8* pEncMsg,UINT nEncLen);
extern int PKCS_Decode(BYTE8* pEncMsg,UINT nEncLen,BYTE8* pOriMsg,UINT& nOriSize);
#endif
