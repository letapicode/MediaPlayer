#ifndef MEDIAPLAYER_YOUTUBERESOLVER_H
#define MEDIAPLAYER_YOUTUBERESOLVER_H

#include <string>

namespace mediaplayer {

class YouTubeResolver {
public:
  // Resolve a YouTube link to a direct media stream URL using youtube-dl.
  std::string resolve(const std::string &url) const;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_YOUTUBERESOLVER_H
