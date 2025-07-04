#include "mediaplayer/MediaPlayer.h"
#ifdef _WIN32
#include "mediaplayer/AudioOutputWASAPI.h"
#elif defined(__APPLE__)
#include "mediaplayer/AudioOutputCoreAudio.h"
#elif defined(__linux__)
#include "mediaplayer/AudioOutputPulse.h"
#endif
#include "mediaplayer/ProjectMVisualizer.h"
#include "mediaplayer/VideoFrame.h"
#include "mediaplayer/Visualizer.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <memory>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <vector>

namespace mediaplayer {

MediaPlayer::MediaPlayer() {
  avformat_network_init();
#ifdef _WIN32
  m_output = std::make_unique<AudioOutputWASAPI>();
#elif defined(__APPLE__)
  m_output = std::make_unique<AudioOutputCoreAudio>();
#elif defined(__linux__)
  m_output = std::make_unique<AudioOutputPulse>();
#else
  m_output = std::make_unique<NullAudioOutput>();
#endif
#ifdef _WIN32
  m_videoOutput = std::make_unique<OpenGLVideoOutput>();
#elif defined(__APPLE__)
  m_videoOutput = std::make_unique<OpenGLVideoOutput>();
#elif defined(ANDROID)
  m_videoOutput = std::make_unique<NullVideoOutput>();
#else
  m_videoOutput = std::make_unique<OpenGLVideoOutput>();
#endif
#ifdef MEDIAPLAYER_HW_DECODING
#ifdef _WIN32
  m_hwDevice = "dxva2";
#elif defined(__APPLE__)
  m_hwDevice = "videotoolbox";
#elif defined(__linux__)
  m_hwDevice = "vaapi";
#endif
#endif
}

MediaPlayer::~MediaPlayer() {
  stop();
  if (m_videoOutput) {
    m_videoOutput->shutdown();
  }
  m_audioPackets.clear();
  m_videoPackets.clear();
  m_subtitlePackets.clear();
  m_frameQueue.clear();
  m_framePool.clear();
  avformat_network_deinit();
}

static bool isUrl(const std::string &path) {
  return path.rfind("http://", 0) == 0 || path.rfind("https://", 0) == 0;
}

bool MediaPlayer::open(const std::string &path) {
  m_demuxer.setBufferSize(m_networkBufferSize);
  if (!m_demuxer.open(path)) {
    return false;
  }
  AVFormatContext *fmtCtx = m_demuxer.context();
  if (m_demuxer.audioStream() >= 0) {
    if (!m_audioDecoder.open(fmtCtx, m_demuxer.audioStream())) {
      std::cerr << "Failed to open audio decoder\n";
      m_demuxer.close();
      m_audioDecoder = AudioDecoder();
      return false;
    }
    if (m_output && !m_output->init(m_audioDecoder.sampleRate(), m_audioDecoder.channels())) {
      std::cerr << "Failed to init audio output\n";
      m_demuxer.close();
      m_audioDecoder = AudioDecoder();
      return false;
    }
  }
  if (m_demuxer.videoStream() >= 0) {
#ifdef MEDIAPLAYER_HW_DECODING
    if (!m_videoDecoder.open(fmtCtx, m_demuxer.videoStream(), m_hwDevice)) {
#else
    if (!m_videoDecoder.open(fmtCtx, m_demuxer.videoStream())) {
#endif
      std::cerr << "Failed to open video decoder\n";
      m_demuxer.close();
      m_audioDecoder = AudioDecoder();
      m_videoDecoder = VideoDecoder();
      return false;
    }
    if (m_videoOutput && !m_videoOutput->init(m_videoDecoder.width(), m_videoDecoder.height())) {
      std::cerr << "Failed to init video output\n";
      m_demuxer.close();
      m_audioDecoder = AudioDecoder();
      m_videoDecoder = VideoDecoder();
      return false;
    }
    int lines[3] = {m_videoDecoder.width(), m_videoDecoder.width() / 2, m_videoDecoder.width() / 2};
    m_framePool.preallocate(m_videoDecoder.width(), m_videoDecoder.height(), lines, 2);
  }
  if (m_demuxer.subtitleStream() >= 0) {
    m_subtitleDecoder.open(fmtCtx, m_demuxer.subtitleStream());
  }
  // Extract metadata
  m_metadata = {};
  m_metadata.path = path;
  if (fmtCtx->duration != AV_NOPTS_VALUE)
    m_metadata.duration = fmtCtx->duration / (double)AV_TIME_BASE;
  if (m_demuxer.videoStream() >= 0) {
    m_metadata.width = m_videoDecoder.width();
    m_metadata.height = m_videoDecoder.height();
  }
  AVDictionaryEntry *tag = av_dict_get(fmtCtx->metadata, "title", nullptr, 0);
  if (tag && tag->value)
    m_metadata.title = tag->value;
  tag = av_dict_get(fmtCtx->metadata, "artist", nullptr, 0);
  if (tag && tag->value)
    m_metadata.artist = tag->value;
  tag = av_dict_get(fmtCtx->metadata, "album", nullptr, 0);
  if (tag && tag->value)
    m_metadata.album = tag->value;
  if (!isUrl(path)) {
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
  }
  if (m_metadata.title.empty())
    m_metadata.title = std::filesystem::path(path).filename().string();
  if (m_callbacks.onTrackLoaded)
    m_callbacks.onTrackLoaded(m_metadata);
  m_audioClock = 0.0;
  m_videoClock = 0.0;
  m_subtitlePackets.clear();
  m_audioPackets.clear();
  m_videoPackets.clear();
  m_playRecorded = false;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_library && !isUrl(path) && !m_library->mediaExists(path)) {
      std::thread t = m_library->scanFileAsync(path);
      t.detach();
    }
  }
  std::cout << "Opened " << path << '\n';
  return true;
}

double MediaPlayer::position() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_demuxer.audioStream() >= 0)
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

void MediaPlayer::convertAudioFile(const std::string &input, const std::string &output,
                                   const AudioEncodeOptions &options,
                                   FormatConverter::ProgressCallback progress,
                                   FormatConverter::CompletionCallback done) {
  m_converter.convertAudioAsync(input, output, options, std::move(progress), std::move(done));
}

void MediaPlayer::convertVideoFile(const std::string &input, const std::string &output,
                                   const VideoEncodeOptions &options,
                                   FormatConverter::ProgressCallback progress,
                                   FormatConverter::CompletionCallback done) {
  m_converter.convertVideoAsync(input, output, options, std::move(progress), std::move(done));
}

void MediaPlayer::waitForConversion() { m_converter.wait(); }

bool MediaPlayer::conversionRunning() const { return m_converter.isRunning(); }

void MediaPlayer::cancelConversion() { m_converter.cancel(); }

std::vector<MediaMetadata> MediaPlayer::allMedia() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_library)
    return m_library->allMedia();
  return {};
}

std::vector<std::string> MediaPlayer::allPlaylists() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_library)
    return m_library->allPlaylists();
  return {};
}

std::vector<MediaMetadata> MediaPlayer::playlistItems(const std::string &name) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_library)
    return m_library->playlistItems(name);
  return {};
}

void MediaPlayer::setAudioOutput(std::unique_ptr<AudioOutput> output) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_output) {
    m_output->shutdown();
  }
  m_output = std::move(output);
  if (m_output)
    m_output->setVolume(m_volume);
}

void MediaPlayer::setVideoOutput(std::unique_ptr<VideoOutput> output) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_videoOutput) {
    m_videoOutput->shutdown();
  }
  m_videoOutput = std::move(output);
}

void MediaPlayer::setPreferredHardwareDevice(const std::string &device) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_hwDevice = device;
}

void MediaPlayer::setLibrary(LibraryDB *db) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_library = db;
}

void MediaPlayer::setVisualizer(std::shared_ptr<Visualizer> vis) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_visualizer = std::move(vis);
}

void MediaPlayer::enableVisualization(bool enable) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_visualizationEnabled = enable;
}

bool MediaPlayer::visualizationEnabled() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_visualizationEnabled;
}

void MediaPlayer::cycleVisualizationPreset() {
  std::shared_ptr<Visualizer> vis;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    vis = m_visualizer;
  }
  auto pm = std::dynamic_pointer_cast<ProjectMVisualizer>(vis);
  if (pm)
    pm->nextPreset();
}

void MediaPlayer::addAudioEffect(std::shared_ptr<AudioEffect> effect) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (effect)
    m_audioEffects.push_back(std::move(effect));
}

void MediaPlayer::removeAudioEffect(std::shared_ptr<AudioEffect> effect) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = std::remove(m_audioEffects.begin(), m_audioEffects.end(), effect);
  m_audioEffects.erase(it, m_audioEffects.end());
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
  if (m_output)
    m_output->setVolume(volume);
}

double MediaPlayer::volume() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_volume;
}

void MediaPlayer::setNetworkBufferSize(size_t size) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_networkBufferSize = size;
}

size_t MediaPlayer::networkBufferSize() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_networkBufferSize;
}

void MediaPlayer::setAutoAdvance(bool enable) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_autoAdvance = enable;
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
  m_stopRequested.store(false);
  m_paused = false;
  m_running = true;
  if (m_output)
    m_output->resume();
  m_startTime = av_gettime() / 1000000.0;
  m_demuxThread = std::thread(&MediaPlayer::demuxLoop, this);
  if (m_demuxer.audioStream() >= 0)
    m_audioThread = std::thread(&MediaPlayer::audioLoop, this);
  if (m_demuxer.videoStream() >= 0)
    m_videoThread = std::thread(&MediaPlayer::videoLoop, this);
  if (m_demuxer.subtitleStream() >= 0)
    m_subtitleThread = std::thread(&MediaPlayer::subtitleLoop, this);
  if (!m_playRecorded && m_library) {
    m_library->recordPlayback(m_metadata.path);
    m_playRecorded = true;
  }
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
    m_stopRequested.store(true);
    m_paused = false;
    m_cv.notify_all();
  }
  if (m_demuxThread.joinable())
    m_demuxThread.join();
  if (m_audioThread.joinable())
    m_audioThread.join();
  if (m_videoThread.joinable())
    m_videoThread.join();
  if (m_subtitleThread.joinable())
    m_subtitleThread.join();
  if (m_output)
    m_output->shutdown();
  if (m_videoOutput)
    m_videoOutput->shutdown();
  m_audioPackets.clear();
  m_videoPackets.clear();
  m_subtitlePackets.clear();
  m_frameQueue.clear();
  m_framePool.clear();
  m_running = false;
  if (m_callbacks.onStop)
    m_callbacks.onStop();
}

void MediaPlayer::seek(double seconds) {
  if (!m_demuxer.context())
    return;
  int64_t ts = static_cast<int64_t>(seconds * AV_TIME_BASE);
  av_seek_frame(m_demuxer.context(), -1, ts, AVSEEK_FLAG_BACKWARD);
  m_demuxer.resetEof();
  m_audioDecoder.flush();
  m_videoDecoder.flush();
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_audioPackets.clear();
    m_videoPackets.clear();
    m_subtitlePackets.clear();
    m_frameQueue.clear();
  }
  m_startTime = av_gettime() / 1000000.0 - seconds;
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

void MediaPlayer::enableShuffle(bool enabled) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_playlist.enableShuffle(enabled);
}

bool MediaPlayer::shuffleEnabled() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_playlist.shuffleEnabled();
}

std::vector<std::string> MediaPlayer::queue() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_playlist.items();
}

bool MediaPlayer::removeFromQueue(size_t index) {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_playlist.removeAt(index);
}

bool MediaPlayer::moveQueueItem(size_t from, size_t to) {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_playlist.moveItem(from, to);
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
    if (m_stopRequested.load())
      break;
    if (m_audioPackets.full() && m_videoPackets.full() && m_subtitlePackets.full()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    if (!m_demuxer.readPacket(pkt)) {
      if (m_callbacks.onFinished)
        m_callbacks.onFinished();
      bool advance = false;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        advance = m_autoAdvance && !m_playlist.empty();
      }
      if (advance) {
        std::thread([this]() { nextTrack(); }).detach();
      }
      break;
    }
    if (pkt.stream_index == m_demuxer.audioStream() && !m_audioPackets.full()) {
      m_audioPackets.push(&pkt);
    } else if (pkt.stream_index == m_demuxer.videoStream() && !m_videoPackets.full()) {
      m_videoPackets.push(&pkt);
    } else if (pkt.stream_index == m_demuxer.subtitleStream() && !m_subtitlePackets.full()) {
      m_subtitlePackets.push(&pkt);
    }
    av_packet_unref(&pkt);
  }
}

void MediaPlayer::audioLoop() {
  uint8_t audioBuffer[4096];
  while (true) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() {
        return (!m_paused && !m_stopRequested.load()) || m_stopRequested.load();
      });
      if (m_stopRequested.load() && m_audioPackets.size() == 0)
        break;
    }
    AVPacket *pkt = nullptr;
    if (m_audioPackets.pop(pkt, [this]() { return m_stopRequested.load() || m_demuxer.eof(); })) {
      int bytes = m_audioDecoder.decode(pkt, audioBuffer, sizeof(audioBuffer));
      av_packet_free(&pkt);
      if (bytes > 0 && m_output) {
        m_audioClock = m_audioDecoder.lastPts();
        double vol;
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          vol = m_volume;
        }
        int16_t *samples = reinterpret_cast<int16_t *>(audioBuffer);
        int sampleCount = bytes / sizeof(int16_t);
        if (vol < 0.999) {
          for (int i = 0; i < sampleCount; ++i) {
            int32_t s = static_cast<int32_t>(samples[i] * vol);
            if (s < -32768)
              s = -32768;
            else if (s > 32767)
              s = 32767;
            samples[i] = static_cast<int16_t>(s);
          }
        }
        std::vector<std::shared_ptr<AudioEffect>> effects;
        std::shared_ptr<Visualizer> vis;
        bool visEnabled;
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          effects = m_audioEffects;
          vis = m_visualizer;
          visEnabled = m_visualizationEnabled;
        }
        if (vis && visEnabled)
          vis->onAudioPCM(samples, sampleCount, m_audioDecoder.sampleRate(),
                          m_audioDecoder.channels());
        for (auto &effect : effects)
          effect->process(samples, sampleCount);
        m_output->write(audioBuffer, bytes);
        if (m_callbacks.onPosition)
          m_callbacks.onPosition(m_audioClock);
      }
    } else {
      break;
    }
  }
}

void MediaPlayer::videoLoop() {

  std::thread renderThread([this]() {
    while (true) {
      VideoFrame *frame = nullptr;
      if (m_frameQueue.pop(frame, [this]() { return m_stopRequested.load(); })) {
        if (!frame)
          continue;
        m_videoClock = frame->pts;
        double master =
            m_demuxer.audioStream() >= 0 ? m_audioClock : (av_gettime() / 1000000.0 - m_startTime);
        double delay = m_videoClock - master;
        if (delay > 0)
          std::this_thread::sleep_for(std::chrono::duration<double>(delay));
        if (m_videoOutput)
          m_videoOutput->displayFrame(*frame);
        if (m_callbacks.onPosition)
          m_callbacks.onPosition(m_videoClock);
        m_framePool.release(frame);
      } else if (m_stopRequested.load()) {
        break;
      }
    }
  });

  while (true) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() {
        return (!m_paused && !m_stopRequested.load()) || m_stopRequested.load();
      });
      if (m_stopRequested.load() && m_videoPackets.size() == 0)
        break;
    }
    AVPacket *pkt = nullptr;
    if (m_videoPackets.pop(pkt, [this]() { return m_stopRequested.load() || m_demuxer.eof(); })) {
      VideoFrame tmp{};
      bool ok = m_videoDecoder.decodeYUV(pkt, tmp);
      av_packet_free(&pkt);
      if (ok) {
        int lines[3] = {tmp.linesize[0], tmp.linesize[1], tmp.linesize[2]};
        auto *frame = m_framePool.acquire(tmp.width, tmp.height, lines);
        frame->width = tmp.width;
        frame->height = tmp.height;
        frame->linesize[0] = tmp.linesize[0];
        frame->linesize[1] = tmp.linesize[1];
        frame->linesize[2] = tmp.linesize[2];
        frame->pts = m_videoDecoder.lastPts();
        for (int y = 0; y < frame->height; ++y)
          memcpy(frame->data[0] + y * frame->linesize[0], tmp.data[0] + y * tmp.linesize[0],
                 frame->linesize[0]);
        for (int y = 0; y < frame->height / 2; ++y) {
          memcpy(frame->data[1] + y * frame->linesize[1], tmp.data[1] + y * tmp.linesize[1],
                 frame->linesize[1]);
          memcpy(frame->data[2] + y * frame->linesize[2], tmp.data[2] + y * tmp.linesize[2],
                 frame->linesize[2]);
        }
        while (!m_frameQueue.push(frame)) {
          if (m_stopRequested.load()) {
            m_framePool.release(frame);
            break;
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
      }
    } else {
      break;
    }
  }

  m_stopRequested.store(true);
  m_frameQueue.clear();
  m_framePool.clear();
  if (renderThread.joinable())
    renderThread.join();
}

bool MediaPlayer::setSubtitleTrack(int index) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_demuxer.setSubtitleStream(index)) {
    m_subtitleDecoder.flush();
    m_subtitlePackets.clear();
    return true;
  }
  return false;
}

void MediaPlayer::subtitleLoop() {
  while (true) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() {
        return (!m_paused && !m_stopRequested.load()) || m_stopRequested.load();
      });
      if (m_stopRequested.load() && m_subtitlePackets.size() == 0)
        break;
    }
    AVPacket *pkt = nullptr;
    if (m_subtitlePackets.pop(pkt,
                              [this]() { return m_stopRequested.load() || m_demuxer.eof(); })) {
      SubtitleCue cue{};
      if (m_subtitleDecoder.decode(pkt, cue)) {
        if (m_callbacks.onSubtitle)
          m_callbacks.onSubtitle(cue);
      }
      av_packet_free(&pkt);
    } else {
      break;
    }
  }
}

} // namespace mediaplayer
