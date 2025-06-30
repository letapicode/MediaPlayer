#ifndef MEDIAPLAYER_DASHSTREAM_H
#define MEDIAPLAYER_DASHSTREAM_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>

namespace mediaplayer {

class DashStream {
public:
  DashStream();
  ~DashStream();

  bool open(const std::string &url, int variantIndex = -1, int bandwidth = 0);
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

private:
  AVFormatContext *m_ctx{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_DASHSTREAM_H
