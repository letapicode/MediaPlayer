#include "MediaPlayerController.h"
#include "../AudioOutputQt.h"
#include "../VideoOutputQt.h"
#include "../VisualizerQt.h"
#include "NowPlayingModel.h"
#include <QAudioDevice>
#include <QVariantMap>
#include <QtQml/qqml.h>
#include <string>
#ifdef Q_OS_MAC
void updateNowPlayingInfo(const mediaplayer::MediaMetadata &meta);
#endif
#include "mediaplayer/ProjectMVisualizer.h"

using namespace mediaplayer;

MediaPlayerController::MediaPlayerController(QObject *parent) : QObject(parent) {
  auto output = std::make_unique<VideoOutputQt>();
  m_videoOutput = output.get();
  m_player.setVideoOutput(std::move(output));
  auto vis = std::make_shared<ProjectMVisualizer>();
  m_player.setVisualizer(vis);
  m_visualizer = new VisualizerQt(this);
  m_visualizer->setVisualizer(vis);
  m_nowPlaying = new NowPlayingModel(&m_player, this);
  PlaybackCallbacks cb;
  cb.onPlay = [this]() {
    m_playing = true;
    emit playbackStateChanged();
  };
  cb.onPause = [this]() {
    m_playing = false;
    emit playbackStateChanged();
  };
  cb.onStop = [this]() {
    m_playing = false;
    m_position = 0.0;
    emit playbackStateChanged();
    emit positionChanged();
  };
  cb.onTrackLoaded = [this](const MediaMetadata &meta) {
    m_meta = meta;
    m_position = 0.0;
    emit currentMetadataChanged(meta);
#ifdef Q_OS_MAC
    updateNowPlayingInfo(meta);
#endif
  };
  cb.onPosition = [this](double pos) {
    m_position = pos;
    emit positionChanged();
  };
  cb.onError = [this](const std::string &msg) { emit errorOccurred(QString::fromStdString(msg)); };
  m_player.setCallbacks(cb);
}
void MediaPlayerController::openFile(const QString &path) {
  if (!m_player.open(path.toStdString()))
    emit errorOccurred(tr("Failed to open file"));
}

void MediaPlayerController::play() {
  m_player.play();
  m_playing = true;
  emit playbackStateChanged();
}
void MediaPlayerController::pause() {
  m_player.pause();
  m_playing = false;
  emit playbackStateChanged();
}
void MediaPlayerController::stop() {
  m_player.stop();
  m_playing = false;
  m_position = 0.0;
  emit playbackStateChanged();
  emit positionChanged();
}
void MediaPlayerController::seek(double position) {
  m_player.seek(position);
  m_position = position;
  emit positionChanged();
}

void MediaPlayerController::setVolume(double vol) {
  m_player.setVolume(vol);
  emit volumeChanged();
}

void MediaPlayerController::setAudioDevice(const QAudioDevice &device) {
  auto out = std::make_unique<AudioOutputQt>(device);
  m_player.setAudioOutput(std::move(out));
}

void MediaPlayerController::addToQueue(const QString &path) {
  m_player.addToPlaylist(path.toStdString());
  m_nowPlaying->refresh();
  emit queueUpdated();
}

void MediaPlayerController::nextTrack() {
  if (m_player.nextTrack()) {
    m_nowPlaying->refresh();
    emit queueUpdated();
  }
}

void MediaPlayerController::previousTrack() {
  if (m_player.previousTrack()) {
    m_nowPlaying->refresh();
    emit queueUpdated();
  }
}

void MediaPlayerController::removeFromQueue(int row) {
  if (m_player.removeFromQueue(static_cast<size_t>(row))) {
    m_nowPlaying->refresh();
    emit queueUpdated();
  }
}

void MediaPlayerController::moveQueueItem(int from, int to) {
  if (m_player.moveQueueItem(static_cast<size_t>(from), static_cast<size_t>(to))) {
    m_nowPlaying->refresh();
    emit queueUpdated();
  }
}

void MediaPlayerController::setLibrary(LibraryDB *db) { m_player.setLibrary(db); }

bool MediaPlayerController::playing() const { return m_playing; }
double MediaPlayerController::position() const { return m_position; }
double MediaPlayerController::volume() const { return m_player.volume(); }
QString MediaPlayerController::title() const { return QString::fromStdString(m_meta.title); }
QString MediaPlayerController::artist() const { return QString::fromStdString(m_meta.artist); }
QString MediaPlayerController::album() const { return QString::fromStdString(m_meta.album); }
double MediaPlayerController::duration() const { return m_meta.duration; }

QVariantMap MediaPlayerController::currentTrack() const {
  QVariantMap m;
  m[QStringLiteral("path")] = QString::fromStdString(m_meta.path);
  m[QStringLiteral("title")] = QString::fromStdString(m_meta.title);
  m[QStringLiteral("artist")] = QString::fromStdString(m_meta.artist);
  m[QStringLiteral("album")] = QString::fromStdString(m_meta.album);
  m[QStringLiteral("genre")] = QString::fromStdString(m_meta.genre);
  m[QStringLiteral("duration")] = m_meta.duration;
  m[QStringLiteral("width")] = m_meta.width;
  m[QStringLiteral("height")] = m_meta.height;
  m[QStringLiteral("rating")] = m_meta.rating;
  return m;
}

void mediaplayer::registerMediaPlayerControllerQmlType() {
  qmlRegisterType<MediaPlayerController>("MediaPlayer", 1, 0, "MediaPlayerController");
}
