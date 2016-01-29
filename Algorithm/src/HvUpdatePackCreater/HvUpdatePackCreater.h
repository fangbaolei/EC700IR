#ifndef _HVUPDATEPACKCREATER_H_
#define _HVUPDATEPACKCREATER_H_

#include <stdio.h>

#ifdef WIN32

#include <Windows.h>
#include <string.h>
#include <sys/stat.h>
#include <io.h>

#define strcasecmp stricmp
#define fsync fflush

#define mmap mmap_win32
#define munmap munmap_win32
#define caddr_t unsigned char *

#define PROT_READ 0
#define PROT_WRITE 0
#define MAP_SHARED 0

// 在Windows下模拟Linux中的mmap和munmap函数。
void* mmap_win32(void *start, size_t length, int prot, int flags, int fd, off_t offset);
int munmap_win32(void *start, size_t length);

// 在Windows下模拟Linux中的文件操作相关函数。
int open(const char *, int, ...);
int read(int, void *, unsigned int);
int write(int, const void *, unsigned int);
int close(int);
int fstat(int, struct stat *);

#endif

int ExistFile(char* szFileName);

//
extern int HvCryptFile(char* szInFile, char* szOutFile);
extern int HvEncryptFile(char* szInFile, char* szOutFile);
extern int HvEncryptFile_DSP(char* szInFile, char* szOutFile);
extern int HvMakeImage(char* szExeName, char* szArgument);

extern int CryptoByMAC_File(char* szInFile, char* szOutFile, unsigned char* keyMAC);

#endif
