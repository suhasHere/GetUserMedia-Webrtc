/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: snandaku@cisco.com

#include <iostream>
#include <string>
#include <unistd.h>



#include "mozilla/Scoped.h"

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


#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"



// Defining this here, usually generated for libxul
// should not be needed by these tests
const mozilla::Module *const *const kPStaticModules[] = {
  NULL
};

//Dummy External VOE Media Processs
class DummyVoEMediaProcess : public webrtc::VoEMediaProcess
{
public:
    virtual void Process(const int channel, const webrtc::ProcessingTypes type,
                         WebRtc_Word16 audio10ms[], const int length,
                         const int samplingFreq, const bool isStereo)
{
   std::cout <<"In Process type is " << type << std::endl;
}

};

namespace {

class WebrtcTest : public ::testing::Test {
 public:
  WebrtcTest():mVoiceEngine(NULL),
   		mPtrVoEBase(NULL),
   		mPtrVoEFile(NULL),
   		mChannel(-1),
   		initDone(false) {
  filename = "recorded.pcm";
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
   fileToPlay = rootpath+"media"+kPathDelimiter+"mtransport"+kPathDelimiter+"test"+kPathDelimiter+filename;
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
    int error = 0;
    Init();
    mChannel = mPtrVoEBase->CreateChannel();
    ASSERT_TRUE(mChannel != -1);
    std::cerr << " Playing audio for 5 Seconds " << std::endl;
    StartMedia(0); 
    sleep(5);
    StopMedia();
    error = mPtrVoEBase->DeleteChannel(mChannel);
    ASSERT_TRUE(error != -1);
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
	     break;	
    }

    error = mPtrVoEBase->StartPlayout(mChannel);
    ASSERT_TRUE(error != -1);
    
    error = mPtrVoEFile->StartPlayingFileAsMicrophone(mChannel,
							fileToPlay.c_str(),
							false,
							false);
    ASSERT_TRUE(error != -1);


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
  } 

private:

  webrtc::VoiceEngine* mVoiceEngine;
  webrtc::VoEBase*    mPtrVoEBase;
  webrtc::VoEFile*   mPtrVoEFile;
  webrtc::VoEExternalMedia* mPtrVoEXmedia;
  webrtc::VoECodec * mPtrVoECodec;

  int mChannel;
  std::string fileToPlay;
  std::string filename;
  bool playToSpeaker; // flag to indicate if
  bool initDone;
  DummyVoEMediaProcess dummyVoEMedia; 
};


// Test 1: Play a file to the speaker
TEST_F(WebrtcTest, TestSpeakerPlayout) {
 TestAudioFileLocalPlayout();
}

// Test 2: Test Dummy External Xport
//TEST_F(WebrtcTest, TestDummyMediaWithTransport) {
 // TestDummyMediaAndTransport();
//}

//TEST_F(WebrtcTest, TestDummyExternalXportPlayback) {
// TestAudioFilePlayoutExternalTransport();
//}


}  // end namespace

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


