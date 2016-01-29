#ifndef CAMTRANSMIT_H_INCLUDED
#define CAMTRANSMIT_H_INCLUDED

#include "hvutils.h"
#include "TcpipCfg.h"
#include "CamTransmitBase.h"
#include "hvthreadbase.h"

#define TRANSMIT_TYPE_CAM_NULL  0
#define TRANSMIT_TYPE_CAM_CY    1
#define TRANSMIT_TYPE_CAM_NVC   2

HRESULT StartCamTransmit(int nCamType, char* pszCamIP, TcpipParam cTcpipCfgParam1, TcpipParam cTcpipCfgParam2);

HRESULT ChangeCamTransmit(DWORD dwLAN2IP, DWORD dwLAN2Mask);

void StopCamTransmit();

/* CamTransmit.cpp */
extern ICamTransmit* g_pCamTransmit;

#endif // CAMTRANSMIT_H_INCLUDED
