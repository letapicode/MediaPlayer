#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

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

  double duration() const; // seconds
};

} // namespace mediaplayer

#endif // MEDIA_PLAYER_H
