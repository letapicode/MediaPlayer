#include "mediaplayer/remote/RemoteControlClient.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

namespace mediaplayer {
namespace remote {

static size_t writeData(char *ptr, size_t size, size_t nmemb, void *userdata) {
  auto *str = static_cast<std::string *>(userdata);
  str->append(ptr, size * nmemb);
  return size * nmemb;
}

DeviceStatus RemoteControlClient::getStatus(const std::string &host, uint16_t port) {
  DeviceStatus status;
  CURL *curl = curl_easy_init();
  if (!curl)
    return status;
  std::string url = "http://" + host + ":" + std::to_string(port) + "/status";
  std::string resp;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeData);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK)
    return status;
  auto json = nlohmann::json::parse(resp, nullptr, false);
  if (!json.is_discarded())
    status = DeviceStatus::fromJson(json);
  return status;
}

bool RemoteControlClient::sendPlay(const std::string &host, uint16_t port, const std::string &path,
                                   double position) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return false;
  std::string url = "http://" + host + ":" + std::to_string(port) + "/play";
  nlohmann::json body{{"path", path}, {"position", position}};
  std::string bodyStr = body.dump();
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return res == CURLE_OK;
}

} // namespace remote
} // namespace mediaplayer
