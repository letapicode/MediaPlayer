#ifndef MEDIAPLAYER_DLNA_CLIENT_H
#define MEDIAPLAYER_DLNA_CLIENT_H

#include <chrono>
#include <string>
#include <vector>

namespace mediaplayer {
namespace upnp {

struct DlnaDevice {
  std::string friendlyName;
  std::string location;
  std::string controlUrl;
};

class DlnaClient {
public:
  std::vector<DlnaDevice>
  discover(std::chrono::milliseconds timeout = std::chrono::milliseconds(3000));
  std::vector<std::string> listMedia(const DlnaDevice &device, const std::string &objectId = "0");
};

} // namespace upnp
} // namespace mediaplayer

#endif // MEDIAPLAYER_DLNA_CLIENT_H
