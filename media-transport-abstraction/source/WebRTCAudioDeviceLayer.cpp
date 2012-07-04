//IMPLEMENTATION
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WebRTCAudioDeviceLayer.h"

namespace mozilla 
{

static const double kMaxVolumeLevel = 255.0;


WebRtcAudioDeviceLayer::WebRtcAudioDeviceLayer()
      : ref_count_(0), 
      audio_transport_callback_(NULL),
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

WebRtcAudioDeviceLayer::~WebRtcAudioDeviceLayer() {
}

int32_t WebRtcAudioDeviceLayer::AddRef() {
  ref_count_++;
  return ref_count_;
}

int32_t WebRtcAudioDeviceLayer::Release() {
  if(--ref_count_ == 0)
  {
    delete this;
  }
  return ref_count_;
}


int32_t WebRtcAudioDeviceLayer::ChangeUniqueId(const int32_t id) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::TimeUntilNextProcess() {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::Process() {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::ActiveAudioLayer(AudioLayer* audio_layer) const {
  return -1;
}

webrtc::AudioDeviceModule::ErrorCode WebRtcAudioDeviceLayer::LastError() const {
  return last_error_;
}

int32_t WebRtcAudioDeviceLayer::RegisterEventObserver(
    webrtc::AudioDeviceObserver* event_callback) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RegisterAudioCallback(
    webrtc::AudioTransport* audio_callback) {
  audio_transport_callback_ = audio_callback;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::Init() {

  return 0;
}
int32_t WebRtcAudioDeviceLayer::Terminate() {
  initialized_ = false;
  return 0;
}

bool WebRtcAudioDeviceLayer::Initialized() const {
  return initialized_;
}

int16_t WebRtcAudioDeviceLayer::PlayoutDevices() {
  return -1;
}

int16_t WebRtcAudioDeviceLayer::RecordingDevices() {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RecordingDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutDevice(uint16_t index) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutDevice(WindowsDeviceType device) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingDevice(uint16_t index) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingDevice(WindowsDeviceType device) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::PlayoutIsAvailable(bool* available) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitPlayout() {
  return 0;
}

bool WebRtcAudioDeviceLayer::PlayoutIsInitialized() const {
  return true;
}

int32_t WebRtcAudioDeviceLayer::RecordingIsAvailable(bool* available) {
  *available = true; 
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitRecording() {
  return 0;
}

bool WebRtcAudioDeviceLayer::RecordingIsInitialized() const {
  return true; 
}

int32_t WebRtcAudioDeviceLayer::StartPlayout() {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StopPlayout() {
  playing_ = false;
  return 0;
}

bool WebRtcAudioDeviceLayer::Playing() const {
  return playing_;
}

int32_t WebRtcAudioDeviceLayer::StartRecording() {
  recording_ = true;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StopRecording() {
  recording_ = false;
  return 0;
}

bool WebRtcAudioDeviceLayer::Recording() const {
  return recording_;
}

int32_t WebRtcAudioDeviceLayer::SetAGC(bool enable) {
  agc_is_enabled_ = enable;
  return 0;
}

bool WebRtcAudioDeviceLayer::AGC() const {
  // To reduce the usage of IPC messages, an internal AGC state is used.
  // TODO(henrika): investigate if there is a need for a "deeper" getter.
  return agc_is_enabled_;
}

int32_t WebRtcAudioDeviceLayer::SetWaveOutVolume(uint16_t volume_left,
                                                uint16_t volume_right) {
  return -1;
}
int32_t WebRtcAudioDeviceLayer::WaveOutVolume(
    uint16_t* volume_left,
    uint16_t* volume_right) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerIsAvailable(bool* available) {
  *available = true;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitSpeaker() {
  return 0;
}

bool WebRtcAudioDeviceLayer::SpeakerIsInitialized() const {
  return true;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneIsAvailable(bool* available) {
  *available = true;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitMicrophone() {
  return 0;
}

bool WebRtcAudioDeviceLayer::MicrophoneIsInitialized() const {
  return true;
}

int32_t WebRtcAudioDeviceLayer::SpeakerVolumeIsAvailable(
    bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetSpeakerVolume(uint32_t volume) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerVolume(uint32_t* volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MaxSpeakerVolume(uint32_t* max_volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MinSpeakerVolume(uint32_t* min_volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerVolumeStepSize(
    uint16_t* step_size) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneVolumeIsAvailable(bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetMicrophoneVolume(uint32_t volume) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneVolume(uint32_t* volume) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MaxMicrophoneVolume(uint32_t* max_volume) const {
  *max_volume = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::MinMicrophoneVolume(uint32_t* min_volume) const {
  *min_volume = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneVolumeStepSize(
    uint16_t* step_size) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerMuteIsAvailable(bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetSpeakerMute(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerMute(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneMuteIsAvailable(
    bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetMicrophoneMute(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneMute(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneBoostIsAvailable(bool* available) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetMicrophoneBoost(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneBoost(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StereoPlayoutIsAvailable(bool* available) const {
  *available = false; 
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetStereoPlayout(bool enable) {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StereoPlayout(bool* enabled) const {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StereoRecordingIsAvailable(
    bool* available) const {
  *available = 1;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetStereoRecording(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StereoRecording(bool* enabled) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingChannel(const ChannelType channel) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RecordingChannel(ChannelType* channel) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutBuffer(const BufferType type,
                                                uint16_t size_ms) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutBuffer(BufferType* type,
                                             uint16_t* size_ms) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutDelay(uint16_t* delay_ms) const {
  // Report the cached output delay value.
  *delay_ms = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::RecordingDelay(uint16_t* delay_ms) const {
  *delay_ms = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::CPULoad(uint16_t* load) const {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StartRawOutputFileRecording(
    const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StopRawOutputFileRecording() {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StartRawInputFileRecording(
    const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StopRawInputFileRecording() {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingSampleRate(
    const uint32_t samples_per_sec) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RecordingSampleRate(
    uint32_t* samples_per_sec) const {
  *samples_per_sec = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutSampleRate(
    const uint32_t samples_per_sec) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutSampleRate(
    uint32_t* samples_per_sec) const {
  *samples_per_sec = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::ResetAudioDevice() {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetLoudspeakerStatus(bool enable) {
  return -1;
}

int32_t WebRtcAudioDeviceLayer::GetLoudspeakerStatus(bool* enabled) const {
  return -1;
}

void WebRtcAudioDeviceLayer::SetSessionId(int session_id) {
  session_id_ = session_id;
}

} //namespace


