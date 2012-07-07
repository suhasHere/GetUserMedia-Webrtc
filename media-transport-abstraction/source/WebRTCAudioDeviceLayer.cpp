// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Pieces of this code has been taken from Chromium source

#include "WebRTCAudioDeviceLayer.h"


namespace mozilla 
{

//Input sample rates
static int kInputRates[] = {96000, 48000, 44100, 32000, 16000, 8000};

//Output sample rates
static int kOutputRates[] = {96000, 48000, 44100};

WebRtcAudioDeviceLayer::WebRtcAudioDeviceLayer()
      :ref_count_(0), 
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
   std::cout << "WebRtcAudioDeviceLayer:: WebRtcAudioDeviceLayer" << std::endl;
}

WebRtcAudioDeviceLayer::~WebRtcAudioDeviceLayer() {
   std::cout << "WebRtcAudioDeviceLayer:: ~WebRtcAudioDeviceLayer" << std::endl;
}

int32_t WebRtcAudioDeviceLayer::AddRef() {
   std::cout << "WebRtcAudioDeviceLayer:: AddRef" << std::endl;
  ref_count_++;
  return ref_count_;
}

int32_t WebRtcAudioDeviceLayer::Release() {
   std::cout << "WebRtcAudioDeviceLayer:: Release" << std::endl;
  if(--ref_count_ == 0)
  {
    delete this;
  }
  return ref_count_;
}


int32_t WebRtcAudioDeviceLayer::ChangeUniqueId(const int32_t id) {
   std::cout << "WebRtcAudioDeviceLayer:: ChangeUniqId" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::TimeUntilNextProcess() {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::Process() {
  return 0;
}

int32_t WebRtcAudioDeviceLayer::ActiveAudioLayer(AudioLayer* audio_layer) const {
   std::cout << "WebRtcAudioDeviceLayer:: ActiveAudioLayer" << std::endl;
  return -1;
}

webrtc::AudioDeviceModule::ErrorCode WebRtcAudioDeviceLayer::LastError() const {
   std::cout << "WebRtcAudioDeviceLayer:: LastError" << std::endl;
  return last_error_;
}

int32_t WebRtcAudioDeviceLayer::RegisterEventObserver(
    webrtc::AudioDeviceObserver* event_callback) {
   std::cout << "WebRtcAudioDeviceLayer:: RegEventObserver" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RegisterAudioCallback(
    webrtc::AudioTransport* audio_callback) {
   std::cout << "WebRtcAudioDeviceLayer:: RegAudioCallback" << std::endl;
  audio_transport_callback_ = audio_callback;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::Init() {

   std::cout << "WebRtcAudioDeviceLayer:: Init" << std::endl;
  return 0;
}
int32_t WebRtcAudioDeviceLayer::Terminate() {
   std::cout << "WebRtcAudioDeviceLayer:: Terminate`" << std::endl;
  initialized_ = false;
  return 0;
}

bool WebRtcAudioDeviceLayer::Initialized() const {
   std::cout << "WebRtcAudioDeviceLayer:: Initialized" << std::endl;
  return initialized_;
}

int16_t WebRtcAudioDeviceLayer::PlayoutDevices() {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutDevices" << std::endl;
  return -1;
}

int16_t WebRtcAudioDeviceLayer::RecordingDevices() {
   std::cout << "WebRtcAudioDeviceLayer:: RecDevices" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutDevName" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RecordingDeviceName(
    uint16_t index,
    char name[webrtc::kAdmMaxDeviceNameSize],
    char guid[webrtc::kAdmMaxGuidSize]) {
   std::cout << "WebRtcAudioDeviceLayer:: RecDevName" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutDevice(uint16_t index) {
   std::cout << "WebRtcAudioDeviceLayer:: SetPlaputDev" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutDevice(WindowsDeviceType device) {
   std::cout << "WebRtcAudioDeviceLayer:: SetplaputDevWin" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingDevice(uint16_t index) {
   std::cout << "WebRtcAudioDeviceLayer:: SetRecDev" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingDevice(WindowsDeviceType device) {
   std::cout << "WebRtcAudioDeviceLayer:: SetRecDevWin" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::PlayoutIsAvailable(bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutIsAvail" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitPlayout() {
   std::cout << "WebRtcAudioDeviceLayer:: InitPlayout" << std::endl;
  return 0;
}

bool WebRtcAudioDeviceLayer::PlayoutIsInitialized() const {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutisInit" << std::endl;
  return true;
}

int32_t WebRtcAudioDeviceLayer::RecordingIsAvailable(bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: RecisAvail" << std::endl;
  *available = true; 
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitRecording() {
   std::cout << "WebRtcAudioDeviceLayer:: InitRec" << std::endl;
  return 0;
}

bool WebRtcAudioDeviceLayer::RecordingIsInitialized() const {
   std::cout << "WebRtcAudioDeviceLayer:: RecisInit" << std::endl;
  return true; 
}

int32_t WebRtcAudioDeviceLayer::StartPlayout() {
   std::cout << "WebRtcAudioDeviceLayer:: StartPlayout" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StopPlayout() {
   std::cout << "WebRtcAudioDeviceLayer:: StopPlayput" << std::endl;
  playing_ = false;
  return 0;
}

bool WebRtcAudioDeviceLayer::Playing() const {
   std::cout << "WebRtcAudioDeviceLayer:: Playing" << std::endl;
  return playing_;
}

int32_t WebRtcAudioDeviceLayer::StartRecording() {
   std::cout << "WebRtcAudioDeviceLayer:: StartRec" << std::endl;
  recording_ = true;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StopRecording() {
   std::cout << "WebRtcAudioDeviceLayer:: StopRecording" << std::endl;
  recording_ = false;
  return 0;
}

bool WebRtcAudioDeviceLayer::Recording() const {
   std::cout << "WebRtcAudioDeviceLayer:: Recortfin?" << std::endl;
  return recording_;
}

int32_t WebRtcAudioDeviceLayer::SetAGC(bool enable) {
   std::cout << "WebRtcAudioDeviceLayer:: SetAgc" << std::endl;
  agc_is_enabled_ = enable;
  return 0;
}

bool WebRtcAudioDeviceLayer::AGC() const {
   std::cout << "WebRtcAudioDeviceLayer:: Agc()" << std::endl;
  // To reduce the usage of IPC messages, an internal AGC state is used.
  // TODO(henrika): investigate if there is a need for a "deeper" getter.
  return agc_is_enabled_;
}

int32_t WebRtcAudioDeviceLayer::SetWaveOutVolume(uint16_t volume_left,
                                                uint16_t volume_right) {
   std::cout << "WebRtcAudioDeviceLayer:: SetWavOutVol" << std::endl;
  return -1;
}
int32_t WebRtcAudioDeviceLayer::WaveOutVolume(
    uint16_t* volume_left,
    uint16_t* volume_right) const {
   std::cout << "WebRtcAudioDeviceLayer:: WavOutVol" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerIsAvailable(bool* available) {
  *available = true;
   std::cout << "WebRtcAudioDeviceLayer:: SpkrisAvail" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitSpeaker() {
   std::cout << "WebRtcAudioDeviceLayer:: InitSpkr" << std::endl;
  return 0;
}

bool WebRtcAudioDeviceLayer::SpeakerIsInitialized() const {
   std::cout << "WebRtcAudioDeviceLayer:: SpkrsIsInitn" << std::endl;
  return true;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneIsAvailable(bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: MicisAvaial" << std::endl;
  *available = true;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::InitMicrophone() {
   std::cout << "WebRtcAudioDeviceLayer:: InitMic" << std::endl;
  return 0;
}

bool WebRtcAudioDeviceLayer::MicrophoneIsInitialized() const {
   std::cout << "WebRtcAudioDeviceLayer:: MicIsInit" << std::endl;
  return true;
}

int32_t WebRtcAudioDeviceLayer::SpeakerVolumeIsAvailable(
    bool* available) {
   std::cout << "WebRtcAudioDeviceLayer::SpeakerVolumeIsAvailable " << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetSpeakerVolume(uint32_t volume) {
   std::cout << "WebRtcAudioDeviceLayer:: SetSpkrVol" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerVolume(uint32_t* volume) const {
   std::cout << "WebRtcAudioDeviceLayer:: SpeakerVol" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MaxSpeakerVolume(uint32_t* max_volume) const {
   std::cout << "WebRtcAudioDeviceLayer:: MaxSpeakerVolume" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MinSpeakerVolume(uint32_t* min_volume) const {
   std::cout << "WebRtcAudioDeviceLayer:: MinSpeakerVolume" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerVolumeStepSize(
    uint16_t* step_size) const {
   std::cout << "WebRtcAudioDeviceLayer:: SpeakerVolumeStepSize" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneVolumeIsAvailable(bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneVolumeIsAvailable" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetMicrophoneVolume(uint32_t volume) {
   std::cout << "WebRtcAudioDeviceLayer:: SetMicrophoneVolume" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneVolume(uint32_t* volume) const {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneVolume" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MaxMicrophoneVolume(uint32_t* max_volume) const {
   std::cout << "WebRtcAudioDeviceLayer:: MinMicrophoneVolume" << std::endl;
  *max_volume = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::MinMicrophoneVolume(uint32_t* min_volume) const {
   std::cout << "WebRtcAudioDeviceLayer:: MinMicrophoneVolume" << std::endl;
  *min_volume = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneVolumeStepSize(
    uint16_t* step_size) const {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneVolumeStepSize" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerMuteIsAvailable(bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneMuteIsAvailabl" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetSpeakerMute(bool enable) {
   std::cout << "WebRtcAudioDeviceLayer::SetSpeakerMute" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SpeakerMute(bool* enabled) const {
   std::cout << "WebRtcAudioDeviceLayer:: SpeakerMute" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneMuteIsAvailable(
    bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneMuteIsAvailabl" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetMicrophoneMute(bool enable) {
   std::cout << "WebRtcAudioDeviceLayer:: SetMicrophoneMute" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneMute(bool* enabled) const {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneMute" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneBoostIsAvailable(bool* available) {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneBoostIsAvailable" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetMicrophoneBoost(bool enable) {
  return -1;
   std::cout << "WebRtcAudioDeviceLayer:: SetMicrophoneBoost" << std::endl;
}

int32_t WebRtcAudioDeviceLayer::MicrophoneBoost(bool* enabled) const {
   std::cout << "WebRtcAudioDeviceLayer:: MicrophoneBoost" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StereoPlayoutIsAvailable(bool* available) const {
   std::cout << "WebRtcAudioDeviceLayer:: StereoPlayoutIsAvailable" << std::endl;
  *available = false; 
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetStereoPlayout(bool enable) {
   std::cout << "WebRtcAudioDeviceLayer:: SetStereoPlayout" << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StereoPlayout(bool* enabled) const {
   std::cout << "WebRtcAudioDeviceLayer::StereoPlayout " << std::endl;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::StereoRecordingIsAvailable(
    bool* available) const {
   std::cout << "WebRtcAudioDeviceLayer:: StereoRecordingIsAvailable" << std::endl;
  *available = 1;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetStereoRecording(bool enable) {
   std::cout << "WebRtcAudioDeviceLayer:: SetStereoRecording" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StereoRecording(bool* enabled) const {
   std::cout << "WebRtcAudioDeviceLayer:: StereoRecording" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingChannel(const ChannelType channel) {
   std::cout << "WebRtcAudioDeviceLayer:: SetRecordingChannel" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RecordingChannel(ChannelType* channel) const {
   std::cout << "WebRtcAudioDeviceLayer:: RecordingChannel" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutBuffer(const BufferType type,
                                                uint16_t size_ms) {
   std::cout << "WebRtcAudioDeviceLayer:: SetPlayoutBuffer" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutBuffer(BufferType* type,
                                             uint16_t* size_ms) const {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutBuffer" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutDelay(uint16_t* delay_ms) const {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutDelay" << std::endl;
  // Report the cached output delay value.
  *delay_ms = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::RecordingDelay(uint16_t* delay_ms) const {
   std::cout << "WebRtcAudioDeviceLayer:: RecordingDelay" << std::endl;
  *delay_ms = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::CPULoad(uint16_t* load) const {
   std::cout << "WebRtcAudioDeviceLayer:: CPULoad" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StartRawOutputFileRecording(
    const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) {
   std::cout << "WebRtcAudioDeviceLayer:: rStartRawOutputFileRecording" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StopRawOutputFileRecording() {
   std::cout << "WebRtcAudioDeviceLayer:: StopRawOutputFileRecording" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StartRawInputFileRecording(
    const char pcm_file_name_utf8[webrtc::kAdmMaxFileNameSize]) {
   std::cout << "WebRtcAudioDeviceLayer:: StartRawInputFileRecording" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::StopRawInputFileRecording() {
   std::cout << "WebRtcAudioDeviceLayer:: StopRawInputFileRecording" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetRecordingSampleRate(
    const uint32_t samples_per_sec) {
   std::cout << "WebRtcAudioDeviceLayer:: SetRecordingSampleRate" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::RecordingSampleRate(
    uint32_t* samples_per_sec) const {
   std::cout << "WebRtcAudioDeviceLayer::RecordingSmapleRate" << std::endl;
  *samples_per_sec = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::SetPlayoutSampleRate(
    const uint32_t samples_per_sec) {
   std::cout << "WebRtcAudioDeviceLayer:: SetPlayoutSampleRate" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::PlayoutSampleRate(
    uint32_t* samples_per_sec) const {
   std::cout << "WebRtcAudioDeviceLayer:: PlayoutSampleRate" << std::endl;
  *samples_per_sec = 0;
  return 0;
}

int32_t WebRtcAudioDeviceLayer::ResetAudioDevice() {
   std::cout << "WebRtcAudioDeviceLayer:: ResetAudioDevice" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::SetLoudspeakerStatus(bool enable) {
   std::cout << "WebRtcAudioDeviceLayer:: SetLoudSpkrSts" << std::endl;
  return -1;
}

int32_t WebRtcAudioDeviceLayer::GetLoudspeakerStatus(bool* enabled) const {
   std::cout << "WebRtcAudioDeviceLayer:: GetLoudSpkrSts" << std::endl;
  return -1;
}

void WebRtcAudioDeviceLayer::SetSessionId(int session_id) {
   std::cout << "WebRtcAudioDeviceLayer:: SetSession" << std::endl;
  session_id_ = session_id;
}

} //namespace


