#ifndef MEDIAPLAYER_AUDIOCONVERTER_H
#define MEDIAPLAYER_AUDIOCONVERTER_H

#include "EncodeOptions.h"
#include <atomic>
#include <functional>
#include <string>

namespace mediaplayer {

class AudioConverter {
public:
  // Convert input audio file to the format implied by outputPath extension.
  // Returns true on success.
  bool convert(const std::string &inputPath, const std::string &outputPath,
               const AudioEncodeOptions &options = {}, std::function<void(float)> progress = {},
               std::atomic<bool> *cancelFlag = nullptr);
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOCONVERTER_H
