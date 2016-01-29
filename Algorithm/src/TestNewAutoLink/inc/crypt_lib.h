#ifndef __CRYPT_2_H__
#define __CRYPT_2_H__

#define _XCOS_VERSION 0x0200

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include <windows.h>
#else
#define WINAPI
#endif

#ifndef MAX_RSA_MODULUS_BITS

#define DA_MD2 3
#define DA_MD5 5
#define DA_SHS 0x1a

/* RSA key lengths.
 */
#define MIN_RSA_MODULUS_BITS	508
#define MAX_RSA_MODULUS_BITS	1024
#define MAX_RSA_MODULUS_LEN		((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS		((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN		((MAX_RSA_PRIME_BITS + 7) / 8)

/* Error codes.
 */
#define RE_SUCCESS				0x0000
#define RE_CONTENT_ENCODING		0x0400
#define RE_DATA					0x0401
#define RE_DIGEST_ALGORITHM		0x0402
#define RE_ENCODING				0x0403
#define RE_KEY					0x0404
#define RE_KEY_ENCODING			0x0405
#define RE_LEN					0x0406
#define RE_MODULUS_LEN			0x0407
#define RE_NEED_RANDOM			0x0408
#define RE_PRIVATE_KEY			0x0409
#define RE_PUBLIC_KEY			0x040a
#define RE_SIGNATURE			0x040b
#define RE_SIGNATURE_ENCODING	0x040c
#define RE_ENCRYPTION_ALGORITHM	0x040d

/* RSA public and private key.*/
typedef struct {
  unsigned int bits;                           /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];           /* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct {
  unsigned int bits;                           /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
  unsigned char publicExponent[MAX_RSA_MODULUS_LEN];     /* public exponent */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];          /* private exponent */
  unsigned char prime[2][MAX_RSA_PRIME_LEN];               /* prime factors */
  unsigned char primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
  unsigned char coefficient[MAX_RSA_PRIME_LEN];          /* CRT coefficient */
} R_RSA_PRIVATE_KEY;
#endif

#define COS_RSA_MODULUS_LEN	0x80
#define COS_RSA_EXP_LEN		0x04
#define COS_RSA_PRIME_LEN	0x40

#if _XCOS_VERSION < 0x0200
/* COS format of RSA key
*/
typedef struct {
  unsigned char modulus[COS_RSA_MODULUS_LEN];  /* modulus */
  unsigned char exponent[COS_RSA_EXP_LEN]; /* public exponent */
} COS_RSA_PUBLIC_KEY;

typedef struct {
  unsigned char p[COS_RSA_PRIME_LEN];   /* p */
  unsigned char q[COS_RSA_PRIME_LEN];   /* q */
  unsigned char exponent[COS_RSA_EXP_LEN]; /* public exponent */
} COS_RSA_PRIVATE_KEY;

#else
/* tlv record format (tag-length-value) */
typedef struct {
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x80];
	}n;
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x04];
	}e;
} COS_RSA_PUBLIC_KEY;

typedef struct {
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x80];
	}n;
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x80];
	}d;
} COS_RSA_PRIVATE_KEY_2;

typedef struct {
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x40];
	}p;
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x40];
	}q;
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x40];
	}dp;
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x40];
	}dq;
	struct {
		char tag;
		unsigned char length;
		unsigned char value[0x40];
	}qinv;
} COS_RSA_PRIVATE_KEY;
#endif /* if _XCOS_VERSION */


/*RSA key generate functions*/
int WINAPI X_GenerateRsaKeys(
	COS_RSA_PUBLIC_KEY *cPubKey,
	COS_RSA_PRIVATE_KEY *cPriKey
);
int WINAPI R_GenerateRsaKeys(
	R_RSA_PUBLIC_KEY *pubKey,
	R_RSA_PRIVATE_KEY *priKey
);

/*key type convert function*/
int WINAPI X_Pub2Cos(
	COS_RSA_PUBLIC_KEY *cPubKey,
	R_RSA_PUBLIC_KEY *pubKey
);
int WINAPI X_Pri2Cos(
	COS_RSA_PRIVATE_KEY *cPriKey,
	R_RSA_PRIVATE_KEY *priKey
);

int WINAPI X_Pri2CosEx(
	int iOutType,
	COS_RSA_PRIVATE_KEY *cPriKey,
	R_RSA_PRIVATE_KEY *priKey
);

int WINAPI X_Cos2Pub(
	R_RSA_PUBLIC_KEY *pubKey,
	COS_RSA_PUBLIC_KEY *cPubKey
);
int WINAPI X_Cos2Pri(
	R_RSA_PRIVATE_KEY *priKey,
	COS_RSA_PRIVATE_KEY *cPriKey
);

/*RSA Encrypt/Decrypt function*/
int WINAPI RSAPublicEncrypt(
	unsigned char *output,
	unsigned int *outputLen,
	unsigned char *input,
	unsigned int inputLen,
	R_RSA_PUBLIC_KEY *publicKey
);
int WINAPI RSAPrivateDecrypt(
	unsigned char *output,
	unsigned int *outputLen,
	unsigned char *input,
	unsigned int inputLen,
	R_RSA_PRIVATE_KEY *privateKey
);

/*digest function*/
int WINAPI Digest(
	int digestAlgorithm,
	unsigned char *plain,
	unsigned int plainLen,
	unsigned char *digest,
	unsigned int *digestLen
);

/*sign function*/
int WINAPI Sign(
	int digestAlgorithm,
	unsigned char *plain,
	unsigned int plainLen,
	unsigned char *signature,
	unsigned int *signatureLen,
	R_RSA_PRIVATE_KEY *privateKey
);

/*verify function*/
int WINAPI Verify(
	int digestAlgorithm,
	unsigned char *plain,
	unsigned int plainLen,
	unsigned char *signature,
	unsigned int signatureLen,
	R_RSA_PUBLIC_KEY *publicKey
);

/*DES function*/
int WINAPI DES(
	unsigned char *key,
	int encrypt,
	unsigned char *output,
	unsigned char *input,
	unsigned int inputLen
);

int WINAPI TDES(
	unsigned char *key,
	int encrypt,
	unsigned char *output,
	unsigned char *input,
	unsigned int inputLen
);

/*TDES function*/
int WINAPI DES3(
	unsigned char *key,
	int encrypt,
	unsigned char *output,
	unsigned char *input,
	unsigned int inputLen
);


#ifdef __cplusplus
}
#endif


#endif //__CRYPT_2_H__