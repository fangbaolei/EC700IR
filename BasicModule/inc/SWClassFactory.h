#ifndef __SW_CLASS_FACTORY_H__
#define __SW_CLASS_FACTORY_H__
#include "SWLog.h"
/**
 *@brief 定义一个创建其他类的基类
 */
class CSWCreateClass : public CSWObject
{
	CLASSINFO(CSWCreateClass, CSWObject)
public: 
	CSWCreateClass();	
	virtual ~CSWCreateClass();
	/**
	 *@brief 取得对象名称
	 */
	virtual LPCSTR GetObjectName(void);
	/**
	 *@brief 创建对象
	 */
	virtual CSWObject* CreateObject(const char *szclsName);
};

/**
 *@brief 类工厂
 */
class CSWClassFactory : public CSWCreateClass
{
	CLASSINFO(CSWClassFactory, CSWCreateClass)
public:	
	CSWClassFactory();
	virtual ~CSWClassFactory();
	/**
	 *@brief 根据类名称创建对象
	 */
	virtual CSWObject *CreateObject(LPCSTR lpzclsName);
	/**
	 *@brief 将类添加到类工厂中
	 */
	HRESULT Add(CSWCreateClass *pClass);
private:	
	CSWList<CSWCreateClass *>m_lstClass;
};
extern CSWClassFactory theClassFactory;

/**
 *@brief 注册类到类工厂中
 */
#define REGISTER_CLASS(cls)\
	class cls##_Factory_Registration : public CSWCreateClass\
	{\
		CLASSINFO(cls##_Factory_Registration, CSWCreateClass)\
	public:\
		cls##_Factory_Registration()\
		{\
			theClassFactory.Add(this);\
		}\
		virtual ~cls##_Factory_Registration()\
		{\
		}\
		virtual LPCSTR GetObjectName(void)\
		{\
			return #cls;\
		}\
		virtual CSWObject* CreateObject(const char *szclsName)\
		{\
			return new cls;\
		}\
		VOID Print(VOID)\
		{\
			SW_TRACE_DEBUG("load class %s...\n", #cls);\
		}\
	}cls##_Factory_Registration_Instance;\
	void cls##_Factory_Registration_Initialize()\
	{\
		extern cls##_Factory_Registration cls##_Factory_Registration_Instance;\
		cls##_Factory_Registration* pTmp = &cls##_Factory_Registration_Instance;\
		theClassFactory.Add(pTmp);\
		pTmp->Print();\
	}

/**
 *@brief 加载类到类工厂
 */
#define LOAD_CLASS(cls) extern void cls##_Factory_Registration_Initialize(); cls##_Factory_Registration_Initialize();

/**
 *@brief 创建对象
 */	
#define CREATE_OBJECT(clsName) theClassFactory.CreateObject(clsName);

/**
 *@brief 创建Filter对象
 */
#define CREATE_FILTER(clsName) (CSWBaseFilter *)CREATE_OBJECT(clsName) 

/**
 *初始化类工厂
 */
void ClassInitialize();
#endif
