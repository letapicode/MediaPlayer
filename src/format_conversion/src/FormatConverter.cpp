#include "mediaplayer/FormatConverter.h"

#include <utility>

namespace mediaplayer {

FormatConverter::FormatConverter() = default;

FormatConverter::~FormatConverter() { wait(); }

void FormatConverter::convertAudioAsync(const std::string &input, const std::string &output,
                                        const AudioEncodeOptions &options,
                                        ProgressCallback progress, CompletionCallback done) {
  wait();
  m_cancelFlag = false;
  m_running = true;
  m_thread = std::thread([=, this]() {
    AudioConverter conv;
    bool ok = conv.convert(input, output, options, progress, &this->m_cancelFlag);
    if (done)
      done(ok && !this->m_cancelFlag.load());
    this->m_running = false;
  });
}

void FormatConverter::convertVideoAsync(const std::string &input, const std::string &output,
                                        const VideoEncodeOptions &options,
                                        ProgressCallback progress, CompletionCallback done) {
  wait();
  m_cancelFlag = false;
  m_running = true;
  m_thread = std::thread([=, this]() {
    VideoConverter conv;
    bool ok = conv.convert(input, output, options, progress, &this->m_cancelFlag);
    if (done)
      done(ok && !this->m_cancelFlag.load());
    this->m_running = false;
  });
}

void FormatConverter::cancel() { m_cancelFlag = true; }

bool FormatConverter::isCancelled() const { return m_cancelFlag.load(); }

void FormatConverter::wait() {
  if (m_thread.joinable())
    m_thread.join();
  m_cancelFlag = false;
  m_running = false;
}

bool FormatConverter::isRunning() const { return m_running.load(); }

} // namespace mediaplayer
