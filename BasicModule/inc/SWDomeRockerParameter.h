#ifndef _SWDOMEROCKERPARAMETER_H_
#define _SWDOMEROCKERPARAMETER_H_

typedef struct _tagDomeRockerParam
{
    BOOL fRockerEnable; // 使能标志
	INT iBaudrate;      // 波特率
	INT iProtocol;      // 协议(目前只支持pelco-d)

    _tagDomeRockerParam()
    {
        fRockerEnable = FALSE;
		iBaudrate = 9600;
        iProtocol = 0;
    }
}DOMEROCKER_PARAM;

#endif // _SWDOMEROCKERPARAMETER_H_
