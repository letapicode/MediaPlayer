#include "mediaplayer/VideoConverter.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <algorithm>
#include <iostream>

namespace mediaplayer {

bool VideoConverter::convert(const std::string &inputPath, const std::string &outputPath,
                             const VideoEncodeOptions &options,
                             std::function<void(float)> progress) {
  AVFormatContext *inCtx = nullptr;
  if (avformat_open_input(&inCtx, inputPath.c_str(), nullptr, nullptr) < 0) {
    std::cerr << "Failed to open input" << std::endl;
    return false;
  }
  if (avformat_find_stream_info(inCtx, nullptr) < 0) {
    std::cerr << "Failed to find stream info" << std::endl;
    avformat_close_input(&inCtx);
    return false;
  }
  double totalDuration =
      inCtx->duration != AV_NOPTS_VALUE ? static_cast<double>(inCtx->duration) / AV_TIME_BASE : 0.0;
  int vStream = av_find_best_stream(inCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (vStream < 0) {
    std::cerr << "No video stream" << std::endl;
    avformat_close_input(&inCtx);
    return false;
  }
  AVStream *inSt = inCtx->streams[vStream];
  const AVCodec *dec = avcodec_find_decoder(inSt->codecpar->codec_id);
  if (!dec) {
    avformat_close_input(&inCtx);
    return false;
  }
  AVCodecContext *decCtx = avcodec_alloc_context3(dec);
  avcodec_parameters_to_context(decCtx, inSt->codecpar);
  if (avcodec_open2(decCtx, dec, nullptr) < 0) {
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }
  int width = options.width > 0 ? options.width : decCtx->width;
  int height = options.height > 0 ? options.height : decCtx->height;

  AVFormatContext *outCtx = nullptr;
  if (avformat_alloc_output_context2(&outCtx, nullptr, nullptr, outputPath.c_str()) < 0) {
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }
  const AVCodec *enc = nullptr;
  if (!options.codec.empty())
    enc = avcodec_find_encoder_by_name(options.codec.c_str());
  if (!enc)
    enc = avcodec_find_encoder(outCtx->oformat->video_codec);
  if (!enc) {
    std::cerr << "No encoder" << std::endl;
    avformat_free_context(outCtx);
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }
  AVStream *outSt = avformat_new_stream(outCtx, enc);
  if (!outSt) {
    avformat_free_context(outCtx);
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }

  AVCodecContext *encCtx = avcodec_alloc_context3(enc);
  encCtx->width = width;
  encCtx->height = height;
  encCtx->pix_fmt = enc->pix_fmts ? enc->pix_fmts[0] : AV_PIX_FMT_YUV420P;
  encCtx->time_base = {1, 25};
  encCtx->bit_rate = options.bitrate > 0 ? options.bitrate : 1000000;
  outSt->time_base = encCtx->time_base;
  if (outCtx->oformat->flags & AVFMT_GLOBALHEADER)
    encCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  if (avcodec_open2(encCtx, enc, nullptr) < 0) {
    avcodec_free_context(&encCtx);
    avformat_free_context(outCtx);
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }
  avcodec_parameters_from_context(outSt->codecpar, encCtx);

  if (!(outCtx->oformat->flags & AVFMT_NOFILE)) {
    if (avio_open(&outCtx->pb, outputPath.c_str(), AVIO_FLAG_WRITE) < 0) {
      avcodec_free_context(&encCtx);
      avformat_free_context(outCtx);
      avcodec_free_context(&decCtx);
      avformat_close_input(&inCtx);
      return false;
    }
  }
  if (avformat_write_header(outCtx, nullptr) < 0) {
    avio_closep(&outCtx->pb);
    avcodec_free_context(&encCtx);
    avformat_free_context(outCtx);
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }

  SwsContext *sws = sws_getContext(decCtx->width, decCtx->height, decCtx->pix_fmt, width, height,
                                   encCtx->pix_fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);
  AVFrame *frame = av_frame_alloc();
  AVFrame *scaled = av_frame_alloc();
  scaled->format = encCtx->pix_fmt;
  scaled->width = width;
  scaled->height = height;
  av_frame_get_buffer(scaled, 0);
  AVPacket pkt{};

  while (av_read_frame(inCtx, &pkt) >= 0) {
    if (pkt.stream_index != vStream) {
      av_packet_unref(&pkt);
      continue;
    }
    avcodec_send_packet(decCtx, &pkt);
    av_packet_unref(&pkt);
    while (avcodec_receive_frame(decCtx, frame) == 0) {
      sws_scale(sws, frame->data, frame->linesize, 0, decCtx->height, scaled->data,
                scaled->linesize);
      scaled->pts = frame->pts;
      avcodec_send_frame(encCtx, scaled);
      if (totalDuration > 0 && progress) {
        double sec = frame->pts != AV_NOPTS_VALUE ? frame->pts * av_q2d(inSt->time_base) : 0.0;
        progress(std::min(1.0f, static_cast<float>(sec / totalDuration)));
      }
      AVPacket outPkt{};
      while (avcodec_receive_packet(encCtx, &outPkt) == 0) {
        outPkt.stream_index = outSt->index;
        outPkt.pts = av_rescale_q(outPkt.pts, encCtx->time_base, outSt->time_base);
        outPkt.dts = av_rescale_q(outPkt.dts, encCtx->time_base, outSt->time_base);
        outPkt.duration = av_rescale_q(outPkt.duration, encCtx->time_base, outSt->time_base);
        av_interleaved_write_frame(outCtx, &outPkt);
        av_packet_unref(&outPkt);
      }
    }
  }

  avcodec_send_frame(encCtx, nullptr);
  AVPacket outPkt{};
  while (avcodec_receive_packet(encCtx, &outPkt) == 0) {
    outPkt.stream_index = outSt->index;
    outPkt.pts = av_rescale_q(outPkt.pts, encCtx->time_base, outSt->time_base);
    outPkt.dts = av_rescale_q(outPkt.dts, encCtx->time_base, outSt->time_base);
    outPkt.duration = av_rescale_q(outPkt.duration, encCtx->time_base, outSt->time_base);
    av_interleaved_write_frame(outCtx, &outPkt);
    av_packet_unref(&outPkt);
  }

  if (totalDuration > 0 && progress)
    progress(1.0f);

  av_write_trailer(outCtx);
  av_frame_free(&frame);
  av_frame_free(&scaled);
  sws_freeContext(sws);
  if (!(outCtx->oformat->flags & AVFMT_NOFILE))
    avio_closep(&outCtx->pb);
  avcodec_free_context(&encCtx);
  avformat_free_context(outCtx);
  avcodec_free_context(&decCtx);
  avformat_close_input(&inCtx);
  return true;
}

} // namespace mediaplayer
