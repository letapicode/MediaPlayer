#ifndef MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H
#define MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H

#include "mediaplayer/MediaPlayer.h"
#include <QObject>

namespace mediaplayer {

class MediaPlayerController : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool playing READ playing NOTIFY playbackStateChanged)
  Q_PROPERTY(double position READ position NOTIFY positionChanged)
  Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
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

signals:
  void playbackStateChanged();
  void positionChanged();
  void volumeChanged();
  void errorOccurred(const QString &message);

private:
  MediaPlayer m_player;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MEDIAPLAYERCONTROLLER_H
