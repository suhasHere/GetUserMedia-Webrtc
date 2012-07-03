
#ifndef CODEC_CONFIG
#define CODEC_CONFIG

#include <string>


namespace mozilla {

struct CodecConfig
{
  int mType;
  std::string mName;
  int mFreq;
  int mPacSize;
  int mChannels;
  int mRate;

  CodecConfig()
  {
  }

  CodecConfig(int type, std::string name, int freq, int pacSize, int channels, int rate) {
    mType = type;
    mName = name;
    mFreq = freq;
    mPacSize = pacSize;
    mChannels = channels;
    mRate = rate;
  }
 
};

};
#endif


