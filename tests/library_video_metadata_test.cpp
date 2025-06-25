#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

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

static void queryVideoMeta(sqlite3 *db, const std::string &path, int &dur, int &w, int &h) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT duration,width,height FROM MediaItem WHERE path=?1;", -1, &stmt,
                     nullptr);
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    dur = sqlite3_column_int(stmt, 0);
    w = sqlite3_column_int(stmt, 1);
    h = sqlite3_column_int(stmt, 2);
  }
  sqlite3_finalize(stmt);
}

int main() {
  const char *dbPath = "video_meta.db";
  const std::string video = "meta_test.mp4";
  createTestMp4(video);
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  db.scanDirectory(".");
  db.close();
  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  int duration = 0, width = 0, height = 0;
  queryVideoMeta(conn, video, duration, width, height);
  sqlite3_close(conn);
  std::remove(video.c_str());
  std::remove(dbPath);
  assert(duration > 0);
  assert(width == 32 && height == 32);
  return 0;
}
