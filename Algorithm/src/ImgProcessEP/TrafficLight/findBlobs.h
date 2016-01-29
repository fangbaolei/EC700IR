#ifndef _FIND_BLOBS_H
#define _FIND_BLOBS_H

//#include <vector>

#include "sequence.h"
#include "swbasetype.h"

namespace signalway
{

typedef sequence<HV_POINT> blob ;

// findBlobs
void findBlobs(const unsigned char* image, sequence<blob>& blobs, 
			   int width, int height, int widthStep, float bgClr = 0) ;

void findBlobs2(const unsigned char* image, sequence<blob>& blobs, 
			   int width, int height, int widthStep, float bgClr = 0) ;

void findBlobs3(const unsigned char* image, sequence<blob>& blobs, 
				int width, int height, int widthStep, float bgClr = 0) ;

// calcBoundingRect
HV_RECT calcBoundingRect(const blob& b) ;

} // signalway

#endif // _FIND_BLOBS_H

