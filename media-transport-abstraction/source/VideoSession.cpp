/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VideoSession.h"


namespace mozilla {

//Factory Implementation
mozilla::TemporaryRef<VideoSessionConduit> VideoSessionConduit::Create()
{
  printf("WebrtcVideoConduitImpl:: Create ");  	
  return  new WebrtcVideoConduit();
}


void WebrtcVideoConduit::Init()
{
  int res = 0; 

  if(!mVideoEngine) 
  {
   mVideoEngine = webrtc::VideoEngine::Create();
   if(NULL == mVideoEngine)
    {
        printf("WebrtcVideoConduitImpl:: Unable to create video engine ");
        return;
    }
  }

  printf("\nWebrtcVideoConduitImpl:: Engine Created: Init'ng the interfaces "); 
  mPtrViEBase = webrtc::ViEBase::GetInterface(mVideoEngine);
  if(!mPtrViEBase)
  	return;
 
  // init the engine with our audio device layer
  res = mPtrViEBase->Init();
  if(-1 == res)
  {
  	printf("\nWebrtcVideoConduitImpl::VoiceEngine Init Failed %d ", 
												mPtrViEBase->LastError());
  	return;  	
  }

  //temp logging only
  mVideoEngine->SetTraceFilter(webrtc::kTraceAll);
  mVideoEngine->SetTraceFile( "ViEvideotrace.out" );

  mPtrViENetwork = webrtc::ViENetwork::GetInterface(mVideoEngine);
  mPtrViECodec   = webrtc::ViECodec::GetInterface(mVideoEngine);
  mPtrViERender  = webrtc::ViERender::GetInterface(mVideoEngine);
  mPtrViECapture = webrtc::ViECapture::GetInterface(mVideoEngine);

  if(!mPtrViENetwork || !mPtrViECodec || !mPtrViECodec || !mPtrViECapture)
  {
  	printf("\nWebrtcVideoConduitImpl:: Creating Voice-Engine Interfaces Failed %d ", 
														mPtrViEBase->LastError());
  	return;
  }

  if(-1 == mPtrViEBase->CreateChannel(mChannel))
  	return;
 
  printf("\nWebrtcVideoConduitImpl::Created Channel %d", mChannel);

  //Register ourselves as external transport
  if(-1 == mPtrViENetwork->RegisterSendTransport(mChannel, *this))
	return;

  mPtrExtCapture = 0;
  if(-1 == mPtrViECapture->AllocateExternalCaptureDevice(mCapId, 
													mPtrExtCapture))	
  {
  	printf("\nWebrtcVideoConduitImpl::Unable to Allocate capture module: %d",
															mPtrViEBase->LastError()); 

  }
  printf("\nWebrtcVideoConduitImpl:: Capture Id allocated:%d", mCapId);											
  //connect the capture device
  if(-1 == mPtrViECapture->ConnectCaptureDevice(mCapId,
												mChannel))
  {
  	printf("\nWebrtcVideoConduitImpl::Unable to Connect capture module: %d",
															mPtrViEBase->LastError()); 
  }

   if(-1 == mPtrViERender->AddRenderer(mChannel, 	
									webrtc::kVideoI420, 
									(webrtc::ExternalRenderer*) this))
  {
  	printf("\nWebrtcVideoConduitImpl:: Failed to added external renderer ");
	return;
  }


  printf("\n VideoSessionConduit Initialization Done");  
  
  // we should be good here
  initDone = true;
}

// VideoSessionConduit Impelmentation

int WebrtcVideoConduit::AttachRenderer(mozilla::RefPtr<VideoRenderer> aVideoRenderer)
{
  printf("WebrtcVideoConduitImpl:: AttachRenderer");
  mRenderer = aVideoRenderer;
  printf("\nWebrtcVideoConduit::Starting the Renderer");
  if(!mEnginePlaying)
  {
    if(-1 == mPtrViERender->StartRender(mChannel))
        	return mPtrViEBase->LastError();
    mEnginePlaying = true;
  }
  return 0;
}

void WebrtcVideoConduit::AttachTransport(mozilla::RefPtr<TransportInterface> aTransport)
{
  printf("WebrtcVideoConduitImpl:: AttachTransport ");
  mTransport = aTransport;
}

//TODO:crypt: Remove the harcode here
int WebrtcVideoConduit::ConfigureSendMediaCodec(CodecConfig* codecInfo)
{
  printf("WebrtcVideoConduitImpl:: ConfigureSendMediaCode : %s ",
											 codecInfo->mName.c_str());
  int error = 0;
  //hardcoded for now
  webrtc::VideoCodec  video_codec;
  memset(&video_codec, 0, sizeof(webrtc::VideoCodec));
  //VP8 only for now
  for(int idx=0; idx < mPtrViECodec->NumberOfCodecs(); idx++)
  {
  	if(0 == mPtrViECodec->GetCodec(idx, video_codec))
    {
		if(video_codec.codecType == webrtc::kVideoCodecVP8)
        {
			video_codec.width = 640;
			video_codec.height = 480;
			break;
		}
    }
  }

  error = mPtrViECodec->SetSendCodec(mChannel, video_codec);
  if(-1 == error)
  { 
  	printf("\n Setting Send Codec Failed %d ", mPtrViEBase->LastError());
    return mPtrViEBase->LastError();
  }

  //Let's Send Transport State-machine on the Engine
  error = mPtrViEBase->StartSend(mChannel);
  if(-1 == error) 
  {
  	printf("\n WebrtcVideoConduitImpl: StartSend() Failed %d ", 
												mPtrViEBase->LastError());
    return mPtrViEBase->LastError();
  }
  
  return error;
}

//TODO:crypt: Remove the harcode here
int WebrtcVideoConduit::ConfigureRecvMediaCodec(CodecConfig* codecInfo)
{
  printf("WebrtcVideoConduitImpl:: ConfigureRecvMediaCodec: %s",
											 codecInfo->mName.c_str());
  int error = 0;
  webrtc::VideoCodec video_codec;
  memset(&video_codec, 0, sizeof(webrtc::VideoCodec));

  for (int idx = 0; idx < mPtrViECodec->NumberOfCodecs(); idx++) 
  {
    if(0 == mPtrViECodec->GetCodec(idx, video_codec))
    {
    	if (video_codec.codecType != webrtc::kVideoCodecI420) 
		{
      		video_codec.width = 640;
      		video_codec.height = 480;
    	}

    	if (video_codec.codecType == webrtc::kVideoCodecI420) 
		{
			video_codec.width = 176;
      		video_codec.height = 144;
    	}
  		error = mPtrViECodec->SetReceiveCodec(mChannel,video_codec);
  		if(-1 == error) 
  		{
  			printf("\n Setting Receive Codec Failed %d ", mPtrViEBase->LastError());
			return mPtrViEBase->LastError();
  		}
  	}
 }//end for


  error = mPtrViEBase->StartReceive(mChannel);
  if(-1 == error)
  {
  	printf("\nWebrtcVideoConduitImpl:: StartReceive Failed %d ", 
												mPtrViEBase->LastError());
	return mPtrViEBase->LastError();
  }

  return error;
}

//TODO: Make this function's Interface and Implementation flexible
// of samples lenght and frequency.
// Currently it works only for 16-bit linear PCM audio
int WebrtcVideoConduit::SendVideoFrame(unsigned char* video_frame,
                           				unsigned int video_frame_length,
                           				unsigned short width,
                           				unsigned short height,
                           				VideoType video_type,
                           				uint64_t capture_time)
{
  printf("\n WebrtcVideoConduitImpl::SendVideoFrame width:%d, height:%d",
													width,height);

  if(!initDone) 
  {
  	printf("\n SendAudioFrame : Init Not Done ");
  	return -1;
  }

  if(!mEnginePlaying)
  {
  	if(-1 == mPtrViERender->StartRender(mChannel))
    	return mPtrViEBase->LastError();

    mEnginePlaying = true;
  }

  //insert the frame to video engine
  mPtrExtCapture->IncomingFrame(video_frame, 
								 video_frame_length,
								 width, height,
								 webrtc::kVideoI420,
								(unsigned long long)capture_time);
  printf("\nWebrtcVideoConduitImpl:: Inserted A Frame ");
  return 0;
}

// Transport Layer Callbacks 
void WebrtcVideoConduit::ReceivedRTPPacket(const void *data, int len)
{
  printf("WebrtcVideoConduitImpl:: ReceivedRTPPacket: Len %d ", len);
  mPtrViENetwork->ReceivedRTPPacket(mChannel,(unsigned char*)data,len);
}

void WebrtcVideoConduit::ReceivedRTCPPacket(const void *data, int len)
{
  printf("WebrtcVideoConduitImpl:: ReceivedRTCPPacket");
  mPtrViENetwork->ReceivedRTCPPacket(mChannel,(unsigned char*)data,len);
}

//WebRTC::RTP Callback Implementation
int WebrtcVideoConduit::SendPacket(int channel, const void* data, int len)
{
  printf("\nWebrtcVideoConduitImpl:: SendRtpPacket len %d ",len);
  //TODO:crypt:Add lock here ???
   if(mTransport)
     mTransport->SendRtpPacket(data, len); 

  return 0;
}

int WebrtcVideoConduit::SendRTCPPacket(int channel, const void* data, int len)
{
  printf("WebrtcVideoConduitImpl:: SendRtcpPacket : channel %d ", channel);
  //TODO: Add lock here ???
  if(mTransport)
  	mTransport->SendRtcpPacket(data, len);

  return 0;
}

// WebRTC::ExternalMedia Implementation
int WebrtcVideoConduit::FrameSizeChange(unsigned int width, unsigned int height,
										 unsigned int numStreams)
{
  mRenderer->FrameSizeChange(width, height, numStreams);
  return 0;
}

int WebrtcVideoConduit::DeliverFrame(unsigned char* buffer, int buffer_size,
                           				uint32_t time_stamp, int64_t render_time)
{
  //add lock here
  mRenderer->RenderVideoFrame(buffer, buffer_size, time_stamp, render_time);
  return 0;
}

}// end namespace

