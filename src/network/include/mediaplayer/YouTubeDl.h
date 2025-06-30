#ifndef MEDIAPLAYER_YOUTUBEDL_H
#define MEDIAPLAYER_YOUTUBEDL_H

#include <string>

namespace mediaplayer {

// Return direct media stream URL by invoking youtube-dl.
// Returns empty string on failure.
std::string youtubeDlUrl(const std::string &url);

} // namespace mediaplayer

#endif // MEDIAPLAYER_YOUTUBEDL_H
