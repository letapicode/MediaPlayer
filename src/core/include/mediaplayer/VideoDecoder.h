#ifndef MEDIAPLAYER_VIDEODECODER_H
#define MEDIAPLAYER_VIDEODECODER_H

#include "MediaDecoder.h"
#include <string>
#ifdef MEDIAPLAYER_HW_DECODING
#include <libavutil/hwcontext.h>
#endif

extern "C" {
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

namespace mediaplayer {

class VideoDecoder : public MediaDecoder {
public:
  VideoDecoder();
  ~VideoDecoder() override;

  bool open(AVFormatContext *fmtCtx, int streamIndex) override {
    return open(fmtCtx, streamIndex, "");
  }
  bool open(AVFormatContext *fmtCtx, int streamIndex, const std::string &preferredHwDevice);
  // Decode packet and write RGBA data into outBuffer. Returns bytes written.
  int decode(AVPacket *pkt, uint8_t *outBuffer, int outBufferSize) override;
  void flush() override;
  int width() const { return m_codecCtx ? m_codecCtx->width : 0; }
  int height() const { return m_codecCtx ? m_codecCtx->height : 0; }
  double lastPts() const;

private:
  AVCodecContext *m_codecCtx{nullptr};
  SwsContext *m_swsCtx{nullptr};
  AVFrame *m_frame{nullptr};
  AVRational m_timeBase{1, 1};
  int64_t m_lastPts{AV_NOPTS_VALUE};
#ifdef MEDIAPLAYER_HW_DECODING
  AVBufferRef *m_hwDeviceCtx{nullptr};
#endif
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEODECODER_H
