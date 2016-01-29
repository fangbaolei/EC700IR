#include "swpa.h"
#include "SWFC.h"
#include "SWEEPROMLogDevice.h"

CSWEEPROMLogDevice::CSWEEPROMLogDevice()
{
}

CSWEEPROMLogDevice::~CSWEEPROMLogDevice()
{
}


VOID CSWEEPROMLogDevice::ShowLog(LPCSTR szLog)
{
#define SINGLE_LOG_LEN 256

    char szText[SINGLE_LOG_LEN] = {0};
    
    CSWFile cFile;
    
    printf("start open EEPROM/0/BLACK_BOX\n");
    if(S_OK == cFile.Open("EEPROM/0/BLACK_BOX", "a+"))
    {
        DWORD dwTotalSize = 0;
        cFile.IOCtrl(SWPA_FILE_IOCTL_GET_MAX_SIZE, &dwTotalSize);
        
        DWORD iUsedSize = 0;
        cFile.GetSize(&iUsedSize);
        printf("open file EEPROM/0/BLACK_BOX ok,[%d/%d]\n", iUsedSize, dwTotalSize);

        BOOL fInited = FALSE;
        DWORD dwIndex = 0;

        //arrHeader[0] - offset
        //arrHeader[1] - overwritten flag
        //arrHeader[2] - crc
        DWORD arrHeader[3] = {0};

        if (0 < iUsedSize)
        {
            if (FAILED(cFile.Seek(0, SWPA_SEEK_SET)))
            {
                printf("failed to seek blackbox to SEEK_SET.\n");
                return ;
            }

            if (FAILED(cFile.Read(arrHeader, sizeof(arrHeader))))
            {
                printf("failed to read blackbox header.\n");
                return ;
            }

            DWORD dwCrc = CSWUtils::CalcCrc32(0, (PBYTE)arrHeader, sizeof(arrHeader[0]) + sizeof(arrHeader[1]));
            if (dwCrc == arrHeader[2])
            {
                fInited = TRUE;
                dwIndex = arrHeader[0];
            }
        }

        if (!fInited)
        {
            printf("init blackbox...\n");
            
            dwIndex = SINGLE_LOG_LEN;
            
            //clear header
            if (FAILED(cFile.Seek(0, SWPA_SEEK_SET)))
            {
                printf("failed to seek blackbox to SEEK_SET.\n");
                return ;
            }
            if (FAILED(cFile.Write(szText, sizeof(szText))))
            {
                printf("failed to clear blackbox.\n");
                return ;
            }
        }

        swpa_strncpy(szText, szLog, SINGLE_LOG_LEN-1);
        
        //print text
        if (FAILED(cFile.Seek(dwIndex, SWPA_SEEK_SET)))
        {    
            printf("failed to seek blackbox to SEEK_SET+%d.\n", dwIndex);
            return ;
        }

        if (FAILED(cFile.Write(szText, sizeof(szText))))
        {
            printf("failed to write blackbox.\n");
            return ;
        }

        //save header info
        arrHeader[0] = (dwIndex + SINGLE_LOG_LEN) % dwTotalSize;
        if (SINGLE_LOG_LEN >= arrHeader[0])
        {
            arrHeader[0] = SINGLE_LOG_LEN;
            arrHeader[1] ++;
        }
        arrHeader[2] = CSWUtils::CalcCrc32(0, (PBYTE)arrHeader, sizeof(arrHeader[0]) + sizeof(arrHeader[1]));
        if (FAILED(cFile.Seek(0, SWPA_SEEK_SET)))
        {    
            printf("failed to seek blackbox to SEEK_SET.\n");
            return ;
        }
        if (FAILED(cFile.Write(arrHeader, sizeof(arrHeader))))
        {
            printf("failed to write blackbox header.\n");
            return ;
        }

        cFile.Close();
        printf("write EEPROM/0/BLACK_BOX -- ok.\n");
    }
    else
    {
        printf("open file EEPROM/0/BLACK_BOX failed\n");
    }
}



