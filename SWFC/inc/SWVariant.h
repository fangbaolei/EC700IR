#ifndef __SW_VARIANT_H__
#define __SW_VARIANT_H__
#include "SWBaseType.h"

#define VT_TYPE(t)  VT_##t
#define VT_VALUE(type) val##type
#define UNION(type)    type VT_VALUE(type)

typedef enum 
{
	VT_TYPE(NONE   ),
	VT_TYPE(CHAR   ),
	VT_TYPE(BYTE   ),
	VT_TYPE(WORD   ),
	VT_TYPE(DWORD  ),
	VT_TYPE(LONGLONG  ),
	VT_TYPE(INT    ),
	VT_TYPE(LONG   ),
	VT_TYPE(FLOAT  ),
	VT_TYPE(DOUBLE ),
	VT_TYPE(PBYTE  ),
	VT_TYPE(PWORD  ),
	VT_TYPE(PDWORD ),
	VT_TYPE(PINT   ),
	VT_TYPE(PLONG  ),
	VT_TYPE(PFLOAT ),
	VT_TYPE(PDOUBLE),
	VT_TYPE(PSTR   ),
	VT_TYPE(PVOID  ),
	VT_MAX
}SWVARTYPE;

//构造函数
#define CONSTRUCT(type)             CSWVariant(type VT_VALUE(type)){this->vt = VT_TYPE(type); this->VT_VALUE(type) = VT_VALUE(type);}

//赋值函数
#define OPERATOR_ASSIGN(type)      CSWVariant & operator =(const type & VT_VALUE(type)){this->vt = VT_##type; this->VT_VALUE(type)=VT_VALUE(type);return *this;}

//赋值函数
#define OPERATOR_OP(type,op)\
	virtual CSWVariant & operator op(const type & VT_VALUE(type))\
	{\
		switch(this->vt)\
		{\
		 case VT_TYPE(CHAR)  :this->VT_VALUE(type) op (type)VT_VALUE(CHAR);break;\
		 case VT_TYPE(BYTE)  :this->VT_VALUE(type) op (type)VT_VALUE(BYTE);break;\
		 case VT_TYPE(WORD)  :this->VT_VALUE(type) op (type)VT_VALUE(WORD);break;\
		 case VT_TYPE(DWORD) :this->VT_VALUE(type) op (type)VT_VALUE(DWORD);break;\
	 	 case VT_TYPE(LONGLONG) :this->VT_VALUE(type) op (type)VT_VALUE(LONGLONG);break;\
		 case VT_TYPE(INT)   :this->VT_VALUE(type) op (type)VT_VALUE(INT);break;\
		 case VT_TYPE(LONG)  :this->VT_VALUE(type) op (type)VT_VALUE(LONG);break;\
		 case VT_TYPE(FLOAT) :this->VT_VALUE(type) op (type)VT_VALUE(FLOAT);break;\
		 case VT_TYPE(DOUBLE):this->VT_VALUE(type) op (type)VT_VALUE(DOUBLE);break;\
		}\
		return *this;\
	}
	
//转换函数
#define OPERATOR_VAR(type)\
	virtual operator type()\
	{\
		type val = 0;\
		switch(this->vt)\
		{\
			case VT_TYPE(CHAR)  : val = (type)(this->VT_VALUE(CHAR));break;\
			case VT_TYPE(BYTE)  : val = (type)(this->VT_VALUE(BYTE));break;\
			case VT_TYPE(WORD)  : val = (type)(this->VT_VALUE(WORD));break;\
			case VT_TYPE(DWORD) : val = (type)(this->VT_VALUE(DWORD));break;\
			case VT_TYPE(LONGLONG) : val = (type)(this->VT_VALUE(LONGLONG));break;\
			case VT_TYPE(INT)   : val = (type)(this->VT_VALUE(INT));break;\
			case VT_TYPE(LONG)  : val = (type)(this->VT_VALUE(LONG));break;\
			case VT_TYPE(FLOAT) : val = (type)(this->VT_VALUE(FLOAT));break;\
			case VT_TYPE(DOUBLE): val = (type)(this->VT_VALUE(DOUBLE));break;\
	  }\
		return val;\
	}
	
//转换函数
#define OPERATOR_PVAR(type)\
	virtual operator type()\
	{\
		type val = 0;\
		switch(this->vt)\
		{\
			case VT_TYPE(PSTR)   : val = (type)(this->VT_VALUE(PSTR));break;\
			case VT_TYPE(PBYTE)  : val = (type)(this->VT_VALUE(PBYTE));break;\
			case VT_TYPE(PWORD)  : val = (type)(this->VT_VALUE(PWORD));break;\
			case VT_TYPE(PDWORD) : val = (type)(this->VT_VALUE(PDWORD));break;\
			case VT_TYPE(PINT)   : val = (type)(this->VT_VALUE(PINT));break;\
			case VT_TYPE(PLONG)  : val = (type)(this->VT_VALUE(PLONG));break;\
			case VT_TYPE(PFLOAT) : val = (type)(this->VT_VALUE(PFLOAT));break;\
			case VT_TYPE(PDOUBLE): val = (type)(this->VT_VALUE(PDOUBLE));break;\
			case VT_TYPE(PVOID)  : val = (type)(this->VT_VALUE(PVOID));break;\
	  }\
		return val;\
	}
	
//比较函数
#define OPERATOR_BOOL(type, op)\
	virtual bool operator op(const type & val)\
	{\
		switch(this->vt)\
		{\
		case VT_TYPE(CHAR)  : return val op (type)VT_VALUE(CHAR);break;\
		case VT_TYPE(BYTE)  : return val op (type)VT_VALUE(BYTE);break;\
		case VT_TYPE(WORD)  : return val op (type)VT_VALUE(WORD);break;\
		case VT_TYPE(DWORD) : return val op (type)VT_VALUE(DWORD);break;\
		case VT_TYPE(LONGLONG) : return val op (type)VT_VALUE(LONGLONG);break;\
		case VT_TYPE(INT)   : return val op (type)VT_VALUE(INT);break;\
		case VT_TYPE(LONG)  : return val op (type)VT_VALUE(LONG);break;\
		case VT_TYPE(FLOAT) : return val op (type)VT_VALUE(FLOAT);break;\
		case VT_TYPE(DOUBLE): return val op (type)VT_VALUE(DOUBLE);break;\
		}\
		return false;\
	}
	
//结构体
typedef struct tagSWVARIANT 
{
    SWVARTYPE vt;
    union 
    {
    	UNION(CHAR   );
        UNION(BYTE   );
        UNION(WORD   );
        UNION(DWORD  );
		UNION(LONGLONG);
        UNION(INT    );
        UNION(LONG   );
        UNION(FLOAT  );
        UNION(DOUBLE );
        UNION(PBYTE  );
        UNION(PWORD  );
        UNION(PDWORD );
        UNION(PINT   );
        UNION(PLONG  );
        UNION(PFLOAT );
        UNION(PDOUBLE);
        UNION(PSTR   );
        UNION(PVOID  );
    };
}SWVARIANT;

class CSWVariant : public SWVARIANT
{
public:
	CSWVariant(){this->vt = VT_NONE;}
	virtual ~CSWVariant(){}
	
  CONSTRUCT(CHAR   )
  CONSTRUCT(BYTE   )
  CONSTRUCT(WORD   )
  CONSTRUCT(DWORD  )
  CONSTRUCT(LONGLONG)
  CONSTRUCT(INT    )
  CONSTRUCT(LONG   )
  CONSTRUCT(FLOAT  )
  CONSTRUCT(DOUBLE )
  CONSTRUCT(PBYTE  )
  CONSTRUCT(PWORD  )
  CONSTRUCT(PDWORD )
  CONSTRUCT(PINT   )
  CONSTRUCT(PLONG  )
  CONSTRUCT(PFLOAT )
  CONSTRUCT(PDOUBLE)
  CONSTRUCT(PSTR   )
  CONSTRUCT(PVOID  )

  OPERATOR_ASSIGN(CHAR   )
  OPERATOR_ASSIGN(BYTE   )
  OPERATOR_ASSIGN(WORD   )
  OPERATOR_ASSIGN(DWORD  )
  OPERATOR_ASSIGN(LONGLONG)
  OPERATOR_ASSIGN(INT    )
  OPERATOR_ASSIGN(LONG   )
  OPERATOR_ASSIGN(FLOAT  )
  OPERATOR_ASSIGN(DOUBLE )
  OPERATOR_ASSIGN(PBYTE  )
  OPERATOR_ASSIGN(PWORD  )
  OPERATOR_ASSIGN(PDWORD )
  OPERATOR_ASSIGN(PINT   )
  OPERATOR_ASSIGN(PLONG  )
  OPERATOR_ASSIGN(PFLOAT )
  OPERATOR_ASSIGN(PDOUBLE)
  OPERATOR_ASSIGN(PSTR   )
  OPERATOR_ASSIGN(PVOID  )
  
  OPERATOR_BOOL(CHAR  , ==)
  OPERATOR_BOOL(BYTE  , ==)
  OPERATOR_BOOL(WORD  , ==)
  OPERATOR_BOOL(DWORD , ==)
  OPERATOR_BOOL(LONGLONG , ==)
  OPERATOR_BOOL(INT   , ==)
  OPERATOR_BOOL(LONG  , ==)
  OPERATOR_BOOL(FLOAT , ==)
  OPERATOR_BOOL(DOUBLE, ==)
  
  OPERATOR_BOOL(CHAR  , >)
  OPERATOR_BOOL(BYTE  , >)
  OPERATOR_BOOL(WORD  , >)
  OPERATOR_BOOL(DWORD , >)
  OPERATOR_BOOL(LONGLONG , >)
  OPERATOR_BOOL(INT   , >)
  OPERATOR_BOOL(LONG  , >)
  OPERATOR_BOOL(FLOAT , >)
  OPERATOR_BOOL(DOUBLE, >)

  OPERATOR_BOOL(CHAR  , >=)
  OPERATOR_BOOL(BYTE  , >=)
  OPERATOR_BOOL(WORD  , >=)
  OPERATOR_BOOL(DWORD , >=)
  OPERATOR_BOOL(LONGLONG , >=)
  OPERATOR_BOOL(INT   , >=)
  OPERATOR_BOOL(LONG  , >=)
  OPERATOR_BOOL(FLOAT , >=)
  OPERATOR_BOOL(DOUBLE, >=)

  OPERATOR_BOOL(CHAR  , <)
  OPERATOR_BOOL(BYTE  , <)
  OPERATOR_BOOL(WORD  , <)
  OPERATOR_BOOL(DWORD , <)
  OPERATOR_BOOL(LONGLONG , <)
  OPERATOR_BOOL(INT   , <)
  OPERATOR_BOOL(LONG  , <)
  OPERATOR_BOOL(FLOAT , <)
  OPERATOR_BOOL(DOUBLE, <)

  OPERATOR_BOOL(CHAR  , <=)
  OPERATOR_BOOL(BYTE  , <=)
  OPERATOR_BOOL(WORD  , <=)
  OPERATOR_BOOL(DWORD , <=)
  OPERATOR_BOOL(LONGLONG , <=)
  OPERATOR_BOOL(INT   , <=)
  OPERATOR_BOOL(LONG  , <=)
  OPERATOR_BOOL(FLOAT , <=)
  OPERATOR_BOOL(DOUBLE, <=)
  
  OPERATOR_OP(CHAR   , +=)
  OPERATOR_OP(BYTE   , +=)
  OPERATOR_OP(WORD   , +=)
  OPERATOR_OP(DWORD  , +=)
  OPERATOR_OP(LONGLONG  , +=)
  OPERATOR_OP(INT    , +=)
  OPERATOR_OP(LONG   , +=)
  OPERATOR_OP(FLOAT  , +=)
  OPERATOR_OP(DOUBLE , +=)
  
  OPERATOR_OP(CHAR   , -=)
  OPERATOR_OP(BYTE   , -=)
  OPERATOR_OP(WORD   , -=)
  OPERATOR_OP(DWORD  , -=)
  OPERATOR_OP(LONGLONG  , -=)
  OPERATOR_OP(INT    , -=)
  OPERATOR_OP(LONG   , -=)
  OPERATOR_OP(FLOAT  , -=)
  OPERATOR_OP(DOUBLE , -=)
  
  OPERATOR_OP(CHAR   , *=)
  OPERATOR_OP(BYTE   , *=)
  OPERATOR_OP(WORD   , *=)
  OPERATOR_OP(DWORD  , *=)
  OPERATOR_OP(LONGLONG  , *=)
  OPERATOR_OP(INT    , *=)
  OPERATOR_OP(LONG   , *=)
  OPERATOR_OP(FLOAT  , *=)
  OPERATOR_OP(DOUBLE , *=)
  
  OPERATOR_OP(CHAR   , /=)
  OPERATOR_OP(BYTE   , /=)
  OPERATOR_OP(WORD   , /=)
  OPERATOR_OP(DWORD  , /=)
  OPERATOR_OP(LONGLONG  , /=)
  OPERATOR_OP(INT    , /=)
  OPERATOR_OP(LONG   , /=)
  OPERATOR_OP(FLOAT  , /=)
  OPERATOR_OP(DOUBLE , /=)
  
  OPERATOR_VAR(CHAR    )
  OPERATOR_VAR(BYTE    )
  OPERATOR_VAR(WORD    )
  OPERATOR_VAR(DWORD   )
  OPERATOR_VAR(LONGLONG   )
  OPERATOR_VAR(INT     )
  OPERATOR_VAR(LONG    )
  OPERATOR_VAR(FLOAT   )
  OPERATOR_VAR(DOUBLE  )
  OPERATOR_PVAR(PBYTE  )
  OPERATOR_PVAR(PWORD  )
  OPERATOR_PVAR(PDWORD )
  OPERATOR_PVAR(PINT   )
  OPERATOR_PVAR(PLONG  )
  OPERATOR_PVAR(PFLOAT )
  OPERATOR_PVAR(PDOUBLE)
  OPERATOR_PVAR(PSTR   )
  OPERATOR_PVAR(PVOID  )
};
#endif
