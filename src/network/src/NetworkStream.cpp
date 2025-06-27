#include "mediaplayer/NetworkStream.h"
#include <iostream>

namespace mediaplayer {

NetworkStream::NetworkStream() = default;

NetworkStream::~NetworkStream() {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
}

bool NetworkStream::open(const std::string &url) {
  if (avformat_open_input(&m_ctx, url.c_str(), nullptr, nullptr) < 0) {
    std::cerr << "Failed to open URL: " << url << '\n';
    return false;
  }
  return true;
}

AVFormatContext *NetworkStream::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

} // namespace mediaplayer
