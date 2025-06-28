#ifndef MEDIAPLAYER_MEDIADEMUXER_H
#define MEDIAPLAYER_MEDIADEMUXER_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>

namespace mediaplayer {

class BufferedReader;

class MediaDemuxer {
public:
  MediaDemuxer();
  ~MediaDemuxer();

  bool open(const std::string &path);
  void close();
  bool readPacket(AVPacket &pkt);
  int audioStream() const { return m_audioStream; }
  int videoStream() const { return m_videoStream; }
  AVFormatContext *context() const { return m_ctx; }
  bool eof() const { return m_eof; }
  void setBufferSize(size_t size) { m_bufferSize = size; }
  size_t bufferSize() const { return m_bufferSize; }

private:
  static bool isUrl(const std::string &path);

  AVFormatContext *m_ctx{nullptr};
  int m_audioStream{-1};
  int m_videoStream{-1};
  bool m_eof{false};
  size_t m_bufferSize{0};
  std::unique_ptr<BufferedReader> m_bufferedReader;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIADEMUXER_H
