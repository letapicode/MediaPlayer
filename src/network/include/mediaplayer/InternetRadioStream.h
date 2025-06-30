#ifndef MEDIAPLAYER_INTERNETRADIOSTREAM_H
#define MEDIAPLAYER_INTERNETRADIOSTREAM_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <functional>
#include <string>

namespace mediaplayer {

class InternetRadioStream {
public:
  using MetadataCallback = std::function<void(const std::string &title)>;

  InternetRadioStream();
  ~InternetRadioStream();

  bool open(const std::string &url);
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

  void setMetadataCallback(MetadataCallback cb);
  void updateMetadata();

private:
  AVFormatContext *m_ctx{nullptr};
  bool m_ownsContext{true};
  MetadataCallback m_metadataCallback{};
  std::string m_lastTitle;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_INTERNETRADIOSTREAM_H
