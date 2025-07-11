#ifndef MEDIAPLAYER_HLSSTREAM_H
#define MEDIAPLAYER_HLSSTREAM_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>

namespace mediaplayer {

class HlsStream {
public:
  HlsStream();
  ~HlsStream();

  bool open(const std::string &playlistUrl);
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

private:
  AVFormatContext *m_ctx{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_HLSSTREAM_H
