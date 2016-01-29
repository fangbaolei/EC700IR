#ifndef HVSTOREINFO_H_INCLUDED
#define HVSTOREINFO_H_INCLUDED

// 写入要存储的信息, 成功返回0, 失败返回-1
#define HvDebugStateInfo(szInfo) _HvDebugStateInfoWrite(__FILE__, __LINE__, szInfo)

//增加函数执行结果宏,黄国超增加,函数执行失败则写入黑盒子
//func 函数
//rtnType返回值类型:0-不返回;1-错误才返回,2-返回执行结果
#define WRITE_TO_BLACK_BOX_IF_FAILED(func, rtnType)\
{\
    HRESULT hr = func;\
    if (FAILED(hr))\
    {\
        CHvString s;\
        s.Format("%s=%d", #func, hr);\
        _HvDebugStateInfoWrite(__FILE__, __LINE__, s.GetBuffer());\
        if (rtnType == 1)\
        {\
            return hr;\
        }\
    }\
    if(rtnType == 2)\
    {\
        return hr;\
    }\
}

#ifdef __cplusplus
extern "C" {
#endif

// 写入要存储的信息, 成功返回0, 失败返回-1
int _HvDebugStateInfoWrite(const char* szFile, int nCodeLine, const char* szInfo);

// 读取存储的信息, 成功返回0, 失败返回-1
int HvDebugStateInfoReadAll(char* szBuf, int* piInfoLen);

#ifdef __cplusplus
}
#endif

#endif // HVSTOREINFO_H_INCLUDED

