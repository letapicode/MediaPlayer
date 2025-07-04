#include "LibraryModel.h"
#include "MediaPlayerController.h"
#include "PlaylistModel.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#ifdef _WIN32
void setupWindowsIntegration();
#include <QVariant>
#include <QtGlobal>
#endif

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QTranslator translator;
  translator.load("player_en", "translations");
  app.installTranslator(&translator);

  QQmlApplicationEngine engine;
  mediaplayer::MediaPlayerController controller;
  mediaplayer::LibraryModel libraryModel;
  mediaplayer::PlaylistModel playlistModel;

  engine.rootContext()->setContextProperty("player", &controller);
  engine.rootContext()->setContextProperty("libraryModel", &libraryModel);
  engine.rootContext()->setContextProperty("playlistModel", &playlistModel);

  const QUrl url = QUrl::fromLocalFile("qml/Main.qml");
  engine.load(url);
  if (engine.rootObjects().isEmpty())
    return -1;
#ifdef _WIN32
  app.setProperty("playerController",
                  QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(&controller)));
  setupWindowsIntegration();
#endif
  return app.exec();
}
