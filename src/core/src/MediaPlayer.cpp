#include "mediaplayer/MediaPlayer.h"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
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
  if (m_formatCtx) {
    avformat_close_input(&m_formatCtx);
    m_audioStream = -1;
    m_videoStream = -1;
  }
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
  // Extract metadata
  m_metadata = {};
  m_metadata.path = path;
  if (m_formatCtx->duration != AV_NOPTS_VALUE)
    m_metadata.duration = m_formatCtx->duration / (double)AV_TIME_BASE;
  if (m_videoStream >= 0) {
    m_metadata.width = m_videoDecoder.width();
    m_metadata.height = m_videoDecoder.height();
  }
  AVDictionaryEntry *tag = av_dict_get(m_formatCtx->metadata, "title", nullptr, 0);
  if (tag && tag->value)
    m_metadata.title = tag->value;
  tag = av_dict_get(m_formatCtx->metadata, "artist", nullptr, 0);
  if (tag && tag->value)
    m_metadata.artist = tag->value;
  tag = av_dict_get(m_formatCtx->metadata, "album", nullptr, 0);
  if (tag && tag->value)
    m_metadata.album = tag->value;
  TagLib::FileRef f(path.c_str());
  if (!f.isNull() && f.tag()) {
    if (m_metadata.title.empty())
      m_metadata.title = f.tag()->title().to8Bit(true);
    if (m_metadata.artist.empty())
      m_metadata.artist = f.tag()->artist().to8Bit(true);
    if (m_metadata.album.empty())
      m_metadata.album = f.tag()->album().to8Bit(true);
    if (f.audioProperties() && m_metadata.duration == 0.0)
      m_metadata.duration = f.audioProperties()->length();
  }
  if (m_metadata.title.empty())
    m_metadata.title = std::filesystem::path(path).filename().string();
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

void MediaPlayer::setCallbacks(PlaybackCallbacks callbacks) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_callbacks = std::move(callbacks);
}

void MediaPlayer::setVolume(double volume) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (volume < 0.0)
    volume = 0.0;
  if (volume > 1.0)
    volume = 1.0;
  m_volume = volume;
}

double MediaPlayer::volume() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_volume;
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
  lock.unlock();
  if (m_callbacks.onPlay)
    m_callbacks.onPlay();
}

void MediaPlayer::pause() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_running && !m_paused) {
    m_paused = true;
    if (m_output)
      m_output->pause();
    if (m_callbacks.onPause)
      m_callbacks.onPause();
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
  if (m_callbacks.onStop)
    m_callbacks.onStop();
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

void MediaPlayer::setPlaylist(const std::vector<std::string> &paths) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_playlist.set(paths);
}

void MediaPlayer::addToPlaylist(const std::string &path) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_playlist.add(path);
}

void MediaPlayer::clearPlaylist() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_playlist.clear();
}

bool MediaPlayer::nextTrack() {
  std::string path;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_playlist.empty())
      return false;
    path = m_playlist.next();
  }
  stop();
  if (!open(path))
    return false;
  play();
  return true;
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
      if (m_callbacks.onFinished)
        m_callbacks.onFinished();
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
        double vol;
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          vol = m_volume;
        }
        if (vol < 0.999) {
          int16_t *samples = reinterpret_cast<int16_t *>(audioBuffer);
          int sampleCount = bytes / sizeof(int16_t);
          for (int i = 0; i < sampleCount; ++i) {
            int32_t s = static_cast<int32_t>(samples[i] * vol);
            if (s < -32768)
              s = -32768;
            else if (s > 32767)
              s = 32767;
            samples[i] = static_cast<int16_t>(s);
          }
        }
        m_output->write(audioBuffer, bytes);
        if (m_callbacks.onPosition)
          m_callbacks.onPosition(m_audioClock);
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
        if (m_callbacks.onPosition)
          m_callbacks.onPosition(m_videoClock);
      }
    } else if (m_eof) {
      break;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

} // namespace mediaplayer
