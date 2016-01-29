////---------------------------------------------------------------------------
//
//#ifndef __CONSOLE_H__
//#define __CONSOLE_H__
//#define _WIN32_WINNT 0x0500	
//#include <stdio.h>
//#include <stdarg.h>
//#include <windows.h>
//class CConsole
//{
//public:
//                 CConsole();
//        virtual ~CConsole();
//        static bool Open(void);
//        static void Close(void);
//        static bool IsOpen(void){return m_nInitNum > 0;}
//        static bool SetLogFile(const char * fname);
//        static bool IsNewProcess(void){return m_bAttach;}
//        static bool Write(const char * fmt, ...);
//        static bool WriteV(const char *fmt, va_list & argptr);
//private:
//        static int        m_nInitNum;
//        static bool       m_bAttach;
//        static bool       m_bLogFile;
//        static char       m_szLogFile[256];
//        static CRITICAL_SECTION m_csLock;
//		static bool       m_bInitLock;
//};
////---------------------------------------------------------------------------
//#endif
 