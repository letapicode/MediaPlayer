#ifndef MEDIAPLAYER_DEVICE_STATUS_H
#define MEDIAPLAYER_DEVICE_STATUS_H

#include <nlohmann/json.hpp>
#include <string>

namespace mediaplayer {
namespace remote {

struct DeviceStatus {
  std::string name;
  std::string path;
  double position{0.0};

  nlohmann::json toJson() const {
    return nlohmann::json{{"name", name}, {"path", path}, {"position", position}};
  }

  static DeviceStatus fromJson(const nlohmann::json &j) {
    DeviceStatus s;
    if (j.contains("name"))
      s.name = j["name"].get<std::string>();
    if (j.contains("path"))
      s.path = j["path"].get<std::string>();
    if (j.contains("position"))
      s.position = j["position"].get<double>();
    return s;
  }
};

} // namespace remote
} // namespace mediaplayer

#endif // MEDIAPLAYER_DEVICE_STATUS_H
