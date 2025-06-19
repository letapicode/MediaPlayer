#include "mediaplayer/MediaPlayer.h"
#include <iostream>
#include <libavformat/avformat.h>

namespace mediaplayer {

MediaPlayer::MediaPlayer() { avformat_network_init(); }

MediaPlayer::~MediaPlayer() {
  if (m_formatCtx) {
    avformat_close_input(&m_formatCtx);
  }
  avformat_network_deinit();
}

bool MediaPlayer::open(const std::string &path) {
  if (avformat_open_input(&m_formatCtx, path.c_str(), nullptr, nullptr) < 0) {
    std::cerr << "Failed to open media: " << path << '\n';
    return false;
  }
  if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
    std::cerr << "Failed to retrieve stream info\n";
    avformat_close_input(&m_formatCtx);
    return false;
  }
  std::cout << "Opened " << path << '\n';
  return true;
}

void MediaPlayer::play() { std::cout << "play not implemented\n"; }

void MediaPlayer::pause() { std::cout << "pause not implemented\n"; }

void MediaPlayer::stop() { std::cout << "stop not implemented\n"; }

double MediaPlayer::position() const { return 0.0; }

} // namespace mediaplayer
