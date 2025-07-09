#include "mediaplayer/cloud/CloudSyncService.h"
#include <iostream>

namespace mediaplayer {
namespace cloud {

void CloudSyncService::pushStatus(const std::string &path, double position) {
  std::cout << "[CloudSync] push " << path << " @" << position << " to " << m_serverUrl
            << std::endl;
  // TODO: implement HTTP request
}

void CloudSyncService::pullStatus() {
  std::cout << "[CloudSync] pull from " << m_serverUrl << std::endl;
  // TODO: implement HTTP request
}

} // namespace cloud
} // namespace mediaplayer
