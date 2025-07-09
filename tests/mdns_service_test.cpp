#include "mdns.h"
#include "mediaplayer/mdns/MdnsService.h"
#include <cassert>
#include <chrono>
#include <thread>

using namespace mediaplayer::mdns;

int main() {
  MdnsService service("TestDevice", 56000);
  bool found = false;
  service.setDeviceFoundCallback([&](const MdnsDevice &) { found = true; });
  assert(service.start());

  int sock = mdns_socket_open_ipv4(nullptr);
  char buffer[1024];
  mdns_record_t record{};
  record.name = mdns_string_make_from_cstr("Remote._mediaplayer._tcp.local.");
  record.type = MDNS_RECORDTYPE_SRV;
  record.data.srv.name = mdns_string_make_from_cstr("Remote");
  record.data.srv.port = 56001;
  mdns_announce_multicast(sock, buffer, sizeof(buffer), record, nullptr, 0, nullptr, 0);

  for (int i = 0; i < 10 && !found; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
  service.stop();
  mdns_socket_close(sock);
  assert(found);
  return 0;
}
