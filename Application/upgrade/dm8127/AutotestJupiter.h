/**
* @file
* @brief 
*
*/

#ifndef _AUTOTESTJUPITER_H_
#define _AUTOTESTJUPITER_H_

#include "SWThread.h"
#include "Autotest.h"

class CAutotestJupiter : public CAutotest
{
	//CLASSINFO(CAutotest, CSWApplication);
public:
	
    virtual HRESULT ReleaseObj();

    virtual HRESULT Initialize();

    virtual HRESULT AutotestRunOnce(_TEST_RESULT* sResult, DWORD& dwCount);

    virtual HRESULT Autotest();

};



#endif //_AUTOTESTJUPITER_H_

