#include "mediaplayer/ProtocolSupport.h"

extern "C" {
#include <libavformat/avformat.h>
}

namespace mediaplayer {

bool protocolAvailable(const std::string &name) {
  void *opaque = nullptr;
  const char *protocol = nullptr;
  while ((protocol = avio_enum_protocols(&opaque, 0))) {
    if (name == protocol)
      return true;
  }
  return false;
}

} // namespace mediaplayer
