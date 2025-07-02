#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>

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

static void setFileTitle(const std::string &path, const std::string &title) {
  TagLib::FileRef f(path.c_str());
  if (!f.isNull() && f.tag()) {
    f.tag()->setTitle(title.c_str());
    f.save();
  }
}

static std::string titleFor(sqlite3 *db, const std::string &path) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT title FROM MediaItem WHERE path=?1;", -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  std::string title;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *txt = sqlite3_column_text(stmt, 0);
    if (txt)
      title = reinterpret_cast<const char *>(txt);
  }
  sqlite3_finalize(stmt);
  return title;
}

int main() {
  const char *dbPath = "rescan.db";
  const std::string video = "rescan_test.mp4";
  createTestMp4(video);
  setFileTitle(video, "First");
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  db.scanDirectory(".");
  db.close();

  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  assert(titleFor(conn, video) == "First");
  sqlite3_close(conn);

  setFileTitle(video, "Second");
  mediaplayer::LibraryDB db2(dbPath);
  assert(db2.open());
  db2.scanDirectory(".");
  db2.close();

  sqlite3_open(dbPath, &conn);
  assert(titleFor(conn, video) == "Second");
  sqlite3_close(conn);

  std::remove(video.c_str());
  std::remove(dbPath);
  return 0;
}
