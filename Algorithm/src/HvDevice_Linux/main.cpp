
#define __DEBUG
#ifdef __DEBUG

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "HvDeviceEx.h"
#include "HvAutoLinkDevice.h"
#include "encode.h"
int ActinveConnect();

int SetCallBackType( int iType);
void SycTime();
void ShowAllDevice();
void MyGetIPDWORD( char *chTempIP, DWORD32 &dwIP);
void MyGetIPString(DWORD32 dwIP, char *chIp);
void MyGetMacString(DWORD64 dwMac, char *chMac);
void TestOpenAndCLose();

void  ShowActinveDevice( );
int ActinveConnect();

const int MAX_CAMERA_COUNT = 100;
static DWORD64 g_DeviceMac[100] = { 0 };

 void *g_pHandle = NULL;

 bool g_threadexit = false;

static int CreateDir(const char *sPathName)
{
    char DirName[256];
    strcpy(DirName, sPathName);
    int i, len = strlen(DirName);
    if (DirName[len-1] != '/') strcat(DirName, "/");
    len = strlen(DirName);
    for (i=1; i<len; i++)
    {
        if (DirName[i] == '/')
        {
            DirName[i] = 0;
            if (access(DirName, NULL) != 0)
            {
                if (mkdir(DirName, 0755) == -1) return -1;
            }
            DirName[i] = '/';
        }
    }
    return 0;
}

void MyGetIPDWORD( char *chTempIP, DWORD32 &dwIP)
    {
        DWORD32 temp[4] = {0};
        sscanf(chTempIP, "%d.%d.%d.%d", temp+3, temp+2, temp+1, temp);
        dwIP = (temp[3]<<24) | (temp[2]<<16) | (temp[1]<<8) | temp[0];
        return ;
    }

    void MyGetIPString(DWORD32 dwIP, char *chIp)
    {
        unsigned char * pByte = (unsigned char*)&dwIP;
        sprintf(chIp, "%d.%d.%d.%d", pByte[3], pByte[2], pByte[1], pByte[0]);
        return ;
    }

    void MyGetMacString(DWORD64 dwMac, char *chMac)
    {
        if (chMac == NULL)
            return ;
        unsigned  char *pByte = (unsigned char *)&dwMac;
        sprintf(chMac, "%02X-%02X-%02X-%02X-%02X-%02X", pByte[0], pByte[1],pByte[2], pByte[3], pByte[4],pByte[5]);
        return ;
    }


INT  HVAPICALLBACKRECORDINFOBEGIN(PVOID pUserData, DWORD dwCarID)
{
    printf("Rev a result Start \n");
    return 0;
}

INT HVAPICALLBACKRECORDINFOEND(PVOID pUserData, DWORD dwCarID)
{
    printf("Rev a reuslt End\n");
    return 0;
}

INT HVAPICALLBACKRECORDPLATE(PVOID pUserData,
                                                                                            DWORD dwCarID,
                                                                                            LPCSTR pcPlateNo,
                                                                                            LPCSTR pcAppendInfo,
                                                                                            DWORD dwRecordType,
                                                                                            DWORD64 dw64TimeMs)
{

    int iLen = 1024*10;
    //char *szTemp = new char[iLen];
   // memset(szTemp, 0, iLen);

  //  HVAPIUTILS_ParsePlateXmlStringEx(pcAppendInfo, szTemp, iLen);

  //  printf("%s\n", szTemp);

    time_t mytime_t = dw64TimeMs / 1000;
    struct tm *mytm = localtime(&mytime_t);
    char chTimeFmt[100] = {0};
    strftime(chTimeFmt, 100, "%Y-%m-%d %H:%M:%S", mytm);
    //printf( "CallBack Plate:  %s %s\n", pcPlateNo, chTimeFmt );

    int iBuffLen = 10420;
    char szBuff[10420] = {0};
    //printf("%s\n*****\n", pcAppendInfo);

   //HVAPIUTILS_ParsePlateXmlStringEx(pcAppendInfo, szBuff, 1042 );
   //UTF8ToGB2312();
   GB2312ToUTF8(pcAppendInfo, szBuff, iBuffLen);
   printf("%s\n*****\n", szBuff);


    //printf("%s\n", szBuff);

        char filePath[1024] = { 0 };
        strcpy(filePath, "./result/");
        CreateDir(filePath);


        char filename[256] = {0};
        sprintf(filename, "%s%d_platexml.xml", filePath,  dwCarID);

        int tfdjpg = 0;
        FILE *myfile = fopen(filename, "w");
        if (myfile)
        {
              fprintf(myfile, "%s",szBuff);
           fclose(myfile);
        }

    return 0;
}

INT HVAPICALLBACKRECORDBIGIMAGE(PVOID pUserData,
                                                                                                    DWORD dwCarID,
                                                                                                    WORD wImageTYpe,
                                                                                                    WORD wWidth,
                                                                                                    WORD wHeight,
                                                                                                    PBYTE pbPIcData,
                                                                                                    DWORD dwImgDataLen,
                                                                                                    DWORD dwRecordType,
                                                                                                    DWORD64 dw64TimeMs)
{

    time_t mytime_t = dw64TimeMs / 1000;
    struct tm *mytm = localtime(&mytime_t);
    char chTimeFmt[100] = {0};
    strftime(chTimeFmt, 100, "%Y-%m-%d %H:%M:%S", mytm);
    printf("BIgImage: %s\n", chTimeFmt);
    printf("BIgImage -  CarID:%d  ImageType:%d\n", dwCarID, wImageTYpe);
    printf( "BIgImageSize: %d\n", dwImgDataLen );


    char filePath[1024] = { 0 };
    strcpy(filePath, "./result/");

    CreateDir(filePath);

    if ( pbPIcData!= NULL  && dwImgDataLen != 0 )
    {
        char filename[256] = {0};
        sprintf(filename, "%s%s_%d_%d.jpg", filePath, chTimeFmt, dwCarID, wImageTYpe);

        int tfdjpg = 0;
        tfdjpg = open(filename, O_WRONLY | O_CREAT, S_IRUSR |S_IWUSR);
        if (tfdjpg)
        {
            write(tfdjpg, pbPIcData, dwImgDataLen);
            close(tfdjpg);
        }
    }

    int a[4]={0};
    int iLen = sizeof(a);

    HVAPI_GetExtensionInfoEx(g_pHandle,  wImageTYpe, a, &iLen);

    char szLog[512] = {0};
    sprintf(szLog, "left :  y-%d x-%d right: y-%d, x-%d\n", a[0], a[1], a[2],a[3]);
    printf(szLog);

    return 0;
}


INT HVAPICALLBACKRECORDSMALLIMAGE(PVOID pUserData,
                                                                                                    DWORD dwCarID,
                                                                                                    WORD wWidth,
                                                                                                    WORD wHeight,
                                                                                                    PBYTE pbPIcData,
                                                                                                    DWORD dwImgDataLen,
                                                                                                    DWORD dwRecordType,
                                                                                                    DWORD64 dw64TimeMs)
{
    printf("SmallImage-CarID:%d\n", dwCarID );

    int iLen = 1024 * 1024;
    PBYTE pBuff = new BYTE[iLen];
    if (  HVAPIUTILS_SmallImageToBitmapEx(pbPIcData, wWidth, wHeight, pBuff, &iLen) == S_OK )
    {
         time_t mytime_t = dw64TimeMs / 1000;
        struct tm *mytm = localtime(&mytime_t);
        char chTimeFmt[100] = {0};
        strftime(chTimeFmt, 100, "%Y-%m-%d %H:%M:%S", mytm);
        printf("SmallImage: %s\n", chTimeFmt);
        printf("SmallImage -  CarID:%d\n", dwCarID);
        printf( "SmallSize: %d\n", iLen );


        char filePath[1024] = { 0 };
        strcpy(filePath, "./result/");
        CreateDir(filePath);


        char filename[256] = {0};
        sprintf(filename, "%s%s_%d.bmp", filePath, chTimeFmt, dwCarID);

        int tfdjpg = 0;
        tfdjpg = open(filename, O_WRONLY | O_CREAT, S_IRUSR |S_IWUSR);
        if (tfdjpg)
        {
                write(tfdjpg, pBuff, iLen);
                close(tfdjpg);
        }

    }

     if ( pBuff != NULL )
     {
         delete[] pBuff;
         pBuff = NULL;
     }

    return 0;
}

INT HVAPICALLBACKRECORDBINARYIMAGE(PVOID pUserData,
                                                                                                        DWORD dwCarID,
                                                                                                        WORD wWidth,
                                                                                                        WORD wHeight,
                                                                                                        PBYTE pbPIcData,
                                                                                                        DWORD dwImgDataLen,
                                                                                                        DWORD dwRecordType,
                                                                                                        DWORD64 dw64TimeMs)
{
    printf("BinaryImage-CarID:%d\n", dwCarID);

    int iLen = 1024 * 500;
    PBYTE pBuff = new BYTE[iLen];
    if ( HVAPIUTILS_BinImageToBitmapEx( pbPIcData, pBuff,  &iLen) == S_OK )
    {
        time_t mytime_t = dw64TimeMs / 1000;
        struct tm *mytm = localtime(&mytime_t);
        char chTimeFmt[100] = {0};
        strftime(chTimeFmt, 100, "%Y-%m-%d %H:%M:%S", mytm);
        printf("BinImage: %s\n", chTimeFmt);
        printf("BInImage -  CarID:%d\n", dwCarID);
        printf( "BInSize: %d\n", iLen );


        char filePath[1024] = { 0 };
        strcpy(filePath, "./result/");
        CreateDir(filePath);


        char filename[256] = {0};
        sprintf(filename, "%s%s_%d_bin.bmp", filePath, chTimeFmt, dwCarID);

        int tfdjpg = 0;
        tfdjpg = open(filename, O_WRONLY | O_CREAT, S_IRUSR |S_IWUSR);
        if (tfdjpg)
        {
                write(tfdjpg, pBuff, iLen);
                close(tfdjpg);
        }
    }

    if (pBuff  != NULL )
    {
        delete[] pBuff;
        pBuff = NULL;
    }
    return 0;
}

INT HVAPICALLBACKSTRING(PVOID pUserData, LPCSTR pString, DWORD dwStrLen)
{
    printf("Recv String : %s\n", pString);
    return 0;
}

static int  g_iFrames = 0;
INT HVAPICALLBACKJPEG(PVOID pUserData,PBYTE pbImageData,DWORD dwImageDataLen,
DWORD dwImageType, LPCSTR szImageExtInfo)
{
    printf("Recv  a JPEG fram %d \n", dwImageDataLen );
    printf("ExtInfo:%s\n", szImageExtInfo);

   time_t  tt  = time(NULL);
    struct tm *mytm = localtime(&tt);
    char chTimeFmt[100] = {0};
    char chFileTime[100] = { 0 };
    strftime(chTimeFmt, 100, "%Y%m%d%H%M,", mytm);
    strftime(chFileTime, 100, "%Y%m%d%H%M%S,", mytm);


    char filePath[1024] = { 0 };
    sprintf(filePath, "./result/VideoJPEG/%s/", chTimeFmt);

    CreateDir(filePath);

    if ( pbImageData!= NULL  && dwImageDataLen != 0 )
    {
        char filename[256] = {0};
        sprintf(filename, "%s%s_%d_%d.jpg", filePath, chFileTime, g_iFrames++);

        int tfdjpg = 0;
        tfdjpg = open(filename, O_WRONLY | O_CREAT, S_IRUSR |S_IWUSR);
        if (tfdjpg)
        {
            write(tfdjpg, pbImageData, dwImageDataLen);
            close(tfdjpg);
        }
    }



    return 0;
}

INT HVAPICALLBACKH264( PVOID pUserData,
                                                                        PBYTE pbVideoData,
                                                                        DWORD dwVIdeoDataLen,
                                                                        DWORD dwVIdeoType,
                                                                        LPCSTR szVideoExtInfo)
{
    printf("Recv  a H264  fram %d \n", dwVIdeoDataLen );
    printf("ExtInfo:%s\n", szVideoExtInfo);

/*
   time_t  tt  = time(NULL);
    struct tm *mytm = localtime(&tt);
    char chTimeFmt[100] = {0};
    char chFileTime[100] = { 0 };
    strftime(chTimeFmt, 100, "%Y%m%d%H%M", mytm);
    strftime(chFileTime, 100, "%Y%m%d%H%M%S", mytm);


    char filePath[1024] = { 0 };
    sprintf(filePath, "./result/VideoH264/%s/", chTimeFmt);

    CreateDir(filePath);

    if ( pbVideoData != NULL  && dwVIdeoDataLen != 0 )
    {
        char filename[256] = {0};
        sprintf(filename, "%s%s_%d_%d.jpg", filePath, chFileTime, g_iFrames++);

        int tfdjpg = 0;
        tfdjpg = open(filename, O_WRONLY | O_CREAT, S_IRUSR |S_IWUSR);
        if (tfdjpg)
        {
            write(tfdjpg, pbVideoData, dwVIdeoDataLen);
            close(tfdjpg);
        }
    }*/

    return 0;
}



 INT HVAPICALLBACKHISTORYVIDEO( PVOID pUserData,
                                                                                            PBYTE pbVideoData,
                                                                                            DWORD dwVIdeoDataLen,
                                                                                            DWORD dwVIdeoType,
                                                                                            LPCSTR szVideoExtInfo)
{
    return 0;
}



int main()
{
     printf(" 0---Actinve Connect, 1---beidong Connect\n");
    int iSelect = 0;
    scanf("%d", &iSelect);
    if ( iSelect == 0 )
    {
        return  ActinveConnect();
    }


    char chTempsz[100] = {"3069663536"};
   unsigned long  dw = strtoul(chTempsz, NULL, 20);
   //unsigned long dw1 = 3069663536;

   sscanf(chTempsz, "%u", &dw);

    unsigned int dwDeviceCount = 0;
    if ( HVAPI_SearchDeviceCount(&dwDeviceCount) ==S_OK )
    {
        char szLog[24] = {0};
        printf("%d\n", dwDeviceCount);
        DWORD64  dw64MacAddr = 0;
        DWORD32 dwIP = 0;
        DWORD32 dwMask = 0;
        DWORD32 dwGateWay=0;
        for ( int i=0; i<dwDeviceCount; i++)
        {
            HVAPI_GetDeviceAddr(i, &dw64MacAddr,  &dwIP, &dwMask, &dwGateWay);
            DWORD32 dwTemp = dwIP;
            DWORD32 temp1 = dwIP & 0x000000ff;
            DWORD32 temp2 =( dwIP & 0x0000ff00) >> 8;
            DWORD32 temp3 = (dwIP & 0x00ff0000) >> 16;
            DWORD32 temp4 = (dwIP & 0xff000000) >> 24;
            printf( "Index:%d -- %d.%d.%d.%d\n", i, temp4, temp3, temp2, temp1);

            char chDeviceInfo[1024] = {0};
            HVAPI_GetDeviceInfoEx(i, chDeviceInfo, 1024);
            printf("%s\n", chDeviceInfo);
        }
    }


    //ShowAllDevice();

    //
//TestOpenAndCLose();

  char szIP[34] = { 0 };

  printf("Input connect device IP \n");
  scanf("%s", szIP);


/*
  int iDeviceType = -1;
  HVAPI_GetDevTypeEx(szIP,  &iDeviceType);
  printf("DeviceType: %d\n", iDeviceType);
    int iTryCount = 0;
    g_pHandle = HVAPI_OpenEx(szIP, NULL);

    if ( g_pHandle != NULL )
    {
           SetCallBackType( 1 );
            printf("connect success\n");
            SycTime();

            //SyncTime&

            int iLen = 1024*1024;
            char *szParamDoc = new char[iLen];
            int nRetLen = 0;
            if (  HVAPI_GetParamEx(g_pHandle,szParamDoc,iLen, &nRetLen) == S_OK )
            {
                printf("HVAPI_GetParamEx Success \n ");
                sleep(3);
               if (  HVAPI_SetParamEx(g_pHandle,szParamDoc)  == S_OK )
                printf("Set ParamEx Success\n");
            }
        }
        else
        {
            printf("connect fale\n");
        }

        while(true)
        {
                usleep(1000000);
            if ( g_pHandle == NULL )
            {
                g_pHandle = HVAPI_OpenEx(szIP, NULL);
                if ( g_pHandle != NULL )
                    SetCallBackType( 1 );
                continue ;
            }

            DWORD iconnect = -1;
            iTryCount++;
             HVAPI_GetConnStatusEx(g_pHandle, CONN_TYPE_RECORD, &iconnect );
             if ( iconnect == CONN_STATUS_NORMAL)
             {
                printf("connect  Normal: %d\n", iTryCount);
              //   HVAPI_CloseEx();
             }
             else
             {
                HVAPI_CloseEx(g_pHandle);
                g_pHandle = NULL;
                 printf("connect disconnect\n");
                 printf("reconnect \n");
        //g_pHandle = HVAPI_OpenEx(szIP, NULL);
               // if ( g_pHandle != NULL )
                 //   printf("Open Device  Success\n");
               // SetCallBackType( 1 );
             }
        }
*/

    //Set CallBack TYPE
    int iType = -1;


    //SetCallBackType( iType );


   // while (true)
    {
        if ( g_pHandle == NULL )
        {
              g_pHandle = HVAPI_OpenEx(szIP, NULL);
              if (g_pHandle )
              {
                   int iLen = 1024*1024;
                    char *szParamDoc = new char[iLen];
                    int nRetLen = 0;
                    if (  HVAPI_GetParamEx(g_pHandle,szParamDoc,iLen, &nRetLen) == S_OK )
                    {
                        printf("HVAPI_GetParamEx Success \n ");
                        sleep(3);
                    }
                    if ( szParamDoc != NULL)
                    {
                        delete[] szParamDoc;
                        szParamDoc = NULL;
                    }

                    printf("Set CallBack Type");
                    printf("Record  Type ..............1\n");
                    printf("H264 Type ....................2\n");
                    printf("Image Type ................3\n");
                    scanf("%d", &iType);

                   if ( SetCallBackType( iType ) == -1 )
                   {
                       HVAPI_CloseEx(g_pHandle);
                        g_pHandle = NULL;
                   }
              }

        }

    }


    int t = 10;
    while( true )
    {
        DWORD  dwCamareConnStatus = -1;
		HVAPI_GetConnStatusEx(g_pHandle, CONN_TYPE_RECORD, &dwCamareConnStatus);
		if ( dwCamareConnStatus != CONN_STATUS_NORMAL )
            printf("dwCamareConnStatus != CONN_STATUS_NORMAL \n");
       // else
         //   printf("dwCamareConnStatus == CONN_STATUS_NORMAL \n");

		sleep(1);
    }

    int iStop = 0;
    scanf("%d", &iStop);

    if (g_pHandle != NULL )
    {
        HVAPI_CloseEx(g_pHandle);
       g_pHandle = NULL;
    }
    return 0;
}

//Test
void TestOpenAndCLose()
{
    printf("Input Device Addr\n");
    char szIP[32] = {0};
    scanf("%s",szIP);
    void *handle = HVAPI_OpenEx(szIP, NULL);
    printf("*******************8OutPut Info:*************************8\n");
    if ( handle == NULL )
    {
        printf("Open Device Failt\n");
         //   return  0 ;
    }
    else
    {
        printf("Open Device Success\n" );
        printf("handle: 0x%x\n", &handle);
    }

    if ( HVAPI_CloseEx(handle) == S_OK )
    {
        printf("Close handle Success\n");
        handle = NULL;
    }
    else
    {
         printf("Close handle Success\n");
    }
    printf("*******************8OutPut Info:*************************8\n");

}


int SetCallBackType( int iType)
{
    if ( g_pHandle == NULL )
            return -1;
    switch(iType)
    {
        case 1:
        {
            printf("start set Callback\n");

           if ( HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKRECORDINFOBEGIN, NULL,  0, CALLBACK_TYPE_RECORD_INFOBEGIN , NULL ) != S_OK
             || HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKRECORDINFOEND,   NULL, 0,  CALLBACK_TYPE_RECORD_INFOEND , NULL ) != S_OK
           ||  HVAPI_SetCallBackEx(g_pHandle,   (void*) HVAPICALLBACKRECORDSMALLIMAGE,   NULL, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL ) != S_OK
            || HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKRECORDBINARYIMAGE,   NULL, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL ) != S_OK
            || HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKRECORDBIGIMAGE,   NULL, 0,  CALLBACK_TYPE_RECORD_BIGIMAGE , NULL ) != S_OK
            || HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKRECORDPLATE,   NULL, 0, CALLBACK_TYPE_RECORD_PLATE , NULL ) != S_OK )
          //  if (  HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKRECORDPLATE,   NULL, 0, CALLBACK_TYPE_RECORD_PLATE , NULL ) != S_OK )
            {
                printf("close Handle\n");
                HVAPI_CloseEx(g_pHandle);


                g_pHandle = NULL;
                printf("finish set Callback\n");
                return -1;
            }
            else{
                printf("SetCallBack Success\n");
            }

               printf("finish set Callback\n");

        //    HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKSTRING,   NULL, 0, CALLBACK_TYPE_STRING , NULL );
        }
        break ;

        case 2:
          {
           if ( HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKH264,   NULL, 0, CALLBACK_TYPE_H264_VIDEO , NULL ) != S_OK )
           return -1;
        }
        break;
        case 3:
        {
            if ( HVAPI_SetCallBackEx(g_pHandle,   (void*)HVAPICALLBACKJPEG,   NULL, 0, CALLBACK_TYPE_JPEG_FRAME , NULL ) != S_OK )
            return -1;
        }
        break ;
        default:
        return  -1;
    }

    return 0;
}

void SycTime()
{
    time_t tt = time(NULL);
    struct tm *pTm = localtime(&tt);
    char chCmdTemp[256] = { 0 };
    char szRetBuf[2042] = { 0 };
    int iRetLen = 0;
    sprintf(chCmdTemp, "SetTime,Date[%04d.%02d.%02d],Time[%02d:%02d:%02d]",
    pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour,pTm->tm_min,
    pTm->tm_sec);

   if ( HVAPI_ExecCmdEx(g_pHandle , chCmdTemp, szRetBuf, 2042,  &iRetLen) == S_OK )
   {
       printf("SyncTIme SUCCESS \n%s\n", szRetBuf);
   }
    return ;
}

void ShowAllDevice()
{
    //
    DWORD32  iDeviceCount = 0;
    HVAPI_SearchDeviceCount(&iDeviceCount);
    printf("Search DeviceCount : %d\n", iDeviceCount);

    DWORD64 dwMacAddr = 0;
    DWORD32 dwIP=0, dwMask = 0, dwGateWay = 0;
    char szMac[124] = {0};
    char szAddr[64] = {0};
    char szMask[64] = {0};
    char szGateWay[64] = { 0 };
    char szDeveTypeInfo[64] = { 0 };

    for (int i=0; i<(int)iDeviceCount; i++)
    {
        memset(szMac, 0, 124);
        memset(szAddr, 0, 64);
        memset(szMask, 0, 64);
        memset(szGateWay, 0, 64);
        memset(szDeveTypeInfo, 0, 64);
        if ( HVAPI_GetDeviceAddr((DWORD32)i, &dwMacAddr, &dwIP,&dwMask, &dwGateWay)  == S_OK )
        {
            g_DeviceMac[i] = dwMacAddr;
            MyGetMacString(dwMacAddr, szMac);
            MyGetIPString(dwIP, szAddr);
            MyGetIPString(dwMask, szMask);
            MyGetIPString(dwGateWay, szGateWay);
            printf("%d---mac:%s addr:%s  mask:%s, gateway:%s\n", i, szMac, szAddr, szMask, szGateWay);
        }

        if ( HVAPI_GetDeviceInfoEx((DWORD32)i, szDeveTypeInfo, 64 ) == S_OK )
        {
            printf("DeviceTypeInfo:%s\n\n",szDeveTypeInfo);

        }
    }

    if ( iDeviceCount == 0 )
        return ;
    while(true)
    {
         printf("1---Modify NetAddr Info\n");
        printf("2--- return\n ");
        int iSelect = -1;
        //
        scanf("%d" ,&iSelect);

        switch(iSelect)
        {
            case 1:
            {
                int index = -1;
                printf("InPut index:");
                scanf("%d", &index);
                if ( index < 0 || index > iDeviceCount -1)
                {
                    printf("Input error\n");
                    break;
                }

                memset(szAddr, 0, 64);
                memset(szMask, 0, 64);
                memset(szGateWay, 0, 64);

                printf("Input Device IP:");
                scanf("%s", szAddr);
                printf("Input Device Mask:");
                scanf("%s", szMask);
                printf("Input Device GateWay:");
                scanf("%s", szGateWay);

                DWORD32 dwIP, dwMask, dwGateWay;
                MyGetIPDWORD(szAddr, dwIP);
                MyGetIPDWORD(szMask, dwMask);
                MyGetIPDWORD(szGateWay, dwGateWay);

                if ( HVAPI_SetIPByMacAddr(g_DeviceMac[index], dwIP, dwMask,dwGateWay) == S_OK )
                {
                    printf("Setting Success\n");
                     HVAPI_SearchDeviceCount(&iDeviceCount);
                }
                else
                {
                    printf("Setting Failt\n");
                }
            }
            break;
            case 2:
                return ;
        }
    }

    return ;
}


int ActinveConnect()
{

    bool bServer = false;
    pthread_t thActinve = 0;
    while( true)
    {
        int iSelect = 0;
        printf("0-- quit\n");
        printf("1--Start Server\n");
        printf("2--CloseServer\n");
        printf("3--ShowAllActiveDev\n");
        printf("4--Connect Dev\n");

        scanf("%d", &iSelect);
        if ( iSelect == 0 )
        {
            g_threadexit = true;
            break ;
        }

        switch(iSelect)
        {
            case 2 :
            {
                if ( !bServer )
                    break ;

                g_threadexit = true;

                if ( HVAPI_CloseServer(LISTEN_TYPE_RECORD) == S_OK )
                printf(" CLose Server Success\n");

                if ( HVAPI_UnLoadMonitor() == S_OK )
                printf( "UnLoadMonitor Success\n");

                 bServer = false;

                // if ( thActinve != 0 )
               //  {
                   //  pthread_join(thActinve, NULL);
             //        thActinve = 0;
                // }
            }
            break;
            case  1:
            {

                int MaxListenNum = 50;
                if ( S_OK == HVAPI_LoadMonitor(MaxListenNum, NULL) )
                printf("LoadMonitor Success\n");

                int nPort = 6665;
                if ( S_OK == HVAPI_OpenServer(nPort, LISTEN_TYPE_RECORD, NULL))
                printf("Open Server Success\n");

                 g_threadexit = false;
               // if ( pthread_create(&thActinve, NULL, ShowActinveDeviceStatus, NULL ) )
               // {
                //    printf("create status thread fail\n");
             //   }

                bServer = true;

            }
            break;
            case 3:
            {
                printf("Enter ShowActinveDevice\n ");
                ShowActinveDevice( );
                printf("Quit  ShowActinveDevice\n");
            }
            break;
            case 4:
            {
                char szDevNo[256] = { 0 };
                printf("Input Need Connect DevNo:");
                scanf("%s", szDevNo);
                g_pHandle  = HVAPI_OpenAutoLink(szDevNo,NULL);
                if ( g_pHandle == NULL )
                {
                    printf("OpenAutoLink Fail\n");
                    break;
                }

                SetCallBackType(1);

                //
            }
            break;
        }
    }

    if ( bServer )
    {
         if ( HVAPI_CloseServer(LISTEN_TYPE_RECORD) == S_OK )
                printf(" CLose Server Success\n");

                if ( HVAPI_UnLoadMonitor() == S_OK )
                printf( "UnLoadMonitor Success\n");
    }

}


void  ShowActinveDevice( )
{
    printf("1\n");
    int nDevListLen = 0;
    int nDevListBufLen = 128 * 100;
    char *pDevListBuf = new char[nDevListBufLen];

    int nRecordConStatus = CONN_STATUS_UNKNOWN;
    int nReConnectCount = 0;

    char *pszSN=NULL;
    char *pszTemp = NULL;

    if ( S_OK != HVAPI_GetDeviceListSize(nDevListLen, NULL ))
    {
       printf(" search dev Fail\n");
       return ;
    }
    else{
        printf("GetDeviceListSize Success:%d\n", nDevListLen);
    }

    if ( nDevListLen == 0 )
        printf("no device Find\n");

     if ( nDevListBufLen < nDevListLen )
    {
            if ( NULL != pDevListBuf)
            {
                delete[] pDevListBuf;
                pDevListBuf = NULL;
            }//if
            nDevListBufLen = nDevListLen;
            pDevListBuf = new char[nDevListBufLen];
    }//if

     if ( nDevListLen != 0 && S_OK == HVAPI_GetDeviceList(pDevListBuf,  nDevListBufLen, NULL ) )
    {
        printf("Device List %s", pDevListBuf);

        pszSN = pDevListBuf;
        while(  pszTemp = strstr(pszSN, ";") )
        {
            *pszTemp='\0';
            HVAPI_GetDeviceStatus(pszSN, nRecordConStatus, nReConnectCount, NULL);

            printf("%s: %x--%d", pszSN,  nRecordConStatus , nReConnectCount );
            pszSN = pszTemp+1;
        }
        //
    }
    else
    {
        printf("Get Device List\n ");
    }


    if ( pDevListBuf != NULL  )
    {
        delete[] pDevListBuf;
        pDevListBuf = NULL;
    }

    printf("2\n");
}
#endif

