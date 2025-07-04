#ifndef MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H
#define MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H

#include "../VideoOutputQt.h"
#include "../VisualizerQt.h"
#include "mediaplayer/MediaPlayer.h"
#include <QObject>

namespace mediaplayer {

class MediaPlayerController : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool playing READ playing NOTIFY playbackStateChanged)
  Q_PROPERTY(double position READ position NOTIFY positionChanged)
  Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
  Q_PROPERTY(VideoOutputQt *videoOutput READ videoOutput CONSTANT)
  Q_PROPERTY(VisualizerQt *visualizer READ visualizer CONSTANT)
public:
  explicit MediaPlayerController(QObject *parent = nullptr);

  Q_INVOKABLE void openFile(const QString &path);
  Q_INVOKABLE void play();
  Q_INVOKABLE void pause();
  Q_INVOKABLE void stop();
  Q_INVOKABLE void seek(double position);
  Q_INVOKABLE void setVolume(double vol);

  bool playing() const;
  double position() const;
  double volume() const;

  VideoOutputQt *videoOutput() const { return m_videoOutput; }
  VisualizerQt *visualizer() const { return m_visualizer; }

signals:
  void playbackStateChanged();
  void positionChanged();
  void volumeChanged();
  void errorOccurred(const QString &message);

private:
  MediaPlayer m_player;
  VideoOutputQt *m_videoOutput{nullptr};
  VisualizerQt *m_visualizer{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H
