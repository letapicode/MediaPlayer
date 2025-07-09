#include "mediaplayer/mdns/MdnsService.h"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace mediaplayer {
namespace mdns {

MdnsService::MdnsService(const std::string &name, uint16_t port) : m_name(name), m_port(port) {}

MdnsService::~MdnsService() { stop(); }

bool MdnsService::start() {
  m_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (m_sock < 0)
    return false;

  int yes = 1;
  setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5353);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(m_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    return false;

  ip_mreq mreq{};
  inet_pton(AF_INET, "224.0.0.251", &mreq.imr_multiaddr.s_addr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

  m_running = true;
  m_thread = std::thread(&MdnsService::run, this);
  return true;
}

void MdnsService::stop() {
  m_running = false;
  if (m_sock >= 0) {
    close(m_sock);
    m_sock = -1;
  }
  if (m_thread.joinable())
    m_thread.join();
}

std::vector<MdnsDevice> MdnsService::discoveredDevices() const { return m_devices; }

void MdnsService::setDeviceFoundCallback(std::function<void(const MdnsDevice &)> cb) {
  m_callback = std::move(cb);
}

void MdnsService::run() {
  char buf[512];
  while (m_running) {
    sockaddr_in src{};
    socklen_t slen = sizeof(src);
    int n = recvfrom(m_sock, buf, sizeof(buf) - 1, 0, reinterpret_cast<sockaddr *>(&src), &slen);
    if (n <= 0)
      continue;
    buf[n] = '\0';
    std::string msg(buf);
    if (msg.rfind("MP|", 0) == 0) {
      MdnsDevice dev;
      size_t p = msg.find('|', 3);
      dev.name = msg.substr(3, p - 3);
      dev.port = static_cast<uint16_t>(atoi(msg.c_str() + p + 1));
      char ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &src.sin_addr, ip, sizeof(ip));
      dev.address = ip;
      bool known = false;
      for (const auto &d : m_devices) {
        if (d.address == dev.address && d.port == dev.port) {
          known = true;
          break;
        }
      }
      if (!known) {
        m_devices.push_back(dev);
        if (m_callback)
          m_callback(dev);
      }
    } else {
      std::string announce = "MP|" + m_name + "|" + std::to_string(m_port);
      sockaddr_in dest{};
      dest.sin_family = AF_INET;
      dest.sin_port = htons(5353);
      inet_pton(AF_INET, "224.0.0.251", &dest.sin_addr);
      sendto(m_sock, announce.c_str(), announce.size(), 0, reinterpret_cast<sockaddr *>(&dest),
             sizeof(dest));
    }
  }
}

} // namespace mdns
} // namespace mediaplayer
