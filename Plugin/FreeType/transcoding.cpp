#include "transcoding.h"
#include "__gb2unicode__.h"

/** 
 * utf8字符串转unicode字符串
 * 
 * @param unicode  unicode字符串
 * @param utf8     utf8字符串
 * @param len      utf8字符串长度
 * 
 * @return 返回转换后的unicode字符串长度(1个字符占2字节)
 *
 * @note 
 *         1. 所有字符的unicode编码为2字节？？
 */
int utf8_to_unicode(unsigned short* unicode, const char* utf8, const int len)
{
    unsigned short uc=0;
    int need=0, i, p=0;
    unsigned char ch;

    for (i=0; i<len; i++)
    {
        ch = utf8[i];
        if (need)
        {
            if ((ch&0xc0) == 0x80)
            {
                uc = (uc << 6) | (ch & 0x3f);
                need--;
                if ( !need )
                {
                    if(p >= (len - 1))
                        break;
                    unicode[p++] = uc;
                }
            }
            else
            {
                unicode[p++] = 0xfffd;
                need = 0;
            }
        }
        else
        {
            if (ch < 0x80)  // ascii code
            {
                unicode[p++] = ch;
            }
            else if ((ch&0xe0) == 0xc0) // 2 bytes
            {
                uc = ch &0x1f;
                need = 1;
            }
            else if ((ch&0xf0) == 0xe0) // 3 bytes
            {
                uc = ch &0x0f;
                need = 2;
            }
        }
    }

    return p;
}

int utf8_to_unicode1(unsigned short *unicode, const char* utf8, int len)
{
    int i, j;
    unsigned short ch;

    for ( i=0, j=0; i < len; ++i, ++j ) {
        ch = ((const unsigned char *)utf8)[i];
        if ( ch >= 0xF0 ) {
            ch  =  (unsigned short)(utf8[i]&0x07) << 18;
            ch |=  (unsigned short)(utf8[++i]&0x3F) << 12;
            ch |=  (unsigned short)(utf8[++i]&0x3F) << 6;
            ch |=  (unsigned short)(utf8[++i]&0x3F);
        } else
        if ( ch >= 0xE0 ) {
            ch  =  (unsigned short)(utf8[i]&0x0F) << 12;
            ch |=  (unsigned short)(utf8[++i]&0x3F) << 6;
            ch |=  (unsigned short)(utf8[++i]&0x3F);
        } else
        if ( ch >= 0xC0 ) {
            ch  =  (unsigned short)(utf8[i]&0x1F) << 6;
            ch |=  (unsigned short)(utf8[++i]&0x3F);
        }
        unicode[j] = ch;
    }
    //unicode[j] = 0;   // 用该语句时，当len为10或18时，malloc出错，为何?

    return j;
}

int ascii_to_unicode(unsigned short *unicode, const char *text, int len)
{
    int i;

    for ( i=0; i < len; ++i ) {
        unicode[i] = ((const unsigned char *)text)[i];
    }

    return i;
}
/** 
 * gb2312字符串转unicode字符串
 * 
 * @param unicode  unicode字符串
 * @param utf8     gb2312字符串
 * @param len      gb2312字符串长度
 * 
 * @return 返回转换后的unicode字符串长度(1个字符占2字节)
 *
 * @note 
 *         1. 所有字符的unicode编码为2字节？？
 *         2. 必须使用unsigned char进行移位计算，否则结果不正确
 */
int gb2312_to_unicode(unsigned short int* unicode, const char* gb, int len)
{
    int i,j;
    i = 0;
    unsigned char* gb_temp = (unsigned char *)gb;   // 必须转换成无符号

    for(j = 0; i < len; j++)
    {
        if (gb_temp[i] <= 0x80)
        {
            unicode[j] = gb_temp[i];
            i++;
        }
        else
        {
            unsigned short int temp;
            // old
            //temp = (gb_temp[i + 1] << 8) + gb_temp[i];
            // new way
            temp = (gb_temp[i] << 8) + gb_temp[i+1];
            temp -= 0x8140; // 查询表从0x8140开始存储，故减去此数
            unicode[j] = giGB2312[temp];
            i += 2;
        }
    }

    return j;
}

//int unicode2gb(char*gb,unsigned short int*unicode,int len)
//{
//    int j;
//    int i;
//    unsigned short iTmp;
//    len=len/2;
//    j=0;
//    for(i=0;i<len;i++)
//    {
//        if (!(ntohs(unicode[i])&0xff00))
//        {
//            gb[j]=ntohs(unicode[i]);
//            j++;
//        }
//        else
//        {
//            iTmp=getgb(unicode[i]);
//            /*   printf(" 0x%04X,0x%04X",unicode[i],iTmp);*/
//            *(unsigned short int*)(gb+j)=iTmp;
//            j+=2;
//        }
//    }
//    return j;
//}
