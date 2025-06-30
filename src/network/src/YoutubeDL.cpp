#include "mediaplayer/YoutubeDL.h"
#include <array>
#include <cstdio>
#include <memory>
#include <string>

namespace mediaplayer {

static std::string exec(const std::string &cmd) {
  std::array<char, 256> buf{};
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe)
    return result;
  while (fgets(buf.data(), buf.size(), pipe.get()))
    result += buf.data();
  return result;
}

std::string resolveYouTubeUrl(const std::string &url) {
  std::string cmd = "python3 -m youtube_dl -g '" + url + "' 2>/dev/null";
  std::string out = exec(cmd);
  size_t pos = out.find_last_not_of(" \n\r\t");
  if (pos != std::string::npos)
    out = out.substr(0, pos + 1);
  return out;
}

} // namespace mediaplayer
