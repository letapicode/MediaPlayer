#include "mediaplayer/NetworkUtils.h"
extern "C" {
#include <libavformat/avformat.h>
}
#include <string>
#include <vector>

namespace mediaplayer {

std::vector<std::string> listProtocols(bool output) {
  void *opaque = nullptr;
  const char *name = nullptr;
  std::vector<std::string> result;
  (void)output;
  while ((name = avio_enum_protocols(&opaque, 0))) {
    result.emplace_back(name);
  }
  opaque = nullptr;
  while ((name = avio_enum_protocols(&opaque, 1))) {
    if (std::find(result.begin(), result.end(), name) == result.end())
      result.emplace_back(name);
  }
  return result;
}

bool supportsProtocol(const std::string &protocol) {
  void *opaque = nullptr;
  const char *name = nullptr;
  while ((name = avio_enum_protocols(&opaque, 0))) {
    if (protocol == name)
      return true;
  }
  opaque = nullptr;
  while ((name = avio_enum_protocols(&opaque, 1))) {
    if (protocol == name)
      return true;
  }
  return false;
}

bool supportsDemuxer(const std::string &name) {
  return av_find_input_format(name.c_str()) != nullptr;
}

} // namespace mediaplayer
