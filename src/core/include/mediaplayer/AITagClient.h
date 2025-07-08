#ifndef MEDIAPLAYER_AITAGCLIENT_H
#define MEDIAPLAYER_AITAGCLIENT_H

#include <string>
#include <vector>

namespace mediaplayer {

class AITagClient {
public:
  explicit AITagClient(const std::string &serviceUrl = "http://localhost:8000");

  std::vector<std::string> tagAudio(const std::string &path);
  std::vector<std::string> tagVideo(const std::string &path);

private:
  std::string m_serviceUrl;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AITAGCLIENT_H
