#pragma once

using namespace std; 

#include <eh.h> 
#include <string> 
#include <exception> 



class seh_exception_base :
	public std::exception 
{
public:
	seh_exception_base(void);

	seh_exception_base(const PEXCEPTION_POINTERS pExp, std::string what )
		:m_ExceptionRecord(*pExp->ExceptionRecord)
		,m_ContextRecord(*pExp->ContextRecord)
		,m_what(what)
	{

	}


	~seh_exception_base(void) throw() {};

	virtual const char* what() const throw() 
	{ 
		return m_what.c_str(); 
	} 

	virtual DWORD exception_code() const throw() 
	{ 
		return m_ExceptionRecord.ExceptionCode; 
	} 

	virtual const EXCEPTION_RECORD& get_exception_record() const throw() 
	{ 
		return m_ExceptionRecord; 
	} 

	virtual const CONTEXT& get_context() const throw() 
	{ 
		return m_ContextRecord; 
	} 

	// 初始化函数 
	static void initialize_seh_trans_to_ce() 
	{ 
		_set_se_translator( trans_func ); 
	} 
	static void trans_func(unsigned int u, EXCEPTION_POINTERS *pExp);

protected:
	std::string m_what;
	EXCEPTION_RECORD m_ExceptionRecord;
	CONTEXT m_ContextRecord;
};

//////////////////////////////////////////////////////////////////////////////// 

// 下面是系统异常被转换后的 C++ 类型的异常 

// 篇幅有限，因此只简单设计了对几种常见的系统异常的转换 

//////////////////////////////////////////////////////////////////////////////// 

class seh_exception_access_violation : public seh_exception_base 

{ 

public: 

	seh_exception_access_violation (const PEXCEPTION_POINTERS pExp, std::string what) 

		: seh_exception_base(pExp, what) {}; 

	~seh_exception_access_violation() throw(){}; 

}; 

//////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////// 

class seh_exception_divide_by_zero : public seh_exception_base 

{ 

public: 

	seh_exception_divide_by_zero (const PEXCEPTION_POINTERS pExp, std::string what) 

		: seh_exception_base(pExp, what) {}; 

	~seh_exception_divide_by_zero() throw(){}; 

}; 

//////////////////////////////////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////////////////////////// 

class seh_exception_invalid_handle : public seh_exception_base 

{ 

public: 

	seh_exception_invalid_handle (const PEXCEPTION_POINTERS pExp, std::string what) 

		: seh_exception_base(pExp, what) {}; 

	~seh_exception_invalid_handle() throw(){}; 

}; 


