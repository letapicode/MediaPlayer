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
  for (unsigned i = 0; i < m_formatCtx->nb_streams; ++i) {
    if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      m_audioStream = i;
      break;
    }
  }
  if (m_audioStream >= 0) {
    if (!m_audioDecoder.open(m_formatCtx, m_audioStream)) {
      std::cerr << "Failed to open audio decoder\n";
      return false;
    }
  }
  std::cout << "Opened " << path << '\n';
  return true;
}

void MediaPlayer::play() { std::cout << "play not implemented\n"; }

void MediaPlayer::pause() { std::cout << "pause not implemented\n"; }

void MediaPlayer::stop() { std::cout << "stop not implemented\n"; }

double MediaPlayer::position() const { return 0.0; }

int MediaPlayer::readAudio(uint8_t *buffer, int bufferSize) {
  if (!m_formatCtx || m_audioStream < 0) {
    return 0;
  }
  AVPacket pkt;
  av_init_packet(&pkt);
  while (av_read_frame(m_formatCtx, &pkt) >= 0) {
    if (pkt.stream_index == m_audioStream) {
      int ret = m_audioDecoder.decode(&pkt, buffer, bufferSize);
      av_packet_unref(&pkt);
      return ret;
    }
    av_packet_unref(&pkt);
  }
  return 0;
}

} // namespace mediaplayer
