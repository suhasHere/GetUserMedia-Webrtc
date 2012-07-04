/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SESSION_H_
#define SESSION_H_

#include "MediaTransportAbstraction.h"
#include "WebRTCAudioDeviceLayer.h"

// Audio Engine Includes
#include "common_types.h"
#include "voice_engine/main/interface/voe_base.h"
#include "voice_engine/main/interface/voe_volume_control.h"
#include "voice_engine/main/interface/voe_codec.h"
#include "voice_engine/main/interface/voe_network.h"
#include "voice_engine/main/interface/voe_external_media.h"

/** This file hosts several structures identifying different aspects
 * of a RTP Session.
 */

namespace mozilla {

/**
 * Concrete class for Audio session. Hooks up  
 *  - media-source and target to external transport 
 * TODO: crypt
 *    Move WebRTC::Transport , VoEExternalMedia into 
 *    separate classes , if it is worth doing
 */
class WebrtcAudioConduit  : public AudioSessionConduit			
	      		           , public webrtc::Transport
	      		           , public webrtc::VoEMediaProcess
{

public:

  //AudioSessionConduit Implementation
  void AttachRenderer(mozilla::RefPtr<AudioRenderer> aAudioRenderer);
  virtual void ReceivedRTPPacket(const void *data, int len);
  virtual void ReceivedRTCPPacket(const void *data, int len);
  virtual int ConfigureSendMediaCodec(CodecConfig* codecInfo);
  virtual int ConfigureRecvMediaCodec(CodecConfig* codecInfo);
  virtual void AttachTransport(mozilla::RefPtr<TransportInterface> aTransport);
  virtual int SendAudioFrame(const int16_t speechData[],
                           unsigned int lengthSamples,
                           uint32_t samplingFreqHz,
                           uint64_t capture_time);


  
  // Webrtc transport implementation
  virtual int SendPacket(int channel, const void *data, int len) ;
  virtual int SendRTCPPacket(int channel, const void *data, int len) ;

	
  // Webrtc external-media implementation.
  virtual void Process(const int channel,
			const webrtc::ProcessingTypes type,
			WebRtc_Word16 audio10ms[],
			const int length,
			const int samplingFreq, 
			const bool isStereo);

	
  explicit WebrtcAudioConduit(): initDone(false)
								,mChannel(-1)
        						,mTransport(0)
	        		    		,mRenderer(0)
			                    ,sessionId(-1) 
								,mVoiceEngine(0)
								,mEnginePlaying(false)
  {
  	printf("\n WebrtcAudioConduit : Constructor ");
    Init(); 
  }

  virtual ~WebrtcAudioConduit() 
  {
  }


private:
  void Init();

  bool initDone;
  int mChannel;
  mozilla::RefPtr<TransportInterface> mTransport;
  mozilla::RefPtr<AudioRenderer> mRenderer; 
  uint32_t sessionId; // this session - for book-keeping
  webrtc::VoiceEngine* mVoiceEngine;
  webrtc::VoENetwork*  mPtrVoENetwork;
  webrtc::VoEBase*     mPtrVoEBase;
  webrtc::VoECodec*    mPtrVoECodec;
  webrtc::VoEExternalMedia* mPtrVoEXmedia;
  bool mEnginePlaying;
  //local object
  WebRtcAudioDeviceLayer mAudioDevice;
};



} // end namespace
#endif
