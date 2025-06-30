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

std::string NetworkStream::metadata(const std::string &key) const {
  if (!m_ctx)
    return {};
  AVDictionaryEntry *tag = av_dict_get(m_ctx->metadata, key.c_str(), nullptr, 0);
  if (tag && tag->value)
    return tag->value;
  return {};
}

std::string NetworkStream::icyTitle() const {
  auto title = metadata("icy-title");
  if (title.empty())
    title = metadata("StreamTitle");
  return title;
}

std::string NetworkStream::icyName() const {
  auto name = metadata("icy-name");
  if (name.empty())
    name = metadata("icy-description");
  return name;
}

} // namespace mediaplayer
