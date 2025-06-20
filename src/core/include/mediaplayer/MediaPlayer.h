#ifndef MEDIAPLAYER_MEDIAPLAYER_H
#define MEDIAPLAYER_MEDIAPLAYER_H

#include <libavformat/avformat.h>
#include <string>

#include "AudioDecoder.h"
#include "AudioOutput.h"
#include "NullAudioOutput.h"
#include "NullVideoOutput.h"
#include "PacketQueue.h"
#include "VideoDecoder.h"
#include "VideoOutput.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

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
  void setAudioOutput(std::unique_ptr<AudioOutput> output);
  void setVideoOutput(std::unique_ptr<VideoOutput> output);
  double position() const; // seconds
  int readAudio(uint8_t *buffer, int bufferSize);
  int readVideo(uint8_t *buffer, int bufferSize);

private:
  void demuxLoop();
  void audioLoop();
  void videoLoop();
  AVFormatContext *m_formatCtx{nullptr};
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
  bool m_stopRequested{false};
  int m_audioStream{-1};
  int m_videoStream{-1};
  PacketQueue m_audioPackets;
  PacketQueue m_videoPackets;
  bool m_eof{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYER_H
