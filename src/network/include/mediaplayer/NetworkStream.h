#ifndef MEDIAPLAYER_NETWORKSTREAM_H
#define MEDIAPLAYER_NETWORKSTREAM_H

extern "C" {
#include <libavformat/avformat.h>
}

#include <map>
#include <string>

namespace mediaplayer {

class NetworkStream {
public:
  NetworkStream();
  ~NetworkStream();

  bool open(const std::string &url);
  void setOption(const std::string &key, const std::string &value);
  std::string getMetadata(const std::string &key) const;
  std::map<std::string, std::string> metadata() const;
  AVFormatContext *context() const { return m_ctx; }
  AVFormatContext *release();

private:
  AVFormatContext *m_ctx{nullptr};
  AVDictionary *m_opts{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_NETWORKSTREAM_H
