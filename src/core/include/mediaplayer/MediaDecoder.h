#ifndef MEDIAPLAYER_MEDIADECODER_H
#define MEDIAPLAYER_MEDIADECODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace mediaplayer {

class MediaDecoder {
public:
  virtual ~MediaDecoder() = default;
  virtual bool open(AVFormatContext *fmtCtx, int streamIndex) = 0;
  virtual int decode(AVPacket *pkt, uint8_t *outBuffer, int outBufferSize) = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIADECODER_H
