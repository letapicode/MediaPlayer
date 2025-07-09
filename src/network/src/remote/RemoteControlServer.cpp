#include "mediaplayer/remote/RemoteControlServer.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace mediaplayer {
namespace remote {

RemoteControlServer::RemoteControlServer(quint16 port, QObject *parent)
    : QObject(parent), m_port(port) {}

bool RemoteControlServer::start() {
  if (m_server.listen(QHostAddress::Any, m_port)) {
    connect(&m_server, &QTcpServer::newConnection, this, &RemoteControlServer::onNewConnection);
    m_port = m_server.serverPort();
    return true;
  }
  return false;
}

void RemoteControlServer::stop() { m_server.close(); }

void RemoteControlServer::setStatus(const DeviceStatus &status) { m_status = status; }

void RemoteControlServer::onNewConnection() {
  QTcpSocket *sock = m_server.nextPendingConnection();
  if (sock)
    handleRequest(sock);
}

void RemoteControlServer::handleRequest(QTcpSocket *socket) {
  socket->waitForReadyRead(3000);
  QByteArray data = socket->readAll();
  QString request = QString::fromLatin1(data);
  if (request.startsWith("GET /status")) {
    QJsonObject obj{{"name", QString::fromStdString(m_status.name)},
                    {"path", QString::fromStdString(m_status.path)},
                    {"position", m_status.position}};
    QByteArray body = QJsonDocument(obj).toJson();
    QByteArray resp = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                      QByteArray::number(body.size()) + "\r\n\r\n" + body;
    socket->write(resp);
  } else if (request.startsWith("POST /play")) {
    int idx = data.indexOf("\r\n\r\n");
    QByteArray body = idx >= 0 ? data.mid(idx + 4) : QByteArray();
    auto obj = QJsonDocument::fromJson(body).object();
    QString path = obj.value("path").toString();
    double pos = obj.value("position").toDouble();
    emit playRequested(path, pos);
    QByteArray resp = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    socket->write(resp);
  } else {
    QByteArray resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    socket->write(resp);
  }
  socket->disconnectFromHost();
}

} // namespace remote
} // namespace mediaplayer
