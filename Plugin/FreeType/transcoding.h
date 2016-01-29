/**
 * @file   transcoding.h
 * @author
 * @date   Sat Jan 05 11:29:02 2013
 *
 * @brief  ×ªÂëº¯Êý
 *
 *
 */

#ifndef __TRANSCODING_H
#define __TRANSCODING_H

#ifdef __cplusplus
extern "C"{
#endif
int utf8_to_unicode(unsigned short* unicode, const char* utf8, int len);

int utf8_to_unicode1(unsigned short* unicode, const char* utf8, int len);

int ascii_to_unicode(unsigned short *unicode, const char *text, int len);

int gb2312_to_unicode(unsigned short int* unicode, const char* gb, int len);
#ifdef __cplusplus
}
#endif
#endif // __TRANSCODING_H
