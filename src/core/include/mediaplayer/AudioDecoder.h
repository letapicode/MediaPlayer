#ifndef MEDIAPLAYER_AUDIODECODER_H
#define MEDIAPLAYER_AUDIODECODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}

namespace mediaplayer {

class AudioDecoder {
public:
  AudioDecoder();
  ~AudioDecoder();

  bool open(AVFormatContext *fmtCtx, int streamIndex);
  int decode(AVPacket *pkt, uint8_t *outBuffer, int outBufferSize);
  void flush();
  int sampleRate() const;
  int channels() const;
  double lastPts() const;

private:
  AVCodecContext *m_codecCtx{nullptr};
  SwrContext *m_swrCtx{nullptr};
  AVFrame *m_frame{nullptr};
  AVRational m_timeBase{1, 1};
  int64_t m_lastPts{AV_NOPTS_VALUE};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIODECODER_H
