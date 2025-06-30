#include "mediaplayer/NetworkUtils.h"
#include <cassert>
#include <iostream>

int main() {
  auto protocols = mediaplayer::listProtocols();
  assert(!protocols.empty());
  assert(mediaplayer::supportsProtocol("http"));
  std::cout << "Available protocols: " << protocols.size() << std::endl;
  if (mediaplayer::supportsDemuxer("hls"))
    std::cout << "HLS supported" << std::endl;
  return 0;
}
