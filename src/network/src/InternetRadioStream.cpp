#include "mediaplayer/InternetRadioStream.h"
#include <iostream>
#include <mutex>

namespace mediaplayer {

InternetRadioStream::InternetRadioStream() = default;

InternetRadioStream::~InternetRadioStream() {
  if (m_ctx && m_ownsContext) {
    avformat_close_input(&m_ctx);
  }
}

bool InternetRadioStream::open(const std::string &url) {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });
  AVDictionary *opts = nullptr;
  av_dict_set(&opts, "icy", "1", 0);
  if (avformat_open_input(&m_ctx, url.c_str(), nullptr, &opts) < 0) {
    std::cerr << "Failed to open URL: " << url << '\n';
    av_dict_free(&opts);
    return false;
  }
  av_dict_free(&opts);
  if (avformat_find_stream_info(m_ctx, nullptr) < 0) {
    std::cerr << "Failed to retrieve stream info\n";
    avformat_close_input(&m_ctx);
    return false;
  }
  updateMetadata();
  return true;
}

AVFormatContext *InternetRadioStream::release() {
  m_ownsContext = false;
  return m_ctx;
}

void InternetRadioStream::setMetadataCallback(MetadataCallback cb) {
  m_metadataCallback = std::move(cb);
}

void InternetRadioStream::updateMetadata() {
  if (!m_ctx)
    return;
  AVDictionaryEntry *tag = av_dict_get(m_ctx->metadata, "StreamTitle", nullptr, 0);
  if (!tag)
    tag = av_dict_get(m_ctx->metadata, "icy-title", nullptr, 0);
  if (tag && tag->value) {
    std::string title = tag->value;
    if (title != m_lastTitle) {
      m_lastTitle = title;
      if (m_metadataCallback)
        m_metadataCallback(title);
    }
  }
}

} // namespace mediaplayer
