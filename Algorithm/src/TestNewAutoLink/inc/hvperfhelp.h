#ifndef _HV_PERF_HELP
#define _HV_PERF_HELP

#include "HvInterface.h"

class CHvPerfHelp
{
public:
	CHvPerfHelp();
	~CHvPerfHelp();

	HRESULT Start(  HvCore::IHvPerformance* pHvPerf, LPCSTR szSessionName );

private:
	HvCore::IHvPerformance* m_pHvPerf;
	char m_szSessionName[128];

	BOOL m_fStartOK;
};

#endif
