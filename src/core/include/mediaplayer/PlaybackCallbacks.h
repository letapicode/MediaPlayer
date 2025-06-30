#ifndef MEDIAPLAYER_PLAYBACKCALLBACKS_H
#define MEDIAPLAYER_PLAYBACKCALLBACKS_H

#include "MediaMetadata.h"
#include <functional>

namespace mediaplayer {

struct PlaybackCallbacks {
  std::function<void()> onPlay;
  std::function<void()> onPause;
  std::function<void()> onStop;
  std::function<void()> onFinished;
  std::function<void(double)> onPosition;
  // Called whenever a new track is loaded. Provides the metadata of the track.
  std::function<void(const MediaMetadata &)> onTrackChanged;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYBACKCALLBACKS_H
