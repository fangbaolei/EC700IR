#include "StdAfx.h"
#include "seh_exception_base.h"

seh_exception_base::seh_exception_base(void)
{
}

void seh_exception_base::trans_func( unsigned int u, EXCEPTION_POINTERS* pExp ) 
{ 

	std:string strTemp;
	char chTemp[32] = { 0 };
	switch(pExp->ExceptionRecord->ExceptionCode) 
	{ 

	case EXCEPTION_ACCESS_VIOLATION : 
		{
			sprintf_s(chTemp, "异常代码:%d", pExp->ExceptionRecord->ExceptionCode);
			strTemp = " 存储保护异常 ";
			strTemp+=chTemp;
			throw seh_exception_access_violation(pExp, strTemp); 
		}
		break; 

	case EXCEPTION_INT_DIVIDE_BY_ZERO : 
		{
			sprintf_s(chTemp, "异常代码:%d", pExp->ExceptionRecord->ExceptionCode);
			strTemp = " 存储保护异常 ";
			strTemp+=chTemp;
			throw seh_exception_divide_by_zero(pExp, strTemp); 
		}
		break; 

	case EXCEPTION_INVALID_HANDLE : 
		{
			sprintf_s(chTemp, "异常代码:%d", pExp->ExceptionRecord->ExceptionCode);
			strTemp = " 存储保护异常 ";
			strTemp+=chTemp;
			throw seh_exception_invalid_handle(pExp, strTemp); 
		}
		break; 
	default : 
		{
			sprintf_s(chTemp, "异常代码:%d", pExp->ExceptionRecord->ExceptionCode);
			strTemp = " 存储保护异常 ";
			strTemp+=chTemp;
			throw seh_exception_base(pExp, strTemp); 
		}
		

		break; 

	} 

} 

