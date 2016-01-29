#include "HvUpdatePackCreater.h"

#ifdef WIN32

#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif

void* mmap_win32(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
	// 注：fd为已经打开的可读文件描述符。

	if ( NULL==start && 0==offset && length>0 )
	{
		char* pbBuf = (char*)malloc(length);
		int bytes = read(fd, pbBuf, length);
		if ( length == bytes )
		{
			return (void*)pbBuf;
		}
		else
		{
			free(pbBuf);
		}
	}
	return (void*)MAP_FAILED;
}

int munmap_win32(void *start,size_t length)
{
	if ( start && length>0 )
	{
		free(start);
		return 0;
	}
	return -1;
}

#include <fcntl.h>

int open(const char *filename, int mode, ...)
{
	if ( O_RDONLY == mode || O_RDONLY|O_BINARY == mode )
	{
		return fopen(filename, "rb");
	}
	else if ( mode&O_RDWR )
	{
		return fopen(filename, "wb+");
	}

	return -1;
}

int read(int fd, void * buf, unsigned int len)
{
	return fread(buf, 1, len, fd);
}

int write(int fd, const void * buf, unsigned int len)
{
	return fwrite(buf, 1, len, fd);
}

int close(int fd)
{
	return fclose(fd);
}

int fstat(int fd, struct stat * s)
{
	// 注：fd为已经打开的可读文件描述符。

	int st_size = 0;

	fseek(fd, 0, SEEK_END);
	st_size = ftell(fd);
	s->st_size = st_size;
	fseek(fd, 0, SEEK_SET);

	return 0;
}

#endif

extern int main_mkimage (int argc, char **argv);
int HvMakeImage(char* szExeName, char* szArgument)
{
    char* argv[17+1] = {NULL};
    char table[17][256];
    int i = 0;

    char *token = NULL;
    token = strtok(szArgument, " ");
    while( token != NULL )
    {
        strcpy(table[i++], token);
        token = strtok(NULL, " ");
    }

    if ( 17 != i )
    {
        exit(-1); // szArgument参数非法，程序退出。
    }

    argv[0] = szExeName;
    for ( i=0; i<17; ++i )
    {
        argv[i+1] = table[i];
    }

    // 去掉 -n 'name' 这个选项中name两边的'号。
    if ( '\'' == argv[14][0] )
    {
        char* szName = argv[14];
        int iLen = strlen(szName);
        char szNameOld[256];
        strcpy(szNameOld, szName);
        memcpy(szName, szNameOld+1, iLen-2);
        memset(szName+iLen-2, 0, 1);
    }

    return main_mkimage(18, &argv);
}

int ExistFile(char* szFileName)
{
    int iFounded = -1;
    FILE* fp = fopen(szFileName, "rb");
    if ( fp )
    {
        iFounded = 0;
        fclose(fp);
    }
    return iFounded;
}
