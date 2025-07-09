#include "mediaplayer/cloud/CloudSyncService.h"
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace mediaplayer {
namespace cloud {

void CloudSyncService::pushStatus(const std::string &path, double position) {
  if (m_serverUrl.empty())
    return;
  CURL *curl = curl_easy_init();
  if (!curl)
    return;
  std::string url = m_serverUrl + "/status";
  nlohmann::json body{{"user", m_user}, {"path", path}, {"position", position}};
  std::string bodyStr = body.dump();
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  if (!m_token.empty()) {
    std::string auth = "Authorization: Bearer " + m_token;
    headers = curl_slist_append(headers, auth.c_str());
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
  curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
}

bool CloudSyncService::pullStatus() {
  if (m_serverUrl.empty())
    return false;
  CURL *curl = curl_easy_init();
  if (!curl)
    return false;
  std::string url = m_serverUrl + "/status?user=" + m_user;
  std::string resp;
  struct curl_slist *headers = nullptr;
  if (!m_token.empty()) {
    std::string auth = "Authorization: Bearer " + m_token;
    headers = curl_slist_append(headers, auth.c_str());
  }
  auto writeCb = [](char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string *s = static_cast<std::string *>(userdata);
    s->append(ptr, size * nmemb);
    return size * nmemb;
  };
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK)
    return false;
  auto json = nlohmann::json::parse(resp, nullptr, false);
  if (json.is_discarded())
    return false;
  m_lastPath = json.value("path", std::string());
  m_lastPosition = json.value("position", 0.0);
  return true;
}

} // namespace cloud
} // namespace mediaplayer
