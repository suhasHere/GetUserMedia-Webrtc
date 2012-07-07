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

