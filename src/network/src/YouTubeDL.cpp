#include "mediaplayer/YouTubeDL.h"
#include <array>
#include <cstdio>
#include <iostream>

namespace mediaplayer {

std::optional<std::string> fetchYouTubeStream(const std::string &url) {
  std::string cmd = "python3 -m youtube_dl -g \"" + url + "\" 2>/dev/null";
  std::array<char, 512> buffer{};
  std::string output;
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "youtube_dl not available\n";
    return std::nullopt;
  }
  while (fgets(buffer.data(), buffer.size(), pipe)) {
    output += buffer.data();
  }
  int ret = pclose(pipe);
  if (ret != 0 || output.empty())
    return std::nullopt;
  if (!output.empty() && output.back() == '\n')
    output.pop_back();
  return output;
}

} // namespace mediaplayer
