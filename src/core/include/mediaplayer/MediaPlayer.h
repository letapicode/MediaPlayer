#ifndef MEDIAPLAYER_MEDIAPLAYER_H
#define MEDIAPLAYER_MEDIAPLAYER_H

#include <libavformat/avformat.h>
#include <string>

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

private:
  AVFormatContext *m_formatCtx{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYER_H
