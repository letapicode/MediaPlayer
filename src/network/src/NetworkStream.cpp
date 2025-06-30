#include "mediaplayer/NetworkStream.h"
#include <iostream>
#include <map>
#include <mutex>

namespace mediaplayer {

NetworkStream::NetworkStream() = default;

NetworkStream::~NetworkStream() {
  if (m_ctx)
    avformat_close_input(&m_ctx);
  if (m_opts)
    av_dict_free(&m_opts);
}

bool NetworkStream::open(const std::string &url) {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });
  if (avformat_open_input(&m_ctx, url.c_str(), nullptr, &m_opts) < 0) {
    std::cerr << "Failed to open URL: " << url << '\n';
    return false;
  }
  av_dict_free(&m_opts);
  return true;
}

void NetworkStream::setOption(const std::string &key, const std::string &value) {
  av_dict_set(&m_opts, key.c_str(), value.c_str(), 1);
}

std::string NetworkStream::getMetadata(const std::string &key) const {
  if (!m_ctx)
    return {};
  AVDictionaryEntry *tag = av_dict_get(m_ctx->metadata, key.c_str(), nullptr, 0);
  if (tag)
    return tag->value;
  return {};
}

std::map<std::string, std::string> NetworkStream::metadata() const {
  std::map<std::string, std::string> map;
  if (!m_ctx)
    return map;
  AVDictionaryEntry *tag = nullptr;
  while ((tag = av_dict_get(m_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    map[tag->key] = tag->value ? tag->value : "";
  }
  return map;
}

AVFormatContext *NetworkStream::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

} // namespace mediaplayer
