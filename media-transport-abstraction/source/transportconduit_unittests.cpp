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


//Foreward Declaration
class AudioStartCaptureEvent;
class AudioStartRenderEvent;


/**
 * A Dummy Video Conduit Tester. 
 *
**/
class VideoSendAndReceive
{
public:
  VideoSendAndReceive():width(640), height(480)
  {
  }

  ~VideoSendAndReceive()
  {
  }

  void Init(mozilla::RefPtr<mozilla::VideoSessionConduit> aSession)
  {
  	mSession = aSession;
  }
  void GenerateAndReadSamples()
  {
    // let's send 33 frames - no big logic here 
    	webrtc::ViEVideoFrameI420 *aFrame = new webrtc::ViEVideoFrameI420();
    	int yPitch = width;
    	int uPitch = (width + 1)/2;
    	int vPitch = (width + 1)/2;
    	int uv_size = (width/2) * (height/2);

    	aFrame->width = width;
    	aFrame->height = height;
    	aFrame->y_pitch = width;
    	aFrame->u_pitch = uPitch;
    	aFrame->v_pitch = vPitch;

    	aFrame->y_plane = new unsigned char[yPitch * width];
    	aFrame->u_plane = new unsigned char[uPitch * (height/2)];
    	aFrame->v_plane = new unsigned char[vPitch * (height/2)];

    	memset(aFrame->y_plane, 16, (width * height));
    	memset(aFrame->u_plane, 128, uv_size);
    	memset(aFrame->v_plane, 128, uv_size);

     	printf("\n Video Frame is 0x%p, ", aFrame);
        int count = 33;
        do
        {
	 	mSession->SendI420VideoFrame(*aFrame, 0);
		usleep(500 * 1000);
		--count;
       }while(count > 0);
    delete aFrame;
  }
private:
mozilla::RefPtr<mozilla::VideoSessionConduit> mSession;
int width, height;
};



/**
 * A Dummy AudioConduit Tester
 *
**/
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
  //PR_DestroyLock(lck);
  }

 void Init(mozilla::RefPtr<mozilla::AudioSessionConduit> aSession, 
      std::string fileIn, std::string fileOut)
  {
  mSession = aSession;  
  iFile = fileIn;
  oFile = fileOut;
#if 0
// UNCOMMENT THIS AFTER WE FIX THE THREAD LINKER ISSUS
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

  //Kick start the test
  void GenerateAndReadSamples();  

  //void Start();
private:
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

#if 0
//UNCOMMENT THIS AFTER WE FIX THE THREAD LINKER ISSUES
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

void AudioSendAndReceive::Start()
 {
    
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
#endif

//Hardcoded for 16 bit samples for now
//TODO:crypt: Improve fiele handling - this is too dumb a impln
void AudioSendAndReceive::GenerateAndReadSamples()
{
  int16_t audioIn10ms[160]; // samples sent to the transport
  int16_t audioOut10ms[160]; // samples  to write out
  ifstream iStream;
  const int sample_length  = 160;

  memset(audioIn10ms, 0, 160 * sizeof(short));
  memset(audioOut10ms, 0, 160 * sizeof(short));

  iStream.open(iFile.c_str(), ios::binary);

  FILE* pFile;
  pFile = fopen ( oFile.c_str() , "wb" );
  streamsize curPos = 0;

  if (iStream.is_open())
  {
    while ( iStream.good() )
    {
        streamsize sz = 320; 
	    iStream.get((char*)audioIn10ms, sz);	
    	mSession->SendAudioFrame(audioIn10ms,sample_length, 16000, 10);
        usleep(10*1000);
		curPos += (160*2);
		iStream.seekg(curPos,ios::beg);
		//read a sample .. not a perfect way to step
		int sampleLength = 0;
    	mSession->GetAudioFrame(audioOut10ms, (uint32_t) 16000,(uint64_t) 10, (unsigned int&)sampleLength);
  		fwrite (audioOut10ms, 1 , sizeof(audioOut10ms) , pFile );
    }
    std::cout << " Done Reading the file " << std::endl;
    iStream.close();
  	fclose (pFile);
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

// Dummy Video Target for the conduit
//TODO:crypt: What do we do with the delivered frames ??
class DummyVideoTarget: public mozilla::VideoRenderer
{
public:
  DummyVideoTarget()
  {
    std::cerr << " DummyVideoTarget created " << std::endl;
  }

  virtual ~DummyVideoTarget()
  {
    std::cerr << " Deleting Dummy Video Renderer " << std::endl;
  }


  int RenderVideoFrame(unsigned char* buffer,
                                unsigned int buffer_size,
                                // RTP timestamp
                                uint32_t time_stamp,
                                // Wallclock render time in miliseconds
                                int64_t render_time)
 {
  //write the frame to the file
  printf("\n Deliver  Frame called %d ", buffer_size);
  return 0;
 }

 int FrameSizeChange(unsigned int, unsigned int, unsigned int)
 {
    //do nothing
    return 0;
 }


};


//Fake Transport Audio/Video
class FakeMediaTransport : public mozilla::TransportInterface 
{
public:
  FakeMediaTransport():mAudio(false),mVideo(false)
  {
  }

  ~FakeMediaTransport()
  {
  	std::cout << " Deleting Fake Media Transport " << std::endl;
  }

  virtual int SendRtpPacket(const void* data, int len)
  {
    ++numPkts;
  	printf("\nFAT: SendRtpPacket(%d): Len:%d ",numPkts, len );
    // we just return thus obtained packet back to the engine 
    // for decoding and eventual playing
    if(mAudio)
      mAudioSession->ReceivedRTPPacket(data,len);
    else
      mVideoSession->ReceivedRTPPacket(data,len);

    return 0;
  }

  virtual int SendRtcpPacket(const void* data, int len)
  {
  	std::cout << " FAT: SendRtcpPacket: " << std::endl;
    if(mAudio)
      mAudioSession->ReceivedRTCPPacket(data,len);
    else
      mVideoSession->ReceivedRTCPPacket(data,len);

    return 0;
  }

  void SetAudioSession(mozilla::RefPtr<mozilla::AudioSessionConduit> aSession)
  {
  	mAudioSession = aSession;
    mAudio = true;
  }

  void SetVideoSession(mozilla::RefPtr<mozilla::VideoSessionConduit> aSession)
  {
  	mVideoSession = aSession;
    mVideo = true;
  }
private:
  mozilla::RefPtr<mozilla::AudioSessionConduit> mAudioSession;
  mozilla::RefPtr<mozilla::VideoSessionConduit> mVideoSession;
  int numPkts;
  bool mAudio, mVideo;
};


namespace {

class TransportConduitTest : public ::testing::Test {
 public:
  TransportConduitTest() 
  {
    iAudiofilename = "audio_short16.pcm";
    oAudiofilename = "recorded.pcm";
    std::string rootpath = ProjectRootPath();

   fileToPlay = rootpath+"media"+kPathDelimiter+"webrtc"+kPathDelimiter+"trunk"+kPathDelimiter+"test"+kPathDelimiter+"data"+kPathDelimiter+"voice_engine"+kPathDelimiter+iAudiofilename;

   std::cout << " Filename to play is " << fileToPlay << std::endl;
   fileToRecord = rootpath+kPathDelimiter+"media"+kPathDelimiter+"mtransport"+kPathDelimiter+"test"+kPathDelimiter+oAudiofilename;
   std::cout << " Filename to record is " << fileToRecord<< std::endl;
  }

  ~TransportConduitTest() 
  {
  }

  //1. Dump audio samples to dummy external transport
  void TestDummyAudioAndTransport() 
  {
    //get pointer to AudioSessionConduit
    mAudioSession = mozilla::AudioSessionConduit::Create();
    if( !mAudioSession )
      ASSERT_TRUE(mAudioSession != NULL);

    mAudioRenderer = new DummyAudioTarget();
	ASSERT_TRUE(mAudioRenderer != NULL);

    FakeMediaTransport* xport = new FakeMediaTransport();
	ASSERT_TRUE(xport != NULL);
    xport->SetAudioSession(mAudioSession);
    mAudioTransport = xport;

    // attach the transport and renderer to audio-conduit
    mAudioSession->AttachRenderer(mAudioRenderer);
    mAudioSession->AttachTransport(mAudioTransport);

    //configure send and recv codecs on the audio-conduit
    mozilla::AudioCodecConfig cinst;
    cinst.mChannels = 1;
    cinst.mName = "ISAC";
    cinst.mType = 103;
    cinst.mRate = -1; // default rate
    cinst.mPacSize = 480; // 30ms
    cinst.mFreq = 16000;
    mAudioSession->ConfigureSendMediaCodec(&cinst);

    cinst.mRate = 32000;
    mAudioSession->ConfigureRecvMediaCodec(&cinst);

    //start generating samples
    audioTester.Init(mAudioSession, fileToPlay,fileToRecord);
  	audioTester.GenerateAndReadSamples();
  }

  //2. Dump audio samples to dummy external transport
  void TestDummyVideoAndTransport()
  {
    //get pointer to VideoSessionConduit
    mVideoSession = mozilla::VideoSessionConduit::Create();
    if( !mVideoSession )
      ASSERT_TRUE(mVideoSession != NULL);


    // create audio-renderer, audio-transport
    mVideoRenderer = new DummyVideoTarget();
    ASSERT_TRUE(mVideoRenderer != NULL);

    FakeMediaTransport* xport = new FakeMediaTransport();
    ASSERT_TRUE(xport != NULL);

    xport->SetVideoSession(mVideoSession);
    mVideoTransport = xport;

    // attach the transport and renderer to video-conduit
    mVideoSession->AttachRenderer(mVideoRenderer);
    mVideoSession->AttachTransport(mVideoTransport);

    //configure send and recv codecs on theconduit
    mozilla::VideoCodecConfig cinst;
    cinst.mName = "VP8";
    cinst.mHeight = 480;
    cinst.mWidth  = 640; 

    mVideoSession->ConfigureSendMediaCodec(&cinst);
    mVideoSession->ConfigureRecvMediaCodec(&cinst);

    //start generating samples
    videoTester.Init(mVideoSession );
    videoTester.GenerateAndReadSamples();
  }

private:
  //Audio Conduit Test Objects
  mozilla::RefPtr<mozilla::AudioSessionConduit> mAudioSession;
  mozilla::RefPtr<mozilla::AudioRenderer> mAudioRenderer;
  mozilla::RefPtr<mozilla::TransportInterface> mAudioTransport;
  AudioSendAndReceive audioTester;

  //Video Conduit Test Objects
  mozilla::RefPtr<mozilla::VideoSessionConduit> mVideoSession;
  mozilla::RefPtr<mozilla::VideoRenderer> mVideoRenderer;
  mozilla::RefPtr<mozilla::TransportInterface> mVideoTransport;
  VideoSendAndReceive videoTester;

  std::string fileToPlay;
  std::string fileToRecord;
  std::string iAudiofilename;
  std::string oAudiofilename;
};


// Test 1: Test Dummy External Xport
TEST_F(TransportConduitTest, TestDummyAudioWithTransport) {
  TestDummyAudioAndTransport();
}

// Test 2: Test Dummy External Xport
TEST_F(TransportConduitTest, TestDummyVideoWithTransport) {
  TestDummyVideoAndTransport();
}

}  // end namespace

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  //base::AtExitManager exit_manager;
  return RUN_ALL_TESTS();
}


