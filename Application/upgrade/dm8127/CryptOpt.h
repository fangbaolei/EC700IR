#ifndef __CRYPT_OPT_H__
#define __CRYPT_OPT_H__
//加密芯片操作相关
class CCryptOpt
{
public:

	//初始化加密功能
	static int CryptInit(void);

	//烧写AT88(烧NC和KEY)
	static int InitCryptConfig(unsigned char* nc, unsigned char* key);

	//加密读
	static int CryptRead(unsigned int unAddr,unsigned char *pBuf,unsigned int *Len);
	//int CryptWrite()

	//熔断熔丝
	static int FuseWrite(void);

	//读取熔丝状态
	static int FuseGetStatus(unsigned char *pvalue);

	//解密数据
	static int DecryptUserData(unsigned char *data, unsigned int len);

	//简单解密数据，用于解密内核文件系统等
	static int SimpleDecryptData(unsigned char *data, unsigned int len,unsigned long *pos);

private:
	static int GetNcKey(unsigned int (*rgdwNC)[4], unsigned int (*prgdwKey)[4]);

	static int xxtea_decrypt(
		unsigned char* pbIn ,			//输入
		unsigned int nLenght ,		//输入长度
		unsigned int* pKey			//128位密钥
		);

	static int crypt(unsigned char *addr, unsigned int len, unsigned long* pos);

};
#endif /*__CRYPT_OPT_H__*/