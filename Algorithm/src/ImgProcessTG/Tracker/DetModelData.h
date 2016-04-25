/// @file
/// @brief 电警模型数据声明
/// @author ganzz
/// @date 10/31/2013 15:31:21
/// 
/// 修改说明:
/// [10/31/2013 15:31:21 ganzz] 最初版本
/// 

#pragma once

//#define BIKE_DET_MODE  // 非机动车检测模式

namespace swTgVvdApp
{

struct TG_DET_MODE_INFO
{
    unsigned char* pData;
    int nDataLen;
};

extern TG_DET_MODE_INFO g_cTgDetMode_DaySmall;
extern TG_DET_MODE_INFO g_cTgDetMode_DayLarge;
extern TG_DET_MODE_INFO g_cTgDetMode_DuskSmall;
extern TG_DET_MODE_INFO g_cTgDetMode_DuskLarge;
extern TG_DET_MODE_INFO g_cTgDetMode_DuskSmallEx;
extern TG_DET_MODE_INFO g_cTgDetMode_DuskLargeEx;
extern TG_DET_MODE_INFO g_cTgDetMode_NightSmall;
extern TG_DET_MODE_INFO g_cTgDetMode_NightLarge;

}