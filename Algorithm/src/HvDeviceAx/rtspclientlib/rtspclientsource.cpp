
#include "rtspclientsource.h"

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "MediaSink.hh"
#include "MediaSource.hh"

#include <time.h>

// Forward function definitions:

// RTSP 'response handlers':
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
  // called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")




// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);
RTSPClient* openURL(UsageEnvironment& env, char const* progName, char const* rtspURL,SLW_StreamBack *h264_stream_back, void* paratemer);




//
class RTSPClientSource
{
public:
    RTSPClientSource();
    ~RTSPClientSource();
    bool addUrl(std::string url,SLW_StreamBack *stream_back,void* parameter);
    int queryState();// 0 stop 1 runing;
    bool stop();

    char m_eventLoopWatchVariable;
    bool        m_isRuning; //标记是否在取流
    friend DWORD WINAPI run(LPVOID );

private:
    bool init();
    bool m_init;

    TaskScheduler* m_scheduler;
    UsageEnvironment* m_env;
	RTSPClient *m_rtspClient;

    HANDLE      m_hThread;
    SLW_StreamBack *stream_back;
	void* m_pFirstparameter;
    bool HeartBeat();

};




DLL_HERAD HANDLE xlw_RtspClient_start(const char *url,SLW_StreamBack *stream_back, void* pFirstParameter)
{
    RTSPClientSource *client = new RTSPClientSource;
    if(client)
        client->addUrl(url,stream_back, pFirstParameter);
    return  client;
}
DLL_HERAD void xlw_RtspClient_stop(HANDLE fd)
{

    YY_DEBUG("");
    if(fd == NULL)
        return;
    RTSPClientSource *client = (RTSPClientSource *)fd;
    delete client;
    fd = NULL;
    YY_DEBUG("");

}
DLL_HERAD int xlw_RtspClient_queryState(HANDLE fd)
{
    if(fd == NULL)
        return -1;
    RTSPClientSource *client = (RTSPClientSource *)fd;
    return client->queryState();


}



//



// A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient) {
  return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}

// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) {
  return env << subsession.mediumName() << "/" << subsession.codecName();
}

void usage(UsageEnvironment& env, char const* progName) {
  env << "Usage: " << progName << " <rtsp-url-1> ... <rtsp-url-N>\n";
  env << "\t(where each <rtsp-url-i> is a \"rtsp://\" URL)\n";
}

// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:

class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;
};

// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:

class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
                  int verbosityLevel = 0,
                  char const* applicationName = NULL,
                  portNumBits tunnelOverHTTPPortNum = 0);
  SLW_StreamBack *getStreamBack();
  void* GetFirstparameter()
  {
	  return m_pFirstparameter;
  }
  void setStreamBack(SLW_StreamBack *StreamBack, void* pFirstparameter);
protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
        int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
private:
  SLW_StreamBack *m_stream_back;
  void* m_pFirstparameter;
};

// Define a data sink (a subclass of "MediaSink") to receive the data for each subsession (i.e., each audio or video 'substream').
// In practice, this might be a class (or a chain of classes) that decodes and then renders the incoming audio or video.
// Or it might be a "FileSink", for outputting the received data into a file (as is done by the "openRTSP" application).
// In this example code, however, we define a simple 'dummy' sink that receives incoming data, but does nothing with it.

class DummySink: public MediaSink {
public:
  static DummySink* createNew(UsageEnvironment& env,
                  MediaSubsession& subsession, // identifies the kind of data that's being received
                  char const* streamId = NULL); // identifies the stream itself (optional)
  SLW_StreamBack *getStreamBack();
  void* GetFirstparameter()
  {
	  return m_pFirstparameter;
  }

  void setStreamBack(SLW_StreamBack *StreamBack, void* pFirstparameter);
private:
  DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId);
    // called only by "createNew()"
  virtual ~DummySink();

  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
                struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
             struct timeval presentationTime, unsigned durationInMicroseconds);


private:
  // redefined virtual functions:
  virtual Boolean continuePlaying();

private:
  u_int8_t* fReceiveBuffer;
  MediaSubsession& fSubsession;
  char* fStreamId;
//  u_int8_t *m_avibuf;
  SLW_StreamBack *m_stream_back;
  void* m_pFirstparameter;
  bool fHaveWrittenFirstFrame;
  u_int8_t* m_h264Buffer;
  // to avi
 // unsigned char *m_buf;

};



RTSPClientSource::RTSPClientSource()
{
   m_scheduler = NULL;
   m_env = NULL;
   m_init = false;
   //m_isRuning = false;
   m_hThread = NULL;
   m_pFirstparameter = NULL;
   init();
}
//static  char m_eventLoopWatchVariable = 0;
//static bool m_isRuning = false;
DWORD WINAPI run(LPVOID p)
{
    RTSPClientSource *thread = (RTSPClientSource *)p;
//    thread->m_isRuning = true;
    YY_DEBUG("");
    thread->m_env->taskScheduler().doEventLoop(&(thread->m_eventLoopWatchVariable));
        YY_DEBUG("");
//    thread->m_isRuning = false;
   // ExitThread(4);
   return 0;
}


RTSPClientSource::~RTSPClientSource()
{
     YY_DEBUG("");
    stop();

    if(m_env)
        m_env->reclaim();

    delete m_scheduler;
}

bool RTSPClientSource::init()
{
    if(m_init)
        return false;
    m_scheduler = BasicTaskScheduler::createNew();
    if(!m_scheduler)
        return false;

    m_env = BasicUsageEnvironment::createNew(*m_scheduler);
    if(!m_env)
        return false;

    m_init = true;
    YY_DEBUG("init");
    return true;
}

bool RTSPClientSource::addUrl(std::string url,SLW_StreamBack *stream_back,void* pFirstparameter)
{
    if(m_env == NULL)
        return false;
    m_rtspClient = openURL(*m_env,"RTSPClientSource",url.c_str(),stream_back, pFirstparameter);
    m_env->renewTickCount();
    m_eventLoopWatchVariable = 0;
    m_hThread = CreateThread(NULL, 0,run,this, 0, NULL);
    return true;
}
int RTSPClientSource::queryState()
{
        DWORD dword = GetTickCount() - m_env->lastTickCount();
        if(dword > 9000)//9 miao
        {
            m_eventLoopWatchVariable = -1;
            return 0;
        }
        else
            return 1;
}


void SafeStopThread(HANDLE handl)
{
    if(handl)
    {
        int iWaitTimes = 0;
        int MAXWAITTIME = 8;
        while(WaitForSingleObject(handl, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
        {
            iWaitTimes++;
        }

        if(iWaitTimes >= MAXWAITTIME)
        {
            TerminateThread(handl, 0);
        }
        else
        {
            CloseHandle(handl);
        }
    }
    handl = NULL;
}


bool RTSPClientSource::stop()
{
    YY_DEBUG("");
    m_eventLoopWatchVariable = -1;
    SafeStopThread(m_hThread);
	 m_hThread = NULL;

	if(m_rtspClient != NULL)
	{
		fprintf(stderr, "[RTSPClient]: shutdown rtsp stream...\n");
    	shutdownStream(m_rtspClient);
		m_rtspClient = NULL;
	}

   
    YY_DEBUG("");
    return true;
}

static unsigned rtspClientCount = 0; // Counts how many streams (i.e., "RTSPClient"s) are currently in use.

#define RTSP_CLIENT_VERBOSITY_LEVEL 1 // by default, print verbose output from each "RTSPClient"

// The main streaming routine (for each "rtsp://" URL):
RTSPClient* openURL(UsageEnvironment& env, char const* progName, char const* rtspURL,SLW_StreamBack *stream_back, void* pFirstparameter)

{
  // Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
  // to receive (even if more than stream uses the same "rtsp://" URL).
  YY_DEBUG(rtspURL);
  RTSPClient* rtspClient = ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
  if (rtspClient == NULL) {
    env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
    return NULL;
  }


  ourRTSPClient *p = (ourRTSPClient*)rtspClient;
  p->setStreamBack(stream_back,pFirstparameter);


  ++rtspClientCount;

  // Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
  // Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
  // Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
  rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
  YY_DEBUG("");

	return rtspClient;
}



// Implementation of the RTSP 'response handlers':

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
    YY_DEBUG("");
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
      delete[] resultString;
      break;
    }

    char* const sdpDescription = resultString;
    env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

    // Create a media session object from this SDP description:
    scs.session = MediaSession::createNew(env, sdpDescription);
    delete[] sdpDescription; // because we don't need it anymore
    if (scs.session == NULL) {
      env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
      break;
    } else if (!scs.session->hasSubsessions()) {
      env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
      break;
    }

    // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
    // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
    // (Each 'subsession' will have its own data source.)
    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);
    return;
  } while (0);
    YY_DEBUG("");
  // An unrecoverable error occurred with this stream.
  //shutdownStream(rtspClient);
}

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP, change the following to True:
#define REQUEST_STREAMING_OVER_TCP True

void setupNextSubsession(RTSPClient* rtspClient) {
    YY_DEBUG("");
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  scs.subsession = scs.iter->next();
  if (scs.subsession != NULL) {
    if (!scs.subsession->initiate()) {
      env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
      env << *rtspClient << "Initiated the \"" << *scs.subsession
      << "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";

      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
      rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
    }
    return;
  }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.session->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
  }
  YY_DEBUG("");
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
    YY_DEBUG("");
    do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
      break;
    }
     YY_DEBUG("");
    env << *rtspClient << "Set up the \"" << *scs.subsession
    << "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";

    // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
    // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
    // after we've sent a RTSP "PLAY" command.)
     YY_DEBUG("");
    scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url());
      // perhaps use your own custom "MediaSink" subclass instead
      YY_DEBUG("");
    if (scs.subsession->sink == NULL) {
      env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
      << "\" subsession: " << env.getResultMsg() << "\n";
        YY_DEBUG("");
      break;
    }
     YY_DEBUG("");
    DummySink *p = (DummySink *)scs.subsession->sink;
    ourRTSPClient *client = (ourRTSPClient *)rtspClient;
    p->setStreamBack(client->getStreamBack(), client->GetFirstparameter());

     YY_DEBUG("");

    env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
    scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession
    scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
                       subsessionAfterPlaying, scs.subsession);
     YY_DEBUG("");
    // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
    if (scs.subsession->rtcpInstance() != NULL) {
      scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
       YY_DEBUG("");
    }
  } while (0);
  delete[] resultString;
    YY_DEBUG("");

  // Set up the next subsession, if any:
  setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
  Boolean success = False;
  YY_DEBUG("");
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
      break;
    }

    // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
    // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
    // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
    // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
    if (scs.duration > 0) {
      unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
      scs.duration += delaySlop;
      unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
      scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
    }

    env << *rtspClient << "Started playing session";
    if (scs.duration > 0) {
      env << " (for up to " << scs.duration << " seconds)";
    }
    env << "...\n";

    success = True;
  } while (0);
  delete[] resultString;
    YY_DEBUG("");

  if (!success) 
  {
    // An unrecoverable error occurred with this stream.
	  //shutdownStream(rtspClient);
  }

}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
    YY_DEBUG("");
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

  // Begin by closing this subsession's stream:
  Medium::close(subsession->sink);
  subsession->sink = NULL;

  // Next, check whether *all* subsessions' streams have now been closed:
  MediaSession& session = subsession->parentSession();
  MediaSubsessionIterator iter(session);
  while ((subsession = iter.next()) != NULL) {
    if (subsession->sink != NULL) return; // this subsession is still active
  }

  // All subsessions' streams have now been closed, so shutdown the client:
 // shutdownStream(rtspClient);
  YY_DEBUG("");
}

void subsessionByeHandler(void* clientData) {
    YY_DEBUG("");
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
  UsageEnvironment& env = rtspClient->envir(); // alias

  env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";

  // Now act as if the subsession had closed:
  subsessionAfterPlaying(subsession);
  YY_DEBUG("");
}

void streamTimerHandler(void* clientData) {
    YY_DEBUG("");
  ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
 // shutdownStream(rtspClient);
  YY_DEBUG("");
}



void shutdownStream(RTSPClient* rtspClient, int exitCode) {
    YY_DEBUG("");
    if(rtspClient == NULL)
        return ;
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  // First, check whether any subsessions have still to be closed:
  if (scs.session != NULL) {

    YY_DEBUG(rtspClient->url());
    Boolean someSubsessionsWereActive = False;
    MediaSubsessionIterator iter(*scs.session);
    MediaSubsession* subsession;
    while ((subsession = iter.next()) != NULL) {
        YY_DEBUG(rtspClient->url());
      if (subsession->sink != NULL) {
          YY_DEBUG(rtspClient->url());
    Medium::close(subsession->sink);
    subsession->sink = NULL;

    if (subsession->rtcpInstance() != NULL) {
        YY_DEBUG(rtspClient->url());
      subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
    }

    someSubsessionsWereActive = True;
      }
    }
    if (someSubsessionsWereActive) {
        YY_DEBUG(rtspClient->url());
      // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
      // Don't bother handling the response to the "TEARDOWN".
      rtspClient->sendTeardownCommand(*scs.session, NULL);
    }
  }
 // env << *rtspClient << rtspClient->url() << "Closing the stream.\n";
 // YY_DEBUG(rtspClient->url());
   Medium::close(rtspClient);
  // rtspClient = NULL;
    // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.
   YY_DEBUG("");
  if (--rtspClientCount == 0) {
    // The final stream has ended, so exit the application now.
    // (Of course, if you're embedding this code into your own application, you might want to comment this out,
    // and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
    //exit(exitCode);
      YY_DEBUG("");
  }
}


// Implementation of "ourRTSPClient":

ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
                    int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
  return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

SLW_StreamBack *ourRTSPClient::getStreamBack()
{
    return m_stream_back;
}

void ourRTSPClient::setStreamBack(SLW_StreamBack *StreamBack,void* pFirstparameter)
{
    m_stream_back = StreamBack;
	m_pFirstparameter = pFirstparameter;
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
                 int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) {
    m_stream_back = NULL;
}

ourRTSPClient::~ourRTSPClient() {
    m_stream_back = NULL;
}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
  : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
  delete iter;
  if (session != NULL) {
    // We also need to delete "session", and unschedule "streamTimerTask" (if set)
    UsageEnvironment& env = session->envir(); // alias

    env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
    Medium::close(session);
  }
}


// Implementation of "DummySink":

// Even though we're not going to be doing anything with the incoming data, we still need to receive it.
// Define the size of the buffer that we'll use:
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 5*1024*1024

DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId) {
  return new DummySink(env, subsession, streamId);

}

SLW_StreamBack *DummySink::getStreamBack()
{
    return m_stream_back;
}

void DummySink::setStreamBack(SLW_StreamBack *StreamBack, void* pFirstparameter)
{
    m_stream_back = StreamBack;
	m_pFirstparameter = pFirstparameter;
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
  : MediaSink(env),
    fSubsession(subsession) {
    YY_DEBUG("");
  fStreamId = strDup(streamId);
  fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
  m_stream_back = NULL;
   YY_DEBUG("");
//  m_avibuf = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
  m_h264Buffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
  fHaveWrittenFirstFrame = false;
   YY_DEBUG("");



}

DummySink::~DummySink() {
  delete[] fReceiveBuffer;
  delete[] fStreamId;
  delete[] m_h264Buffer;
//  delete[] m_avibuf;
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
                  struct timeval presentationTime, unsigned durationInMicroseconds) {
  DummySink* sink = (DummySink*)clientData;
  sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}







void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
                  struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
  // We've just received a frame of data.  (Optionally) print out information about it:


    //  YY_DEBUG("");
    if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
      envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
      // return;
    }
    ///////////////////////////////////////////
    /*
    static unsigned int i = 0;
   //   qDebug() << frameSize ;//<< hex << frame[4] << frame[5];
    char buf[32];
    sprintf(buf,"2641/h264_%d_%d_%d",i++,fReceiveBuffer[0],fReceiveBuffer[1]);

    FILE *FP;
    if(( FP = fopen(buf,"ab+"))==NULL)
    {
        YY_DEBUG("2641.264!\n");
        return;
    }

    fwrite(fReceiveBuffer,1, frameSize, FP);
    fclose(FP);*/
    /////////////////////////////////////////
   // time_t time1 =presentationTime.tv_sec;
    //YY_DEBUG(ctime(&time1));

    //  qDebug() << fSubsession.codecName();;
    envir().renewTickCount();
    if(strcmp(fSubsession.mediumName(),"video")== 0)
    {
        FRAME_HEAD_S head;
        memset(&head,0,sizeof(head));
        //qDebug() << presentationTime.tv_sec<< ":" << presentationTime.tv_usec;
      //  qint64 time = presentationTime.tv_sec;
      //  time = time <<32 | presentationTime.tv_usec;
        head.TimeStamp = presentationTime.tv_sec ;
        head.TimeStamp = head.TimeStamp *1000 +presentationTime.tv_usec /1000;
        head.videoInfo.u16ImageWidth = fSubsession.videoWidth();
        head.videoInfo.u16ImageHeight = fSubsession.videoHeight();
        head.videoInfo.u32FrameRate = fSubsession.videoFPS();

        if(strcmp(fSubsession.codecName(),"H264")== 0)
        {
                if(m_stream_back)
                {
                    char start_code[4] = {0x00,0x00,0x00,0x01};
                    memset(m_h264Buffer,0,DUMMY_SINK_RECEIVE_BUFFER_SIZE);

                    head.videoInfo.u16EncodeType = 0;
                    int nal_type=fReceiveBuffer[0]&0x1f;


                    unsigned char *p = &fReceiveBuffer[0];
                    unsigned char *end = &fReceiveBuffer[frameSize];
                    unsigned size = frameSize;

                    static int typ_sps = 0;
                    static int typ_pps = 0;
                    static int typ_sei = 0;
                    static int typ_idr = 0;
                    static unsigned char p_buffer[1*1024*1024] = {0};   // 临时组装缓冲区
                    static int total_size = 0;

                    // 测试代码，发送所有的帧，不判断
#if 0
                    head.u16FrameType = nal_type;
                    memcpy(m_h264Buffer,start_code,4);
                    memcpy(m_h264Buffer+4,fReceiveBuffer,frameSize);
                    frameSize = 4 + frameSize;
                    m_stream_back(m_h264Buffer,frameSize,head);
#endif
                    if(nal_type == 7)// SPS
                    {
                        typ_sps = 1;
                        memcpy(p_buffer,start_code,4);
                        memcpy(p_buffer+4,fReceiveBuffer,size);
                        total_size = 4 + size;
                    }
                    else if (nal_type == 8) // PPS
                    {
                        typ_pps = 1;
                        memcpy(p_buffer+total_size,start_code,4);
                        memcpy(p_buffer+total_size+4,fReceiveBuffer,size);
                        total_size += 4 + size;
                    }
                    else if (nal_type == 6) // SEI
                    {
                        typ_sei = 1;
                        memcpy(p_buffer+total_size,start_code,4);
                        memcpy(p_buffer+total_size+4,fReceiveBuffer,size);
                        total_size += 4 + size;
                    }
                    else if (nal_type == 5) // IDR
                    {
                        typ_idr = 1;
                        //qDebug() << "IIIII" << i++;
                        //head.u16FrameType = 1;//I
                        memcpy(p_buffer+total_size,start_code,4);
                        memcpy(p_buffer+total_size+4,fReceiveBuffer,size);
                        total_size += 4 + size;
                    }
                    else if (nal_type == 1) // P
                    {
                        //qDebug() << "xxxx" << i++ << nal_type;
                        head.u16FrameType = 3;//P
                        memcpy(p_buffer,start_code,4);
                        memcpy(p_buffer+4,fReceiveBuffer,size);
                        size = 4 + size;
                        m_stream_back(m_pFirstparameter, p_buffer,size,head);
                    }

                    // 注：必须将sps、pps、sei、idr这几个组装成“I帧”传输，
                    // 视频才不会卡顿
                    if (typ_sps && typ_pps && typ_sei && typ_idr)
                    {
                        typ_sps = 0;
                        typ_pps = 0;
                        typ_sei = 0;
                        typ_idr = 0;
                        head.u16FrameType = 1;  // mark it to I
                        memcpy(m_h264Buffer,p_buffer,total_size);
                        m_stream_back(m_pFirstparameter, m_h264Buffer,total_size,head);

                        total_size = 0;
                    }
                }
        }
        else if(strcmp(fSubsession.codecName(),"JPEG")== 0)
        {


            if(m_stream_back)
            {
                head.videoInfo.u16EncodeType = 2;
                 m_stream_back(m_pFirstparameter, fReceiveBuffer,frameSize,head);
            }

        }
        else if(strcmp(fSubsession.codecName(),"MPV")== 0)
        {
            if(m_stream_back)
            {
                 m_stream_back(m_pFirstparameter,fReceiveBuffer,frameSize,head);
            }

        }
        else
        {
            if(m_stream_back)
            {
                 m_stream_back(m_pFirstparameter,fReceiveBuffer,frameSize,head);
            }
            YY_DEBUG(fSubsession.codecName());
        }

    }
    else //NO VIDEO
    {}

  // Then continue, to request the next frame of data:
  continuePlaying();
}


Boolean DummySink::continuePlaying() {
  if (fSource == NULL) return False; // sanity check (should not happen)
    YY_DEBUG("");
  // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
  fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
                        afterGettingFrame, this,
                        onSourceClosure, this);
  YY_DEBUG("");

  return True;
}
