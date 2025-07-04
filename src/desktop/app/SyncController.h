#ifndef MEDIAPLAYER_SYNCCONTROLLER_H
#define MEDIAPLAYER_SYNCCONTROLLER_H

#include "mediaplayer/SyncService.h"
#include <QObject>
#include <QVariant>

namespace mediaplayer {

class SyncController : public QObject {
  Q_OBJECT
public:
  explicit SyncController(QObject *parent = nullptr);
  ~SyncController() override;

  Q_INVOKABLE QVariantList discoverDevices();
  Q_INVOKABLE void sendSync(const QString &address, quint16 port, const QString &path,
                            double position);
  Q_INVOKABLE void updateStatus(const QString &path, double position);

signals:
  void syncReceived(const QString &path, double position);

private:
  SyncService m_service;
  SyncClient m_client;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SYNCCONTROLLER_H
