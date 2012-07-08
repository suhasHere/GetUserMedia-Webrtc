
#ifndef CODEC_CONFIG
#define CODEC_CONFIG

#include <string>


namespace mozilla {

struct CodecConfig
{

};

struct AudioCodecConfig : public CodecConfig
{
  int mType;
  std::string mName;
  int mFreq;
  int mPacSize;
  int mChannels;
  int mRate;

  AudioCodecConfig()
  {
  }

  AudioCodecConfig(int type, std::string name, int freq, int pacSize, int channels, int rate) {
    mType = type;
    mName = name;
    mFreq = freq;
    mPacSize = pacSize;
    mChannels = channels;
    mRate = rate;
  }
 
};

//Minimal Config- More can be added later
struct VideoCodecConfig : public CodecConfig
{

  int mType;
  std::string mName;
  int mWidth;
  int mHeight;

  VideoCodecConfig()
  {
  }

  VideoCodecConfig(int type, std::string name,int width, int height)
  {
    mType = type;
    mName = name;
    mWidth = width;
    mHeight = height;
  }

};

};
#endif


