/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Session.h"

namespace mozilla {


mozilla::TemporaryRef<AudioSessionConduit> AudioSessionConduit::Create()
{
  printf("WebrtcAudioConduitImpl:: Create ");  	
  return  new WebrtcAudioConduit();
}

NS_IMPL_THREADSAFE_ISUPPORTS1(WebrtcAudioConduit, nsITimerCallback)

void WebrtcAudioConduit::Init()
{
  int res = 0; 

  if(!mVoiceEngine) {
   mVoiceEngine = webrtc::VoiceEngine::Create();
   if(NULL == mVoiceEngine)
    {
        printf("WebrtcAudioConduitImpl:: Unable to create voice engine ");
        return;
    }
  }

  printf("\nWebrtcAudioConduitImpl:: Engine Created: Let's init the interfaces "); 
  mPtrVoEBase = webrtc::VoEBase::GetInterface(mVoiceEngine);
  if(!mPtrVoEBase)
  	return;
 
  res = mPtrVoEBase->Init();
  if(-1 == res)
  {
  	printf("\nWebrtcAudioConduitImpl:: VoiceEngine Init Failed %d ", mPtrVoEBase->LastError());
  	return;  	
  }

  //temp only
  mVoiceEngine->SetTraceFilter(webrtc::kTraceAll);
  mVoiceEngine->SetTraceFile( "Voevideotrace.out" );

  mPtrVoENetwork = webrtc::VoENetwork::GetInterface(mVoiceEngine);
  mPtrVoECodec = webrtc::VoECodec::GetInterface(mVoiceEngine);
  mPtrVoEXmedia = webrtc::VoEExternalMedia::GetInterface(mVoiceEngine);
  if(!mPtrVoENetwork || !mPtrVoECodec || !mPtrVoEXmedia)
  {
  	printf("\nWebrtcAudioConduitImpl:: Creating Voice-Engine Interfaces Failed %d ", 
														mPtrVoEBase->LastError());
  	return;
  }

  if(-1 == (mChannel = mPtrVoEBase->CreateChannel()))
  	return;
 
  printf("\nWebrtcAudioConduitImpl::Created Channel %d", mChannel);

  // Set external recording status as true
  if(-1 == mPtrVoEXmedia->SetExternalRecordingStatus(true)) 
  {
  	printf("\nWebrtcAudioConduitImpl::SetExternalRecordingStatus Failed %d ", 
													mPtrVoEBase->LastError());		
	return;
  }

  //Register ourselves as external transport
  if(-1 == mPtrVoENetwork->RegisterExternalTransport(mChannel, *this))
	return;


  // Set external playout as true
  if(-1 == mPtrVoEXmedia->SetExternalPlayoutStatus(true)) 
  {
  	printf("\nWebrtcAudioConduitImpl::SetExternalPlayoutStatus Failed %d ", 
													mPtrVoEBase->LastError());		
	return;
  }

  /*Register External Media Processing
  if(-1 == mPtrVoEXmedia->RegisterExternalMediaProcessing(mChannel,
												  		webrtc::kPlaybackPerChannel,
											      		*this))
  {
	printf("\nWebrtcAudioConduitImpl::RegExtMediaProc Failed %d ",
											mPtrVoEBase->LastError());
	return;
  }
 */
  mTimer = do_CreateInstance(NS_TIMER_CONTRACTID);
  if (!mTimer) {
    printf("\nWebrtcAudioConduitImpl::Timer creation failed ");
    return;
  }

  printf("\n AudioSessionConduit Initialization Done");  
  // we should be good here
  initDone = true;
}

// AudioSessionConduit Impelmentation

void WebrtcAudioConduit::AttachRenderer(mozilla::RefPtr<AudioRenderer> aAudioRenderer)
{
  printf("WebrtcAudioConduitImpl:: AttachRenderer");
  mRenderer = aAudioRenderer;
}

void WebrtcAudioConduit::AttachTransport(mozilla::RefPtr<TransportInterface> aTransport)
{
  printf("WebrtcAudioConduitImpl:: AttachTransport ");
  mTransport = aTransport;
  
}

int WebrtcAudioConduit::ConfigureSendMediaCodec(CodecConfig* codecInfo)
{
  printf("WebrtcAudioConduitImpl:: ConfigureSendMediaCode : %s ", codecInfo->mName.c_str());
  int error = 0;
  webrtc::CodecInst cinst;
#if 0
  strcpy(cinst.plname, codecInfo->mName.c_str());
  cinst.pltype = codecInfo->mType;
  cinst.rate = codecInfo->mRate; // default rate
  cinst.pacsize = codecInfo->mPacSize; // 30ms
  cinst.plfreq =  codecInfo->mFreq;
 #endif
  cinst.channels = 1;
  strcpy(cinst.plname, "ISAC");
  cinst.pltype = 103;
  cinst.rate = -1; // default rate
  cinst.pacsize = 480; // 30ms
  cinst.plfreq = 16000;
 
  error = mPtrVoECodec->SetSendCodec(mChannel, cinst);
  if(-1 == error)
  	printf("\n Setting Send Codec Failed %d ", mPtrVoEBase->LastError());

  //Let's Send Transport State-machine on the Engine
  error = mPtrVoEBase->StartSend(mChannel);
  if(-1 == error)
  	printf("\n WebrtcAudioConduitImpl: StartSend() Failed %d ", mPtrVoEBase->LastError());
  
  return error;
}

int WebrtcAudioConduit::ConfigureRecvMediaCodec(CodecConfig* codecInfo)
{
  printf("WebrtcAudioConduitImpl:: ConfigureRecvMediaCodec: %s", codecInfo->mName.c_str());
  int error = 0;
  webrtc::CodecInst tempCodec;
#if 0
  strcpy(cinst.plname, codecInfo->mName.c_str());
  cinst.pltype = codecInfo->mType;
  cinst.rate = codecInfo->mRate; // default rate
  cinst.pacsize = codecInfo->mPacSize; // 30ms
  cinst.plfreq =  codecInfo->mFreq;
#endif

  tempCodec.channels = 1;
  tempCodec.pacsize = 480;
  tempCodec.plfreq = 16000;
  strcpy(tempCodec.plname, "ISAC");
  tempCodec.pltype = 103;
  tempCodec.rate = 32000;

  error = mPtrVoECodec->SetRecPayloadType(mChannel,tempCodec);
  if(-1 == error)
  	printf("\n Setting Receive Codec Failed %d ", mPtrVoEBase->LastError());

  error = mPtrVoEBase->StartReceive(mChannel);
  if(-1 == error)
  	printf("\nWebrtcAudioConduitImpl:: StartReceive Failed %d ", mPtrVoEBase->LastError());

  return error;
}

int WebrtcAudioConduit::SendAudioFrame(const int16_t speechData[],
                           				unsigned int lengthSamples,
                           				uint32_t samplingFreqHz,
                           				uint64_t capture_time) 
{
  printf("\n WebrtcAudioConduitImpl:: SendAudioFrame lenght %d, samplingFreq %d ", lengthSamples, samplingFreqHz);

  if(!initDone) 
  {
  	printf("\n SendAudioFrame : Init Not Done ");
  	return -1;
  }

  int error = 0;
  if(!mEnginePlaying)
  {
  	if(-1 == (error = mPtrVoEBase->StartPlayout(mChannel)))
    	return -1;

    mEnginePlaying = true;
  }

  //pass in the frames to voice-engine
  error = mPtrVoEXmedia->ExternalRecordingInsertData(speechData, lengthSamples,samplingFreqHz, 10);
  if(-1 == error )
  	printf("\n Passing Frame to voice engine failed %d ", mPtrVoEBase->LastError());

  return error;
}

// Transport calls us on received media data
void WebrtcAudioConduit::ReceivedRTPPacket(const void *data, int len)
{
  printf("WebrtcAudioConduitImpl:: ReceivedRTPPacket: Len %d ", len);
  //play it locally ..
  if(!mPlayoutTimerStarted)
  {
	 printf("\n Sarting Timer to collect the samples for playout");
	 mTimer->InitWithCallback(this, 10, nsITimer::TYPE_REPEATING_SLACK);
     mPlayoutTimerStarted = true;
  }

  mPtrVoENetwork->ReceivedRTPPacket(mChannel,(unsigned char*)data,len);
}

void WebrtcAudioConduit::ReceivedRTCPPacket(const void *data, int len)
{
  printf("WebrtcAudioConduitImpl:: ReceivedRTCPPacket");
}

//WebRTC::External Transport Implementation
int WebrtcAudioConduit::SendPacket(int channel, const void* data, int len)
{
  printf("\nWebrtcAudioConduitImpl:: SendRtpPacket len %d ",len);
  //TODO: Add lock here ???
   if(mTransport)
     mTransport->SendRtpPacket(data, len); 

  return 0;
}

int WebrtcAudioConduit::SendRTCPPacket(int channel, const void* data, int len)
{
  printf("WebrtcAudioConduitImpl:: SendRtcpPacket : channel %d ", channel);
  //TODO: Add lock here ???
  if(mTransport)
  	mTransport->SendRtcpPacket(data, len);

  return 0;
}

// WebRTC::ExternalMedia Implementation
void WebrtcAudioConduit::Process(const int channel,
                        const webrtc::ProcessingTypes type,
                        WebRtc_Word16 audio10ms[],
                        const int length,   
                        const int samplingFreq, 
                        const bool isStereo) 
{
  printf("WebrtcAudioConduitImpl::Process: channel:%d, length:%d",
											channel, length);
}

NS_IMETHODIMP
WebrtcAudioConduit::Notify(nsITimer* aTimer)
{
   // just one audio sample
  static int16_t audio10ms[160];
  int sample_length = 0;
  memset(audio10ms, 0, 160 * sizeof(short));
  mPtrVoEXmedia->ExternalPlayoutGetData(audio10ms, 16000, 10, sample_length);
  if(sample_length == 0)
  {
    printf("\n Sample Length is 0 ");
    return NS_OK;
  }
  printf("\nWebrtcAudioConduitImpl: Notify: Sample Lenght is %d ", sample_length);
  return NS_OK;
}

}// end namespace

