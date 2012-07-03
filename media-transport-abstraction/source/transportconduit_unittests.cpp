/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: snandaku@cisco.com

#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

#include "mozilla/Scoped.h"
#include <MediaTransportAbstraction.h>
#include "nsStaticComponents.h"

#include "resource_mgr.h"

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"



// Defining this here, usually generated for libxul
// should not be needed by these tests
const mozilla::Module *const *const kPStaticModules[] = {
  NULL
};

// This class acts as source for providing PCM audio samples
// In the actual usage the samples come from MediaStream
class AudioSamplesSource 
{
public:
  void Init(mozilla::RefPtr<mozilla::AudioSessionConduit> aSession, std::string file)
   {
	mSession = aSession;	
	aFile = file;
   }

  void GenerateSamples();
private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mSession;
  std::string aFile;
};

void AudioSamplesSource::GenerateSamples()
{
  int16_t audio10ms[160]; // samples sent to the transport
  char audioBuffer[320]; //160 samples of 2 bytes each
  ifstream aStream;
  const int sample_length  = 160;

  memset(audio10ms, 0, 160 * sizeof(short));
  memset(audioBuffer, 0, 160 * sizeof(short));

  aStream.open(aFile.c_str(), ios::binary);
  aStream.seekg(0, ios::end); 
  int length = aStream.tellg();
  cout << " length of the file " << length;
  aStream.seekg(0,ios::beg);

  streamsize curPos = 0;
  if (aStream.is_open())
  {
    while ( aStream.good() )
    {
	   	usleep(10*1000);
		//std::cout << " current file pointer is " << aStream.tellg();
        streamsize sz = 320; 
	    aStream.get(audioBuffer, sz);	
		memcpy(audio10ms, audioBuffer, 320);
		//Let our conduit know off the audio samples
    	mSession->SendAudioFrame(audio10ms,sample_length, 16000, 0);
  		memset(audio10ms, 0, 160 * sizeof(short));
  		memset(audioBuffer, 0, 160 * sizeof(short));
		curPos += 320;
		aStream.seekg(curPos, ios::beg);
    }
    std::cout << " Done Reading the file " << std::endl;
    aStream.close();
  } else 
  {
	std::cout << "Opening file failed"<< std::endl;
  }

}

// Dummy Media Target for the conduit 
class DummyAudioTarget: public mozilla::AudioRenderer
{
public:
  DummyAudioTarget()
  {
  	std::cerr << " DummyAudioTarget created " << std::endl;
  }

  virtual ~DummyAudioTarget()
  {
  	std::cerr << " Deleting Dummy Audio Renderer " << std::endl;
  }

 virtual void RenderAudioFrame(int16_t speechData[],
						        uint32_t samplingFreqHz,
							    int lengthSample)
  {
  	std::cerr << "RenderAudioFrame Called: " << std::endl;
  }
};

//Fake Transport Audio
class FakeAudioTransport : public mozilla::TransportInterface 
{
public:
  virtual ~FakeAudioTransport()
  {
  	std::cout << " Deleting Fake Audio Transport " << std::endl;
  }

  virtual int SendRtpPacket(const void* data, int len)
  {
  	std::cout << " FAT: SendRtpPacket: Len " << len << std::endl;
    // we just return thus obtained packet back to the engine 
    // for decoding and eventual playing
    mSession->ReceivedRTPPacket(data,len);
    return 0;
  }

  virtual int SendRtcpPacket(const void* data, int len)
  {
  	std::cout << " FAT: SendRtcpPacket: " << std::endl;
    mSession->ReceivedRTCPPacket(data,len);
    return 0;
  }

  // only for testing ..
  FakeAudioTransport(mozilla::RefPtr<mozilla::AudioSessionConduit> aSession):
								mSession(aSession)
  {
  }

private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mSession;

};


namespace {

class TransportConduitTest : public ::testing::Test {
 public:
  TransportConduitTest() 
  {
    filename = "audio_short16.pcm";
     std::string rootpath = ProjectRootPath();
   fileToPlay = rootpath+"media"+kPathDelimiter+"webrtc"+kPathDelimiter+"trunk"+kPathDelimiter+"test"+kPathDelimiter+"data"+kPathDelimiter+"voice_engine"+kPathDelimiter+filename;
   std::cout << " Filename is " << fileToPlay << std::endl;
  }

  ~TransportConduitTest() 
  {
  }

  //2. Dump audio samples to dummy external transport
  void TestDummyMediaAndTransport() 
  {
    int error = 0;
    //get pointer to AudioSessionConduit
    mAudioSession = mozilla::AudioSessionConduit::Create();
    if( !mAudioSession )
      ASSERT_TRUE(mAudioSession != NULL);
    std::cerr << " AudioSession is " << mAudioSession << std::endl;
    // create audio-renderer, audio-transport
    mAudioRenderer = new DummyAudioTarget();
    mAudioTransport = new FakeAudioTransport(mAudioSession);
    // attach the transport and renderer to audio-conduit
    mAudioSession->AttachRenderer(mAudioRenderer);
    mAudioSession->AttachTransport(mAudioTransport);
    //configure send and recv codecs on the audio-conduit
    mozilla::CodecConfig cinst;
    cinst.mChannels = 1;
    cinst.mName = "ISAC";
    cinst.mType = 103;
    cinst.mRate = -1; // default rate
    cinst.mPacSize = 480; // 30ms
    cinst.mFreq = 16000;
    mAudioSession->ConfigureSendMediaCodec(&cinst);
    mAudioSession->ConfigureRecvMediaCodec(&cinst);
    //start generating samples
    audioSource.Init(mAudioSession, fileToPlay);
    audioSource.GenerateSamples();
  }


private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mAudioSession;
  mozilla::RefPtr<mozilla::AudioRenderer> mAudioRenderer;
  mozilla::RefPtr<mozilla::TransportInterface> mAudioTransport;
  AudioSamplesSource audioSource;
  std::string fileToPlay;
  std::string filename;
};


// Test 1: Play a file to the speaker
//TEST_F(WebrtcTest, TestSpeakerPlayout) {
 // TestAudioFileLocalPlayout();
//}

// Test 2: Test Dummy External Xport
TEST_F(TransportConduitTest, TestDummyMediaWithTransport) {
  TestDummyMediaAndTransport();
}

//TEST_F(WebrtcTest, TestDummyExternalXportPlayback) {
// TestAudioFilePlayoutExternalTransport();
//}


}  // end namespace

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  //base::AtExitManager exit_manager;
  return RUN_ALL_TESTS();
}


