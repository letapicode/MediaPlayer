#include "mediaplayer/InternetRadioStream.h"
#include <algorithm>
#include <cstring>
#include <iostream>

namespace mediaplayer {

InternetRadioStream::InternetRadioStream() = default;

InternetRadioStream::~InternetRadioStream() { close(); }

void InternetRadioStream::setMetadataCallback(MetadataCallback cb) { m_callback = std::move(cb); }

bool InternetRadioStream::open(const std::string &url) {
  m_curl = curl_easy_init();
  if (!m_curl)
    return false;

  curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &InternetRadioStream::writeCallback);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, &InternetRadioStream::headerCallback);
  curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this);
  curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "MediaPlayer/1.0");
  curl_easy_setopt(m_curl, CURLOPT_BUFFERSIZE, 4096L);
  curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);

  m_stop = false;
  m_metaInt = 0;
  m_bytesUntilMeta = 0;

  m_thread = std::thread(&InternetRadioStream::readLoop, this);
  return true;
}

void InternetRadioStream::close() {
  if (!m_curl)
    return;
  m_stop = true;
  if (m_thread.joinable())
    m_thread.join();
  curl_easy_cleanup(m_curl);
  m_curl = nullptr;
}

size_t InternetRadioStream::headerCallback(char *buffer, size_t size, size_t nitems,
                                           void *userdata) {
  InternetRadioStream *self = static_cast<InternetRadioStream *>(userdata);
  size_t total = size * nitems;
  std::string header(buffer, total);
  const std::string key = "icy-metaint:";
  auto it = std::search(header.begin(), header.end(), key.begin(), key.end(),
                        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
  if (it != header.end()) {
    long value = std::strtol(header.c_str() + (it - header.begin()) + key.size(), nullptr, 10);
    if (value > 0) {
      self->m_metaInt = value;
      self->m_bytesUntilMeta = value;
    }
  }
  return total;
}

size_t InternetRadioStream::writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  InternetRadioStream *self = static_cast<InternetRadioStream *>(userdata);
  size_t total = size * nmemb;
  size_t offset = 0;

  while (offset < total) {
    if (self->m_metaInt > 0 && self->m_bytesUntilMeta == 0) {
      unsigned char metaLen = ptr[offset++];
      size_t toRead = static_cast<size_t>(metaLen) * 16;
      size_t available = std::min(toRead, total - offset);
      self->m_partialMetadata.append(ptr + offset, available);
      offset += available;
      toRead -= available;
      if (toRead == 0) {
        self->parseMetadata(self->m_partialMetadata);
        self->m_partialMetadata.clear();
        self->m_bytesUntilMeta = self->m_metaInt;
      } else {
        self->m_bytesUntilMeta = -static_cast<long>(toRead);
      }
    } else if (self->m_metaInt > 0) {
      size_t consume = std::min(static_cast<size_t>(self->m_bytesUntilMeta), total - offset);
      offset += consume;
      self->m_bytesUntilMeta -= consume;
    } else {
      offset = total;
    }

    if (self->m_bytesUntilMeta < 0) {
      long needed = -self->m_bytesUntilMeta;
      size_t available = std::min(static_cast<size_t>(needed), total - offset);
      self->m_partialMetadata.append(ptr + offset, available);
      offset += available;
      needed -= available;
      if (needed == 0) {
        self->parseMetadata(self->m_partialMetadata);
        self->m_partialMetadata.clear();
        self->m_bytesUntilMeta = self->m_metaInt;
      } else {
        self->m_bytesUntilMeta = -needed;
      }
    }
  }
  return total;
}

void InternetRadioStream::parseMetadata(const std::string &data) {
  auto pos = data.find("StreamTitle='");
  if (pos == std::string::npos)
    return;
  pos += 13; // length of "StreamTitle='"
  auto end = data.find("';", pos);
  if (end == std::string::npos)
    end = data.find('\'', pos);
  if (end == std::string::npos)
    return;
  std::string title = data.substr(pos, end - pos);
  if (m_callback)
    m_callback(title);
}

void InternetRadioStream::readLoop() {
  CURLcode res = curl_easy_perform(m_curl);
  if (res != CURLE_OK) {
    std::cerr << "CURL error: " << curl_easy_strerror(res) << '\n';
  }
}

} // namespace mediaplayer
