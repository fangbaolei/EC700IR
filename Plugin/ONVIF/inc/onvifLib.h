#ifndef ONVIFLIB_H_INCLUDED
#define ONVIFLIB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	PTZ_PAN,
	PTZ_STOP_PAN,
	PTZ_TILT,
	PTZ_STOP_TILT,
	PTZ_PANTILT,
	PTZ_STOP_PANTILT,
	PTZ_ZOOM,
	PTZ_STOP_ZOOM,

	PTZ_SET_PANTILT,
	PTZ_SET_ZOOM,

	PTZ_PRESET_SET,
	PTZ_PRESET_CALL,
	PTZ_PRESET_REMOVE,

	PTZ_GOTO_HOMEPOSITION,
	
};

typedef struct __ONVIF_PTZ_PARAM
{
	int fInited; // !! must be the 1st , don't move it
	
	int fFixedHomePos;
	int fHomeSupported;

	int fAbsoluteMoveSupport;
	int iAbsolutePRangeMax;
	int iAbsolutePRangeMin;
	int iAbsoluteTRangeMax;
	int iAbsoluteTRangeMin;
	int iAbsoluteZRangeMax;
	int iAbsoluteZRangeMin;

	int fContinuousMoveSupport;
	int iContinuousPSpeedMax;
	int iContinuousPSpeedMin;
	int iContinuousTSpeedMax;
	int iContinuousTSpeedMin;
	int iContinuousZSpeedMax;
	int iContinuousZSpeedMin;

	int iPresetCount;
}ONVIF_PTZ_PARAM;

typedef struct _UserArray_t
{
	char	user[32];			///< username
	char	password[16];	///< password
	int  	authority;					///< user authority
}UserArray_t;

typedef int (*P_Trace)(int nRank, const char* szfmt, ...);
typedef int (*PTZControlCallback)(const int iCmdID, void* pvArg);
typedef int (*UserManageCallback)(const int len, void* pvArg);
typedef int (*GetUserCallback)(void* psize, void* pvArg);

//³£¹æÉèÖÃ
//iVideoWidth = 1600
//iVideoHeight = 1088
//ivideoFramerate = 15
//ionvifResponsePort = 8080
//ionvifDiscover = 3702
//irtspPort = 554
int StartOnvif( int ivideoFramerate,
 int ionvifResponsePort, int ionvifDiscover, int irtspPort, P_Trace  prt,int* ipAvg);


int StartOnvif_One( int iVideoWidth, int iVideoHeight);
int StartOnvif_EX( int iVideoWidth, int iVideoHeight);

int StopOnvif( );

int RegisterUserManage(UserManageCallback pUser);

int ReggisterGetUser(GetUserCallback pGetUser);

int RegisterPTZCallback(PTZControlCallback pfPTZ);

int SetPTZParam(const ONVIF_PTZ_PARAM* psParam);

int GetPTZParam(ONVIF_PTZ_PARAM* psParam);


#ifdef __cplusplus
}
#endif

#endif // ONVIFLIB_H_INCLUDED
