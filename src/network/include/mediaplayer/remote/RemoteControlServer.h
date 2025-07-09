#ifndef MEDIAPLAYER_REMOTE_CONTROL_SERVER_H
#define MEDIAPLAYER_REMOTE_CONTROL_SERVER_H

#include "DeviceStatus.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

namespace mediaplayer {
namespace remote {

class RemoteControlServer : public QObject {
  Q_OBJECT
public:
  explicit RemoteControlServer(quint16 port = 56791, QObject *parent = nullptr);
  bool start();
  void stop();
  void setStatus(const DeviceStatus &status);
  quint16 port() const { return m_port; }

signals:
  void playRequested(const QString &path, double position);

private slots:
  void onNewConnection();

private:
  void handleRequest(QTcpSocket *socket);
  QTcpServer m_server;
  DeviceStatus m_status;
  quint16 m_port;
};

} // namespace remote
} // namespace mediaplayer

#endif // MEDIAPLAYER_REMOTE_CONTROL_SERVER_H
