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

  bool open(const std::string &mpdUrl);
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

  const std::string &videoRepresentation() const { return m_videoRep; }
  const std::string &audioRepresentation() const { return m_audioRep; }

private:
  bool parseManifest(const std::string &manifest);
  bool downloadFile(const std::string &url, std::string &data);

  AVFormatContext *m_ctx{nullptr};
  std::string m_videoRep;
  std::string m_audioRep;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_DASHSTREAM_H
