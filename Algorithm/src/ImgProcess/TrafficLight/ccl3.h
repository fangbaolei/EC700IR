#ifndef _CCL3_H
#define _CCL3_H

#include "sequence.h"
#include "swbasetype.h"

//#define _USE_MEM_POOL

namespace 
{
using namespace signalway ;
// node
template<typename T> 
struct node
{
	node(unsigned int elemCount)
		: prev_(0)
		, next_(0)
		, data_(new sequence<T>())
	{
		data_->reserve(elemCount) ;
	}
	~node()
	{
		delete data_ ;
	}

	void push_back(const T& val)
	{
		data_->push_back(val) ;
	}

	T& back()
	{
		return data_->back() ;
	}

	const T& back() const
	{
		return data_->back() ;
	}
	
	void clear() 
	{
		data_->clear() ;
	}

	unsigned int size()
	{
		return data_->size() ;
	}

	node*                      prev_ ;
	node*                      next_ ;
	signalway::sequence<T>*    data_ ;
}; // end of node

// memory_pool
template<typename T>
class memory_pool
{
public:
	memory_pool(unsigned int blockSize)
		: block_size_(blockSize)
		, begin_node_(0)
		, cur_node_(0)
	{ }

	~memory_pool()
	{
		for (node<T>* i = begin_node_; i != 0; )
		{
			i = i->next_ ;
			delete i ;
		} // end of for (node* i = begin_node_; i != 0; )
	}

	void push_back(const T& val)
	{
		if (0 == begin_node_)
		{
			node<T>* newNode = new node<T>(block_size_) ;
			cur_node_ = begin_node_ = newNode ;
		}
		if (cur_node_->size() == block_size_)
		{
			// current block is full
			if (!cur_node_->next_)
			{
				node<T>* newNode = new node<T>(block_size_) ;
				newNode->prev_ = cur_node_ ;
				cur_node_->next_ = newNode ;
			}
			cur_node_ = cur_node_->next_ ;
		}

		cur_node_->push_back(val) ;

	}

	T& back()
	{
		return cur_node_->back() ;
	}

	const T& back() const
	{
		return cur_node_->back() ;
	}

	void clear() 
	{
		for (node<T>* i = begin_node_; i != 0; )
		{
			i->clear() ;
			i = i->next_ ;
		} // end of for (node* i = begin_node_; i != 0; )
	}

private:
	unsigned int    block_size_ ;
	node<T>*        begin_node_ ;
	node<T>*        cur_node_ ;
	
}; // end of memory_pool

} // namespace

namespace signalway
{

// ccl_line
struct ccl_line
{
	ccl_line()
		: label_(0)
		, current_(0)
		, begin_(0)
		, end_(0)
		, next_(0)
	{ }
	ccl_line(std::size_t label, std::size_t current, std::size_t begin, std::size_t end)
		: label_(label)
		, current_(current)
		, begin_(begin)
		, end_(end)
		, next_(0)
	{ }

	std::size_t    label_ ;
	std::size_t    current_ ;
	std::size_t    begin_ ;
	std::size_t    end_ ;
	ccl_line*      next_ ;

}; // ccl_line

class ccl_table
{
public:
	ccl_table() : head_(0), tail_(0) { }
	ccl_table(ccl_line* head) 
		: head_(head)
		, tail_(head)
	{
		if (tail_ != 0)
		{
			while (tail_->next_)
			{
				tail_ = tail_->next_ ;
			}
		} // end of if (tail_ != 0)
	}
	~ccl_table()
	{ }

public:
	void add_node(ccl_line* node)
	{
		if (!head_)
		{
			head_ = tail_ = node ;
		}
		else
		{
			tail_->next_ = node ;
			tail_ = node ;
		}

		while (tail_->next_)
		{
			tail_ = tail_->next_ ;
		}

	}

	void merge(const ccl_table& cc)
	{
		if (!cc.head_)
		{
			return ;
		}

		if (tail_)
		{
			tail_->next_ = cc.head_ ;
			tail_ = cc.tail_ ;
		}

	}

	void clear()
	{
		head_ = tail_ = 0 ;
	}

	bool empty()
	{
		return head_ != 0 ;
	}

	void to_blob(sequence<HV_POINT>& blob)
	{
		blob.clear() ;
		ccl_line* p = head_ ;
		while (p != 0)
		{
			int y = p->current_ ;
			for (int x = p->begin_; x <= p->end_; ++x)
			{
				HV_POINT pt ;
				pt.x = x ;
				pt.y = y ;
				blob.push_back(pt) ;
			}
			p = p->next_ ;
		}
	}
	
private:
	ccl_line*    head_ ;
	ccl_line*    tail_ ;

}; // ccl_table

// ccl3
class ccl3
{
public:
	ccl3() ;
	~ccl3() ;

template
<
	typename T,
	typename Comparer,
	typename Excluder
>
void operator()( const T* src, std::size_t width, std::size_t height, 
		std::size_t widthStep, sequence<sequence<HV_POINT> >& blobs, 
		Comparer isSame, Excluder isExcluded )
	{

		if (src == 0 || width <= 0 || height <= 0)
		{
			//throw std::exception("Invalid image size") ;
			blobs.clear() ;
			return ;
		}

		initialize() ;

		const T* thisRow = src ;
		const T* lastRow = 0 ;
		int step = widthStep/sizeof(T) ;

		int lastEnd = 0 ;
		for (int y = 0; y < height; ++y)
		{
			// first position of this row
			int lineBegin = 0 ;
			int lastCount = last_lines_.size() ;
			for (int x = 1; x < width; ++x)
			{
				if (!isSame(thisRow[x-1], thisRow[x]))
				{
					if (!isExcluded(thisRow[x-1]))
					{
						add_line(thisRow, lastRow, lastCount, y, lineBegin, x-1, isSame) ;
					} // end of if (!isExcluded(thisRow[x-1]))

					lineBegin = x ; // meet a new line's beginning

				} // end of if (!isSame(thisRow[x-1], thisRow[x]))

			} // end of for (int x = 1; x < width; ++x)

			if (!isExcluded(thisRow[width-1]))
			{
				add_line(thisRow, lastRow, lastCount, y, lineBegin, width-1, isSame) ;
			}

			next_checked_ = 0 ;

			lastRow = thisRow ;
			thisRow += step ;

			last_lines_.swap(current_lines_) ;
			if (last_lines_.size())
			{
				last_end_  = last_lines_.back()->end_ ;
			}
			current_lines_.clear() ;

		} // end of for (int y = 0; y < height; ++y)

		blobs.clear() ;
		int tabelCount = tables_.size() ;
		sequence<HV_POINT> blob ;
		for (int i = 0; i < tabelCount; ++i)
		{
			tables_[i].to_blob(blob) ;
			if (blob.size())
			{
				blobs.push_back(blob) ;
			}
		}

	}

private: 
	void initialize() ;
	bool connected(const ccl_line& l1, const ccl_line& l2) ;
	template < typename T, typename Comparer > 
	void add_line( const T* const thisRow, const T* const lastRow, 
		int lastCount, int y, int x0, int x1, Comparer& isSame )
	{

		lines_.push_back(ccl_line(INVALID_LABEL, y, x0, x1)) ;
		ccl_line* line = &lines_.back() ;


		bool connectivity = false ; // connectivity with lines on last row

		if (next_checked_ < lastCount)
		{
			if (line->begin_ > last_end_+1)
			{
				next_checked_ = lastCount ;
			}
			else
			{
				for (int i = next_checked_; i < lastCount; ++i)
				{
					ccl_line* lastLine = last_lines_[i] ;
					if (line->end_+1 < lastLine->begin_)
					{
						break ;
					}
					else if (line->begin_ <= lastLine->end_+1 
						&& isSame(thisRow[x0], lastRow[lastLine->begin_]))
					{
						if (!connectivity)
						{
							line->label_ = lastLine->label_ ;
							tables_[lastLine->label_].add_node(line) ;
							connectivity = true ;

						} // end of if (!connectivity)
						else if (line->label_ != lastLine->label_)
						{
							tables_[line->label_].merge(tables_[lastLine->label_]) ;
							tables_[lastLine->label_].clear() ;
							lastLine->label_ = line->label_ ;
						}

						if (line->end_ >= lastLine->end_)
						{
							++next_checked_ ;
						} // end of if (line->end_ >= lastLine->end_)
						else
						{
							break ;
						}

					}

				} // end of for (int i = nextChecking; i < lastCount; ++i)

			}

		} // end of if (nextChecking < lastCount)

		if (!connectivity)
		{
			// no connectivity with last line, assign it a new label
			line->label_ = label_++ ;
			tables_.push_back(ccl_table(line)) ;
		}

		current_lines_.push_back(line) ;
	}

private:
	enum { INVALID_LABEL = -1 };



private:
	int                       label_ ;
	int                       next_checked_ ;
	int                       last_end_ ;
#ifdef _USE_MEM_POOL
	memory_pool<ccl_line>     lines_ ;
#else
	sequence<ccl_line>     lines_ ; 
#endif
	sequence<ccl_line*>    last_lines_ ;
	sequence<ccl_line*>    current_lines_ ;
	sequence<ccl_table>    tables_ ;

}; // ccl3

ccl3::ccl3()
	: label_(0)
	, next_checked_(0)
	, last_end_(0)
#ifdef _USE_MEM_POOL
	, lines_(1000)
#endif
{
#ifndef _USE_MEM_POOL
	lines_.reserve(10000) ;
#endif
}

ccl3::~ccl3()
{ }

bool ccl3::connected( const ccl_line& l1, const ccl_line& l2 )
{
	return l1.begin_ <= l2.end_ && l2.begin_ <= l1.end_ ;
}

void ccl3::initialize()
{
	label_ = 0 ;
	next_checked_ = 0 ;
	last_end_ = 0 ;
	lines_.clear() ;
	last_lines_.clear() ;
	current_lines_.clear() ;
	tables_.clear() ;
}

} // signalway

#endif // _CCL3_H