#ifndef MEDIAPLAYER_MDNS_SERVICE_H
#define MEDIAPLAYER_MDNS_SERVICE_H

#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace mediaplayer {
namespace mdns {

struct MdnsDevice {
  std::string name;
  std::string address;
  uint16_t port{0};
};

class MdnsService {
public:
  explicit MdnsService(const std::string &name, uint16_t port = 56791);
  ~MdnsService();

  bool start();
  void stop();
  std::vector<MdnsDevice> discoveredDevices() const;
  void setDeviceFoundCallback(std::function<void(const MdnsDevice &)> cb);

private:
  void run();
  void addDevice(const MdnsDevice &dev);
  int m_socket{-1};
  std::string m_name;
  uint16_t m_port;
  std::thread m_thread;
  bool m_running{false};
  std::vector<MdnsDevice> m_devices;
  std::function<void(const MdnsDevice &)> m_callback;
};

} // namespace mdns
} // namespace mediaplayer

#endif // MEDIAPLAYER_MDNS_SERVICE_H
