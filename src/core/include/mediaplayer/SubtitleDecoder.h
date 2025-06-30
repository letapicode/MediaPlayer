#ifndef MEDIAPLAYER_SUBTITLEDECODER_H
#define MEDIAPLAYER_SUBTITLEDECODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "mediaplayer/SrtParser.h"

namespace mediaplayer {

class SubtitleDecoder {
public:
  SubtitleDecoder();
  ~SubtitleDecoder();

  bool open(AVFormatContext *fmtCtx, int streamIndex);
  bool decode(AVPacket *pkt, SubtitleCue &cue);
  void flush();

private:
  AVCodecContext *m_codecCtx{nullptr};
  AVRational m_timeBase{1, 1};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SUBTITLEDECODER_H
