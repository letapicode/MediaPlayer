#include "mediaplayer/VideoConverter.h"
#include <cassert>
#include <cstring>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

// Helper to create a small MP4 video for testing
static void createTestMp4(const std::string &path) {
  AVFormatContext *ctx = nullptr;
  avformat_alloc_output_context2(&ctx, nullptr, nullptr, path.c_str());
  const AVCodec *codec = avcodec_find_encoder(ctx->oformat->video_codec);
  AVStream *st = avformat_new_stream(ctx, codec);
  AVCodecContext *c = avcodec_alloc_context3(codec);
  c->width = 32;
  c->height = 32;
  c->pix_fmt = codec->pix_fmts ? codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
  c->time_base = {1, 25};
  if (ctx->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  avcodec_open2(c, codec, nullptr);
  avcodec_parameters_from_context(st->codecpar, c);
  st->time_base = c->time_base;
  if (!(ctx->oformat->flags & AVFMT_NOFILE))
    avio_open(&ctx->pb, path.c_str(), AVIO_FLAG_WRITE);
  avformat_write_header(ctx, nullptr);
  AVFrame *frame = av_frame_alloc();
  frame->format = c->pix_fmt;
  frame->width = c->width;
  frame->height = c->height;
  av_frame_get_buffer(frame, 0);
  for (int i = 0; i < 25; ++i) {
    av_frame_make_writable(frame);
    memset(frame->data[0], i, c->width * c->height);
    memset(frame->data[1], 128, c->width * c->height / 4);
    memset(frame->data[2], 128, c->width * c->height / 4);
    frame->pts = i;
    avcodec_send_frame(c, frame);
    AVPacket pkt{};
    while (avcodec_receive_packet(c, &pkt) == 0) {
      pkt.stream_index = st->index;
      av_interleaved_write_frame(ctx, &pkt);
      av_packet_unref(&pkt);
    }
  }
  avcodec_send_frame(c, nullptr);
  AVPacket pkt{};
  while (avcodec_receive_packet(c, &pkt) == 0) {
    pkt.stream_index = st->index;
    av_interleaved_write_frame(ctx, &pkt);
    av_packet_unref(&pkt);
  }
  av_write_trailer(ctx);
  av_frame_free(&frame);
  if (!(ctx->oformat->flags & AVFMT_NOFILE))
    avio_closep(&ctx->pb);
  avcodec_free_context(&c);
  avformat_free_context(ctx);
}

int main() {
  const std::string in = "test_in.mp4";
  const std::string out = "test_out.mp4";
  createTestMp4(in);
  mediaplayer::VideoConverter conv;
  mediaplayer::VideoEncodeOptions opts;
  opts.width = 16;
  opts.height = 16;
  opts.bitrate = 500000;
  bool ok = conv.convert(in, out, opts);
  assert(ok && "conversion failed");
  AVFormatContext *ctx = nullptr;
  assert(avformat_open_input(&ctx, out.c_str(), nullptr, nullptr) == 0);
  avformat_close_input(&ctx);
  return 0;
}
