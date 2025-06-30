#include "mediaplayer/YouTubeResolver.h"

#include <array>
#include <cstdio>
#include <string>

#ifndef YOUTUBE_RESOLVER_SCRIPT
#define YOUTUBE_RESOLVER_SCRIPT "youtube_resolver.py"
#endif

namespace mediaplayer {

static std::string runCommand(const std::string &cmd) {
  std::array<char, 256> buf{};
  std::string result;
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    return result;
  while (fgets(buf.data(), static_cast<int>(buf.size()), pipe)) {
    result += buf.data();
  }
  pclose(pipe);
  if (!result.empty() && result.back() == '\n')
    result.pop_back();
  return result;
}

std::string YouTubeResolver::resolve(const std::string &url) const {
  std::string cmd = "python3 \"" YOUTUBE_RESOLVER_SCRIPT "\" \"" + url + "\"";
  return runCommand(cmd);
}

} // namespace mediaplayer
