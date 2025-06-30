#include "mediaplayer/YouTubeDl.h"

#include <array>
#include <cstdio>
#include <memory>

namespace mediaplayer {

std::string youtubeDlUrl(const std::string &url) {
  std::string cmd = "python3 -m youtube_dl -g --no-playlist \"" + url + "\" 2>/dev/null";
  std::array<char, 1024> buffer{};
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe)
    return {};
  if (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()))
    result = buffer.data();
  if (!result.empty() && result.back() == '\n')
    result.pop_back();
  return result;
}

} // namespace mediaplayer
