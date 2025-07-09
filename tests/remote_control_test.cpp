#include "mediaplayer/remote/RemoteControlClient.h"
#include "mediaplayer/remote/RemoteControlServer.h"
#include <QCoreApplication>
#include <cassert>
#include <thread>

using namespace mediaplayer::remote;

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  RemoteControlServer server(57900);
  assert(server.start());
  DeviceStatus status{"TestDevice", "sample.mp3", 3.0};
  server.setStatus(status);

  bool playCalled = false;
  QObject::connect(&server, &RemoteControlServer::playRequested,
                   [&](const QString &path, double pos) { playCalled = true; });

  RemoteControlClient client;
  DeviceStatus got = client.getStatus("127.0.0.1", server.port());
  assert(got.path == status.path);
  client.sendPlay("127.0.0.1", server.port(), "foo.mp3", 1.0);
  QThread::msleep(100);
  QCoreApplication::processEvents();
  assert(playCalled);
  server.stop();
  return 0;
}
