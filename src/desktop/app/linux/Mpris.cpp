#include "Mpris.h"
#ifdef Q_OS_LINUX
#include "MediaPlayerController.h"
#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

class MprisRootAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
  Q_PROPERTY(QString Identity READ identity CONSTANT)
  Q_PROPERTY(bool CanQuit READ canQuit CONSTANT)
  Q_PROPERTY(bool CanRaise READ canRaise CONSTANT)
  Q_PROPERTY(bool HasTrackList READ hasTrackList CONSTANT)
  Q_PROPERTY(QString DesktopEntry READ desktopEntry CONSTANT)

public:
  explicit MprisRootAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent) {}

  QString identity() const { return QStringLiteral("MediaPlayer"); }
  bool canQuit() const { return false; }
  bool canRaise() const { return false; }
  bool hasTrackList() const { return false; }
  QString desktopEntry() const { return QStringLiteral("mediaplayer"); }

public slots:
  void Raise() {}
  void Quit() {}
};

class MprisPlayerAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
  Q_PROPERTY(QString PlaybackStatus READ playbackStatus NOTIFY playbackStatusChanged)
  Q_PROPERTY(double Volume READ volume WRITE setVolume NOTIFY volumeChanged)
  Q_PROPERTY(qlonglong Position READ position NOTIFY positionChanged)

public:
  MprisPlayerAdaptor(QObject *parent, mediaplayer::MediaPlayerController *c)
      : QDBusAbstractAdaptor(parent), m_controller(c) {}

  QString playbackStatus() const {
    return m_controller->playing() ? QStringLiteral("Playing") : QStringLiteral("Paused");
  }
  double volume() const { return m_controller->volume(); }
  void setVolume(double v) { m_controller->setVolume(v); }
  qlonglong position() const {
    return static_cast<qlonglong>(m_controller->position() * 1000000.0);
  }

public slots:
  void Play() { m_controller->play(); }
  void Pause() { m_controller->pause(); }
  void Stop() { m_controller->stop(); }
  void PlayPause() {
    if (m_controller->playing())
      m_controller->pause();
    else
      m_controller->play();
  }
  void Next() {}
  void Previous() {}
  void Seek(qlonglong offset) {
    double pos = m_controller->position() + (offset / 1000000.0);
    m_controller->seek(pos);
  }

signals:
  void playbackStatusChanged(const QString &status);
  void volumeChanged(double volume);
  void positionChanged(qlonglong position);

private:
  mediaplayer::MediaPlayerController *m_controller;
};

static QObject *s_mprisObject = nullptr;

void setupMprisIntegration(mediaplayer::MediaPlayerController *controller) {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) {
    qWarning() << "DBus connection failed:" << bus.lastError().message();
    return;
  }

  s_mprisObject = new QObject();
  new MprisRootAdaptor(s_mprisObject);
  new MprisPlayerAdaptor(s_mprisObject, controller);

  const QString service = QStringLiteral("org.mpris.MediaPlayer2.mediaplayer");
  if (!bus.registerService(service)) {
    qWarning() << "Could not register MPRIS service:" << bus.lastError().message();
    delete s_mprisObject;
    s_mprisObject = nullptr;
    return;
  }
  bus.registerObject("/org/mpris/MediaPlayer2", s_mprisObject, QDBusConnection::ExportAdaptors);
}
#endif // Q_OS_LINUX
