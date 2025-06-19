#include "mediaplayer/AudioDecoder.h"
#include <iostream>

namespace mediaplayer {

AudioDecoder::AudioDecoder() { m_frame = av_frame_alloc(); }

AudioDecoder::~AudioDecoder() {
  if (m_codecCtx) {
    avcodec_free_context(&m_codecCtx);
  }
  if (m_swrCtx) {
    swr_free(&m_swrCtx);
  }
  if (m_frame) {
    av_frame_free(&m_frame);
  }
}

bool AudioDecoder::open(AVFormatContext *fmtCtx, int streamIndex) {
  if (streamIndex < 0 || streamIndex >= static_cast<int>(fmtCtx->nb_streams)) {
    return false;
  }
  AVStream *stream = fmtCtx->streams[streamIndex];
  const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (!dec) {
    std::cerr << "No decoder for stream" << std::endl;
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
  m_swrCtx = swr_alloc_set_opts(nullptr, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                m_codecCtx->sample_rate, m_codecCtx->channel_layout,
                                m_codecCtx->sample_fmt, m_codecCtx->sample_rate, 0, nullptr);
  if (!m_swrCtx || swr_init(m_swrCtx) < 0) {
    std::cerr << "Failed to init resampler" << std::endl;
    return false;
  }
  return true;
}

int AudioDecoder::decode(AVPacket *pkt, uint8_t *outBuffer, int outBufferSize) {
  if (avcodec_send_packet(m_codecCtx, pkt) < 0) {
    return -1;
  }
  int total = 0;
  while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
    int dst_nb_samples =
        av_rescale_rnd(swr_get_delay(m_swrCtx, m_codecCtx->sample_rate) + m_frame->nb_samples,
                       m_codecCtx->sample_rate, m_codecCtx->sample_rate, AV_ROUND_UP);
    int needed = av_samples_get_buffer_size(nullptr, 2, dst_nb_samples, AV_SAMPLE_FMT_S16, 1);
    if (total + needed > outBufferSize) {
      break;
    }
    uint8_t *dst = outBuffer + total;
    int len = swr_convert(m_swrCtx, &dst, dst_nb_samples, (const uint8_t **)m_frame->extended_data,
                          m_frame->nb_samples);
    if (len < 0) {
      return -1;
    }
    total += len * 2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
  }
  return total;
}

} // namespace mediaplayer
