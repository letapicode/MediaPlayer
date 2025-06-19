#ifndef MEDIAPLAYER_MEDIAPLAYER_H
#define MEDIAPLAYER_MEDIAPLAYER_H

#include <string>

namespace mediaplayer {

class MediaPlayer {
public:
  MediaPlayer();
  ~MediaPlayer();

  bool open(const std::string &path);
  void play();
  void pause();
  void stop();
  double position() const; // seconds
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYER_H
