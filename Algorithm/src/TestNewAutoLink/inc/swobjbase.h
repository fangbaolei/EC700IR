#ifndef _SWOBJBASE_INCLUDED__
#define _SWOBJBASE_INCLUDED__

#include "config.h"
#include "swBaseType.h"

#ifdef STDMETHODCALLTYPE 
	#undef STDMETHODCALLTYPE 
#endif

#ifdef STDMETHOD 
	#undef STDMETHOD
#endif

#if RUN_PLATFORM == PLATFORM_WINDOWS
#define STDMETHODCALLTYPE __stdcall
#else
#define STDMETHODCALLTYPE
#endif

#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method

#ifndef interface
#define interface struct
#endif

#endif // _SWOBJBASE_INCLUDED__

