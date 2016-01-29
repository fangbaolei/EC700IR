#ifndef _TRAFFIC_LIGHT_PARAM_
#define _TRAFFIC_LIGHT_PARAM_

typedef struct _tagTrafficLightParam
{
    bool fEnhanceRed;
    int nLightCount;                                //灯组数量
    char rgszLightPos[MAX_TRAFFICLIGHT_COUNT][64];  //灯组信息

    _tagTrafficLightParam()
    {
        fEnhanceRed = true;
        nLightCount = 0;
        for (int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
        {
            strcpy(rgszLightPos[i], "(0,0,0,0),0,0,00");
        }
    }
}TRAFFIC_LIGHT_PARAM;


#endif
