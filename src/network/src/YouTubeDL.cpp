#include "mediaplayer/YouTubeDL.h"
#include <cstdio>
#include <iostream>

namespace mediaplayer {

bool YouTubeDL::getStreamUrl(const std::string &videoUrl, std::string &outUrl) {
  std::string cmd = "youtube-dl -g '" + videoUrl + "' 2>/dev/null";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "Failed to run youtube-dl\n";
    return false;
  }
  char buffer[4096];
  if (fgets(buffer, sizeof(buffer), pipe)) {
    outUrl = std::string(buffer);
    // remove trailing newline
    if (!outUrl.empty() && outUrl.back() == '\n')
      outUrl.pop_back();
  }
  int ret = pclose(pipe);
  return ret == 0 && !outUrl.empty();
}

} // namespace mediaplayer
