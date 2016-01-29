#ifndef _SWOBJBASE_INCLUDED__
#define _SWOBJBASE_INCLUDED__

//#include "config.h"
//#include "swbasetype.h"

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


#define MIN_INT( int1, int2 )	( ( int1 ) < ( int2 ) ? ( int1 ) : ( int2 ) )
#define MAX_INT( int1, int2 )	( ( int1 ) > ( int2 ) ? ( int1 ) : ( int2 ) )

#endif // _SWOBJBASE_INCLUDED__


