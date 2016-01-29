#ifndef __PROCESS_RULE_H__
#define __PROCESS_RULE_H__
#define MAX_OUTPUT_BUF_LEN 32*1024
/**
* @brief 结果后处理模块参数结构体
*/
typedef struct _ProcessRule
{
    //识别器端配置文件上，\\HvDsp\\FilterRule下相应子键的键值
    CHAR szCompatyRule[MAX_OUTPUT_BUF_LEN];
    CHAR szReplaceRule[MAX_OUTPUT_BUF_LEN];
    CHAR szLeachRule[MAX_OUTPUT_BUF_LEN];
    BOOL fCompaty;
    BOOL fReplace;
    BOOL fLoopReplace;
    BOOL fLeach;
	BOOL fOutPutFilterInfo;		// 输出后处理信息

    _ProcessRule()
    {
        swpa_strcpy(szCompatyRule, "NULL");
        swpa_strcpy(szReplaceRule, "NULL");
        swpa_strcpy(szLeachRule, "NULL");
        fCompaty = TRUE;
        fReplace = TRUE;
        fLoopReplace = TRUE;
        fLeach = TRUE;
		fOutPutFilterInfo = FALSE;
    };

} PROCESS_RULE;  //后处理规则
#endif
