#include "mediaplayer/HlsStream.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

extern "C" {
#include <libavformat/avio.h>
}

namespace mediaplayer {

HlsStream::HlsStream() = default;

HlsStream::~HlsStream() {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
}

static std::string resolveUrl(const std::string &base, const std::string &rel) {
  if (rel.rfind("http://", 0) == 0 || rel.rfind("https://", 0) == 0)
    return rel;
  auto pos = base.find_last_of('/');
  if (pos == std::string::npos)
    return rel;
  return base.substr(0, pos + 1) + rel;
}

bool HlsStream::open(const std::string &playlistUrl) {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });

  AVIOContext *pb = nullptr;
  if (avio_open2(&pb, playlistUrl.c_str(), AVIO_FLAG_READ, nullptr, nullptr) < 0) {
    std::cerr << "Failed to open playlist: " << playlistUrl << '\n';
    return false;
  }
  std::vector<char> buf(4096);
  std::string data;
  for (;;) {
    int r = avio_read(pb, reinterpret_cast<unsigned char *>(buf.data()), buf.size());
    if (r < 0) {
      avio_closep(&pb);
      return false;
    }
    if (r == 0)
      break;
    data.append(buf.data(), r);
  }
  avio_closep(&pb);

  std::istringstream iss(data);
  std::string line;
  int bestBw = -1;
  std::string bestUrl;
  while (std::getline(iss, line)) {
    if (line.rfind("#EXT-X-STREAM-INF", 0) == 0) {
      size_t bwPos = line.find("BANDWIDTH=");
      if (bwPos != std::string::npos) {
        bwPos += 10; // length of "BANDWIDTH="
        size_t end = line.find(',', bwPos);
        int bw = std::stoi(line.substr(bwPos, end - bwPos));
        std::string next;
        if (std::getline(iss, next)) {
          if (bw > bestBw) {
            bestBw = bw;
            bestUrl = resolveUrl(playlistUrl, next);
          }
        }
      }
    }
  }

  std::string finalUrl = bestUrl.empty() ? playlistUrl : bestUrl;
  if (avformat_open_input(&m_ctx, finalUrl.c_str(), nullptr, nullptr) < 0) {
    std::cerr << "Failed to open stream: " << finalUrl << '\n';
    return false;
  }
  return true;
}

AVFormatContext *HlsStream::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

} // namespace mediaplayer
