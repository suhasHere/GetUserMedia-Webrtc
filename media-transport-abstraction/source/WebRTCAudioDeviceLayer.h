// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_WEBRTC_AUDIO_DEVICE_IMPL_H_
#define CONTENT_RENDERER_MEDIA_WEBRTC_AUDIO_DEVICE_IMPL_H_
#pragma once

#include <string>
#include <vector>

#include "modules/audio_device/main/interface/audio_device.h"

namespace mozilla 
{

class WebRtcAudioDeviceLayer
    	: public webrtc::AudioDeviceModule
 {
 public:
  WebRtcAudioDeviceLayer();
  
  virtual int32_t AddRef() ;
  virtual int32_t Release() ;



  // webrtc::Module implementation.
  virtual int32_t ChangeUniqueId(const int32_t id) ;
  virtual int32_t TimeUntilNextProcess() ;
  virtual int32_t Process() ;

  // webrtc::AudioDeviceModule implementation.
  virtual int32_t ActiveAudioLayer(AudioLayer* audio_layer) const ;
  virtual ErrorCode LastError() const ;

  virtual int32_t RegisterEventObserver(
      webrtc::AudioDeviceObserver* event_callback) ;
  virtual int32_t RegisterAudioCallback(webrtc::AudioTransport* audio_callback)
      ;

  virtual int32_t Init() ;
  virtual int32_t Terminate() ;
  virtual bool Initialized() const ;

  virtual int16_t PlayoutDevices() ;
  virtual int16_t RecordingDevices() ;
  virtual int32_t PlayoutDeviceName(uint16_t index,
                                    char name[webrtc::kAdmMaxDeviceNameSize],
                                    char guid[webrtc::kAdmMaxGuidSize])
                                    ;
  virtual int32_t RecordingDeviceName(uint16_t index,
                                      char name[webrtc::kAdmMaxDeviceNameSize],
                                      char guid[webrtc::kAdmMaxGuidSize])
                                      ;
  virtual int32_t SetPlayoutDevice(uint16_t index) ;
  virtual int32_t SetPlayoutDevice(WindowsDeviceType device) ;
  virtual int32_t SetRecordingDevice(uint16_t index) ;
  virtual int32_t SetRecordingDevice(WindowsDeviceType device) ;

  virtual int32_t PlayoutIsAvailable(bool* available) ;
  virtual int32_t InitPlayout() ;
  virtual bool PlayoutIsInitialized() const ;
  virtual int32_t RecordingIsAvailable(bool* available) ;
  virtual int32_t InitRecording() ;
  virtual bool RecordingIsInitialized() const ;

  virtual int32_t StartPlayout() ;
  virtual int32_t StopPlayout() ;
  virtual bool Playing() const ;
  virtual int32_t StartRecording() ;
  virtual int32_t StopRecording() ;
  virtual bool Recording() const ;

  virtual int32_t SetAGC(bool enable) ;
  virtual bool AGC() const ;

  virtual int32_t SetWaveOutVolume(uint16_t volume_left,
                                   uint16_t volume_right) ;
  virtual int32_t WaveOutVolume(uint16_t* volume_left,
                                uint16_t* volume_right) const ;

  virtual int32_t SpeakerIsAvailable(bool* available) ;
  virtual int32_t InitSpeaker() ;
  virtual bool SpeakerIsInitialized() const ;
  virtual int32_t MicrophoneIsAvailable(bool* available) ;
  virtual int32_t InitMicrophone() ;
  virtual bool MicrophoneIsInitialized() const ;

  virtual int32_t SpeakerVolumeIsAvailable(bool* available) ;
  virtual int32_t SetSpeakerVolume(uint32_t volume) ;
  virtual int32_t SpeakerVolume(uint32_t* volume) const ;
  virtual int32_t MaxSpeakerVolume(uint32_t* max_volume) const ;
  virtual int32_t MinSpeakerVolume(uint32_t* min_volume) const ;
  virtual int32_t SpeakerVolumeStepSize(uint16_t* step_size) const ;

  virtual int32_t MicrophoneVolumeIsAvailable(bool* available) ;
  virtual int32_t SetMicrophoneVolume(uint32_t volume) ;
  virtual int32_t MicrophoneVolume(uint32_t* volume) const ;
  virtual int32_t MaxMicrophoneVolume(uint32_t* max_volume) const ;
  virtual int32_t MinMicrophoneVolume(uint32_t* min_volume) const ;
  virtual int32_t MicrophoneVolumeStepSize(uint16_t* step_size) const ;

  virtual int32_t SpeakerMuteIsAvailable(bool* available) ;
  virtual int32_t SetSpeakerMute(bool enable) ;
  virtual int32_t SpeakerMute(bool* enabled) const ;

  virtual int32_t MicrophoneMuteIsAvailable(bool* available) ;
  virtual int32_t SetMicrophoneMute(bool enable) ;
  virtual int32_t MicrophoneMute(bool* enabled) const ;

  virtual int32_t MicrophoneBoostIsAvailable(bool* available) ;
  virtual int32_t SetMicrophoneBoost(bool enable) ;
  virtual int32_t MicrophoneBoost(bool* enabled) const ;

  virtual int32_t StereoPlayoutIsAvailable(bool* available) const ;
  virtual int32_t SetStereoPlayout(bool enable) ;
  virtual int32_t StereoPlayout(bool* enabled) const ;
  virtual int32_t StereoRecordingIsAvailable(bool* available) const ;
  virtual int32_t SetStereoRecording(bool enable) ;
  virtual int32_t StereoRecording(bool* enabled) const ;
  virtual int32_t SetRecordingChannel(const ChannelType channel) ;
  virtual int32_t RecordingChannel(ChannelType* channel) const ;

  virtual int32_t SetPlayoutBuffer(
      const BufferType type, uint16_t size_ms) ;
  virtual int32_t PlayoutBuffer(
      BufferType* type, uint16_t* size_ms) const ;
  virtual int32_t PlayoutDelay(uint16_t* delay_ms) const ;
  virtual int32_t RecordingDelay(uint16_t* delay_ms) const ;

  virtual int32_t CPULoad(uint16_t* load) const ;

  virtual int32_t StartRawOutputFileRecording(
      const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) ;
  virtual int32_t StopRawOutputFileRecording() ;
  virtual int32_t StartRawInputFileRecording(
      const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) ;
  virtual int32_t StopRawInputFileRecording() ;

  virtual int32_t SetRecordingSampleRate(
      const uint32_t samples_per_sec) ;
  virtual int32_t RecordingSampleRate(uint32_t* samples_per_sec) const ;
  virtual int32_t SetPlayoutSampleRate(const uint32_t samples_per_sec) ;
  virtual int32_t PlayoutSampleRate(uint32_t* samples_per_sec) const ;

  virtual int32_t ResetAudioDevice() ;
  virtual int32_t SetLoudspeakerStatus(bool enable) ;
  virtual int32_t GetLoudspeakerStatus(bool* enabled) const ;

  // Sets the session id.
  void SetSessionId(int session_id);


  int input_delay_ms() const { return input_delay_ms_; }
  int output_delay_ms() const { return output_delay_ms_; }
  bool initialized() const { return initialized_; }
  bool playing() const { return playing_; }
  bool recording() const { return recording_; }

  virtual ~WebRtcAudioDeviceLayer();
 private:

  int ref_count_;

  // Weak reference to the audio callback.
  webrtc::AudioTransport* audio_transport_callback_;

  int input_delay_ms_;
  int output_delay_ms_;

  webrtc::AudioDeviceModule::ErrorCode last_error_;

  int session_id_;
  int bytes_per_sample_;
  bool initialized_;
  bool playing_;
  bool recording_;

  // Local copy of the current Automatic Gain Control state.
  bool agc_is_enabled_;

};

}
#endif
