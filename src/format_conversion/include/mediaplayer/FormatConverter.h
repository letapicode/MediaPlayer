#ifndef MEDIAPLAYER_FORMATCONVERTER_H
#define MEDIAPLAYER_FORMATCONVERTER_H

#include "AudioConverter.h"
#include "VideoConverter.h"

#include <atomic>
#include <functional>
#include <string>
#include <thread>

namespace mediaplayer {

class FormatConverter {
public:
  using ProgressCallback = std::function<void(float)>;
  using CompletionCallback = std::function<void(bool)>;

  FormatConverter();
  ~FormatConverter();

  void convertAudioAsync(const std::string &input, const std::string &output,
                         ProgressCallback progress = {}, CompletionCallback done = {});

  void convertVideoAsync(const std::string &input, const std::string &output, int width = 0,
                         int height = 0, int bitrate = 1000000, ProgressCallback progress = {},
                         CompletionCallback done = {});

  void wait();
  bool isRunning() const;

private:
  std::thread m_thread;
  std::atomic<bool> m_running{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_FORMATCONVERTER_H
