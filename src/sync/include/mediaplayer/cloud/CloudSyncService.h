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
  bool configured() const { return !m_serverUrl.empty(); }

  const std::string &lastPath() const { return m_lastPath; }
  double lastPosition() const { return m_lastPosition; }

  void pushStatus(const std::string &path, double position);
  bool pullStatus();

private:
  std::string m_serverUrl;
  std::string m_user;
  std::string m_token;
  std::string m_lastPath;
  double m_lastPosition{0.0};
};

} // namespace cloud
} // namespace mediaplayer

#endif // MEDIAPLAYER_CLOUD_SYNC_SERVICE_H
