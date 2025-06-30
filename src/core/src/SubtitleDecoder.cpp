#include "mediaplayer/SubtitleDecoder.h"
#include <iostream>
#include <libavutil/avutil.h>

namespace mediaplayer {

SubtitleDecoder::SubtitleDecoder() {}

SubtitleDecoder::~SubtitleDecoder() {
  if (m_codecCtx)
    avcodec_free_context(&m_codecCtx);
}

bool SubtitleDecoder::open(AVFormatContext *fmtCtx, int streamIndex) {
  if (!fmtCtx || streamIndex < 0 || streamIndex >= static_cast<int>(fmtCtx->nb_streams))
    return false;
  AVStream *stream = fmtCtx->streams[streamIndex];
  const AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (!dec)
    return false;
  m_codecCtx = avcodec_alloc_context3(dec);
  if (!m_codecCtx)
    return false;
  if (avcodec_parameters_to_context(m_codecCtx, stream->codecpar) < 0)
    return false;
  if (avcodec_open2(m_codecCtx, dec, nullptr) < 0)
    return false;
  m_timeBase = stream->time_base;
  return true;
}

bool SubtitleDecoder::decode(AVPacket *pkt, SubtitleCue &cue) {
  if (!m_codecCtx || !pkt)
    return false;
  AVSubtitle sub{};
  int got = 0;
  int ret = avcodec_decode_subtitle2(m_codecCtx, &sub, &got, pkt);
  if (ret < 0 || !got)
    return false;
  std::string text;
  for (unsigned i = 0; i < sub.num_rects; ++i) {
    AVSubtitleRect *r = sub.rects[i];
    if (r->text) {
      text += r->text;
    } else if (r->ass) {
      std::string ass = r->ass;
      size_t pos = ass.rfind(',');
      if (pos != std::string::npos)
        text += ass.substr(pos + 1);
      else
        text += ass;
    }
    if (i + 1 < sub.num_rects)
      text += "\n";
  }
  cue.text = text;
  if (sub.pts != AV_NOPTS_VALUE)
    cue.start = sub.pts / (double)AV_TIME_BASE;
  else if (pkt->pts != AV_NOPTS_VALUE)
    cue.start = pkt->pts * av_q2d(m_timeBase);
  else
    cue.start = 0.0;
  cue.end = cue.start + sub.end_display_time / 1000.0;
  avsubtitle_free(&sub);
  return true;
}

void SubtitleDecoder::flush() {
  if (m_codecCtx)
    avcodec_flush_buffers(m_codecCtx);
}

} // namespace mediaplayer
