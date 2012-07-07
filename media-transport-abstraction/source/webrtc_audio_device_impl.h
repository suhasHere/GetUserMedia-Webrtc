// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_WEBRTC_AUDIO_DEVICE_IMPL_H_
#define CONTENT_RENDERER_MEDIA_WEBRTC_AUDIO_DEVICE_IMPL_H_
#pragma once

#include <string>
#include <vector>
#include "modules/audio_device/main/interface/audio_device.h"

class WebRtcAudioDeviceImpl
    : public webrtc::AudioDeviceModule {
 public:
  // Methods called on main render thread.
  WebRtcAudioDeviceImpl();

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

  // Make destructor private to ensure that we can only be deleted by Release().
  virtual ~WebRtcAudioDeviceImpl();
 private:

  // Weak reference to the audio callback.
  // The webrtc client defines |audio_transport_callback_| by calling
  // RegisterAudioCallback().
  webrtc::AudioTransport* audio_transport_callback_;

  // Cached values of utilized audio parameters. Platform dependent.

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

//IMPLEMENTATION
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webrtc_audio_device_impl.h"


static const double kMaxVolumeLevel = 255.0;


WebRtcAudioDeviceImpl::WebRtcAudioDeviceImpl()
      : audio_transport_callback_(NULL),
      input_delay_ms_(0),
      output_delay_ms_(0),
      last_error_(AudioDeviceModule::kAdmErrNone),
      session_id_(0),
      bytes_per_sample_(0),
      initialized_(false),
      playing_(false),
      recording_(false),
      agc_is_enabled_(false) {
}

WebRtcAudioDeviceImpl::~WebRtcAudioDeviceImpl() {
}


int32_t WebRtcAudioDeviceImpl::ChangeUniqueId(const int32_t id) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::TimeUntilNextProcess() {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::Process() {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::ActiveAudioLayer(AudioLayer* audio_layer) const {
  return -1;
}

webrtc::AudioDeviceModule::ErrorCode WebRtcAudioDeviceImpl::LastError() const {
  return last_error_;
}

int32_t WebRtcAudioDeviceImpl::RegisterEventObserver(
    webrtc::AudioDeviceObserver* event_callback) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::RegisterAudioCallback(
    webrtc::AudioTransport* audio_callback) {
  audio_transport_callback_ = audio_callback;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::Init() {

  return 0;
}
int32_t WebRtcAudioDeviceImpl::Terminate() {
  initialized_ = false;
  return 0;
}

bool WebRtcAudioDeviceImpl::Initialized() const {
  return initialized_;
}

int16_t WebRtcAudioDeviceImpl::PlayoutDevices() {
  return -1;
}

int16_t WebRtcAudioDeviceImpl::RecordingDevices() {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::PlayoutDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::RecordingDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetPlayoutDevice(uint16_t index) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::SetPlayoutDevice(WindowsDeviceType device) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::SetRecordingDevice(uint16_t index) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::SetRecordingDevice(WindowsDeviceType device) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::PlayoutIsAvailable(bool* available) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::InitPlayout() {
  return 0;
}

bool WebRtcAudioDeviceImpl::PlayoutIsInitialized() const {
  return true;
}

int32_t WebRtcAudioDeviceImpl::RecordingIsAvailable(bool* available) {
  *available = true; 
  return 0;
}

int32_t WebRtcAudioDeviceImpl::InitRecording() {
  return 0;
}

bool WebRtcAudioDeviceImpl::RecordingIsInitialized() const {
  return true; 
}

int32_t WebRtcAudioDeviceImpl::StartPlayout() {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::StopPlayout() {
  playing_ = false;
  return 0;
}

bool WebRtcAudioDeviceImpl::Playing() const {
  return playing_;
}

int32_t WebRtcAudioDeviceImpl::StartRecording() {
  recording_ = true;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::StopRecording() {
  recording_ = false;
  return 0;
}

bool WebRtcAudioDeviceImpl::Recording() const {
  return recording_;
}

int32_t WebRtcAudioDeviceImpl::SetAGC(bool enable) {
  agc_is_enabled_ = enable;
  return 0;
}

bool WebRtcAudioDeviceImpl::AGC() const {
  // To reduce the usage of IPC messages, an internal AGC state is used.
  // TODO(henrika): investigate if there is a need for a "deeper" getter.
  return agc_is_enabled_;
}

int32_t WebRtcAudioDeviceImpl::SetWaveOutVolume(uint16_t volume_left,
                                                uint16_t volume_right) {
  return -1;
}
int32_t WebRtcAudioDeviceImpl::WaveOutVolume(
    uint16_t* volume_left,
    uint16_t* volume_right) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SpeakerIsAvailable(bool* available) {
  *available = true;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::InitSpeaker() {
  return 0;
}

bool WebRtcAudioDeviceImpl::SpeakerIsInitialized() const {
  return true;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneIsAvailable(bool* available) {
  *available = true;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::InitMicrophone() {
  return 0;
}

bool WebRtcAudioDeviceImpl::MicrophoneIsInitialized() const {
  return true;
}

int32_t WebRtcAudioDeviceImpl::SpeakerVolumeIsAvailable(
    bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetSpeakerVolume(uint32_t volume) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SpeakerVolume(uint32_t* volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MaxSpeakerVolume(uint32_t* max_volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MinSpeakerVolume(uint32_t* min_volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SpeakerVolumeStepSize(
    uint16_t* step_size) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneVolumeIsAvailable(bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetMicrophoneVolume(uint32_t volume) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneVolume(uint32_t* volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MaxMicrophoneVolume(uint32_t* max_volume) const {
  *max_volume = 0;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::MinMicrophoneVolume(uint32_t* min_volume) const {
  *min_volume = 0;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneVolumeStepSize(
    uint16_t* step_size) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SpeakerMuteIsAvailable(bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetSpeakerMute(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SpeakerMute(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneMuteIsAvailable(
    bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetMicrophoneMute(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneMute(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneBoostIsAvailable(bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetMicrophoneBoost(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::MicrophoneBoost(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::StereoPlayoutIsAvailable(bool* available) const {
  *available = false; 
  return 0;
}

int32_t WebRtcAudioDeviceImpl::SetStereoPlayout(bool enable) {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::StereoPlayout(bool* enabled) const {
  return 0;
}

int32_t WebRtcAudioDeviceImpl::StereoRecordingIsAvailable(
    bool* available) const {
  *available = 1;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::SetStereoRecording(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::StereoRecording(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetRecordingChannel(const ChannelType channel) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::RecordingChannel(ChannelType* channel) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetPlayoutBuffer(const BufferType type,
                                                uint16_t size_ms) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::PlayoutBuffer(BufferType* type,
                                             uint16_t* size_ms) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::PlayoutDelay(uint16_t* delay_ms) const {
  // Report the cached output delay value.
  *delay_ms = 0;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::RecordingDelay(uint16_t* delay_ms) const {
  *delay_ms = 0;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::CPULoad(uint16_t* load) const {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::StartRawOutputFileRecording(
    const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::StopRawOutputFileRecording() {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::StartRawInputFileRecording(
    const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::StopRawInputFileRecording() {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetRecordingSampleRate(
    const uint32_t samples_per_sec) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::RecordingSampleRate(
    uint32_t* samples_per_sec) const {
  *samples_per_sec = 0;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::SetPlayoutSampleRate(
    const uint32_t samples_per_sec) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::PlayoutSampleRate(
    uint32_t* samples_per_sec) const {
  *samples_per_sec = 0;
  return 0;
}

int32_t WebRtcAudioDeviceImpl::ResetAudioDevice() {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::SetLoudspeakerStatus(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceImpl::GetLoudspeakerStatus(bool* enabled) const {
  return -1;
}

void WebRtcAudioDeviceImpl::SetSessionId(int session_id) {
  session_id_ = session_id;
}



#endif  // CONTENT_RENDERER_MEDIA_WEBRTC_AUDIO_DEVICE_IMPL_H_
