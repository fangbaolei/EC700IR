///////////////////////////////////////////////////////////
//  CSWUtils.h
//  Implementation of the Class CSWUtils
//  Created on:      28-二月-2013 14:09:47
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_D05791F0_16D4_41b3_A75B_85744A71101C__INCLUDED_)
#define EA_D05791F0_16D4_41b3_A75B_85744A71101C__INCLUDED_
#include "SWBaseType.h"
#include "SWErrCode.h"
/**
 * @brief 工具类
 */
class CSWUtils
{
public:
	CSWUtils();
	virtual ~CSWUtils();

  static WORD BYTE8_WORD16(BYTE * pbData);
	static DWORD BYTE8_DWORD32(BYTE * pbData);

  static HRESULT WORD16_BYTE8(WORD wValue, PBYTE pbData);
	static HRESULT DWORD32_BYTE8(DWORD dwValue, PBYTE pbData);

	static HRESULT ROUND_DIV(INT sdwDividend, INT sdwDivisor);
	static HRESULT ROUND_DIV_NEGATIVE(INT sdwDividend, INT sdwDivisor);
	static HRESULT ROUND_DIV_POSITIVE(INT sdwDividend, INT sdwDivisor);

  static UINT CalcCrc32(UINT nCrc32, BYTE* pbBuf, UINT nLen);

	static VOID RGB2YUV(BYTE r, BYTE g, BYTE b, BYTE *y, BYTE *u, BYTE *v);
	static VOID YUV2RGB(BYTE y, BYTE u, BYTE v, BYTE *r, BYTE *g, BYTE *b);
	static INT HexStringToBytes(LPCSTR strCmd, BYTE* pbCmd, INT iLen);
	/* BEGIN: Modified by huangdch, 2015/8/19 
	*按字符szCh分隔返回第num个字段的值,测试用*/
	static int Strtok(char* szList, const char* szCh, const int num);
};

#endif // !defined(EA_D05791F0_16D4_41b3_A75B_85744A71101C__INCLUDED_)

