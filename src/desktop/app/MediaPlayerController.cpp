#include "MediaPlayerController.h"
#include "../AudioOutputQt.h"
#include "../VideoOutputQt.h"
#include "../VisualizerQt.h"
#include "NowPlayingModel.h"
#include <QAudioDevice>
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
  cb.onPlay = [this]() { emit playbackStateChanged(); };
  cb.onPause = [this]() { emit playbackStateChanged(); };
  cb.onStop = [this]() { emit playbackStateChanged(); };
  cb.onTrackLoaded = [this](const MediaMetadata &meta) {
    m_meta = meta;
    emit currentMetadataChanged(meta);
#ifdef Q_OS_MAC
    updateNowPlayingInfo(meta);
#endif
  };
  cb.onPosition = [this](double) { emit positionChanged(); };
  m_player.setCallbacks(cb);
}

void MediaPlayerController::openFile(const QString &path) {
  if (!m_player.open(path.toStdString()))
    emit errorOccurred(tr("Failed to open file"));
}

void MediaPlayerController::play() {
  m_player.play();
  emit playbackStateChanged();
}
void MediaPlayerController::pause() {
  m_player.pause();
  emit playbackStateChanged();
}
void MediaPlayerController::stop() {
  m_player.stop();
  emit playbackStateChanged();
}
void MediaPlayerController::seek(double position) { m_player.seek(position); }

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

bool MediaPlayerController::playing() const { return m_player.isPlaying(); }
double MediaPlayerController::position() const { return m_player.position(); }
double MediaPlayerController::volume() const { return m_player.volume(); }
QString MediaPlayerController::title() const { return QString::fromStdString(m_meta.title); }
QString MediaPlayerController::artist() const { return QString::fromStdString(m_meta.artist); }
QString MediaPlayerController::album() const { return QString::fromStdString(m_meta.album); }
double MediaPlayerController::duration() const { return m_meta.duration; }
