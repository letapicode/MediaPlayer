#ifndef MEDIAPLAYER_PLAYLIST_H
#define MEDIAPLAYER_PLAYLIST_H

#include "mediaplayer/MediaMetadata.h"
#include <string>
#include <vector>

namespace mediaplayer {

struct Playlist {
  std::string name;
  std::vector<MediaMetadata> items;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYLIST_H
