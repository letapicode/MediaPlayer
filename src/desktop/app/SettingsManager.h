#ifndef MEDIAPLAYER_SETTINGSMANAGER_H
#define MEDIAPLAYER_SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

namespace mediaplayer {
class SettingsManager : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
  Q_PROPERTY(QString audioDevice READ audioDevice WRITE setAudioDevice NOTIFY audioDeviceChanged)
  Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
public:
  explicit SettingsManager(QObject *parent = nullptr);

  QString theme() const;
  void setTheme(const QString &theme);

  QString audioDevice() const;
  void setAudioDevice(const QString &name);

  double volume() const;
  void setVolume(double volume);

signals:
  void themeChanged(const QString &theme);
  void audioDeviceChanged(const QString &name);
  void volumeChanged(double volume);

private:
  QSettings m_settings;
};
} // namespace mediaplayer

#endif // MEDIAPLAYER_SETTINGSMANAGER_H
