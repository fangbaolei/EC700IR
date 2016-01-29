/*
 * coff.c
 *
 *  Created on: 2009-7-27
 *      Author: Administrator
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "DSPLink.h"
#include "coff.h"

/* from SPRAAO8-April 2009 Common Object File Format P6 */
#define SECT_NOLOAD 0x0002u
#define SECT_BSS    0x0080u
#define SECT_COPY   0x0010u /* note: 手册中为loaded，但官方DSPLink添加该选项 */
#define SECT_DSECT  0x0001u

static unsigned short covertbytetoword16( unsigned char *pData )
{
    unsigned short uTemp;
    unsigned char *pTemp;

    pTemp = pData;
    uTemp = ( *pTemp++ ) & 0xFF;
    uTemp = ( ( *pTemp & 0xFF ) << 8 ) | uTemp;
    return uTemp;
}

static unsigned int covertbytetoword32( unsigned char *pData )
{
    unsigned int uiTemp;
    unsigned char *pTemp;

    pTemp = pData;
    uiTemp = ( *pTemp++ ) & 0xFF;
    uiTemp = ( ( *pTemp++ & 0xFF ) << 8 ) | uiTemp;
    uiTemp = ( ( *pTemp++ & 0xFF ) << 16 ) | uiTemp;
    uiTemp = ( ( *pTemp++ & 0xFF ) << 24 ) | uiTemp;
    return uiTemp;
}

static Uint32 LoadCoff(int fd, unsigned char* addr, WriteText wr)
{
    SecHeader secHd[1];
    FHeader header;
    OptFHeader opt;
    int i, j;
    unsigned char* value;
    unsigned char* pTemp;
    unsigned int myentryPoint = 0;

    //read File Header
    pTemp = addr;
    header.VerId = covertbytetoword16( pTemp );
    pTemp += 2;
    header.nSector = covertbytetoword16( pTemp );

    pTemp += 2;
    header.stamp = covertbytetoword32( pTemp );
    pTemp += 4;
    header.symPoint = covertbytetoword32( pTemp );
    pTemp += 4;
    header.nEntry = covertbytetoword32( pTemp );
    pTemp += 4;
    header.szOpt = covertbytetoword16( pTemp );
    pTemp += 2;
    header.flags = covertbytetoword16( pTemp );
    pTemp += 2;
    header.magicId = covertbytetoword16( pTemp );
    pTemp += 2;

    //read optional file header and get EntryPoint
    if (header.szOpt != 0)
    {
        opt.magicId = covertbytetoword16( pTemp );
        pTemp += 2;
        opt.version = covertbytetoword16( pTemp );
        pTemp += 2;
        opt.szExecCode = covertbytetoword32( pTemp );
        pTemp += 4;
        opt.szInitData = covertbytetoword32( pTemp );
        pTemp += 4;
        opt.szUnInitData = covertbytetoword32( pTemp );
        pTemp += 4;
        opt.entryPtr = covertbytetoword32( pTemp );
        pTemp += 4;
        opt.exec_start = covertbytetoword32( pTemp );
        pTemp += 4;
        opt.data_start = covertbytetoword32( pTemp );
        pTemp += 4;

        myentryPoint = opt.entryPtr;
    }

    //read all sector header
    for (i = 0; i < header.nSector; i ++)
    {
        for ( j = 0; j < 8; j ++ )
        {
            secHd[0].name[j] = *( pTemp ++ );
        }

        secHd[0].phyAddr = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].virAddr = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].size = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].rawPtr = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].relPtr = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].resv1 = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].nRelEntry = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].nLineEntry = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].flags = covertbytetoword32( pTemp );
        pTemp += 4;
        secHd[0].resv2 = covertbytetoword16( pTemp );
        pTemp += 2;
        secHd[0].nPage = covertbytetoword16( pTemp );
        pTemp += 2;

        if ( ( secHd[0].rawPtr == 0 )
                || ( secHd[0].size ) == 0
                || ( secHd[0].virAddr ) == 0 )
        {
            continue;
        }

        if ( ( secHd[0].flags == SECT_NOLOAD )
                || ( secHd[0].flags == SECT_BSS )
                || ( secHd[0].flags == SECT_COPY )
                || ( secHd[0].flags == SECT_DSECT ) )
        {
            continue;
        }

        value = addr + secHd[0].rawPtr;

        wr(fd, secHd[0].virAddr, (Uint32)value, secHd[0].size);
    }

    return myentryPoint;
}

Uint32 C6xDspLoad(int fd, char* file, WriteText wr)
{
    Uint32 uRet = 0;
    FILE* fp = NULL;
    int iStartPos = 0;
    int iEndPos = 0;
    int iFileLen = 0;

    fp = fopen(file,"rb");
    if ( NULL == fp )
    {
        printf("C6xDspLoad can't open: [%s]\n", file);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    iStartPos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    iEndPos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    iFileLen = iEndPos - iStartPos;

    unsigned char* pbBuf = (unsigned char*)malloc(iFileLen);
    if ( NULL == pbBuf )
    {
        printf("C6xDspLoad can't malloc enough mem: [%d]\n", iFileLen);
        return -1;
    }

    if ( iFileLen == fread(pbBuf, 1, iFileLen, fp) )
    {
        uRet = LoadCoff(fd, pbBuf, wr);
    }
    else
    {
        uRet = -1;
    }

    free(pbBuf);
    fclose(fp);
    return uRet;
}
