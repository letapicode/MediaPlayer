#include "SyncController.h"

using namespace mediaplayer;

SyncController::SyncController(QObject *parent)
    : QObject(parent), m_service("Desktop"), m_mdns("Desktop"), m_server(56791) {
  m_service.setSyncCallback([this](const std::string &path, double pos) {
    emit syncReceived(QString::fromStdString(path), pos);
  });
  m_service.start();
  m_server.start();
  m_mdns.setDeviceFoundCallback([this](const mdns::MdnsDevice &dev) {
    QVariantMap m;
    m["name"] = QString::fromStdString(dev.name);
    m["address"] = QString::fromStdString(dev.address);
    m["port"] = dev.port;
    m_devices.append(m);
    emit deviceListChanged();
  });
  m_mdns.start();
  connect(&m_server, &remote::RemoteControlServer::playRequested, this,
          [this](const QString &path, double pos) { emit syncReceived(path, pos); });
}

SyncController::~SyncController() {
  m_service.stop();
  m_mdns.stop();
  m_server.stop();
}

QVariantList SyncController::devices() const { return m_devices; }

void SyncController::sendSync(const QString &address, quint16 port, const QString &path,
                              double position) {
  m_rcClient.sendPlay(address.toStdString(), port, path.toStdString(), position);
}

void SyncController::updateStatus(const QString &path, double position) {
  m_service.setStatus(path.toStdString(), position);
  remote::DeviceStatus status{"Desktop", path.toStdString(), position};
  m_server.setStatus(status);
  if (m_cloud.configured())
    m_cloud.pushStatus(path.toStdString(), position);
}

QVariantMap SyncController::pullCloudStatus() {
  QVariantMap m;
  if (m_cloud.pullStatus()) {
    m["path"] = QString::fromStdString(m_cloud.lastPath());
    m["position"] = m_cloud.lastPosition();
  }
  return m;
}

void SyncController::setCloudServer(const QString &url, const QString &user, const QString &token) {
  m_cloud.setServerUrl(url.toStdString());
  m_cloud.setCredentials(user.toStdString(), token.toStdString());
}

void SyncController::setServerPort(quint16 port) {
  m_server.stop();
  m_server = remote::RemoteControlServer(port);
  m_server.start();
}
