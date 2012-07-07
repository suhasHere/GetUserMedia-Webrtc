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

//for threading
#include "prmem.h"
#include "nsITimer.h"
#include "nsIThread.h"
#include "nsIRunnable.h"
#include "prthread.h"

#include "video_engine/include/vie_capture.h"

#include "resource_mgr.h"

#define GTEST_HAS_RTTI 0
#include "gtest/gtest.h"
#include "gtest_utils.h"

//something to be happy with
const int THREAD_STACK_SIZE = (128 * 1024);


// Defining this here, usually generated for libxul
// should not be needed by these tests
const mozilla::Module *const *const kPStaticModules[] = {
  NULL
};


void GenerateI420Frame()
{
webrtc::ViEVideoFrameI420 aFrame;
int yPitch = 640;
int uPitch = (640+ 1)/2;
int vPitch = (640 + 1)/2;
int uv_size = (641/2) * (481/2);

aFrame.width = 640;
aFrame.height = 480;
aFrame.y_pitch = 640;
aFrame.u_pitch = uPitch;
aFrame.v_pitch = vPitch;

aFrame.y_plane = new unsigned char[yPitch * 480];
aFrame.u_plane = new unsigned char[uPitch * (480/2)];
aFrame.v_plane = new unsigned char[vPitch * (480/2)];

//memset(aFrame.y_plane,0,sizeof(*aFrame.y_plane)*yplane_bytes); 
//memset(chroma_planes,128,sizeof(*chroma_planes)*chroma_plane_bytes);
};

class VideoSendAndReceive
{


};

//Foreward Declaration
class AudioStartCaptureEvent;
class AudioStartRenderEvent;


class AudioSendAndReceive
{
public:
  friend class AudioStartCaptureEvent;
  friend class AudioStartRenderEvent;

  AudioSendAndReceive():lck(PR_NewLock()),
                        amIDone(false)
  {
    std::cout<<"AudioSendAndReceive()"<<std::endl;   
  }

  ~AudioSendAndReceive()
  {
  std::cout<<"~AudioSendAndReceive()"<<std::endl;  
    PR_DestroyLock(lck);
  }

 void Init(mozilla::RefPtr<mozilla::AudioSessionConduit> aSession, 
      std::string fileIn, std::string fileOut)
  {
  mSession = aSession;  
  iFile = fileIn;
  oFile = fileOut;
#if 0
    //create our threads
    if(!mAudioCaptureThread)
  {
      NS_NewThread(getter_AddRefs(mAudioCaptureThread),
                         nsnull,
                         THREAD_STACK_SIZE);      
  }

  if(!mAudioRenderThread)
  {
      NS_NewThread(getter_AddRefs(mAudioRenderThread),
                         nsnull,
                         THREAD_STACK_SIZE);      

  }
#endif
 }

  void Start();

  void StartTest(); 
  void GenerateAndReadSamples();  
private:
  //void GenerateSamples();
  void ReadSamples();

  mozilla::RefPtr<mozilla::AudioSessionConduit> mSession;
  std::string iFile;
  std::string oFile;
  nsCOMPtr<nsIThread> mAudioCaptureThread; 
  nsCOMPtr<nsIThread> mAudioRenderThread; 
  PRLock* lck;
  PRCondVar* cndVar; 
  bool amIDone;
  nsCOMPtr<nsIRunnable> eventStartCapture;
  nsCOMPtr<nsIRunnable> eventStartRender;

};

// This class acts as source for providing PCM audio samples
// In the actual usage the samples come from MediaStream
#if 0
class AudioStartCaptureEvent : public nsRunnable
{
public:
  AudioStartCaptureEvent(AudioSendAndReceive* aOwner)
  {
  	mOwner = aOwner;
  }

  NS_IMETHOD Run()
  {
    //read 10 ms frame off our file and trasnmit it
  	mOwner->GenerateSamples();
    mOwner->amIDone = true;
  	return NS_OK; 
  }

private:
AudioSendAndReceive* mOwner;  	

};


class AudioStartRenderEvent : public nsRunnable
{
public:
  AudioStartRenderEvent(AudioSendAndReceive* aOwner)
  {
    mOwner = aOwner;
  }

  NS_IMETHOD Run()
  {
    while(!mOwner->amIDone)
    {
    	mOwner->ReadSamples();    
	}
    return NS_OK;
  }
private:
AudioSendAndReceive* mOwner;
};
#endif

void AudioSendAndReceive::Start()
 {
    
#if 0    
    eventStartCapture = new AudioStartCaptureEvent(this);
    mAudioCaptureThread->Dispatch(eventStartCapture,0);

    eventStartRender = new AudioStartRenderEvent(this);
    mAudioRenderThread->Dispatch(eventStartRender,0);

    //let's wait till test ends
    while(!amIDone)
    {
      std::cout << " Waating for the events to end " << std::endl;
      sleep(2);
    }
#endif
 }

void AudioSendAndReceive::ReadSamples()
{
  std::cout <<"AudioSamplesSource::ReadSamples()" << std::endl; 
  int16_t audio10ms[160];
  unsigned int sample_length = 0;
  memset(audio10ms, 0, 160 * sizeof(short));

  mSession->GetAudioFrame(audio10ms,(uint32_t) 16000,(uint64_t) 10,sample_length);

  std::cout << "GetAudioFrame: Sample length "<<sample_length << std::endl;
  //sleep now
  usleep(10 * 1000);
}

void AudioSendAndReceive::GenerateAndReadSamples()
{
  int16_t audioIn10ms[160]; // samples sent to the transport
  int16_t audioOut10ms[160]; // samples sent to the transport
  ifstream iStream;
  ofstream oStream;
  const int sample_length  = 160;

  memset(audioIn10ms, 0, 160 * sizeof(short));
  memset(audioOut10ms, 0, 160 * sizeof(short));

  iStream.open(iFile.c_str(), ios::binary);
  //oStream.open(oFile.c_str(), ios::binary);

//  FILE* pFile;
 // pFile = fopen ( oFile.c_str() , "wb" );
  streamsize curPos = 0;
  if (iStream.is_open())
  {
    while ( iStream.good() )
    {
	    	
		printf("\nDone sleeping .. start writing" );
        streamsize sz = 320; 
	    iStream.get((char*)audioIn10ms, sz);	
    	mSession->SendAudioFrame(audioIn10ms,sample_length, 16000, 10);
        usleep(10*1000);
		curPos += (160*2);
		iStream.seekg(curPos,ios::beg);
		//read a sample .. not a perfect way to step
		int sampleLength = 0;
        usleep(10*1000);
    	mSession->GetAudioFrame(audioOut10ms, (uint32_t) 16000,(uint64_t) 10, (unsigned int&)sampleLength);
//		printf("\nwriting %d bytes to filed", sizeof(audioOut10ms));
  //		fwrite (audioOut10ms, 1 , sizeof(audioOut10ms) , pFile );
//		printf("\nDone writing audio to the file ");
    }
    std::cout << " Done Reading the file " << std::endl;
    iStream.close();
 // 	fclose (pFile);
    oStream.close();
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
    ++numPkts;
  	printf("\nFAT: SendRtpPacket(%d): Len:%d ",numPkts, len );
    // we just return thus obtained packet back to the engine 
    // for decoding and eventual playing
    mSession->ReceivedRTPPacket(data,len);
    //let's read a sample
    printf("\nReadSamples" );
  	int16_t audio10ms[160];
  	unsigned int sample_length = 0;
 	memset(audio10ms, 0, 160 * sizeof(short));

  	//mSession->GetAudioFrame(audio10ms,(uint32_t) 16000,(uint64_t) 10,sample_length);

  	//printf("\nGetAudioFrame: Sample length:%d ",sample_length) ;

 
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
  	numPkts = 0;
  }

private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mSession;
  int numPkts;

};


namespace {

class TransportConduitTest : public ::testing::Test {
 public:
  TransportConduitTest() 
  {
    ifilename = "audio_short16.pcm";
    ofilename = "recorded.pcm";
     std::string rootpath = ProjectRootPath();

   fileToPlay = rootpath+"media"+kPathDelimiter+"webrtc"+kPathDelimiter+"trunk"+kPathDelimiter+"test"+kPathDelimiter+"data"+kPathDelimiter+"voice_engine"+kPathDelimiter+ifilename;


   std::cout << " Filename to play is " << fileToPlay << std::endl;
   fileToRecord = rootpath+kPathDelimiter+ofilename;
   std::cout << " Filename to record is " << fileToRecord<< std::endl;
  }

  ~TransportConduitTest() 
  {
  }

  //2. Dump audio samples to dummy external transport
  void TestDummyMediaAndTransport() 
  {
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
    audioTester.Init(mAudioSession, fileToPlay,fileToRecord);
  	audioTester.GenerateAndReadSamples();
  }


private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mAudioSession;
  mozilla::RefPtr<mozilla::AudioRenderer> mAudioRenderer;
  mozilla::RefPtr<mozilla::TransportInterface> mAudioTransport;
  AudioSendAndReceive audioTester;
  std::string fileToPlay;
  std::string fileToRecord;
  std::string ifilename;
  std::string ofilename;
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


