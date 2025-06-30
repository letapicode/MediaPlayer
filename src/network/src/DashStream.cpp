#include "mediaplayer/DashStream.h"
#include <iostream>
#include <mutex>
#include <regex>

namespace mediaplayer {

DashStream::DashStream() = default;

DashStream::~DashStream() {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
}

static std::string extractBaseURL(const std::string &section) {
  const std::regex baseRegex("<BaseURL>([^<]+)</BaseURL>", std::regex_constants::icase);
  std::smatch match;
  if (std::regex_search(section, match, baseRegex)) {
    return match[1];
  }
  return {};
}

bool DashStream::parseManifest(const std::string &manifest) {
  std::regex adaptRegex("<AdaptationSet[^>]+>", std::regex_constants::icase);
  std::regex videoRegex("contentType=\"video\"|mimeType=\"video", std::regex_constants::icase);
  std::regex audioRegex("contentType=\"audio\"|mimeType=\"audio", std::regex_constants::icase);
  auto begin = std::sregex_iterator(manifest.begin(), manifest.end(), adaptRegex);
  auto end = std::sregex_iterator();
  size_t pos = 0;
  for (auto it = begin; it != end; ++it) {
    const auto &match = *it;
    size_t start = match.position(0) + match.length(0);
    size_t close = manifest.find("</AdaptationSet>", start);
    if (close == std::string::npos)
      continue;
    std::string block = manifest.substr(start, close - start);
    bool isVideo = std::regex_search(match.str(0), videoRegex);
    bool isAudio = std::regex_search(match.str(0), audioRegex);
    std::regex repRegex("<Representation[^>]*>", std::regex_constants::icase);
    std::smatch repMatch;
    if (std::regex_search(block, repMatch, repRegex)) {
      std::string repBlock = block.substr(repMatch.position(0) + repMatch.length(0));
      std::string url = extractBaseURL(repBlock);
      if (isVideo && m_videoRep.empty()) {
        m_videoRep = url;
      } else if (isAudio && m_audioRep.empty()) {
        m_audioRep = url;
      }
    }
    if (!m_videoRep.empty() && !m_audioRep.empty())
      break;
    pos = close + 1;
  }
  return !m_videoRep.empty() || !m_audioRep.empty();
}

bool DashStream::downloadFile(const std::string &url, std::string &data) {
  AVIOContext *ioCtx = nullptr;
  if (avio_open2(&ioCtx, url.c_str(), AVIO_FLAG_READ, nullptr, nullptr) < 0) {
    std::cerr << "Failed to download: " << url << '\n';
    return false;
  }
  unsigned char buf[4096];
  while (true) {
    int ret = avio_read(ioCtx, buf, sizeof(buf));
    if (ret <= 0)
      break;
    data.append(reinterpret_cast<char *>(buf), ret);
  }
  avio_close(ioCtx);
  return true;
}

bool DashStream::open(const std::string &mpdUrl) {
  static std::once_flag initFlag;
  std::call_once(initFlag, [] { avformat_network_init(); });

  std::string manifest;
  if (!downloadFile(mpdUrl, manifest))
    return false;
  parseManifest(manifest);
  if (avformat_open_input(&m_ctx, mpdUrl.c_str(), nullptr, nullptr) < 0) {
    std::cerr << "Failed to open DASH stream: " << mpdUrl << '\n';
    return false;
  }
  return true;
}

AVFormatContext *DashStream::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

} // namespace mediaplayer
