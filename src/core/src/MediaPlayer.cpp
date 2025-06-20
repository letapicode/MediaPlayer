#include "mediaplayer/MediaPlayer.h"
#include <chrono>
#include <iostream>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <vector>

namespace mediaplayer {

MediaPlayer::MediaPlayer() {
  avformat_network_init();
  m_output = std::make_unique<NullAudioOutput>();
  m_videoOutput = std::make_unique<NullVideoOutput>();
  m_eof = false;
}

MediaPlayer::~MediaPlayer() {
  stop();
  if (m_formatCtx) {
    avformat_close_input(&m_formatCtx);
  }
  if (m_videoOutput) {
    m_videoOutput->shutdown();
  }
  m_audioPackets.clear();
  m_videoPackets.clear();
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
    if (m_videoOutput && !m_videoOutput->init(m_videoDecoder.width(), m_videoDecoder.height())) {
      std::cerr << "Failed to init video output\n";
      return false;
    }
  }
  m_audioClock = 0.0;
  m_videoClock = 0.0;
  m_audioPackets.clear();
  m_videoPackets.clear();
  m_eof = false;
  std::cout << "Opened " << path << '\n';
  return true;
}

double MediaPlayer::position() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_audioStream >= 0)
    return m_audioClock;
  return m_videoClock;
}

int MediaPlayer::readAudio(uint8_t *buffer, int bufferSize) {
  AVPacket *pkt = nullptr;
  if (m_audioPackets.pop(pkt)) {
    int ret = m_audioDecoder.decode(pkt, buffer, bufferSize);
    av_packet_free(&pkt);
    return ret;
  }
  return 0;
}

int MediaPlayer::readVideo(uint8_t *buffer, int bufferSize) {
  AVPacket *pkt = nullptr;
  if (m_videoPackets.pop(pkt)) {
    int ret = m_videoDecoder.decode(pkt, buffer, bufferSize);
    av_packet_free(&pkt);
    return ret;
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

void MediaPlayer::setVideoOutput(std::unique_ptr<VideoOutput> output) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_videoOutput) {
    m_videoOutput->shutdown();
  }
  m_videoOutput = std::move(output);
}

void MediaPlayer::play() {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_running) {
    m_paused = false;
    if (m_output)
      m_output->resume();
    m_cv.notify_all();
    return;
  }
  m_stopRequested = false;
  m_paused = false;
  m_running = true;
  if (m_output)
    m_output->resume();
  m_startTime = av_gettime() / 1000000.0;
  m_demuxThread = std::thread(&MediaPlayer::demuxLoop, this);
  if (m_audioStream >= 0)
    m_audioThread = std::thread(&MediaPlayer::audioLoop, this);
  if (m_videoStream >= 0)
    m_videoThread = std::thread(&MediaPlayer::videoLoop, this);
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
  if (m_demuxThread.joinable())
    m_demuxThread.join();
  if (m_audioThread.joinable())
    m_audioThread.join();
  if (m_videoThread.joinable())
    m_videoThread.join();
  if (m_output)
    m_output->shutdown();
  if (m_videoOutput)
    m_videoOutput->shutdown();
  m_audioPackets.clear();
  m_videoPackets.clear();
  m_running = false;
}

void MediaPlayer::seek(double seconds) {
  if (!m_formatCtx)
    return;
  int64_t ts = static_cast<int64_t>(seconds * AV_TIME_BASE);
  av_seek_frame(m_formatCtx, -1, ts, AVSEEK_FLAG_BACKWARD);
  m_audioDecoder.flush();
  m_videoDecoder.flush();
  m_audioClock = seconds;
  m_videoClock = seconds;
}

void MediaPlayer::demuxLoop() {
  AVPacket pkt;
  while (true) {
    if (m_stopRequested)
      break;
    if (m_audioPackets.full() && m_videoPackets.full()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    if (av_read_frame(m_formatCtx, &pkt) < 0) {
      m_eof = true;
      break;
    }
    if (pkt.stream_index == m_audioStream && !m_audioPackets.full()) {
      m_audioPackets.push(&pkt);
    } else if (pkt.stream_index == m_videoStream && !m_videoPackets.full()) {
      m_videoPackets.push(&pkt);
    }
    av_packet_unref(&pkt);
  }
}

void MediaPlayer::audioLoop() {
  uint8_t audioBuffer[4096];
  while (true) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() { return (!m_paused && !m_stopRequested) || m_stopRequested; });
      if (m_stopRequested && m_audioPackets.size() == 0)
        break;
    }
    AVPacket *pkt = nullptr;
    if (m_audioPackets.pop(pkt)) {
      int bytes = m_audioDecoder.decode(pkt, audioBuffer, sizeof(audioBuffer));
      av_packet_free(&pkt);
      if (bytes > 0 && m_output) {
        m_audioClock = m_audioDecoder.lastPts();
        m_output->write(audioBuffer, bytes);
      }
    } else if (m_eof) {
      break;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void MediaPlayer::videoLoop() {
  const int videoBufferSize =
      av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_videoDecoder.width(), m_videoDecoder.height(), 1);
  std::vector<uint8_t> videoBuffer(videoBufferSize);
  while (true) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() { return (!m_paused && !m_stopRequested) || m_stopRequested; });
      if (m_stopRequested && m_videoPackets.size() == 0)
        break;
    }
    AVPacket *pkt = nullptr;
    if (m_videoPackets.pop(pkt)) {
      int bytes = m_videoDecoder.decode(pkt, videoBuffer.data(), videoBufferSize);
      av_packet_free(&pkt);
      if (bytes > 0 && m_videoOutput) {
        m_videoClock = m_videoDecoder.lastPts();
        double master =
            m_audioStream >= 0 ? m_audioClock : (av_gettime() / 1000000.0 - m_startTime);
        double delay = m_videoClock - master;
        if (delay > 0)
          std::this_thread::sleep_for(std::chrono::duration<double>(delay));
        m_videoOutput->displayFrame(videoBuffer.data(), m_videoDecoder.width() * 4);
      }
    } else if (m_eof) {
      break;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

} // namespace mediaplayer
