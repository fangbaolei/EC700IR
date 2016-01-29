#ifndef ENCODE_H__
#define ENCODE_H__

#ifdef __cplusplus
extern "C"
{

#endif

int GB2312ToUTF8( char const *srcStr, char* desBuff, int desBuffLength);

int UTF8ToGB2312( char const *srcStr, char* desBuff, int desBuffLength);

#ifdef __cplusplus
}; //end of extern "C" {
#endif

#endif //__ENCODE_H_
