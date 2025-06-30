#include "mediaplayer/MediaDemuxer.h"
#include "mediaplayer/BufferedReader.h"
#include "mediaplayer/NetworkStream.h"
#include <iostream>

namespace mediaplayer {

MediaDemuxer::MediaDemuxer() { m_subtitleStream = -1; }

MediaDemuxer::~MediaDemuxer() { close(); }

bool MediaDemuxer::isUrl(const std::string &path) {
  return path.rfind("http://", 0) == 0 || path.rfind("https://", 0) == 0;
}

void MediaDemuxer::close() {
  if (m_ctx) {
    avformat_close_input(&m_ctx);
  }
  if (m_bufferedReader) {
    m_bufferedReader->close();
    m_bufferedReader.reset();
  }
  m_audioStream = -1;
  m_videoStream = -1;
  m_subtitleStream = -1;
  m_eof = false;
}

bool MediaDemuxer::open(const std::string &path) {
  close();
  if (isUrl(path)) {
    if (m_bufferSize > 0) {
      m_bufferedReader = std::make_unique<BufferedReader>();
      if (!m_bufferedReader->open(path, m_bufferSize)) {
        m_bufferedReader.reset();
        return false;
      }
      m_ctx = m_bufferedReader->context();
    } else {
      NetworkStream stream;
      if (!stream.open(path)) {
        return false;
      }
      m_ctx = stream.release();
    }
  } else if (avformat_open_input(&m_ctx, path.c_str(), nullptr, nullptr) < 0) {
    std::cerr << "Failed to open media: " << path << '\n';
    return false;
  }
  if (avformat_find_stream_info(m_ctx, nullptr) < 0) {
    std::cerr << "Failed to retrieve stream info\n";
    avformat_close_input(&m_ctx);
    return false;
  }
  for (unsigned i = 0; i < m_ctx->nb_streams; ++i) {
    if (m_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStream < 0) {
      m_audioStream = i;
    } else if (m_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStream < 0) {
      m_videoStream = i;
    } else if (m_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE &&
               m_subtitleStream < 0) {
      m_subtitleStream = i;
    }
  }
  m_eof = false;
  return true;
}

bool MediaDemuxer::readPacket(AVPacket &pkt) {
  if (!m_ctx || m_eof)
    return false;
  if (av_read_frame(m_ctx, &pkt) < 0) {
    m_eof = true;
    return false;
  }
  return true;
}

void MediaDemuxer::resetEof() { m_eof = false; }

} // namespace mediaplayer
