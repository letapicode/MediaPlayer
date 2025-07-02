#ifndef MEDIAPLAYER_SYNCSERVICE_H
#define MEDIAPLAYER_SYNCSERVICE_H

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace mediaplayer {

struct DeviceInfo {
  std::string name;
  std::string address;
  uint16_t port{0};
  std::string path;
  double position{0.0};
};

class SyncService {
public:
  explicit SyncService(const std::string &name, uint16_t port = 56790);
  ~SyncService();

  bool start();
  void stop();

  void setStatus(const std::string &path, double position);
  void setSyncCallback(std::function<void(const std::string &path, double position)> cb);

private:
  void run();
  int m_sock{-1};
  std::string m_name;
  uint16_t m_port;
  std::string m_path;
  double m_position{0.0};
  std::thread m_thread;
  std::atomic<bool> m_running{false};
  std::function<void(const std::string &, double)> m_syncCallback;
};

class SyncClient {
public:
  explicit SyncClient(uint16_t port = 56790);
  std::vector<DeviceInfo>
  discover(std::chrono::milliseconds timeout = std::chrono::milliseconds(500));
  bool sendSync(const DeviceInfo &device, const std::string &path, double position);

private:
  uint16_t m_port;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SYNCSERVICE_H
