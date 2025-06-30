#ifndef MEDIAPLAYER_YOUTUBEDL_H
#define MEDIAPLAYER_YOUTUBEDL_H

#include <string>

namespace mediaplayer {

// Utility to resolve YouTube URLs to direct stream URLs using youtube-dl.
class YouTubeDL {
public:
  // Returns true and sets outUrl to a direct stream URL on success.
  static bool getStreamUrl(const std::string &videoUrl, std::string &outUrl);
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_YOUTUBEDL_H
