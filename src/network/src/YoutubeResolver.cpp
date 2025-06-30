#include "mediaplayer/YoutubeResolver.h"

#include <cstdio>
#include <iostream>
#include <memory>

namespace mediaplayer {

static std::string readPipe(FILE *pipe) {
  std::string result;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe)) {
    result += buffer;
  }
  return result;
}

std::string YoutubeResolver::resolve(const std::string &ytUrl) {
  std::string cmd = "python3 scripts/youtube_resolve.py \"" + ytUrl + "\"";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "Failed to run youtube resolver script\n";
    return {};
  }
  std::string output = readPipe(pipe);
  pclose(pipe);
  // trim newline characters
  while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) {
    output.pop_back();
  }
  return output;
}

} // namespace mediaplayer
