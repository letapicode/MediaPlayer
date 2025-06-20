#ifndef MEDIAPLAYER_MEDIAMETADATA_H
#define MEDIAPLAYER_MEDIAMETADATA_H

#include <string>

namespace mediaplayer {

struct MediaMetadata {
  std::string path;     // Original path to media
  std::string title;    // Title (from tags or file name)
  std::string artist;   // Artist if available
  std::string album;    // Album if available
  double duration{0.0}; // In seconds
  int width{0};         // Video width, 0 for audio-only
  int height{0};        // Video height
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAMETADATA_H
