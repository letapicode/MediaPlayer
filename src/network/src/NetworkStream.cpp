#include "mediaplayer/NetworkStream.h"
#include <iostream>
#include <mutex>

namespace mediaplayer {

NetworkStream::NetworkStream() = default;

NetworkStream::~NetworkStream() {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
}

bool NetworkStream::open(const std::string &url) {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });
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
