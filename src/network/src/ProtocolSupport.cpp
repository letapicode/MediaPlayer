#include "mediaplayer/ProtocolSupport.h"

extern "C" {
#include <libavformat/avio.h>
}

namespace mediaplayer {

std::vector<std::string> inputProtocols() {
  std::vector<std::string> protos;
  void *opaque = nullptr;
  const char *name = nullptr;
  while ((name = avio_enum_protocols(&opaque, 0))) {
    protos.emplace_back(name);
  }
  return protos;
}

bool isInputProtocolSupported(const std::string &protocol) {
  void *opaque = nullptr;
  const char *name = nullptr;
  while ((name = avio_enum_protocols(&opaque, 0))) {
    if (protocol == name)
      return true;
  }
  return false;
}

} // namespace mediaplayer
