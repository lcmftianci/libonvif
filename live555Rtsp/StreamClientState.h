#pragma once

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "Boolean.hh"

class StreamClientState
{
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