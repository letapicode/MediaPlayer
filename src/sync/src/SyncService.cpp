#include "mediaplayer/SyncService.h"

#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace mediaplayer {

static int closeSocket(int s) {
#ifdef _WIN32
  return closesocket(s);
#else
  return close(s);
#endif
}

SyncService::SyncService(const std::string &name, uint16_t port) : m_name(name), m_port(port) {}

SyncService::~SyncService() { stop(); }

bool SyncService::start() {
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  m_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (m_sock < 0)
    return false;
  int opt = 1;
  setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(m_port);
  if (bind(m_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    closeSocket(m_sock);
    m_sock = -1;
    return false;
  }
  m_running = true;
  m_thread = std::thread(&SyncService::run, this);
  return true;
}

void SyncService::stop() {
  m_running = false;
  if (m_sock >= 0) {
    closeSocket(m_sock);
    m_sock = -1;
  }
  if (m_thread.joinable())
    m_thread.join();
#ifdef _WIN32
  WSACleanup();
#endif
}

void SyncService::setStatus(const std::string &path, double position) {
  m_path = path;
  m_position = position;
}

void SyncService::setSyncCallback(std::function<void(const std::string &, double)> cb) {
  m_syncCallback = std::move(cb);
}

void SyncService::run() {
  char buf[1024];
  while (m_running) {
    sockaddr_in src{};
    socklen_t srclen = sizeof(src);
    int n = recvfrom(m_sock, buf, sizeof(buf) - 1, 0, reinterpret_cast<sockaddr *>(&src), &srclen);
    if (n <= 0)
      continue;
    buf[n] = '\0';
    std::string msg(buf);
    if (msg == "DISCOVER") {
      char out[1024];
      snprintf(out, sizeof(out), "DEVICE|%s|%u|%s|%f", m_name.c_str(), m_port, m_path.c_str(),
               m_position);
      sendto(m_sock, out, strlen(out), 0, reinterpret_cast<sockaddr *>(&src), srclen);
    } else if (msg.rfind("PLAY|", 0) == 0) {
      size_t p1 = msg.find('|', 5);
      if (p1 == std::string::npos)
        continue;
      std::string path = msg.substr(5, p1 - 5);
      double pos = atof(msg.c_str() + p1 + 1);
      if (m_syncCallback)
        m_syncCallback(path, pos);
    }
  }
}

SyncClient::SyncClient(uint16_t port) : m_port(port) {}

std::vector<DeviceInfo> SyncClient::discover(std::chrono::milliseconds timeout) {
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    return {};
  int broadcast = 1;
  setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast, sizeof(broadcast));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(m_port);
  addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  const char *req = "DISCOVER";
  sendto(sock, req, strlen(req), 0, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

  struct timeval tv;
  tv.tv_sec = static_cast<long>(timeout.count() / 1000);
  tv.tv_usec = static_cast<long>((timeout.count() % 1000) * 1000);
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));

  std::vector<DeviceInfo> devices;
  char buf[1024];
  while (true) {
    sockaddr_in src{};
    socklen_t srclen = sizeof(src);
    int n = recvfrom(sock, buf, sizeof(buf) - 1, 0, reinterpret_cast<sockaddr *>(&src), &srclen);
    if (n <= 0)
      break;
    buf[n] = '\0';
    std::string msg(buf);
    if (msg.rfind("DEVICE|", 0) == 0) {
      DeviceInfo info;
      size_t p1 = msg.find('|', 7);
      size_t p2 = msg.find('|', p1 + 1);
      size_t p3 = msg.find('|', p2 + 1);
      if (p1 == std::string::npos || p2 == std::string::npos)
        continue;
      info.name = msg.substr(7, p1 - 7);
      info.port = static_cast<uint16_t>(atoi(msg.c_str() + p1 + 1));
      if (p3 != std::string::npos) {
        info.path = msg.substr(p2 + 1, p3 - p2 - 1);
        info.position = atof(msg.c_str() + p3 + 1);
      } else {
        info.path = msg.substr(p2 + 1);
        info.position = 0.0;
      }
      char ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &src.sin_addr, ip, sizeof(ip));
      info.address = ip;
      devices.push_back(std::move(info));
    }
  }
  closeSocket(sock);
#ifdef _WIN32
  WSACleanup();
#endif
  return devices;
}

bool SyncClient::sendSync(const DeviceInfo &device, const std::string &path, double position) {
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    return false;
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(device.port);
  inet_pton(AF_INET, device.address.c_str(), &addr.sin_addr);
  char msg[1024];
  snprintf(msg, sizeof(msg), "PLAY|%s|%f", path.c_str(), position);
  int ret = sendto(sock, msg, strlen(msg), 0, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
  closeSocket(sock);
#ifdef _WIN32
  WSACleanup();
#endif
  return ret >= 0;
}

} // namespace mediaplayer
