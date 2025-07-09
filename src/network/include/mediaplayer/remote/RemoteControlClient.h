#ifndef MEDIAPLAYER_REMOTE_CONTROL_CLIENT_H
#define MEDIAPLAYER_REMOTE_CONTROL_CLIENT_H

#include "DeviceStatus.h"
#include <string>

namespace mediaplayer {
namespace remote {

class RemoteControlClient {
public:
  DeviceStatus getStatus(const std::string &host, uint16_t port);
  bool sendPlay(const std::string &host, uint16_t port, const std::string &path, double position);
};

} // namespace remote
} // namespace mediaplayer

#endif // MEDIAPLAYER_REMOTE_CONTROL_CLIENT_H
