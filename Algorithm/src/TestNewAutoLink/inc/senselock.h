#pragma once

// Lock Instructions
#define LI_WAITFORDATA	0x00
#define LI_OUTPUTRESULT	0x01
#define LI_CLEARMEMORY	0x02

#pragma pack(push,1)
struct DataContext
{
	float fltConf;
	BYTE bId;
};
struct DataSequence
{
	static const m_kcContextSize=49;
	BYTE bInstruction;
	DataContext rgContext[m_kcContextSize];
};
#pragma pack(pop)
