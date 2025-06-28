#ifndef MEDIAPLAYER_MEDIADEMUXER_H
#define MEDIAPLAYER_MEDIADEMUXER_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <string>

namespace mediaplayer {

class MediaDemuxer {
public:
  MediaDemuxer();
  ~MediaDemuxer();

  bool open(const std::string &path);
  void close();
  bool readPacket(AVPacket &pkt);
  int audioStream() const { return m_audioStream; }
  int videoStream() const { return m_videoStream; }
  int subtitleStream() const { return m_subtitleStream; }
  AVFormatContext *context() const { return m_ctx; }
  bool eof() const { return m_eof; }

private:
  static bool isUrl(const std::string &path);

  AVFormatContext *m_ctx{nullptr};
  int m_audioStream{-1};
  int m_videoStream{-1};
  int m_subtitleStream{-1};
  bool m_eof{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIADEMUXER_H
