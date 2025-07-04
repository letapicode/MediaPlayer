#include "LibraryModel.h"
#include "MediaPlayerController.h"
#include "PlaylistModel.h"
#ifdef Q_OS_LINUX
#include "linux/Mpris.h"
#endif
#include "../FormatConverterQt.h"
#include "../VideoOutputQt.h"
#include "../VisualizerItem.h"
#include "../VisualizerQt.h"
#include "AudioDevicesModel.h"
#include "SyncController.h"
#include "TranslationManager.h"
#include "VideoItem.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqml.h>
#ifdef Q_OS_MAC
void setupMacIntegration();
#endif
#ifdef _WIN32
void setupWindowsIntegration();
#include <QVariant>
#include <QtGlobal>
#endif

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

#ifdef Q_OS_MAC
  setupMacIntegration();
#endif

  QQmlApplicationEngine engine;
  mediaplayer::registerVideoOutputQtQmlType();
  mediaplayer::registerVisualizerQtQmlType();
  mediaplayer::registerVisualizerItemQmlType();
  qmlRegisterType<mediaplayer::VideoItem>("MediaPlayer", 1, 0, "VideoItem");
  mediaplayer::MediaPlayerController controller;
  mediaplayer::LibraryModel libraryModel;
  mediaplayer::PlaylistModel playlistModel;
  mediaplayer::AudioDevicesModel audioDevicesModel;
  mediaplayer::SyncController syncController;
  mediaplayer::TranslationManager translation;
  mediaplayer::FormatConverterQt converter;
#ifdef Q_OS_LINUX
  setupMprisIntegration(&controller);
#endif

  engine.rootContext()->setContextProperty("player", &controller);
  engine.rootContext()->setContextProperty("libraryModel", &libraryModel);
  engine.rootContext()->setContextProperty("playlistModel", &playlistModel);
  engine.rootContext()->setContextProperty("audioDevicesModel", &audioDevicesModel);
  engine.rootContext()->setContextProperty("sync", &syncController);
  engine.rootContext()->setContextProperty("translation", &translation);
  engine.rootContext()->setContextProperty("formatConverter", &converter);

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
