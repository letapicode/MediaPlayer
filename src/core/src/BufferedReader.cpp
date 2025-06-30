#include "mediaplayer/BufferedReader.h"
#include <algorithm>
#include <cstring>
#include <iostream>

extern "C" {
#include <libavformat/avio.h>
}

namespace mediaplayer {

BufferedReader::BufferedReader() = default;

BufferedReader::~BufferedReader() { close(); }

void BufferedReader::close() {
  if (m_avio) {
    av_freep(&m_avio->buffer);
    avio_context_free(&m_avio);
  }
  if (m_inner) {
    avio_closep(&m_inner);
  }
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
  m_ring.clear();
  m_avioBuffer.clear();
  m_readPos = 0;
  m_dataSize = 0;
  m_eof = false;
}

AVFormatContext *BufferedReader::release() {
  AVFormatContext *ctx = m_ctx;
  m_ctx = nullptr;
  return ctx;
}

int BufferedReader::readPacket(void *opaque, uint8_t *buf, int buf_size) {
  auto *br = static_cast<BufferedReader *>(opaque);
  int total = 0;
  while (total < buf_size) {
    if (br->m_dataSize == 0) {
      int toRead = static_cast<int>(br->m_ring.size());
      int r = avio_read(br->m_inner, br->m_ring.data(), toRead);
      if (r <= 0) {
        br->m_eof = true;
        break;
      }
      br->m_readPos = 0;
      br->m_dataSize = static_cast<size_t>(r);
    }
    int copy = std::min<int>(buf_size - total, br->m_dataSize);
    std::memcpy(buf + total, br->m_ring.data() + br->m_readPos, copy);
    br->m_readPos += copy;
    br->m_dataSize -= copy;
    total += copy;
    if (br->m_eof && br->m_dataSize == 0)
      break;
  }
  if (total == 0 && br->m_eof)
    return AVERROR_EOF;
  return total;
}

bool BufferedReader::open(const std::string &url, size_t bufferSize) {
  close();
  m_ring.resize(bufferSize);
  m_avioBuffer.resize(4096);
  if (avio_open2(&m_inner, url.c_str(), AVIO_FLAG_READ, nullptr, nullptr) < 0) {
    std::cerr << "Failed to open URL: " << url << '\n';
    return false;
  }
  uint8_t *buffer = static_cast<uint8_t *>(av_malloc(m_avioBuffer.size()));
  if (!buffer)
    return false;
  m_avio = avio_alloc_context(buffer, static_cast<int>(m_avioBuffer.size()), 0, this,
                              &BufferedReader::readPacket, nullptr, nullptr);
  if (!m_avio) {
    av_free(buffer);
    return false;
  }
  m_ctx = avformat_alloc_context();
  if (!m_ctx)
    return false;
  m_ctx->pb = m_avio;
  m_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;
  if (avformat_open_input(&m_ctx, "", nullptr, nullptr) < 0) {
    std::cerr << "Failed to open format context for URL: " << url << '\n';
    close();
    return false;
  }
  return true;
}

} // namespace mediaplayer
