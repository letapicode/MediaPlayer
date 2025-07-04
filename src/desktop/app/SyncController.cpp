#include "SyncController.h"

using namespace mediaplayer;

SyncController::SyncController(QObject *parent) : QObject(parent), m_service("Desktop") {
  m_service.setSyncCallback([this](const std::string &path, double pos) {
    emit syncReceived(QString::fromStdString(path), pos);
  });
  m_service.start();
}

SyncController::~SyncController() { m_service.stop(); }

QVariantList SyncController::discoverDevices() {
  QVariantList list;
  for (const auto &dev : m_client.discover()) {
    QVariantMap m;
    m["name"] = QString::fromStdString(dev.name);
    m["address"] = QString::fromStdString(dev.address);
    m["port"] = dev.port;
    m["path"] = QString::fromStdString(dev.path);
    m["position"] = dev.position;
    list.push_back(m);
  }
  return list;
}

void SyncController::sendSync(const QString &address, quint16 port, const QString &path,
                              double position) {
  DeviceInfo info;
  info.address = address.toStdString();
  info.port = port;
  m_client.sendSync(info, path.toStdString(), position);
}

void SyncController::updateStatus(const QString &path, double position) {
  m_service.setStatus(path.toStdString(), position);
}
