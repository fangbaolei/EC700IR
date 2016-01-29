#ifndef _TRAFFICLIGHT_RECOGNIZER_H
#define _TRAFFICLIGHT_RECOGNIZER_H

// created by zoul, 2012-01-10

//#include <vector>
#include "sequence.h"
#include "swbasetype.h"
#include "swimage.h"

namespace signalway
{

// light_status
enum light_status
{
	UNKNOWN,
	GREEN,
	RED,
	YELLOW
}; // light_status

// direction
enum direction
{
	HORZ,  // horizontal
	VERT   // vertical
}; // direction

// light_group
struct light_group
{
	light_group() ;
	light_group(HV_RECT pos, direction d, int lightCount) ;

	HV_RECT     position_ ;
	direction   direction_ ;
	int			light_amount_ ;
	int			light_width_ ;
	int			light_height_ ;
}; // light_group

// trafficlight
struct trafficlight
{
	trafficlight() ;
	trafficlight(HV_RECT pos, light_status status, float confidence = 1.0f) ;

	HV_RECT         position_ ;
	light_status    status_ ;
	float           confidence_ ;
}; // trafficlight

// createPoint
HV_POINT createPoint(int x, int y) ;

// trafficlight_recognizer
class CTrafficLightRecognizer
{
public:
	CTrafficLightRecognizer(light_group light_group, HV_SIZE imageSize, float offsetScale = 1.0f) ;
	CTrafficLightRecognizer();
	~CTrafficLightRecognizer() ;
	void SetRecognizerParam(light_group light_group, HV_SIZE imageSize, float offsetScale = 1.0f);

public:
	void FindTrafficLight(const HV_COMPONENT_IMAGE* src,    // source image
		sequence<trafficlight>& lights,
		bool smallGroup = false,
		bool processAdhesion = false,
		bool night = false,
		HV_POINT lightGroupCenter = createPoint(-1,-1)) ; // the new center of light group 

private:
	HV_RECT calc_roi(HV_SIZE imageSize) ;
	HV_RECT calc_center_region(HV_SIZE sz);
	HV_RECT recalc_group_rect(HV_POINT newCenter, HV_SIZE imageSize);
	void decide_scale(HV_SIZE roiSize, float &horzScale, float &vertScale);

private:
	light_group                  light_group_ ;
	float                        offset_scale_ ;
	//std::vector<trafficlight>    lights_ ;

}; // trafficlight_recognizer

} // signalway



#endif // _TRAFFICLIGHT_RECOGNIZER_H