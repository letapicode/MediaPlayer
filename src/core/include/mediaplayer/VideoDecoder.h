#ifndef MEDIAPLAYER_VIDEODECODER_H
#define MEDIAPLAYER_VIDEODECODER_H

#include "MediaDecoder.h"
#include <string>
#ifdef MEDIAPLAYER_HW_DECODING
#include <libavutil/hwcontext.h>
#if defined(__ANDROID__)
#include <jni.h>
#endif
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
  // `preferredHwDevice` can be one of "dxva2", "d3d11va", "videotoolbox",
  // "vaapi" or "mediacodec" depending on the platform.
  bool open(AVFormatContext *fmtCtx, int streamIndex, const std::string &preferredHwDevice);
  // Decode packet and write YUV420P data into outBuffer. Returns bytes written.
  int decode(AVPacket *pkt, uint8_t *outBuffer, int outBufferSize) override;
  void flush() override;
  int width() const { return m_codecCtx ? m_codecCtx->width : 0; }
  int height() const { return m_codecCtx ? m_codecCtx->height : 0; }
  double lastPts() const;

#ifdef MEDIAPLAYER_HW_DECODING
  static enum AVPixelFormat get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *fmts);
#endif

private:
  AVCodecContext *m_codecCtx{nullptr};
  SwsContext *m_swsCtx{nullptr};
  AVFrame *m_frame{nullptr};
  AVRational m_timeBase{1, 1};
  int64_t m_lastPts{AV_NOPTS_VALUE};
#ifdef MEDIAPLAYER_HW_DECODING
  AVBufferRef *m_hwDeviceCtx{nullptr};
  AVPixelFormat m_hwPixFmt{AV_PIX_FMT_NONE};
  AVFrame *m_swFrame{nullptr};
#endif
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEODECODER_H
