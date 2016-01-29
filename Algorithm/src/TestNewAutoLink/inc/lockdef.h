#ifndef _LOCKDEF_H_
#define _LOCKDEF_H_

// Lock Instructions

// Basic Info Instructions
#define LI_GETINFO			0x10
#define GETINFOFILENAME		"0105"
#define INFODATAFILE		0x010A
#define ACCESSCOUNTFILE		0x0200
#define RSAKEYDATAFILE		0x010E
#define MAJORVER			3
#define MINORVER			0

// Char Set Instructions
#define LI_TRANSLATE		0x20
#define GETPLATFILENAME		"0108"

// Parameter Instructions
#define LI_GETPARAMCOUNT	0x30
#define LI_GETPARAMNAME		0x31
#define LI_GETPARAMDATA		0x32
#define GETPARAMFILENAME	"010C"
#define PARAMDATAFILE		0x010D
#define PARAMHEADERLEN		sizeof(WORD)*3

#define g_szUserPin			"高清测试"

//随机数长度
#define RAND_DATA_SIZE		128
#endif

