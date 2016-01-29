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
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2014 Live Networks, Inc.  All rights reserved.
// A file source that is a plain byte stream (rather than frames)
// Implementation

#include "ByteStreamSocketSource.hh"
#include "InputFile.hh"
#include "GroupsockHelper.hh"

//for C socket
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

////////// ByteStreamSocketSource //////////

ByteStreamSocketSource*
ByteStreamSocketSource::createNew(UsageEnvironment& env, char const* fileName,
				unsigned preferredFrameSize,
				unsigned playTimePerFrame) {

	struct sockaddr_un local;
	struct timeval timeout = {4, 0}; // 4 s
	int ret = 0;
	int sockfd = socket(PF_UNIX, SOCK_STREAM, 0);   
	if(sockfd < 0){   
		fprintf(stderr, "create sockfd error\n");
		return NULL;
	}      

	int nRecvBuf=128*1024;	//receive buffer : 128 KB
	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,
			(const char*)&nRecvBuf,sizeof(int));
	setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO,
			   (char *)&timeout, sizeof(struct timeval));

	memset(&local, 0, sizeof(struct sockaddr_un));
	local.sun_family	= AF_UNIX;
	strncpy(local.sun_path, fileName, strlen(fileName));
	//unlink(fileName);
	int addrlen = strlen(local.sun_path) + sizeof(local.sun_family);

	//connect server   
	ret = connect(sockfd, (struct sockaddr*)&local, addrlen);   
	if(ret < 0){   
		fprintf(stderr, "[RTSP %s - %d] connect error!\n", __FILE__, __LINE__);
		::close(sockfd);   
		return NULL;   
	}   

  ByteStreamSocketSource* newSource = new ByteStreamSocketSource(env, sockfd, 
		  			preferredFrameSize, playTimePerFrame);

  return newSource;
}

ByteStreamSocketSource*
ByteStreamSocketSource::createNew(UsageEnvironment& env, int sockfd,
				unsigned preferredFrameSize,
				unsigned playTimePerFrame) {
  if (sockfd < 0) return NULL;

  ByteStreamSocketSource* newSource = new ByteStreamSocketSource(env, sockfd, 
		  			preferredFrameSize, playTimePerFrame);

  return newSource;
}

/*
void ByteStreamSocketSource::seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream) {
  SeekFile64(fFid, (int64_t)byteNumber, SEEK_SET);

  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

void ByteStreamSocketSource::seekToByteRelative(int64_t offset) {
  SeekFile64(fFid, offset, SEEK_CUR);
}

void ByteStreamSocketSource::seekToEnd() {
  SeekFile64(fFid, 0, SEEK_END);
}
*/

ByteStreamSocketSource::ByteStreamSocketSource(UsageEnvironment& env, 
		int sockfd,
		unsigned preferredFrameSize,
		unsigned playTimePerFrame)
	: FramedSource(env), fSockfd(sockfd), fFileSize(0), 
	fPreferredFrameSize(preferredFrameSize),
	fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0),
	fHaveStartedReading(False), fLimitNumBytesToStream(False), 
	fNumBytesToStream(0) {
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
	makeSocketNonBlocking(fSockfd);
#endif

	// Test whether the file is seekable
	//fFidIsSeekable = FileIsSeekable(fFid);
	fFidIsSeekable = False;
}

ByteStreamSocketSource::~ByteStreamSocketSource() {
  if (fSockfd < 0) return;

#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  envir().taskScheduler().turnOffBackgroundReadHandling(fSockfd);
#endif

  ::close(fSockfd);
//  CloseInputFile(fFid);
}

unsigned int ByteStreamSocketSource::dateTimeGetTick(void)
{
	unsigned int tick; 
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

	return tick;
}


void ByteStreamSocketSource::doGetNextFrame() {
  if ((fLimitNumBytesToStream && fNumBytesToStream == 0)) {
    //handleClosure();
    return;
  }

#ifdef READ_FROM_FILES_SYNCHRONOUSLY
  doReadFromSocket();
#else
  if (!fHaveStartedReading) {
    // Await readable data from the file:
    envir().taskScheduler().turnOnBackgroundReadHandling((fSockfd),
	       (TaskScheduler::BackgroundHandlerProc*)&socketReadableHandler, this);
    fHaveStartedReading = True;
  }
#endif
}

void ByteStreamSocketSource::doStopGettingFrames() {
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  envir().taskScheduler().turnOffBackgroundReadHandling(fSockfd);
  fHaveStartedReading = False;
#endif
}

void ByteStreamSocketSource::socketReadableHandler(ByteStreamSocketSource* source, int /*mask*/) {
  if (!source->isCurrentlyAwaitingData()) {
    source->doStopGettingFrames(); // we're not ready for the data yet
    return;
  }
  source->doReadFromSocket();
}

void ByteStreamSocketSource::doReadFromSocket() {
  // Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
  if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fMaxSize) {
    fMaxSize = (unsigned)fNumBytesToStream;
  }
  if (fPreferredFrameSize > 0 && fPreferredFrameSize < fMaxSize) {
    fMaxSize = fPreferredFrameSize;
  }

  tFrameHead frameHead;
  int frameHeadLength = 0;
  memset(&frameHead, 0, sizeof(tFrameHead));

  frameHeadLength = recv(fSockfd, &frameHead, sizeof(tFrameHead), 0);
  if(frameHeadLength != sizeof(tFrameHead) || frameHeadLength < 0){
	if(frameHeadLength == 0){
		handleClosure();
	}
	else if(frameHeadLength < 0){
		fprintf(stderr, "Receive frame length error, (errno:%d %s)\n", 
						errno, strerror(errno));
		handleClosure();
	}else if(frameHeadLength != sizeof(tFrameHead)){
		fprintf(stderr, "ERROR: frame length(%d), should be %d \n", 
					frameHeadLength, sizeof(tFrameHead));
	}
  	return;
  }

  if (FRAME_HEAD_SYNC != frameHead.bySync)
  {
	  // If the sync sign in error, receive rest of the data then discard that all
	  int tmpBufLen = 64*1024; //64KB
	  char *tmpBuf = new char[tmpBufLen];
	  if(tmpBuf != NULL){
		  recv(fSockfd, tmpBuf, tmpBufLen, 0);
		  delete tmpBuf;
	  }
	  fprintf(stderr, "ERROR: sync failed\n");
	  return;
  }

  unsigned int iNeedLen = frameHead.unDataLen;
  unsigned int iRecvLen = 0;
  int ret = 0;

  if(iNeedLen > fMaxSize)
	  iNeedLen = fMaxSize;

  do
  {
	  ret = recv(fSockfd, fTo + iRecvLen, iNeedLen, 0);
	  if (ret < 0)
	  {
		  if(EAGAIN == errno)
			  continue;
		  fprintf(stderr, "recv data failed,need len %d\n",iNeedLen);
		  return;
	  }
	  if (0 == ret)
	  {
		  fprintf(stderr, "[RTSP %s-%d]: Recv len 0\n", __FILE__, __LINE__);
		  handleClosure();
		  return;
	  }

	  iRecvLen += ret;
	  iNeedLen -= ret;
	  if (0 == iNeedLen || iRecvLen == frameHead.unDataLen)
		  break;

  }while(1);

  
  fFrameSize = frameHead.unDataLen;
  //fFrameSize = recv(fSockfd, fTo, fMaxSize, 0);

  /*
  unsigned int curTime = dateTimeGetTick();
  if(curTime > frameHead.unTimeStamp){
	  fprintf(stdout, "curTime:%d, frameTime:%d diff:%d frameSize:%d\n", 
			  curTime, frameHead.unTimeStamp, 
			  curTime - frameHead.unTimeStamp,
			  frameHead.unDataLen);
  }
  */

  /*
  if (fFrameSize == 0) {
    handleClosure();
    return;
  }
  */
  //FIXME
  if(fFrameSize < 0){
	  fFrameSize = 0;
	  fprintf(stderr, "get fFrameSize error:%d\n", fFrameSize);
	  handleClosure();
	  return;
  } else if(fFrameSize == 0){
	return;
  } else if(fFrameSize > fMaxSize){
	  fprintf(stderr, "fFrameSize biger than fMaxSize\n");
  	return;
  }
  fNumBytesToStream -= fFrameSize;

  // Set the 'presentation time':
  if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0) {
    if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
      // This is the first frame, so use the current time:
      gettimeofday(&fPresentationTime, NULL);
    } else {
      // Increment by the play time of the previous data:
      unsigned uSeconds	= fPresentationTime.tv_usec + fLastPlayTime;
      fPresentationTime.tv_sec += uSeconds/1000000;
      fPresentationTime.tv_usec = uSeconds%1000000;
    }

    // Remember the play time of this data:
    fLastPlayTime = (fPlayTimePerFrame*fFrameSize)/fPreferredFrameSize;
    fDurationInMicroseconds = fLastPlayTime;
  } else {
    // We don't know a specific play time duration for this data,
    // so just record the current time as being the 'presentation time':
    gettimeofday(&fPresentationTime, NULL);
  }

  // Inform the reader that he has data:
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
  // To avoid possible infinite recursion, we need to return to the event loop to do this:
  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
				(TaskFunc*)FramedSource::afterGetting, this);
#else
  // Because the file read was done from the event loop, we can call the
  // 'after getting' function directly, without risk of infinite recursion:
  FramedSource::afterGetting(this);
#endif
}
