#ifndef  _PARAM_INFO_H_
#define _PARAM_INFO_H_

typedef struct {
char* chIniParamName;	//INI参数名
char* chIniSectionName;	//INI参数段
char* chXmlParamName;	//XML参数名
char* chXmlSectionName;	//XML参数段
char* chParamDefault;	//默认值
char* chParamMin;		//最小值
char* chParamMax;		//最大值
char* chParamChineseName;//中文名
char* chParamContext;	//注释
int iParamRank;			//级别
char* chType;			//参数类型
} INI_PARAM_INFO;

typedef struct{
char* pszParentName;	//父结点
char* pszEnglishName;	//英文名
char* pszChineseName;	//对应的中文名
}EN_TO_CH;


extern INI_PARAM_INFO rgIniParamInfo[];
extern EN_TO_CH rgstrEnToCh[],rgstrEnToChEPolice[];

#endif