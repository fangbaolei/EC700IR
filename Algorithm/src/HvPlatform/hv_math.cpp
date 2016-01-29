#include "swbasetype.h"
#include "hv_math.h"

#ifdef CHIP_6455
#pragma DATA_SECTION ("fastdata");
#endif
BYTE8 sqrtTable[MAX_SQRT_NUM];

// Exp查找表
#ifdef CHIP_6455
#pragma DATA_SECTION ("fastdata");
#endif
DWORD32 wLowBit[EXP_TABLE_LEN];
#ifdef CHIP_6455
#pragma DATA_SECTION ("fastdata");
#endif
DWORD32 wHighBit[EXP_TABLE_LEN];
#ifdef CHIP_6455
#pragma DATA_SECTION ("fastdata");
#endif
DWORD32 wLowBitN[EXP_TABLE_LEN];
#ifdef CHIP_6455
#pragma DATA_SECTION ("fastdata");
#endif
DWORD32 wHighBitN[EXP_TABLE_LEN];

// Exp查找表2
#ifdef CHIP_6455
#pragma DATA_SECTION ("fastdata");
#endif
WORD16 waExpValue[EXP_TABLE2_LEN];

//量化exp,超过20不能计算，由函数外保证
DWORD32 quanExpPositive(DWORD32 dwNum) {

	if (dwNum > 726817) {			//计算上限
		return 0xffffffff;
	}

	DWORD32 dwLowValue = wLowBit[dwNum & 0x3ff];
	DWORD32 dwHighValue = wHighBit[(dwNum >> 10) & 0x3ff];

	DWORD32 dwValue;

	if (dwHighValue >= 0x800000) {
		//低位为17位，高位最多允许23位，不然DSP会溢出(DSP只有40位)
		if (dwHighValue >= 0x8000000) {
			//先移动9位
			dwHighValue = dwHighValue >> 9;
			dwValue = (DWORD32)(((QWORD64)dwHighValue * dwLowValue) >> 7);
		} else {
			//先移动4位
			dwHighValue = dwHighValue >> 4;
			dwValue = (DWORD32)(((QWORD64)dwHighValue * dwLowValue) >> 12);
		}
	} else {
		dwValue = (DWORD32)(((QWORD64)dwHighValue * dwLowValue) >> QuanBit);
	}
	return dwValue;
}

//量化exp,超过20不能计算，由函数外保证
DWORD32 quanExpNegative(DWORD32 dwNum) {

	if (dwNum >= 1048576) {								//16 * 65536
		return 0;									//e-16 以上作0计算
	}

	DWORD32 dwLowValue = wLowBitN[dwNum & 0x3ff];
	DWORD32 dwHighValue = wHighBitN[(dwNum >> 10) & 0x3ff];
	DWORD32 dwValue;
	if (dwLowValue == 65536) {
		dwValue = dwHighValue;
	} else {
		dwValue = (dwHighValue * (WORD16)dwLowValue + 32768) >> QuanBit;
	}

	return dwValue;
}


//iShift为量化位数,必须为偶数
DWORD32 quanSqrt(DWORD32 dwNum, DWORD32 iShift) {
	DWORD32 dwSqrt;
	DWORD32 num = dwNum;
	int moveCount = 0;		//移动计数器,负数表示右移，正数表示左移(1表示移动2位)

	if (dwNum == 0)
		return 0;

	//将量化数转到0~65536的取值范围（以查表），并计算移位次数
	if (dwNum > 65536) {
		while(num > 65536) {
			num = num >> 2;
			moveCount--;			//
		} //等于或小于65536结束
	} else if (dwNum < 65536) {
		while(num < 65536) {
			num = num << 2;
			moveCount++;
		} //大于或等于65536结束
		if (num > 65536) {
			num = num >> 2;
			moveCount--;
		}
	}//结果为num=65536范围内的正数,moveCount为移位次数

	//查表得到sqrt的数值
	if (num == 65536)
		dwSqrt = 256;
	else {
		dwSqrt = (SDWORD32)sqrtTable[num - 1];
	}

	moveCount = (iShift >> 1) - moveCount;			//考虑量化
	if (moveCount >= 0) {
		dwSqrt = dwSqrt << moveCount;
	} else {
		dwSqrt = dwSqrt >> (-moveCount);
	}
	return dwSqrt;
}


