#include "mediaplayer/FormatConverter.h"

#include <utility>

namespace mediaplayer {

FormatConverter::FormatConverter() = default;

FormatConverter::~FormatConverter() { wait(); }

void FormatConverter::convertAudioAsync(const std::string &input, const std::string &output,
                                        ProgressCallback progress, CompletionCallback done) {
  wait();
  m_running = true;
  m_thread = std::thread([=]() {
    AudioConverter conv;
    bool ok = conv.convert(input, output, progress);
    if (done)
      done(ok);
    m_running = false;
  });
}

void FormatConverter::convertVideoAsync(const std::string &input, const std::string &output,
                                        int width, int height, int bitrate,
                                        ProgressCallback progress, CompletionCallback done) {
  wait();
  m_running = true;
  m_thread = std::thread([=]() {
    VideoConverter conv;
    bool ok = conv.convert(input, output, width, height, bitrate, progress);
    if (done)
      done(ok);
    m_running = false;
  });
}

void FormatConverter::wait() {
  if (m_thread.joinable())
    m_thread.join();
}

bool FormatConverter::isRunning() const { return m_running.load(); }

} // namespace mediaplayer
