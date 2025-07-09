#include "mediaplayer/upnp/DlnaClient.h"
#include <iostream>

int main() {
  mediaplayer::upnp::DlnaClient client;
  auto devices = client.discover();
  for (const auto &dev : devices) {
    std::cout << dev.friendlyName << " (" << dev.location << ")\n";
  }
  return 0;
}
