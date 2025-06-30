#ifndef MEDIAPLAYER_INTERNETRADIOSTREAM_H
#define MEDIAPLAYER_INTERNETRADIOSTREAM_H

#include <functional>
#include <string>
#include <thread>

#include <curl/curl.h>

namespace mediaplayer {

class InternetRadioStream {
public:
  InternetRadioStream();
  ~InternetRadioStream();

  bool open(const std::string &url);
  void close();

  using MetadataCallback = std::function<void(const std::string &)>;
  void setMetadataCallback(MetadataCallback cb);

private:
  static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata);
  static size_t headerCallback(char *buffer, size_t size, size_t nitems, void *userdata);
  void readLoop();
  void parseMetadata(const std::string &data);

private:
  CURL *m_curl{nullptr};
  std::thread m_thread;
  bool m_stop{false};
  long m_metaInt{0};
  long m_bytesUntilMeta{0};
  std::string m_partialMetadata;
  MetadataCallback m_callback;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_INTERNETRADIOSTREAM_H
