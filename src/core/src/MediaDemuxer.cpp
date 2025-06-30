#include "mediaplayer/MediaDemuxer.h"
#include "mediaplayer/BufferedReader.h"
#include "mediaplayer/DashStream.h"
#include "mediaplayer/HlsStream.h"
#include "mediaplayer/NetworkStream.h"
#include <algorithm>
#include <iostream>
#include <libavcodec/avcodec.h>

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
  m_subtitleTracks.clear();
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
      std::string lower = path;
      std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
      if (lower.rfind(".m3u8") != std::string::npos) {
        HlsStream hls;
        if (!hls.open(path))
          return false;
        m_ctx = hls.release();
      } else if (lower.rfind(".mpd") != std::string::npos) {
        DashStream dash;
        if (!dash.open(path))
          return false;
        m_ctx = dash.release();
      } else {
        NetworkStream stream;
        if (!stream.open(path)) {
          return false;
        }
        m_ctx = stream.release();
      }
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
    } else if (m_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
      SubtitleTrackInfo info{};
      info.index = i;
      AVDictionaryEntry *lang = av_dict_get(m_ctx->streams[i]->metadata, "language", nullptr, 0);
      if (lang && lang->value)
        info.language = lang->value;
      info.codec = avcodec_get_name(m_ctx->streams[i]->codecpar->codec_id);
      m_subtitleTracks.push_back(std::move(info));
      if (m_subtitleStream < 0)
        m_subtitleStream = i;
    }
  }
  m_eof = false;
  return true;
}

bool MediaDemuxer::setSubtitleStream(int index) {
  for (const auto &info : m_subtitleTracks) {
    if (info.index == index) {
      m_subtitleStream = index;
      return true;
    }
  }
  return false;
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

} // namespace mediaplayer
