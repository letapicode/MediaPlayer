#ifndef MEDIAPLAYER_MEDIAPLAYER_H
#define MEDIAPLAYER_MEDIAPLAYER_H

#include <string>

#include "AudioDecoder.h"
#include "AudioEffect.h"
#include "AudioOutput.h"
#include "LibraryDB.h"
#include "MediaDemuxer.h"
#include "MediaMetadata.h"
#include "NullAudioOutput.h"
#include "NullVideoOutput.h"
#include "OpenGLVideoOutput.h"
#include "PacketQueue.h"
#include "PlaybackCallbacks.h"
#include "PlaylistManager.h"
#include "VideoDecoder.h"
#include "VideoFrameQueue.h"
#include "VideoOutput.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace mediaplayer {

class MediaPlayer {
public:
  MediaPlayer();
  ~MediaPlayer();

  bool open(const std::string &path);
  void play();
  void pause();
  void stop();
  void seek(double seconds);
  void setPlaylist(const std::vector<std::string> &paths);
  void addToPlaylist(const std::string &path);
  void clearPlaylist();
  bool nextTrack();
  void enableShuffle(bool enabled);
  bool shuffleEnabled() const;
  void setAudioOutput(std::unique_ptr<AudioOutput> output);
  void setVideoOutput(std::unique_ptr<VideoOutput> output);
  void setPreferredHardwareDevice(const std::string &device);
  void setCallbacks(PlaybackCallbacks callbacks);
  void setLibrary(LibraryDB *db);
  void addAudioEffect(std::shared_ptr<AudioEffect> effect);
  void removeAudioEffect(std::shared_ptr<AudioEffect> effect);
  void setVolume(double volume); // 0.0 - 1.0
  double volume() const;
  double position() const; // seconds
  void setNetworkBufferSize(size_t size);
  size_t networkBufferSize() const;
  void setAutoAdvance(bool enable);
  bool autoAdvance() const { return m_autoAdvance; }
  const MediaMetadata &metadata() const { return m_metadata; }
  int readAudio(uint8_t *buffer, int bufferSize);
  int readVideo(uint8_t *buffer, int bufferSize);

private:
  void demuxLoop();
  void audioLoop();
  void videoLoop();
  MediaDemuxer m_demuxer;
  AudioDecoder m_audioDecoder;
  VideoDecoder m_videoDecoder;
  std::unique_ptr<AudioOutput> m_output;
  std::unique_ptr<VideoOutput> m_videoOutput;
  std::thread m_demuxThread;
  std::thread m_audioThread;
  std::thread m_videoThread;
  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
  std::atomic<bool> m_running{false};
  bool m_paused{false};
  double m_audioClock{0.0};
  double m_videoClock{0.0};
  double m_startTime{0.0};
  size_t m_networkBufferSize{0};
  std::atomic<bool> m_stopRequested{false};
  PacketQueue m_audioPackets;
  PacketQueue m_videoPackets;
  VideoFrameQueue m_frameQueue;
  PlaybackCallbacks m_callbacks;
  std::vector<std::shared_ptr<AudioEffect>> m_audioEffects;
  PlaylistManager m_playlist;
  LibraryDB *m_library{nullptr};
  bool m_playRecorded{false};
  double m_volume{1.0};
  MediaMetadata m_metadata;
  std::string m_hwDevice;
  bool m_autoAdvance{true};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYER_H
