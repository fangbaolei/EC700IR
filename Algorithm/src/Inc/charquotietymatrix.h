#ifndef _ICHARQUOTIETYMATRIX_H_
#define _ICHARQUOTIETYMATRIX_H_

//字符权值矩阵接口
class ICharQuotietyMatrix
{
public:
	STDMETHOD( Initialize )() = 0;
	STDMETHOD( GetQuotiety )( int iCharIndex, int iCharPos, float &fltQuotiety ) = 0;
};

#endif
