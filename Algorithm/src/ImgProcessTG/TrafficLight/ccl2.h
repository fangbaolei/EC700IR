#ifndef _CCL2_H
#define _CCL2_H

#include "sequence.h"
#include "swbasetype.h"


namespace signalway
{
const int INVALID_LABEL = -1 ;

// default_comparer
template<typename T>
struct default_comparer
{
	bool operator()(const T& lhs, const T& rhs)
	{
		return lhs == rhs ;
	}
}; // default_comparer

template<typename T>
struct default_excluder
{
	bool operator()(const T& val)
	{
		return false ;
	}
} ;

// ccl
class ccl
{
public:
	ccl() ;
	~ccl() ;

public:
	template
	<
		typename T, 
		typename Comparer,
		typename Excluder
	>
	void operator()(const T* srcimg, int width, int height,
		int widthStep, sequence<sequence<HV_POINT> >& blobs, 
		Comparer isSame, Excluder isExcluded, bool K8_CONNECTIVITY)
	{
		if ( width * height > labels_.capacity())
		{
			labelimage_.resize(width * height) ;
		}

		clear() ;

		const T* srow = srcimg ;
		const T* lastSRow = 0 ;
		int* lrow = &labelimage_[0] ;
		int* lastLRow = 0 ;
		int sstep = widthStep/sizeof(T) ;
		int lstep = width ;

		/************************************************************************/
		/* labeling                                                             */
		/************************************************************************/
		lrow[0] = draw_a_label(isExcluded(srow[0])) ;
		
		// label the first row
		for (int x = 1; x < width; ++x)
		{
			if (isSame(srow[x], srow[x-1]))
			{
				lrow[x] = lrow[x-1] ;
			}
			else
			{
				lrow[x] = draw_a_label(isExcluded(srow[x])) ;
			}

		} // end of for (int x = 1, y = 0; x < width; ++x)

		// label subsequent rows
		for (int y = 1; y < height; ++y)
		{
			lastSRow = srow ;
			lastLRow = lrow ;
			srow += sstep ;
			lrow += lstep ;

			// label the first pixel on this row
			if (isSame(srow[0], lastSRow[0]))
			{
				lrow[0] = lastLRow[0] ;
			}
			else
			{
				lrow[0] = draw_a_label(isExcluded(srow[0])) ;
			}

			for (int x = 1; x < width; ++x)
			{
				int thisLabel = INVALID_LABEL ;

				// inherit label from the left neighbor if they are same
				if (isSame(srow[x], srow[x-1]))
				{
					thisLabel = lrow[x-1] ;
				}
				for (int k = (K8_CONNECTIVITY ? -1 : 0); k < 1; ++k)
				{
					if (isSame(srow[x], lastSRow[x+k]))
					{
						if (thisLabel != INVALID_LABEL)
						{
							merge(thisLabel, lastLRow[x+k]) ;
						}
						else
						{
							thisLabel = lastLRow[x+k] ;
						}
					} // end of if (isSame(srow[x], lastSRow[x+k]))
				} // end of for (int k = (K8_CONNECTIVITY ? -1 : 0); k < 1; ++k)

				if (thisLabel != INVALID_LABEL)
				{
					lrow[x] = thisLabel ;
				}
				else
				{
					lrow[x] = draw_a_label(isExcluded(srow[x])) ;
				}

				if (K8_CONNECTIVITY && lrow[x-1] != INVALID_LABEL
					&& isSame(srow[x-1], lastSRow[x]))
				{
					merge(lrow[x-1], lastLRow[x]) ;
				}

			} // end of for (int x = 1; x < width; ++x)

		} // end of for (int y = 1; y < height; ++y)

		/************************************************************************/
		/* relabeling                                                           */
		/************************************************************************/
		int blobCount = relabeling(width, height) ;
		blobs.clear() ;
		if (blobCount == 0)
		{
			return ;
		}

		/************************************************************************/
		/* extract blobs from labeled image                                     */
		/************************************************************************/
		
		blobs.resize(blobCount) ;
		int asumeArea = width*height / blobCount ;
		for (int i = 0; i < blobCount; ++i)
		{
			blobs[i].reserve(asumeArea) ;
		}
		lrow = &labelimage_[0] ;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (lrow[x] != INVALID_LABEL)
				{
					HV_POINT pt ;
					pt.x = x , pt.y = y ;
					blobs[lrow[x]].push_back(pt) ;
				}
			} // end of for (int x = 0; x < width; ++x)
			lrow += lstep ;
		} // end of for (int y = 0; y < height; ++y)

	}

private:
	void clear() ;
	int draw_a_label(bool isExcluded = false) ;
	int relabeling(int width, int height) ;
	void merge(int l1, int l2) ;
	bool is_equivalent(int l1, int l2) ;
	bool is_root(int l) ;
	int root_of(int l) ;
	

private:
	struct similarity
	{
		similarity()
			: label_(0), same_as_(0)
		{ }
		similarity(int label)
			: label_(label), same_as_(label)
		{ }
		similarity(int label, int same_as) 
			: label_(label), same_as_(same_as)
		{ }

		int     label_ ;
		int     same_as_ ;
		int     tag_ ;
	};

private:
	sequence<int>           labelimage_ ;
	sequence<similarity>    labels_ ;
	int                        max_label_ ;
	
}; // ccl

ccl::ccl()
	: max_label_(0)
{ }

ccl::~ccl()
{ }


void ccl::clear()
{
	max_label_ = 0 ;
	labels_.clear() ;
}

int ccl::draw_a_label( bool isExcluded /*= false*/ )
{
	if (isExcluded)
	{
		return INVALID_LABEL ;
	}

	if (max_label_+1 > labels_.capacity())
	{
		labels_.reserve(max_label_*2) ;
	}
	labels_.push_back(similarity(max_label_)) ;
	return max_label_++ ;

}

void ccl::merge( int l1, int l2 )
{
	if (!is_equivalent(l1,l2))
	{
		labels_[root_of(l1)].same_as_ = root_of(l2) ;
	}
}

bool ccl::is_equivalent( int l1, int l2 )
{
	return root_of(l1) == root_of(l2) ;
}

bool ccl::is_root( int l )
{
	return labels_[l].same_as_ == labels_[l].label_ ;
}

int ccl::root_of( int l )
{
	while (!is_root(l))
	{
		// link this node to its parent's parent, just to shorten the tree.
		l = labels_[l].same_as_ = labels_[labels_[l].same_as_].same_as_ ;
	}

	return l ;

}

int ccl::relabeling( int width, int height )
{
	int imageSize = width * height ;


	//std::cout << "Before relabel: " << std::endl ;

	//for (size_t i = 0 ; i < imageSize; ++i)
	//{
	//	if (i % width == 0)
	//		std::cout << std::endl ;

	//	std::cout << std::setw(5) << labelimage_[i] ;
	//}

	//std::cout << std::endl ;

	int newTag = 0 ;
	int labelCount = labels_.size() ;
	for (int l = 0; l < labelCount; ++l)
	{
		if (is_root(l))
		{
			labels_[l].tag_ = newTag ++ ;
		}
	} // end of for (int l = 0; l < labelCount; ++l)

	for (int i = 0; i < imageSize; ++i)
	{
		if (labelimage_[i] != INVALID_LABEL)
		{
			labelimage_[i] = labels_[root_of(labelimage_[i])].tag_ ;
		}
	}

	//std::cout << "After relabel: " << std::endl ;

	//for (size_t i = 0 ; i < imageSize; ++i)
	//{
	//	if (i % width == 0)
	//		std::cout << std::endl ;

	//	std::cout << std::setw(5) << labelimage_[i] ;
	//}

	//std::cout << std::endl ;

	return newTag ;
}


} // namespace signalway

#endif // _CCL2_H