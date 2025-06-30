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

bool NetworkStream::open(const std::string &url) { return open(url, nullptr); }

bool NetworkStream::open(const std::string &url, AVDictionary *options) {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });
  if (avformat_open_input(&m_ctx, url.c_str(), nullptr, &options) < 0) {
    std::cerr << "Failed to open URL: " << url << '\n';
    if (options)
      av_dict_free(&options);
    return false;
  }
  if (options)
    av_dict_free(&options);
  return true;
}

AVFormatContext *NetworkStream::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

} // namespace mediaplayer
