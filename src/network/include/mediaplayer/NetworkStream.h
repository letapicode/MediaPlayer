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
  bool open(const std::string &url, AVDictionary *options);
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

private:
  AVFormatContext *m_ctx{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_NETWORKSTREAM_H
