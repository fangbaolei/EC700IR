// SWFC宏定义处
#ifndef _SWFC_DEFINE_
#define _SWFC_DEFINE_

#include "SWConfig.h"
#include "swpa.h"


#ifdef SWFC_DEBUG_ARG_ENABLE
    #define SWFC_DEBUG_ARG(fmt, ...) printf("%s[%d]%s"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);
#else
    #define SWFC_DEBUG_ARG(fmt, ...)
#endif

#ifdef SWFC_DEBUG_ERR_ENABLE
    #define SWFC_DEBUG_ERR(fmt, ...) printf("%s[%d]%s"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);
#else
    #define SWFC_DEBUG_ERR(x)
#endif

#if defined(WIN32)
#define IsDecendant(cls, ptr) (NULL != dynamic_cast<cls *>ptr)
#else
#define IsDecendant(cls, ptr) (ptr && (ptr)->IsDescendant(cls::GetID()) ? (cls *)ptr : NULL)
#endif

#define CLASSNAME(cls)             #cls
#define CLASSID(cls)               cls::GetID()
#define CLASSIDBYNAME(clsName)     CSWUtils::CalcCrc32(0, (PBYTE)clsName, (UINT)swpa_strlen(clsName))

#define CLASSINFO(cls, par) \
    public: \
    static LPCSTR Class() { return #cls; } \
    static DWORD GetID(){static DWORD id = 0;if(0==id){id=CLASSIDBYNAME(#cls);} return id;}\
    virtual LPCSTR Name(){return Class();}\
    virtual DWORD ID(){return GetID();}\
    virtual bool IsDescendant(const char * clsName) const \
    { \
        return swpa_strcmp(clsName, cls::Class()) == 0 || (swpa_strcmp(cls::Class(),par::Class()) && par::IsDescendant(clsName)); \
    }\
    virtual bool IsDescendant(DWORD id) const \
    { \
    		return id == cls::GetID() || cls::GetID() != par::GetID() && par::IsDescendant(id);\
    }



#define REGISTER_CLASS_LISTEN(cls) DWORD g_dwRef##cls = 0;
#define CLASS_LISTEN_ADD(cls)     {extern DWORD g_dwRef##cls; g_dwRef##cls++;}
#define CLASS_LISTEN_RELEASE(cls) {extern DWORD g_dwRef##cls; g_dwRef##cls--;}
#define CLASS_LISTEN_PRINT(cls)   {extern DWORD g_dwRef##cls; printf("%s:%d\n", #cls, g_dwRef##cls);}

#define SAFE_ADDREF(obj)  {if(NULL != (obj)){(obj)->AddRef();}}
#define SAFE_RELEASE(obj) {if(NULL != (obj)){(obj)->Release(); (obj) = NULL;}}

#ifndef SAFE_MEM_FREE
#define SAFE_MEM_FREE(ptr)							\
	if (NULL != ptr)								\
	{												\
		swpa_mem_free((ptr));						\
		(ptr) = NULL;								\
	}
#endif


#ifndef SAFE_DELETE
#define SAFE_DELETE(pObj)							\
	if (pObj)										\
	{												\
		delete pObj;								\
		pObj = NULL;								\
	}
#endif


//RGB增益宏
#define SW_RGB(r,g,b) (((DWORD)(r))|(((DWORD)(g))<<8)|(((DWORD)(b))<<16))
#define SW_R(rgb) ((BYTE)rgb)
#define SW_G(rgb) ((BYTE)((rgb & 0x0000FF00) >> 8))
#define SW_B(rgb) ((BYTE)((rgb & 0x00FF0000) >> 16))

//字节对齐宏
#define ALGIN_SIZE(size, algin) (((size) + (algin) - 1)/(algin)*(algin))

//自动化函数宏定义
#define INVOKE_0( ret, func)  ret = func()
#define INVOKE_1( ret, func)  ret = func(v1)
#define INVOKE_2( ret, func)  ret = func(v1,v2)
#define INVOKE_3( ret, func)  ret = func(v1,v2,v3)
#define INVOKE_4( ret, func)  ret = func(v1,v2,v3,v4)
#define INVOKE_5( ret, func)  ret = func(v1,v2,v3,v4,v5)
#define INVOKE_6( ret, func)  ret = func(v1,v2,v3,v4,v5,v6)
#define INVOKE_7( ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7)
#define INVOKE_8( ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8)
#define INVOKE_9( ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9)
#define INVOKE_10(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)
#define INVOKE_11(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11)
#define INVOKE_12(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12)
#define INVOKE_13(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13)
#define INVOKE_14(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14)
#define INVOKE_15(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15)
#define INVOKE_16(ret, func)  ret = func(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16)
#define INVOKE(n, ret, func)  INVOKE_##n(ret,func)
//自动化宏定义函数
#define SW_BEGIN_DISP_MAP(theClass, baseClass)\
virtual CSWVariant Invoke(LPCSTR szFunctionName,\
                          CSWVariant v1  = 0\
                         ,CSWVariant v2  = 0\
                         ,CSWVariant v3  = 0\
                         ,CSWVariant v4  = 0\
                         ,CSWVariant v5  = 0\
                         ,CSWVariant v6  = 0\
                         ,CSWVariant v7  = 0\
                         ,CSWVariant v8  = 0\
                         ,CSWVariant v9  = 0\
                         ,CSWVariant v10 = 0\
                         ,CSWVariant v11 = 0\
                         ,CSWVariant v12 = 0\
                         ,CSWVariant v13 = 0\
                         ,CSWVariant v14 = 0\
                         ,CSWVariant v15 = 0\
                         ,CSWVariant v16 = 0)\
{\
	CSWVariant varRet = E_NOTIMPL;\
	if(swpa_strcmp(#theClass, #baseClass))\
	{\
		varRet = baseClass::Invoke(szFunctionName, v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16);\
	}\
	if(varRet == E_NOTIMPL)\
	{\
		if(0){}
/**
 *@brief      函数映射
 *@param func 函数名称
 *@param n    参数个数
 */
#define SW_DISP_METHOD(func, n) else if(!swpa_strcmp(szFunctionName, #func)){INVOKE(n, varRet, func);}

/**
 *@brief      属性映射
 *@param prop 属性名称
 */
#define SW_DISP_PROPERTY(prop) else if(!swpa_strcmp(szFunctionName, #prop)){prop = v1; varRet = S_OK;}
/**
 *结束映射
 */
#define SW_END_DISP_MAP() }return varRet;}

#endif // _SWFC_DEFINE_

