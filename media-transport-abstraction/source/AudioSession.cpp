/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AudioSession.h"

//TODO:crypt: deal with printfs
//TODO:crypt: Make WebRTC pointers scoped
//TODO:crypt: Implement simple state-machinery for RAII
//TODO:crypt: check return types of Codec Config functions
//TODO:crypt: Implement Sycnhronization after gathering feedback

namespace mozilla {

//Factory Implementation
mozilla::TemporaryRef<AudioSessionConduit> AudioSessionConduit::Create()
{
  printf("WebrtcAudioConduitImpl:: Create ");  	
  return  new WebrtcAudioConduit();
}


void WebrtcAudioConduit::Construct()
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

  printf("\nWebrtcAudioConduitImpl:: Engine Created: Init'ng the interfaces "); 
  mPtrVoEBase = webrtc::VoEBase::GetInterface(mVoiceEngine);
  if(!mPtrVoEBase)
  	return;
 
  // init the engine with our audio device layer
  res = mPtrVoEBase->Init();
  if(-1 == res)
  {
  	printf("\nWebrtcAudioConduitImpl:: VoiceEngine Init Failed %d ", mPtrVoEBase->LastError());
  	return;  	
  }

  //temp logging only
  mVoiceEngine->SetTraceFilter(webrtc::kTraceAll);
  mVoiceEngine->SetTraceFile( "Voevideotrace.out" );

  mPtrVoENetwork = webrtc::VoENetwork::GetInterface(mVoiceEngine);
  mPtrVoECodec = webrtc::VoECodec::GetInterface(mVoiceEngine);
  mPtrVoEXmedia = webrtc::VoEExternalMedia::GetInterface(mVoiceEngine);

#if 0
//only for testing
  mPtrVoEFile  = webrtc::VoEFile::GetInterface(mVoiceEngine);
#endif

  if(!mPtrVoENetwork || !mPtrVoECodec || !mPtrVoEXmedia)
  {
  	printf("\nWebrtcAudioConduitImpl:: Creating Voice-Engine Interfaces Failed %d ", 
														mPtrVoEBase->LastError());
  	return;
  }

  if(-1 == (mChannel = mPtrVoEBase->CreateChannel()))
  	return;
 
  printf("\nWebrtcAudioConduitImpl::Created Channel %d", mChannel);

  //Register ourselves as external transport
  if(-1 == mPtrVoENetwork->RegisterExternalTransport(mChannel, *this))
	return;

  // Set external recording status as true
  if(-1 == mPtrVoEXmedia->SetExternalRecordingStatus(true)) 
  {
  	printf("\nWebrtcAudioConduitImpl::SetExternalRecordingStatus Failed %d ", 
													mPtrVoEBase->LastError());		
	return;
  }

  if(-1 == mPtrVoEXmedia->SetExternalPlayoutStatus(true)) 
  {
  	printf("\nWebrtcAudioConduitImpl::SetExternalPlayoutStatus Failed %d ", 
													mPtrVoEBase->LastError());		
	return;
  }

  printf("\n AudioSessionConduit Initialization Done");  

  // we should be good here
  initDone = true;
}

//TODO:Crypt: Discuss the engine ownership
void WebrtcAudioConduit::Destruct()
{  
  printf("\nWebrtcAudioConduitImpl::AudioSessionConduit Destruct");
  int error = 0;   


  //Deal with External Media
  error = mPtrVoEBase->StopPlayout(mChannel); 
  error = mPtrVoEXmedia->SetExternalRecordingStatus(false);
  error = mPtrVoEXmedia->SetExternalPlayoutStatus(false);
  mEnginePlaying = false;

  //Deal with the transport  
  error = mPtrVoEBase->StopSend(mChannel);
  error = mPtrVoEBase->StopReceive(mChannel);
  error = mPtrVoENetwork->DeRegisterExternalTransport(mChannel);
  error = mPtrVoEBase->DeleteChannel(mChannel);
  mChannel = -1;    

  //clean up webrtc interfaces  
  error = mPtrVoEXmedia->Release();
  error = mPtrVoECodec->Release();
  error = mPtrVoENetwork->Release();

  //finally the mother of all interfaces
   error = mPtrVoEBase->Terminate();
   error = mPtrVoEBase->Release();

   bool weOwnTheEngine = true;  
   if(weOwnTheEngine)  
   {
      webrtc::VoiceEngine::Delete(mVoiceEngine);
      mVoiceEngine = NULL;  
   }

  initDone = false;
}


// AudioSessionConduit Impelmentation

int WebrtcAudioConduit::AttachRenderer(mozilla::RefPtr<AudioRenderer> aAudioRenderer)
{
  printf("WebrtcAudioConduitImpl:: AttachRenderer");
  mRenderer = aAudioRenderer;
  return 0; //re-think the return
}

void WebrtcAudioConduit::AttachTransport(mozilla::RefPtr<TransportInterface> aTransport)
{
  printf("WebrtcAudioConduitImpl:: AttachTransport ");
  mTransport = aTransport;
}

//TODO:crypt: Remove the harcode of channels 
int WebrtcAudioConduit::ConfigureSendMediaCodec(CodecConfig* codecInfo)
{

  AudioCodecConfig* codecConfig = (AudioCodecConfig*) codecInfo;
  
  if(!codecConfig)
  {
    return -1;
  }

  printf("WebrtcAudioConduitImpl:: ConfigureSendMediaCode : %s ",
									 codecConfig->mName.c_str());


  int error = 0;
  webrtc::CodecInst cinst;
  strcpy(cinst.plname, codecConfig->mName.c_str());
  cinst.pltype = codecConfig->mType;
  cinst.rate = codecConfig->mRate; // default rate
  cinst.pacsize = codecConfig->mPacSize; // 30ms
  cinst.plfreq =  codecConfig->mFreq;
  cinst.channels = 1;

 
  error = mPtrVoECodec->SetSendCodec(mChannel, cinst);
  if(-1 == error)
  { 
  	printf("\nWebrtcAudioConduitImpl:Setting Send Codec Failed %d ", 
                                           mPtrVoEBase->LastError());
    return mPtrVoEBase->LastError();
  }

  //Let's Send Transport State-machine on the Engine
  error = mPtrVoEBase->StartSend(mChannel);
  if(-1 == error) 
  {
  	printf("\n WebrtcAudioConduitImpl: StartSend() Failed %d ", 
									mPtrVoEBase->LastError());
    return mPtrVoEBase->LastError();
  }
  
  return error;
}

//TODO:crypt: Remove the harcode here
int WebrtcAudioConduit::ConfigureRecvMediaCodec(CodecConfig* codecInfo)
{


  AudioCodecConfig* codecConfig = (AudioCodecConfig*) codecInfo;
  
  if(!codecConfig)
  {
    return -1;
  }

  printf("WebrtcAudioConduitImpl:: ConfigureSendMediaCode : %s ",
									 codecConfig->mName.c_str());

  int error = 0;
  webrtc::CodecInst cinst;
  strcpy(cinst.plname, codecConfig->mName.c_str());
  cinst.pltype = codecConfig->mType;
  cinst.rate = codecConfig->mRate; // default rate
  cinst.pacsize = codecConfig->mPacSize; // 30ms
  cinst.plfreq =  codecConfig->mFreq;
  cinst.channels = 1;

  error = mPtrVoECodec->SetRecPayloadType(mChannel,cinst);
  if(-1 == error) 
  {
  	printf("\nWebrtcAudioConduitImpl:Setting Receive Codec Failed %d ", 
   											mPtrVoEBase->LastError());
	return mPtrVoEBase->LastError();
  }

  error = mPtrVoEBase->StartReceive(mChannel);
  if(-1 == error)
  {
  	printf("\nWebrtcAudioConduitImpl:: StartReceive Failed %d ", 
										mPtrVoEBase->LastError());
	return mPtrVoEBase->LastError();
  }

  return error;
}

int WebrtcAudioConduit::SendAudioFrame(const int16_t audio_data[],
                           				unsigned int numberOfSamples,
                           				uint32_t samplingFreqHz,
                           				uint64_t capture_time) 
{
  printf("\n WebrtcAudioConduitImpl:: SendAudioFrame numberOfSamples %d, samplingFreq %d ", 
													numberOfSamples, samplingFreqHz);

  if(!initDone) 
  {
  	printf("\n SendAudioFrame : Init Not Done ");
  	return -1;
  }

  if(!mEnginePlaying)
  {
  	if(-1 == mPtrVoEBase->StartPlayout(mChannel))
    	return mPtrVoEBase->LastError();
    mEnginePlaying = true;

#if 0
    //only for testing
	if(-1 == mPtrVoEFile->StartRecordingPlayout(mChannel, "/tmp/suhas.pcm"))
	{
		printf("\n Start Recording Faile %d",mPtrVoEBase->LastError());
	}
#endif

  }

  if(-1 == mPtrVoEXmedia->ExternalRecordingInsertData(audio_data,
													  numberOfSamples,
													  samplingFreqHz,
													  capture_time) )
  {
	printf("\n WebrtcAudioConduitImpl::SendFailed : %d",
									mPtrVoEBase->LastError());
	return  mPtrVoEBase->LastError();
  }
  // we should be good here
  return 0;
}

void WebrtcAudioConduit::GetAudioFrame(int16_t speechData[],
                           				uint32_t samplingFreqHz,
                           				uint64_t capture_delay,
									    unsigned int& lengthSamples)
{
  printf("\nWebrtcAudioConduitImpl:GetAudioFrame ");
  lengthSamples = 0;

  if(mEnginePlaying)
   	mPtrVoEXmedia->ExternalPlayoutGetData((WebRtc_Word16*) speechData, (int)samplingFreqHz,
								 							capture_delay, (int&)lengthSamples);

  printf("\nWebrtcAudioConduitImpl:GetAudioFrame:Got samples: length:%d",lengthSamples);
}

// Transport Layer Callbacks 
void WebrtcAudioConduit::ReceivedRTPPacket(const void *data, int len)
{
  printf("WebrtcAudioConduiWtImpl:: ReceivedRTPPacket: Len %d ", len);
  if(mEnginePlaying)
    mPtrVoENetwork->ReceivedRTPPacket(mChannel,data,len);
}

void WebrtcAudioConduit::ReceivedRTCPPacket(const void *data, int len)
{
  printf("WebrtcAudioConduitImpl:: ReceivedRTCPPacket");
  if(mEnginePlaying)
    mPtrVoENetwork->ReceivedRTCPPacket(mChannel, data, len);
}

//WebRTC::RTP Callback Implementation

int WebrtcAudioConduit::SendPacket(int channel, const void* data, int len)
{
  printf("\nWebrtcAudioConduitImpl:: SendRtpPacket len %d ",len);

   if(mTransport)
     mTransport->SendRtpPacket(data, len); 


  return 0;
}

int WebrtcAudioConduit::SendRTCPPacket(int channel, const void* data, int len)
{
  printf("WebrtcAudioConduitImpl:: SendRtcpPacket : channel %d ", channel);
  if(mTransport)
  	mTransport->SendRtcpPacket(data, len);

  return 0;
}

}// end namespace

