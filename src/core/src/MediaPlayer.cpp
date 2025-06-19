#include "MediaPlayer.h"
#include <iostream>

namespace mediaplayer {

MediaPlayer::MediaPlayer() = default;
MediaPlayer::~MediaPlayer() = default;

bool MediaPlayer::open(const std::string &path) {
  std::cout << "Opening media: " << path << std::endl;
  return true;
}

void MediaPlayer::play() { std::cout << "Play" << std::endl; }

void MediaPlayer::pause() { std::cout << "Pause" << std::endl; }

void MediaPlayer::stop() { std::cout << "Stop" << std::endl; }

double MediaPlayer::duration() const { return 0.0; }

} // namespace mediaplayer
