/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**********/
// Copyright (c) 1996-2014, Live Networks, Inc.  All rights reserved
// A test program that reads a H.264 Elementary Stream video file
// and streams it using RTP
// main program
//
// NOTE: For this application to work, the H.264 Elementary Stream video file *must* contain SPS and PPS NAL units,
// ideally at or near the start of the file.  These SPS and PPS NAL units are used to specify 'configuration' information
// that is set in the output stream's SDP description (by the RTSP server that is built in to this application).
// Note also that - unlike some other "*Streamer" demo applications - the resulting stream can be received only using a
// RTSP client (such as "openRTSP")

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
// C++
#include <list>
// C
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
// Local
#include "RTSPLIB.h"
#include "ByteStreamSocketSource.hh"
//#include "swH264VideoStreamer.hh"

typedef struct PlayClientData_t {
	RTPSink *videoSink;
	H264VideoStreamFramer *videoSource;
	char inputFileName[64];
}PLAY_CLIENT_DATA_T;

static const char *firstInputFileName = "/tmp/rtsph264.sock";
static const char *secondInputFileName = "/tmp/rtsph264Second.sock";
//static const char *streamName = "h264ESVideoTest";
//static const char *secondStreamName = "h264ESVideoTestSecond";
static pthread_t tid_RTSPServer = 0;
static TaskScheduler* scheduler;
static UsageEnvironment *env;

static void afterPlaying(void* clientData);

static void play(RTPSink* videoSink, const char *inputFileName)
{
	//FIXME: 
	unsigned int GET_SOURCE_MAX_TRY = 60, tries = 0;
	H264VideoStreamFramer* videoSource;
	ByteStreamSocketSource *socketSource = NULL;

	// Open the input file as a 'byte-stream file source':
	while(GET_SOURCE_MAX_TRY > tries){
		socketSource = ByteStreamSocketSource::createNew(*env, inputFileName);
		if (socketSource == NULL) {
			sleep(1);   //wait LprApp 
			tries++;
		} else {
			break;
		}
	}
	if(socketSource == NULL){
		*env << "Unable to open file \"" << inputFileName
			 << "\" as a byte-stream file source\n";
		*env << "tries = " << tries;
		return;
	}
	FramedSource* videoES = socketSource;
	PLAY_CLIENT_DATA_T *clientData = new PLAY_CLIENT_DATA_T;
	if(clientData == NULL){
		*env << "new PLAY_CLIENT_DATA error";
		return;
	}
	// Create a framer for the Video Elementary Stream:
	videoSource = H264VideoStreamFramer::createNew(*env, videoES);

	clientData->videoSink = videoSink;
	clientData->videoSource = videoSource;
	if(sizeof(clientData->inputFileName) > strlen(inputFileName)){
		memcpy(clientData->inputFileName, inputFileName, strlen(inputFileName));
	} else {
		*env << "ERROR: inputFileName too long! Should be less than 64 bytes!";
		return;
	}

	// Finally, start playing:
	*env << "Beginning to read from file...\n";
	videoSink->startPlaying(*videoSource, afterPlaying, (void *)clientData);
}

static void afterPlaying(void* clientData) {
  //*env << "...done reading from file\n";
	PLAY_CLIENT_DATA_T *pclientData = (PLAY_CLIENT_DATA_T *)clientData;
	if(pclientData == NULL){
		*env << "afterPlaying: pclientData == NULL";
		return;
	}
	RTPSink *videoSink = pclientData->videoSink;
	H264VideoStreamFramer* videoSource = pclientData->videoSource;
	char inputFileName[64] = {0};
	memcpy(inputFileName, pclientData->inputFileName, 64);
	videoSink->stopPlaying();
  // Note that this also closes the input file that this source read from.
	Medium::close(videoSource);
	delete pclientData;
  // Start playing once again:
	play(videoSink, inputFileName);
}



static void RtspServerStart(RTSP_PARAM_INFO *cRtspParamInfo)
{
	enum {ENUM_UNICAST = 0, ENUM_MULTICAST};
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);
	Boolean reuseFirstSource = true;
	OutPacketBuffer::maxSize = 500000;
	const unsigned short statusPort = 10086;
	Port RTSPStatusPort(statusPort);
	Port RTSPServerPort(cRtspParamInfo->iRTSPServerPort);
	RTPSink *videoSinkMajor = NULL;
	RTPSink *videoSinkMinor = NULL;
	char streamName[RTSP_STRING_LEN] = {0};
	char secondStreamName[RTSP_STRING_LEN] = {0};
	strncpy(streamName, cRtspParamInfo->rgStreamName[RTSP_STREAM_MAJOR], RTSP_STRING_LEN - 1);
	strncpy(secondStreamName, cRtspParamInfo->rgStreamName[RTSP_STREAM_MINOR], RTSP_STRING_LEN - 1);
	// Create 'groupsocks' for RTP and RTCP:
	struct in_addr destinationAddressMajor;
	struct in_addr destinationAddressMinor;
	UserAuthenticationDatabase *authDB = NULL;
//#ifdef ACCESS_CONTROL
	if(cRtspParamInfo->iAuthenticateEnable){
		authDB = new UserAuthenticationDatabase;
//		authDB->addUserRecord(cRtspParamInfo->rgUserName, cRtspParamInfo->rgPassword);
		authDB->addUserRecord("admin", "admin");
		fprintf(stdout, "%s %d Authentication Enable!\n", __FILE__, __LINE__);
	}
//#endif
	destinationAddressMajor.s_addr = chooseRandomIPv4SSMAddress(*env);
	destinationAddressMinor.s_addr = chooseRandomIPv4SSMAddress(*env);
	// Note: This is a multicast address.  If you wish instead to stream
	// using unicast, then you should use the "testOnDemandRTSPServer"
	// test program - not this test program - as a model.

	const unsigned short rtpPortNumMajor = 18888;
	const unsigned short rtcpPortNumMajor = rtpPortNumMajor + 1;
	const unsigned short rtpPortNumMinor = rtcpPortNumMajor + 1;
	const unsigned short rtcpPortNumMinor = rtpPortNumMinor + 1;
	const unsigned char ttl = 255;

	const Port rtpPortMajor(rtpPortNumMajor);
	const Port rtcpPortMajor(rtcpPortNumMajor);
	const Port rtpPortMinor(rtpPortNumMinor);
	const Port rtcpPortMinor(rtcpPortNumMinor);

	Groupsock rtpGroupsockMajor(*env, destinationAddressMajor, rtpPortMajor, ttl);
	rtpGroupsockMajor.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsockMajor(*env, destinationAddressMajor, rtcpPortMajor, ttl);
	rtcpGroupsockMajor.multicastSendOnly(); // we're a SSM source
	Groupsock rtpGroupsockMinor(*env, destinationAddressMinor, rtpPortMinor, ttl);
	rtpGroupsockMinor.multicastSendOnly();
	Groupsock rtcpGroupsockMinor(*env, destinationAddressMinor, rtcpPortMinor, ttl);
	rtcpGroupsockMinor.multicastSendOnly();

	// Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 1000000;

	// Create (and start) a 'RTCP instance' for this RTP sink:
	const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen+1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0'; // just in case

	RTSPServer* rtspServer = RTSPServer::createNew(*env, RTSPServerPort, authDB);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " 
			 << env->getResultMsg() << "\n";
		exit(1);
	}

	//first stream
	if(cRtspParamInfo->rgCommunicationMode[RTSP_STREAM_MAJOR] == ENUM_UNICAST){
		ServerMediaSession* smsMajor
			= ServerMediaSession::createNew(*env, streamName, 
											streamName, "Session streamed by \"testH264VideoStreamer\"");
		smsMajor->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env, 
																			  firstInputFileName, reuseFirstSource));
		rtspServer->addServerMediaSession(smsMajor);
	}else if(cRtspParamInfo->rgCommunicationMode[RTSP_STREAM_MAJOR] == ENUM_MULTICAST){
		videoSinkMajor = H264VideoRTPSink::createNew(*env, &rtpGroupsockMajor, 96);
		RTCPInstance* rtcpMajor = RTCPInstance::createNew(*env, &rtcpGroupsockMajor,
														  estimatedSessionBandwidth, CNAME,
														  videoSinkMajor, NULL /* we're a server */,
														  True /* we're a SSM source */);
		ServerMediaSession* smsMajor = ServerMediaSession::createNew(*env, streamName, firstInputFileName,
																	 "Session streamed by \"swH264VideoStreamer\"",
																	 True /*SSM*/);
		smsMajor->addSubsession(PassiveServerMediaSubsession::createNew(*videoSinkMajor, rtcpMajor));
		rtspServer->addServerMediaSession(smsMajor);
		play(videoSinkMajor, firstInputFileName);
	}

	if((cRtspParamInfo->iRTSPStreamNum > 1) && 
	   (cRtspParamInfo->rgCommunicationMode[RTSP_STREAM_MINOR] == ENUM_UNICAST)){
		ServerMediaSession* smsMinor
			= ServerMediaSession::createNew(*env, secondStreamName, 
											secondStreamName,
											"Session streamed by \"testH264VideoStreamer\"");
		smsMinor->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env, 
																			  secondInputFileName, reuseFirstSource));
		rtspServer->addServerMediaSession(smsMinor);
	}else if((cRtspParamInfo->iRTSPStreamNum > 1) && 
			 (cRtspParamInfo->rgCommunicationMode[RTSP_STREAM_MINOR] == ENUM_MULTICAST)){
		videoSinkMinor = H264VideoRTPSink::createNew(*env, &rtpGroupsockMinor, 96);
		RTCPInstance* rtcpMinor = RTCPInstance::createNew(*env, &rtcpGroupsockMinor,
														  estimatedSessionBandwidth, CNAME,
														  videoSinkMinor, NULL /* we're a server */,
														  True /* we're a SSM source */);
		ServerMediaSession *smsMinor = ServerMediaSession::createNew(*env, secondStreamName, secondInputFileName,
																	 "Session streamed by \"swH264VideoStreamer\"",
																	 True /*SSM*/);
		smsMinor->addSubsession(PassiveServerMediaSubsession::createNew(*videoSinkMinor, rtcpMinor));
		rtspServer->addServerMediaSession(smsMinor);
		play(videoSinkMinor, secondInputFileName);
	}

	rtspServer->setUpConnectionStatus(RTSPStatusPort);
	rtspServer->setStreamName(0, streamName, strlen(streamName));		//0 for major
	rtspServer->setStreamName(1, secondStreamName, strlen(secondStreamName));
	rtspServer->setAutoControlBitrate(cRtspParamInfo->iAutoControlBitrateEnable);

	if(rtspServer->setUpTunnelingOverHTTP(80) ||
	   rtspServer->setUpTunnelingOverHTTP(8000) ||
	   rtspServer->setUpTunnelingOverHTTP(8080)) {
		*env << "\n(We use port " << rtspServer->httpServerPortNum() 
			 << " for optional RTSP-over-HTTP tunneling.)\n";
	} else {
		*env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
	}

	env->taskScheduler().doEventLoop(); // does not return
	
}


static void *Fun_thread_RTSPServer(void* arg) 
{
  //enum {ENUM_UNICAST = 0, ENUM_MULTICAST};
  RTSP_PARAM_INFO *cRtspParamInfo = (RTSP_PARAM_INFO *)arg;


  RtspServerStart(cRtspParamInfo);
  // int iCurrentMode = cRtspParamInfo->iCommunicationMode;
  // // Begin by setting up our usage environment:

  // switch(iCurrentMode){
  // 	case ENUM_MULTICAST:
  // 		Multicast(cRtspParamInfo);	//TODO: complete this
  // 		break;
  // 	case ENUM_UNICAST:
  // 		Unicast(cRtspParamInfo);
  // 		break;
  // 	default:
  // 		Unicast(cRtspParamInfo);
  // 		break;
  // }


  return 0; // only to prevent compiler warning
}


bool StartRTSP( int* piErr, RTSP_PARAM_INFO *cRtspParamInfo)
{

    int err = pthread_create(&tid_RTSPServer, NULL, Fun_thread_RTSPServer, (void *)cRtspParamInfo);
    if( err != 0 )
    {
        *piErr = err;
        return false;
    }


    *piErr = err;
    return true;
}

bool StopRTSP( int* piErr)
{
	printf("RTSP: stop StopRTSP.\n");
    int err = 0;
	void* retval = NULL;
    if(tid_RTSPServer != 0 )
    {
        err = pthread_cancel(tid_RTSPServer);
        if( err != 0 )
        {
            printf("RTSP: pthread_cancel error err=%d",err);
            *piErr = err;
            return false;
        }
		else
		{

			err = pthread_join(tid_RTSPServer, &retval);
		}
    }


    printf("StopRTSP ... ok ");
    *piErr = err;
    tid_RTSPServer = 0;
    return true;
}

