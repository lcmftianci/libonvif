#include "DummySink.h"
#include "H264_2_RGB.h"



DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId) 
{
	return new DummySink(env, subsession, streamId);
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId): MediaSink(env),	fSubsession(subsession)
{
	//fFileSink = NULL;
	fStreamId = strDup(streamId);
	fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
	decBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
}

DummySink::~DummySink() 
{
	delete[] fReceiveBuffer;
	delete[] fStreamId;
	//H264_Release();
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds) 
{
	DummySink* sink = (DummySink*)clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

FILE *fp = NULL;
int nSize = 0;

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
	// We've just received a frame of data.  (Optionally) print out information about it:
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
	if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
	envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";
	if (numTruncatedBytes > 0) envir() << " (with " << numTruncatedBytes << " bytes truncated)";
	char uSecsStr[6 + 1]; // used to output the 'microseconds' part of the presentation time
	sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
	envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;

	if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
		envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
	}
#ifdef DEBUG_PRINT_NPT
	envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
	envir() << "\n";
#endif

	// Then continue, to request the next frame of data:
	nSize = frameSize;
	//if (fp == NULL)
	//	fp = fopen("1.h264", "wb");

	//fwrite(fReceiveBuffer, 1, nSize, fp);
	/*cout << "/////////////////////////////////////////"<< fReceiveBuffer << endl;*/
	//printf("%s", fReceiveBuffer);
	unsigned char* RgbBuf;
	unsigned int RgbSize = 0;
	int RgbWidth = 0;
	int RgbHeight = 0;

	unsigned char start_code[] = { 0x00, 0x00, 0x00, 0x01 };
	memset(decBuffer, 0, DUMMY_SINK_RECEIVE_BUFFER_SIZE);
	if (strcmp(fSubsession.mediumName(), "video") == 0 && strcmp(fSubsession.codecName(), "H264") == 0)
	{
		int nTotalLen = 0;
		unsigned int num = 1;
		SPropRecord * sps = parseSPropParameterSets(fSubsession.fmtp_spropparametersets(), num);
		for (int i = 0; i < num; i++)
		{
			memcpy(decBuffer + nTotalLen, start_code, 4);
			nTotalLen += 4;
			memcpy(decBuffer + nTotalLen, sps[i].sPropBytes, sps[i].sPropLength);
			nTotalLen += sps[i].sPropLength;
		}
		delete[] sps;
		memcpy(decBuffer + nTotalLen, start_code, 4);
		nTotalLen += 4;
		memcpy(decBuffer + nTotalLen, fReceiveBuffer, frameSize);
		nTotalLen += frameSize;
		H264_2_RGB(decBuffer, nTotalLen, RgbBuf, &RgbSize, &RgbWidth, &RgbHeight);
	}
	continuePlaying();
}



Boolean DummySink::continuePlaying() 
{
	if (fSource == NULL) return false; // sanity check (should not happen)

									   // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
		afterGettingFrame, this,
		onSourceClosure, this);
	return true;
}
