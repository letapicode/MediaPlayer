#include "mediaplayer/DashStream.h"
#include <iostream>
#include <mutex>

namespace mediaplayer {

DashStream::DashStream() = default;

DashStream::~DashStream() {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
}

bool DashStream::open(const std::string &url, int variantIndex, int bandwidth) {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });

  AVDictionary *opts = nullptr;
  if (variantIndex >= 0)
    av_dict_set_int(&opts, "program", variantIndex, 0);
  if (bandwidth > 0)
    av_dict_set_int(&opts, "variant_bitrate", bandwidth, 0);

  if (avformat_open_input(&m_ctx, url.c_str(), nullptr, &opts) < 0) {
    std::cerr << "Failed to open DASH URL: " << url << '\n';
    av_dict_free(&opts);
    return false;
  }

  av_dict_free(&opts);
  return true;
}

AVFormatContext *DashStream::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

} // namespace mediaplayer
