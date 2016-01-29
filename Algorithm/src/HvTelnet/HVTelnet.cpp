/*
 * HVTelnet.c
 *
 *  Created on: May 9, 2011
 *      Author: ganzz
 */
#include "TelnetSvr.h"
#include "Log.h"
#include <stdio.h>
#include <stdarg.h>

#include "HVTelnet.h"

int TelnetInit()
{
    return TelnetSvr::GetInstance()->Start(NULL);
}

void TelnetUnInit()
{
    TelnetSvr::GetInstance()->Close();
}

void TelnetOutputDataString(int nLevel, char* fmt, ...)
{
    static char buf[16384] = {0};

    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    Log* pLog = TelnetSvr::GetInstance()->GetLog();
    pLog->Write(nLevel, buf, 0);
}
