#ifndef MEDIAPLAYER_BUFFEREDREADER_H
#define MEDIAPLAYER_BUFFEREDREADER_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>
#include <vector>

namespace mediaplayer {

class BufferedReader {
public:
  BufferedReader();
  ~BufferedReader();

  bool open(const std::string &url, size_t bufferSize);
  void close();
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

private:
  static int readPacket(void *opaque, uint8_t *buf, int buf_size);

  AVIOContext *m_inner{nullptr};
  AVIOContext *m_avio{nullptr};
  AVFormatContext *m_ctx{nullptr};
  std::vector<uint8_t> m_ring;
  size_t m_readPos{0};
  size_t m_dataSize{0};
  bool m_eof{false};
  std::vector<uint8_t> m_avioBuffer;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_BUFFEREDREADER_H
