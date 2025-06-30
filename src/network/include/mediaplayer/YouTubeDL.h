#ifndef MEDIAPLAYER_YOUTUBEDL_H
#define MEDIAPLAYER_YOUTUBEDL_H

#include <optional>
#include <string>

namespace mediaplayer {

std::optional<std::string> fetchYouTubeStream(const std::string &url);

} // namespace mediaplayer

#endif // MEDIAPLAYER_YOUTUBEDL_H
