#ifndef MEDIAPLAYER_VIDEOCONVERTER_H
#define MEDIAPLAYER_VIDEOCONVERTER_H

#include <functional>
#include <string>

namespace mediaplayer {

class VideoConverter {
public:
  // Convert input video to output path. If width/height are 0, keep input size.
  // Bitrate is in bits per second.
  bool convert(const std::string &inputPath, const std::string &outputPath, int width = 0,
               int height = 0, int bitrate = 1000000, std::function<void(float)> progress = {});
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOCONVERTER_H
