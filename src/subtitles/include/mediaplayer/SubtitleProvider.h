#ifndef MEDIAPLAYER_SUBTITLEPROVIDER_H
#define MEDIAPLAYER_SUBTITLEPROVIDER_H

#include "SrtParser.h"

namespace mediaplayer {

class SubtitleProvider {
public:
  // Load subtitles from file (currently only SRT supported)
  bool load(const std::string &path);

  // Adjust subtitle timing by offset (in seconds). Positive values delay.
  void setOffset(double offset) { m_offset = offset; }
  double offset() const { return m_offset; }

  // Retrieve subtitle cue for given playback time (seconds).
  const SubtitleCue *subtitleAt(double time) const;

private:
  SrtParser m_parser;
  double m_offset{0.0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SUBTITLEPROVIDER_H
