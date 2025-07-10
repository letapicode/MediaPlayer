#include "../LibraryQt.h"
#include "LibraryModel.h"
#include "MediaPlayerController.h"
#include "PlaylistModel.h"
#include "mediaplayer/LibraryDB.h"
#ifdef Q_OS_LINUX
#include "linux/Mpris.h"
#endif
#include "../FormatConverterQt.h"
#include "../VideoOutputQt.h"
#include "../VisualizerItem.h"
#include "../VisualizerQt.h"
#include "../gesture_voice/VoiceCommandProcessor.h"
#include "../gesture_voice/VoskRecognizer.h"
#include "AudioDevicesModel.h"
#include "MicrophoneInput.h"
#include "MouseGestureFilter.h"
#include "SettingsManager.h"
#include "SmartPlaylistManager.h"
#include "SyncController.h"
#include "TranslationManager.h"
#include "VideoItem.h"
#include "mediaplayer/MediaPlayer.h"
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QtQml/qqml.h>
#include <chrono>
#ifdef Q_OS_MAC
void setupMacIntegration(mediaplayer::MediaPlayerController *controller);
void connectNowPlayingInfo(mediaplayer::MediaPlayerController *controller);
#endif
#ifdef _WIN32
void setupWindowsIntegration();
#include <QVariant>
#include <QtGlobal>
#endif

int main(int argc, char *argv[]) {
  auto start = std::chrono::steady_clock::now();
  QQuickStyle::setStyle("Material");
  QGuiApplication app(argc, argv);

  qInfo() << "MediaPlayer core version:"
          << QString::fromStdString(mediaplayer::MediaPlayer::version());

  auto afterApp = std::chrono::steady_clock::now();
  qInfo() << "Startup time:"
          << std::chrono::duration_cast<std::chrono::milliseconds>(afterApp - start).count()
          << "ms";

  mediaplayer::SettingsManager settings;

  QQmlApplicationEngine engine;
  mediaplayer::registerVideoOutputQtQmlType();
  mediaplayer::registerVisualizerQtQmlType();
  mediaplayer::registerVisualizerItemQmlType();
  mediaplayer::registerMediaPlayerControllerQmlType();
  qmlRegisterType<mediaplayer::VideoItem>("MediaPlayer", 1, 0, "VideoItem");
  mediaplayer::MediaPlayerController controller;
  mediaplayer::LibraryModel libraryModel;
  mediaplayer::PlaylistModel playlistModel;
  mediaplayer::LibraryQt libraryQt;
  mediaplayer::SmartPlaylistManager spManager;
  mediaplayer::LibraryDB libraryDb(
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString() +
      "/library.db");
  libraryDb.open();
  libraryModel.setLibrary(&libraryDb);
  playlistModel.setLibrary(&libraryDb);
  spManager.setPlaylistModel(&playlistModel);
  libraryQt.setLibrary(&libraryDb);
  controller.setLibrary(&libraryDb);
  mediaplayer::AudioDevicesModel audioDevicesModel;
  mediaplayer::SyncController syncController;
  mediaplayer::TranslationManager translation;
  translation.switchLanguage(QLocale::system().name());
  mediaplayer::FormatConverterQt converter;
  mediaplayer::MicrophoneInput mic;
  mediaplayer::VoskRecognizer recognizer;
  mediaplayer::VoiceCommandProcessor cmdProcessor(&controller);

  // Apply saved settings
  QString theme = settings.theme();
  if (!theme.isEmpty())
    app.setProperty("theme", theme);
  controller.setVolume(settings.volume());
  QAudioDevice dev = audioDevicesModel.defaultDevice();
  for (int i = 0; i < audioDevicesModel.rowCount({}); ++i) {
    if (audioDevicesModel
            .data(audioDevicesModel.index(i, 0), mediaplayer::AudioDevicesModel::NameRole)
            .toString() == settings.audioDevice()) {
      dev = audioDevicesModel.deviceAt(i);
      break;
    }
  }
  controller.setAudioDevice(dev);
#ifdef Q_OS_MAC
  setupMacIntegration(&controller);
#endif
#ifdef Q_OS_LINUX
  setupMprisIntegration(&controller);
#endif

  engine.rootContext()->setContextProperty("player", &controller);
  engine.rootContext()->setContextProperty("libraryModel", &libraryModel);
  engine.rootContext()->setContextProperty("playlistModel", &playlistModel);
  engine.rootContext()->setContextProperty("smartPlaylistManager", &spManager);
  engine.rootContext()->setContextProperty("libraryQt", &libraryQt);
  engine.rootContext()->setContextProperty("audioDevicesModel", &audioDevicesModel);
  engine.rootContext()->setContextProperty("sync", &syncController);
  engine.rootContext()->setContextProperty("translation", &translation);
  engine.rootContext()->setContextProperty("settings", &settings);
  engine.rootContext()->setContextProperty("formatConverter", &converter);
  engine.rootContext()->setContextProperty("nowPlayingModel", controller.nowPlaying());
  engine.rootContext()->setContextProperty("microphoneInput", &mic);
  engine.rootContext()->setContextProperty("voiceRecognizer", &recognizer);

  recognizer.loadModel(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) +
                       "/vosk-model");
  QObject::connect(&mic, &mediaplayer::MicrophoneInput::audioDataReady, &recognizer,
                   &mediaplayer::VoskRecognizer::feedAudio);
  QObject::connect(&recognizer, &mediaplayer::VoskRecognizer::textRecognized, &cmdProcessor,
                   &mediaplayer::VoiceCommandProcessor::processText);
  QObject::connect(&cmdProcessor, &mediaplayer::VoiceCommandProcessor::commandUnknown,
                   [](const QString &t) { qWarning() << "Unknown command" << t; });

  QObject::connect(&controller, &mediaplayer::MediaPlayerController::volumeChanged,
                   [&settings, &controller]() { settings.setVolume(controller.volume()); });

  const QUrl url = QUrl::fromLocalFile("qml/Main.qml");
  engine.load(url);
  if (engine.rootObjects().isEmpty())
    return -1;
  QObject *rootObj = engine.rootObjects().first();
  MouseGestureFilter gestureFilter(&controller);
  rootObj->installEventFilter(&gestureFilter);
#ifdef _WIN32
  app.setProperty("playerController",
                  QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(&controller)));
  setupWindowsIntegration();
#endif
#ifdef Q_OS_MAC
  connectNowPlayingInfo(&controller);
#endif
  int ret = app.exec();
  libraryDb.close();
  return ret;
}
