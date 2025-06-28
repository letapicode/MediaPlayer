#include "mediaplayer/VideoDecoder.h"
#include <iostream>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>

namespace mediaplayer {

VideoDecoder::VideoDecoder() { m_frame = av_frame_alloc(); }

VideoDecoder::~VideoDecoder() {
  if (m_codecCtx) {
    avcodec_free_context(&m_codecCtx);
  }
  if (m_swsCtx) {
    sws_freeContext(m_swsCtx);
  }
  if (m_frame) {
    av_frame_free(&m_frame);
  }
#ifdef MEDIAPLAYER_HW_DECODING
  if (m_hwDeviceCtx) {
    av_buffer_unref(&m_hwDeviceCtx);
  }
#endif
}

bool VideoDecoder::open(AVFormatContext *fmtCtx, int streamIndex,
                        const std::string &preferredHwDevice) {
  if (streamIndex < 0 || streamIndex >= static_cast<int>(fmtCtx->nb_streams)) {
    return false;
  }
  AVStream *stream = fmtCtx->streams[streamIndex];
  m_timeBase = stream->time_base;
  const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (!dec) {
    std::cerr << "No video decoder" << std::endl;
    return false;
  }
  m_codecCtx = avcodec_alloc_context3(dec);
  if (!m_codecCtx) {
    return false;
  }
  if (avcodec_parameters_to_context(m_codecCtx, stream->codecpar) < 0) {
    avcodec_free_context(&m_codecCtx);
    return false;
  }
#ifdef MEDIAPLAYER_HW_DECODING
  if (!preferredHwDevice.empty()) {
    AVHWDeviceType type = av_hwdevice_find_type_by_name(preferredHwDevice.c_str());
    if (type != AV_HWDEVICE_TYPE_NONE) {
      if (av_hwdevice_ctx_create(&m_hwDeviceCtx, type, nullptr, nullptr, 0) == 0) {
        m_codecCtx->hw_device_ctx = av_buffer_ref(m_hwDeviceCtx);
      }
    }
  }
#endif
  if (avcodec_open2(m_codecCtx, dec, nullptr) < 0) {
    avcodec_free_context(&m_codecCtx);
    return false;
  }
  m_swsCtx =
      sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt, m_codecCtx->width,
                     m_codecCtx->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
  if (!m_swsCtx) {
    avcodec_free_context(&m_codecCtx);
    return false;
  }
  return true;
}

int VideoDecoder::decode(AVPacket *pkt, uint8_t *outBuffer, int outBufferSize) {
  if (avcodec_send_packet(m_codecCtx, pkt) < 0) {
    return -1;
  }
  int total = 0;
  const int frameBytes =
      av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_codecCtx->width, m_codecCtx->height, 1);
  while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
    m_lastPts = m_frame->best_effort_timestamp;
    if (total + frameBytes > outBufferSize) {
      break;
    }
    uint8_t *dstData[4] = {outBuffer + total, nullptr, nullptr, nullptr};
    int dstLinesize[4] = {m_codecCtx->width * 4, 0, 0, 0};
    sws_scale(m_swsCtx, m_frame->data, m_frame->linesize, 0, m_codecCtx->height, dstData,
              dstLinesize);
    total += frameBytes;
  }
  return total;
}

void VideoDecoder::flush() {
  if (m_codecCtx) {
    avcodec_flush_buffers(m_codecCtx);
  }
}

double VideoDecoder::lastPts() const {
  if (m_lastPts == AV_NOPTS_VALUE)
    return 0.0;
  return m_lastPts * av_q2d(m_timeBase);
}

} // namespace mediaplayer
