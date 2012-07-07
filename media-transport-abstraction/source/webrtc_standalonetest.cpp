/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: snandaku@cisco.com

#include <iostream>
#include <string>
#include <unistd.h>



#include "mozilla/Scoped.h"
#include <MediaTransportAbstraction.h>

#include "nsCOMPtr.h"
#include "nsNetCID.h"
#include "nsXPCOMGlue.h"
#include "nsXPCOM.h"
#include "nsIComponentManager.h"
#include "nsIComponentRegistrar.h"
#include "nsIIOService.h"
#include "nsIServiceManager.h"
#include "nsISocketTransportService.h"
#include "nsServiceManagerUtils.h"
#include "nsComponentManagerUtils.h"
#include "nsStaticComponents.h"
#include "nsITimer.h"

#include "common_types.h"
#include "voice_engine/main/interface/voe_base.h"
#include "voice_engine/main/interface/voe_file.h"
#include "voice_engine/main/interface/voe_hardware.h"
#include "voice_engine/main/interface/voe_codec.h"
#include "voice_engine/main/interface/voe_external_media.h"



#include "video_engine/include/vie_base.h"
#include "video_engine/include/vie_codec.h"
#include "video_engine/include/vie_render.h"
#include "video_engine/include/vie_capture.h"

//webrtc test utilities
#include "resource_mgr.h"

class WebRtcAudioDeviceImpl;

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"



// Defining this here, usually generated for libxul
// should not be needed by these tests
const mozilla::Module *const *const kPStaticModules[] = {
  NULL
};

//Dummy External Transport - Was dumping purposes only
class DummyExternalTransport: public webrtc::Transport 
{
public:
   DummyExternalTransport():rtpCount(0),
			    rtcpCount(0)
   {
   }

  ~DummyExternalTransport() {};

protected:
  virtual int SendPacket(int channel, const void* data, int len) {
    rtpCount++;
    return 0;
  }

  virtual int SendRTCPPacket(int channel, const void* data, int len) {
    rtcpCount++;
    return 0;
  }

public:
  int rtpCount;
  int rtcpCount;

};

// This class acts as source for providing PCM audio samples
// In the actual usage the samples come from MediaStream

class AudioSamplesSource 
{
public:
   void Init(mozilla::RefPtr<mozilla::AudioSessionConduit> aSession,
	      webrtc::VoEExternalMedia* mPtrVoEXmedia)
   {
  		mSession = aSession;	
	stop = false;
	mVoEXmedia = mPtrVoEXmedia;
        
   }

   AudioSamplesSource(): lck(PR_NewLock())
   {
   }
   
   void Start();
   void Stop();
   bool stop;
  mozilla::RefPtr<mozilla::AudioSessionConduit> mSession;
  webrtc::VoEExternalMedia* mVoEXmedia;
  PRLock* lck;

};

void AudioSamplesSource::Start()
{
  int t = 5000; // 5 seconds
  int curT = 0;
  while(curT < t)
  {
    usleep(10 * 1000);
    curT += 5;
    printf("\n Done Sleeping %d ", curT);
    // just one audio sample
 	static int16_t audio10ms[160];
 	static int16_t dest[160];
  	int sample_length  =0;
  	memset(audio10ms, 0, 160 * sizeof(short));
    mVoEXmedia->ExternalPlayoutGetData(audio10ms, 16000, 10, sample_length);
    if(sample_length == 0)
    {
     std::cout << " Sample Length is 0 ";
     return;
    }
    PR_Lock(lck);
    std::cout << " Sample Length is  " << sample_length << std::endl;

    mSession->SendAudioFrame(audio10ms,sample_length, 16000, 0);
    PR_Unlock(lck);
  }
}

void AudioSamplesSource::Stop()
{

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
  	std::cout << " FAT: SendRtpPacket: " << std::endl;
        return 0;
  }

  virtual int SendRtcpPacket(const void* data, int len)
  {
  	std::cout << " FAT: SendRtcpPacket: " << std::endl;
        return 0;
  }

};

namespace {

class WebrtcTest : public ::testing::Test {
 public:
  WebrtcTest():mVoiceEngine(NULL),
   		mPtrVoEBase(NULL),
   		mPtrVoEBase2(NULL),
   		mPtrVoEFile(NULL),
   		mChannel(-1),
   		initDone(false) {
  filename = "audio_short16.pcm";
  }

  ~WebrtcTest() 
  {
  	Cleanup();
  }

  void Init() 
  {
   if( initDone )
  	return; 

   std::cerr << "  Creating Voice Engine " << std::endl;
   mVoiceEngine = webrtc::VoiceEngine::Create();
   ASSERT_TRUE(mVoiceEngine != NULL);

   mPtrVoEBase = webrtc::VoEBase::GetInterface(mVoiceEngine);
   ASSERT_TRUE(mPtrVoEBase != NULL);
   int res = mPtrVoEBase->Init();
   ASSERT_EQ(0, res);

   mPtrVoEFile = webrtc::VoEFile::GetInterface(mVoiceEngine);
   ASSERT_TRUE(mPtrVoEFile != NULL);

   mPtrVoEXmedia = webrtc::VoEExternalMedia::GetInterface(mVoiceEngine);
   ASSERT_TRUE(mPtrVoEXmedia != NULL);

   mPtrVoECodec = webrtc::VoECodec::GetInterface(mVoiceEngine);
   ASSERT_TRUE(mPtrVoECodec != NULL);

   //temp logging
   mVoiceEngine->SetTraceFilter(webrtc::kTraceAll);
   mVoiceEngine->SetTraceFile( "Voevideotrace.out" );


   std::string rootpath = ProjectRootPath();
   fileToPlay = rootpath+"media"+kPathDelimiter+"webrtc"+kPathDelimiter+"trunk"+kPathDelimiter+"test"+kPathDelimiter+"data"+kPathDelimiter+"voice_engine"+kPathDelimiter+filename;
   initDone = true;
 }

  void Cleanup()
  {
    std::cerr << " Cleanup the Engine " << std::endl;
    int error = 0;


    //release interfaces
    error = mPtrVoEBase->Terminate();
    ASSERT_TRUE(error != -1);


    error = mPtrVoEFile->Release();
    ASSERT_TRUE(error != -1);


    error  = mPtrVoEXmedia->Release();
    ASSERT_TRUE(error != -1);

    error  = mPtrVoEBase->Release();
    ASSERT_TRUE(error != -1);

    // delete the engine 
    webrtc::VoiceEngine::Delete(mVoiceEngine);

    initDone = false;
  }

  //1. Play audio file to speaker
  void TestAudioFileLocalPlayout() 
  {
    Init();
    mChannel = mPtrVoEBase->CreateChannel();
    ASSERT_TRUE(mChannel != -1);
    std::cerr << " Playing audio for 5 Seconds " << std::endl;
    StartMedia(0); 
    sleep(5);
    StopMedia();
    int error = mPtrVoEBase->DeleteChannel(mChannel);
    ASSERT_TRUE(error != -1);
  }

  //2. Dump audio samples to dummy external transport
  void TestDummyMediaAndTransport() 
  {
    int error = 0;
    Init();
    mChannel = mPtrVoEBase->CreateChannel();
    int channel = mPtrVoEBase2->CreateChannel();
    std::cerr << " Channel created --> " << mChannel <<  std::endl;
    std::cerr << " Channel created --> " << channel <<  std::endl;
    ASSERT_TRUE(mChannel != -1);
#if 0
    //get pointer to AudioSessionConduit
    mAudioSession = mozilla::AudioSessionConduit::Create();
   if( !mAudioSession )
     ASSERT_TRUE(mAudioSession != NULL);
   std::cerr << " AudioSession is " << mAudioSession << std::endl;
   int res = 0;
    mAudioSession->SetEngineAndChannel((void*) mVoiceEngine, mChannel);
    std::cerr << "Setting Up Audio Session Conduit " << std::endl;
    mAudioSession->AttachRenderer(new DummyAudioTarget());
    mAudioSession->AttachTransport(new FakeAudioTransport());
    mozilla::CodecConfig cinst;
    cinst.mChannels = 1;
    cinst.mName = "ISAC";
    cinst.mType = 103;
    cinst.mRate = -1; // default rate
    cinst.mPacSize = 480; // 30ms
    cinst.mFreq = 16000;
    mAudioSession->ConfigureSendMediaCodec(&cinst);
    mAudioSession->ConfigureRecvMediaCodec(&cinst);
    std::cerr << " Done Configuring AudioSessionConnduit " << std::endl;
    audioSource.Init(mAudioSession, mPtrVoEXmedia);
    error = mPtrVoEXmedia->SetExternalPlayoutStatus(true);
    ASSERT_TRUE(error != -1);
    StartMedia(1);
    std::cout << " SUHAS SUHAS SUHAS " << std::endl;
    StopMedia(); 
    error = mPtrVoEBase->DeleteChannel(mChannel);
    ASSERT_TRUE(error != -1);
#endif
  }

  // Play audio samples from a file to the speaker 
  // via the external transport
  void TestAudioFilePlayoutExternalTransport()
  {
   /*
    int error = 0;
    Init();
    mChannel = mPtrVoEBase->CreateChannel();
    ASSERT_TRUE(mChannel != -1);
    ScopedDeletePtr<WebrtcExternalTransport> extXport;
    extXport = new WebrtcExternalTransport(mPtrVoENetwork);
    ASSERT_TRUE(extXport != NULL);
    error  = mPtrVoENetwork->RegisterExternalTransport(mChannel, *extXport);
    ASSERT_TRUE(error != -1);
    StartMedia(true);
    std::cerr << " Dumping audio packet info for 5 Seconds " << std::endl;
    sleep(5);
    StopMedia();
    error = mPtrVoENetwork->DeRegisterExternalTransport(mChannel);
    ASSERT_TRUE(error != -1);
    error = mPtrVoEBase->DeleteChannel(mChannel);
    ASSERT_TRUE(error != -1);
   */
  }

  void StartMedia(int choice) {
    int error = 0;
    std::cerr << " Starting the media flow " << std::endl;
    const int DEFAULT_PORT = 55555;
    switch(choice)
    {
		//local playout
  		case 0:
			error = mPtrVoEBase->SetLocalReceiver(mChannel,DEFAULT_PORT);
        	ASSERT_TRUE(error != -1);
        	error = mPtrVoEBase->SetSendDestination(mChannel,DEFAULT_PORT,"127.0.0.1");
        	ASSERT_TRUE(error != -1);
    		error = mPtrVoEBase->StartReceive(mChannel);
    		ASSERT_TRUE(error != -1);
    		error = mPtrVoEBase->StartSend(mChannel);
    		ASSERT_TRUE(error != -1);
		break;
		case 1:
			webrtc::CodecInst cinst;
            cinst.channels = 1;
  			strcpy(cinst.plname, "ISAC");
  			cinst.pltype = 103;
  			cinst.rate = -1; // default rate
  			cinst.pacsize = 480; // 30ms
  			cinst.plfreq = 16000;
            //rror = mPtrVoECodec->SetSendCodec(mChannel, cinst);
        //	error = mPtrVoEBase->SetSendDestination(mChannel,DEFAULT_PORT,"127.0.0.1");
        //	ASSERT_TRUE(error != -1);
    	//	error = mPtrVoEBase->StartSend(mChannel);
    	//	ASSERT_TRUE(error != -1);
		break;
		
    }

    error = mPtrVoEBase->StartPlayout(mChannel);
    ASSERT_TRUE(error != -1);
    
    error = mPtrVoEFile->StartPlayingFileAsMicrophone(mChannel,
							fileToPlay.c_str(),
							false,
							false);
    ASSERT_TRUE(error != -1);

   //start our timeout for grabbing samples
   if(choice == 1)
   {
  	audioSource.Start();
   }

  } 

  void StopMedia()
  {
    int error = 0;
    error = mPtrVoEBase->StopPlayout(mChannel);
    ASSERT_TRUE(error != -1);
    error = mPtrVoEBase->StopReceive(mChannel);
    ASSERT_TRUE(error != -1);
    error = mPtrVoEBase->StopSend(mChannel);
    ASSERT_TRUE(error != -1);
    audioSource.stop = true;
  } 

private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mAudioSession;
  mozilla::RefPtr<mozilla::AudioRenderer> mAudioRenderer;
  mozilla::RefPtr<mozilla::TransportInterface> mAudioTransport;
  AudioSamplesSource audioSource;

  webrtc::VoiceEngine* mVoiceEngine;
  webrtc::VoEBase*    mPtrVoEBase;
  webrtc::VoEBase*    mPtrVoEBase2;
  webrtc::VoEFile*   mPtrVoEFile;
  webrtc::VoEExternalMedia* mPtrVoEXmedia;
  webrtc::VoECodec * mPtrVoECodec;

  int mChannel;
  std::string fileToPlay;
  std::string filename;
  bool playToSpeaker; // flag to indicate if
  bool initDone;
};


// Test 1: Play a file to the speaker
//TEST_F(WebrtcTest, TestSpeakerPlayout) {
 // TestAudioFileLocalPlayout();
//}

// Test 2: Test Dummy External Xport
TEST_F(WebrtcTest, TestDummyMediaWithTransport) {
  TestDummyMediaAndTransport();
}

//TEST_F(WebrtcTest, TestDummyExternalXportPlayback) {
// TestAudioFilePlayoutExternalTransport();
//}


}  // end namespace

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


