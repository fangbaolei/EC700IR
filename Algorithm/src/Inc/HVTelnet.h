/*
 * HVTelnet.h
 *
 *  Created on: May 9, 2011
 *      Author: root
 */

#ifndef HVTELNET_H_
#define HVTELNET_H_

//////////////////////////////
// functions
//////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

// 以独立线程运行Telnet服务器,错误返回0
int TelnetInit();

// 结束Telnet服务器线程
void TelnetUnInit();

// 最大支持 2048 byte 格式化后的字符串长度
void TelnetOutputDataString(int nLevel, char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* HVTELNET_H_ */
