// 该文件编码格式必须是WIN936
//

#ifndef  _CAMERACMDPROCESS_H_
#define _CAMERACMDPROCESS_H_

#include "HvCameraLink.h"

using namespace HiVideo;

//...
typedef int ( *CAMERA_CMD_FUNC )( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink);

typedef struct tag_CameraCmdFunc
{
    DWORD32 dwCmdID;
    CAMERA_CMD_FUNC fpCmdFunc;
}
CAMERA_CMD_MAP_ITEM;

extern CAMERA_CMD_MAP_ITEM g_CameraCmdFuncMap[];

//
class CCameraCmdProcess : public ICameraCmdProcess
{
public:
    // ICamCmdProcess
    virtual HRESULT Process( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
    static HRESULT CreateICameraCmdProcess(ICameraCmdProcess** ppICameraCmdProcess);

public:
    CCameraCmdProcess();
    ~CCameraCmdProcess();

    HRESULT Initialize(CAMERA_CMD_MAP_ITEM* pCmdMap, const int& iItemCount);
    HRESULT ExecuteCommand(CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink);

private:
    CAMERA_CMD_MAP_ITEM* m_pCmdMap;
    int m_iCmdCount;
};

#endif
