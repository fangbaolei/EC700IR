#include "misc.h"
#include "hvsocket.h"
#include "tinyxml.h"
#include <dirent.h>

void LongMulti(WORD16 *pwData, int iWordCount, WORD16 wMulti)
{
    WORD16 wCarry = 0;
    for ( int i = 0; i < iWordCount; i++ )
    {
        DWORD32 dwTemp = pwData[ i ] * wMulti + wCarry;
        pwData[ i ] = dwTemp & 0xffff;
        wCarry = dwTemp >> 16;
    }
}

//函数说明:32位实现64位的乘法,不考虑64位的溢出
//参数: dw32Low 低32位, dw32High 高32位, dwIER 乘数
void MultiplicationDWORD64(DWORD32& dw32Low, DWORD32& dw32High, const DWORD32& dwIER)
{
    DWORD32 dwLowTmp, dwHighTmp, dwLowResult, dwHighResult;
    DWORD32 dwIERTmp = dwIER;
    dwLowTmp = dw32Low;
    dwHighTmp = dw32High;
    dwLowResult = dwHighResult = 0;
    DWORD32 dwTmp;
    while (dwIERTmp != 0)
    {
        if ( (dwIERTmp & 0x01) == 0x01)
        {
            dwTmp = (0xFFFFFFFF - dwLowTmp);
            if ( dwTmp >= dwLowResult )
            {
                dwLowResult += dwLowTmp;
                dwHighResult += dwHighTmp;
            }
            else
            {
                dwLowResult -= (dwTmp + 1);
                dwHighResult += (dwHighTmp + 1);
            }
        }

        dwIERTmp >>= 1;

        if ( (dwLowTmp & 0x80000000) == 0x80000000 )
        {
            dwHighTmp <<= 1;
            dwHighTmp |= 0x01;
            dwLowTmp <<= 1;
        }
        else
        {
            dwHighTmp <<= 1;
            dwLowTmp <<= 1;
        }
    }

    dw32Low = dwLowResult;
    dw32High = dwHighResult;
}

int DelDirContent(const char* szDir)
{
    if ( strlen(szDir) >= MAX_PATH )
    {
        return -1;
    }

    char szCommand[MAX_PATH];
    sprintf(szCommand, "rm -rf %s/*", szDir);

    if ( -1 == HV_System(szCommand) )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int GetFileSize(const char* szFileName, unsigned long& ulFileSize)
{
    struct stat buf;
    if (0 <= stat(szFileName, &buf))
    {
        return buf.st_size;
    }
    return -1;
}

int CreateEmptyFile(const char* szFileName)
{
    int iRet = -1;
    FILE* fp = fopen(szFileName, "wb");
    if ( fp )
    {
        iRet = 0;
        fclose(fp);
        fp = NULL;
    }
    return iRet;
}

int WriteDataToFile(const char* szFileName, unsigned char* pbData, unsigned long ulDataSize)
{
    int fp = open(szFileName, O_WRONLY|O_CREAT);
    if (fp > 0)
    {
        pwrite(fp, pbData, ulDataSize, 0);
        close(fp);
        return 0;
    }
    else
    {
        perror("open file error.\n");
        return -1;
    }
}

int ReadDataFromFile(const char* szFileName, unsigned char* pbData, unsigned long ulDataSize)
{
    unsigned long ulReadedSize = 0;
    FILE *fp = fopen(szFileName, "rb");
    if ( fp )
    {
        ulReadedSize = fread(pbData, 1, ulDataSize, fp);
        fclose(fp);
        return ( ulReadedSize == ulDataSize ) ? 0 : -1;
    }
    return -1;
}

int ShowSystemStatus(char* szStatus, int iSize)
{
    char szLine[255];
    strcpy(szStatus, "");
    FILE * fp = popen("top -n 1", "r");
    if (fp)
    {
        while (!feof(fp))
        {
            fgets(szLine, 255, fp);
            int iLen = strlen(szLine);
            if (iSize > iLen)
            {
                strcat(szStatus, szLine);
                iSize -= iLen;
            }

        }
        fclose(fp);
    }
    return 0;
}

bool MakeSureDirectoryPathExists(
    const char* DirPath
)
{
    if ( NULL == DirPath )
    {
        return false;
    }

    CHvString strPath(DirPath);
    int iSlashPos = 0;
    struct stat st;
    while ( true )
    {
        iSlashPos = strPath.Find("/", iSlashPos);
        if ( -1 == iSlashPos )
        {
            break;
        }
        else
        {
            if (stat(strPath.Left(iSlashPos+1).GetBuffer(), &st) >= 0 && S_ISDIR(st.st_mode))
            {
                ++iSlashPos;
                continue;
            }
            if ( 0 != mkdir(strPath.Left(iSlashPos+1).GetBuffer(), 0777) )
            {
                if ( 17 != errno )  // 17: File exists
                {
                    perror("mkdir");
                    return false;
                }
            }
            ++iSlashPos;
        }
    }

    return true;
}

HRESULT GetDirFileCount(const char* szDir, int& iCount)
{
    struct dirent *ptr;
    int nTotal = 0;
    DIR *dir = opendir(szDir);
    if (!dir)
    {
        return E_FAIL;
    }
    while ((ptr = readdir(dir)))
    {
        if (strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, "..") && ptr->d_type == DT_REG)
        {
            ++nTotal;
        }
    }
    closedir(dir);
    iCount = nTotal;
    return S_OK;
}

int GetDirSize(const char * szDir)
{
    struct dirent *ptr;
    struct stat buf;
    double dblTotalSize = 0;
    DIR *dir = opendir(szDir);
    if (!dir)
    {
        return 0;
    }
    char szSubDir[255];
    while ((ptr = readdir(dir)))
    {
        if (strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, ".."))
        {
            strcpy(szSubDir, szDir);
            strcat(szSubDir, ptr->d_name);

            if (0 <= stat(szSubDir, &buf))
            {
                if (S_ISDIR(buf.st_mode))
                {
                    dblTotalSize += GetDirSize(szSubDir);
                }
                else if (ptr->d_type == DT_REG)
                {
                    dblTotalSize += buf.st_size;
                }
            }
        }
    }
    closedir(dir);
    return int(dblTotalSize/1024);
}

//功能：比较时间大小，比较到小时级别（在可靠性保存中使用）
bool ComputeHour( DWORD32 dwBTimeLow, DWORD32 dwBTimeHigh, DWORD32 dwETimeLow, DWORD32 dwETimeHigh )
{
    REAL_TIME_STRUCT rtBegin, rtEnd;
    ConvertMsToTime(dwBTimeLow, dwBTimeHigh, &rtBegin);
    ConvertMsToTime(dwETimeLow, dwETimeHigh, &rtEnd);

    if ( rtEnd.wYear > rtBegin.wYear )
    {
        return true;
    }
    else if ( rtEnd.wYear == rtBegin.wYear )
    {
        if ( rtEnd.wMonth > rtBegin.wMonth )
        {
            return true;
        }
        else if ( rtEnd.wMonth == rtBegin.wMonth )
        {
            if ( rtEnd.wDay > rtBegin.wDay )
            {
                return true;
            }
            else if ( rtEnd.wDay == rtBegin.wDay )
            {
                if ( rtEnd.wHour > rtBegin.wHour )
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//功能：比较时间BTime和ETime的大小，如果ETime大于BTime则返回1，小于返回-1，等于返回0。
int ComputeTimeMs(DWORD32 dwBTimeLow, DWORD32 dwBTimeHigh, DWORD32 dwETimeLow, DWORD32 dwETimeHigh)
{
    REAL_TIME_STRUCT rtBegin, rtEnd;
    ConvertMsToTime(dwBTimeLow, dwBTimeHigh, &rtBegin);
    ConvertMsToTime(dwETimeLow, dwETimeHigh, &rtEnd);

    if ( rtEnd.wYear > rtBegin.wYear )
    {
        return 1;
    }
    else if ( rtEnd.wYear == rtBegin.wYear )
    {
        if ( rtEnd.wMonth > rtBegin.wMonth )
        {
            return 1;
        }
        else if ( rtEnd.wMonth == rtBegin.wMonth )
        {
            if ( rtEnd.wDay > rtBegin.wDay )
            {
                return 1;
            }
            else if ( rtEnd.wDay == rtBegin.wDay )
            {
                if ( rtEnd.wHour > rtBegin.wHour )
                {
                    return 1;
                }
                else if ( rtEnd.wHour == rtBegin.wHour )
                {
                    if ( rtEnd.wMinute > rtBegin.wMinute )
                    {
                        return 1;
                    }
                    else if ( rtEnd.wMinute == rtBegin.wMinute )
                    {
                        if ( rtEnd.wSecond > rtBegin.wSecond )
                        {
                            return 1;
                        }
                        else if ( rtEnd.wSecond == rtBegin.wSecond )
                        {
                            if ( rtEnd.wMSecond > rtBegin.wMSecond )
                            {
                                return 1;
                            }
                            else if ( rtEnd.wMSecond == rtBegin.wMSecond )
                            {
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }

    return -1;
}

// 时间加一秒
void TimeMsAddOneSecond(DWORD32& dwTimeMsLow, DWORD32& dwTimeMsHigh)
{
    DWORD32 dwTimeOneSec = 1000;
/*计算时间错误
    if ( dwTimeMsLow < 0xffffffff - dwTimeOneSec )
    {
        dwTimeMsLow += dwTimeOneSec;
    }
    else
*/
    {
        DWORD64 dw64TimeMs = dwTimeMsHigh;
        dw64TimeMs <<= 32;
        dw64TimeMs |= dwTimeMsLow;
        dw64TimeMs += dwTimeOneSec;
        dwTimeMsLow = (DWORD32)(dw64TimeMs);
        dwTimeMsHigh = (DWORD32)(dw64TimeMs>>32);
    }
}

// 提取XML格式附加信息中的车牌号
HRESULT GetPlateNumByXmlExtInfo(char* szXmlExtInfo, char* szPlateNum)
{
    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szXmlExtInfo) )
    {
        const TiXmlElement* pRootElement = cXmlDoc.RootElement();
        if ( pRootElement )
        {
            const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
            if ( pElementResultSet )
            {
                const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
                if ( pElementResult )
                {
                    const TiXmlElement* pElementPlateName = pElementResult->FirstChildElement("PlateName");
                    if ( pElementPlateName )
                    {
                        strcpy(szPlateNum, pElementPlateName->GetText());
                        return S_OK;
                    }
                }
            }
        }
    }

    return E_FAIL;
}

HRESULT GetYPlateByXmlExtInfo(char* szXmlExtInfo, int& iYPlate)
{
    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szXmlExtInfo) )
    {
        const TiXmlElement* pRootElement = cXmlDoc.RootElement();
        if ( pRootElement )
        {
            const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
            if ( pElementResultSet )
            {
                const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
                if ( pElementResult )
                {
                    const TiXmlElement* pElementYPlate = pElementResult->FirstChildElement("PlateLight");
                    if ( pElementYPlate )
                    {
                        const char* pszValue = pElementYPlate->Attribute("value");
                        iYPlate = atoi(pszValue);
                        return S_OK;
                    }
                }
            }
        }
    }

    return E_FAIL;
}
// 调试输出指定长度的字符串
void DebugPrintf(const char* szDebugInfo, DWORD dwDumpLen, const char* szID)
{
    DWORD32 iLen = (strlen(szDebugInfo) > dwDumpLen) ? (dwDumpLen) : (strlen(szDebugInfo));
    PBYTE8 pbBuf = new BYTE8[iLen+1];
    if ( pbBuf )
    {
        memcpy(pbBuf, szDebugInfo, iLen);
        memset(pbBuf+iLen, 0, 1);

        printf("DebugPrintf[%s]: [%s]\n", szID,pbBuf);

        delete[] pbBuf;
        pbBuf = NULL;
    }
}

void HV_Dump(unsigned char* buf, int len)
{
    int i, j, n;
    int line = 16;
    char c;

    n = len / line;
    if (len % line)
    {
        n++;
    }

    for (i=0; i<n; i++)
    {
        printf("0x%08x: ", int(buf+i*line));

        for (j=0; j<line; j++)
        {
            if ((i*line+j) < len)
            {
                printf("%02x ", buf[i*line+j]);
            }
            else
            {
                printf("   ");
            }
        }

        printf("  ");
        for (j=0; j<line && (i*line+j)<len; j++)
        {
            if ((i*line+j) < len)
            {
                c = buf[i*line+j];
                printf("%c", c > ' ' && c < '~' ? c : '.');
            }
            else
            {
                printf("   ");
            }
        }

        printf("\n");
    }
}

void DebugShowTime(const char* lpszFlag, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh)
{
    REAL_TIME_STRUCT rtTime;
    ConvertMsToTime(dwTimeMsLow, dwTimeMsHigh, &rtTime);
    HV_Trace(5, "<DebugShowTime> [%04d_%02d_%02d %02d:%02d:%02d.%03d] - %s\n",
             rtTime.wYear,
             rtTime.wMonth,
             rtTime.wDay,
             rtTime.wHour,
             rtTime.wMinute,
             rtTime.wSecond,
             rtTime.wMSecond,
             lpszFlag
            );
}

void OutputDebugInfoWithTime(char* szDebugInfo)
{
    static BOOL fLastStrIsPoint = FALSE;
    static char szBuff[1024] = {0};

    if (strcmp(szDebugInfo, ".") != 0
            || (strcmp(szDebugInfo, ".") == 0 && !fLastStrIsPoint))
    {
        DWORD32 dwTimeMsLow;
        DWORD32 dwTimeMsHigh;
        GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);
        REAL_TIME_STRUCT rtTime;
        ConvertMsToTime(dwTimeMsLow, dwTimeMsHigh, &rtTime);
        DWORD32 dwSrcLen = strlen(szDebugInfo);
        sprintf(
            szBuff, "%s[%04d/%02d/%02d %02d:%02d:%02d.%03d] ",
            fLastStrIsPoint ? "\n" : "",
            rtTime.wYear, rtTime.wMonth, rtTime.wDay,
            rtTime.wHour, rtTime.wMinute, rtTime.wSecond,
            rtTime.wMSecond
        );
        DWORD32 dwAddLen = strlen(szBuff);
        if ( (dwAddLen + dwSrcLen) < sizeof(szBuff) )
        {
            strcat(szBuff, szDebugInfo);
        }

        fLastStrIsPoint = (strcmp(szDebugInfo, ".") == 0);

        printf(szBuff);
    }
    else
    {
        fLastStrIsPoint = TRUE;
        printf(szDebugInfo);
    }
}

void DebugSystemPrint(const char* szShellCmd, char* szBuf, int iBufLen)
{
    char szText[256] = {0};

    FILE* fp = popen(szShellCmd, "r");
    if (fp)
    {
        fread(szText, 1, sizeof(szText), fp);
        pclose(fp);

        if ( szBuf != NULL && iBufLen > (int)strlen(szText) )
        {
            strcpy(szBuf, szText);
        }
    }
}

HRESULT GetSecondsSinceLinuxBoot(int* piSeconds)
{
    struct sysinfo info;

    if ( 0 == sysinfo(&info) )
    {
        if ( piSeconds )
        {
            *piSeconds = (int)info.uptime;
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT GetMiddleString(
    char *pszSrc,
    const char *pszBegin,
    const char *pszEnd,
    char *pszOut,
    int nBufSize
)
{
    if (pszSrc == NULL
            || pszBegin == NULL
            || pszEnd == NULL
            || pszOut == NULL)
    {
        return E_POINTER;
    }

    char *pszStr1 = strstr(pszSrc, pszBegin), *pszStr2 = NULL;
    if (pszStr1 == NULL)
    {
        return E_FAIL;
    }

    pszStr2 = strstr(pszStr1, pszEnd);
    if (pszStr2 == NULL)
    {
        return E_FAIL;
    }

    int nStrLen = pszStr2 - pszStr1 + 1;
    if (nStrLen >= nBufSize)
    {
        return E_FAIL;
    }

    memcpy(pszOut, pszStr1, nStrLen);
    pszOut[nStrLen] = 0;
    return S_OK;
}
