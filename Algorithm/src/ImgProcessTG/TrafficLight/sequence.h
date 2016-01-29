#ifndef _SEQUENCE_H
#define _SEQUENCE_H

//#include <vector>
//#include <exception>

#include "alloc.h"

namespace signalway
{

template<typename T> inline
	void swap(T& left, T& right)
{	
	// exchange values stored at left and right
	T tmp = left;
	left = right, right = tmp;
}

template<typename T, typename Alloc = allocator<T> >
class sequence
{
public:
	// typedefs
	typedef T value_type ;
	typedef value_type* pointer ;
	typedef const value_type* const_pointer ;
	typedef value_type& reference ;
	typedef const value_type& const_reference ;
	typedef unsigned int size_type ;
	typedef int difference_type ;
	typedef value_type* iterator ;
	typedef const value_type* const_iterator ;


public:
	sequence() : first_(0), last_(0), end_of_storage_(0) { }
	sequence(size_type n, const T& value)
		: first_(0)
		, last_(0)
		, end_of_storage_(0)
	{
		//size_type cap = n + SPARE_CAPACITY ;
		//pointer ptr = new T[cap] ;

		//fill_n(ptr, n, value) ;

		//first_ = ptr ;
		//last_  = first_ + n ;
		//end_of_storage_ = first_ + cap ;

		resize(n, value) ;

	}

	sequence(size_type n)
		: first_(0)
		, last_(0)
		, end_of_storage_(0)
	{
		//size_type cap = n + SPARE_CAPACITY ;
		//pointer ptr = new T[cap] ; 

		//first_ = ptr ;
		//last_  = first_ + n ;
		//end_of_storage_ = first_ + cap ;
			
		resize(n) ;
	}

	sequence(const sequence<T>& rhs)
		: first_(0)
		, last_(0)
		, end_of_storage_(0)
	{
		// TODO: copy constructor
		size_type sz = rhs.size() ;
		pointer ptr = allocator_.allocate(sz) ;
		//try
		//{
			umove(rhs.first_, rhs.last_, ptr) ;
		//}
		//catch (std::exception& e)
		//{
		//	allocator_.deallocate(ptr, sz) ;
		//	throw e ;
		//}

		first_ = ptr ;
		last_  = ptr + sz ;
		end_of_storage_ = ptr + sz ;

	}

	~sequence()
	{
		destroy(first_, last_) ;
		allocator_.deallocate(first_, end_of_storage_-first_) ;
	}

public:
	iterator begin() { return first_ ; }
	const_iterator begin() const { return first_ ; }
	iterator end() { return last_ ; }
	const_iterator end() const { return last_ ; }

	size_type size() const { return size_type(end()-begin()) ; }
	size_type max_size() const { return size_type(-1) ; }
	size_type capacity() const { return size_type(end_of_storage_-begin()) ; }

	bool empty() const { return begin() == end() ; }
	reference operator[](size_type n) { return *(begin()+n) ; }
	const_reference operator[](size_type n) const { return *(begin()+n) ; }  // read only
	reference back() { return *(end()-1) ; }
	const_reference back() const { return *(end()-1) ; }  // read only

	void clear() { erase(first_, last_) ; }

	void reserve(size_type n)
	{
		if (max_size() < n)
		{
			xlen() ;
		}
		else if (capacity() < n)
		{
			pointer ptr = allocator_.allocate(n) ;

			//try
			//{
				umove(first_, last_, ptr) ;
			//}
			//catch (std::exception& e)
			//{
			//	allocator_.deallocate(ptr, n) ;
			//	throw e ;
			//}

			size_type sz = size() ;
			if (first_ != 0)
			{
				destroy(first_, last_) ;
				allocator_.deallocate(first_, size()) ;
			}

			first_ = ptr ;
			last_  = ptr + sz ;
			end_of_storage_ = ptr + n ;

		}
	}

	void resize(size_type n)
	{
		size_type oldSize = size() ;
		if (n < oldSize)
		{
			erase(first_+n, last_) ;
		}
		else if (oldSize < n)
		{
			_reserve(n - oldSize) ;
			construct_n(last_, n-oldSize, T()) ;
			last_ += n - oldSize ;
		}
	}

	void resize(size_type n, T val)
	{
		size_type oldSize = size() ;
		if (n < oldSize)
		{
			erase(first_+n, last_) ;
		}
		else if (oldSize < n)
		{
			_reserve(n - oldSize) ;
			construct_n(last_, n-oldSize, val) ;
			last_ += n - oldSize ;
		}
	}

	void push_back(const T& val)
	{
		if (last_ != end_of_storage_)
		{
			//*last_ = val ;
			construct_n(last_, 1, val) ;
			++last_ ;
		}
		else
		{
			resize(size()+1, val) ;
		}

	}

	iterator erase(const_iterator where)
	{
		move(where+1, last_, where) ;
		destroy(last_-1, last_) ;
		--last_ ;
		return make_iter(where) ;
	}

	iterator erase(const_iterator first, const_iterator last)
	{

		iterator firstIter = make_iter(first) ;
		iterator lastIter  = make_iter(last) ;

		if (firstIter != lastIter)
		{
			iterator ptr = move(lastIter, last_, firstIter) ;
			destroy(ptr, last_) ;
			last_ = ptr ;
		}
	
		return firstIter ;

	}

	sequence<T, Alloc>& operator=(const sequence<T, Alloc>& right)
	{
		if (this != &right)
		{	
			// worth doing
			if (right.size() == 0)
				clear();	// new sequence empty, free storage
			else if (right.size() <= size())
			{	
				// enough elements, copy new and destroy old
				pointer ptr = copy(right.first_, right.last_,
					first_);	// copy new
				destroy(ptr, last_);	// destroy old
				last_ = first_ + right.size();
			}
			else if (right.size() <= capacity())
			{	
				// enough room, copy and construct new
				pointer ptr = right.first_ + size();
				copy(right.first_, ptr, first_);
				last_ = umove(ptr, right.last_, last_);
			}
			else
			{	// not enough room, allocate new array and construct new
				if (first_ != 0)
				{	
					// discard old array
					destroy(first_, last_);
					this->allocator_.deallocate(first_, end_of_storage_ - first_);
				}
				if (buy(right.size()))
					last_ = umove(right.first_, right.last_, first_);
			}
		}
		return (*this);
	}

	void swap(sequence<T, Alloc>& right)
	{
		if (this->allocator_ == right.allocator_)
		{
			// same allocator, swap control information
			signalway::swap(first_, right.first_);
			signalway::swap(last_, right.last_);
			signalway::swap(end_of_storage_, right.end_of_storage_);
		}
		else
		{
			sequence<T, Alloc> ts = *this; *this = right, right = ts;
		}

	}

private:
	void fill_n(iterator& dest, size_type n, const T& val)
	{

		iterator iter = dest ;

		while (n--)
		{
			*iter++ = val ;
		}

	}

	iterator copy(iterator first, iterator last, iterator& dest)
	{

		iterator iter = first ;
		iterator outIter = dest ;
		while (iter != last)
		{
			*outIter = *iter ;

			++iter ;
			++outIter ;
		} // end of while (iter ! = last)

		return outIter ;

	}

	iterator umove(iterator first, iterator last, pointer dest)
	{
		iterator outIter = dest ;
		for (iterator iter = first; iter != last; ++iter, ++outIter)
		{
			allocator_.construct(outIter, *iter) ;
		}

		return outIter ;

	}

	iterator move(iterator first, iterator last, pointer dest)
	{
		return copy(first, last, dest) ;
	}

	size_type grow_to(size_type n) const
	{
		int cap = capacity() ;
		cap = max_size() - cap / 2 < cap ? 0 : cap + cap / 2 ;  // try to grow by 50%
		if (cap < n)
		{
			cap = n ;
		}

		return cap ;
	}

	void _reserve(size_type n)
	{
		size_type sz = size() ;
		if (max_size() - n < sz)
		{
			xlen() ;
		}

		sz += n ;

		if (sz > capacity())
		{
			reserve(grow_to(sz)) ;
		}

	}

	void destroy(iterator first, iterator last)
	{
		iterator iter = first ;
		while (iter != last)
		{
			allocator_.destroy(iter) ;
			++iter ;
		} // end of while (iter != last)
	}

	void construct_n(iterator& dest, size_type n, const T& val)
	{
		iterator iter = dest ;
		while (n--)
		{
			allocator_.construct(iter, val) ;
			++iter ;
		}

	}

	bool buy(size_type capacity)
	{	// allocate array with _Capacity elements
		first_ = 0, last_ = 0, end_of_storage_ = 0 ;
		if (capacity == 0)
			return (false);
		else if (max_size() < capacity)
			xlen();	// result too long
		else
		{	
			// nonempty array, allocate storage
			first_ = this->allocator_.allocate(capacity);
			last_ = first_ ;
			end_of_storage_ = first_ + capacity ;
		}
		return (true);
	}

	iterator make_iter(const_iterator where)
	{
		iterator iter = begin() ;
		if (!empty())
		{
			iter += where - begin() ;
		}

		return iter ;
	}

	void xlen()
	{
		//throw std::exception("vector<T> too long") ;
		;
	}

protected:

	//enum { SPARE_CAPACITY = 16 };

	iterator    first_ ;
	iterator    last_ ;
	iterator    end_of_storage_ ;
	Alloc       allocator_ ;

}; // end of class sequence

} // signalway

#endif // _SEQUENCE_H