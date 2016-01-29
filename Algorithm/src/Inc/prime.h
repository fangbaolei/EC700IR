#ifndef __PRIME_H__
#define __PRIME_H__ 

#include "vlong.h"

class prime_factory
{
  public:
  UINT np;
  UINT *pl;
  prime_factory( UINT MP = 2000 ); // sieve size
  ~prime_factory();
  vlong find_prime( vlong & start );
  long make_prime( vlong & r, vlong &k, const vlong & rmin );
};

long is_probable_prime( const vlong &p );

#endif
