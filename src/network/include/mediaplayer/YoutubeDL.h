#ifndef MEDIAPLAYER_YOUTUBEDL_H
#define MEDIAPLAYER_YOUTUBEDL_H

#include <string>

namespace mediaplayer {

std::string youtubeDlGetUrl(const std::string &url, const std::string &scriptPath = "");

} // namespace mediaplayer

#endif // MEDIAPLAYER_YOUTUBEDL_H
