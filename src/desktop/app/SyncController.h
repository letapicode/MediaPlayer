#ifndef MEDIAPLAYER_SYNCCONTROLLER_H
#define MEDIAPLAYER_SYNCCONTROLLER_H

#include "mediaplayer/SyncService.h"
#include "mediaplayer/cloud/CloudSyncService.h"
#include "mediaplayer/mdns/MdnsService.h"
#include "mediaplayer/remote/RemoteControlClient.h"
#include "mediaplayer/remote/RemoteControlServer.h"
#include <QObject>
#include <QVariant>

namespace mediaplayer {

class SyncController : public QObject {
  Q_OBJECT
public:
  explicit SyncController(QObject *parent = nullptr);
  ~SyncController() override;

  Q_INVOKABLE QVariantList devices() const;
  Q_INVOKABLE void sendSync(const QString &address, quint16 port, const QString &path,
                            double position);
  Q_INVOKABLE void updateStatus(const QString &path, double position);
  Q_INVOKABLE QVariantMap pullCloudStatus();
  void setCloudServer(const QString &url, const QString &user, const QString &token);
  void setServerPort(quint16 port);

signals:
  void syncReceived(const QString &path, double position);
  void deviceListChanged();

private:
  SyncService m_service;
  SyncClient m_client;
  mdns::MdnsService m_mdns;
  remote::RemoteControlServer m_server;
  remote::RemoteControlClient m_rcClient;
  cloud::CloudSyncService m_cloud;
  QVariantList m_devices;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SYNCCONTROLLER_H
