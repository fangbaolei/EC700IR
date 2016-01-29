#ifndef _PRODUCT_NAME_H
#define _PRODUCT_NAME_H

#include "swBaseType.h"

extern HRESULT InitProductName();

extern HRESULT GetProductName( char* szName, UINT* pLen );

extern HRESULT SetProductName( const char* pbName , UINT nLen) ;

#endif
