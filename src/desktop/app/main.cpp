#include "LibraryModel.h"
#include "MediaPlayerController.h"
#include "PlaylistModel.h"
#ifdef Q_OS_LINUX
#include "linux/Mpris.h"
#endif
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QTranslator translator;
  translator.load("player_en", "translations");
  app.installTranslator(&translator);

  QQmlApplicationEngine engine;
  mediaplayer::MediaPlayerController controller;
  mediaplayer::LibraryModel libraryModel;
  mediaplayer::PlaylistModel playlistModel;
#ifdef Q_OS_LINUX
  setupMprisIntegration(&controller);
#endif

  engine.rootContext()->setContextProperty("player", &controller);
  engine.rootContext()->setContextProperty("libraryModel", &libraryModel);
  engine.rootContext()->setContextProperty("playlistModel", &playlistModel);

  const QUrl url = QUrl::fromLocalFile("qml/Main.qml");
  engine.load(url);
  if (engine.rootObjects().isEmpty())
    return -1;
  return app.exec();
}
