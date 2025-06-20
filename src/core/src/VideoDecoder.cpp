#include "mediaplayer/VideoDecoder.h"
#include <iostream>

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
}

bool VideoDecoder::open(AVFormatContext *fmtCtx, int streamIndex) {
  if (streamIndex < 0 || streamIndex >= static_cast<int>(fmtCtx->nb_streams)) {
    return false;
  }
  AVStream *stream = fmtCtx->streams[streamIndex];
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
    return false;
  }
  if (avcodec_open2(m_codecCtx, dec, nullptr) < 0) {
    return false;
  }
  m_swsCtx =
      sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt, m_codecCtx->width,
                     m_codecCtx->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
  return m_swsCtx != nullptr;
}

int VideoDecoder::decode(AVPacket *pkt) {
  if (avcodec_send_packet(m_codecCtx, pkt) < 0) {
    return -1;
  }
  int got = 0;
  while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
    // For stub, just pretend we converted frame to RGBA
    got += m_codecCtx->width * m_codecCtx->height * 4;
  }
  return got;
}

} // namespace mediaplayer
