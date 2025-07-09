#include "mediaplayer/upnp/DlnaClient.h"
#include <arpa/inet.h>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace mediaplayer {
namespace upnp {

static size_t writeData(char *ptr, size_t size, size_t nmemb, void *userdata) {
  auto *str = static_cast<std::string *>(userdata);
  str->append(ptr, size * nmemb);
  return size * nmemb;
}

std::vector<DlnaDevice> DlnaClient::discover(std::chrono::milliseconds timeout) {
  std::vector<DlnaDevice> devices;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    return devices;

  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1900);
  inet_pton(AF_INET, "239.255.255.250", &addr.sin_addr);

  const char *msg = "M-SEARCH * HTTP/1.1\r\n"
                    "HOST:239.255.255.250:1900\r\n"
                    "MAN:\"ssdp:discover\"\r\n"
                    "MX:1\r\n"
                    "ST:upnp:rootdevice\r\n\r\n";

  sendto(sock, msg, strlen(msg), 0, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

  struct timeval tv{};
  tv.tv_sec = static_cast<long>(timeout.count() / 1000);
  tv.tv_usec = static_cast<long>((timeout.count() % 1000) * 1000);
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  char buf[2048];
  while (true) {
    struct sockaddr_in src{};
    socklen_t srclen = sizeof(src);
    int n = recvfrom(sock, buf, sizeof(buf) - 1, 0, reinterpret_cast<sockaddr *>(&src), &srclen);
    if (n <= 0)
      break;
    buf[n] = '\0';
    std::string response(buf);
    auto locPos = response.find("LOCATION:");
    if (locPos == std::string::npos)
      continue;
    auto end = response.find('\r', locPos);
    std::string location = response.substr(locPos + 9, end - locPos - 9);
    DlnaDevice dev{};
    dev.location = location;

    // fetch description
    CURL *curl = curl_easy_init();
    if (!curl)
      continue;
    std::string xml;
    curl_easy_setopt(curl, CURLOPT_URL, location.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &xml);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
      continue;
    auto namePos = xml.find("<friendlyName>");
    if (namePos != std::string::npos) {
      auto nameEnd = xml.find("</friendlyName>", namePos);
      dev.friendlyName = xml.substr(namePos + 14, nameEnd - namePos - 14);
    } else {
      dev.friendlyName = location;
    }
    auto ctrlPos = xml.find("<controlURL>");
    if (ctrlPos != std::string::npos) {
      auto ctrlEnd = xml.find("</controlURL>", ctrlPos);
      dev.controlUrl = xml.substr(ctrlPos + 12, ctrlEnd - ctrlPos - 12);
      if (dev.controlUrl.rfind("http", 0) != 0) {
        // relative URL
        auto slash = location.find('/', 7);
        dev.controlUrl = location.substr(0, slash) + dev.controlUrl;
      }
    }
    devices.push_back(std::move(dev));
  }

  close(sock);
  return devices;
}

std::vector<std::string> DlnaClient::listMedia(const DlnaDevice &device,
                                               const std::string &objectId) {
  std::vector<std::string> items;
  CURL *curl = curl_easy_init();
  if (!curl)
    return items;
  std::string soap = "<?xml version=\"1.0\"?>"
                     "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                     "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                     "<s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\""
                     " BrowseFlag=\"BrowseDirectChildren\" Filter=\"*\" StartingIndex=\"0\" "
                     "RequestedCount=\"0\" SortCriteria=\"\" ObjectID=\"" +
                     objectId + "\"></u:Browse></s:Body></s:Envelope>";
  std::string resp;
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: text/xml; charset=\"utf-8\"");
  headers = curl_slist_append(
      headers, "SOAPACTION: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"");
  curl_easy_setopt(curl, CURLOPT_URL, device.controlUrl.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, soap.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeData);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK)
    return items;
  size_t pos = 0;
  while ((pos = resp.find("<dc:title>", pos)) != std::string::npos) {
    size_t end = resp.find("</dc:title>", pos);
    if (end == std::string::npos)
      break;
    items.push_back(resp.substr(pos + 10, end - pos - 10));
    pos = end + 11;
  }
  return items;
}

} // namespace upnp
} // namespace mediaplayer
