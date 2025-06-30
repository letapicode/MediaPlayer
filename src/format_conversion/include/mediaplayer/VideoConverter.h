#ifndef MEDIAPLAYER_VIDEOCONVERTER_H
#define MEDIAPLAYER_VIDEOCONVERTER_H

#include "EncodeOptions.h"
#include <atomic>
#include <functional>
#include <string>

namespace mediaplayer {

class VideoConverter {
public:
  // Convert input video to output path. If width/height are 0, keep input size.
  // Bitrate is in bits per second.
  bool convert(const std::string &inputPath, const std::string &outputPath,
               const VideoEncodeOptions &options = {}, std::function<void(float)> progress = {},
               std::atomic<bool> *cancelFlag = nullptr);
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOCONVERTER_H
