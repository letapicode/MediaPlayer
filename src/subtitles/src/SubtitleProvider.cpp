#include "mediaplayer/SubtitleProvider.h"

namespace mediaplayer {

bool SubtitleProvider::load(const std::string &path) { return m_parser.load(path); }

const SubtitleCue *SubtitleProvider::subtitleAt(double time) const {
  return m_parser.cueAt(time - m_offset);
}

} // namespace mediaplayer
