#include "mediaplayer/YoutubeDL.h"
#include <array>
#include <cstdio>
#include <memory>
#include <string>

namespace mediaplayer {

std::string youtubeDlGetUrl(const std::string &url, const std::string &scriptPath) {
  std::string cmd = "python3 ";
  if (!scriptPath.empty())
    cmd += '"' + scriptPath + '"';
  else
    cmd += "-m youtube_dl -g";
  cmd += " \"" + url + "\" 2>/dev/null";

  std::array<char, 1024> buf;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe)
    return {};
  while (fgets(buf.data(), buf.size(), pipe.get())) {
    result += buf.data();
  }
  if (!result.empty() && result.back() == '\n')
    result.pop_back();
  return result;
}

} // namespace mediaplayer
