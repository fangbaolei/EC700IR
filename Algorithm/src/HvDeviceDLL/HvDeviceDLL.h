#ifndef _HVDEVICEDLL_H_
#define _HVDEVICEDLL_H_

#include <stdio.h>
#include <time.h>
#include <tchar.h>

#ifndef _INC_WINDOWS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif // _INC_WINDOWS

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


// 源代码树公共头文件
#include "tinyxml.h"
#include "HvCameraLinkOpt.h"
#include "HvCameraLink.h"

// 本工程内部头文件
#include "HvDeviceUtils.h"
#include "HvDevice.h"

//#include "HvAutoLinkNew.h"

#endif // _HVDEVICEDLL_H_
