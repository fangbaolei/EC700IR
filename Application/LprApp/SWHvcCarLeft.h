#ifndef __SW_TOLL_GATE_CARLEFT_H__
#define __SW_TOLL_GATE_CARLEFT_H__
#include "SWCarLeft.h"
class CSWHvcCarLeft: public CSWCarLeft
{
CLASSINFO(CSWHvcCarLeft, CSWCarLeft)
public:
    CSWHvcCarLeft();
    virtual ~CSWHvcCarLeft();

protected:
    virtual HRESULT GetEventDetInfo(CSWString& strInfo);
    virtual HRESULT BuildPlateString(TiXmlDocument& xmlDoc);
    virtual CSWString BuildPlateString(VOID);

    HRESULT SetTollEvasionFlag(const BOOL fEvasion)
    {
        m_fTollEvasion = fEvasion;
        return S_OK;
    }

    HRESULT SetTollEvasionTick(const DWORD dwTick)
    {
        m_dwTollEvasionTick = dwTick;
        return S_OK;
    }

     //×Ô¶¯»¯Ó³Éäºê
    SW_BEGIN_DISP_MAP(CSWHvcCarLeft, CSWHvcCarLeft)
        SW_DISP_METHOD(SetTollEvasionFlag, 1)
        SW_DISP_METHOD(SetTollEvasionTick, 1)
    SW_END_DISP_MAP();
private:
    BOOL m_fTollEvasion;
    BOOL m_dwTollEvasionTick;
    
};
REGISTER_CLASS(CSWHvcCarLeft)
#endif
