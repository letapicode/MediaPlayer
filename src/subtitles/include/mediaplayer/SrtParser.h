#ifndef MEDIAPLAYER_SRTPARSER_H
#define MEDIAPLAYER_SRTPARSER_H

#include <string>
#include <vector>

namespace mediaplayer {

struct SubtitleCue {
  double start{0.0};
  double end{0.0};
  std::string text;
};

class SrtParser {
public:
  bool load(const std::string &path);
  const std::vector<SubtitleCue> &cues() const { return m_cues; }
  const SubtitleCue *cueAt(double time) const;

private:
  static double parseTimestamp(const std::string &ts);
  std::vector<SubtitleCue> m_cues;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SRTPARSER_H
