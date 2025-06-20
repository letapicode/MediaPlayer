#include "mediaplayer/MediaPlayer.h"
#include <iostream>
#include <libavformat/avformat.h>

namespace mediaplayer {

MediaPlayer::MediaPlayer() {
  avformat_network_init();
  m_output = std::make_unique<NullAudioOutput>();
}

MediaPlayer::~MediaPlayer() {
  stop();
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
    if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStream < 0) {
      m_audioStream = i;
    } else if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
               m_videoStream < 0) {
      m_videoStream = i;
    }
  }
  if (m_audioStream >= 0) {
    if (!m_audioDecoder.open(m_formatCtx, m_audioStream)) {
      std::cerr << "Failed to open audio decoder\n";
      return false;
    }
    if (m_output && !m_output->init(m_audioDecoder.sampleRate(), m_audioDecoder.channels())) {
      std::cerr << "Failed to init audio output\n";
      return false;
    }
  }
  if (m_videoStream >= 0) {
    if (!m_videoDecoder.open(m_formatCtx, m_videoStream)) {
      std::cerr << "Failed to open video decoder\n";
      return false;
    }
  }
  std::cout << "Opened " << path << '\n';
  return true;
}

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

int MediaPlayer::readVideo(uint8_t *buffer, int bufferSize) {
  if (!m_formatCtx || m_videoStream < 0) {
    return 0;
  }
  AVPacket pkt;
  av_init_packet(&pkt);
  while (av_read_frame(m_formatCtx, &pkt) >= 0) {
    if (pkt.stream_index == m_videoStream) {
      int ret = m_videoDecoder.decode(&pkt, buffer, bufferSize);
      av_packet_unref(&pkt);
      return ret;
    }
    av_packet_unref(&pkt);
  }
  return 0;
}

void MediaPlayer::setAudioOutput(std::unique_ptr<AudioOutput> output) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_output) {
    m_output->shutdown();
  }
  m_output = std::move(output);
}

void MediaPlayer::play() {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_running) {
    m_paused = false;
    m_output->resume();
    m_cv.notify_all();
    return;
  }
  m_stopRequested = false;
  m_paused = false;
  m_running = true;
  m_output->resume();
  m_playThread = std::thread([this]() {
    uint8_t buffer[4096];
    while (true) {
      {
        std::unique_lock<std::mutex> tlock(m_mutex);
        m_cv.wait(tlock, [this]() { return !m_paused || m_stopRequested; });
        if (m_stopRequested)
          break;
      }
      int bytes = readAudio(buffer, sizeof(buffer));
      if (bytes <= 0)
        break;
      if (m_output)
        m_output->write(buffer, bytes);
    }
    if (m_output)
      m_output->shutdown();
    m_running = false;
  });
}

void MediaPlayer::pause() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_running && !m_paused) {
    m_paused = true;
    if (m_output)
      m_output->pause();
  }
}

void MediaPlayer::stop() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_running)
      return;
    m_stopRequested = true;
    m_paused = false;
    m_cv.notify_all();
  }
  if (m_playThread.joinable())
    m_playThread.join();
  if (m_output)
    m_output->shutdown();
}

void MediaPlayer::seek(double seconds) {
  if (!m_formatCtx)
    return;
  int64_t ts = static_cast<int64_t>(seconds * AV_TIME_BASE);
  av_seek_frame(m_formatCtx, -1, ts, AVSEEK_FLAG_BACKWARD);
  m_audioDecoder.flush();
  m_videoDecoder.flush();
}

} // namespace mediaplayer
