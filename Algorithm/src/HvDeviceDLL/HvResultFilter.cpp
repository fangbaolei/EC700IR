#include "HvResultFilter.h"

#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif
#ifndef VK_TAB
#define VK_TAB 0x09
#endif

#include <atltime.h>

//////////////////////////////////////////////////////////////////////////////////////
//时间格式类
///////////////////////////////////////////////////////////////////////////////////////

CRuleTime::CRuleTime()
: m_iHour(0)
, m_iMinute(0)
, m_iSecond(0)
{
}

CRuleTime::~CRuleTime()
{
}

bool CRuleTime::SetTime( const int iHour, const int iMinute, const int iSecond )
{
	m_iHour = (iHour < 0 || iHour > 23) ? 0 : iHour;
	m_iMinute = (iMinute < 0 || iMinute > 59) ? 0 : iMinute;
	m_iSecond = (iSecond < 0 || iSecond > 59) ? 0 : iSecond;

	return (iHour < 0 || iHour > 23 || iMinute < 0 || iMinute > 59 || iSecond < 0 || iSecond > 59) ? false : true;
}

bool CRuleTime::SetTime( LPCSTR lpszTime )
{
	bool fRet = true;

	m_iHour = m_iMinute = m_iSecond = 0;
	if( 3 != sscanf(lpszTime, "%d:%d:%d", &m_iHour, &m_iMinute, &m_iSecond) )
	{
		m_iHour = m_iMinute = m_iSecond = 0;
		fRet = false;
	}
	else
	{
		fRet = SetTime( m_iHour, m_iMinute, m_iSecond );
	}

	return fRet;
}

bool CRuleTime::operator == (const CRuleTime& crt) const
{
	return (m_iHour == crt.GetHour() && m_iMinute == crt.GetMinute() && m_iSecond == crt.GetSecond()) ? true : false; 
}

bool CRuleTime::operator > (const CRuleTime& crt) const 
{
	if( m_iHour > crt.GetHour() )
	{
		return true;
	}
	else if( m_iHour == crt.GetHour() )
	{
		if( m_iMinute > crt.GetMinute() )
		{
			return true;
		}
		else if( m_iMinute == crt.GetMinute() )
		{
			if( m_iSecond > crt.GetSecond() && m_iSecond != crt.GetSecond() )
			{
				return true;
			}
		}
	}

	return false;
}

bool CRuleTime::operator < (const CRuleTime& crt) const 
{
	return (*this > crt || *this == crt) ? false : true; 
}

bool CRuleTime::operator >= (const CRuleTime& crt) const 
{
	return (*this > crt || *this == crt) ? true : false;
}

bool CRuleTime::operator <= (const CRuleTime& crt) const 
{
	return (*this < crt || *this == crt) ? true : false;
}

bool CRuleTime::IsCourseOfTime( const CRuleTime& crtLeft, const CRuleTime& crtRight, const CRuleTime& crtNow )
{
	bool fRet = true;
	if( crtRight >= crtLeft )
	{
		if( crtNow < crtLeft || crtNow > crtRight )
		{
			fRet = false;
		}
 	}
	else if( crtRight < crtLeft )
	{
		if( crtNow > crtRight && crtNow < crtLeft )
		{
			fRet = false;
		}
	}
	return fRet;
}

///////////////////////////////////////////////////////////////////////////////////////
//                        后处理类
///////////////////////////////////////////////////////////////////////////////////////////

const int g_cbMaxRead = 1024 * 64;

//后处理字段名
const char* g_szLeach = "过滤规则";
const char* g_szAlram  = "报警规则";
const char* g_szFolder = "分类目录";
const char* g_szReplace = "替换规则";
const char* g_szCompaty = "通配符规则";


//全角转半角
extern BOOL SBC2DBC( CHvString& str )
{
	if( str.IsEmpty() )
	{
		return TRUE;
	}

	int iLen = str.GetLength();
	unsigned char* pszTemp = NULL;
	pszTemp = new unsigned char[ iLen + 1 ];
	if( pszTemp == NULL ) 
	{
		return FALSE;
	}
	int iPos = 0;
	unsigned char ch = '\0';
	for( int i = 0; i < iLen; ++i )
	{
		ch = str[i];
		if( ch != 163 )
		{
			if( ch > 127 )
			{
				pszTemp[ iPos++ ] = ch;
				pszTemp[ iPos++ ] = str[++i];
			}
			else
			{
				pszTemp[ iPos++ ] = ch;
			}	
		}
		else
		{
			++i;
			if( i >= iLen )
			{
				break;
			}
			ch = str[i];
			pszTemp[ iPos++ ]  = ch - 128;
		}
	}
	pszTemp[ iPos ] = '\0';
	str = (char*)pszTemp;
	delete[] pszTemp;
	return TRUE;
}

#if RUN_PLATFORM == PLATFORM_WINDOWS //Win32
//得到当前时间
void GetTime( CRuleTime& crtime )
{
	CTime ctime = CTime::GetCurrentTime();
	crtime.SetTime( ctime.GetHour(), ctime.GetMinute(), ctime.GetSecond() );
}
#else
//得到当前时间
void GetTime( CRuleTime& crtime )
{
	DWORD32 dwTimeLow, dwTimeHigh;
	HV_GetSystemTime(&dwTimeLow, &dwTimeHigh);
	REAL_TIME_STRUCT RealTime;

	if ( ( dwTimeLow == 0xFFFFFFFF ) && ( dwTimeHigh == 0xFFFFFFFF ) ) return;

	DWORD32 dwTimeLowTemp  = dwTimeLow + 1;
	DWORD32 dwTimeHighTemp = dwTimeHigh;
	ConvertMsToTime( &RealTime, dwTimeLowTemp, dwTimeHighTemp );	

	crtime.SetTime( (int)RealTime.wHour, (int)RealTime.wMinute, (int)RealTime.wSecond );
}
#endif

//从lpszFileName读出一个小节lpszSection的内容到lpszOutStr, 结构为 key=value\0
int GetFileSectionFromIni( LPCSTR lpszSection, LPSTR lpszOutStr, int iLen, CParamStore* pParam, LPCSTR lpszFileName )
{
#if RUN_PLATFORM == PLATFORM_WINDOWS
	if( lpszSection == NULL || lpszFileName == NULL || lpszOutStr == NULL )
	{
		return 0;
	}
	memset( lpszOutStr, 0, iLen );
	const int iMaxLine = 1024;
	char szLine[ iMaxLine ] = {0};
	char szSection[ iMaxLine ] = {0};
	if( strlen( lpszSection ) + 2 > iMaxLine )
	{
		return 0;
	}
	sprintf( szSection, "[%s]", lpszSection );

	int iNowLen = 0;
	FILE* pfile = fopen( lpszFileName, "rt" );

	bool fIsFind = false;
	if( pfile != NULL )
	{
		while( fgets( szLine, iMaxLine, pfile ) != NULL )
		{
			int iPos =  0;
			while( szLine[ iPos ] != 0 )
			{
				if( szLine[ iPos ] == '\r' || szLine[ iPos ] == '\n' )
				{
					szLine[ iPos ] = 0;
					break;
				}
				++iPos;
			}
			if( fIsFind )
			{
				if( szLine[0] == '[' )
				{
					break;
				}
				if( szLine[0] == ';' )
				{
					continue;
				}
				strcpy( lpszOutStr + iNowLen, szLine );
				iNowLen += (int)strlen( szLine ) + 1;
			}
			else
			{
				if( szLine[0] == '[' )
				{
					if( strcmp( szLine, szSection ) == 0 )
					{
						fIsFind = true;
					}
				}
			}
		}
		fclose( pfile );
	}

	return iNowLen;
#else
	return 0;
#endif
}

//从lpszFileName读出一个小节lpszSection的内容到lpszOutStr, 结构为 key=value\0
int GetFileSectionFromXML( LPCSTR lpszSection, LPSTR lpszOutStr, int iLen, CParamStore* pParam, LPCSTR lpszFileName )
{
	/*
	int iRet = 0;
	if( lpszOutStr == NULL || iLen <= 0 ) return iRet;

	DWORD32 cbRead = 0;
	HRESULT hr = E_FAIL;

	unsigned char* pbBuffer = new unsigned char[8 * 1024];
	if( pbBuffer == NULL ) return iRet;

	if( strcmp(lpszSection, g_szLeach) == 0 )
	{
		cbRead = 8 * 1024;
		hr = GetLeachData( pParam, pbBuffer, &cbRead );
	}
	else if( strcmp(lpszSection, g_szReplace) == 0)
	{
		cbRead = 8 * 1024;
		hr = GetReplaceData( pParam, pbBuffer, &cbRead );
	}
	else if( strcmp(lpszSection, g_szCompaty) == 0 )
	{
		cbRead = 8 * 1024;
		hr = GetCompatyData( pParam, pbBuffer, &cbRead );
	}

	if( hr == S_OK && cbRead > 1 )
	{
		int iPos = 0;
		if( pbBuffer[0] == '$' )
		{
			iPos = 1;
		}
		iRet = iLen > (int)cbRead ? cbRead : iLen;
		if( pbBuffer[cbRead - 1] == '$' )
		{
			iRet -= iPos + 1;
		}
		else
		{
			iRet -= iPos;
		}
		
		memset(lpszOutStr, 0, iLen);
		strncpy(lpszOutStr, (const char*)&(pbBuffer[iPos]), iRet);
		iPos = 0;
		while( lpszOutStr[iPos] != '\0' && iPos < iLen )
		{
			if( lpszOutStr[iPos] == '$' )
			{
				lpszOutStr[iPos] = '\0';
			}
			++iPos;
		}
	}

	delete[] pbBuffer;
	return iRet;
	*/
	return 0;
}

int GetFileSection( LPCSTR lpszSection, LPSTR lpszOutStr, int iLen, CParamStore* pParam, LPCSTR lpszFileName )
{
	if (pParam)
	{
		return GetFileSectionFromXML(lpszSection, lpszOutStr, iLen, pParam, lpszFileName);
	}
	else
	{
		return GetFileSectionFromIni(lpszSection, lpszOutStr, iLen, pParam, lpszFileName);
	}
}

CHvResultFilter::CHvResultFilter(void)
{
	m_fInitialize = FALSE;
}

CHvResultFilter::~CHvResultFilter(void)
{
}

//初始化规则
BOOL CHvResultFilter::InitRule( CParamStore* pParam, const char* strFilePath )
{
	m_strFilePath = strFilePath;
	m_pParam = pParam;
	if( InitLeach() && InitAlarm() && InitFolder() && InitReplace() && InitCompaty() )
	{
		m_fInitialize = TRUE;
	}
	else
	{
		m_fInitialize = FALSE;
	}
	
	return m_fInitialize;
}

//执行后处理
BOOL CHvResultFilter::FilterProcess( 
				    RESULT_INFO* pResultInfo,
				    const BOOL& fReplace/* = TRUE*/,
				    const BOOL& fAlarm /*= TRUE*/, 
				    const BOOL& fLeach /*= TRUE*/ ,
				    const BOOL& fLoopReplace /*= FALSE*/
				    )
{
	if( pResultInfo == NULL ) return FALSE;

	m_strApplied = "";
	m_strOther = pResultInfo->strOther;

	if( fReplace )
	{
		if( !ReplacePlate( pResultInfo->strPlate, fLoopReplace ) ) return FALSE;
	}
	if( fLeach )
	{
		pResultInfo->fLeach = IsNeedLeach( pResultInfo->strPlate );
	}
	if( fAlarm )
	{
		pResultInfo->fAlarm = IsNeedAlarm( pResultInfo->strPlate );
	}
	GetPlateFolder( pResultInfo->strPlate, pResultInfo->strFolder );

	pResultInfo->strApplied = m_strApplied;

	return TRUE;
}

//规则库的初始化
BOOL CHvResultFilter::InitLeach()
{
	m_lstLeach.RemoveAll();

	CHvString strSection = g_szLeach;
	char* buf = new char[ g_cbMaxRead ];
	if (!buf)
	{
		return FALSE;
	}
	memset(buf, 0, g_cbMaxRead);

	int cbRead = GetFileSection( strSection.GetBuffer(), buf, g_cbMaxRead, m_pParam, m_strFilePath.IsEmpty() ? NULL : m_strFilePath.GetBuffer() );

	if( cbRead == 0 )
	{
		delete [] buf;
		return TRUE;
	}

	CHvString strResult = buf;
	LEACH_INFO leachinfo;

	int iPos1 = 0;
	int iPos2 = 0;
	int iLen = strResult.GetLength() + 1;

	strResult.Remove( VK_TAB );
	strResult.Remove( VK_SPACE );
	while( !strResult.IsEmpty() )
	{
		iPos1 = strResult.Find( "=" );
		iPos2 = strResult.Find( ";" );
		if( iPos1 != -1 )
		{
			leachinfo.strPattern = strResult.Left( iPos1 );
			int iTmp = 0;
			int iLeach = 0;
			if( iPos2 != -1 )
			{
				iTmp = sscanf( strResult.Mid( iPos1+1, iPos2-iPos1-1 ),
					"%d", &iLeach );
				leachinfo.strComment = strResult.Mid( iPos2 );
			}
			else
			{
				iTmp = sscanf( strResult.Mid( iPos1+1 ),
					"%d", &iLeach );
				leachinfo.strComment = "";
			}

			if( iTmp == 1 && !leachinfo.strPattern.IsEmpty() )
			{
				leachinfo.fLeach = (iLeach == 1); 
				SBC2DBC( leachinfo.strPattern );
				SBC2DBC( leachinfo.strComment );

				//避免规则的重复
				HVPOSITION hvpos = m_lstLeach.GetHeadPosition();
				LEACH_INFO infoTmp;
				BOOL fHas = FALSE;
				while(hvpos != 0)
				{
					infoTmp = m_lstLeach.GetNext(hvpos);
					if( infoTmp.strPattern == leachinfo.strPattern )
					{
						fHas = TRUE;
						break;
					}
				}
				if( !fHas )
				{
					m_lstLeach.AddTail(leachinfo);
				}
			}
		}
		strResult = ( buf + iLen );
		iLen += strResult.GetLength() + 1;
		strResult.Remove( VK_TAB );
		strResult.Remove( VK_SPACE );
	}

	delete [] buf;
	return TRUE;
}

BOOL CHvResultFilter::InitAlarm()
{
	m_lstAlarm.RemoveAll();

	CHvString strSection = g_szAlram;
	char* buf = new char[ g_cbMaxRead ];
	if (!buf)
	{
		return FALSE;
	}
	memset(buf, 0, g_cbMaxRead);

	int cbRead = GetFileSection( strSection.GetBuffer(), buf, g_cbMaxRead, m_pParam, m_strFilePath.IsEmpty() ? NULL : m_strFilePath.GetBuffer() );

	if( cbRead == 0 )
	{
		delete [] buf;
		return TRUE;
	}

	CHvString strResult = buf;
	ALARM_INFO alarminfo;

	int iPos1 = 0;
	int iPos2 = 0;
	int iLen = strResult.GetLength() + 1;

	strResult.Remove( VK_TAB );
	strResult.Remove( VK_SPACE );
	while( !strResult.IsEmpty() )
	{
		iPos1 = strResult.Find( "=" );
		iPos2 = strResult.Find( ";" );
		if( iPos1 != -1 )
		{
			alarminfo.strPattern = strResult.Left( iPos1 );
			int iTmp = 0;
			int iAlarm = 0;
			if( iPos2 != -1 )
			{
				iTmp = sscanf( strResult.Mid( iPos1+1, iPos2-iPos1-1 ),
					"%d", &iAlarm );
				alarminfo.strComment = strResult.Mid( iPos2 );
			}
			else
			{
				iTmp = sscanf( strResult.Mid( iPos1+1 ),
					"%d", &iAlarm );
				alarminfo.strComment = "";
			}

			if( iTmp == 1 && !alarminfo.strPattern.IsEmpty() )
			{
				alarminfo.fAlarm = (iAlarm == 1); 
				SBC2DBC( alarminfo.strPattern );
				SBC2DBC( alarminfo.strComment );

				//避免规则的重复
				HVPOSITION hvpos = m_lstAlarm.GetHeadPosition();
				ALARM_INFO infoTmp;
				BOOL fHas = FALSE;
				while(hvpos != 0)
				{
					infoTmp = m_lstAlarm.GetNext(hvpos);
					if( infoTmp.strPattern == alarminfo.strPattern )
					{
						fHas = TRUE;
						break;
					}
				}
				if( !fHas )
				{
					m_lstAlarm.AddTail(alarminfo);
				}
			}
		}
		strResult = ( buf + iLen );
		iLen += strResult.GetLength() + 1;
		strResult.Remove( VK_TAB );
		strResult.Remove( VK_SPACE );
	}

	delete [] buf;
	return TRUE;
}

BOOL CHvResultFilter::InitFolder()
{
	m_lstFolder.RemoveAll();

	CHvString strSection = g_szFolder;
	char* buf = new char[ g_cbMaxRead ];
	if (!buf)
	{
		return FALSE;
	}
	memset(buf, 0, g_cbMaxRead);

	int cbRead = GetFileSection( strSection.GetBuffer(), buf, g_cbMaxRead, m_pParam, m_strFilePath.GetBuffer() );

	if( cbRead == 0 ) 
	{
		delete [] buf;
		return TRUE;
	}

	CHvString strResult = buf;
	FOLDER_INFO folderinfo;
	int iPos1 = 0;
	int iPos2 = 0;
	int iLen = strResult.GetLength() + 1;

	strResult.Remove( VK_TAB );
	strResult.Remove( VK_SPACE );

	while( !strResult.IsEmpty() )
	{
		iPos1 = strResult.Find( "=" );
		iPos2 = strResult.Find( ";" );
		if( iPos1 != -1 )
		{
			folderinfo.strPattern = strResult.Left( iPos1 );
			if( iPos2 != -1 )
			{
				folderinfo.strFolder = strResult.Mid( iPos1+1, iPos2-iPos1-1 );
				folderinfo.strComment = strResult.Mid( iPos2 );
			}
			else
			{
				folderinfo.strFolder  = strResult.Mid( iPos1+1 );
				folderinfo.strComment = "";
			}
			if( !folderinfo.strPattern.IsEmpty()
				&& !folderinfo.strFolder.IsEmpty() )
			{
				SBC2DBC( folderinfo.strPattern );
				SBC2DBC( folderinfo.strFolder );
				SBC2DBC( folderinfo.strComment );

				//避免规则的重复
				//HVPOSITION hvpos = m_lstFolder.GetHeadPosition();
				//FOLDER_INFO infoTmp;
				//BOOL fHas = FALSE;
				//while(hvpos != 0)
				//{
				//	infoTmp = m_lstFolder.GetNext(hvpos);
				//	if( infoTmp.strPattern == folderinfo.strPattern )
				//	{
				//		fHas = TRUE;
				//		break;
				//	}
				//}
				//if( !fHas )
				//{
				//	m_lstFolder.AddTail(folderinfo);
				//}

				//分类目录比较特殊,不过滤相同的规则
				m_lstFolder.AddTail(folderinfo);
			}
		}
		strResult = ( buf + iLen );
		iLen += strResult.GetLength() + 1;

		strResult.Remove( VK_TAB );
		strResult.Remove( VK_SPACE );
	}

	delete [] buf;
	return TRUE;
}

BOOL CHvResultFilter::InitReplace()
{
	m_lstReplace.RemoveAll();

	CHvString strSection = g_szReplace;
	char* buf = new char[ g_cbMaxRead ];
	if (!buf)
	{
		return FALSE;
	}
	memset(buf, 0, g_cbMaxRead);

	int cbRead = GetFileSection( strSection.GetBuffer(), buf, g_cbMaxRead, m_pParam, m_strFilePath.GetBuffer() );

	if( cbRead == 0 ) 
	{
		delete [] buf;
		return TRUE;
	}

	CHvString strResult = buf;
	REPLACE_INFO replaceinfo;
	int iPos1 = 0;
	int iPos2 = 0;
	int iLen = strResult.GetLength() + 1;

	strResult.Remove( VK_TAB );
	strResult.Remove( VK_SPACE );

	while( !strResult.IsEmpty() )
	{
		iPos1 = strResult.Find( "=" );
		iPos2 = strResult.Find( ";" );
		if( iPos1 != -1 )
		{
			replaceinfo.strPattern = strResult.Left( iPos1 );
			if( iPos2 != -1 )
			{
				replaceinfo.strResult = strResult.Mid( iPos1+1, iPos2-iPos1-1 );
				replaceinfo.strComment = strResult.Mid( iPos2 );
			}
			else
			{
				replaceinfo.strResult  = strResult.Mid( iPos1+1 );
				replaceinfo.strComment = "";
			}
			if( !replaceinfo.strPattern.IsEmpty()
				&& !replaceinfo.strResult.IsEmpty() )
			{
				SBC2DBC( replaceinfo.strPattern );
				SBC2DBC( replaceinfo.strResult );
				SBC2DBC( replaceinfo.strComment );

				//避免规则的重复
				HVPOSITION hvpos = m_lstReplace.GetHeadPosition();
				REPLACE_INFO infoTmp;
				BOOL fHas = FALSE;
				while(hvpos != 0)
				{
					infoTmp = m_lstReplace.GetNext(hvpos);
					if( infoTmp.strPattern == replaceinfo.strPattern )
					{
						fHas = TRUE;
						break;
					}
				}
				if( !fHas )
				{
					m_lstReplace.AddTail(replaceinfo);
				}
			}
		}
		strResult = ( buf + iLen );
		iLen += strResult.GetLength() + 1;

		strResult.Remove( VK_TAB );
		strResult.Remove( VK_SPACE );
	}

	delete [] buf;
	return TRUE;
}

BOOL CHvResultFilter::InitCompaty()
{
	m_lstCompaty.RemoveAll();

	CHvString strSection = g_szCompaty;
	char* buf = new char[ g_cbMaxRead ];
	if (!buf)
	{
		return FALSE;
	}
	memset(buf, 0, g_cbMaxRead);

	int cbRead = GetFileSection( strSection.GetBuffer(), buf, g_cbMaxRead, m_pParam, m_strFilePath.GetBuffer() );

	if( cbRead == 0 )
	{
		delete [] buf;
		return TRUE;
	}

	CHvString strResult = buf;
	COMPATY_WORD compatyword;
	int iPos1 = 0;
	int iPos2 = 0;
	int iLen = strResult.GetLength() + 1;

	strResult.Remove( VK_TAB );
	strResult.Remove( VK_SPACE );
	while( !strResult.IsEmpty() )
	{
		iPos1 = strResult.Find( "=" );
		iPos2 = strResult.Find( ";" );
		if( iPos1 != -1 )
		{
			compatyword.strPattern = strResult.Left( iPos1 );
			if( iPos2 != -1 )
			{
				compatyword.strResult  = strResult.Mid( iPos1+1, iPos2-iPos1-1 );
			}
			else
			{
				compatyword.strResult  = strResult.Mid( iPos1+1 );
			}
			if( !compatyword.strPattern.IsEmpty()
				&& !compatyword.strResult.IsEmpty() )
			{
				SBC2DBC( compatyword.strPattern );
				SBC2DBC( compatyword.strResult );

				//避免规则的重复
				HVPOSITION hvpos = m_lstCompaty.GetHeadPosition();
				COMPATY_WORD infoTmp;
				BOOL fHas = FALSE;
				while(hvpos != 0)
				{
					infoTmp = m_lstCompaty.GetNext(hvpos);
					if( infoTmp.strPattern == compatyword.strPattern )
					{
						fHas = TRUE;
						break;
					}
				}
				if( !fHas )
				{
					m_lstCompaty.AddTail(compatyword);
				}
			}
		}
		strResult = ( buf + iLen );
		iLen += strResult.GetLength() + 1;

		strResult.Remove( VK_TAB );
		strResult.Remove( VK_SPACE );
	}
	delete [] buf;
	return TRUE;	
}

//车牌替换
BOOL CHvResultFilter::ReplacePlate( CHvString& strPlate, const BOOL& fLoopReplace )
{
	if( !m_fInitialize ) return FALSE;
	if( strPlate.IsEmpty() ) return FALSE;
	if( m_lstReplace.IsEmpty() ) return TRUE;

	REPLACE_INFO replaceinfo;
	REPLACE_INFO bestinfo;
	CHvString strSrc;
	CHvString strDest(strPlate);
	CHvString strTmp;

	CRuleTime crtime;
	GetTime(crtime);

	CHvString strCur("*");
	int iPos = -1;

	HVPOSITION position = m_lstReplace.GetHeadPosition();
	while(position != 0)
	{
		replaceinfo = m_lstReplace.GetNext(position);
		strSrc = replaceinfo.strPattern;
		if( Match( strSrc, strDest ) )
		{
			if( !replaceinfo.strComment.IsEmpty() )
			{
				if( !(CheckRuleTimer( replaceinfo.strComment, crtime ) 
					&& CheckRuleRely( replaceinfo.strComment, m_strOther )
					&& CheckPlateType( replaceinfo.strComment, m_strOther ) 
					&& CheckPlateSpeed( replaceinfo.strComment, m_strOther )
					&& CheckRoadNumber(replaceinfo.strComment, m_strOther )
					) )
				{
					continue;
				}
			}
			//是否是循环替换方式
			if( fLoopReplace )
			{
				DoReplace(strDest, replaceinfo);
			}
			else
			{
				int iTmp = RuleCompare( strSrc, strCur );
				if( iTmp == 2 || iTmp == 0 )
				{
					strCur = strSrc;
					bestinfo = replaceinfo;
					iPos = 0;
				}
			}
		}
	}

	if( !fLoopReplace && iPos != -1)
	{
		DoReplace(strDest, bestinfo);
	}

	strPlate = strDest;

	return TRUE;
}

void CHvResultFilter::DoReplace(CHvString& strPlate, REPLACE_INFO& replaceinfo)
{
	unsigned char bTemp = 0;
	CHvString strResult(replaceinfo.strResult);
	CHvString strConv;
	CHvString strDest(strPlate);
	for (int j=0, i=0; i<strResult.GetLength() && j<strDest.GetLength(); j++, ++i)
	{
		if (strResult[i]=='?')
		{
			strConv += strDest[j];
			bTemp = strDest[j];
			if( (bTemp & 0x80) == 0x80 )
			{
				strConv += strDest[++j];
			}
			continue;
		}
		if (strResult[i]=='*')
		{
			strConv += strDest.Mid(j);
			break;
		}

		strConv += strResult[i];			
		bTemp = strResult[i];
		if( (bTemp & 0x80) == 0x80 )
		{
			strConv += strResult[++i];
		}
		bTemp = strDest[j];
		if( (bTemp & 0x80) == 0x80 )
		{
			++j;
		}
	}
	strDest = strConv;

	//通配符替换
	CHvString strPlateTmp;
	CHvString strWord;
	char szTmp[8] ={0};
	for( int iStr = 0; iStr < strDest.GetLength(); ++iStr)
	{
		szTmp[0] = strDest[iStr];
		strWord = szTmp;
		HVPOSITION position = m_lstCompaty.GetHeadPosition();
		while(position != 0)
		{
			COMPATY_WORD compatyword = m_lstCompaty.GetNext(position);
			if( strDest[iStr] == compatyword.strPattern[0] )
			{
				strWord = compatyword.strResult;
				break;
			}
		}
		strPlateTmp += strWord;
	}
	strDest = strPlateTmp;

	CHvString strTmp;
	strTmp.Format("\n车牌替换:(%s->%s) [%s=%s]", strPlate.GetBuffer(), strDest.GetBuffer(),replaceinfo.strPattern.GetBuffer(), replaceinfo.strResult.GetBuffer());
	strPlate = strDest;
	m_strApplied.Append( strTmp );
}

//分类目录
BOOL CHvResultFilter::GetPlateFolder( CHvString& strPlate, CHvString& strFolder )
{
	//只有WIN32平台支持此方法
#if RUN_PLATFORM != PLATFORM_WINDOWS
	return TRUE;
#else
	if( !m_fInitialize ) return FALSE;
	if( strPlate.IsEmpty() ) return FALSE;

	//读取默认值
	strFolder = "常规";
	char szFolder[ 1024 ] = {0};
	GetPrivateProfileStringA( "分类目录", "默认", "常规", szFolder, 1024, m_strFilePath.GetBuffer() );
	strFolder = szFolder;
	int iTmp = strFolder.Find( ";" );
	if( iTmp != -1 ) strFolder = strFolder.Left( iTmp );
	strFolder.Remove( VK_TAB );
	strFolder.Remove( VK_SPACE );

	CHvString strSrc;
	CHvString strDest(strPlate);

	FOLDER_INFO folderinfo;
	CHvString strCur("*");

	CRuleTime crtime;
	GetTime(crtime);

	HVPOSITION position = m_lstFolder.GetHeadPosition();
	while(position != 0)
	{
		folderinfo = m_lstFolder.GetNext(position);
		strSrc = folderinfo.strPattern;
		if( Match( strSrc, strDest ) )
		{
			if( !folderinfo.strComment.IsEmpty() )
			{
				if( !(CheckRuleTimer( folderinfo.strComment, crtime ) 
					&& CheckRuleRely( folderinfo.strComment, m_strOther )
					&& CheckPlateType( folderinfo.strComment, m_strOther ) 
					&& CheckPlateSpeed( folderinfo.strComment, m_strOther ) ))
				{
					continue;
				}
			}
			int iTmp = RuleCompare( strSrc, strCur );
			if( iTmp == 2 || iTmp == 0 )
			{
				strCur = strSrc;
				strFolder = folderinfo.strFolder;
			}
		}

	}

	return TRUE;
#endif
}

//是否要过滤
BOOL CHvResultFilter::IsNeedLeach(CHvString& strPlate)
{
	if( !m_fInitialize ) return FALSE;
	if( strPlate.IsEmpty() ) return FALSE;
	if( m_lstLeach.IsEmpty() ) return FALSE;

	BOOL fRet = FALSE;
	CHvString strSrc;
	CHvString strDest(strPlate);

	LEACH_INFO leachinfo;
	CHvString strCur("*");

	CRuleTime crtime;
	GetTime(crtime);

	HVPOSITION position = m_lstLeach.GetHeadPosition();
	while(position != 0)
	{
		leachinfo = m_lstLeach.GetNext(position);
		strSrc = leachinfo.strPattern;
		if( Match( strSrc, strDest ) )
		{
			if( !leachinfo.strComment.IsEmpty() )
			{
				if( !(CheckRuleTimer( leachinfo.strComment, crtime ) 
					&& CheckRuleRely( leachinfo.strComment, m_strOther )
					&& CheckPlateType( leachinfo.strComment, m_strOther ) 
					&& CheckPlateSpeed(leachinfo.strComment, m_strOther )
					&& CheckReverseRun(leachinfo.strComment, m_strOther ) 
					&& CheckRoadNumber(leachinfo.strComment, m_strOther )
					) )
				{
					continue;
				}
			}

			int iTmp = RuleCompare( strSrc, strCur );
			if( iTmp == 2 || iTmp == 0 )
			{
				strCur = strSrc;
				fRet = leachinfo.fLeach;
			}
		}
	}

	return fRet;
}

//是否要报警
BOOL CHvResultFilter::IsNeedAlarm(CHvString& strPlate)
{
	if( !m_fInitialize ) return FALSE;
	if( strPlate.IsEmpty() ) return FALSE;
	if( m_lstAlarm.IsEmpty() ) return FALSE;

	BOOL fRet = FALSE;
	CHvString strSrc;
	CHvString strDest(strPlate);

	ALARM_INFO alarminfo;
	CHvString strCur("*");

	CRuleTime crtime;
	GetTime(crtime);

	HVPOSITION position = m_lstAlarm.GetHeadPosition();
	while(position != 0)
	{
		alarminfo = m_lstAlarm.GetNext(position);
		strSrc = alarminfo.strPattern;
		if( Match( strSrc, strDest ) )
		{
			if( !alarminfo.strComment.IsEmpty() )
			{
				if( !(CheckRuleTimer( alarminfo.strComment, crtime ) 
					&& CheckRuleRely( alarminfo.strComment, m_strOther )
					&& CheckPlateType( alarminfo.strComment, m_strOther ) 
					&& CheckPlateSpeed( alarminfo.strComment, m_strOther) ) )
				{
					continue;
				}
			}

			int iTmp = RuleCompare( strSrc, strCur );
			if( iTmp == 2 || iTmp == 0 )
			{
				strCur = strSrc;
				fRet = alarminfo.fAlarm;
			}
		}
	}

	return fRet;
}

//进行对比
BOOL CHvResultFilter::Match( LPCSTR lpszPattern, LPCSTR lpszTarget )
{
	if( lpszPattern == NULL || lpszTarget == NULL ) return FALSE;
	int iPlusPattern = ( (unsigned char)lpszPattern[0] & 0x80 ) == 0x80 ? 2 : 1;
	int iPlusTarger  = ( (unsigned char)lpszTarget[0] & 0x80 ) == 0x80 ? 2 : 1;

	switch (lpszPattern[0])
	{
	case 0:
		return (lpszTarget[0]==0);
		//break;
	case '*':
		do
		{
			if (Match(lpszPattern + iPlusPattern, lpszTarget))
			{
				return TRUE;
			}
			if( *(lpszTarget) == 0 )
			{
				break;
			}
			lpszTarget += iPlusTarger;
		} while (true);
		break;
	case '?':
		if (lpszTarget[0]==0)
		{
			return FALSE;
		}
		else
		{
			return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger);
		}
		//break;
	case '%'://%代表数字
		if (lpszTarget[0]<'0' || lpszTarget[0]>'9')
		{
			return FALSE;
		}
		else
		{
			return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger);
		}
	case '@'://@代表字母
		if (lpszTarget[0]<'A' || lpszTarget[0]>'Z')
		{
			return FALSE;
		}
		else
		{
			return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger);
		}
	default:
		if (lpszPattern[0]==lpszTarget[0] && lpszPattern[0 + iPlusPattern - 1] == lpszTarget[0 + iPlusTarger -1] )
		{
			return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger );
		}
		break;
	}

	return FALSE;	
}

int CHvResultFilter::RuleCompare( LPCSTR lpszLeft, LPCSTR lpszRight )
{
	if( lpszLeft == NULL || lpszRight == NULL ) return -1;
	if( !Match( lpszLeft, lpszRight ) && !Match( lpszRight, lpszLeft ) )
	{
		if( IsCross( lpszLeft, lpszRight ) ) return -2;
		return -1;
	}
	if( Match( lpszLeft, lpszRight ) && Match( lpszRight, lpszLeft ) )
	{
		return 0;
	}
	if( Match( lpszLeft, lpszRight ) && !Match( lpszRight, lpszLeft ) )
	{
		return 1;
	}
	if( !Match( lpszLeft, lpszRight ) && Match( lpszRight, lpszLeft ) )
	{
		return 2;
	}
	return -1;
}

BOOL CHvResultFilter::IsCross( LPCSTR lpszLeft, LPCSTR lpszRight )
{
	if( lpszLeft == NULL || lpszRight == NULL ) return FALSE;

	int iPos = 0;
	while( lpszLeft[iPos] != 0 && lpszRight[iPos] != 0 )
	{
		if( lpszLeft[iPos] != '?' && lpszLeft[iPos] != '*'&& lpszRight[iPos] != '?'&& lpszRight[iPos] != '*'
		  &&lpszLeft[iPos] != '%' && lpszLeft[iPos] != '@'&& lpszRight[iPos] != '%'&& lpszRight[iPos] != '@'
		  )
		{
			if( lpszLeft[iPos] != lpszRight[iPos] ) return FALSE;
		}
		++iPos;
	}

	return TRUE;
}

//检查时间
BOOL CHvResultFilter::CheckRuleTimer( const CHvString& strComment, const CRuleTime& crtime )
{
	BOOL fRet = TRUE;
	CHvString strCmt = strComment;

	if( strCmt.IsEmpty() ) return fRet;

	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );

	int iPos = strCmt.Find("[TIME:");
	if( iPos == -1 ) return fRet;
	int iMutriPos = strCmt.Find("_");

	iPos  += 6;
	BOOL fLastTimes = TRUE;
	do
	{
		int lH,lM,lS,rH,rM,rS;
		lH = lM = lS = rH = rM = rS = 0;
		int iFiled = 0;
		if(iMutriPos == -1 && iPos > -1)
		{
			iFiled = sscanf( strCmt.MakeUpper().Mid(iPos).GetBuffer(), "%d:%d:%d,%d:%d:%d]",
				&lH, &lM, &lS, &rH, &rM, &rS );
			fLastTimes = FALSE;
		}
		else if( iMutriPos > -1 && iPos > -1)
		{
			iFiled = sscanf( strCmt.MakeUpper().Mid(iPos).GetBuffer(), "%d:%d:%d,%d:%d:%d_",
				&lH, &lM, &lS, &rH, &rM, &rS );
		}
		if( iFiled != 6 ) return fRet;

		CRuleTime crtLeft;
		crtLeft.SetTime(lH, lM, lS);
		CRuleTime crtRight;
		crtRight.SetTime(rH, rM, rS);

		fRet = CRuleTime::IsCourseOfTime( crtLeft, crtRight, crtime);
		if(fRet)
			break;
		iPos = iMutriPos;
		if(iMutriPos > 0)
		{
			iPos += 1;
			iMutriPos = strCmt.Find("_", iMutriPos + 1);
			fLastTimes = TRUE;
		}
	}while(fLastTimes);

	return fRet;
}

//检查规则的可信度条件是否附合
BOOL  CHvResultFilter::CheckRuleRely( const CHvString& strComment, const CHvString& strPlateInfo )
{
	BOOL fRet = TRUE;
	CHvString strCmt = strComment;
	CHvString strInfo = strPlateInfo;

	if( strCmt.IsEmpty() || strInfo.IsEmpty() )
	{
		return fRet;
	}

	int iAve = -1;
	int iWord = -1;
	char szTmp[256] = {0};
	memset(szTmp, 0, 256);
	int iPos = strInfo.Find( "平均可信度:" );
	int iPosPoint = -1;
	if( iPos != -1 )
	{
		iPos += (int)strlen("平均可信度:");
		iPosPoint = strInfo.Find(".", iPos);
		if( iPosPoint != -1 )
		{
			if( 1 != sscanf( strInfo.Mid(iPosPoint+1, 2), "%d", &iAve ) )
			{
				iAve = -1;
			}
			else
			{
				if( strInfo[iPosPoint -1] == '1' )
				{
					iAve += 100;
				}
			}
		}
	}
	iPos = -1;
	iPosPoint = -1;
	iPos = strInfo.Find( "首字可信度:" );
	if( iPos != -1 )
	{
		iPos += (int)strlen("首字可信度:");
		iPosPoint = strInfo.Find(".", iPos);
		if( iPosPoint != -1 )
		{
			if( 1 != sscanf( strInfo.Mid(iPosPoint+1, 2), "%d", &iWord ) )
			{
				iWord = -1;
			}
			else
			{
				if( strInfo[iPosPoint -1] == '1' )
				{
					iWord += 100;
				}
			}
		}
	}

	if( iAve == -1 && iWord == -1 )
	{
		return fRet;
	}

	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );

	iPos = strCmt.MakeUpper().Find("[RELY:");
	if( -1 == iPos )
	{
		return fRet;
	}

	int iAveSet = -1;
	int iWordSet = -1;

	int iPosNext = strCmt.MakeUpper().Find( "P:", iPos );
	if( -1 != iPosNext )
	{
		sscanf( strCmt.MakeUpper().Mid( iPosNext ), "P:%d", &iAveSet );
	}
	iPosNext = strCmt.MakeUpper().Find("W:", iPos );
	if( -1 != iPosNext )
	{
		sscanf( strCmt.MakeUpper().Mid(iPosNext), "W:%d", &iWordSet );
	}

	if( iAveSet != -1 && iAve != -1 && iAveSet > iAve
		&&( iWordSet == -1 || iWord == -1 ) )
	{
		return fRet;
	}

	if( iWord != -1 && iWordSet != -1 && iWordSet > iWord 
		&& (iAve == -1 || iAveSet ==-1) )
	{
		return fRet;
	}

	if( iWord != -1 && iWordSet != -1 && iAve != -1 && iAveSet != -1 
		&& iWordSet > iWord && iAveSet > iAve )
	{
		return fRet;
	}

	return FALSE;
}

//检查车牌类型是否符合
//车牌类型
// 0:普通单层牌, 1:双层牌, 2:摩托车牌
BOOL  CHvResultFilter::CheckPlateType( const CHvString& strComment, const CHvString& strPlateInfo )
{
	BOOL fRet = TRUE;
	CHvString strCmt = strComment;
	CHvString strInfo = strPlateInfo;

	if( strCmt.IsEmpty() || strInfo.IsEmpty() )
	{
		return fRet;
	}

	//是否有设置车牌类型
	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );
	int iPos = strCmt.Find("[PLATETYPE:");
	int iSetType = 0;
	if( iPos == -1 || 1 != sscanf(strCmt.Mid(iPos), "[PLATETYPE:%d]", &iSetType) )
	{
		return fRet;
	}

	//识别结果的车牌类型
	int iNowType = 0;
	iPos = strInfo.Find("车牌类型:");
	if( iPos == -1 )
	{
		iNowType = 0;
	}
	else if( strncmp(strInfo.Mid(iPos + 9), "双", 2) == 0 )
	{
		iNowType = 1;
	}
	else if( strncmp(strInfo.Mid(iPos + 9), "摩", 2) == 0 )
	{
		iNowType = 2;
	}

	if( iSetType != iNowType )
	{
		fRet = FALSE;
	}

	return fRet;
}

//检查车速条件
BOOL CHvResultFilter::CheckPlateSpeed( const CHvString& strComment, const CHvString& strPlateInfo )
{
	BOOL fRet = TRUE;
	CHvString strCmt = strComment;
	CHvString strInfo = strPlateInfo;

	if( strCmt.IsEmpty() || strInfo.IsEmpty() )
	{
		return fRet;
	}

	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );

	int iPos = strCmt.MakeUpper().Find("[SPEED:");
	int iLow = -1;
	int iHigh = -1;
	if( iPos != -1 )
	{
		int iPosNext = strCmt.MakeUpper().Find("LS:", iPos);
		if( iPosNext != -1 )
		{
			sscanf(strCmt.MakeUpper().Mid(iPosNext), "LS:%d", &iLow);
		}
		iPosNext = strCmt.MakeUpper().Find("HS:", iPos);
		if( iPosNext != -1 )
		{
			sscanf(strCmt.MakeUpper().Mid(iPosNext), "HS:%d", &iHigh);
		}
	}
	else
	{
		return fRet;
	}

	if( iLow == -1 && iHigh == -1 )
	{
		return fRet;
	}

	//查找车速
	iPos = strInfo.Find( "车速" );
	int iSpeed = -1;
	if( iPos != -1 )
	{
		sscanf(strInfo.Mid(iPos), "车速:%d", &iSpeed);
	}
	else
	{
		iPos = strInfo.Find("视频测速");
		if( iPos != -1 )
		{
			sscanf(strInfo.Mid(iPos), "视频测速:%d", &iSpeed);
		}
	}

	if( iSpeed == -1 )
	{
		return fRet;
	}

	if( iLow != -1 && iHigh == -1 && iLow > iSpeed )
	{
		fRet = FALSE;
	}
	if( iLow == -1 && iHigh != -1 && iHigh < iSpeed )
	{
		fRet = FALSE;
	}
	if( iLow != -1 && iHigh != -1 && (iLow > iSpeed || iHigh < iSpeed) )
	{
		fRet = FALSE;
	}

	return fRet;
}
//检查逆行条件
BOOL CHvResultFilter::CheckReverseRun( const CHvString& strComment, const CHvString& strPlateInfo )
{
	BOOL fRet = TRUE;
	CHvString strCmt = strComment;
	CHvString strInfo = strPlateInfo;

	if( strCmt.IsEmpty() || strInfo.IsEmpty() )
	{
		return fRet;
	}

	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );

	int iPos = strCmt.MakeUpper().Find("[DIRECT:");
	int iValue = -1;
	if( iPos != -1 )
	{
		sscanf(strCmt.Mid(iPos), "[DIRECT:%d", &iValue);
	}
	else
	{
		return fRet;
	}
	
	if(iValue != 1)
	{
		fRet = FALSE;
		return fRet;
	}
	int iCarPos = strInfo.Find("车辆逆向行驶:是");
	if(iCarPos == -1)
	{
		fRet = FALSE;
	}	
	
	return fRet;
}
BOOL CHvResultFilter::CheckRoadNumber( const CHvString& strComment, const CHvString& strPlateInfo )
{
	BOOL fRet = TRUE;
	CHvString strCmt = strComment;
	CHvString strInfo = strPlateInfo;

	if( strCmt.IsEmpty() || strInfo.IsEmpty() )
	{
		return fRet;
	}

	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );

	int iPos = strCmt.MakeUpper().Find("[ROAD:");
	int iValue = -1;
	if( iPos != -1 )
	{
		sscanf(strCmt.Mid(iPos), "[ROAD:%d", &iValue);
	}
	int iCarPos = strInfo.Find("车道:");
	int iRoad = -1;
	if( iCarPos != -1 )
	{
		sscanf(strInfo.Mid(iCarPos), "车道:%d", &iRoad);
	}
	if(iValue != iRoad && iValue != -1 && iRoad != -1)
	{
		fRet = FALSE;
	}
	
	return fRet;
}
//检测规则
CHvString CHvResultFilter::CheckRule( CHvString& strLeft, CHvString& strRight,
		    CHvString& strStyle)
{
	CHvString str, strLine;
	CHvString strRet;
	switch( RuleCompare( strLeft.GetBuffer(), strRight.GetBuffer()) )
	{
	case -1:
		break;

	case 0:
		str.Format( "\"%s\"等于\"%s\"", strLeft.GetBuffer(), strRight.GetBuffer() );
		strLine.Format( "%-20s%-32s%-35s\n", strStyle.GetBuffer(), str.GetBuffer(), "未处理" );
		strRet.Append( strLine );
		break;

	case 1:
		str.Format( "\"%s\"包含\"%s\"", strLeft.GetBuffer(), strRight.GetBuffer());
		strLine.Format( "%-20s%-32s%-35s\n", strStyle.GetBuffer(), str.GetBuffer(), "未处理" );
		strRet.Append( strLine );
		break;

	case 2:
		str.Format( "\"%s\"包含\"%s\"", strRight.GetBuffer(), strLeft.GetBuffer() );
		strLine.Format( "%-20s%-32s%-35s\n", strStyle.GetBuffer(), str.GetBuffer(), "未处理" );
		strRet.Append( strLine );
		break;

	case -2:
		str.Format( "\"%s\"与\"%s\"相冲突", strLeft.GetBuffer(), strRight.GetBuffer() );
		strLine.Format( "%-20s%-32s%-35s\n", strStyle.GetBuffer(), str.GetBuffer(), "未处理" );
		strRet.Append( strLine );
		break;

	default:
		break;
	}
	return strRet;
}

//生成检查规则报告
void CHvResultFilter::MakeReport( LPSTR lpszReport, int iLen )
{
	//检查规则的合法性
	return;
}
/*
HRESULT GetLeachData( CParamStore* pParam, BYTE8* pBuffer, DWORD32* pcbData )
{
	CFastMemAlloc cStack;
	char* pszBuf = (char*)cStack.StackAlloc( MAX_STRING_LEN, FALSE );
	if (pszBuf == NULL) return E_OUTOFMEMORY;
	HV_memset(pszBuf, 0, MAX_STRING_LEN);
	HV_memset(pBuffer, 0, *pcbData);

	HRESULT hr = pParam->GetString(
		"HvDsp\\FilterRule", "Leach", 
		(char*)pszBuf, MAX_STRING_LEN,
		"过滤规则","",5
		);


	if(hr == S_OK) 
	{
		*pcbData = (strlen((char*)pszBuf) + 1 > *pcbData) ? *pcbData : (strlen((char*)pszBuf));
		HV_memcpy(pBuffer, pszBuf, *pcbData);
	}
	else
	{
		*pcbData = 0;	
	}

	pParam->Save();

	return hr;
}

HRESULT GetReplaceData( CParamStore* pParam, BYTE8* pBuffer, DWORD32* pcbData )
{
	CFastMemAlloc cStack;
	char* pszBuf = (char*)cStack.StackAlloc( MAX_STRING_LEN, FALSE );
	if (pszBuf == NULL) return E_OUTOFMEMORY;
	HV_memset(pszBuf, 0, MAX_STRING_LEN);
	HV_memset(pBuffer, 0, *pcbData);

	HRESULT hr = pParam->GetString( 
		"HvDsp\\FilterRule", "Replace", 
		(char*)pszBuf, MAX_STRING_LEN,
		"替换规则","",5
		);

	if(hr == S_OK)
	{
		*pcbData = (strlen((char*)pszBuf) + 1 > *pcbData) ? *pcbData : (strlen((char*)pszBuf));
		HV_memcpy(pBuffer, pszBuf, *pcbData);
	}
	else
	{
		*pcbData = 0;
	}

	pParam->Save();

	return hr;
}

HRESULT GetCompatyData( CParamStore* pParam, BYTE8* pBuffer, DWORD32* pcbData )
{
	CFastMemAlloc cStack;
	char* pszBuf = (char*)cStack.StackAlloc( MAX_STRING_LEN, FALSE );
	if (pszBuf == NULL) return E_OUTOFMEMORY;
	HV_memset(pszBuf, 0, MAX_STRING_LEN);
	HV_memset(pBuffer, 0, *pcbData);

	HRESULT hr = pParam->GetString( 
		"HvDsp\\FilterRule", "Compaty", 
		(char*)pszBuf, MAX_STRING_LEN,
		"通配符规则","",5
		);

	if(hr == S_OK)
	{
		*pcbData = (strlen((char*)pszBuf) + 1 > *pcbData) ? *pcbData : (strlen((char*)pszBuf));
		HV_memcpy(pBuffer, pszBuf, *pcbData);
	}
	else
	{
		*pcbData = 0;
	}

	pParam->Save();

	return hr;
}
*/
