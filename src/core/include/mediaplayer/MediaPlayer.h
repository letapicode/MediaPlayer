#ifndef MEDIAPLAYER_MEDIAPLAYER_H
#define MEDIAPLAYER_MEDIAPLAYER_H

#include <libavformat/avformat.h>
#include <string>

#include "AudioDecoder.h"

namespace mediaplayer {

class MediaPlayer {
public:
  MediaPlayer();
  ~MediaPlayer();

  bool open(const std::string &path);
  void play();
  void pause();
  void stop();
  double position() const; // seconds
  int readAudio(uint8_t *buffer, int bufferSize);

private:
  AVFormatContext *m_formatCtx{nullptr};
  AudioDecoder m_audioDecoder;
  int m_audioStream{-1};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYER_H
