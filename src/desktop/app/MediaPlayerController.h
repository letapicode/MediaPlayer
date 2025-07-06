#ifndef MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H
#define MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H

#include "../VideoOutputQt.h"
#include "../VisualizerQt.h"
#include "NowPlayingModel.h"
#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/MediaMetadata.h"
#include "mediaplayer/MediaPlayer.h"
#include <QAudioDevice>
#include <QObject>
#include <memory>

namespace mediaplayer {

class MediaPlayerController : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool playing READ playing NOTIFY playbackStateChanged)
  Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
  Q_PROPERTY(double position READ position NOTIFY positionChanged)
  Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
  Q_PROPERTY(VideoOutputQt *videoOutput READ videoOutput CONSTANT)
  Q_PROPERTY(VisualizerQt *visualizer READ visualizer CONSTANT)
  Q_PROPERTY(QString title READ title NOTIFY currentMetadataChanged)
  Q_PROPERTY(QString artist READ artist NOTIFY currentMetadataChanged)
  Q_PROPERTY(QString album READ album NOTIFY currentMetadataChanged)
  Q_PROPERTY(double duration READ duration NOTIFY currentMetadataChanged)
  Q_PROPERTY(NowPlayingModel *nowPlaying READ nowPlaying CONSTANT)
public:
  explicit MediaPlayerController(QObject *parent = nullptr);

  enum PlaybackState { Stopped, Playing, Paused };
  Q_ENUM(PlaybackState)

  Q_INVOKABLE void openFile(const QString &path);
  Q_INVOKABLE void play();
  Q_INVOKABLE void pause();
  Q_INVOKABLE void stop();
  Q_INVOKABLE void seek(double position);
  Q_INVOKABLE void setVolume(double vol);
  Q_INVOKABLE void setAudioDevice(const QAudioDevice &device);
  Q_INVOKABLE void addToQueue(const QString &path);
  Q_INVOKABLE void nextTrack();
  Q_INVOKABLE void previousTrack();
  Q_INVOKABLE void removeFromQueue(int row);
  Q_INVOKABLE void moveQueueItem(int from, int to);
  void setLibrary(LibraryDB *db);

  bool playing() const;
  double position() const;
  double volume() const;
  QString title() const;
  QString artist() const;
  QString album() const;
  double duration() const;
  NowPlayingModel *nowPlaying() const { return m_nowPlaying; }

  PlaybackState playbackState() const { return m_state; }

  VideoOutputQt *videoOutput() const { return m_videoOutput; }
  VisualizerQt *visualizer() const { return m_visualizer; }

signals:
  void playbackStateChanged();
  void positionChanged();
  void volumeChanged();
  void currentMetadataChanged(const MediaMetadata &meta);
  void errorOccurred(const QString &message);
  void queueUpdated();

private:
  MediaPlayer m_player;
  VideoOutputQt *m_videoOutput{nullptr};
  VisualizerQt *m_visualizer{nullptr};
  MediaMetadata m_meta;
  NowPlayingModel *m_nowPlaying{nullptr};
  PlaybackState m_state{Stopped};
};

void registerMediaPlayerControllerQmlType();

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H
