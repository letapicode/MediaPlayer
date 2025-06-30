#include "mediaplayer/SubtitleDecoder.h"
#include <cassert>
#include <fstream>

int main() {
  const char *srt =
      "1\n00:00:01,000 --> 00:00:02,000\nHello\n\n2\n00:00:03,000 --> 00:00:04,000\nWorld\n";
  const char *path = "decoder.srt";
  std::ofstream out(path);
  out << srt;
  out.close();

  AVFormatContext *fmt = nullptr;
  assert(avformat_open_input(&fmt, path, nullptr, nullptr) == 0);
  assert(avformat_find_stream_info(fmt, nullptr) >= 0);
  mediaplayer::SubtitleDecoder dec;
  assert(dec.open(fmt, 0));
  AVPacket pkt;
  bool got = false;
  while (av_read_frame(fmt, &pkt) == 0) {
    mediaplayer::SubtitleCue cue{};
    if (dec.decode(&pkt, cue)) {
      if (!got) {
        assert(cue.text == "Hello");
        got = true;
      } else {
        assert(cue.text == "World");
      }
    }
    av_packet_unref(&pkt);
  }
  avformat_close_input(&fmt);
  std::remove(path);
  return 0;
}
