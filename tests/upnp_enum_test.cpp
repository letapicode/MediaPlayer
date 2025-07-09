#include "mediaplayer/upnp/DlnaClient.h"
#include <iostream>

int main() {
  mediaplayer::upnp::DlnaClient client;
  auto devices = client.discover();
  if (devices.empty()) {
    std::cerr << "No DLNA devices found\n";
    return 1;
  }
  std::cout << "Found " << devices.size() << " DLNA device(s)\n";
  return 0;
}
