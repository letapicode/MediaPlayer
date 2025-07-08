#include "mediaplayer/AITagClient.h"
#include <chrono>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>

namespace mediaplayer {

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t total = size * nmemb;
  std::string *s = static_cast<std::string *>(userp);
  s->append(static_cast<char *>(contents), total);
  return total;
}

AITagClient::AITagClient(const std::string &serviceUrl) : m_serviceUrl(serviceUrl) {}

static std::vector<std::string> parseTags(const nlohmann::json &json) {
  std::vector<std::string> tags;
  for (auto it = json.begin(); it != json.end(); ++it) {
    const auto &v = it.value();
    if (v.is_array()) {
      for (const auto &s : v)
        tags.push_back(s.get<std::string>());
    } else if (v.is_string()) {
      tags.push_back(v.get<std::string>());
    }
  }
  return tags;
}

static std::vector<std::string> pollResult(const std::string &baseUrl, int jobId) {
  for (int i = 0; i < 100; ++i) {
    CURL *curl = curl_easy_init();
    if (!curl)
      break;
    std::string url = baseUrl + "/result/" + std::to_string(jobId);
    std::string resp;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
      break;
    try {
      auto json = nlohmann::json::parse(resp);
      if (json.contains("status") && json["status"] == "pending") {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        continue;
      }
      return parseTags(json);
    } catch (...) {
      break;
    }
  }
  return {};
}

std::vector<std::string> AITagClient::tagAudio(const std::string &path) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return {};
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
    return {};
  try {
    auto json = nlohmann::json::parse(response);
    int jobId = json.value("job_id", 0);
    if (jobId)
      return pollResult(m_serviceUrl, jobId);
  } catch (...) {
  }
  return {};
}

std::vector<std::string> AITagClient::tagVideo(const std::string &path) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return {};
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
    return {};
  try {
    auto json = nlohmann::json::parse(response);
    int jobId = json.value("job_id", 0);
    if (jobId)
      return pollResult(m_serviceUrl, jobId);
  } catch (...) {
  }
  return {};
}

} // namespace mediaplayer
