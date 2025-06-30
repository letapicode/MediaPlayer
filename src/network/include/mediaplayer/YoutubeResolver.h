#ifndef MEDIAPLAYER_YOUTUBERESOLVER_H
#define MEDIAPLAYER_YOUTUBERESOLVER_H

#include <string>

namespace mediaplayer {

class YoutubeResolver {
public:
  // Resolve a YouTube URL to a direct stream URL using the helper script.
  static std::string resolve(const std::string &ytUrl);
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_YOUTUBERESOLVER_H
