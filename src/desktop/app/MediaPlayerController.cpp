#include "MediaPlayerController.h"
#include "../VideoOutputQt.h"
#include "../VisualizerQt.h"
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

bool MediaPlayerController::playing() const { return m_player.position() > 0.0; }
double MediaPlayerController::position() const { return m_player.position(); }
double MediaPlayerController::volume() const { return m_player.volume(); }
