#include "mediaplayer/AITagClient.h"
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace mediaplayer {

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t total = size * nmemb;
  std::string *s = static_cast<std::string *>(userp);
  s->append(static_cast<char *>(contents), total);
  return total;
}

AITagClient::AITagClient(const std::string &serviceUrl) : m_serviceUrl(serviceUrl) {}

std::vector<std::string> AITagClient::tagAudio(const std::string &path) {
  CURL *curl = curl_easy_init();
  std::vector<std::string> tags;
  if (!curl)
    return tags;
  curl_mime *form = curl_mime_init(curl);
  curl_mimepart *part = curl_mime_addpart(form);
  curl_mime_name(part, "file");
  curl_mime_filedata(part, path.c_str());
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, (m_serviceUrl + "/tag/audio").c_str());
  curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  CURLcode res = curl_easy_perform(curl);
  curl_mime_free(form);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK)
    return tags;
  try {
    auto json = nlohmann::json::parse(response);
    if (json.is_array()) {
      for (const auto &v : json)
        tags.push_back(v.get<std::string>());
    } else if (json.contains("tags")) {
      for (const auto &v : json["tags"])
        tags.push_back(v.get<std::string>());
    }
  } catch (...) {
  }
  return tags;
}

std::vector<std::string> AITagClient::tagVideo(const std::string &path) {
  CURL *curl = curl_easy_init();
  std::vector<std::string> tags;
  if (!curl)
    return tags;
  curl_mime *form = curl_mime_init(curl);
  curl_mimepart *part = curl_mime_addpart(form);
  curl_mime_name(part, "file");
  curl_mime_filedata(part, path.c_str());
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, (m_serviceUrl + "/tag/video").c_str());
  curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  CURLcode res = curl_easy_perform(curl);
  curl_mime_free(form);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK)
    return tags;
  try {
    auto json = nlohmann::json::parse(response);
    if (json.is_array()) {
      for (const auto &v : json)
        tags.push_back(v.get<std::string>());
    } else if (json.contains("tags")) {
      for (const auto &v : json["tags"])
        tags.push_back(v.get<std::string>());
    }
  } catch (...) {
  }
  return tags;
}

} // namespace mediaplayer
