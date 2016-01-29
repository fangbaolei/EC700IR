#ifndef  _INCLUDE_HVRESULTFILTER_H_
#define  _INCLUDE_HVRESULTFILTER_H_

#include "swbasetype.h"
#include "hvvartype.h"
#include "hvutils.h"
#include "hvparamstore.h"

#if RUN_PLATFORM == PLATFORM_DSP_BIOS //DSP
#include "hvdsptime.h"
#endif

#if RUN_PLATFORM == PLATFORM_WINDOWS
#include <vector>
#include <atlstr.h>
#include <atltime.h>
using namespace std;
#endif

//全角转半角
bool SBC2DBC( LPCSTR lpszSrc, LPSTR lpszDest );

//规则时间格式
class CRuleTime
{
public:
	CRuleTime();
	~CRuleTime();

public:
	bool SetTime( const int iHour, const int iMinute, const int iSecond );
	bool SetTime( LPCSTR lpszTime );
	
	int GetHour() const
	{
		return m_iHour;
	}
	int GetMinute() const
	{
		return m_iMinute;
	}
	int GetSecond() const 
	{
		return m_iSecond;
	}

	bool operator == (const CRuleTime& crt) const;
	bool operator > (const CRuleTime& crt) const;
	bool operator < (const CRuleTime& crt) const;
	bool operator >= (const CRuleTime& crt) const;
	bool operator <= (const CRuleTime& crt) const;

public:
	//时间 crtNow 是否是在 crtLeft 到 crtRight 区间里
	static bool IsCourseOfTime( const CRuleTime& crtLeft, const CRuleTime& crtRight, const CRuleTime& crtNow );

private:
	int m_iHour;
	int m_iMinute;
	int m_iSecond;
};


///////////////////////////////////////////////////////////////////////////
//                           后处理类
/////////////////////////////////////////////////////////////////////////////////////////

//从lpszFileName读出一个小节lpszSection的内容到lpszOutStr, 结构为 key=value\0
int GetFileSection( LPCSTR lpszSection, LPSTR lpszOutStr, int iLen, CParamStore* pParam, LPCSTR lpszFileName = NULL );
//得到当前时间
void GetTime( CRuleTime& crtime );

//车牌处理的相关信息
typedef struct ResultInfo
{
	CHvString strPlate;
	BOOL    fAlarm;
	BOOL    fLeach;
	CHvString strFolder;
	CHvString strOther;
	CHvString strApplied;
}RESULT_INFO;

//过滤规则结构
typedef struct LeachInfo
{
	CHvString strPattern;
	BOOL fLeach;
	CHvString strComment;
}LEACH_INFO;

//报警规则结构
typedef struct AlarmInfo
{
	CHvString strPattern;
	BOOL fAlarm;
	CHvString strComment;
}ALARM_INFO;

//分类目录的规则结构
typedef struct FolderInfo
{
	CHvString strPattern;
	CHvString strFolder;
	CHvString strComment;
}FOLDER_INFO;

//车牌替换的规则结构
typedef struct ReplaceInfo
{
	CHvString strPattern;
	CHvString strResult;
	CHvString strComment;
}REPLACE_INFO;

//通配符规则结构
typedef struct  CompatyWord
{
	CHvString strPattern;
	CHvString strResult;
}COMPATY_WORD;

class CHvResultFilter
{
public:
	CHvResultFilter(void);
	~CHvResultFilter(void);

public:
	//初始化规则
	BOOL InitRule( CParamStore* pParam, const char* strFilePath );

	//后处理
	BOOL FilterProcess( RESULT_INFO* pResultInfo, const BOOL& fReplace = TRUE,
		const BOOL& fAlarm = TRUE, const BOOL& fLeach = TRUE, const BOOL& fLoopReplace = FALSE );

private:
	//车牌替换
	BOOL ReplacePlate( CHvString& strPlate, const BOOL& fLoopReplace = FALSE );
	void DoReplace(CHvString& strPlate, REPLACE_INFO& replaceinfo);
	//分类目录
	BOOL GetPlateFolder( CHvString& strPlate, CHvString& strFolder );
	//是否要过滤
	BOOL IsNeedLeach(CHvString& strPlate);
	//是否要报警
	BOOL IsNeedAlarm(CHvString& strPlate);

	//规则库的初始化
	BOOL InitLeach();
	BOOL InitAlarm();
	BOOL InitFolder();
	BOOL InitReplace();	
	BOOL InitCompaty();

	//检查规则的时效性
	BOOL CheckRuleTimer( const CHvString& strComment, const CRuleTime& crtime );
	//检查规则的可信度条件是否附合
	BOOL CheckRuleRely( const CHvString& strComment, const CHvString& strPlateInfo );
	//检查车牌类型是否符合
	//车牌类型
	// 0:普通单层牌, 1:双层牌, 2:摩托车牌
	BOOL CheckPlateType( const CHvString& strComment, const CHvString& strPlateInfo );
	//检查车速条件
	BOOL CheckPlateSpeed( const CHvString& strComment, const CHvString& strPlateInfo );
	//检查逆行条件
	BOOL CheckReverseRun( const CHvString& strComment, const CHvString& strPlateInfo );

	//判断lpszPattern是否能推出lpszTarget
	BOOL Match( LPCSTR lpszPattern, LPCSTR lpszTarget );
	//判断两条规则是否交叉
	BOOL IsCross( LPCSTR lpszLeft, LPCSTR lpszRight );
	//生成检查规则报告
	void MakeReport( LPSTR lpszReport, int iLen );
	//检测规则
	CHvString CheckRule( CHvString& strLeft, CHvString& strRight,
		CHvString& strStyle);

public:
	//比较两条规则的包含关系, -1:两规则互不交叉,0:相等,1:包含,2:被包含,-2:交叉
	int  RuleCompare( LPCSTR lpszLeft, LPCSTR lpszRight );

private:
	BOOL m_fInitialize;
	CHvString m_strFilePath;
	CParamStore* m_pParam;
	//应用到的规则
	CHvString m_strApplied;
	//附加信息
	CHvString m_strOther;
	//最大规则数
	static const int MAX_RULE_COUNT = 150;
	//规则库
	CHvList<LEACH_INFO, MAX_RULE_COUNT> m_lstLeach;
	CHvList<ALARM_INFO, MAX_RULE_COUNT> m_lstAlarm;
	CHvList<REPLACE_INFO, MAX_RULE_COUNT> m_lstReplace;
	CHvList<FOLDER_INFO, MAX_RULE_COUNT> m_lstFolder;
	CHvList<COMPATY_WORD, MAX_RULE_COUNT> m_lstCompaty;
};

extern HRESULT GetLeachData( CParamStore* pParam, BYTE8* pBuffer, DWORD32* pcbData );
extern HRESULT GetReplaceData( CParamStore* pParam, BYTE8* pBuffer, DWORD32* pcbData );
extern HRESULT GetCompatyData( CParamStore* pParam, BYTE8* pBuffer, DWORD32* pcbData );


#endif
