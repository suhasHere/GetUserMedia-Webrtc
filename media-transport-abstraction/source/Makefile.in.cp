# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is Mozilla code.
#
# The Initial Developer of the Original Code is the Mozilla Foundation.
# Portions created by the Initial Developer are Copyright (C) 2010
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#  Chris Pearce <chris@pearce.org.nz>
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****


DEPTH = ../../../
topsrcdir = @top_srcdir@
srcdir = @srcdir@
VPATH = @srcdir@

include $(DEPTH)/config/autoconf.mk

MODULE = test_mtransport

DEFINES += -DHAVE_STRDUP -DNR_SOCKET='void *' \
	$(NULL)

LIBS		= \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_aec/libaec.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_aecm/libaecm.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_agc/libagc.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_apm_util/libapm_util.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_audio_coding_module/libaudio_coding_module.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_audio_conference_mixer/libaudio_conference_mixer.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_audio_device/libaudio_device.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_audio_processing/libaudio_processing.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_CNG/libCNG.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_G711/libG711.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_G722/libG722.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_iLBC/libiLBC.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_iSAC/libiSAC.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_iSACFix/libiSACFix.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_media_file/libmedia_file.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_ns/libns.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_PCM16B/libPCM16B.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_NetEq/libNetEq.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/common_audio/common_audio_resampler/libresampler.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_rtp_rtcp/librtp_rtcp.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/common_audio/common_audio_signal_processing/libsignal_processing.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/common_audio/common_audio_vad/libvad.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/system_wrappers/source/system_wrappers_system_wrappers/libsystem_wrappers.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_udp_transport/libudp_transport.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_video_capture_module/libvideo_capture_module.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/video_engine/video_engine_video_engine_core/libvideo_engine_core.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_video_processing/libvideo_processing.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_video_render_module/libvideo_render_module.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/voice_engine/voice_engine_voice_engine_core/libvoice_engine_core.$(LIB_SUFFIX) \
  $(DEPTH)media/libvpx/libvpx.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_webrtc_i420/libwebrtc_i420.$(LIB_SUFFIX) \
  $(DEPTH)media/libjpeg/libmozjpeg.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/common_video/common_video_webrtc_jpeg/libwebrtc_jpeg.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_webrtc_utility/libwebrtc_utility.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_webrtc_video_coding/libwebrtc_video_coding.$(LIB_SUFFIX) \
 $(DEPTH)media/webrtc/trunk/src/modules/modules_webrtc_vp8/libwebrtc_vp8.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/common_video/common_video_webrtc_libyuv/libwebrtc_libyuv.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/third_party/libyuv/libyuv_libyuv/libyuv.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_video_processing_sse2/libvideo_processing_sse2.$(LIB_SUFFIX) \
  $(DEPTH)media/webrtc/trunk/src/modules/modules_aec_sse2/libaec_sse2.$(LIB_SUFFIX) \
  $(EXTRA_DSO_LIBS) \
  $(XPCOM_LIBS) \
  $(NSPR_LIBS) \
  $(NSS_LIBS) \
  $(DEPTH)/media/mtransport/$(LIB_PREFIX)mtransport.$(LIB_SUFFIX) \
  $(DEPTH)/media/mtransport/third_party/nICEr/nicer_nicer/$(LIB_PREFIX)nicer.$(LIB_SUFFIX) \
  $(DEPTH)/media/mtransport/third_party/nrappkit/nrappkit_nrappkit/$(LIB_PREFIX)nrappkit.$(LIB_SUFFIX) \
  $(DEPTH)/media/webrtc/trunk/testing/gtest_gtest/$(LIB_PREFIX)gtest.$(LIB_SUFFIX) \
  $(NULL)

ifeq ($(OS_ARCH),Darwin)
LIBS += \
  -framework AudioToolbox \
  -framework AudioUnit \
  -framework Carbon \
  -framework CoreAudio \
  -framework OpenGL \
  -framework QTKit \
  -framework QuartzCore \
 -framework Security \
  -framework SystemConfiguration \
 $(NULL)
endif


LOCAL_INCLUDES += \
 -I. \
 -I$(topsrcdir)/media/webrtc/signaling/transport/ \
 -I$(topsrcdir)/media/webrtc/trunk/third_party/libjingle/source/ \
 -I$(topsrcdir)/media/webrtc/trunk/testing/gtest/include/ \
 -I$(topsrcdir)/media/mtransport/ \
 $(NULL)

LOCAL_INCLUDES += \
 -I. \
 -I$(topsrcdir)/media/webrtc/trunk/third_party/libjingle/source/ \
 -I$(topsrcdir)/media/mtransport/third_party/ \
 -I$(topsrcdir)/media/mtransport/third_party/ \
 -I$(topsrcdir)/media/mtransport/third_party/nICEr/src/crypto \
 -I$(topsrcdir)/media/mtransport/third_party/nICEr/src/ice \
 -I$(topsrcdir)/media/mtransport/third_party/nICEr/src/net \
 -I$(topsrcdir)/media/mtransport/third_party/nICEr/src/stun \
 -I$(topsrcdir)/media/mtransport/third_party/nICEr/src/util \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/share \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/util/libekr \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/log \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/registry \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/stats \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/plugin \
 -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/event \

# TODO: need to expand for other platforms
ifeq ($(OS_ARCH), Darwin)
LOCAL_INCLUDES +=  -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/port/darwin/include
endif

ifeq ($(OS_ARCH), Linux)
LOCAL_INCLUDES +=  -I$(topsrcdir)/media/mtransport/third_party/nrappkit/src/port/linux/include
endif

CPP_UNIT_TESTS = \
transportconduit_unittests.cpp \
  $(NULL)

include $(topsrcdir)/config/config.mk
include $(topsrcdir)/media/webrtc/webrtc-config.mk
include $(topsrcdir)/ipc/chromium/chromium-config.mk
include $(topsrcdir)/config/rules.mk
