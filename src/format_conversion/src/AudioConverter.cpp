#include "mediaplayer/AudioConverter.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#include <iostream>

namespace mediaplayer {

bool AudioConverter::convert(const std::string &inputPath, const std::string &outputPath) {
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
  int audioStream = av_find_best_stream(inCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
  if (audioStream < 0) {
    std::cerr << "No audio stream" << std::endl;
    avformat_close_input(&inCtx);
    return false;
  }

  AVStream *inSt = inCtx->streams[audioStream];
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

  AVFormatContext *outCtx = nullptr;
  if (avformat_alloc_output_context2(&outCtx, nullptr, nullptr, outputPath.c_str()) < 0) {
    avcodec_free_context(&decCtx);
    avformat_close_input(&inCtx);
    return false;
  }
  const AVCodec *enc = avcodec_find_encoder(outCtx->oformat->audio_codec);
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
  encCtx->channels = decCtx->channels;
  encCtx->channel_layout = av_get_default_channel_layout(encCtx->channels);
  encCtx->sample_rate = decCtx->sample_rate;
  encCtx->sample_fmt = enc->sample_fmts ? enc->sample_fmts[0] : decCtx->sample_fmt;
  encCtx->bit_rate = 192000;
  outSt->time_base = {1, encCtx->sample_rate};
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

  SwrContext *swr = swr_alloc_set_opts(nullptr, encCtx->channel_layout, encCtx->sample_fmt,
                                       encCtx->sample_rate, decCtx->channel_layout,
                                       decCtx->sample_fmt, decCtx->sample_rate, 0, nullptr);
  swr_init(swr);
  AVPacket pkt{};
  AVFrame *frame = av_frame_alloc();
  AVFrame *resampled = av_frame_alloc();
  resampled->channel_layout = encCtx->channel_layout;
  resampled->format = encCtx->sample_fmt;
  resampled->sample_rate = encCtx->sample_rate;
  int outSamples = encCtx->frame_size > 0 ? encCtx->frame_size : 1024;
  av_frame_set_nb_samples(resampled, outSamples);
  av_frame_get_buffer(resampled, 0);

  while (av_read_frame(inCtx, &pkt) >= 0) {
    if (pkt.stream_index != audioStream) {
      av_packet_unref(&pkt);
      continue;
    }
    avcodec_send_packet(decCtx, &pkt);
    av_packet_unref(&pkt);
    while (avcodec_receive_frame(decCtx, frame) == 0) {
      swr_convert(swr, resampled->data, resampled->nb_samples, (const uint8_t **)frame->data,
                  frame->nb_samples);
      resampled->pts = frame->pts;
      avcodec_send_frame(encCtx, resampled);
      AVPacket outPkt{};
      while (avcodec_receive_packet(encCtx, &outPkt) == 0) {
        outPkt.stream_index = outSt->index;
        av_interleaved_write_frame(outCtx, &outPkt);
        av_packet_unref(&outPkt);
      }
    }
  }
  // flush encoder
  avcodec_send_frame(encCtx, nullptr);
  AVPacket outPkt{};
  while (avcodec_receive_packet(encCtx, &outPkt) == 0) {
    outPkt.stream_index = outSt->index;
    av_interleaved_write_frame(outCtx, &outPkt);
    av_packet_unref(&outPkt);
  }

  av_write_trailer(outCtx);
  av_frame_free(&frame);
  av_frame_free(&resampled);
  swr_free(&swr);
  if (!(outCtx->oformat->flags & AVFMT_NOFILE))
    avio_closep(&outCtx->pb);
  avcodec_free_context(&encCtx);
  avformat_free_context(outCtx);
  avcodec_free_context(&decCtx);
  avformat_close_input(&inCtx);
  return true;
}

} // namespace mediaplayer
