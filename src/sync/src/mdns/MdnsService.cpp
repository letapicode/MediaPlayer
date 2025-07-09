#include "mediaplayer/mdns/MdnsService.h"
#include "mdns.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>

namespace mediaplayer {
namespace mdns {

static int record_callback(int sock, const struct sockaddr *from, size_t addrlen,
                           mdns_entry_type_t entry, uint16_t /*query_id*/, uint16_t rtype,
                           uint16_t /*rclass*/, uint32_t /*ttl*/, const void *data, size_t size,
                           size_t name_offset, size_t name_length, size_t record_offset,
                           size_t record_length, void *user_data) {
  if (entry != MDNS_ENTRYTYPE_ANSWER)
    return 0;
  auto *self = static_cast<MdnsService *>(user_data);
  MdnsDevice dev;
  dev.name.assign((const char *)data + name_offset, name_length);
  if (rtype == MDNS_RECORDTYPE_SRV) {
    mdns_record_srv_t srv =
        mdns_record_parse_srv(data, size, record_offset, record_length, nullptr, 0);
    dev.port = srv.port;
  } else if (rtype == MDNS_RECORDTYPE_A) {
    sockaddr_in addr{};
    mdns_record_parse_a(data, size, record_offset, record_length, &addr);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
    dev.address = ip;
  } else {
    return 0;
  }
  if (!dev.address.empty() && dev.port)
    self->addDevice(dev);
  return 0;
}

MdnsService::MdnsService(const std::string &name, uint16_t port) : m_name(name), m_port(port) {}

MdnsService::~MdnsService() { stop(); }

bool MdnsService::start() {
  m_socket = mdns_socket_open_ipv4(nullptr);
  if (m_socket < 0)
    return false;
  m_running = true;
  m_thread = std::thread(&MdnsService::run, this);
  return true;
}

void MdnsService::stop() {
  m_running = false;
  if (m_socket >= 0) {
    mdns_socket_close(m_socket);
    m_socket = -1;
  }
  if (m_thread.joinable())
    m_thread.join();
}

std::vector<MdnsDevice> MdnsService::discoveredDevices() const { return m_devices; }

void MdnsService::setDeviceFoundCallback(std::function<void(const MdnsDevice &)> cb) {
  m_callback = std::move(cb);
}

void MdnsService::addDevice(const MdnsDevice &dev) {
  for (const auto &d : m_devices) {
    if (d.address == dev.address && d.port == dev.port)
      return;
  }
  m_devices.push_back(dev);
  if (m_callback)
    m_callback(dev);
}

void MdnsService::run() {
  char buffer[2048];
  mdns_string_t service = mdns_string_make_from_cstr("_mediaplayer._tcp.local.");
  mdns_query_t query{MDNS_RECORDTYPE_PTR, service.str, service.length};

  while (m_running) {
    mdns_multiquery_send(m_socket, &query, 1, buffer, sizeof(buffer), 0);
    mdns_query_recv(m_socket, buffer, sizeof(buffer), record_callback, this, 0);

    mdns_record_t answer{};
    std::string host = m_name + "._mediaplayer._tcp.local.";
    answer.name = mdns_string_make(host.c_str(), host.size());
    answer.type = MDNS_RECORDTYPE_SRV;
    answer.data.srv.name = mdns_string_make_from_cstr(m_name.c_str());
    answer.data.srv.port = m_port;
    mdns_announce_multicast(m_socket, buffer, sizeof(buffer), answer, nullptr, 0, nullptr, 0);

    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

} // namespace mdns
} // namespace mediaplayer
