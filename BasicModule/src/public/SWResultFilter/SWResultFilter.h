//该文件编码格式必须为WINDOWS-936格式

#ifndef  _INCLUDE_HVRESULTFILTER_H_
#define  _INCLUDE_HVRESULTFILTER_H_
#include "SWBaseFilter.h"
#include "ProcessRule.h"
#include "SWMessage.h"

//全角转半角
BOOL SBC2DBC( LPCSTR lpszSrc, LPSTR lpszDest );

//规则时间格式
class CRuleTime
{
public:
    CRuleTime();
    ~CRuleTime();

public:
    BOOL SetTime( const INT iHour, const INT iMinute, const INT iSecond );
    BOOL SetTime( LPCSTR lpszTime );

    INT GetHour() const
    {
        return m_iHour;
    }
    INT GetMinute() const
    {
        return m_iMinute;
    }
    INT GetSecond() const
    {
        return m_iSecond;
    }

    BOOL operator == (const CRuleTime& crt) const;
    BOOL operator > (const CRuleTime& crt) const;
    BOOL operator < (const CRuleTime& crt) const;
    BOOL operator >= (const CRuleTime& crt) const;
    BOOL operator <= (const CRuleTime& crt) const;

public:
    //时间 crtNow 是否是在 crtLeft 到 crtRight 区间里
    static BOOL IsCourseOfTime( const CRuleTime& crtLeft, const CRuleTime& crtRight, const CRuleTime& crtNow );

private:
    INT m_iHour;
    INT m_iMinute;
    INT m_iSecond;
};


///////////////////////////////////////////////////////////////////////////
//                           后处理类
/////////////////////////////////////////////////////////////////////////////////////////

//从lpszFileName读出一个小节lpszSection的内容到lpszOutStr, 结构为 key=value\0
//INT GetFileSection( LPCSTR lpszSection, LPSTR lpszOutStr, INT iLen, CParamStore* pParam, LPCSTR lpszFileName = NULL );
//得到当前时间
void GetTime( CRuleTime& crtime );

//车牌处理的相关信息
typedef struct ResultInfo
{
    CSWString strPlate;
    BOOL    fAlarm;
    BOOL    fLeach;
	INT		iLeachType;
	INT		iPT;
    CSWString strFolder;
    CSWString strOther;
    CSWString strApplied;
}RESULT_INFO;

//过滤规则结构
typedef struct LeachInfo
{
    CSWString strPattern;
    BOOL fLeach;
	INT iLeachType;
    CSWString strComment;
}LEACH_INFO;

//报警规则结构
typedef struct AlarmInfo
{
    CSWString strPattern;
    BOOL fAlarm;
    CSWString strComment;
}ALARM_INFO;

//分类目录的规则结构
typedef struct FolderInfo
{
    CSWString strPattern;
    CSWString strFolder;
    CSWString strComment;
}FOLDER_INFO;

//车牌替换的规则结构
typedef struct ReplaceInfo
{
    CSWString strPattern;
    CSWString strResult;
    CSWString strComment;
}REPLACE_INFO;

//通配符规则结构
typedef struct  CompatyWord
{
    CSWString strPattern;
    CSWString strResult;
}COMPATY_WORD;

class CSWResultFilter : public CSWBaseFilter, public CSWMessage
{
		CLASSINFO(CSWResultFilter, CSWBaseFilter)
public:
	  CSWResultFilter();
    CSWResultFilter(PROCESS_RULE *pRule);
    virtual ~CSWResultFilter(void);
		virtual HRESULT Receive(CSWObject* obj);
public:
    //初始化规则
    HRESULT InitRule(PVOID pProcessRule);

    //后处理
    BOOL FilterProcess(RESULT_INFO* pResultInfo);

private:
    //车牌替换
    BOOL ReplacePlate( CSWString& strPlate, const BOOL& fLoopReplace, INT& iPT );
    void DoReplace(CSWString& strPlate, REPLACE_INFO& replaceinfo);
    //是否要过滤
    INT IsNeedLeach(CSWString& strPlate, INT& iPT);

    //规则库的初始化
    BOOL InitLeach();
    BOOL InitReplace();
    BOOL InitCompaty();

    //检查规则的时效性
    BOOL CheckRuleTimer( const CSWString& strComment, const CRuleTime& crtime );
    //检查规则的可信度条件是否附合
    BOOL CheckRuleRely( const CSWString& strComment, const CSWString& strPlateInfo );
    //检查车牌类型是否符合
    //车牌类型
    // 0:普通单层牌, 1:双层牌, 2:摩托车牌
    BOOL CheckPlateType( const CSWString& strComment, const CSWString& strPlateInfo );
    //检查车速条件
    BOOL CheckPlateSpeed( const CSWString& strComment, const CSWString& strPlateInfo );
    //检查逆行条件
    BOOL CheckReverseRun( const CSWString& strComment, const CSWString& strPlateInfo );
    //检查车道号条件
    BOOL CheckRoadNumber( const CSWString& strComment, const CSWString& strPlateInfo );
    //判断lpszPattern是否能推出lpszTarget
    BOOL Match( LPCSTR lpszPattern, LPCSTR lpszTarget );
    //判断两条规则是否交叉
    BOOL IsCross( LPCSTR lpszLeft, LPCSTR lpszRight );
    //生成检查规则报告
    void MakeReport( LPSTR lpszReport, INT iLen );
    //检测规则
    CSWString CheckRule( CSWString& strLeft, CSWString& strRight,
        CSWString& strStyle);
	//检查违章类型条件
	BOOL CheckPT( const CSWString& strComment, INT& iPT );

	HRESULT OnOverlayPosChanged(WPARAM wParam, LPARAM lParam);

public:
    //比较两条规则的包含关系, -1:两规则互不交叉,0:相等,1:包含,2:被包含,-2:交叉
    INT  RuleCompare( LPCSTR lpszLeft, LPCSTR lpszRight );
	SW_BEGIN_DISP_MAP(CSWResultFilter,CSWBaseFilter)
        SW_DISP_METHOD(InitRule, 1)
	SW_END_DISP_MAP()

	SW_BEGIN_MESSAGE_MAP(CSWResultFilter, CSWMessage)
	    SW_MESSAGE_HANDLER(MSG_RESULT_FILTER_OVERLAY_POS_CHANGED, OnOverlayPosChanged)
	SW_END_MESSAGE_MAP()
private:
    BOOL m_fInitialize;
    PROCESS_RULE *m_pProcessRule;
    //应用到的规则
    CSWString m_strApplied;
    //附加信息
    CSWString m_strOther;
    //最大规则数
    static const INT MAX_RULE_COUNT = 150;
    //规则库
    CSWList<LEACH_INFO> m_lstLeach;
    CSWList<REPLACE_INFO> m_lstReplace;
    CSWList<COMPATY_WORD> m_lstCompaty;
};
REGISTER_CLASS(CSWResultFilter)
#endif

