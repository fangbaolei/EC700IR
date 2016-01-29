// 该文件编码必须是WINDOWS-936格式

#include "swplate.h"
#include <string.h>

int ProcessPlateNameAlpha(char* pszPlateName)
{
	char* pszBuff = pszPlateName;

	if (NULL == pszPlateName) return -1;

	if ( strncmp( pszBuff,"双",2 ) == 0 || strncmp( pszBuff,"摩",2 ) == 0)
	{
		if ((strncmp(pszBuff+2,"沈",2) == 0 ) ||
			(strncmp(pszBuff+2,"北",2) == 0 ) ||
			(strncmp(pszBuff+2,"兰",2) == 0 ) ||
			(strncmp(pszBuff+2,"济",2) == 0 ) ||
			(strncmp(pszBuff+2,"南",2) == 0 ) ||
			(strncmp(pszBuff+2,"成",2) == 0 ) ||
			(strncmp(pszBuff+2,"广",2) == 0 ) ||
			(strncmp(pszBuff+2,"军",2) == 0 ) ||
			(strncmp(pszBuff+2,"炮",2) == 0 ) ||
			(strncmp(pszBuff+2,"空",2) == 0 ) ||
			(strncmp(pszBuff+2,"海",2) == 0 ) ||
			(strncmp(pszBuff+2,"B",1) == 0 ) ||             //打开新军牌识别开关后判断军牌的车牌颜色
			(strncmp(pszBuff+2,"C",1) == 0 ) ||
			(strncmp(pszBuff+2,"E",1) == 0 ) ||
			(strncmp(pszBuff+2,"G",1) == 0 ) ||
			(strncmp(pszBuff+2,"H",1) == 0 ) ||
			(strncmp(pszBuff+2,"J",1) == 0 ) ||
			(strncmp(pszBuff+2,"K",1) == 0 ) ||
			(strncmp(pszBuff+2,"L",1) == 0 ) ||
			(strncmp(pszBuff+2,"N",1) == 0 ) ||
			(strncmp(pszBuff+2,"S",1) == 0 ) ||
			(strncmp(pszBuff+2,"V",1) == 0 ) ||
			(strncmp(pszBuff+2,"Z",1) == 0 ))
		{
			memcpy(pszBuff,"白",2);
		}
		else
		{
			memcpy(pszBuff,"黄",2);
		}
	}
	return 0;
}

#if !(defined(RELEASE_TO_MARKET) || defined(RELEASE_TO_IN))

const char *g_rgszPlateCharAlpha[] = {
	/*0*/ "#", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	/*37*/ "警",
	/*38*/ "京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑", "苏", "浙", "皖", "闽", "赣", "鲁", "豫", "鄂", "湘", "粤", "桂", "琼", "川", "贵", "云", "藏", "陕", "甘", "青", "宁", "新",
	/*69*/ "甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸", "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥",
	/*91*/ "使",
	/*92*/ "消", "边", "通", "森", "金", "警", "电",
	/*99*/ "试", "挂", "学", "临",
	/*103*/"沈", "北", "兰", "济", "南", "成", "广", "军", "炮", "空", "海",
	/*114*/"港", "澳"
};

/*字符集合长度*/
int g_cnCharsAlpha = sizeof(g_rgszPlateCharAlpha) / sizeof(g_rgszPlateCharAlpha[0]);

char NO_PLATE[] = "  无车牌";

/*可移植的函数*/
int GetPlateNameAlpha(
	char *pszPlateName,
	const PLATE_TYPE nPlateType,
	const PLATE_COLOR nPlateColor,
	const unsigned char* pbContent
	)
{
	int i, nTemp, iCharNum = 7;
	char *pszTemp = pszPlateName;
	int fWhitePlate = 0;

	if (NULL == pbContent || NULL == pszPlateName)
	{
		return -1;
	}
	// 无牌车输出
	if (0 == *pbContent)
	{
		strcpy( ( char* )pszPlateName, ( char* )NO_PLATE );
		return 0;
	}
	switch ( nPlateType )
	{
	case PLATE_NORMAL:		// 所有蓝牌、黑牌
		if (nPlateColor == PC_BLACK)
			strcpy( pszTemp, "黑" );
		else
			strcpy( pszTemp, "蓝" );
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_WJ:				// 武警车牌
	case PLATE_DOUBLE_WJ:       // 双层武警车牌
	case PLATE_POLICE2:			// 辽-A1234-警
	case PLATE_MILITARY:		// 军货
		fWhitePlate = 1;
		strcpy( pszTemp, "白" );
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_POLICE:			// 包括所有单行黄牌，及与黄牌格式相同的警车
		if ( ( pbContent[6] == 37 ) ||
			( ( pbContent[0] >= 103 ) && ( pbContent[0] < 114 ) )
			|| ( nPlateColor == PC_WHITE ) )
		{
			fWhitePlate = 1;
			strcpy( pszTemp, "白" );
		}
		else if (nPlateColor == PC_LIGHTBLUE)
		{
			strcpy( pszTemp, "蓝" );
		}
		else
		{
			strcpy( pszTemp, "黄" );
		}
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_DOUBLE_YELLOW:	// 双行黄牌
		strcpy( pszTemp, "双" );
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_DOUBLE_GREEN:
		strcpy( pszTemp, "绿" );
		pszTemp += strlen(pszTemp);
		iCharNum = 8;
		break;
	case PLATE_DOUBLE_MOTO:	// 双行摩托牌
		strcpy( pszTemp, "摩" );
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_INDIVIDUAL:
		strcpy( pszTemp, "白" );
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_SHI:
		strcpy( pszTemp, "黑" );
		pszTemp += strlen(pszTemp);
		break;
	case PLATE_UNKNOWN:
		break;
    default:
        break;
	}

	if((nPlateType==PLATE_WJ) || (nPlateType==PLATE_DOUBLE_WJ))
	{
		*(pszTemp++)='W';
		*(pszTemp++)='J';
	}

	/*if (pbContent[0] >= 11 && pbContent[0] <= 36)
	{
		strcpy( pszTemp, "京" );
		pszTemp += strlen(pszTemp);
	}*/

	for (i=0; i<iCharNum; i++)
	{
		if((nPlateType==PLATE_DOUBLE_WJ || nPlateType==PLATE_WJ) && i == 0)
		{
			continue;
		}
		nTemp = pbContent[i];
		if (nTemp >= g_cnCharsAlpha)
		{
			nTemp = 0;
		}
		if (i == 6)
		{
			if ( (37 == nTemp && !fWhitePlate )		//非白牌最后一位不为“警”字
				|| (pbContent[0] >= 103
				&& pbContent[0] <= 113 && nTemp > 36)	//军牌最后一位不为汉字
				)
			{
				nTemp = 1;
			}
			if (nPlateType==PLATE_POLICE2)			// 辽-A1234-警
			{
				nTemp=37;
			}
		}
		strcpy(pszTemp, g_rgszPlateCharAlpha[nTemp]);
		pszTemp += strlen(pszTemp);
	}
	*pszTemp = '\0';

	ProcessPlateNameAlpha(pszPlateName);

	return 1;
}

#endif
