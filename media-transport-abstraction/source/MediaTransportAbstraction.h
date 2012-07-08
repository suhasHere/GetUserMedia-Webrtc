/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MEDIATRANSPORT_ABSTRACTION_
#define MEDIATRANSPORT_ABSTRACTION_

#include "mozilla/RefPtr.h"
#include "video_engine/include/vie_capture.h"
#include "CodecConfig.h"
#include "VideoTypes.h"


namespace mozilla {
/** 
 * Abstract Interface for transporting RTP packets - audio/vidoeo
 * The consumers of this interface are responsible for passing in
 * the data from the media-sources.
 * public RefCounted<VolumeManager>
 */
class TransportInterface : public mozilla::RefCounted<TransportInterface>
{
public:
  virtual ~TransportInterface() {};

  //send packet to a peer as setup by the transport
  virtual int SendRtpPacket(const void* data, int len) = 0;
  virtual int SendRtcpPacket(const void* data, int len) = 0;
};


/**
 *  Generic renderer for video data
 * On obtaining the frames, the concrete implementation shall
 * be responsbile for connecting to the right outputs.
 */ 
class VideoRenderer : public mozilla::RefCounted<VideoRenderer>
{
 public:
  virtual ~VideoRenderer() {};

  // This method will be called when the stream to be rendered changes in
  // resolution or number of streams mixed in the image.
  virtual int FrameSizeChange(unsigned int width,
                               unsigned int height,
                               unsigned int number_of_streams) = 0;

  // This method is called when a new frame should be rendered.
  virtual int RenderVideoFrame(unsigned char* buffer,
                                unsigned int buffer_size,
								// RTP timestamp
                                uint32_t time_stamp,
                                // Wallclock render time in miliseconds
                                int64_t render_time) = 0;
};



/**
 * Generic renderer for audio data
 * On obtaining the frames, the concrete implementation shall
 * be responsbile for connecting to the right outputs.
 */
class AudioRenderer : public mozilla::RefCounted<AudioRenderer>
{
public: 
  virtual ~AudioRenderer() {};

  // This method is called to render a audio sample
  virtual void RenderAudioFrame(int16_t speechData[],
			  //16000, 32000, 44000, or 48000 Hz 
			  uint32_t samplingFreqHz,
			  //obtained length of the sample
			  int lengthSample) = 0;

};

/**
 * MediaSessionConduit 
 * This interface acts as conduit for
 *  - pass in the media-data to be transported
 *  - grab the frames off the transport for processing
 * Also provides API configuring the media sent and recevied 
 * 
 * TODO: crypt: move codec configuration to a seperate class.
 */
class MediaSessionConduit :  public mozilla::RefCounted<MediaSessionConduit>
{
public:
  virtual ~MediaSessionConduit() {};

  // Transport callbacks on arriving packets on the network 
  virtual void ReceivedRTPPacket(const void *data, int len) = 0;
  virtual void ReceivedRTCPPacket(const void *data, int len) = 0;  

  // Apis to configure codecs for send and recieve media for this session
  virtual int ConfigureSendMediaCodec(CodecConfig* sendSessionConfig) = 0;
  virtual int ConfigureRecvMediaCodec(CodecConfig* recvSessionConfig) = 0;


  // API to attach a transport end-pont for transmitting
  virtual void AttachTransport(mozilla::RefPtr<TransportInterface> aTransport) = 0;

};


/**
 * MediaSessionConduit for video
 *
 */
class VideoSessionConduit : public MediaSessionConduit
{
public:
  // Factory Object 
  static mozilla::TemporaryRef<VideoSessionConduit> Create();

  virtual ~VideoSessionConduit() {};

  virtual int AttachRenderer(mozilla::RefPtr<VideoRenderer> renderer) = 0;

  // This method is called by the user to deliver a new captured frame to
  // VideoEngine to prepare it for eventual transporting.
  virtual int SendVideoFrame(unsigned char* video_frame,
			   unsigned int video_frame_length,
			   unsigned short width,
			   unsigned short height,
			   VideoType video_type,
			   uint64_t capture_time) = 0;

 //test only - send I420 frame
 virtual int SendI420VideoFrame(const webrtc::ViEVideoFrameI420& video_frame,
									unsigned long long captureTime = 0) = 0;


};

/**
 * MediaSessionConduit for audio 
 */
class AudioSessionConduit : public MediaSessionConduit 
{
public:

  // Factory Object 
  static mozilla::TemporaryRef<AudioSessionConduit> Create();

  virtual ~AudioSessionConduit() {};

  // Attach render/playout for the audio data
  virtual int AttachRenderer(mozilla::RefPtr<AudioRenderer> aRenderer) = 0;

  //This method is called to insert externally captured data into
  // Audio Engine
  virtual int SendAudioFrame(const int16_t speechData[], 
			   unsigned int lengthSamples,
			   uint32_t samplingFreqHz,
			   uint64_t capture_time) = 0;

  // Pull based API to get audio sample from the jitter buffe 
  virtual void GetAudioFrame(int16_t speechData[],
                           uint32_t samplingFreqHz,
                           uint64_t capture_delay,
                           unsigned int& lengthSamples) = 0;
  

};


}

#endif

 



 
