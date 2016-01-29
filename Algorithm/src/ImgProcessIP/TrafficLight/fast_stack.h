#ifndef _FAST_STACK_H
#define _FAST_STACK_H 

#include <new>
namespace signalway
{

// data_list: memory manager list 
//
//#if RUN_PLATFORM == PLATFORM_WINDOWS
//#endif

// data_list: memory manager list 
//template<unsigned int byte_size> 
//class data_list
//{ 
//	struct node
//	{ 
//		node*            prev_ ; 
//		node*            next_ ; 
//		unsigned char    data_[byte_size] ; // data space, use to store the elems
//	}; // end of struct node
//
//public: 
//	data_list()
//		: begin_node_(0)
//		, cur_node_(0)
//		, size_(0)
//	{ } 
//
//	~data_list() 
//	{ 
//		for (node* i = begin_node_; i != 0; ) 
//		{ 
//			node* next = i->next_; 
//			delete i; 
//			i = next; 
//		} 
//	} 
//
//	unsigned int size() const 
//	{ 
//		return size_ ; 
//	} 
//
//	void push()
//	{ 
//		if (0 == begin_node_) 
//		{ 
//			// it's an empty list
//			begin_node_ = new node ; 
//			begin_node_->prev_ = 0 ; 
//			begin_node_->next_ = 0 ; 
//			cur_node_ = begin_node_ ; 
//		} 
//		else if (0 != cur_node_->next_)
//		{ 
//			// there is a free node
//			cur_node_ = cur_node_->next_; 
//		} 
//		else 
//		{ 
//			node* endNode = new node ; 
//			endNode->prev_ = cur_node_ ; 
//			endNode->next_ = 0 ; 
//			cur_node_->next_ = endNode ; 
//			cur_node_ = endNode ;    
//		} 
//
//		++size_ ; 
//	} 
//
//	unsigned char* top() 
//	{ 
//		return cur_node_->data_ ; 
//	}
//
//	void pop() 
//	{ 
//		node*& tempNode= cur_node_->next_ ; 
//		if (tempNode != 0) 
//		{ 
//			delete tempNode ;
//			tempNode = 0 ; 
//		} 
//
//		cur_node_ = cur_node_->prev_ ; 
//		--size_; 
//	} 
//
//private: 
//	node*           cur_node_; 
//	node*           begin_node_; 
//	unsigned int    size_; // used to track the size of list
//
//}; // end of class data list

template<unsigned int byte_size> 
class data_list
{ 
	struct node
	{ 
		node()
			: prev_(0)
			, next_(0)
			, data_((unsigned char*)::operator new(byte_size))
		{
			// TODO: just for testing
			if (data_ == 0)
			{
				HV_Trace(5, "Failed memory allocation: node::node()\n") ;
			}
		}

		~node()
		{
			::operator delete(data_) ;
		}

		node*             prev_ ; 
		node*             next_ ; 
		unsigned char*    data_ ;
	}; // end of struct node

public: 
	data_list()
		: begin_node_(0)
		, cur_node_(0)
		, size_(0)
	{ } 

	~data_list() 
	{ 
		for (node* i = begin_node_; i != 0; ) 
		{ 
			node* next = i->next_; 
			delete i; 
			i = next; 
		} 
	} 

	unsigned int size() const 
	{ 
		return size_ ; 
	} 

	void push()
	{ 
		if (0 == begin_node_) 
		{ 
			// it's an empty list
			begin_node_ = new node ; 
			begin_node_->prev_ = 0 ; 
			begin_node_->next_ = 0 ; 
			cur_node_ = begin_node_ ; 
		} 
		else if (0 != cur_node_->next_)
		{ 
			// there is a free node
			cur_node_ = cur_node_->next_; 
		} 
		else 
		{ 
			node* endNode = new node ; 
			endNode->prev_ = cur_node_ ; 
			endNode->next_ = 0 ; 
			cur_node_->next_ = endNode ; 
			cur_node_ = endNode ;    
		} 

		++size_ ; 
	} 

	unsigned char* top() 
	{ 
		return cur_node_->data_ ; 
	}

	void pop() 
	{ 
		node*& tempNode= cur_node_->next_ ; 
		if (tempNode != 0) 
		{ 
			delete tempNode ;
			cur_node_->next_ = 0 ; 
		} 

		cur_node_ = cur_node_->prev_ ; 
		--size_; 
	} 

private: 
	node*           cur_node_; 
	node*           begin_node_; 
	unsigned int    size_; // used to track the size of list

}; // end of class data list

//fast_stack 
template<typename T, bool IsPOD = false> 
class fast_stack
{ 
public: 
	enum
	{ 
		elem_size = sizeof(T),
		elem_count_per_block = (1020/elem_size)+1
	}; 


	typedef T value_type ; 
	typedef unsigned int size_type ; 
	
public:

	fast_stack()
		: begin_node_(0)
		, last_node_(0) 
		, free_node_(0)
		, elem_count_(0)
	{ }

	~fast_stack() 
	{ 
		if ( (!IsPOD) && (mem_list_.size() > 0) ) 
		{ 
			// destroy objects stored in current memory block
			for ( T* i = begin_node_; i < free_node_; ++i )
			{
				i->T::~T() ;
			} // end of for (T* i=begin_node_;i<cur_node_;++i)

			// destroy objects stored in previous memory block
			int nsize = (int)mem_list_.size() ; 
			for (int j = 0; j < (nsize-1); ++j) 
			{ 
				mem_list_.pop() ; 
				free_node_ = (T*)mem_list_.top() ; 
				for (int i = 0; i < elem_count_per_block; ++i) 
				{ 
					free_node_->T::~T() ; 
					++free_node_ ; 
				} // end of for (int i = 0; i < elem_count_per_block; ++i) 

			} // end of for (int j = 0; j < (nsize-1); ++j)

		} // end of if ((!IsPOD)&&(node_list_.size()>0))

	} 

	bool empty() const    
	{ 
		return ( 0 == size()) ; 
	}

	size_type size() const 
	{ 
		//return (free_node_ - begin_node_) + elem_count_per_block * (std::max((int)mem_list_.size()-1,0)) ;
		return elem_count_ ;
	}

	value_type& top() 
	{ 
		return *(free_node_-1) ; 
	}

	const value_type& top() const 
	{ 
		return *(free_node_-1) ; 
	}

	void push(const value_type& x) 
	{ 
		if (free_node_ == last_node_) 
		{ 
			_ToNextNode() ; 
		} 
		
		new (free_node_) T(x) ; 
		++free_node_ ; 
		++elem_count_ ;
	}

	void pop() 
	{ 
		if (free_node_ == begin_node_) 
		{ 
			_ToPrevNode() ; 
		} 
		--free_node_ ; 
		free_node_->T::~T() ; 
		--elem_count_ ;
	} 

protected:

	void _ToNextNode() 
	{ 
		mem_list_.push() ;    
		begin_node_ = (T*)(mem_list_.top()) ; 
		last_node_ = begin_node_ + elem_count_per_block ; 
		free_node_  = begin_node_ ; 
	}

	void _ToPrevNode() 
	{ 
		mem_list_.pop() ; 
		begin_node_ = (T*)(mem_list_.top()) ; 
		last_node_ = begin_node_ + elem_count_per_block ; 
		free_node_  = last_node_ ; 
	}

protected: 
	typedef data_list<elem_size*elem_count_per_block> memory_list ;

	T*             begin_node_ ; 
	T*             last_node_ ; 
	T*             free_node_ ; 
	size_type      elem_count_ ;
	memory_list    mem_list_ ;

}; // end of class fast_stack


} // signalway

#endif // _FAST_STACK_H