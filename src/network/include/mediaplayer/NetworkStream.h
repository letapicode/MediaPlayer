#ifndef MEDIAPLAYER_NETWORKSTREAM_H
#define MEDIAPLAYER_NETWORKSTREAM_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>

namespace mediaplayer {

class NetworkStream {
public:
  NetworkStream();
  ~NetworkStream();

  bool open(const std::string &url);
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

  // Retrieve a metadata value from the opened stream.
  std::string metadata(const std::string &key) const;
  // Convenience helpers for common ICY tags used by internet radio streams.
  std::string icyTitle() const;
  std::string icyName() const;

private:
  AVFormatContext *m_ctx{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_NETWORKSTREAM_H
