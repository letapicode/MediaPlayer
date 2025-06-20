#ifndef MEDIAPLAYER_PLAYBACKCALLBACKS_H
#define MEDIAPLAYER_PLAYBACKCALLBACKS_H

#include <functional>

namespace mediaplayer {

struct PlaybackCallbacks {
  std::function<void()> onPlay;
  std::function<void()> onPause;
  std::function<void()> onStop;
  std::function<void()> onFinished;
  std::function<void(double)> onPosition;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYBACKCALLBACKS_H
