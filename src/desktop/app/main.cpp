#include "mediaplayer/MediaPlayer.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  mediaplayer::MediaPlayer player;
  player.setVolume(1.0);
  const QUrl url = QUrl::fromLocalFile("qml/Main.qml");
  engine.load(url);
  if (engine.rootObjects().isEmpty())
    return -1;
  return app.exec();
}
