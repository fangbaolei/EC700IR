#ifndef _HIST_H
#define _HIST_H

#include "swimage.h"
#include "hvutils.h"

namespace signalway
{
	// histogram: 简单的直方图结构; added by zoul, 20110815
	struct histogram
	{
	public:
		histogram(int size)
			: size_(size)
			, bins_(new float[size])
		{ }

		~histogram()
		{
			delete [] bins_ ;
		}

		histogram& operator=(const histogram& hist)
		{
			if (size_ < hist.size_)
			{
				float* bins = new float[hist.size_] ;
				if (!bins)
				{
					// 内存分配失败, 不进行任何复制
					return *this ;
				}
				
				// 释放原有的bins的内存
				delete [] bins_ ;

				bins_ = bins ;
				size_ = hist.size_ ;

			}

			// 内存拷贝
			memcpy((void*)bins_, (void*)hist.bins_, sizeof(float)*hist.size_) ;

			return *this ;
		}

		void clear()
		{
			memset((void*)bins_, 0, sizeof(float)*size_) ;
		}

		int        size_ ;
		float*    bins_ ;

	};

	// COMP_METHOD: 直方图比较方法; added by zoul, 20110805
	enum COMP_METHOD
	{
		COMP_CORREL = 0 ,
		COMP_CHISQR     ,
		COMP_INTERSECT  ,
		COMP_BHATTACHARYYA
	};

	// utility functions

	// calc_hist: 计算直方图; added by zoul, 20110815
	void calc_hist(const HV_COMPONENT_IMAGE* image, histogram& hist) ;

	// calc_hist: 计算二维直方图, 暂时支持YUV; added by zoul, 20110818
	void calc_hist_2d(const HV_COMPONENT_IMAGE* image, histogram& hist, const int histSize[2]) ;

	// normalize_hist: 归一化直方图; added by zoul, 20110815
	void normalize_hist(histogram& hist, float factor = 1.) ;

	// compare_hist: 比较直方图; added by zoul, 20110805
	float compare_hist(const histogram& hist1, const histogram& hist2, int method) ;

	// calc_hist_u: 增加计算U通道直方图，
	void calc_hist_u(const HV_COMPONENT_IMAGE* image, histogram& hist);

}


#endif // _HIST_H
