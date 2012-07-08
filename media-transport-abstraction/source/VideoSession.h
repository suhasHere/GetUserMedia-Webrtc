/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VIDEO_SESSION_H_
#define VIDEO_SESSION_H_

#include "MediaTransportAbstraction.h"

// Video Engine Includes
#include "common_types.h"
#include "video_engine/include/vie_base.h"
#include "video_engine/include/vie_capture.h"
#include "video_engine/include/vie_codec.h"
#include "video_engine/include/vie_render.h"
#include "video_engine/include/vie_network.h"
#include "video_engine/include/vie_file.h"

/** This file hosts several structures identifying different aspects
 * of a RTP Session.
 */

namespace mozilla {

/**
 * Concrete class for Video session. Hooks up  
 *  - media-source and target to external transport 
 */
class WebrtcVideoConduit  : public VideoSessionConduit			
	      		           , public webrtc::Transport
	      		           , public webrtc::ExternalRenderer
{

public:

  //VideoSessionConduit Implementation
  int AttachRenderer(mozilla::RefPtr<VideoRenderer> aVideoRenderer);

  virtual void ReceivedRTPPacket(const void *data, int len);
  virtual void ReceivedRTCPPacket(const void *data, int len);
  virtual int ConfigureSendMediaCodec(CodecConfig* codecInfo);
  virtual int ConfigureRecvMediaCodec(CodecConfig* codecInfo);
  virtual void AttachTransport(mozilla::RefPtr<TransportInterface> aTransport);
  virtual int SendVideoFrame(unsigned char* video_frame,
							unsigned int video_frame_length,
               				unsigned short width,
               				unsigned short height,
               				VideoType video_type,
               				uint64_t capture_time);
			      
  
  //Test Only
  virtual int SendI420VideoFrame(const webrtc::ViEVideoFrameI420& video_frame,
                                    unsigned long long captureTime = 0) ;



   

  // Webrtc transport implementation
  virtual int SendPacket(int channel, const void *data, int len) ;
  virtual int SendRTCPPacket(int channel, const void *data, int len) ;

	
   // ViEExternalRenderer implementation
  virtual int FrameSizeChange(
        unsigned int, unsigned int, unsigned int
  	);

 virtual int DeliverFrame(
        unsigned char*,int, uint32_t , int64_t
  	);


  explicit WebrtcVideoConduit(): initDone(false)
								,mChannel(-1)
								,mCapId(-1)
        						,mTransport(0)
	        		    		,mRenderer(0)
			                    ,sessionId(-1) 
								,mVideoEngine(0)
								,mEnginePlaying(false)
  {
  	printf("\n WebrtcVideoConduit : Constructor ");
    Construct(); 
  }

  virtual ~WebrtcVideoConduit() 
  {
   Destruct();
  }


private:
  void Construct();
  void Destruct();

  bool initDone;
  int mChannel;
  int mCapId;
  mozilla::RefPtr<TransportInterface> mTransport;
  mozilla::RefPtr<VideoRenderer> mRenderer; 
  uint32_t sessionId; // this session - for book-keeping
  webrtc::VideoEngine* mVideoEngine; 
  webrtc::ViEBase* mPtrViEBase;
  webrtc::ViECapture* mPtrViECapture;
  webrtc::ViECodec* mPtrViECodec;
  webrtc::ViENetwork* mPtrViENetwork;
  webrtc::ViERender* mPtrViERender;
  webrtc::ViEExternalCapture*  mPtrExtCapture;
  bool mEnginePlaying;
};



} // end namespace
#endif
