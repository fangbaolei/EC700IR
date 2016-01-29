#ifndef __VLONG_H__
#define __VLONG_H__

#include "swbasetype.h"

#ifndef NULL
#define NULL 0
#endif

// Macros for doing double precision multiply
#define BPU ( 8*sizeof(UINT) )       // 一个字节位 UINT
#define lo(x) ( ((UINT)(x)) & (UINT)((((UINT)1)<<(BPU/2))-((UINT)1)) ) // lower half of UINT
#define hi(x) ( ((UINT)(x)) >> (BPU/2) )         // 生 1/2
#define lh(x) ( ((UINT)(x)) << (BPU/2) )         // 实施

// Provides storage allocation and index checking
class flex_unit
{
public:
	UINT n; // used units (read-only)
	flex_unit();
	~flex_unit();
	void clear(); // set n to zero
	UINT get( UINT i ) const;     // 获取 ith UINT
	void set( UINT i, UINT x );   // 设置 ith UINT
	void reserve( UINT x );           // storage hint

	// Time critical routine
	void fast_mul( flex_unit &x, flex_unit &y, UINT n );
//private: //lchen modi
	UINT * a; // array of units
	UINT z; // units allocated
};

class vlong_value : public flex_unit
{
public:
	UINT share; // share count, used by vlong to delay physical copying
	long is_zero() const;
	UINT bit( UINT i ) const;
	void setbit( UINT i );
	void clearbit( UINT i );
	UINT bits() const;
	long cf( vlong_value& x ) const;
	long product( vlong_value &x ) const;
	void shl();
	long  shr(); // result is carry
	void shr( UINT n );
	void add( vlong_value& x );
	void vlong_xor( vlong_value& x );
	void vlong_and( vlong_value& x );
	void subtract( vlong_value& x );
	void init( UINT x );
	void copy( vlong_value& x );
	UINT to_unsigned(); // Unsafe conversion to UINT
	vlong_value();
	void mul( vlong_value& x, vlong_value& y );
	void divide( vlong_value& x, vlong_value& y, vlong_value& rem );
};

class vlong // very long integer - can be used like long
{
public:
	// Standard arithmetic operators
	friend vlong operator +( const vlong& x, const vlong& y );
	friend vlong operator -( const vlong& x, const vlong& y );
	friend vlong operator *( const vlong& x, const vlong& y );
	friend vlong operator /( const vlong& x, const vlong& y );
	friend vlong operator %( const vlong& x, const vlong& y );
	friend vlong operator ^( const vlong& x, const vlong& y );
	friend vlong pow2( UINT n );
	friend vlong operator &( const vlong& x, const vlong& y );

	friend vlong operator <<( const vlong& x, UINT n );

	vlong& operator +=( const vlong& x );
	vlong& operator -=( const vlong& x );
	vlong& operator >>=( UINT n );

	// Standard comparison operators
	friend long operator !=( const vlong& x, const vlong& y );
	friend long operator ==( const vlong& x, const vlong& y );
	friend long operator >=( const vlong& x, const vlong& y );
	friend long operator <=( const vlong& x, const vlong& y );
	friend long operator > ( const vlong& x, const vlong& y );
	friend long operator < ( const vlong& x, const vlong& y );

	// Absolute value
	friend vlong abs( const vlong & x );

	// Construction and conversion operations
	vlong ( UINT x=0 );
	vlong ( const vlong& x );
	~vlong();
	friend UINT to_unsigned( const vlong &x );
	vlong& operator =(const vlong& x);

	// Bit operations
	UINT bits() const;
	UINT bit(UINT i) const;
	void setbit(UINT i);
	void clearbit(UINT i);
	vlong& operator ^=( const vlong& x );
	vlong& operator &=( const vlong& x );
	vlong& ror( UINT n ); // single  bit rotate
	vlong& rol( UINT n ); // single bit rotate
	friend long product( const vlong & x, const vlong & y ); // parity of x&y

	void load( UINT * a, UINT n ); // 读值, a[0]
	void store( UINT * a, UINT n ) const; // low level save, a[0] is lsw

	UINT getLen(void);  //返回字节数
	void from_str( const char * s );
	void getBuffer(unsigned char **pbBuf,UINT* nSize);

private:
	class vlong_value * value;
	long negative;
	long cf( const vlong & x ) const;
	void docopy();
	friend class monty;
};

vlong modexp( const vlong & x, const vlong & e, const vlong & m ); // m 必须已添加

vlong gcd( const vlong &X, const vlong &Y ); // greatest common denominator
vlong modinv( const vlong &a, const vlong &m ); // modular inverse

vlong monty_exp( const vlong & x, const vlong & e, const vlong & m );
vlong monty_exp( const vlong & x, const vlong & e, const vlong & m, const vlong &p, const vlong &q );

class rng
{
public:
	virtual vlong next()=0;
};

class vlstr
{
public:
	virtual void put( const vlong & x )=0;
	virtual vlong get()=0;
};

vlong lucas ( vlong P, vlong Z, vlong k, vlong p ); // P^2 - 4Z != 0
vlong sqrt( vlong g, vlong p ); // 平方根的模 p


#endif// __VLONG_H__

