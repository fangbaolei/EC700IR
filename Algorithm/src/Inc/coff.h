/*
 * coff.h
 *
 *  Created on: 2009-7-27
 *      Author: Administrator
 */

#ifndef COFF_H_
#define COFF_H_

#include "DSPLinkType.h"

typedef struct _tagFileHeader{
	Uint16		VerId;				/*Version ID,indicates version of COFF file structure*/
	Uint16		nSector;			/*number of section headers*/
	Uint32		stamp;				/*time and date stamp;indicate when the file was created*/
	Uint32		symPoint;			/*File pointer,contains the symbol table's stating address*/
	Uint32		nEntry;				/*Number of entries in the symbol table*/
	Uint16		szOpt;				/*number of bytes in the optional header*/
	Uint16		flags;
	Uint16		magicId;			/*Target ID;magic number indicates the file can be executed in a specific TI system*/
}FHeader,*FHdHandle;

/*
 * File Header Flags(Bytes 18 to 19)
 */
#define F_RELFLG	(0x0001)	/*Relocation information was stripped from the file*/
#define F_EXEC		(0x0002)	/*The file is relocatable (it contains no unresolved external references)*/
#define F_LNNO		(0x0004)	/*For TMS430 and TMS470 only*/
#define F_LSYMS		(0x0008)	/*Local symbols were stripped from the file*/
#define F_LITTLE	(0x0100)	/*The target is a little-endian device*/
#define F_BIG		(0x0200)	/*For C6000, MSP430, and TMS470 only*/
#define F_SYMMERGE	(0x1000)	/*For C2800, MSP430, and TMS470: Duplicate symbols were removed. For C6000: Reserved*/

/*
 * Magic Number
 */
typedef enum{
	TMS470 = 0x0097,
	TMS320C5400,
	TMS320C6000,
	TMS320C5500,
	TMS320C2800,
	MSP430=0x00A0,
	TMS320C5500P
}MAGICID;

/*
 * Optional File Header Format
 */
typedef struct _tagOptFHeader{
	Uint16	magicId;		/*Optional file header magic number (0108h)*/
	Uint16	version;		/*Version stamp*/
	Uint32	szExecCode;		/*Size (in bytes) of executable code*/
	Uint32	szInitData;		/*Size (in bytes) of executable code*/
	Uint32	szUnInitData;	/*Size (in bytes) of uninitialized data*/
	Uint32	entryPtr;		/*Entry point*/
	Uint32	exec_start;		/*Beginning address of executable code*/
	Uint32	data_start;		/*Beginning address of initialized data*/
}OptFHeader,*OptFHdHandle;

#define OPT_MAGIC_ID	(0x0108)

/*
 *Section Header structure
 */
typedef struct _tagSectHeader{
	Uint8	name[8];	/*This field contains one of the following: 1) An 8-character section name padded
						with nulls. 2) A pointer into the string table if the symbol name is longer than
						eight characters.*/
	Uint32	phyAddr;	/*Section's physical address*/
	Uint32	virAddr;	/*Section's virtual address*/
	Uint32	size;		/*Section size in bytes (C6000, C55x, TMS470 and TMS430) or words (C2800, C5400)*/
	Uint32	rawPtr;		/*File pointer to raw data*/
	Uint32	relPtr;		/*File pointer to relocation entries*/
	Uint32	resv1;
	Uint32	nRelEntry;	/*Number of relocation entries*/
	Uint32	nLineEntry;	/*For TMS470 and TMS430 only: Number of line number entries. For other devices: Reserved*/
	Uint32	flags;
	Uint16	resv2;
	Uint16	nPage;		/*Memory page number*/
}SecHeader,*SecHdHandle;

#define STYP_REG 	(0x00000000)  /*Regular section (allocated, relocated, loaded)*/
#define STYP_DSECT 	(0x00000001)  /*Dummy section (relocated, not allocated, not loaded)*/
#define STYP_NOLOAD (0x00000002)  /*Noload section (allocated, relocated, not loaded)*/
#define STYP_GROUP 	(0x00000004)  /*Grouped section (formed from several input sections). Other devices:Reserved*/
#define STYP_PAD 	(0x00000008)  /*Padding section (loaded, not allocated, not relocated). Other devices:Reserved*/
#define STYP_COPY 	(0x00000010)  /*Copy section (relocated, loaded, but not allocated; relocation entries are processed normally)*/
#define STYP_TEXT 	(0x00000020)  /*Section contains executable code*/
#define STYP_DATA 	(0x00000040)  /*Section contains initialized data*/
#define STYP_BSS 	(0x00000080)  /*Section contains uninitialized data*/
#define STYP_BLOCK 	(0x00001000)  /*Alignment used as a blocking factor.*/
#define STYP_PASS 	(0x00002000)  /*Section should pass through unchanged.*/
#define STYP_CLINK 	(0x00004000)  /*Section requires conditional linking*/
#define STYP_VECTOR (0x00008000)  /*Section contains vector table.*/
#define STYP_PADDED (0x00010000)  /*section has been padded.*/

/*
 * Structuring Relocation Information
 * Only for C2800, C6000, MSP430, and TMS470
 */
typedef struct _tagRelInfo{
	Uint32	virAddr;
	Uint16	symIndx;
	Uint16	rsv;
	Uint16	type;
}RelInfo,*RelInfoHandle;

typedef void (*WriteText)(int fd, Uint32 dst, Uint32 src, Uint32 sz);
Uint32 C6xDspLoad(int fd, char* file, WriteText wr);

#endif /* COFF_H_ */

