#ifndef MEDIAPLAYER_ENCODEOPTIONS_H
#define MEDIAPLAYER_ENCODEOPTIONS_H

#include <string>

namespace mediaplayer {

struct AudioEncodeOptions {
  int bitrate = 192000; // bits per second
  int sampleRate = 0;   // 0 = keep input sample rate
  std::string codec;    // encoder name, empty for default
};

struct VideoEncodeOptions {
  int width = 0;         // 0 = keep input width
  int height = 0;        // 0 = keep input height
  int bitrate = 1000000; // bits per second
  std::string codec;     // encoder name, empty for default
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_ENCODEOPTIONS_H
