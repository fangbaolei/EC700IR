#ifndef _ALLOC_H
#define _ALLOC_H

#include <new>

namespace signalway
{

template<typename T>
class allocator
{
public : 
	// typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef unsigned int size_type;
	typedef int difference_type;

public : 
	// convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef allocator<U> other;
	};

public : 
	inline explicit allocator() {}
	inline ~allocator() {}
	inline explicit allocator(allocator const&) {}
	template<typename U>
	inline explicit allocator(allocator<U> const&) {}

	// address
	inline pointer address(reference r) { return &r; }
	inline const_pointer address(const_reference r) { return &r; }

	// memory allocation
	inline pointer allocate(size_type cnt) 
	{ 
		return (pointer)(::operator new(cnt * sizeof (T))); 
	}
	inline void deallocate(pointer p, size_type) 
	{ 
		::operator delete(p); 
	}

	//    size
	inline size_type max_size() const 
	{ 
		return size_type(-1) ;
	}

	//    construction/destruction
	inline void construct(pointer p, const T& t) { new(p) T(t); }
	inline void destroy(pointer p) { p->~T(); }

	inline bool operator==(allocator const&) { return true; }
	inline bool operator!=(allocator const& rhs) { return !operator==(rhs); }
} ; // end of class allocator


} // signalway

#endif // _ALLOC_H