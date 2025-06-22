#include "mediaplayer/SrtParser.h"

#include <fstream>
#include <sstream>

namespace mediaplayer {

static bool parseLine(std::istream &is, std::string &line) {
  std::getline(is, line);
  if (!line.empty() && line.back() == '\r')
    line.pop_back();
  return !is.fail();
}

double SrtParser::parseTimestamp(const std::string &ts) {
  int h = 0, m = 0, s = 0, ms = 0;
  char sep1, sep2, comma;
  std::stringstream ss(ts);
  ss >> h >> sep1 >> m >> sep2 >> s >> comma >> ms;
  return h * 3600.0 + m * 60.0 + s + ms / 1000.0;
}

bool SrtParser::load(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open())
    return false;
  m_cues.clear();
  std::string line;
  while (parseLine(file, line)) {
    if (line.empty())
      continue; // skip blank lines
    // skip index number
    if (!parseLine(file, line))
      break;
    auto arrow = line.find("-->");
    if (arrow == std::string::npos)
      break;
    std::string startStr = line.substr(0, arrow - 1);
    std::string endStr = line.substr(arrow + 3);
    SubtitleCue cue{};
    cue.start = parseTimestamp(startStr);
    cue.end = parseTimestamp(endStr);
    cue.text.clear();
    while (parseLine(file, line) && !line.empty()) {
      if (!cue.text.empty())
        cue.text += "\n";
      cue.text += line;
    }
    m_cues.push_back(std::move(cue));
  }
  return !m_cues.empty();
}

const SubtitleCue *SrtParser::cueAt(double time) const {
  for (const auto &cue : m_cues) {
    if (time >= cue.start && time <= cue.end)
      return &cue;
  }
  return nullptr;
}

} // namespace mediaplayer
