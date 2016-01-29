#ifndef _CONTROLCOMMAND_H_
#define _CONTROLCOMMAND_H_

#include "hvutils.h"
#include "hvsysinterface.h"
#include "resultsend.h"

typedef int (*HV_CMD_FUNC)(HvSys::HV_CMD_INFO* pCmdInfo, HvSys::ICmdDataLink* pCmdLink);

typedef struct _CMD_MAP_ITEM
{
    DWORD32 dwCmdID;
    HV_CMD_FUNC fpCmdFunc;

} CMD_MAP_ITEM;

HRESULT InitCtrlCommand(
    IResultIO* pResultIO
);

HRESULT ExecuteCommand(
    HvSys::HV_CMD_INFO* pCmdInfo,
    HvSys::ICmdDataLink* pCmdLink
);

#endif
