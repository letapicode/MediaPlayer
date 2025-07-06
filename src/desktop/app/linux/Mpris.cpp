#include "Mpris.h"
#ifdef Q_OS_LINUX
#include "MediaPlayerController.h"
#include <QCoreApplication>
#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusObjectPath>
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
  void Quit() { QCoreApplication::quit(); }
};

class MprisPlayerAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
  Q_PROPERTY(QString PlaybackStatus READ playbackStatus NOTIFY playbackStatusChanged)
  Q_PROPERTY(QString LoopStatus READ loopStatus CONSTANT)
  Q_PROPERTY(double Rate READ rate CONSTANT)
  Q_PROPERTY(bool Shuffle READ shuffle CONSTANT)
  Q_PROPERTY(double Volume READ volume WRITE setVolume NOTIFY volumeChanged)
  Q_PROPERTY(qlonglong Position READ position NOTIFY positionChanged)
  Q_PROPERTY(QVariantMap Metadata READ metadata NOTIFY metadataChanged)
  Q_PROPERTY(double MinimumRate READ minimumRate CONSTANT)
  Q_PROPERTY(double MaximumRate READ maximumRate CONSTANT)
  Q_PROPERTY(bool CanGoNext READ canGoNext CONSTANT)
  Q_PROPERTY(bool CanGoPrevious READ canGoPrevious CONSTANT)
  Q_PROPERTY(bool CanPlay READ canPlay CONSTANT)
  Q_PROPERTY(bool CanPause READ canPause CONSTANT)
  Q_PROPERTY(bool CanSeek READ canSeek CONSTANT)
  Q_PROPERTY(bool CanControl READ canControl CONSTANT)

public:
  MprisPlayerAdaptor(QObject *parent, mediaplayer::MediaPlayerController *c)
      : QDBusAbstractAdaptor(parent), m_controller(c) {}

  QString playbackStatus() const {
    return m_controller->playing() ? QStringLiteral("Playing") : QStringLiteral("Paused");
  }
  QString loopStatus() const { return QStringLiteral("None"); }
  double rate() const { return 1.0; }
  bool shuffle() const { return false; }
  double volume() const { return m_controller->volume(); }
  void setVolume(double v) { m_controller->setVolume(v); }
  qlonglong position() const {
    return static_cast<qlonglong>(m_controller->position() * 1000000.0);
  }
  QVariantMap metadata() const {
    QVariantMap m;
    m[QStringLiteral("xesam:title")] = m_controller->title();
    m[QStringLiteral("xesam:artist")] = m_controller->artist();
    m[QStringLiteral("xesam:album")] = m_controller->album();
    return m;
  }
  double minimumRate() const { return 1.0; }
  double maximumRate() const { return 1.0; }
  bool canGoNext() const { return true; }
  bool canGoPrevious() const { return true; }
  bool canPlay() const { return true; }
  bool canPause() const { return true; }
  bool canSeek() const { return true; }
  bool canControl() const { return true; }

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
  void Next() { m_controller->nextTrack(); }
  void Previous() { m_controller->previousTrack(); }
  void Seek(qlonglong offset) {
    double pos = m_controller->position() + (offset / 1000000.0);
    m_controller->seek(pos);
  }
  void SetPosition(const QDBusObjectPath &, qlonglong pos) {
    m_controller->seek(pos / 1000000.0);
    emit Seeked(pos);
  }
  void OpenUri(const QString &uri) { m_controller->openFile(uri); }

signals:
  void playbackStatusChanged(const QString &status);
  void volumeChanged(double volume);
  void positionChanged(qlonglong position);
  void metadataChanged(const QVariantMap &metadata);
  void Seeked(qlonglong position);

private:
  mediaplayer::MediaPlayerController *m_controller;
};

static QObject *s_mprisObject = nullptr;
static MprisPlayerAdaptor *s_playerAdaptor = nullptr;

void setupMprisIntegration(mediaplayer::MediaPlayerController *controller) {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) {
    qWarning() << "DBus connection failed:" << bus.lastError().message();
    return;
  }

  s_mprisObject = new QObject();
  new MprisRootAdaptor(s_mprisObject);
  s_playerAdaptor = new MprisPlayerAdaptor(s_mprisObject, controller);

  const QString service = QStringLiteral("org.mpris.MediaPlayer2.mediaplayer");
  if (!bus.registerService(service)) {
    qWarning() << "Could not register MPRIS service:" << bus.lastError().message();
    delete s_mprisObject;
    s_mprisObject = nullptr;
    return;
  }
  bus.registerObject("/org/mpris/MediaPlayer2", s_mprisObject, QDBusConnection::ExportAdaptors);

  QObject::connect(controller, &mediaplayer::MediaPlayerController::currentMetadataChanged,
                   [](const mediaplayer::MediaMetadata &) {
                     if (s_playerAdaptor)
                       emit s_playerAdaptor->metadataChanged(s_playerAdaptor->metadata());
                   });
  QObject::connect(controller, &mediaplayer::MediaPlayerController::positionChanged,
                   [controller]() {
                     if (s_playerAdaptor)
                       emit s_playerAdaptor->positionChanged(
                           static_cast<qlonglong>(controller->position() * 1000000.0));
                   });
  QObject::connect(controller, &mediaplayer::MediaPlayerController::volumeChanged, [controller]() {
    if (s_playerAdaptor)
      emit s_playerAdaptor->volumeChanged(controller->volume());
  });
  QObject::connect(
      controller, &mediaplayer::MediaPlayerController::playbackStateChanged, [controller]() {
        if (s_playerAdaptor)
          emit s_playerAdaptor->playbackStatusChanged(s_playerAdaptor->playbackStatus());
      });
}
#endif // Q_OS_LINUX
