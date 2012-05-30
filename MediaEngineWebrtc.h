/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MEDIAENGINEWEBRTC_H_
#define MEDIAENGINEWEBRTC_H_

#include "prmem.h"
#include "nsITimer.h"
#include "nsIThread.h"
#include "nsIRunnable.h"


#include "nsCOMPtr.h"
#include "nsDOMMediaStream.h"
#include "nsComponentManagerUtils.h"

#include "prthread.h"
#include "nsThreadUtils.h"
#include "Layers.h"
#include "VideoUtils.h"
#include "MediaEngine.h"
#include "ImageLayers.h"
#include "VideoSegment.h"
#include "AudioSegment.h"
#include "StreamBuffer.h"
#include "MediaStreamGraph.h"

//Webrtc Inlcudes - GUM Specific
// Audio Engine Includes
#include "voice_engine/main/interface/voe_base.h"
#include "voice_engine/main/interface/voe_hardware.h"
#include "voice_engine/main/interface/voe_audio_processing.h"
#include "voice_engine/main/interface/voe_volume_control.h"
#include "voice_engine/main/interface/voe_external_media.h"
// Video Engine Includes
#include "video_engine/include/vie_base.h"
#include "video_engine/include/vie_codec.h"
#include "video_engine/include/vie_render.h"
#include "video_engine/include/vie_capture.h"



namespace mozilla {

/**
 * The webrtc implementation of the MediaEngine interface.
 */

enum WebrtcEngineState {
  kAllocated,
  kStarted,
  kStopped,
  kReleased,
};

//Foreward declarations
class VideoStartCaptureEvent;
class VideoRenderToStreamEvent;

//Webrtc Video Subsystem abstraction

class MediaEngineWebrtcVideoSource : public nsITimerCallback,
				     public MediaEngineVideoSource,
				     public webrtc::ExternalRenderer 

{
public:

  // ViEExternalRenderer implementation
  virtual int FrameSizeChange(
        unsigned int, unsigned int, unsigned int
  );

 virtual int DeliverFrame(
        unsigned char*,int, uint32_t , int64_t
  );

 explicit MediaEngineWebrtcVideoSource(webrtc::VideoEngine* videoEnginePtr,
					int index, int aFps = 30) 
					: mTimer(nsnull),
				          mVideoCaptureThread(nsnull),
					  mVideoRenderingThread(nsnull),
					  mVideoEngine(videoEnginePtr),
					  mCapIndex(index),
					  mWidth(352),
					  mHeight(288),
					  mState(kReleased),
					  mMonitor("WebrtcCamera.Monitor"),
					  mFps(aFps),
					  mInitDone(false) { 
		//XXX: eventually move this out of constructor
		Init();
   }

  ~MediaEngineWebrtcVideoSource()
  {
    printf("\n Video Source Destructor Invoked ");
    Shutdown();
  }
 
  virtual void GetName(nsAString&);
  virtual void GetUUID(nsAString&);
  virtual MediaEngineVideoOptions GetOptions();
  virtual already_AddRefed<nsDOMMediaStream> Allocate();
  virtual nsresult Deallocate();
  virtual nsresult Start(SourceMediaStream*, TrackID);
  virtual nsresult Stop();


  NS_DECL_ISUPPORTS
  NS_DECL_NSITIMERCALLBACK

protected:
 
  
  nsCOMPtr<nsITimer> mTimer; // not used 
  nsCOMPtr<nsIThread> mVideoCaptureThread; // QT Run loop for capture needs a thread
  nsCOMPtr<nsIThread> mVideoRenderingThread; // in video-frames to media-stream

private:
  friend class VideoStartCaptureEvent;
  friend class VideoRenderToStreamEvent;
  
  //statics
  static const unsigned int KMaxDeviceNameLength;
  static const unsigned int KMaxUniqueIdLength;
  
  // Initialize the needed Video engine interfaces
  void Init();
  void Shutdown();

  //GUM needs
  webrtc::VideoEngine* mVideoEngine; // weak reference .. dont delete it here
  webrtc::ViEBase* mViEBase;
  webrtc::ViECapture* mViECapture;
  webrtc::ViERender* mViERender;
  webrtc::CaptureCapability mCaps; //doesn't work on OSX

  //capture index for the associated device
  int mCapIndex;
  int mWidth, mHeight;
  TrackID mTrackID;

  WebrtcEngineState mState;
  mozilla::ReentrantMonitor mMonitor; //Monitor for processing webrtc frames
  SourceMediaStream* mSource; //in video-frames 
  int mFps;  // track rate - 30 fps default
  bool mInitDone;
  nsRefPtr<layers::ImageContainer> mImageContainer;
  VideoSegment mVideoSegment;
};

class MediaEngineWebrtcAudioSource : public nsITimerCallback,
                                     public MediaEngineAudioSource
{
public:

  static const unsigned int PLAYOUT_SAMPLE_FREQUENCY; //default is 16000
  
  explicit MediaEngineWebrtcAudioSource(webrtc::VoiceEngine* voiceEngine,
						int aIndex) 
						: mTimer(nsnull),
						  mVoiceEngine(voiceEngine),
						  mMonitor("WebrtcAIn.Monitor"),
						  mCapIndex(aIndex), 
						  mChannel(-1),
						  mInitDone(false),
					          mState(kReleased) {
    	//XXX: Eventually move out of constructor
		Init();		
	}

  ~MediaEngineWebrtcAudioSource() 
  {
    //XXX:eventually plan the right place to invoke it
    printf("\n MediaEngineWebrtcAudioSource Destructor ");
   Shutdown(); 
  }



  virtual void GetName(nsAString&);
  virtual void GetUUID(nsAString&);

  virtual already_AddRefed<nsDOMMediaStream> Allocate();
  virtual nsresult Deallocate();
  virtual nsresult Start(SourceMediaStream*, TrackID);
  virtual nsresult Stop();

  NS_DECL_ISUPPORTS
  NS_DECL_NSITIMERCALLBACK

protected:
  nsCOMPtr<nsITimer> mTimer; // 10ms audio samples timer

private:
  //statics
  static const unsigned int KMaxDeviceNameLength;
  static const unsigned int KMaxUniqueIdLength;
  
  void Init(); //Initialize voice engine interfaces
  void Shutdown();

  //we don't need ref-couting since engine does it for us
  webrtc::VoiceEngine* mVoiceEngine; 
  webrtc::VoEBase* mVoEBase;
  webrtc::VoEHardware* mVoEHw;
  webrtc::VoEExternalMedia* mVoEXmedia; // external playout 

  mozilla::ReentrantMonitor mMonitor; //audio-frame processing (???)
  int mCapIndex;
  int mChannel;
  TrackID mTrackID;
  bool mInitDone;
  WebrtcEngineState mState;
  SourceMediaStream* mSource;
  AudioSegment mAudioSegment;
  
};

class MediaEngineWebrtc : public MediaEngine
{
public:
  MediaEngineWebrtc():mVideoEngine(NULL),
			  mVoiceEngine(NULL),
			  mVideoEngineInit(false),
		          mAudioEngineInit(false) {
  }

  ~MediaEngineWebrtc() {
    printf("\n MediaEngineWebrtc: Destructor ");
	Shutdown();
  }

  //Client's should ensure to clean-up sources video/audio sources
  //before invoking Shutdown on this class.
  void Shutdown();
  virtual void EnumerateVideoDevices(nsTArray<nsRefPtr<MediaEngineVideoSource> >*);
  virtual void EnumerateAudioDevices(nsTArray<nsRefPtr<MediaEngineAudioSource> >*);


private:
  //video engine instance pointer 
  webrtc::VideoEngine* mVideoEngine;
  //audio engine instace pointer
  webrtc::VoiceEngine* mVoiceEngine;	

  // need this to avoid unneccesary webrtc calls while enumerating
  bool mVideoEngineInit;
  bool mAudioEngineInit;

};

}

#endif /* NSMEDIAENGINEWEBRTC_H_ */
