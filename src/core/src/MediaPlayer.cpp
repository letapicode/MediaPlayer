#include "mediaplayer/MediaPlayer.h"
#include <iostream>

namespace mediaplayer {

MediaPlayer::MediaPlayer() = default;
MediaPlayer::~MediaPlayer() = default;

bool MediaPlayer::open(const std::string & /*path*/) {
  // TODO: implement opening media via FFmpeg
  std::cout << "open not implemented\n";
  return false;
}

void MediaPlayer::play() { std::cout << "play not implemented\n"; }

void MediaPlayer::pause() { std::cout << "pause not implemented\n"; }

void MediaPlayer::stop() { std::cout << "stop not implemented\n"; }

double MediaPlayer::position() const { return 0.0; }

} // namespace mediaplayer
