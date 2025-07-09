#ifndef MEDIAPLAYER_CLOUD_SYNC_SERVICE_H
#define MEDIAPLAYER_CLOUD_SYNC_SERVICE_H

#include <string>

namespace mediaplayer {
namespace cloud {

class CloudSyncService {
public:
  void setServerUrl(const std::string &url) { m_serverUrl = url; }
  void setCredentials(const std::string &user, const std::string &token) {
    m_user = user;
    m_token = token;
  }

  void pushStatus(const std::string &path, double position);
  void pullStatus();

private:
  std::string m_serverUrl;
  std::string m_user;
  std::string m_token;
};

} // namespace cloud
} // namespace mediaplayer

#endif // MEDIAPLAYER_CLOUD_SYNC_SERVICE_H
